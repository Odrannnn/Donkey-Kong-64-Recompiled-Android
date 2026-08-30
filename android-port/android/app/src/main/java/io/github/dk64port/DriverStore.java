package io.github.dk64port;

import android.content.Context;
import android.os.Build;
import android.util.AtomicFile;
import org.json.JSONObject;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;

final class DriverStore {
    static final class Selection {
        final String directory, library, label;
        Selection(String directory, String library, String label) {
            this.directory = directory; this.library = library; this.label = label;
        }
    }
    private static Path root(Context context) { return new File(context.getFilesDir(), "gpu-drivers").toPath(); }
    private static AtomicFile selectionFile(Context context) {
        return new AtomicFile(root(context).resolve("active.json").toFile());
    }
    static Selection selected(Context context) throws Exception {
        AtomicFile file = selectionFile(context);
        if (!file.getBaseFile().exists()) return new Selection("", "", "System driver");
        JSONObject json = new JSONObject(new String(file.readFully(), StandardCharsets.UTF_8));
        Path imports = root(context).resolve("imports").toRealPath();
        Path library = imports.resolve(json.getString("path")).toRealPath();
        if (!library.startsWith(imports) || !Files.isRegularFile(library)) throw new IOException("Invalid saved driver path.");
        DriverArchive.validateArm64(library);
        return new Selection(library.getParent().toString(), library.getFileName().toString(), json.getString("label"));
    }
    static void useSystem(Context context) { selectionFile(context).delete(); }

    static String importDriver(Context context, InputStream source) throws Exception {
        if (Build.VERSION.SDK_INT < 28) throw new IOException("Custom drivers require Android 9 or newer.");
        Path imports = root(context).resolve("imports");
        Files.createDirectories(imports);
        Path staging = Files.createTempDirectory(imports, ".import-");
        Path installed = null;
        boolean committed = false;
        try {
            List<Path> files = DriverArchive.extract(source, staging);
            Path metadata = null;
            for (Path path : files) {
                if (path.getFileName().toString().equals("meta.json")) {
                    if (metadata != null) throw new IOException("Driver ZIP contains multiple metadata files.");
                    metadata = path;
                }
            }
            Path library = null;
            String label = "Custom Vulkan driver";
            if (metadata != null) {
                if (Files.size(metadata) > 65536) throw new IOException("Driver metadata is too large.");
                JSONObject meta = new JSONObject(new String(Files.readAllBytes(metadata), StandardCharsets.UTF_8));
                if (meta.optInt("minApi", 28) > Build.VERSION.SDK_INT) throw new IOException("This driver needs a newer Android version.");
                String soname = meta.getString("libraryName");
                if (!soname.matches("[A-Za-z0-9_-][A-Za-z0-9_.-]*\\.so")) throw new IOException("Invalid driver library name.");
                library = metadata.getParent().resolve(soname);
                label = meta.optString("name", label);
                if (label.length() > 120) label = label.substring(0, 120);
            } else {
                Set<String> names = new HashSet<>(Arrays.asList("libvulkan_freedreno.so", "vulkan.freedreno.so", "libvulkan.so"));
                for (Path path : files) if (names.contains(path.getFileName().toString())) {
                    if (library != null) throw new IOException("Ambiguous ZIP: more than one Vulkan driver.");
                    library = path;
                }
            }
            if (library == null || !Files.isRegularFile(library)) throw new IOException("No Vulkan driver library found in the ZIP.");
            DriverArchive.validateArm64(library);
            Path relative = staging.relativize(library);
            // Imported files are immutable after validation; selection is the only mutable state.
            for (Path path : files) if (!path.toFile().setReadOnly()) throw new IOException("Cannot protect imported driver files.");
            installed = imports.resolve(UUID.randomUUID().toString());
            Files.move(staging, installed, StandardCopyOption.ATOMIC_MOVE);
            JSONObject selected = new JSONObject();
            selected.put("path", imports.relativize(installed.resolve(relative)).toString());
            selected.put("label", label);
            AtomicFile config = selectionFile(context);
            FileOutputStream out = config.startWrite();
            try {
                out.write(selected.toString().getBytes(StandardCharsets.UTF_8));
                config.finishWrite(out);
            } catch (Exception error) { config.failWrite(out); throw error; }
            committed = true;
            return label;
        } finally {
            if (Files.exists(staging)) DriverArchive.removePrivateTree(staging);
            if (!committed && installed != null && Files.exists(installed)) DriverArchive.removePrivateTree(installed);
        }
    }
}
