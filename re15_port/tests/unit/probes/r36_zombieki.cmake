# Runde 36 (2026-10-10), Spur F "zombieki" — Riegel und Mess-Sonden, zaehlen zur Suite.
# Dossier: analysis/befunde_runde36/F_zombieki.md
#
#   unit_r36_zombieki_wecker  Punkt 1: Weck-Zensus — jeder stehende Zombie jedes Leon-Raums der Stage 1,
#                             Leon 800 vor der Nase, 480 Bilder ohne Eingabe (RE2 DECISION[0]
#                             @0x80101308-1C sofort bei Sicht+Kegel, Selbstwecker <= 405 Bilder
#                             @0x80101478-88/@0x80101544-7C). `test_r36_zombieki_wecker mess [raum]` = Tabelle.
add_executable(test_r36_zombieki_wecker ${CMAKE_CURRENT_LIST_DIR}/../test_r36_zombieki_wecker.c)
target_link_libraries(test_r36_zombieki_wecker PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_zombieki_wecker PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_zombieki_wecker PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r36_zombieki_wecker PRIVATE m)
endif()
add_test(NAME unit_r36_zombieki_wecker COMMAND test_r36_zombieki_wecker
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r36_zombieki_wecker PROPERTIES TIMEOUT 600)

#   unit_r36_zombieki_szenen  Punkt 1: Szenen nach einer Interaktion (Griff ohne Eingabe, Abwehr, 1/3/6
#                             Schuesse Handfeuerwaffe) in ROOM1010 — greift der Zombie danach erneut an,
#                             wenn Leon still vor ihm steht? `test_r36_zombieki_szenen <szene>` = Zeitlinie.
add_executable(test_r36_zombieki_szenen ${CMAKE_CURRENT_LIST_DIR}/../test_r36_zombieki_szenen.c)
target_link_libraries(test_r36_zombieki_szenen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_zombieki_szenen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_zombieki_szenen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r36_zombieki_szenen PRIVATE m)
endif()
add_test(NAME unit_r36_zombieki_szenen COMMAND test_r36_zombieki_szenen
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r36_zombieki_szenen PROPERTIES TIMEOUT 300)

#   unit_r36_zombieki_affen  Punkt 3: Gorilla-Leiche ROOM11C0 — Schattenrecord +0xBC/+0xBE = 1700/1800
#                            (@0x801171d8-f0), Lache 0x00ffff38 + 8/Takt ueber 91 Takte (FUN_8011be54
#                            @0x8011bed0-bf44) am echten Schusspfad; Gegenprobe ohne Haken = Befund.
add_executable(test_r36_zombieki_affen ${CMAKE_CURRENT_LIST_DIR}/../test_r36_zombieki_affen.c)
target_link_libraries(test_r36_zombieki_affen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_zombieki_affen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_zombieki_affen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r36_zombieki_affen PRIVATE m)
endif()
add_test(NAME unit_r36_zombieki_affen COMMAND test_r36_zombieki_affen
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r36_zombieki_affen PROPERTIES TIMEOUT 120)

#   unit_r36_zombieki_affen_block  Punkt 2: Gorilla "blockiert" in ROOM11C0 — drei Szenen Jagd ohne Sicht an der Klemme
#                                  (Gegenprobe = Original-Regel A[3] @0x80117b40-c74 ohne Ausgang) gegen die PORT-WAHL
#                                  Pad-Suche (re15_zombieki_r36.h) + Zufallsgang 6000 Bilder ohne Stillstand.
#                                  Sonden: `fest <x> <z> [bilder] [gx gz [+0x7]]` = Szene, `[bilder] [seed] [0|1]` = Zufallsgang.
add_executable(test_r36_zombieki_affen_block ${CMAKE_CURRENT_LIST_DIR}/../test_r36_zombieki_affen_block.c)
target_link_libraries(test_r36_zombieki_affen_block PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_zombieki_affen_block PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_zombieki_affen_block PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r36_zombieki_affen_block PRIVATE m)
endif()

#   unit_r36_zombieki_sicht  Punkt 1: Sicht-Zensus — Raster von Leon-Plaetzen im Vorderkegel jedes stehenden Zombies
#                            (Stage 1, Leon-Raeume), je Platz Zell-Strahl (Slab) / RE1.5-Strahl (Nibble 3) / Reaktion
#                            in 480 Bildern. `test_r36_zombieki_sicht mess [raum] [bilder]` = Tabelle.
add_executable(test_r36_zombieki_sicht ${CMAKE_CURRENT_LIST_DIR}/../test_r36_zombieki_sicht.c)
target_link_libraries(test_r36_zombieki_sicht PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_zombieki_sicht PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_zombieki_sicht PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r36_zombieki_sicht PRIVATE m)
endif()
add_test(NAME unit_r36_zombieki_sicht COMMAND test_r36_zombieki_sicht
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r36_zombieki_sicht PROPERTIES TIMEOUT 900)
add_test(NAME unit_r36_zombieki_affen_block COMMAND test_r36_zombieki_affen_block riegel
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r36_zombieki_affen_block PROPERTIES TIMEOUT 600)

#   unit_r36_zombieki_eintritt  Punkt 1 (Nachbesserung 1): Eintritts-Zensus NACH LAGE — jede Tuer in einen Stage-1-
#                               Raum mit Zombies (Door_aot_set-Spawn), Leon steht bzw. geht per Pad auf jeden Zombie zu;
#                               PASSIV (nie reagiert, Leon im Kegel nah) / KLEMME (reagiert, dann auf der Stelle).
#                               `test_r36_zombieki_eintritt mess [raum] [bilder]` = Tabelle.
add_executable(test_r36_zombieki_eintritt ${CMAKE_CURRENT_LIST_DIR}/../test_r36_zombieki_eintritt.c)
target_link_libraries(test_r36_zombieki_eintritt PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_zombieki_eintritt PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_zombieki_eintritt PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r36_zombieki_eintritt PRIVATE m)
endif()
add_test(NAME unit_r36_zombieki_eintritt COMMAND test_r36_zombieki_eintritt
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r36_zombieki_eintritt PROPERTIES TIMEOUT 600)
