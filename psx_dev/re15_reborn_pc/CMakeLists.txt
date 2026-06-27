# RE1.5 Rebuilt — PC port (Phase 4.2..4.5.4, 2026-05-18)
#
# Cross-platform build target. Tested on:
#   - Windows: MSVC 2022 + CMake 3.21+ + Ninja or Visual Studio generators
#   - Linux:   gcc 11+ or clang 14+ + CMake 3.21+ + make or Ninja
#   - macOS:   should work (untested) — same gcc/clang toolchain
#
# Shares game logic + parsers + SCD VM with the PSX build via #include of
# common headers and direct CMake reference to shared .c files.
#
# Phase 4.2 introduced SDL2 (320×240 software framebuffer + 4× window scale).
# Phase 4.3 added asset loading (TIM textures).
# Phase 4.4 added SCD VM. Phase 4.5 added MD1 mesh rendering.
#
# Build commands:
#   Windows (MSVC):
#     mkdir build && cd build
#     cmake ..
#     cmake --build . --config Release
#
#   Linux:
#     mkdir build && cd build
#     cmake -DCMAKE_BUILD_TYPE=Release ..
#     cmake --build .
#
# SDL2 deps on Linux (apt example for Debian/Ubuntu):
#   sudo apt install libsdl2-dev libasound2-dev libpulse-dev libx11-dev \
#                    libxext-dev libxrandr-dev libxcursor-dev libxi-dev \
#                    libxinerama-dev libxxf86vm-dev libgl1-mesa-dev
# The FetchContent build below will compile SDL2 from source against
# whichever of those it finds. Audio/video subsystems we don't use can
# be missing — SDL2 will just disable them.

cmake_minimum_required(VERSION 3.21)

project(
    re15_reborn_pc
    LANGUAGES    C
    VERSION      0.5.4
    DESCRIPTION  "RE1.5 reborn — PC port (cross-platform)"
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)  # stick to standard C — no GNU extensions

# Default to Release if the user didn't pick one (Linux make uses this).
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---- SDL2 via FetchContent --------------------------------------------------
# Downloads SDL2 source on first build, compiles with our toolchain. Static
# link → single self-contained binary on Windows (no SDL2.dll needed) and a
# single ELF on Linux (still needs system libs like libX11/libGL at runtime).
include(FetchContent)
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-2.30.7
    GIT_SHALLOW    TRUE
)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
set(SDL_TEST   OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL2)

# ---- Engine source ----------------------------------------------------------
# Share headers + cross-platform sources with the PSX build. Single source of
# truth for the engine API — only render/input/asset backends differ.
set(SHARED_INCLUDE ${CMAKE_SOURCE_DIR}/../re15_reborn/include)
set(SHARED_SRC     ${CMAKE_SOURCE_DIR}/../re15_reborn/src)

add_executable(re15_reborn_pc
    main.c
    src/render_pc.c
    src/input_pc.c
    src/asset_pc.c
    src/bg_pc.c
    src/audio_pc.c
    src/room_pc.c          # PC ARCH half of the per-room RDT loader (file, not CD)
    # Shared with PSX build — one implementation, two backends
    ${SHARED_SRC}/tim_common.c
    ${SHARED_SRC}/scd_vm.c
    ${SHARED_SRC}/scd_room_setup.c
    ${SHARED_SRC}/md1_common.c
    ${SHARED_SRC}/bss_common.c
    ${SHARED_SRC}/bss_vlc.c
    ${SHARED_SRC}/bss_mdec.c
    ${SHARED_SRC}/vab_common.c
    ${SHARED_SRC}/emd_common.c
    ${SHARED_SRC}/skeleton_common.c
    ${SHARED_SRC}/camera_common.c
    ${SHARED_SRC}/game_state.c
    ${SHARED_SRC}/player_common.c
    ${SHARED_SRC}/aot_common.c
    ${SHARED_SRC}/inventory_common.c
    ${SHARED_SRC}/rdt_common.c
    ${SHARED_SRC}/re15_collision.c
    ${SHARED_SRC}/stair_common.c
    ${SHARED_SRC}/game_step_common.c
    ${SHARED_SRC}/room_common.c
    ${SHARED_SRC}/actor_common.c
    ${SHARED_SRC}/actor_locomotion.c
    ${SHARED_SRC}/anim_select_common.c
    ${SHARED_SRC}/enemy_common.c
    ${SHARED_SRC}/re15_to_re2.c
    ${SHARED_SRC}/msg_common.c
    ${SHARED_SRC}/pri_common.c
    ${SHARED_SRC}/light_common.c
    ${SHARED_SRC}/rotor_common.c
    src/skeleton_trig_pc.c
)

target_include_directories(re15_reborn_pc PRIVATE
    include
    ${SHARED_INCLUDE}
)

target_compile_definitions(re15_reborn_pc PRIVATE
    RE15_PLATFORM_PC=1
)

target_link_libraries(re15_reborn_pc PRIVATE
    SDL2::SDL2-static
)

# MinGW builds in this workspace use a regular C main() entry point and do not
# provide SDL_main. Linking SDL2main there causes an unresolved SDL_main symbol.
if(NOT MINGW)
    target_link_libraries(re15_reborn_pc PRIVATE SDL2::SDL2main)
endif()

# Linux/Unix: SDL2-static needs the system threading + dynamic-loader libs
# (FetchContent's SDL2-static target *should* propagate these automatically,
#  but adding them explicitly makes cross-compilation more predictable).
if(UNIX AND NOT APPLE)
    target_link_libraries(re15_reborn_pc PRIVATE pthread dl m)
endif()

# Math library is automatic on Windows MSVC; needed explicitly on Linux/gcc.
if(NOT MSVC)
    target_link_libraries(re15_reborn_pc PRIVATE m)
endif()

# Compiler warning flags — tuned per toolchain so neither shouts spurious
# warnings while still catching real bugs.
if(MSVC)
    target_compile_options(re15_reborn_pc PRIVATE
        /W4 /WX-
    )
    # Silence the noisy "this function may be unsafe" warning for sprintf-class
    # functions — we use snprintf with explicit sizes, but MSVC warns anyway.
    target_compile_definitions(re15_reborn_pc PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    # gcc / clang: enable common warnings but don't fail on them. SDL2 itself
    # triggers some so -Werror would be hostile.
    target_compile_options(re15_reborn_pc PRIVATE
        -Wall -Wextra
        -Wno-unused-parameter   # SDL callbacks / unused argc/argv
        -Wno-unused-function    # dead-code branches across platforms
    )
endif()
