# Runde 16 (2026-09-19) — SKEPTIKER-Gegensonde zum Dossier
# analysis/befunde_2026-09-19/tuer-animation-1040.md. Reine Messsonde (kein add_test).
# Faehrt denselben Tuer-Pfad ROOM1030 -> ROOM1040 wie probe_r16_tuer1040, rechnet aber
# pro Bild die RENDER-POSE (re15_skel_compute_pose, Composite-Skelett wie main.c) aus,
# simuliert den Fix-Plan (§4) im TESTCODE (motion=210, af=0, frac=0 nach present) und
# prueft eine Waffenbank, in der Clip 1 Bild 0 != Clip 3 Bild 0 ist (PL00W0C.PLW).
add_executable(probe_r16_sk_tuer_animation_1040 probe_r16_sk_tuer_animation_1040.c)
target_link_libraries(probe_r16_sk_tuer_animation_1040 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_sk_tuer_animation_1040 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_sk_tuer_animation_1040 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
