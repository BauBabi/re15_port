# Runde 16 (2026-09-19) — Thema "trefferhoehe": Zielhoehe hoch/eben/tief gegen stehende,
# kriechende und liegende RE2-Zombies (Teile-Maske word0>>26&7 des RE2-Appliers FUN_800410CC).
# Messsonde; im Modus "pin" der ctest unit_r16_trefferhoehe_pin (Phase 2): jede Zelle
# STEHEND/KRIECHER/LIEGEND-P3/AUFSTEHEN-P6 x {W3,W8} x {HOCH,EBEN,TIEF} x 8 Distanzen gegen
# den nachgerechneten RE2-Applier (FUN_800410CC mit +0x1EE-Radius), Schadensspalten je Klammer
# und der P7-Rueckbau bei Clip-Bild 55. Dossier: analysis/befunde_2026-09-19/trefferhoehe.md §6
add_executable(probe_r16_trefferhoehe probe_r16_trefferhoehe.c)
target_link_libraries(probe_r16_trefferhoehe PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_trefferhoehe PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_trefferhoehe PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_r16_trefferhoehe_pin COMMAND probe_r16_trefferhoehe pin)
set_tests_properties(unit_r16_trefferhoehe_pin PROPERTIES TIMEOUT 180)
