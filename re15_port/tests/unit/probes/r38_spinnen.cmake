# Runde 38 Spur R "spinnen" — Stage-2-Spinnen (Nutzerpunkte AUFTRAG.md Z. 68-70: Schacht/Luft, Gift, Minispinnen).
#
# Dossier: analysis/befunde_runde38/R_spinnen.md; Code: engine/src/spinnen_r38.c, include/re15_spinnen_r38.h,
# Haken in enemy_ai_common.c, enemy_ai_re2_spider.c, re15_esp.c, game_step_common.c, platform/pc/main.c.
#
#   unit_r38_spinnen   Mechanik im echten Spielschritt (scd_vm_tick + re15_game_step + ESP-Tick) mit den echten
#                      STAGE2-RDTs und EM025-Baenken: A Schacht ROOM2060 (RE1.5- und RE2-KI): Halten bis Cut 1,
#                      neun Phasen @0x80114ec8 mit den Original-Bildabstaenden, Landung auf dem Boden; B Gift: Spucken
#                      (Effekt 0x0d, 3 Stroeme), Treffer -10 HP + Gift-Bit, Wirkung -2 HP alle 121 Bilder, nie
#                      toedlich, Gegengift; C Minispinnen (RE2-KI, ROOM2030): Boden des Elternteils, Bisse.
add_executable(test_r38_spinnen ${CMAKE_CURRENT_LIST_DIR}/../test_r38_spinnen.c)
target_link_libraries(test_r38_spinnen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_spinnen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_spinnen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r38_spinnen PRIVATE m)
endif()
add_test(NAME unit_r38_spinnen COMMAND test_r38_spinnen)
set_tests_properties(unit_r38_spinnen PROPERTIES TIMEOUT 300)
