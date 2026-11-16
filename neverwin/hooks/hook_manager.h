#pragma once
#include <windows.h>
#include <cstdint>
#include <vector>

namespace hooks {

struct HookRecord {
    uint64_t script_id = 0;
    void*    target = nullptr;
    void*    detour = nullptr;
    void*    original = nullptr;
    bool     enabled = false;
};

class HookManager {
public:
    static HookManager& Instance() { static HookManager h; return h; }
    static constexpr int MAX_HOOKS = 64;

    bool Install(void* target, void* detour, void** original = nullptr, const char* name = "");
    bool Remove(void* target);
    void RemoveAll();
    void* GetOriginal(void* target) const;
    size_t ActiveCount() const;

private:
    std::vector<HookRecord> hooks_;

    static bool IsExecutableMemory(void* ptr);
    static bool MinHook_CreateHook(void* target, void* detour, void** original);
    static bool MinHook_EnableHook(void* target);
    static bool MinHook_DisableHook(void* target);
    static bool MinHook_RemoveHook(void* target);
};

} // namespace hooks
