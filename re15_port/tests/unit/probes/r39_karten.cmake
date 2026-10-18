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
#                            Vorlagen (0 Abweichungen), B1..3F ohne Doppelwand; ROOM1090 oben am Nutzerpunkt: Marker-Ring
#                            im Rot des aktuellen Raums
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
set_tests_properties(unit_r39_karten_1090 unit_r39_karten_b1 PROPERTIES TIMEOUT 600)

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
