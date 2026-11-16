# Runde 42 "story" (v0.8.31), Stufe STAGE6 (Zug 6030 Leon / 6031 Elza) - Riegel. Konstanten/Belege include/re15_story6_r42.h, Code
# engine/src/story6_r42.c, Programme tools/r42_story/szenen_stage6.py -> engine/src/gen/story6_r42_szenen.inc, Texte
# tools/r42_story/texte_stage6.py -> engine/src/gen/story6_r42_texte.inc, Leons Wagen tools/r41_story80/szenen_bauen.py, Masken-Tiefe
# tools/maske/liege_tiefe_r42.py (-> MASKS/ROOM6030.MSK, ROOM6031.MSK, tests/unit/gen_r42_liege_tiefe.inc).
# PORT-STORY nach dem Konzept (3.2 L21, 4.2 E6/E9, 5 T-05, F-10).
#
#   unit_r42_story6_texte       6030 msg 21/2/3/22/4/5, 6031 msg 0/1/2/12..17: Wortlaut, Farbe, Dauer, Seiten, Breite
#   unit_r42_story6_wagen6030   Leon: Wagen-Programm mit den neuen Saetzen in der Folge 0,1,21,2,3,22,4,5,6,7,23,24,8
#   unit_r42_story6_wagen6031   Elza: Wagen-Szene E6 0,1,2,12,13, danach Sperren frei
#   unit_r42_story6_vorher6031  vor dem Tor: Original, kein Zugstart
#   unit_r42_story6_tuer6031    nach dem Tor, vor dem Zugstart: Ausgang Tuer, Tuer nach 5091 msg 4; danach Original msg 11
#   unit_r42_story6_zug6031     Zugstart E9: 14..17, (9,130), Zug-an vor Krachen, Sherry/John, einmalig
#   unit_r42_story6_t05         Cut-9-Tiefen ueber der Liege, Marvins Kern frei
#   integration_r42_story6_leon / _elza  Durchlaeufe (tests/integration/test_r42_story6.cmake)
add_executable(test_r42_story6 ${CMAKE_CURRENT_LIST_DIR}/../test_r42_story6.c)
target_link_libraries(test_r42_story6 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_story6 PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src ${CMAKE_CURRENT_LIST_DIR}/..)
target_compile_definitions(test_r42_story6 PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_story6 PRIVATE m)
endif()
foreach(_teil texte wagen6030 wagen6031 vorher6031 tuer6031 zug6031 t05)
    add_test(NAME unit_r42_story6_${_teil} COMMAND test_r42_story6 ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_story6_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_story6.cmake)
    foreach(_teil leon elza)
        add_test(NAME integration_r42_story6_${_teil}
                 COMMAND ${CMAKE_COMMAND} -DRE15_PC_EXE=$<TARGET_FILE:re15_pc> -DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}/r42_story6_${_teil}
                         -DTEIL=${_teil} -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_story6.cmake)
        set_tests_properties(integration_r42_story6_${_teil} PROPERTIES TIMEOUT 3000)
    endforeach()
endif()
