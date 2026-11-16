#pragma once
// Reconstructed from sub_1803C93F0 (main thread init)

#include "config.h"
#include <cstdint>

namespace core {

enum class InitPhase {
    None = 0, Patterns, Schema, Hooks, Lua, Rendering, Scripts, Complete, Failed
};

struct InitState {
    InitPhase  phase = InitPhase::None;
    int        step = 0;
    int        total_steps = 10;
    const char* status_text = "";
    bool       failed = false;
    const char* error_text = "";
};

extern InitState g_init;

// Declaration only — implementation in core.cpp
bool Initialize();
void Shutdown();

} // namespace core
