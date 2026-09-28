# Runde 30 (2026-09-27), Thema D "titel-blinken": das Titelmenue blinkt zu schnell.
# Dossier: analysis/befunde_runde30/titel-blinken.md
# Reine Messsonde (kein add_test): fuehrt den ORIGINAL-Puls-Handler FUN_801028ec aus den
# ausgelieferten Bytes von BIN/TITLE.BIN aus und prueft jedes im Dossier zitierte
# Befehlswort in TITLE.BIN und PSX.EXE. Sie haengt NICHT an der Engine.
add_executable(probe_r30_titel_blinken_puls "probe_r30_titel-blinken_puls.c")
target_compile_definitions(probe_r30_titel_blinken_puls PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_R30_PSX_EXE="${CMAKE_SOURCE_DIR}/../info/Re1.5/PSX.EXE")

# --- RIEGEL 1 (Bau Runde 30): die RECHENBAUSTEINE in engine/src/title_pulse.c.
#     Teil A fuehrt FUN_801028ec aus den Bytes von BIN/TITLE.BIN aus und stellt jedem der
#     180 Aufrufe re15_title_pulse_step() gegenueber (Soll 180 von 180 Wertepaaren gleich).
#     Teil C: T_TICK = 2 VBlanks (DAT_800b5456 := 2 @0x8002130c-14, VSync @0x8002147c-80)
#     bei 59,826 Hz (psx-spx, NTSC non-interlaced) -> 33431 us = 1 Durchgang,
#     2005817 us = 60 Durchgaenge = eine Pulsperiode (0x3c @0x80102918).
#     Teil D: dieselbe Laufzeit, abgetastet mit 20/30/60/144/1000 Hz, ergibt dieselbe
#     Schrittzahl. Teil E: Titel-Einblende gegen die AUSGEFUEHRTEN Bytes — Titel-Init
#     TITLE.BIN 0x80102054-7c (Schritt `ori a1,zero,0xfc00` @0x80102058) mit FUN_800217b0 /
#     FUN_800216ec aus PSX.EXE (Pegel 0x7fff @0x80021710-20), dann je Durchgang FUN_80021880
#     (Farbe = Pegel >> 7 @0x800218c8-d0 vor der Integration @0x80021928).
#     ⛔ REICHWEITE: bindet nur re15_engine, NICHT platform/pc/main.c / render_pc.c. Nimmt man
#     die Verdrahtung in main.c zurueck, bleibt dieser Riegel GRUEN — er beweist nicht, dass
#     das Spiel richtig blinkt. (Eine fruehere Fassung behauptete in einem Teil F "der Riegel
#     faellt am alten Stand"; Teil F zaehlte aber nur eine for-Schleife. Gestrichen.) ---
add_executable(test_r30_titel_blinken test_r30_titel_blinken.c)
target_link_libraries(test_r30_titel_blinken PRIVATE re15_engine re15_test_support)
target_include_directories(test_r30_titel_blinken PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r30_titel_blinken PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_R30_PSX_EXE="${CMAKE_SOURCE_DIR}/../info/Re1.5/PSX.EXE")
add_test(NAME r30_titel_blinken COMMAND test_r30_titel_blinken)
set_tests_properties(r30_titel_blinken PROPERTIES TIMEOUT 60)

# --- RIEGEL 2 (Nachbesserung Runde 30): das SYMPTOM an der ECHTEN re15_pc.exe.
#     Misst ueber die Messschiene RE15_TITLE_PULSE_LOG (platform/pc/main.c) die Pulsperiode im
#     Titel (Soll 60 x 2 VBlanks / 59,826 Hz = 2005817 us, Toleranz = gemessene Bilddauer), die
#     Pulsaenderungen im Bestaetigungs-Fade (Soll 0: FUN_80102a10 ruft FUN_801028ec nicht) und
#     die Dauer der Titel-Einblende (32 Durchgaenge, Schritt 0xfc00 @0x80102058). Bestaetigt wird
#     ueber den Zeit-Testhaken RE15_TITLE_CONFIRM_MS, beendet ueber RE15_PSELECT_AUTO +
#     RE15_BOOT_EXIT_AT=1. Gegenprobe am Ausgangsstand master d98e9639 (mit nachgeruesteter
#     Messschiene): ROT — Dossier titel-blinken.md, Abschnitt UMSETZUNG Nachbesserung. ---
if(TARGET re15_pc)
    add_test(NAME integration_r30_titel_puls
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r30_titel_puls_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r30_titel_puls.cmake)
    set_tests_properties(integration_r30_titel_puls PROPERTIES TIMEOUT 240)
endif()
