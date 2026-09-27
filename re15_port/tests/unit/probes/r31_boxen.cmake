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
add_executable(probe_r31_boxen probe_r31_boxen.c)
target_link_libraries(probe_r31_boxen PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r31_boxen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r31_boxen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME r31_trefferboxen COMMAND probe_r31_boxen riegel)
set_tests_properties(r31_trefferboxen PROPERTIES TIMEOUT 400)
