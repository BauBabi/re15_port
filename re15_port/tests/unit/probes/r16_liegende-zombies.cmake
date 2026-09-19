# Runde 16 (2026-09-19) — Thema "liegende-zombies": passiv liegende 0x88-Spawns (ROOM1140/10E0)
# reagieren im RE2-Flavor auf den Spieler. Reine Messsonde, kein add_test.
add_executable(probe_r16_liegende_zombies probe_r16_liegende_zombies.c)
target_link_libraries(probe_r16_liegende_zombies PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_liegende_zombies PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_liegende_zombies PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
