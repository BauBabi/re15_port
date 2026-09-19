# --- Runde 16 (2026-09-19), Skeptiker-Gegensonde zu kriecher-1010. Reine Messsonde, kein add_test.
#     Dossier: analysis/befunde_2026-09-19/kriecher-1010.md (+ .skeptiker.md) ---
add_executable(probe_r16_sk_kriecher_1010 probe_r16_sk_kriecher_1010.c)
target_link_libraries(probe_r16_sk_kriecher_1010 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_sk_kriecher_1010 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_sk_kriecher_1010 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
