# Spur L (Runde 36) "elza" — Elza-Szenario: Fenster 1121 + Kraehe, Irons 1151 tot, Unlock-Ton 1211,
# Schuesse + Roys Sterbeton 1021, Roys Stoehnen main16.5 in 1011. Dossier analysis/befunde_runde36/L_elza.md.
#
# MESS-SONDE (kein add_test): Zeitlinie einer Szene am echten VM (Faden-Stellen, Clips, Nachrichten).
#   probe_r36_elza_mess <raum-hex> <x> <z> <rot> <cut> <ereignis|-1> <bilder> [bank:bit ...]
add_executable(probe_r36_elza_mess ${CMAKE_CURRENT_LIST_DIR}/../probe_r36_elza_mess.c)
target_link_libraries(probe_r36_elza_mess PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_elza_mess PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r36_elza_mess PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

# RIEGEL (Unit): misst die MECHANIK jedes Nutzer-Punkts ueber die ECHTEN Haken (Installer beim Raumaufbau, Anker-Scan
# in scd_register_current_rdt, Opcode-/Takt-Haken im VM). Teile: fenster irons unlock schuss roy stoehnen bank signatur.
add_executable(test_r36_elza ${CMAKE_CURRENT_LIST_DIR}/../test_r36_elza.c)
target_link_libraries(test_r36_elza PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_elza PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_elza PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_teil fenster irons unlock schuss roy stoehnen bank signatur)
    add_test(NAME unit_r36_elza_${_teil} COMMAND test_r36_elza ${_teil})
    set_tests_properties(unit_r36_elza_${_teil} PROPERTIES TIMEOUT 240)
endforeach()
