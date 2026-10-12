# Runde 37 (2026-10-12), Spur F "sperre1050" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde37/F_sperre1050.md, Konstanten include/re15_sperre1050_r37.h,
# Code engine/src/sperre1050_r37.c; Text-Weg tools/r36_p/texte_bauen.py (UNTERSUCHEN) -> gen/story_r36_texte.inc.
# Haken: scd_room_setup.c (Installer-Ende, "Runde 37 Spur F"), platform/pc/main.c (Boot-/CONTINUE-Block).
#
#   unit_r37_sperre1050_bedingung  16 Flag-Stellungen: gesperrt genau bei (3,0xBB)=1 (9,71)=1 (9,73)=0
#   unit_r37_sperre1050_raum       ROOM1050 echter Aufbau je Spielabschnitt (Ada-Logik / Tuer / Text-Platz msg 26 /
#                                  Tuer nach Irons), ROOM1051 (Elza) frei
#   unit_r37_sperre1050_druck      Aktionstaste vor Slot 4: Endspiel Text + Freeze ohne Raumwechsel; Mittelspiel und
#                                  nach Irons -> ROOM10A0
#   unit_r37_sperre1050_luke       die Luecke in den Original-Daten (Gitter 10F0, Absprung-Zelle 1090, Hof-Tuer)
#   unit_r37_sperre1050_elza50b1   ROOM50B1 msg 0 ohne Ada-Bezug (Form = Original) + Sweep aller Elza-RDTs
#   integration_r37_sperre1050     echte exe (test_r37_sperre1050.cmake): Luke-Weg Ende-zu-Ende 10F0 -> 1090 -> 1050,
#                                  Gegenproben Mittelspiel / nach Irons, Elza ROOM50B1
add_executable(test_r37_sperre1050 ${CMAKE_CURRENT_LIST_DIR}/../test_r37_sperre1050.c)
target_link_libraries(test_r37_sperre1050 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_sperre1050 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r37_sperre1050 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r37_sperre1050 PRIVATE m)
endif()
foreach(_teil bedingung raum druck luke elza50b1)
    add_test(NAME unit_r37_sperre1050_${_teil} COMMAND test_r37_sperre1050 ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r37_sperre1050_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

if(TARGET re15_pc)
    add_test(NAME integration_r37_sperre1050
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r37_sperre1050_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_sperre1050.cmake)
    set_tests_properties(integration_r37_sperre1050 PROPERTIES TIMEOUT 1200)
endif()
