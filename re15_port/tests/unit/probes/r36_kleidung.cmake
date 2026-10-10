# Spur B (Runde 36) — "kleidung": Leons zerrissene/blutige Kleidung (Blut-Decal-Wundsystem
# FUN_80037edc @0x80037edc, Dispatcher der Opfer-FSM @0x8010a580-5b0). Dossier
# analysis/befunde_runde36/B_kleidung.md.
#
# Riegel: griff_re2 / griff_re15 — ROOM1140 auf dem echten game_step-Weg: ueberlebte Griffe muessen
# die Wund-Tabelle stempeln (nach drei ueberlebten Griffen Panel 5/7 Level 1, blood_decals.md §3.4).
add_executable(test_r36_kleidung ${CMAKE_CURRENT_SOURCE_DIR}/test_r36_kleidung.c)
target_link_libraries(test_r36_kleidung PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_kleidung PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_kleidung PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_fl re2 re15)
    add_test(NAME unit_r36_kleidung_griff_${_fl} COMMAND test_r36_kleidung griff ${_fl})
    set_tests_properties(unit_r36_kleidung_griff_${_fl} PROPERTIES TIMEOUT 240)
endforeach()
