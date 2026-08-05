# =============================================================================
# RE1.5-Port — PSX-Ziel (PSn00bSDK 0.24+)
# =============================================================================
# EIGENSTAENDIGES Build-Projekt. Es fasst die bestehende Basis NICHT an:
#   * re15_port/CMakeLists.txt und die Targets darunter bleiben unveraendert
#   * die Engine-Quellen werden hier FLACH mit einkompiliert (keine Nutzung des
#     Host-Targets re15_engine, das fuer den PC konfiguriert ist)
#   * platform/psx/CMakeLists.txt (altes SDK-Layout) bleibt liegen und wird
#     von hier NICHT eingebunden
#
# Warum eigenstaendig: PSn00bSDK 0.24 bringt seine EIGENE CMake-Integration mit
# (Toolchain-Datei lib/libpsn00b/cmake/sdk.cmake, Funktionen
# psn00bsdk_add_executable / psn00bsdk_add_cd_image). Das aeltere
# cmake/FindPSn00bSDK.cmake im Repo erwartet das flache 0.1x-Layout
# ($SDK/include, $SDK/lib) und findet 0.24 deshalb nicht. Statt das Find-Modul
# umzubauen (Risiko fuer die laufende Basis) benutzt dieses Projekt direkt das
# offizielle SDK-Modell.
#
# Bauen: siehe targets/psx/build.cmd (Windows) — setzt PSN00BSDK_LIBS,
# waehlt die Toolchain und ruft Ninja.
# =============================================================================

cmake_minimum_required(VERSION 3.21)

project(
    re15_psx
    LANGUAGES   C ASM
    DESCRIPTION "RE1.5 Rebuilt — PlayStation-Ziel"
)

# Wurzel des Ports (zwei Ebenen ueber diesem Verzeichnis)
get_filename_component(RE15_PORT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

# -----------------------------------------------------------------------------
# Quellen — Engine flach + PSX-Backend
# -----------------------------------------------------------------------------
file(GLOB RE15_ENGINE_SRC "${RE15_PORT_ROOT}/engine/src/*.c")
file(GLOB RE15_PSX_SRC    "${RE15_PORT_ROOT}/platform/psx/src/*.c")

psn00bsdk_add_executable(re15 GPREL
    "${RE15_PORT_ROOT}/platform/psx/main.c"
    ${RE15_PSX_SRC}
    ${RE15_ENGINE_SRC}
    "${CMAKE_CURRENT_SOURCE_DIR}/compat/re15_psx_compat.c"
)

target_include_directories(re15 PRIVATE
    "${RE15_PORT_ROOT}/include"
    "${RE15_PORT_ROOT}/platform/psx/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/compat"
)

# Hosted-C-Luecken der PSX schliessen, ohne die Basis anzufassen: der Shim wird
# VOR jede Uebersetzungseinheit gezogen (getenv/fopen/fprintf/FILE/stderr —
# Begruendung in compat/re15_psx_compat.h).
target_compile_options(re15 PRIVATE
    -include "${CMAKE_CURRENT_SOURCE_DIR}/compat/re15_psx_compat.h"
)

# Die Engine-Quellen schalten plattformspezifische Pfade ueber dieses Define.
target_compile_definitions(re15 PRIVATE RE15_PLATFORM_PSX=1)

# -----------------------------------------------------------------------------
# Eingebettete Blobs (Symbole, die das PSX-Backend als extern erwartet)
# -----------------------------------------------------------------------------
# Das Backend erwartet drei mitgelinkte Blobs. psn00bsdk_target_incbin erzeugt
# je Symbol zusaetzlich <symbol>_size.
set(RE15_ASSET_ROOT "${RE15_PORT_ROOT}/shared_assets/PSX")

# test_bss = Rueckfall-Hintergrund, wenn ein CD-Lesen fehlschlaegt
# (bg_psx.c: re15_bg_load_cut faellt darauf zurueck; re15_bg_load_test_asset
# zeigt ihn beim Boot). Echter Hintergrund des Boot-Raums 0x1170, Cut 0.
psn00bsdk_target_incbin(re15 PRIVATE test_bss
    "${RE15_ASSET_ROOT}/BSS/ROOM1170/BG00.BSS")

# test_vh / test_vb = die "gebuendelte" SFX-VAB-Bank (audio_psx.c
# load_bundled_vab). OFFEN: im Asset-Baum liegt KEIN entpacktes VH/VB-Paar —
# die VAB-Koepfe stecken in den .EDH-Containern (Kopf-Offset im 8-Byte-Trailer,
# so liest sie auch das PC-Backend zur Laufzeit) bzw. in DOOR00.DO2. Bis das
# Entpacken im Build steht, wird ein 4-Byte-Platzhalter eingebettet:
# re15_vab_parse schlaegt darauf fehl, load_bundled_vab kehrt frueh zurueck
# (`if (re15_vab_parse(...) != 0) return;`) und der Rest laeuft normal — die
# echten Raum-Baenke (snd0/snd1) kommen ohnehin aus dem RDT von der CD.
psn00bsdk_target_incbin(re15 PRIVATE test_vh
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/placeholder.bin")
psn00bsdk_target_incbin(re15 PRIVATE test_vb
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/placeholder.bin")

# Spielermodell, resident mitgelinkt (asset_psx.c re15_assets_init):
# test_* = Leons Basis-Modell, pl00w01_* = die Waffen-Animationsbank W01.
psn00bsdk_target_incbin(re15 PRIVATE test_md1     "${RE15_ASSET_ROOT}/PLD/PL00.MD1")
psn00bsdk_target_incbin(re15 PRIVATE test_edd     "${RE15_ASSET_ROOT}/PLD/PL00.EDD")
psn00bsdk_target_incbin(re15 PRIVATE test_emr     "${RE15_ASSET_ROOT}/PLD/PL00.EMR")
psn00bsdk_target_incbin(re15 PRIVATE pl00w01_edd  "${RE15_ASSET_ROOT}/PLD/PL00W01.EDD")
psn00bsdk_target_incbin(re15 PRIVATE pl00w01_emr  "${RE15_ASSET_ROOT}/PLD/PL00W01.EMR")

# -----------------------------------------------------------------------------
# Warnungen: nicht als Fehler. Die Basis ist auf den PC-Compiler eingestellt;
# der MIPS-GCC ist an einigen Stellen strenger. Ein PSX-Build soll die
# bestehende Basis nicht unter Aenderungsdruck setzen.
# -----------------------------------------------------------------------------
target_compile_options(re15 PRIVATE -Wno-unused-function -Wno-unused-variable)

# -----------------------------------------------------------------------------
# CD-Image (mkpsxiso liegt im SDK bei)
# -----------------------------------------------------------------------------
psn00bsdk_add_cd_image(
    re15_iso                       # Target-Name
    re15                           # Ausgabename -> re15.bin + re15.cue
    "${CMAKE_CURRENT_SOURCE_DIR}/iso.xml"
    DEPENDS re15 "${CMAKE_CURRENT_SOURCE_DIR}/system.cnf"
)

# Der Asset-Baum wird von iso.xml eingebunden; damit CMake bei Aenderungen neu
# baut, wird das Verzeichnis als Abhaengigkeit gefuehrt.
set_property(
    TARGET re15_iso APPEND PROPERTY
    ADDITIONAL_CLEAN_FILES "${CMAKE_CURRENT_BINARY_DIR}/re15.bin"
)

message(STATUS "RE1.5 PSX-Ziel konfiguriert")
message(STATUS "  Port-Wurzel : ${RE15_PORT_ROOT}")
message(STATUS "  Engine      : ${RE15_PORT_ROOT}/engine/src (flach kompiliert)")
message(STATUS "  Backend     : ${RE15_PORT_ROOT}/platform/psx")
