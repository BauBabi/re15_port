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
# =============================================================================
# BAU Spur G2 (Runde 34 Nacht) — ROOM1150/1151 Leuchtschrift "HEAVEN" blinkt (Opcode 0x45).
#   unit_r34n_g_maskgrp  PIN (immer an): Aufbau FUN_800392d4 + Opcode 0x45 ueber die echte VM
#                        (echte RDT-Bytes), Takt 20/20 nach echtem Raumstart 1150 UND 1151,
#                        Cut-Wechsel in einer AUS-Phase. Dossier G2_schrift1150.md §9.
# =============================================================================
add_executable(probe_r34n_g_maskgrp probe_r34n_g_maskgrp.c)
target_link_libraries(probe_r34n_g_maskgrp PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_g_maskgrp PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r34n_g_maskgrp PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r34n_g_maskgrp COMMAND probe_r34n_g_maskgrp
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r34n_g_maskgrp PROPERTIES TIMEOUT 120)
