// Global definitions — all extern declarations resolved here.
// Included from main.cpp to avoid ODR violations.

#include "sdk/game.h"
#include "features/antiaim.h"
#include "features/movement.h"
#include "features/doubletap.h"
#include "features/avatar.h"
#include "features/aimbot.h"
#include "render/render_backend.h"
#include "core.h"

namespace sdk {
    GamePointers g_game{};
    SteamAPI     g_steam{};
}

namespace features {
    AntiAimState   g_aa{};
    MovementConfig g_movement{};
    DoubleTapState g_dt{};
    AvatarOverride g_avatar_override{};
    RageConfig     g_rage{};
}

namespace render {
    RenderBackend  g_backend{};
}

namespace core {
    InitState g_init{};
}
