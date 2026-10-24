# Runde 41 (2026-10-23), Spur A "wasser20b0" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde41/A_wasser20b0.md, Konstanten include/re15_wasser20b0_r41.h,
# Code engine/src/wasser20b0_r41.c. Haken: enemy_ai_common.c (EM024-Zweig: Init-Zeichenmaske, +0x5-Dispatch
# vor der Drift, kein Bildvorschub), aot_common.c (AUTO-Ereignis 20B0/20B1 unter player_mode 2).
#
#   unit_r41_wasser20b0_bytes      Spawn-Tabellen/Takte/Dispatch-Tabellen gegen die rohen Bytes von STAGE2.BIN
#   unit_r41_wasser20b0_modell     EM024-Teil-0-Matrix = Original-RAM (T = Lage + (0,-3329,0), Einheitsrotation)
#   unit_r41_wasser20b0_sequencer  grid 0/1/5 am Sequencer mit der Raum-Bank 20B0 (Takte, Phasen, Codes, a1, Anker)
#   unit_r41_wasser20b0_raum       ROOM20B0/20B1 am VM: 35 EM024 unsichtbar, Drift byte-gleich zu ts_10/Umbruch/ts_14,
#                                  Partikel 0x0f im Pool, Spawn-Zaehlung je grid
#   unit_r41_wasser20b0_landung    Punkt 2a: Landungs-Spritzer @0x80038730-d0 (8 x 0x06033000, Gier-Faecher, Gate Stempel < y)
#   unit_r41_wasser20b0_npc        Punkt 2b: NPC-Schritt FUN_80045630 Typ >= 0x40 (Boden-/Stempel-Zweig, Ton -8, Bildwort-Takt);
#                                  Marken je Typ/Archiv, PORT-WAHL nur fuer Fortbewegungs-Clips ohne Marke (0x4b, 0x47 CDEMD1 c5)
#   unit_r41_wasser20b0_npc_wasser Nachbesserung 1: der echte Geh-Sub (state 4 / sub 4) fuehrt jeden NPC-Typ (beide Archive)
#                                  durch den Wasserfleck ROOM2000 -> Spritzer 0x06050D00 + Ton (OWN-Bank-Marken, 0x4b PORT-WAHL)
#   unit_r41_wasser20b0_stempel    Nachbesserung 1 (M3): WATER-Stempel +0x88 fuer NPC und Spieler auch unter player_mode 2
#   unit_r41_wasser20b0_npc_subs   Nachbesserung 1 (Fortsetzung): Schritt auch in Sub 9 / Kante 0x12 / Schub 0x13, Sub 4/5
#                                  Phase 2 VOR Kurs/Vorschub, Bildwort + Part des GEZEICHNETEN Bilds (kein Doppelschritt am Laufbeginn)
#   integration_r41_wasser20b0_npc Nachbesserung 1 (M5): echte exe: NPC-Schritte in der 2000-Szene (Marvin/Ada), Landung ueber
#                                  die Tuer 20A0->2090 (Leon) und 20A1->2091 (Elza)
#   integration_r41_wasser20b0     echte exe: Debug-Sprung 20B0 (Leon) + 20B1 (Elza): Partikel 0x0f gezeichnet
#                                  (RE15_FX_LOG), sub05 feuert (Cut 7, Tuerblatt fliegt, (3,0xEB)), Spieler wieder frei
add_executable(test_r41_wasser20b0 ${CMAKE_CURRENT_LIST_DIR}/../test_r41_wasser20b0.c)
target_link_libraries(test_r41_wasser20b0 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_wasser20b0 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_wasser20b0 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_wasser20b0 PRIVATE m)
endif()
foreach(_teil bytes modell sequencer raum landung npc npc_wasser stempel npc_subs)
    add_test(NAME unit_r41_wasser20b0_${_teil} COMMAND test_r41_wasser20b0 ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_wasser20b0_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_wasser20b0_npc.cmake)
    add_test(NAME integration_r41_wasser20b0_npc
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_wasser20b0_npc_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_wasser20b0_npc.cmake)
    set_tests_properties(integration_r41_wasser20b0_npc PROPERTIES TIMEOUT 1800)
endif()
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_wasser20b0.cmake)
    add_test(NAME integration_r41_wasser20b0
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_wasser20b0_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_wasser20b0.cmake)
    set_tests_properties(integration_r41_wasser20b0 PROPERTIES TIMEOUT 1800)
endif()
