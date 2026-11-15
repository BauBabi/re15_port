# Runde 42 "story" (v0.8.31), Stufe STAGE1 - Riegel. Konstanten/Belege include/re15_story1_r42.h, Code engine/src/story1_r42.c,
# Texte tools/r42_story/texte_stage1.py -> engine/src/gen/story1_r42_texte.inc. PORT-STORY nach dem Konzept (3.2 L3, 4.2 E2, 6b;
# K-07, K-08, F-26), Bits/Ids aus analysis/befunde_runde42/VERTRAG.md.
#
#   unit_r42_story1_texte        11A0 msg 3..6 / 11A1 msg 3..10: Untertitel mit Sprecherkopf und Textdauer
#   unit_r42_story1_programme    Programme: Zeilen in Reihenfolge (Leon 3..6, Elza 3,4,5,10, Elza mit Uhr 3..10), Set(9,118/120,1)
#   unit_r42_story1_szene(_elza, _elza_uhr)  Harness: Spieler in der Ausloesezone -> Szene, Begleiter kommen, Abblende, Begleiter weg,
#                                Bit 120; Uhr nur mit Uhr aus dem Inventar; zweites Betreten: keine Szene; Mod-Begleiter nie da;
#                                Nachbesserung 1: Abgang sichtbar (K, B, Schwarz >= 30, Kamera 2, oben bis Z/ZS, weg erst in
#                                Kamera 3), Elza kniet zur Uebergabe
#   unit_r42_story1_altstand     4:198 = 1 (Szene 3000/3001 gelaufen, sub00 @0x01188): keine Aufhol-Szene (Pruefer B5)
#   integration_r42_story1_*     echte exe (tests/integration/test_r42_story1.cmake): Weg 207x -> 11Ax -> Kante -> Szene -> 300x
add_executable(test_r42_story1 ${CMAKE_CURRENT_LIST_DIR}/../test_r42_story1.c)
target_link_libraries(test_r42_story1 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_story1 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_story1 PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_story1 PRIVATE m)
endif()
foreach(_teil texte programme szene szene_elza szene_elza_uhr altstand)
    add_test(NAME unit_r42_story1_${_teil} COMMAND test_r42_story1 ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_story1_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_story1.cmake)
    foreach(_teil leiter leiter_elza leiter_elza_ohne)
        add_test(NAME integration_r42_story1_${_teil}
                 COMMAND ${CMAKE_COMMAND} -DRE15_PC_EXE=$<TARGET_FILE:re15_pc> -DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}/r42_story1_${_teil}
                         -DTEIL=${_teil} -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_story1.cmake)
        set_tests_properties(integration_r42_story1_${_teil} PROPERTIES TIMEOUT 1300)
    endforeach()
endif()
