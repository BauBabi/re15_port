# --- Phase 2 (2026-09-19), Thema liegende-und-aufstehen ---------------------------------------
#     Pins zu den Dossiers analysis/befunde_2026-09-19/liegende-zombies.md (Fix-Plan Schritt 3)
#     und analysis/befunde_2026-09-19/aufstehen-schuss.md (Fix-Plan Schritt 8).
#     Beide fahren den ECHTEN Weg (ROOM1140/10E0.RDT + raum-eigenes sub00, RE2-Baenke aus
#     shared_assets/RE2/CDEMD0.EMS, Pistole ueber das Pad R1/SQUARE durch re15_game_step).

# (1) Der passive Liegende (Deskriptor 0x87/0x88, Nibble 7/8 + Bit 0x80) ist im RE2-Flavor
#     UNSCHIESSBAR (RE1.5 @0x80103AAC-AB8, RE2 @0x80047138-64) — mit Gegenproben (P) Fresser
#     bleibt treffbar, (N) Skript-Bump auf 0x89 weckt ihn und macht ihn treffbar, und der
#     RE1.5-Flavor als Regressionswache.
add_executable(test_r16_liegende_unschiessbar test_r16_liegende_unschiessbar.c)
target_link_libraries(test_r16_liegende_unschiessbar PRIVATE re15_engine re15_test_support)
target_include_directories(test_r16_liegende_unschiessbar PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r16_liegende_unschiessbar PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_r16_liegende_unschiessbar COMMAND test_r16_liegende_unschiessbar)

# (2) Treffer WAEHREND des Boden-Aufstehers (EXEC[5] P7 Clip 8/9, EXEC[8] P4 Clip 0x15)
#     laufen ueber FUN_80107A78 (Weiche @0x80105014-38): der Clip laeuft weiter, kein
#     Neustart ueber 0x60501, kein Haupt-Treffer; der Exit stellt +0x22C wieder her (+0x6+1).
add_executable(test_r16_aufstehen_schuss test_r16_aufstehen_schuss.c)
target_link_libraries(test_r16_aufstehen_schuss PRIVATE re15_engine re15_test_support)
target_include_directories(test_r16_aufstehen_schuss PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r16_aufstehen_schuss PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_r16_aufstehen_schuss COMMAND test_r16_aufstehen_schuss)
