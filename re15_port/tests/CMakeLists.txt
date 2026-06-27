# =============================================================================
# Tests — CTest-Integration für RE 1.5 Port
# =============================================================================
# Läuft nur auf PC (kein GPU/PSX-Devkit nötig).
# enable_testing() wird bereits im Root-CMakeLists.txt aufgerufen.
# =============================================================================

# Test-Runner Executable
add_executable(re15_tests test_main.c)
target_link_libraries(re15_tests PRIVATE re15_engine)
target_include_directories(re15_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# CTest registrieren
add_test(NAME re15_tests COMMAND re15_tests)

# 30-Sekunden-Timeout pro Test (Requirement 12.7)
set_tests_properties(re15_tests PROPERTIES TIMEOUT 30)

# =============================================================================
# Unterverzeichnisse (wenn CMakeLists.txt dort existiert)
# =============================================================================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/property/CMakeLists.txt")
    add_subdirectory(property)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/unit/CMakeLists.txt")
    add_subdirectory(unit)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/integration/CMakeLists.txt")
    add_subdirectory(integration)
endif()
