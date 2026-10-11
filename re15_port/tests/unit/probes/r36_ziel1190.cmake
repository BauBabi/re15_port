# Runde 36 Spur H "ziel1190" — Zielscheiben-Texte HINTEN an den Scheiben (ROOM1190 Leon / ROOM1191 Elza
# vertauscht), Knoepfe wieder Original. Dossier analysis/befunde_runde36/H_ziel1190.md, Konstanten
# include/re15_ziel1190.h.

# Mechanik-Riegel: echte ROOM1190/1191.RDT, Raumaufbau wie im Spiel (Installer-Block), Aktion an den GEMESSENEN
# Haltepositionen vor jeder Scheibe (2 Raeume x 3 Raumzustaende x 4 Scheiben), Knopf-Seite gegen die
# Original-Nachrichten, Platz faehrt mit der Scheibe (Fahrt per Knopf, Betreten mit Scheibe vorn).
add_executable(test_r36_ziel1190 test_r36_ziel1190.c)
target_link_libraries(test_r36_ziel1190 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_ziel1190 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_ziel1190 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r36_ziel1190 COMMAND test_r36_ziel1190)
set_tests_properties(unit_r36_ziel1190 PROPERTIES TIMEOUT 120)

# MESS-WERKZEUG (kein add_test): Speicherkarte im Schiessstand fuer den exe-Riegel (Leon ROOM1190 / Elza
# ROOM1191 ueber den Charakter des Spielstands, CONTINUE-Weg).
add_executable(probe_r36_ziel1190_karte probe_r36_ziel1190_karte.c)
target_link_libraries(probe_r36_ziel1190_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_ziel1190_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

# exe-Riegel ueber den LADE-Weg (Speicherkarte Leon 1190 / Elza 1191, CONTINUE, echter Laufweg zu jeder
# Scheibe, Knopf 0 = Original): tests/integration/test_r36_ziel1190_scheiben.cmake.
if(TARGET re15_pc)
    add_test(NAME integration_r36_ziel1190_scheiben
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_ziel1190_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r36_ziel1190_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r36_ziel1190_scheiben.cmake)
    set_tests_properties(integration_r36_ziel1190_scheiben PROPERTIES TIMEOUT 1500)
endif()
