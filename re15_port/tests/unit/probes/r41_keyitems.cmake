# Runde 41 Spur O "keyitems" — Dossier: analysis/befunde_runde41/O_keyitems.md
#   unit_r41_keyitems_{stellen,use,aktion,besitz,voll,kurbel,waffe,daten}
#       Key Items ueber das Inventar in RE2-Form (engine/src/keyitems_r41.c): Stellen-Tabelle gegen die RDTs, USE richtig/falsch,
#       Aktionstaste ohne Frage, Besitz der Flag-Gegenstaende (Kurbel, Bombe, Sicherung), volles Inventar, Mechanik des Fliegens
#       (Kurbel ohne Blende im ersten Skripttakt), Waffe ablegen wie RE2, Name/Bild der Kurbel.
add_executable(test_r41_keyitems ${CMAKE_CURRENT_LIST_DIR}/../test_r41_keyitems.c)
target_link_libraries(test_r41_keyitems PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_keyitems PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_keyitems PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_keyitems PRIVATE m)
endif()
foreach(_teil stellen use aktion besitz voll kurbel waffe daten)
    add_test(NAME unit_r41_keyitems_${_teil} COMMAND test_r41_keyitems ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_keyitems_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
#   integration_r41_keyitems_{kurbel,elza,renn,geh,west,falsch,aktion,karte,karte_aktion}
#       an der ECHTEN exe (tests/integration/test_r41_keyitems.cmake), Auswerter test_r41_keyitems_exe.
add_executable(test_r41_keyitems_exe ${CMAKE_CURRENT_LIST_DIR}/../test_r41_keyitems_exe.c)
if(TARGET re15_pc)
    foreach(_teil kurbel elza renn geh west falsch aktion karte karte_aktion)
        add_test(NAME integration_r41_keyitems_${_teil}
            COMMAND ${CMAKE_COMMAND}
                -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                -DRE15_WERKZEUG=$<TARGET_FILE:test_r41_keyitems_exe>
                -DWORKDIR=${CMAKE_BINARY_DIR}/r41_keyitems_wd
                -DTEIL=${_teil}
                -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_keyitems.cmake)
        set_tests_properties(integration_r41_keyitems_${_teil} PROPERTIES TIMEOUT 900)
    endforeach()
endif()
