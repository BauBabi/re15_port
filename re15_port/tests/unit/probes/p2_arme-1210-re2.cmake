# Runde 16 / Phase 2 (2026-09-19) — Thema "arme-1210-re2": die ROOM1210-Gitterhaende fahren
# unter dem RE2-Flavor die RE2-Zellenarm-Maschine (Typ 0x2D) auf dem RE2-Modell EM2D.
# Dossier: analysis/befunde_2026-09-19/arme-1210-re2.md (Abschnitt 6 = Umsetzung).
# Die Mess-Sonde probe_r16_arme_1210_re2 (r16_arme-1210-re2.cmake) traegt seit Phase 2 den
# Teil D (RE2-Lauf); hier steht der PIN.
add_executable(test_p2_1210_arme_re2 test_p2_1210_arme_re2.c)
target_link_libraries(test_p2_1210_arme_re2 PRIVATE re15_engine re15_test_support)
target_include_directories(test_p2_1210_arme_re2 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_p2_1210_arme_re2 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_1210_arme_re2 COMMAND test_p2_1210_arme_re2)
set_tests_properties(unit_1210_arme_re2 PROPERTIES TIMEOUT 120)
