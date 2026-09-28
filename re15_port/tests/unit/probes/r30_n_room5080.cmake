# --- Runde 30 (2026-09-28), Nachschliff Spur room5080: der ROOM5080-Birkin VOR der
#     Generator-Folge. Dossier: analysis/befunde_runde30/nachschliff-room5080.md, Abschnitt 8.1.
#     Die Birkin-Wandklemme nimmt das Band aus dem +0x82-Zustands-Byte (FUN_8003b0a4
#     @0x8003b234 `lbu v1,130(a3)` / @0x8003b23c), nicht aus der Hoehe; +0x82 = Spawn-Byte pc[4]
#     (Sce_em_set @0x800421c8/@0x800421d0), ROOM5080 Datei 0x0074A = 0.
#     RIEGEL: ROOM5080, Spieler steht im Westgang (-23350,-18200), 2400 Bilder ohne Eingabe.
#       A Abdeckung (Spawn wie Record, EMERGENCE, Birkin laeuft >= 5000 und erreicht die Nordwand)
#       B Birkin nie in einer Band-0-Zelle und nie im Raum (z < -6654, Nordwand SCA @0x00320)
#       C Spieler hp bleibt 100, kein Griff
#       D die Folge lief nicht (Bank 5 Bit 32 / (3,48) bleiben 0)
#     Vor dem Bau: ROT (B1 2024 Bilder in einer Band-0-Zelle, B2 2024 Bilder im Raum,
#     C hp min 0 bei 1447 Griff-Bildern); nach dem Bau GRUEN (Birkin steht bei z -5636 vor
#     der Nordwand, 0 Bilder in Zellen, hp 100). ---
add_executable(probe_r30_n_room5080 probe_r30_n_room5080.c)
target_link_libraries(probe_r30_n_room5080 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_n_room5080 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_n_room5080 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r30_n_room5080_birkin_wand COMMAND probe_r30_n_room5080)
set_tests_properties(unit_r30_n_room5080_birkin_wand PROPERTIES TIMEOUT 240 SKIP_RETURN_CODE 77)
