# --- Runde 30 (2026-09-27), Thema C "Android: R1 als Umschalter".
#     Dossier: analysis/befunde_runde30/android-r1-toggle.md
#     RIEGEL (Bau Runde 30): die Sonde bindet den AUSGELIEFERTEN Umschalter
#     platform/pc/src/touch_r1_toggle_pc.h ein (header-only, kein SDL) und fragt den Phasen-Riegel
#     re15_player_pad_live() der Engine ab (engine/src/pad_phase_common.c).
#     Sie faehrt den echten Spielschritt dreifach (HALTEN / RASTE / NAIV) und faellt mit
#     exit 1, sobald die RASTE einen Sollwert verfehlt (Fall A 10/10/1 wie HALTEN, B kein R1
#     im Menue + FILE-Sprung, C +5/+10, D-I Raste faellt, H Raste bleibt). ---
add_executable(probe_r30_android_r1_toggle probe_r30_android_r1_toggle.c)
target_link_libraries(probe_r30_android_r1_toggle PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_android_r1_toggle PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/platform/pc/src)
target_compile_definitions(probe_r30_android_r1_toggle PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_r30_android_r1_toggle COMMAND probe_r30_android_r1_toggle)
set_tests_properties(unit_r30_android_r1_toggle PROPERTIES TIMEOUT 60)
