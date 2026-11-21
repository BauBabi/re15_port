# Runde 42 Strang fabrikplan (20.11.2026, Konzept K-16): der Fabrikplan, den Marvin in 3060 uebergibt, muss vorher wirklich
# aufgenommen werden. NUTZERWUNSCH woertlich: "Im Story Zweig - room 3060 übergibt Marvin ein plan of the plant. Das macht
# natürlich nur sinn wenn er ihn auch wirklich irgendwo aufnimmt...". Belege: include/re15_kartenitems_r36.h (STRANG FABRIKPLAN).
#   unit_r42_fabrikplan_marvin / _mit / _ohne / _alt / _sicht   (tests/unit/test_r42_fabrikplan.c)
# Stufe 4 (21.11.2026): unit_r42_fabrikplan_d0 / _d0sicht - der Plan haengt auch in 30D0 (Halle nach der Lobby-Szene),
# solange weder Bit 35 noch Bit 139 steht; Gegenprobe auf 974e4b9b3 (Stand vor Stufe 4) rot.
# Gegenprobe: auf 9426267fa (r42/story, Plan immer uebergeben, keiner an der Wand in 30C0) sind alle fuenf Teile rot.
add_executable(test_r42_fabrikplan ${CMAKE_CURRENT_LIST_DIR}/../test_r42_fabrikplan.c)
target_link_libraries(test_r42_fabrikplan PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_fabrikplan PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_fabrikplan PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_fabrikplan PRIVATE m)
endif()
foreach(_teil marvin mit ohne alt sicht d0 d0sicht)
    add_test(NAME unit_r42_fabrikplan_${_teil} COMMAND test_r42_fabrikplan ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_fabrikplan_${_teil} PROPERTIES TIMEOUT 600)
endforeach()

#   integration_r42_fabrikplan_d0_bild   Stufe 4: echte exe, je Kamera von 30D0 mit dem Plakat ganz im Bild (1/2/6/8/10/11/13) Lauf mit
#                                        und ohne Bit 35 -> Plakat >= 90 % der projizierten Flaeche sichtbar (nicht verdeckt), sonst 0.
if(TARGET re15_pc)
    add_test(NAME integration_r42_fabrikplan_d0_bild
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r35_inhalt_karte>
                     -DRE15_BILD_TOOL=$<TARGET_FILE:probe_r34n_e_bild>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r42_fabrikplan_d0_bild_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_fabrikplan_d0_bild.cmake)
    set_tests_properties(integration_r42_fabrikplan_d0_bild PROPERTIES TIMEOUT 1500)
endif()
