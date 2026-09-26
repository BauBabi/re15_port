# Runde 27: RIEGEL fuer den FAHRSTUHL-FAHRTON (RE2-ERGAENZUNG).
# Laedt die Mini-Bank shared_assets/RE2/ELEVSE.VBS, prueft die zwei aus RE2
# geschnittenen Saetze (se 0x11/0x12, pitch 761/861) und weist nach, dass beim
# echten Fahrt-Ereignis im echten SCD-VM genau diese beiden Ids ausgeloest werden.
add_executable(test_elev_se ${CMAKE_CURRENT_SOURCE_DIR}/test_elev_se.c)
target_link_libraries(test_elev_se PRIVATE re15_engine re15_test_support)
target_include_directories(test_elev_se PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_elev_se PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_elev_se COMMAND test_elev_se)
set_tests_properties(unit_elev_se PROPERTIES TIMEOUT 60)
