// Astraea — reconstructed from neverwin.dll
// Entry point + global instances + init/shutdown.

#include "core.h"
#include "config.h"
#include "net.h"
#include "render.h"
#include "sdk/game.h"
#include "sdk/pattern_scanner.h"
#include "hooks/hook_manager.h"
#include "hooks/create_move.h"
#include "features/aimbot.h"
#include "features/antiaim.h"
#include "features/autowall.h"
#include "features/avatar.h"
#include "features/doubletap.h"
#include "features/esp.h"
#include "features/inventory.h"
#include "features/movement.h"
#include "features/prediction.h"
#include "lua/lua_api.h"
#include "lua/lua_api_full.h"
#include "render/render_backend.h"
#include "ui/menu.h"

#include <windows.h>

// Global instances defined in globals.cpp


// ─── Thread proc ──────────────────────────────────────────────────────────
static DWORD WINAPI InitThread(LPVOID) {
    core::Initialize();
    return 0;
}

// ─── DllMain ───────────────────────────────────────────────────────────────
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        // From sub_1803C93F0: CreateThread → main init
        HANDLE thread = CreateThread(nullptr, 0, InitThread, hModule, 0, nullptr);
        if (thread) {
            CloseHandle(thread);
        }
    }
    else if (reason == DLL_PROCESS_DETACH) {
        core::Shutdown();
    }
    return TRUE;
}


