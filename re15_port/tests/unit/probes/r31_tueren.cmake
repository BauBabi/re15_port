# Runde 31 - Tueren (analysis/befunde_runde31/tueren_04_bau.md).
# GEPINNT -> add_test je Teil von probe_r31_tueren:
#   viereck  40-B-Tuersaetze ROOM4030/4031 (Punkte pc+6..21, Nutzlast pc+22, FUN_80014368),
#            Installation + echter Durchgang im Spielschritt nach ROOM4040 / ROOM4080.
add_executable(probe_r31_tueren probe_r31_tueren.c)
target_link_libraries(probe_r31_tueren PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r31_tueren PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r31_tueren PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_r31_viereck COMMAND probe_r31_tueren viereck)
set_tests_properties(unit_r31_viereck PROPERTIES TIMEOUT 120)
