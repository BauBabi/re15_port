# Runde 38 Spur T — "spiderman": Typ 0x2B (RE15Editor "SPIDER-MAN") kommt richtig aus dem Schacht, und ein Griff setzt
# den Spieler nie mehr ausserhalb des Raums ("Leon verschwindet komplett von der Karte").
# Dossier: analysis/befunde_runde38/T_spiderman.md; Code: engine/src/spiderman_r38.c, include/re15_spiderman_r38.h,
# Haken in enemy_ai_common.c (Zustand 4, B[5]/B[8]/B[9]/B[10]/B[15], Opfer-Clips/-Platzierung), re15_damage.c
# (Ziel-Sperre im Schacht) und platform/pc/main.c (Schatten-Sperre im Schacht).
#
#   unit_r38_spiderman   Mechanik am echten Raumskript ROOM4001 + echter game_step (s. Kopf von test_r38_spiderman.c):
#                        austritt (versteckt bis grid 0x42, sichtbar Bild 6, greifbar Bild 0x21, ACTIVE nach 35 Bildern),
#                        griff1 mit vergiftetem Spieler-Anker (Anker-Kopie im Latch, Raum-Invariante je Griffbild),
#                        griff2 (einmalige Spielerlage, danach keine Bewegung).
add_executable(test_r38_spiderman ${CMAKE_CURRENT_LIST_DIR}/../test_r38_spiderman.c)
target_link_libraries(test_r38_spiderman PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_spiderman PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_spiderman PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r38_spiderman PRIVATE m)
endif()
add_test(NAME unit_r38_spiderman COMMAND test_r38_spiderman)
set_tests_properties(unit_r38_spiderman PROPERTIES TIMEOUT 300 SKIP_RETURN_CODE 77)
