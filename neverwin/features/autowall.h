#pragma once
// Reconstructed from sub_18029C4F0 / sub_1802A7CF0
// Autowall / penetration damage calculation.

#include "schema.h"
#include "game.h"
#include <cmath>

namespace features {

// ─── Damage calculation (from sub_18029C4F0) ─────────────────────────────
struct PenetrationResult {
    float   damage;
    bool    can_penetrate;
    int     hit_count;
    void*   hit_entity;
    float   armor_factor;
};

inline float CalculateRangeModifier(float distance, float base_range_modifier) {
    // v13 = sub_18029AD80(start, end); if (v13 > max_range) return 0
    // v17 = dot product scaled by direction
    return base_range_modifier;
}

inline float CalculateArmorDamage(float damage, float armor_ratio, int armor_value, bool has_helmet, bool is_headshot) {
    // from sub_18029C4F0:
    //   if (armor > 0) {
    //     v108 = sub_18046E550(entity, hitbox);  // armor multiplier
    //     v100 = v108 * base_damage;
    //     if (armor_value > 0) {
    //       v113 = (armor_ratio * 0.5) * v100;
    //       if ((v100 - v113) * 0.5 <= armor_value)
    //           v100 = v113;
    //       else
    //           v100 = v100 - (armor_value + armor_value);
    //     }
    //   }
    if (armor_value <= 0) return damage;

    float result = damage;
    float armor_dmg = (armor_ratio * 0.5f) * damage;

    if ((damage - armor_dmg) * 0.5f <= (float)armor_value) {
        result = armor_dmg;
    } else {
        result = damage - (float)(armor_value + armor_value);
    }

    if (is_headshot && has_helmet) {
        // headshot with helmet
        result *= 0.5f;
    }

    return result;
}

inline float CalculateHeadshotDamage(float damage, float hs_multiplier, int hitbox_id) {
    // hitbox_id <= 8 = head hitboxes (from sub_18029C4F0)
    if (hitbox_id <= 8) {
        return damage * hs_multiplier;
    }
    return damage;
}

inline float CalculatePelletDamage(float damage, int num_bullets, float cycle_time) {
    // from sub_18029C4F0: v128 = (num_bullets + 1) * cycle_time * 1.25
    // clamp to 70 (weapon 40) or 105 (weapon 9)
    float v = (float)(num_bullets + 1) * cycle_time * 1.25f;

    // if weapon == 40 → clamp 70
    // if weapon == 9  → clamp 105
    if (v > 105.0f) v = 105.0f;
    if (v < 1.0f)   v = 1.0f;

    return damage * v;
}

// ─── Trace bullet (from sub_1802A7CF0) ────────────────────────────────────
enum class TraceMode {
    Ray,            // a5 == 0
    BulletPenetration // a5 != 0
};

struct TraceParams {
    float    start[3];
    float    end[3];
    void*    weapon;
    void*    target;
    int      mask;       // 1843211 for ray
    TraceMode mode;
};

// Mode 0: TraceRay through game_trace_manager
// Mode 1: TraceBullet with penetration
// Returns entity hit, or nullptr
inline void* TraceBullet(const TraceParams& params) {
    if (!sdk::g_game.game_trace_manager)
        return nullptr;

    if (params.mode == TraceMode::Ray) {
        // sub_180044180 → TraceRay
        // mask = 1843211
        return nullptr; // placeholder
    } else {
        // sub_1802AFA00 → TraceBullet
        // uses m_nDamage, m_flPenetration, m_flRangeModifier from VData
        return nullptr; // placeholder
    }
}

// ─── Can penetrate (full damage check) ────────────────────────────────────
inline PenetrationResult CanPenetrate(
    float start[3],
    float end[3],
    void* weapon,
    void* target,
    const sdk::WeaponVData& vdata,
    int   armor_value,
    bool  has_helmet)
{
    PenetrationResult result{};

    // 1. Distance check
    float dx = end[0] - start[0];
    float dy = end[1] - start[1];
    float dz = end[2] - start[2];
    float dist = sqrtf(dx*dx + dy*dy + dz*dz);

    if (dist > vdata.m_flRange) {
        result.damage = 0;
        result.can_penetrate = false;
        return result;
    }

    // 2. Range modifier
    float range_mod = CalculateRangeModifier(dist, vdata.m_flRange);

    // 3. Base damage
    float damage = (float)vdata.m_nDamage * range_mod;

    // 4. Armor
    damage = CalculateArmorDamage(damage, vdata.m_flArmorRatio, armor_value, has_helmet, false);

    // 5. Pellets
    if (vdata.m_nNumBullets > 1) {
        damage = CalculatePelletDamage(damage, vdata.m_nNumBullets, vdata.m_flCycleTime);
    }

    result.damage = damage;
    result.can_penetrate = damage > 0.0f;
    return result;
}

} // namespace features
