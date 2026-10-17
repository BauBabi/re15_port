# --- Runde 30 / Welle 2 (2026-09-27), Thema "DIE TREFFERBOXEN ALLER RE2-TYPEN".
#     Nutzer-Auftrag: "Und die Trefferboxen wenn die fehlen muessen natuerlich auch
#     ermttelt und uebernommen werden. Das soll alles sauber Resident Evil 2 entsprechen."
#     RIEGEL (Argument "riegel"):
#       * jeder der fuenf RE2-Typen traegt nach dem INIT genau die Original-Werte
#         (Vollscan `sh rt,152/158(rs)` ueber die fuenf Overlays, Adressen im Quelltext);
#       * re2_hit_box_set ist NUR beim Hund 0x20 gesetzt — die vier anderen fielen
#         gemessen 0/240 durch das fuenfte Tor und waeren damit dauerhaft untreffbar;
#       * die KRIECHER-Box des Zombies ist KEINE Sackgasse: die Rampe @0x8010366C-94
#         zieht sie messbar wieder auf -1500/1500;
#       * kein Typ wurde durch die Aenderung zugemacht (Treffer > 0 je Typ).
#     RUNDE 38 (Integration, Schachtspinnen der Spur R in ROOM2060/20A0):
#       * Zensus ueber JEDE ausgelieferte Spinne (+ ROOM2090); ausserhalb des Schacht-Wartens
#         traegt keine die Unterkante 0;
#       * genau die vier Sce_em_set-Saetze mit grid 0x41 warten im Schacht (Zustand 4/1) mit
#         leerem RE2-Kasten;
#       * je Schachtspinne: Original-Ausloeser (work[0x0A] -> sub01 -> Member_set 0x0C 0x42),
#         Landung + RE2-INIT mit -1400/1400 (Tabelle @0x801063A0), danach Treffer > 0.
#     NICHT im Riegel: die Treffbarkeit IM Schacht. Das Original zielt und trifft dort nicht
#     (FUN_8003703c grid & 0x60 @0x8003710c, Resolver-Band @0x800120e4-ec), der Port schon —
#     gemeldeter Spielfehler, gemessen in jedem Lauf; Urteil: `probe_r31_boxen versteckt`.
add_executable(probe_r31_boxen probe_r31_boxen.c)
target_link_libraries(probe_r31_boxen PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r31_boxen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r31_boxen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME r31_trefferboxen COMMAND probe_r31_boxen riegel)
set_tests_properties(r31_trefferboxen PROPERTIES TIMEOUT 400)
