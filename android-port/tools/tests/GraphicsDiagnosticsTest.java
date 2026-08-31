package io.github.dk64port;

import java.io.IOException;

/** Regression checks for upgrade/default behavior; no Android services are invoked. */
public final class GraphicsDiagnosticsTest {
    private static int checks;

    private static void check(boolean value, String description) {
        if (!value) throw new AssertionError(description);
        checks++;
    }

    private static String launchFlags(String saved) throws IOException {
        return GraphicsDiagnostics.flagsForMode(GraphicsDiagnostics.modeForSavedValue(saved), true);
    }

    public static void main(String[] args) throws Exception {
        check(launchFlags(null).equals("gmem"), "New installations must launch Turnip with GMEM");
        check(launchFlags("").equals("gmem"), "Old Off/default must adopt GMEM");
        check(launchFlags("gmem").equals("gmem"), "The phone's dev9 GMEM choice must survive updating");
        check(launchFlags("sysmem").equals("sysmem"), "Keep an explicitly saved sysmem workaround");
        for (String saved : new String[] {"nolrz", "noubwc", "flushall,syncdraw", "gmem,flushall,syncdraw"}) {
            check(launchFlags(saved).equals(saved), "Keep explicit diagnostic: " + saved);
        }

        String automatic = GraphicsDiagnostics.savedValue(GraphicsDiagnostics.DRIVER_DEFAULTS_MODE);
        check(!automatic.isEmpty(), "A new automatic opt-out must differ from the old empty default");
        check(launchFlags(automatic).isEmpty(), "Explicit automatic selection must survive saving/reopening");
        check(launchFlags(GraphicsDiagnostics.savedValue(GraphicsDiagnostics.COMPATIBILITY_MODE)).equals("sysmem"),
            "Enabling compatibility mode must select only sysmem");
        check(launchFlags(GraphicsDiagnostics.savedValue(GraphicsDiagnostics.DEFAULT_MODE)).equals("gmem"),
            "Disabling compatibility mode must return to GMEM");
        for (int index = 0; index < GraphicsDiagnostics.LABELS.length; index++) {
            check(GraphicsDiagnostics.flagsForMode(index, false).isEmpty(), "System driver gets no flags: " + index);
        }
        check(GraphicsDiagnostics.flags(null, false).isEmpty(), "System-driver launches must not access saved preferences");
        try {
            GraphicsDiagnostics.modeForSavedValue("gmem,sysmem");
            throw new AssertionError("Unknown/mixed flags must not be passed through");
        } catch (IOException expected) { checks++; }
        try {
            GraphicsDiagnostics.savedValue(-1);
            throw new AssertionError("Invalid mode must not reach preference writing");
        } catch (IllegalArgumentException expected) { checks++; }
        System.out.println("Graphics default/upgrade checks passed: " + checks);
    }
}
