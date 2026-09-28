# Runde 30 (2026-09-27), Thema E2 "Irons Diary: Welt-Prop auf dem Schreibtisch + Memory Card".
# REINE MESS-Sonde (kein add_test): Kameratabelle/Sichtmatrix ROOM1150 mit den Engine-
# Funktionen, Projektion/Rueckprojektion der beiden Nutzer-Marken, Prop-Pool nach dem
# Hochfahren. Dossier: analysis/befunde_runde30/irons-diary-welt.md
add_executable(probe_r30_irons-diary-welt "probe_r30_irons-diary-welt.c")
target_link_libraries(probe_r30_irons-diary-welt PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_irons-diary-welt PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_irons-diary-welt PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

# =============================================================================
# BAU (Runde 30, Thema irons-diary-welt) — die Riegel zum Umbau (immer an, zaehlen zur Suite).
#
#   unit_r30_irons_tisch        Props obj 5/6 + Zonen Slot 7/8 in ROOM1150 UND 1151 nach dem
#                               echten Raumstart; genommen -> weg; Aufheben per Aktionstaste
#                               (Karte: Modal 0x21, 3 Stueck; Diary: Leser, Meldung, Abraeumen);
#                               Marken des Nutzerbilds in Cut 2; Tiefen-Klemme gegen die
#                               Tischmasken (Tiefe 87, beide RDTs); eingebackene Modelle
#   integration_r30_irons_tisch_laden   echte exe, CONTINUE in ROOM1150/1151: Buch und Karte
#                               liegen am Lade-Weg im Pool und werden gezeichnet; genommen weg
#   integration_r30_irons_tisch_bild    echte exe, vier Framedumps Cut 2 (mit/ohne Props, Spieler
#                               weit weg / vor dem Tisch): beide Props SICHTBAR an den Marken
#                               des Nutzerbilds, die Figur vor dem Tisch liegt darueber
#                               (Nachbesserung: ohne Tiefen-Klemme blieb die Suite gruen)
# Dossier: analysis/befunde_runde30/irons-diary-welt.md (S6, UMSETZUNG)
# =============================================================================
add_executable(test_r30_irons_tisch ${CMAKE_CURRENT_LIST_DIR}/../test_r30_irons_tisch.c)
target_link_libraries(test_r30_irons_tisch PRIVATE re15_engine re15_test_support)
target_include_directories(test_r30_irons_tisch PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r30_irons_tisch PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r30_irons_tisch COMMAND test_r30_irons_tisch
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r30_irons_tisch PROPERTIES TIMEOUT 120)

# MESS-WERKZEUG (kein add_test): schreibt die Speicherkarte fuer den Lade-Pin.
add_executable(probe_r30_irons_tisch_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r30_irons_tisch_karte.c)
target_link_libraries(probe_r30_irons_tisch_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_irons_tisch_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

# AUSWERTER (kein add_test) fuer den Bild-Riegel: vergleicht vier Framedumps.
add_executable(probe_r30_irons_tisch_bild ${CMAKE_CURRENT_LIST_DIR}/../probe_r30_irons_tisch_bild.c)

if(TARGET re15_pc)
    add_test(NAME integration_r30_irons_tisch_laden
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r30_irons_tisch_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r30_irons_tisch_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r30_irons_tisch_laden.cmake)
    set_tests_properties(integration_r30_irons_tisch_laden PROPERTIES TIMEOUT 300)

    # Nachbesserung (Gegenpruefer, Mutationsprobe): das SICHTBARE Ergebnis der Tiefen-Klemme.
    add_test(NAME integration_r30_irons_tisch_bild
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r30_irons_tisch_karte>
                     -DRE15_BILD_TOOL=$<TARGET_FILE:probe_r30_irons_tisch_bild>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r30_irons_tisch_bild_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r30_irons_tisch_bild.cmake)
    set_tests_properties(integration_r30_irons_tisch_bild PROPERTIES TIMEOUT 400)
endif()
