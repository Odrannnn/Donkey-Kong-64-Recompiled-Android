package io.github.dk64port;

import android.content.Context;
import android.util.AtomicFile;
import android.util.Log;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;

/** Opt-in Turnip experiments; never infer a workaround from the GPU name. */
final class GraphicsDiagnostics {
    static final int DEFAULT_MODE = 0;
    static final int COMPATIBILITY_MODE = 1;
    static final String[] LABELS = {
        "Off — driver defaults",
        "Compatibility mode (system-memory rendering)",
        "Disable depth optimization (nolrz)",
        "Disable framebuffer compression (noubwc)",
        "Strict GPU synchronization — very slow"
    };
    private static final String[] FLAGS = {"", "sysmem", "nolrz", "noubwc", "flushall,syncdraw"};

    private static AtomicFile file(Context context) {
        return new AtomicFile(new File(context.getFilesDir(), "gpu-drivers/diagnostics.txt"));
    }

    static int selected(Context context) {
        AtomicFile saved = file(context);
        if (!saved.getBaseFile().exists()) return 0;
        try {
            if (saved.getBaseFile().length() > 128) throw new IOException("Oversized diagnostics selection");
            String value = new String(saved.readFully(), StandardCharsets.UTF_8);
            for (int i = 0; i < FLAGS.length; i++) if (FLAGS[i].equals(value)) return i;
            throw new IOException("Unknown diagnostics selection");
        } catch (IOException error) {
            Log.w("DK64Graphics", "Ignoring invalid diagnostics selection", error);
            return 0;
        }
    }

    static void select(Context context, int index) throws IOException {
        if (index < 0 || index >= FLAGS.length) throw new IllegalArgumentException("Invalid diagnostics mode");
        AtomicFile saved = file(context);
        File parent = saved.getBaseFile().getParentFile();
        if (!parent.isDirectory() && !parent.mkdirs()) throw new IOException("Cannot save diagnostics selection");
        FileOutputStream output = null;
        try {
            output = saved.startWrite();
            output.write(FLAGS[index].getBytes(StandardCharsets.UTF_8));
            saved.finishWrite(output);
        } catch (IOException error) {
            if (output != null) saved.failWrite(output);
            throw error;
        }
    }

    static String flags(Context context, boolean customDriver) {
        return customDriver ? FLAGS[selected(context)] : "";
    }
}
