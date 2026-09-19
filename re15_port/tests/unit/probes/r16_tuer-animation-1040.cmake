# Runde 16 (2026-09-19) — Sonde "komische Spieler-Animation direkt nach dem Raumeintritt (ab ROOM1040)".
# Reine Messsonde (kein add_test): faehrt die Tuer ROOM1030 -> ROOM1040 ueber den echten
# Pfad (aot_scan -> apply_pending -> transition_present) und protokolliert die ersten Bilder
# inkl. der Render-Bankwahl (anim_select). Dossier: analysis/befunde_2026-09-19/tuer-animation-1040.md
add_executable(probe_r16_tuer1040 probe_r16_tuer1040.c)
target_link_libraries(probe_r16_tuer1040 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_tuer1040 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_tuer1040 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
