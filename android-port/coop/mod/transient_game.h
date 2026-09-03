#ifndef DKCOOP_TRANSIENT_GAME_H
#define DKCOOP_TRANSIENT_GAME_H

typedef struct {
    unsigned short map, object;
    unsigned char kind, activation;
} CoopTransientObject;

// Character-spawner private data is not part of common_structs.h. Giant Clam
// uses only the signed word at pinned offset 0x2C for its 90-frame countdown.
typedef struct { unsigned char pad[0x2C]; int timer; } CoopClamData;
// Ice Tomato's private controller mode is pinned at offset 0x38. The shared
// global board contains -1 (unclaimed), 0 (player) or 1 (Tomato) per cell.
typedef struct { unsigned char pad[0x38]; unsigned char state, menu_state; } CoopIceTomatoData;
#ifndef DKCOOP_ENEMY_SPAWNER_TABLE_DEFINED
#define DKCOOP_ENEMY_SPAWNER_TABLE_DEFINED
typedef struct { s16 count, padding; EnemySpawner* first; } CoopEnemySpawnerTable;
#endif
extern CoopEnemySpawnerTable D_807FDC88;

static Prop_ScriptData* coop_transient_script(unsigned object);

// Pinned loaded scripts whose state drives a reviewed reversible switch,
// timer-driven presentation, or linked platform. Permanent gate/door readers
// come from coop_live_world_states below.
static const CoopTransientObject coop_transient_extra_objects[] = {
    // Japes Diddy-cage coconut switches. Their local state-2 blocks update the
    // shared vanilla cage counter; the cage/reward scripts remain local.
    {7, 0x30, COOP_TRANSIENT_TRIGGER, 2}, {7, 0x31, COOP_TRANSIENT_TRIGGER, 2},
    {7, 0x32, COOP_TRANSIENT_TRIGGER, 2},
    // Japes hive-area feather switches. The local state-2 blocks own the
    // counter and gate sequence; completed-state refresh still uses flag 7.
    {7, 0x34, COOP_TRANSIENT_TRIGGER, 2}, {7, 0x35, COOP_TRANSIENT_TRIGGER, 2},
    // Remaining Japes gun switches: painting room, paired Diddy cave, Rambi.
    // Each state-2 block owns its linked gate/counter and presentation locally.
    {7, 0x28, COOP_TRANSIENT_TRIGGER, 2}, {7, 0x29, COOP_TRANSIENT_TRIGGER, 2},
    {7, 0x2A, COOP_TRANSIENT_TRIGGER, 2}, {7, 0x123, COOP_TRANSIENT_TRIGGER, 2},
    // Japes' four Kong hut switches and the Rambi rock-wall switch. Their
    // state-2 blocks own the permanent flag and linked hut/wall presentation.
    {7, 0x38, COOP_TRANSIENT_TRIGGER, 2}, {7, 0x39, COOP_TRANSIENT_TRIGGER, 2},
    {7, 0x3A, COOP_TRANSIENT_TRIGGER, 2}, {7, 0x3B, COOP_TRANSIENT_TRIGGER, 2},
    {7, 0x115, COOP_TRANSIENT_TRIGGER, 2},
    // Remaining Japes feather, grape and peanut switches. The first pair use
    // state 11 as their exact timed-door activation; peanut uses state 2.
    {7, 0x1F, COOP_TRANSIENT_TRIGGER, 11}, {7, 0x20, COOP_TRANSIENT_TRIGGER, 11},
    {7, 0x58, COOP_TRANSIENT_TRIGGER, 2},
    // Paired cannon-game, shipwreck and lighthouse gun switches. Completed
    // presentation still uses the existing permanent live-world flag rows.
    {30, 0x06, COOP_TRANSIENT_TRIGGER, 2}, {30, 0x07, COOP_TRANSIENT_TRIGGER, 2},
    {30, 0x08, COOP_TRANSIENT_TRIGGER, 2}, {30, 0x09, COOP_TRANSIENT_TRIGGER, 2},
    {30, 0x0A, COOP_TRANSIENT_TRIGGER, 2}, {30, 0x0B, COOP_TRANSIENT_TRIGGER, 2},
    {30, 0, COOP_TRANSIENT_TIMER, 0}, {30, 1, COOP_TRANSIENT_TIMER, 0},
    // Galleon's five instrument pads and Lanky/Tiny slam switches. State 2 is
    // the shared vanilla post-input entry; each linked door remains local.
    {30, 0x11, COOP_TRANSIENT_TRIGGER, 2}, {30, 0x12, COOP_TRANSIENT_TRIGGER, 2},
    {30, 0x13, COOP_TRANSIENT_TRIGGER, 2}, {30, 0x14, COOP_TRANSIENT_TRIGGER, 2},
    {30, 0x1B, COOP_TRANSIENT_TRIGGER, 2}, {30, 0x1C, COOP_TRANSIENT_TRIGGER, 2},
    {30, 0x1D, COOP_TRANSIENT_TRIGGER, 2},
    // Cannon-game targets may be hit only after each local game has exposed
    // that target at state 12. State 13 retains local scoring/link updates.
    {30, 0x2F, COOP_TRANSIENT_TRIGGER, 13}, {30, 0x30, COOP_TRANSIENT_TRIGGER, 13},
    {30, 0x31, COOP_TRANSIENT_TRIGGER, 13},
    // Enguarde door. Its reviewed state-2 entry starts the stock local door,
    // cutscene and permanent-completion path after an Enguarde charge.
    {30, 0x21, COOP_TRANSIENT_TRIGGER, 2},
    // Galleon breakable gate enters state 1 directly from unbroken state 0.
    {30, 0x3F, COOP_TRANSIENT_TRIGGER, 1},
    // Llama Temple coconut switch and DK bongo pad. Their local state-2
    // sequences own the cooling/llama-spit flags and linked gate presentation.
    {20, 0x12, COOP_TRANSIENT_TRIGGER, 2}, {20, 0x16, COOP_TRANSIENT_TRIGGER, 2},
    // DK's five-door-temple coconut panels. Each state-2 script owns its four
    // local wall-panel updates and texture sequence.
    {19, 0x04, COOP_TRANSIENT_TRIGGER, 2}, {19, 0x05, COOP_TRANSIENT_TRIGGER, 2},
    // Diddy, Tiny, Lanky and Chunky five-door-temple weapon panels. Their
    // state-2 scripts retain all linked wall-panel and texture operations.
    {21, 0x04, COOP_TRANSIENT_TRIGGER, 2}, {21, 0x05, COOP_TRANSIENT_TRIGGER, 2},
    {22, 0x04, COOP_TRANSIENT_TRIGGER, 2}, {22, 0x05, COOP_TRANSIENT_TRIGGER, 2},
    {23, 0x04, COOP_TRANSIENT_TRIGGER, 2}, {23, 0x05, COOP_TRANSIENT_TRIGGER, 2},
    {24, 0x04, COOP_TRANSIENT_TRIGGER, 2}, {24, 0x05, COOP_TRANSIENT_TRIGGER, 2},
    {24, 0x07, COOP_TRANSIENT_TRIGGER, 2},
    // Llama Temple matching game heads. Every head is armed in state 11 and
    // enters state 12 after a local grape hit. Its own script supplies the
    // sound, five-frame debounce and matching-controller notification.
    {20, 0x19, COOP_TRANSIENT_TRIGGER, 12}, {20, 0x1A, COOP_TRANSIENT_TRIGGER, 12},
    {20, 0x1B, COOP_TRANSIENT_TRIGGER, 12}, {20, 0x1C, COOP_TRANSIENT_TRIGGER, 12},
    {20, 0x1D, COOP_TRANSIENT_TRIGGER, 12}, {20, 0x1E, COOP_TRANSIENT_TRIGGER, 12},
    {20, 0x1F, COOP_TRANSIENT_TRIGGER, 12}, {20, 0x20, COOP_TRANSIENT_TRIGGER, 12},
    {20, 0x21, COOP_TRANSIENT_TRIGGER, 12}, {20, 0x22, COOP_TRANSIENT_TRIGGER, 12},
    {20, 0x23, COOP_TRANSIENT_TRIGGER, 12}, {20, 0x24, COOP_TRANSIENT_TRIGGER, 12},
    {20, 0x25, COOP_TRANSIENT_TRIGGER, 12}, {20, 0x26, COOP_TRANSIENT_TRIGGER, 12},
    {20, 0x27, COOP_TRANSIENT_TRIGGER, 12}, {20, 0x28, COOP_TRANSIENT_TRIGGER, 12},
    // Llama Temple quicksand-tunnel slam switch. State 2 begins the reviewed
    // local delay and tunnel-door update; permanent flag 0x3E stays local.
    {20, 0x69, COOP_TRANSIENT_TRIGGER, 2},
    // Llama Temple grape switch. State 2 begins its local timed gate cycle.
    {20, 0x6B, COOP_TRANSIENT_TRIGGER, 2},
    // Tiny Temple opening switch, Diddy guitar pad and charge switch.
    {16, 0x00, COOP_TRANSIENT_TRIGGER, 2}, {16, 0x04, COOP_TRANSIENT_TRIGGER, 2},
    {16, 0x14, COOP_TRANSIENT_TRIGGER, 2},
    // Synthetic logical progress for the K-O-N-G letter chain. Object C is
    // the record key; F, E, D and C remain the four locally gated steps.
    {16, 0x0C, COOP_TRANSIENT_SEQUENCE, 0},
    // Aztec's three exterior llama switches. Only states 2-6 are the local
    // projectile sequence; their distinct linked states 10-12 are not shared.
    {38, 0x0D, COOP_TRANSIENT_TRIGGER, 2}, {38, 0x0E, COOP_TRANSIENT_TRIGGER, 2},
    {38, 0x0F, COOP_TRANSIENT_TRIGGER, 2},
    // Aztec exterior Diddy guitar pad; its local state-2 block owns the timer
    // and linked rotating-tower object.
    {38, 0x44, COOP_TRANSIENT_TRIGGER, 2},
    // Aztec blueprint-door coconut switches. Override the completed live-world
    // rows with their narrower state-2 projectile activation while unfinished.
    {38, 0x9D, COOP_TRANSIENT_TRIGGER, 2}, {38, 0x9E, COOP_TRANSIENT_TRIGGER, 2},
    // Four exterior gate switches and the remaining two pineapple blueprint-
    // door switches. Each uses the same exact state-1 to state-2 hit edge.
    {38, 0x02, COOP_TRANSIENT_TRIGGER, 2}, {38, 0x03, COOP_TRANSIENT_TRIGGER, 2},
    {38, 0x04, COOP_TRANSIENT_TRIGGER, 2}, {38, 0x05, COOP_TRANSIENT_TRIGGER, 2},
    {38, 0x9F, COOP_TRANSIENT_TRIGGER, 2}, {38, 0xA0, COOP_TRANSIENT_TRIGGER, 2},
    // Five-door totem weapon switches are locally revealed by permanent flag
    // 0x37. Only exact ready state 12 may enter the projectile-hit state 13.
    {38, 0x10, COOP_TRANSIENT_TRIGGER, 13}, {38, 0x11, COOP_TRANSIENT_TRIGGER, 13},
    {38, 0x12, COOP_TRANSIENT_TRIGGER, 13}, {38, 0x13, COOP_TRANSIENT_TRIGGER, 13},
    {38, 0x14, COOP_TRANSIENT_TRIGGER, 13},
    // Isles trombone pad. Its local barrel dependency moves it to state 2;
    // only then may the reviewed instrument activation enter state 3.
    {34, 0x31, COOP_TRANSIENT_TRIGGER, 3},
    // Isles sax pad uses the same local-barrel prerequisite and state-3
    // instrument edge before its Fairy Island door/reward sequence.
    {34, 0x33, COOP_TRANSIENT_TRIGGER, 3},
    {48, 4, COOP_TRANSIENT_TIMER, 0}, {48, 5, COOP_TRANSIENT_TIMER, 0},
    // Fungi green-tunnel feather/pineapple pairs and yellow-tunnel grape
    // switch. Each state-2 block owns its linked gate and completion locally.
    {48, 0x18, COOP_TRANSIENT_TRIGGER, 2}, {48, 0x19, COOP_TRANSIENT_TRIGGER, 2},
    {48, 0x1A, COOP_TRANSIENT_TRIGGER, 2}, {48, 0x1B, COOP_TRANSIENT_TRIGGER, 2},
    {48, 0x1E, COOP_TRANSIENT_TRIGGER, 2},
    // Fungi Diddy night slam and Lanky mushroom slam switches. State 2 starts
    // their local timer, object and permanent-completion sequences.
    {48, 0x0F, COOP_TRANSIENT_TRIGGER, 2}, {48, 0xEB, COOP_TRANSIENT_TRIGGER, 2},
    // Fungi main-map breakable door enters its local break/reset path at state 2.
    {48, 0x07, COOP_TRANSIENT_TRIGGER, 2},
    // Fungi dark-attic guitar pad and Lanky attic slam switch. State 2 starts
    // each local room sequence after its exact vanilla move check.
    {56, 0x00, COOP_TRANSIENT_TRIGGER, 2}, {58, 0x00, COOP_TRANSIENT_TRIGGER, 2},
    // Fungi Winch Room up button waits at state 10 and enters its local
    // 400-frame winch/reward sequence at state 11.
    {57, 0x03, COOP_TRANSIENT_TRIGGER, 11},
    // Thornvine Barn's two DK slam boxes enter their local 60-frame sequences
    // at state 2 after the reviewed move and contact checks.
    {59, 0x01, COOP_TRANSIENT_TRIGGER, 2}, {59, 0x24, COOP_TRANSIENT_TRIGGER, 2},
    // Fungi Mill-front slam box uniquely waits at state 10 and starts its
    // local timed box/camera sequence at state 11.
    {61, 0x06, COOP_TRANSIENT_TRIGGER, 11},
    // Fungi rear-mill triangle pad enters its local crusher activation and
    // reward path at state 2 after the stock instrument check.
    {62, 0x00, COOP_TRANSIENT_TRIGGER, 2},
    // Rear-mill question-mark boxes are locally armed at exact state 10 and
    // enter their break/reward paths at state 11.
    {62, 0x01, COOP_TRANSIENT_TRIGGER, 11}, {62, 0x03, COOP_TRANSIENT_TRIGGER, 11},
    // Hideout Helm instrument pads. Bongo uses its state-2 entry; the other
    // four are armed at state 11 and enter their local door sequence at 12.
    {17, 0x2C, COOP_TRANSIENT_TRIGGER, 2}, {17, 0x2D, COOP_TRANSIENT_TRIGGER, 12},
    {17, 0x2E, COOP_TRANSIENT_TRIGGER, 12}, {17, 0x2F, COOP_TRANSIENT_TRIGGER, 12},
    {17, 0x30, COOP_TRANSIENT_TRIGGER, 12},
    // Isles level-lobby feather switches. State 2 starts the local panel/door
    // presentation in the Aztec and Fungi lobbies respectively.
    {173, 0x10, COOP_TRANSIENT_TRIGGER, 2}, {178, 0x05, COOP_TRANSIENT_TRIGGER, 2},
    // Generic lobby instrument pads. Each exact state-1 input edge starts a
    // self-contained state 2/3/4 animation and 60-frame timer before returning
    // to state 1. No permanent flag or transition is written on this path.
    {173, 0x00, COOP_TRANSIENT_TRIGGER, 2}, {173, 0x01, COOP_TRANSIENT_TRIGGER, 2},
    {173, 0x02, COOP_TRANSIENT_TRIGGER, 2}, {173, 0x03, COOP_TRANSIENT_TRIGGER, 2},
    {174, 0x04, COOP_TRANSIENT_TRIGGER, 2}, {174, 0x05, COOP_TRANSIENT_TRIGGER, 2},
    {174, 0x06, COOP_TRANSIENT_TRIGGER, 2}, {174, 0x07, COOP_TRANSIENT_TRIGGER, 2},
    {174, 0x08, COOP_TRANSIENT_TRIGGER, 2},
    {175, 0x00, COOP_TRANSIENT_TRIGGER, 2}, {175, 0x01, COOP_TRANSIENT_TRIGGER, 2},
    {175, 0x02, COOP_TRANSIENT_TRIGGER, 2}, {175, 0x03, COOP_TRANSIENT_TRIGGER, 2},
    {175, 0x04, COOP_TRANSIENT_TRIGGER, 2},
    {193, 0x02, COOP_TRANSIENT_TRIGGER, 2}, {193, 0x03, COOP_TRANSIENT_TRIGGER, 2},
    {193, 0x04, COOP_TRANSIENT_TRIGGER, 2}, {193, 0x05, COOP_TRANSIENT_TRIGGER, 2},
    {193, 0x06, COOP_TRANSIENT_TRIGGER, 2},
    {194, 0x10, COOP_TRANSIENT_TRIGGER, 2}, {194, 0x11, COOP_TRANSIENT_TRIGGER, 2},
    {194, 0x12, COOP_TRANSIENT_TRIGGER, 2}, {194, 0x13, COOP_TRANSIENT_TRIGGER, 2},
    {194, 0x14, COOP_TRANSIENT_TRIGGER, 2},
    // Caves small/large boulder pads. The small pad accepts state 2 from ready
    // state 1; the large pad must finish its local reveal at state 12 first.
    {72, 0x2E, COOP_TRANSIENT_TRIGGER, 2}, {72, 0x2F, COOP_TRANSIENT_TRIGGER, 13},
    // Three Caves main-map breakable ice walls. Their state-2 entries retain
    // local debris, collision, visual-pair and permanent-completion work.
    {72, 0x1D, COOP_TRANSIENT_TRIGGER, 2}, {72, 0x1E, COOP_TRANSIENT_TRIGGER, 2},
    {72, 0x1F, COOP_TRANSIENT_TRIGGER, 2},
    // Chunky's Caves cabin gun targets. Each hit also advances the local
    // Gorilla Gone controller by one reviewed step in the apply adapter.
    {90, 0x03, COOP_TRANSIENT_TRIGGER, 2}, {90, 0x04, COOP_TRANSIENT_TRIGGER, 2},
    {90, 0x05, COOP_TRANSIENT_TRIGGER, 2},
    // Tiny's Caves igloo target has four locally gated hits. The synthetic
    // sequence record carries only bounded progress; reward/failure stays local.
    {84, 0x00, COOP_TRANSIENT_SEQUENCE, 0},
    // Castle Lanky/Tiny crypt grape and Simian Slam switches. Both enter their
    // local door/platform sequences through reviewed state 2.
    {108, 0x00, COOP_TRANSIENT_TRIGGER, 2}, {108, 0x04, COOP_TRANSIENT_TRIGGER, 2},
    // Castle DK/Diddy/Chunky crypt gun switches. Their state-2 entries retain
    // all local sound, linked-door and reset behavior.
    {112, 0x0D, COOP_TRANSIENT_TRIGGER, 2}, {112, 0x0E, COOP_TRANSIENT_TRIGGER, 2},
    {112, 0x0F, COOP_TRANSIENT_TRIGGER, 2},
    // Castle basement DK, Diddy and Lanky slam switches. Their reviewed
    // state-2 entries preserve each local door/platform sequence.
    {163, 0x04, COOP_TRANSIENT_TRIGGER, 2}, {163, 0x05, COOP_TRANSIENT_TRIGGER, 2},
    {163, 0x06, COOP_TRANSIENT_TRIGGER, 2},
    // Castle tree grape and Chunky-punch switches. The second has a distinct
    // state-5 activation edge; each local script owns its door and reset.
    {164, 0x01, COOP_TRANSIENT_TRIGGER, 2}, {164, 0x09, COOP_TRANSIENT_TRIGGER, 5},
    {194, 6, COOP_TRANSIENT_PLATFORM, 0},
    // Factory Snatch Room grate. Unlike ordinary triggers, its reviewed punch
    // edge enters state 1 directly from ready state 0.
    {26, 0x15, COOP_TRANSIENT_TRIGGER, 1},
    // Remaining Factory punch switches and breakable metal grates also enter
    // state 1 from exact raw state 0; the coconut switch enters state 2.
    {26, 0x20, COOP_TRANSIENT_TRIGGER, 1}, {26, 0x3C, COOP_TRANSIENT_TRIGGER, 1},
    {26, 0x13A, COOP_TRANSIENT_TRIGGER, 1}, {26, 0x13C, COOP_TRANSIENT_TRIGGER, 1},
    {26, 0x140, COOP_TRANSIENT_TRIGGER, 2},
    // Japes Mountain's paired slam switches. A real hit occupies states 2-5;
    // states 10/11/15/16 are cross-linked presentation states, not hits.
    {4, 0x0A, COOP_TRANSIENT_TRIGGER, 2}, {4, 0x0B, COOP_TRANSIENT_TRIGGER, 2},
    // Japes Mountain GB slam switch enters its local 50-frame reward sequence
    // at state 2; completed state 20 stays on the item channel.
    {4, 0x06, COOP_TRANSIENT_TRIGGER, 2},
    // Factory production switches: Chunky, Tiny, Lanky and Diddy. Their
    // vanilla state-2 entry owns the timer/reward sequence locally.
    {26, 0x2E, COOP_TRANSIENT_TRIGGER, 2}, {26, 0x2F, COOP_TRANSIENT_TRIGGER, 2},
    {26, 0x30, COOP_TRANSIENT_TRIGGER, 2}, {26, 0x31, COOP_TRANSIENT_TRIGGER, 2},
    // Chunky's cage switch. State 2 starts its stock 600-frame cage/rescue
    // sequence; the cage, cutscene and ownership flag stay locally scripted.
    {26, 0x24, COOP_TRANSIENT_TRIGGER, 2},
    // Factory triangle, guitar and trombone pads. Their state-2 blocks own the
    // 80-frame presentation and linked production-room object locally.
    {26, 0x37, COOP_TRANSIENT_TRIGGER, 2}, {26, 0x38, COOP_TRANSIENT_TRIGGER, 2},
    {26, 0x3B, COOP_TRANSIENT_TRIGGER, 2},
    // Diddy's 3-1-2-4 switch room. Each local script owns its long cutscene,
    // switch hiding, enemy spawn and final reward after state 5 is entered.
    {26, 0x3F, COOP_TRANSIENT_TRIGGER, 5}, {26, 0x40, COOP_TRANSIENT_TRIGGER, 5},
    {26, 0x41, COOP_TRANSIENT_TRIGGER, 5},
    {26, 0x14, COOP_TRANSIENT_SEQUENCE, 0}, // Lanky's piano controller.
    {26, 0x7F, COOP_TRANSIENT_SEQUENCE, 0}, // Tiny's dartboard controller.
};

// The bridge result persists between fresh UDP samples. Remember the last timer
// sample applied in this room so a countdown can keep running locally instead of
// being reset to the same network value on every render frame.
static CoopTransientRecord coop_transient_applied_timers[COOP_TRANSIENT_RECORDS];
static unsigned coop_transient_applied_timer_epoch;
#define COOP_TRANSIENT_CUTSCENE_HOLD_FRAMES 90
static CoopTransientRecord coop_transient_last_cutscene;
static unsigned coop_transient_cutscene_epoch, coop_transient_cutscene_hold;
static unsigned coop_lobby_pad_applied_epoch, coop_lobby_pad_applied_map;
static unsigned coop_lobby_pad_applied_object;

// The generic bonus controller is shared by many unrelated minigames. Admit
// only the four vanilla Kosh identities whose current map, source map and
// target all match this immutable table.
typedef struct {
    unsigned char map, parent, target;
} CoopKoshIdentity;
typedef struct {
    unsigned short intro;
    short remaining, initial;
    unsigned char padding[3], text;
} CoopKoshData;
static const CoopKoshIdentity coop_kosh_identities[4] = {
    {10, 24, 18}, {115, 47, 22}, {116, 151, 25}, {117, 170, 28},
};
static void coop_kosh_behavior(void);
static void (*coop_kosh_original)(void);
static unsigned coop_kosh_hook, coop_kosh_pending_epoch, coop_kosh_pending_key;
static unsigned coop_kosh_applied_epoch, coop_kosh_applied_key;
static unsigned coop_kosh_success_epoch, coop_kosh_success_key;

// Minecart Mayhem uses a different overlay controller. Its success and failure
// both enter state 5, so only the exact player success action may be published.
typedef struct {
    unsigned char map, parent, kong;
} CoopMinecartIdentity;
static const CoopMinecartIdentity coop_minecart_identities[3] = {
    {77, 7, 4}, {129, 59, 0}, {130, 88, 1},
};
static void coop_minecart_behavior(void);
static void (*coop_minecart_original)(void);
static unsigned coop_minecart_hook, coop_minecart_pending_epoch, coop_minecart_pending_key;
static unsigned coop_minecart_applied_epoch, coop_minecart_applied_key;
static unsigned coop_minecart_success_epoch, coop_minecart_success_key;

// Both Fungi Rabbit Race rounds use one main-map actor. Round one is the
// permanent 0xF8 progression step; round two exposes the object 0x57 GB.
// The wrapper shares only the stock state-2 to state-0x28 success edge.
static void coop_rabbit_behavior(void);
static void (*coop_rabbit_original)(void);
static unsigned coop_rabbit_hook, coop_rabbit_pending_epoch, coop_rabbit_pending_key;
static unsigned coop_rabbit_applied_epoch, coop_rabbit_applied_key;
static unsigned coop_rabbit_success_epoch, coop_rabbit_success_key;

extern CharacterSpawner* D_global_asm_807FDC9C;
extern s32 func_global_asm_805FF800(Maps* map, s32* exit);
extern void func_bonus_80024158(void);
extern void func_bonus_800264E0(u8 success, u8 text);
extern void func_minecart_80024FD0(void);
extern void func_minecart_80024000(u8 success, u8 text);
extern void func_global_asm_806BE8BC(void);
extern void func_global_asm_80726EE0(u8 state);

static unsigned coop_kosh_actor_live(Actor* actor, unsigned generation) {
    if (!actor || D_global_asm_807FBB34 > 64) return 0;
    for (unsigned i = 0; i < D_global_asm_807FBB34; ++i)
        if (D_global_asm_807FB930[i].actor == actor) return actor->unk54 == generation;
    return 0;
}

static unsigned coop_kosh_identity(Actor* actor) {
    if (!coop_kosh_hook || !actor || actor != gCurrentActorPointer
            || actor->unk58 != ACTOR_MINIGAME_CONTROLLER
            || !(actor->object_properties_bitfield & 0x10) || !actor->unk178
            || !actor->unk11C || !D_global_asm_807FDC9C
            || D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER] != coop_kosh_behavior
            || !coop_kosh_actor_live(actor, actor->unk54)) return 0;
    CoopKoshData* data = (CoopKoshData*)actor->unk178;
    if (data->intro < 2 || D_global_asm_807FDC9C[1].pad0[0] != 2) return 0;
    Maps parent = current_map;
    s32 exit = -1;
    if (!func_global_asm_805FF800(&parent, &exit) || exit != 0) return 0;
    unsigned target = D_global_asm_807FDC9C[0].unkA_u8[0];
    for (unsigned i = 0; i < 4; ++i) {
        const CoopKoshIdentity* row = &coop_kosh_identities[i];
        if (row->map == (unsigned)current_map && row->parent == (unsigned)parent
                && row->target == target) return i + 1;
    }
    return 0;
}

static void coop_kosh_behavior(void) {
    Actor* actor = gCurrentActorPointer;
    unsigned generation = actor ? actor->unk54 : 0;
    unsigned key = coop_kosh_identity(actor);
    if (coop_kosh_pending_key && (coop_kosh_pending_epoch != epoch
            || transient_file_changed || loading_zone_transition_speed != 0.0f
            || !gPlayerPointer)) coop_kosh_pending_key = 0;
    if (key && coop_kosh_pending_epoch == epoch && coop_kosh_pending_key == key
            && !(coop_kosh_applied_epoch == epoch && coop_kosh_applied_key == key)
            && !transient_file_changed && loading_zone_transition_speed == 0.0f
            && gPlayerPointer && actor->control_state == 0) {
        coop_kosh_pending_key = 0;
        func_bonus_800264E0(1, 0xE);
        coop_kosh_applied_epoch = epoch;
        coop_kosh_applied_key = key;
    }
    if (coop_kosh_original) coop_kosh_original();
    if (key && coop_kosh_actor_live(actor, generation) && actor->control_state != 0
            && gPlayerPointer && gPlayerPointer->control_state == 0x44) {
        coop_kosh_success_epoch = epoch;
        coop_kosh_success_key = key;
        coop_kosh_applied_epoch = epoch;
        coop_kosh_applied_key = key;
        coop_kosh_pending_key = 0;
    }
}

static unsigned coop_minecart_identity(Actor* actor) {
    if (!coop_minecart_hook || !actor || actor != gCurrentActorPointer
            || actor->unk58 != ACTOR_MINECART_BONUS
            || !(actor->object_properties_bitfield & 0x10)
            || !actor->additional_actor_data || !actor->animation_state || !actor->unk11C
            || !gPlayerPointer || !gPlayerPointer->additional_actor_data
            || D_global_asm_8074C0A0[ACTOR_MINECART_BONUS] != coop_minecart_behavior
            || !coop_kosh_actor_live(actor, actor->unk54)) return 0;
    Maps parent = current_map;
    s32 exit = -1;
    if (!func_global_asm_805FF800(&parent, &exit) || exit != 0) return 0;
    for (unsigned i = 0; i < 3; ++i) {
        const CoopMinecartIdentity* row = &coop_minecart_identities[i];
        if (row->map == (unsigned)current_map && row->parent == (unsigned)parent
                && row->kong == current_character_index[0]) return i + 1;
    }
    return 0;
}

static void coop_minecart_latch_success(unsigned key) {
    coop_minecart_success_epoch = epoch;
    coop_minecart_success_key = key;
    coop_minecart_applied_epoch = epoch;
    coop_minecart_applied_key = key;
    coop_minecart_pending_key = 0;
}

static void coop_minecart_behavior(void) {
    Actor* actor = gCurrentActorPointer;
    unsigned generation = actor ? actor->unk54 : 0;
    unsigned key = coop_minecart_identity(actor);
    Actor* child = key ? actor->unk11C : 0;
    unsigned child_generation = child ? child->unk54 : 0;
    unsigned before = key ? actor->control_state : 0;
    if (coop_minecart_pending_key && (coop_minecart_pending_epoch != epoch
            || transient_file_changed || loading_zone_transition_speed != 0.0f
            || !gPlayerPointer)) coop_minecart_pending_key = 0;
    if (coop_minecart_original) coop_minecart_original();
    if (!key || !coop_kosh_actor_live(actor, generation)
            || coop_minecart_identity(actor) != key || actor->unk11C != child
            || !coop_kosh_actor_live(child, child_generation)) {
        coop_minecart_pending_key = 0;
        return;
    }
    if (before >= 1 && before <= 3 && actor->control_state == 5) {
        if (!transient_file_changed && loading_zone_transition_speed == 0.0f
                && gPlayerPointer && gPlayerPointer->control_state == 0x44)
            coop_minecart_latch_success(key);
        else coop_minecart_pending_key = 0;
        return;
    }
    if (actor->animation_state->unk64 == 0x292) {
        coop_minecart_pending_key = 0;
        return;
    }
    if (!(before >= 1 && before <= 3)) {
        if ((before == 4 || before == 10) && actor->control_state == 3) return;
        coop_minecart_pending_key = 0;
        return;
    }
    if (actor->control_state >= 1 && actor->control_state <= 3
            && coop_minecart_pending_epoch == epoch && coop_minecart_pending_key == key
            && !(coop_minecart_applied_epoch == epoch && coop_minecart_applied_key == key)
            && !transient_file_changed && loading_zone_transition_speed == 0.0f
            && gPlayerPointer) {
        coop_minecart_pending_key = 0;
        func_global_asm_80726EE0(0);
        func_minecart_80024000(1, 0xE);
        actor->control_state = 5;
        coop_minecart_latch_success(key);
    } else if (actor->control_state == 5 || actor->control_state == 6) {
        coop_minecart_pending_key = 0;
    }
}

static unsigned coop_rabbit_round_key(void) {
    if (isFlagSet(PERMFLAG_ITEM_GB_FUNGI_RABBIT_RACE, FLAG_TYPE_PERMANENT)) return 0;
    return isFlagSet(PERMFLAG_PROGRESS_RABBIT_RACE_1_COMPLETE,
        FLAG_TYPE_PERMANENT) ? 2 : 1;
}

static unsigned coop_rabbit_identity(Actor* actor) {
    if (!coop_rabbit_hook || !actor || actor != gCurrentActorPointer
            || (unsigned)current_map != MAP_FUNGI || current_character_index[0] != 2
            || actor->unk58 != ACTOR_RABBIT_RACE
            || !(actor->object_properties_bitfield & 0x10) || !actor->animation_state
            || transient_file_changed || loading_zone_transition_speed != 0.0f
            || !gPlayerPointer || !gPlayerPointer->additional_actor_data
            || !extra_player_info_pointer
            || gPlayerPointer->additional_actor_data != extra_player_info_pointer
            || (extra_player_info_pointer->unk1F0 & 0x100000)
            || !(extra_player_info_pointer->unk1F4 & 0x40)
            || !(D_global_asm_807FBB64 & 4)
            || D_global_asm_8074C0A0[ACTOR_RABBIT_RACE] != coop_rabbit_behavior
            || !coop_kosh_actor_live(actor, actor->unk54)) return 0;
    Prop_ScriptData* race = coop_transient_script(0x1F);
    if (!race || race->unk48[0] != 3) return 0;
    unsigned key = coop_rabbit_round_key();
    if (key == 2 && !coop_transient_script(0x57)) return 0;
    return key;
}

static void coop_rabbit_latch_success(unsigned key) {
    coop_rabbit_success_epoch = epoch;
    coop_rabbit_success_key = key;
    coop_rabbit_applied_epoch = epoch;
    coop_rabbit_applied_key = key;
    coop_rabbit_pending_key = 0;
}

static void coop_rabbit_behavior(void) {
    Actor* actor = gCurrentActorPointer;
    unsigned generation = actor ? actor->unk54 : 0;
    unsigned key = coop_rabbit_identity(actor);
    unsigned before = key ? actor->control_state : 0;
    if (coop_rabbit_pending_key && (coop_rabbit_pending_epoch != epoch
            || transient_file_changed || loading_zone_transition_speed != 0.0f
            || !gPlayerPointer)) coop_rabbit_pending_key = 0;
    if (coop_rabbit_original) coop_rabbit_original();
    if (!key || transient_file_changed || loading_zone_transition_speed != 0.0f
            || !gPlayerPointer || !coop_kosh_actor_live(actor, generation)
            || coop_rabbit_identity(actor) != key) {
        coop_rabbit_pending_key = 0;
        return;
    }
    if (before == 2 && actor->control_state == 0x28) {
        if (!transient_file_changed && loading_zone_transition_speed == 0.0f
                && gPlayerPointer) coop_rabbit_latch_success(key);
        else coop_rabbit_pending_key = 0;
        return;
    }
    if (before != 2 || actor->control_state == 0x27) {
        coop_rabbit_pending_key = 0;
        return;
    }
    if (actor->control_state == 2
            && coop_rabbit_pending_epoch == epoch && coop_rabbit_pending_key == key
            && !(coop_rabbit_applied_epoch == epoch && coop_rabbit_applied_key == key)
            && !transient_file_changed && loading_zone_transition_speed == 0.0f
            && gPlayerPointer) {
        actor->control_state = 0x28;
        actor->control_state_progress = 0;
        coop_rabbit_latch_success(key);
    } else if (actor->control_state != 2) {
        coop_rabbit_pending_key = 0;
    }
}

static void coop_transient_init(void) {
    if (!transient_enabled) return;
    coop_kosh_original = D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER];
    if (coop_kosh_original != func_bonus_80024158) {
        recomp_printf("[dk64-coop] Kosh controller modified by another mod; shared Kosh success disabled.\n");
        coop_kosh_original = 0;
    } else {
        D_global_asm_8074C0A0[ACTOR_MINIGAME_CONTROLLER] = coop_kosh_behavior;
        coop_kosh_hook = 1;
    }
    coop_minecart_original = D_global_asm_8074C0A0[ACTOR_MINECART_BONUS];
    if (coop_minecart_original != func_minecart_80024FD0) {
        recomp_printf("[dk64-coop] Minecart controller modified by another mod; shared Minecart success disabled.\n");
        coop_minecart_original = 0;
    } else {
        D_global_asm_8074C0A0[ACTOR_MINECART_BONUS] = coop_minecart_behavior;
        coop_minecart_hook = 1;
    }
    coop_rabbit_original = D_global_asm_8074C0A0[ACTOR_RABBIT_RACE];
    if (coop_rabbit_original != func_global_asm_806BE8BC) {
        recomp_printf("[dk64-coop] Rabbit Race actor modified by another mod; shared Rabbit success disabled.\n");
        coop_rabbit_original = 0;
        coop_rabbit_hook = 0;
    } else {
        D_global_asm_8074C0A0[ACTOR_RABBIT_RACE] = coop_rabbit_behavior;
        coop_rabbit_hook = 1;
    }
}

static void coop_transient_add_kosh(CoopTransientInput* input) {
    if (coop_kosh_success_epoch != epoch || !coop_kosh_success_key
            || coop_kosh_success_key > 4 || input->count >= COOP_TRANSIENT_RECORDS
            || coop_kosh_identities[coop_kosh_success_key - 1].map != (unsigned)current_map) return;
    input->records[input->count++] = (CoopTransientRecord){
        COOP_TRANSIENT_MINIGAME_SUCCESS, coop_kosh_success_key, 1, 0};
}

static void coop_transient_apply_kosh(CoopTransientRecord record) {
    if (!coop_kosh_hook || record.key < 1 || record.key > 4
            || record.state != 1 || record.value
            || coop_kosh_identities[record.key - 1].map != (unsigned)current_map
            || (coop_kosh_applied_epoch == epoch && coop_kosh_applied_key == record.key)) return;
    coop_kosh_pending_epoch = epoch;
    coop_kosh_pending_key = record.key;
}

static void coop_transient_add_minecart(CoopTransientInput* input) {
    if (coop_minecart_success_epoch != epoch || !coop_minecart_success_key
            || coop_minecart_success_key > 3 || input->count >= COOP_TRANSIENT_RECORDS
            || coop_minecart_identities[coop_minecart_success_key - 1].map
                != (unsigned)current_map) return;
    input->records[input->count++] = (CoopTransientRecord){
        COOP_TRANSIENT_MINECART_SUCCESS, coop_minecart_success_key, 1, 0};
}

static void coop_transient_apply_minecart(CoopTransientRecord record) {
    if (!coop_minecart_hook || record.key < 1 || record.key > 3
            || record.state != 1 || record.value
            || coop_minecart_identities[record.key - 1].map != (unsigned)current_map
            || (coop_minecart_applied_epoch == epoch
                && coop_minecart_applied_key == record.key)) return;
    coop_minecart_pending_epoch = epoch;
    coop_minecart_pending_key = record.key;
}

static void coop_transient_add_rabbit(CoopTransientInput* input) {
    if (coop_rabbit_success_epoch != epoch || !coop_rabbit_success_key
            || coop_rabbit_success_key > 2 || input->count >= COOP_TRANSIENT_RECORDS
            || (unsigned)current_map != MAP_FUNGI) return;
    input->records[input->count++] = (CoopTransientRecord){
        COOP_TRANSIENT_RABBIT_SUCCESS, coop_rabbit_success_key, 1, 0};
}

static void coop_transient_apply_rabbit(CoopTransientRecord record) {
    if (!coop_rabbit_hook || record.key < 1 || record.key > 2
            || record.state != 1 || record.value || (unsigned)current_map != MAP_FUNGI
            || coop_rabbit_round_key() != record.key
            || (coop_rabbit_applied_epoch == epoch
                && coop_rabbit_applied_key == record.key)) return;
    coop_rabbit_pending_epoch = epoch;
    coop_rabbit_pending_key = record.key;
}

static unsigned coop_transient_timer_sample_is_new(CoopTransientRecord record) {
    if (coop_transient_applied_timer_epoch != epoch) {
        coop_transient_applied_timer_epoch = epoch;
        for (unsigned i = 0; i < COOP_TRANSIENT_RECORDS; ++i)
            coop_transient_applied_timers[i] = (CoopTransientRecord){0};
    }
    unsigned free_slot = COOP_TRANSIENT_RECORDS;
    for (unsigned i = 0; i < COOP_TRANSIENT_RECORDS; ++i) {
        CoopTransientRecord* previous = &coop_transient_applied_timers[i];
        if (!previous->kind && free_slot == COOP_TRANSIENT_RECORDS) free_slot = i;
        if (previous->kind != record.kind || previous->key != record.key) continue;
        if (previous->state == record.state && previous->value == record.value) return 0;
        *previous = record; return 1;
    }
    if (free_slot == COOP_TRANSIENT_RECORDS) free_slot = record.key % COOP_TRANSIENT_RECORDS;
    coop_transient_applied_timers[free_slot] = record;
    return 1;
}

// The piano controller has 25 correct-note gates. A received count never
// selects a raw state: the adapter can only enter the next correct-hit state
// from its matching local wait state, preserving every intermediate block.
static const unsigned char coop_piano_wait_states[25] = {
    12, 14, 16, 19, 21, 23, 25, 28, 30, 32, 34, 36, 39,
    41, 43, 45, 47, 49, 52, 54, 56, 58, 60, 62, 64,
};
static const unsigned char coop_piano_hit_states[25] = {
    13, 15, 17, 20, 22, 24, 26, 29, 31, 33, 35, 37, 40,
    42, 44, 46, 48, 50, 53, 55, 57, 59, 61, 63, 65,
};

static unsigned coop_piano_progress(unsigned raw) {
    if (raw >= 250) return 0; // Vanilla failure/restart path is local.
    unsigned progress = 0;
    for (unsigned i = 0; i < 25; ++i)
        if (raw >= coop_piano_hit_states[i]) progress = i + 1;
    return progress;
}

static const unsigned char coop_dartboard_wait_states[6] = {15, 16, 17, 18, 19, 20};
static const unsigned char coop_dartboard_hit_states[6] = {50, 52, 54, 56, 58, 23};

static unsigned coop_dartboard_progress(unsigned raw) {
    if (raw == 23 || raw == 24) return 6;
    if (raw == 20 || raw == 58 || raw == 59) return 5;
    if (raw == 19 || raw == 56 || raw == 57) return 4;
    if (raw == 18 || raw == 54 || raw == 55) return 3;
    if (raw == 17 || raw == 52 || raw == 53) return 2;
    if (raw == 16 || raw == 50 || raw == 51) return 1;
    return 0;
}

static const unsigned char coop_tiny_igloo_wait_states[4] = {1, 4, 6, 8};
static const unsigned char coop_tiny_igloo_hit_states[4] = {2, 5, 7, 9};

static unsigned coop_tiny_igloo_progress(unsigned raw) {
    if (raw == 1) return 0;
    if (raw == 2 || raw == 3 || raw == 4 || raw == 70) return 1;
    if (raw == 5 || raw == 6) return 2;
    if (raw == 7 || raw == 8) return 3;
    if (raw == 9 || (raw >= 30 && raw <= 35)) return 4;
    return 0; // Vanilla failure/restart states remain local.
}

static unsigned coop_tiny_temple_kong_progress(void) {
    Prop_ScriptData* letter_c = coop_transient_script(0x0C);
    Prop_ScriptData* letter_d = coop_transient_script(0x0D);
    Prop_ScriptData* letter_e = coop_transient_script(0x0E);
    if (!letter_c || !letter_d || !letter_e) return 0;
    unsigned state_c = letter_c->unk48[0];
    if (state_c == 11 || (state_c >= 20 && state_c <= 22)) return 4;
    if (state_c == 10) return 3;
    if (letter_d->unk48[0] == 10) return 2;
    if (letter_e->unk48[0] == 10) return 1;
    return 0;
}

static unsigned coop_transient_object_activation(unsigned map, unsigned object) {
    for (unsigned i = 0; i < sizeof(coop_transient_extra_objects) / sizeof(coop_transient_extra_objects[0]); ++i) {
        const CoopTransientObject* entry = &coop_transient_extra_objects[i];
        if (entry->map == map && entry->object == object) return entry->activation;
    }
    return 0;
}

static unsigned coop_transient_object_kind(unsigned map, unsigned object) {
    for (unsigned i = 0; i < sizeof(coop_transient_extra_objects) / sizeof(coop_transient_extra_objects[0]); ++i) {
        const CoopTransientObject* entry = &coop_transient_extra_objects[i];
        if (entry->map == map && entry->object == object) return entry->kind;
    }
    for (unsigned i = 0; i < COOP_LIVE_WORLD_STATE_COUNT; ++i)
        if (coop_live_world_states[i].map == map && coop_live_world_states[i].object == object)
            return COOP_TRANSIENT_SCRIPT;
    return COOP_TRANSIENT_NONE;
}

static unsigned coop_transient_trigger_fired(unsigned map, unsigned object,
        unsigned raw, unsigned activation) {
    if (map == 4 && (object == 0x0A || object == 0x0B))
        return raw >= 2 && raw <= 5;
    if (map == 38 && object >= 0x0D && object <= 0x0F)
        return raw >= 2 && raw <= 6;
    return raw >= activation && raw < 20;
}

static unsigned coop_lobby_instrument_pad(unsigned map, unsigned object) {
    return (map == 173 && object <= 0x03)
        || (map == 174 && object >= 0x04 && object <= 0x08)
        || (map == 175 && object <= 0x04)
        || (map == 193 && object >= 0x02 && object <= 0x06)
        || (map == 194 && object >= 0x10 && object <= 0x14);
}

static unsigned coop_transient_trigger_ready(unsigned map, unsigned object,
        unsigned raw, unsigned activation) {
    if (coop_lobby_instrument_pad(map, object))
        return activation == 2 && raw == 1;
    // Matching heads must already be armed. Accepting state 10 would bypass
    // the vanilla initialization that enables contact and the sound actor.
    if (map == 20 && object >= 0x19 && object <= 0x28)
        return activation == 12 && raw == 11;
    if ((map == 26 && (object == 0x15 || object == 0x20 || object == 0x3C
                || object == 0x13A || object == 0x13C))
            || (map == 30 && object == 0x3F))
        return activation == 1 && raw == 0;
    if (map == 34 && (object == 0x31 || object == 0x33))
        return activation == 3 && raw == 2;
    if (map == 72 && object == 0x2F)
        return activation == 13 && raw == 12;
    if (map == 17 && object >= 0x2D && object <= 0x30)
        return activation == 12 && raw == 11;
    if (map == 164 && object == 0x09)
        return activation == 5 && raw == 1;
    if (map == 7 && (object == 0x1F || object == 0x20))
        return activation == 11 && raw == 1;
    if (map == 30 && object >= 0x2F && object <= 0x31)
        return activation == 13 && raw == 12;
    if (map == 38 && object >= 0x10 && object <= 0x14)
        return activation == 13 && raw == 12;
    if ((map == 57 && object == 0x03) || (map == 61 && object == 0x06)
            || (map == 62 && (object == 0x01 || object == 0x03)))
        return activation == 11 && raw == 10;
    return raw > 0 && raw < activation;
}

static Prop_ScriptData* coop_transient_script(unsigned object) {
    for (unsigned slot = 0; slot < COOP_LIVE_WORLD_SCRIPT_SLOTS; ++slot) {
        if ((unsigned short)D_global_asm_807F6240[slot] != object) continue;
        int prop = func_global_asm_80659470((int)object);
        if (prop >= 0 && D_global_asm_807F6000 && D_global_asm_807F6000[prop].unk7C)
            return D_global_asm_807F6000[prop].unk7C;
        return 0;
    }
    return 0;
}

static void coop_transient_add_object(unsigned object, unsigned kind,
        unsigned wanted_page, unsigned* ordinal, CoopTransientInput* input) {
    unsigned current = (*ordinal)++;
    if (current / COOP_TRANSIENT_RECORDS != wanted_page || input->count >= COOP_TRANSIENT_RECORDS)
        return;
    Prop_ScriptData* script = coop_transient_script(object);
    if (!script) return;
    unsigned state = script->unk48[0];
    if (state > 0xFF) return;
    unsigned value = 0;
    if (kind == COOP_TRANSIENT_TIMER) {
        value = (unsigned short)script->unk44[0];
    } else if (kind == COOP_TRANSIENT_TRIGGER) {
        value = coop_transient_object_activation(current_map, object);
        if (!value) return;
        state = coop_transient_trigger_fired(current_map, object, state, value) ? 2 : 1;
    } else if (kind == COOP_TRANSIENT_SEQUENCE) {
        if ((unsigned)current_map == 26 && object == 0x14)
            state = coop_piano_progress(state);
        else if ((unsigned)current_map == 26 && object == 0x7F)
            state = coop_dartboard_progress(state);
        else if ((unsigned)current_map == 16 && object == 0x0C)
            state = coop_tiny_temple_kong_progress();
        else if ((unsigned)current_map == 84 && object == 0x00)
            state = coop_tiny_igloo_progress(state);
        else return;
    }
    input->records[input->count++] = (CoopTransientRecord){kind, object, state, value};
}

static void coop_transient_add_clams(unsigned wanted_page, unsigned* ordinal,
        CoopTransientInput* input) {
    if ((unsigned)current_map != MAP_JAPES_SHELL || D_807FDC88.count <= 0
            || D_807FDC88.count > 256 || !D_807FDC88.first) return;
    for (unsigned i = 0; i < (unsigned)D_807FDC88.count; ++i) {
        Actor* actor = D_807FDC88.first[i].tied_actor;
        if (!actor || actor->unk58 != ACTOR_CLAM
                || !(actor->object_properties_bitfield & 0x10)
                || !actor->additional_actor_data || actor->control_state > 3) continue;
        unsigned current = (*ordinal)++;
        if (current / COOP_TRANSIENT_RECORDS != wanted_page
                || input->count >= COOP_TRANSIENT_RECORDS) continue;
        CoopClamData* data = (CoopClamData*)actor->additional_actor_data;
        unsigned timer = data->timer < 0 ? 0 : (unsigned)data->timer;
        if (timer > 90) timer = 90;
        input->records[input->count++] = (CoopTransientRecord){
            COOP_TRANSIENT_ACTOR_CYCLE, i + 1, actor->control_state, timer};
    }
}

static void coop_transient_apply_clam(CoopTransientRecord record) {
    if ((unsigned)current_map != MAP_JAPES_SHELL || !record.key
            || D_807FDC88.count <= 0 || record.key > (unsigned)D_807FDC88.count
            || D_807FDC88.count > 256 || !D_807FDC88.first) return;
    Actor* actor = D_807FDC88.first[record.key - 1].tied_actor;
    if (!actor || actor->unk58 != ACTOR_CLAM
            || !(actor->object_properties_bitfield & 0x10)
            || !actor->additional_actor_data || actor->control_state > 3) return;
    CoopClamData* data = (CoopClamData*)actor->additional_actor_data;
    unsigned local = actor->control_state;
    if (local == record.state) {
        // Timers exist only in the two stable phases. Animation phases retain
        // their local callbacks and progress and are never frame-skipped.
        if (local == 0 || local == 2) data->timer = (s16)record.value;
        return;
    }
    if (local == 0 && (record.state == 1 || record.state == 2)) {
        // Reproduce the stock closed-to-opening edge. The original handler
        // remains responsible for animation completion and the open timer.
        playActorAnimation(actor, 0x35D);
        func_global_asm_80614D00(actor, 0.5f, 0.0f);
        actor->control_state = 1;
        actor->control_state_progress = 0;
    } else if (local == 2 && (record.state == 3 || record.state == 0)) {
        // Reproduce the stock open-to-closing edge, including collision mode.
        actor->unk132 = 2;
        func_global_asm_80614D00(actor, 0.5f, 0.0f);
        actor->control_state = 3;
        actor->control_state_progress = 0;
    }
}

static Actor* coop_transient_ice_tomato(void) {
    if ((unsigned)current_map != MAP_CAVES_ICE_CASTLE) return 0;
    for (unsigned i = 0; i < D_global_asm_807FBB34; ++i) {
        Actor* actor = D_global_asm_807FB930[i].actor;
        if (actor && actor->unk58 == ACTOR_TOMATO_ICE
                && (actor->object_properties_bitfield & 0x10) && actor->unk178)
            return actor;
    }
    return 0;
}

static Actor* coop_transient_tomato_clock(Actor* tomato) {
    Actor* timer = tomato ? tomato->unk11C : 0;
    if (!timer || timer->unk58 != ACTOR_TIMER_CONTROLLER
            || !timer->additional_actor_data || timer->control_state < 2
            || timer->control_state > 5) return 0;
    return timer;
}

static unsigned coop_transient_tomato_board_value(unsigned* valid) {
    unsigned value = 0;
    *valid = 1;
    for (unsigned i = 0; i < 16; ++i) {
        int cell = D_global_asm_807FC8C0[i];
        if (cell < -1 || cell > 1) { *valid = 0; return 0; }
        value |= (unsigned)(cell + 1) << (i * 2);
    }
    return value;
}

static void coop_transient_add_tomato_board(unsigned wanted_page, unsigned* ordinal,
        CoopTransientInput* input) {
    if ((unsigned)current_map != MAP_CAVES_ICE_CASTLE) return;
    unsigned current = (*ordinal)++;
    if (current / COOP_TRANSIENT_RECORDS != wanted_page
            || input->count >= COOP_TRANSIENT_RECORDS) return;
    Actor* actor = coop_transient_ice_tomato();
    CoopIceTomatoData* data = actor ? (CoopIceTomatoData*)actor->unk178 : 0;
    if (!data) return;
    unsigned active = (data->state == 3 || data->state == 4)
        && isFlagSet(TEMPFLAG_ICE_TOMATO_BOARD_ACTIVE, FLAG_TYPE_TEMPORARY);
    unsigned finished = data->state == 6 || data->state == 7;
    if (!active && !finished) return;
    unsigned valid = 0, value = coop_transient_tomato_board_value(&valid);
    if (valid) input->records[input->count++] = (CoopTransientRecord){
        COOP_TRANSIENT_TOMATO_BOARD, 0, finished ? 2u : 1u, value};
}

static void coop_transient_apply_tomato_board(CoopTransientRecord record) {
    if ((unsigned)current_map != MAP_CAVES_ICE_CASTLE || record.key
            || (record.state != 1 && record.state != 2)) return;
    Actor* actor = coop_transient_ice_tomato();
    CoopIceTomatoData* data = actor ? (CoopIceTomatoData*)actor->unk178 : 0;
    if (!data || (data->state != 3 && data->state != 4)
            || !isFlagSet(TEMPFLAG_ICE_TOMATO_BOARD_ACTIVE, FLAG_TYPE_TEMPORARY)) return;
    signed char wanted[16];
    for (unsigned i = 0; i < 16; ++i) {
        unsigned cell = (record.value >> (i * 2)) & 3u;
        if (cell == 3u) return;
        wanted[i] = (signed char)cell - 1;
    }
    for (unsigned i = 0; i < 16; ++i) if (D_global_asm_807FC8C0[i] != wanted[i]) {
        D_global_asm_807FC8C0[i] = wanted[i];
        if (wanted[i] < 0) func_global_asm_8063DA40((s16)i, 0);
        else func_global_asm_8063DA78((s16)i, 1, 1);
    }
}

static void coop_transient_add_tomato_clock(unsigned wanted_page, unsigned* ordinal,
        CoopTransientInput* input) {
    if ((unsigned)current_map != MAP_CAVES_ICE_CASTLE) return;
    unsigned current = (*ordinal)++;
    if (current / COOP_TRANSIENT_RECORDS != wanted_page
            || input->count >= COOP_TRANSIENT_RECORDS) return;
    Actor* tomato = coop_transient_ice_tomato();
    CoopIceTomatoData* tomato_data = tomato ? (CoopIceTomatoData*)tomato->unk178 : 0;
    if (!tomato_data) return;
    if (tomato_data->state == 6 || tomato_data->state == 7) {
        input->records[input->count++] = (CoopTransientRecord){
            COOP_TRANSIENT_TOMATO_CLOCK, 0, 2, 0};
        return;
    }
    if ((tomato_data->state != 3 && tomato_data->state != 4)
            || !isFlagSet(TEMPFLAG_ICE_TOMATO_BOARD_ACTIVE, FLAG_TYPE_TEMPORARY)) return;
    Actor* timer = coop_transient_tomato_clock(tomato);
    if (!timer) return;
    CoopCountdownData* timer_data = (CoopCountdownData*)timer->additional_actor_data;
    if (timer_data->duration < 0 || timer_data->duration > 60
            || timer_data->elapsed > (unsigned)timer_data->duration) return;
    unsigned remaining = (unsigned)timer_data->duration - timer_data->elapsed;
    input->records[input->count++] = (CoopTransientRecord){
        COOP_TRANSIENT_TOMATO_CLOCK, 0, timer->control_state == 5 ? 2u : 1u,
        timer->control_state == 5 ? 0u : remaining};
}

static void coop_transient_apply_tomato_clock(CoopTransientRecord record) {
    if ((unsigned)current_map != MAP_CAVES_ICE_CASTLE || record.key
            || (record.state != 1 && record.state != 2)
            || (record.state == 1 && record.value > 60)
            || (record.state == 2 && record.value)) return;
    Actor* tomato = coop_transient_ice_tomato();
    CoopIceTomatoData* tomato_data = tomato ? (CoopIceTomatoData*)tomato->unk178 : 0;
    if (!tomato_data || (tomato_data->state != 3 && tomato_data->state != 4)
            || !isFlagSet(TEMPFLAG_ICE_TOMATO_BOARD_ACTIVE, FLAG_TYPE_TEMPORARY)) return;
    Actor* timer = coop_transient_tomato_clock(tomato);
    if (!timer || !coop_transient_timer_sample_is_new(record)) return;
    CoopCountdownData* timer_data = (CoopCountdownData*)timer->additional_actor_data;
    if (record.state == 2) {
        timer->control_state = 5;
        timer->control_state_progress = 0;
        return;
    }
    if (timer->control_state != 2 && timer->control_state != 4) return;
    if (timer_data->elapsed > 60 || timer_data->elapsed + record.value > 60) return;
    timer_data->duration = (int)(timer_data->elapsed + record.value);
}

static void coop_transient_capture(unsigned present) {
    transient_input = (CoopTransientInput){0};
    if (coop_transient_cutscene_epoch != epoch) {
        coop_transient_cutscene_epoch = epoch;
        coop_transient_cutscene_hold = 0;
        coop_transient_last_cutscene = (CoopTransientRecord){0};
    }
    if (transient_enabled && present && current_file < 3) {
        if (!transient_file) transient_file = current_file + 1;
        else if (transient_file != current_file + 1) transient_file_changed = 1;
    }
    if (!transient_enabled || !present || current_file >= 3 || (unsigned)current_map > 255
            || transient_file_changed || loading_zone_transition_speed != 0.0f) return;
    transient_input.enabled = 1; transient_input.file = current_file;
    transient_input.map = current_map; transient_input.epoch = epoch;
    transient_input.revision = ++transient_revision;
    if (!transient_input.revision) transient_input.revision = ++transient_revision;
    unsigned ordinal = 0;
    for (unsigned row = 0; row < COOP_LIVE_WORLD_STATE_COUNT; ++row) {
        const CoopLiveWorldState* state = &coop_live_world_states[row];
        if (state->map != (unsigned)current_map) continue;
        unsigned duplicate = 0;
        for (unsigned earlier = 0; earlier < row; ++earlier)
            duplicate |= coop_live_world_states[earlier].map == state->map
                && coop_live_world_states[earlier].object == state->object;
        if (!duplicate && coop_transient_object_kind(state->map, state->object) == COOP_TRANSIENT_SCRIPT)
            coop_transient_add_object(state->object, COOP_TRANSIENT_SCRIPT,
                transient_page, &ordinal, &transient_input);
    }
    for (unsigned i = 0; i < sizeof(coop_transient_extra_objects) / sizeof(coop_transient_extra_objects[0]); ++i) {
        const CoopTransientObject* entry = &coop_transient_extra_objects[i];
        if (entry->map == (unsigned)current_map)
            coop_transient_add_object(entry->object, entry->kind,
                transient_page, &ordinal, &transient_input);
    }
    coop_transient_add_clams(transient_page, &ordinal, &transient_input);
    coop_transient_add_tomato_board(transient_page, &ordinal, &transient_input);
    coop_transient_add_tomato_clock(transient_page, &ordinal, &transient_input);
    coop_transient_add_kosh(&transient_input);
    coop_transient_add_minecart(&transient_input);
    coop_transient_add_rabbit(&transient_input);
    // Retain the last same-epoch target briefly after the host finishes so a
    // lagging copy can consume missed phases. Apply still requires the Join to
    // be running the exact same cutscene and flags, so this cannot launch one.
    CoopTransientRecord cutscene = {0};
    if (is_cutscene_active == 1 && D_global_asm_807476F8 >= 0 && D_global_asm_807476F8 < 0xFF
            && D_global_asm_807F5CF0 <= 0xFF) {
        cutscene = (CoopTransientRecord){
            COOP_TRANSIENT_CUTSCENE, (unsigned)D_global_asm_807476F8 + 1,
            (unsigned)D_global_asm_807F5CF0, (unsigned)D_global_asm_807F5CF4 & 0xFF};
        coop_transient_last_cutscene = cutscene;
        coop_transient_cutscene_hold = COOP_TRANSIENT_CUTSCENE_HOLD_FRAMES;
    } else if (coop_transient_cutscene_hold && coop_transient_last_cutscene.kind) {
        cutscene = coop_transient_last_cutscene;
        coop_transient_cutscene_hold--;
    }
    if (cutscene.kind) {
        if (transient_input.count < COOP_TRANSIENT_RECORDS)
            transient_input.records[transient_input.count++] = cutscene;
        else
            transient_input.records[COOP_TRANSIENT_RECORDS - 1] = cutscene;
    }
    unsigned pages = (ordinal + COOP_TRANSIENT_RECORDS - 1) / COOP_TRANSIENT_RECORDS;
    transient_page = pages ? (transient_page + 1) % pages : 0;
}

static void coop_transient_apply(void) {
    if (transient_result.status == COOP_TRANSIENT_SYNCED
            && transient_result.map == (unsigned)current_map
            && transient_result.epoch == epoch) {
        coop_lobby_pad_applied_epoch = 0;
        coop_lobby_pad_applied_map = coop_lobby_pad_applied_object = 0;
    }
    if (!transient_enabled || transient_file_changed || role == ROLE_OFF
            || transient_result.status != COOP_TRANSIENT_APPLYING
            || transient_result.map != (unsigned)current_map || transient_result.epoch != epoch
            || loading_zone_transition_speed != 0.0f) return;
    for (unsigned i = 0; i < transient_result.count && i < COOP_TRANSIENT_RECORDS; ++i) {
        CoopTransientRecord record = transient_result.records[i];
        if (record.kind == COOP_TRANSIENT_MINIGAME_SUCCESS) {
            coop_transient_apply_kosh(record);
            continue;
        }
        if (record.kind == COOP_TRANSIENT_MINECART_SUCCESS) {
            coop_transient_apply_minecart(record);
            continue;
        }
        if (record.kind == COOP_TRANSIENT_RABBIT_SUCCESS) {
            coop_transient_apply_rabbit(record);
            continue;
        }
        if (role != ROLE_JOIN) continue;
        if (record.kind == COOP_TRANSIENT_ACTOR_CYCLE) {
            coop_transient_apply_clam(record);
            continue;
        }
        if (record.kind == COOP_TRANSIENT_TOMATO_BOARD) {
            coop_transient_apply_tomato_board(record);
            continue;
        }
        if (record.kind == COOP_TRANSIENT_TOMATO_CLOCK) {
            coop_transient_apply_tomato_clock(record);
            continue;
        }
        if (record.kind == COOP_TRANSIENT_CUTSCENE) {
            unsigned local_id = D_global_asm_807476F8 >= 0 ? (unsigned)D_global_asm_807476F8 + 1 : 0;
            if (is_cutscene_active == 1 && record.key == local_id
                    && record.value == ((unsigned)D_global_asm_807F5CF4 & 0xFF)
                    && record.state > (unsigned)D_global_asm_807F5CF0)
                // A 20 Hz sample can miss a short camera phase. Catch up one
                // phase per rendered frame so vanilla observes every phase;
                // never jump directly to the sampled state or rewind.
                D_global_asm_807F5CF0++;
            continue;
        }
        unsigned kind = coop_transient_object_kind(current_map, record.key);
        if (kind != record.kind || record.state > 0xFF) continue;
        Prop_ScriptData* script = coop_transient_script(record.key);
        if (!script) continue;
        if (kind == COOP_TRANSIENT_TRIGGER) {
            // Never copy a later timer/presentation state or rewind a local
            // action. The only remote command is the reviewed vanilla entry.
            unsigned activation = coop_transient_object_activation(current_map, record.key);
            unsigned lobby_pad = coop_lobby_instrument_pad(current_map, record.key);
            if (lobby_pad && coop_lobby_pad_applied_epoch == epoch
                    && coop_lobby_pad_applied_map == (unsigned)current_map
                    && coop_lobby_pad_applied_object == record.key) continue;
            if (record.value == activation && record.state == 2
                    && coop_transient_trigger_ready(current_map, record.key,
                        script->unk48[0], activation)) {
                if ((unsigned)current_map == 194 && record.key >= 0x10
                        && record.key <= 0x14
                        && !isFlagSet(0x19D, FLAG_TYPE_PERMANENT)) continue;
                if ((unsigned)current_map == 90 && record.key >= 0x03 && record.key <= 0x05) {
                    Prop_ScriptData* controller = coop_transient_script(0x06);
                    if (!controller || controller->unk48[0] < 1 || controller->unk48[0] >= 4)
                        continue;
                    // Vanilla function 28 adds one to controller state index 0
                    // before the target enters state 2.
                    controller->unk48[0] += 1;
                }
                if ((unsigned)current_map == 4 && (record.key == 0x0A || record.key == 0x0B)) {
                    unsigned linked = record.key == 0x0A ? 0x0B : 0x0A;
                    Prop_ScriptData* partner = coop_transient_script(linked);
                    if (!partner || partner->unk48[0] != 1) continue;
                    // The stock hit block puts the opposite switch into its
                    // linked state before entering this switch's state 2.
                    coop_live_world_set_object(linked, 10);
                }
                coop_live_world_set_object(record.key, activation);
                if (lobby_pad) {
                    coop_lobby_pad_applied_epoch = epoch;
                    coop_lobby_pad_applied_map = current_map;
                    coop_lobby_pad_applied_object = record.key;
                }
            }
            continue;
        }
        if (kind == COOP_TRANSIENT_SEQUENCE) {
            if (record.value) continue;
            if ((unsigned)current_map == 16 && record.key == 0x0C) {
                static const unsigned char letters[4] = {0x0F, 0x0E, 0x0D, 0x0C};
                unsigned progress = coop_tiny_temple_kong_progress();
                if (record.state <= 4 && record.state > progress && progress < 4) {
                    Prop_ScriptData* letter = coop_transient_script(letters[progress]);
                    if (letter && letter->unk48[0] == 10)
                        coop_live_world_set_object(letters[progress], 11);
                }
                continue;
            }
            if ((unsigned)current_map == 84 && record.key == 0x00) {
                unsigned progress = coop_tiny_igloo_progress(script->unk48[0]);
                if (record.state <= 4 && record.state > progress && progress < 4
                        && script->unk48[0] == coop_tiny_igloo_wait_states[progress])
                    coop_live_world_set_object(record.key, coop_tiny_igloo_hit_states[progress]);
                continue;
            }
            if ((unsigned)current_map != 26) continue;
            unsigned progress, count;
            const unsigned char* waits;
            const unsigned char* hits;
            if (record.key == 0x14) {
                progress = coop_piano_progress(script->unk48[0]); count = 25;
                waits = coop_piano_wait_states; hits = coop_piano_hit_states;
            } else if (record.key == 0x7F) {
                progress = coop_dartboard_progress(script->unk48[0]); count = 6;
                waits = coop_dartboard_wait_states; hits = coop_dartboard_hit_states;
            } else continue;
            if (record.state <= count && record.state > progress && progress < count
                    && script->unk48[0] == waits[progress])
                coop_live_world_set_object(record.key, hits[progress]);
            continue;
        }
        if (kind == COOP_TRANSIENT_TIMER) {
            if (!coop_transient_timer_sample_is_new(record)) continue;
            if (script->unk48[0] != record.state)
                coop_live_world_set_object(record.key, record.state);
            script->unk44[0] = (short)record.value;
            continue;
        }
        if (script->unk48[0] == record.state) continue;
        coop_live_world_set_object(record.key, record.state);
    }
}

#endif
