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
