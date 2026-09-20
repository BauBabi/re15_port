# --- Runde 18 (2026-09-21), Thema birkin-restluecken: die drei offenen Punkte des
#     G5-Endkampfs aus analysis/befunde_2026-09-20/birkin-1zu1.md §5 —
#     Rumble-Ringe (@0x800396fc), Blut-Spawns der Trefferroutine (@0x8001bf10) und die
#     Devour-Opfermaschine der Spieler-Routine 6 (@0x80103908).
#     Belegt in analysis/befunde_2026-09-21/birkin-restluecken.md.
#     Braucht die RE2-Bank shared_assets/RE2/CDEMD0.EMS (sonst SKIP 77). ---
add_executable(probe_r18_birkin_rest probe_r18_birkin_rest.c)
target_link_libraries(probe_r18_birkin_rest PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r18_birkin_rest PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r18_birkin_rest PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r18_birkin_restluecken_pin COMMAND probe_r18_birkin_rest pin)
set_tests_properties(unit_r18_birkin_restluecken_pin PROPERTIES TIMEOUT 180 SKIP_RETURN_CODE 77)
