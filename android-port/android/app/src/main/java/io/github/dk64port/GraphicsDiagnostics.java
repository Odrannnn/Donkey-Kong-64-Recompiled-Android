package io.github.dk64port;

import android.content.Context;
import android.util.AtomicFile;
import android.util.Log;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;

/** App-wide Turnip default and explicit overrides; never modify the system driver. */
final class GraphicsDiagnostics {
    static final int DEFAULT_MODE = 0;
    static final int COMPATIBILITY_MODE = 1;
    static final int DRIVER_DEFAULTS_MODE = 5;
    private static final String DRIVER_DEFAULTS_VALUE = "driver-defaults";
    static final String[] LABELS = {
        "GMEM rendering — default",
        "Compatibility mode (system-memory rendering)",
        "Disable depth optimization (nolrz)",
        "Disable framebuffer compression (noubwc)",
        "Strict GPU synchronization — very slow",
        "Driver defaults (automatic)",
        "GMEM + strict synchronization — very slow"
    };
    // Selections are stored as strings, not indices. Keep all existing explicit flags valid.
    private static final String[] FLAGS = {"gmem", "sysmem", "nolrz", "noubwc", "flushall,syncdraw",
        "", "gmem,flushall,syncdraw"};

    static int modeForSavedValue(String value) throws IOException {
        // Missing preferences and the old empty Off/default selection adopt the app default.
        if (value == null || value.isEmpty()) return DEFAULT_MODE;
        if (DRIVER_DEFAULTS_VALUE.equals(value)) return DRIVER_DEFAULTS_MODE;
        for (int i = 0; i < FLAGS.length; i++) if (FLAGS[i].equals(value)) return i;
        throw new IOException("Unknown diagnostics selection");
    }

    static String savedValue(int index) {
        if (index < 0 || index >= FLAGS.length) throw new IllegalArgumentException("Invalid diagnostics mode");
        // Distinguish a new explicit opt-out from the old empty default during future launches.
        return index == DRIVER_DEFAULTS_MODE ? DRIVER_DEFAULTS_VALUE : FLAGS[index];
    }

    static String flagsForMode(int index, boolean customDriver) {
        return customDriver ? FLAGS[index] : "";
    }

    private static AtomicFile file(Context context) {
        return new AtomicFile(new File(context.getFilesDir(), "gpu-drivers/diagnostics.txt"));
    }

    static int selected(Context context) {
        AtomicFile saved = file(context);
        if (!saved.getBaseFile().exists()) return DEFAULT_MODE;
        try {
            if (saved.getBaseFile().length() > 128) throw new IOException("Oversized diagnostics selection");
            String value = new String(saved.readFully(), StandardCharsets.UTF_8);
            return modeForSavedValue(value);
        } catch (IOException error) {
            Log.w("DK64Graphics", "Ignoring invalid diagnostics selection", error);
            return DEFAULT_MODE;
        }
    }

    static void select(Context context, int index) throws IOException {
        String value = savedValue(index);
        AtomicFile saved = file(context);
        File parent = saved.getBaseFile().getParentFile();
        if (!parent.isDirectory() && !parent.mkdirs()) throw new IOException("Cannot save diagnostics selection");
        FileOutputStream output = null;
        try {
            output = saved.startWrite();
            output.write(value.getBytes(StandardCharsets.UTF_8));
            saved.finishWrite(output);
        } catch (IOException error) {
            if (output != null) saved.failWrite(output);
            throw error;
        }
    }

    static String flags(Context context, boolean customDriver) {
        return customDriver ? flagsForMode(selected(context), true) : "";
    }
}
