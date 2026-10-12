# Runde 36 / G2 "generator_ruhezweig": nach dem Merge der Spur R (Ruhe-Zweig des Objekt-Passes, LAB_8002c0b8 =
# FUN_8003b558(obj,2) `jal` @0x8002c0d0 + Ausschiebung FUN_8003da78 `jal` @0x8003b79c, engine/src/zellschub_r36.c)
# steht der Panel-Cursor an den Rahmenzellen seines Schalterfelds — wie im Original (DuckStation + GDB gemessen,
# analysis/befunde_runde36/G2_generator_ruhezweig.md, G2_belege/orig_cursor_lauf1/log.txt: ROOM11F0 OBEN z 29232,
# UNTEN z 16068, LINKS x -29782, RECHTS x -9668).
#
#   probe_r36_g2_cursor_rahmen   Zensus aller Panel-Cursor (Typ 4, Band 0, 0x0101, h900; 16 Cursor in 14 RDTs): je
#                                Richtung +-200 je Bild nach der SCD und vor dem Objekt-Pass (= Add_speed @0x80040f40),
#                                bis der Cursor 15 Bilder steht; Endlage, Bilanz, erste Ausschiebung.
add_executable(probe_r36_g2_cursor_rahmen
    ${CMAKE_CURRENT_SOURCE_DIR}/probe_r36_g2_cursor_rahmen.c)
target_link_libraries(probe_r36_g2_cursor_rahmen PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_g2_cursor_rahmen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r36_g2_cursor_rahmen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
#   unit_r36_g2_cursor_rahmen    Riegel (Argument pin): je Cursor und Richtung Bilder mit Bewegung, Endlage und erste
#                                Ausschiebung = ORIGINAL (G2_belege/orig_zensus/log.txt, orig_cursor_zensus.py: dieselbe
#                                Nachbildung am Haltepunkt @0x8002bd44). Rueckbau-Nachweis (Ruhe-Zweig aus) in
#                                G2_generator_ruhezweig.md, P3.
add_test(NAME unit_r36_g2_cursor_rahmen COMMAND probe_r36_g2_cursor_rahmen pin)
set_tests_properties(unit_r36_g2_cursor_rahmen PROPERTIES TIMEOUT 300)
