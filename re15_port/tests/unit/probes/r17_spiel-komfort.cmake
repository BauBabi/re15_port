# Runde 17 (2026-09-20) — Thema "spiel-komfort": drei NUTZER-ENTSCHEIDUNGEN (ausdruecklich
# KEINE byte-true Fragen; die Mechanismen/Nummern sind belegt, die Entscheidung ist es nicht):
#   1) aufgesammelte Munition wird halbiert (Abrundung, Mindestmenge 1)
#   2) Klick-Laut am Cursor-Raetsel (RE2-Vorbild FUN_8006b358 @0x8006b57c-84 / @0x8006b5b4-bc)
#   3) Untertitel bleiben so lange stehen wie die Sprachaufnahme laeuft (NACHHALL)
# Dossier: analysis/befunde_2026-09-20/spiel-komfort.md

add_executable(probe_r17_munition probe_r17_munition.c)
target_link_libraries(probe_r17_munition PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r17_munition PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_r17_munition_pin COMMAND probe_r17_munition pin)
set_tests_properties(unit_r17_munition_pin PROPERTIES TIMEOUT 60)

add_executable(probe_r17_cursor_klick probe_r17_cursor_klick.c)
target_link_libraries(probe_r17_cursor_klick PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r17_cursor_klick PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r17_cursor_klick PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r17_cursor_klick_pin COMMAND probe_r17_cursor_klick pin)
set_tests_properties(unit_r17_cursor_klick_pin PROPERTIES TIMEOUT 180)

add_executable(probe_r17_untertitel probe_r17_untertitel.c)
target_link_libraries(probe_r17_untertitel PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r17_untertitel PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r17_untertitel PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_SYNCHRO_DIR="${CMAKE_SOURCE_DIR}/../synchro")
add_test(NAME unit_r17_untertitel_pin COMMAND probe_r17_untertitel pin)
set_tests_properties(unit_r17_untertitel_pin PROPERTIES TIMEOUT 60)
