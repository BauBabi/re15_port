# Runde 30 (2026-09-27), Thema D "titel-blinken": das Titelmenue blinkt zu schnell.
# Dossier: analysis/befunde_runde30/titel-blinken.md
# Reine Messsonde (kein add_test): fuehrt den ORIGINAL-Puls-Handler FUN_801028ec aus den
# ausgelieferten Bytes von BIN/TITLE.BIN aus und prueft jedes im Dossier zitierte
# Befehlswort in TITLE.BIN und PSX.EXE. Sie haengt NICHT an der Engine.
add_executable(probe_r30_titel_blinken_puls "probe_r30_titel-blinken_puls.c")
target_compile_definitions(probe_r30_titel_blinken_puls PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_R30_PSX_EXE="${CMAKE_SOURCE_DIR}/../info/Re1.5/PSX.EXE")

# --- RIEGEL (Bau Runde 30): Pulsfolge, Tick-Rechnung, Uhr, Einblende.
#     Teil A fuehrt FUN_801028ec aus den Bytes von BIN/TITLE.BIN aus und stellt jedem der
#     180 Aufrufe re15_title_pulse_step() gegenueber (Soll 180 von 180 Wertepaaren gleich).
#     Teil C: T_TICK = 2 VBlanks (DAT_800b5456 := 2 @0x8002130c-14, VSync @0x8002147c-80)
#     bei 59,826 Hz (psx-spx, NTSC non-interlaced) -> 33431 us = 1 Durchgang,
#     2005817 us = 60 Durchgaenge = eine Pulsperiode (0x3c @0x80102918).
#     Teil D: dieselbe Laufzeit, abgetastet mit 20/30/60/144/1000 Hz, ergibt dieselbe
#     Schrittzahl. Teil E: Titel-Einblende gegen den Integrator FUN_80021880
#     (Schritt 0xfc00 @0x80102058). Teil F ist die Gegenprobe (alter Stand faellt). ---
add_executable(test_r30_titel_blinken test_r30_titel_blinken.c)
target_link_libraries(test_r30_titel_blinken PRIVATE re15_engine re15_test_support)
target_include_directories(test_r30_titel_blinken PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r30_titel_blinken PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME r30_titel_blinken COMMAND test_r30_titel_blinken)
set_tests_properties(r30_titel_blinken PROPERTIES TIMEOUT 60)
