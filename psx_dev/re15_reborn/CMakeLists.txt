# RE1.5 Rebuilt — Phase 4 (2026-05-18)
#
# Build a new Resident Evil engine from scratch using PSn00bSDK, with
# RE2's architecture as reference, that runs RE1.5's content.
#
# This is a multi-year-style port project. Phase 4.1 = skeleton EXE that
# boots and initializes systems. Subsequent phases add asset loaders,
# rendering, audio, game logic.

cmake_minimum_required(VERSION 3.21)

project(
    re15_reborn
    LANGUAGES    C ASM
    VERSION      0.1.0
    DESCRIPTION  "RE1.5 reborn on RE2-faithful architecture"
)

# Auto-pick up all .c files under src/
file(GLOB _sources
    "src/*.c"
    "main.c"
)

psn00bsdk_add_executable(re15_reborn GPREL ${_sources})

# Include directory for our public headers
target_include_directories(re15_reborn PRIVATE include)

# Phase 4.7: test.tim (Leon) + elliot.tim are CD-streamed (TEST.TIM/ELLIOT.TIM in
# iso.xml) → VRAM → freed, not incbin'd. (The heli obj02 TIM is no longer CD-streamed
# — Phase 2 slices it from the RDT prop table like every other prop.)

# Phase 4.5: embed test MD1 mesh
psn00bsdk_target_incbin(re15_reborn PRIVATE test_md1 assets/test.md1)

# Phase 4.5.6: embed test BSS background chunk (ROOM11000.bs — first MDEC
# frame of stage1 room 110). 64 KB chunk = one full 320×240 frame.
psn00bsdk_target_incbin(re15_reborn PRIVATE test_bss assets/test.bss)

# Phase 4.7 CD-streaming (2026-06-02): the 6 ROOM1170 per-cut BSS frames (cuts
# 0/1/2/3/4/6, 384KB) are NO LONGER incbin'd — they are added to the CD image as
# BG00..BG06.BSS (iso.xml) and streamed on cut-change into a 64KB staging buffer
# (re15_cdfs + bg_psx.c). Net ~320KB RAM reclaimed; this is the prerequisite for
# many-cut rooms (ROOM5011=46, ROOM4010=44). test.bss stays resident (fallback).

# Phase 4.6.2: embed test VAB (DOOR00 — door SFX bank). VH = 3 KB header,
# VB = ~18 KB ADPCM body. Used by audio subsystem to validate VAB parsing
# + SPU upload + sample playback.
psn00bsdk_target_incbin(re15_reborn PRIVATE test_vh assets/test.vh)
psn00bsdk_target_incbin(re15_reborn PRIVATE test_vb assets/test.vb)

# Footstep chain: the room's SE bank snd0 (VH+VB+EDT) is now SLICED from the parsed
# RDT at runtime (re15_rdt_parse → snd_vh/vb/edt[0]) — globalization Phase 2
# (2026-06-13). No longer incbin'd; works for any room's snd0.

# Phase 4.5.7: embed PL00 (Leon) animation + skeleton data. EDD = 3 KB
# animation table; EMR = 57 KB skeleton + ~711 keyframes worth of bone
# Euler angles (12-bit packed). Used by skeletal-mesh rendering.
psn00bsdk_target_incbin(re15_reborn PRIVATE test_edd assets/test.edd)
psn00bsdk_target_incbin(re15_reborn PRIVATE test_emr assets/test.emr)

# Phase 4.7 (multi-room foundation): the room RDT (ROOM1170, ~296KB) is NO LONGER
# incbin'd — it's streamed from CD as ROOM1170.RDT (iso.xml) into a resident room
# buffer via re15_room_load(). A door transition just re-calls re15_room_load(next).

# (room1170_rbj incbin removed 2026-06-14g — the cinematic rbj overlay is CD-streamed
# per-room from \RBJ\ROOM1170.RBJ into the per-room arena, freed on room change. It was
# room1170-only ballast resident in every room. See asset_psx.c re15_assets_init.)

# (room1170_light + room1170_msgs incbin removed 2026-06-14i — both are now RDT-sliced
# per room (g_room_rdt.lights @ RDT+0x2c, .messages @ RDT+0x3c). The incbin were dead.)

# #2 prop loop + heli: every room's Obj_model_set props (MD1+TIM) — INCLUDING obj02,
# the ROOM1170 helicopter body (actor type 0x47) — are now SLICED from the parsed RDT
# at runtime (re15_load_room_props + the heli load in re15_assets_init read
# g_room_rdt.prop_md1/tim[], the 0x30 model-pointer table) — globalization Phase 2
# (2026-06-13). room1170 obj00..05 + room1140 obj00 are no longer incbin'd, and the
# OBJ0N.TIM CD files are off the disc (verified byte-true vs the old incbin/CD bytes).
# The heli TIM goes to a relocated VRAM slot in asset_psx (baked coords collide w/ FB).

# (Elliot's MD1/EDD/EMR incbin removed 2026-06-14g — his mesh+skeleton are CD-streamed
# per-room from \PLD\ELLIOT.MD1/EMR into the per-room arena, freed on room change. EDD is
# not loaded (the rbj overlay replaces his animation). Was room1170-only ballast.)

# PL00W01 (Leon handgun weapon-track) — the real Walk_Forward (clip 5) + Run
# (clip 0) cycle. Used as the WALK animation source for any actor in walk/run
# motion (sentinels 105/100): Leon AND Elliot (rbj/PL05 have no real walk
# cycle). Composite = PL00 bind pose + W01 keyframes (bone hierarchy identical,
# 15 bones). Mirrors the verified PC build (AD-round). EDD 1KB + EMR 17KB.
psn00bsdk_target_incbin(re15_reborn PRIVATE pl00w01_edd assets/pl00w01.edd)
psn00bsdk_target_incbin(re15_reborn PRIVATE pl00w01_emr assets/pl00w01.emr)

# 2026-06-02: the 17 individual room1170_msgXX.msg incbins were REPLACED by the
# single consolidated room1170_msgs.bin (parsed in asset_psx → shared re15_msg).
# Removed as dead weight (they were unreferenced in code).

# Build bootable CD image (BIN/CUE) — PSn00bSDK invokes elf2x + mkpsxiso
psn00bsdk_add_cd_image(
    re15_reborn_iso re15_reborn iso.xml
    DEPENDS re15_reborn system.cnf
)

install(
    FILES
        ${PROJECT_BINARY_DIR}/re15_reborn.exe
        ${PROJECT_BINARY_DIR}/re15_reborn.bin
        ${PROJECT_BINARY_DIR}/re15_reborn.cue
    TYPE BIN
)
