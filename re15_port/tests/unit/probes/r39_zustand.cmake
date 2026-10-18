# Runde 39 Spur F "zustand" — volle Heilung = ganze Kleidung; Gift zieht Energie wie RE2 (EKG).
#
# Dossier: analysis/befunde_runde39/F_zustand.md; Code: engine/src/zustand_r39.c, include/re15_zustand_r39.h,
# Haken in game_step_common.c, spinnen_r38.c, item_use_common.c, re15_inv_screen.c, platform/pc/main.c.
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
