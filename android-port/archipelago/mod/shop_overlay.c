// SPDX-License-Identifier: GPL-3.0-or-later
#include "common_structs.h"
#include "modding.h"
#include "ap_shops.h"
#include "PR/gu.h"

extern volatile u32 ap_gameplay_mode;
extern u32 current_map;
extern Actor* gCurrentActorPointer;
extern Actor* getSpawnerTiedActor(s16 trigger, u16 unused);
extern void ap_stock_func_menu_80027988(void);
extern void deleteActor(Actor* actor), renderActor(Actor* actor, u8 unused);
extern void addActorToTextOverlayRenderArray(void* callback, Actor* actor, u8 layer);
extern Gfx* printStyledText(Gfx* dl, s16 font, s16 x, s16 y, u8* text, u32 flags);

// Stock purchase overlay PAAD; never extend the game's allocation.
typedef struct {
    const char *upper, *lower;
    u8 opacity, unused9[7];
    Mtx matrix[2];
    s32 timer;
    Actor* owner;
} overlay_paad;
_Static_assert(sizeof(overlay_paad) == 0x98, "Stock overlay PAAD size");
#define OVERLAY_SLOTS 8
typedef struct {
    Actor* actor;
    overlay_paad* paad;
    u32 map;
    unsigned int generation;
    ap_shop_label label;
} overlay_slot;
static overlay_slot slots[OVERLAY_SLOTS];

static overlay_slot* find(Actor* actor) {
    for (int i = 0; i < OVERLAY_SLOTS; ++i) if (slots[i].actor == actor && actor) return &slots[i];
    return 0;
}
static void dismiss(Actor* actor, overlay_paad* paad, overlay_slot* slot) {
    if (paad) { paad->upper = paad->lower = 0; paad->timer = 0; paad->opacity = 0; }
    if (slot) slot->actor = 0;
    deleteActor(actor);
}
Gfx* ap_shop_overlay_draw(Gfx* dl, Actor* actor) {
    overlay_slot* slot = find(actor);
    if (!slot || !(ap_gameplay_mode & AP_GAMEPLAY_SHOPS) || slot->map != current_map
        || slot->generation != ap_apshop_generation() || slot->paad != actor->additional_actor_data) return dl;
    overlay_paad* paad = slot->paad;
    gSPDisplayList(dl++, 0x01000118);
    gSPMatrix(dl++, 0x02000180, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPPipeSync(dl++);
    gDPSetCombineLERP(dl++, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(dl++, 0, 0, 255, 255, 255, paad->opacity);
    if (slot->label.item[0]) {
        gSPMatrix(dl++, &paad->matrix[0], G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        dl = printStyledText(dl, 1, 0, 0, (u8*)slot->label.item, 0x80);
        gSPPopMatrix(dl++, G_MTX_MODELVIEW);
    }
    if (slot->label.subtitle[0]) {
        gSPMatrix(dl++, &paad->matrix[1], G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        dl = printStyledText(dl, 6, 0, 0, (u8*)slot->label.subtitle, 0x80);
        gSPPopMatrix(dl++, G_MTX_MODELVIEW);
    }
    return dl;
}

RECOMP_PATCH void func_menu_80027988(void) {
    Actor* actor = gCurrentActorPointer;
    if (!actor) return;
    overlay_paad* paad = actor->additional_actor_data;
    overlay_slot* slot = find(actor);
    // The engine may reuse an actor address. Its new initialization/allocation
    // must never inherit the prior actor's text or lifetime.
    if (slot && (ACTOR_UNINITIALIZED(actor) || slot->paad != paad)) { slot->actor = 0; slot = 0; }
    if (slot && (!(ap_gameplay_mode & AP_GAMEPLAY_SHOPS) || slot->map != current_map
        || slot->generation != ap_apshop_generation())) { dismiss(actor, paad, slot); return; }
    if (!paad || (actor->unk58 != 0x140 && actor->unk58 != 0x144)) {
        if (slot) slot->actor = 0;
        ap_stock_func_menu_80027988(); return;
    }
    if (!slot) {
        Actor* owner = paad->owner;
        if (!owner) { owner = getSpawnerTiedActor(1, 0); paad->owner = owner; }
        ap_shop_paad* shop = owner ? owner->unk178 : 0;
        if (!shop) return; // stock waits for a shop owner too; do not look it up twice
        if (shop->item_type != AP_SHOP_ITEM_TYPE) { ap_stock_func_menu_80027988(); return; }
        ap_shop_label label;
        // No success overlay for a rejected purchase. Never send AP type 21
        // into the stock function's five-entry item-name lookup.
        // A queued actor can reach its first update after the gate turns off.
        // It still contains AP type 21 and cannot safely fall back to stock.
        if (!(ap_gameplay_mode & AP_GAMEPLAY_SHOPS) || !ACTOR_UNINITIALIZED(actor) || !ap_apshop_display(owner, shop, &label)) {
            dismiss(actor, paad, 0); return;
        }
        unsigned int generation = ap_apshop_generation();
        int occupied = 0;
        for (int i = 0; i < OVERLAY_SLOTS; ++i) {
            if (slots[i].actor && (slots[i].map != current_map || slots[i].generation != generation)) slots[i].actor = 0;
            if (slots[i].actor) ++occupied;
            else if (!slot) slot = &slots[i];
        }
        if (!slot) { dismiss(actor, paad, 0); return; } // bounded pool; never overwrite an active message
        slot->actor = actor; slot->paad = paad; slot->map = current_map; slot->generation = generation;
        slot->label = label; // frozen copy; late scout updates cannot change this overlay
        paad->owner = owner; paad->upper = slot->label.item; paad->lower = slot->label.subtitle;
        paad->opacity = 0; paad->timer = label.frames; actor->control_state = 0;
        f32 scale[4][4], translation[4][4];
        guScaleF(scale, 0.6f, 0.6f, 1.0f);
        guTranslateF(translation, 640.0f, 800.0f - 100.0f * occupied, 0.0f);
        guMtxCatF(scale, translation, scale); guMtxF2L(scale, &paad->matrix[0]);
        guTranslateF(translation, 0.0f, 48.0f, 0.0f);
        guMtxCatF(scale, translation, scale); guMtxF2L(scale, &paad->matrix[1]);
    }
    // Original AP timing, including its short-duration fade adjustment.
    int duration = slot->label.frames;
    int fade_out = duration < 70 ? (duration - 30) / 2 : 30;
    int fade_rate = duration < 70 ? 256 / (fade_out - 4) : 16;
    if (paad->timer <= 1) { dismiss(actor, paad, slot); return; }
    --paad->timer;
    if (paad->timer == duration - 2) actor->control_state = 1;
    else if (paad->timer == fade_out) actor->control_state = 2;
    int opacity = paad->opacity;
    if (actor->control_state == 1) opacity += fade_rate;
    else if (actor->control_state == 2) opacity -= fade_rate;
    paad->opacity = opacity < 0 ? 0 : opacity > 255 ? 255 : opacity;
    if (actor->control_state) addActorToTextOverlayRenderArray(ap_shop_overlay_draw, actor, 3);
    if (actor->unk58 == 0x140) renderActor(actor, 0);
}
