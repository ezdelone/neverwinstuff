#include "core.h"
#include "core/console.h"
#include "sdk/pattern_scanner.h"
#include "sdk/game.h"
#include "hooks/hook_manager.h"
#include "hooks/present_hook.h"
#include "hooks/create_move_impl.h"

#include "lua/lua_api_full.h"
#include "render/render_backend.h"
#include "ui/menu.h"
#include "config.h"

#include <MinHook.h>
#include <windows.h>

namespace core {

bool Initialize() {
    // ?? Step 0: Console ?????????????????????????????????????????????????
    Console::Instance().Init("Astraea");
    Console::Instance().EnableFileLog("C:\\astraea.log");
    LOG_SUCCESS("[astraea] initializing...");

    g_init.total_steps = 10;
    g_init.step = 1;

    // ?? Steps 1-2: Pattern scanner ?????????????????????????????????????
    g_init.phase = InitPhase::Patterns;
    g_init.status_text = "Scanning patterns";
    LOG_INFO("[init] 1/10 ? pattern scanner");
    sdk::PatternScanner::Instance().Init();
    g_init.step = 3;

    // ?? Steps 3-4: Schema resolution ????????????????????????????????????
    g_init.phase = InitPhase::Schema;
    g_init.status_text = "Resolving schema";
    LOG_INFO("[init] 3/10 ? schema fields");
    // Schema offsets are resolved by game at runtime
    g_init.step = 5;

    // ?? Steps 5-6: Hook installation ????????????????????????????????????
    g_init.phase = InitPhase::Hooks;
    g_init.status_text = "Installing hooks";
    LOG_INFO("[init] 5/10 ? installing hooks");

    // Initialize MinHook
    MH_STATUS mh = MH_Initialize();
    if (mh != MH_OK && mh != MH_ERROR_ALREADY_INITIALIZED) {
        LOG_ERROR("[init] MinHook init failed: %d", (int)mh);
    } else {
        LOG_SUCCESS("[init] MinHook initialized");
    }

    // Install CreateMove hook
    void* create_move = sdk::PatternScanner::Instance().Resolve("client.dll", "create_move");
    if (create_move) {
        hooks::InstallCreateMoveHook(create_move);
    } else {
        LOG_WARN("[init] create_move not found, skipping");
    }

    // Install Present hook via dummy swap chain
    hooks::InstallPresentHookAsync();

    g_init.step = 7;

    // ?? Step 7: Lua state ??????????????????????????????????????????????
    g_init.phase = InitPhase::Lua;
    g_init.status_text = "Loading Lua";
    LOG_INFO("[init] 7/10 ? Lua state");
    // Present hook will be installed when swap chain is available
    // We hook Present through vtable when we get a swap chain
    g_init.step = 8;

    // ?? Step 8: Rendering ??????????????????????????????????????????????
    g_init.phase = InitPhase::Rendering;
    g_init.status_text = "Initializing render";
    LOG_INFO("[init] 8/10 ? render backend");

    // Find swap chain through D3D11
    // For now, Present hook installs lazily on first frame
    // (see present_hook.h ? InstallPresentHook is called when we get swap chain)
    g_init.step = 9;

    // ?? Step 9: Script scan ?????????????????????????????????????????????
    g_init.phase = InitPhase::Scripts;
    g_init.status_text = "Loading scripts";
    LOG_INFO("[init] 9/10 ? script scan");
    g_init.step = 10;

    // ?? Step 10: Complete ???????????????????????????????????????????????
    g_init.phase = InitPhase::Complete;
    g_init.status_text = "Initialization complete";
    LOG_SUCCESS("[init] 10/10 ? complete");
    LOG_SUCCESS("[astraea] ready | press INSERT to open menu");

    return true;
}

void Shutdown() {
    LOG_INFO("[astraea] shutting down...");

    // Remove Present hook
    hooks::RemovePresentHook();

    // Remove all hooks
    hooks::HookManager::Instance().RemoveAll();
    LOG_INFO("[astraea] hooks removed");

    // Shutdown render
    render::ShutdownRenderBackend();
    LOG_INFO("[astraea] render backend released");

    // Close console
    Console::Instance().Shutdown();
}

} // namespace core



