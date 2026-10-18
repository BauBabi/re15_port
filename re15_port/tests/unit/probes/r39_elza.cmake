# Runde 39 Spur H "elza" — drei Elza-Befunde: ROOM1031 Hand nur einmal heben, Karte direkt nach Roys Tagebuch,
# kein Klettern am Loescher in ROOM1091/1090. Dossier analysis/befunde_runde39/H_elza.md, Belege include/re15_elza_r39.h.
#
# RIEGEL (Unit, je Nutzer-Punkt die MECHANIK):
#   unit_r39_elza_klettern_zensus    Punkt 3: alle Obj_model_set-Records der 240 RDTs — Null-Box-Records liegen nie auf
#                                    dem Spielerband ihrer Hoehe (Original-Form der Regel re15_elza_r39_prop_band_ueber_boden)
#   unit_r39_elza_klettern_loescher  Punkt 3: echter Raumstart Elza ROOM1091 / Leon ROOM1090 mit den Port-Installern:
#                                    Loescher auf Band 2, Aktions-Sonde startet an der gemessenen Stelle in keiner Richtung
#                                    den Aufstieg, Original-Kiste weiter kletterbar, Untersuchen-Platz bleibt
#   unit_r39_elza_hand               Punkt 1: Eintritts-Latch vs. Plc_motion (Kommandowort 4): Clip 11 bleibt stehen
#   (Punkt 2: unit_r38_tagebuch_ablauf + integration_r38_tagebuch_lesen pruefen den neuen Zeitpunkt der Kartenkette —
#    tests/unit/test_r38_tagebuch.c, tests/integration/test_r38_tagebuch.cmake, angepasst in Runde 39 Spur H)
add_executable(test_r39_elza_klettern ${CMAKE_CURRENT_LIST_DIR}/../test_r39_elza_klettern.c)
target_link_libraries(test_r39_elza_klettern PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_elza_klettern PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_elza_klettern PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_elza_klettern PRIVATE m)
endif()
foreach(_teil zensus loescher)
    add_test(NAME unit_r39_elza_klettern_${_teil} COMMAND test_r39_elza_klettern ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r39_elza_klettern_${_teil} PROPERTIES TIMEOUT 240)
endforeach()

add_executable(test_r39_elza_hand ${CMAKE_CURRENT_LIST_DIR}/../test_r39_elza_hand.c)
target_link_libraries(test_r39_elza_hand PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_elza_hand PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_elza_hand PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_elza_hand PRIVATE m)
endif()
add_test(NAME unit_r39_elza_hand COMMAND test_r39_elza_hand WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r39_elza_hand PROPERTIES TIMEOUT 240)

# INTEGRATION (echte exe, tests/integration/test_r39_elza.cmake):
#   integration_r39_elza_hand      Neues Spiel Elza bis ROOM1031 sub13: Clip 11 posiert ab dem Plc_motion (ANIM_TRACE),
#                                  kein 210/200 danach; Clip 0 rueckwaerts bei +40 Bildern
#   integration_r39_elza_karte     Roys Tagebuch: Hinweis-Beginn direkt nach dem Leser (kein Plc_motion/Plc_ret davor),
#                                  Faden laeuft erst nach dem Hinweis weiter
#   integration_r39_elza_loescher  Elza ROOM1091 neben dem Loescher, Viereck in 4 Richtungen: kein sub=9 (CLIMB_TRACE),
#                                  Untersuchen msg 29
if(TARGET re15_pc AND TARGET probe_r37_feuerloescher_karte AND TARGET probe_r36_elza_karte)
    foreach(_teil hand karte loescher)
        add_test(NAME integration_r39_elza_${_teil}
            COMMAND ${CMAKE_COMMAND}
                -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                -DRE15_KARTE_FL=$<TARGET_FILE:probe_r37_feuerloescher_karte>
                -DRE15_KARTE_ELZA=$<TARGET_FILE:probe_r36_elza_karte>
                -DWORKDIR=${CMAKE_BINARY_DIR}/r39_elza_wd
                -DTEIL=${_teil}
                -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_elza.cmake)
        set_tests_properties(integration_r39_elza_${_teil} PROPERTIES TIMEOUT 1200)
    endforeach()
endif()
