# Runde 38 (2026-10-16), Spur F "dining1141" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde38/F_dining1141.md, Konstanten include/re15_dining_r38.h, Code engine/src/dining_r38.c,
# Programme engine/src/gen/dining_r38_programme.inc (tools/r38_dining/programme_bauen.py).
# Haken: scd_room_setup.c (Installer-Ende, "Runde 38 Spur F"), scd_vm.c (Ereignis-/Evt_exec-Haken), platform/pc/main.c
# (Boot-/CONTINUE-Block, Prop-Lader, Icon/Bild, Messhaken), audio_pc.c (Port-Baenke 0x3B/0x3C/0x3D).
#
#   unit_r38_dining1141_texte          fuenf Port-Texte dekodiert = Wortlaut; nirgends "Help" als Text (Punkt 1)
#   unit_r38_dining1141_programme      HAEMMERN ohne Message_on; BEFREIUNG ohne msg 0/1/2, mit 40, Lagen = Kopf
#   unit_r38_dining1141_leiche1021     Roys Leiche -> msg 40 mit Hinweis (Punkt 1/2)
#   unit_r38_dining1141_schluessel1011 Schluessel nur nach der 1021-Szene, Aufnahme -> Item 0x43 + (9,64) (Punkt 2)
#   unit_r38_dining1141_tuer1131       Tuer zu ohne Schluessel, Aufschliessen + (9,66), dann Durchgang (Punkt 2)
#   unit_r38_dining1141_phase_h        Nachtrag 2 + Punkt 2: John ab Bild 0 hinter dem Rolladen, Ton-Takt, Gemaelde,
#                                      Szene ohne msg 0/1, Sperre msg 41, Abgang ausserhalb des Bildes, 1171
#   unit_r38_dining1141_leon1140       Punkt 3: Rolladen-Ton Leon ROOM1140
#   integration_r38_dining1141         echte exe (test_r38_dining1141.cmake): Elza-Weg 1021 -> 1011 -> 1021, 1131 -> 1141
#                                      -> 1171 mit Bildern/Ton-Mitschnitt, Leon 1140
add_executable(test_r38_dining1141 ${CMAKE_CURRENT_LIST_DIR}/../test_r38_dining1141.c)
target_link_libraries(test_r38_dining1141 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_dining1141 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_dining1141 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r38_dining1141 PRIVATE m)
endif()
foreach(_teil texte programme leiche1021 schluessel1011 tuer1131 phase_h leon1140)
    add_test(NAME unit_r38_dining1141_${_teil} COMMAND test_r38_dining1141 ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r38_dining1141_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_dining1141.cmake)
    add_test(NAME integration_r38_dining1141
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r38_dining1141_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_dining1141.cmake)
    set_tests_properties(integration_r38_dining1141 PROPERTIES TIMEOUT 3600)
endif()
