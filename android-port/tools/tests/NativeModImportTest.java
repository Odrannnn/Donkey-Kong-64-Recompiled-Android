package io.github.dk64port;

import java.io.*;
import java.nio.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.util.zip.*;
import org.json.*;

/** Host checks use structural ELF fixtures, never execute them or claim game compatibility. */
public final class NativeModImportTest {
    private static int passed;
    private static Path scratch;
    private interface Task { void run() throws Exception; }
    private static final class InterruptedProcess extends Error { }
    private static void check(boolean value, String message) { if (!value) throw new AssertionError(message); }
    private static void test(String label, Task task) throws Exception { task.run(); passed++; System.out.println("PASS " + label); }
    private static void rejects(Task task) throws Exception {
        try { task.run(); } catch (IOException | JSONException expected) { return; }
        throw new AssertionError("Expected rejection");
    }
    private static byte[] zip(Map<String, byte[]> files) throws Exception {
        ByteArrayOutputStream data = new ByteArrayOutputStream();
        try (ZipOutputStream zip = new ZipOutputStream(data)) {
            for (Map.Entry<String, byte[]> file : files.entrySet()) {
                zip.putNextEntry(new ZipEntry(file.getKey())); zip.write(file.getValue()); zip.closeEntry();
            }
        }
        return data.toByteArray();
    }
    private static byte[] elf(int marker) {
        ByteBuffer data = ByteBuffer.allocate(128).order(ByteOrder.LITTLE_ENDIAN);
        data.put(new byte[] { 0x7f, 'E', 'L', 'F', 2, 1, 1 });
        data.putShort(16, (short)3).putShort(18, (short)183).putInt(20, 1).putLong(32, 64);
        data.putShort(52, (short)64).putShort(54, (short)56).putShort(56, (short)1);
        data.putInt(64, 1).putInt(68, 5).putLong(96, 128).putLong(104, 128).putLong(112, 16384);
        data.putInt(124, marker);
        return data.array();
    }
    private static byte[] nrm(String id, String version, String stem) throws Exception {
        JSONObject manifest = new JSONObject().put("game_id", "dk64").put("id", id).put("display_name", id)
            .put("version", version).put("authors", new JSONArray().put("Local test fixture"))
            .put("minimum_recomp_version", "1.0.1").put("enabled_by_default", false);
        if (stem != null) manifest.put("native_libraries", new JSONObject().put(stem, new JSONArray().put("bridge_ping")));
        return zip(Map.of("mod.json", manifest.toString().getBytes(StandardCharsets.UTF_8)));
    }
    private static byte[] bundle(String id, String version, String stem, int marker) throws Exception {
        return zip(Map.of("release/Test.nrm", nrm(id, version, stem), "release/" + stem + ".so", elf(marker), "README.md", new byte[0]));
    }
    private static void install(ModStore store, byte[] bytes) throws Exception {
        try (ModStore.Pending pending = store.prepare(new ByteArrayInputStream(bytes), "Android.zip")) { store.install(pending); }
    }
    private static ModStore store() throws Exception { return new ModStore(Files.createTempDirectory(scratch, "native-").toFile()); }
    private static ModStore original() throws Exception {
        ModStore store = store(); install(store, bundle("coop", "1.0.0", "dk64_bridge", 1)); return store;
    }
    private static void assertOriginal(ModStore store) throws Exception {
        check(store.list().size() == 1 && store.list().get(0).version.equals("1.0.0"), "Original mod version lost");
        check(Arrays.equals(Files.readAllBytes(store.directory.resolve("dk64_bridge.so")), elf(1)), "Original bridge lost");
        check(!Files.exists(store.directory.resolve("new_bridge.so")), "Stray updated library left behind");
    }
    public static void main(String[] args) throws Exception {
        scratch = Path.of(args[0]);
        test("native ZIP installs bridge beside unchanged nrm", () -> {
            ModStore store = original(); assertOriginal(store);
            check(Arrays.equals(Files.readAllBytes(store.directory.resolve("Test.nrm")), nrm("coop", "1.0.0", "dk64_bridge")), "Importer rewrote mod archive");
            check(!store.list().get(0).enabled, "Disabled default lost");
        });
        test("native update replaces code and bridge together", () -> {
            ModStore store = original(); install(store, bundle("coop", "1.1.0", "dk64_bridge", 2));
            check(store.list().get(0).version.equals("1.1.0"), "Mod not updated");
            check(Arrays.equals(Files.readAllBytes(store.directory.resolve("dk64_bridge.so")), elf(2)), "Old bridge remained");
        });
        test("renamed bridge update removes obsolete companion", () -> {
            ModStore store = original(); install(store, bundle("coop", "1.1.0", "new_bridge", 2));
            check(!Files.exists(store.directory.resolve("dk64_bridge.so")) && Files.exists(store.directory.resolve("new_bridge.so")), "Obsolete library retained");
        });
        test("native to ordinary mod update removes old bridge", () -> {
            ModStore store = original(); install(store, zip(Map.of("Test.nrm", nrm("coop", "1.1.0", null))));
            check(!Files.exists(store.directory.resolve("dk64_bridge.so")) && store.list().get(0).libraries.isEmpty(), "Old native dependency remained");
        });
        test("cancel does not change native installation", () -> {
            ModStore store = original();
            try (ModStore.Pending ignored = store.prepare(new ByteArrayInputStream(bundle("coop", "1.1.0", "new_bridge", 2)), "Android.zip")) { }
            assertOriginal(store);
        });
        test("disable keeps bridge; removal preserves saves", () -> {
            ModStore store = original(); ModStore.Mod mod = store.list().get(0);
            store.setEnabled(mod, true); store.setEnabled(mod, false);
            check(Files.exists(store.directory.resolve("dk64_bridge.so")), "Disabling deleted bridge");
            Path save = store.directory.getParent().resolve("save.bin"); Files.writeString(save, "keep");
            store.remove(mod);
            check(store.list().isEmpty() && !Files.exists(store.directory.resolve("dk64_bridge.so")) && Files.readString(save).equals("keep"), "Removal touched wrong files");
        });
        test("missing or misplaced bridge rejected", () -> {
            ModStore store = store(); byte[] mod = nrm("coop", "1.0.0", "dk64_bridge");
            rejects(() -> install(store, zip(Map.of("Test.nrm", mod))));
            rejects(() -> install(store, zip(Map.of("release/Test.nrm", mod, "dk64_bridge.so", elf(1)))));
        });
        test("undeclared and extra native companions rejected", () -> {
            ModStore store = store();
            rejects(() -> install(store, zip(Map.of("Test.nrm", nrm("coop", "1.0.0", null), "dk64_bridge.so", elf(1)))));
            rejects(() -> install(store, zip(Map.of("Test.nrm", nrm("coop", "1.0.0", "dk64_bridge"), "dk64_bridge.so", elf(1), "extra.so", elf(1)))));
        });
        test("embedded native library remains unsupported", () -> {
            ModStore store = store();
            byte[] embedded = zip(Map.of("mod.json", "{}".getBytes(), "dk64_bridge.so", elf(1)));
            rejects(() -> store.prepare(new ByteArrayInputStream(embedded), "Embedded.nrm"));
        });
        test("malformed native declarations rejected", () -> {
            for (String stem : new String[] { "../bridge", "bridge.so", "bridge/path", "C:bridge", "" })
                rejects(() -> NativeMod.libraries(new JSONObject().put("native_libraries", new JSONObject().put(stem, new JSONArray())).toString().getBytes()));
            for (Object field : new Object[] { JSONObject.NULL, new JSONArray(), "bad",
                    new JSONObject().put("one", new JSONArray()).put("two", new JSONArray()),
                    new JSONObject().put("one", new JSONArray().put("ping").put("ping")),
                    new JSONObject().put("one", new JSONArray().put(7)) })
                rejects(() -> NativeMod.libraries(new JSONObject().put("native_libraries", field).toString().getBytes()));
        });
        test("wrong architecture, truncation and 4 KiB ELF rejected", () -> {
            ModStore store = store();
            for (int field : new int[] { 4, 16, 18, 54, 56, 112 }) {
                byte[] bad = elf(1); bad[field] ^= 1;
                rejects(() -> install(store, zip(Map.of("Test.nrm", nrm("coop", "1.0.0", "dk64_bridge"), "dk64_bridge.so", bad))));
            }
            byte[] smallAlignment = elf(1); ByteBuffer.wrap(smallAlignment).order(ByteOrder.LITTLE_ENDIAN).putLong(112, 4096);
            rejects(() -> install(store, zip(Map.of("Test.nrm", nrm("coop", "1.0.0", "dk64_bridge"), "dk64_bridge.so", smallAlignment))));
            rejects(() -> install(store, zip(Map.of("Test.nrm", nrm("coop", "1.0.0", "dk64_bridge"), "dk64_bridge.so", new byte[12]))));
        });
        test("native library cannot overwrite another mod's companion", () -> {
            ModStore store = original();
            byte[] other = zip(Map.of("Other.nrm", nrm("other", "1.0.0", "dk64_bridge"), "dk64_bridge.so", elf(2)));
            rejects(() -> install(store, other)); assertOriginal(store);
        });
        test("unmanaged and case-conflicting native files are preserved", () -> {
            for (String name : new String[] { "dk64_bridge.so", "DK64_BRIDGE.so" }) {
                ModStore store = store(); Files.writeString(store.directory.resolve(name), "keep");
                rejects(() -> install(store, bundle("coop", "1.0.0", "dk64_bridge", 1)));
                check(Files.readString(store.directory.resolve(name)).equals("keep"), "Unmanaged file overwritten");
            }
        });
        test("missing installed bridge is reported and full ZIP repairs it", () -> {
            ModStore store = original(); Files.delete(store.directory.resolve("dk64_bridge.so"));
            check(store.list().get(0).error != null && !store.list().get(0).enabled, "Missing library not detected");
            install(store, bundle("coop", "1.0.0", "dk64_bridge", 1)); assertOriginal(store);
        });
        test("I/O failure rolls both files back", () -> {
            ModStore original = original();
            ModStore failing = new ModStore(original.filesDirectory, index -> { throw new IOException("Injected write failure"); });
            rejects(() -> install(failing, bundle("coop", "1.1.0", "new_bridge", 2)));
            assertOriginal(new ModStore(original.filesDirectory));
        });
        test("process interruption at every update step recovers previous version", () -> {
            for (int stop = 1; stop <= 3; stop++) {
                ModStore original = original(); final int interruptAt = stop;
                ModStore failing = new ModStore(original.filesDirectory, index -> { if (index == interruptAt) throw new InterruptedProcess(); });
                try { install(failing, bundle("coop", "1.1.0", "new_bridge", 2)); throw new AssertionError("Interruption not reached"); }
                catch (InterruptedProcess expected) { }
                assertOriginal(new ModStore(original.filesDirectory));
                assertOriginal(new ModStore(original.filesDirectory)); // Reopening must not roll back twice.
            }
        });
        test("interrupted fresh install leaves no partial mod", () -> {
            ModStore empty = store();
            ModStore failing = new ModStore(empty.filesDirectory, index -> { throw new InterruptedProcess(); });
            try { install(failing, bundle("coop", "1.0.0", "dk64_bridge", 1)); } catch (InterruptedProcess expected) { }
            ModStore recovered = new ModStore(empty.filesDirectory);
            check(recovered.list().isEmpty() && !Files.exists(recovered.directory.resolve("dk64_bridge.so")), "Partial install remained");
        });
        test("interrupted removal restores mod and bridge", () -> {
            ModStore original = original();
            ModStore failing = new ModStore(original.filesDirectory, index -> { throw new InterruptedProcess(); });
            try { failing.remove(failing.list().get(0)); } catch (InterruptedProcess expected) { }
            assertOriginal(new ModStore(original.filesDirectory));
        });
        test("corrupt manifest removal never guesses native ownership", () -> {
            ModStore store = original(); Files.writeString(store.directory.resolve("Test.nrm"), "bad");
            store.remove(store.list().get(0));
            check(Files.exists(store.directory.resolve("dk64_bridge.so")), "Unknown native owner was deleted");
        });
        System.out.println(passed + " native bundle checks passed. ELF fixtures validate packaging only; no native code was executed.");
    }
}
