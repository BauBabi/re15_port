# Runde 42 Strang fabrikplan (20.11.2026, Konzept K-16): der Fabrikplan, den Marvin in 3060 uebergibt, muss vorher wirklich
# aufgenommen werden. NUTZERWUNSCH woertlich: "Im Story Zweig - room 3060 übergibt Marvin ein plan of the plant. Das macht
# natürlich nur sinn wenn er ihn auch wirklich irgendwo aufnimmt...". Belege: include/re15_kartenitems_r36.h (STRANG FABRIKPLAN).
#   unit_r42_fabrikplan_marvin / _mit / _ohne / _alt / _sicht   (tests/unit/test_r42_fabrikplan.c)
# Gegenprobe: auf 9426267fa (r42/story, Plan immer uebergeben, keiner an der Wand in 30C0) sind alle fuenf Teile rot.
add_executable(test_r42_fabrikplan ${CMAKE_CURRENT_LIST_DIR}/../test_r42_fabrikplan.c)
target_link_libraries(test_r42_fabrikplan PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_fabrikplan PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_fabrikplan PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_fabrikplan PRIVATE m)
endif()
foreach(_teil marvin mit ohne alt sicht)
    add_test(NAME unit_r42_fabrikplan_${_teil} COMMAND test_r42_fabrikplan ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_fabrikplan_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
