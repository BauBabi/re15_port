# Runde 37 (2026-10-12), Spur L "re2zombie" — RE2-Zombie-KI: die drei 0x201-Produzenten (Arme-hoch-Gang EXEC[2]),
# DECISION[0] Block 3, EXEC[0] P1. Dossier analysis/befunde_runde37/L_re2zombie.md, Schalter include/re15_re2zombie_r37.h.
#
#   unit_r37_re2zombie_prod1     (1) Timer @0x80101D68-DBC: je Wurf 0x201 genau bei (rand&3)==0 und +0x1F0 < 0x1388,
#                                Reseed (r>>3)+100, zwei Wuerfe; Gegenprobe +0x1F0 == 0x1388.
#   unit_r37_re2zombie_prod2     (2) Nahbereich @0x80101EC4-F10: +0x1F0 < 0x7D0, +-512-Kegel, Spieler unbeansprucht; Gegenproben
#                                hinten / 0x7D0 / beansprucht / Maske ohne (2).
#   unit_r37_re2zombie_prod3     (3) Rennen @0x80101F14-F60: Spieler rennt, +0x1F0 < 0x9C4, +-1324-Kegel; Gegenproben gehen /
#                                0x9C4 / hinten.
#   unit_r37_re2zombie_b         DECISION[0] Block 3 committet 0xC01 genau bei (rand&1)==1 (@0x801013c8).
#   unit_r37_re2zombie_c         EXEC[0] P1: Wander-Wurf + Selbstwecker im selben Takt, P2 erst im naechsten (@0x80101540-a0).
#   unit_r37_re2zombie_nahkampf  Positivkontrolle Messer (Aufbau test_re2_zombie_abc): > 250 und >= 95 % von Maske 0.
#   unit_r37_re2zombie_raum1130  Nutzer-Beispiel (Gang nach Irons' Tod): Wechsel in den Arme-hoch-Gang vor Leon (+0x1F0 < 0x7D0,
#                                RE2-Original 1987/1999), Griff aus dem Gang; Gegenprobe Maske 0: keiner.
#   unit_r37_re2zombie_raum1180  dasselbe im Normalspiel (ROOM1180).
add_executable(test_r37_re2zombie ${CMAKE_CURRENT_LIST_DIR}/../test_r37_re2zombie.c)
target_link_libraries(test_r37_re2zombie PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_re2zombie PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r37_re2zombie PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r37_re2zombie PRIVATE m)
endif()
foreach(_teil prod1 prod2 prod3 b c nahkampf raum1130 raum1180)
    add_test(NAME unit_r37_re2zombie_${_teil} COMMAND test_r37_re2zombie ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r37_re2zombie_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
