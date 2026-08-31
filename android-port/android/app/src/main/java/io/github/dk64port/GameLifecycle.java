package io.github.dk64port;

/** Resume can arrive before lock-screen focus or the render surface is restored. */
final class GameLifecycle {
    boolean resumed, focused, surfaceReady;
    boolean active() { return resumed && focused && surfaceReady; }
}
