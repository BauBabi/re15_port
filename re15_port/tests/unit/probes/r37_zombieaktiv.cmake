# Runde 37 (2026-10-12), Spur H "zombieaktiv" — Zombies teilweise aktiv im Gang wie RE2.
# Dossier analysis/befunde_runde37/H_zombieaktiv.md, Mechanik include/re15_zombieaktiv_r37.h.
#
#   unit_r37_zombieaktiv_abbildung      Spawn-Abbildung am echten Sce_em_set: nur RE1.5 grid 0x00 wird RE2-Gang
#                                       (Deskriptor 6 @0x80100ab8-abc, Bild <= 2 wie RE2-Original Takt 2), alle anderen
#                                       RE1.5-Formen unveraendert; ohne Sce_em_set und im Montage-Schritt Steher.
#   unit_r37_zombieaktiv_nachspawn1130  Nutzer-Beispiel ROOM1130 nach der Montage: 5 Zombies im Gang ab Bild <= 2, nach
#                                       300 Bildern >= 4500 naeher (RE2-Original >= 15,4 je Takt); Gegenprobe v0.8.24:
#                                       keiner erreicht den Gang in 600 Bildern.
#   unit_r37_zombieaktiv_raum1180       allgemein: ROOM1180 (Normalspiel, 3x grid 0x00) dasselbe; Gegenprobe: keiner.
add_executable(test_r37_zombieaktiv ${CMAKE_CURRENT_LIST_DIR}/../test_r37_zombieaktiv.c)
target_link_libraries(test_r37_zombieaktiv PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_zombieaktiv PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r37_zombieaktiv PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r37_zombieaktiv PRIVATE m)
endif()
foreach(_teil abbildung nachspawn1130 raum1180)
    add_test(NAME unit_r37_zombieaktiv_${_teil} COMMAND test_r37_zombieaktiv ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r37_zombieaktiv_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

# INTEGRATION (echte exe): Spielstand in ROOM1150 nach der Montage + CONTINUE + Tuer -> ROOM1130 (Nachspawn Programm 10),
# Leon steht: Bild 7 alle fuenf im Gang, Bild 306 jeder >= 4500 naeher. Arbeitsordner r37_zombieaktiv_wd (nie gleich einem
# Target-Namen: Linux-exe ohne Endung). Kartenwerkzeug = probe_r36_gegnerzahl_karte (Runde 36 Spur E, raum x z gierung bits).
if(TARGET re15_pc)
    add_test(NAME integration_r37_zombieaktiv
        COMMAND ${CMAKE_COMMAND}
            -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
            -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_gegnerzahl_karte>
            -DWORKDIR=${CMAKE_BINARY_DIR}/r37_zombieaktiv_wd
            -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_zombieaktiv.cmake)
    set_tests_properties(integration_r37_zombieaktiv PROPERTIES TIMEOUT 900)
endif()
