# Runde 17 (2026-09-20), Thema "ada-und-weste".
# Dossier: analysis/befunde_2026-09-20/ada-und-weste.md
#
# probe_r17_weste_karte = MESS-WERKZEUG (kein add_test): schreibt eine Speicherkarte
#   mit angelegter R.P.D.-Weste, damit die echte exe per RE15_CONTINUE_TEST in genau
#   den Nutzer-Zustand geladen werden kann.
add_executable(probe_r17_weste_karte probe_r17_weste_karte.c)
target_link_libraries(probe_r17_weste_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r17_weste_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

# PIN integration_weste_load_pin: faehrt die ECHTE exe per CONTINUE in einen Spielstand
#   mit angelegter R.P.D.-Weste und prueft, dass der Lade-Weg das Spielermodell nachzieht
#   (PL01.PLD). Gemessener Vorher-Stand: die [pld]-Zeile fehlte komplett.
#   Original: FUN_800396fc @0x80039760-8c -> FUN_800314b0. Braucht die gebaute exe.
if(TARGET re15_pc)
    add_test(NAME integration_weste_load_pin
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r17_weste_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/weste_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_weste_load_pin.cmake)
    set_tests_properties(integration_weste_load_pin PROPERTIES TIMEOUT 240)
endif()
