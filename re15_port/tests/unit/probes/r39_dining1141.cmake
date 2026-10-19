# Runde 39 (2026-10-18), Spur J "dining1141" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde39/J_dining1141.md, Konstanten include/re15_dining_r39.h, Code engine/src/dining_r39.c,
# Programme engine/src/gen/dining_r39_programme.inc (tools/r39_dining/programme_bauen.py), PC platform/pc/src/dining_r39_pc.c.
# Haken: dining_r38.c (Phase-H-Programme, Se_on 0x3E, Installer-Ende, Messhaken), dining_r38_pc.c (Init), audio_pc.c
# (help-Stamm, Musikwechsel 1141 -> 1171), tests/unit/test_r36_kartenbits.c k_port[] (Bit 94).
#
#   unit_r39_dining1141_texte      Port-Texte 50/51/52 = Wortlaut des Nutzers (Punkte 5, 8)
#   unit_r39_dining1141_programme  Programm-Bytes je Nutzer-Punkt 1-8 (Formen/Reihenfolgen, Kopf-Konstanten)
#   unit_r39_dining1141_stamm      help01..05 = Sprach-Ids 53..57 nur in ROOM1141
#   unit_r39_dining1141_phase_h    ROOM1141 am VM mit allen Installern: John auf dem Dach, Klopfen/Hilferufe + Reaktion,
#                                  Tuer-Auftritt mit Kameraschnitten, Glasbruch, Blick ab msg 9, Zeilen 51/52, Abgang
#   unit_r39_dining1141_fenster    Nachbesserung 1: Gemaelde im Fenster erster Ruf..Reaktion -> Reaktion erst nach der Rolladen-
#                                  Szene, genau einmal (M1); Wiederbetreten mit offenem Rolladen + Entriegeln (N4.1/N4.2)
#   unit_r39_dining1141_senken     Nachbesserung 3 (Abnahme 2 M1): wer unter der Pad-Sperre (2,7) zielt, senkt die Waffe
#                                  (FUN_80030444 kappt das Held-Wort, Ziel-FSM HOLD/Messer/Rueckstoss -> SENKEN)
#   integration_r39_dining1141_d   echte exe, echter Weg mit Elza 1131 -> 1141 (Gemaelde, frei laufen, Riegel, Szene, John
#                                  schiesst) -> 1171 (Ton-Mitschnitt, Messschiene
#                                  RE15_R39J_LOG, debug.log): alle Punkte 1-9 am Lauf, Punkt 9 am Mitschnitt
#   integration_r39_dining1141_zielen Nachbesserung 3 (Abnahme 2 M1): echter Weg, Browning, Elza haelt R1 ueber Kampfende und
#                                  Schnitt: Sperre >= 29 Bilder vor Cut 8, gesenkt, kein Bild in Cut 8 mit erhobener Waffe
#   integration_r39_dining1141_kampf  Nachbesserung 2 (Abnahme 1 M1): echter Weg, Pistole, Gorillas LEBEND, Elza neben John:
#                                  Treffer auch bei Elza im Griff (a), kein Schuss ausser Kegel (b), Zuwendung je Treffer-Salve (c),
#                                  John beim Schiessen in Cut 0, Querungs-Wache
add_executable(test_r39_dining1141 ${CMAKE_CURRENT_LIST_DIR}/../test_r39_dining1141.c)
target_link_libraries(test_r39_dining1141 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_dining1141 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_dining1141 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_dining1141 PRIVATE m)
endif()
foreach(_teil texte programme stamm phase_h fenster senken)
    add_test(NAME unit_r39_dining1141_${_teil} COMMAND test_r39_dining1141 ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r39_dining1141_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_dining1141.cmake)
    add_test(NAME integration_r39_dining1141_d
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_R39J_WERKZEUG=$<TARGET_FILE:test_r39_dining1141>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r39_dining1141_d_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_dining1141.cmake)
    set_tests_properties(integration_r39_dining1141_d PROPERTIES TIMEOUT 3600)
endif()
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_dining1141_kampf.cmake)
    add_test(NAME integration_r39_dining1141_kampf
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r39_dining1141_kampf_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_dining1141_kampf.cmake)
    set_tests_properties(integration_r39_dining1141_kampf PROPERTIES TIMEOUT 3600)
endif()
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_dining1141_zielen.cmake)
    # Nachbesserung 3 (Abnahme 2 M1): Elza zielt beim Kampfende -> Sperre vor dem Schnitt, gesenkt, Cut 8 ohne erhobene Waffe
    add_test(NAME integration_r39_dining1141_zielen
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_R39J_WERKZEUG=$<TARGET_FILE:test_r39_dining1141>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r39_dining1141_zielen_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_dining1141_zielen.cmake)
    set_tests_properties(integration_r39_dining1141_zielen PROPERTIES TIMEOUT 3600)
endif()
