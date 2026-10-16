# Spur M (Runde 38) "npckante" — NPCs (John/Kendo 0x49, Marvin 0x40, Ada 0x42) springen an Kanten hinunter und
# klettern Kanten hinauf wie im Original: Kantensonde FUN_8002d7d8 (@0x80051374 Sub 4 / @0x800516c4 Sub 5),
# Kanten-Sub 0x80052508 (0x12/0x16), Sprung-Sub 0x80052c14 (0x17/0x18). Dossier analysis/befunde_runde38/M_npckante.md.
#
# MESSWERKZEUG (kein add_test): Spielstand mit Hoehe + Kamera-Cut + Flags fuer den LADE-Weg der echten exe.
add_executable(probe_r38_npckante_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r38_npckante_karte.c)
target_link_libraries(probe_r38_npckante_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r38_npckante_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
