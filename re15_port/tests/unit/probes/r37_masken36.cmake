# Runde 37 (2026-10-12), Spur P "masken36" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde37/P_masken36.md. Orchestrator-Auftrag P (Regression 53741fe5): die Generator-Masken der
# Stages 3..6 (R15M ohne eigenen Atlas) zeichnet der Lader wieder mit dem BSS-Atlas des Cuts (re15_pri_atlas_quelle);
# Rechtecke, die eine Figur VOR jedem Objekt uebermalen, bleiben aus (engine/src/masken36_r37.c).
#
#   unit_r37_masken36_zensus      alle MSK-Dateien: je Sektion Atlas-Quelle, kein toter Cut, Blit-Pruefung (pruef_gen.py),
#                                 Gegenproben alte Regel (216 tot) und falscher Atlas (ROOM1220 faellt durch)
#   unit_r37_masken36_ausschluss  Ausschluss-Tabelle == Nachrechnung; danach keine Fehlverdeckung, Verdeckung je Stage bleibt
#   unit_r37_masken36_verdeckung  die an der exe gemessenen Szenen je Stage (HINTER verdeckt, DAVOR frei)
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
    set_tests_properties(unit_r37_masken36_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
