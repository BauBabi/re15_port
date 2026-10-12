# Runde 37 Spur G — der 3-Schuss-Feuerstoss der Beretta M93R (5) / Glock 18 (6) trifft JE KUGEL
# (Nutzer-Nachtrag 2026-10-12: "Werden sie nur ein einziges mal getroffen bei 3 Schuss, ist es nur Schussverschwendung").
#
# Dossier: analysis/befunde_runde37/G_beretta.md; Code: engine/src/beretta_r37.c, include/re15_beretta_r37.h,
# Haken in game_step_common.c (Eintrittsbild + Tick) und re15_damage.c (Schaden je Kugel = Spalte 3).
#
#   unit_r37_beretta          Mechanik ohne exe (echte ROOM1140.RDT): Kugel 2/3 in Tick 3/6, drei Treffer auf
#                             denselben Zombie trotz RE1.5-Riegel +0x93 (Negativkontrolle), Schaden je Kugel
#                             Spalte 3 (RE1.5-Zeile 5 statt 15, Gorilla 12 statt 28), Patronen je Kugel, Abbruch,
#                             Sterbender bekommt keine zweite Kugel, andere Waffen unberuehrt.
add_executable(test_r37_beretta ${CMAKE_CURRENT_LIST_DIR}/../test_r37_beretta.c)
target_link_libraries(test_r37_beretta PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_beretta PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(test_r37_beretta PRIVATE m)
endif()
add_test(NAME unit_r37_beretta COMMAND test_r37_beretta)
set_tests_properties(unit_r37_beretta PROPERTIES TIMEOUT 120)

#   integration_r37_beretta   echte exe (Messkopie re15_pc_r37g_haken_*): ROOM1140, EIN Abzug auf Platz 5 —
#                             Leon RE2-KI M93R / Glock 18 und RE1.5-KI M93R: DREI HP-Abzuege im Eintrittsbild,
#                             +3 und +6, Munition 15 -> 12; Gegenprobe Browning: EIN Abzug, 15 -> 14; Elza (Spielerwahl,
#                             ROOM11E1, RE2-KI) M93R: drei Treffer.
if(TARGET re15_pc)
    add_test(NAME integration_r37_beretta
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r37_beretta_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_beretta.cmake)
    set_tests_properties(integration_r37_beretta PROPERTIES TIMEOUT 900)
endif()
