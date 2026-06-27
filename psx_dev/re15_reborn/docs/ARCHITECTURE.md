# RE1.5 Rebuilt — Architecture Documentation

**Last updated: 2026-05-18 (Phase 4.6.0)**

This document tracks the engine architecture as it grows. Every phase
update must touch this file (per memory rule
`feedback_architecture_and_tests_mandatory`).

---

## 1. Top-level layout

```
psx_dev/
├── re15_reborn/              # PSX target + shared code
│   ├── CMakeLists.txt        # PSn00bSDK build + ISO image
│   ├── main.c                # game loop
│   ├── system.cnf            # PSX boot config
│   ├── iso.xml               # mkpsxiso ISO description
│   ├── include/
│   │   ├── re15_engine.h     # engine API (cross-platform)
│   │   ├── re15_tim.h        # TIM parser API
│   │   └── re15_scd.h        # SCD VM API
│   ├── src/
│   │   ├── render.c          # PSX backend: GPU / OT / SPRT
│   │   ├── input.c           # PSX backend: PAD via BIOS
│   │   ├── asset_psx.c       # PSX backend: TIM → VRAM
│   │   ├── tim_common.c      # SHARED: TIM parser (no platform deps)
│   │   ├── scd_vm.c          # SHARED: SCD interpreter
│   │   └── scd_demo.c        # SHARED: demo bytecode
│   ├── assets/
│   │   └── test.tim          # RE1.5 DOOR00.tim (8bpp test asset)
│   ├── docs/
│   │   └── ARCHITECTURE.md   # this file
│   └── tests/
│       ├── CMakeLists.txt    # host-build test runner
│       ├── test_tim.c        # TIM parser tests
│       └── test_scd.c        # SCD VM tests
└── re15_reborn_pc/           # PC target
    ├── CMakeLists.txt        # MSVC/CMake + SDL2 FetchContent
    ├── main.c                # same logic as PSX main.c
    └── src/
        ├── render_pc.c       # PC backend: SDL2 framebuffer
        ├── input_pc.c        # PC backend: keyboard (stub)
        └── asset_pc.c        # PC backend: fopen + RGB conversion
```

**Shared sources** (tim_common.c, scd_vm.c, scd_demo.c) live under
`re15_reborn/src/` and are referenced from the PC build's CMakeLists via
absolute path.

---

## 2. Build & run

### PSX

```powershell
$env:Path = "C:\Program Files\CMake\bin;C:\PSn00bSDK\sdk\PSn00bSDK-0.24-win32\bin;" + $env:Path
cd psx_dev\re15_reborn
mkdir build; cd build
cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE='C:/PSn00bSDK/sdk/PSn00bSDK-0.24-win32/lib/libpsn00b/cmake/sdk.cmake' ..
cmake --build .
# → re15_reborn.bin + .cue → load in DuckStation
```

### PC (cross-platform)

Windows (MSVC):
```powershell
cd psx_dev\re15_reborn_pc
mkdir build; cd build
cmake ..              # downloads SDL2 first time (~28s)
cmake --build . --config Release
# → build\Release\re15_reborn_pc.exe
```

Linux (gcc/clang):
```bash
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

Code portability practices used throughout:
- `snprintf` (universal), never `sprintf_s` (MSVC-only) or `asprintf` (gnu-only)
- No `<windows.h>` / `<unistd.h>` includes — SDL2 handles all timing/threading
- `<stdint.h>` integer types — never `__int64` or `LONG`
- File I/O via `<stdio.h>` `fopen`/`fread`/`fclose` — no `CreateFile`
- Forward-slash paths in source — Windows CRT and POSIX both accept them
- `volatile int running = 1` loop pattern avoids the MSVC-vs-gcc warning split
  between "unreachable return" and "no return from non-void"

### Tests (host build)

```powershell
cd psx_dev\re15_reborn\tests
mkdir build; cd build
cmake ..
cmake --build . --config Release
ctest -C Release
```

---

## 3. Engine API (cross-platform)

Defined in `include/re15_engine.h`. Both targets implement this API
with platform-specific backends.

### Types

```c
typedef struct {
    uint32_t frame_count;        // frames rendered since boot
    uint16_t pad_current;         // pad bits THIS frame (active-high)
    uint16_t pad_previous;        // pad bits LAST frame
    uint16_t pad_pressed;         // bits set this frame only
    uint16_t pad_released;        // bits cleared this frame only
} re15_engine_state_t;

extern re15_engine_state_t g_engine;
```

### Subsystem init/tick

| Function | Purpose | PSX impl | PC impl |
|---|---|---|---|
| `re15_render_init()` | open framebuffer | `ResetGraph` + DispEnv/DrawEnv + FntLoad | `SDL_CreateWindow` + Renderer + Texture |
| `re15_render_begin_frame()` | per-frame setup | (no-op) | `SDL_PollEvent` + clear framebuffer |
| `re15_render_end_frame()` | present | `DrawSync` + `VSync` + `DrawOTagEnv` | `SDL_UpdateTexture` + `SDL_RenderPresent` |
| `re15_input_init()` | start PAD | `InitPAD` + `StartPAD` | (no-op) |
| `re15_input_tick()` | poll pad | read PADTYPE | (stub — Phase 4.x adds SDL keyboard) |
| `re15_assets_init()` | upload assets | parse TIM → VRAM via `LoadImage` | (no-op; PC reads files on demand) |

### Drawing primitives

| Function | Purpose |
|---|---|
| `re15_debug_text(x,y,z,text)` | draw text at (x,y), Z-bucket z |
| `re15_render_tile(x,y,w,h,z,r,g,b)` | solid-color rectangle |
| `re15_render_test_tim(x,y,z)` | render the loaded test TIM (Phase 4.3 helper) |

---

## 4. Subsystem: Rendering

### Flow (PSX)

```
re15_render_init()
  └─ ResetGraph(0)
  └─ FntLoad(960, 0)           ← debug font at VRAM (960, 0)
  └─ Two RenderBuffers @ VRAM (0, 0) and (0, 240)
     ├─ DRAWENV + DISPENV per buffer
     ├─ OT[16] ordering table
     └─ buffer[8192] primitive packet pool
  └─ SetDispMask(1)             ← turn on output

frame loop:
  re15_render_begin_frame()    ← no-op
  ┌── game/demo draws primitives ──┐
  │   re15_render_tile(...)         │  → TILE prim into OT[z]
  │   re15_debug_text(...)          │  → FntSort prim into OT[z]
  │   re15_render_test_tim(...)     │  → SPRT + DR_TPAGE into OT[z]
  └─────────────────────────────────┘
  re15_render_end_frame()
    ├─ DrawSync(0)              ← wait for prev frame GPU done
    ├─ VSync(0)                 ← wait for vblank (60 Hz)
    ├─ PutDispEnv(disp_buffer)  ← swap display
    ├─ DrawOTagEnv(draw_buffer) ← submit OT to GPU
    └─ flip active buffer + ClearOTagR
```

### Flow (PC)

```
re15_render_init()
  └─ SDL_Init(VIDEO | EVENTS)
  └─ SDL_CreateWindow(1280x960)
  └─ SDL_CreateRenderer(VSync)
  └─ SDL_CreateTexture(320x240 streaming)
  └─ SDL_RenderSetLogicalSize(320, 240)
  └─ s_framebuffer[320*240] u32 RGBA8888

frame loop:
  re15_render_begin_frame()
    ├─ SDL_PollEvent (handle quit)
    └─ memset framebuffer to CLEAR_RGB
  ┌── game/demo draws primitives ──┐
  │   re15_render_tile  → CPU fill rect in framebuffer  │
  │   re15_debug_text   → 6x8 bitmap font blit         │
  └────────────────────────────────────────────────────┘
  re15_render_end_frame()
    ├─ SDL_UpdateTexture(framebuffer)
    ├─ SDL_RenderClear + Copy
    └─ SDL_RenderPresent     ← VSync-blocked
```

### VRAM layout (PSX)

```
(0,   0)..(320, 240)    Framebuffer #1
(0, 240)..(320, 480)    Framebuffer #2
(0, 500)..(256, 501)    Test TIM CLUT (Phase 4.3)
(640, 0)..(768, 256)    Test TIM pixel data (Phase 4.3)
(960, 0)..(1024, 256)   PSn00bSDK debug font (FntLoad)
remaining               free for Phase 4.5 atlas
```

---

## 5. Subsystem: Asset loading

### TIM parser (`re15_tim.h` + `tim_common.c`, shared)

```
re15_tim_t {
    int bpp;                    // 4 / 8 / 16 / 24
    int has_clut;
    int clut_x, clut_y;          // original VRAM coords (informational)
    int clut_entries;
    const u16 *clut;             // RGB555 entries, points into source
    int data_x, data_y;          // original VRAM coords (informational)
    int width, height;           // PIXEL dimensions
    const u16 *pixels;           // pixel data, points into source
}

int re15_tim_parse(const u8 *data, int size, re15_tim_t *out);
    // 0 = ok, -1 = bad magic / truncated
```

### PSX upload (`asset_psx.c`)

```
re15_assets_init()
  └─ re15_tim_parse(test_tim, ...) → re15_tim_t
  └─ Upload CLUT via LoadImage at OUR coords (0, 500) — ignore baked-in
  └─ Upload pixel data via LoadImage at OUR coords (640, 0)
  └─ getTPage(bpp_mode, 0, 640, 0) → re15_test_tim_tpage
  └─ getClut(0, 500)               → re15_test_tim_clut
  └─ DrawSync(0)

Why ignore baked-in coords: RE1.5 TIMs have hardcoded VRAM positions that
collide with our framebuffer (DOOR00 wants (0,0) = OVER framebuffer #1).
Phase 4.5 will introduce a proper VRAM allocator.
```

### MD1 parser (`re15_md1.h` + `md1_common.c`, shared, Phase 4.5.1)

```
re15_md1_t {
    raw, raw_size                   — caller-owned source
    length, unknown, object_count   — file header (12B)
    mesh_count                      — = object_count / 2, clamped MD1_MAX_MESHES (32)
    meshes[MD1_MAX_MESHES] = {
        tri_vertices, tri_vertex_count
        tri_normals,  tri_normal_count
        triangles,    triangle_count
        triangle_uvs
        quad_vertices, quad_vertex_count
        quad_normals,  quad_normal_count
        quads,         quad_count
        quad_uvs
    }
}

int re15_md1_parse(const uint8_t *data, int size, re15_md1_t *out);
   // 0 = ok, -1 = truncation / OOB offset
```

Non-allocating: all out-pointers reference into `data`. Bounds-checked
against `size` for every block (vertices, normals, faces, UVs).

### PSX 3D pipeline (`mesh_psx.c`, Phase 4.5.2)

GTE (Geometry Transformation Engine) — fixed-function 3D coprocessor on the
PSX CPU. Phase 4.5.2 wraps it for rendering MD1 wireframes.

```
mesh_psx_init():
   gte_InitGeom()                  — enable coprocessor
   gte_SetGeomScreen(300)          — perspective focal length
   gte_SetGeomOffset(160, 120)     — project origin = screen centre

mesh_psx_render_test(z, angle_4096):
   RotMatrix({0, angle, 0}, &mtx)   — build Y-axis rotation
   TransMatrix(&mtx, {0, 0, 4000})  — push mesh forward in Z
   gte_SetRotMatrix(&mtx)
   gte_SetTransMatrix(&mtx)
   for each tri:
       gte_ldv3(&v0, &v1, &v2)       — load 3 verts into GTE
       gte_rtpt()                    — rotate+translate+project
       gte_stsxy3(&xy0, &xy1, &xy2)  — read 3 packed s16:s16 screen XYs
       emit 3× LINE_F2 (triangle edges)
```

PSX angle convention: 4096 = full turn (not 360°). Z buckets identical to PC
so visual diff between targets immediately surfaces coord-system bugs.

**Quads (Phase 4.5.3):** RE1.5 MD1 stores quads in a separate section from
triangles with their own vertex/normal arrays. Quad winding order is
(v0, v1, v3, v2) — emit edges as 0→1, 1→3, 3→2, 2→0 (NOT the naive
0→1, 1→2, 2→3, 3→0 which would cross). GTE pipeline: rtpt on first 3
vertices + ldv0/rtps on the 4th (one-vertex variant).

**Backface culling (Phase 4.5.4):**

PSX side: after `gte_rtpt`, call `gte_nclip()` + `gte_stopz(&n)`. n>0 means
front-facing (winding matches PSX convention). n<=0 = backfacing OR
degenerate. Skip the prim.

PC side: same logic in CPU via screen-space cross product:
`((bx-ax) * (cy-ay) - (by-ay) * (cx-ax)) <= 0` ⇒ backface.
CRITICAL: cast to `long long` not `long`. Windows MSVC long is 32-bit and
30000×60000 already overflows s32, causing front faces to be mis-culled
on large meshes. `test_backface.c` test 4 covers this.

For quads we cull on the FIRST 3 vertices' winding only — RE1.5 quads are
planar to a tight tolerance, so v0/v1/v2 winding equals v0/v1/v3 winding.

**Per-mesh position (Phase 4.5.4):** the renderer signature now takes
`pos_x/pos_y/pos_z`. The translation matrix uses these instead of the old
hardcoded (0,0,4000). Multiple meshes can render at different positions
just by re-uploading rotation+translation for each call.

**Textured triangles (Phase 4.5.5):** new `mode` parameter (`MESH_PSX_MODE_TEXTURED`)
switches the renderer from wireframe LINE_F2 to POLY_FT3 (PSX) /
SDL_RenderGeometry (PC). Per-face data:
- UVs come from `re15_md1_tri_uv_t` / `re15_md1_quad_uv_t` in the MD1
- CLUT + tpage are OVERRIDDEN to point at our re-mapped TIM upload coords
  (`re15_test_tim_clut`, `re15_test_tim_tpage`) — the baked values in the
  MD1 reference original RE1.5 VRAM layout that conflicts with our
  framebuffer placement

Quads are emitted as 2 textured triangles using the (v0,v1,v3) and (v0,v3,v2)
split — preserves the (v0,v1,v3,v2) winding from RE1.5. POLY_FT4 (single-prim
quad) deferred to Phase 4.5.6 since the split-triangle pattern is identical
to how SDL_RenderGeometry needs to be called anyway.

### PC textured triangle queue (Phase 4.5.5)

```
re15_render_textured_tri()
   appends 3 SDL_Vertex to s_textri_queue[]
   (position + tint colour + tex_coord in [0..1])

re15_render_end_frame()
   1. UpdateTexture(framebuffer_texture, s_framebuffer)
   2. RenderCopy(framebuffer_texture)              ← 2D layer
   3. SDL_RenderGeometry(s_tim_texture, queue, count*3, NULL, 0)  ← 3D overlay
   4. queue count = 0
   5. RenderPresent
```

The TIM is decoded once at startup via `re15_render_pc_upload_tim()` into
an SDL_Texture with `SDL_PIXELFORMAT_ARGB8888`. SDL_RenderGeometry handles
the per-pixel UV interpolation via host GPU (or its CPU fallback).

### Line primitive (Phase 4.5.1)

```c
void re15_render_line(int x0, int y0, int x1, int y1, int z,
                      uint8_t r, uint8_t g, uint8_t b);
```

PSX: LINE_F2 GPU primitive into OT.
PC:  Bresenham CPU line in software framebuffer.

### PC blit (`asset_pc.c`)

```
re15_asset_read_file(path, &size) → malloc'd buffer
re15_tim_blit_pc(&tim, x, y):
  for each pixel:
    if bpp=8:  idx = src[y*w + x]; color = tim->clut[idx]
    if bpp=4:  idx = nibble of src; color = tim->clut[idx]
    if bpp=16: color = src[y*w + x]
    rgba = rgb555_to_rgba8888(color)
    put_pixel(x+dx, y+dy, rgba)
```

---

## 6. Subsystem: SCD VM (Phase 4.4)

### State

```c
scd_vm_t {
    scd_thread_t threads[10];   // 10 concurrent threads
    u32          tick_count;     // global VM ticks since init

    // Phase 4.4.2: script→render bridge
    u8  cam_change_pending;     // 1 if Cut_chg fired this tick
    u8  cam_id, cam_arg2, cam_arg3;
    u8  message_active;          // 1 if Message_on fired
    u8  message_id, message_arg2, message_arg3;
}

scd_thread_t {
    u8  active, kill_pending;
    i8  call_depth;              // 0..3
    i8  frame_tag[4];            // per-depth blocking flags
    u8  sleep_count[4];          // per-depth sleep slot allocation
    const u8 *pc;                // current bytecode position
    u8  locals[128];             // scratch for opcode args
    u16 sleep_slots[4][16];      // per-depth × 16 countdown counters
    const u8 *call_stack[12];    // Gosub return PCs
}
```

### Tick flow

```
scd_vm_tick()  ← called per 30 Hz tick (= every 2nd frame)
  g_scd.tick_count++
  for thread in threads:
    if !active: continue
    safety_counter = 1024
    while safety_counter--:
      if kill_pending: active=0, break
      if !pc: active=0, break
      op = *pc
      r = opcode_table[op](thread)
      switch r:
        case 0:  pop frame (call_depth--, exit if 0)
        case 1:  continue loop (next opcode)
        case 2:  break loop (yield, next tick)
```

### Opcodes (Phase 4.4.1 + 4.4.2 + 4.4.3 = 21 opcodes)

| Op | Name | Bytes | Handler return | Notes |
|----|------|-------|----------------|-------|
| 0x00 | Nop | 1 | 1 (continue) | advance PC by 1 |
| 0x01 | Evt_end | 1 | 0 (pop frame) | terminates if depth==0 |
| 0x06 | If | 6 | 1 | `[op flag - - skip_lo skip_hi]` — skip if locals[flag]==0 |
| 0x07 | Else | 3 | 1 | unconditional skip after If body |
| 0x08 | End_if | 2 | 1 | marker, advance |
| 0x09 | Sleep | 1 | 1 | reserve sleep slot, fall through to Sleeping |
| 0x0A | Sleeping | 3 | 1 or 2 | decrement counter; wake when 0 (same tick) |
| **0x13** | **Switch** | 4 | 1 | `[op var_idx skip_lo skip_hi]` snapshot locals[var_idx] |
| **0x14** | **Case** | 4 | 1 | `[op value skip_lo skip_hi]` — match → +4, mismatch → +skip |
| **0x15** | **Default** | 1 | 1 | always fall through |
| **0x16** | **EndSwitch** | 1 | 1 | marker, advance |
| 0x17 | Goto | 3 | 1 | `[op offset_lo offset_hi]` signed jump |
| 0x18 | Gosub | 3 | 1 | push PC, jump |
| 0x19 | Return | 1 | 1 | pop call stack |
| **0x29** | **Cut_chg** | 4 | 1 | `[op cam_id arg2 arg3]` → sets g_scd.cam_*_pending |
| **0x2B** | **Message_on** | 4 | 1 | `[op msg_id arg2 arg3]` → sets g_scd.message_* |
| **0x36** | **Se_on** | 12 | 1 | SFX: enqueue audio event {bank, sample, vol, pan, x, y, z} |
| **0x57** | **Sce_bgmtbl_set** | 8 | 1 | enqueue BGMTBL_SET event {table, stage, room, disk, ...} |
| **0x59** | **Xa_on** | 4 | 1 | enqueue XA_ON event {track_id, vol} |
| **0x80** | **Se_vol** | 2 | 1 | enqueue SE_VOL event {scale} |
| 0xFE | DbgText | 4+N | 1 | CUSTOM: `[op x y len text...]` for VM bring-up |

**Bold = Phase 4.4.2/4.4.3 additions.** Unknown opcodes terminate the thread.

### Audio event queue (Phase 4.4.3)

```c
scd_audio_event_t {
    u8  kind;        // SCD_AUDIO_* enum (NONE/SE_ON/BGMTBL_SET/XA_ON/SE_VOL)
    u8  bank;        // SE bank index OR BGM table index
    u8  sample_id;   // sample within bank / XA track id
    u8  volume;
    u8  pan;
    s16 pos_x, pos_y, pos_z;   // 3D positional (Se_on only)
    u16 raw_w0, raw_w1;        // op-specific extra args
};

g_scd.audio_queue[16]    — ring buffer
g_scd.audio_head         — next free slot
g_scd.audio_count        — events waiting
g_scd.audio_total        — lifetime event count (never decreases)

int scd_audio_queue_pop(scd_audio_event_t *out);   // FIFO consumer API
```

Producer: SCD opcode handlers call internal `enqueue_audio()`.
Consumer: Phase 4.6 audio subsystem polls `scd_audio_queue_pop()` per frame.
On overflow: events are dropped silently but `audio_total` still increments,
so monitors can detect queue saturation.

### Main loop integration

```
psx_dev/re15_reborn/main.c (and re15_reborn_pc/main.c, identical):
  scd_vm_init()
  scd_thread_start(0, scd_demo_bytecode())
  for ever:
    re15_render_begin_frame()
    re15_input_tick()
    if (frame_count & 1) == 0:
      scd_vm_tick()              ← 30 Hz tick rate
    ... rendering ...
    re15_render_end_frame()
    frame_count++
```

---

## 7. Subsystem summary table

| Phase | Subsystem | Files | Status |
|---|---|---|---|
| 4.1 | Boot skeleton | main.c, render.c, input.c, system.cnf | ✅ |
| 4.2 | Rendering | render.c, render_pc.c (TILE, text) | ✅ |
| 4.3 | TIM assets | tim_common.c, asset_psx.c, asset_pc.c | ✅ |
| 4.4.1 | SCD VM core | scd_vm.c, scd_demo.c (11 opcodes) | ✅ |
| 4.4.2 | SCD UI ops | scd_vm.c (Cut_chg, Message_on, Switch/Case/Default/EndSwitch — 17 total) | ✅ |
| 4.4.3 | SCD audio ops | scd_vm.c (Se_on, Sce_bgmtbl_set, Xa_on, Se_vol — 21 total) + audio event queue | ✅ |
| 4.4.4+ | SCD remaining | scd_vm.c | pending (~70 opcodes) |
| 4.5.1 | MD1 parser + line primitive + PC wireframe | md1_common.c, render*.c LINE_F2 | ✅ |
| 4.5.2 | PSX 3D via GTE — triangles only | mesh_psx.c (GTE init + RotMatrix/TransMatrix/rtpt pipeline) | ✅ |
| 4.5.3 | Quads wireframe (both targets) | mesh_psx.c quad loop + PC quad loop | ✅ |
| 4.5.4 | Backface culling + per-mesh position | gte_nclip / cross product, expanded API | ✅ |
| 4.5.5 | Textured triangles | PSX POLY_FT3, PC SDL_RenderGeometry queue + ARGB SDL_Texture | ✅ |
| 4.6.0 | Background gradient + multi-mesh demo | re15_render_background_gradient, 3-instance demo | ✅ |
| 4.6.1+ | MDEC-decoded BSS backgrounds | mdec_psx.c (libpress), mdec_pc.c (software) | pending |
| 4.5.6 | POLY_FT4 quads (perf) + Z-sorting | future | pending |
| 4.6 | Audio | audio_*.c | pending |
| 4.7 | Save / state | save_*.c, gsb.c | pending |
| 4.8 | Combat | combat.c, weapon.c | pending |
| 4.9 | Integration | full game | pending |

---

## 8. Tests

See `tests/CMakeLists.txt` and `tests/test_*.c`. Built and run on PC
(host build), no PSX emulator needed. Cover all platform-agnostic code
(tim_common, scd_vm, ...).

Run after every subsystem change:

```powershell
cd psx_dev\re15_reborn\tests\build
cmake --build . --config Release
ctest -C Release --output-on-failure
```
