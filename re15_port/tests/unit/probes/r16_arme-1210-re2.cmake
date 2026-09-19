# Runde 16 (2026-09-19) — Thema "arme-1210-re2": Port-Ist der ROOM1210-Gitterhaende (Lauf durch den
# Flur, Zustaende/Clips/Positionen je Bild, Sichtbarkeit im Cut-Viereck) + Geometrie des RE2-Vorbilds
# EM2D (CDEMD0.EMS kind 0x2D, Hand-Reichweite je Clip). Reine Messsonde, kein add_test.
# Dossier: analysis/befunde_2026-09-19/arme-1210-re2.md
add_executable(probe_r16_arme_1210_re2 probe_r16_arme_1210_re2.c)
target_link_libraries(probe_r16_arme_1210_re2 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_arme_1210_re2 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_arme_1210_re2 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
