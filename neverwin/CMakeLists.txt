cmake_minimum_required(VERSION 3.20)
project(astraea LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(minhook)

set(IMGUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.8
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

if (MSVC)
    add_compile_options(/W4 /permissive- /Zc:preprocessor)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

set(SRC ${CMAKE_CURRENT_SOURCE_DIR})

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC d3d11 dxgi d3dcompiler)

add_library(astraea SHARED
    ${SRC}/main.cpp
    ${SRC}/globals.cpp
    ${SRC}/core.cpp
    ${SRC}/sdk/pattern_scanner.cpp
    ${SRC}/hooks/hook_manager.cpp
    ${SRC}/render/render_backend.cpp
    ${SRC}/features/avatar.cpp
)

target_include_directories(astraea PRIVATE
    ${SRC}
    ${SRC}/sdk
    ${SRC}/features
    ${SRC}/hooks
    ${SRC}/lua
    ${SRC}/render
    ${SRC}/ui
    ${SRC}/core
)

target_link_libraries(astraea PRIVATE
    imgui
    minhook
    d3d11
    dxgi
    dxguid
    psapi
    winhttp
    ws2_32
)
