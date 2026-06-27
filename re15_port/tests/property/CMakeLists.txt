# =============================================================================
# Property-Tests — Exhaustive Korrektheitstests für RE 1.5 Port
# =============================================================================
# Jeder Property-Test ist ein eigenständiges Executable, registriert via CTest.
# =============================================================================

# --- Property 1: RDT-Dateiname-Konstruktion (Requirement 2.3) ---
add_executable(prop_rdt_filename prop_rdt_filename.c)
target_link_libraries(prop_rdt_filename PRIVATE re15_engine)
target_include_directories(prop_rdt_filename PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_rdt_filename COMMAND prop_rdt_filename)
set_tests_properties(prop_rdt_filename PROPERTIES TIMEOUT 30)

# --- Property 15: RDT Null-Offset-Sektionsbehandlung (Requirement 10.8) ---
add_executable(prop_rdt_null_offset prop_rdt_null_offset.c)
target_link_libraries(prop_rdt_null_offset PRIVATE re15_engine)
target_include_directories(prop_rdt_null_offset PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_rdt_null_offset COMMAND prop_rdt_null_offset)
set_tests_properties(prop_rdt_null_offset PROPERTIES TIMEOUT 30)

# --- Property 6: SCD-Opcode PC-Advancement (Requirement 4.6) ---
add_executable(prop_scd_pc_advance prop_scd_pc_advance.c)
target_link_libraries(prop_scd_pc_advance PRIVATE re15_engine)
target_include_directories(prop_scd_pc_advance PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_scd_pc_advance COMMAND prop_scd_pc_advance)
set_tests_properties(prop_scd_pc_advance PROPERTIES TIMEOUT 30)

# --- Property 3: Door_aot_set_4p Parse-Roundtrip (Requirements 3.1, 3.2, 4.1) ---
add_executable(prop_door_roundtrip prop_door_roundtrip.c)
target_link_libraries(prop_door_roundtrip PRIVATE re15_engine)
target_include_directories(prop_door_roundtrip PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_door_roundtrip COMMAND prop_door_roundtrip)
set_tests_properties(prop_door_roundtrip PROPERTIES TIMEOUT 30)

# --- Property 8: Fixkomma-Rendering-Präzision (Requirement 5.3) ---
add_executable(prop_fixpoint_precision prop_fixpoint_precision.c)
target_link_libraries(prop_fixpoint_precision PRIVATE re15_engine m)
target_include_directories(prop_fixpoint_precision PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_fixpoint_precision COMMAND prop_fixpoint_precision)
set_tests_properties(prop_fixpoint_precision PROPERTIES TIMEOUT 30)

# --- Property 12: Tank-Controls Zustandsupdate (Requirements 11.2, 11.4) ---
add_executable(prop_tank_controls prop_tank_controls.c)
target_link_libraries(prop_tank_controls PRIVATE re15_engine)
target_include_directories(prop_tank_controls PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_tank_controls COMMAND prop_tank_controls)
set_tests_properties(prop_tank_controls PROPERTIES TIMEOUT 30)

# --- Property 11: Input-Edge-Detection (Requirement 7.1) ---
add_executable(prop_input_edge prop_input_edge.c)
target_include_directories(prop_input_edge PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_input_edge COMMAND prop_input_edge)
set_tests_properties(prop_input_edge PROPERTIES TIMEOUT 30)

# --- Property 13: SCA-Kollisionserkennung (Requirement 11.3) ---
add_executable(prop_sca_collision prop_sca_collision.c)
target_link_libraries(prop_sca_collision PRIVATE re15_engine)
target_include_directories(prop_sca_collision PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_sca_collision COMMAND prop_sca_collision)
set_tests_properties(prop_sca_collision PROPERTIES TIMEOUT 30)

# --- Property 5: Point-in-4P-Polygon Korrektheit (Requirements 4.2, 11.5, 11.6) ---
add_executable(prop_point_in_poly prop_point_in_poly.c)
target_link_libraries(prop_point_in_poly PRIVATE re15_engine)
target_include_directories(prop_point_in_poly PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_point_in_poly COMMAND prop_point_in_poly)
set_tests_properties(prop_point_in_poly PROPERTIES TIMEOUT 30)

# --- Property 14: Zone-Containment → Kamera/Aktion-Dispatch (Requirements 11.5, 11.6) ---
add_executable(prop_zone_dispatch prop_zone_dispatch.c)
target_link_libraries(prop_zone_dispatch PRIVATE re15_engine)
target_include_directories(prop_zone_dispatch PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_zone_dispatch COMMAND prop_zone_dispatch)
set_tests_properties(prop_zone_dispatch PROPERTIES TIMEOUT 30)

# --- Property 16: BSS-Chunk-Selektion nach Kamera-ID (Requirement 10.5) ---
add_executable(prop_bss_chunk prop_bss_chunk.c)
target_link_libraries(prop_bss_chunk PRIVATE re15_engine)
target_include_directories(prop_bss_chunk PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_bss_chunk COMMAND prop_bss_chunk)
set_tests_properties(prop_bss_chunk PROPERTIES TIMEOUT 30)

# --- Property 7: Spawn-Position Tür-Entered-Invariante (Requirement 4.8) ---
add_executable(prop_spawn_entered prop_spawn_entered.c)
target_link_libraries(prop_spawn_entered PRIVATE re15_engine)
target_include_directories(prop_spawn_entered PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_spawn_entered COMMAND prop_spawn_entered)
set_tests_properties(prop_spawn_entered PROPERTIES TIMEOUT 30)

# --- Property 9: OT Z-Sortierung (Requirement 5.5) ---
add_executable(prop_ot_sort prop_ot_sort.c)
target_include_directories(prop_ot_sort PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_ot_sort COMMAND prop_ot_sort)
set_tests_properties(prop_ot_sort PROPERTIES TIMEOUT 30)

# --- Property 10: ADPCM-Dekodierung (Requirement 6.3) ---
add_executable(prop_adpcm_decode prop_adpcm_decode.c)
target_include_directories(prop_adpcm_decode PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_adpcm_decode COMMAND prop_adpcm_decode)
set_tests_properties(prop_adpcm_decode PROPERTIES TIMEOUT 30)

# --- Property 4: Strukturdaten JSON-Roundtrip (Requirements 3.4, 8.5) ---
add_executable(prop_json_roundtrip prop_json_roundtrip.c)
target_include_directories(prop_json_roundtrip PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_json_roundtrip COMMAND prop_json_roundtrip)
set_tests_properties(prop_json_roundtrip PROPERTIES TIMEOUT 30)

# --- Property 17: Savestate-Vergleichstoleranz (Requirement 12.3) ---
add_executable(prop_savestate_cmp prop_savestate_cmp.c)
target_link_libraries(prop_savestate_cmp PRIVATE re15_engine)
target_include_directories(prop_savestate_cmp PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME prop_savestate_cmp COMMAND prop_savestate_cmp)
set_tests_properties(prop_savestate_cmp PROPERTIES TIMEOUT 30)
