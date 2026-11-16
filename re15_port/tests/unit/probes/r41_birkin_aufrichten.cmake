# Runde 41 "birkin_aufrichten" (Nutzerbefund 16.11.2026: Kraftraum-Birkin beugt sich nach dem Aufrichten noch einmal) - Riegel.
# Code enemy_ai_common.c re15_birkin_anim_szene (T-03/T-04 aus 9fe525fb2, woertlich) + Auftritt Sub 10; Belege im Testkopf.
#
#   unit_r41_birkin_aufrichten_kraftraum_leon  ROOM50E0 Sub 9: letztes Bild im Wechselbild, Clip 0xd mit a3 0x100
#   unit_r41_birkin_aufrichten_kraftraum_elza  ROOM50F1 dasselbe
#   unit_r41_birkin_aufrichten_s3070           ROOM3070 Sub 9
#   unit_r41_birkin_aufrichten_s3071           ROOM3071 Sub 10: Clipende 0xf und 0xd, a3 0x100
#   unit_r41_birkin_aufrichten_s3080           ROOM3080 Szenen-Modus: Stoss 1 haelt das letzte Bild, a3 0x400
add_executable(test_r41_birkin_aufrichten ${CMAKE_CURRENT_LIST_DIR}/../test_r41_birkin_aufrichten.c)
target_link_libraries(test_r41_birkin_aufrichten PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_birkin_aufrichten PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_birkin_aufrichten PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_birkin_aufrichten PRIVATE m)
endif()
foreach(_teil kraftraum_leon kraftraum_elza s3070 s3071 s3080)
    add_test(NAME unit_r41_birkin_aufrichten_${_teil} COMMAND test_r41_birkin_aufrichten ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_birkin_aufrichten_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
