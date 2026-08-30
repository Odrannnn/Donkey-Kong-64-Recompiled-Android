package io.github.dk64port;

import android.app.*;
import android.content.*;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.OpenableColumns;
import android.widget.*;
import java.io.*;
import java.util.*;
import java.util.concurrent.*;

/** DocumentsUI import and private-folder management; no broad storage permission needed. */
public final class ModActivity extends Activity {
    private static final int PICK_MOD = 10;
    private final ExecutorService io = Executors.newSingleThreadExecutor();
    private LinearLayout rows;
    private TextView status;
    private Button importButton, refreshButton, gameButton;
    private boolean busy;
    private boolean requestImport;
    private volatile ModStore.Pending pending;
    private interface Operation { String run(ModStore store) throws Exception; }

    @Override public void onCreate(Bundle state) {
        super.onCreate(state);
        LinearLayout column = new LinearLayout(this);
        column.setOrientation(LinearLayout.VERTICAL);
        int padding = (int)(20 * getResources().getDisplayMetrics().density);
        column.setPadding(padding, padding, padding, padding);
        column.setOnApplyWindowInsetsListener((view, insets) -> {
            view.setPadding(padding + insets.getSystemWindowInsetLeft(), padding + insets.getSystemWindowInsetTop(),
                padding + insets.getSystemWindowInsetRight(), padding + insets.getSystemWindowInsetBottom());
            return insets;
        });
        TextView title = new TextView(this);
        title.setText("Manage Mods"); title.setTextSize(28); column.addView(title);
        TextView note = new TextView(this);
        note.setText("Import trusted DK64 recomp .nrm mods, .rtz texture packs, or an Android ZIP containing one mod and its declared ARM64 .so bridge. Mods run code with this app's access, including networking. Windows DLLs, embedded native libraries and .apworld files are not supported.\n\nChanges apply on the next game launch. In-game Mods still provides ordering, options and dependency details; required dependencies can be enabled automatically by the game.");
        column.addView(note);
        importButton = button(column, "Import mod file", this::pickMod);
        refreshButton = button(column, "Refresh installed mods", () -> perform("Reading mods…", store -> "Installed mods refreshed."));
        gameButton = button(column, "Close game to manage mods", () -> {
            startActivity(new Intent(this, GameActivity.class).putExtra("manageMods", true));
        });
        gameButton.setVisibility(android.view.View.GONE);
        button(column, "Back to launcher", () -> {
            if (busy) return;
            startActivity(new Intent(this, MainActivity.class).addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP));
            finish();
        });
        status = new TextView(this); status.setTextSize(16); column.addView(status);
        rows = new LinearLayout(this); rows.setOrientation(LinearLayout.VERTICAL); column.addView(rows);
        ScrollView scroll = new ScrollView(this); scroll.addView(column); setContentView(scroll);
        requestImport = state == null && getIntent().getBooleanExtra("importMod", false);
    }
    @Override protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        setIntent(intent);
        requestImport = intent.getBooleanExtra("importMod", false);
    }
    private Button button(LinearLayout parent, String label, Runnable action) {
        Button button = new Button(this); button.setText(label);
        button.setOnClickListener(view -> action.run()); parent.addView(button); return button;
    }
    private void pickMod() {
        if (busy || pending != null) return;
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT).setType("*/*").addCategory(Intent.CATEGORY_OPENABLE);
        try { startActivityForResult(intent, PICK_MOD); }
        catch (ActivityNotFoundException error) { status.setText("No Android document picker is available."); }
    }
    @Override protected void onResume() {
        super.onResume();
        if (!busy && pending == null) perform("Reading mods…", store -> "Select a mod below, or import a new one.");
    }
    private void setBusy(boolean value, String message) {
        busy = value; status.setText(message);
        importButton.setEnabled(!value); refreshButton.setEnabled(!value); gameButton.setEnabled(!value);
        for (int i = 0; i < rows.getChildCount(); i++) rows.getChildAt(i).setEnabled(!value);
    }
    private void perform(String message, Operation operation) {
        if (busy || pending != null) return;
        setBusy(true, message);
        io.execute(() -> {
            String result;
            List<ModStore.Mod> mods = Collections.emptyList();
            try (ModSession ignored = ModSession.acquire(getFilesDir())) {
                ModStore store = new ModStore(getFilesDir());
                result = operation.run(store);
                mods = store.list();
            } catch (Exception error) { result = "Error: " + error.getMessage(); }
            final String text = result;
            final List<ModStore.Mod> snapshot = mods;
            runOnUiThread(() -> {
                if (isDestroyed()) return;
                setBusy(false, text); render(snapshot);
                gameButton.setVisibility(text.contains("is active") ? android.view.View.VISIBLE : android.view.View.GONE);
                if (requestImport && !text.startsWith("Error:")) { requestImport = false; pickMod(); }
            });
        });
    }
    private void render(List<ModStore.Mod> mods) {
        rows.removeAllViews();
        if (mods.isEmpty()) {
            TextView empty = new TextView(this); empty.setText("No installed mods to display."); rows.addView(empty);
        }
        for (ModStore.Mod mod : mods) {
            Button row = button(rows, mod.name + "  " + mod.version + "\n"
                + (mod.error == null ? (mod.enabled ? "Enabled" : "Disabled") : "Invalid: " + mod.error), () -> showMod(mod));
            row.setAllCaps(false);
        }
    }
    private void showMod(ModStore.Mod mod) {
        if (busy || pending != null) return;
        AlertDialog.Builder dialog = new AlertDialog.Builder(this).setTitle(mod.name)
            .setMessage(mod.file.getFileName() + "\n" + (mod.error == null ? "ID: " + mod.id + "\nChanges apply next launch." : mod.error))
            .setNegativeButton("Cancel", null)
            .setNeutralButton("Remove", (d, which) -> new AlertDialog.Builder(this)
                .setTitle("Remove " + mod.name + "?")
                .setMessage("The installed mod and its declared companion bridge will be removed. Saves, mod settings and the original downloaded file will be kept. If the manifest is corrupt, only the mod archive can be removed safely.")
                .setNegativeButton("Cancel", null)
                .setPositiveButton("Remove", (confirm, button) -> perform("Removing mod…", store -> {
                    store.remove(mod); return "Removed " + mod.name + ". Saves and settings kept.";
                })).show());
        if (mod.error == null) dialog.setPositiveButton(mod.enabled ? "Disable" : "Enable", (d, which) -> perform("Saving mod selection…", store -> {
            store.setEnabled(mod, !mod.enabled); return mod.name + (mod.enabled ? " disabled." : " enabled.") + " Applies next launch.";
        }));
        dialog.show();
    }
    @Override protected void onActivityResult(int request, int result, Intent intent) {
        super.onActivityResult(request, result, intent);
        if (request != PICK_MOD || result != RESULT_OK || intent == null || intent.getData() == null) return;
        Uri uri = intent.getData();
        setBusy(true, "Checking mod package…");
        io.execute(() -> {
            String failure = null;
            boolean update = false;
            try (ModSession ignored = ModSession.acquire(getFilesDir())) {
                String name = null;
                try (Cursor cursor = getContentResolver().query(uri, new String[] { OpenableColumns.DISPLAY_NAME }, null, null, null)) {
                    if (cursor != null && cursor.moveToFirst()) name = cursor.getString(0);
                }
                if (name == null) throw new IOException("The document provider did not supply a filename.");
                ModStore store = new ModStore(getFilesDir());
                try (InputStream input = getContentResolver().openInputStream(uri)) {
                    if (input == null) throw new IOException("Cannot read the selected mod.");
                    pending = store.prepare(input, name);
                    update = store.isUpdate(pending);
                }
            } catch (Exception error) {
                failure = error.getMessage();
                discardPending();
            }
            final String error = failure;
            final boolean replaces = update;
            runOnUiThread(() -> {
                if (isDestroyed()) return;
                setBusy(false, error == null ? "Package checked. Review before installing." : "Import failed: " + error);
                if (error == null && pending != null) {
                    new AlertDialog.Builder(this).setTitle(replaces ? "Update installed mod?" : "Install mod?")
                        .setMessage(pending.mod.name + "\nVersion: " + pending.mod.version + "\n\n"
                            + (replaces ? "The installed version will be replaced. " : "")
                            + (pending.mod.libraries.isEmpty() ? "" : "Includes native code: " + String.join(", ", pending.mod.libraries) + ". It runs with the app's permissions and is not sandboxed separately. ")
                            + "Install only if you trust the source. Game compatibility and dependencies are checked by the runtime at launch.")
                        .setPositiveButton(replaces ? "Update" : "Install", (d, which) -> finishImport(true))
                        .setNegativeButton("Cancel", (d, which) -> finishImport(false))
                        .setOnCancelListener(d -> finishImport(false)).show();
                }
            });
        });
    }
    private void finishImport(boolean install) {
        ModStore.Pending selected = pending;
        if (selected == null || busy) return;
        // Keep ownership until the worker has finished, even if acquiring the session lock fails.
        setBusy(true, install ? "Installing mod…" : "Cancelling import…");
        io.execute(() -> {
            String result;
            try {
                if (install) try (ModSession ignored = ModSession.acquire(getFilesDir())) {
                    new ModStore(getFilesDir()).install(selected);
                }
                result = install ? "Installed " + selected.mod.name + ". It will appear in Mods at the next launch." : "Import cancelled. No installed files changed.";
            } catch (Exception error) { result = "Import failed: " + error.getMessage(); }
            finally { discardPending(); }
            final String text = result;
            runOnUiThread(() -> {
                if (isDestroyed()) return;
                busy = false;
                perform(text, store -> text);
            });
        });
    }
    private void discardPending() {
        ModStore.Pending selected = pending;
        pending = null;
        if (selected != null) try { selected.close(); } catch (IOException ignored) { }
    }
    @Override public void onBackPressed() {
        if (busy) { Toast.makeText(this, "Wait for the mod operation to finish.", Toast.LENGTH_SHORT).show(); return; }
        super.onBackPressed();
    }
    @Override protected void onDestroy() {
        io.execute(this::discardPending);
        io.shutdown();
        super.onDestroy();
    }
}
