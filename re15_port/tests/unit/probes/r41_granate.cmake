# Runde 41 Spur GR "granate" — Nutzer (2026-11-02): "Und die Granate sollte laut den Videos der 80% Version die Zombies
# zerfetzen, also die normale, richtige Handgranate. Ausserdem ist sie noch zu schwach, die sollte auch die dicken Zombies
# zerfetzen". Regel: Gore = Maximum aus RE1.5, RE2 und 80%-Videos (PORT-WAHL, Einzelmechanismus mit @0x).
#
# Dossier: analysis/befunde_runde41/GR_granate.md; Code: re15_damage.c (Art 2 an der Zombie-Familie = 1000 @0x8006f41c),
# enemy_ai_re2_zombie.c re2z_granate_zerfetzen, zombiegore_r15.c z_granate.
#
#   unit_r41_granate       [1] Schaden 1000 an 0x10/0x11/0x12/0x13/0x16/0x18 (auch der dicke 0x11 mit 250 stirbt),
#                          Saeure/Brand und Nicht-Zombies unveraendert; [2] RE2-Gehirn (Nachbesserung 1, Video CD2 19,25 s
#                          + Nutzer-Entscheid "Teile bleiben liegen"): sieben Stuecke fliegen (Wurf 0x1062), liegen nach 150
#                          Bildern am Boden, Scheitel gegen die Zielhoehe, gestreut, alles weiter gezeichnet, Rauch-Nachlauf;
#                          [3] RE1.5-Gehirn z_granate (dieselben Stuecke); [5] Hund RE2/RE1.5: 7 Parts fliegen und liegen.
#                          [6] Nachbesserung 2 (Nutzerentscheid 2026-11-02): 1000 an allen normalen Gegnern unter jeder KI;
#                          Bosse je Form gemessen (echter Hitscan + echte Explosion): keine Form stirbt an einer Granate,
#                          Granate + 4 Pistolenschuesse = Gorilla, Granate + 8 = Birkin-Formen/Alligator; G5/Gator-Boss alt.
#   unit_r41_granate_aus   eigener Prozess mit RE15_KEIN_80PROZENT_GORE=1: keine Zerfetzung (RE2 0x80108BEC), Schaden 1000.
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
