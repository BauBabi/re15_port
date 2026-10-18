# Runde 39 Spur F "zustand" — volle Heilung = ganze Kleidung; Gift zieht Energie wie RE2 (EKG).
#
# Dossier: analysis/befunde_runde39/F_zustand.md; Code: engine/src/zustand_r39.c, include/re15_zustand_r39.h,
# Haken in game_step_common.c, spinnen_r38.c, item_use_common.c, re15_inv_screen.c, platform/pc/main.c (2 Zeilen);
# PC-Seite der Haut-Wiederherstellung: platform/pc/src/zustand_r39_pc.c (Nachbesserung 1).
#
#   unit_r39_zustand   Mechanik (Wund-Tabelle nach voller/teilweiser Heilung, RE2-Gift-Takt im Spielschritt,
#                      EKG-Farbe des Gift-Zustands); das Programm ist zugleich das Werkzeug der Echtlauf-Riegel
#                      (karte / bild / ekg / hp).
add_executable(test_r39_zustand ${CMAKE_CURRENT_LIST_DIR}/../test_r39_zustand.c)
target_link_libraries(test_r39_zustand PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_zustand PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_zustand PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r39_zustand PRIVATE m)
endif()
add_test(NAME unit_r39_zustand COMMAND test_r39_zustand)
set_tests_properties(unit_r39_zustand PROPERTIES TIMEOUT 300)

#   integration_r39_zustand_kleidung   echte exe (Leon ROOM1130, Elza ROOM1131): Spielstand mit Wunden + Heilspray,
#                                      CONTINUE, Spray ueber den Statusschirm -> Framedump 650 == Lauf ohne Wunden
#                                      (Textur der Figur), Bild 150 != (Wunden vorher sichtbar).
#   integration_r39_zustand_gift       echte exe: Messschiene RE15_R39_GIFT_AT/RE15_R39_LOG -> -1 HP alle 31 Bilder
#                                      ab Bild 330, Untergrenze HP 1; Statusschirm-Framedump: EKG-Spur magenta.
if(TARGET re15_pc)
    add_test(NAME integration_r39_zustand_kleidung
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_TOOL=$<TARGET_FILE:test_r39_zustand>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r39_zustand_kleidung_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_zustand_kleidung.cmake)
    set_tests_properties(integration_r39_zustand_kleidung PROPERTIES TIMEOUT 900)
    add_test(NAME integration_r39_zustand_gift
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_TOOL=$<TARGET_FILE:test_r39_zustand>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r39_zustand_gift_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_zustand_gift.cmake)
    set_tests_properties(integration_r39_zustand_gift PROPERTIES TIMEOUT 900)
endif()
