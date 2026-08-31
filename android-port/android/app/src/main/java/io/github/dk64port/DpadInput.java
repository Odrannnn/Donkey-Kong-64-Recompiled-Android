package io.github.dk64port;

/** N64 digital directions, independent of the analog stick and C buttons. */
final class DpadInput {
    static final int UP = 0x0800, DOWN = 0x0400, LEFT = 0x0200, RIGHT = 0x0100;
    static int directions(float x, float y) {
        if (!Float.isFinite(x) || !Float.isFinite(y) || Math.abs(x) > 1 || Math.abs(y) > 1) return 0;
        float ax = Math.abs(x), ay = Math.abs(y);
        if (Math.max(ax, ay) <= .25f) return 0;
        int mask = 0;
        if (ax > .25f && ax >= ay * .41421356f) mask |= x < 0 ? LEFT : RIGHT;
        if (ay > .25f && ay >= ax * .41421356f) mask |= y < 0 ? UP : DOWN;
        return mask;
    }
    static int withoutOpposites(int mask) {
        if ((mask & (UP | DOWN)) == (UP | DOWN)) mask &= ~(UP | DOWN);
        if ((mask & (LEFT | RIGHT)) == (LEFT | RIGHT)) mask &= ~(LEFT | RIGHT);
        return mask;
    }
}
