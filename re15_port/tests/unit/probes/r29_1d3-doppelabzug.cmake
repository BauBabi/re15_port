# --- Runde 29 (2026-09-27), Thema "die Trefferpause +0x1D3 wird doppelt abgezogen".
#     Das Original hat GENAU EIN Dekrement je Bild, im Root-Prolog des jeweiligen Overlays
#     (Zombie EMZ0.BIN @0x80100484-98, Hund EMD0G_MOD0.BIN @0x80100028-3C,
#      Kraehe EMOVL21_S0.BIN @0x80100160-74, 0x25 EMS25.BIN @0x801000F4-100,
#      0x26 EMS26.BIN @0x80100048-54).
#     Die Sonde faehrt den ECHTEN Weg (re15_game_step + Pad) und misst das Delta je Bild.
add_executable(probe_r29_1d3_doppelabzug probe_r29_1d3_doppelabzug.c)
target_link_libraries(probe_r29_1d3_doppelabzug PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r29_1d3_doppelabzug PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r29_1d3_doppelabzug PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
#     RIEGEL (Argument "riegel"): je Typ MUSS gelten
#       (a) Delta je Bild == 1  (ein Abzug, byte-true zum Root-Prolog)
#       (b) +0x1D3 erreicht garantiert 0   <- die Runde-14-Falle: eine eingefrorene Pause
#           macht den Gegner DAUERHAFT unverwundbar
#     GEGEN-RIEGEL gegen Ueberkorrektur: die Pause nach einem echten Pistolentreffer ist
#     GENAU der Sollwert der Stun-Zeile (Zombie 0x800A412C = 15, Hund 0x800A4424 = 15;
#     Stun = (Zeile[+4] >> 9) & 0x7F, Stempel @0x80047338-4C).
add_test(NAME r29_1d3_doppelabzug COMMAND probe_r29_1d3_doppelabzug riegel)
set_tests_properties(r29_1d3_doppelabzug PROPERTIES TIMEOUT 240)
