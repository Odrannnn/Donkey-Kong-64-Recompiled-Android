#ifndef DKCOOP_ANIMATION_ACTOR_H
#define DKCOOP_ANIMATION_ACTOR_H
#include "animation_policy.h"

extern void func_global_asm_80613CA8(Actor*, s16, f32, f32);
extern void func_global_asm_80614644(Actor*, AnimationStateUnk0*, f32);

typedef struct { unsigned cooldown; } CoopPose;

static void coop_pose_apply(CoopPose* pose, Actor* actor, int clip, float frame) {
    if (pose->cooldown) --pose->cooldown;
    if (!actor || !actor->animation_state || !actor->animation_state->unk0) return;
    ActorAnimationState* animation = actor->animation_state;
    // Never run a player's animation script, callback, weapon or sound commands.
    animation->unk68 = 0;
    animation->unk6C = 0;
    animation->unk70 = NULL;
    animation->unk74 = 0;
    // The network timeline supplies the frame. Prevent the engine advancing it again.
    animation->unk48 = 0.0f;
    animation->unk58 = 0.0f;
    if (clip < 0 || clip >= 2048 || !coop_animation_valid_frame(frame)) {
        if (animation->unk0->unk10 != -1) func_global_asm_80613CA8(actor, -1, 0.0f, 0.0f);
        return;
    }
    if (animation->unk0->unk10 != clip) {
        // Bound asset-cache churn, including retrying a full cache. No blending
        // keeps each remote proxy to one raw clip and releases the old clip.
        if (pose->cooldown) return;
        pose->cooldown = 3;
        func_global_asm_80613CA8(actor, (s16)clip, 0.0f, 0.0f);
    }
    AnimationStateUnk0* track = animation->unk0;
    if (!track->unk0 || track->unk10 != clip || track->unk0->unk12 == 0) return;
    func_global_asm_80614644(actor, track, coop_animation_frame(frame, track->unk0->unk12));
}
#endif
