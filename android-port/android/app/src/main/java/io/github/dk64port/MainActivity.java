package io.github.dk64port;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Build;
import android.app.ActivityManager;
import android.app.ApplicationExitInfo;
import android.view.View;
import android.widget.*;
import java.io.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.nio.file.Files;
import java.nio.charset.StandardCharsets;
import org.json.JSONObject;

public final class MainActivity extends Activity {
    private static final int PICK_ROM = 1;
    private static final int PICK_DRIVER = 2;
    private final ExecutorService io = Executors.newSingleThreadExecutor();
    private TextView status;
    private Button play;
    private Button pick;
    private File dataDir;
    private File programDir;
    private TextView driverStatus;
    private TextView exitStatus;
    private Button importDriver;
    private Button systemDriver;
    private Button graphicsDiagnostics;
    private Switch turnipCompatibility;
    private boolean updatingGraphicsControls;
    private boolean assetsReady;

    @Override public void onCreate(Bundle state) {
        super.onCreate(state);
        dataDir = new File(getFilesDir(), "data");
        programDir = new File(getFilesDir(), "program");
        LinearLayout column = new LinearLayout(this);
        column.setOrientation(LinearLayout.VERTICAL);
        int padding = (int)(24 * getResources().getDisplayMetrics().density);
        column.setPadding(padding, padding, padding, padding);
        column.setOnApplyWindowInsetsListener((view, insets) -> {
            view.setPadding(padding, padding + insets.getSystemWindowInsetTop(), padding,
                padding + insets.getSystemWindowInsetBottom());
            return insets;
        });
        TextView title = new TextView(this);
        title.setText("Donkey Kong 64\nAndroid development port");
        title.setTextSize(28);
        column.addView(title);
        TextView note = new TextView(this);
        note.setText("Based on DK64 Recompiled 1.0.1. Experimental: graphics, suspend/resume and saves need validation.\n\nImport your own original US ROM. No ROM is included. A Bluetooth/USB controller or the touch controls can be used.");
        note.setTextSize(16);
        column.addView(note);
        pick = new Button(this);
        pick.setText("Import US DK64 ROM");
        pick.setOnClickListener(view -> {
            Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
            intent.setType("*/*");
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            startActivityForResult(intent, PICK_ROM);
        });
        column.addView(pick);
        TextView driverWarning = new TextView(this);
        driverWarning.setText("Graphics driver warning\nThe system Vulkan driver may not work and can cause a black screen or crash. A compatible Turnip driver may be required on supported Qualcomm Adreno GPUs. Use Import graphics driver ZIP below. Turnip is not compatible with every GPU; custom-driver import requires Android 9 or newer.");
        driverWarning.setTextSize(16);
        driverWarning.setTextColor(android.graphics.Color.rgb(92, 52, 0));
        driverWarning.setBackgroundColor(android.graphics.Color.rgb(255, 243, 205));
        int warningPadding = (int)(12 * getResources().getDisplayMetrics().density);
        driverWarning.setPadding(warningPadding, warningPadding, warningPadding, warningPadding);
        column.addView(driverWarning);
        play = new Button(this);
        play.setText("Launch game");
        play.setEnabled(false);
        play.setOnClickListener(view -> {
            getPreferences(MODE_PRIVATE).edit().putLong("lastLaunch", System.currentTimeMillis()).apply();
            startActivity(new Intent(this, GameActivity.class));
        });
        column.addView(play);
        status = new TextView(this);
        status.setText("Preparing program assets…");
        column.addView(status);
        Button mods = new Button(this);
        mods.setText("Manage Mods");
        mods.setOnClickListener(view -> startActivity(new Intent(this, ModActivity.class)));
        column.addView(mods);
        driverStatus = new TextView(this);
        driverStatus.setTextSize(16);
        column.addView(driverStatus);
        TextView driverNote = new TextView(this);
        driverNote.setText("Custom drivers (such as Turnip) affect this app only. Import ARM64 ZIPs from trusted sources: drivers run native code inside the app. Changes apply on the next game launch.");
        column.addView(driverNote);
        importDriver = new Button(this);
        importDriver.setText("Import graphics driver ZIP");
        importDriver.setEnabled(Build.VERSION.SDK_INT >= 28);
        importDriver.setOnClickListener(view -> {
            Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
            intent.setType("*/*");
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            startActivityForResult(intent, PICK_DRIVER);
        });
        column.addView(importDriver);
        systemDriver = new Button(this);
        systemDriver.setText("Use system driver");
        systemDriver.setOnClickListener(view -> { DriverStore.useSystem(this); updateDriverStatus(); });
        column.addView(systemDriver);
        turnipCompatibility = new Switch(this);
        turnipCompatibility.setText("Turnip compatibility mode (sysmem)");
        turnipCompatibility.setTextSize(18);
        turnipCompatibility.setPadding(warningPadding, warningPadding, warningPadding, warningPadding);
        turnipCompatibility.setOnCheckedChangeListener((button, enabled) -> {
            if (updatingGraphicsControls) return;
            try {
                // Reuse the dev6 setting so an existing sysmem workaround stays enabled.
                GraphicsDiagnostics.select(this, enabled ? GraphicsDiagnostics.COMPATIBILITY_MODE
                    : GraphicsDiagnostics.DEFAULT_MODE);
                Toast.makeText(this, "Saved for the next game launch. The running game is unchanged.", Toast.LENGTH_LONG).show();
            } catch (IOException error) { showError(error); }
            // Restore the persisted state even if saving failed.
            updateDriverStatus();
        });
        column.addView(turnipCompatibility);
        TextView compatibilityInfo = new TextView(this);
        compatibilityInfo.setText("Turnip rendering\nGMEM is the default for imported Turnip drivers on all GPUs. Reported to fix water on Poco F8 Ultra / Adreno 840 with Turnip Gen8 V35. Broader device compatibility and performance remain unverified.\n\nEnable this switch to use system-memory rendering (sysmem) as a fallback. Sysmem was also reported to fix water on the Poco with V30 and V35. Turning the switch off selects GMEM again. Existing explicit diagnostic choices are retained after updating. Your choice is saved and applies on the next game launch. Only affects this app with an imported driver; it does not change the system driver.");
        compatibilityInfo.setTextSize(16);
        compatibilityInfo.setTextColor(android.graphics.Color.rgb(22, 58, 89));
        compatibilityInfo.setBackgroundColor(android.graphics.Color.rgb(230, 242, 255));
        compatibilityInfo.setPadding(warningPadding, warningPadding, warningPadding, warningPadding);
        column.addView(compatibilityInfo);
        graphicsDiagnostics = new Button(this);
        graphicsDiagnostics.setOnClickListener(view -> showGraphicsDiagnostics());
        column.addView(graphicsDiagnostics);
        TextView diagnosticsNote = new TextView(this);
        diagnosticsNote.setText("Advanced troubleshooting only. GMEM rendering is the app default. Driver defaults (automatic) explicitly opts out and sends no Turnip debug flags. Other test modes replace the default or sysmem fallback; they are not combined. GMEM + strict synchronization adds cache flushes and GPU waits (gmem,flushall,syncdraw); it is not a high-accuracy setting and may be very slow. Applies on the next game launch. Ignored when using the system driver.");
        column.addView(diagnosticsNote);
        exitStatus = new TextView(this);
        column.addView(exitStatus);
        ScrollView scroll = new ScrollView(this);
        scroll.addView(column);
        setContentView(scroll);
        pick.setEnabled(false);
        io.execute(() -> {
            try {
                copyAssets("program", programDir);
                runOnUiThread(() -> { assetsReady = true; updateReady("Program assets ready."); });
            } catch (Exception error) { showError(error); }
        });
    }

    private void copyAssets(String path, File target) throws IOException {
        String[] children = getAssets().list(path);
        if (children == null) throw new IOException("Missing program assets.");
        if (children.length > 0) {
            if (!target.isDirectory() && !target.mkdirs()) throw new IOException("Cannot create program directory.");
            for (String child : children) copyAssets(path + "/" + child, new File(target, child));
        } else {
            try (InputStream input = getAssets().open(path); FileOutputStream output = new FileOutputStream(target)) {
                byte[] buffer = new byte[32768];
                int count;
                while ((count = input.read(buffer)) != -1) output.write(buffer, 0, count);
            }
        }
    }

    private void updateReady(String message) {
        if (isDestroyed()) return;
        boolean ready = new File(dataDir, "imported.z64").isFile();
        pick.setEnabled(true);
        play.setEnabled(ready && assetsReady);
        status.setText(message + (ready ? (assetsReady ? "\nROM imported. Ready to launch." : "\nROM imported; program assets are not ready.") : "\nSelect your ROM to continue."));
    }

    private void showError(Exception error) {
        runOnUiThread(() -> { if (!isDestroyed()) updateReady("Error: " + error.getMessage()); });
    }

    @Override protected void onActivityResult(int request, int result, Intent intent) {
        super.onActivityResult(request, result, intent);
        if (request == PICK_DRIVER && result == RESULT_OK && intent != null && intent.getData() != null) {
            importDriver.setEnabled(false);
            systemDriver.setEnabled(false);
            play.setEnabled(false);
            driverStatus.setText("Checking and importing graphics driver…");
            io.execute(() -> {
                String message;
                try (InputStream input = getContentResolver().openInputStream(intent.getData())) {
                    if (input == null) throw new IOException("Cannot open the selected ZIP.");
                    message = "Imported " + DriverStore.importDriver(this, input);
                } catch (Exception error) { message = "Driver import failed: " + error.getMessage(); }
                final String resultMessage = message;
                runOnUiThread(() -> {
                    if (isDestroyed()) return;
                    importDriver.setEnabled(Build.VERSION.SDK_INT >= 28);
                    systemDriver.setEnabled(true);
                    updateReady(resultMessage);
                    updateDriverStatus();
                });
            });
            return;
        }
        if (request != PICK_ROM || result != RESULT_OK || intent == null || intent.getData() == null) return;
        pick.setEnabled(false);
        play.setEnabled(false);
        status.setText("Checking ROM and copying to private app storage…");
        io.execute(() -> {
            try (InputStream input = getContentResolver().openInputStream(intent.getData())) {
                if (input == null) throw new IOException("The selected file could not be opened.");
                RomImporter.importRom(input, dataDir);
                runOnUiThread(() -> updateReady("Verified original US DK64 ROM."));
            } catch (Exception error) { showError(error); }
        });
    }

    private void showGraphicsDiagnostics() {
        final int[] selection = { GraphicsDiagnostics.selected(this) };
        new android.app.AlertDialog.Builder(this)
            .setTitle("Advanced Turnip diagnostics — next launch")
            .setSingleChoiceItems(GraphicsDiagnostics.LABELS, selection[0],
                (dialog, which) -> selection[0] = which)
            .setNegativeButton("Cancel", null)
            .setPositiveButton("Save", (dialog, which) -> {
                try {
                    GraphicsDiagnostics.select(this, selection[0]);
                    updateDriverStatus();
                    Toast.makeText(this, "Saved for the next game launch. The running game is unchanged.", Toast.LENGTH_LONG).show();
                } catch (IOException error) { showError(error); }
            }).show();
    }

    private void updateDriverStatus() {
        int graphicsMode = GraphicsDiagnostics.selected(this);
        updatingGraphicsControls = true;
        turnipCompatibility.setChecked(graphicsMode == GraphicsDiagnostics.COMPATIBILITY_MODE);
        updatingGraphicsControls = false;
        graphicsDiagnostics.setText("Advanced Turnip diagnostics: " + GraphicsDiagnostics.LABELS[graphicsMode]);
        String text;
        try { text = "Selected graphics driver: " + DriverStore.selected(this).label; }
        catch (Exception error) { text = "Saved driver is invalid. Use system driver or import another ZIP."; }
        File report = new File(dataDir, "graphics-driver.json");
        try {
            if (report.isFile() && report.length() < 16384) {
                JSONObject last = new JSONObject(new String(Files.readAllBytes(report.toPath()), StandardCharsets.UTF_8));
                text += "\nLast launch: " + last.optString("label") + " — " + last.optString("device", "device not initialized")
                    + "\n" + last.optString("message");
            }
        } catch (Exception ignored) { /* Report may be being written by the game process. */ }
        driverStatus.setText(text);
    }

    @Override protected void onResume() {
        super.onResume();
        if (driverStatus == null) return;
        updateDriverStatus();
        if (Build.VERSION.SDK_INT >= 30) {
            long launched = getPreferences(MODE_PRIVATE).getLong("lastLaunch", Long.MAX_VALUE);
            ActivityManager manager = (ActivityManager)getSystemService(ACTIVITY_SERVICE);
            for (ApplicationExitInfo exit : manager.getHistoricalProcessExitReasons(getPackageName(), 0, 8)) {
                if (exit.getProcessName().endsWith(":game") && exit.getTimestamp() >= launched
                        && (exit.getReason() == ApplicationExitInfo.REASON_CRASH_NATIVE || exit.getReason() == ApplicationExitInfo.REASON_CRASH)) {
                    exitStatus.setText("The last game launch crashed. If using a custom driver, try Use system driver or another compatible Turnip ZIP. Diagnostics are saved in the app's native.log.");
                    break;
                }
            }
        }
    }

    @Override protected void onDestroy() {
        io.shutdown();
        super.onDestroy();
    }
}
