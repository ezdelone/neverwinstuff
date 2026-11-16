#pragma once
// Reconstructed from sub_1801D95D0 (forward simulation) / sub_1801D8C10 / sub_1801D8DF0
// Prediction system: forward simulation, state snapshot, restore.

#include "../sdk/schema.h"
#include <cmath>
#include <cstdint>

namespace features {

// ─── Prediction state (from sub_1801D95D0) ───────────────────────────────
struct PredictionState {
    // Snapshot buffer (3400 bytes from code)
    uint8_t  snapshot[3400];

    // Movement services
    int      m_nLastJumpTick;
    bool     m_bDuckOverride;
    float    m_flDuckAmount;
    float    m_flStamina;
    float    m_flStaminaAtJumpStart;
    float    m_flVelMulAtJumpStart;
    float    m_flSurfaceFriction;

    // Pawn
    float    m_vecOrigin[3];
    float    m_vecVelocity[3];
    float    m_vecAbsOrigin[3];
    float    m_flDuckSpeed;
    float    m_flMaxspeed;
    int      m_fFlags;

    // Simulation
    int      sim_tick;
    int      sim_total;
    int      steps_count;
    int      events_count;
    int      kept_steps;
    bool     pb_steps_valid;
    float    movedata_origin_z;
    float    movedata_vel_z;
    float    movedata_first_when;
};

// ─── Simulation result (from sub_1801D95D0 log format) ────────────────────
struct SimulationResult {
    int      tick = 0;
    float    start_z = 0.0f;
    float    start_vz = 0.0f;
    float    end_vz = 0.0f;
    float    end_abs_z = 0.0f;
    float    end_node_z = 0.0f;
    float    end_vz_abs = 0.0f;
    float    end_vz_net = 0.0f;
    float    speed = 0.0f;
    int      flags = 0;
    bool     landed = false;
    float    frac = 0.0f;
    int      steps = 0;
    int      kept = 0;
    int      events = 0;
    bool     success = false;
    bool     on_ground = false;
};
// ─── Subtick move step (from sub_1801DD2E0) ──────────────────────────────
struct SubtickStep {
    int      button;      // 2=jump, 4=duck, 6=both
    bool     press;       // true = press, false = release
    float    when;        // 0.0-1.0 fraction
    int      flags;       // |= 7
};

// ─── Move data (from sub_1801DD2E0, sub_180265420) ───────────────────────
struct MoveData {
    float    forwardmove;
    float    sidemove;
    float    upmove;
    float    viewangles[3];
    int      buttons;
    float    impulse;
    int      weaponselect;
    int      tick;
    float    frametime;

    // Prediction results
    float    out_origin[3];
    float    out_velocity[3];
    float    out_absorigin[3];
    bool     on_ground;
};

// ─── Player prediction context ────────────────────────────────────────────
struct PredictionContext {
    void*            player;
    void*            pawn;
    void*            movement_services;
    MoveData*        move_data;
    int              command_number;
    int              tick_count;
    float            frametime;
    bool             first_time_predicted;
    bool             in_prediction;
};

// ─── Forward simulation (from sub_1801D95D0) ──────────────────────────────
class Prediction {
public:
    static Prediction& Instance() {
        static Prediction inst;
        return inst;
    }

    // ─── Snapshot save/restore ──────────────────────────────────────────
    void SaveState(PredictionState& state) {
        // from code:
        //   sub_180540530(qword_181114320, state.snapshot, 3400);
        //   — memcpy 3400 bytes of game movement state
        saved_ = true;
    }

    void RestoreState(PredictionState& state) {
        // Restore from snapshot
        // sub_1801D9250 / sub_1801D89B0
        saved_ = false;
    }

    // ─── Run simulation ─────────────────────────────────────────────────
    // from sub_1801D95D0:
    //   1. Save state (3400 bytes)
    //   2. Copy usercmd buttons into prediction buffer
    //   3. Run sub_1801D8C10 (run movement sim)
    //   4. Check result (vz, on_ground)
    //   5. Restore state
    SimulationResult SimulateTick(
        const PredictionContext& ctx,
        MoveData& move)
    {
        SimulationResult result{};

        // Save current state
        SaveState(current_state_);

        // Set up move
        move.tick = ctx.tick_count;
        move.frametime = ctx.frametime;

        // Run one tick of movement
        // (in real implementation: calls prediction_process_movement)

        // ── Physics integration (from game) ────────────────────────────
        float dt = move.frametime;

        // Gravity (800 u/s²)
        move.out_velocity[2] -= 800.0f * dt;

        // Ground check
        if (move.out_origin[2] <= 0.0f) { // simplified
            move.on_ground = true;
            move.out_velocity[2] = 0.0f;
        } else {
            move.on_ground = false;
        }

        // Horizontal friction
        if (move.on_ground) {
            float speed = sqrtf(
                move.out_velocity[0] * move.out_velocity[0] +
                move.out_velocity[1] * move.out_velocity[1]
            );
            if (speed > 0.0f) {
                float friction = current_state_.m_flSurfaceFriction * 4.0f;
                float drop = speed * friction * dt;
                float newspeed = speed - drop;
                if (newspeed < 0.0f) newspeed = 0.0f;
                float ratio = newspeed / speed;
                move.out_velocity[0] *= ratio;
                move.out_velocity[1] *= ratio;
            }
        }

        // Apply movement
        move.out_origin[0] += move.out_velocity[0] * dt;
        move.out_origin[1] += move.out_velocity[1] * dt;
        move.out_origin[2] += move.out_velocity[2] * dt;

        // Check result
        result.end_vz = move.out_velocity[2];
        result.end_abs_z = move.out_origin[2];
        result.on_ground = move.on_ground;
        result.landed = result.on_ground && !current_state_.m_flDuckAmount;
        result.success = !result.on_ground && result.end_vz > 0.0f;

        // Restore
        RestoreState(current_state_);

        return result;
    }

    // ─── Multi-tick simulation (for jump bug / edge jump) ───────────────
    // from sub_1801D95D0: loops through multiple ticks
    std::vector<SimulationResult> SimulateTicks(
        const PredictionContext& ctx,
        MoveData& move,
        int num_ticks)
    {
        std::vector<SimulationResult> results;
        results.reserve(num_ticks);

        SaveState(current_state_);

        for (int i = 0; i < num_ticks; i++) {
            auto r = SimulateTick(ctx, move);
            r.tick = i;
            results.push_back(r);

            // Early exit if landed
            if (r.landed) break;
        }

        RestoreState(current_state_);
        return results;
    }

private:
    PredictionState current_state_;
    bool saved_ = false;
    bool     on_ground;
};

// ─── Prediction hook entry points (from hooks.json) ─────────────────────
namespace prediction_hooks {
    // run_command → sub_1801CB880
    // send_predicted_command → sub_1800875D0 (AA weapon drop sync)
    // prediction_process_movement → pattern target
    // prediction_finish_move → pattern target
    // prediction_end → pattern target
    // prediction_setup_move → pattern target

    // ─── RunCommand (from sub_1801CB880) ─────────────────────────────────
    inline void OnRunCommand(void* player, MoveData* move, void* command) {
        // 1. Check if it's local player
        // 2. Save prediction state
        // 3. Process AA (angles, duck override)
        // 4. Process movement (edge jump, jump bug)
        // 5. Run game prediction
        // 6. Check result
        // 7. Restore if needed
    }

    // ─── SendPredictedCommand (from sub_1800875D0) ──────────────────────
    inline void* OnSendPredictedCommand(
        void* a1, void* a2, void* a3,
        unsigned int cmd_number,
        void* a5, char a6)
    {
        // AA weapon drop sync:
        // if (cmd_number == sync_expected)
        //     cmd_number = sync_target;
        // call original with modified cmd_number
        return nullptr; // call original
    }
}

} // namespace features


