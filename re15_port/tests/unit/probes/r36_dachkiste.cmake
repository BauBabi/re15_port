# Runde 36 / Spur R — "dachkiste": kann die Dach-Kiste obj 1 in ROOM1090/1091 (die einzige Stufe zum Tor nach
# ROOM1100, Tuer slot 1 @0x0213A Band 6) durch Schieben mit dem Spieler eine Absprungzelle erreichen und
# fallen (Objekt-Fall FUN_8002c444, Spur A)? Dossier analysis/befunde_runde36/R_dachkiste.md.
#
#   probe_r36_dachkiste   Freiraum-Zensus MIT SPIELER (gemeinsame Suche Kistenzelle x Spielerseite) mit den
#                         Praedikaten der Schubkette FUN_8002bd44 (@0x8002bf38 Band, @0x8002bf50 Reichweite
#                         FUN_8002d1e8 563/900, @0x8002bfa4 FUN_8002cabc(.,.,1) Kiste = Spieler +- (hx+450) auf der
#                         Achse des groesseren |d| @0x8002cce8-d10, @0x8002bfb0 FUN_8003b558(obj,2) u0 & 2
#                         @0x8003b64c-660 mit r = hx @0x8003b68c-690, Ausschiebung @0x8002c0d8-e8) und des
#                         Objekt-Pass-Ausloesers (@0x8002c120-134 FUN_8003b7f0(.,0x1c2,Band) & 2). Spieler-
#                         Freiheit absichtlich ueberschaetzt (notwendige Bedingung).
#     unit_r36_dachkiste_zensus   Riegel: ROOM1090/1091 obj 1 erreicht KEINE Absprungzelle, Kistenmitte bleibt
#                         in z [-15503, -15482] (Bruestung r2[0] @0x01F20: kein Spieler noerdlich der Kiste
#                         moeglich); ROOM3050/3051 obj 1 und ROOM2050/2051 obj 6 als Diagnose (Punkt 4).
add_executable(probe_r36_dachkiste
    ${CMAKE_CURRENT_SOURCE_DIR}/probe_r36_dachkiste.c)
target_link_libraries(probe_r36_dachkiste PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_dachkiste PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r36_dachkiste PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r36_dachkiste_zensus COMMAND probe_r36_dachkiste pin)
set_tests_properties(unit_r36_dachkiste_zensus PROPERTIES TIMEOUT 600)
