# Runde 39 Spur G "karten" — Karten 1F/2F/3F/B2 nach den Nutzer-Vorlagen (map*_correct.png),
# B1 neu als Schema-Zeichnung aus Kollision + Tueren, Marker ROOM1090 im Raum. Dossier: analysis/befunde_runde39/G_karten.md
#
#   probe_r39_karten ops|marker|flut|markerflaeche|weg ...   MESS-SONDEN (Aufrufe im Dossier)
#   unit_r39_karten_1090     Punkt 2: ROOM1090/1091 beide Ebenen - in KEINER begehbaren Lage (echter Kollisionsloeser,
#                            jeder 3. Rasterpunkt) steht der Marker neben der gemalten Kachel (vorher obere Ebene 160/1225);
#                            Nutzerpunkt (-12528,-9000,959) auf dem Gamma; Tuer -> 1050 auf ihrer Marke (191,69)
#   unit_r39_karten_b1       Punkt 1 B1: alle B1-Orte (Leon/Elza, 1160 ohne Strom, 11D0-Leiterkammer, 10A0-Absatz) - Marker
#                            in jeder begehbaren Lage auf der eigenen Zeichnung; jede Tuer auf der GEMEINSAMEN Wandlinie
#                            beider Zeichnungen (keine Doppelwand), Treppen im Innenraum; geradlinig durch jeden
#                            Gang-Abschnitt gleichmaessig 2,08..2,38 px je 1000 Einheiten (RE2 /450), Querversatz <= 1,
#                            Schritt <= 2, EINE Zone; Tuerdurchgang <= 3 px Sprung
#   integration_r39_karten   (nur mit re15_pc) echte exe: Sweep aller Blaetter - B2/1F/2F/3F gegen die Soll-Haelften der
#                            Vorlagen (0 Abweichungen; 3F mit den drei begruendeten Treppen-Abweichungen der Nachbesserung 1),
#                            B1..3F ohne Doppelwand; ROOM1090 oben am Nutzerpunkt: Marker-Ring im Rot des aktuellen Raums;
#                            Nachbesserung 1 (M3): Teilzustaende (ROOM1000 Nord-WC, 1070, 1010 je allein) ohne offene Raumseite
# Nachbesserung 1 (Abnahme 0, Dossier G_karten.md "Nachbesserung 1"):
#   unit_r39_karten_teil     M3: jede Doppelwand-Korrektur hat einen Traeger mit Ersatzlinie (Kachel-Index 4); jeder korrigierte
#                            Ort ALLEIN besucht bleibt geschlossen (Klassenbild der echten Op-Liste: kein Raumkoerper am Grund;
#                            vorher 1070 34, 1010 je 30, ROOM1000 38/23/9 ...); Ort + Traeger besucht: keine Doppelwand
#   unit_r39_karten_treppen  M4/M5/M1: jedes Treppensymbol der Treppenhaeuser 1060/10A0 und von 1170 Zone 1 liegt <= 2 px von der
#                            Projektion seines STAIR-AOT-Laufs (Zeile des Blatts, Formel FUN_800473f8); vorher bis 5 px
#   unit_r39_karten_marker   M5: 1030/1060/10A0/1110/1170 Z1/10C0/1120 - kein begehbarer Rasterpunkt neben der Zeichnung
# Nachbesserung 2 (Abnahme 1, N1 - ROOM1210 Suedwest-Kammer auf B2 gezeichnet):
#   unit_r39_karten_marker   + ROOM1210/1211 an JEDEM Rasterpunkt 0 neben der Zeichnung (vorher 1693 von 10387), Elza mit
#                            offener und geschlossener Kammer (sub00 Sca_id_set, Flags (4,0xEC)/(4,0xFC))
#   unit_r39_karten_teil     + (e) Kammer: Linien auf den gemalten (x186/y112/y129), Durchgang x201 y117..119 nur mit
#                            gezeichneter Kammer, keine zweite Linie x200, geschlossen in fuenf Zustaenden (auch Umriss)
#   integration_r39_karten   + Lauf K (Abnahme-Weg 1210 Flur -> Kammer, Ring im Rot der Kammer), Sweep-Kammer gegen SCA
add_executable(probe_r39_karten ${CMAKE_CURRENT_LIST_DIR}/../test_r39_karten.c)
target_link_libraries(probe_r39_karten PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r39_karten PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/..)
target_compile_definitions(probe_r39_karten PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(probe_r39_karten PRIVATE m)
endif()
add_test(NAME unit_r39_karten_1090 COMMAND probe_r39_karten pruef 1090)
add_test(NAME unit_r39_karten_b1 COMMAND probe_r39_karten pruef b1)
add_test(NAME unit_r39_karten_teil COMMAND probe_r39_karten pruef teil)
add_test(NAME unit_r39_karten_treppen COMMAND probe_r39_karten pruef treppen)
add_test(NAME unit_r39_karten_marker COMMAND probe_r39_karten pruef marker)
set_tests_properties(unit_r39_karten_1090 unit_r39_karten_b1 unit_r39_karten_teil unit_r39_karten_treppen
                     unit_r39_karten_marker PROPERTIES TIMEOUT 600)

if(TARGET re15_pc AND TARGET probe_r36_karte)
    add_test(NAME integration_r39_karten
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PROBE=$<TARGET_FILE:probe_r39_karten>
                     -DRE15_PROBE36=$<TARGET_FILE:probe_r36_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r39_karten_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_karten.cmake)
    set_tests_properties(integration_r39_karten PROPERTIES TIMEOUT 600)
endif()
