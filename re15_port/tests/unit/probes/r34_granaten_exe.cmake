# Runde 34 (Granaten) — Integration W9 (analysis/befunde_runde34_granaten/integration.md).
#
#   integration_r34_granaten   echte exe, ROOM1140 per RE15_DEBUG_JUMP: alle drei Granaten (0x09/0x0A/0x0B)
#                              je RE2- und RE1.5-KI gegen einen aufgestandenen Zombie 0x10 — kein Schaden im
#                              Abzugsbild, Explosion im Bild Liegen + 36, Treffer im Explosionsbild mit der
#                              Reaktionszeile je KI, Reaktion im Folgebild, Leiche statt Haenger, exe laeuft
#                              bis RE15_EXIT_AT; dazu der Item-Debug-Weg des Statusschirms (SELECT, 9x R1)
#                              und ein Abzugs-Lauf mit Zombie in <= 1000; NACHBESSERUNG: derselbe MITTE-
#                              Wurf mit/ohne Leon im Bild -> gr.log byte-gleich (Anker M-HE-1). Skript und Belege:
#                              tests/integration/test_r34_granaten.cmake.
# MESS-WERKZEUG (kein add_test): wirksamer Effekt-Beitrag aus zwei Framedumps (Integration W8).
add_executable(probe_r34_ppm_beitrag ${CMAKE_CURRENT_LIST_DIR}/../probe_r34_ppm_beitrag.c)

if(TARGET re15_pc)
    add_test(NAME integration_r34_granaten
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r34_ppm_beitrag>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r34_granaten_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r34_granaten.cmake)
    set_tests_properties(integration_r34_granaten PROPERTIES TIMEOUT 1200)
endif()
