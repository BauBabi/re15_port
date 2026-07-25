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

# --- Room-Probe-Sweep (roadmap S1-1 / RL-3): load+tick all 40 Leon STAGE1 rooms ---
add_executable(test_room_probe_sweep test_room_probe_sweep.c)
target_link_libraries(test_room_probe_sweep PRIVATE re15_engine re15_test_support)
target_include_directories(test_room_probe_sweep PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME integration_room_probe_sweep COMMAND test_room_probe_sweep)
set_tests_properties(integration_room_probe_sweep PROPERTIES TIMEOUT 60)

# --- RVD camera-cut coverage (roadmap S1-1 / RL-5): validate every room's RVD table ---
add_executable(test_rvd_coverage test_rvd_coverage.c)
target_link_libraries(test_rvd_coverage PRIVATE re15_engine re15_test_support)
target_include_directories(test_rvd_coverage PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME integration_rvd_coverage COMMAND test_rvd_coverage)
set_tests_properties(integration_rvd_coverage PROPERTIES TIMEOUT 60)

# --- Door traversal (roadmap S1-1 / RL-6): walk ROOM1140 -> >=3 rooms via real doors ---
add_executable(test_door_traversal test_door_traversal.c)
target_link_libraries(test_door_traversal PRIVATE re15_engine re15_test_support)
target_include_directories(test_door_traversal PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME integration_door_traversal COMMAND test_door_traversal)
set_tests_properties(integration_door_traversal PROPERTIES TIMEOUT 60)

# --- Door lock (roadmap S1-4 / PROG-1): locked-door MESSAGE vs open-door DOOR transition ---
add_executable(test_door_lock test_door_lock.c)
target_link_libraries(test_door_lock PRIVATE re15_engine re15_test_support)
target_include_directories(test_door_lock PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME integration_door_lock COMMAND test_door_lock)
set_tests_properties(integration_door_lock PROPERTIES TIMEOUT 30)

# --- STAGE1 progression-flag census (roadmap S1-4 / PROG-3..5 foundation) ---
add_executable(test_flag_census test_flag_census.c)
target_link_libraries(test_flag_census PRIVATE re15_engine re15_test_support)
target_include_directories(test_flag_census PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME integration_flag_census COMMAND test_flag_census)
set_tests_properties(integration_flag_census PROPERTIES TIMEOUT 30)

# --- STAGE1 keycard door flip (roadmap S1-4 / PROG-4) ---
add_executable(test_keydoor test_keydoor.c)
target_link_libraries(test_keydoor PRIVATE re15_engine re15_test_support)
target_include_directories(test_keydoor PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME integration_keydoor COMMAND test_keydoor)
set_tests_properties(integration_keydoor PROPERTIES TIMEOUT 30)
