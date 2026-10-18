# Runde 39 Spur R "hund1121" — Elza, Panikphase: in ROOM1121 springt ein Hund durch das Fenster, durch das bei Leon in
# ROOM1120 die Kraehe hereinfliegt. Dossier analysis/befunde_runde39/R_hund1121.md, Code engine/src/hund1121_r39.c +
# include/re15_hund1121_r39.h, Startmaschine engine/src/enemy_ai_re2_dog.c (re2d_windowjump_launch).
#
#   unit_r39_hund1121_ereignis       Tor (9,97) + Einmal-Bit (7,0xAE), Slot 4 AUTO-Ereignis 30 auf Leons Band, Programm-Bytes,
#                                    Ausloeser ueber AOT-Scan + VM: Wand Band 1 frei, Hund (5400,-1827,12100), Leon unberuehrt
#   unit_r39_hund1121_sprung         RE2-Hirn + echte RE2-Bank: ROOM21A0-sub07-Schritte, Anlauf 8 x 65, Absprung SE 3, Bogen
#                                    gegen unabhaengige Rechnung, Glas im Durchgang (13 Splitter, Knall +3/+8), Wand wieder
#                                    zu, Landung SE 10, danach Sub 2 RUN, Schatten erst diesseits der Glasebene
#   unit_r39_hund1121_re15           RE1.5-KI-Modus: grid 0x40 wartet, Freigabe 0x43, Sprung, Ausgang Zustand 1, Glas
#   unit_r39_hund1121_einmal         Wiedereintritt / ohne (9,97): kein Ereignis
#   unit_r39_hund1121_startmaschine  RE2 Sub 14: Phase 0 wartet, Phase 1 faellt in den Rumpf, 8 Anlaufbilder
#   integration_r39_hund1121         echte exe, Elza-Spielstand + CONTINUE am Ankunftsplatz von Tuer 0, echter Weg durchs
#                                    Band: Log-Kette, Bilder (Hund im Fenster), Mitschnitt (Knall); Gegenlaeufe ohne (9,97),
#                                    mit (7,0xAE), Leon ROOM1120
add_executable(test_r39_hund1121 ${CMAKE_CURRENT_LIST_DIR}/../test_r39_hund1121.c)
target_link_libraries(test_r39_hund1121 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_hund1121 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_hund1121 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_hund1121 PRIVATE m)
endif()
foreach(_r39r_teil ereignis sprung re15 einmal startmaschine)
    add_test(NAME unit_r39_hund1121_${_r39r_teil} COMMAND test_r39_hund1121 ${_r39r_teil})
    set_tests_properties(unit_r39_hund1121_${_r39r_teil} PROPERTIES TIMEOUT 240)
endforeach()

if(TARGET re15_pc)
    add_test(NAME integration_r39_hund1121
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_WERKZEUG=$<TARGET_FILE:test_r39_hund1121>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r39_hund1121_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_hund1121.cmake)
    set_tests_properties(integration_r39_hund1121 PROPERTIES TIMEOUT 900)
endif()
