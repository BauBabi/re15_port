# Runde 42 "story" (v0.8.31), Stufe STAGE3a (Marvin) - Riegel. Konstanten/Belege include/re15_story3a_r42.h, Code
# engine/src/story3a_r42.c, Texte tools/r42_story/texte_stage3a.py -> engine/src/gen/story3a_r42_texte.inc. PORT-STORY nach dem Konzept
# (5 T-01/T-02, 3.2 L5-L7, L-05..L-10; F-04, F-05, F-06), Bits/Ids aus analysis/befunde_runde42/VERTRAG.md.
#
#   unit_r42_story3a_texte        3020 msg 0..2/7..9, 3060 msg 11..27 (Sprecher, Farben, Systemtexte 16/18)
#   unit_r42_story3a_programme    3020 sub02 + Zeilen 7/8 vor dem Wechsel, 3060 sub03 + Zeile 11 + Abblende, Wiedersehen je Marvin-Seite
#   unit_r42_story3a_tausch       T-01/T-02 Leon <-> Marvin (Seite, Platz, Waffe; Funde fuer die Uebergabe); Weste tauscht nicht
#   unit_r42_story3a_tausch_elza  Elza <-> Sherry unveraendert
#   unit_r42_story3a_speichern    Spielstand v10 als Marvin: laden, Neuladen ohne Schwanz, Rueckwechsel
#   unit_r42_story3a_hebung       v9 -> v10
#   unit_r42_story3a_wiedersehen  3060 Wiedersehen mit Uebergabe (Keycard, Fabrikplan, Redhawk, Magnum), Bit 124
#   unit_r42_story3a_karte3010    Kartentuer 3010 erst nach der Uebergabe (L-10 = T-01 + Kistenregel K7 @0x020AE)
#   integration_r42_story3a_*     echte exe (tests/integration/test_r42_story3a.cmake): kette = Durchlauf Leon 3000 .. 30E0 mit Marvin,
#                                 speichern = Speichern/Laden als Marvin. Elzas STAGE3 (3061/3091) pruefen die Runde-41-Laeufe
#                                 integration_r41_stage3szenen_a_3061 / _b_sherry / _b_3091 unveraendert.
add_executable(test_r42_story3a ${CMAKE_CURRENT_LIST_DIR}/../test_r42_story3a.c)
target_link_libraries(test_r42_story3a PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_story3a PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_story3a PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_story3a PRIVATE m)
endif()
foreach(_teil texte programme tausch tausch_elza speichern hebung wiedersehen karte3010)
    add_test(NAME unit_r42_story3a_${_teil} COMMAND test_r42_story3a ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_story3a_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_story3a.cmake)
    foreach(_teil kette speichern)
        add_test(NAME integration_r42_story3a_${_teil}
                 COMMAND ${CMAKE_COMMAND} -DRE15_PC_EXE=$<TARGET_FILE:re15_pc> -DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}/r42_story3a_${_teil}
                         -DTEIL=${_teil} -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_story3a.cmake)
        set_tests_properties(integration_r42_story3a_${_teil} PROPERTIES TIMEOUT 3000)
    endforeach()
endif()
