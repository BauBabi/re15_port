# --- Runde 27, Thema hund-biss: "In 11d0 ... haben es die Hunde mit dem Angriff wieder
#     geschafft mich in die Wand zu treiben" / "der Spieler wurde durch den BISS der Hunde
#     in die Wand getrieben. Nicht die hunde durch den Spieler."
#     Ursache: dem Gegner-Schwanz fehlte `jal 0x8002aec4` @0x8010d880 (Hund aus dem Spieler).
#     probe_r27_hund_biss = Messstand (kein add_test, braucht shared_assets/RE2/CDEMD0.EMS),
#     test_r27_hund_wandtrieb = der Riegel. ---
add_executable(probe_r27_hund_biss probe_r27_hund_biss.c)
target_link_libraries(probe_r27_hund_biss PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r27_hund_biss PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r27_hund_biss PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

add_executable(test_r27_hund_wandtrieb test_r27_hund_wandtrieb.c)
target_link_libraries(test_r27_hund_wandtrieb PRIVATE re15_engine re15_test_support)
target_include_directories(test_r27_hund_wandtrieb PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r27_hund_wandtrieb PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r27_hund_wandtrieb COMMAND test_r27_hund_wandtrieb)
set_tests_properties(unit_r27_hund_wandtrieb PROPERTIES TIMEOUT 300 SKIP_RETURN_CODE 77)
