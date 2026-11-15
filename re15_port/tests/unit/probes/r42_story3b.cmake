# Runde 42 "story" (v0.8.31), Stufe STAGE3b (30E0, 3070, 3080) - Riegel. Konstanten/Belege include/re15_story3b_r42.h, Code
# engine/src/story3b_r42.c, Texte tools/r42_story/texte_stage3b.py -> engine/src/gen/story3b_r42_texte.inc, Birkin-Clipende
# enemy_ai_common.c re15_birkin_anim_szene. PORT-STORY nach dem Konzept (3.2 L8-L10, L-11/L-12, F-08; 5 T-03/T-04, F-09).
#
#   unit_r42_story3b_texte      30E0 msg 10/11/13/16/17, 3070 msg 6 (Sprecher, Farben, Wortlaut nach dem Raumaufbau)
#   unit_r42_story3b_programm   ein Besuch aus Original-Bytes sub02/sub03 + Uebergang + Warten; Altstand; 3070-Zeile
#   unit_r42_story3b_besuch     30E0 im Spiel: Zeilenfolge, Marvin tritt ein, Flags, Ende erst nach Untertitel UND Stimme
#   unit_r42_story3b_altstand   (3,75)=1: sub03 mit Warten
#   unit_r42_story3b_elza       30E1 unveraendert
#   unit_r42_story3b_zeile3070  3070 msg 6 einmal nach (4,194), Elza 3071 ohne
#   unit_r42_story3b_t03        3070-Auftritt: letztes Bild im Wechselbild, Clip 0xd mit a3 0x100
#   unit_r42_story3b_t04        3080-Hand: zweiter Stoss aus dem gehaltenen Bild, a3 0x400
#   integration_r42_story3b_leon  Durchlauf Leon 3060 -> 3010 -> 30E0 -> 3070 -> 30E0 -> 3070 -> 3080 -> 4000 (tests/integration/test_r42_story3b.cmake)
#   integration_r42_story3b_elza  Durchlauf Elza 30E1 -> 3071 -> 30E1 -> 3071 -> 3081 -> 4001 (keine Port-Szene, T-04)
add_executable(test_r42_story3b ${CMAKE_CURRENT_LIST_DIR}/../test_r42_story3b.c)
target_link_libraries(test_r42_story3b PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_story3b PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_story3b PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_story3b PRIVATE m)
endif()
foreach(_teil texte programm besuch altstand elza zeile3070 t03 t04)
    add_test(NAME unit_r42_story3b_${_teil} COMMAND test_r42_story3b ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_story3b_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_story3b.cmake)
    foreach(_teil leon elza)
        add_test(NAME integration_r42_story3b_${_teil}
                 COMMAND ${CMAKE_COMMAND} -DRE15_PC_EXE=$<TARGET_FILE:re15_pc> -DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}/r42_story3b_${_teil}
                         -DTEIL=${_teil} -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_story3b.cmake)
        set_tests_properties(integration_r42_story3b_${_teil} PROPERTIES TIMEOUT 2600)
    endforeach()
endif()
