# --- Phase 3 (2026-09-19), Thema re-restposten: Pins zu den vier Restposten
#     [A] Messer durch den RE2-Applier (trefferhoehe.md §5)
#     [B] HP-Halbierung des Kriecher-INIT @0x80100B3C-5C (kriecher-1010.md §6.6 F3)
#     [C] Treffbarkeit erst am Pose-Ausgang (liegende-zombies.md §5 Punkt 2)
#     [D] Deskriptoren 0x84/0x85 = Fresser-Familie (liegende-zombies.md §5 Punkt 1)
#     Braucht die RE2-Bank shared_assets/RE2/CDEMD0.EMS (sonst SKIP 77). ---
add_executable(probe_p3_restposten probe_p3_restposten.c)
target_link_libraries(probe_p3_restposten PRIVATE re15_engine re15_test_support)
target_include_directories(probe_p3_restposten PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_p3_restposten PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_p3_restposten_pin COMMAND probe_p3_restposten pin)
set_tests_properties(unit_p3_restposten_pin PROPERTIES TIMEOUT 120 SKIP_RETURN_CODE 77)
