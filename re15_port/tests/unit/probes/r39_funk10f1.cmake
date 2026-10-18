# Runde 39 / Spur I "funk10f1" — Funkszene ROOM10F1: Elza geht beim ersten Betreten nach rechts zum
# Kommunikationssystem, drueckt darauf ("Hello, anyone, can you hear me? We Need help!"), "...", "Uhh, this Thing is
# broken!". Dossier: analysis/befunde_runde39/I_funk10f1.md, Konstanten: include/re15_funk10f1_r39.h,
# Code: engine/src/funk10f1_r39.c (+ gen/funk10f1_r39_szene.inc, Generator tools/r39_funk10f1/szene_bauen.py).
#
#   test_r39_funk10f1          RIEGEL (Teil ueber das Argument):
#     unit_r39_funk10f1_form   Programm: Laengen schliessen auf Evt_end, Set(9,70) zuerst, Rahmen/Warten/Druck/Senken/
#                              Umsehen/Stirn/Schwanz BYTEGLEICH mit ROOM1031 sub13/sub14, Weg frei von allen SCA-Zellen
#                              (Abstand > 468), Pruefpunkt in der Text-Zone des Kommunikationssystems (Slot 2 @0x00F72)
#     unit_r39_funk10f1_texte  msg 40..42 = Nutzer-Wortlaut, Kopf/Ende = ROOM1031 msg 19 @0x2FA7
#     unit_r39_funk10f1_szene  echte VM: Start beim Raumaufbau, (9,70) ab Bild 0, Drehung nach rechts, msg 40 -> 41 -> 42,
#                              Stimme 40/41/42 angefordert, Druck 11 gehalten waehrend msg 40, 0 rueckwaerts, 9, 12,
#                              Platz an der Konsole (Cut 1, Blick Osten), Balken weg, Spieler frei
#     unit_r39_funk10f1_einmal zweiter Raumaufbau ohne Szene/Nachricht/Stimme/Leihe; Leon ROOM10F0 nie
#   integration_r39_funk10f1_echt   echte exe, Elza durch die Kartenleser-Tuer ROOM10D1 -> ROOM10F1 (RE2-Tuersequenz),
#                                   Weg, Gesten (ANIM_TRACE), Nachrichten + Stimme (Stellvertreter main40.wav im Laufordner),
#                                   danach zweites Betreten (Raum neu aufgebaut) ohne Szene
#   integration_r39_funk10f1_zweit  echte Tuer mit (9,70)=1: keine Szene, Spieler frei
#   integration_r39_funk10f1_leon   Leon ROOM10F0: keine Funkszene, keine Leihe
add_executable(test_r39_funk10f1 ${CMAKE_CURRENT_LIST_DIR}/../test_r39_funk10f1.c)
target_link_libraries(test_r39_funk10f1 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_funk10f1 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_funk10f1 PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_funk10f1 PRIVATE m)
endif()
foreach(_teil form texte szene einmal)
    add_test(NAME unit_r39_funk10f1_${_teil} COMMAND test_r39_funk10f1 ${_teil})
    set_tests_properties(unit_r39_funk10f1_${_teil} PROPERTIES TIMEOUT 240)
endforeach()

if(TARGET re15_pc)
    foreach(_teil echt zweit leon)
        add_test(NAME integration_r39_funk10f1_${_teil}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_SYNCHRO_DIR=${CMAKE_SOURCE_DIR}/../synchro
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r39_funk10f1_wd
                         -DTEIL=${_teil}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_funk10f1.cmake)
        set_tests_properties(integration_r39_funk10f1_${_teil} PROPERTIES TIMEOUT 900)
    endforeach()
endif()
