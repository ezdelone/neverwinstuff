#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <MinHook.h>
#include <windows.h>
#include <thread>
#include <atomic>
#include "../core/console.h"
#include "../render/render_backend.h"
#include "../ui/menu.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

namespace hooks {

typedef long(__stdcall* PresentFn)(IDXGISwapChain*, UINT, UINT);
static PresentFn g_orig_present = nullptr;
static bool g_present_hooked = false;
static HWND g_hwnd = nullptr;
static WNDPROC g_orig_wndproc = nullptr;
static std::atomic<bool> g_menu_visible{false};

// ??? Menu key (configurable, default INSERT) ??????????????????????????????
static int g_menu_key = VK_INSERT;

void SetMenuKey(int vk) { g_menu_key = vk; }
int  GetMenuKey()       { return g_menu_key; }
bool IsMenuVisible()    { return g_menu_visible.load(); }
void SetMenuVisible(bool v) { g_menu_visible.store(v); }

// ??? WndProc ???????????????????????????????????????????????????????????????
static LRESULT CALLBACK WndProcHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) {
        if ((int)wParam == g_menu_key) {
            g_menu_visible.store(!g_menu_visible.load());
            LOG_INFO("[menu] %s", g_menu_visible.load() ? "opened" : "closed");
            return 1;
        }
    }

    if (g_menu_visible.load()) {
        // Block game input when menu is open
        if (msg >= WM_KEYFIRST && msg <= WM_KEYLAST) return 1;
        if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) return 1;
    }

    return CallWindowProcW(g_orig_wndproc, hwnd, msg, wParam, lParam);
}

// ??? ImGui init ????????????????????????????????????????????????????????????
static bool InitImGui(IDXGISwapChain* sc) {
    if (render::g_backend.initialized && g_hwnd)
        return true;

    DXGI_SWAP_CHAIN_DESC desc{};
    sc->GetDesc(&desc);
    g_hwnd = desc.OutputWindow;

    if (!g_hwnd) {
        LOG_ERROR("[render] no window handle");
        return false;
    }

    if (!render::InitRenderBackend(sc)) {
        LOG_ERROR("[render] backend init failed");
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.10f, 0.94f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.30f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.55f, 0.35f, 0.85f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.65f, 0.45f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.55f, 0.35f, 0.85f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.25f, 0.20f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.25f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.30f, 0.45f, 1.00f);

    ImGui_ImplWin32_Init(g_hwnd);
    ImGui_ImplDX11_Init(render::g_backend.device, render::g_backend.context);

    LOG_SUCCESS("[render] ImGui initialized (%dx%d)", render::g_backend.width, render::g_backend.height);
    return true;
}

// ??? Draw menu ?????????????????????????????????????????????????????????????
static void DrawOverlay() {
    if (!g_menu_visible.load())
        return;

    ImGui::SetNextWindowSize(ImVec2(750, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Astraea", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    // Header
    ImGui::Text("Astraea");
    ImGui::SameLine();
    ImGui::TextDisabled("Counter-Strike 2");
    ImGui::SameLine(ImGui::GetWindowWidth() - 200);
    ImGui::Text("Awhat | 67 days");
    ImGui::Separator();

    // Tabs
    static int active_tab = 0;
    const char* tabs[] = {"Aimbot", "Rage", "Legit", "Common", "Visuals", "Inventory", "Misc", "Lua"};

    ImGui::BeginChild("sidebar", ImVec2(150, -1), true);
    for (int i = 0; i < 8; i++) {
        if (ImGui::Selectable(tabs[i], active_tab == i)) {
            active_tab = i;
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("content", ImVec2(0, -30), true);
    ImGui::Text("Tab: %s", tabs[active_tab]);
    ImGui::EndChild();

    // Footer
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 25);
    ImGui::Text("Awhat");
    ImGui::SameLine(ImGui::GetWindowWidth() - 100);
    ImGui::Text("67 days");

    ImGui::End();
}

// ??? Present detour ????????????????????????????????????????????????????????
static long __stdcall PresentDetour(IDXGISwapChain* sc, UINT sync, UINT flags) {
    if (!g_present_hooked) {
        g_present_hooked = true;
        if (InitImGui(sc)) {
            g_orig_wndproc = (WNDPROC)SetWindowLongPtrW(g_hwnd, GWLP_WNDPROC, (LONG_PTR)WndProcHook);
            LOG_SUCCESS("[render] WndProc hooked");
        }
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    DrawOverlay();

    ImGui::EndFrame();
    ImGui::Render();

    render::g_backend.context->OMSetRenderTargets(1, &render::g_backend.rtv, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return g_orig_present(sc, sync, flags);
}

// ??? Find swap chain by polling ????????????????????????????????????????????
// Waits for game to create swap chain, then hooks Present
static void FindSwapChainAndHook() {
    LOG_INFO("[hook] waiting for game swap chain...");

    // Method: enumerate windows and try to hook via dummy device
    // If dummy fails, we poll for D3D11 device
    
    for (int attempt = 0; attempt < 60; attempt++) {  // 30 seconds max
        // Try to create dummy swap chain with game's window
        HWND hwnd = FindWindowA("Valve001", nullptr);  // CS2 window class
        if (!hwnd) {
            hwnd = FindWindowA(nullptr, "Counter-Strike 2");
        }

        if (hwnd) {
            ID3D11Device* device = nullptr;
            ID3D11DeviceContext* context = nullptr;
            IDXGISwapChain* swap_chain = nullptr;

            DXGI_SWAP_CHAIN_DESC sc_desc{};
            sc_desc.BufferCount = 2;
            sc_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sc_desc.BufferDesc.Width = 100;
            sc_desc.BufferDesc.Height = 100;
            sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sc_desc.OutputWindow = hwnd;
            sc_desc.SampleDesc.Count = 1;
            sc_desc.Windowed = TRUE;
            sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };

            HRESULT hr = D3D11CreateDeviceAndSwapChain(
                nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
                levels, 3, D3D11_SDK_VERSION,
                &sc_desc, &swap_chain, &device, nullptr, &context
            );

            if (SUCCEEDED(hr) && swap_chain) {
                void** vtable = *(void***)swap_chain;
                void* present_addr = vtable[8];

                LOG_INFO("[hook] Present vtable: 0x%p", present_addr);

                if (MH_CreateHook(present_addr, (void*)PresentDetour, (void**)&g_orig_present) == MH_OK) {
                    if (MH_EnableHook(present_addr) == MH_OK) {
                        LOG_SUCCESS("[hook] Present installed");
                        swap_chain->Release();
                        if (device) device->Release();
                        if (context) context->Release();
                        return;
                    }
                    MH_RemoveHook(present_addr);
                }

                swap_chain->Release();
                if (device) device->Release();
                if (context) context->Release();
            }
        }

        // Wait 500ms before retry
        Sleep(500);
    }

    LOG_ERROR("[hook] Present: could not find swap chain after 30s");
}

// ??? Install Present hook (async thread) ???????????????????????????????????
void InstallPresentHookAsync() {
    std::thread finder([]() {
        FindSwapChainAndHook();
    });
    finder.detach();
}

void RemovePresentHook() {
    if (g_orig_wndproc && g_hwnd) {
        SetWindowLongPtrW(g_hwnd, GWLP_WNDPROC, (LONG_PTR)g_orig_wndproc);
        g_orig_wndproc = nullptr;
    }
}

} // namespace hooks

