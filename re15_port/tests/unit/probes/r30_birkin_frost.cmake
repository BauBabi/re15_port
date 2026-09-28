# --- Runde 30 (2026-09-28), Spur birkin-frost: die Frost-Schranke der Birkin-Wurzel
#     (STAGE5 @0x80116a7c/@0x80116a84/@0x80116a88, STAGE3 @0x80116268/@0x80116270/@0x80116274)
#     und der einmalige Wurzelaufruf beim Spawn (Sce_em_set @0x8004256c-@0x80042608),
#     gemessen an ALLEN 13 Birkin-Spawns. Dossier: analysis/befunde_runde30/nachschliff-room5080.md,
#     Abschnitt 9. Je Spawn: (a) vor der Freigabe eingefroren, (b) Freigabe durch den Member_set
#     am Datei-Offset der Tabelle erreicht (sonst Softlock), (c) danach EMERGENCE -> WALK -> Angriff. ---
add_executable(probe_r30_birkin_frost probe_r30_birkin_frost.c)
target_link_libraries(probe_r30_birkin_frost PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_birkin_frost PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_birkin_frost PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r30_birkin_frost COMMAND probe_r30_birkin_frost)
set_tests_properties(unit_r30_birkin_frost PROPERTIES TIMEOUT 240 SKIP_RETURN_CODE 77)
