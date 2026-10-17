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
#   unit_r38_dining1141_szene1021      Nachbesserung 1 M1: echte 1021-Szene, danach Roys Leiche im SELBEN Besuch -> msg 40
#   unit_r38_dining1141_abgang         Nachbesserung 1 M3: Johns Abgang mit Elza an sechs Lagen (Abnahme E10/E5/E6/E7 + 2):
#                                      nie verschoben, im Bild nie naeher als 900, in der sichtbaren Bahn wartet der Abgang
#   unit_r38_dining1141_weg1131        Nachbesserung 1 M3: Elza geht waehrend des Wartens hinaus -> John fort, Dachtuer offen
#   integration_r38_dining1141_w|z|d|l echte exe (test_r38_dining1141.cmake): Elza-Weg 1021 -> 1011 -> 1021, 1131 -> 1141
#                                      -> 1171 mit Bildern/Ton-Mitschnitt, Leon 1140
#   integration_r38_dining1141_s       Nachtrag 2 per Framedump: zwei Laeufe (mit/ohne Johns Bild), JEDES Bild von 1141
#                                      verglichen (Werkzeug test_r38_dining1141 sicht) — John erscheint/verschwindet nie
#                                      ohne Bewegung (Fortsetzung 4, Dossier 4.4)
add_executable(test_r38_dining1141 ${CMAKE_CURRENT_LIST_DIR}/../test_r38_dining1141.c)
target_link_libraries(test_r38_dining1141 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_dining1141 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_dining1141 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r38_dining1141 PRIVATE m)
endif()
foreach(_teil texte programme leiche1021 schluessel1011 tuer1131 phase_h leon1140 szene1021 abgang weg1131)
    add_test(NAME unit_r38_dining1141_${_teil} COMMAND test_r38_dining1141 ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r38_dining1141_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

#   Je Teil ein eigener Test (gemessen unter Last 2026-10-16: W ~25 min, Z ~8, D ~40, L ~10 — zusammen ueber jedem
#   vernuenftigen Einzel-Zeitlimit); eigener Arbeitsordner und eigene exe-Kopie je Teil.
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_dining1141.cmake)
    foreach(_teil W Z D L S)
        string(TOLOWER ${_teil} _klein)
        add_test(NAME integration_r38_dining1141_${_klein}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_R38F_WERKZEUG=$<TARGET_FILE:test_r38_dining1141>
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r38_dining1141_${_klein}_wd
                         -DTEIL=${_teil}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_dining1141.cmake)
        set_tests_properties(integration_r38_dining1141_${_klein} PROPERTIES TIMEOUT 3600)
    endforeach()
endif()
