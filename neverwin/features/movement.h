#pragma once
// Reconstructed from sub_1801DD2E0 / sub_1801D95D0 / sub_1801EACB0
// Movement: edge jump, jump bug, forward simulation, strafe.

#include "../sdk/schema.h"
#include <cmath>

namespace features {

// ─── Jump types (from sub_1801DD2E0) ──────────────────────────────────────
enum class JumpType {
    None = 0,
    Normal = 1,
    EdgeJump = 2,
    EdgeJumpPress = 3,
    EdgeJumpRelease = 4,
    MiniJump = 5
};

// ─── Movement config (from config_tree) ───────────────────────────────────
struct MovementConfig {
    bool  enabled = false;
    // movement.* (visuals)
    bool  jumpbug;
    bool  edgebug;
    bool  edge_jump;
    bool  long_jump;
    bool  mini_jump;
    bool  ladder_jump;
    bool  null_strafe;
    bool  fireman;
    bool  slide_walk;
    bool  pixel_surf;
    bool  texture_bug;
    bool  fast_ladder;
    bool  mouse_limiter;
    bool  strafe_assist;
    bool  quick_stop;
    bool  air_strafe;
    bool  bunny_hop;

    // nl_movement.* (rage)
    bool  nl_jump_bug;
    bool  nl_edge_jump;
    bool  nl_fast_ladder;
    bool  nl_strafe_assist;
    bool  nl_standalone_quick_stop;

    // edgebug options
    int   edgebug_max_ticks;      // dword_180920CF0 (1-32)
    bool  edgebug_assisted;
    float edgebug_fov;            // dword_180920CF8
    bool  edgebug_silent;
    int   edgebug_passes;         // dword_180920CEC (1-5)
    bool  edgebug_jump_steps;

    // long jump
    bool  long_jump_block_ws;

    // bunny hop
    int   bhop_press_ms;   // 80ms
    int   bhop_release_ms; // 80ms
};

extern MovementConfig g_movement;

// ─── Movement state snapshot (from sub_1801E67B0) ──────────────────────────
struct MovementState {
    bool  on_ground;
    float velocity[3];
    float origin[3];
    float surface_friction;
    bool  ducking;
    bool  duck_override;
    float duck_amount;
    float stamina;
    int   last_jump_tick;
    float stamina_at_jump;
    float velmul_at_jump;
};

// ─── Edge jump decision (from sub_1801DD2E0) ──────────────────────────────
inline JumpType DetermineJumpType(
    bool on_ground,
    bool jump_pressed,
    bool jump_held,
    float speed,
    bool ramp_slide)
{
    // from code:
    //   if (!on_ground && speed > 0 && (jump_pressed || jump_held))
    //     if (ramp_slide) return EdgeJump;
    //     else return Normal;
    //   if (jump_pressed && !on_ground)
    //     return MiniJump;
    //   if (on_ground && jump_pressed)
    //     return EdgeJumpPress;

    if (!on_ground) {
        if (speed > 0.0f) {
            if (ramp_slide) return JumpType::EdgeJump;
            return JumpType::Normal;
        }
        if (jump_pressed) return JumpType::MiniJump;
    }

    if (on_ground && jump_pressed) {
        return JumpType::EdgeJumpPress;
    }

    return JumpType::None;
}

// ─── Jump bug verification (from sub_1801EACB0) ──────────────────────────
struct JumpBugResult {
    bool  success;
    float vz;
    float stamina_at_jump;
    float velmul_at_jump;
    bool  on_ground;
    bool  ramp;
};

inline JumpBugResult VerifyJumpBug(const MovementState& state) {
    JumpBugResult r{};
    r.vz = state.velocity[2];
    r.stamina_at_jump = state.stamina_at_jump;
    r.velmul_at_jump = state.velmul_at_jump;
    r.on_ground = state.on_ground;
    r.ramp = state.surface_friction < 1.0f;

    // from code:
    //   if (v18 & 1) → FAIL (standing on ground)
    //   else if (vz > 0) → success
    r.success = !state.on_ground && state.velocity[2] > 0.0f;
    return r;
}

// ─── Strafe calculation (from sub_180267880) ──────────────────────────────
struct StrafeResult {
    float  view_angles[3];
    float  move[2];
    float  speed_gain;
    float  yaw_shift;
    bool   snap;
};

inline StrafeResult CalculateStrafe(
    const MovementState& state,
    float target_speed,
    bool  want_stop,
    int   buttons)
{
    StrafeResult r{};

    float current_speed = sqrtf(
        state.velocity[0] * state.velocity[0] +
        state.velocity[1] * state.velocity[1]
    );

    // from code: strafe speed gain based on angle
    if (current_speed < target_speed && !want_stop) {
        // air strafe: turn view to gain speed
        float optimal_angle = 0.0f;
        if (current_speed > 0.0f && current_speed < target_speed) {
            optimal_angle = acosf(current_speed / target_speed) * 57.295776f;
            // clamp
            if (optimal_angle > 90.0f) optimal_angle = 90.0f;
            if (optimal_angle < 0.0f)  optimal_angle = 0.0f;
        }
        r.yaw_shift = optimal_angle;
        r.speed_gain = target_speed - current_speed;
    } else if (want_stop) {
        r.yaw_shift = 180.0f;
        r.speed_gain = -current_speed;
    }

    r.move[0] = 0.0f;
    r.move[1] = 0.0f;

    // buttons: IN_FORWARD=1, IN_BACK=2, IN_MOVELEFT=4, IN_MOVERIGHT=8
    if (buttons & 1) r.move[0] = 1.0f;  // forward
    if (buttons & 2) r.move[0] = -1.0f; // back
    if (buttons & 4) r.move[1] = -1.0f; // left
    if (buttons & 8) r.move[1] = 1.0f;  // right

    return r;
}

// ─── Forward simulation step (from sub_1801D95D0) ──────────────────────────
struct SimStep {
    int   tick;
    float start_z;
    float start_vz;
    float end_abs_z;
    float end_node_z;
    float end_vz_abs;
    float end_vz_net;
    float speed;
    int   flags;
    bool  landed;
    float frac;
    int   steps;
    int   kept;
    int   events;
};

inline SimStep SimulateJumpTick(
    const MovementState& state,
    int tick,
    float gravity = 800.0f)
{
    SimStep s{};
    s.tick = tick;
    s.start_z = state.origin[2];
    s.start_vz = state.velocity[2];
    s.speed = sqrtf(
        state.velocity[0] * state.velocity[0] +
        state.velocity[1] * state.velocity[1]
    );

    // integrate velocity
    float new_vz = state.velocity[2] - gravity * 0.015625f; // 1 tick @ 64 tick
    s.end_vz_abs = new_vz;
    s.end_vz_net = state.velocity[2];
    s.end_abs_z = state.origin[2] + new_vz * 0.015625f;

    return s;
}

} // namespace features

