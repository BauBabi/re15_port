# Runde 24: MESSUNG "Abfrage ohne Besitz" je Benutzungsstelle im ausgelieferten
# Unterprogramm. Reine Sonde, kein add_test.
add_executable(probe_besitz_zensus ${CMAKE_CURRENT_SOURCE_DIR}/probe_besitz_zensus.c)
target_link_libraries(probe_besitz_zensus PRIVATE re15_engine re15_test_support)
target_include_directories(probe_besitz_zensus PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(probe_besitz_zensus PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
