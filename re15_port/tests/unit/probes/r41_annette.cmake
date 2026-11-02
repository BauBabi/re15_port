# Runde 41 Spur AN: Annette-Szene ROOM5090 (include/re15_annette_r41.h, analysis/befunde_runde41/AN_annette.md).
# Eigene Datei (GLOB in tests/unit/CMakeLists.txt), damit die gemeinsame CMakeLists unberuehrt bleibt.
add_executable(test_r41_annette ${CMAKE_CURRENT_SOURCE_DIR}/test_r41_annette.c)
target_link_libraries(test_r41_annette PRIVATE re15_engine re15_test_support m)
target_include_directories(test_r41_annette PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_annette PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r41_annette COMMAND test_r41_annette)
set_tests_properties(unit_r41_annette PROPERTIES TIMEOUT 120)
if(TARGET re15_pc)
    # Echter Lauf (exe): Leon mit Tor offen (Szene, danach Kampf), Leon ohne Tor (keine Frau, kein Szenenstart), Elza.
    add_test(NAME integration_r41_annette
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_annette_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_annette.cmake)
    set_tests_properties(integration_r41_annette PROPERTIES TIMEOUT 600)
endif()
