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
