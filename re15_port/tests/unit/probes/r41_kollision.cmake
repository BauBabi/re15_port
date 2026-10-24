# Runde 41 (2026-10-24), Spur H "kollision" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde41/H_kollision.md, Code engine/src/kollision_r41.c + include/re15_kollision_r41.h.
# Haken (je 1 Zeile, "Runde 41 Spur H"): enemy_ai_common.c re15_npc_ai_tick (vor re15_npc_wall_clamp),
# aot_common.c re15_prop_push_tick (Objekt-gegen-Objekt-Pass), game_step_common.c (Spiegel obj+0x42 nach dem Objekt-Pass).
#
#   unit_r41_kollision_npc       Mechanik FUN_8002aec4(&Spieler,NPC) @0x8011cc3c + FUN_8002b544 @0x8011cc44: exakte
#                                Schub-Formel dX*pen/(dist+1), nur der Geschobene bewegt sich, Spieler-Schub, Gates
#                                (toter Spieler, Leiche, Y-Band), Kaesten 0x45/0x4b
#   unit_r41_kollision_raum3070  echte ROOM3070: Ada + Marvin als Eskorte (sub00 @0x0343C/@0x03450) folgen demselben Punkt
#                                und bleiben getrennt (vor Runde 41: 14..23 Einheiten)
#   unit_r41_kollision_raum3001  Elza: ROOM3001 John + Sherry (sub00 @0x015A2/@0x015B6), Radien-Summe 750
#   unit_r41_kollision_kiste     FUN_8002cdf4 + Durchlauf @0x8002c03c-c0a8 an den ROOM2050-Kisten (Grenze inklusiv, Gate
#                                0x40, Y, vertikaler Eintritt), Raetsel-Endlage ohne Sperre
#   integration_r41_kollision_*  echte exe (test_r41_kollision.cmake): kiste (2050 obj 2 gegen obj 3: SCHUB-GESPERRT +
#                                OBJEKT-GESPERRT, Kiste bleibt bei |dX| > 1800), raetsel_leon / raetsel_elza (die drei Kisten
#                                in die Kerben slot 3/4/5 -> (5,0..2) = 1, ROOM2050 Leon / ROOM2051 Elza), npc3070 (Leon,
#                                Ada + Marvin), npc3001 (Elza, John + Sherry) — Abstand ueber RE15_STATE_LOG
add_executable(test_r41_kollision ${CMAKE_CURRENT_LIST_DIR}/../test_r41_kollision.c)
target_link_libraries(test_r41_kollision PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_kollision PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_kollision PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_kollision PRIVATE m)
endif()
foreach(_teil npc raum3070 raum3001 kiste)
    add_test(NAME unit_r41_kollision_${_teil} COMMAND test_r41_kollision ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_kollision_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

# Auswerter (kein eigener Test): Abstand der beiden NPC je Bild aus RE15_STATE_LOG.
add_executable(probe_r41_kollision_abstand ${CMAKE_CURRENT_LIST_DIR}/../probe_r41_kollision_abstand.c)
if(NOT WIN32)
    target_link_libraries(probe_r41_kollision_abstand PRIVATE m)
endif()

# Je Teil ein eigener Test mit eigenem Arbeitsordner und eigener exe-Kopie (Praefix je Teil).
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_kollision.cmake)
    foreach(_teil kiste raetsel_leon raetsel_elza npc3070 npc3001)
        add_test(NAME integration_r41_kollision_${_teil}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_ABSTAND_TOOL=$<TARGET_FILE:probe_r41_kollision_abstand>
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_kollision_${_teil}_wd
                         -DTEIL=${_teil}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_kollision.cmake)
        set_tests_properties(integration_r41_kollision_${_teil} PROPERTIES TIMEOUT 900)
    endforeach()
endif()
