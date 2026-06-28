# =============================================================================
# Integration-Tests — Subsystem-übergreifende Tests
# =============================================================================
# Testen die Zusammenarbeit mehrerer Module (AOT + Door + Room + Stage).
# Linken gegen re15_engine (statische Lib).
# =============================================================================

# --- Raumwechsel Integration-Test (Requirement 4.2, 4.3, 4.4, 4.7) ---
add_executable(test_room_transition test_room_transition.c)
target_link_libraries(test_room_transition PRIVATE re15_engine re15_test_support)
target_include_directories(test_room_transition PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME integration_room_transition COMMAND test_room_transition)
set_tests_properties(integration_room_transition PROPERTIES TIMEOUT 30)

# --- Cross-Stage Integration-Test (Requirement 4.4, 12.2, 12.5) ---
add_executable(test_cross_stage test_cross_stage.c)
target_link_libraries(test_cross_stage PRIVATE re15_engine re15_test_support)
target_include_directories(test_cross_stage PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME integration_cross_stage COMMAND test_cross_stage)
set_tests_properties(integration_cross_stage PROPERTIES TIMEOUT 30)

# --- Headless Run Integration-Test (Requirement 4.4, 12.2, 12.5) ---
add_executable(test_headless_run test_headless_run.c)
target_link_libraries(test_headless_run PRIVATE re15_engine re15_test_support)
target_include_directories(test_headless_run PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME integration_headless_run COMMAND test_headless_run)
set_tests_properties(integration_headless_run PROPERTIES TIMEOUT 30)
