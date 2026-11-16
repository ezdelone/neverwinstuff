// Hook manager — full implementation with MinHook
// Reconstructed from sub_1801D0610

#include "hook_manager.h"
#include <MinHook.h>
#include <mutex>

namespace hooks {

static std::mutex g_hook_mutex;

bool HookManager::MinHook_CreateHook(void* target, void* detour, void** original) {
    MH_STATUS status = MH_CreateHook(target, detour, original);
    return status == MH_OK;
}

bool HookManager::MinHook_EnableHook(void* target) {
    MH_STATUS status = MH_EnableHook(target);
    return status == MH_OK;
}

bool HookManager::MinHook_DisableHook(void* target) {
    MH_STATUS status = MH_DisableHook(target);
    return status == MH_OK;
}

bool HookManager::MinHook_RemoveHook(void* target) {
    MH_STATUS status = MH_RemoveHook(target);
    return status == MH_OK;
}

bool HookManager::Install(void* target, void* detour, void** original, const char* name) {
    std::lock_guard<std::mutex> lock(g_hook_mutex);

    if (!target || !detour)
        return false;

    // Check if already hooked
    for (auto& h : hooks_) {
        if (h.enabled && h.target == target) {
            return false;
        }
    }

    // Count active hooks
    int active = 0;
    for (auto& h : hooks_) {
        if (h.enabled) active++;
    }
    if (active >= MAX_HOOKS) {
        return false;
    }

    // Validate executable memory (from sub_1801D0610)
    if (!IsExecutableMemory(target)) {
        return false;
    }
    if (!IsExecutableMemory(detour)) {
        return false;
    }

    // Initialize MinHook if not yet
    static bool mh_initialized = false;
    if (!mh_initialized) {
        if (MH_Initialize() != MH_OK) {
            return false;
        }
        mh_initialized = true;
    }

    // Create hook
    void* trampoline = nullptr;
    if (!MinHook_CreateHook(target, detour, &trampoline)) {
        return false;
    }

    if (!MinHook_EnableHook(target)) {
        MinHook_RemoveHook(target);
        return false;
    }

    // Find free slot or append
    HookRecord record{};
    record.script_id = 0;
    record.target = target;
    record.detour = detour;
    record.original = trampoline;
    record.enabled = true;

    for (auto& h : hooks_) {
        if (!h.enabled) {
            h = record;
            if (original) *original = trampoline;
            return true;
        }
    }

    hooks_.push_back(record);
    if (original) *original = trampoline;
    return true;
}

bool HookManager::Remove(void* target) {
    std::lock_guard<std::mutex> lock(g_hook_mutex);

    for (auto& h : hooks_) {
        if (h.enabled && h.target == target) {
            MinHook_DisableHook(target);
            MinHook_RemoveHook(target);
            h.enabled = false;
            return true;
        }
    }
    return false;
}

void HookManager::RemoveAll() {
    std::lock_guard<std::mutex> lock(g_hook_mutex);

    for (auto& h : hooks_) {
        if (h.enabled) {
            MinHook_DisableHook(h.target);
            MinHook_RemoveHook(h.target);
            h.enabled = false;
        }
    }

    // Uninitialize MinHook
    static bool mh_initialized = true;
    if (mh_initialized) {
        MH_Uninitialize();
        mh_initialized = false;
    }
}

void* HookManager::GetOriginal(void* target) const {
    for (auto& h : hooks_) {
        if (h.enabled && h.target == target)
            return h.original;
    }
    return nullptr;
}

size_t HookManager::ActiveCount() const {
    size_t n = 0;
    for (auto& h : hooks_) {
        if (h.enabled) n++;
    }
    return n;
}

bool HookManager::IsExecutableMemory(void* ptr) {
    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(ptr, &mbi, sizeof(mbi)))
        return false;
    return mbi.State == MEM_COMMIT &&
          (mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ |
                          PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) != 0;
}

} // namespace hooks
