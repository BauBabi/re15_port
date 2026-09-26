# Runde 26 (2026-09-26) — Thema "mg-blut": der MG-/Gatling-Blutstoss des RE2-Zombies.
# Riegel gegen den Nutzer-Report "Das Maschinengewehr sorgt in Resident Evil 2 fuer viel
# viel mehr Blut bei den Zombies bei treffern."
# Gemessen: Blutmenge je Treffer (MG gegen Pistole), Zahl der RNG-Zuege (fuenf eigene fuer
# den Stoss: X @0x80108000, Y @0x80108010, Z @0x80108024, sub @0x80108034,
# scale @0x8010803C), Anker Part 0 (@0x80108064), Versatz-Band und Skalierungsband
# 7000..9040 (@0x80108050). Handler 0x80107EF0 aus info/re2leon/COMMON/BIN/EMZ0.BIN.
# Dossiers: analysis/befunde_2026-09-26/mg-mehr-blut.md + pruefung-mg-mehr-blut.md
add_executable(probe_r26_mg_blut probe_r26_mg_blut.c)
target_link_libraries(probe_r26_mg_blut PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r26_mg_blut PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r26_mg_blut PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r26_mg_blut COMMAND probe_r26_mg_blut)
set_tests_properties(unit_r26_mg_blut PROPERTIES TIMEOUT 60)
