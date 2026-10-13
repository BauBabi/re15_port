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

#   unit_r37_kiste2feld   Engine-Riegel (test_r37_kiste2feld.c): (A) Zeilen-Op-Vertrag fuer ALLE Ids 1..255 auf der
#                         Auswahlzeile der echten Kisten-FSM — 2-Feld 49x19 mit Quell-Spanne u 80 (RE2 POLY_FT4 u 40..120,
#                         @0x800717b4/@0x8007183c), schmal 25x19 mit Spanne 40 (@0x80071844); (B) Gitter: jede Zelle einer
#                         2-Feld-Waffe traegt ihre eigene Breitbild-Haelfte (Kopf 0..39, Schwanz 40..79; FUN_80049390
#                         MoveImage je Zelle @0x8004945c), Kopf in jeder Speicher-Spalte; (C) GPU-Abtastung 80->49, 30->19,
#                         40->25 = geschlossene Form, streng steigend (keine wiederholte Spalte).
add_executable(test_r37_kiste2feld ${CMAKE_CURRENT_LIST_DIR}/../test_r37_kiste2feld.c)
target_link_libraries(test_r37_kiste2feld PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_kiste2feld PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(test_r37_kiste2feld PRIVATE m)
endif()
add_test(NAME unit_r37_kiste2feld COMMAND test_r37_kiste2feld)
set_tests_properties(unit_r37_kiste2feld PROPERTIES TIMEOUT 120)

#   integration_r37_kiste2feld   echte exe, echter Weg (Spielstand, CONTINUE, Viereck am Kisten-AOT bzw. Statusschirm),
#                                RE15_FRAMEDUMP (RE15_WINDOW_SCALE=1) -> probe_r37_kiste2feld_bild: Kisten-Liste Leon
#                                1150 (4 Bilder, alle sechs 2-Feld-Ids + zwei schmale) und Elza 1151, Inventar-Gitter
#                                im Kisten- und Statusschirm (Kopf in jeder Speicher-Spalte), Ausruest-Kasten mit der MC51
#                                (Kopf in Speicher-Spalte 2) — je Symbol/Haelfte pixelgleich mit der Quelle.
if(TARGET re15_pc)
    add_test(NAME integration_r37_kiste2feld
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r37_kiste2feld_karte>
                     -DRE15_BILD_TOOL=$<TARGET_FILE:probe_r37_kiste2feld_bild>
                     -DRE15_ASSETS=${CMAKE_SOURCE_DIR}/shared_assets/PSX
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r37_kiste2feld_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_kiste2feld.cmake)
    set_tests_properties(integration_r37_kiste2feld PROPERTIES TIMEOUT 900)
endif()
