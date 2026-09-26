# --- Runde 27, Thema hund-biss: "In 11d0 ... haben es die Hunde mit dem Angriff wieder
#     geschafft mich in die Wand zu treiben" / "der Spieler wurde durch den BISS der Hunde
#     in die Wand getrieben. Nicht die hunde durch den Spieler."
#     Messstand ohne add_test (er braucht die RE2-Bank shared_assets/RE2/CDEMD0.EMS). ---
add_executable(probe_r27_hund_biss probe_r27_hund_biss.c)
target_link_libraries(probe_r27_hund_biss PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r27_hund_biss PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r27_hund_biss PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
