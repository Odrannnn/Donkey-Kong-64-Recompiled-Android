#include "android_bridge.h"
#include <algorithm>
#include <cmath>
#include <mutex>
#include <jni.h>
#include <SDL.h>
#include "recompinput/profiles.h"
#include "recompinput/recompinput.h"
#include "ultramodern/ultramodern.hpp"

namespace {
std::mutex touch_mutex;
uint16_t touch_buttons = 0;
float touch_x = 0.0f;
float touch_y = 0.0f;
}

extern "C" void plume_set_android_surface_ready(int ready);

extern "C" void dk64_android_open_mod_manager(bool install) {
    JNIEnv* env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());
    if (!env || !activity) return;
    jclass type = env->GetObjectClass(activity);
    jmethodID method = env->GetMethodID(type, "openAndroidModManager", "(Z)V");
    if (method) env->CallVoidMethod(activity, method, static_cast<jboolean>(install));
    if (env->ExceptionCheck()) { env->ExceptionDescribe(); env->ExceptionClear(); }
    env->DeleteLocalRef(type);
    env->DeleteLocalRef(activity);
}

extern "C" JNIEXPORT void JNICALL
Java_io_github_dk64port_GameActivity_nativeTouchState(JNIEnv*, jclass, jint buttons, jfloat x, jfloat y) {
    std::lock_guard lock(touch_mutex);
    touch_buttons = static_cast<uint16_t>(buttons);
    touch_x = std::isfinite(x) ? std::clamp(x, -1.0f, 1.0f) : 0.0f;
    touch_y = std::isfinite(y) ? std::clamp(y, -1.0f, 1.0f) : 0.0f;
}

extern "C" JNIEXPORT void JNICALL
Java_io_github_dk64port_GameActivity_nativePauseScheduler(JNIEnv*, jclass, jboolean paused) {
    ultramodern::set_vi_scheduler_paused(paused);
}

extern "C" JNIEXPORT void JNICALL
Java_io_github_dk64port_GameActivity_nativeSurfaceReady(JNIEnv*, jclass, jboolean ready) {
    plume_set_android_surface_ready(ready ? 1 : 0);
}

bool dk64_android_get_input(int player, uint16_t* buttons, float* x, float* y) {
    bool available = recompinput::profiles::get_n64_input(player, buttons, x, y);
    if (player == 0 && !recompinput::game_input_disabled()) {
        std::lock_guard lock(touch_mutex);
        if (!available) { *buttons = 0; *x = 0.0f; *y = 0.0f; }
        *buttons |= touch_buttons;
        *x = std::clamp(*x + touch_x, -1.0f, 1.0f);
        *y = std::clamp(*y + touch_y, -1.0f, 1.0f);
        return true;
    }
    return available;
}
