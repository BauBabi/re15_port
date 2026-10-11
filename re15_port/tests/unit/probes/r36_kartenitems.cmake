# Runde 36 (2026-10-10), Spur N "kartenitems" — Lageplaene (RE2 karte01) zum Aufsammeln, Etagenkarte
# freischalten wie in RE2. Dossier: analysis/befunde_runde36/N_kartenitems.md
#
#   unit_r36_kartenitems   Tabelle/Modelle/Texte, Raumstart Leon+Elza (Prop obj 15, EVENT-Satz Ereignis 28,
#                          ROOM1030/1031 Muenztelefon -> Slot 39), Bit gesetzt -> nichts, echter Druck
#                          (Bild-Ablauf re15_game_step) -> Frage -> Ja/Nein -> Bit/Zone/Modell/Meldung,
#                          Kartenblatt ohne/mit Plan (RE2 FUN_8006E120) + Etagenwahl (Bank 35).
add_executable(test_r36_kartenitems ${CMAKE_CURRENT_LIST_DIR}/../test_r36_kartenitems.c)
target_link_libraries(test_r36_kartenitems PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_kartenitems PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_kartenitems PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r36_kartenitems PRIVATE m)
endif()
add_test(NAME unit_r36_kartenitems COMMAND test_r36_kartenitems
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r36_kartenitems PROPERTIES TIMEOUT 300)
