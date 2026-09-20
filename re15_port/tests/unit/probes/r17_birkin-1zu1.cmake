# --- Runde 17 (2026-09-20), Thema birkin-1zu1: die Koerper-Kollision des G5-Endbosses
#     (Nutzer: "Birkin ist nicht solid"). Belegt in
#     analysis/befunde_2026-09-20/birkin-1zu1.md.
#     Braucht die RE2-Bank shared_assets/RE2/CDEMD0.EMS (sonst SKIP 77). ---
add_executable(probe_r17_birkin_1zu1 probe_r17_birkin_1zu1.c)
target_link_libraries(probe_r17_birkin_1zu1 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r17_birkin_1zu1 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r17_birkin_1zu1 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r17_birkin_1zu1_pin COMMAND probe_r17_birkin_1zu1 pin)
set_tests_properties(unit_r17_birkin_1zu1_pin PROPERTIES TIMEOUT 180 SKIP_RETURN_CODE 77)
