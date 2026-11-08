# Runde 41 (2026-11-02), Spur ST80-T "Story-Texte umsetzen" — Riegel, zaehlen zur Suite.
# Dossier: analysis/story80/TEXTE_UMSETZUNG.md. Texte tools/r36_p/texte_bauen.py -> engine/src/gen/story_r36_texte.inc,
# Szenen tools/r41_story80/szenen_bauen.py -> engine/src/gen/story80_szenen.inc (engine/src/story80_r41.c).
#
#   unit_r41_story80_hebetisch1151  f.2: 1151 Fehldruck am Hebetisch, dann Irons -> "He has already passed..."
#   unit_r41_story80_texte          jede Zeile der Nutzerliste nach dem Laden: Wortlaut, <= 128 B, <= 3 Zeilen, <= 271 px
#   unit_r41_story80_ablauf30c1     30C1 (Elza/Marvin): genau msg 1..5, Stimmen 1..5, msg 6..8 nie, Abgang -> Cut 3
#   unit_r41_story80_ablauf6030     6030 Wagen 13 Zeilen + Sherrys Drehung, Fuehrerstand msg 11,12,25,26,13..19
#   unit_r41_story80_stimmen        Stimmen-Pfade main<Id>.wav ohne Umleitung (stimmen_r39 / elza_r36)
#   unit_r41_story80_reihenfolge6030  Nachbesserung 1: 6030 Fuehrerstand vor dem Wagen (msg 11, 31, 13..19 ohne Sherry),
#                                   Wagen, Nachtrag vorne (msg 40, 25), jede Zeile hoechstens einmal
#   unit_r41_story80_torweg6030     Integration A2: vorne -> Wagen -> ohne Rueckkehr -> Tor -> Fuehrerstand: Nachtrag 40, 25
#                                   vor dem AN-Zugstart (27..30, naechstes Betreten); Wagen zuerst: Zugstart sofort
# Echte exe: integration_r36_story_leon/_elza (tests/integration/test_r36_story.cmake, P4a/P4b/P4f/P4g).
add_executable(test_r41_story80 ${CMAKE_CURRENT_LIST_DIR}/../test_r41_story80.c)
target_link_libraries(test_r41_story80 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_story80 PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r41_story80 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_story80 PRIVATE m)
endif()
foreach(_teil hebetisch1151 texte ablauf30c1 ablauf6030 stimmen reihenfolge6030 torweg6030)
    add_test(NAME unit_r41_story80_${_teil} COMMAND test_r41_story80 ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_story80_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
