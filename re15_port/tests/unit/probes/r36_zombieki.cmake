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
