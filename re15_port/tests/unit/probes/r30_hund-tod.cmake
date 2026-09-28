# --- Runde 30 (2026-09-28), Thema I "Hund: der Spieler stirbt nicht".
#     Dossier: analysis/befunde_runde30/hund-tod.md
#     ERMITTLUNGS-SONDE, noch KEIN add_test: der Modus "riegel" ist die Abnahme des
#     Bau-Agenten und steht vor dem Bau ROT (der Spieler steht wieder auf). Der Bau-Agent
#     schaltet add_test scharf, sobald der Riegel gruen ist (Dossier Abschnitt 5). ---
add_executable(probe_r30_hund_tod probe_r30_hund_tod.c)
target_link_libraries(probe_r30_hund_tod PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_hund_tod PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_hund_tod PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

# ZENSUS-Sonde (Dossier Abschnitt 4.4): derselbe Test fuer die uebrigen Gegner mit Griff/Biss.
add_executable(probe_r30_hund_tod_zensus probe_r30_hund_tod_zensus.c)
target_link_libraries(probe_r30_hund_tod_zensus PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_hund_tod_zensus PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_hund_tod_zensus PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
