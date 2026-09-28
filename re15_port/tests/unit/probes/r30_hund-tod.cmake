# --- Runde 30 (2026-09-28), Thema I "Hund: der Spieler stirbt nicht".
#     Dossier: analysis/befunde_runde30/hund-tod.md
#     Die Sonde war in der Ermittlung ohne add_test (der Riegel stand ROT: der Spieler
#     stand nach dem Kehlbiss wieder auf). Seit dem Bau (Dossier Abschnitt UMSETZUNG) sind
#     beide Riegel scharf. ---
add_executable(probe_r30_hund_tod probe_r30_hund_tod.c)
target_link_libraries(probe_r30_hund_tod PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_hund_tod PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_hund_tod PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

# ZENSUS-Sonde (Dossier Abschnitt 4.4): derselbe Test fuer die uebrigen Gegner mit Griff/Biss.
add_executable(probe_r30_hund_tod_zensus probe_r30_hund_tod_zensus.c)
target_link_libraries(probe_r30_hund_tod_zensus PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_hund_tod_zensus PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_hund_tod_zensus PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

# RIEGEL 1 — der Kehlbiss toetet, mit und ohne Tastendruck. 3 Blickrichtungen x 4 Druck-Arten
# (keine Taste / Kreuz jedes zweite Bild / ein Druck / R1 gehalten), ROOM11D0, hp 20.
# Verlangt je Lauf: toedlicher Biss, hp nie wieder >= 0, Todes-Praesentation, Game Over,
# 0 Steh-Bilder bei hp < 0, 0 Neustarts des Opfer-Clips, 0 Zielpose-Bilder bei hp < 0;
# Abdeckung mindestens 6 Laeufe mit Latch. Vor dem Bau: RIEGEL ROT (30).
# Belege: Sub 7 schreibt den Spieler nicht (@0x80101F04-44, @0x8010221C-20); das Bit 0 im
# Zaehler-Fenster ist die Figuren-Nummer (lbu v0,8(s3) / andi v0,v0,0x1 @0x80102010-18).
add_test(NAME unit_r30_hund_tod COMMAND probe_r30_hund_tod riegel)
set_tests_properties(unit_r30_hund_tod PROPERTIES TIMEOUT 300 SKIP_RETURN_CODE 77)

# RIEGEL 2 — N2: wer beim Tod zielt, zielt danach nicht mehr (cmd 3 ersetzt das
# Kommando-Register, sb 3,4(s1) / sb zero,5(s1) / sb zero,6(s1) @0x80012EF0-EFC).
# Vor dem Bau: 200 von 200 Todesbildern mit aktiver Zielphase.
add_test(NAME unit_r30_hund_tod_n2 COMMAND probe_r30_hund_tod n2)
set_tests_properties(unit_r30_hund_tod_n2 PROPERTIES TIMEOUT 120 SKIP_RETURN_CODE 77)
