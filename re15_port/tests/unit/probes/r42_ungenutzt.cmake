# Runde 42 Strang "rest" (v0.8.31), Stufe 3 "Ungenutzte Inhalte STAGE2-6" - Riegel. Herleitung include/re15_ungenutzt_r42.h, Code
# engine/src/ungenutzt_r42.c, Texte tools/r42_story/texte_ungenutzt.py. U-07 in unit_r42_story2_loch_elza, U-03 in
# unit_r42_story6_wagen6031.
#
#   unit_r42_ungenutzt_rechner   5010/5011 (U-05): Disc #40 vor dem Speicher-Text, jedes Mal
#   unit_r42_ungenutzt_plan      4020/4021 (U-11): Wandplan gibt Blatt 8 (Bit 36) einmal, mit "You've taken the laboratory map."
#   unit_r42_ungenutzt_schalter  3091 (U-02): ohne Strom "There's a power supply." vor "I can't reach the breaker switch..."
#   unit_r42_ungenutzt_szene     3091 (U-01): Blick zur Hoftuer, D309 #21 (msg 33), #24 (msg 34), #25 (msg 36) in der Szene
#   unit_r42_ungenutzt_bytes     Programme gegen die Original-Saetze
#   unit_r42_ungenutzt_tuer401   Stufe 5 U-08: Elza 4001 Slot 1 -> 4011 (Disc @0x1012), Kiste/Rechner/Funde/Rueckweg,
#                                Leon 4000 Slot 1 unveraendert
add_executable(test_r42_ungenutzt ${CMAKE_CURRENT_LIST_DIR}/../test_r42_ungenutzt.c)
target_link_libraries(test_r42_ungenutzt PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_ungenutzt PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src ${CMAKE_CURRENT_LIST_DIR}/..)
target_compile_definitions(test_r42_ungenutzt PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_ungenutzt PRIVATE m)
endif()
foreach(_teil rechner plan schalter szene bytes tuer401)
    add_test(NAME unit_r42_ungenutzt_${_teil} COMMAND test_r42_ungenutzt ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_ungenutzt_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
# Runde 2 (Pruefung s3_r1 G3): die ausgelieferte neue 3091-Szene im echten Spiel (ohne Messhaken RE15_U42_ORIGINAL)
#   integration_r42_ungenutzt_3091  echte exe, Riegeltuer 3091 -> sub07: Blick zum Tonort, Zeilen 33/34/36, danach Lobby 30C1
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_ungenutzt_3091.cmake)
    add_test(NAME integration_r42_ungenutzt_3091
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r42_ungenutzt_3091_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_ungenutzt_3091.cmake)
    set_tests_properties(integration_r42_ungenutzt_3091 PROPERTIES TIMEOUT 1200)
endif()
# Stufe 5 (U-08, NUTZERWUNSCH 18.11.): Elza 4001 -> 401 -> Kiste/Rechner/Funde -> zurueck, durchgehend mit der echten exe
#   integration_r42_ungenutzt_401  echte exe, Tuer 4001 Slot 1 (Disc @0x1012), Kiste msg 6, Rechner msg 7, Bits (9,11)/(9,12), 4011 -> 4001
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_ungenutzt_401.cmake)
    add_test(NAME integration_r42_ungenutzt_401
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r42_ungenutzt_401_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_ungenutzt_401.cmake)
    set_tests_properties(integration_r42_ungenutzt_401 PROPERTIES TIMEOUT 1200)
endif()
