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
#     RUNDE 38 INTEGRATION — Riegel r31_trefferboxen_versteckt (Argument "versteckt"): die Spinne IM
#     Schacht ist wie im Original kein Ziel, RE2- UND RE1.5-KI:
#       * wartend/kriechend: Zielhilfe waehlt sie nicht (FUN_8003703c grid & 0x60 `andi v0,a0,0x60` /
#         `bne` @0x8003710c-10), der Resolver nimmt sie nicht (leeres Band @0x800120e4-ec, INIT
#         @0x80110d38-48, Stempel nur ACTIVE @0x80110fd0; Latch (+0x93&3)==3 @0x80114f7c-88 /
#         @0x80012100 in jedem Ausstiegsbild), Salven treffen nicht, sie landet;
#       * Grenze Phase 8 / erstes ACTIVE: direkt nach Phase 8 kein Kandidat (Original word0 0x00000801),
#         nach dem ersten ACTIVE-/RE2-INIT-Takt Kandidat + Zielhilfe (0x60000801); danach Treffer, kein
#         Bild ueber dem Boden;
#       * Tuerweg 2030 -> 2060 (Leon) / 2031 -> 2061 (Elza): Blick bleibt 0x800, kein Treffer;
#       * Zensus grid & 0x60 (Hund, Affe, Spiderman, Zombie, Gorilla, Birkin, Kraehe): versteckt nicht
#         gewaehlt, nach der Original-Freigabe wieder gewaehlt (bzw. ohne Freigabe nie).
#     Gegenprobe: Stand 7fff66c5 (vor dem Fix) -> FEHLGESCHLAGEN (Abweichungen im Dossier R_spinnen.md).
add_executable(probe_r31_boxen probe_r31_boxen.c)
target_link_libraries(probe_r31_boxen PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r31_boxen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r31_boxen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME r31_trefferboxen COMMAND probe_r31_boxen riegel)
set_tests_properties(r31_trefferboxen PROPERTIES TIMEOUT 400)
add_test(NAME r31_trefferboxen_versteckt COMMAND probe_r31_boxen versteckt)
set_tests_properties(r31_trefferboxen_versteckt PROPERTIES TIMEOUT 400)
