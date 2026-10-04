# Runde 35 (2026-10-03), Spur F "inhalt" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde35/F_inhalt.md
#
#   unit_r35_inhalt_doku   Punkt 2: Codes 4312/5632 in FILE28/FILE29 im Textmaler-Gruen
#                          (Steuerbyte 05 01 -> TEX.TIM CLUT-Zeile 2, @0x80028974-94), gemessen
#                          ueber den echten Seitenlader re15_re2doc_pixel.
add_executable(test_r35_inhalt_doku ${CMAKE_CURRENT_LIST_DIR}/../test_r35_inhalt_doku.c)
target_link_libraries(test_r35_inhalt_doku PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_inhalt_doku PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r35_inhalt_doku PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_r35_inhalt_doku COMMAND test_r35_inhalt_doku
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r35_inhalt_doku PROPERTIES TIMEOUT 60)

#   unit_r35_inhalt_zombies  Punkt 3: Zombies ROOM1010/1220 weiter von der Eintrittstuer — echte
#                            Spielschleife je Eintritt: STEHEN (Bild des ersten Griffs) und FLUCHT
#                            (sofort umdrehen + VIERECK an der Tuer: raus oder gegriffen), Original
#                            gegen Port im selben Prozess. `test_r35_inhalt_zombies mess` = Tabelle.
add_executable(test_r35_inhalt_zombies ${CMAKE_CURRENT_LIST_DIR}/../test_r35_inhalt_zombies.c)
target_link_libraries(test_r35_inhalt_zombies PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_inhalt_zombies PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r35_inhalt_zombies PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r35_inhalt_zombies PRIVATE m)
endif()
add_test(NAME unit_r35_inhalt_zombies COMMAND test_r35_inhalt_zombies
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r35_inhalt_zombies PROPERTIES TIMEOUT 300)

# MESS-WERKZEUG (kein add_test): Speicherkarte an frei gewaehlter Stelle fuer Laeufe mit der echten exe.
add_executable(probe_r35_inhalt_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r35_inhalt_karte.c)
target_link_libraries(probe_r35_inhalt_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r35_inhalt_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

#   unit_r35_inhalt_items  Punkte 4/5: Memory Card im Regal ROOM1010/1011 + Shotgun Shells auf dem
#                          Aussenluefter ROOM1090/1091 — Prop/Zone nach dem echten Raumstart, Bit
#                          -> weg, Projektion in die Nutzer-Marke, echter Aktionsdruck -> Modal -> Ja.
add_executable(test_r35_inhalt_items ${CMAKE_CURRENT_LIST_DIR}/../test_r35_inhalt_items.c)
target_link_libraries(test_r35_inhalt_items PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_inhalt_items PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r35_inhalt_items PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r35_inhalt_items PRIVATE m)
endif()
add_test(NAME unit_r35_inhalt_items COMMAND test_r35_inhalt_items
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r35_inhalt_items PROPERTIES TIMEOUT 120)

# MESS-SONDE (kein add_test), Punkt 1: Griff-Symmetrie jeder Doppeltuer-Wahl der Tuer-Tabelle.
add_executable(probe_r35_inhalt_tueren ${CMAKE_CURRENT_LIST_DIR}/../probe_r35_inhalt_tueren.c)
target_link_libraries(probe_r35_inhalt_tueren PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r35_inhalt_tueren PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r35_inhalt_tueren PRIVATE
    RE15_ASSET_SHARED_DIR="${CMAKE_SOURCE_DIR}/shared_assets")
if(NOT WIN32)
    target_link_libraries(probe_r35_inhalt_tueren PRIVATE m)
endif()
#   unit_r35_inhalt_tueren  Punkt 1: Griff-Symmetrie aller Port-Doppeltueren (alt: Befund, neu: < 10 Grad,
#                           RE2-Originale unveraendert) — dieselbe Sonde mit Argument "test".
add_test(NAME unit_r35_inhalt_tueren COMMAND probe_r35_inhalt_tueren test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r35_inhalt_tueren PROPERTIES TIMEOUT 120)
