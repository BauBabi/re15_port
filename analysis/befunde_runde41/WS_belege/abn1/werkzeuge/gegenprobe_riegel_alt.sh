#!/usr/bin/env bash
# Umgekehrte Gegenprobe (Abnahme WS 1): Riegel-Dateien aus 9e0066c26 gegen die Engine des Kopfes linken und laufen lassen.
G="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/abn1/gegenprobe/riegel_alt"
W=C:/workspace/git/reAi_v2/.claude/worktrees/r41_waffen/re15_port; BLD=$W/build
cd "C:/workspace/git/reAi_v2/.claude/worktrees/r41_waffen"
for t in probe_r34_schaden probe_r34_wurf test_r35_granate probe_r34_reaktion test_birkin_bahnsteig; do git show 9e0066c26:re15_port/tests/unit/$t.c > "$G/${t}_alt.c"; done
export PATH=/c/msys64/mingw64/bin:/usr/bin
for t in probe_r34_schaden probe_r34_wurf test_r35_granate probe_r34_reaktion test_birkin_bahnsteig; do
  gcc -DRE15_ASSETS_PATH=$W/shared_assets/PSX -DRE15_ASSET_PSX_DIR=\"$W/shared_assets/PSX\" -DRE15_PLATFORM_PC=1 -I$W/include -I$W/platform/pc/src -I$W/tests -std=c11 -o "$G/${t}_alt.obj" -c "$G/${t}_alt.c"
  X=""; [ $t = test_r35_granate ] && X="$BLD/tests/unit/CMakeFiles/test_r35_granate.dir/__/__/platform/pc/src/fx_plattform_pc.c.obj"
  gcc "$G/${t}_alt.obj" $X -o "$G/${t}_alt.exe" $BLD/engine/libre15_engine.a $BLD/tests/libre15_test_support.a -lm -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32
done
# Aufrufe: probe_r34_schaden alle | probe_r34_wurf | test_r35_granate alle | probe_r34_reaktion alle | test_birkin_bahnsteig
