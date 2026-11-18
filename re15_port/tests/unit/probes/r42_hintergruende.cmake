# Runde 42 Strang "rest" (v0.8.31), Stufe 2 "Kleine Hintergrund-Funde" - Riegel der nie gezeigten Kameras beim Untersuchen.
# Herleitung include/re15_hintergruende_r42.h, Code engine/src/hintergruende_r42.c; D4 (6031 Cut 11) in unit_r42_story6_wagen6031.
#
#   unit_r42_hintergruende_funk   10F0/10F1 Platz 3 (Nordwand) und 2 (Ostwand): msg 0 "... out of commission ..." mit Cut 10
#   unit_r42_hintergruende_regal  10F0/10F1 Platz 6: msg 2 (Regal) mit Cut 9
#   unit_r42_hintergruende_karte  1010/1011: Aufnahme-Frage der Memory Card mit Cut 7, Nein/Ja, danach stumm
#   unit_r42_hintergruende_feuer  1090 (Leon): nach "Ja" zur Loesch-Frage Cut 14 (Feuerschein) bis zur Selbst-Tuer (A3)
#   unit_r42_hintergruende_bytes  Programme gegen die Original-Saetze (ROOM10F0 @0x00F72/@0x00F86/@0x00FC2, ROOM1051 @0x00CB2..)
#   unit_r42_hintergruende_fremd  ROOM1020 unberuehrt
add_executable(test_r42_hintergruende ${CMAKE_CURRENT_LIST_DIR}/../test_r42_hintergruende.c)
target_link_libraries(test_r42_hintergruende PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_hintergruende PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_hintergruende PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_hintergruende PRIVATE m)
endif()
foreach(_teil funk regal karte feuer bytes fremd)
    add_test(NAME unit_r42_hintergruende_${_teil} COMMAND test_r42_hintergruende ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_hintergruende_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
