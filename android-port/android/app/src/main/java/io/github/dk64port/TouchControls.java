package io.github.dk64port;

import android.content.Context;
import android.graphics.*;
import android.view.*;
import java.util.*;
import org.libsdl.app.SDLActivity;

/** Multi-touch N64 controls; touches outside controls pass through to SDL's menus. */
final class TouchControls extends View {
    private static final int STICK = -1, VISIBILITY = -2, MENU = -3;
    private static final class Control {
        final String label;
        final int mask;
        final RectF bounds;
        Control(String label, int mask, float x, float y, float size) {
            this.label = label; this.mask = mask;
            bounds = new RectF(x - size/2, y - size/2, x + size/2, y + size/2);
        }
    }
    private final Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
    private final ArrayList<Control> controls = new ArrayList<>();
    private final Map<Integer, Control> pointers = new HashMap<>();
    private boolean visible = true;
    private float stickX, stickY;
    private Control stick;

    TouchControls(Context context) { super(context); setFocusable(false); }

    @Override public WindowInsets onApplyWindowInsets(WindowInsets insets) {
        int left = insets.getSystemWindowInsetLeft(), top = insets.getSystemWindowInsetTop();
        int right = insets.getSystemWindowInsetRight(), bottom = insets.getSystemWindowInsetBottom();
        if (left != getPaddingLeft() || top != getPaddingTop() || right != getPaddingRight() || bottom != getPaddingBottom()) {
            setPadding(left, top, right, bottom);
            layoutControls(getWidth(), getHeight());
            invalidate();
        }
        return insets;
    }

    @Override protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        layoutControls(w, h);
    }

    private void layoutControls(int width, int height) {
        reset();
        controls.clear();
        int w = width - getPaddingLeft() - getPaddingRight();
        int h = height - getPaddingTop() - getPaddingBottom();
        if (w <= 0 || h <= 0) { stick = null; return; }
        float s = Math.min(h * .115f, 76 * getResources().getDisplayMetrics().density);
        stick = new Control("", STICK, s*1.65f, h-s*1.65f, s*2.5f);
        controls.add(stick);
        controls.add(new Control("A", 0x8000, w-s*.8f, h-s*1.4f, s));
        controls.add(new Control("B", 0x4000, w-s*2f, h-s*.85f, s));
        controls.add(new Control("Z", 0x2000, s*.8f, h-s*3.5f, s));
        controls.add(new Control("L", 0x0020, s*2f, h-s*3.5f, s));
        controls.add(new Control("R", 0x0010, w-s*.8f, h-s*4.4f, s));
        controls.add(new Control("C↑", 0x0008, w-s*2.9f, h-s*3.8f, s*.8f));
        controls.add(new Control("C↓", 0x0004, w-s*2.9f, h-s*2f, s*.8f));
        controls.add(new Control("C←", 0x0002, w-s*3.8f, h-s*2.9f, s*.8f));
        controls.add(new Control("C→", 0x0001, w-s*2f, h-s*2.9f, s*.8f));
        controls.add(new Control("Start", 0x1000, w*.5f, h-s*.6f, s));
        controls.add(new Control("Touch", VISIBILITY, w*.5f-s*.7f, s*.6f, s));
        controls.add(new Control("Menu", MENU, w*.5f+s*.7f, s*.6f, s));
        for (Control control : controls) control.bounds.offset(getPaddingLeft(), getPaddingTop());
    }

    @Override protected void onDraw(Canvas canvas) {
        for (Control control : controls) {
            if (!visible && control.mask != VISIBILITY && control.mask != MENU) continue;
            boolean pressed = pointers.containsValue(control);
            paint.setColor(pressed ? 0xbbd9a441 : 0x77333333);
            canvas.drawRoundRect(control.bounds, 20, 20, paint);
            paint.setColor(0xddeeeeee);
            paint.setStyle(Paint.Style.STROKE);
            paint.setStrokeWidth(2);
            canvas.drawRoundRect(control.bounds, 20, 20, paint);
            paint.setStyle(Paint.Style.FILL);
            paint.setTextAlign(Paint.Align.CENTER);
            paint.setTextSize(control.bounds.height()*.27f);
            canvas.drawText(control.label, control.bounds.centerX(), control.bounds.centerY()+paint.getTextSize()*.35f, paint);
        }
        if (visible && stick != null) {
            paint.setColor(0xaaeeeeee);
            float radius = stick.bounds.width()*.2f;
            canvas.drawCircle(stick.bounds.centerX()+stickX*radius*1.3f,
                stick.bounds.centerY()-stickY*radius*1.3f, radius, paint);
        }
    }

    @Override public boolean onTouchEvent(MotionEvent event) {
        int action = event.getActionMasked(), index = event.getActionIndex();
        int pointer = event.getPointerId(index);
        if (action == MotionEvent.ACTION_DOWN || action == MotionEvent.ACTION_POINTER_DOWN) {
            Control hit = null;
            for (Control control : controls) {
                if ((visible || control.mask == VISIBILITY || control.mask == MENU)
                    && control.bounds.contains(event.getX(index), event.getY(index))) { hit = control; break; }
            }
            if (hit == null && action == MotionEvent.ACTION_DOWN) return false;
            if (hit != null) {
                if (hit.mask == VISIBILITY) { visible = !visible; reset(); }
                else if (hit.mask == MENU) {
                    SDLActivity.onNativeKeyDown(KeyEvent.KEYCODE_ESCAPE);
                    SDLActivity.onNativeKeyUp(KeyEvent.KEYCODE_ESCAPE);
                } else if (hit.mask != STICK || !pointers.containsValue(stick)) pointers.put(pointer, hit);
            }
        } else if (action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_POINTER_UP) {
            Control released = pointers.remove(pointer);
            if (released == stick) { stickX = 0; stickY = 0; }
        } else if (action == MotionEvent.ACTION_CANCEL) {
            reset();
        }
        for (int i = 0; i < event.getPointerCount(); i++) {
            if (pointers.get(event.getPointerId(i)) == stick) {
                float radius = stick.bounds.width()*.4f;
                float x = (event.getX(i)-stick.bounds.centerX())/radius;
                float y = (stick.bounds.centerY()-event.getY(i))/radius;
                float length = (float)Math.hypot(x, y);
                stickX = length > 1 ? x/length : x;
                stickY = length > 1 ? y/length : y;
            }
        }
        sendState();
        invalidate();
        return true;
    }

    void reset() { pointers.clear(); stickX = stickY = 0; sendState(); invalidate(); }
    private void sendState() {
        int buttons = 0;
        for (Control control : pointers.values()) if (control.mask > 0) buttons |= control.mask;
        GameActivity.nativeTouchState(buttons, stickX, stickY);
    }
}
