# Runde 41 (2026-10-24), Spur M "masken36" — Riegel, zaehlen zur Suite. Dossier: analysis/befunde_runde41/M_masken36.md.
#
# Punkt 1 (AUFTRAG.md): "Die von dir erstellten Masken fuer STAGE3-6 sind relativ unbrauchbar, weil die ueberall Loecher
# aufweisen." -> "danach kannst du deine Masken schliessen in der naechsten Version."
#   unit_r41_masken36_tiefe   Tiefe aus der Erreichbarkeit (engine/src/masken36_r41.c, gen/masken36_tiefe_r41.inc) ==
#                             Nachrechnung aus MSK/eigenem Atlas/RDT (Werkzeug test_r37_masken36 erzeuge_tiefe), keine
#                             verwaiste Zeile, Gegenprobe: ohne Tabelle verdeckten Rechtecke Figuren VOR dem Objekt.
add_test(NAME unit_r41_masken36_tiefe COMMAND test_r37_masken36 tiefe
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_masken36_tiefe PROPERTIES TIMEOUT 900)
#   unit_r41_masken36_loecher  Mechanik des Schliessens an allen 216 Generator-Sektionen (test_r41_masken36.c): eigener Atlas,
#                             den der Lader nimmt; offene Kerben (Kuenstler-Atlas deckend + farbgleich direkt an der Kante
#                             eines alten Rechtecks, nicht gezeichnet) NACHHER <= VORHER/10, Gegenprobe VORHER > 10000; Blit.
add_executable(test_r41_masken36 ${CMAKE_CURRENT_LIST_DIR}/../test_r41_masken36.c)
target_link_libraries(test_r41_masken36 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_masken36 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_masken36 PRIVATE RE15_PORT_SRC_DIR="${CMAKE_SOURCE_DIR}")
if(NOT WIN32)
    target_link_libraries(test_r41_masken36 PRIVATE m)
endif()
add_test(NAME unit_r41_masken36_loecher COMMAND test_r41_masken36 loecher WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_masken36_loecher PROPERTIES TIMEOUT 600)
# Punkt 2 (AUFTRAG.md): "Ich habe diverse neue PRIs fuer STAGE2 erstellt. die sollen alle sauber eingebaut werden!"
#   unit_r41_masken36_kurbel  ROOM2020 Cut 3 an der Kurbel (Pos_set @0x0A8C (3650,-1800,-2925)): die Sichtlinie Kamera ->
#                             Leon kreuzt die solide Zelle der Suedreling (Leon steht HINTER ihr), das Original hat in Cut 3
#                             keine Masken (NULL-Sektion), die Reling-Maske verdeckt die Beine nach dem Original-Urteil.
add_test(NAME unit_r41_masken36_kurbel COMMAND test_r41_masken36 kurbel WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
