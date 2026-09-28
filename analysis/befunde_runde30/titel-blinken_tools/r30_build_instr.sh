#!/usr/bin/env bash
# r30_build_instr.sh - Runde 30 / Thema D (Titelmenue blinkt zu schnell).
#
# Baut eine INSTRUMENTIERTE KOPIE von re15_pc.exe, OHNE den Quellbaum anzufassen:
#   1. kopiert re15_port/platform/pc/src/render_pc.c nach build/r30_titel-blinken/instr/
#   2. haengt in der KOPIE an das Ende von re15_render_pc_title_menu eine Logzeile
#      (Zeit in Mikrosekunden aus SDL_GetPerformanceCounter, Pulszaehler, Pulswert)
#   3. uebersetzt die Kopie mit DENSELBEN Schaltern wie das Original-Target und linkt sie
#      gegen die unveraenderten uebrigen Objekte aus re15_port/build_r30_titel-blinken
# Ergebnis: re15_port/build_r30_titel-blinken/platform/pc/re15_pc_r30instr.exe
# Logdatei zur Laufzeit: Umgebungsvariable R30_PULSE_LOG=<pfad>.
set -euo pipefail
export PATH="/c/msys64/mingw64/bin:/c/msys64/usr/bin:$PATH"
REPO=/c/workspace/git/reAi_v2
BLD=$REPO/re15_port/build_r30_titel-blinken
OUT=$REPO/build/r30_titel-blinken/instr
mkdir -p "$OUT"
SRC=$REPO/re15_port/platform/pc/src/render_pc.c
CPY=$OUT/render_pc_r30instr.c
python - "$SRC" "$CPY" <<'PY'
import sys
src = open(sys.argv[1], encoding="utf-8", errors="surrogateescape").read()
anchor = ("    if (++s_tmoji_pulse_ctr >= 0x3c) { s_tmoji_pulse_ctr = 0; s_tmoji_pulse_val = 0x80; }\n")
assert src.count(anchor) == 1, "Anker nicht eindeutig: %d" % src.count(anchor)
probe = anchor + (
"    /* R30-MESSKOPIE: nur in build/r30_titel-blinken/instr, nie im Quellbaum. */\n"
"    { static FILE *r30_lf; static int r30_init;\n"
"      if (!r30_init) { r30_init = 1; const char *p = getenv(\"R30_PULSE_LOG\"); if (p && *p) r30_lf = fopen(p, \"w\"); }\n"
"      if (r30_lf) { unsigned long long c = (unsigned long long) SDL_GetPerformanceCounter();\n"
"                    unsigned long long f = (unsigned long long) SDL_GetPerformanceFrequency();\n"
"                    fprintf(r30_lf, \"%llu %d %d\\n\", (c * 1000000ull) / f, s_tmoji_pulse_ctr, s_tmoji_pulse_val);\n"
"                    fflush(r30_lf); } }\n")
open(sys.argv[2], "w", encoding="utf-8", errors="surrogateescape", newline="").write(src.replace(anchor, probe))
print("Kopie geschrieben:", sys.argv[2])
PY
cd "$BLD"
cc.exe -DRE15_ASSETS_PATH=C:/workspace/git/reAi_v2/re15_port/shared_assets/PSX \
  '-DRE15_ASSET_ROOT_DEFAULT="C:/workspace/git/reAi_v2/re15_port/shared_assets/PSX"' \
  '-DRE15_CD_ROOT_DEFAULT="C:/workspace/git/reAi_v2/re15_port/shared_assets/PSX"' \
  -DRE15_PLATFORM_PC -DRE15_PLATFORM_PC=1 \
  -IC:/workspace/git/reAi_v2/re15_port/platform/pc/src -IC:/workspace/git/reAi_v2/re15_port/include \
  -I_deps/sdl2-build/include -I_deps/sdl2-build/include/SDL2 -I_deps/sdl2-build/include-config-/SDL2 \
  -std=c11 -o "$OUT/render_pc_r30instr.obj" -c "$CPY"
D=platform/pc/CMakeFiles/re15_pc.dir
cc.exe -static -Wl,--subsystem,windows -Wl,-e,mainCRTStartup \
  $D/main.c.obj $D/src/asset_pc.c.obj $D/src/asset_root_pc.c.obj $D/src/asset_selftest_pc.c.obj \
  $D/src/audio_pc.c.obj $D/src/bg_pc.c.obj $D/src/input_pc.c.obj $D/src/inv_render_pc.c.obj \
  "$OUT/render_pc_r30instr.obj" $D/src/room_pc.c.obj $D/src/skeleton_trig_pc.c.obj $D/src/touch_overlay_pc.c.obj \
  -o platform/pc/re15_pc_r30instr.exe \
  engine/libre15_engine.a _deps/sdl2-build/libSDL2.a -lm -lkernel32 -luser32 -lgdi32 -lwinmm -limm32 \
  -lole32 -loleaut32 -lversion -luuid -ladvapi32 -lsetupapi -lshell32 -ldinput8 -lwinspool -lcomdlg32
ls -la platform/pc/re15_pc_r30instr.exe
