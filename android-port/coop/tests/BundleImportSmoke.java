package io.github.dk64port;

import java.nio.file.*;
import java.util.*;
import java.util.zip.*;

/** Exercises the Android importer's real package path in a temporary host directory. */
public final class BundleImportSmoke {
    public static void main(String[] args) throws Exception {
        Path bundle = Path.of(args[0]);
        Path scratch = Files.createTempDirectory(Path.of(args[1]), "real-bundle-");
        ModStore store = new ModStore(scratch.toFile());
        try (ModStore.Pending pending = store.prepare(Files.newInputStream(bundle), bundle.getFileName().toString())) {
            store.install(pending);
        }
        try (ZipFile zip = new ZipFile(bundle.toFile())) {
            for (String name : List.of("dk64_lan_coop.nrm", "dk64_coop_bridge.so")) {
                if (!Arrays.equals(zip.getInputStream(zip.getEntry(name)).readAllBytes(), Files.readAllBytes(store.directory.resolve(name))))
                    throw new AssertionError("Installed bytes changed: " + name);
            }
        }
        if (store.list().size() != 1 || store.list().get(0).error != null) throw new AssertionError("Mod listing invalid");
        System.out.println("PASS: actual Android co-op ZIP imports NRM and ARM64 companion unchanged");
    }
}
