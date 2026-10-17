# Runde 38 (2026-10-16), Spur Q "kleinkram" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde38/Q_kleinkram.md, Belege include/re15_kleinkram_r38.h, Logik engine/src/kleinkram_r38.c
# (Haken menu_common.c / scd_vm.c / elza_r36.c, "Runde 38 Spur Q").
#
#   unit_r38_kleinkram_inventar       P1: START-Annahme -> CORE Satz 6 (RE2 @0x8002652c), Schliessen START/Kreuz/EXIT
#                                     -> Satz 5 (RE2 @0x8006a968/@0x8006a930/@0x8006a8d4), Box-Anforderung ohne Ton
#   unit_r38_kleinkram_irons          P2: ROOM1151 echter Aktions-Druck -> sub03 Hinbeugen, Text 20 nach 80 Bildern, kein
#                                     msg 2, Irons tot; danach Text-Platz; Leon 1150 unveraendert
#   unit_r38_kleinkram_flammenwerfer  P3: ROOM30E0 Panel -> Zelle 4/5 -> Aufnahme-Modal 0x0E x100 statt msg 15; Ja / Nein /
#                                     voll
add_executable(test_r38_kleinkram ${CMAKE_CURRENT_LIST_DIR}/../test_r38_kleinkram.c)
target_link_libraries(test_r38_kleinkram PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_kleinkram PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r38_kleinkram PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r38_kleinkram PRIVATE m)
endif()
foreach(_teil inventar irons flammenwerfer)
    add_test(NAME unit_r38_kleinkram_${_teil} COMMAND test_r38_kleinkram ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r38_kleinkram_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

#   integration_r38_kleinkram_inventar       ECHTE exe: START in Bild 400/500 gegen Lauf ohne START -> genau zwei hoerbare
#                                            Abschnitte im Abstand 100 (RE15_AUDIO_CAP_SYNC), debug.log Satz 6 / Satz 5
#   integration_r38_kleinkram_irons          Elza-Spielstand ROOM1151 + CONTINUE, echtes Viereck: Text 20 erst nach dem
#                                            Hinbeugen (Bild 140..145, mo=11), msg 2 nie, Irons tot, 2. Klick nur Text
#   integration_r38_kleinkram_flammenwerfer  Leon-Spielstand ROOM30E0 + CONTINUE, Panel -> Zelle 4 -> Zelle 5: Modal 0x0E x100
#                                            statt msg 15, Ja = 2 Zellen, Nein = liegt + Panel wieder scharf
if(TARGET re15_pc)
    foreach(_teil inventar irons flammenwerfer)
        add_test(NAME integration_r38_kleinkram_${_teil}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_elza_karte>
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r38_kleinkram_wd
                         -DTEIL=${_teil}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_kleinkram.cmake)
        set_tests_properties(integration_r38_kleinkram_${_teil} PROPERTIES TIMEOUT 1200)
    endforeach()
endif()
