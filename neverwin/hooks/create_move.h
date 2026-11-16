#pragma once
// Reconstructed from sub_1801C8650 (create_move hook)
// Full usercmd processing pipeline.

#include "../sdk/schema.h"
#include "../sdk/game.h"
#include "features/antiaim.h"
#include "features/movement.h"
#include "features/doubletap.h"
#include "features/autowall.h"

#include <cmath>
#include <cstdint>

namespace hooks {

// ─── Button flags (from sub_1801DD2E0, sub_1802A2DA0) ─────────────────────
enum UserCmdButtons : int {
    IN_ATTACK      = (1 << 0),
    IN_JUMP        = (1 << 1),
    IN_DUCK        = (1 << 2),
    IN_FORWARD     = (1 << 3),
    IN_BACK        = (1 << 4),
    IN_LEFT        = (1 << 5),
    IN_RIGHT       = (1 << 6),
    IN_MOVELEFT    = (1 << 9),
    IN_MOVERIGHT   = (1 << 10),
    IN_ATTACK2     = (1 << 11),
    IN_SCORE       = (1 << 16),
    IN_BULLRUSH    = (1 << 22),
    IN_USE         = (1 << 23)
};

// ─── CUserCmd (reconstructed from sub_1801D95D0, sub_180267880) ───────────
struct UserCmd {
    void*    cmd_base;        // vtable / base pointer
    int      command_number;  // +8
    int      tick_count;      // +12
    float    view_angles[3];  // +16..28
    float    aimangles[3];    // +28..40
    float    forwardmove;     // +40
    float    sidemove;        // +44
    float    upmove;          // +48
    int      buttons;         // +52
    uint8_t  impulse;         // +56
    int      weaponselect;    // +60
    int      weaponsubtype;   // +64
    int      random_seed;     // +68
    int      server_tick;     // +72 (from sub_1801DD2E0: v4+32 = steps)
};

// ─── CreateMove context ──────────────────────────────────────────────────
struct CreateMoveCtx {
    UserCmd*              cmd;
    void*                 local_player;
    sdk::PlayerPawn*      pawn;
    sdk::MovementServices* movement;
    sdk::WeaponVData*     weapon_vdata;
    float                 frametime;
    int                   tick;
    bool                  send_packet;
    bool                  first_cmd_of_tick;
};

// ─── Sub-tick move data (from sub_1801DD2E0, sub_180265420) ───────────────
struct SubtickMoveStep {
    int      button;       // which button (2=jump, 4=duck, 6=both)
    bool     press;        // true = press, false = release
    float    when;         // 0.0-1.0 fraction of tick
    int      flags;        // |= 7
};

// ─── Full pipeline order (from sub_1801C8650 decompilation) ──────────────
// 1.  Get usercmd
// 2.  Check aimbot conditions
// 3.  Anti-Aim processing
// 4.  Movement (quick stop, edge jump, air strafe, bhop)
// 5.  Double tap state update
// 6.  Subtick move steps (button scheduling)
// 7.  Final view angles write
// 8.  Final move write
// 9.  Send packet decision

inline void ProcessCreateMove(CreateMoveCtx& ctx) {
    if (!ctx.cmd || !ctx.pawn)
        return;

    // ── Phase 1: Anti-Aim ─────────────────────────────────────────────
    features::AntiAimState& aa = features::g_aa;

    if (aa.enabled) {
        // Validate angles
        features::AngleInput input{};
        input.yaw = ctx.cmd->view_angles[1];
        input.pitch = ctx.cmd->view_angles[0];
        input.view_yaw = ctx.cmd->aimangles[1];
        input.view_pitch = ctx.cmd->aimangles[0];
        input.use_mode = ctx.movement && (ctx.movement->m_bIsGrabbingHostage || ctx.movement->m_bDuckOverride);

        auto reason = features::ValidateAngles(input);
        if (reason != features::RestoreReason::None) {
            // Restore original angles
            ctx.cmd->view_angles[0] = ctx.cmd->aimangles[0];
            ctx.cmd->view_angles[1] = ctx.cmd->aimangles[1];
            ctx.cmd->view_angles[2] = ctx.cmd->aimangles[2];
        }
    }

    // ── Phase 2: Movement ────────────────────────────────────────────
    features::MovementConfig& mc = features::g_movement;

    if (mc.enabled) {
        features::MovementState state{};
        state.on_ground = !(ctx.cmd->buttons & UserCmdButtons::IN_JUMP);
        state.velocity[0] = 0.0f;
        state.velocity[1] = 0.0f;
        state.velocity[2] = 0.0f;
        state.origin[2] = 0.0f;

        if (mc.edge_jump || mc.jumpbug) {
            auto jump_type = features::DetermineJumpType(
                state.on_ground,
                ctx.cmd->buttons & UserCmdButtons::IN_JUMP,
                false,
                250.0f,
                false
            );

            switch (jump_type) {
                case features::JumpType::EdgeJump:
                    ctx.cmd->buttons |= UserCmdButtons::IN_JUMP;
                    break;
                case features::JumpType::MiniJump:
                    ctx.cmd->buttons |= UserCmdButtons::IN_JUMP;
                    break;
                case features::JumpType::EdgeJumpPress:
                    ctx.cmd->buttons |= UserCmdButtons::IN_JUMP;
                    break;
                default:
                    break;
            }
        }

        if (mc.air_strafe || mc.strafe_assist) {
            auto strafe = features::CalculateStrafe(
                state,
                250.0f,
                ctx.cmd->buttons & UserCmdButtons::IN_FORWARD,
                ctx.cmd->buttons
            );
            ctx.cmd->view_angles[1] += strafe.yaw_shift;
            ctx.cmd->forwardmove = strafe.move[0];
            ctx.cmd->sidemove = strafe.move[1];
        }

        if (mc.quick_stop) {
            // Stop in air / on land
            ctx.cmd->forwardmove = 0.0f;
            ctx.cmd->sidemove = 0.0f;
        }
    }

    // ── Phase 3: Double Tap ──────────────────────────────────────────
    features::DoubleTapState& dt = features::g_dt;

    if (dt.enabled) {
        bool weapon_ready = ctx.weapon_vdata && ctx.weapon_vdata->m_iClip1 > 0;
        bool can_attack = !(ctx.cmd->buttons & UserCmdButtons::IN_ATTACK);

        features::UpdateDoubleTap(dt, can_attack, weapon_ready);

        if (dt.state == features::DTState::Ready) {
            ctx.cmd->buttons |= UserCmdButtons::IN_ATTACK;
        }
    }

    // ── Phase 4: Subtick moves ───────────────────────────────────────
    // Queue button presses/releases at specific fractions of the tick
    // from sub_180265420:
    //   step->button = 2 (jump) or 4 (duck) or 6 (both)
    //   step->press = true/false
    //   step->when = 0.0-1.0
    //   step->flags |= 7

    // ── Phase 5: Final clamp ─────────────────────────────────────────
    // Clamp view angles to valid range
    for (int i = 0; i < 3; i++) {
        if (ctx.cmd->view_angles[i] > 180.0f)  ctx.cmd->view_angles[i] = 180.0f;
        if (ctx.cmd->view_angles[i] < -180.0f) ctx.cmd->view_angles[i] = -180.0f;
    }
    if (ctx.cmd->view_angles[0] > 89.0f)  ctx.cmd->view_angles[0] = 89.0f;
    if (ctx.cmd->view_angles[0] < -89.0f) ctx.cmd->view_angles[0] = -89.0f;
}

} // namespace hooks
