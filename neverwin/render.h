#pragma once
// Reconstructed from sub_1801CEBD0 (Present hook) / sub_180323780 (menu draw)
// DX11 rendering + ImGui overlay.

#include <d3d11.h>
#include <dxgi.h>
#include <cstdint>

namespace render {

// ─── Render state (from sub_1801CEBD0) ────────────────────────────────────
struct RenderState {
    ID3D11Device*           device;
    ID3D11DeviceContext*    context;
    IDXGISwapChain*         swap_chain;
    ID3D11RenderTargetView* rtv;
    ID3D11Texture2D*        back_buffer;

    bool    imgui_initialized;
    bool    menu_visible;
    int     menu_key;          // INSERT (0x2D)
    float   menu_scale;
    int     width;
    int     height;
};

extern RenderState g_render;

// ─── Menu structure (from config_tree + ida_present_menu.json) ────────────
struct MenuItem {
    enum class Type {
        Checkbox,
        Slider,
        ColorPicker,
        ComboBox,
        KeyBind,
        Button,
        Separator,
        Group,
        Tab,
        Category
    };

    Type        type;
    const char* label;
    const char* config_key;
    float       min_value;
    float       max_value;
    const char* format;
};

// ─── Sidebar tabs (from screenshot + config_tree) ─────────────────────────
namespace tabs {
    constexpr const char* AIMBOT          = "Aimbot";
    constexpr const char* RAGE            = "Rage";
    constexpr const char* LEGIT           = "Legit";
    constexpr const char* COMMON          = "Common";
    constexpr const char* VISUALS         = "Visuals";
    constexpr const char* INVENTORY       = "Inventory";
    constexpr const char* MISCELLANEOUS   = "Miscellaneous";
    constexpr const char* LUA             = "Lua";
}

// ─── Rage subtabs (from decompilation) ────────────────────────────────────
namespace rage_subtabs {
    constexpr const char* MAIN        = "Main";
    constexpr const char* SELECTION   = "Selection";
    constexpr const char* OTHER       = "Other";
    constexpr const char* ANTIAIM     = "Anti Aim";
}

// ─── Menu items structure (from sub_1800F6940) ───────────────────────────
struct RageConfig {
    // Main
    bool    enabled;
    bool    silent_aim;
    bool    automatic_fire;
    bool    limiters;
    bool    refine_shot;
    float   field_of_view;         // 0-180

    // Selection
    bool    hitboxes_head;
    bool    hitboxes_chest;
    bool    hitboxes_stomach;
    float   extrapolation;          // 0-100
    int     hit_chance;             // 0-100
    int     min_damage;             // 0-100
    bool    quick_stop;
    bool    quick_scope;

    // Other
    int     history;                // 0-200ms
    bool    delay_shot;
    int     delay_damage;
    bool    hit_chance_override;
    int     hit_chance_override_value;
    bool    min_damage_override;
    int     min_damage_override_value;
    bool    remove_spread;
    bool    duck_peek_assist;
    bool    quick_peek_assist;
    bool    double_precision;
    bool    double_tap;
    bool    force_shot;
    int     force_shot_hitchance;
    bool    auto_scope;
    bool    auto_stop;
    bool    stop_in_air;
    bool    stop_on_land;
    bool    stop_between_shots;
};

// ─── Anti-Aim config (from sub_1801C4890) ──────────────────────────────────
struct AntiAimConfig {
    bool    enabled;
    int     leg_movement;       // 0=off, 1=sliding
    bool    pitch_custom;
    bool    avoid_backstab;
    bool    auto_adjust;
    float   yaw_offset;
    bool    jitter;
    bool    spin;
    int     jitter_type;
    bool    jitter_disable_air;
    bool    jitter_disable_override;
    float   jitter_range;
    float   spin_speed;
    int     pitch_order;
    float   pitch;
    int     manual;
    bool    hide_shots;
    bool    mouse_override;
};

// ─── About panel (from sub_180323780) ──────────────────────────────────────
struct AboutInfo {
    const char* product_name;    // "Astraea" (was "neverwin")
    const char* window_title;    // "Counter-Strike 2"
    const char* branch;          // "Release"
    const char* updated;         // "Oct 25 2026"
    const char* expires;         // "67 days" (was "Lifetime")
    const char* username;        // "Awhat" (was "cracker")
    const char* copyright;       // "neverwin.one © 2025-2026"
};

// ─── Present hook logic (from sub_1801CEBD0) ──────────────────────────────
inline bool OnPresent(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) {
    if (!g_render.device) {
        // First frame — initialize
        if (FAILED(swap_chain->GetDevice(__uuidof(ID3D11Device), (void**)&g_render.device)))
            return false;

        g_render.device->GetImmediateContext(&g_render.context);

        // Create RTV
        ID3D11Texture2D* back_buffer = nullptr;
        if (FAILED(swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer)))
            return false;

        g_render.device->CreateRenderTargetView(back_buffer, nullptr, &g_render.rtv);
        back_buffer->Release();

        // Init ImGui here
        g_render.imgui_initialized = true;
    }

    // Begin frame
    g_render.context->OMSetRenderTargets(1, &g_render.rtv, nullptr);

    // Draw menu if visible
    if (g_render.menu_visible) {
        // sub_180323780 — draw menu
        // All tabs: Aimbot, Rage, Legit, Common, Visuals, Inventory, Misc, Lua
    }

    // Draw overlays (ESP, watermark, indicators)

    return true;
}

// ─── WndProc (for menu toggle) ────────────────────────────────────────────
inline LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN && wParam == VK_INSERT) {
        g_render.menu_visible = !g_render.menu_visible;
        return 1;
    }
    return CallWindowProcW(nullptr, hwnd, msg, wParam, lParam);
}

} // namespace render
