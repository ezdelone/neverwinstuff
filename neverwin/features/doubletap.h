#pragma once
// Reconstructed from sub_1800C24C0 / sub_18047B4A0 / sub_1800F6940
// Double Tap state machine.

#include <cstdint>

namespace features {

enum class DTState {
    Idle = 0,
    Charging = 1,
    Ready = 2,
    Firing = 3,
    Recharging = 4,
    Blocked = 5
};

enum class DTFailReason {
    None = 0,
    ShootposMisprediction,
    Spread,
    HitchanceMisprediction,
    PredictionError,
    LagcompMisprediction,
    LagcompFailure,
    Occlusion,
    Unknown,
    Extrapolation,
    Death,
    PlayerDeath,
    EventTimeout
};

inline const char* ToString(DTFailReason r) {
    switch (r) {
        case DTFailReason::ShootposMisprediction:  return "shootpos misprediction";
        case DTFailReason::Spread:                 return "spread";
        case DTFailReason::HitchanceMisprediction: return "hitchance misprediction";
        case DTFailReason::PredictionError:        return "prediction error";
        case DTFailReason::LagcompMisprediction:   return "lagcomp misprediction";
        case DTFailReason::LagcompFailure:         return "lagcomp failure";
        case DTFailReason::Occlusion:              return "occlusion";
        case DTFailReason::Unknown:                return "unknown";
        case DTFailReason::Extrapolation:          return "extrapolation";
        case DTFailReason::Death:                  return "death";
        case DTFailReason::PlayerDeath:            return "player death";
        case DTFailReason::EventTimeout:           return "event timeout";
        default: return "none";
    }
}

struct DoubleTapState {
    DTState     state;
    int         charge_ticks;
    int         max_charge_ticks;
    DTFailReason last_fail;
    int         cmd_number;
    bool        can_fire;
    bool        waiting_for_shot;

    // config
    bool        enabled;         // ragebot.double_tap
    int         hitchance;
    int         min_damage;
    bool        force_shot;
    int         force_shot_hitchance;
};

extern DoubleTapState g_dt;

// ─── State machine (from sub_1800C24C0) ──────────────────────────────────
inline DTState UpdateDoubleTap(DoubleTapState& dt, bool can_attack, bool weapon_ready) {
    switch (dt.state) {
        case DTState::Idle:
            if (dt.enabled && weapon_ready) {
                dt.state = DTState::Charging;
                dt.charge_ticks = 0;
            }
            break;

        case DTState::Charging:
            dt.charge_ticks++;
            if (dt.charge_ticks >= dt.max_charge_ticks) {
                dt.state = DTState::Ready;
                dt.can_fire = true;
            }
            break;

        case DTState::Ready:
            if (can_attack && dt.enabled) {
                dt.state = DTState::Firing;
            } else if (!dt.enabled) {
                dt.state = DTState::Idle;
            }
            break;

        case DTState::Firing:
            dt.can_fire = false;
            dt.waiting_for_shot = true;
            dt.state = DTState::Recharging;
            dt.charge_ticks = 0;
            break;

        case DTState::Recharging:
            if (!dt.waiting_for_shot) {
                dt.state = DTState::Charging;
                dt.charge_ticks = 0;
            }
            break;

        case DTState::Blocked:
            if (weapon_ready) {
                dt.state = DTState::Charging;
            }
            break;
    }

    return dt.state;
}

} // namespace features

