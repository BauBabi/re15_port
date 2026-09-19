# --- Phase 2 (2026-09-19), Thema kriecher-1010: Pins zu F1 (Rueckversatz nach Treffer) und
#     F2 (Totstellen, RE2-Deskriptor 1 -> 0x201) + Griff-Ausgang (Risiko 1).
#     Dossier: analysis/befunde_2026-09-19/kriecher-1010.md §6. Braucht die RE2-Bank
#     shared_assets/RE2/CDEMD0.EMS (sonst SKIP 77). ---
add_executable(test_p2_kriecher1010 test_p2_kriecher1010.c)
target_link_libraries(test_p2_kriecher1010 PRIVATE re15_engine re15_test_support)
target_include_directories(test_p2_kriecher1010 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_p2_kriecher1010 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_re2z_crawl_hurt_reentry COMMAND test_p2_kriecher1010 hurt_reentry)
set_tests_properties(unit_re2z_crawl_hurt_reentry PROPERTIES TIMEOUT 120 SKIP_RETURN_CODE 77)
add_test(NAME unit_re2z_crawler_spawn_wait COMMAND test_p2_kriecher1010 spawn_wait)
set_tests_properties(unit_re2z_crawler_spawn_wait PROPERTIES TIMEOUT 120 SKIP_RETURN_CODE 77)
