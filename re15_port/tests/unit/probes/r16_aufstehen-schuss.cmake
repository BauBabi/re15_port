# --- Runde 16 (2026-09-19), Thema aufstehen-schuss: Treffer waehrend des Boden-Aufstehers
#     (RE2-Zombie EXEC[5] P6/P7). Reine Messsonde, kein add_test.
#     Dossier: analysis/befunde_2026-09-19/aufstehen-schuss.md ---
add_executable(probe_r16_aufstehen_schuss probe_r16_aufstehen_schuss.c)
target_link_libraries(probe_r16_aufstehen_schuss PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_aufstehen_schuss PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_aufstehen_schuss PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
