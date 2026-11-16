// CreateMove hook ? full implementation
// Reconstructed from sub_1801C8650

#pragma once
#include <MinHook.h>
#include <cstdint>
#include "../core/console.h"
#include "../features/antiaim.h"
#include "../features/movement.h"
#include "../features/doubletap.h"
#include "../sdk/game.h"

namespace hooks {

// ??? Function pointer type for CreateMove ??????????????????????????????????
// CS2: bool CreateMove(void* thisptr, float frametime, void* cmd)
typedef bool(__fastcall* CreateMoveFn)(void*, float, void*);
static CreateMoveFn g_orig_create_move = nullptr;
static bool g_create_move_hooked = false;

// ??? UserCmd offsets (from pattern analysis) ??????????????????????????????
namespace usercmd {
    constexpr int COMMAND_NUMBER  = 0x08;
    constexpr int TICK_COUNT      = 0x0C;
    constexpr int VIEW_ANGLES     = 0x10;   // 3 floats (pitch, yaw, roll)
    constexpr int AIM_ANGLES      = 0x1C;   // 3 floats
    constexpr int FORWARDMOVE     = 0x28;
    constexpr int SIDEMOVE        = 0x2C;
    constexpr int UPMOVE          = 0x30;
    constexpr int BUTTONS         = 0x34;
    constexpr int IMPULSE         = 0x38;
    constexpr int WEAPONSELECT    = 0x3C;
    constexpr int RANDOM_SEED     = 0x44;
}

// ??? Button flags ??????????????????????????????????????????????????????????
enum Buttons : int {
    IN_ATTACK   = (1 << 0),
    IN_JUMP     = (1 << 1),
    IN_DUCK     = (1 << 2),
    IN_FORWARD  = (1 << 3),
    IN_BACK     = (1 << 4),
    IN_LEFT     = (1 << 5),
    IN_RIGHT    = (1 << 6),
    IN_MOVELEFT = (1 << 9),
    IN_MOVERIGHT= (1 << 10),
    IN_ATTACK2  = (1 << 11),
    IN_USE      = (1 << 23)
};

// ??? CreateMove detour ?????????????????????????????????????????????????????
static bool __fastcall CreateMoveDetour(void* thisptr, float frametime, void* cmd) {
    // Call original first
    bool result = g_orig_create_move(thisptr, frametime, cmd);

    if (!cmd)
        return result;

    // Read usercmd fields
    auto read_float = [&](int offset) -> float {
        return *(float*)((uint8_t*)cmd + offset);
    };
    auto read_int = [&](int offset) -> int {
        return *(int*)((uint8_t*)cmd + offset);
    };
    auto write_float = [&](int offset, float value) {
        *(float*)((uint8_t*)cmd + offset) = value;
    };
    auto write_int = [&](int offset, int value) {
        *(int*)((uint8_t*)cmd + offset) = value;
    };

    float view_pitch = read_float(usercmd::VIEW_ANGLES);
    float view_yaw   = read_float(usercmd::VIEW_ANGLES + 4);
    float view_roll  = read_float(usercmd::VIEW_ANGLES + 8);
    int   buttons    = read_int(usercmd::BUTTONS);
    int   cmd_num    = read_int(usercmd::COMMAND_NUMBER);

    // ??? Anti-Aim processing ???????????????????????????????????????????
    if (features::g_aa.enabled) {
        // Apply yaw offset
        if (features::g_aa.yaw_enabled) {
            float new_yaw = view_yaw + features::g_aa.yaw_offset;

            // Jitter
            if (features::g_aa.jitter_enabled) {
                static bool jitter_state = false;
                static int last_cmd = -1;

                if (cmd_num != last_cmd) {
                    jitter_state = !jitter_state;
                    last_cmd = cmd_num;
                }

                if (jitter_state) {
                    new_yaw += features::g_aa.jitter_range;
                } else {
                    new_yaw -= features::g_aa.jitter_range;
                }
            }

            // Spin
            if (features::g_aa.spin_enabled) {
                static float spin_angle = 0.0f;
                spin_angle += features::g_aa.spin_speed;
                if (spin_angle > 180.0f) spin_angle -= 360.0f;
                if (spin_angle < -180.0f) spin_angle += 360.0f;
                new_yaw = spin_angle;
            }

            // Clamp
            if (new_yaw > 180.0f) new_yaw -= 360.0f;
            if (new_yaw < -180.0f) new_yaw += 360.0f;

            write_float(usercmd::VIEW_ANGLES + 4, new_yaw);
        }

        // Pitch
        if (features::g_aa.pitch_enabled) {
            float new_pitch = 89.0f;
            if (features::g_aa.pitch_order == 1) {
                new_pitch = -89.0f;
            } else if (features::g_aa.pitch_order == 2) {
                new_pitch = 0.0f;
            }
            write_float(usercmd::VIEW_ANGLES, new_pitch);
        }
    }

    // ??? Movement processing ???????????????????????????????????????????
    if (features::g_movement.enabled) {
        // Bunny hop
        if (features::g_movement.bunny_hop) {
            // Hold jump when on ground
            buttons |= IN_JUMP;
        }

        // Auto strafe
        if (features::g_movement.air_strafe) {
            // Detect strafe direction from mouse
            if (buttons & IN_MOVELEFT) {
                write_float(usercmd::VIEW_ANGLES + 4, view_yaw - 90.0f);
            } else if (buttons & IN_MOVERIGHT) {
                write_float(usercmd::VIEW_ANGLES + 4, view_yaw + 90.0f);
            }
        }

        // Quick stop
        if (features::g_movement.quick_stop) {
            if (buttons & IN_ATTACK) {
                write_float(usercmd::FORWARDMOVE, 0.0f);
                write_float(usercmd::SIDEMOVE, 0.0f);
            }
        }
    }

    // ??? Double Tap ????????????????????????????????????????????????????
    if (features::g_dt.enabled) {
        // Simple DT: charge ticks, then release
        static int charge = 0;
        if (features::g_dt.state == features::DTState::Charging) {
            charge++;
            if (charge >= features::g_dt.max_charge_ticks) {
                features::g_dt.state = features::DTState::Ready;
                charge = 0;
            }
        }
    }

    // Write buttons back
    write_int(usercmd::BUTTONS, buttons);

    // Clamp angles
    float final_pitch = read_float(usercmd::VIEW_ANGLES);
    if (final_pitch > 89.0f)  write_float(usercmd::VIEW_ANGLES, 89.0f);
    if (final_pitch < -89.0f) write_float(usercmd::VIEW_ANGLES, -89.0f);

    return result;
}

// ??? Install / Remove ??????????????????????????????????????????????????????
bool InstallCreateMoveHook(void* target) {
    if (!target) {
        LOG_ERROR("[hook] CreateMove: no target");
        return false;
    }

    if (MH_CreateHook(target, (void*)CreateMoveDetour, (void**)&g_orig_create_move) != MH_OK) {
        LOG_ERROR("[hook] CreateMove: CreateHook failed");
        return false;
    }

    if (MH_EnableHook(target) != MH_OK) {
        LOG_ERROR("[hook] CreateMove: EnableHook failed");
        return false;
    }

    g_create_move_hooked = true;
    LOG_SUCCESS("[hook] CreateMove installed (0x%p)", target);
    return true;
}

} // namespace hooks
