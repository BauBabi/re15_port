# Runde 42 Strang lageplaene, Stufe 1 - STAGE1-Lageplaene in der RE2-Form (EIN Revierplan ROOM1030, EIN Kellerplan
# ROOM1200; die Etagenplaene 1140/10D0/11D0 sind entfernt). Belege: include/re15_kartenitems_r36.h (RUNDE 42).
#   unit_r42_lageplaene_tabelle / _revier / _keller / _weg / _texte / _alt   (tests/unit/test_r42_lageplaene.c)
# Gegenprobe: auf aaea5f8c2 (fuenf Etagenplaene) sind alle sechs Teile rot.
add_executable(test_r42_lageplaene ${CMAKE_CURRENT_LIST_DIR}/../test_r42_lageplaene.c)
target_link_libraries(test_r42_lageplaene PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_lageplaene PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_lageplaene PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_lageplaene PRIVATE m)
endif()
foreach(_teil tabelle revier keller weg texte alt)
    add_test(NAME unit_r42_lageplaene_${_teil} COMMAND test_r42_lageplaene ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_lageplaene_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
