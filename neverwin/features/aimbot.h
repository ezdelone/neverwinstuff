#pragma once
// Reconstructed from sub_1802A21E0 (target selection) / sub_1802ABD00 (shot decision)
// / sub_1800F5F60 / sub_1800F6940 (config)

#include "autowall.h"
#include "../sdk/schema.h"
#include <cmath>
#include <cstdint>
#include <vector>

namespace features {

// ─── Hitbox IDs (from sub_18029C4F0: hitbox <= 8 = head) ──────────────────
enum Hitbox {
    HITBOX_HEAD        = 0,
    HITBOX_NECK        = 1,
    HITBOX_UPPER_CHEST = 2,
    HITBOX_CHEST       = 3,
    HITBOX_LOWER_CHEST = 4,
    HITBOX_STOMACH     = 5,
    HITBOX_PELVIS      = 6,
    HITBOX_LEFT_THIGH  = 7,
    HITBOX_LEFT_CALF   = 8,    // boundary — 0-8 = "head group"
    HITBOX_RIGHT_THIGH = 9,
    HITBOX_RIGHT_CALF  = 10,
    HITBOX_LEFT_FOOT   = 11,
    HITBOX_RIGHT_FOOT  = 12,
    HITBOX_LEFT_ARM    = 13,
    HITBOX_LEFT_HAND   = 14,
    HITBOX_RIGHT_ARM   = 15,
    HITBOX_RIGHT_HAND  = 16
};

// ─── Target record (from rage_target_process.c / sub_1802A21E0) ───────────
struct TargetRecord {
    void*    entity;
    void*    pawn;
    int      entity_index;
    int      hitbox_id;
    float    hitbox_origin[3];
    float    head_origin[3];      // for AA desync correction
    float    origin[3];
    float    velocity[3];
    int      health;
    int      armor;
    bool     has_helmet;
    float    simulation_time;
    float    interpolation;
    int      lag_records;         // how many lag comp records
    int      best_record;         // index of best lag record
    float    damage;
    float    distance;
    float    fov_to_crosshair;
    bool     visible;
    bool     behind_wall;
    bool     immune;              // m_bGunGameImmunity
    bool     scoped;
    int      threat_level;

    // Scoring
    float    score;
    bool     selected;
};

// ─── Point record (from sub_18029C4F0) ─────────────────────────────────────
struct PointRecord {
    void*    entity;
    int      hitbox;
    float    origin[3];
    float    center[3];
    float    damage;          // autowall result
    bool     accepted;
    bool     confirmed;
    int      priority;
};

// ─── Aimbot config (from sub_1800F5F60, sub_1800F6940) ───────────────────
struct RageConfig {
    // Main
    bool     enabled;
    bool     silent_aim;
    bool     automatic_fire;
    bool     limiters;
    bool     refine_shot;
    float    field_of_view;        // 0-180
    bool     auto_scope;
    bool     auto_stop;
    bool     stop_in_air;
    bool     stop_on_land;
    bool     stop_between_shots;

    // Selection
    bool     hitboxes[17];         // head, chest, stomach, etc.
    float    extrapolation;        // 0-100
    int      hit_chance;          // 0-100
    int      min_damage;          // 0-100
    bool     quick_stop;
    bool     quick_scope;

    // Overrides
    bool     hit_chance_override;
    int      hit_chance_override_value;
    bool     min_damage_override;
    int      min_damage_override_value;

    // Other
    int      history;             // backtrack ticks
    bool     delay_shot;
    int      delay_damage;
    bool     remove_spread;
    bool     duck_peek_assist;
    bool     quick_peek_assist;
    bool     double_precision;
    bool     double_tap;
    bool     force_shot;
    int      force_shot_hit_chance;

    // Groups (per-weapon)
    int      groups_count;
    bool     group_enabled[8];
    int      group_hit_chance[8];
    int      group_min_damage[8];
    int      group_point_scale[8];
};

// ─── Selection criteria ────────────────────────────────────────────────────
enum class TargetPriority {
    Distance,      // closest first
    Damage,        // highest damage first
    FOV,           // closest to crosshair
    Threat,        // highest threat first
    Health,        // lowest health first
    Headshot       // prefer head
};

// ─── Target selection (from sub_1802A21E0) ────────────────────────────────
inline std::vector<TargetRecord> SelectTargets(
    const std::vector<void*>& entities,
    const RageConfig& config,
    float view_origin[3],
    float view_angles[3])
{
    std::vector<TargetRecord> targets;

    for (void* entity : entities) {
        TargetRecord t{};
        t.entity = entity;

        // ── Filter conditions ─────────────────────────────────────────
        // m_bGunGameImmunity → skip
        // m_lifeState != 0 → skip
        // m_iHealth <= 0 → skip
        // m_iTeamNum == local_team → skip (unless friendly fire)

        // ── Calculate metrics ──────────────────────────────────────────
        // distance to player
        // fov to crosshair
        // visibility check (trace)
        // autowall damage

        // ── Hitbox selection ──────────────────────────────────────────
        // iterate through configured hitboxes
        // for each: calculate point, check damage

        // ── Score ─────────────────────────────────────────────────────
        // based on: damage, distance, fov, headshot availability

        targets.push_back(t);
    }

    // Sort by score (highest first)
    return targets;
}

// ─── Shot decision (from sub_1802ABD00) ────────────────────────────────────
struct ShotDecision {
    bool     can_fire;
    bool     should_fire;
    float    hit_chance;
    float    spread;
    float    inaccuracy;
    float    damage;
    void*    target;
    int      hitbox;
    float    aim_point[3];
    int      delay_ticks;
    const char* reason;
};

inline ShotDecision DecideShot(
    const TargetRecord& target,
    const RageConfig& config,
    const sdk::WeaponVData& vdata,
    float view_origin[3],
    float view_angles[3])
{
    ShotDecision d{};

    // ── Hit chance calculation ────────────────────────────────────────
    // from weapon_calculate_spread / get_spread / get_inaccuracy
    // spread = weapon spread + movement inaccuracy + air inaccuracy
    float spread = 0.0f;       // get_spread()
    float inacc = 0.0f;        // get_inaccuracy()

    // hit chance = 1 - (spread_angle / fov_to_target)
    float fov_to_target = atan2f(
        target.distance,
        sqrtf(target.fov_to_crosshair)
    );

    if (fov_to_target > 0.0f) {
        d.hit_chance = 1.0f - (spread + inacc) / fov_to_target;
    } else {
        d.hit_chance = 0.0f;
    }

    d.hit_chance *= 100.0f; // to percentage

    // ── Damage check ──────────────────────────────────────────────────
    d.damage = target.damage;
    if (d.damage < (float)config.min_damage) {
        d.can_fire = false;
        d.reason = "min damage not met";
        return d;
    }

    // ── Hit chance check ──────────────────────────────────────────────
    int required_hc = config.hit_chance;
    if (config.hit_chance_override) {
        required_hc = config.hit_chance_override_value;
    }

    if ((int)d.hit_chance < required_hc) {
        d.can_fire = false;
        d.reason = "hit chance too low";
        return d;
    }

    // ── FOV check ────────────────────────────────────────────────────
    if (target.fov_to_crosshair > config.field_of_view) {
        d.can_fire = false;
        d.reason = "target outside fov";
        return d;
    }

    // ── Can fire ─────────────────────────────────────────────────────
    d.can_fire = true;
    d.should_fire = true;
    d.target = target.entity;
    d.hitbox = target.hitbox_id;

    // aim point
    d.aim_point[0] = target.hitbox_origin[0];
    d.aim_point[1] = target.hitbox_origin[1];
    d.aim_point[2] = target.hitbox_origin[2];

    return d;
}

// ─── Aim calculation (from aim processing) ────────────────────────────────
inline void CalculateAim(
    const ShotDecision& decision,
    float view_origin[3],
    float& out_yaw,
    float& out_pitch)
{
    if (!decision.can_fire) return;

    float dx = decision.aim_point[0] - view_origin[0];
    float dy = decision.aim_point[1] - view_origin[1];
    float dz = decision.aim_point[2] - view_origin[2];

    float hyp = sqrtf(dx*dx + dy*dy);

    out_yaw = atan2f(dy, dx) * 57.295776f;
    out_pitch = atan2f(-dz, hyp) * 57.295776f;
}

// ─── Legit bot (from sub_1800F0860, sub_1800F5F60) ────────────────────────
struct LegitConfig {
    bool     enabled;
    bool     aim_enabled;
    bool     triggerbot_enabled;
    bool     rcs;
    float    rcs_pitch;
    float    rcs_yaw;
    int      rcs_start;
    float    first_shot_delay;
    bool     autoscope;
    bool     draw_fov;
    float    fov;
    float    smooth;
    bool     silent;
    int      trigger_hitboxes;
    int      trigger_hitchance;
    int      trigger_delay;
    int      trigger_min_damage;
    bool     trigger_autoscope;
    bool     require_key;
    int      key;
};

inline float ApplySmoothing(float current_angle, float target_angle, float smooth) {
    if (smooth <= 0.0f) return target_angle;
    return current_angle + (target_angle - current_angle) / smooth;
}

// ─── Backtracking (from history config) ──────────────────────────────────
struct LagRecord {
    float    origin[3];
    float    head_origin[3];
    int      tick;
    int      cmd;
    float    simulation_time;
    bool     valid;
};

inline LagRecord GetBestLagRecord(
    const std::vector<LagRecord>& records,
    float view_origin[3],
    int max_ticks)
{
    LagRecord best{};
    float best_score = -1.0f;

    for (const auto& r : records) {
        if (!r.valid) continue;
        if (max_ticks > 0 && r.tick > max_ticks) continue;

        // score: prefer records that are visible and recent
        float dx = r.head_origin[0] - view_origin[0];
        float dy = r.head_origin[1] - view_origin[1];
        float dz = r.head_origin[2] - view_origin[2];
        float dist = sqrtf(dx*dx + dy*dy + dz*dz);

        float score = 1.0f / (dist + 1.0f);
        if (score > best_score) {
            best_score = score;
            best = r;
        }
    }

    return best;
}

} // namespace features
