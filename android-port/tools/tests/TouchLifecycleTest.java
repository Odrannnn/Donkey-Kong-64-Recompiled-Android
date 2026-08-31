package io.github.dk64port;

public final class TouchLifecycleTest {
    static int checks;
    static void check(boolean condition) { checks++; if (!condition) throw new AssertionError("check " + checks); }
    public static void main(String[] args) {
        check(DpadInput.directions(0, -.8f) == DpadInput.UP);
        check(DpadInput.directions(0, .8f) == DpadInput.DOWN);
        check(DpadInput.directions(-.8f, 0) == DpadInput.LEFT);
        check(DpadInput.directions(.8f, 0) == DpadInput.RIGHT);
        check(DpadInput.directions(-.8f, -.8f) == (DpadInput.UP | DpadInput.LEFT));
        check(DpadInput.directions(.8f, -.8f) == (DpadInput.UP | DpadInput.RIGHT));
        check(DpadInput.directions(-.8f, .8f) == (DpadInput.DOWN | DpadInput.LEFT));
        check(DpadInput.directions(.8f, .8f) == (DpadInput.DOWN | DpadInput.RIGHT));
        check(DpadInput.directions(.9f, .3f) == DpadInput.RIGHT);
        check(DpadInput.directions(0, 0) == 0);
        check(DpadInput.directions(.25f, -.25f) == 0);
        check(DpadInput.directions(1.01f, 0) == 0);
        check(DpadInput.directions(0, Float.NaN) == 0);
        check(DpadInput.directions(Float.POSITIVE_INFINITY, 0) == 0);
        check(DpadInput.withoutOpposites(DpadInput.UP | DpadInput.DOWN | 0x8000) == 0x8000);
        check(DpadInput.withoutOpposites(DpadInput.RIGHT | DpadInput.LEFT | DpadInput.UP) == DpadInput.UP);
        for (int mask = 0; mask < 16; mask++) {
            int value = DpadInput.withoutOpposites(mask << 8);
            check((value & 0xc00) != 0xc00 && (value & 0x300) != 0x300);
        }
        // All possible focus/surface/activity combinations, including resume before unlock.
        for (int bits = 0; bits < 8; bits++) {
            GameLifecycle state = new GameLifecycle();
            state.resumed = (bits & 1) != 0;
            state.focused = (bits & 2) != 0;
            state.surfaceReady = (bits & 4) != 0;
            check(state.active() == (bits == 7));
        }
        GameLifecycle lock = new GameLifecycle();
        lock.resumed = lock.focused = lock.surfaceReady = true;
        lock.focused = false; check(!lock.active()); // Lock screen covers an otherwise resumed activity.
        lock.resumed = false; lock.surfaceReady = false;
        lock.resumed = true; check(!lock.active());
        lock.surfaceReady = true; check(!lock.active());
        lock.focused = true; check(lock.active());
        lock.surfaceReady = false; check(!lock.active()); // Surface recreation while focused.
        System.out.println("PASS: " + checks + " D-pad direction/diagonal/dead-zone and lock-screen lifecycle checks");
    }
}
