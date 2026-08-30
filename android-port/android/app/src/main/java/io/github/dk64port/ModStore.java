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
    static final class Mod {
        final Path file;
        final String id, name, version, error;
        final boolean defaultEnabled;
        boolean enabled;
        Mod(Path file, String id, String name, String version, boolean defaultEnabled, String error) {
            this.file = file; this.id = id; this.name = name; this.version = version;
            this.defaultEnabled = defaultEnabled; this.error = error;
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
        this.filesDirectory = filesDirectory;
        directory = new File(filesDirectory, "data/mods").toPath();
        configPath = new File(filesDirectory, "data/mods.json").toPath();
        Files.createDirectories(directory);
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
        Object libraries = json.opt("native_libraries");
        if (libraries != null && (!(libraries instanceof JSONObject) || ((JSONObject)libraries).length() != 0)) {
            throw new IOException("This mod requires native libraries. Android native-mod packages are not supported by this importer yet.");
        }
        Object enabled = json.opt("enabled_by_default");
        if (enabled != null && !(enabled instanceof Boolean)) throw new IOException("Invalid enabled_by_default field.");
        return new Mod(path, id, name, version, enabled == null || (Boolean)enabled, null);
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
                try { mod = describe(path, path.getFileName().toString(), ModArchive.readManifest(path)); }
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
        int matching = 0;
        for (Mod mod : installed) if (mod.id.equals(pending.mod.id)) { target = mod.file; matching++; }
        if (matching > 1) throw new IOException("Duplicate installed mod IDs. Remove duplicate versions first.");
        if (matching == 1 && !target.toString().endsWith(pending.candidate.filename.substring(pending.candidate.filename.length() - 4))) {
            throw new IOException("This update changes the mod container type. Remove the old version first.");
        }
        if (Files.exists(target) && matching == 0) throw new IOException("A different mod already uses that filename. Rename your import or remove that file first.");
        // One mod per transaction: atomic replacement leaves either the old or the new complete archive.
        Files.move(pending.candidate.file, target, StandardCopyOption.ATOMIC_MOVE, StandardCopyOption.REPLACE_EXISTING);
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
    void remove(Mod mod) throws IOException {
        Files.delete(checkedPath(mod.file)); // Keep saves, per-mod settings and remembered activation state.
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
