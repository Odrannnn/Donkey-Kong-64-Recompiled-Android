// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// Internal capabilities. No configuration option or native export may set
// these; only the future validated game-thread campaign orchestrator can.
#define AP_GAMEPLAY_MOVES 1u
#define AP_GAMEPLAY_REWARDS 2u
#define AP_GAMEPLAY_SHOPS 4u
#define AP_GAMEPLAY_SAVE 8u
#define AP_GAMEPLAY_CHECKS 16u
#define AP_GAMEPLAY_ITEMS 32u
#define AP_GAMEPLAY_TRAPS 64u
