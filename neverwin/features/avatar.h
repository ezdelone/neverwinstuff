#pragma once
#include "../sdk/game.h"
#include <windows.h>
#include <cstdint>

namespace features {

struct AvatarOverride {
    bool     enabled = false;
    uint64_t steam_id = 0;
    void*    texture = nullptr;
};

extern AvatarOverride g_avatar_override;

bool InitSteamAPI(sdk::SteamAPI& steam);
void* GetAvatar(uint64_t steam_id);
uint64_t GetLocalSteamID();

} // namespace features
