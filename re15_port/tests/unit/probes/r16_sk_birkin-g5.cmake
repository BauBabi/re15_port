# Skeptiker-Gegensonde Runde 16, Thema birkin-g5 (analysis/befunde_2026-09-19/birkin-g5.skeptiker.md).
# Reine MESSUNG, kein Pin -> kein add_test.
add_executable(probe_r16_sk_birkin_g5 probe_r16_sk_birkin_g5.c)
target_link_libraries(probe_r16_sk_birkin_g5 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_sk_birkin_g5 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_sk_birkin_g5 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
