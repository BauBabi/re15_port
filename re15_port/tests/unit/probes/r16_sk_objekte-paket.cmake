# Skeptiker-Gegensonde Runde 16 (2026-09-19) zum Dossier objekte-paket.md. Reine Messsonde,
# kein add_test. Ergebnis: analysis/befunde_2026-09-19/objekte-paket.skeptiker.md
add_executable(probe_r16_sk_objekte_paket probe_r16_sk_objekte_paket.c)
target_link_libraries(probe_r16_sk_objekte_paket PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_sk_objekte_paket PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_sk_objekte_paket PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
