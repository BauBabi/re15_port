# Runde 41 Form-Werkzeug: Probemodell aus tools/form_emd.py durch die echten Parser des Ports.
add_executable(test_r41_form_emd test_r41_form_emd.c)
target_link_libraries(test_r41_form_emd PRIVATE re15_engine re15_test_support m)
target_include_directories(test_r41_form_emd PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_form_emd PRIVATE
    RE15_FORM_PROBE_DIR="${CMAKE_SOURCE_DIR}/tests/unit/form_probe"
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r41_form_emd COMMAND test_r41_form_emd)
set_tests_properties(unit_r41_form_emd PROPERTIES TIMEOUT 30)
