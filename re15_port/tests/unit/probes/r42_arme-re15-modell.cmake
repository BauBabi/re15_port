# Runde 42 Strang "original" Stufe 2 (18.11.2026): die ROOM1210/1211-Gitterhaende zeichnen das RE1.5-Modell EM1A,
# das Verhalten bleibt das RE2-Zellenarm-Gehirn (NUTZERWUNSCH, Wortlaut im Test). Modul enemy_ai_re2_zellenarm.c,
# Block MODELL.
add_executable(test_r42_arme_re15_modell test_r42_arme_re15_modell.c)
target_link_libraries(test_r42_arme_re15_modell PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_arme_re15_modell PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_arme_re15_modell PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_r42_arme_re15_modell COMMAND test_r42_arme_re15_modell)
set_tests_properties(unit_r42_arme_re15_modell PROPERTIES TIMEOUT 120)
