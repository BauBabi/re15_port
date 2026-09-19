# Phase 2, Thema birkin-g5 (analysis/befunde_2026-09-19/birkin-g5.md §6).
# GEPINNT -> add_test: Auftritt (-9000/-23400 @0x801011d0/d8), 2-Bone-Skinning/Augen/Kopf-
# Tracking (FUN_800197f4, 0x80105064, FUN_80017FDC), Tentakel-Anker/Treffer.
add_executable(probe_p2_birkin_g5 probe_p2_birkin_g5.c)
target_link_libraries(probe_p2_birkin_g5 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_p2_birkin_g5 PRIVATE
    ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(probe_p2_birkin_g5 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME probe_p2_birkin_g5 COMMAND probe_p2_birkin_g5)
