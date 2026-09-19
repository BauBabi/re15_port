# Phase 3, Thema birkin-rest (analysis/befunde_2026-09-19/birkin-g5.md §7).
# GEPINNT -> add_test: Timer-Kante der Intro-Sender ([T4]/[T5]/[T10] lesen +0x158 VOR dem +1,
# @0x801013a4/@0x80101430/@0x80101640), Phasenbyte statt Routine-Wort ([T4] t==30 -> Phase 8
# @0x801013bc-e0) samt durchlaufendem Arm-Zeitgeber, und die vier Griff-Opfermaschinen der
# Spieler-Routine 5 (Haken 0x800CE300[0x37] = 0x80104288, Variantentabelle @0x8010582c).
add_executable(probe_p3_birkin_rest probe_p3_birkin_rest.c)
target_link_libraries(probe_p3_birkin_rest PRIVATE re15_engine re15_test_support)
target_include_directories(probe_p3_birkin_rest PRIVATE
    ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(probe_p3_birkin_rest PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME probe_p3_birkin_rest COMMAND probe_p3_birkin_rest)
