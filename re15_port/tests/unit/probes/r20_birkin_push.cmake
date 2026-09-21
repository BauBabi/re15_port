# --- Runde 20 (2026-09-21), Thema birkin-push: der G5-Endboss schiebt den Spieler aus dem
#     begehbaren Bereich (Nutzer: "Jetzt wurde ich von birkin rausgeschoben ausserhalb des
#     begehbaren BEreiches"). Belegt in analysis/befunde_2026-09-21/birkin-rausgeschoben.md.
#     Braucht die RE2-Bank shared_assets/RE2/CDEMD0.EMS (sonst SKIP 77). ---
add_executable(probe_r20_birkin_push probe_r20_birkin_push.c)
target_link_libraries(probe_r20_birkin_push PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r20_birkin_push PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r20_birkin_push PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r20_birkin_push_pin COMMAND probe_r20_birkin_push pin)
set_tests_properties(unit_r20_birkin_push_pin PROPERTIES TIMEOUT 300 SKIP_RETURN_CODE 77)
