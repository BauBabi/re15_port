# Runde 35 / Spur K — Neue Szene beim ERSTEN Betreten von ROOM10F0 (Ada/Leon/Marvin), danach Karte
# ROOM11C0 -> ROOM1150 und MAIN01 bis zum Parkplatz.
# Dossier: analysis/befunde_runde35/K_cut10f0.md, Konstanten: include/re15_cut10f0.h,
# Code: engine/src/cut_10f0.c (+ gen/cut10f0_szene.inc, gen/cut10f0_tuerton.inc).
#
#   test_r35_cut10f0        RIEGEL (Teil ueber das Argument):
#     unit_r35_cut10f0_programm  statischer Lauf ueber das Programm: Opcode-Laengen schliessen auf Evt_end,
#                                Set(9,71) als ERSTES, Message_on 6..23 streng aufsteigend, Se_on nur Bank 0x0E
#                                Satz 1, Ck nur Bank 5 Bits 0..2, Spawns Ada/Marvin in der 11B0-Form
#     unit_r35_cut10f0_texte     18 Nachrichten dekodieren zum Nutzer-Wortlaut, Sprecher/Farbe, Dauer gesetzt
#     unit_r35_cut10f0_tuerton   eingebackener Tonteil == shared_assets/RE2/DOOR/DOOR13.DO2[0..0x3DA8)
#     unit_r35_cut10f0_szene     echte VM + Spielschritt: Szene startet beim Raumaufbau, Nachrichtenfolge,
#                                Ada an den Monitoren, Marvin geparkt -> an der Tuer -> schraeg links, Leon
#                                links von Ada, Kamera 2 -> 0 -> 2 -> 0 -> 2 -> 0 -> 2, Tuerknall 2x, Balken,
#                                Abgang beider, Flags danach, Hinweis angefordert, MAIN01-Weiche aktiv
#     unit_r35_cut10f0_einmal    danach erneuter Raumaufbau: keine Szene, kein Spawn; Elza-Raum 10F1: nie
#     unit_r35_cut10f0_karte     Hinweiskette K1 (ROOM11C0, Blatt 0/Rechteck 4) -> K2 (ROOM1150, Blatt 4/
#                                Rechteck 2), beide Ziele in der normalen Karte bis zum Besuch; Runde-33-
#                                Eintrag unveraendert
#     unit_r35_cut10f0_bgm       Tabellenweiche: -1 vor der Szene, 0xFF01 danach fuer jeden STAGE1-Raum
#                                ausser 0x1C, -1 in anderen Stages, -1 sobald ROOM11C0 besucht ist
#   integration_r35_cut10f0     echte exe, Spielstand + CONTINUE in ROOM10D0, Aktionstaste an der Tuer,
#                               Szene im Zielraum genau einmal (test_r35_cut10f0.cmake)
add_executable(test_r35_cut10f0 ${CMAKE_CURRENT_LIST_DIR}/../test_r35_cut10f0.c)
target_link_libraries(test_r35_cut10f0 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_cut10f0 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r35_cut10f0 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r35_cut10f0 PRIVATE m)
endif()
foreach(_teil programm texte tuerton szene einmal karte bgm)
    add_test(NAME unit_r35_cut10f0_${_teil} COMMAND test_r35_cut10f0 ${_teil})
    set_tests_properties(unit_r35_cut10f0_${_teil} PROPERTIES TIMEOUT 240)
endforeach()

# MESS-WERKZEUG (kein add_test): Speicherkarte mit Stand in ROOM10D0 vor der Tuer zum Communication Room
#   probe_r35_cut10f0_karte <kartendatei> [gesehen]
add_executable(probe_r35_cut10f0_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r35_cut10f0_karte.c)
target_link_libraries(probe_r35_cut10f0_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r35_cut10f0_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(TARGET re15_pc AND TARGET probe_r35_cut10f0_karte)
    add_test(NAME integration_r35_cut10f0
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r35_cut10f0_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r35_cut10f0_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r35_cut10f0.cmake)
    set_tests_properties(integration_r35_cut10f0 PROPERTIES TIMEOUT 900)
endif()
