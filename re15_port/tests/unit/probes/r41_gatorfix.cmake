# Runde 41 Spur G2 "gatorfix" - Riegel und Werkzeuge. Dossier: analysis/befunde_runde41/G2_gatorfix.md.
add_executable(test_r41_gatorfix ${CMAKE_CURRENT_LIST_DIR}/../test_r41_gatorfix.c)
target_link_libraries(test_r41_gatorfix PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_gatorfix PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/..)
target_compile_definitions(test_r41_gatorfix PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r41_gatorfix PRIVATE m)
endif()
#   unit_r41_gatorfix_maul(_elza)  Punkt 1: in allen Haltebildern (Koppelbild + 26 .. Wurf, Fang .. weg) kein Leichen-Scheitel ueber dem Schaedel
#                                  (Part 7) oder unter dem Unterkiefer (Part 6), Laengsfenster +-150; Gegenprobe Leiche wie vor G2 (Pruefhaken)
#   unit_r41_gatorfix_wurf(_elza)  Punkt 2: Wurf Clip 5 Bild 57, hochgeworfen (>= H/2 ueber der Maullage), Fang Bild 75 im Maul (<= 40), weg Bild 77
#                                  samt Schluessel; Gegenprobe Leiche wie vor G2: kein Wurf
foreach(_teil maul maul_elza wurf wurf_elza)
    add_test(NAME unit_r41_gatorfix_${_teil} COMMAND test_r41_gatorfix ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_gatorfix_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
#   unit_r41_gatorfix_montage(_elza)  Punkt 3: fuenf Zombies in der Montage, Blick zur Leiter (Gang: integration_r41_gatorfix_gang_montage*)
#   unit_r41_gatorfix_marvin(_elza)   Punkte 4 + 5: drei Zombies, Blick zur Leiter, alle erschossen <= 90 Bilder, beim Tod >= 2000 von der Leiter;
#                                     Musik-Eintrag 0xFF3C in 2080 waehrend der Szene und zurueck; Gegenprobe 2080 ohne Szene = Raumtabelle
#   integration_r41_gatorfix_gang_*   exe: Gang zur Leiter (Montage: alle fuenf >= 1000 naeher; Szene: naeher beim Tod, >= 2000 Rest, letzter Schuss <= F90)
foreach(_teil montage montage_elza marvin marvin_elza)
    add_test(NAME unit_r41_gatorfix_${_teil} COMMAND test_r41_gatorfix ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_gatorfix_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_gatorfix.cmake)
    foreach(_teil gang_montage gang_montage_elza gang_szene gang_szene_elza)
        add_test(NAME integration_r41_gatorfix_${_teil}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_TOOL=$<TARGET_FILE:test_r41_gatorfix>
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_gatorfix_${_teil}_wd
                         -DTEIL=${_teil}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_gatorfix.cmake)
        set_tests_properties(integration_r41_gatorfix_${_teil} PROPERTIES TIMEOUT 900)
    endforeach()
endif()
