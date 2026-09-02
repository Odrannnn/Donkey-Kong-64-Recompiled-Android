#ifndef DKCOOP_TRANSIENT_GAME_H
#define DKCOOP_TRANSIENT_GAME_H

typedef struct {
    unsigned short map, object;
    unsigned char kind, activation;
} CoopTransientObject;

// Pinned loaded scripts whose state drives a reviewed reversible switch,
// timer-driven presentation, or linked platform. Permanent gate/door readers
// come from coop_live_world_states below.
static const CoopTransientObject coop_transient_extra_objects[] = {
    {30, 0, COOP_TRANSIENT_TIMER, 0}, {30, 1, COOP_TRANSIENT_TIMER, 0},
    {48, 4, COOP_TRANSIENT_TIMER, 0}, {48, 5, COOP_TRANSIENT_TIMER, 0},
    {194, 6, COOP_TRANSIENT_PLATFORM, 0},
    // Factory production switches: Chunky, Tiny, Lanky and Diddy. Their
    // vanilla state-2 entry owns the timer/reward sequence locally.
    {26, 0x2E, COOP_TRANSIENT_TRIGGER, 2}, {26, 0x2F, COOP_TRANSIENT_TRIGGER, 2},
    {26, 0x30, COOP_TRANSIENT_TRIGGER, 2}, {26, 0x31, COOP_TRANSIENT_TRIGGER, 2},
    // Diddy's 3-1-2-4 switch room. Each local script owns its long cutscene,
    // switch hiding, enemy spawn and final reward after state 5 is entered.
    {26, 0x3F, COOP_TRANSIENT_TRIGGER, 5}, {26, 0x40, COOP_TRANSIENT_TRIGGER, 5},
    {26, 0x41, COOP_TRANSIENT_TRIGGER, 5},
};

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
    if (kind == COOP_TRANSIENT_TRIGGER) {
        value = coop_transient_object_activation(current_map, object);
        if (value < 2) return;
        state = state >= value && state < 20 ? 2 : 1;
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
        if (!duplicate) coop_transient_add_object(state->object, COOP_TRANSIENT_SCRIPT,
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
                    && script->unk48[0] > 0 && script->unk48[0] < activation)
                coop_live_world_set_object(record.key, activation);
            continue;
        }
        if (script->unk48[0] == record.state) continue;
        coop_live_world_set_object(record.key, record.state);
    }
}

#endif
