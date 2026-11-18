# Runde 42 Strang "finale", Stufe 2 "Abfahrt" (v0.8.31): Zugfilm im Zugstart (Leon ROOM6030 / Elza ROOM6031), WARNING beim
# Krachen, Loch in Wagen B (ROOM509 Bild 10). Herleitung include/re15_zws_r41.h ("Zugfilm Raum 600"), include/re15_zug_r42.h,
# engine/src/annette_r41.c (z_film, z_warnung), tools/r42_story/szenen_stage6.py; Film tools/r41_zws114/film_bauen.py --film 600.
#
#   unit_r42_zug_datei    R600ZUG.STR: 405 Bilder 320x240 mit den Port-Dekodern, CD-XA = Filmlaenge, Ton steigt ab Film-Bild 18
#   unit_r42_zug_tabelle  Filmtabelle zug600/zug601 an Port-Zeile 60, parkt bis Filmende, Einblenden wie 114
#   unit_r42_zug_leon     ROOM6030 Zugstart: Pultlicht -> Film -> Cut 1 -> msg 27; kein Zug-an-Signal; Krachen -> WARNING @0x01224
#   unit_r42_zug_elza     ROOM6031 Zugstart: dasselbe mit @0x00FF0, msg 14 / 15
#   unit_r42_zug_musik    Stufe 2 Runde 2: vor dem Film MAIN (Slot 0) Sce_bgm_control Op 4 (SsSeqPause @0x80044efc), danach Op 3 (SsSeqReplay @0x80044ec4)
#   unit_r42_zug_loch     5090/5091 nach dem Zugstart Cut_replace(8,10): Zonen 7->10 / 9->10; vorher Original
add_executable(test_r42_zug ${CMAKE_CURRENT_LIST_DIR}/../test_r42_zug.c)
target_link_libraries(test_r42_zug PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_zug PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_zug PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_zug PRIVATE m)
endif()
foreach(_teil datei tabelle leon elza musik loch)
    add_test(NAME unit_r42_zug_${_teil} COMMAND test_r42_zug ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_zug_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
#   integration_r42_zug_loch_leon / _elza  echte exe: Tuer Wagen A -> B (derselbe Raum) tauscht genau einmal, Kamera 10 im Lochbereich
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_zug_loch.cmake)
    foreach(_teil leon elza)
        add_test(NAME integration_r42_zug_loch_${_teil}
                 COMMAND ${CMAKE_COMMAND} -DRE15_PC_EXE=$<TARGET_FILE:re15_pc> -DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}/r42_zug_loch_${_teil}
                         -DTEIL=${_teil} -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_zug_loch.cmake)
        set_tests_properties(integration_r42_zug_loch_${_teil} PROPERTIES TIMEOUT 600)
    endforeach()
endif()
