# --- Runde 37 (2026-10-12), Spur R "kiste2feld". Dossier: analysis/befunde_runde37/R_kiste2feld.md
#     Orchestrator-Auftrag R (zu Nachtrag 9): 2-Feld-Symbole (49 px, Waffen 0x0e..0x13) in der Kisten-Liste wie RE2
#     vollstaendig aus der BREITEN Kunst zeichnen (RE2: POLY_FT4 u 40..0x78 ueber zwei Cache-Kacheln, @0x8007183c /
#     @0x80071848, zweite Kachel FUN_80069c40 @0x80069cd8) — und dieselbe Kachel-Logik im Inventar-Gitter pruefen.

# Kartenwerkzeug (kein eigener Test): Spielstand vor der Kiste, Kiste mit ALLEN 2-Feld-Waffen 0x0e..0x13, Inventar
# mit vier 2-Feld-Waffen (Kopfzellen 0/2/5/8 = jede Spalte des Symbol-Speichers).
add_executable(probe_r37_kiste2feld_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r37_kiste2feld_karte.c)
target_link_libraries(probe_r37_kiste2feld_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r37_kiste2feld_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r37_kiste2feld_karte PRIVATE m)
endif()

# Bildwerkzeug (kein eigener Test): vergleicht die 2-Feld-Symbole in Framedumps der exe pixelgenau mit der Quelle
# (ITPS-Breitbild 80x30, ST_00-CLUT) — Kisten-Liste (49x19) und Inventar-Gitter (zwei 40x30-Zellen).
add_executable(probe_r37_kiste2feld_bild ${CMAKE_CURRENT_LIST_DIR}/../probe_r37_kiste2feld_bild.c)
if(NOT WIN32)
    target_link_libraries(probe_r37_kiste2feld_bild PRIVATE m)
endif()
