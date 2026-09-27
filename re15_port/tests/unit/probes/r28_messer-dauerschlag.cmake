# --- Runde 28 (2026-09-27), Thema messer-dauerschlag: "wenn ich mit dem Messer die ganze Zeit
#     nach unten schlage, kommen die Zombies nicht nah genug an mich ran, um mich zu beissen."
#     Sonde faehrt den ECHTEN Weg (re15_game_step + Pad, ROOM1140, RE2-Bank EM010). ---
add_executable(probe_r28_messer_dauerschlag probe_r28_messer_dauerschlag.c)
target_link_libraries(probe_r28_messer_dauerschlag PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r28_messer_dauerschlag PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r28_messer_dauerschlag PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
#     RIEGEL: exit 1, sobald die Messer-Reichweite ueber der RE1.5-Kegel-Grenze 1500 liegt
#     (Reichweite @0x8006E5A0[1] = 1100 + Gegner-Radius 400, Tester @0x8006E548[1] =
#     FUN_800127FC). Am alten Stand: 3000 (TIEF) / 3400 (EBEN) = ROT.
#     GEGEN-RIEGEL: Messer trifft weiterhin (>= 1400); der geschlagene Zombie beisst nicht
#     OEFTER als der unbehelligte; d_min im Dauertreffer-Lauf bleibt >= 850 (Koerper-
#     Standabstand 400+450, FUN_8002aec4 @0x8002b164).
add_test(NAME r28_messer_dauerschlag COMMAND probe_r28_messer_dauerschlag 1400)
set_tests_properties(r28_messer_dauerschlag PROPERTIES TIMEOUT 180)
