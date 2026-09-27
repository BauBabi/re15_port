# --- Runde 30 / Welle 2 (2026-09-27), Thema "die MUENDUNGSHOEHE und das fuenfte Tor".
#     Welle 1 hatte das Tor @0x8004716C-A4 gelesen, aber NICHT gebaut: ihm fehlte die
#     Zielhoehe `lw a0,4(s4)` @0x8004718C. Welle 2 stellt sie engine-seitig her
#     (re15_player_muzzle_world = Bone 11 der Kette @0x80042E60-94, t[1] @sp+56) und
#     misst sie auf dem echten Weg (re15_game_step + Pad, echte RDTs, RE2-Bank).
#     RIEGEL (Argument "riegel"): die Muendung MUSS headless verfuegbar sein und die
#     Hunde-Kette MUSS messbar durchlaufen — sonst waere jedes Tor darauf ein leeres Feld.
add_executable(probe_r30b_muendung probe_r30b_muendung.c)
target_link_libraries(probe_r30b_muendung PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30b_muendung PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30b_muendung PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME r30b_muendungshoehe COMMAND probe_r30b_muendung riegel)
set_tests_properties(r30b_muendungshoehe PROPERTIES TIMEOUT 300)
