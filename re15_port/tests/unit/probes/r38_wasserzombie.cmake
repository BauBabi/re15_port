# Runde 38 Spur U — die Zombies im Wasser (Stage 2) ziehen sich am Rand hoch wie in RE1.5, beide KI-Varianten.
#
# Dossier: analysis/befunde_runde38/U_wasserzombie.md; Code: engine/src/wasserzombie_r38.c, include/re15_wasserzombie_r38.h,
# Haken in enemy_ai_common.c (Aufruf ANIMATE[0x0A], Clip-Uhr), enemy_ai_re2_zombie.c (Zustands-Dispatch), schuss_r36.c
# (Bank), platform/pc/main.c (Bank je Typ aus dem Hybrid-Lader, Pose, Schatten).
#
#   unit_r38_wasserzombie       Mechanik ohne exe (echte ROOM2020/ROOM2000.RDT, echte SCA, game_step):
#                               A Clip 0x16 der RE1.5-Aktionsbank IST das Hochziehen (159 Bilder, Wurzel -1817 -> -3278,
#                                 +1444 vor), B Ausloeser @0x80101f00-44, C RE1.5-KI 2020 (Clip 1 -> 0x16, Bild 0x46
#                                 +0x1ba -1800, oben y -1800/Band 1, Vorschub 1140 @0x80103474, kein Sturz danach),
#                               D RE2-KI 2020 (dieselbe Maschine, RE1.5-Bank je Typ; Gegenprobe ohne Bank), E ROOM2000
#                               beide KI (Deskriptor 0x84), F RE2-Abbruch (Partner-Domino 0x901) setzt +0x1ba zurueck.
add_executable(test_r38_wasserzombie ${CMAKE_CURRENT_LIST_DIR}/../test_r38_wasserzombie.c)
target_link_libraries(test_r38_wasserzombie PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_wasserzombie PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_wasserzombie PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r38_wasserzombie PRIVATE m)
endif()
add_test(NAME unit_r38_wasserzombie COMMAND test_r38_wasserzombie)
set_tests_properties(unit_r38_wasserzombie PROPERTIES TIMEOUT 300)

#   probe_r38_wasserzombie_karte   Werkzeug (kein add_test): Spielstand in ROOM2020/2000 (Leon) bzw. 2021/2001 (Elza).
add_executable(probe_r38_wasserzombie_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r38_wasserzombie_karte.c)
target_link_libraries(probe_r38_wasserzombie_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r38_wasserzombie_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r38_wasserzombie_karte PRIVATE m)
endif()

#   integration_r38_wasserzombie   echte exe (Messkopie re15_pc_r38u_haken_*), LADE-Weg CONTINUE:
#                                  a ROOM2020 Leon Standard-KI (RE2), b ROOM2020 Leon RE1.5-KI, c ROOM2021 Elza,
#                                  d ROOM2000 Leon (0x84) — RE15_WASSERZOMBIE_LOG-Folge eintritt/klettern/boden-1800/
#                                  oben (x,-1800,2222) Band 1 +0x1ba -1800; kein Gegner unter dem Wasserboden/auf Band 2.
if(TARGET re15_pc)
    add_test(NAME integration_r38_wasserzombie
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r38_wasserzombie_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r38_wasserzombie_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_wasserzombie.cmake)
    set_tests_properties(integration_r38_wasserzombie PROPERTIES TIMEOUT 1500)
endif()
