# Runde 37 (2026-10-12), Spur P "masken36" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde37/P_masken36.md. Orchestrator-Auftrag P (Regression 53741fe5): die Generator-Masken der
# Stages 3..6 (R15M ohne eigenen Atlas) zeichnet der Lader wieder mit dem BSS-Atlas des Cuts (re15_pri_atlas_quelle);
# Rechtecke, die eine Figur VOR jedem Objekt uebermalen, bleiben aus (engine/src/masken36_r37.c).
#
#   unit_r37_masken36_zensus      alle MSK-Dateien: je Sektion Atlas-Quelle, kein toter Cut, Blit-Pruefung (pruef_gen.py),
#                                 Gegenproben alte Regel (216 tot) und falscher Atlas (ROOM1220 faellt durch)
#   unit_r37_masken36_ausschluss  Ausschluss-Tabelle == Nachrechnung; danach keine Fehlverdeckung, Verdeckung je Stage bleibt
#   unit_r37_masken36_verdeckung  die an der exe gemessenen Szenen je Stage (HINTER verdeckt, DAVOR frei)
#   integration_r37_masken36_stage3..6  echte exe je Stage (Leon und Elza), integration_r37_masken36_nb2  Nachbesserung 2 (Cut 15, Kopf)
add_executable(test_r37_masken36 ${CMAKE_CURRENT_LIST_DIR}/../test_r37_masken36.c)
target_link_libraries(test_r37_masken36 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_masken36 PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/..)
target_compile_definitions(test_r37_masken36 PRIVATE RE15_PORT_SRC_DIR="${CMAKE_SOURCE_DIR}")
if(NOT WIN32)
    target_link_libraries(test_r37_masken36 PRIVATE m)
endif()
foreach(_teil zensus ausschluss verdeckung)
    add_test(NAME unit_r37_masken36_${_teil} COMMAND test_r37_masken36 ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    # Nachbesserung 1: ausschluss rechnet mit der Zellform aus dem Resolver (allein ~85 s) - Luft fuer Last
    set_tests_properties(unit_r37_masken36_${_teil} PROPERTIES TIMEOUT 900)
endforeach()

# Messwerkzeug der Integration (drei Framedumps: mit Masken / ohne Masken / ohne Figur).
add_executable(probe_r37_masken36_ppm ${CMAKE_CURRENT_LIST_DIR}/../probe_r37_masken36_ppm.c)

# Echte exe je Stage 3..6, Leon UND Elza: HINTER verdeckt (>= Haelfte der Figurpunkte), DAVOR frei, Log fg_atlas=1 + Zeile der Spur P.
if(TARGET re15_pc)
    foreach(_stufe 3 4 5 6)
        add_test(NAME integration_r37_masken36_stage${_stufe}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r37_masken36_ppm>
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r37_masken36_s${_stufe}_wd
                         -DSTUFE=${_stufe}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_masken36_verdeckung.cmake)
        set_tests_properties(integration_r37_masken36_stage${_stufe} PROPERTIES TIMEOUT 900)
    endforeach()
    # Nachbesserung 2 (Abnahme 1): M1 ROOM3050/3051 Cut 15 Konsolen-Cursor nicht uebermalt (Leon + Elza),
    # M2 Kopfoberkante auf dem Zufallsweg ROOM30A0 Bild 380/400 (Dossier NB2.1/NB2.2).
    add_test(NAME integration_r37_masken36_nb2
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r37_masken36_ppm>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r37_masken36_nb2_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_masken36_nb2.cmake)
    set_tests_properties(integration_r37_masken36_nb2 PROPERTIES TIMEOUT 900)
endif()
