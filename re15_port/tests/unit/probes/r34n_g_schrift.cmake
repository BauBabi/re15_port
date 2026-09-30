# Spur G (Runde 34 Nacht, ROOM1170 blinkende Gebaeudeschrift) — Mess-Sonden, kein add_test.
# probe_r34n_g_bss: dekodiert jeden BSS-Chunk mit den Engine-Funktionen des Ports nach PPM.
add_executable(probe_r34n_g_bss probe_r34n_g_bss.c)
target_link_libraries(probe_r34n_g_bss PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_g_bss PRIVATE ${CMAKE_SOURCE_DIR}/include)
