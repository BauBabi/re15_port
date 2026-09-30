# Spur A (Runde 34 Nacht) — Mess-Sonde fuer den Bauplan "Rolltor ROOM1050: Sicherung einsetzen".
#
# Faehrt den GEPLANTEN Bytecode (Dossier analysis/befunde_runde34_nacht/A_rolltor.md §5.3) mit der
# echten VM ueber einen echten ROOM1050/1051-Raumaufbau, ohne Engine-Aenderung. Kein add_test:
# die Sonde misst den Plan, sie ist kein Riegel (der kommt mit dem Bau).
#
#   cmake --build re15_port/build --target probe_r34n_a_rolltor
#   re15_port/build/tests/unit/probe_r34n_a_rolltor.exe
add_executable(probe_r34n_a_rolltor probe_r34n_a_rolltor.c)
target_link_libraries(probe_r34n_a_rolltor PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_a_rolltor PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r34n_a_rolltor PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
