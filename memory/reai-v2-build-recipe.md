---
name: reai-v2-build-recipe
description: Verifizierte PC-Build-Recipe und Toolchain für den re15_port C-Port in reAi_v2
metadata: 
  node_type: memory
  type: project
  originSessionId: 073aec74-f763-4c50-aa34-afe1225f60d0
---

`reAi_v2` (c:\workspace\git\reAi_v2) ist der bewusst aufgeräumte Neu-Aufbau des RE1.5-Ports (vorher c:\workspace\git\reAi, war zu groß). Kern ist der CMake-C-Port unter `re15_port/` (PC SDL2/OpenGL + PSX PSn00bSDK), spezifiziert in `.kiro/specs/re15-port-setup/`.

**Verifizierte PC-Build-Recipe (2026-06-27, Exit 0, 26/26 ctest grün):**
- Toolchain auf dieser Maschine: mingw64 GCC 15.2 (`C:\msys64\mingw64\bin`), Ninja, CMake 4.3.2. Kein MSVC/Clang. PATH muss mingw64 enthalten, `CC=gcc`.
- `cmake -S re15_port -B re15_port/build -G Ninja -DRE15_BUILD_PC=ON` (RE15_ASSETS_PATH optional)
- `cmake --build re15_port/build` ; `ctest --test-dir re15_port/build --timeout 30`
- **Alle Assets** konsolidiert in `re15_port/shared_assets/PSX/` (vollständiger CD-Baum, alle 6 Stages; 2026-06-27 aus `info/Re1.5/PSX` kopiert). CMake-Root defaultet RE15_ASSETS_PATH auf dieses Verzeichnis (überschreibbar via -D/Env). Extraktions-Intermediates nach `build/extracted/` verschoben.

**4 Portabilitäts-Blocker, die ich behoben habe (Code war nie für GCC 15/C23 + nie für PC gelinkt):**
1. `include/re15_types.h`: `typedef uint8_t bool;` per `__STDC_VERSION__ < 202311L` gegen C23 geschützt.
2. `platform/pc/main.c`: fehlendes `#include "re15_rdt.h"`.
3. `g_render`/`g_io` waren extern deklariert aber nie definiert → Definition in `render_pc.c` bzw. `io_pc.c` ergänzt.
4. `platform/pc/CMakeLists.txt`: SDL2-Linkreihenfolge auf `SDL2::SDL2main` vor `SDL2::SDL2` gedreht.

Offen/Notizen: tasks.md hakt alles [x] ab, aber der „Build beider Targets"-Checkpoint war faktisch nie erfolgt (nur Engine-Lib). `tools/overlay_mapper` ist nicht ins `tools/CMakeLists.txt` eingehängt. `prop_case_resolve` nicht im CMake. Siehe [[reai-v2-open-gaps]].
