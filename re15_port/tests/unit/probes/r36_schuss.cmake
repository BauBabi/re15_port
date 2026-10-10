# Runde 36 Spur C — der allgemeine Schusspfad: Sichtlinie (RE1.5 FUN_8001b9b4 @0x80012168-74), Fresser-Sperre
# (RE2 FUN_800410CC @0x80041270-9c), Blut-Anker, Kraehe gegen Magnum (RE2 EMOVL21_S0.BIN GIB @0x80102CA0).
#
# Dossier: analysis/befunde_runde36/C_schuss.md; Code: engine/src/schuss_r36.c, include/re15_schuss_r36.h,
# Haken in re15_damage.c (Sichtlinie), enemy_ai_re2_zombie.c (Fresser), enemy_ai_common.c (Export).
#
#   unit_r36_schuss           Mechanik ohne exe: Sichtlinie ueber den Konferenztisch ROOM1140 (echte RDT, alle
#                             Waffenklassen), Fresser-Sperre je Gate (RE2-KI), Aufstehen gibt frei.
add_executable(test_r36_schuss ${CMAKE_CURRENT_LIST_DIR}/../test_r36_schuss.c)
target_link_libraries(test_r36_schuss PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_schuss PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(test_r36_schuss PRIVATE m)
endif()
add_test(NAME unit_r36_schuss COMMAND test_r36_schuss)
set_tests_properties(unit_r36_schuss PROPERTIES TIMEOUT 120)

#   integration_r36_schuss    echte exe (ROOM1140, RE15_DEBUG_JUMP, RE2-KI): Python ueber den Konferenztisch
#                             trifft NICHT (Plaetze 4/5 hp 250 bleiben), Fresser vom Tuerpunkt (> 4000) werden
#                             NICHT getroffen, Zombie vor dem Tisch wird getroffen (Gegenprobe).
if(TARGET re15_pc)
    add_test(NAME integration_r36_schuss
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r36_schuss_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r36_schuss.cmake)
    set_tests_properties(integration_r36_schuss PROPERTIES TIMEOUT 900)
endif()

#   probe_r36_blut            Messsonde + Pin fuer den Blut-Anker des RE2-Zombie-Treffers (ROOM1140, echter
#                             game_step, RE2-Bank): Lage des Bluts gegen R_part*a3 + t_part aus der gezeichneten
#                             Pose (RE2 EMZ0 @0x80105650-708 / @0x80105D14-DBC / @0x801050B0-158).
add_executable(probe_r36_blut ${CMAKE_CURRENT_LIST_DIR}/../probe_r36_blut.c)
target_link_libraries(probe_r36_blut PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_blut PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r36_blut PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(probe_r36_blut PRIVATE m)
endif()
add_test(NAME unit_r36_blut_pin_eben COMMAND probe_r36_blut pin eben)
add_test(NAME unit_r36_blut_pin_tief COMMAND probe_r36_blut pin tief)
set_tests_properties(unit_r36_blut_pin_eben unit_r36_blut_pin_tief PROPERTIES TIMEOUT 120)
