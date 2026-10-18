# Runde 39 Spur D "treffer" - Nutzerpunkte AUFTRAG.md: halb aufstehende Zombies treffbar; Zombies ueber den Tisch
# treffbar; Zielhilfe nur fuer sichtbare Gegner.
#
# Dossier: analysis/befunde_runde39/D_treffer.md; Code: engine/src/treffer_r39.c, include/re15_treffer_r39.h,
# gen/treffer_r39_moebel.inc (Generator tools/r39_treffer_moebel_gen.py), Haken in schuss_r36.c (Schuss-Sichtlinie)
# und re15_damage.c re15_player_aim_target (Zielhilfe).
#
#   unit_r39_treffer_tisch       ROOM1140 im echten Spielschritt (RDT, Sce_em_set, RE2-Baenke, re15_player_weapon_fire):
#                                Pistole ueber den Konferenztisch [6] trifft (vorher 0 Treffer, Sichtlinie verworfen),
#                                im freien Gang trifft, hinter dem Wandblock [3] bleibt gesperrt; Gegenprobe alter
#                                Stand (Moebel-Liste aus) = 0 Treffer ueber den Tisch.
#   unit_r39_treffer_zielhilfe   Zielhilfe (re15_player_aim_target): ein Zombie hinter dem Wandblock wird nicht
#                                angevisiert, der weiter entfernte sichtbare gewinnt; nur der verdeckte -> kein Ziel;
#                                ueber den Tisch wird angevisiert; Gegenprobe alter Stand = der verdeckte gewinnt.
#   unit_r39_treffer_moebel      Riegel der Moebel-Liste gegen die RDT-Bytes: jeder Eintrag ist eine Art-3-Zelle mit
#                                genau dieser Geometrie in seinem Raum, kein Eintrag ist Huelle (Wand), Leon/Elza-Paare.
#   unit_r39_treffer_aufsteher   RE2-Zombie im Boden-Aufsteher (EXEC[5] P7, Clip 8): TIEF trifft vor Clip-Bild 55,
#                                EBEN erst ab Bild 55 (Maske @0x801036d0-f0), Kopfhoehe je Bild als Messung.
add_executable(test_r39_treffer ${CMAKE_CURRENT_LIST_DIR}/../test_r39_treffer.c)
target_link_libraries(test_r39_treffer PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_treffer PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_treffer PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r39_treffer PRIVATE m)
endif()
foreach(_t tisch zielhilfe moebel aufsteher)
    add_test(NAME unit_r39_treffer_${_t} COMMAND test_r39_treffer ${_t})
    set_tests_properties(unit_r39_treffer_${_t} PROPERTIES TIMEOUT 300)
endforeach()
