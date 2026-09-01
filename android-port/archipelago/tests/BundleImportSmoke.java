package io.github.dk64port;

import java.nio.file.*;
import java.util.*;
import java.util.zip.*;

/** Test the actual Android importer without installing into any live app. */
public final class BundleImportSmoke {
    public static void main(String[] args) throws Exception {
        Path bundle = Path.of(args[0]);
        Path scratch = Files.createTempDirectory(Path.of(args[1]), "ap-bundle-");
        ModStore store = new ModStore(scratch.toFile());
        try (ModStore.Pending pending = store.prepare(Files.newInputStream(bundle), bundle.getFileName().toString())) {
            store.install(pending);
        }
        try (ZipFile zip = new ZipFile(bundle.toFile())) {
            for (String name : List.of("dk64_archipelago.nrm", "dk64_ap_bridge.so")) {
                if (!Arrays.equals(zip.getInputStream(zip.getEntry(name)).readAllBytes(), Files.readAllBytes(store.directory.resolve(name))))
                    throw new AssertionError("Installed bytes changed: " + name);
            }
        }
        if (store.list().size() != 1 || store.list().get(0).error != null) throw new AssertionError("Mod listing invalid");
        System.out.println("PASS: actual Archipelago Android ZIP imports NRM and ARM64 companion unchanged");
    }
}
