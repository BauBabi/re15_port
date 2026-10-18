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
#   integration_r39_dining1141_d   echte exe, echter Weg mit Elza 1131 -> 1141 -> 1171 (Ton-Mitschnitt, Messschiene
#                                  RE15_R39J_LOG, debug.log): alle Punkte 1-9 am Lauf, Punkt 9 am Mitschnitt
add_executable(test_r39_dining1141 ${CMAKE_CURRENT_LIST_DIR}/../test_r39_dining1141.c)
target_link_libraries(test_r39_dining1141 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_dining1141 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_dining1141 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_dining1141 PRIVATE m)
endif()
foreach(_teil texte programme stamm phase_h)
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
