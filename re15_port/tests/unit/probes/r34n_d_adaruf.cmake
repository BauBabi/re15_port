# Spur D (Runde 34 Nacht) — Ada-Ruf an der Tuer ROOM1050 -> ROOM10A0, Sperre bis zur Ada-Rettung.
# Dossier analysis/befunde_runde34_nacht/D_adaruf.md, Werkzeuge re15_port/tools/r34n_d/.
#
# MESS-SONDE (Ermittlung): faehrt den geplanten Bytecode k_ruf mit der echten VM + echtem Spielschritt
# ueber einen ROOM1050-Raumaufbau; Installation und Ereignis-Weiche spielt die Sonde selbst nach
# (es gibt noch keinen Port-Code). Kein add_test.
#   cmake --build re15_port/build --target probe_r34n_d_adaruf
#   re15_port/build/tests/unit/probe_r34n_d_adaruf.exe
add_executable(probe_r34n_d_adaruf probe_r34n_d_adaruf.c)
target_link_libraries(probe_r34n_d_adaruf PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_d_adaruf PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r34n_d_adaruf PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
