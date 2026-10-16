# Spur M (Runde 38) "npckante" — NPCs (John/Kendo 0x49, Marvin 0x40, Ada 0x42) springen an Kanten hinunter und
# klettern Kanten hinauf wie im Original: Kantensonde FUN_8002d7d8 (@0x80051374 Sub 4 / @0x800516c4 Sub 5),
# Kanten-Sub 0x80052508 (0x12/0x16), Sprung-Sub 0x80052c14 (0x17/0x18). Dossier analysis/befunde_runde38/M_npckante.md.
#
# MESSWERKZEUG (kein add_test): Spielstand mit Hoehe + Kamera-Cut + Flags fuer den LADE-Weg der echten exe.
add_executable(probe_r38_npckante_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r38_npckante_karte.c)
target_link_libraries(probe_r38_npckante_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r38_npckante_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

# RIEGEL (Unit, headless am Engine-Kern mit den echten RDTs und Modellarchiven):
#   clips     Kanten-Clips 2..5 der Loco-Bank (John CDEMD1, Marvin/Ada CDEMD0): Bildzahl + Wurzel-Y
#   sonde     FUN_8002d7d8-Zwilling an den Messstellen der Vorher-Messung (1261 [29]/[15], 1260 [28]/[12], 10B1 [31]/[18])
#   john1261  Elza ROOM1261 Cut 15: Sprung Sued + Klettern Ost, nie in der Luft, Ende Band 1 am Ziel
#   ada1260   Leon ROOM1260 Cut 7: Ada UND Marvin springen + klettern, nie in der Luft
#   john10b1  Elza ROOM10B1 sub02 Cut 2: Sprung vom Band-5-Steg
#   gegen     Ada ROOM1050 ohne Kante: kein Kanten-Sub, kein Bandwechsel
add_executable(test_r38_npckante ${CMAKE_CURRENT_LIST_DIR}/../test_r38_npckante.c)
target_link_libraries(test_r38_npckante PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_npckante PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_npckante PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_teil clips sonde john1261 ada1260 john10b1 gegen)
    add_test(NAME unit_r38_npckante_${_teil} COMMAND test_r38_npckante ${_teil})
    set_tests_properties(unit_r38_npckante_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
