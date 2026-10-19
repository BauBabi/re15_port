# Runde 39 Spur Q "elzazombies" — Elza: Zombie-Durchbrueche in der Panikphase (Bank-9-Bit 97, Spur P):
# 10E1-Ueberlebende durch die 10C1-Tuer ins Treppenhaus ROOM1061 (Etage 4), Kriecher durch das Rollo ROOM1031
# (Original-Szene sub08), nicht besiegte 1061-Zombies beim naechsten Betreten von ROOM1041 durchs Rolltor (Nachtrag 11; seit
# Nachbesserung 2 nur in der Panikphase (9,97)) — Montage-Form wie Leon nach Irons' Tod. NACHTRAG 12 (Angleichung "aus den
# gleichen Tueren"): Elza zusaetzlich ROOM1071 (1031 Cut 7 = Leons 1070-Schritt), Leon zusaetzlich das Treppenhaus (ROOM1060
# Cut 2 in der Montage nach Irons' Tod, Haken in irons_tod_1150.c). Dossier analysis/befunde_runde39/Q_elzazombies.md.
# Alle Riegel unten loeschen vor dem ersten Engine-/exe-Aufruf jede geerbte RE15_*-Variable (VERTRAG Runde 39
# Abschnitt 2, Nachbesserung 1: test_r39_elzazombies.c main(), test_r39_elzazombies.cmake nach dem include).
#
# MESS-WERKZEUG (kein add_test):
#   probe_r39_elzazombies_karte <karte> <raum-hex> <x> <y> <z> <gier> <cut> [bank:bit ...] [give:id]
add_executable(probe_r39_elzazombies_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r39_elzazombies_karte.c)
target_link_libraries(probe_r39_elzazombies_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r39_elzazombies_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

# RIEGEL (Unit, echte RDTs, Raumaufbau wie der Tuer-Weg, Bilder wie main.c; RE2-Gegnerbaenke wie die exe):
#   unit_r39_elzazombies_zensus    Quell-Records 10E1/1071 = Konstanten; Kopie-Bits frei (alle Sce_em_set STAGE1-3), die 11 Elza-
#                                  Bits nicht Leons, die 1031-Cut-7-Bits = Leons Vorlage 6; Raum-Paare 1060/1061, 10E0/10E1,
#                                  10C0/10C1, 1070/1071 byte-gleich; Programm L + Weiche (Leon);
#                                  1031-Programm = RDT-Zeilen sub08; sub06/07/09 = ROOM1030; ROOM1041 = ROOM1040 (sub08, Schalter,
#                                  Rolltor, Limit, RID); Programm G = 1041-sub08-Zeilen; Kameras = RID; Tuer 10C1 -> 1061 Etage 4;
#                                  Tuer-Slot/Ereignis/Bank-5-Bits frei
#   unit_r39_elzazombies_kette_4   Kette 1041 -> 1061 (4 Kopien, Cut 2) -> 1031 (Cut 7: 5 Kopien der 1071-Zombies, Original-
#                                  Knall; Rollo Cut 12, Kriecher) -> 1041; jede Kopie >= 400 naeher an der Kamera; HP 100;
#                                  Rueckkehr an die Stelle; (9,95); zweite Flanke ohne Wirkung; GEGENPROBE Modul aus = keine Kette
#   unit_r39_elzazombies_kette_0 / _2 / _00  10E1 leer + 1071 voll (Auftakt -> 1031 Cut 7) / 2 + 3 / beide leer (gleich Cut 12)
#   unit_r39_elzazombies_leon_4 / _2 / _0  Leon (Nachtrag 12): Kette 1150 -> [1130] -> 1060 Cut 2 -> 1040 -> 1030 mit 4 / 2 (1140
#                                  leer) / 0 (wie bisher) lebenden 10E0-Zombies; K3-Folge, Kopien naeher, HP 100; leon_4 mit
#                                  Gegenprobe und Vergleich: Leons 1040-Schritt nach 1060 unveraendert
#   unit_r39_elzazombies_leon_nach1060  Leon betritt 1060 nach der Szene: Kopien Westseite, gehalten bis Etage 4; ohne (9,96) nichts
#   unit_r39_elzazombies_altstand  Nachbesserung 3, Mangel 1: alter Leon-Spielstand nach Irons' Tod (v0.8.22..v0.8.26, (9,96)=0):
#                                  ROOM1060 ohne Gegner (10E0 getoetet: Karte der Abnahme M10; 10E0 lebt: bleiben in 10E0, nicht
#                                  doppelt); Gegenprobe (9,96)=1 -> 4 Kopien; re15_r39q_leon_umzug setzt (9,96) + Kopie-Bits
#   unit_r39_elzazombies_oben      Nachbesserung 3, Mangel 2 / E1: Ankunft von oben (Tuer Etage 8, Elza 1061 + Leon 1060): Ostseite,
#                                  im Spawn-Bild kein Haltebit (INIT), danach gehalten + initialisiert, frei am Ende der Treppe A
#                                  mit Abstand >= 4591, kein Griff; Tuer 10C1 (Etage 4): Westseite, sofort frei
#   unit_r39_elzazombies_nach1061  Nachspawn Westseite, gehalten 200 Bilder auf Etage 0, frei auf Etage 4 (>= 4000), kommen
#                                  danach naeher; getoetete Kopie fehlt
#   unit_r39_elzazombies_nach1031  Nachspawn 1031 mit (9,95) = 3 Kriecher + die lebenden 1071-Kopien (Leons Nachspawn-Form);
#                                  ohne (9,95) nichts
#   unit_r39_elzazombies_durch1041 Nachtrag 11: Betreten von 1041 (Tuer 1061) mit lebenden 1061-Kopien -> Rolltor-Durchbruch
#                                  (Cut 1, Elza geparkt, Rolltor hoch, Kopien durch und naeher, HP 100, Elza zurueck, Halt los);
#                                  naechstes Betreten = Nachspawn ohne zweiten Durchbruch; Nordteil / alle besiegt: nichts;
#                                  H1: (9,97)=0 (John gerettet) -> kein Durchbruch
#   unit_r39_elzazombies_ohne      Bit 97 im Spielstand = keine Flanke; ohne Bit 97 nichts; Leon (1040/1060): nichts
add_executable(test_r39_elzazombies ${CMAKE_CURRENT_LIST_DIR}/../test_r39_elzazombies.c)
target_link_libraries(test_r39_elzazombies PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_elzazombies PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_elzazombies PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_elzazombies PRIVATE m)
endif()
foreach(_teil zensus kette_4 kette_0 kette_2 kette_00 nach1061 nach1031 durch1041 ohne leon_4 leon_2 leon_0 leon_nach1060
              altstand oben)
    add_test(NAME unit_r39_elzazombies_${_teil} COMMAND test_r39_elzazombies ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r39_elzazombies_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

# INTEGRATION (echte exe, tests/integration/test_r39_elzazombies.cmake; Arbeitsordner r39_elzazombies_wd_<teil>, nicht der
# Target-Name):
#   integration_r39_elzazombies_kette   Spielstand 1041 + (9,97) bei Bild 200: ganze Kette im debug.log, Kopien naeher an der
#                                       Kamera, HP 100, Rueckkehr an die gemerkte Stelle; mit 1071-Schritt (1031 Cut 7)
#   integration_r39_elzazombies_durch   echter Weg Tuer 1061 -> 1041 mit lebenden 1061-Kopien: Rolltor-Durchbruch (Nachtrag 11)
#   integration_r39_elzazombies_knall   zwei RE15_AUDIO_CAP_SYNC-Mitschnitte (mit/ohne RE15_R39Q_KNALL_STUMM): zwei hoerbare
#                                       Tuerknaelle
#   integration_r39_elzazombies_treppe  echter Weg Tuer 1041 -> 1061, Treppe A + B: Kopien an der Westseite gehalten, frei bei
#                                       der Ankunft (z 24568), Abstand >= 4000; Bild ankunft.ppm
#   integration_r39_elzazombies_ohne    Leon 1040 + Flanke: nichts; Elza mit (9,97) im Spielstand: keine Flanke; H1: Tuer
#                                       1061 -> 1041 ohne (9,97): kein Durchbruch
#   integration_r39_elzazombies_leon    Nachtrag 12, echter Weg Leon 1130 -> Szene -> 1130 -> Treppenhaus 1060 -> 1040 -> 1030
#   integration_r39_elzazombies_altstand Nachbesserung 3, Mangel 1: Karte der Abnahme M10 (Leon nach Irons' Tod, 10E0 getoetet, ohne
#                                       (9,96)) -> Tuer 1040 -> 1060: kein Gegner (state.log Bild 90 = v0.8.26); Gegenprobe mit
#                                       (9,96): Nachspawn mit 4 Kopien
#   integration_r39_elzazombies_oben    Nachbesserung 3, Mangel 2 / E1: echter Weg von oben (Tuer Etage 8 -> Treppe B -> Treppe A)
#                                       fuer Leon (1060) und Elza (1061): Kopien an der Ostseite, in Schnitt 2 initialisiert
#                                       stehend (state.log st != 0, hp > 0), frei am Ende der Treppe A mit Abstand >= 4591, kein
#                                       Griff; Bild ankunft_oben.ppm
if(TARGET re15_pc)
    foreach(_teil kette knall treppe durch ohne leon altstand oben)
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
