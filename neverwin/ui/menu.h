#pragma once
// Full menu ? styled like neverwin.dll original
// Layout: header | sidebar | sub-tabs | content | footer

#include "../config.h"
#include "imgui.h"

static inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
    return ImVec2(a.x + b.x, a.y + b.y);
}

#include <string>
#include <vector>

namespace ui {

// ??? Colors (neverwin theme) ???????????????????????????????????????????????
namespace colors {
    inline ImVec4 BG_DARK()      { return ImVec4(24/255.0f, 24/255.0f, 28/255.0f, 1.0f); }
    inline ImVec4 BG_PANEL()     { return ImVec4(32/255.0f, 32/255.0f, 38/255.0f, 1.0f); }
    inline ImVec4 BG_SIDEBAR()   { return ImVec4(28/255.0f, 28/255.0f, 34/255.0f, 1.0f); }
    inline ImVec4 BG_CONTENT()   { return ImVec4(36/255.0f, 36/255.0f, 42/255.0f, 1.0f); }
    inline ImVec4 BG_HOVER()     { return ImVec4(45/255.0f, 45/255.0f, 55/255.0f, 1.0f); }
    inline ImVec4 BG_ACTIVE()    { return ImVec4(55/255.0f, 40/255.0f, 80/255.0f, 1.0f); }
    inline ImVec4 BG_INPUT()     { return ImVec4(40/255.0f, 40/255.0f, 48/255.0f, 1.0f); }
    inline ImVec4 ACCENT()       { return ImVec4(140/255.0f, 89/255.0f, 217/255.0f, 1.0f); }
    inline ImVec4 ACCENT_DIM()   { return ImVec4(90/255.0f, 60/255.0f, 140/255.0f, 1.0f); }
    inline ImVec4 TEXT_MAIN()    { return ImVec4(230/255.0f, 230/255.0f, 235/255.0f, 1.0f); }
    inline ImVec4 TEXT_DIM()     { return ImVec4(140/255.0f, 140/255.0f, 150/255.0f, 1.0f); }
    inline ImVec4 TEXT_ACTIVE()  { return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); }
    inline ImVec4 BORDER()       { return ImVec4(55/255.0f, 55/255.0f, 65/255.0f, 1.0f); }
    inline ImVec4 SEPARATOR()    { return ImVec4(45/255.0f, 45/255.0f, 55/255.0f, 1.0f); }
    inline ImVec4 SUCCESS()      { return ImVec4(80/255.0f, 200/255.0f, 120/255.0f, 1.0f); }
    inline ImVec4 WARNING()      { return ImVec4(255/255.0f, 200/255.0f, 60/255.0f, 1.0f); }
    inline ImVec4 DANGER()       { return ImVec4(220/255.0f, 80/255.0f, 80/255.0f, 1.0f); }
}

// ??? Tab structure ??????????????????????????????????????????????????????????
struct Tab {
    const char* name;
    const char* icon;
    std::vector<const char*> sub_tabs;
};

// ??? All tabs (from neverwin layout) ???????????????????????????????????????
static const Tab TABS[] = {
    {"Aimbot",        "??", {"General", "Triggerbot", "Backtrack"}},
    {"Rage",          "?", {"Main", "Selection", "Other", "Anti Aim", "Double Tap"}},
    {"Legit",         "??", {"Aim", "Trigger", "RCS", "Visuals"}},
    {"Common",        "?",  {"General", "Binds", "Presets", "About"}},
    {"Visuals",       "??", {"Players", "World", "Effects", "Sky", "Screen"}},
    {"Inventory",     "??", {"Skins", "Stickers", "Charms", "Pets"}},
    {"Misc",          "??", {"Movement", "Exploits", "Utility", "Auto Buy"}},
    {"Lua",           "??", {"Scripts", "Extensions", "Settings"}}
};
constexpr int TAB_COUNT = 8;

// ??? State ??????????????????????????????????????????????????????????????????
static int g_active_tab = 0;
static int g_active_subtab = 0;
static float g_menu_scale = 1.0f;

// ??? Config values (bound to config system) ????????????????????????????????
struct MenuValues {
    // Rage
    bool rage_enabled = false;
    bool rage_silent = false;
    bool rage_auto_fire = false;
    bool rage_limiters = false;
    bool rage_refine_shot = false;
    float rage_fov = 180.0f;
    float rage_extrapolation = 50.0f;
    float rage_hitchance = 65.0f;
    float rage_min_damage = 10.0f;
    bool rage_quick_stop = true;
    bool rage_quick_scope = true;
    bool rage_auto_scope = false;
    bool rage_auto_stop = true;
    bool rage_stop_in_air = true;
    bool rage_stop_on_land = true;
    bool rage_stop_between = false;
    bool rage_double_tap = false;
    bool rage_force_shot = false;
    float rage_force_hitchance = 90.0f;
    float rage_history = 100.0f;
    bool rage_delay_shot = false;
    float rage_delay_damage = 30.0f;
    bool rage_remove_spread = false;
    bool rage_duck_peek = false;
    bool rage_quick_peek = false;

    // Anti-Aim
    bool aa_enabled = false;
    int  aa_leg_movement = 0;
    bool aa_pitch = false;
    bool aa_yaw = false;
    bool aa_mouse_override = false;
    bool aa_avoid_backstab = false;
    bool aa_auto_adjust = false;
    float aa_yaw_offset = 0.0f;
    bool aa_jitter = false;
    int  aa_jitter_type = 0;
    bool aa_jitter_disable_air = false;
    bool aa_jitter_disable_override = false;
    float aa_jitter_range = 30.0f;
    bool aa_spin = false;
    float aa_spin_speed = 20.0f;
    int  aa_pitch_order = 0;
    bool aa_hide_shots = false;

    // Aimbot
    bool aim_enabled = false;
    bool aim_silent = false;
    bool aim_autofire = false;
    float aim_fov = 5.0f;
    float aim_smooth = 3.0f;
    int aim_hitchance = 75;
    int aim_mindamage = 20;

    // Triggerbot
    bool trig_enabled = false;
    int trig_hitchance = 70;
    int trig_delay = 50;
    int trig_mindamage = 15;
    bool trig_autoscope = false;

    // Legit
    bool legit_enabled = false;
    bool legit_aim = false;
    bool legit_rcs = false;
    float legit_rcs_pitch = 50.0f;
    float legit_rcs_yaw = 50.0f;
    float legit_rcs_start = 2.0f;
    bool legit_draw_fov = false;

    // Movement
    bool mov_bhop = false;
    bool mov_edge_jump = false;
    bool mov_edgebug = false;
    bool mov_long_jump = false;
    bool mov_mini_jump = false;
    bool mov_null_strafe = false;
    bool mov_slide_walk = false;
    bool mov_pixel_surf = false;
    bool mov_fast_ladder = false;
    bool mov_air_strafe = false;
    bool mov_quick_stop = false;
    bool mov_auto_peek = false;
    int mov_edgebug_ticks = 12;
    bool mov_edgebug_assisted = false;
    float mov_edgebug_fov = 90.0f;
    bool mov_edgebug_silent = false;
    int mov_edgebug_passes = 3;

    // Visuals - Players
    bool esp_enabled = false;
    bool esp_box = false;
    bool esp_health = false;
    bool esp_armor = false;
    bool esp_name = false;
    bool esp_weapon = false;
    bool esp_skeleton = false;
    bool esp_flags = false;
    bool esp_distance = false;
    bool esp_chams = false;
    bool esp_glow = false;
    bool esp_dormant = false;
    float esp_dormant_fade = 0.5f;

    // Visuals - World
    bool esp_grenades = false;
    bool esp_smoke = false;
    bool esp_molotov = false;
    bool esp_c4 = false;
    bool esp_dropped = false;

    // Common
    int  menu_key = 0x2D;  // VK_INSERT
    float menu_scale_val = 1.0f;
    bool dev_mode = false;

    // Misc
    bool third_person = false;
    int third_person_key = 0;
    bool watermark = false;
    bool spectators = false;
    bool hitmarker = false;
    bool hitsounds = false;
};
static MenuValues g_values{};

// ??? Helper: styled checkbox ???????????????????????????????????????????????
static bool StyledCheckbox(const char* label, bool* v) {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, colors::BG_INPUT());
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, colors::BG_HOVER());
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, colors::ACCENT());
    ImGui::PushStyleColor(ImGuiCol_CheckMark, colors::ACCENT());
    bool result = ImGui::Checkbox(label, v);
    ImGui::PopStyleColor(4);
    return result;
}

// ??? Helper: styled slider ????????????????????????????????????????????????
static bool StyledSliderFloat(const char* label, float* v, float min, float max, const char* fmt = "%.1f") {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, colors::BG_INPUT());
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, colors::BG_HOVER());
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, colors::BG_HOVER());
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, colors::ACCENT());
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, colors::ACCENT());
    bool result = ImGui::SliderFloat(label, v, min, max, fmt);
    ImGui::PopStyleColor(5);
    return result;
}

static bool StyledSliderInt(const char* label, int* v, int min, int max, const char* fmt = "%d") {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, colors::BG_INPUT());
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, colors::BG_HOVER());
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, colors::BG_HOVER());
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, colors::ACCENT());
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, colors::ACCENT());
    bool result = ImGui::SliderInt(label, v, min, max, fmt);
    ImGui::PopStyleColor(5);
    return result;
}

// ??? Helper: styled combo ??????????????????????????????????????????????????
static bool StyledCombo(const char* label, int* current, const char* const items[], int count) {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, colors::BG_INPUT());
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, colors::BG_HOVER());
    ImGui::PushStyleColor(ImGuiCol_Button, colors::ACCENT_DIM());
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors::ACCENT());
    bool result = ImGui::Combo(label, current, items, count);
    ImGui::PopStyleColor(4);
    return result;
}

// ???????????????????????????????????????????????????????????????????????????
// TAB CONTENT DRAWERS
// ???????????????????????????????????????????????????????????????????????????

static void DrawAimbotContent(int sub) {
    switch (sub) {
        case 0: { // General
            ImGui::Text("Aimbot");
            ImGui::Separator();
            StyledCheckbox("Enabled", &g_values.aim_enabled);
            StyledCheckbox("Silent Aim", &g_values.aim_silent);
            StyledCheckbox("Automatic Fire", &g_values.aim_autofire);
            StyledSliderFloat("Field of View", &g_values.aim_fov, 0.0f, 180.0f, "%.1f?");
            StyledSliderFloat("Smoothing", &g_values.aim_smooth, 1.0f, 20.0f);
            StyledSliderInt("Hit Chance", &g_values.aim_hitchance, 0, 100, "%d%%");
            StyledSliderInt("Min Damage", &g_values.aim_mindamage, 0, 100, "%d");
            break;
        }
        case 1: { // Triggerbot
            ImGui::Text("Triggerbot");
            ImGui::Separator();
            StyledCheckbox("Enabled", &g_values.trig_enabled);
            StyledSliderInt("Hit Chance", &g_values.trig_hitchance, 0, 100, "%d%%");
            StyledSliderInt("Delay", &g_values.trig_delay, 0, 500, "%d ms");
            StyledSliderInt("Min Damage", &g_values.trig_mindamage, 0, 100, "%d");
            StyledCheckbox("Autoscope", &g_values.trig_autoscope);
            break;
        }
        case 2: { // Backtrack
            ImGui::Text("Backtrack");
            ImGui::Separator();
            StyledSliderFloat("History", &g_values.rage_history, 0.0f, 200.0f, "%.0f ms");
            StyledCheckbox("Delay Shot", &g_values.rage_delay_shot);
            StyledSliderFloat("Delay Damage", &g_values.rage_delay_damage, 0.0f, 100.0f, "%.0f");
            break;
        }
    }
}

static void DrawRageContent(int sub) {
    switch (sub) {
        case 0: { // Main
            ImGui::Text("Rage ? Main");
            ImGui::Separator();
            StyledCheckbox("Enabled", &g_values.rage_enabled);
            StyledCheckbox("Silent Aim", &g_values.rage_silent);
            StyledCheckbox("Automatic Fire", &g_values.rage_auto_fire);
            StyledCheckbox("Limiters", &g_values.rage_limiters);
            StyledCheckbox("Refine Shot", &g_values.rage_refine_shot);
            StyledSliderFloat("Field of View", &g_values.rage_fov, 0.0f, 180.0f, "%.1f?");
            StyledCheckbox("Auto Scope", &g_values.rage_auto_scope);
            StyledCheckbox("Auto Stop", &g_values.rage_auto_stop);
            StyledCheckbox("Stop In Air", &g_values.rage_stop_in_air);
            StyledCheckbox("Stop On Land", &g_values.rage_stop_on_land);
            StyledCheckbox("Stop Between Shots", &g_values.rage_stop_between);
            break;
        }
        case 1: { // Selection
            ImGui::Text("Rage ? Selection");
            ImGui::Separator();
            StyledSliderFloat("Extrapolation", &g_values.rage_extrapolation, 0.0f, 100.0f, "%.0f");
            StyledSliderFloat("Hit Chance", &g_values.rage_hitchance, 0.0f, 100.0f, "%d%%");
            StyledSliderFloat("Min Damage", &g_values.rage_min_damage, 0.0f, 100.0f, "%.0f");
            StyledCheckbox("Quick Stop", &g_values.rage_quick_stop);
            StyledCheckbox("Quick Scope", &g_values.rage_quick_scope);
            break;
        }
        case 2: { // Other
            ImGui::Text("Rage ? Other");
            ImGui::Separator();
            StyledCheckbox("Delay Shot", &g_values.rage_delay_shot);
            StyledSliderFloat("Delay Damage", &g_values.rage_delay_damage, 0.0f, 100.0f, "%.0f");
            StyledCheckbox("Remove Spread", &g_values.rage_remove_spread);
            StyledCheckbox("Duck Peek Assist", &g_values.rage_duck_peek);
            StyledCheckbox("Quick Peek Assist", &g_values.rage_quick_peek);
            StyledCheckbox("Double Precision", &g_values.rage_double_tap);
            StyledCheckbox("Force Shot", &g_values.rage_force_shot);
            StyledSliderFloat("Force Shot HC", &g_values.rage_force_hitchance, 0.0f, 100.0f, "%d%%");
            break;
        }
        case 3: { // Anti Aim
            ImGui::Text("Anti Aim");
            ImGui::Separator();
            StyledCheckbox("Enabled", &g_values.aa_enabled);
            const char* leg_modes[] = {"Off", "Sliding", "Static"};
            StyledCombo("Leg Movement", &g_values.aa_leg_movement, leg_modes, 3);
            StyledCheckbox("Pitch", &g_values.aa_pitch);
            const char* pitch_orders[] = {"Down", "Up", "Zero", "Jitter"};
            StyledCombo("Pitch Order", &g_values.aa_pitch_order, pitch_orders, 4);
            StyledCheckbox("Yaw", &g_values.aa_yaw);
            StyledSliderFloat("Yaw Offset", &g_values.aa_yaw_offset, -180.0f, 180.0f, "%.1f?");
            StyledCheckbox("Mouse Override", &g_values.aa_mouse_override);
            StyledCheckbox("Avoid Backstab", &g_values.aa_avoid_backstab);
            StyledCheckbox("Auto Adjust", &g_values.aa_auto_adjust);
            StyledCheckbox("Jitter", &g_values.aa_jitter);
            const char* jitter_types[] = {"Offset", "Center", "Random", "Skitter"};
            StyledCombo("Jitter Type", &g_values.aa_jitter_type, jitter_types, 4);
            StyledCheckbox("Jitter Disable Air", &g_values.aa_jitter_disable_air);
            StyledCheckbox("Jitter Disable Override", &g_values.aa_jitter_disable_override);
            StyledSliderFloat("Jitter Range", &g_values.aa_jitter_range, 0.0f, 180.0f, "%.1f?");
            StyledCheckbox("Spin", &g_values.aa_spin);
            StyledSliderFloat("Spin Speed", &g_values.aa_spin_speed, 0.0f, 100.0f, "%.0f");
            StyledCheckbox("Hide Shots", &g_values.aa_hide_shots);
            break;
        }
        case 4: { // Double Tap
            ImGui::Text("Double Tap");
            ImGui::Separator();
            StyledCheckbox("Enabled", &g_values.rage_double_tap);
            StyledCheckbox("Force Shot", &g_values.rage_force_shot);
            StyledSliderFloat("Force Shot HC", &g_values.rage_force_hitchance, 0.0f, 100.0f, "%d%%");
            break;
        }
    }
}

static void DrawLegitContent(int sub) {
    switch (sub) {
        case 0: {
            ImGui::Text("Legit ? Aim");
            ImGui::Separator();
            StyledCheckbox("Enabled", &g_values.legit_enabled);
            StyledCheckbox("Aim Assist", &g_values.legit_aim);
            StyledSliderFloat("FOV", &g_values.aim_fov, 0.0f, 30.0f, "%.1f?");
            StyledSliderFloat("Smoothing", &g_values.aim_smooth, 1.0f, 20.0f);
            break;
        }
        case 1: {
            ImGui::Text("Legit ? Trigger");
            ImGui::Separator();
            StyledCheckbox("Enabled", &g_values.trig_enabled);
            StyledSliderInt("Hit Chance", &g_values.trig_hitchance, 0, 100, "%d%%");
            StyledSliderInt("Delay", &g_values.trig_delay, 0, 500, "%d ms");
            StyledSliderInt("Min Damage", &g_values.trig_mindamage, 0, 100, "%d");
            break;
        }
        case 2: {
            ImGui::Text("Legit ? RCS");
            ImGui::Separator();
            StyledCheckbox("RCS", &g_values.legit_rcs);
            StyledSliderFloat("RCS Pitch", &g_values.legit_rcs_pitch, 0.0f, 100.0f, "%.0f%%");
            StyledSliderFloat("RCS Yaw", &g_values.legit_rcs_yaw, 0.0f, 100.0f, "%.0f%%");
            StyledSliderFloat("RCS Start", &g_values.legit_rcs_start, 0.0f, 10.0f, "%.0f bullets");
            break;
        }
        case 3: {
            ImGui::Text("Legit ? Visuals");
            ImGui::Separator();
            StyledCheckbox("Draw FOV", &g_values.legit_draw_fov);
            break;
        }
    }
}

static void DrawCommonContent(int sub) {
    switch (sub) {
        case 0: {
            ImGui::Text("Common");
            ImGui::Separator();
            StyledSliderFloat("Menu Scale", &g_values.menu_scale_val, 0.5f, 2.0f, "%.2fx");
            StyledCheckbox("Developer Mode", &g_values.dev_mode);
            break;
        }
        case 1: { // Binds
            ImGui::Text("Binds");
            ImGui::Separator();
            ImGui::Text("Menu Key: INSERT (0x%02X)", g_values.menu_key);
            break;
        }
        case 3: { // About
            ImGui::Text("About");
            ImGui::Separator();
            ImGui::Text("Astraea");
            ImGui::TextDisabled("Counter-Strike 2");
            ImGui::Separator();
            ImGui::Text("Branch: Release");
            ImGui::Text("Updated: Oct 25 2026");
            ImGui::Text("Expires: 67 days");
            ImGui::Text("Username: Awhat");
            break;
        }
    }
}

static void DrawVisualsContent(int sub) {
    switch (sub) {
        case 0: { // Players
            ImGui::Text("Visuals ? Players");
            ImGui::Separator();
            StyledCheckbox("Enabled", &g_values.esp_enabled);
            StyledCheckbox("Box", &g_values.esp_box);
            StyledCheckbox("Health Bar", &g_values.esp_health);
            StyledCheckbox("Armor Bar", &g_values.esp_armor);
            StyledCheckbox("Name", &g_values.esp_name);
            StyledCheckbox("Weapon", &g_values.esp_weapon);
            StyledCheckbox("Skeleton", &g_values.esp_skeleton);
            StyledCheckbox("Flags", &g_values.esp_flags);
            StyledCheckbox("Distance", &g_values.esp_distance);
            StyledCheckbox("Chams", &g_values.esp_chams);
            StyledCheckbox("Glow", &g_values.esp_glow);
            StyledCheckbox("Dormant ESP", &g_values.esp_dormant);
            StyledSliderFloat("Dormant Fade", &g_values.esp_dormant_fade, 0.0f, 1.0f, "%.2f");
            break;
        }
        case 1: { // World
            ImGui::Text("Visuals ? World");
            ImGui::Separator();
            StyledCheckbox("Grenade ESP", &g_values.esp_grenades);
            StyledCheckbox("Smoke ESP", &g_values.esp_smoke);
            StyledCheckbox("Molotov ESP", &g_values.esp_molotov);
            StyledCheckbox("C4 ESP", &g_values.esp_c4);
            StyledCheckbox("Dropped Weapons", &g_values.esp_dropped);
            break;
        }
    }
}

static void DrawInventoryContent(int sub) {
    ImGui::Text("Inventory");
    ImGui::Separator();
    ImGui::Text("Skins, Stickers, Charms");
}

static void DrawMiscContent(int sub) {
    switch (sub) {
        case 0: { // Movement
            ImGui::Text("Movement");
            ImGui::Separator();
            StyledCheckbox("Bunny Hop", &g_values.mov_bhop);
            StyledCheckbox("Edge Jump", &g_values.mov_edge_jump);
            StyledCheckbox("Edge Bug", &g_values.mov_edgebug);
            StyledSliderInt("Edge Bug Max Ticks", &g_values.mov_edgebug_ticks, 1, 32, "%d");
            StyledCheckbox("Edge Bug Assisted", &g_values.mov_edgebug_assisted);
            StyledSliderFloat("Edge Bug FOV", &g_values.mov_edgebug_fov, 0.0f, 180.0f, "%.0f?");
            StyledCheckbox("Edge Bug Silent", &g_values.mov_edgebug_silent);
            StyledSliderInt("Edge Bug Passes", &g_values.mov_edgebug_passes, 1, 5, "%d");
            StyledCheckbox("Long Jump", &g_values.mov_long_jump);
            StyledCheckbox("Mini Jump", &g_values.mov_mini_jump);
            StyledCheckbox("Null Strafe", &g_values.mov_null_strafe);
            StyledCheckbox("Slide Walk", &g_values.mov_slide_walk);
            StyledCheckbox("Pixel Surf", &g_values.mov_pixel_surf);
            StyledCheckbox("Fast Ladder", &g_values.mov_fast_ladder);
            StyledCheckbox("Air Strafe", &g_values.mov_air_strafe);
            StyledCheckbox("Quick Stop", &g_values.mov_quick_stop);
            StyledCheckbox("Auto Peek", &g_values.mov_auto_peek);
            break;
        }
        case 1: { // Utility
            ImGui::Text("Utility");
            ImGui::Separator();
            StyledCheckbox("Third Person", &g_values.third_person);
            StyledCheckbox("Watermark", &g_values.watermark);
            StyledCheckbox("Spectators", &g_values.spectators);
            StyledCheckbox("Hitmarker", &g_values.hitmarker);
            StyledCheckbox("Hit Sounds", &g_values.hitsounds);
            break;
        }
    }
}

static void DrawLuaContent(int sub) {
    ImGui::Text("Lua");
    ImGui::Separator();
    ImGui::Text("Scripts and extensions");
}

// ???????????????????????????????????????????????????????????????????????????
// MAIN MENU DRAW
// ???????????????????????????????????????????????????????????????????????????

static void DrawMenu() {
    // Window setup
    ImVec2 display_size = ImGui::GetIO().DisplaySize;
    ImVec2 window_size(display_size.x * 0.4f, display_size.y * 0.55f);
    ImVec2 window_pos((display_size.x - window_size.x) * 0.5f, (display_size.y - window_size.y) * 0.5f);

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(window_size, ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, colors::BG_DARK());

    if (!ImGui::Begin("##astraea", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar)) {
        ImGui::End();
        return;
    }

    ImVec2 content_size = ImGui::GetContentRegionAvail();
    float header_height = 60.0f;
    float sidebar_width = 180.0f;
    float subtab_height = 35.0f;
    float footer_height = 40.0f;

    // ??? HEADER ????????????????????????????????????????????????????????
    ImGui::BeginChild("##header", ImVec2(content_size.x, header_height), false);
    {
        ImGui::SetCursorPos(ImVec2(20, 18));
        ImGui::TextColored(colors::TEXT_MAIN(), "Astraea");

        ImGui::SetCursorPos(ImVec2(110, 22));
        ImGui::TextColored(colors::TEXT_DIM(), "Counter-Strike 2");

        // Right side: username | expiry
        char user_info[128];
        snprintf(user_info, sizeof(user_info), "Awhat | 67 days");
        ImVec2 user_size = ImGui::CalcTextSize(user_info);
        ImGui::SetCursorPos(ImVec2(content_size.x - user_size.x - 20, 22));
        ImGui::TextColored(colors::TEXT_DIM(), "%s", user_info);
    }
    ImGui::EndChild();

    ImGui::GetWindowDrawList()->AddLine(
        ImGui::GetWindowPos() + ImVec2(0, header_height),
        ImGui::GetWindowPos() + ImVec2(content_size.x, header_height),
        ImGui::GetColorU32(colors::SEPARATOR()), 1.0f);

    // ??? SIDEBAR ???????????????????????????????????????????????????????
    ImGui::SetCursorPos(ImVec2(0, header_height));
    ImGui::BeginChild("##sidebar", ImVec2(sidebar_width, content_size.y - header_height - footer_height), false,
        ImGuiWindowFlags_NoScrollbar);
    {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, colors::BG_SIDEBAR());
        for (int i = 0; i < TAB_COUNT; i++) {
            bool active = (g_active_tab == i);

            ImVec2 item_size(sidebar_width - 8, 35);
            ImVec2 item_pos(4, ImGui::GetCursorPosY() + 2);

            // Background
            if (active) {
                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImGui::GetWindowPos() + item_pos,
                    ImGui::GetWindowPos() + ImVec2(item_pos.x + item_size.x, item_pos.y + item_size.y),
                    ImGui::GetColorU32(colors::BG_ACTIVE()), 4.0f);
            }

            // Text
            ImGui::SetCursorPos(ImVec2(20, item_pos.y + 8));
            if (active) {
                ImGui::TextColored(colors::TEXT_ACTIVE(), "%s %s", TABS[i].icon, TABS[i].name);
            } else {
                ImGui::TextColored(colors::TEXT_DIM(), "%s %s", TABS[i].icon, TABS[i].name);
            }

            // Click detection
            ImGui::SetCursorPos(item_pos);
            ImGui::InvisibleButton(TABS[i].name, item_size);
            if (ImGui::IsItemClicked()) {
                g_active_tab = i;
                g_active_subtab = 0;
            }

            ImGui::SetCursorPos(ImVec2(0, item_pos.y + item_size.y));
        }
        ImGui::PopStyleColor();
    }
    ImGui::EndChild();

    // ??? SUB-TABS ??????????????????????????????????????????????????????
    float content_x = sidebar_width;
    float content_width = content_size.x - sidebar_width;

    ImGui::SetCursorPos(ImVec2(content_x, header_height));
    ImGui::BeginChild("##subtabs", ImVec2(content_width, subtab_height), false,
        ImGuiWindowFlags_NoScrollbar);
    {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, colors::BG_PANEL());

        const Tab& tab = TABS[g_active_tab];
        float x = 15.0f;
        for (size_t j = 0; j < tab.sub_tabs.size(); j++) {
            bool active = (g_active_subtab == (int)j);
            const char* name = tab.sub_tabs[j];
            ImVec2 text_size = ImGui::CalcTextSize(name);

            if (active) {
                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImGui::GetWindowPos() + ImVec2(x - 5, 8),
                    ImGui::GetWindowPos() + ImVec2(x + text_size.x + 5, 27),
                    ImGui::GetColorU32(colors::BG_ACTIVE()), 3.0f);
            }

            ImGui::SetCursorPos(ImVec2(x, 12));
            if (active) {
                ImGui::TextColored(colors::TEXT_ACTIVE(), "%s", name);
            } else {
                ImGui::TextColored(colors::TEXT_DIM(), "%s", name);
            }

            // Click
            ImGui::SetCursorPos(ImVec2(x - 5, 8));
            ImGui::InvisibleButton(name, ImVec2(text_size.x + 10, 20));
            if (ImGui::IsItemClicked()) {
                g_active_subtab = (int)j;
            }

            x += text_size.x + 30.0f;
        }
        ImGui::PopStyleColor();
    }
    ImGui::EndChild();

    // ??? CONTENT ???????????????????????????????????????????????????????
    float content_y = header_height + subtab_height;
    float content_h = content_size.y - header_height - subtab_height - footer_height;

    ImGui::SetCursorPos(ImVec2(content_x, content_y));
    ImGui::BeginChild("##content", ImVec2(content_width, content_h), false);
    {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, colors::BG_CONTENT());
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

        // Dispatch to tab drawer
        switch (g_active_tab) {
            case 0: DrawAimbotContent(g_active_subtab); break;
            case 1: DrawRageContent(g_active_subtab); break;
            case 2: DrawLegitContent(g_active_subtab); break;
            case 3: DrawCommonContent(g_active_subtab); break;
            case 4: DrawVisualsContent(g_active_subtab); break;
            case 5: DrawInventoryContent(g_active_subtab); break;
            case 6: DrawMiscContent(g_active_subtab); break;
            case 7: DrawLuaContent(g_active_subtab); break;
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();
    }
    ImGui::EndChild();

    // ??? FOOTER ????????????????????????????????????????????????????????
    ImGui::SetCursorPos(ImVec2(0, content_size.y - footer_height));
    ImGui::BeginChild("##footer", ImVec2(content_size.x, footer_height), false);
    {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, colors::BG_SIDEBAR());

        // Left: username
        ImGui::SetCursorPos(ImVec2(20, 12));
        ImGui::TextColored(colors::TEXT_DIM(), "Awhat");

        // Right: expiry
        const char* expiry = "67 days";
        ImVec2 expiry_size = ImGui::CalcTextSize(expiry);
        ImGui::SetCursorPos(ImVec2(content_size.x - expiry_size.x - 20, 12));
        ImGui::TextColored(colors::TEXT_DIM(), "%s", expiry);

        ImGui::PopStyleColor();
    }
    ImGui::EndChild();

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

} // namespace ui




