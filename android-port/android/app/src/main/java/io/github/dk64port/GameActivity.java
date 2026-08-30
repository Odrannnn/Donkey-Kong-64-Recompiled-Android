package io.github.dk64port;

import android.os.Bundle;
import android.content.Context;
import android.content.Intent;
import android.app.AlertDialog;
import android.view.SurfaceHolder;
import android.view.ViewGroup;
import android.view.WindowManager;
import java.io.File;
import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

public final class GameActivity extends SDLActivity {
    private TouchControls touchControls;
    private ModSession modSession;
    private boolean openingMods, importMod, modDialogOpen;
    static native void nativeTouchState(int buttons, float x, float y);
    private static native void nativePauseScheduler(boolean paused);
    private static native void nativeSurfaceReady(boolean ready);
    @Override protected String[] getLibraries() { return new String[] { "c++_shared", "SDL2", "main" }; }
    @Override public void loadLibraries() {
        try {
            modSession = ModSession.acquire(getFilesDir());
            new ModStore(getFilesDir()); // Recover interrupted bundle installation before starting native code.
        } catch (java.io.IOException error) {
            throw new UnsatisfiedLinkError("Cannot prepare mods: " + error.getMessage());
        }
        super.loadLibraries();
    }
    @Override protected String[] getArguments() {
        DriverStore.Selection driver;
        try { driver = DriverStore.selected(this); }
        catch (Exception error) { driver = new DriverStore.Selection("", "", "System driver (invalid selection)"); }
        // SDL calls getArguments after loading its native library and before entering
        // the game's main(), which opens Turnip. Its environment is read at startup.
        String turnipFlags = GraphicsDiagnostics.flags(this, !driver.directory.isEmpty());
        SDLActivity.nativeSetenv("TU_DEBUG", turnipFlags);
        android.util.Log.i("DK64Graphics", "Launch diagnostics: TU_DEBUG=" + turnipFlags
            + "; customDriver=" + !driver.directory.isEmpty());
        return new String[] { new File(getFilesDir(), "program").getAbsolutePath(),
            new File(getFilesDir(), "data").getAbsolutePath(), getApplicationInfo().nativeLibraryDir,
            getCodeCacheDir().getAbsolutePath(), driver.directory, driver.library, driver.label };
    }
    @Override protected void onCreate(Bundle state) {
        super.onCreate(state);
        if (mBrokenLibraries) return;
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        touchControls = new TouchControls(this);
        addContentView(touchControls, new ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        if (getIntent().getBooleanExtra("manageMods", false)) openAndroidModManager(false);
    }
    // Called through SDL's Activity reference from the native mod-menu buttons.
    public void openAndroidModManager(boolean install) {
        runOnUiThread(() -> {
            if (isFinishing() || modDialogOpen) return;
            modDialogOpen = true;
            new AlertDialog.Builder(this).setTitle("Close game and manage mods?")
                .setMessage("Installing or removing mods requires closing this game session. Save your progress before continuing; unsaved progress will be lost.")
                .setNegativeButton("Keep playing", null)
                .setPositiveButton("Close game", (dialog, which) -> {
                    openingMods = true; importMod = install; finish();
                }).setOnDismissListener(dialog -> modDialogOpen = false).show();
        });
    }
    @Override protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        if (intent.getBooleanExtra("manageMods", false)) openAndroidModManager(false);
    }
    @Override protected SDLSurface createSDLSurface(Context context) {
        return new SDLSurface(context) {
            @Override public void surfaceCreated(SurfaceHolder holder) {
                super.surfaceCreated(holder);
                nativeSurfaceReady(true);
            }
            @Override public void surfaceDestroyed(SurfaceHolder holder) {
                nativeSurfaceReady(false);
                super.surfaceDestroyed(holder);
            }
        };
    }
    @Override protected void onPause() {
        if (touchControls != null) touchControls.reset();
        if (!mBrokenLibraries) nativePauseScheduler(true);
        super.onPause();
    }
    @Override protected void onResume() {
        super.onResume();
        if (!mBrokenLibraries) nativePauseScheduler(false);
    }
    @Override protected void onDestroy() {
        if (!mBrokenLibraries) nativePauseScheduler(false);
        super.onDestroy();
        // Keep the lease until process death: native configuration workers may still exist
        // after SDL returns. The OS releases the lock when this dedicated process exits.
        if (openingMods) startActivity(new Intent(this, ModActivity.class)
            .addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP)
            .putExtra("importMod", importMod));
        // Game globals are not restartable; this Activity has a dedicated :game process.
        android.os.Process.killProcess(android.os.Process.myPid());
    }
}
