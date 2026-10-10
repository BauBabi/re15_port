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

# MESS-SONDE (kein add_test), Punkt 2: Gorilla-Blockaden in ROOM11C0 bei Leons Zufallsgang (Strecken >= 90 Bilder
# ohne Bewegung: Lage, Sicht, Klemme, Zonenspruenge, Zellen). Aufruf: test_r36_zombieki_affen_block [bilder] [seed]
add_executable(test_r36_zombieki_affen_block ${CMAKE_CURRENT_LIST_DIR}/../test_r36_zombieki_affen_block.c)
target_link_libraries(test_r36_zombieki_affen_block PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_zombieki_affen_block PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_zombieki_affen_block PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r36_zombieki_affen_block PRIVATE m)
endif()
