# Runde 41 (2026-11-02), Spur ST80-T "Story-Texte umsetzen" — Riegel, zaehlen zur Suite.
# Dossier: analysis/story80/TEXTE_UMSETZUNG.md. Texte tools/r36_p/texte_bauen.py -> engine/src/gen/story_r36_texte.inc,
# Szenen tools/r41_story80/szenen_bauen.py -> engine/src/gen/story80_szenen.inc (engine/src/story80_r41.c).
#
#   unit_r41_story80_hebetisch1151  f.2: 1151 Fehldruck am Hebetisch, dann Irons -> "He has already passed..."
add_executable(test_r41_story80 ${CMAKE_CURRENT_LIST_DIR}/../test_r41_story80.c)
target_link_libraries(test_r41_story80 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_story80 PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r41_story80 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_story80 PRIVATE m)
endif()
foreach(_teil hebetisch1151)
    add_test(NAME unit_r41_story80_${_teil} COMMAND test_r41_story80 ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_story80_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
