# Runde 41 Spur L "spinnenanim" — Nutzer (AUFTRAG.md Z. 47): "Die Spinnen Animation beim herausklettern vom Gitter und
# runter klettern an der Wand stimmen noch nicht ganz."
#
# Dossier: analysis/befunde_runde41/L_spinnenanim.md; Code: engine/src/spinnenanim_r41.c, include/re15_spinnenanim_r41.h,
# Haken in platform/pc/main.c (Gegner-Zeichner), enemy_ai_common.c (Spinnen-Takt, anim_set-Zwilling), spinnen_r38.c
# (lauer_anim, Gift-Anker), scd_room_setup.c (Merker je Raum).
#
#   unit_r41_spinnenanim   Mechanik des Zeichnens im echten Spielschritt (ROOM2060, RE1.5- UND RE2-KI) gegen die
#                          Original-Messung je Takt (test_r41_spinnenanim_orig.inc aus L_belege/orig_2060_gdb.log):
#                          A1 Figur-Matrix == Original +0x20 (RotMatrix(+0x68) @0x8001e8f4 x ScaleMatrix 0xccc @0x8001e940),
#                          A2 gezeichnetes Bild/Ueberblendung == Original (+0x95/+0x8f vor dem Vorschub, FUN_8001f3bc),
#                          A3 Wurzelteil wie gezeichnet == Original Teil 0 (Toleranz belegt), A4 Gegenprobe alter Weg,
#                          B Gift-Anker == Original-Spawnlage (5741,-320,4049).
add_executable(test_r41_spinnenanim ${CMAKE_CURRENT_LIST_DIR}/../test_r41_spinnenanim.c)
target_link_libraries(test_r41_spinnenanim PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_spinnenanim PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/..)
target_compile_definitions(test_r41_spinnenanim PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r41_spinnenanim PRIVATE m)
endif()
add_test(NAME unit_r41_spinnenanim COMMAND test_r41_spinnenanim)
set_tests_properties(unit_r41_spinnenanim PROPERTIES TIMEOUT 300)

#   integration_r41_spinnenanim   echte exe, Messschiene RE15_SPINNENANIM_LOG (die Matrix, die main.c WIRKLICH zeichnet,
#                                 und das gezeichnete Bild je Takt): Leon ROOM2060 (RE2-KI = Standard), Leon ROOM2060
#                                 RE1.5-KI, Elza ROOM2061; Auswerter `test_r41_spinnenanim pruef_log`.
if(TARGET re15_pc)
    add_test(NAME integration_r41_spinnenanim
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_TOOL=$<TARGET_FILE:test_r41_spinnenanim>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_spinnenanim_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_spinnenanim.cmake)
    set_tests_properties(integration_r41_spinnenanim PROPERTIES TIMEOUT 900)
endif()
