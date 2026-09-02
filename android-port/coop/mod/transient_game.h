#ifndef DKCOOP_TRANSIENT_GAME_H
#define DKCOOP_TRANSIENT_GAME_H

typedef struct {
    unsigned short map, object;
    unsigned char kind, activation;
} CoopTransientObject;

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
    // Llama Temple coconut switch and DK bongo pad. Their local state-2
    // sequences own the cooling/llama-spit flags and linked gate presentation.
    {20, 0x12, COOP_TRANSIENT_TRIGGER, 2}, {20, 0x16, COOP_TRANSIENT_TRIGGER, 2},
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
    // Isles trombone pad. Its local barrel dependency moves it to state 2;
    // only then may the reviewed instrument activation enter state 3.
    {34, 0x31, COOP_TRANSIENT_TRIGGER, 3},
    {48, 4, COOP_TRANSIENT_TIMER, 0}, {48, 5, COOP_TRANSIENT_TIMER, 0},
    // Fungi green-tunnel feather/pineapple pairs and yellow-tunnel grape
    // switch. Each state-2 block owns its linked gate and completion locally.
    {48, 0x18, COOP_TRANSIENT_TRIGGER, 2}, {48, 0x19, COOP_TRANSIENT_TRIGGER, 2},
    {48, 0x1A, COOP_TRANSIENT_TRIGGER, 2}, {48, 0x1B, COOP_TRANSIENT_TRIGGER, 2},
    {48, 0x1E, COOP_TRANSIENT_TRIGGER, 2},
    // Isles level-lobby feather switches. State 2 starts the local panel/door
    // presentation in the Aztec and Fungi lobbies respectively.
    {173, 0x10, COOP_TRANSIENT_TRIGGER, 2}, {178, 0x05, COOP_TRANSIENT_TRIGGER, 2},
    // Caves small/large boulder pads. The small pad accepts state 2 from ready
    // state 1; the large pad must finish its local reveal at state 12 first.
    {72, 0x2E, COOP_TRANSIENT_TRIGGER, 2}, {72, 0x2F, COOP_TRANSIENT_TRIGGER, 13},
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
    if (map == 38 && object >= 0x0D && object <= 0x0F)
        return raw >= 2 && raw <= 6;
    return raw >= activation && raw < 20;
}

static unsigned coop_transient_trigger_ready(unsigned map, unsigned object,
        unsigned raw, unsigned activation) {
    // Matching heads must already be armed. Accepting state 10 would bypass
    // the vanilla initialization that enables contact and the sound actor.
    if (map == 20 && object >= 0x19 && object <= 0x28)
        return activation == 12 && raw == 11;
    if (map == 34 && object == 0x31)
        return activation == 3 && raw == 2;
    if (map == 72 && object == 0x2F)
        return activation == 13 && raw == 12;
    if (map == 164 && object == 0x09)
        return activation == 5 && raw == 1;
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
        if (value < 2) return;
        state = coop_transient_trigger_fired(current_map, object, state, value) ? 2 : 1;
    } else if (kind == COOP_TRANSIENT_SEQUENCE) {
        if ((unsigned)current_map == 26 && object == 0x14)
            state = coop_piano_progress(state);
        else if ((unsigned)current_map == 26 && object == 0x7F)
            state = coop_dartboard_progress(state);
        else if ((unsigned)current_map == 16 && object == 0x0C)
            state = coop_tiny_temple_kong_progress();
        else return;
    }
    input->records[input->count++] = (CoopTransientRecord){kind, object, state, value};
}

static void coop_transient_capture(unsigned present) {
    transient_input = (CoopTransientInput){0};
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
    // Cutscene records only align two already-running copies. Instance scripts
    // start their own local cutscene, so packets cannot launch rewards/endings.
    if (is_cutscene_active == 1 && D_global_asm_807476F8 >= 0 && D_global_asm_807476F8 < 0xFF
            && D_global_asm_807F5CF0 <= 0xFF
            && transient_input.count < COOP_TRANSIENT_RECORDS) {
        transient_input.records[transient_input.count++] = (CoopTransientRecord){
            COOP_TRANSIENT_CUTSCENE, (unsigned)D_global_asm_807476F8 + 1,
            (unsigned)D_global_asm_807F5CF0, (unsigned)D_global_asm_807F5CF4 & 0xFF};
    }
    unsigned pages = (ordinal + COOP_TRANSIENT_RECORDS - 1) / COOP_TRANSIENT_RECORDS;
    transient_page = pages ? (transient_page + 1) % pages : 0;
}

static void coop_transient_apply(void) {
    if (!transient_enabled || transient_file_changed || role != ROLE_JOIN
            || transient_result.status != COOP_TRANSIENT_APPLYING
            || transient_result.map != (unsigned)current_map || transient_result.epoch != epoch
            || loading_zone_transition_speed != 0.0f) return;
    for (unsigned i = 0; i < transient_result.count && i < COOP_TRANSIENT_RECORDS; ++i) {
        CoopTransientRecord record = transient_result.records[i];
        if (record.kind == COOP_TRANSIENT_CUTSCENE) {
            unsigned local_id = D_global_asm_807476F8 >= 0 ? (unsigned)D_global_asm_807476F8 + 1 : 0;
            if (is_cutscene_active == 1 && record.key == local_id
                    && record.value == ((unsigned)D_global_asm_807F5CF4 & 0xFF)
                    && record.state == (unsigned)D_global_asm_807F5CF0 + 1)
                D_global_asm_807F5CF0 = record.state;
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
            if (record.value == activation && record.state == 2
                    && coop_transient_trigger_ready(current_map, record.key,
                        script->unk48[0], activation))
                coop_live_world_set_object(record.key, activation);
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
