#include "common_structs.h"

RECOMP_DECLARE_EVENT(recomp_on_simple_text_get(s32 fileIndex, s32 stringIndex, u8** text_ptr_ptr, u32 *len_ptr));

typedef struct Struct8070E548 Struct8070E548;
typedef struct Struct8070E34C Struct8070E34C;
struct Struct8070E34C {
    s16 unk0;
    u8 unk2[0x10 - 0x2];
    u16 unk10;
    u8 unk12;
    u8 unk13[0xA0 - 0x13];
    Struct8070E34C *unkA0;
    s32 unkA4;
};

typedef struct {
    s32 offset;
    u16 len;
} TextStringDesc;

typedef struct {
    u16 unk0;
    u8 unk2;
    u8 unk3;
} Struct8070E548_unk4_unk8_unk4;
typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    Struct8070E548_unk4_unk8_unk4 *unk4;
} Struct8070E548_unk4_unk8;
typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    s32 unk4;
    Struct8070E548_unk4_unk8 *unk8;
    f32 unkC;
} Struct8070E548_unk4;
struct Struct8070E548 {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    Struct8070E548_unk4 *unk4;
    s32 unk8;
    Struct8070E34C *unkC;
    u16 unk10;
    s16 unk12;
    f32 unk14;
    f32 unk18;
    Struct8070E548 *unk1C;
};

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    Struct8070E548_unk4 *unk4;
} Struct8070DDDC_unk4;

typedef struct {
    u16 unk0;
    u8 unk2;
    u8 unk3;
    Struct8070DDDC_unk4 *unk4;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    s32 unkC;
} Struct8070DDDC;

extern void *_malloc(s32);
Struct8070DDDC *func_global_asm_8070DDDC(u16 arg0, s32 arg1);
void func_global_asm_8061134C(void *arg0);
extern u8 D_global_asm_807550C8;

RECOMP_PATCH u8 *getTextString(s32 fileIndex, s32 stringIndex, s32 arg2) {
    u32 new_var4;
    Struct8070E548_unk4_unk8 *new_var2;
    s32 pad;
    u8 *text;
    u32 len;
    u8 *sp18;
    Struct8070E548_unk4 *new_var3;
    Struct8070DDDC *temp_v0;
    TextStringDesc *temp_v1;

    temp_v0 = func_global_asm_8070DDDC(fileIndex, 1);
    new_var3 = temp_v0->unk4[stringIndex].unk4;
    new_var2 = &new_var3->unk8[D_global_asm_807550C8];
    temp_v1 = (TextStringDesc *) new_var2->unk4;
    text = (u8*)(temp_v1->offset + temp_v0->unkC);
    len = temp_v1->len;
    recomp_on_simple_text_get(fileIndex, stringIndex, &text, &len);
    sp18 = _malloc(len + 1); // +1 for null-terminator
    if (arg2 & 1) {
        func_global_asm_8061134C(sp18);
    }
    new_var4 = len;
    _memcpy(sp18, text, new_var4);
    sp18[new_var4] = '\0';
    return sp18;
}