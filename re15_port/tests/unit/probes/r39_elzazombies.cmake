# Runde 39 Spur Q "elzazombies" — Elza: Zombie-Durchbrueche in der Panikphase (Bank-9-Bit 97, Spur P):
# 10E1-Ueberlebende durch die 10C1-Tuer ins Treppenhaus ROOM1061 (Etage 4), Kriecher durch das Rollo ROOM1031
# (Original-Szene sub08), nicht besiegte 1061-Zombies beim naechsten Betreten von ROOM1041 durchs Rolltor (Nachtrag 11;
# ROOM1071 bleibt unberuehrt) — Montage-Form wie Leon nach Irons' Tod. Dossier analysis/befunde_runde39/Q_elzazombies.md.
# Alle Riegel unten loeschen vor dem ersten Engine-/exe-Aufruf jede geerbte RE15_*-Variable (VERTRAG Runde 39
# Abschnitt 2, Nachbesserung 1: test_r39_elzazombies.c main(), test_r39_elzazombies.cmake nach dem include).
#
# MESS-WERKZEUG (kein add_test):
#   probe_r39_elzazombies_karte <karte> <raum-hex> <x> <y> <z> <gier> <cut> [bank:bit ...] [give:id]
add_executable(probe_r39_elzazombies_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r39_elzazombies_karte.c)
target_link_libraries(probe_r39_elzazombies_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r39_elzazombies_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

# RIEGEL (Unit, echte RDTs, Raumaufbau wie der Tuer-Weg, Bilder wie main.c; RE2-Gegnerbaenke wie die exe):
#   unit_r39_elzazombies_zensus    Quell-Records 10E1 = Konstanten; Kopie-Bits frei (alle Sce_em_set STAGE1-3) und nicht Leons;
#                                  1031-Programm = RDT-Zeilen sub08; sub06/07/09 = ROOM1030; ROOM1041 = ROOM1040 (sub08, Schalter,
#                                  Rolltor, Limit, RID); Programm G = 1041-sub08-Zeilen; Kameras = RID; Tuer 10C1 -> 1061 Etage 4;
#                                  Tuer-Slot/Ereignis/Bank-5-Bits frei
#   unit_r39_elzazombies_kette_4   Kette 1041 -> 1061 (4 Kopien, Cut 2) -> 1031 (Rollo Cut 12, Kriecher) -> 1041; jede Kopie
#                                  >= 400 naeher an der Kamera; HP 100; Rueckkehr an die Stelle; (9,95); 1071 unberuehrt;
#                                  zweite Flanke ohne Wirkung; GEGENPROBE Modul aus = keine Kette
#   unit_r39_elzazombies_kette_0 / _2  10E1 leer (kein 1061-Schritt, gleich Cut 12) / 2 Kopien
#   unit_r39_elzazombies_nach1061  Nachspawn Westseite, gehalten 200 Bilder auf Etage 0, frei auf Etage 4 (>= 4000), kommen
#                                  danach naeher; getoetete Kopie fehlt
#   unit_r39_elzazombies_nach1031  Nachspawn 1031 mit (9,95) = 3 Kriecher, keine Kopie; ohne (9,95) nichts
#   unit_r39_elzazombies_durch1041 Nachtrag 11: Betreten von 1041 (Tuer 1061) mit lebenden 1061-Kopien -> Rolltor-Durchbruch
#                                  (Cut 1, Elza geparkt, Rolltor hoch, Kopien durch und naeher, HP 100, Elza zurueck, Halt los);
#                                  naechstes Betreten = Nachspawn ohne zweiten Durchbruch; Nordteil / alle besiegt: nichts
#   unit_r39_elzazombies_ohne      Bit 97 im Spielstand = keine Flanke; ohne Bit 97 nichts; Leon (1040/1060): nichts
add_executable(test_r39_elzazombies ${CMAKE_CURRENT_LIST_DIR}/../test_r39_elzazombies.c)
target_link_libraries(test_r39_elzazombies PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_elzazombies PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_elzazombies PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_elzazombies PRIVATE m)
endif()
foreach(_teil zensus kette_4 kette_0 kette_2 kette_00 nach1061 nach1031 durch1041 ohne leon_4 leon_2 leon_0 leon_nach1060)
    add_test(NAME unit_r39_elzazombies_${_teil} COMMAND test_r39_elzazombies ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r39_elzazombies_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

# INTEGRATION (echte exe, tests/integration/test_r39_elzazombies.cmake; Arbeitsordner r39_elzazombies_wd_<teil>, nicht der
# Target-Name):
#   integration_r39_elzazombies_kette   Spielstand 1041 + (9,97) bei Bild 200: ganze Kette im debug.log, Kopien naeher an der
#                                       Kamera, HP 100, Rueckkehr an die gemerkte Stelle (kein 1071-Schritt)
#   integration_r39_elzazombies_durch   echter Weg Tuer 1061 -> 1041 mit lebenden 1061-Kopien: Rolltor-Durchbruch (Nachtrag 11)
#   integration_r39_elzazombies_knall   zwei RE15_AUDIO_CAP_SYNC-Mitschnitte (mit/ohne RE15_R39Q_KNALL_STUMM): zwei hoerbare
#                                       Tuerknaelle
#   integration_r39_elzazombies_treppe  echter Weg Tuer 1041 -> 1061, Treppe A + B: Kopien an der Westseite gehalten, frei bei
#                                       der Ankunft (z 24568), Abstand >= 4000; Bild ankunft.ppm
#   integration_r39_elzazombies_ohne    Leon 1040 + Flanke: nichts; Elza mit (9,97) im Spielstand: keine Flanke
if(TARGET re15_pc)
    foreach(_teil kette knall treppe durch ohne leon)
        add_test(NAME integration_r39_elzazombies_${_teil}
            COMMAND ${CMAKE_COMMAND}
                -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                -DRE15_KARTE=$<TARGET_FILE:probe_r39_elzazombies_karte>
                -DRE15_AUSWERTER=$<TARGET_FILE:test_r39_elzazombies>
                -DWORKDIR=${CMAKE_BINARY_DIR}/r39_elzazombies_wd
                -DTEIL=${_teil}
                -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_elzazombies.cmake)
        set_tests_properties(integration_r39_elzazombies_${_teil} PROPERTIES TIMEOUT 1200)
    endforeach()
endif()
