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
