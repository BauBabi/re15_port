# Build-Workflow Skill

Dieses Skill unterstützt beim Bauen des RE 1.5 Ports für PC und PSX und bei der Diagnose von Build-Fehlern.

## Kontext

Das Projekt nutzt CMake mit zwei Build-Targets:
- **PC** (SDL2/OpenGL): Standard-Entwicklungstarget, benötigt keine Spezialhardware
- **PSX** (PSn00bSDK): Cross-Compilation für PlayStation 1

Projektverzeichnis: `re15_port/`
CMake-Root: `re15_port/CMakeLists.txt`

## Build-Befehle

> Hinweis: `RE15_ASSETS_PATH` ist optional — der CMake-Root defaultet auf das in-repo Asset-Verzeichnis `re15_port/shared_assets/PSX` (dort liegen ALLE Spiel-Assets). Nur angeben, um den Pfad zu überschreiben.

### PC-Target (Standard-Entwicklung)

```bash
# Konfigurieren
cmake -B build -DRE15_BUILD_PC=ON -DRE15_ASSETS_PATH=<pfad_zum_extrahierten_cd_image>

# Bauen
cmake --build build

# Tests ausführen
cd build && ctest --output-on-failure
```

### PSX-Target (Cross-Compilation)

```bash
# Konfigurieren (PSn00bSDK muss installiert sein)
cmake -B build_psx \
  -DRE15_BUILD_PSX=ON \
  -DCMAKE_TOOLCHAIN_FILE=cmake/psx_toolchain.cmake \
  -DPSN00BSDK_PATH=<pfad_zu_psn00bsdk> \
  -DRE15_ASSETS_PATH=<pfad_zum_extrahierten_cd_image>

# Bauen
cmake --build build_psx
```

### Beide Targets parallel

```bash
# PC konfigurieren + bauen
cmake -B build -DRE15_BUILD_PC=ON -DRE15_ASSETS_PATH=<assets_pfad> && cmake --build build

# PSX konfigurieren + bauen (separates Build-Verzeichnis)
cmake -B build_psx -DRE15_BUILD_PSX=ON -DCMAKE_TOOLCHAIN_FILE=cmake/psx_toolchain.cmake -DPSN00BSDK_PATH=<sdk_pfad> -DRE15_ASSETS_PATH=<assets_pfad> && cmake --build build_psx
```

### Nur Tests (ohne vollständigen Rebuild)

```bash
cd build && ctest --output-on-failure --timeout 30
```

### Clean Rebuild

```bash
# PC
rm -rf build && cmake -B build -DRE15_BUILD_PC=ON -DRE15_ASSETS_PATH=<assets_pfad> && cmake --build build

# PSX
rm -rf build_psx && cmake -B build_psx -DRE15_BUILD_PSX=ON -DCMAKE_TOOLCHAIN_FILE=cmake/psx_toolchain.cmake -DPSN00BSDK_PATH=<sdk_pfad> -DRE15_ASSETS_PATH=<assets_pfad> && cmake --build build_psx
```

## Build-Fehler-Diagnose

Wenn ein Build fehlschlägt, analysiere den Fehler-Output wie folgt:

### Schritt 1: Fehlertyp identifizieren

| Fehlertyp | Erkennungsmuster | Typische Ursache |
|-----------|-----------------|------------------|
| Compiler-Fehler | `error:` mit Datei:Zeile | Syntaxfehler, fehlende Includes, Typfehler |
| Linker-Fehler | `undefined reference to` | Fehlende Implementierung, nicht gelinkte Library |
| CMake-Fehler | `CMake Error` | Fehlende Abhängigkeit, ungültiger Pfad |
| Header nicht gefunden | `fatal error: ... No such file` | Fehlender Include-Pfad, Header nicht erstellt |

### Schritt 2: Datei und Zeile extrahieren

Compiler-Fehler folgen dem Muster:
```
<datei>:<zeile>:<spalte>: error: <beschreibung>
```

Extrahiere:
- **Datei**: Relativer Pfad zur Quelldatei
- **Zeile**: Zeilennummer im Quellcode
- **Fehler**: Die Fehlerbeschreibung nach `error:`

### Schritt 3: Lösungsvorschlag generieren

#### Häufige Fehler und Lösungen

**1. `RE15_ASSETS_PATH nicht gesetzt oder ungültig`**
- Ursache: CMake-Parameter fehlt oder Pfad existiert nicht
- Lösung: `cmake -DRE15_ASSETS_PATH=/pfad/zum/extrahierten/cd ..` setzen
- Prüfe: Existiert der Pfad? Enthält er STAGE1/, STAGE2/, etc.?

**2. `PSn00bSDK nicht gefunden`**
- Ursache: PSN00BSDK_PATH nicht gesetzt bei PSX-Build
- Lösung: `cmake -DPSN00BSDK_PATH=/pfad/zu/PSn00bSDK ..`
- Alternativ: Umgebungsvariable `PSN00BSDK_PATH` setzen
- Installationsanleitung: https://github.com/Lameguy64/PSn00bSDK

**3. `undefined reference to 're15_...'`**
- Ursache: Neue .c-Datei in `engine/src/` wurde nach letztem CMake-Configure hinzugefügt
- Lösung: CMake neu konfigurieren (GLOB wird beim Configure aktualisiert):
  ```bash
  cmake --build build  # Löst automatisch Reconfigure aus bei CMakeLists-Änderung
  # Oder manuell:
  cmake -B build -DRE15_BUILD_PC=ON -DRE15_ASSETS_PATH=<pfad>
  ```

**4. `fatal error: re15_*.h: No such file or directory`**
- Ursache: Include-Verzeichnis nicht korrekt gesetzt
- Lösung: Prüfe ob der Header in `include/` existiert. Engine-Library setzt `include/` als PUBLIC include directory.

**5. `fatal error: SDL2/SDL.h: No such file or directory`**
- Ursache: SDL2 FetchContent fehlgeschlagen (Netzwerk?)
- Lösung: Internetverbindung prüfen. SDL2 wird via FetchContent automatisch heruntergeladen.
- Alternativ: SDL2 manuell installieren und CMake-Find-Modul nutzen.

**6. `error: implicit declaration of function`**
- Ursache: Fehlender `#include` für die verwendete Funktion
- Lösung: Passenden Header aus `include/` einbinden:
  - Rendering: `#include "re15_render.h"`
  - Audio: `#include "re15_audio.h"`
  - I/O: `#include "re15_io.h"`
  - Input: `#include "re15_input.h"`
  - SCD-VM: `#include "re15_scd.h"`
  - RDT: `#include "re15_rdt.h"`
  - Spieler: `#include "re15_player.h"`
  - Typen/Fixkomma: `#include "re15_types.h"`

**7. `multiple definition of`**
- Ursache: Globale Variable/Funktion in Header ohne `extern` oder in mehreren .c-Dateien definiert
- Lösung: `extern` Deklaration im Header, Definition nur in einer .c-Datei

**8. PSX-spezifisch: `mipsel-none-elf-gcc: error`**
- Ursache: PSn00bSDK Toolchain nicht korrekt konfiguriert
- Lösung: Prüfe PSN00BSDK_PATH und dass `bin/mipsel-none-elf-gcc` existiert
- Prüfe: `cmake/psx_toolchain.cmake` verweist auf korrekte SDK-Pfade

**9. Test-Fehler: `ctest` meldet FAILED**
- Diagnose: `cd build && ctest --output-on-failure` für Details
- Bei Timeout (>30s): Test hängt, prüfe auf Endlosschleifen oder blockierende I/O
- Bei Assertion-Fehler: Prüfe erwartete vs. tatsächliche Werte im Output

**10. `GLOB` erfasst neue Datei nicht**
- Ursache: CMake-GLOB wird nur beim Configure evaluiert
- Lösung: `cmake -B build -DRE15_BUILD_PC=ON -DRE15_ASSETS_PATH=<pfad>` erneut ausführen

## Projektstruktur-Referenz

```
re15_port/
├── CMakeLists.txt              # Root (definiert PSX + PC Targets)
├── cmake/
│   ├── psx_toolchain.cmake     # PSn00bSDK Toolchain
│   └── FindPSn00bSDK.cmake     # SDK-Detektion
├── engine/
│   ├── CMakeLists.txt          # GLOB für engine/src/*.c
│   └── src/                    # Gemeinsamer Engine-Code
├── include/                    # Gemeinsame Header (re15_*.h)
├── platform/
│   ├── psx/                    # PSX-spezifischer Code
│   └── pc/                     # PC-spezifischer Code (SDL2/OpenGL)
├── tools/                      # Asset-Pipeline-Tools
└── tests/                      # Unit/Property/Integration-Tests
```

## Voraussetzungen

### PC-Build
- CMake >= 3.21
- C-Compiler (MSVC, GCC, oder Clang)
- Internetverbindung für SDL2 FetchContent (beim ersten Configure)
- Extrahiertes CD-Image unter RE15_ASSETS_PATH

### PSX-Build
- Alles vom PC-Build plus:
- PSn00bSDK installiert (gcc-mipsel Cross-Compiler)
- PSN00BSDK_PATH gesetzt auf SDK-Root

### Tests
- Nur PC-Build erforderlich (kein GPU/PSX-Devkit nötig)
- CTest (kommt mit CMake)
- Timeout: 30 Sekunden pro Test
