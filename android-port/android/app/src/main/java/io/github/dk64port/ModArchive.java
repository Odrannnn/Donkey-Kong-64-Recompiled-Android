package io.github.dk64port;

import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.util.zip.*;
import org.json.JSONException;

/** Private staging only. No archive path is ever used as a destination path. */
final class ModArchive {
    static final long MAX_INPUT = 512L * 1024 * 1024;
    static final long MAX_EXPANDED = 2L * 1024 * 1024 * 1024;
    static final int MAX_ENTRIES = 32768;
    static final int MAX_MANIFEST = 256 * 1024;
    static final class Candidate {
        final Path file;
        final String filename;
        final byte[] manifest;
        final Map<String, Path> libraries = new LinkedHashMap<>();
        Candidate(Path file, String filename, byte[] manifest) {
            this.file = file; this.filename = filename; this.manifest = manifest;
        }
    }
    private static final class Budget { long bytes; int entries; }
    private static final class Scan {
        byte[] manifest;
        boolean textures;
        final List<String> mods = new ArrayList<>();
        final List<String> libraries = new ArrayList<>();
    }
    static boolean isMod(String name) {
        String lower = name.toLowerCase(Locale.ROOT);
        return lower.endsWith(".nrm") || lower.endsWith(".rtz");
    }
    private static void checkFilename(String name) throws IOException {
        if (name == null || name.length() > 180 || !name.matches("[A-Za-z0-9_][A-Za-z0-9_. ()-]*")
                || name.toLowerCase(Locale.ROOT).endsWith(".offline.nrm")) {
            throw new IOException("Unsupported filename or offline/native mod. Use a standard .nrm or .rtz package.");
        }
    }
    static void copyBounded(InputStream input, OutputStream output, long limit) throws IOException {
        byte[] buffer = new byte[32768];
        long size = 0;
        int count;
        while ((count = input.read(buffer)) != -1) {
            size += count;
            if (size > limit) throw new IOException("Mod file exceeds the import size limit.");
            output.write(buffer, 0, count);
        }
    }
    static Candidate prepare(InputStream input, String name, Path staging) throws IOException, JSONException {
        checkFilename(name);
        if (!isMod(name) && !name.toLowerCase(Locale.ROOT).endsWith(".zip")) {
            throw new IOException("Choose a .nrm, .rtz or mod ZIP. ROMs, drivers and .apworld files are not recomp mods.");
        }
        Path source = staging.resolve("source");
        try (OutputStream output = Files.newOutputStream(source, StandardOpenOption.CREATE_NEW)) {
            copyBounded(input, output, MAX_INPUT);
        }
        Budget budget = new Budget();
        Scan scan = scan(source, budget, !isMod(name));
        if (isMod(name)) {
            Candidate candidate = candidate(source, name, scan);
            if (!NativeMod.libraries(candidate.manifest).isEmpty())
                throw new IOException("Import the Android ZIP containing this .nrm and its native .so bridge together.");
            return candidate;
        }
        if (scan.mods.size() != 1) {
            throw new IOException("The ZIP must contain exactly one .nrm or .rtz. For a multi-mod bundle, import its mods separately.");
        }
        String entryName = scan.mods.get(0);
        String filename = entryName.substring(entryName.lastIndexOf('/') + 1);
        checkFilename(filename);
        Path extracted = staging.resolve("mod");
        try (ZipFile zip = new ZipFile(source.toFile()); InputStream stream = zip.getInputStream(zip.getEntry(entryName));
                OutputStream output = Files.newOutputStream(extracted, StandardOpenOption.CREATE_NEW)) {
            copyBounded(stream, output, MAX_INPUT);
        }
        Candidate candidate = candidate(extracted, filename, scan(extracted, budget, false));
        Set<String> expected = NativeMod.libraries(candidate.manifest);
        String parent = entryName.substring(0, entryName.length() - filename.length());
        Set<String> expectedPaths = new HashSet<>();
        for (String library : expected) expectedPaths.add(parent + library);
        if (!expectedPaths.equals(new HashSet<>(scan.libraries)))
            throw new IOException("ZIP must contain exactly the declared Android .so bridge beside the .nrm, with no extra native libraries.");
        try (ZipFile zip = new ZipFile(source.toFile())) {
            for (String library : expected) {
                Path target = staging.resolve(library); // Name validated by NativeMod, never an archive path.
                try (InputStream stream = zip.getInputStream(zip.getEntry(parent + library));
                        OutputStream output = Files.newOutputStream(target, StandardOpenOption.CREATE_NEW)) {
                    copyBounded(stream, output, NativeMod.MAX_LIBRARY);
                }
                NativeMod.validate(target);
                candidate.libraries.put(library, target);
            }
        }
        return candidate;
    }
    private static Candidate candidate(Path file, String name, Scan scan) throws IOException {
        if (scan.manifest == null && (!name.toLowerCase(Locale.ROOT).endsWith(".rtz") || !scan.textures)) {
            throw new IOException("Not a recomp mod: missing root mod.json (or rt64.json for textures).");
        }
        String normalizedName = name.substring(0, name.length() - 4) + name.substring(name.length() - 4).toLowerCase(Locale.ROOT);
        return new Candidate(file, normalizedName, scan.manifest);
    }
    static byte[] readManifest(Path file) throws IOException {
        try (ZipFile zip = new ZipFile(file.toFile())) {
            ZipEntry entry = zip.getEntry("mod.json");
            if (entry == null) {
                if (file.getFileName().toString().toLowerCase(Locale.ROOT).endsWith(".rtz") && zip.getEntry("rt64.json") != null) return null;
                throw new IOException("Missing mod manifest.");
            }
            try (InputStream input = zip.getInputStream(entry); ByteArrayOutputStream output = new ByteArrayOutputStream()) {
                copyBounded(input, output, MAX_MANIFEST);
                return output.toByteArray();
            }
        }
    }
    private static Scan scan(Path source, Budget budget, boolean allowCompanions) throws IOException {
        Scan result = new Scan();
        Set<String> seen = new HashSet<>();
        try (ZipFile zip = new ZipFile(source.toFile())) {
            Enumeration<? extends ZipEntry> entries = zip.entries();
            while (entries.hasMoreElements()) {
                ZipEntry entry = entries.nextElement();
                String name = entry.getName();
                if (++budget.entries > MAX_ENTRIES) throw new IOException("Too many entries in mod package.");
                if (name.isEmpty() || name.length() > 512 || name.startsWith("/") || name.contains("\\") || name.contains(":")) {
                    throw new IOException("Unsafe path in mod package.");
                }
                for (int i = 0; i < name.length(); i++) if (Character.isISOControl(name.charAt(i))) throw new IOException("Unsafe path in mod package.");
                for (String part : name.split("/")) if (part.isEmpty() || part.equals(".") || part.equals("..")) throw new IOException("Unsafe path in mod package.");
                if (!seen.add(name.toLowerCase(Locale.ROOT))) throw new IOException("Duplicate path in mod package.");
                String lower = name.toLowerCase(Locale.ROOT);
                if (lower.endsWith(".dll") || lower.endsWith(".dylib") || lower.endsWith(".so") || lower.contains(".so.")) {
                    if (!allowCompanions || !name.endsWith(".so") || entry.isDirectory())
                        throw new IOException("Use an Android ZIP with the .so beside the .nrm. Embedded, desktop and versioned native libraries are unsupported.");
                    if (entry.getSize() > NativeMod.MAX_LIBRARY) throw new IOException("Native bridge is too large.");
                    result.libraries.add(name);
                }
                if (entry.isDirectory()) continue;
                if (entry.getSize() < 0 || entry.getSize() > MAX_EXPANDED - budget.bytes) throw new IOException("Expanded mod package is too large.");
                boolean manifest = name.equals("mod.json");
                if (manifest && entry.getSize() > MAX_MANIFEST) throw new IOException("Mod manifest is too large.");
                ByteArrayOutputStream data = manifest ? new ByteArrayOutputStream() : null;
                CRC32 crc = new CRC32();
                long actual = 0;
                try (InputStream stream = zip.getInputStream(entry)) {
                    byte[] buffer = new byte[32768];
                    int count;
                    while ((count = stream.read(buffer)) != -1) {
                        budget.bytes += count;
                        actual += count;
                        if (budget.bytes > MAX_EXPANDED || (manifest && actual > MAX_MANIFEST)) throw new IOException("Expanded mod package is too large.");
                        crc.update(buffer, 0, count);
                        if (data != null) data.write(buffer, 0, count);
                    }
                }
                if (actual != entry.getSize() || crc.getValue() != entry.getCrc()) throw new IOException("Corrupt mod package entry.");
                if (manifest) result.manifest = data.toByteArray();
                if (name.equals("rt64.json")) result.textures = true;
                if (isMod(name)) result.mods.add(name);
            }
        }
        return result;
    }
}
