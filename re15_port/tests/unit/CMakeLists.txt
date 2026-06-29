# =============================================================================
# Unit-Tests — Einzelne Parser/Module isoliert testen
# =============================================================================
# Jede Testdatei wird als eigenständiges Executable gebaut und bei CTest
# registriert. Alle Tests linken gegen re15_engine (statische Lib).
# =============================================================================

# --- RDT-Parser Unit-Test (Requirement 10.1, 10.8, 12.1) ---
add_executable(test_rdt_parser test_rdt_parser.c)
target_link_libraries(test_rdt_parser PRIVATE re15_engine re15_test_support)
target_include_directories(test_rdt_parser PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_rdt_parser COMMAND test_rdt_parser)
set_tests_properties(unit_rdt_parser PROPERTIES TIMEOUT 30)

# --- SCA-Parser Unit-Test ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test_sca_parse.c")
    add_executable(test_sca_parser test_sca_parse.c)
    target_link_libraries(test_sca_parser PRIVATE re15_engine re15_test_support)
    target_include_directories(test_sca_parser PRIVATE ${CMAKE_SOURCE_DIR}/include)
    add_test(NAME unit_sca_parser COMMAND test_sca_parser)
    set_tests_properties(unit_sca_parser PROPERTIES TIMEOUT 30)
endif()

# --- SCD-Opcodes Unit-Test (Requirement 4.6, 12.4) ---
add_executable(test_scd_opcodes test_scd_opcodes.c)
target_link_libraries(test_scd_opcodes PRIVATE re15_engine re15_test_support)
target_include_directories(test_scd_opcodes PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_scd_opcodes COMMAND test_scd_opcodes)
set_tests_properties(unit_scd_opcodes PROPERTIES TIMEOUT 30)

# --- Fixkomma-Mathematik Unit-Test (Requirement 5.3, 11.2) ---
add_executable(test_math_common test_math_common.c)
target_link_libraries(test_math_common PRIVATE re15_engine re15_test_support)
target_include_directories(test_math_common PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_math_common PRIVATE m)
add_test(NAME unit_math_common COMMAND test_math_common)
set_tests_properties(unit_math_common PROPERTIES TIMEOUT 30)

# --- Room-Load Unit-Test (Requirement 10.1, 12.2) ---
add_executable(test_room_load test_room_load.c)
target_link_libraries(test_room_load PRIVATE re15_engine re15_test_support)
target_include_directories(test_room_load PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_room_load COMMAND test_room_load)
set_tests_properties(unit_room_load PROPERTIES TIMEOUT 30)

# --- Member_set/cmp Unit-Test (#11/#12 — byte-true RE1.5 id-Tabelle FUN_8004116c/FUN_80041358) ---
add_executable(test_member test_member.c)
target_link_libraries(test_member PRIVATE re15_engine re15_test_support)
target_include_directories(test_member PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_member COMMAND test_member)
set_tests_properties(unit_member PROPERTIES TIMEOUT 30)

# --- Player-Damage Unit-Test (#13 — byte-true Player-Branch von FUN_80012d60) ---
add_executable(test_damage test_damage.c)
target_link_libraries(test_damage PRIVATE re15_engine re15_test_support)
target_include_directories(test_damage PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_damage COMMAND test_damage)
set_tests_properties(unit_damage PROPERTIES TIMEOUT 30)

# --- Enemy-AI-FSM Unit-Test (Phase 2 — byte-true FUN_8011d6d4/d84c/d9f4 + 80101b64/c7c/de4) ---
add_executable(test_enemy_ai test_enemy_ai.c)
target_link_libraries(test_enemy_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_enemy_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_enemy_ai COMMAND test_enemy_ai)
set_tests_properties(unit_enemy_ai PROPERTIES TIMEOUT 30)

# --- Audio VAB-Parsing Unit-Test (Requirement 6.6, 12.1) ---
add_executable(test_audio_vab test_audio_vab.c)
target_include_directories(test_audio_vab PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_audio_vab COMMAND test_audio_vab)
set_tests_properties(unit_audio_vab PROPERTIES TIMEOUT 30)
