# =============================================================================
# Tests — CTest-Integration für RE 1.5 Port
# =============================================================================
# Läuft nur auf PC (kein GPU/PSX-Devkit nötig).
# enable_testing() wird bereits im Root-CMakeLists.txt aufgerufen.
# =============================================================================

# Test-Support: Host-/Plattform-Symbole, die die Engine-Lib referenziert, aber
# Test-Executables (nur re15_engine gelinkt) sonst nicht bereitstellen
# (re15_sin_q12/cos_q12 via echtem skeleton_trig_pc.c + g_engine/debug_text/bgm-Stubs).
# PUBLIC m => libm (sinf/cosf) propagiert transitiv an jeden Test, der dies linkt.
add_library(re15_test_support STATIC
    test_support.c
    ${CMAKE_SOURCE_DIR}/platform/pc/src/skeleton_trig_pc.c)
target_include_directories(re15_test_support PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(re15_test_support PUBLIC m)

# Test-Runner Executable
add_executable(re15_tests test_main.c)
target_link_libraries(re15_tests PRIVATE re15_engine re15_test_support)
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
