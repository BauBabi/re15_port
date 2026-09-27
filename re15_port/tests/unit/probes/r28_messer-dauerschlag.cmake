# --- Runde 28 (2026-09-27), Thema messer-dauerschlag: "wenn ich mit dem Messer die ganze Zeit
#     nach unten schlage, kommen die Zombies nicht nah genug an mich ran, um mich zu beissen."
#     Sonde faehrt den ECHTEN Weg (re15_game_step + Pad, ROOM1140, RE2-Bank EM010). ---
add_executable(probe_r28_messer_dauerschlag probe_r28_messer_dauerschlag.c)
target_link_libraries(probe_r28_messer_dauerschlag PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r28_messer_dauerschlag PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r28_messer_dauerschlag PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
