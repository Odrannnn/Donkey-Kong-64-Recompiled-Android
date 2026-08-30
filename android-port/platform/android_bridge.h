#pragma once
#include "android_driver.h"
#include <cstdint>
bool dk64_android_get_input(int player, uint16_t* buttons, float* x, float* y);
