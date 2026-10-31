# Runde 41 Spur Q "restpunkte". Dossier: analysis/befunde_runde41/Q_restpunkte.md.
#   unit_r41_restpunkte_gator              Tor vor dem Alligator: Bit 110 sperrt nicht, (9,111) sperrt (RE2 ROOM40A0 @0x0054, sub02 @0x00AE, Tot-Bit @0x80102660)
#   unit_r41_restpunkte_original           Prozess mit RE15_GATOR_ORIGINAL=1: Spinnen-Filter aus, Tor aus
#   unit_r41_restpunkte_gpu                Groessengrenze der PSX-GPU 1023 x 511 (psx-spx), Rechnung gegen den Ueberlauf in SDL_triangle.c:618
#   unit_r41_restpunkte_teil8(_elza)       ROOM6010/6011 Cut 5 Teil 8: je OT-Stufe ein Rechteck, Tiefe == bucket(Eintritt) an jedem Punkt (Abnahme M 5 Mangel 1)
#   integration_r41_restpunkte_gator(_elza)     exe: natuerlicher R1-Tod, Tuer, Spielstand, Leiter: kein Alligator, CONTINUE: kein Alligator, Water Key zu Fuss aufgehoben, danach Raum leer
#   integration_r41_restpunkte_original(_elza)  exe: RE15_GATOR_ORIGINAL zeigt die Spinnen des ausgelieferten Raums und den RE1.5-Alligator, keine Szene
#   integration_r41_restpunkte_masken      exe: Teil 8 HINTER verdeckt, DAVOR nicht uebermalt, Leon und Elza
#   integration_r41_ventilkette_kette_elza exe: die Kette als Elza, 2031 -> 20B1 -> 2051 (UEBERGABE 3b)
add_executable(test_r41_restpunkte ${CMAKE_CURRENT_LIST_DIR}/../test_r41_restpunkte.c)
target_link_libraries(test_r41_restpunkte PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_restpunkte PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/..)
if(NOT WIN32)
    target_link_libraries(test_r41_restpunkte PRIVATE m)
endif()
add_test(NAME unit_r41_restpunkte_gator COMMAND test_r41_restpunkte gator WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME unit_r41_restpunkte_original COMMAND test_r41_restpunkte original WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME unit_r41_restpunkte_gpu COMMAND test_r41_restpunkte gpu WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_restpunkte_original PROPERTIES ENVIRONMENT "RE15_GATOR_ORIGINAL=1")
add_test(NAME unit_r41_restpunkte_teil8 COMMAND test_r37_masken36 stufen 6010 5 113 127 426 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME unit_r41_restpunkte_teil8_elza COMMAND test_r37_masken36 stufen 6011 5 113 127 426 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_restpunkte_gator unit_r41_restpunkte_original unit_r41_restpunkte_gpu unit_r41_restpunkte_teil8 unit_r41_restpunkte_teil8_elza PROPERTIES TIMEOUT 300)

if(TARGET re15_pc)
    foreach(_teil gator gator_elza original original_elza)
        add_test(NAME integration_r41_restpunkte_${_teil}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_WEG_TOOL=$<TARGET_FILE:test_r41_alligator2>
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_restpunkte_${_teil}_wd
                         -DTEIL=${_teil}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_restpunkte.cmake)
        set_tests_properties(integration_r41_restpunkte_${_teil} PROPERTIES TIMEOUT 1300)
    endforeach()
    add_test(NAME integration_r41_restpunkte_masken
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r37_masken36_ppm>
                     -DRE15_SICHT_TOOL=$<TARGET_FILE:probe_r41_masken36_sicht>
                     -DRE15_BG_TOOL=$<TARGET_FILE:probe_bg_dump>
                     -DCD_ROOT=${CMAKE_SOURCE_DIR}/shared_assets/PSX
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_restpunkte_masken_wd
                     -DSZENEN=${CMAKE_SOURCE_DIR}/tests/integration/test_r41_restpunkte_szenen.cmake
                     -DPRAEFIX=r41q1
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_stage2.cmake)
    set_tests_properties(integration_r41_restpunkte_masken PROPERTIES TIMEOUT 1500)
    if(EXISTS ${CMAKE_SOURCE_DIR}/engine/src/wasser20b0_r41.c)
        add_test(NAME integration_r41_ventilkette_kette_elza
                 COMMAND ${CMAKE_COMMAND}
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_SRC=${CMAKE_SOURCE_DIR}
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_ventilkette_kette_elza
                         -DFIGUR=elza
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_ventilkette_kette.cmake)
        set_tests_properties(integration_r41_ventilkette_kette_elza PROPERTIES TIMEOUT 900 RUN_SERIAL TRUE)
    endif()
endif()
