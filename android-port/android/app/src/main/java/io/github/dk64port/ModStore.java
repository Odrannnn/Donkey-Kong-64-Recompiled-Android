package io.github.dk64port;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import org.json.*;

/** Call only while holding ModSession. Uses the runtime's existing mods.json format. */
final class ModStore {
    final File filesDirectory;
    final Path directory;
    private final Path configPath;
    private final ModTransaction transaction;
    static final class Mod {
        final Path file;
        final String id, name, version, error;
        final boolean defaultEnabled;
        final Set<String> libraries;
        boolean enabled;
        Mod(Path file, String id, String name, String version, boolean defaultEnabled, String error) {
            this(file, id, name, version, defaultEnabled, error, Collections.emptySet());
        }
        Mod(Path file, String id, String name, String version, boolean defaultEnabled, String error, Set<String> libraries) {
            this.file = file; this.id = id; this.name = name; this.version = version;
            this.defaultEnabled = defaultEnabled; this.error = error;
            this.libraries = libraries;
        }
    }
    static final class Pending implements AutoCloseable {
        final Path staging;
        final ModArchive.Candidate candidate;
        final Mod mod;
        Pending(Path staging, ModArchive.Candidate candidate, Mod mod) {
            this.staging = staging; this.candidate = candidate; this.mod = mod;
        }
        @Override public void close() throws IOException { DriverArchive.removePrivateTree(staging); }
    }
    ModStore(File filesDirectory) throws IOException {
        this(filesDirectory, null);
    }
    ModStore(File filesDirectory, ModTransaction.Checkpoint checkpoint) throws IOException {
        this.filesDirectory = filesDirectory;
        directory = new File(filesDirectory, "data/mods").toPath();
        configPath = new File(filesDirectory, "data/mods.json").toPath();
        Files.createDirectories(directory);
        if (!Files.isDirectory(directory, LinkOption.NOFOLLOW_LINKS)) throw new IOException("Invalid mods directory.");
        transaction = new ModTransaction(filesDirectory, directory, checkpoint);
        transaction.recover();
    }
    Pending prepare(InputStream input, String filename) throws Exception {
        Path stagingRoot = new File(filesDirectory, "mod-staging").toPath();
        Files.createDirectories(stagingRoot);
        Path staging = Files.createTempDirectory(stagingRoot, "import-");
        try {
            ModArchive.Candidate candidate = ModArchive.prepare(input, filename, staging);
            return new Pending(staging, candidate, describe(candidate.file, candidate.filename, candidate.manifest));
        } catch (Exception error) {
            DriverArchive.removePrivateTree(staging);
            throw error;
        }
    }
    private static String string(JSONObject json, String key) throws Exception {
        Object value = json.get(key);
        if (!(value instanceof String) || ((String)value).isEmpty()) throw new IOException("Invalid mod field: " + key);
        return (String)value;
    }
    private static Mod describe(Path path, String filename, byte[] bytes) throws Exception {
        if (bytes == null) {
            String id = filename.substring(0, filename.length() - 4);
            return new Mod(path, id, id, "Texture pack", true, null);
        }
        JSONObject json = new JSONObject(new String(bytes, StandardCharsets.UTF_8));
        if (!string(json, "game_id").equals("dk64")) throw new IOException("This mod is for another game, not DK64.");
        String id = string(json, "id");
        if (!id.matches("[A-Za-z0-9_][A-Za-z0-9_.-]{0,127}")) throw new IOException("Unsupported mod ID.");
        String name = string(json, "display_name");
        String version = string(json, "version");
        String minimum = string(json, "minimum_recomp_version");
        String versionPattern = "[0-9]+\\.[0-9]+\\.[0-9]+(?:[-+][A-Za-z0-9.+-]+)?";
        if (!version.matches(versionPattern) || !minimum.matches(versionPattern)) throw new IOException("Invalid version in mod manifest.");
        JSONArray authors = json.getJSONArray("authors");
        for (int i = 0; i < authors.length(); i++) if (!(authors.get(i) instanceof String)) throw new IOException("Invalid mod authors.");
        Set<String> libraries = NativeMod.libraries(bytes);
        Object enabled = json.opt("enabled_by_default");
        if (enabled != null && !(enabled instanceof Boolean)) throw new IOException("Invalid enabled_by_default field.");
        return new Mod(path, id, name, version, enabled == null || (Boolean)enabled, null, libraries);
    }
    List<Mod> list() throws Exception {
        JSONObject config = readConfig();
        Set<String> enabled = strings(config, "enabled_mods");
        Set<String> order = strings(config, "mod_order");
        List<Mod> mods = new ArrayList<>();
        try (DirectoryStream<Path> entries = Files.newDirectoryStream(directory)) {
            for (Path path : entries) {
                if (!ModArchive.isMod(path.getFileName().toString()) || !Files.isRegularFile(path, LinkOption.NOFOLLOW_LINKS)) continue;
                Mod mod;
                try {
                    mod = describe(path, path.getFileName().toString(), ModArchive.readManifest(path));
                    for (String library : mod.libraries) if (!Files.isRegularFile(directory.resolve(library), LinkOption.NOFOLLOW_LINKS)) {
                        mod = new Mod(path, mod.id, mod.name, mod.version, mod.defaultEnabled,
                            "Missing native bridge. Reimport the complete Android ZIP.", mod.libraries);
                        break;
                    }
                }
                catch (Exception error) { mod = new Mod(path, "", path.getFileName().toString(), "", false, error.getMessage()); }
                mod.enabled = mod.error == null && (enabled.contains(mod.id) || (!order.contains(mod.id) && mod.defaultEnabled));
                mods.add(mod);
            }
        }
        mods.sort(Comparator.comparing(mod -> mod.name.toLowerCase(Locale.ROOT)));
        return mods;
    }
    boolean isUpdate(Pending pending) throws Exception {
        for (Mod mod : list()) if (mod.id.equals(pending.mod.id)) return true;
        return false;
    }
    void install(Pending pending) throws Exception {
        List<Mod> installed = list();
        Path target = directory.resolve(pending.candidate.filename);
        Mod previous = null;
        int matching = 0;
        for (Mod mod : installed) if (mod.id.equals(pending.mod.id)) { target = mod.file; previous = mod; matching++; }
        if (matching > 1) throw new IOException("Duplicate installed mod IDs. Remove duplicate versions first.");
        if (matching == 1 && !target.toString().endsWith(pending.candidate.filename.substring(pending.candidate.filename.length() - 4))) {
            throw new IOException("This update changes the mod container type. Remove the old version first.");
        }
        if (Files.exists(target) && matching == 0) throw new IOException("A different mod already uses that filename. Rename your import or remove that file first.");
        if (!pending.mod.libraries.equals(pending.candidate.libraries.keySet())) throw new IOException("Incomplete native bundle.");
        Set<String> oldLibraries = previous == null ? Collections.emptySet() : previous.libraries;
        Set<String> affectedLibraries = new LinkedHashSet<>(oldLibraries);
        affectedLibraries.addAll(pending.mod.libraries);
        checkLibraryOwnership(affectedLibraries, previous, installed);
        for (String name : pending.mod.libraries) {
            NativeMod.validate(pending.candidate.libraries.get(name));
            if (!oldLibraries.contains(name) && Files.exists(directory.resolve(name), LinkOption.NOFOLLOW_LINKS))
                throw new IOException("A native library with this filename already exists. It will not be overwritten: " + name);
        }
        Set<String> newNames = new HashSet<>(pending.mod.libraries);
        newNames.add(target.getFileName().toString());
        try (DirectoryStream<Path> entries = Files.newDirectoryStream(directory)) {
            for (Path entry : entries) for (String name : newNames) {
                String existing = entry.getFileName().toString();
                if (!existing.equals(name) && existing.equalsIgnoreCase(name)) throw new IOException("Case-conflicting installed filename: " + name);
            }
        }
        Map<String, Path> replacements = new LinkedHashMap<>(pending.candidate.libraries);
        replacements.put(target.getFileName().toString(), pending.candidate.file);
        Set<String> removals = new LinkedHashSet<>(oldLibraries);
        removals.removeAll(pending.mod.libraries);
        transaction.apply(replacements, removals);
    }
    void setEnabled(Mod mod, boolean enabled) throws Exception {
        checkedPath(mod.file);
        if (mod.error != null) throw new IOException("Cannot enable an invalid mod.");
        JSONObject config = readConfig();
        Set<String> selected = strings(config, "enabled_mods");
        Set<String> order = strings(config, "mod_order");
        order.add(mod.id); // Otherwise a default-enabled mod is treated as newly installed at next scan.
        if (enabled) selected.add(mod.id); else selected.remove(mod.id);
        config.put("enabled_mods", new JSONArray(selected));
        config.put("mod_order", new JSONArray(order));
        writeConfig(config);
    }
    private static void checkLibraryOwnership(Set<String> libraries, Mod owner, List<Mod> installed) throws IOException {
        for (Mod other : installed) {
            if (owner != null && other.file.equals(owner.file)) continue;
            for (String library : libraries) for (String occupied : other.libraries)
                if (library.equalsIgnoreCase(occupied)) throw new IOException("Native bridge filename is also used by " + other.name + ". Shared companion files are unsupported.");
        }
    }
    void remove(Mod mod) throws Exception {
        Path file = checkedPath(mod.file);
        List<Mod> installed = list();
        Mod current = null;
        for (Mod candidate : installed) if (candidate.file.equals(file)) current = candidate;
        if (current == null) throw new IOException("Mod is no longer installed.");
        // If metadata is corrupt, delete only the selected archive; never guess ownership of a .so.
        checkLibraryOwnership(current.libraries, current, installed);
        Set<String> removals = new LinkedHashSet<>(current.libraries);
        removals.add(file.getFileName().toString());
        transaction.apply(Collections.emptyMap(), removals); // Keep saves and per-mod configuration.
    }
    private Path checkedPath(Path path) throws IOException {
        if (!path.toAbsolutePath().normalize().getParent().equals(directory.toAbsolutePath().normalize())
                || !Files.isRegularFile(path, LinkOption.NOFOLLOW_LINKS)) throw new IOException("Invalid installed mod path.");
        return path;
    }
    private static Set<String> strings(JSONObject json, String key) throws Exception {
        Set<String> result = new LinkedHashSet<>();
        if (!json.has(key)) return result;
        JSONArray values = json.getJSONArray(key);
        for (int i = 0; i < values.length(); i++) {
            Object value = values.get(i);
            if (!(value instanceof String)) throw new IOException("Invalid mod configuration: " + key);
            result.add((String)value);
        }
        return result;
    }
    private JSONObject readConfig() throws Exception {
        Exception failure = null;
        for (Path path : new Path[] { configPath, configPath.resolveSibling("mods.json.bak") }) {
            if (!Files.exists(path)) continue;
            try {
                if (Files.size(path) > 1024 * 1024) throw new IOException("Mod configuration is too large.");
                JSONObject config = new JSONObject(new String(Files.readAllBytes(path), StandardCharsets.UTF_8));
                strings(config, "enabled_mods");
                strings(config, "mod_order");
                return config;
            } catch (Exception error) { failure = error; }
        }
        if (failure != null) throw new IOException("Cannot read mods.json or its backup; existing settings were left unchanged.", failure);
        return new JSONObject();
    }
    private void writeConfig(JSONObject config) throws Exception {
        // Preserve other runtime fields (including latest_game_mode), and use its backup convention.
        Path temporary = configPath.resolveSibling("mods.json.android-temp");
        byte[] bytes = config.toString(2).getBytes(StandardCharsets.UTF_8);
        try (FileOutputStream output = new FileOutputStream(temporary.toFile())) { output.write(bytes); output.getFD().sync(); }
        Files.move(temporary, configPath, StandardCopyOption.ATOMIC_MOVE, StandardCopyOption.REPLACE_EXISTING);
        // Both copies must reflect a disable operation, even if the next runtime read needs its backup.
        Files.copy(configPath, configPath.resolveSibling("mods.json.bak"), StandardCopyOption.REPLACE_EXISTING);
    }
}
