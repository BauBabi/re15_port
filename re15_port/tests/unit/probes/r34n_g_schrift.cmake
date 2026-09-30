# Spur G (Runde 34 Nacht, ROOM1170 blinkende Gebaeudeschrift) — Mess-Sonden, kein add_test.
# probe_r34n_g_bss: dekodiert jeden BSS-Chunk mit den Engine-Funktionen des Ports nach PPM.
add_executable(probe_r34n_g_bss probe_r34n_g_bss.c)
target_link_libraries(probe_r34n_g_bss PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_g_bss PRIVATE ${CMAKE_SOURCE_DIR}/include)
# Spur G2 (ROOM1150 Leuchtschrift): probe_r34n_g_sld zieht den Vordergrundatlas eines Cuts genau wie
# bg_pc.c (re15_sld_used_len + re15_sld_atlas_from_chunk) und schreibt ihn als TIM. Kein add_test.
add_executable(probe_r34n_g_sld probe_r34n_g_sld.c)
target_link_libraries(probe_r34n_g_sld PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_g_sld PRIVATE ${CMAKE_SOURCE_DIR}/include)
# Spur G2: probe_r34n_g_karte schreibt einen Spielstand in ROOM1150/1151 mit camera_cut 2 (Lade-Weg-
# Messung der echten exe, RE15_CONTINUE_TEST). Mess-Werkzeug, kein add_test.
add_executable(probe_r34n_g_karte probe_r34n_g_karte.c)
target_link_libraries(probe_r34n_g_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_g_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
