# RE1.5 Rebuilt — Dual-Target Engine (Phase 4)

A new Resident Evil engine in C, built from scratch using PSn00bSDK, designed
to run RE1.5's content on RE2-faithful architecture. Two parallel build targets:

| Target | Location | Toolchain | Output |
|---|---|---|---|
| **PSX** | `psx_dev/re15_reborn/` | PSn00bSDK + gcc-mipsel | `re15_reborn.exe` (20 KB PS-X EXE) |
| **PC** | `psx_dev/re15_reborn_pc/` | MSVC 2022 / CMake | `re15_reborn_pc.exe` (Windows console) |

Shared headers in `psx_dev/re15_reborn/include/` keep the engine API
platform-agnostic. Platform-specific code lives in each target's `src/`.

## Phase 4.1 Status (2026-05-18) — DONE

Both targets build and run:

```
PSX: re15_reborn.exe boots, shows splash + frame counter + pad state
PC:  re15_reborn_pc.exe prints heartbeat every 64 frames for 10 seconds
```

## Roadmap

Each phase adds engine functionality to **both** targets in parallel.
Sessions per phase: rough estimate.

| Phase | What | Sessions |
|---|---|---|
| 4.1 ✅ | Skeleton: boot, input, frame loop, splash text | 1 |
| 4.2 | PC: SDL2 window + GPU-accelerated 2D primitives. PSX: VRAM atlas, sprite framework. | 3-5 |
| 4.3 | Asset loaders: RDT room containers, MD1 meshes, TIM textures, VAB audio. CD reading on PSX, file I/O on PC. | 5-8 |
| 4.4 | SCD VM in C — re-implement RE1.5's script interpreter (~90 opcodes). | 8-12 |
| 4.5 | 3D rendering: GTE matrices on PSX, equivalent math on PC. MDEC backgrounds. | 5-8 |
| 4.6 | Audio: libsnd-faithful VAB/SEQ playback, BGM/SFX/voice. RE2-faithful invariants. | 8-12 |
| 4.7 | Save/Load + game state, inventory, item box. | 3-5 |
| 4.8 | Combat / weapon FSM. | 5-8 |
| 4.9 | Final integration + polish. | 5-10 |

**Total: ~40-70 sessions.** Long-term project. Each session moves the
engine closer to playable on both PSX and PC.

## Architecture principles

- **No surgical patching of RE1.5's broken FSMs.** We're not patching the
  legacy EXE anymore (that's `scripts/patch_save_final.py` for backward compat).
- **RE2-faithful invariants** for every subsystem (save touches no audio,
  audio doesn't depend on save state, etc.).
- **Asset compatibility:** read RE1.5's existing files unchanged. Re-extract
  via `de.re15.*` Java extractors if format changes (it doesn't).
- **Platform abstraction:** engine logic platform-agnostic in shared headers;
  render/input/audio/file_io have PSX and PC backends.
- **Test as we go:** PC build runs immediately for fast iteration. PSX build
  runs in DuckStation for hardware validation.

## Build instructions

### PSX
```bash
cd psx_dev/re15_reborn
mkdir build && cd build
cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE='C:/PSn00bSDK/sdk/PSn00bSDK-0.24-win32/lib/libpsn00b/cmake/sdk.cmake' ..
cmake --build .
# → build/re15_reborn.exe (PS-X EXE format, load in DuckStation)
```

### PC (Windows — MSVC)
```bash
cd psx_dev/re15_reborn_pc
mkdir build && cd build
cmake ..
cmake --build . --config Release
# → build/Release/re15_reborn_pc.exe (statically-linked SDL2, ~1.8 MB)
```

### PC (Linux — gcc/clang)
```bash
# Install SDL2 build dependencies (Debian/Ubuntu example):
sudo apt install build-essential cmake git \
                 libasound2-dev libpulse-dev libx11-dev libxext-dev \
                 libxrandr-dev libxcursor-dev libxi-dev libxinerama-dev \
                 libxxf86vm-dev libgl1-mesa-dev

cd psx_dev/re15_reborn_pc
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
./re15_reborn_pc
```

The first configure downloads SDL2 source via CMake FetchContent (~28 sec on
a warm cache) and compiles it static into the executable — no SDL2 runtime
deps needed at deploy time besides the standard system libs (libX11, libGL).
