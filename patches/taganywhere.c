#include "patches.h"
#include "ultra64.h"

typedef struct floatPos {
	/* 0x000 */ f32 xPos;
	/* 0x004 */ f32 yPos;
	/* 0x008 */ f32 zPos;
} floatPos;

typedef struct actorData {
	/* 0x000 */ s8 unk_00[0x58];
	/* 0x058 */ s32 actorType;
	/* 0x05C */ s8 unk_5C[0x7C-0x5C];
	/* 0x07C */ f32 xPos;
	/* 0x080 */ f32 yPos;
	/* 0x084 */ f32 zPos;
	/* 0x088 */ s8 unk_80[0xB8-0x88];
	/* 0x0B8 */ f32 hSpeed;
	/* 0x0BC */ s8 unk_BC[0x154-0xBC];
	/* 0x154 */ s8 control_state;
	/* 0x155 */ s8 control_state_progress;
	/* 0x156 */ s8 unk_156[0x180-0x156];
	/* 0x180 */ void* tied_character_spawner;
} actorData;

typedef struct cameraData {
	/* 0x000 */ s8 unk_00[0x7C];
	/* 0x07C */ f32 xPos;
	/* 0x080 */ f32 yPos;
	/* 0x084 */ f32 zPos;
	/* 0x088 */ s8 unk_88[0x15F-0x88];
	/* 0x15F */ s8 facing_angle;
	/* 0x160 */ s8 unk_160[0x1FC-0x160];
	/* 0x1FC */ f32 viewportX;
	/* 0x200 */ f32 viewportY;
	/* 0x204 */ f32 viewportZ;
	/* 0x208 */ s8 unk_208[0x22A-0x208];
	/* 0x22A */ s16 viewportRotation;
	/* 0x22C */ s8 unk_22C[0x26B-0x22C];
	/* 0x26B */ s8 camera_state;
} cameraData;

typedef struct bone_array {
	/* 0x000 */ s8 unk_00[0x58];
	/* 0x058 */ s16 xPos;
	/* 0x05A */ s16 yPos;
	/* 0x05C */ s16 zPos;
} bone_array;

typedef struct rendering_params {
	/* 0x000 */ s8 unk_00[0x14];
	/* 0x014 */ bone_array* bone_array1;
	/* 0x018 */ bone_array* bone_array2;
} rendering_params;

typedef struct playerData {
	/* 0x000 */ s8 unk_00[0x4];
	/* 0x004 */ rendering_params* rendering_param_pointer;
	/* 0x008 */ s8 unk_08[0x58 - 0x8];
	/* 0x058 */ s32 characterID; //02 is dk, 03 is diddy, 04 is lanky, etc
	/* 0x05C */ s8 unk_5C[0x60-0x5C];
	/* 0x060 */ s32 obj_props_bitfield;
	/* 0x064 */ s8 unk_64[0x6A-0x64];
	/* 0x06A */ s16 grounded_bitfield;
	/* 0x06C */ s16 unk_bitfield;
	/* 0x06E */ s8 unk_6E[0x7C-0x6E];
	/* 0x07C */ f32 xPos;
	/* 0x080 */ f32 yPos;
	/* 0x084 */ f32 zPos;
	/* 0x088 */ s8 unk_88[0xA4-0x88];
	/* 0x0A4 */ f32 floor;
	/* 0x0A8 */ s8 unk_A8[0xB8-0xA8];
	/* 0x0B8 */ f32 hSpeed;
	/* 0x0BC */ s8 unk_BC[0x4];
	/* 0x0C0 */ f32 yVelocity;
	/* 0x0C4 */ f32 yAccel;
	/* 0x0C8 */ s8 unk_C4[0xE6 - 0xC8];
	/* 0x0E6 */ s16 facing_angle;
	/* 0x0E8 */ s16 skew_angle;
	/* 0x0EA */ s8 unk_EA[0xEE - 0xEA];
	/* 0x0EE */ s16 next_facing_angle;
	/* 0x0F0 */ s8 unk_F0[0x110 - 0xF0];
	/* 0x110 */ s8 touching_object;
	/* 0x111 */ s8 unk_111[0x128 - 0x111];
	/* 0x128 */ s16 strong_kong_value;
	/* 0x12A */ s8 unk_12A[2];
	/* 0x12C */ s16 chunk;
	/* 0x12E */ s8 unk_12E[0x13C - 0x12E];
	/* 0x13C */ void* collision_queue_pointer;
	/* 0x140 */ s8 unk_140[0x147 - 0x140];
	/* 0x147 */ s8 hand_state;
	/* 0x148 */ s8 unk_148[0x154 - 0x148];
	/* 0x154 */ s8 control_state;
	/* 0x155 */ s8 control_state_progress;
	/* 0x156 */ s8 unk_156[0x18A-0x156];
	/* 0x18A */ s16 moving_angle;
	/* 0x18C */ s8 unk_18C[0x1B8-0x18C];
	/* 0x1B8 */ f32 velocity_cap;
	/* 0x1BC */ s8 unk_1BC[0x1D0-0x1BC];
	/* 0x1D0 */ s16 ostand_value;
	/* 0x1D2 */ s8 unk_1D2[0x208-0x1D2];
	/* 0x208 */ actorData* vehicle_actor_pointer;
	/* 0x20C */ s8 was_gun_out;
	/* 0x20D */ s8 unk_20D[0x23C - 0x20D];
	/* 0x23C */ s16 unk_rocketbarrel_value1;
	/* 0x23E */ s16 unk_rocketbarrel_value2;
	/* 0x240 */ s8 unk_240[0x284 - 0x240];
	/* 0x284 */ cameraData* camera_pointer;
	/* 0x288 */ s8 unk_288[0x323 - 0x288];
	/* 0x323 */ s8 unk_rocketbarrel_value3;
	/* 0x324 */ s8 unk_324[0x328 - 0x324];
	/* 0x328 */ actorData* krool_timer_pointer;
	/* 0x32C */ actorData* held_actor;
	/* 0x330 */ s8 unk_330[0x36F - 0x330];
	/* 0x36F */ s8 new_kong;
	/* 0x370 */ s32 strong_kong_ostand_bitfield;
} playerData; //size 0x630

typedef struct TextOverlay {
	/* 0x000 */ s8 unk_00[0x84];
	/* 0x004 */ //u16
	/* 0x006 */ //u16
	/* 0x008 */ //u8
	/* 0x009 */ //u8
	/* 0x054 */ //layer ID?
	/* 0x084 */ f32 style;
	/* 0x088 */ s8 unk_88[0x15F-0x88];
	/* 0x15F */ s8 opacity;
	/* 0x160 */ s8 unk_160[0x0A];
	/* 0x16A */ u8 red;
	/* 0x16B */ u8 green;
	/* 0x16C */ u8 blue;
	/* 0x16D */ s8 unk_16D[0x0B];
	/* 0x178 */ char* string;
} TextOverlay;

typedef struct Screen {
	/* 0x000 */ void* TextArray;
	/* 0x004 */ const int* FunctionArray;
	/* 0x005 */ s8 ArrayItems;
	/* 0x006 */ s8 ParentScreen;
	/* 0x007 */ s8 ParentPosition;
} Screen;

typedef const struct MapWarp {
	/* 0x000 */ const u8* maps;
	/* 0x004 */ const u8* exits;
	/* 0x008 */ s8 screen;
} MapWarp;

typedef struct Controller {
	/* 0x000 */ s16 Buttons;
	/* 0x002 */ s8 stickX;
	/* 0x003 */ s8 stickY;
} Controller;

typedef struct InventoryBase {
	/* 0x000 */ s16 StandardAmmo;
	/* 0x002 */ s16 HomingAmmo;
	/* 0x004 */ s16 Oranges;
	/* 0x006 */ s16 Crystals;
	/* 0x008 */ s16 Film;
	/* 0x00A */ s8 unk0A;
	/* 0x00B */ s8 Health;
	/* 0x00C */ s8 Melons;
} InventoryBase;

typedef struct PosState {
	/* 0x000 */ f32 xCamera;
	/* 0x004 */ f32 yCamera;
	/* 0x008 */ f32 zCamera;
	/* 0x00C */ f32 yVelocity;
	/* 0x010 */ f32 yAccel;
	/* 0x014 */ f32 hVelocity;
	/* 0x018 */ f32 yFloor;
	/* 0x01C */ s16 xStored1;
	/* 0x01E */ s16 yStored1;
	/* 0x020 */ s16 zStored1;
	/* 0x022 */ s16 xStored2;
	/* 0x024 */ s16 yStored2;
	/* 0x026 */ s16 zStored2;
	/* 0x028 */ s16 facing_angle;
	/* 0x02A */ s16 skew_angle;
	/* 0x02C */ f32 xPos;
	/* 0x030 */ f32 yPos;
	/* 0x034 */ f32 zPos;
	/* 0x038 */ s8 control_state;
	/* 0x039 */ s8 control_state_progress;
	/* 0x03A */ s8 map;
	/* 0x03B */ s8 bone_array_counter;
	/* 0x03C */ s16 cameraRotation;
	/* 0x03E */ s8 cameraState;
} PosState;

typedef struct TimerInfo {
	/* 0x000 */ u32 Start;
	/* 0x004 */ u32 Timer;
	/* 0x008 */ s8 Mode;
	/* 0x009 */ s8 StartMode;
	/* 0x00A */ s8 PauseMode;
	/* 0x00B */ s8 FinishMode;
	/* 0x00C */ u32 Reduction;
	/* 0x010 */ u32 TimerPostReduction;
} TimerInfo;

typedef struct AutowalkData {
	/* 0x000 */ s8 unk_00[0x12];
	/* 0x012 */ s16 xPos;
	/* 0x014 */ s8 unk_14[0x2];
	/* 0x016 */ s16 zPos;
} AutowalkData;

typedef struct RGB {
	/* 0x000 */ u8 red;
	/* 0x001 */ u8 green;
	/* 0x002 */ u8 blue;
} RGB;

typedef struct KongBase {
	/* 0x000 */ s8 special_moves;
	/* 0x001 */ s8 simian_slam;
	/* 0x002 */ s8 weapon_bitfield;
	/* 0x003 */ s8 ammo_belt;
	/* 0x004 */ s8 instrument_bitfield;
	/* 0x005 */ s8 unk_05[0x2];
	/* 0x007 */ s8 coins;
	/* 0x008 */ s16 instrument_energy;
	/* 0x00A */ s16 cb_count[0xE];
	/* 0x026 */ s16 tns_cb_count[0xE];
	/* 0x042 */ s16 gb_count[0xE];
} KongBase;

typedef struct ISGFadeoutData {
	/* 0x000 */ s32 FadeoutTime;
	/* 0x004 */ s8 FadeoutMap;
	/* 0x005 */ s8 unk_05[0x3];
} ISGFadeoutData;

typedef struct GiantKoshaData {
	/* 0x000 */ s16 timer;
} GiantKoshaData;

typedef struct SwapObjectData {
	/* 0x000 */ s8 unk_00[0x210];
	/* 0x210 */ floatPos cameraPositions[4];
	/* 0x240 */ s8 unk_21C[0x29C-0x240];
	/* 0x29C */ s16 action_type;
} SwapObjectData;

typedef struct sandstormData {
	/* 0x000 */ s8 unk_00[0x54];
	/* 0x054 */ s8 sandstorm_active;
} sandstormData;

typedef struct snagData {
	/* 0x000 */ s8 unk_00[0x48];
	/* 0x048 */ s8 reset;
	/* 0x049 */ s8 unk_49[0x54-0x49];
	/* 0x054 */ s8 check;
	/* 0x055 */ s8 unk_55[0x60-0x55];
	/* 0x060 */ s8 state;
	/* 0x061 */ s8 unk_61[0x9B-0x61];
	/* 0x09B */ s8 resettrigger;
} snagData;

typedef struct ModelTwoData {
	/* 0x000 */ s8 unk_00[0x7C];
	/* 0x07C */ void* behaviour_pointer;
	/* 0x080 */ s8 unk_80[0x84-0x80];
	/* 0x084 */ s16 object_type;
	/* 0x086 */ s8 unk_86[0x4];
	/* 0x08A */ s16 object_id;
	/* 0x08C */ s8 unk_8C[0x4];
} ModelTwoData;

typedef struct WarpInfo {
	/* 0x000 */ s16 xPos;
	/* 0x002 */ s16 yPos;
	/* 0x004 */ s16 zPos;
	/* 0x006 */ u8 facing_angle; // (val / 255) * 4096
	/* 0x007 */ u8 camera_angle; // (player + 0x284)->0x15F
	/* 0x008 */ s8 will_autowalk;
	/* 0x009 */ s8 spawn_at_origin;
} WarpInfo;

typedef struct flagMenuData {
	/* 0x000 */ const short* flagArray;
	/* 0x004 */ const char* flagTypeArray;
	/* 0x008 */ const int* flagText;
	/* 0x00C */ s8 screenIndex;
	/* 0x00D */ s8 flagCount;
} flagMenuData;

typedef struct cutsceneInfo {
	/* 0x000 */ s8 csdata[0xC];
} cutsceneInfo;

typedef struct cutsceneType {
	/* 0x000 */ s8 unk_00[0xD0];
	/* 0x0D0 */ cutsceneInfo* cutscene_databank;
} cutsceneType;

typedef struct spriteDisplay {
	/* 0x000 */ s32 disappear;
	/* 0x004 */ s8 unk_04[0x11];
	/* 0x015 */ s8 unk_15;
} spriteDisplay;

typedef struct spriteControl {
	/* 0x000 */ f32 movement_style;
	/* 0x004 */ f32 pointer_offset_0x15;
	/* 0x008 */ f32 xPos;
	/* 0x00C */ f32 yPos;
	/* 0x010 */ f32 scale;
	/* 0x014 */ f32 unk_14;
	/* 0x018 */ s32 unk_18;
	/* 0x01C */ s32 unk_1C;
} spriteControl;

typedef struct otherSpriteControl {
	/* 0x000 */ s8 unk_000[0x28];
	/* 0x028 */ u8 left_stretch;
	/* 0x029 */ u8 right_stretch;
	/* 0x02A */ u8 up_stretch;
	/* 0x02B */ u8 down_stretch;
	/* 0x02C */ s8 unk_02C[0x340-0x2C];
	/* 0x340 */ f32 xPos;
	/* 0x344 */ f32 yPos;
	/* 0x348 */ s8 unk_348[8];
	/* 0x350 */ s8 gif_update_frequency;
	/* 0x351 */ s8 unk_351[0xB];
	/* 0x35C */ spriteControl* gif_control_pointer;
	/* 0x360 */ f32 xScale;
	/* 0x364 */ f32 yScale;
	/* 0x368 */ s8 unk_368[0x2];
	/* 0x36A */ s8 transparency1;
	/* 0x36B */ s8 transparency2;
	/* 0x36C */ s8 transparency3;
	/* 0x36D */ s8 transparency4;
	/* 0x36E */ s8 unk_36E[0x384-0x36E];
	/* 0x384 */ void* some_pointer;
} otherSpriteControl;

typedef struct submapInfo {
	/* 0x000 */ s8 in_submap;
	/* 0x001 */ s8 unk_01;
	/* 0x003 */ s16 transition_properties_bitfield;
	/* 0x004 */ s8 unk_04[0x12-4];
	/* 0x012 */ s16 parent_map;
	/* 0x014 */ s8 parent_exit;
} submapInfo;

typedef struct MinigameController {
	/* 0x000 */ s8 unk_00[0x1C5];
	/* 0X000 */ s8 hit_count;
} MinigameController;

typedef struct SpawnerInfo {
	/* 0x000 */ u8 enemy_value;
	/* 0x001 */ s8 unk_01[0x14-0x1];
	/* 0x014 */ s8 respawnTimerInit;
	/* 0x015 */ s8 unk_15[0x18-0x15];
	/* 0x018 */ void* tied_actor;
	/* 0x01C */ s8 unk_1C[0x42-0x1C];
	/* 0x042 */ s8 spawn_state;
	/* 0x043 */ s8 unk_43[0x48 - 0x43];
} SpawnerInfo;

typedef struct pppanicController {
	/* 0x000 */ s8 unk_00[0x1B1];
	/* 0x1B1 */ s8 hit_count1;
	/* 0x1B2 */ s8 unk_1B2;
	/* 0x1B3 */ s8 hit_count2;
} pppanicController;

typedef struct krazykkcontroller {
	/* 0x000 */ s8 unk_00[0x1BD];
	/* 0x1BD */ s8 hit_count1;
	/* 0x1BE */ s8 unk_1BE;
	/* 0x1BF */ s8 hit_count2;
} krazykkcontroller;

typedef struct slotArray {
	/* 0x000 */ s8 unk_10[0x16];
	/* 0x016 */ s16 hit_count;
} slotArray;

typedef struct bbbanditcontroller {
	/* 0x000 */ s8 unk_00[0x11C];
	/* 0x11C */ actorData* slot_pointer;
	/* 0x120 */ s8 unk_120[0x154-0x120];
	/* 0x154 */ s8 control_state;
	/* 0x155 */ s8 unk_155[0x174-0x155];
	/* 0x174 */ slotArray* handle_pointer;
} bbbanditcontroller;

typedef struct bbbashcontroller {
	/* 0x000 */ s8 unk_00[0x1A1];
	/* 0x1A1 */ s8 hit_count;
} bbbashcontroller;

typedef struct kkoshcontroller {
	/* 0x000 */ s8 unk_00[0x1CB];
	/* 0x1CB */ s8 hit_count;
} kkoshcontroller;

typedef struct sseekcontroller {
	/* 0x000 */ s8 unk_00[0x19F];
	/* 0x19F */ s8 hit_count;
} sseekcontroller;

typedef struct SpawnerArray {
	/* 0x000 */ SpawnerInfo SpawnerData[120];
} SpawnerArray;

typedef struct SpawnerMasterInfo {
	/* 0x000 */ s16 count;
	/* 0x002 */ s8 unk_02[2];
	/* 0x004 */ SpawnerArray* array;
} SpawnerMasterInfo;

typedef struct filestateInfo {
	/* 0x000 */ s8 perm_flags[0x140];
	/* 0x140 */ s8 moves_base[0x1E0];
	/* 0x320 */ s8 inventory[0x10];
	/* 0x330 */ s8 temp_flags[0x10];
} filestateInfo;

typedef struct loadedActorArr {
	/* 0x000 */ actorData* actor;
	/* 0x004 */ s32 unk_04;
} loadedActorArr;

typedef struct actorSpawnerData {
	/* 0x000 */ s8 unk_00[4];
	/* 0x004 */ floatPos positions;
	/* 0x010 */ s8 unk_10[0x44-0x10];
	/* 0x044 */ void* tied_actor;
	/* 0x048 */ s8 unk_48[0x5A-0x58];
	/* 0x05A */ s16 id;
	/* 0x05C */ s8 unk_5C[0x64-0x5C];
	/* 0x064 */ void* previous_spawner;
	/* 0x068 */ void* next_spawner;
} actorSpawnerData;

/*
// TODO: This needs to be a char, not an int
enum HUDState {
	Invisible,
	Appearing,
	Visible,
	Disappearing,
};
*/

typedef struct HUDDisplay {
	/* 0x000 */ void* actual_count_pointer;
	/* 0x004 */	s16 hud_count;
	/* 0x006 */	s8 freeze_timer;
	/* 0x007 */	s8 counter_timer;
	/* 0x008 */	u32 screen_x;
	/* 0x00C */	u32 screen_y;
	/* 0x010 */ s8 unk_10[0x20-0x10];
	/* 0x020 */ u32 hud_state;
	/* 0x024 */ s32 unk_24;
	/* 0x028 */	void* counter_pointer;
	/* 0x02C */ s32 unk_2c;
} HUDDisplay;

// Functions
extern void setFlag(s32 flagIndex, s32 value, s8 flagType);
extern s32 isFlagSet(s32 flagIndex, s8 flagType);
extern void* dk_malloc(s32 size);
extern void dk_free(void* mallocPtr);
extern void playSound(s16 soundIndex, s32 volume, f32 unk1, f32 pitch, s32 unk3, s32 unk4);
extern void initiateTransition(s32 map, s32 exit);
extern s32* getFlagBlockAddress(s8 flagType);
extern s32 isAddressActor(void* address);
extern s32 getTimestamp();
extern void dmaFileTransfer(s32 romStart, s32 romEnd, s32 ramStart);
extern void deleteActor(void* actor);
extern s32 spawnActor(s32 actorID, s32 actorBehaviour);
extern void spawnTextOverlay(s32 style, s32 x, s32 y, s8* string, s32 timer1, s32 timer2, u8 effect, u8 speed);
extern f32 dk_sqrt(f32 __x);
extern void dk_strFormat(s8* destination, s8* source, ...);
extern void dk_multiply(f64 val1, f64 val2, s32 unk1, s32 unk2);
extern f64 convertTimestamp(f64 unk0, f64 unk1, u32 unk2, u32 unk3);
extern void resetMap();
extern void prepKongColoring();
extern void* dk_memcpy(void* _dest, void* _src, s32 size);
extern void SaveToGlobal();
extern s32 DetectGameOver();
extern s32 DetectAdventure();
extern void displaySprite(void* control_pointer, void* sprite, s32 x, s32 y, s32 scale, s32 gif_updatefrequency, s32 movement_style);
extern void alterSize(void* object, s32 size);
extern void unkSizeFunction(void* object);
extern void spawnRocketbarrel(void* object, s32 unk);
extern void playSong(s32 songIndex);
extern void playCutscene(void* actor, s32 cutscene_index, s32 cutscene_type);
extern void setHUDItemAsInfinite(s32 item_index, s32 player_index, s8 isInfinite);
extern void copyFromROM(s32 rom_start, void* write_Location, void* file_size_location, s32 unk1, s32 unk2, s32 unk3, s32 unk4);
extern s32 getActorSpawnerID(void* actor);
extern void textOverlayCode(void);

// Vanilla data
extern f32 TransitionSpeed;
extern s8 CutsceneWillPlay;
extern s8 KRoolRound;
extern KongBase D_global_asm_807FC950[6];
extern s32 PlayerOneColor;
extern s8 Mode;
extern s8 global_properties_bitfield;
extern s32 current_map;
extern s32 DestMap;
extern s32 DestExit;
extern u16 D_global_asm_8076A172;
extern s8 story_skip;
extern s8 HelmTimerShown;
extern s8 TempFlagBlock[0x10];
extern submapInfo SubmapData;
extern s8 HelmTimerPaused;
extern s32 LagBoost;
extern s32 FrameLag;
extern s32 FrameReal;
extern s32 RNG;
extern s8 LogosDestMap;
extern s8 LogosDestMode;
extern s8 Gamemode;
extern void* ObjectModel2Pointer; // TODO: Object model 2 struct array
extern s32 ObjectModel2Timer;
extern s32 ObjectModel2Count;
extern s32 ObjectModel2Count_Dupe;
extern s16 D_global_asm_807476F4;
extern s16 D_global_asm_807476F0;
extern s8 is_cutscene_active;
extern cutsceneType* CutsceneTypePointer;
extern s16 PreviousCameraState;
extern s16 CurrentCameraState;
extern s16 CameraStateChangeTimer;
extern AutowalkData* AutowalkPointer;
extern s8 is_autowalking;
extern WarpInfo PositionWarpInfo;
extern s16 PositionWarpBitfield;
extern f32 PositionFloatWarps[3];
extern u16 PositionFacingAngle;
extern s8 ChimpyCam;
extern s8 ScreenRatio;
extern actorData* CurrentActorPointer;
extern s8 LoadedActorCount;
extern loadedActorArr LoadedActorArray[64];
extern SpawnerMasterInfo SpawnerMasterData;
extern void* ActorSpawnerArrayPointer;
extern RGB MenuSkyTopRGB;
extern RGB MenuSkyRGB;
extern actorData* actor_list[];
extern s16 actor_count;
extern s16 ButtonsEnabledBitfield;
extern s8 JoystickEnabledX;
extern s8 JoystickEnabledY;
extern s8 MapState;
extern Controller ControllerInput;
extern Controller newly_pressed_input_copy;
extern playerData* gPlayerPointer;
extern SwapObjectData* character_change_array;
extern s8 current_character_index;
extern cameraData* Camera;
extern s8 ISGActive;
extern u32 ISGTimestampMajor; // TODO: libultra type (OSTime)
extern u32 ISGTimestampMinor;
extern s8 ISGPreviousFadeout;
extern u32 CurrentTimestampMajor; // TODO: libultra type (OSTime)
extern u32 CurrentTimestampMinor;
extern ISGFadeoutData ISGFadeoutArray[];
extern InventoryBase D_global_asm_807FCC40;
extern s8 ModelTwoTouchCount;
extern s16 ModelTwoTouchArray[4];
extern s8 TransitionProgress;
extern Controller BackgroundHeldInput;
extern u32 PauseTimestampMajor; // TODO: libultra type (OSTime)
extern u32 PauseTimestampMinor;
extern u32 HelmStartTimestampMajor; // TODO: libultra type (OSTime)
extern u32 HelmStartTimestampMinor;
extern s32 HelmStartTime;
extern s16 p1PressedButtons; // TODO: libultra type
extern s16 p1HeldButtons; // TODO: libultra type
extern s8 player_count;
extern s32* sprite_table[0xAF];
extern s8 sprite_translucency;
extern s32* bbbandit_array[4];
extern s8 StoredDamage;
extern void* ActorSpawnerPointer;
extern f32 D_global_asm_807FD888;
extern HUDDisplay* D_global_asm_80754280;

// Hack data

enum textTable {
	terminate, //0x00
	nothing_01,
	nothing_02,
	nothing_03,
	nothing_04,
	nothing_05,
	nothing_06,
	nothing_07,
	nothing_08,
	nothing_09,
	nothing_0A,
	nothing_0B,
	nothing_0C,
	nothing_0D,
	nothing_0E,
	nothing_0F,
	nothing_10,
	nothing_11,
	nothing_12,
	nothing_13,
	nothing_14,
	nothing_15,
	nothing_16,
	nothing_17,
	nothing_18,
	nothing_19,
	nothing_1A,
	nothing_1B,
	nothing_1C,
	nothing_1D,
	nothing_1E,
	nothing_1F,
	space, //0x20
	exclamationPoint, //0x21
	nothing_22, //0x22
	nothing_23, //0x23
	nothing_24, //0x24
	percentSign, //0x25
	ampersand, //0x26
	apostrophe, //0x27
	nothing_28, //0x28
	nothing_29, //0x29
	nothing_2A, //0x2A
	nothing_2B, //0x2B
	bigComma, //0x2C
	hypen, //0x2D
	smallComma, //0x2E
	nothing_2F, //0x2F
	zero, //0x30
	one, //0x31
	two, //0x32
	three, //0x33
	four, //0x34
	five, //0x35
	six, //0x36
	seven, //0x37
	eight, //0x38
	nine, //0x39
	colon, //0x3A
	nothing_3B, //0x3B
	nothing_3C, //0x3C
	nothing_3D, //0x3D
	nothing_3E, //0x3E
	questionMark, //0x3F
	nothing_40, //0x40
	A, //0x41 start of normal ascii
	B, //0x42
	C, //0x43
	D, //0x44
	E, //0x45
	F, //0x46
	G, //0x47
	H, //0x48
	I, //0x49
	J, //0x4A
	K, //0x4B
	L, //0x4C
	M, //0x4D
	N, //0x4E
	O, //0x4F
	P, //0x50
	Q, //0x51
	R, //0x52
	S, //0x53
	T, //0x54
	U, //0x55
	V, //0x56
	W, //0x57
	X, //0x58
	Y, //0x59
	Z, //0x5A
	nothing_5B, //0x5B
	nothing_5C, //0x5C
	nothing_5D, //0x5D
	nothing_5E, //0x5E
	nothing_5F, //0x5F
	nothing_60, //0x60
	nothing_61, //0x61
	buttonB_62, //0x62
	buttonCopyright, //0x63
	nothing_64, //0x64
	buttonCRight, //0x65
	nothing_66, //0x66
	buttonStart, //0x67
	nothing_68, //0x68
	nothing_69, //0x69
	nothing_6A, //0x6A
	nothing_6B, //0x6B
	buttonL, //0x6C
	nothing_6D, //0x6D
	buttonCUp, //0x6E
	infinity, //0x6F
	nothing_70, //0x70
	buttonA, //0x71
	buttonR, //0x72
	buttonCDown, //0x73
	nothing_74, //0x74
	nothing_75, //0x75
	nothing_76, //0x76
	buttonCLeft, //0x77
	nothing_78, //0x78
	nothing_79, //0x79
	buttonZ //0x7A
};

#define NULL 0

// SFX Index
#define SFX_Banana 0x2A0
#define SFX_Okay 0x23C
#define SFX_UhOh 0x150
#define SFX_Bell 0x1F
#define SFX_KLumsy 0x31C
#define SFX_Wrong 0x98
#define SFX_Potion 0x214
#define SFX_AmmoPickup 0x157
#define SFX_Coin 0x1D1
#define SFX_BeepHigh 116
#define SFX_BeepLow 117
#define SFX_FeedMe 601
#define SFX_Bounce 458
#define SFX_TimerTock 143
#define SFX_ChunkyFallTooFar 197
#define SFX_Fire 234
#define SFX_MatchingSound 171
#define SFX_CameraPull 441
#define SFX_Quack 170
#define SFX_TagWarp 612
#define SFX_TakeWarp 230
#define SFX_Splat 22
#define SFX_Burp 530

// Other
#define MysteryWriteOffset 0x29C
#define CurrentCharacter 0x36C
#define MovesBaseSize 0x1D8

extern void setPermanentFlag(s16 flagIndex);
extern void setGlobalFlag(s16 flagIndex);
extern void setTemporaryFlag(s16 flagIndex);
extern void* findActorWithType(s32 search_actor_type);

extern s32 inBadMap(void);
extern s32 inBadMovementState(void);
extern void tagAnywhere(void);

void setPermanentFlag(s16 flagIndex) {
	setFlag(flagIndex, 1, 0);
}

void setGlobalFlag(s16 flagIndex) {
	setFlag(flagIndex, 1, 1);
}

void setTemporaryFlag(s16 flagIndex) {
	setFlag(flagIndex, 1, 2);
}

void* findActorWithType(s32 search_actor_type) {
	for (s32 i = 0; i < actor_count; i++) {
		actorData* _actor_ = (actorData*)actor_list[i];
		if (_actor_->actorType == search_actor_type) {
			return _actor_;
		}
	}
	return 0;
}


/////////////////
static const u8 bad_maps[] = {
	1, // Funky's Store
	2, // DK Arcade
	3, // K. Rool Barrel: Lanky's Maze
	5, // Cranky's Lab
	6, // Jungle Japes: Minecart
	9, // Jetpac
	10, // Kremling Kosh! (very easy)
	14, // Angry Aztec: Beetle Race // Note: Softlock at the end if enabled?
	15, // Snide's H.Q.
	18, // Teetering Turtle Trouble! (very easy)
	25, // Candy's Music Shop
	27, // Frantic Factory: Car Race
	31, // Gloomy Galleon: K. Rool's Ship // TODO: Test
	32, // Batty Barrel Bandit! (easy)
	35, // K. Rool Barrel: DK's Target Game
	37, // Jungle Japes: Barrel Blast // Note: The barrels don't work as other kongs so not much point enabling it on this map
	41, // Angry Aztec: Barrel Blast
	42, // Troff 'n' Scoff
	50, // K. Rool Barrel: Tiny's Mushroom Game
	54, // Gloomy Galleon: Barrel Blast
	55, // Fungi Forest: Minecart
	76, // DK Rap
	77, // Minecart Mayhem! (easy)
	78, // Busy Barrel Barrage! (easy)
	79, // Busy Barrel Barrage! (normal)
	80, // Main Menu
	82, // Crystal Caves: Beetle Race
	83, // Fungi Forest: Dogadon
	101, // Krazy Kong Klamour! (easy) // Note: Broken with switch kong
	102, // Big Bug Bash! (very easy) // Note: Broken with switch kong
	103, // Searchlight Seek! (very easy) // Note: Broken with switch kong
	104, // Beaver Bother! (easy) // Note: Broken with switch kong
	106, // Creepy Castle: Minecart
	107, // Kong Battle: Battle Arena // TODO: Would be really cool to get multiplayer working, currently just voids you out when activated
	109, // Kong Battle: Arena 1 // TODO: Would be really cool to get multiplayer working, currently just voids you out when activated
	110, // Frantic Factory: Barrel Blast
	111, // Gloomy Galleon: Puftoss
	115, // Kremling Kosh! (easy)
	116, // Kremling Kosh! (normal)
	117, // Kremling Kosh! (hard)
	118, // Teetering Turtle Trouble! (easy)
	119, // Teetering Turtle Trouble! (normal)
	120, // Teetering Turtle Trouble! (hard)
	121, // Batty Barrel Bandit! (easy)
	122, // Batty Barrel Bandit! (normal)
	123, // Batty Barrel Bandit! (hard)
	131, // Busy Barrel Barrage! (hard)
	136, // Beaver Bother! (normal)
	137, // Beaver Bother! (hard)
	138, // Searchlight Seek! (easy)
	139, // Searchlight Seek! (normal)
	140, // Searchlight Seek! (hard)
	141, // Krazy Kong Klamour! (normal)
	142, // Krazy Kong Klamour! (hard)
	143, // Krazy Kong Klamour! (insane)
	144, // Peril Path Panic! (very easy) // Note: Broken with switch kong
	145, // Peril Path Panic! (easy)
	146, // Peril Path Panic! (normal)
	147, // Peril Path Panic! (hard)
	148, // Big Bug Bash! (easy)
	149, // Big Bug Bash! (normal)
	150, // Big Bug Bash! (hard)
	152, // Hideout Helm (Intro Story) // Note: Handled by cutscene check
	153, // DK Isles (DK Theatre) // Note: Handled by cutscene check
	165, // K. Rool Barrel: Diddy's Kremling Game
	172, // Rock (Intro Story) // Note: Handled by cutscene check
	185, // Enguarde Arena // Note: Handled by character check
	186, // Creepy Castle: Car Race
	187, // Crystal Caves: Barrel Blast
	188, // Creepy Castle: Barrel Blast
	189, // Fungi Forest: Barrel Blast
	190, // Kong Battle: Arena 2 // TODO: Would be really cool to get multiplayer working, currently just voids you out when activated
	191, // Rambi Arena // Note: Handled by character check
	192, // Kong Battle: Arena 3 // TODO: Would be really cool to get multiplayer working, currently just voids you out when activated
	198, // Training Grounds (End Sequence) // Note: Handled by cutscene check
	199, // Creepy Castle: King Kut Out // Note: Doesn't break the kong order but since this fight is explicitly about tagging we might as well disable
	201, // K. Rool Barrel: Diddy's Rocketbarrel Game
	202, // K. Rool Barrel: Lanky's Shooting Game
	203, // K. Rool Fight: DK Phase // Note: Enabling here breaks the fight and may cause softlocks
	204, // K. Rool Fight: Diddy Phase // Note: Enabling here breaks the fight and may cause softlocks
	205, // K. Rool Fight: Lanky Phase // Note: Enabling here breaks the fight and may cause softlocks
	206, // K. Rool Fight: Tiny Phase // Note: Enabling here breaks the fight and may cause softlocks
	207, // K. Rool Fight: Chunky Phase // Note: Enabling here breaks the fight and may cause softlocks
	208, // Bloopers Ending // Note: Handled by cutscene check
	209, // K. Rool Barrel: Chunky's Hidden Kremling Game
	210, // K. Rool Barrel: Tiny's Pony Tail Twirl Game
	211, // K. Rool Barrel: Chunky's Shooting Game
	212, // K. Rool Barrel: DK's Rambi Game
	213, // K. Lumsy Ending // Note: Handled by cutscene check
	214, // K. Rool's Shoe
	215, // K. Rool's Arena // Note: Handled by cutscene check?
};

static const u8 bad_movement_states[] = {
	//0x02, // First Person Camera
	//0x03, // First Person Camera (Water)
	0x04, // Fairy Camera
	0x05, // Fairy Camera (Water)
	0x06, // Locked (Bonus Barrel)
	0x15, // Slipping
	0x16, // Slipping
	0x18, // Baboon Blast Pad
	0x1B, // Simian Spring
	//0x1C, // Simian Slam // Note: As far as I know this doesn't break anything, so we'll save the CPU cycles
	0x20, // Falling/Splat, // Note: Prevents quick recovery from fall damage, and I guess maybe switching to avoid fall damage?
	0x2D, // Shockwave
	0x2E, // Chimpy Charge
	0x31, // Damaged
	0x32, // Stunlocked
	0x33, // Damaged
	0x35, // Damaged
	0x36, // Death
	0x37, // Damaged (Underwater)
	0x38, // Damaged
	0x39, // Shrinking
	0x42, // Barrel
	0x43, // Barrel (Underwater)
	0x44, // Baboon Blast Shot
	0x45, // Cannon Shot
	0x52, // Bananaporter
	0x53, // Monkeyport
	0x54, // Bananaporter (Multiplayer)
	0x56, // Locked
	0x57, // Swinging on Vine
	0x58, // Leaving Vine
	0x59, // Climbing Tree
	0x5A, // Leaving Tree
	0x5B, // Grabbed Ledge
	0x5C, // Pulling up on Ledge
	0x63, // Rocketbarrel // Note: Covered by crystal D_global_asm_80754280 check except for Helm & K. Rool
	0x64, // Taking Photo
	0x65, // Taking Photo
	0x67, // Instrument
	0x69, // Car
	0x6A, // Learning Gun // Note: Handled by map check
	0x6B, // Locked
	0x6C, // Feeding T&S // Note: Handled by map check
	0x6D, // Boat
	0x6E, // Baboon Balloon
	0x6F, // Updraft
	0x70, // GB Dance
	0x71, // Key Dance
	0x72, // Crown Dance
	0x73, // Loss Dance
	0x74, // Victory Dance
	0x78, // Gorilla Grab
	0x79, // Learning Move // Note: Handled by map check
	0x7A, // Locked
	0x7B, // Locked
	0x7C, // Trapped (spider miniBoss)
	0x7D, // Klaptrap Kong (beaver bother) // Note: Handled by map check
	0x83, // Fairy Refill
	0x87, // Entering Portal
	0x88, // Exiting Portal
};

static const u16 speedrun_mode_permanent_flags[] = {
	367, // Diddy FTT
	368, // Lanky FTT
	385, // Kong Unlocked: DK
	6, // Kong Unlocked: Diddy
	70, // Kong Unlocked: Lanky
	66, // Kong Unlocked: Tiny
	117, // Kong Unlocked: Chunky
	369, // Tiny FTT
	370, // Chunky FTT
	42, // Japes: Cutscene by far W1 played // Diddy's help me cutscene
	93, // Aztec: Lanky's help me cutscene
	94, // Aztec: Tiny's help me cutscene
	140, // Factory: Chunky's help me cutscene
	//375, // Cranky's Lab Simian Slam Tutorial
	384, // Cranky's Lab Simian Slam Tutorial
	27, // Japes: Cutscene at the start played
	95, // Aztec: FT Cutscene
	92, // Aztec: Llama Cutscene
	194, // Galleon: First Time Cutscene
	257, // Fungi: First Time Cutscene
	282, // Caves: First Time Cutscene
	299, // Caves: Giant Kosha Cutscene
	349, // Castle: First Time Cutscene
	355, // Bananaporter FTT
	356, // Japes: Baboon Blast Cranky CS
	358, // Crown Pad FTT
	359, // T&S FTT (1)
	360, // Mini Monkey FTT
	361, // Hunky Chunky FTT
	362, // Orangstand Sprint FTT
	363, // Strong Kong FTT
	364, // Rainbow Coin FTT
	365, // Rambi FTT
	366, // Enguarde FTT
	372, // Snide's FTT
	376, // Wrinkly FTT
	377, // Camera/Shockwave
	378, // Training Grounds: Treehouse Squawks Cutscene
	382, // B. Locker FTT
	383, // Training Grounds: Barrels Spawned
	386, // Training Grounds: Dive Barrel Completed
	387, // Training Grounds: Vine Barrel Completed
	388, // Training Grounds: Orange Barrel Completed
	389, // Training Grounds: Barrel Barrel Completed
	390, // Isles: Escape Cutscene
	391, // Training Grounds: All Training Barrels Complete CS
};

static const u16 speedrun_mode_temporary_flags[] = {
	104, // Japes: Army Dillo Long Intro
	103, // Aztec: Dogadon Long Intro
	106, // Factory: Mad Jack Long Intro
	107, // Galleon: Puftoss Long Intro
	105, // Fungi: Dogadon Long Intro
	109, // Caves: Army Dillo Long Intro
	108, // Castle: Kut Out Long Intro
	101, // Caves: Beetle FT Long Intro
	102, // Aztec: Beetle FT Long Intro
};

static const u16 kong_unlocked_flags[] = {
	385, // Kong Unlocked: DK
	6, // Kong Unlocked: Diddy
	70, // Kong Unlocked: Lanky
	66, // Kong Unlocked: Tiny
	117, // Kong Unlocked: Chunky
};

static s32 inBadMapIndex = 0;
static s32 inBadMapCache = 0;
static u16 parentMapCache = 0;
static s32 storySkipLoaded = 0;

s32 inBadMap(void) {
	if (inBadMapIndex == current_map) {
		return inBadMapCache;
	}
	inBadMapCache = 0;
	inBadMapIndex = current_map;
	for (s32 i = 0; i < sizeof(bad_maps) / sizeof(bad_maps[0]); i++) {
		if (current_map == bad_maps[i]) {
			inBadMapCache = 1;
			break;
		}
	}
	return inBadMapCache;
}

s32 inBadMovementState(void) {
	if (gPlayerPointer) {
		for (s32 i = 0; i < sizeof(bad_movement_states) / sizeof(bad_movement_states[0]); i++) {
			if (gPlayerPointer->control_state == bad_movement_states[i]) {
				return 1;
			}
		}
		// Check for gorilla gone in effect bitfield
		if (gPlayerPointer->strong_kong_ostand_bitfield & 0x40) {
			return 1;
		}
	}
	return 0;
}

void tagDenied() {
	if (newly_pressed_input_copy.Buttons & R_JPAD || newly_pressed_input_copy.Buttons & L_TRIG) {
		playSound(152, 0x2FFF, 63.0f, 1.0f, 0, 0);
	} else if (newly_pressed_input_copy.Buttons & L_JPAD) {
		playSound(152, 0x2FFF, 63.0f, 1.0f, 0, 0);
	}
}

void tagAnywhere(void) {
	s32 _dest_character;
	s32 tagDirection;
	s8* Snide;

	// Main Menu
	if (current_map == 80) {
		// Remember Story Skip option through resets
		if (!storySkipLoaded) {
			story_skip = isFlagSet(35, 1);
			storySkipLoaded = 1;
		} else {
			setFlag(35, story_skip, 1);
		}

		if (story_skip) {
			// Start the player in DK Isles instead of Training Grounds
			*(s8 *)(0x80714547) = 34;
			*(s8 *)(0x8071455B) = 0;

			// Make T&S feeding faster
			*(u32 *)(0x806BE3E0) = 0;

			// Enable K. Lumsy cutscene compression
			*(u32 *)(0x806BDC98) = 0;

			// Set temporary flags
			for (s32 i = 0; i < sizeof(speedrun_mode_temporary_flags) / sizeof(speedrun_mode_temporary_flags[0]); i++) {
				setFlag(speedrun_mode_temporary_flags[i], 1 , 2);
			}
			// Set permanent flags
			for (s32 i = 0; i < sizeof(speedrun_mode_permanent_flags) / sizeof(speedrun_mode_permanent_flags[0]); i++) {
				setFlag(speedrun_mode_permanent_flags[i], 1, 0);
			}

			// Unlock moves
			for (s32 i = 0; i < 5; i++) {
				D_global_asm_807FC950[i].special_moves = 3;
				D_global_asm_807FC950[i].simian_slam = 3;
				D_global_asm_807FC950[i].ammo_belt = 2;
				D_global_asm_807FC950[i].weapon_bitfield = 7;
				D_global_asm_807FC950[i].instrument_bitfield = 15;
				D_global_asm_807FC950[i].instrument_energy = 20;
			}

			// Refill consumables
			D_global_asm_807FCC40.Melons = 3;
			D_global_asm_807FCC40.Health = 12;
			D_global_asm_807FCC40.Oranges = 20;
			D_global_asm_807FCC40.Film = 10;
			D_global_asm_807FCC40.StandardAmmo = 200;
			D_global_asm_807FCC40.Crystals = 20 * 150; // 150 ticks per crystal
		} else {
			// Start the player in Training Grounds
			*(s8 *)(0x80714547) = 176;
			*(s8 *)(0x8071455B) = 1;

			// Don't make T&S feeding faster
			*(u32 *)(0x806BE3E0) = 0x15600099;

			// Disable K. Lumsy cutscene compression
			*(u32 *)(0x806BDC98) = 0x14610012;
		}
	}

	// Snide's HQ
	if (current_map == 15) {
		if (story_skip) {
			// Snide's cutscene compression
			// The cutscene the game chooses is based on the parent map (the method used to detect which Snide's H.Q. you're in)
			// The shortest contraption cutscene is chosen with parent map 0
			// So we swap out the original parent map with 0 at the right moment to get short cutscenes
			// Then swap the original value back in at the right moment so that the player isn't taken back to test map when exiting Snide's H.Q.
			if (D_global_asm_807476F4 == 5) {
				if (D_global_asm_807476F0 == 199) {
					// Make a backup copy of the current parent map to restore later
					parentMapCache = D_global_asm_8076A172;
				} else if (D_global_asm_807476F0 == 200) {
					D_global_asm_8076A172 = 0;
				}
			} else if (D_global_asm_807476F4 == 2) {
				// Restore the backup copy of the parent map
				D_global_asm_8076A172 = parentMapCache;
			}

			// Snide turn in compression
			Snide = findActorWithType(184);
			if (Snide) {
				// Read the turn count (Snide + 0x232)
				if (Snide[0x232] != 0) {
					Snide[0x232] = 1;
				}
			}
		}
	}

	// Skip GB/Key dances
	if (story_skip) {
		switch (current_map) {
			// Disable dance skips on the following maps
			// If these are enabled, pause + exit is required to leave the map after collecting the GB
			case 14: // Aztec Beetle Race
			case 27: // Factory Car Race
			case 39: // Galleon Seal Race
			case 82: // Caves Beetle Race
			case 185: // Castle Car Race
				// Don't skip GB dances
				*(u32 *)(0x806EFB9C) = 0xA1EE0154; // Enable Movement Write
				*(u32 *)(0x806EFC1C) = 0x0C189E52; // Enable CS Play Function Call
				*(u32 *)(0x806EFB88) = 0x0C18539E; // Enable Animation Write Function Call
				*(u32 *)(0x806EFC0C) = 0xA58200E6; // Enable Change Rotation Write
				*(u32 *)(0x806EFBA8) = 0xA3000155; // Enable Control State Progress Zeroing
				break;
			default:
				// Skip GB dances
				*(u32 *)(0x806EFB9C) = 0; // Cancel Movement Write
				*(u32 *)(0x806EFC1C) = 0; // Cancel CS Play Function Call
				*(u32 *)(0x806EFB88) = 0; // Cancel Animation Write Function Call
				*(u32 *)(0x806EFC0C) = 0; // Cancel Change Rotation Write
				*(u32 *)(0x806EFBA8) = 0; // Cancel Control State Progress Zeroing
		}
	} else {
		// Don't skip GB dances
		*(u32 *)(0x806EFB9C) = 0xA1EE0154; // Enable Movement Write
		*(u32 *)(0x806EFC1C) = 0x0C189E52; // Enable CS Play Function Call
		*(u32 *)(0x806EFB88) = 0x0C18539E; // Enable Animation Write Function Call
		*(u32 *)(0x806EFC0C) = 0xA58200E6; // Enable Change Rotation Write
		*(u32 *)(0x806EFBA8) = 0xA3000155; // Enable Control State Progress Zeroing
	}

	// Map is loading
	if (D_global_asm_807FD888 > 0) {
		tagDenied();
		return;
	}
	// In tag barrel / paused
	if (global_properties_bitfield & 2) {
		tagDenied();
		return;
	}
	if (is_autowalking) {
		tagDenied();
		return;
	}
	if (is_cutscene_active) {
		tagDenied();
		return;
	}
	if (inBadMap()) {
		tagDenied();
		return;
	}
	// Don't allow tagging when D_global_asm_80754280 is open
	if (D_global_asm_80754280) {
		// Coloured Banana
		if (D_global_asm_80754280[0].hud_state) {
			tagDenied();
			return;
		}
		// Banana Coin
		if (D_global_asm_80754280[1].hud_state) {
			tagDenied();
			return;
		}
		// Crystal Coconut
		if (D_global_asm_80754280[5].hud_state) {
			tagDenied();
			return;
		}
		// GB Count (current_character_index)
		// Note: We can't add the bottom counter because it's always shown in lobbies
		if (D_global_asm_80754280[8].hud_state) {
			tagDenied();
			return;
		}
		// Banana Medal
		if (D_global_asm_80754280[10].hud_state) {
			tagDenied();
			return;
		}
		// Blueprint
		if (D_global_asm_80754280[12].hud_state) {
			tagDenied();
			return;
		}
		// Coloured Banana?
		if (D_global_asm_80754280[13].hud_state) {
			tagDenied();
			return;
		}
		// Banana Coin?
		if (D_global_asm_80754280[14].hud_state) {
			tagDenied();
			return;
		}
	}
	if (inBadMovementState()) {
		tagDenied();
		return;
	}
	if (current_character_index > 4) {
		tagDenied();
		return;
	}

	if (newly_pressed_input_copy.Buttons & R_JPAD || newly_pressed_input_copy.Buttons & L_TRIG) {
		tagDirection = 1;
	} else if (newly_pressed_input_copy.Buttons & L_JPAD) {
		tagDirection = -1;
	} else {
		return;
	}

	_dest_character = current_character_index + tagDirection;
	while (7) {
		// Wrap from DK to Chunky
		if (_dest_character < 0) {
			_dest_character = 4;
		}
		// Wrap from Chunky to DK
		if (_dest_character > 4) {
			_dest_character = 0;
		}
		// Any kong can be tagged in speed mode
		if (story_skip) {
			break;
		}
		// DK can always be tagged
		if (_dest_character == 0) {
			break;
		}
		// Check whether kong has been unlocked before allowing tag to them
		if (isFlagSet(kong_unlocked_flags[_dest_character], 0)) {
			break;
		}
		_dest_character += tagDirection;
	}

	// Without this, a choppy animation occurs if you tag DK -> DK
	if (_dest_character == current_character_index) {
		tagDenied();
		return;
	}

	if (gPlayerPointer) {
		// If the destination kong hasn't bought their gun, or if the current kong does not have their gun out
		if (((D_global_asm_807FC950[_dest_character].weapon_bitfield & 1) == 0) || (gPlayerPointer->was_gun_out == 0)) {
			gPlayerPointer->hand_state = 1;
			gPlayerPointer->was_gun_out = 0;
			// Without this, tags to and from Diddy mess up
			if (_dest_character == 1) {
				gPlayerPointer->hand_state = 0;
			}
		} else {
			gPlayerPointer->hand_state = 2;
			gPlayerPointer->was_gun_out = 1;
			// Without this, tags to and from Diddy mess up
			if (_dest_character == 1) {
				gPlayerPointer->hand_state = 3;
			}
		};
		gPlayerPointer->new_kong = _dest_character + 2;
		if (character_change_array) {
			character_change_array->action_type = 0x3B;
		}
		// Play successful tag SFX
		if (_dest_character == 0) {
			playSound(560, 0x4FFF, 63.0f, 1.0f, 0, 0);
		} else if (_dest_character == 1) {
			playSound(103, 0x4FFF, 63.0f, 1.0f, 0, 0);
		} else if (_dest_character == 2) {
			playSound(218, 0x4FFF, 63.0f, 1.0f, 0, 0);
		} else if (_dest_character == 3) {
			playSound(182, 0x4FFF, 63.0f, 1.0f, 0, 0);
		} else if (_dest_character == 4) {
			playSound(198, 0x4FFF, 63.0f, 1.0f, 0, 0);
		}
	}
}





void cFuncLoop(void) {
	// Enable stack trace upon crash
	*(s8 *)(0x807563B4) = 1;
	*(s32 *)(0x80731F78) = 0;

	// Set Arcade High Scores
	*(u32 *)(0x807467EC) = 999950;
	*(u32 *)(0x807467F0) = 999950;
	*(u32 *)(0x807467F4) = 999950;
	*(u32 *)(0x807467F8) = 999950;
	*(u32 *)(0x807467FC) = 999950;

	// Unlock Mystery Menu
	if (!isFlagSet(0, 1)) {
		for (s32 i = 0; i < 35; i++) {
			setFlag(i, 1, 1);
		}
	}

	tagAnywhere();
}