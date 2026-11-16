#pragma once
// Reconstructed from sub_18006EE50 / ida_lua_full_api.json / ida_lua_api.json
// Complete Lua API implementation skeleton.

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

namespace lua {

// ─── Forward declarations ─────────────────────────────────────────────────
struct LuaState;

// ─── Value types (LuaJIT compatible) ─────────────────────────────────────
enum class LuaType : int {
    NONE      = -1,
    NIL       = 0,
    BOOLEAN   = 1,
    LIGHTUSERDATA = 2,
    NUMBER    = 3,
    STRING    = 4,
    TABLE     = 5,
    FUNCTION  = 6,
    USERDATA  = 7,
    THREAD    = 8,
    CDATA     = 10  // LuaJIT
};

// ─── Lua C function ────────────────────────────────────────────────────────
typedef int (*LuaCFunction)(LuaState* L);

// LuaState defined in lua_api.h

// ─── API registration structure ───────────────────────────────────────────
struct ApiFunction {
    const char*    name;
    LuaCFunction   func;
    const char*    module;     // nullptr = global
};

// ─── All namespaces (from ida_lua_api.json) ────────────────────────────────
// ui, hooks, entity, trace, material, render, http, steam, config,
// callbacks, events, game, user, network, inventory, weapon, prediction,
// sound, fs, ffi, jit, os, string, table, math, bit, wl

namespace api {

// ─── print ─────────────────────────────────────────────────────────────
inline int Print(LuaState* L) {
    // sub_18004ED60
    // reads string arg, logs to wl_state
    return 0;
}

// ─── ui ────────────────────────────────────────────────────────────────
namespace ui {
    // ui.group(name) → creates group
    // ui.tab(name) → creates tab
    // ui.sidebar(name) → sidebar control
    // ui.find(path...) → finds control by path
    // ui.beta_exploits([bool]) → get/set
}

// ─── hooks ─────────────────────────────────────────────────────────────
namespace hooks {
    // hooks.original(index) → get original function
    // hooks.target(name) → get hook target
    // hooks.active(index) → check if hook is active
    // hooks.remove(index) → remove hook
    // hooks.add(target, callback) → install MinHook from Lua
    // hooks.list() → list all hooks

    // From sub_1800A3AB0:
    // - max 64 hooks
    // - requires "allow insecure" setting
    // - validates executable memory via VirtualQuery
    // - both target and callback must be executable
}

// ─── entity ────────────────────────────────────────────────────────────
namespace entity {
    // entity.get_local() → local player
    // entity.get_all() → all entities
    // entity.get_by_index(index) → entity by index
    // entity.get_by_handle(handle) → entity by handle
    // entity.get_class_name(entity) → class name
    // entity.get_team(entity) → team number
    // entity.get_health(entity) → health
    // entity.get_origin(entity) → origin vector
    // entity.get_velocity(entity) → velocity vector
    // entity.get_hitbox_position(entity, hitbox) → world position
    // entity.is_dormant(entity) → dormant check
    // entity.is_alive(entity) → alive check
    // entity.is_enemy(entity) → enemy check
    // entity.__eq / __tostring → metamethods
}

// ─── trace ─────────────────────────────────────────────────────────────
namespace trace {
    // trace.line(start, end, mask) → ray trace
    // trace.hull(start, end, mins, maxs, mask) → hull trace
    // trace.bullet(start, end, weapon) → bullet trace with penetration
    // trace.can_penetrate(start, end, weapon) → bool
    // trace.damage(start, end, weapon, target) → float
    // → all call sub_1802A7CF0
}

// ─── material ──────────────────────────────────────────────────────────
namespace material {
    // material.find(name) → find material
    // material.create(name, shader) → create material
    // material.set_color(mat, r, g, b, a) → set color
    // material.set_flag(mat, flag, value) → set material flag
}

// ─── render ────────────────────────────────────────────────────────────
namespace render {
    // render.text(x, y, text, font, color) → draw text
    // render.rect(x, y, w, h, color) → filled rect
    // render.outlined_rect(x, y, w, h, color) → outline
    // render.line(x1, y1, x2, y2, color) → line
    // render.circle(x, y, r, color) → circle
    // render.texture(x, y, w, h, tex, color) → texture
    // render.measure_text(text, font) → width, height
    // render.get_screen_size() → w, h
    // render.world_to_screen(vec3) → x, y
}

// ─── http ──────────────────────────────────────────────────────────────
namespace http {
    // http.get(url) → async GET
    // http.post(url, body, headers) → async POST
    // → sub_180095270 (WinHTTP)
}

// ─── steam ─────────────────────────────────────────────────────────────
namespace steam {
    // steam.get_steam_id() → uint64
    // steam.get_persona_name() → string
    // steam.get_avatar() → texture handle
}

// ─── config ────────────────────────────────────────────────────────────
namespace config {
    // config.get(key) → value
    // config.set(key, value) → set
    // config.save(name) → save config
    // config.load(name) → load config
    // config.get_default(key) → default value
}

// ─── callbacks ─────────────────────────────────────────────────────────
namespace callbacks {
    // callbacks.add(event_name, function) → register callback
    // callbacks.remove(id) → remove
    // Events: paint, create_move, run_command, frame_stage,
    //          level_init, level_shutdown, player_hurt, bullet_impact,
    //          vote_start, vote_pass, vote_failed
}

// ─── game ──────────────────────────────────────────────────────────────
namespace game {
    // game.get_local_player() → local player
    // game.get_map_name() → map name
    // game.get_tickrate() → tickrate
    // game.is_valve_server() → bool
    // game.get_game_rules() → rules
    // game.time() → current time
    // game.tick_count() → current tick
}

// ─── user ──────────────────────────────────────────────────────────────
namespace user {
    // user.get_username() → username
    // user.get_branch() → branch
    // user.get_expires() → expiry
    // user.is_lifetime() → bool
}

// ─── network ───────────────────────────────────────────────────────────
namespace network {
    // network.get_ping() → ping ms
    // network.get_loss() → packet loss
    // network.get_net_channel() → net channel
}

// ─── inventory ─────────────────────────────────────────────────────────
namespace inventory {
    // inventory.get_items() → all items
    // inventory.get_item_by_index(index) → item
    // inventory.get_skin(item) → skin
    // inventory.set_skin(item, skin) → apply skin
    // inventory.get_sticker(item, slot) → sticker
}

// ─── weapon ────────────────────────────────────────────────────────────
namespace weapon {
    // weapon.get_active() → active weapon
    // weapon.get_all() → all weapons
    // weapon.get_spread(weapon) → spread value
    // weapon.get_inaccuracy(weapon) → inaccuracy
    // weapon.can_fire(weapon) → bool
    // weapon.get_ammo(weapon) → ammo
    // weapon.get_damage(weapon) → damage
    // weapon.is_reloading(weapon) → bool
}

// ─── prediction ────────────────────────────────────────────────────────
namespace prediction {
    // prediction.get_local() → local player prediction
    // prediction.simulate(cmd) → simulate tick
    // prediction.can_edge_jump() → bool
    // prediction.can_jump_bug() → bool
}

// ─── sound ─────────────────────────────────────────────────────────────
namespace sound {
    // sound.play(name, volume) → play sound
    // sound.get_volume(name) → get volume
}

// ─── fs ────────────────────────────────────────────────────────────────
namespace fs {
    // fs.read(path) → file contents
    // fs.write(path, data) → write file
    // fs.exists(path) → bool
    // fs.list(dir) → list files
    // fs.create_dir(path) → create directory
}

// ─── ffi (LuaJIT) ──────────────────────────────────────────────────────
namespace ffi {
    // ffi.cdef(def) → C declaration
    // ffi.new(type, ...) → new cdata
    // ffi.cast(type, value) → cast to type
    // ffi.typeof(type) → type info
    // ffi.sizeof(type) → size in bytes
    // ffi.C → C namespace
}

} // namespace api

// ─── Script lifecycle ─────────────────────────────────────────────────────
enum class ScriptState {
    Unloaded,
    Loading,
    Loaded,
    Errored,
    Unloading
};

struct Script {
    std::string   name;
    std::string   path;
    std::string   author;
    ScriptState   state;
    void*         chunk;
    void*         env;
    bool          allow_insecure;
};

// ─── Script manager ───────────────────────────────────────────────────────
class ScriptManager {
public:
    static ScriptManager& Instance() {
        static ScriptManager inst;
        return inst;
    }

    void ScanScripts() {
        // Scan C:\neverwin\scripts\ for .lua files
        // sub_180069AF0
    }

    void LoadScript(const std::string& path) {
        // sub_18006DC50
        // 1. Read file
        // 2. luaL_loadbuffer
        // 3. Set up environment
        // 4. Execute chunk
    }

    void UnloadScript(Script& script) {
        // Remove all hooks owned by script
        // Remove all callbacks
        // Remove UI elements
    }

    void ReloadScript(Script& script) {
        UnloadScript(script);
        LoadScript(script.path);
    }

private:
    std::vector<Script> scripts_;
};

// ─── Callback system ──────────────────────────────────────────────────────
struct Callback {
    std::string              event;
    std::function<void()>    fn;
    void*                    script;
    int                      id;
};

class CallbackManager {
public:
    static CallbackManager& Instance() {
        static CallbackManager inst;
        return inst;
    }

    int Add(const std::string& event, std::function<void()> fn, void* script) {
        Callback cb{};
        cb.event = event;
        cb.fn = fn;
        cb.script = script;
        cb.id = next_id_++;
        callbacks_.push_back(cb);
        return cb.id;
    }

    void Remove(int id) {
        callbacks_.erase(
            std::remove_if(callbacks_.begin(), callbacks_.end(),
                [id](const Callback& c) { return c.id == id; }),
            callbacks_.end()
        );
    }

    void Fire(const std::string& event) {
        for (auto& cb : callbacks_) {
            if (cb.event == event) {
                cb.fn();
            }
        }
    }

private:
    std::vector<Callback> callbacks_;
    int next_id_ = 1;
};

} // namespace lua

