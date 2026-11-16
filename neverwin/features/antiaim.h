#pragma once
// Reconstructed from sub_180087980 / sub_180084BD0 / sub_180086720
// Anti-Aim core: pitch/yaw override, desync, use-turn, weapon drop sync.

#include "../sdk/schema.h"
#include "../sdk/game.h"
#include <cmath>

namespace features {

// ─── AA state (from sub_180087980) ───────────────────────────────────────
struct AntiAimState {
    // xmmword_180CF9FA0
    float  current_yaw;
    float  current_pitch;
    int    extra;

    // dword_180CCFC10
    int    last_cmd;

    // dword_180CCFB34
    int    last_drop_cmd;

    // dword_180963590/594
    int    sync_target;
    int    sync_expected;

    // byte_180CA1BDD
    bool   active;

    // config flags (from config_tree)
    bool   enabled;
    bool   leg_movement_sliding;
    bool   pitch_enabled;
    bool   yaw_enabled;
    bool   mouse_override;
    bool   avoid_backstab;
    bool   auto_adjust;
    bool   hide_shots;
    bool   jitter_enabled;
    bool   jitter_disable_air;
    bool   jitter_disable_override;
    bool   spin_enabled;
    float  yaw_offset;
    float  jitter_range;
    float  spin_speed;
    int    pitch_order;
    int    jitter_type;
    int    manual_mode;
};

extern AntiAimState g_aa;

// ─── Use mode (from sub_180086720) ────────────────────────────────────────
enum class UseMode {
    None = 0,
    Hostage = 1,
    Bomb = 2
};

// ─── Restore reasons (from strings) ───────────────────────────────────────
enum class RestoreReason {
    None,
    DisabledOnUse,
    ValveServer,
    PitchLimitBelow90,
    AAYawWithin70,
    AAYawWithin120,
    FlipOutside90180,
    FlipAbovePitchLimit,
    ViewPitchNotAbove0,
    AAPitchBelow45
};

inline const char* ToString(RestoreReason r) {
    switch (r) {
        case RestoreReason::DisabledOnUse:       return "restore (disabled on use)";
        case RestoreReason::ValveServer:         return "restore (valve server)";
        case RestoreReason::PitchLimitBelow90:   return "restore (pitch limit below 90)";
        case RestoreReason::AAYawWithin70:       return "restore (aa yaw within 70 of view)";
        case RestoreReason::AAYawWithin120:      return "restore (aa yaw within 120 of view)";
        case RestoreReason::FlipOutside90180:    return "restore (flip outside 90..180)";
        case RestoreReason::FlipAbovePitchLimit: return "restore (flip above pitch limit)";
        case RestoreReason::ViewPitchNotAbove0:  return "restore (view pitch not above 0)";
        case RestoreReason::AAPitchBelow45:      return "restore (aa pitch below 45)";
        default: return "none";
    }
}

// ─── Pitch/Yaw validation (from sub_180087980 core) ──────────────────────
struct AngleInput {
    float yaw;
    float pitch;
    float view_yaw;
    float view_pitch;
    bool  flip;
    bool  use_mode;
    int   use_mode_type;
};

inline RestoreReason ValidateAngles(const AngleInput& in) {
    // pitch < 45 → skip
    if (in.pitch < 45.0f && !in.flip)
        return RestoreReason::AAPitchBelow45;

    // pitch limit below 90
    if (in.pitch < 90.0f && !in.flip)
        return RestoreReason::PitchLimitBelow90;

    // use mode
    if (in.use_mode)
        return RestoreReason::DisabledOnUse;

    // aa yaw within 70/120 of view
    float yaw_diff = fabsf(in.yaw - in.view_yaw);
    if (yaw_diff > 120.0f)
        return RestoreReason::AAYawWithin120;
    if (yaw_diff > 70.0f)
        return RestoreReason::AAYawWithin70;

    // flip validation
    if (in.flip) {
        if (in.pitch > 180.0f || in.pitch < 90.0f)
            return RestoreReason::FlipOutside90180;
        if (in.pitch > 90.0f)
            return RestoreReason::FlipAbovePitchLimit;
    }

    // view pitch
    if (in.view_pitch <= 0.0f)
        return RestoreReason::ViewPitchNotAbove0;

    return RestoreReason::None;
}

// ─── Head-yaw correction (from sub_180084BD0) ────────────────────────────
// Returns correction delta for yaw based on lag record head position
inline float CalculateHeadYawCorrection(
    float head_origin[2],
    float prev_head_origin[2],
    float current_yaw)
{
    // from code:
    //   v25 = (head.y - prev.y)^2 + (head.x - prev.x)^2
    //   v26 = sqrt(v25)
    //   v29 = atan2(head.y, head.x) * 57.295776  (rad → deg)
    //   delta = expected - current
    float dx = head_origin[0] - prev_head_origin[0];
    float dy = head_origin[1] - prev_head_origin[1];
    float dist = sqrtf(dx*dx + dy*dy);

    if (dist < 0.0001f)
        return 0.0f;

    float head_yaw = atan2f(head_origin[1], head_origin[0]) * 57.295776f;
    return head_yaw - current_yaw;
}

// ─── Use turn (from sub_180086720) ────────────────────────────────────────
inline float CalculateUseTurn(
    float current_yaw,
    float target_yaw,
    UseMode mode)
{
    // limit ±5.11° per tick (from code)
    const float max_turn = 5.11f;
    float delta = target_yaw - current_yaw;

    if (delta > max_turn)  delta = max_turn;
    if (delta < -max_turn) delta = -max_turn;

    return delta;
}

// ─── Weapon drop sync (from sub_180087350/5D0) ───────────────────────────
struct WeaponDropSync {
    int  aa_cmd;         // dword_180CCFC10
    int  last_drop_cmd;  // dword_180CCFB34
    int  sync_target;    // dword_180963590
    int  sync_expected;  // dword_180963594
    bool should_keep_aim;

    bool ShouldForceDrop(int cmd) {
        // |cmd - aa_cmd| <= 1 AND |cmd - last_drop| >= 2
        int d1 = abs(cmd - aa_cmd);
        int d2 = abs(cmd + 1 - last_drop_cmd);
        return d1 <= 1 && d2 >= 2;
    }

    int GetSyncCommand(int input_cmd) {
        if (input_cmd == sync_expected) {
            return sync_target;
        }
        return input_cmd;
    }
};

} // namespace features

