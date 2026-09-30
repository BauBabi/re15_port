# Spur A (Runde 34 Nacht) — Rolltor ROOM1050/1051: Sicherung einsetzen mit Nahansicht, Tor erst danach.
# Dossier analysis/befunde_runde34_nacht/A_rolltor.md, Modul engine/src/rolltor_1050.c.
#
# (1) MESS-SONDE (Ermittlung): faehrt den geplanten Bytecode mit der echten VM, ohne Haken. Kein add_test.
#   cmake --build re15_port/build --target probe_r34n_a_rolltor
#   re15_port/build/tests/unit/probe_r34n_a_rolltor.exe
add_executable(probe_r34n_a_rolltor probe_r34n_a_rolltor.c)
target_link_libraries(probe_r34n_a_rolltor PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_a_rolltor PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r34n_a_rolltor PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

# (2) RIEGEL (Bau): echter Raumaufbau + Aktion am Schalter + scd_event_fire(2) (der Haken) + Opcode 0x62.
#     Prueft das ERGEBNIS je Fall (A/A-N/B/B-N/C/W/L, 1050 + 1051), Herkunft der Texte, PC-Schranke.
add_executable(test_r34n_a_rolltor test_r34n_a_rolltor.c)
target_link_libraries(test_r34n_a_rolltor PRIVATE re15_engine re15_test_support)
target_include_directories(test_r34n_a_rolltor PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r34n_a_rolltor PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r34n_a_rolltor COMMAND test_r34n_a_rolltor)
set_tests_properties(unit_r34n_a_rolltor PROPERTIES TIMEOUT 60)
