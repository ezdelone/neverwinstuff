#pragma once
// Reconstructed from config_tree.json (3,618 keys, 67 groups)
// Config system with typed values.

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace config {

// ─── Value types ─────────────────────────────────────────────────────────
enum class ValueType {
    Bool,
    Int,
    Float,
    String,
    Color,
    KeyBind,
    Vec2,
    Vec3,
    Vec4
};

struct Color {
    uint8_t r, g, b, a;
};

struct KeyBind {
    int      key;
    bool     require_active;
    uint8_t  mode;  // 0=always, 1=hold, 2=toggle
};

// ─── Config value ─────────────────────────────────────────────────────────
struct Value {
    ValueType type;
    union {
        bool     b;
        int      i;
        float    f;
    };
    std::string s;
    Color       color;
    KeyBind     bind;
    float       vec[4];
};

// ─── Config entry ────────────────────────────────────────────────────────
struct Entry {
    std::string key;       // e.g. "ragebot.hitchance"
    Value       value;
    Value       default_value;
    std::string group;     // e.g. "ragebot"
};

// ─── Main config storage ─────────────────────────────────────────────────
class Config {
public:
    static Config& Instance() {
        static Config inst;
        return inst;
    }

    // ─── Getters ───────────────────────────────────────────────────────
    bool GetBool(const std::string& key, bool default_val = false) const {
        auto it = entries_.find(key);
        return it != entries_.end() ? it->second.value.b : default_val;
    }

    int GetInt(const std::string& key, int default_val = 0) const {
        auto it = entries_.find(key);
        return it != entries_.end() ? it->second.value.i : default_val;
    }

    float GetFloat(const std::string& key, float default_val = 0.0f) const {
        auto it = entries_.find(key);
        return it != entries_.end() ? it->second.value.f : default_val;
    }

    Color GetColor(const std::string& key, Color default_val = {255,255,255,255}) const {
        auto it = entries_.find(key);
        return it != entries_.end() ? it->second.value.color : default_val;
    }

    KeyBind GetBind(const std::string& key) const {
        auto it = entries_.find(key);
        return it != entries_.end() ? it->second.value.bind : KeyBind{};
    }

    // ─── Setters ───────────────────────────────────────────────────────
    void SetBool(const std::string& key, bool value) {
        Entry e{};
        e.key = key;
        e.value.type = ValueType::Bool;
        e.value.b = value;
        e.default_value = e.value;
        entries_[key] = e;
    }

    void SetInt(const std::string& key, int value) {
        Entry e{};
        e.key = key;
        e.value.type = ValueType::Int;
        e.value.i = value;
        e.default_value = e.value;
        entries_[key] = e;
    }

    void SetFloat(const std::string& key, float value) {
        Entry e{};
        e.key = key;
        e.value.type = ValueType::Float;
        e.value.f = value;
        e.default_value = e.value;
        entries_[key] = e;
    }

    // ─── Serialization (from config_load.c) ─────────────────────────────
    void Save(const std::string& path) {
        // Format: key=value per line (from sub_180042850)
        // Path: C:\neverwin\configs\<name>.cfg
    }

    void Load(const std::string& path) {
        // Read key=value pairs
    }

    size_t GetEntryCount() const { return entries_.size(); }

private:
    std::unordered_map<std::string, Entry> entries_;
};

// ─── All 67 config groups (from config_tree.json) ─────────────────────────
namespace groups {
    constexpr const char* MENU_KEY           = "menu_key";
    constexpr const char* BINDS              = "binds";
    constexpr const char* BINDS_SCHEME       = "binds_scheme";
    constexpr const char* MENU_ACCENT        = "menu_accent";
    constexpr const char* MENU_THEME         = "menu_theme";
    constexpr const char* MENU_SCALE         = "menu_scale";
    constexpr const char* WIDGETS_SCALE      = "widgets_scale";
    constexpr const char* ESP_UNIVERSAL_SCALING = "esp_universal_scaling";
    constexpr const char* ESP_SCALE          = "esp_scale";
    constexpr const char* UNITS              = "units";
    constexpr const char* MENU_LANGUAGE      = "menu_language";
    constexpr const char* PRESETS_AUTO_SAVE  = "presets_auto_save";
    constexpr const char* SYNCHRONIZATION    = "synchronization";
    constexpr const char* DEVELOPER_MODE     = "developer_mode";
    constexpr const char* ITEMS              = "items";
    constexpr const char* TARGETS            = "targets";
    constexpr const char* MODEL              = "model";
    constexpr const char* MOVEMENT           = "movement";
    constexpr const char* NL_MOVEMENT        = "nl_movement";
    constexpr const char* FEATURES           = "features";
    constexpr const char* MISC               = "misc";
    constexpr const char* PEN_CROSSHAIR      = "pen_crosshair";
    constexpr const char* ANTIAIM            = "antiaim";
    constexpr const char* THIRD_PERSON       = "third_person";
    constexpr const char* TRACERS            = "tracers";
    constexpr const char* LUA                = "lua";
    constexpr const char* TRAIL              = "trail";
    constexpr const char* SPARKS             = "sparks";
    constexpr const char* KILL_EFFECT        = "kill_effect";
    constexpr const char* REMOVALS           = "removals";
    constexpr const char* MEDIA              = "media";
    constexpr const char* HOTKEY_LIST        = "hotkey_list";
    constexpr const char* INDICATORS         = "indicators";
    constexpr const char* WATERMARK          = "watermark";
    constexpr const char* SPECTATORS         = "spectators";
    constexpr const char* PETS               = "pets";
    constexpr const char* RAGDOLL           = "ragdoll";
    constexpr const char* SCOPE              = "scope";
    constexpr const char* FOV                = "fov";
    constexpr const char* HIT_EFFECT         = "hit_effect";
    constexpr const char* IMPACTS            = "impacts";
    constexpr const char* HITSOUNDS          = "hitsounds";
    constexpr const char* HITMARKER          = "hitmarker";
    constexpr const char* BOMB_TEXT          = "bomb_text";
    constexpr const char* GRENADE_PREDICTION = "grenade_prediction";
    constexpr const char* AUTO_BUY           = "auto_buy";
    constexpr const char* GRENADE_RELEASE    = "grenade_release";
    constexpr const char* GRENADE_ESP        = "grenade_esp";
    constexpr const char* GRENADE_WARNING    = "grenade_warning";
    constexpr const char* SMOKE_RADIUS       = "smoke_radius";
    constexpr const char* INFERNO_RADIUS     = "inferno_radius";
    constexpr const char* SPREAD_CIRCLE      = "spread_circle";
    constexpr const char* SKINS              = "skins";
    constexpr const char* INVENTORY          = "inventory";
    constexpr const char* VIEWMODEL          = "viewmodel";
    constexpr const char* WORLDS             = "worlds";
    constexpr const char* AMBIENCE           = "ambience";
    constexpr const char* WORLD              = "world";
    constexpr const char* SCREEN             = "screen";
    constexpr const char* SKY                = "sky";
    constexpr const char* MOTION_BLUR        = "motion_blur";
    constexpr const char* WEATHER            = "weather";
    constexpr const char* RAGEBOT            = "ragebot";
    constexpr const char* AUTOPEEK           = "autopeek";
    constexpr const char* LEGITBOT           = "legitbot";
    constexpr const char* TRIGGERBOT         = "triggerbot";
    constexpr const char* VISUALS_WORLD      = "visuals_world";
}

} // namespace config
