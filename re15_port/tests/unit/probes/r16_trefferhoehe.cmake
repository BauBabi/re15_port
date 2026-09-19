# Runde 16 (2026-09-19) — Thema "trefferhoehe": Zielhoehe hoch/eben/tief gegen stehende,
# kriechende und liegende RE2-Zombies (Teile-Maske word0>>26&7 des RE2-Appliers FUN_800410CC).
# Reine Messsonde, kein add_test. Dossier: analysis/befunde_2026-09-19/trefferhoehe.md
add_executable(probe_r16_trefferhoehe probe_r16_trefferhoehe.c)
target_link_libraries(probe_r16_trefferhoehe PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_trefferhoehe PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_trefferhoehe PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
