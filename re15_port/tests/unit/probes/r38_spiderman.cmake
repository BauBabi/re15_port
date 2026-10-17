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

#   unit_r38_griff_<fall>  Punkt 2 GENERELL ("bei keinen Gegner"): je Greifer ein echter Griff mit VERGIFTETEM
#                        Spieler-Anker (Anker eines frueheren Griffs ausserhalb des Raums). Invariante je Griffbild:
#                        Spieler in der Raumhuelle und nie hinter einer Wand (Strecke ab Lage vor dem Griff gegen
#                        solide Typ-1-Zellen). Faelle: RE1.5-Zombie vorn/hinten, RE2-Zombie vorn/hinten, RE2-Kriecher
#                        ROOM1220 (duenne Wand, Runde 37 J_bissarm OFFEN 1), RE1.5-/RE2-Hund, Gorilla 0x27, Spiderman
#                        vorn/hinten. Die Gitterarme 0x1A haben ihren eigenen Anker-Riegel (unit_1210_gitterhaende).
add_executable(test_r38_griff_alle ${CMAKE_CURRENT_LIST_DIR}/../test_r38_griff_alle.c)
target_link_libraries(test_r38_griff_alle PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_griff_alle PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_griff_alle PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r38_griff_alle PRIVATE m)
endif()
foreach(_f zombie15_vorn zombie15_hinten zombie2_vorn zombie2_hinten kriecher2_1220 hund15 hund2 gorilla
           spiderman_vorn spiderman_hinten)
    add_test(NAME unit_r38_griff_${_f} COMMAND test_r38_griff_alle ${_f})
    set_tests_properties(unit_r38_griff_${_f} PROPERTIES TIMEOUT 300 SKIP_RETURN_CODE 77)
endforeach()
