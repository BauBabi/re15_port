# --- Runde 30 (2026-09-28), Nachschliff Spur pfeil: die Blaetter-Pfeile des Lesers fuer
#     Bild-Dokumente (Irons Diary) nach RE2 - Lage, Wipp-Takt, Aussehen.
#     Dossier: analysis/befunde_runde30/nachschliff-pfeil.md
#
# unit_r30_pfeil = RIEGEL:
#   A  Anzeigeliste je Seite (Titel, p01..p15, Ende-Stellung) x Wipp-Stellung 0/1 = RE2s
#      Sprites (FUN_80075fd0 / FUN_800724b4, info/re2leon/PSX.EXE), keine RE1.5-Pfeile,
#   B  0 Pfeil-Pixel auf Glyphen-Pixeln der Textseite (RE2 ST0.TIM gegen FILE25_*_page);
#      Negativ-Kontrolle alte Lage (RE1.5-Emitter) = 79 Pixel auf 10 Stellungen
#      (englischer Satz, Nachtrag J; deutsch vorher 126 auf 13),
#   C  Wipp-Takt im Automaten: Aufnahme-Leser 41/39/39 (Schwelle 0x51 @0x80072800),
#      Ende-Stellung zaehlt nicht, LINKS daraus startet neu, Listen-Leser 26/24/24
#      (Schwelle 0x33 @0x8006d0e8),
#   D  RE1.5-Textleser unveraendert.
add_executable(test_r30_pfeil test_r30_pfeil.c)
target_link_libraries(test_r30_pfeil PRIVATE re15_engine re15_test_support)
target_include_directories(test_r30_pfeil PRIVATE
    ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r30_pfeil PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_r30_pfeil COMMAND test_r30_pfeil)
set_tests_properties(unit_r30_pfeil PROPERTIES TIMEOUT 120)
