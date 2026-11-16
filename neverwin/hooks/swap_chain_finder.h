// Swap chain finder ? creates a dummy D3D11 device to get Present vtable
// Used to install Present hook before game creates its own swap chain

#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <windows.h>
#include "../core/console.h"
#include "present_hook.h"

namespace hooks {

static bool FindAndHookPresent() {
    // Create dummy device
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swap_chain = nullptr;

    DXGI_SWAP_CHAIN_DESC sc_desc{};
    sc_desc.BufferCount = 2;
    sc_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sc_desc.BufferDesc.Width = 640;
    sc_desc.BufferDesc.Height = 480;
    sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sc_desc.OutputWindow = GetForegroundWindow();
    sc_desc.SampleDesc.Count = 1;
    sc_desc.Windowed = TRUE;
    sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        feature_levels, 3, D3D11_SDK_VERSION,
        &sc_desc, &swap_chain, &device, nullptr, &context
    );

    if (FAILED(hr)) {
        // Try software renderer
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0,
            feature_levels, 3, D3D11_SDK_VERSION,
            &sc_desc, &swap_chain, &device, nullptr, &context
        );
    }

    if (FAILED(hr) || !swap_chain) {
        LOG_ERROR("[hook] failed to create dummy swap chain (hr=0x%08X)", (unsigned)hr);
        return false;
    }

    // Get Present function pointer from vtable
    void** vtable = *(void***)swap_chain;
    void* present_addr = vtable[8];  // IDXGISwapChain::Present is index 8

    LOG_INFO("[hook] Present vtable: 0x%p", present_addr);

    // Install hook
    if (MH_CreateHook(present_addr, (void*)PresentDetour, (void**)&g_orig_present) != MH_OK) {
        LOG_ERROR("[hook] Present CreateHook failed");
        swap_chain->Release();
        if (device) device->Release();
        if (context) context->Release();
        return false;
    }

    if (MH_EnableHook(present_addr) != MH_OK) {
        LOG_ERROR("[hook] Present EnableHook failed");
        swap_chain->Release();
        if (device) device->Release();
        if (context) context->Release();
        return false;
    }

    LOG_SUCCESS("[hook] Present installed via dummy swap chain");

    // Release dummy objects (hook stays)
    swap_chain->Release();
    if (device) device->Release();
    if (context) context->Release();

    return true;
}

} // namespace hooks
