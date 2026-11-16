#pragma once
// Reconstructed from sub_18006EE50 / sub_1800A3AB0
// Lua API: modules, hooks, state.

#include <cstdint>
#include <string>

namespace lua {

// ─── Lua state (from sub_18006EE50) ──────────────────────────────────────
struct LuaState {
    void*   L;              // lua_State*
    bool    allow_insecure; // a1+75
    void*   modules;        // wl_modules table
    void*   current_script; // +328
    int     hook_count;     // max 64
};

// ─── Hook entry (from sub_1800A3AB0) ──────────────────────────────────────
struct LuaHook {
    void*    target;
    void*    detour;
    void*    original;
    uint64_t script_id;
    bool     enabled;
};

// ─── API namespaces (from ida_lua_api.json) ──────────────────────────────
// ui, hooks, entity, trace, material, render, http, steam, config,
// callbacks, events, game, user, network, inventory, weapon, prediction,
// sound, fs, ffi, jit, os, string, table, math, bit, wl

// ─── Module list (from sub_18006EE50) ────────────────────────────────────
enum class Module {
    Table,
    String,
    Math,
    Bit,
    Jit,
    FFI,
    OS
};

// ─── Standard functions ──────────────────────────────────────────────────
// load, loadstring, require (active)
// dofile, loadfile (disabled — set to nil)

// ─── Script paths ─────────────────────────────────────────────────────────
constexpr const char* SCRIPTS_PATH   = "C:\\neverwin\\scripts\\";
constexpr const char* LIB_PATH       = "C:\\neverwin\\scripts\\lib\\";
constexpr const char* DATA_PATH      = "C:\\neverwin\\scripts\\data\\";
constexpr const char* IMAGES_PATH    = "C:\\neverwin\\scripts\\images\\";
constexpr const char* FONTS_PATH     = "C:\\neverwin\\scripts\\fonts\\";
constexpr const char* PARTICLES_PATH = "C:\\neverwin\\scripts\\particles\\";
constexpr const char* OWNERS_PATH    = "C:\\neverwin\\lua_owners.txt";

// ─── hooks.add logic (from sub_1800A3AB0) ─────────────────────────────────
inline bool ValidateLuaHook(void* target, void* callback) {
    if (!target)   return false;
    if (!callback) return false;

    // VirtualQuery check — both must be executable
    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(target, &mbi, sizeof(mbi)))
        return false;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & 0xF0) == 0)
        return false;

    if (!VirtualQuery(callback, &mbi, sizeof(mbi)))
        return false;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & 0xF0) == 0)
        return false;

    return true;
}

// ─── require logic (from sub_18006E7B0) ──────────────────────────────────
inline std::string ResolveModule(const std::string& name) {
    // C:\neverwin\scripts\lib\<name>.lua
    return std::string(LIB_PATH) + name + ".lua";
}

// ─── HTTP (from sub_180095270) ────────────────────────────────────────────
struct HttpRequest {
    std::string url;
    std::string method;
    std::string body;
    int         timeout_ms;
    std::string user_agent;   // "newerwin"
};

constexpr const char* HTTP_UA = "newerwin";
constexpr int HTTP_MAX_THREADS = 8;

} // namespace lua
