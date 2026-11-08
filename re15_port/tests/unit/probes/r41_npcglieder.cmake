# Integration Runde 41 (Nutzer-Marke 2026-11-08 ROOM6031 "gestreckte Sherry", Story-Liste ROOM6030 Annette):
# Gliederzusammenhang eines NPC im Sub-0-Kanal nach einem Typwechsel im selben Slot ohne Raumladen.
# Code engine/src/enemy_common.c rbj_resolve_slot, Dossier analysis/befunde_runde41/INT_belege/sherry6031/BEFUND.md.
add_executable(test_r41_npcglieder ${CMAKE_CURRENT_LIST_DIR}/../test_r41_npcglieder.c)
target_link_libraries(test_r41_npcglieder PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_npcglieder PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_npcglieder PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_npcglieder PRIVATE m)
endif()
foreach(_teil sherry6031 annette6030)
    add_test(NAME unit_r41_npcglieder_${_teil} COMMAND test_r41_npcglieder ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_npcglieder_${_teil} PROPERTIES TIMEOUT 120)
endforeach()
