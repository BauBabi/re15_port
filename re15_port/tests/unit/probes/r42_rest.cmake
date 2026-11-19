# Runde 42 Strang "rest" (v0.8.31), Stufe 1 - Riegel der Restpunkte (Code/Belege im Kopf von tests/unit/test_r42_rest.c).
#
#   unit_r42_rest_msglaenge        (a) jede Nachricht jeder ausgelieferten RDT (+ Port-Texte) traegt ihr eigenes Ende; das Original
#                                      liest in-place (@0x800c004c: RDT+0x3C), der Port kopiert - keine Kopie darf abschneiden
#   unit_r42_rest_schreibmaschine  (a) ROOM2040 msg 0 (411 Byte) und ROOM6000 msg 49 (127 Byte) laufen bis an ihr 0x01
add_executable(test_r42_rest ${CMAKE_CURRENT_LIST_DIR}/../test_r42_rest.c)
target_link_libraries(test_r42_rest PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_rest PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_rest PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_rest PRIVATE m)
endif()
foreach(_teil msglaenge schreibmaschine)
    add_test(NAME unit_r42_rest_${_teil} COMMAND test_r42_rest ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_rest_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
