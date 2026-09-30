# Spur D (Runde 34 Nacht) — Ada-Ruf an der Tuer ROOM1050 -> ROOM10A0, Sperre bis zur Ada-Rettung.
# Dossier analysis/befunde_runde34_nacht/D_adaruf.md, Werkzeuge re15_port/tools/r34n_d/,
# Code include/re15_adaruf.h + engine/src/adaruf_1050.c (Haken: scd_room_setup.c, scd_vm.c scd_event_fire).
#
# MESS-SONDE (Ermittlung, Stand vor dem Bau): faehrt den geplanten Bytecode mit der echten VM; spielt
# Installation und Weiche selbst nach. Kein add_test.
#   re15_port/build/tests/unit/probe_r34n_d_adaruf.exe
add_executable(probe_r34n_d_adaruf probe_r34n_d_adaruf.c)
target_link_libraries(probe_r34n_d_adaruf PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_d_adaruf PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r34n_d_adaruf PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

# RIEGEL (Bau): ueber die ECHTEN Haken (Installation beim Raumaufbau, Weiche in scd_event_fire).
# Teile: szene, doppel (Auflage 5), sperre, frei, rettung, speicher (Auflage 8a), elza, raster (Auflage 6).
add_executable(test_r34n_d_adaruf ${CMAKE_CURRENT_SOURCE_DIR}/test_r34n_d_adaruf.c)
target_link_libraries(test_r34n_d_adaruf PRIVATE re15_engine re15_test_support)
target_include_directories(test_r34n_d_adaruf PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r34n_d_adaruf PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_teil szene doppel sperre frei rettung speicher elza raster)
    add_test(NAME unit_r34n_d_adaruf_${_teil} COMMAND test_r34n_d_adaruf ${_teil})
    set_tests_properties(unit_r34n_d_adaruf_${_teil} PROPERTIES TIMEOUT 180)
endforeach()

# MESS-WERKZEUG (Auflage 8a, kein add_test): Speicherkarte mit Stand in ROOM1150 fuer den CONTINUE-Lauf
# der echten exe (Flags aus dem geladenen Stand statt RE15_SET_FLAG).
#   probe_r34n_d_karte <kartendatei> [gesehen] [gerettet]
add_executable(probe_r34n_d_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r34n_d_karte.c)
target_link_libraries(probe_r34n_d_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_d_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
