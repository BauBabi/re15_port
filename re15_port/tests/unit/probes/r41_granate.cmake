# Runde 41 Spur GR "granate" — Nutzer (2026-11-02): "Und die Granate sollte laut den Videos der 80% Version die Zombies
# zerfetzen, also die normale, richtige Handgranate. Ausserdem ist sie noch zu schwach, die sollte auch die dicken Zombies
# zerfetzen". Regel: Gore = Maximum aus RE1.5, RE2 und 80%-Videos (PORT-WAHL, Einzelmechanismus mit @0x).
#
# Dossier: analysis/befunde_runde41/GR_granate.md + WS_waffenschaden.md; Code: re15_damage.c (Art 2 = D_G 251, Art 9 je Typ),
# enemy_ai_re2_zombie.c re2z_granate_zerfetzen, zombiegore_r15.c z_granate.
#
#   unit_r41_granate       [1] Schaden D_G an 0x10/0x11/0x12/0x13/0x16/0x18 (auch der dicke 0x11 mit 250 stirbt),
#                          Saeure/Brand und Nicht-Zombies unveraendert; [2] RE2-Gehirn (Nachbesserung 1, Video CD2 19,25 s
#                          + Nutzer-Entscheid "Teile bleiben liegen"): sieben Stuecke fliegen (Wurf 0x1062), liegen nach 150
#                          Bildern am Boden, Scheitel gegen die Zielhoehe, gestreut, alles weiter gezeichnet, Rauch-Nachlauf;
#                          [3] RE1.5-Gehirn z_granate (dieselben Stuecke); [5] Hund RE2/RE1.5: 7 Parts fliegen und liegen.
#                          [6] Bosse je Form gemessen (echter Hitscan, echte Explosion, echter Raketen-Applier): Granate
#                          == D_G, Granaten und Raketen bis zum Formende (Spur WS, Pruefblatt WS_ENTSCHEIDUNG.md Tabelle d).
#                          [8] Spur WS (Nutzerentscheid 2026-11-07 "mach wie resident evil 2 ... gerade so die Standard
#                          Gegner mit einer Granate"): D_G = 250 + 1 (@0x801008C8 + Todesregel) toetet jeden Standard-Typ,
#                          Dummy mit 251 HP lebt mit 0; Rakete je Typ = RE2 Zeile 17 K0; Gegenprobe alter Stand rot.
#   unit_r41_granate_ws    nur [8] (auch fuer den Vorher-Lauf gegen die alte Engine).
#   unit_r41_granate_aus   eigener Prozess mit RE15_KEIN_80PROZENT_GORE=1: keine Zerfetzung (RE2 0x80108BEC), Schaden D_G.
add_executable(test_r41_granate
    ${CMAKE_CURRENT_LIST_DIR}/../test_r41_granate.c)
target_link_libraries(test_r41_granate PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_granate PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/platform/pc/src)
target_compile_definitions(test_r41_granate PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_granate PRIVATE m)
endif()
add_test(NAME unit_r41_granate COMMAND test_r41_granate alle)
set_tests_properties(unit_r41_granate PROPERTIES TIMEOUT 300)
add_test(NAME unit_r41_granate_aus COMMAND test_r41_granate aus)
set_tests_properties(unit_r41_granate_aus PROPERTIES TIMEOUT 120)
add_test(NAME unit_r41_granate_ws COMMAND test_r41_granate ws)
set_tests_properties(unit_r41_granate_ws PROPERTIES TIMEOUT 120)
