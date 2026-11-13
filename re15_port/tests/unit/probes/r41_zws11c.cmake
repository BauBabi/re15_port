# Runde 41 (2026-11-13), Zwischensequenz Raum 11C (ROOM11C0, Leon und Ada) — Riegel, zaehlen zur Suite.
# Herleitung include/re15_zws_r41.h (Tabelle mit Raum 114), Code engine/src/zws_r41.c + platform/pc/src/zws_r41_pc.c, Haken
# scd_vm.c (Dispatcher-Anker + Raumladen), Film shared_assets/PSX/MOVIE/R11CGORI.STR (tools/r41_zws114/film_bauen.py --film 11C).
#
#   unit_r41_zws11c_datei   R11CGORI.STR: Form wie CAPCOM.STR, 362 Bilder 320x240 mit den Port-Dekodern, CD-XA-Ton = Filmlaenge
#   unit_r41_zws11c_anker   Tabelle 114/11C; ROOM11C0-Bytes an @0x01920 / @0x01982 (+ msg 4 @0x01928, msg 5 @0x01A5E);
#                           armiert nur in ROOM11C0, nicht bei geaendertem Byte, nicht in 11C1/1141/11B0
#   unit_r41_zws11c_ablauf  ROOM11C0 am VM: msg 3 -> Schrei -> Tonlaenge + Sleep 20 -> msg 4 "What!?" -> Drehen -> Film ->
#                           Musik + Kamera 12 -> msg 5 "Ada, you hide ..."; harter Schnitt, danach Einblenden 240..0
#   unit_r41_zws11c_elza    ROOM11C1 (Elza): kein Nachrichtenblock, keine Anker, kein Schrei/Film
add_executable(test_r41_zws11c ${CMAKE_CURRENT_LIST_DIR}/../test_r41_zws11c.c)
target_link_libraries(test_r41_zws11c PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_zws11c PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_zws11c PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_zws11c PRIVATE m)
endif()
foreach(_teil datei anker ablauf elza)
    add_test(NAME unit_r41_zws11c_${_teil} COMMAND test_r41_zws11c ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_zws11c_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
