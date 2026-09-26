# --- Runde 26 (2026-09-26), Thema m93r-nachladen: die M93R laedt nicht automatisch nach,
#     wenn ein Messer-Zieleintritt den Port-Latch s_aim_melee hat stehenlassen.
#     Dossier: analysis/befunde_2026-09-26/messung-m93r.md
#     RIEGEL (add_test): die Sonde faellt mit exit 1, sobald einer der fuenf Laeufe die
#     Nachlade-Phase NICHT erreicht. Am alten Stand (player_common.c mit "|| s_aim_melee"
#     im Nachlade-Eintritt) sind D und D2 beide -1 = NIE, der Test ist also rot.
#     Original-Beleg: Nachlade-Gate @0x80033338-78 traegt genau fuenf Bedingungen und kein
#     Klassen-Flag; die Klassentrennung macht der Dispatch @0x80032e60/0x80074030. ---
add_executable(probe_m93r_nachladen probe_m93r_nachladen.c)
target_link_libraries(probe_m93r_nachladen PRIVATE re15_engine re15_test_support)
target_include_directories(probe_m93r_nachladen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_m93r_nachladen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME m93r_nachladen COMMAND probe_m93r_nachladen)
set_tests_properties(m93r_nachladen PROPERTIES TIMEOUT 60)
