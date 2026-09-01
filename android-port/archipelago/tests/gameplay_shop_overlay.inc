#define LABELS 0x80910000u
#define LABEL_RESULT 0x80914000u
#define OVERLAY_ACTOR(i) (0x80918000u + 0x200u*(i))
#define OVERLAY_PAAD(i) (0x8091C000u + 0x200u*(i))
#define OVERLAY_GFX 0x80922000u
static unsigned overlay_deleted, overlay_registered, overlay_rendered, overlay_prints, overlay_expected_actor, overlay_stock;
static char overlay_lines[2][33];
static void memory_string(unsigned at, const char* text, unsigned capacity) {
    unsigned i = 0;
    while (text[i] && i + 1 < capacity) { put8(at+i, text[i]); ++i; }
    while (i < capacity) put8(at+i++, 0);
}
static void label_at(unsigned at, unsigned location, unsigned frames, const char* title, const char* subtitle) {
    unsigned p = LABELS + at*76;
    for (unsigned i = 0; i < 76; ++i) put8(p+i, 0);
    put32(p, location); put16(p+4, frames);
    memory_string(p+8, title, 33); memory_string(p+41, subtitle, 33);
}
static void expect_string(unsigned at, const char* text) {
    unsigned i = 0; for (; text[i]; ++i) CHECK(get8(at+i) == (unsigned char)text[i]); CHECK(!get8(at+i));
}
static void overlay_delete(uint8_t* rdram, recomp_context* ctx) { (void)rdram; CHECK((unsigned)ctx->r4 == overlay_expected_actor); ++overlay_deleted; }
static void overlay_render(uint8_t* rdram, recomp_context* ctx) { (void)rdram; CHECK((unsigned)ctx->r4 == overlay_expected_actor && ctx->r5 == 0); ++overlay_rendered; }
static void overlay_register(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram; CHECK((unsigned)ctx->r4 == (overlay_stock ? 0x80027808u : TEST_OVERLAY_DRAW));
    CHECK((unsigned)ctx->r5 == overlay_expected_actor && ctx->r6 == 3); ++overlay_registered;
}
static void overlay_print(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram; CHECK(overlay_prints < 2); CHECK(ctx->r5 == (overlay_prints ? 6 : 1) && ctx->r6 == 0 && ctx->r7 == 0);
    CHECK(get32((unsigned)ctx->r29+0x14) == 0x80);
    unsigned text = get32((unsigned)ctx->r29+0x10), i = 0;
    while (i < 32 && get8(text+i)) { overlay_lines[overlay_prints][i] = get8(text+i); ++i; }
    CHECK(!get8(text+i)); overlay_lines[overlay_prints++][i] = 0; ctx->r2 = ctx->r4;
}
static void overlay_spawner(uint8_t* rdram, recomp_context* ctx) { (void)rdram; CHECK(ctx->r4 == 1 && ctx->r5 == 0); ctx->r2 = (int32_t)ACTOR; }
static void stock_overlay_text(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram; CHECK(ctx->r4 == 39 && ctx->r5 < 64 && ctx->r6 == 0); ctx->r2 = (int32_t)LABEL_RESULT;
}
static void overlay_services(void) {
    guScaleF = flow_guScaleF; guTranslateF = flow_guTranslateF; guMtxCatF = flow_guMtxCatF; guMtxF2L = flow_guMtxF2L;
    deleteActor = overlay_delete; renderActor = overlay_render; addActorToTextOverlayRenderArray = overlay_register;
    printStyledText = overlay_print; getSpawnerTiedActor = overlay_spawner; getTextString = stock_overlay_text;
    func_global_asm_806FD894 = zero;
    overlay_deleted = overlay_registered = overlay_rendered = overlay_prints = overlay_stock = 0;
}
static void overlay_actor(unsigned i, unsigned type) {
    memset(memory + OVERLAY_ACTOR(i) - 0x80000000u, 0, 0x200);
    memset(memory + OVERLAY_PAAD(i) - 0x80000000u, 0, 0x200);
    put32(OVERLAY_ACTOR(i)+0x58, type); put32(OVERLAY_ACTOR(i)+0x174, OVERLAY_PAAD(i));
    put32(OVERLAY_PAAD(i)+0x94, ACTOR); put32(ACTOR+0x178, SCRIPT);
}
static void overlay_tick(unsigned i) {
    overlay_expected_actor = OVERLAY_ACTOR(i); put32(0x807FBB40, overlay_expected_actor);
    invoke(test_func_menu_80027988, 0, 0, 0);
    put32(overlay_expected_actor+0x60, get32(overlay_expected_actor+0x60) | 0x10); // engine initialization marker
}
static void overlay_draw(unsigned i, const char* top, const char* bottom) {
    overlay_prints = 0;
    unsigned end = invoke(test_ap_shop_overlay_draw, OVERLAY_GFX, OVERLAY_ACTOR(i), 0);
    CHECK(end == OVERLAY_GFX + (bottom[0] ? 72 : 56));
    CHECK(overlay_prints == (bottom[0] ? 2 : 1)); CHECK(!strcmp(overlay_lines[0], top));
    if (bottom[0]) CHECK(!strcmp(overlay_lines[1], bottom));
    CHECK(get32(OVERLAY_GFX+32) == 0xfa000000u);
    CHECK(get32(OVERLAY_GFX+36) == (0xffffff00u | get8(OVERLAY_PAAD(i)+8)));
}
static void overlay_setup(const ap_shop_location_def* def, unsigned buyer, unsigned frames, int purchase) {
    configure_shop(def, buyer, 17); select_shop(); overlay_services();
    label_at(0, def->id, frames, "BABOON BLAST", "TO PLAYER ONE");
    CHECK(invoke(test_ap_apshop_set_labels, LABELS, 1, 0) == 1);
    if (purchase) buy_shop();
    overlay_actor(0, def->vendor ? 0x144 : 0x140);
}
static void test_shop_overlay(void) {
    const ap_shop_location_def* first = &ap_shop_locations[0];
    unsigned char inventory[0x306];
    // Every shop/eligible buyer resolves the committed location, not whichever
    // Kong happens to be active after the notification has started.
    for (unsigned s = 0; s < AP_SHOP_COUNT; ++s) for (unsigned buyer = 0; buyer < 5; ++buyer) {
        const ap_shop_location_def* def = &ap_shop_locations[s];
        if (def->kong != AP_SHOP_SHARED && def->kong != buyer) continue;
        overlay_setup(def, buyer, 130, 1); memcpy(inventory, memory+0x7FC950, sizeof(inventory));
        overlay_tick(0); CHECK(!overlay_deleted && !overlay_registered && get32(OVERLAY_PAAD(0)+0x90) == 129);
        CHECK(get16(OVERLAY_PAAD(0)+0x10+24) == 640 && get16(OVERLAY_PAAD(0)+0x10+26) == 800);
        CHECK(get16(OVERLAY_PAAD(0)+0x50+26) == 848); // actual stock matrix arithmetic/packing
        overlay_tick(0); CHECK(overlay_registered == 1 && get8(OVERLAY_PAAD(0)+8) == 16);
        overlay_draw(0, "BABOON BLAST", "TO PLAYER ONE");
        label_at(0, def->id, 50, "NEW ITEM", "TO SOMEONE ELSE");
        CHECK(invoke(test_ap_apshop_set_labels, LABELS, 1, 0) == 1);
        put8(0x8074E77C, (buyer+1)%5); put32(OVERLAY_PAAD(0)+0x94, 0); put32(ACTOR+0x178, 0);
        overlay_tick(0); overlay_draw(0, "BABOON BLAST", "TO PLAYER ONE"); // frozen strings, no owner reread
        CHECK(get32(OVERLAY_PAAD(0)+0x90) == 127);
        for (unsigned frame = 3; frame < 130; ++frame) overlay_tick(0);
        CHECK(overlay_deleted == 1 && get32(OVERLAY_PAAD(0)) == 0 && get32(OVERLAY_PAAD(0)+4) == 0);
        CHECK(invoke(test_ap_shop_overlay_draw, OVERLAY_GFX, OVERLAY_ACTOR(0), 0) == OVERLAY_GFX);
        CHECK(!memcmp(inventory, memory+0x7FC950, sizeof(inventory))); expect_shop_snapshot(def->id);
    }
    // Full original AP speed range, exact visibility transitions, expiry and
    // saturation; short durations must neither wrap opacity nor divide by zero.
    for (unsigned duration = 50; duration <= 255; ++duration) {
        overlay_setup(first, 0, duration, 1);
        unsigned fade_out = duration < 70 ? (duration-30)/2 : 30;
        for (unsigned frame = 1; frame <= duration; ++frame) {
            overlay_tick(0); CHECK(overlay_deleted == (frame == duration));
            CHECK(get32(OVERLAY_PAAD(0)+0x90) == duration-frame);
            if (frame == 1) CHECK(!get8(OVERLAY_ACTOR(0)+0x154) && !get8(OVERLAY_PAAD(0)+8));
            if (frame == 2) CHECK(get8(OVERLAY_ACTOR(0)+0x154) == 1 && get8(OVERLAY_PAAD(0)+8) > 0);
            if (duration-frame == fade_out) CHECK(get8(OVERLAY_ACTOR(0)+0x154) == 2);
            if (duration-frame == 1) CHECK(!get8(OVERLAY_PAAD(0)+8));
        }
    }
    // No label yet: explicitly pending metadata, never a fabricated item/player.
    configure_shop(first, 0, 0); select_shop(); buy_shop(); overlay_services(); overlay_actor(0, 0x140);
    put32(OVERLAY_PAAD(0)+0x94, 0); overlay_tick(0); overlay_tick(0);
    overlay_draw(0, "ARCHIPELAGO CHECK", "ITEM DETAILS PENDING");
    // Invalid metadata batches are atomic and cannot change an active label.
    overlay_setup(first, 0, 130, 1);
    for (unsigned invalid = 0; invalid < 12; ++invalid) {
        label_at(0, first->id, 130, "BAD REPLACEMENT", "TO PLAYER TWO");
        if (invalid == 0) put32(LABELS, 14041180);
        if (invalid == 1) put32(LABELS, ap_shop_locations[1].id);
        if (invalid == 2) put16(LABELS+4, 49);
        if (invalid == 3) put16(LABELS+4, 256);
        if (invalid == 4) put16(LABELS+6, 1);
        if (invalid == 5) put8(LABELS+74, 1);
        if (invalid == 6) put8(LABELS+8, '%');
        if (invalid == 7) put8(LABELS+8, '~');
        if (invalid == 8) put8(LABELS+8, 0x80);
        if (invalid == 9) for (unsigned c=0;c<33;++c) put8(LABELS+8+c, 'A');
        if (invalid == 10) { put8(LABELS+8, 0); put8(LABELS+9, 'A'); }
        if (invalid == 11) memory_string(LABELS+8, "   ", 33);
        CHECK(invoke(test_ap_apshop_set_labels, LABELS, 1, 0) == 0);
        CHECK(invoke(test_ap_apshop_display, ACTOR, SCRIPT, LABEL_RESULT) == 1);
        expect_string(LABEL_RESULT+8, "BABOON BLAST");
    }
    label_at(0, first->id, 130, "NEW ITEM", ""); label_at(1, first->id, 130, "OTHER ITEM", "");
    CHECK(!invoke(test_ap_apshop_set_labels, LABELS, 2, 0));
    CHECK(!invoke(test_ap_apshop_set_labels, LABELS, 121, 0)); CHECK(!invoke(test_ap_apshop_set_labels, 0, 1, 0));
    CHECK(invoke(test_ap_apshop_set_labels, 0, 0, 0) == 1);
    CHECK(invoke(test_ap_apshop_display, ACTOR, SCRIPT, LABEL_RESULT) == 1);
    expect_string(LABEL_RESULT+8, "ARCHIPELAGO CHECK");
    label_at(0, first->id, 130, "BABOON BLAST", "TO PLAYER ONE");
    CHECK(invoke(test_ap_apshop_set_labels, LABELS, 1, 0) == 1);
    // A valid first record followed by an invalid second record cannot publish.
    put32(LABELS+76, 14041180); CHECK(!invoke(test_ap_apshop_set_labels, LABELS, 2, 0));
    CHECK(invoke(test_ap_apshop_display, ACTOR, SCRIPT, LABEL_RESULT) == 1); expect_string(LABEL_RESULT+8, "BABOON BLAST");
    // Rejected/tampered purchases and invalid context never show success text.
    for (unsigned invalid = 0; invalid < 6; ++invalid) {
        overlay_setup(first, 0, 130, invalid != 0);
        if (invalid == 1) put8(SCRIPT+5, 18);
        if (invalid == 2) put8(0x807467C8, 1);
        if (invalid == 3) put8(0x80755318, 0);
        if (invalid == 4) put32(0x807FBB4C, 0);
        if (invalid == 5) put32(ACTOR+0x58, 0xbc);
        overlay_tick(0); CHECK(overlay_deleted == 1 && !overlay_registered);
    }
    // Eight overlays may overlap. Overflow must not evict/overwrite one, and
    // reusing an actor address must start with a fresh immutable snapshot.
    overlay_setup(first, 0, 130, 1);
    for (unsigned i = 0; i < 8; ++i) {
        if (i) overlay_actor(i, 0x144);
        overlay_tick(i); CHECK(!overlay_deleted);
        CHECK(get16(OVERLAY_PAAD(i)+0x10+26) == 800-100*i);
    }
    overlay_actor(8, 0x144); overlay_tick(8); CHECK(overlay_deleted == 1);
    for (unsigned i = 0; i < 8; ++i) overlay_draw(i, "BABOON BLAST", "TO PLAYER ONE");
    label_at(0, first->id, 50, "REUSED ACTOR", ""); CHECK(invoke(test_ap_apshop_set_labels, LABELS, 1, 0) == 1);
    overlay_actor(0, 0x144); overlay_tick(0); overlay_draw(0, "REUSED ACTOR", ""); CHECK(get32(OVERLAY_PAAD(0)+0x90) == 49);
    // Gate/map/session transitions remove queued AP callbacks without touching
    // the stock item table or following stale shop-owner pointers.
    for (unsigned lookup = 0; lookup < 2; ++lookup) {
        overlay_setup(first, 0, 130, 1);
        if (lookup) put32(OVERLAY_PAAD(0)+0x94, 0);
        put32(TEST_MODE_ADDRESS, 0); overlay_tick(0);
        CHECK(overlay_deleted == 1 && !overlay_registered && !get32(OVERLAY_PAAD(0)));
    }
    for (unsigned transition = 0; transition < 3; ++transition) {
        overlay_setup(first, 0, 130, 1); overlay_tick(0);
        if (transition == 0) put32(TEST_MODE_ADDRESS, 0);
        if (transition == 1) put32(0x8076A0A8, 8);
        if (transition == 2) {
            put32(TEST_MODE_ADDRESS, 0);
            CHECK(invoke(test_ap_prepare_ap_shops, PAYLOAD, 1, RESTORED) == 1);
            put32(TEST_MODE_ADDRESS, 4);
        }
        CHECK(invoke(test_ap_shop_overlay_draw, OVERLAY_GFX, OVERLAY_ACTOR(0), 0) == OVERLAY_GFX);
        overlay_tick(0); CHECK(overlay_deleted == 1 && !get32(OVERLAY_PAAD(0)));
    }
    // Exact stock fallbacks for all original move kinds and all Kongs, also
    // when a non-AP overlay is encountered with the shop gate enabled.
    unsigned char original_paad[0x98];
    for (unsigned mode = 0; mode <= 4; mode += 4) for (unsigned kind = 0; kind < 5; ++kind) for (unsigned buyer = 0; buyer < 5; ++buyer) {
        for (unsigned oracle = 0; oracle < 2; ++oracle) {
            setup(mode, buyer, 1); overlay_services(); overlay_stock = 1;
            put8(SCRIPT+0xb, kind); put8(SCRIPT+0x11, 1); put8(SCRIPT+0x10, 1); put8(0x807FCC4C, 1);
            overlay_actor(0, 0x144); put32(OVERLAY_PAAD(0)+0x90, 130);
            overlay_expected_actor = OVERLAY_ACTOR(0); put32(0x807FBB40, overlay_expected_actor);
            invoke(oracle ? baseline_func_menu_80027988 : test_func_menu_80027988, 0, 0, 0);
            CHECK(!overlay_deleted);
            if (!oracle) memcpy(original_paad, memory+OVERLAY_PAAD(0)-0x80000000u, sizeof(original_paad));
            else CHECK(!memcmp(original_paad, memory+OVERLAY_PAAD(0)-0x80000000u, sizeof(original_paad)));
        }
    }
    renderActor = noop; addActorToTextOverlayRenderArray = noop;
}
