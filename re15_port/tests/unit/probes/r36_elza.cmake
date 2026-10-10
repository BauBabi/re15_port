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

# MESS-WERKZEUG (kein add_test): Elza-Spielstand (Figur 4) in einem Raum fuer den CONTINUE-Lauf der echten exe.
#   probe_r36_elza_karte <kartendatei> <raum-hex> <x> <z> <gier> [bank:bit ...]
add_executable(probe_r36_elza_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r36_elza_karte.c)
target_link_libraries(probe_r36_elza_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_elza_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

# AUSWERTE-WERKZEUG (kein add_test): Framedump-Vergleich an den Schadens-Pixeln + Mitschnitt-Spitzen je Bildfenster.
add_executable(probe_r36_elza_auswertung ${CMAKE_CURRENT_LIST_DIR}/../probe_r36_elza_auswertung.c)
target_include_directories(probe_r36_elza_auswertung PRIVATE ${CMAKE_SOURCE_DIR}/include)

# INTEGRATION (echte exe, Elza-Spiel): je Nutzer-Punkt ein Lauf ueber den echten Weg (Raumwechsel bzw. Spielstand +
# CONTINUE), gemessen in debug.log/state.log/elza.log, Framedump und RE15_AUDIO_CAP_SYNC-Mitschnitt.
foreach(_teil fenster irons unlock schuss stoehnen)
    add_test(NAME integration_r36_elza_${_teil}
        COMMAND ${CMAKE_COMMAND}
            -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
            -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_elza_karte>
            -DRE15_AUSWERT_TOOL=$<TARGET_FILE:probe_r36_elza_auswertung>
            -DRE15_SYNCHRO_WAV=${CMAKE_SOURCE_DIR}/../synchro/STAGE1/room1170/main00.wav
            -DWORKDIR=${CMAKE_BINARY_DIR}/r36_elza_wd
            -DTEIL=${_teil}
            -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r36_elza.cmake)
    set_tests_properties(integration_r36_elza_${_teil} PROPERTIES TIMEOUT 1200)
endforeach()
