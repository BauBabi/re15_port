# Runde 35 Spur H "raeume" — ROOM1190 Hunde-Schatten + Zielscheiben-Texte, ROOM1200 Trage-Zombie,
# ROOM1210 Gitterarme. Dossier analysis/befunde_runde35/H_raeume.md.

# Punkt 1: Hunde-Schatten auf der Boden-Referenz waehrend des Luken-Sprungs (echte ROOM1190.RDT,
# Sprungmaschine FUN_80111398 Bild fuer Bild, beide KI-Geschmaecker).
add_executable(test_r35_raeume_hundeschatten test_r35_raeume_hundeschatten.c)
target_link_libraries(test_r35_raeume_hundeschatten PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_raeume_hundeschatten PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r35_raeume_hundeschatten PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r35_raeume_hundeschatten COMMAND test_r35_raeume_hundeschatten)
set_tests_properties(unit_r35_raeume_hundeschatten PROPERTIES TIMEOUT 60)

# Punkt 2: Zielscheiben-Texte ROOM1190/1191 (echter Raumaufbau, Aktion am Platz, Text-FSM, drei
# Raumzustaende Strom aus / Strom an / nach Hunden, Gegenprobe ohne work_vars-Stempel).
add_executable(test_r35_raeume_ziel test_r35_raeume_ziel.c)
target_link_libraries(test_r35_raeume_ziel PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_raeume_ziel PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r35_raeume_ziel PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r35_raeume_ziel COMMAND test_r35_raeume_ziel)
set_tests_properties(unit_r35_raeume_ziel PROPERTIES TIMEOUT 60)

# Punkt 3: ROOM1200 Bahren-Zombie faellt auf die Spieler-Ebene — Engine-Schwerkraft FUN_8001bd60
# (Funktion an den echten Zellen + echter Weg KI, beide Geschmaecker).
add_executable(test_r35_raeume_trage test_r35_raeume_trage.c)
target_link_libraries(test_r35_raeume_trage PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_raeume_trage PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r35_raeume_trage PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r35_raeume_trage COMMAND test_r35_raeume_trage)
set_tests_properties(unit_r35_raeume_trage PROPERTIES TIMEOUT 120)
