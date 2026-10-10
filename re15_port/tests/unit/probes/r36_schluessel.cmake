# Runde 36 (2026-10-10), Spur Q "schluessel" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde36/Q_schluessel.md, Konstanten include/re15_schluessel_r36.h.
#
#   unit_r36_schluessel_raum   10F0/10F1: Prop + Zone, Bit -> weg, echter Druck -> Modal -> Ja (Item 0x45/0x42,
#                              Bit 82/83), Prison Key setzt (4,0xEC) wie ROOM1011 sub06 @0x00E2C
#   unit_r36_schluessel_tuer   ROOM11B0 Parkplatz-Tuer: ohne Schluessel msg 28 + Verschlossen-Ton, mit Schluessel
#                              msg 29 + Aufschliess-Ton + (9,93) + Tuer zurueck, naechster Druck -> ROOM11C0;
#                              ohne Strom / ROOM11B1 unberuehrt
#   unit_r36_schluessel_zelle  ROOM1011 Roy-Ereignis 6 -> sub07 (msg 18), ROOM1211 Zelle mit (4,0xEC) -> John
#   unit_r36_schluessel_namen  Prompt-Name, CHECK-Text, Icon-Tile, ITPS-Block (Rechtecke + Leser)
#   unit_r36_schluessel_befreiung  (Nachbesserung 1) ROOM1211 Befreiungsszene sub02 am echten VM + Spielschritt:
#                              nach msg 9 @0x0226A "Discard it?" fuer den Prison Key 0x42 (RE2 @0x80051670/@0x80051834),
#                              Ja -> weg, Nein -> bleibt, ohne Schluessel im Inventar keine Abfrage
add_executable(test_r36_schluessel ${CMAKE_CURRENT_LIST_DIR}/../test_r36_schluessel.c)
target_link_libraries(test_r36_schluessel PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_schluessel PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r36_schluessel PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_RE2_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon")
foreach(_teil raum tuer zelle namen befreiung)
    add_test(NAME unit_r36_schluessel_${_teil} COMMAND test_r36_schluessel ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r36_schluessel_${_teil} PROPERTIES TIMEOUT 120)
endforeach()

# MESS-WERKZEUG (kein add_test): Speicherkarte an frei gewaehlter Stelle mit Flags/Inventar/Figur fuer
# Laeufe mit der echten exe (CONTINUE-Weg).
add_executable(probe_r36_schluessel_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r36_schluessel_karte.c)
target_link_libraries(probe_r36_schluessel_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_schluessel_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

#   integration_r36_schluessel  ECHTE exe am Lade-Weg: Parkplatz-Tuer zu/auf (ROOM11B0 -> ROOM11C0), Aufnahme
#                               Parking Lot Key (Leon 10F0) / Prison Key (Elza 10F1, (4,0xEC)), CHECK-Foto beider
if(TARGET re15_pc)
    add_test(NAME integration_r36_schluessel
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_schluessel_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r36_schluessel_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r36_schluessel.cmake)
    set_tests_properties(integration_r36_schluessel PROPERTIES TIMEOUT 1500)
endif()
