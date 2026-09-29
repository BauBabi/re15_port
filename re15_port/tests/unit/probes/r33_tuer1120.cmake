# Runde 33 (2026-09-29), Thema R: Tuer ROOM1130 -> ROOM1120 erst nach der ersten Irons-Szene
# (ROOM1150 sub08 setzt Flag (3,94) @Datei 0x01110). PORT-WAHL auf Nutzerwunsch, gebaut mit dem
# RE1.5-eigenen Text-Platz-Mechanismus (ROOM1170 @0x01318, ROOM1130 sub01 @0x00A1C).
# Dossier: analysis/befunde_runde33/tuer_1130_1120.md
# RIEGEL, vier Teile: gesperrt (Meldung, kein Wechsel, keine Tuersequenz), frei (Flag gesetzt ->
# ROOM1120 + Sequenz S060), szene (sub08 im Port-VM setzt das Flag selbst -> frei), elza
# (ROOM1131 bleibt offen).
add_executable(test_r33_tuer1120 ${CMAKE_CURRENT_SOURCE_DIR}/test_r33_tuer1120.c)
target_link_libraries(test_r33_tuer1120 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r33_tuer1120 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r33_tuer1120 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r33_tuer1120_gesperrt COMMAND test_r33_tuer1120 gesperrt)
add_test(NAME unit_r33_tuer1120_frei     COMMAND test_r33_tuer1120 frei)
add_test(NAME unit_r33_tuer1120_szene    COMMAND test_r33_tuer1120 szene)
add_test(NAME unit_r33_tuer1120_elza     COMMAND test_r33_tuer1120 elza)
set_tests_properties(unit_r33_tuer1120_gesperrt unit_r33_tuer1120_frei unit_r33_tuer1120_szene
    unit_r33_tuer1120_elza PROPERTIES TIMEOUT 120)
