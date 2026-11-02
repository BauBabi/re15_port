# Runde 41 (2026-11-01), Spur W "wasserblink", OFFEN 4 entschieden (Variante B, wie die 40%-Disc): Die Deckentropfen-Schleife in
# ROOM2000/2001 startet nicht. Code: engine/src/tropfen_r41.c, Haken in scd_vm.c op_evt_exec. Herleitung: include/re15_tropfen_r41.h,
# Dossier analysis/befunde_runde41/W_wasserblink.md OFFEN 4.
#
#   unit_r41_tropfen                    echte RDTs 2000 (Leon) und 2001 (Elza), Raumstart + 700 Takte: Haken einmal, kein Effekt 6 Sub 0
#                                       im Pool, kein Faden in sub07/sub08, die drei Gitterstrahlen 0x0B stehen; Eingrenzung auf Raum,
#                                       main00, @0x0177C, Bytes und Ziel
#   integration_r41_wasserblink_tropfen_leon / _elza   echte exe (tests/integration/test_r41_wasserblink.cmake, Eintrag in
#                                       probes/r41_wasserblink.cmake): fx.log ohne Effekt 6 Sub 0, oberer Bildrand von Cut 2 ruhig
add_executable(test_r41_tropfen test_r41_tropfen.c)
target_link_libraries(test_r41_tropfen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_tropfen PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r41_tropfen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_tropfen PRIVATE m)
endif()
add_test(NAME unit_r41_tropfen COMMAND test_r41_tropfen)
set_tests_properties(unit_r41_tropfen PROPERTIES TIMEOUT 120)
