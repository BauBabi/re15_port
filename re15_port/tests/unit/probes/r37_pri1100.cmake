# Runde 37 (2026-10-12), Spur A "pri1100" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde37/A_pri1100.md.
#
# Nutzer (AUFTRAG.md #1): "Room 1100 das Problem ist immer noch da, siehe Zombie auf den F9 Marker."
# Marke befund_1100_F892_marke1.bmp: ROOM1100 Cut 1, Zombie 1 liegt an der Suedecke, Kopf und Arm stecken
# HINTER der Westwand (Kopf in der Kollisionszelle Suedblock) und wurden ueber die Wand gemalt. Ursache: die Gitter-
# kacheln der alten Maskenkette trugen das MAXIMUM der Wandtiefe (Kachel (166,129) t=161 bei Wandtiefe
# 7118..10275). Jetzt Phase-2-Bau (Rechtecke je Tiefenstufe, Cut 1 Stufe 4 / Cut 2 Stufe 3).
#
#   unit_r37_pri1100_leck    Cut 1 und Cut 2 ueber die ECHTEN Leser: jeder Weltpunkt eines Gitters, der mehr als
#                            (Stufe+1) Buckets HINTER der Westwand-Ostflaeche liegt, ist verdeckt (Original-Urteil
#                            re15_pri_mask_occludes, @0x8002565c / ZSF3 @0x80066c70, Masken-OT @0x80039650-60);
#                            kein Punkt VOR der Flaeche ist verdeckt.
#   unit_r37_pri1100_marke   die gemessenen Kopf-/Armdreiecke von Zombie 1 (Marken F240/F892, RE15_TRILOG) liegen
#                            unter der Maske (mit den v0.8.24-Masken: alle frei, Schluessel 10378).
#   integration_r37_pri1100_marke  (nur mit re15_pc) die echte exe, Szene der Marke F892 (Leon (-14914,-25645) rot 392,
#                            Zombie 1 per RE15_GEGNER_POS auf (-16157,-23876) rot 1024) und der Marke F240 (Zombie 1
#                            an der Spawnlage): im Wandfenster rechts der Ecke (x 168..191, y 126..149, ganz in der
#                            Silhouette) ist das Bild MIT Masken gleich dem Bild ohne Zombie (0 Punkte > 24), OHNE
#                            Masken nicht (der Zombie liegt wirklich dort) - sonst waere das Gruen leer.
add_executable(test_r37_pri1100 ${CMAKE_CURRENT_LIST_DIR}/../test_r37_pri1100.c)
target_link_libraries(test_r37_pri1100 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_pri1100 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r37_pri1100 PRIVATE RE15_PORT_SRC_DIR="${CMAKE_SOURCE_DIR}")
if(NOT WIN32)
    target_link_libraries(test_r37_pri1100 PRIVATE m)
endif()
#
# Nachbesserung 1 (Abnahme A#0 M1, VERTRAG.md 1.7): Elzas ROOM1101 (RDT bytegleich zu ROOM1100, Pflichtweg 10D1 -> 1101)
# hatte KEINE Masken (der Port sucht MASKS/ROOM%04X.MSK mit der vollen Id). Jetzt re15_pri_masken_raum (Masken-Zwilling).
#   unit_r37_pri1100_zwilling  Zensus aller Paare STAGE1..6 aus den Dateien (Leon-MSK da, Elza-MSK nicht, Kamerasaetze
#                            RDT+0x24 Satz+0x00..+0x1B gleich, Hintergrund je Cut gleich) == Tabelle re15_pri_masken_raum.
#   unit_r37_pri1100_elza    leck + marke fuer ROOM1101: Kamera aus ROOM1101.RDT, Masken ueber den Zwilling.
#   integration_r37_pri1100_elza  echte exe als Elza auf dem Tuerweg 10D1 -> 1101 (Cut 1 von selbst): Leon-Masken
#                            geladen, Wandfenster gleich "ohne Zombie", ohne Masken nicht.
foreach(_teil leck marke zwilling elza)
    add_test(NAME unit_r37_pri1100_${_teil} COMMAND test_r37_pri1100 ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r37_pri1100_${_teil} PROPERTIES TIMEOUT 120)
endforeach()

if(TARGET re15_pc AND TARGET probe_r36_ppm_diff)
    add_test(NAME integration_r37_pri1100_marke
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r36_ppm_diff>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r37_pri1100_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_pri1100_marke.cmake)
    set_tests_properties(integration_r37_pri1100_marke PROPERTIES TIMEOUT 600)
    add_test(NAME integration_r37_pri1100_elza
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r36_ppm_diff>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r37_pri1100_elza_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_pri1100_elza.cmake)
    set_tests_properties(integration_r37_pri1100_elza PROPERTIES TIMEOUT 600)
endif()
