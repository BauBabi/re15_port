# Runde 41 (2026-10-24), Spur J "stage3szenen_a" — Nutzer-Punkte (AUFTRAG.md):
#   (1) "Auch die Animation von Marvin, am Ende seine Abschnitten in STAGE3, bevor zurueck gewechselt wird zu Leon ist
#       falsch in ROOM 3060."
#   (2) "In Elzas Szenario, in ROOM 3061 in der cutscene klappt das rein krabbeln von Sherry in den Schacht nicht. Am Ende
#       von dieser Cutscene hat Elza auch erneut eine falsche Animation."
# Dossier analysis/befunde_runde41/J_stage3szenen_a.md; Code engine/src/wclip_r41.c (Spieler-Plc_motion(2,c) = W-Bank),
# engine/src/fussanker_r41.c (Glied-Anker FUN_800369f8), engine/src/superobj_r41.c (Super_set/Super_reset 0x48/0x49).
# (Der RBJ-Binder hat seine eigene Datei r41_stage3szenen_a_rbj.cmake, wegen des cherry-picks der Spur K.)
#
# RIEGEL (Unit, Mechanik):
#   unit_r41_stage3szenen_a_zensus    240 RDTs: Spieler-Plc_motion(2,c) nur 3060 @0x010BE / 3061 @0x011DE; Plc_flg Bit 1/2
#                                     nur 3061 sub04 (5 Stellen); Super_set nur 3061 @0x0127E und 4030/4031
#   unit_r41_stage3szenen_a_anker     Glied-Anker an Sherrys echter Bank: von einem Original-Stand der Reihe o3061_s3 zum
#                                     naechsten (Kriech-Takte aus sub04), Abweichung <= 40
#   unit_r41_stage3szenen_a_superobj  Super_set/Super_reset am echten Bytestrom: Loslass-Lage des Gitters ~ Original
#   unit_r41_stage3szenen_a_wclip     Plc_motion(2,6) -> W-Bank-Clip 6, Halten auf Bild 9, Ende bei Befehl/Raumwechsel
# INTEGRATION (echte exe, tests/integration/test_r41_stage3szenen_a.cmake):
#   integration_r41_stage3szenen_a_3060  Marvin ueber Szene 3020 + Tuer 3040 -> 3060: W-Bank-Clip 6 gehalten (kein Knien)
#   integration_r41_stage3szenen_a_3061  Elza Spielstand + CONTINUE + Quadrat am AOT: Gitter, Anker-Takte, Sherry im
#                                        Schacht, Elza W-Bank-Clip 6 gehalten, Wechsel nach 30C1 (PL0F)
add_executable(test_r41_stage3szenen_a ${CMAKE_CURRENT_LIST_DIR}/../test_r41_stage3szenen_a.c)
target_link_libraries(test_r41_stage3szenen_a PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_stage3szenen_a PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_stage3szenen_a PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_stage3szenen_a PRIVATE m)
endif()
foreach(_teil zensus anker superobj wclip)
    add_test(NAME unit_r41_stage3szenen_a_${_teil} COMMAND test_r41_stage3szenen_a ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_stage3szenen_a_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

if(TARGET re15_pc AND TARGET probe_r36_elza_karte)
    foreach(_teil 3060 3061)
        add_test(NAME integration_r41_stage3szenen_a_${_teil}
            COMMAND ${CMAKE_COMMAND}
                -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                -DRE15_KARTE_ELZA=$<TARGET_FILE:probe_r36_elza_karte>
                -DWORKDIR=${CMAKE_BINARY_DIR}/r41_stage3szenen_a_wd
                -DTEIL=${_teil}
                -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_stage3szenen_a.cmake)
        set_tests_properties(integration_r41_stage3szenen_a_${_teil} PROPERTIES TIMEOUT 1200)
    endforeach()
endif()
