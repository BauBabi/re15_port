# --- Runde 30 (2026-09-28), Nachschliff Spur room5080: der ROOM5080-Birkin VOR der
#     Generator-Folge. Dossier: analysis/befunde_runde30/nachschliff-room5080.md, Abschnitt 8.1.
#     Die Birkin-Wandklemme nimmt das Band aus dem +0x82-Zustands-Byte (FUN_8003b0a4
#     @0x8003b234 `lbu v1,130(a3)` / @0x8003b23c), nicht aus der Hoehe; +0x82 = Spawn-Byte pc[4]
#     (Sce_em_set @0x800421c8/@0x800421d0), ROOM5080 Datei 0x0074A = 0.
#     ⛔ NACHBESSERUNG 2 (Frost-Schranke, Abschnitt 9): die Teile A2/A3 der ersten Fassung
#     ("EMERGENCE laeuft, Birkin laeuft >= 5000 und klemmt an der Nordwand") waren FALSCH und
#     sind umgekehrt. Die Wurzel friert bei grid & 0x20 ein (STAGE5 @0x80116a7c/@0x80116a84/
#     @0x80116a88 -> nur jal 0x8001b064), der INIT lief schon beim Spawn (Sce_em_set @0x8004259c).
#     RIEGEL: ROOM5080, Spieler steht im Westgang (-23350,-18200).
#       Teil 1, 2400 Bilder ohne Folge:
#       A1 Spawn wie der Record; A2 in jedem Bild eingefroren (-18100,-5600,200), grid 0x33,
#       Zustand 1/Sub 9, +0x95 0x10; A3 keine Bewegung; B nie in einer Band-0-Zelle/im Raum;
#       C Spieler hp 100, kein Griff; D Folge lief nicht.
#       Teil 2, die Folge (AOT Platz 1, Ja): E1 Freigabe durch Member_set @0x0083E, E2 zuerst
#       EMERGENCE (Clip 0x10), dann WALK, E3 nach dem Fall y 0, E4 nie in einer Band-0-Zelle.
#     Negativ-Kontrolle: ohne die Schranke ROT (A2/A3/B/C). ---
add_executable(probe_r30_n_room5080 probe_r30_n_room5080.c)
target_link_libraries(probe_r30_n_room5080 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_n_room5080 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_n_room5080 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r30_n_room5080_birkin_wand COMMAND probe_r30_n_room5080)
set_tests_properties(unit_r30_n_room5080_birkin_wand PROPERTIES TIMEOUT 240 SKIP_RETURN_CODE 77)
