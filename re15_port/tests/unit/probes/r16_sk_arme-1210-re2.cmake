# Runde 16 (2026-09-19) — Skeptiker-Gegensonde zum Thema "arme-1210-re2": EM01A-Bone-Welt-y beim
# Spawn (Bank geladen), Bone-/Mesh-Reichweite je Clip, Westwand hin und ZURUECK (Arme 1-4),
# parkender Spieler an der Wand / in der Flurmitte. Reine Messsonde, kein add_test.
# Dossier: analysis/befunde_2026-09-19/arme-1210-re2.md (+ .skeptiker.md)
add_executable(probe_r16_sk_arme_1210_re2 probe_r16_sk_arme_1210_re2.c)
target_link_libraries(probe_r16_sk_arme_1210_re2 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_sk_arme_1210_re2 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_sk_arme_1210_re2 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
