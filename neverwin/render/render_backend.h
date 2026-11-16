#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <cstdint>

namespace render {

struct RenderBackend {
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swap_chain = nullptr;
    ID3D11RenderTargetView* rtv = nullptr;
    bool initialized = false;
    int width = 0;
    int height = 0;
    uint64_t frame_count = 0;
};

extern RenderBackend g_backend;

bool InitRenderBackend(IDXGISwapChain* sc);
void ShutdownRenderBackend();
bool WorldToScreen(const float world[3], const float matrix[16], float screen[2]);

}
