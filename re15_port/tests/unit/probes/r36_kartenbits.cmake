# Runde 36 (2026-10-12), Nachtrag zu Spur N, Zweig r36/kartenbits — die Lageplan-Bits der Spur N lagen auf
# Bank-9-Bits ORIGINALER Item-Saetze (90 = ROOM2090/2091 Item 0x3F, 91 = ROOM2040/2041 Item 0x32).
# Dossier: analysis/befunde_runde36/N_kartenitems.md Abschnitt 9.
#
#   unit_r36_kartenbits   V  Plan ueber das echte Port-Programm (Ereignis 28, Ja) nehmen -> Item_aot_set in
#                            ROOM2040/2041 @0x01364 und ROOM2090/2091 @0x00A12 bleibt scharf (Taken-Test
#                            @0x800406dc); Gegenprobe Original-Bit -> still; Rueckrichtung kein Besitz.
add_executable(test_r36_kartenbits ${CMAKE_CURRENT_LIST_DIR}/../test_r36_kartenbits.c)
target_link_libraries(test_r36_kartenbits PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_kartenbits PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_kartenbits PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r36_kartenbits PRIVATE m)
endif()
add_test(NAME unit_r36_kartenbits COMMAND test_r36_kartenbits
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r36_kartenbits PROPERTIES TIMEOUT 300)
