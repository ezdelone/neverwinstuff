#pragma once
// Reconstructed from ida_present_menu.json / config_tree.json / sub_1802F6830
// ESP / Visuals system: players, world, effects, overlays.

#include "../config.h"
#include "../sdk/schema.h"
#include "../render.h"
#include <cstdint>
#include <cmath>
#include <vector>
#include <string>

namespace visuals {

// ─── Colors ───────────────────────────────────────────────────────────────
struct Color {
    uint8_t r, g, b, a;
};

// ─── ESP box types (from config_tree targets.*) ───────────────────────────
enum class BoxType {
    None,
    Filled,
    Outline,
    Cornered,
    Bordered,
    Gradient
};

// ─── ESP flags (from config_tree targets.*.flags) ──────────────────────────
enum EspFlags {
    ESP_NONE          = 0,
    ESP_HEALTH        = (1 << 0),
    ESP_ARMOR         = (1 << 1),
    ESP_WEAPON        = (1 << 2),
    ESP_AMMO          = (1 << 3),
    ESP_NAME          = (1 << 4),
    ESP_DISTANCE      = (1 << 5),
    ESP_VELOCITY      = (1 << 6),
    ESP_MONEY         = (1 << 7),
    ESP_ARMOR_ICON    = (1 << 8),
    ESP_WEAPON_ICON   = (1 << 9),
    ESP_SCOPED        = (1 << 10),
    ESP_FLASH         = (1 << 11),
    ESP_RELOADING     = (1 << 12),
    ESP_BOMB          = (1 << 13),
    ESP_DEFUSING      = (1 << 14),
    ESP_DORMANT       = (1 << 15),
    ESP_IMMUNE        = (1 << 16),
    ESP_PING          = (1 << 17),
    ESP_STEAM_AVATAR  = (1 << 18),
    ESP_HAS_HELMET     = (1 << 19),
    ESP_HASDEFUSER    = (1 << 20),
    ESP_CHIP          = (1 << 21),
    ESP_LOOT          = (1 << 22),
    ESP_SPREAD        = (1 << 23)
};

// ─── Player ESP config ───────────────────────────────────────────────────
struct PlayerEspConfig {
    bool     enabled;
    BoxType  box_type;
    Color    box_color;
    Color    box_color_visible;
    Color    box_color_occluded;

    // Health bar
    bool     health_bar;
    Color    health_color;
    int      health_bar_style;

    // Armor bar
    bool     armor_bar;
    Color    armor_color;

    // Ammo bar
    bool     ammo_bar;
    Color    ammo_color;

    // Name
    bool     show_name;
    Color    name_color;
    bool     show_avatar;
    bool     show_steam_id;

    // Weapon
    bool     show_weapon;
    Color    weapon_color;
    bool     show_weapon_icon;

    // Skeleton
    bool     skeleton;
    Color    skeleton_color;
    Color    skeleton_color_visible;
    Color    skeleton_color_occluded;

    // Chams
    bool     chams;
    Color    chams_color;
    Color    chams_color_visible;
    Color    chams_color_occluded;
    bool     chams_flat;
    bool     chams_wireframe;
    bool     chams_glow;
    bool     chams_metallic;
    bool     chams_pearlescent;
    bool     chams_animated;

    // Flags
    int      flags;
    Color    flags_color;

    // Out of FOV
    bool     out_of_fov_arrows;
    float    out_of_fov_distance;

    // Grenade ESP
    bool     grenade_esp;
    Color    grenade_color;
    bool     grenade_warning;

    // Bomb ESP
    bool     bomb_esp;
    bool     bomb_timer;

    // Tracers
    bool     tracers;
    Color    tracers_color;
    int      tracer_style;

    // Dormant ESP
    bool     dormant_esp;
    float    dormant_fade;
};

// ─── Entity render context (from sub_1802F6830) ───────────────────────────
struct EntityRenderCtx {
    void*    entity;
    void*    pawn;
    void*    controller;
    int      entity_index;
    int      team;
    int      health;
    int      armor;
    bool     has_helmet;
    bool     has_defuser;
    bool     is_scoped;
    bool     is_defusing;
    bool     is_flashed;
    bool     is_reloading;
    bool     is_dormant;
    bool     is_immune;
    bool     is_alive;
    float    origin[3];
    float    head_origin[3];
    float    feet_origin[3];
    float    velocity[3];
    float    distance;
    float    simulation_time;
    char     name[32];
    char     weapon_name[32];
    int      ping;
    int      money;
    int      ammo;
    int      max_ammo;
};

// ─── Bone positions (from schema: lookup_bone, bone_transform) ────────────
enum BoneId {
    BONE_PELVIS           = 0,
    BONE_SPINE_1          = 1,
    BONE_SPINE_2          = 2,
    BONE_SPINE_3          = 3,
    BONE_SPINE_4          = 4,
    BONE_NECK             = 5,
    BONE_HEAD             = 6,
    BONE_LEFT_SHOULDER    = 7,
    BONE_LEFT_ELBOW       = 8,
    BONE_LEFT_WRIST       = 9,
    BONE_LEFT_HAND        = 10,
    BONE_RIGHT_SHOULDER   = 11,
    BONE_RIGHT_ELBOW      = 12,
    BONE_RIGHT_WRIST      = 13,
    BONE_RIGHT_HAND       = 14,
    BONE_LEFT_HIP         = 15,
    BONE_LEFT_KNEE        = 16,
    BONE_LEFT_ANKLE       = 17,
    BONE_LEFT_FOOT        = 18,
    BONE_RIGHT_HIP        = 19,
    BONE_RIGHT_KNEE       = 20,
    BONE_RIGHT_ANKLE      = 21,
    BONE_RIGHT_FOOT       = 22
};

// ─── Skeleton connections ─────────────────────────────────────────────────
constexpr int SKELETON_CONNECTIONS[][2] = {
    {BONE_HEAD, BONE_NECK},
    {BONE_NECK, BONE_SPINE_4},
    {BONE_SPINE_4, BONE_SPINE_3},
    {BONE_SPINE_3, BONE_SPINE_2},
    {BONE_SPINE_2, BONE_SPINE_1},
    {BONE_SPINE_1, BONE_PELVIS},
    {BONE_NECK, BONE_LEFT_SHOULDER},
    {BONE_LEFT_SHOULDER, BONE_LEFT_ELBOW},
    {BONE_LEFT_ELBOW, BONE_LEFT_WRIST},
    {BONE_LEFT_WRIST, BONE_LEFT_HAND},
    {BONE_NECK, BONE_RIGHT_SHOULDER},
    {BONE_RIGHT_SHOULDER, BONE_RIGHT_ELBOW},
    {BONE_RIGHT_ELBOW, BONE_RIGHT_WRIST},
    {BONE_RIGHT_WRIST, BONE_RIGHT_HAND},
    {BONE_PELVIS, BONE_LEFT_HIP},
    {BONE_LEFT_HIP, BONE_LEFT_KNEE},
    {BONE_LEFT_KNEE, BONE_LEFT_ANKLE},
    {BONE_LEFT_ANKLE, BONE_LEFT_FOOT},
    {BONE_PELVIS, BONE_RIGHT_HIP},
    {BONE_RIGHT_HIP, BONE_RIGHT_KNEE},
    {BONE_RIGHT_KNEE, BONE_RIGHT_ANKLE},
    {BONE_RIGHT_ANKLE, BONE_RIGHT_FOOT}
};

// ─── ESP renderer ──────────────────────────────────────────────────────────
class EspRenderer {
public:
    static EspRenderer& Instance() { static EspRenderer e; return e; }

    void RenderPlayers(const std::vector<EntityRenderCtx>& entities) {
        for (const auto& e : entities) {
            if (!e.is_alive) continue;

            // 1. World-to-screen for head and feet
            float head_screen[2], feet_screen[2];
            if (!WorldToScreen(e.head_origin, head_screen)) continue;
            if (!WorldToScreen(e.feet_origin, feet_screen)) continue;

            // 2. Calculate box dimensions
            float height = feet_screen[1] - head_screen[1];
            float width = height * 0.5f;
            float x = head_screen[0] - width * 0.5f;
            float y = head_screen[1];

            // 3. Draw box
            DrawBox(x, y, width, height, e);

            // 4. Draw health bar
            // 5. Draw armor bar
            // 6. Draw name
            // 7. Draw weapon
            // 8. Draw skeleton
            // 9. Draw flags
            // 10. Draw distance
        }
    }

    void RenderWorld() {
        // Grenades, C4, inferno, smoke, molotov
        // From config: grenade_esp, bomb_text, smoke_radius, inferno_radius
    }

    void RenderEffects() {
        // Bullet impacts, tracers, spread circle
        // From config: impacts, tracers, spread_circle
    }

    void RenderOverlays() {
        // Watermark, indicators, hitmarker, hit sounds
        // From config: watermark, indicators, hitmarker, hitsounds
    }

private:
    bool WorldToScreen(const float world[3], float screen[2]) {
        // Uses view_matrix from pattern scanner
        // v_matrix = sdk::g_game.view_matrix
        return false; // placeholder
    }

    void DrawBox(float x, float y, float w, float h, const EntityRenderCtx& e) {
        // Based on config.box_type
    }

    void DrawSkeleton(const EntityRenderCtx& e) {
        // Uses bone_transform / lookup_bone
        // Draws SKELETON_CONNECTIONS
    }

    void DrawHealthBar(float x, float y, float h, int health, int max_health) {
        // Gradient from red to green
        float frac = (float)health / (float)max_health;
        uint8_t r = (uint8_t)(255 * (1.0f - frac));
        uint8_t g = (uint8_t)(255 * frac);
    }

    void DrawArmorBar(float x, float y, float h, int armor) {
        // Blue bar
    }

    void DrawFlags(float x, float y, const EntityRenderCtx& e) {
        // Scoped, Flashed, Reloading, Defusing, Bomb, Immune, etc.
    }

    void DrawName(float x, float y, const EntityRenderCtx& e) {
        // e.name
    }

    void DrawWeapon(float x, float y, const EntityRenderCtx& e) {
        // e.weapon_name
    }

    void DrawDistance(float x, float y, const EntityRenderCtx& e) {
        // "%.0fm" format
    }
};

// ─── Chams materials (from strings: neverwin/*.vmat) ──────────────────────
enum class ChamMaterial {
    None,
    Flat,
    FlatIZ,
    Glow,
    GlowIZ,
    Glitter,
    GlitterIZ,
    Galaxy,
    GalaxyIZ,
    InnerGlow,
    InnerGlowIZ,
    Bloom,
    BloomIZ,
    AnimatedOutlines,
    AnimatedOutlinesIZ,
    AnimatedGlow,
    AnimatedGlowIZ,
    ScrollGlow,
    ScrollGlowIZ,
    Metallic,
    MetallicIZ,
    Electric,
    ElectricIZ,
    Specular,
    SpecularIZ,
    Pearl,
    PearlIZ,
    Blend,
    Wireframe,
    Custom
};

// ─── Chams config (from config_tree targets.*.chams) ──────────────────────
struct ChamsConfig {
    bool          visible;
    bool          occluded;
    ChamMaterial  material;
    Color         color_visible;
    Color         color_occluded;
    bool          overlay;
    float         glow_intensity;
};

// ─── World ESP (from config_tree world/screen/sky) ────────────────────────
struct WorldEspConfig {
    // Grenades
    bool     grenade_esp;
    bool     grenade_radius;
    Color    grenade_color;

    // Smoke
    bool     smoke_radius;
    bool     smoke_esp;
    Color    smoke_color;

    // Inferno (molotov)
    bool     inferno_radius;
    bool     inferno_esp;
    Color    inferno_color;

    // C4
    bool     bomb_esp;
    bool     bomb_radius;
    bool     bomb_timer;
    Color    bomb_color;

    // Dropped weapons
    bool     weapon_esp;
    Color    weapon_color;

    // Loot
    bool     loot_esp;
    Color    loot_color;

    // Chickens / pets
    bool     chicken_esp;
    Color    chicken_color;
};

} // namespace visuals

