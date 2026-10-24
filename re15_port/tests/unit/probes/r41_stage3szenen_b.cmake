# Runde 41 (2026-10-24), Spur K "stage3szenen_b" - Riegel, zaehlen zur Suite.
# Dossier analysis/befunde_runde41/K_stage3szenen_b.md, Code engine/src/sherry_r41.c, include/re15_sherry_r41.h,
# platform/pc/src/sherry_pc_r41.c/.h (Haken main.c, re15_damage.c, player_common.c, climb_common.c). Der Binder (A-E5) ist
# J's Commit "rbj-binder" (cherry-pick, Riegel probes/r41_stage3szenen_a_rbj.cmake).
#
#   unit_r41_stage3szenen_b_pins     Original-Bytes PSX.EXE / ROOM3061 / ROOM3091 (Modell 0x0F, Waffe 0, Tabellen-Zeile 15,
#                                    Ziel-Sperren, Save 0x10, RBJ rec2 Kind-/Erwachsenenhoehe)
#   unit_r41_stage3szenen_b_waffe    Engine: Figurwechsel -> Waffe 0 / HP 100 / Status 0, Ausruesten gesperrt, Zeile 15,
#                                    zurueck -> Messer (DEBUG.BIN @0x800c00d4, Nachbesserung 1 M2)
#   unit_r41_stage3szenen_b_koerper  PC-Modul am echten PL04/PL0F: Koerper + W00 + keine Hand-Meshes + Wunden + CORE; zurueck
#   integration_r41_stage3szenen_b_sherry  echte exe, Schacht-Tuer 3061 -> 30C1: Koerper, Knochen, Gehen 50, Rennen 120,
#                                          kein Zielen (Original K_belege/k1_orig_*_30c1.txt)
#   integration_r41_stage3szenen_b_3091    echte exe, Riegeltuer 3091 -> sub07: Clipfolge je Aktor = Original (GDB-Lauf);
#                                          Nachbesserung 1 (M1): Sherrys Schleifen-Clip 13 Bild fuer Bild = Original
#   unit_r41_stage3szenen_b_marvin   Nachbesserung 1 (M3): Koerper bei jedem Neuladen (PL0D Marvin, gleiche W-Familie)
#   integration_r41_stage3szenen_b_marvin  echte exe, ROOM3020 -> 30C0 als Marvin: Koerper PL0D (Knochen 10 = 485),
#                                          Waffe 1 / HP 100 / Status 0 (Nachbesserung 1, M2/M3)
#   unit_r41_stage3szenen_b_schleife Nachbesserung 1 (M1): Spieler-Clip-Ende im Kommandowort 4 (plcloop_r41.c, Spur E)
#                                    mit der Laenge des Binder-Records (@0x8001b450), echte RBJ ROOM3091/ROOM2020
add_executable(test_r41_stage3szenen_b
    ${CMAKE_CURRENT_LIST_DIR}/../test_r41_stage3szenen_b.c
    ${CMAKE_SOURCE_DIR}/platform/pc/src/sherry_pc_r41.c)
target_link_libraries(test_r41_stage3szenen_b PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_stage3szenen_b PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/platform/pc/src)
target_compile_definitions(test_r41_stage3szenen_b PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_PSX_EXE="${CMAKE_SOURCE_DIR}/../info/Re1.5/PSX.EXE")
if(NOT WIN32)
    target_link_libraries(test_r41_stage3szenen_b PRIVATE m)
endif()
foreach(_teil pins waffe koerper schleife marvin)
    add_test(NAME unit_r41_stage3szenen_b_${_teil} COMMAND test_r41_stage3szenen_b ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_stage3szenen_b_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_stage3szenen_b_sherry.cmake)
    add_test(NAME integration_r41_stage3szenen_b_sherry
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_WERKZEUG=$<TARGET_FILE:test_r41_stage3szenen_b>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_stage3szenen_b_sherry_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_stage3szenen_b_sherry.cmake)
    set_tests_properties(integration_r41_stage3szenen_b_sherry PROPERTIES TIMEOUT 1200)
endif()
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_stage3szenen_b_3091.cmake)
    add_test(NAME integration_r41_stage3szenen_b_3091
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_WERKZEUG=$<TARGET_FILE:test_r41_stage3szenen_b>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_stage3szenen_b_3091_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_stage3szenen_b_3091.cmake)
    set_tests_properties(integration_r41_stage3szenen_b_3091 PROPERTIES TIMEOUT 2400)
endif()
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_stage3szenen_b_marvin.cmake)
    add_test(NAME integration_r41_stage3szenen_b_marvin
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_WERKZEUG=$<TARGET_FILE:test_r41_stage3szenen_b>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_stage3szenen_b_marvin_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_stage3szenen_b_marvin.cmake)
    set_tests_properties(integration_r41_stage3szenen_b_marvin PROPERTIES TIMEOUT 900)
endif()
