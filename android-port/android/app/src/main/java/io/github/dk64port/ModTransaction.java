package io.github.dk64port;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import org.json.*;

/** Recoverable flat-directory changes, always under ModSession (including at game startup). */
final class ModTransaction {
    interface Checkpoint { void afterWrite(int index) throws IOException; }
    private final Path directory, transaction;
    private final Checkpoint checkpoint;

    ModTransaction(File filesDirectory, Path directory, Checkpoint checkpoint) {
        this.directory = directory.toAbsolutePath().normalize();
        transaction = new File(filesDirectory, "mod-transaction").toPath();
        this.checkpoint = checkpoint;
    }
    private Path target(String name) throws IOException {
        if (!name.matches("[A-Za-z0-9_][A-Za-z0-9_. ()-]{0,179}")
                || !(ModArchive.isMod(name) || name.endsWith(".so"))) throw new IOException("Invalid mod transaction filename.");
        Path path = directory.resolve(name);
        if (Files.exists(path, LinkOption.NOFOLLOW_LINKS) && !Files.isRegularFile(path, LinkOption.NOFOLLOW_LINKS))
            throw new IOException("Mod transaction cannot replace a directory or symbolic link.");
        return path;
    }
    private static void privateDirectory(Path path) throws IOException {
        if (!Files.isDirectory(path, LinkOption.NOFOLLOW_LINKS)) throw new IOException("Invalid mod transaction directory.");
    }
    private static void copySynced(Path source, Path target) throws IOException {
        if (!Files.isRegularFile(source, LinkOption.NOFOLLOW_LINKS)) throw new IOException("Missing mod transaction input.");
        try (InputStream input = Files.newInputStream(source); FileOutputStream output = new FileOutputStream(target.toFile())) {
            ModArchive.copyBounded(input, output, ModArchive.MAX_INPUT);
            output.getFD().sync();
        }
    }
    private void replace(Path source, Path target) throws IOException {
        Path temporary = transaction.resolve("write.tmp");
        copySynced(source, temporary);
        for (int attempt = 0; ; attempt++) {
            try {
                Files.move(temporary, target, StandardCopyOption.ATOMIC_MOVE, StandardCopyOption.REPLACE_EXISTING);
                return;
            } catch (AccessDeniedException error) {
                // Windows host checks can encounter short-lived antivirus/indexer handles.
                // Never fall back to a non-atomic replacement or change file permissions.
                if (attempt >= 8 || !System.getProperty("os.name", "").startsWith("Windows")) throw error;
                try { Thread.sleep(25); }
                catch (InterruptedException interrupted) {
                    Thread.currentThread().interrupt(); throw new IOException("Mod file replacement interrupted.", interrupted);
                }
            }
        }
    }
    private void finish() throws IOException {
        // Retire the journal first, only after commit or complete rollback. A killed
        // cleanup can then discard leftovers without trying to restore deleted backups.
        Files.deleteIfExists(transaction.resolve("journal.json"));
        DriverArchive.removePrivateTree(transaction);
    }
    void recover() throws IOException {
        if (!Files.exists(transaction, LinkOption.NOFOLLOW_LINKS)) return;
        privateDirectory(transaction);
        Path journal = transaction.resolve("journal.json");
        if (Files.exists(journal) && !Files.exists(transaction.resolve("committed"))) {
            if (!Files.isRegularFile(journal, LinkOption.NOFOLLOW_LINKS) || Files.size(journal) > 65536)
                throw new IOException("Invalid mod recovery journal; refusing to start with a partial installation.");
            try {
                JSONObject before = new JSONObject(new String(Files.readAllBytes(journal), StandardCharsets.UTF_8));
                if (before.length() > 8) throw new IOException("Oversized mod recovery journal.");
                List<String> names = new ArrayList<>();
                Iterator<String> keys = before.keys();
                while (keys.hasNext()) {
                    String name = keys.next();
                    target(name);
                    if (!(before.get(name) instanceof Boolean)) throw new IOException("Invalid mod recovery state.");
                    if (before.getBoolean(name) && !Files.isRegularFile(transaction.resolve("backup").resolve(name), LinkOption.NOFOLLOW_LINKS))
                        throw new IOException("Missing mod recovery backup.");
                    names.add(name);
                }
                // Validate the entire journal before changing anything. Backups survive repeated recovery attempts.
                for (String name : names) {
                    if (before.getBoolean(name)) replace(transaction.resolve("backup").resolve(name), target(name));
                    else Files.deleteIfExists(target(name));
                }
            } catch (JSONException error) { throw new IOException("Cannot read mod recovery journal.", error); }
        }
        finish();
    }
    void apply(Map<String, Path> replacements, Set<String> removals) throws IOException {
        recover();
        Set<String> affected = new LinkedHashSet<>(removals);
        affected.addAll(replacements.keySet());
        if (affected.size() > 8) throw new IOException("Too many files in mod transaction.");
        for (String name : affected) target(name);
        Files.createDirectory(transaction);
        Files.createDirectory(transaction.resolve("backup"));
        Files.createDirectory(transaction.resolve("payload"));
        try {
            JSONObject before = new JSONObject();
            for (String name : affected) {
                boolean exists = Files.exists(target(name));
                before.put(name, exists);
                if (exists) copySynced(target(name), transaction.resolve("backup").resolve(name));
            }
            for (Map.Entry<String, Path> replacement : replacements.entrySet())
                copySynced(replacement.getValue(), transaction.resolve("payload").resolve(replacement.getKey()));
            Path temporary = transaction.resolve("journal.tmp");
            try (FileOutputStream output = new FileOutputStream(temporary.toFile())) {
                output.write(before.toString().getBytes(StandardCharsets.UTF_8)); output.getFD().sync();
            }
            Files.move(temporary, transaction.resolve("journal.json"), StandardCopyOption.ATOMIC_MOVE);
            int index = 0;
            for (String name : affected) {
                if (replacements.containsKey(name)) replace(transaction.resolve("payload").resolve(name), target(name));
                else Files.deleteIfExists(target(name));
                if (checkpoint != null) checkpoint.afterWrite(++index);
            }
            try (FileOutputStream output = new FileOutputStream(transaction.resolve("committed").toFile())) {
                output.write(1); output.getFD().sync();
            }
        } catch (IOException | JSONException error) {
            try { recover(); } catch (IOException recoveryError) { error.addSuppressed(recoveryError); }
            throw new IOException("Mod installation failed; recovery is required before the next game launch.", error);
        }
        finish();
    }
}
