# Runde 35 Spur M — ROOM1120 Cut 1: das hintere Fenster zerbricht (RE2-Glas + Knall), Kraehe fliegt herein.
# Dossier analysis/befunde_runde35/M_cut11c0_fenster.md; Code engine/src/fenster_1120.c, re2_fx.c,
# enemy_ai_re2_crow.c, platform/pc/src/glas1120_pc.c.
#
#   unit_r35_fenster_glas      Raum-ESP + Splitter-Bildfolge (Op 1/16/5/39/84) gegen unabhaengige Rechnung
#   unit_r35_fenster_kraehe    RE2-Kraehe State 4 Sub 2 (versteckt, Befehl +0x1D4, 7 Bilder 1890, ACTIVE 4)
#   unit_r35_fenster_knall     RE2-Raumbank GLAS1090 Satz 0x21 = Prog 0 Ton 7..10 (VAG 5), Wellen vorhanden
#   unit_r35_fenster_ereignis  install/VM/AOT/Zeitlinie T+0/2/5/10/23, Schaden nur Cut 1, Tor + Einmaligkeit
#   unit_r35_fenster_satzform  Nachbesserung 1: p0 0x00FF / p1-Unterbyte 0x18 = ROOM1050 @0x0C30/@0x0C32 und
#                              ROOM1020 @0x1E18 (sat 0x41); p0 >= 0xa = erster freier Faden (0x8003ee3c @0x8003ee54)
#   integration_r35_fenster    echte exe: Spielstand ROOM1120 + CONTINUE, Leon laeuft nach Norden ins Band,
#                              Log + Framedump vor/nach (Schadenspixel), Wiedereintritt mit (9,79)=1
add_executable(probe_r35_fenster ${CMAKE_CURRENT_LIST_DIR}/../test_r35_cut11c0_fenster.c)
target_link_libraries(probe_r35_fenster PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r35_fenster PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r35_fenster PRIVATE m)
endif()
foreach(_r35m_teil glas kraehe ereignis knall satzform)
    add_test(NAME unit_r35_fenster_${_r35m_teil} COMMAND probe_r35_fenster ${_r35m_teil})
    set_tests_properties(unit_r35_fenster_${_r35m_teil} PROPERTIES TIMEOUT 120)
endforeach()

if(TARGET re15_pc)
    add_test(NAME integration_r35_fenster
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r35_fenster>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r35_fenster_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r35_cut11c0_fenster.cmake)
    set_tests_properties(integration_r35_fenster PROPERTIES TIMEOUT 600)
endif()
