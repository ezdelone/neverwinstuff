// Render backend — full implementation
// Reconstructed from sub_1801CEBD0 / sub_180363B60

#include "render_backend.h"

namespace render {

// g_backend defined in globals.cpp

bool InitRenderBackend(IDXGISwapChain* sc) {
    if (g_backend.initialized)
        return true;

    if (FAILED(sc->GetDevice(__uuidof(ID3D11Device), (void**)&g_backend.device)))
        return false;

    g_backend.device->GetImmediateContext(&g_backend.context);
    g_backend.swap_chain = sc;

    ID3D11Texture2D* buffer = nullptr;
    if (FAILED(sc->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer)))
        return false;

    if (FAILED(g_backend.device->CreateRenderTargetView(buffer, nullptr, &g_backend.rtv))) {
        buffer->Release();
        return false;
    }
    buffer->Release();

    DXGI_SWAP_CHAIN_DESC desc{};
    sc->GetDesc(&desc);
    g_backend.width = desc.BufferDesc.Width;
    g_backend.height = desc.BufferDesc.Height;

    g_backend.initialized = true;
    return true;
}

void ShutdownRenderBackend() {
    if (g_backend.rtv) {
        g_backend.rtv->Release();
        g_backend.rtv = nullptr;
    }
    if (g_backend.context) {
        g_backend.context->Release();
        g_backend.context = nullptr;
    }
    if (g_backend.device) {
        g_backend.device->Release();
        g_backend.device = nullptr;
    }
    g_backend.swap_chain = nullptr;
    g_backend.initialized = false;
}

bool WorldToScreen(const float world[3], const float matrix[16], float screen[2]) {
    float clip_w = matrix[3] * world[0] +
                   matrix[7] * world[1] +
                   matrix[11] * world[2] +
                   matrix[15];

    if (clip_w < 0.001f)
        return false;

    float inv_w = 1.0f / clip_w;
    float ndc_x = (matrix[0] * world[0] + matrix[4] * world[1] + matrix[8] * world[2] + matrix[12]) * inv_w;
    float ndc_y = (matrix[1] * world[0] + matrix[5] * world[1] + matrix[9] * world[2] + matrix[13]) * inv_w;

    screen[0] = (ndc_x * 0.5f + 0.5f) * (float)g_backend.width;
    screen[1] = (ndc_y * -0.5f + 0.5f) * (float)g_backend.height;

    return true;
}

} // namespace render

