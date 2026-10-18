# Runde 39 Spur Q "elzazombies" — Elza: Zombie-Durchbrueche in der Panikphase (Bank-9-Bit 97, Spur P):
# 10E1-Ueberlebende durch die 10C1-Tuer ins Treppenhaus ROOM1061 (Etage 4), 1071-Ueberlebende in ROOM1031 Cut 7,
# Kriecher durch das Rollo ROOM1031 (Original-Szene sub08) — Montage-Form wie Leon nach Irons' Tod.
# Dossier analysis/befunde_runde39/Q_elzazombies.md.
#
# MESS-WERKZEUG (kein add_test):
#   probe_r39_elzazombies_karte <karte> <raum-hex> <x> <y> <z> <gier> <cut> [bank:bit ...] [give:id]
add_executable(probe_r39_elzazombies_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r39_elzazombies_karte.c)
target_link_libraries(probe_r39_elzazombies_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r39_elzazombies_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
