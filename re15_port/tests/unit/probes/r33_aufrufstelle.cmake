# --- Runde 33 (2026-09-27), Thema "die ZWEITE Aufrufstelle des Kandidatenfilters".
#     Aufgeloest: Teil+0x5C = MATRIX.t der WELT-Matrix von Teil 20 (Projektil-Arbeitsplatz,
#     `addiu s5,s3,92` @0x80046398); Aufrufstelle (A) @0x80042F94 = Waffe 1 (Messer),
#     (B) @0x800467C0 = Waffe 12 (Bolzen); +0x14D = BILDNUMMER im Clip (@0x80029B28-34);
#     Leser der Haltungsklasse (word0>>26)&7 = FUN_800410CC @0x800413C4-D8.
#     GEBAUT: die Haltungsklasse des RE2-Hundes aus FUN_80104088 @0x80104090-D8.
#     RIEGEL (Argument "riegel"): liegend genau 1 / stehend genau 3, kein Typ in 900 Bildern
#     untreffbar, und die KONTROLLE mit genullter Trefferbox MUSS auf 0 Treffer fallen —
#     sonst misst die Sonde das fuenfte Tor gar nicht.
add_executable(probe_r33_aufrufstelle probe_r33_aufrufstelle.c)
target_link_libraries(probe_r33_aufrufstelle PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r33_aufrufstelle PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r33_aufrufstelle PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME r33_aufrufstelle COMMAND probe_r33_aufrufstelle riegel)
set_tests_properties(r33_aufrufstelle PROPERTIES TIMEOUT 900)
