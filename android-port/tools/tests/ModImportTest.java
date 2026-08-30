package io.github.dk64port;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.util.zip.*;
import org.json.*;

public final class ModImportTest {
    private static int passed;
    private interface Task { void run() throws Exception; }
    private static void check(boolean value, String message) { if (!value) throw new AssertionError(message); }
    private static void test(String label, Task task) throws Exception { task.run(); passed++; System.out.println("PASS " + label); }
    private static void rejects(Task task) throws Exception {
        try { task.run(); } catch (IOException | JSONException expected) { return; }
        throw new AssertionError("Expected rejection");
    }
    private static byte[] zip(Map<String, byte[]> files) throws Exception {
        ByteArrayOutputStream data = new ByteArrayOutputStream();
        try (ZipOutputStream zip = new ZipOutputStream(data)) {
            for (Map.Entry<String, byte[]> entry : files.entrySet()) {
                zip.putNextEntry(new ZipEntry(entry.getKey())); zip.write(entry.getValue()); zip.closeEntry();
            }
        }
        return data.toByteArray();
    }
    private static String manifest(String id, String version) {
        return "{\"game_id\":\"dk64\",\"id\":\"" + id + "\",\"display_name\":\"Android Mod Manager Test\","
            + "\"version\":\"" + version + "\",\"authors\":[\"Local test fixture\"],\"minimum_recomp_version\":\"1.0.1\",\"enabled_by_default\":false}";
    }
    private static byte[] nrm(String id, String version) throws Exception {
        return zip(Map.of("mod.json", manifest(id, version).getBytes(StandardCharsets.UTF_8)));
    }
    private static ModStore.Pending prepare(ModStore store, byte[] data, String filename) throws Exception {
        return store.prepare(new ByteArrayInputStream(data), filename);
    }
    public static void main(String[] args) throws Exception {
        Path root = Files.createTempDirectory(Path.of(args[0]), "case-");
        ModStore store = new ModStore(root.toFile());
        byte[] original = nrm("dk64_android_import_smoke", "1.0.0");
        test("valid import and normalized extension", () -> {
            try (ModStore.Pending pending = prepare(store, original, "Test.NRM")) { store.install(pending); }
            check(Files.exists(store.directory.resolve("Test.nrm")), "Canonical extension needed by runtime");
            check(!store.list().get(0).enabled, "Must respect default disabled manifest");
        });
        test("update matches ID without leaving a duplicate", () -> {
            byte[] updated = nrm("dk64_android_import_smoke", "1.1.0");
            try (ModStore.Pending pending = prepare(store, updated, "Another name.nrm")) {
                check(store.isUpdate(pending), "Update should be recognized"); store.install(pending);
            }
            check(store.list().size() == 1 && store.list().get(0).version.equals("1.1.0"), "Single new version expected");
            check(!Files.exists(store.directory.resolve("Another name.nrm")), "Old filename should be reused");
        });
        test("cancel leaves installed version intact", () -> {
            byte[] before = Files.readAllBytes(store.directory.resolve("Test.nrm"));
            try (ModStore.Pending ignored = prepare(store, original, "Test.nrm")) { }
            check(Arrays.equals(before, Files.readAllBytes(store.directory.resolve("Test.nrm"))), "Cancel replaced installed archive");
        });
        test("ZIP with nested mod and documentation", () -> {
            byte[] bundle = zip(Map.of("release/Nested.nrm", nrm("nested", "1.0.0"), "README.md", new byte[0]));
            try (ModStore.Pending pending = prepare(store, bundle, "Release.zip")) { store.install(pending); }
            check(store.list().size() == 2, "Nested mod was not imported");
        });
        test("traversal, absolute and drive paths rejected", () -> {
            for (String path : new String[] { "../escape.nrm", "/escape.nrm", "C:/escape.nrm", "a\\escape.nrm", "a/./escape.nrm" }) {
                rejects(() -> prepare(store, zip(Map.of(path, original)), "bad.zip"));
            }
        });
        test("case-duplicate archive paths rejected", () -> rejects(() -> prepare(store,
            zip(Map.of("mod.json", new byte[0], "MOD.JSON", new byte[0])), "duplicate.nrm")));
        test("malformed and oversized manifests rejected", () -> {
            rejects(() -> prepare(store, zip(Map.of("mod.json", "{".getBytes())), "bad.nrm"));
            rejects(() -> prepare(store, zip(Map.of("mod.json", new byte[ModArchive.MAX_MANIFEST + 1])), "big.nrm"));
        });
        test("wrong game and native requirements rejected", () -> {
            String wrong = manifest("wrong", "1.0.0").replace("\"dk64\"", "\"mm\"");
            rejects(() -> prepare(store, zip(Map.of("mod.json", wrong.getBytes())), "wrong.nrm"));
            String nativeMod = manifest("native", "1.0.0").replace("\"enabled_by_default\":false", "\"native_libraries\":{\"native\":[\"init\"]}");
            rejects(() -> prepare(store, zip(Map.of("mod.json", nativeMod.getBytes())), "native.nrm"));
            rejects(() -> prepare(store, zip(Map.of("test.nrm", original, "mod.dll", new byte[0])), "native.zip"));
            rejects(() -> prepare(store, original, "test.offline.nrm"));
        });
        test("AP worlds, invalid ZIP and ambiguous bundles rejected", () -> {
            rejects(() -> prepare(store, original, "dk64.apworld"));
            rejects(() -> prepare(store, new byte[] { 1, 2, 3 }, "invalid.nrm"));
            rejects(() -> prepare(store, zip(Map.of("a.nrm", original, "b.nrm", original)), "multiple.zip"));
        });
        test("bounded stream refuses excess data", () -> rejects(() -> ModArchive.copyBounded(
            new ByteArrayInputStream(new byte[33]), new ByteArrayOutputStream(), 32)));
        test("CRC mismatch and forged expansion size rejected", () -> {
            for (int field : new int[] { 16, 24 }) {
                byte[] damaged = original.clone();
                for (int i = 0; i < damaged.length - 46; i++) {
                    if (damaged[i] == 0x50 && damaged[i + 1] == 0x4b && damaged[i + 2] == 1 && damaged[i + 3] == 2) {
                        if (field == 16) damaged[i + field] ^= 1;
                        else { damaged[i + 24] = 1; damaged[i + 25] = 0; damaged[i + 26] = 0; damaged[i + 27] = (byte)0x80; }
                        break;
                    }
                }
                rejects(() -> prepare(store, damaged, "corrupt.nrm"));
            }
        });
        test("entry count limit", () -> {
            Map<String, byte[]> files = new LinkedHashMap<>();
            for (int i = 0; i <= ModArchive.MAX_ENTRIES; i++) files.put("entry" + i, new byte[0]);
            rejects(() -> prepare(store, zip(files), "many.zip"));
        });
        test("texture pack without mod.json", () -> {
            try (ModStore.Pending pending = prepare(store, zip(Map.of("rt64.json", "{}".getBytes())), "Textures.rtz")) {
                check(pending.mod.id.equals("Textures"), "Texture ID must match runtime's filename stem");
            }
            rejects(() -> prepare(store, zip(Map.of("image.png", new byte[0])), "Textures.rtz"));
        });
        test("enable/disable persists and preserves other runtime settings", () -> {
            Path config = root.resolve("data/mods.json");
            Files.writeString(config, "{\"latest_game_mode\":\"other_mode\",\"mod_order\":[\"other\"],\"enabled_mods\":[\"other\"]}");
            ModStore.Mod mod = store.list().stream().filter(m -> m.id.equals("nested")).findFirst().orElseThrow();
            store.setEnabled(mod, true);
            check(new ModStore(root.toFile()).list().stream().filter(m -> m.id.equals("nested")).findFirst().orElseThrow().enabled, "Enable did not persist");
            store.setEnabled(mod, false);
            JSONObject json = new JSONObject(Files.readString(config));
            check(json.getString("latest_game_mode").equals("other_mode"), "Game mode changed");
            check(json.getJSONArray("enabled_mods").length() == 1 && json.getJSONArray("mod_order").length() == 2, "Unrelated state lost");
            check(!new ModStore(root.toFile()).list().stream().filter(m -> m.id.equals("nested")).findFirst().orElseThrow().enabled, "Disable did not persist");
        });
        test("default-enabled mod can be disabled on first scan", () -> {
            byte[] data = zip(Map.of("mod.json", manifest("default_on", "1.0.0").replace("false", "true").getBytes()));
            try (ModStore.Pending pending = prepare(store, data, "Default.nrm")) { store.install(pending); }
            ModStore.Mod mod = store.list().stream().filter(m -> m.id.equals("default_on")).findFirst().orElseThrow();
            check(mod.enabled, "Default should enable new mod"); store.setEnabled(mod, false);
            check(!store.list().stream().filter(m -> m.id.equals("default_on")).findFirst().orElseThrow().enabled, "Default overrode explicit disable");
        });
        test("backup recovery and refusal to overwrite unreadable configuration", () -> {
            Path config = root.resolve("data/mods.json");
            Files.writeString(config, "{"); check(!store.list().isEmpty(), "Backup recovery failed");
            Files.writeString(root.resolve("data/mods.json.bak"), "{");
            rejects(store::list);
            check(Files.readString(config).equals("{"), "Damaged config was overwritten");
            Files.writeString(config, "{}");
        });
        test("same filename different ID cannot overwrite a mod", () -> {
            try (ModStore.Pending pending = prepare(store, nrm("different", "1.0.0"), "Test.nrm")) { rejects(() -> store.install(pending)); }
        });
        test("session lock excludes concurrent game/mod operations", () -> {
            try (ModSession ignored = ModSession.acquire(root.toFile())) { rejects(() -> ModSession.acquire(root.toFile())); }
            try (ModSession ignored = ModSession.acquire(root.toFile())) { }
        });
        test("remove preserves saves and outside files", () -> {
            Path save = root.resolve("data/test-save.bin"); Files.writeString(save, "keep");
            ModStore.Mod mod = store.list().get(0); store.remove(mod);
            check(!Files.exists(mod.file) && Files.readString(save).equals("keep"), "Unexpected removal");
            ModStore.Mod outside = new ModStore.Mod(save, "", "", "", false, null);
            rejects(() -> store.remove(outside)); check(Files.exists(save), "Outside file removed");
        });
        Files.write(Path.of(args[0]).getParent().resolve("Android Mod Manager Test.nrm"), original);
        DriverArchive.removePrivateTree(root);
        System.out.println(passed + " mod import/store checks passed. Fixture has metadata only; it does not modify the game.");
    }
}
