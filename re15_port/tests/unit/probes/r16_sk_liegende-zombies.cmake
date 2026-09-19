# Runde 16 Skeptiker (2026-09-19) — Gegen-Sonde zum Dossier liegende-zombies (Fresser-Treffbarkeit,
# Streuung auf den Liegenden, Band-Grenzen, Skript-Bump). Reine Messsonde, kein add_test.
add_executable(probe_r16_sk_liegende_zombies probe_r16_sk_liegende_zombies.c)
target_link_libraries(probe_r16_sk_liegende_zombies PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_sk_liegende_zombies PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_sk_liegende_zombies PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
