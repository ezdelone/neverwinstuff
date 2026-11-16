// Steam avatar — full implementation
// Reconstructed from sub_1800DD490 / sub_1800DD5F0 / sub_1800DD940

#include "avatar.h"
#include "../render/render_backend.h"
#include <d3d11.h>

namespace features {

// g_avatar_override defined in globals.cpp

bool InitSteamAPI(sdk::SteamAPI& steam) {
    if (steam.initialized)
        return true;

    HMODULE mod = GetModuleHandleA("steam_api64.dll");
    if (!mod)
        return false;

    steam.initialized = true;

    auto friends_init = (void*(*)(void))GetProcAddress(mod, "SteamAPI_SteamFriends_v018");
    auto utils_init   = (void*(*)(void))GetProcAddress(mod, "SteamAPI_SteamUtils_v010");
    auto user_init    = (void*(*)(void))GetProcAddress(mod, "SteamAPI_SteamUser_v023");

    steam.ISteamFriends = friends_init ? friends_init() : nullptr;
    steam.ISteamUtils   = utils_init ? utils_init() : nullptr;
    steam.ISteamUser    = user_init ? user_init() : nullptr;

    steam.GetMediumFriendAvatar  = (void*)GetProcAddress(mod, "SteamAPI_ISteamFriends_GetMediumFriendAvatar");
    steam.RequestUserInformation = (void*)GetProcAddress(mod, "SteamAPI_ISteamFriends_RequestUserInformation");
    steam.GetImageSize           = (void*)GetProcAddress(mod, "SteamAPI_ISteamUtils_GetImageSize");
    steam.GetImageRGBA           = (void*)GetProcAddress(mod, "SteamAPI_ISteamUtils_GetImageRGBA");
    steam.GetSteamID             = (void*)GetProcAddress(mod, "SteamAPI_ISteamUser_GetSteamID");

    return steam.ISteamFriends != nullptr &&
           steam.ISteamUtils != nullptr &&
           steam.GetImageRGBA != nullptr;
}

// ─── Create DX11 texture from RGBA buffer (from sub_1800DD5F0) ─────────────
static void* CreateTextureFromRGBA(const uint8_t* rgba, uint32_t width, uint32_t height) {
    if (!render::g_backend.device || !rgba || width == 0 || height == 0)
        return nullptr;

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data{};
    data.pSysMem = rgba;
    data.SysMemPitch = width * 4;
    data.SysMemSlicePitch = width * height * 4;

    ID3D11Texture2D* texture = nullptr;
    if (FAILED(render::g_backend.device->CreateTexture2D(&desc, &data, &texture)))
        return nullptr;

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
    srv_desc.Format = desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = 1;

    ID3D11ShaderResourceView* srv = nullptr;
    if (FAILED(render::g_backend.device->CreateShaderResourceView(texture, &srv_desc, &srv))) {
        texture->Release();
        return nullptr;
    }
    texture->Release();

    return srv;
}

// ─── Avatar cache (from sub_1800DD940) ─────────────────────────────────────
struct AvatarCacheEntry {
    uint64_t steam_id;
    void*    texture;
    bool     requested;
    uint32_t last_request_time;
    uint32_t image_handle;
};

static const int MAX_AVATAR_CACHE = 128;
static AvatarCacheEntry g_avatar_cache[MAX_AVATAR_CACHE]{};
static int g_cache_count = 0;

static AvatarCacheEntry* FindCacheEntry(uint64_t steam_id) {
    for (int i = 0; i < g_cache_count; i++) {
        if (g_avatar_cache[i].steam_id == steam_id)
            return &g_avatar_cache[i];
    }
    return nullptr;
}

static AvatarCacheEntry* GetOrCreateCacheEntry(uint64_t steam_id) {
    AvatarCacheEntry* entry = FindCacheEntry(steam_id);
    if (entry)
        return entry;

    if (g_cache_count >= MAX_AVATAR_CACHE)
        return nullptr;

    entry = &g_avatar_cache[g_cache_count++];
    entry->steam_id = steam_id;
    entry->texture = nullptr;
    entry->requested = false;
    entry->last_request_time = 0;
    entry->image_handle = 0;
    return entry;
}

void* GetAvatar(uint64_t steam_id) {
    // Override check
    if (g_avatar_override.enabled) {
        if (g_avatar_override.steam_id == 0 || g_avatar_override.steam_id == steam_id) {
            return g_avatar_override.texture;
        }
    }

    // Init Steam API
    if (!InitSteamAPI(sdk::g_steam))
        return nullptr;

    if (!sdk::g_steam.GetMediumFriendAvatar || !sdk::g_steam.ISteamFriends)
        return nullptr;

    // Check cache
    AvatarCacheEntry* entry = GetOrCreateCacheEntry(steam_id);
    if (!entry)
        return nullptr;

    // Return cached texture
    if (entry->texture)
        return entry->texture;

    // Rate limit: 1 request per second
    uint32_t now = GetTickCount();
    if (entry->last_request_time != 0 && (int)(now - entry->last_request_time) < 1000)
        return nullptr;

    // Get avatar image handle
    auto get_avatar = (uint32_t(*)(void*, uint64_t))sdk::g_steam.GetMediumFriendAvatar;
    uint32_t image = get_avatar(sdk::g_steam.ISteamFriends, steam_id);

    if (image == 0) {
        // Request user info and retry later
        if (!entry->requested && sdk::g_steam.RequestUserInformation) {
            auto request = (bool(*)(void*, uint64_t, bool))sdk::g_steam.RequestUserInformation;
            request(sdk::g_steam.ISteamFriends, steam_id, false);
            entry->requested = true;
        }
        return nullptr;
    }

    entry->last_request_time = now;
    entry->image_handle = image;

    // Get image size
    uint32_t w = 0, h = 0;
    auto get_size = (bool(*)(void*, uint32_t, uint32_t*, uint32_t*))sdk::g_steam.GetImageSize;
    if (!get_size(sdk::g_steam.ISteamUtils, image, &w, &h))
        return nullptr;

    if (w == 0 || h == 0 || w > 256 || h > 256)
        return nullptr;

    // Allocate RGBA buffer
    uint32_t size = w * h * 4;
    uint8_t* buffer = new uint8_t[size];

    auto get_rgba = (bool(*)(void*, uint32_t, uint8_t*, uint32_t))sdk::g_steam.GetImageRGBA;
    if (!get_rgba(sdk::g_steam.ISteamUtils, image, buffer, size)) {
        delete[] buffer;
        return nullptr;
    }

    // Create texture
    void* texture = CreateTextureFromRGBA(buffer, w, h);
    delete[] buffer;

    entry->texture = texture;
    return texture;
}

uint64_t GetLocalSteamID() {
    if (!InitSteamAPI(sdk::g_steam))
        return 0;

    if (!sdk::g_steam.GetSteamID || !sdk::g_steam.ISteamUser)
        return 0;

    auto get_id = (uint64_t(*)(void*))sdk::g_steam.GetSteamID;
    return get_id(sdk::g_steam.ISteamUser);
}

} // namespace features

