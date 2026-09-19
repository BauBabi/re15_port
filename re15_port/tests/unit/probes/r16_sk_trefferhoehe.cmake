# Runde 16 (2026-09-19) — SKEPTIKER-Gegensonde zum Thema "trefferhoehe": Sub-Box-Grenzen des
# RE2-Appliers FUN_800410CC mit der Radius-Erweiterung `+0x1EE >> 2` (INIT sh 500 @0x80100980).
# Reine Messsonde, kein add_test. Dossier: analysis/befunde_2026-09-19/trefferhoehe.skeptiker.md
add_executable(probe_r16_sk_trefferhoehe probe_r16_sk_trefferhoehe.c)
target_link_libraries(probe_r16_sk_trefferhoehe PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_sk_trefferhoehe PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_sk_trefferhoehe PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
