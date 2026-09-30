# Runde 34 Nacht (2026-09-30), Spur E "Vier neue Dokumente" (ROOM1050/1000/1020/1010).
# REINE MESS-Sonde (kein add_test): Kameras, Rueckprojektion der Nutzer-Marken, Masken ueber
# dem Ablageort, AOT-/Prop-Zensus nach dem echten Raumstart, Erreichbarkeit der Aufhebe-
# Rechtecke, echter Aktionsdruck, Raumlicht. Dossier: analysis/befunde_runde34_nacht/E_dokumente.md
add_executable(probe_r34n_e_dokumente "${CMAKE_CURRENT_LIST_DIR}/../probe_r34n_e_dokumente.c")
target_link_libraries(probe_r34n_e_dokumente PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_e_dokumente PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r34n_e_dokumente PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
# floor/hypot/lround aus <math.h>: unter Linux liegt libm nicht implizit dabei (Runde 30,
# Linux-Container-Bau "undefined reference to floor/hypot").
if(NOT WIN32)
    target_link_libraries(probe_r34n_e_dokumente PRIVATE m)
endif()

# =============================================================================
# BAU (Runde 34 Nacht, Spur E) — Riegel, zaehlen zur Suite.
#   unit_r34n_e_dokumente   Tabelle/Bildsatz, Prop + Zone je Raum und Variante, genommen -> weg,
#                           echter Aktionsdruck -> Leser -> Meldung -> Abraeumen, Speicher-
#                           Rundlauf, Sicht im Nutzer-Cut (Regions-Test), eingebackene Modelle,
#                           dokumentspezifische Vorrang-Faelle (1051 Leichen-Satz, ...)
# Dossier: analysis/befunde_runde34_nacht/E_dokumente.md (6.1, 9)
# =============================================================================
add_executable(test_r34n_e_dokumente ${CMAKE_CURRENT_LIST_DIR}/../test_r34n_e_dokumente.c)
target_link_libraries(test_r34n_e_dokumente PRIVATE re15_engine re15_test_support)
target_include_directories(test_r34n_e_dokumente PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r34n_e_dokumente PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r34n_e_dokumente PRIVATE m)
endif()
add_test(NAME unit_r34n_e_dokumente COMMAND test_r34n_e_dokumente
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r34n_e_dokumente PROPERTIES TIMEOUT 180)
