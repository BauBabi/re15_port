# Runde 18 — ROOM1150 "Modell in der Mitte": was ist es, und wie loest man die Animation aus?
add_executable(probe_irons_mittelmodell ${CMAKE_CURRENT_SOURCE_DIR}/probe_irons_mittelmodell.c)
target_link_libraries(probe_irons_mittelmodell PRIVATE re15_engine re15_test_support)
target_include_directories(probe_irons_mittelmodell PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_irons_mittelmodell PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_irons_mittelmodell COMMAND probe_irons_mittelmodell)
set_tests_properties(unit_irons_mittelmodell PROPERTIES TIMEOUT 60)
