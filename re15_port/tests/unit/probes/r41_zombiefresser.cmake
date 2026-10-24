# Runde 41 Spur I — die 0x84/0x85-Zombies in ROOM2000/2001/3000/3010 LIEGEN und stehen auf, wenn man ihnen naeher
# kommt (RE1.5 Wecker 0x80103814 + Liege-Phasenmaschine 0x801038ec), statt am Boden zu fressen.
#
# Dossier: analysis/befunde_runde41/I_zombiefresser.md; Code: engine/src/zombiefresser_r41.c, include/re15_zombiefresser_r41.h,
# Haken in enemy_ai_re2_zombie.c (INIT-Weiche, EXEC[7]-Wecker, Trefferfilter), enemy_ai_common.c (Grid [4]/[5]/[6]),
# player_common.c (Pin-Freigabe der Phasenmaschine).
#
#   unit_r41_zombiefresser      Mechanik ohne exe (echte RDT/SCD/SCA, game_step), beide KI-Varianten:
#                               A Zensus 2000/2001/3000/3010: jeder 0x84/0x85 liegt (RE2 EXEC[7] + Latch, Clip 23/22; RE1.5
#                                 grid 0x84/0x85, Clip 0x12/0x13), keiner frisst; B Ruhe d >= 4000: 300 Bilder liegend +
#                                 unschiessbar; C je Raum Spieler 500..1500 vor einem Zombie: Wecken im ersten Takt,
#                                 Wartezeit <= 16, Aufsteh-Clip ganz (RE2 8/9, RE1.5 0x12/0x13), steht, greift an;
#                               D Grenze 0xFA0 (4000 nein / 3999 ja); E Wartezeit ueber 48 Seeds; F Fresser 0x86 (1140) bleibt.
add_executable(test_r41_zombiefresser ${CMAKE_CURRENT_LIST_DIR}/../test_r41_zombiefresser.c)
target_link_libraries(test_r41_zombiefresser PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_zombiefresser PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_zombiefresser PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_zombiefresser PRIVATE m)
endif()
add_test(NAME unit_r41_zombiefresser COMMAND test_r41_zombiefresser)
set_tests_properties(unit_r41_zombiefresser PROPERTIES TIMEOUT 600)

#   probe_r41_zombiefresser_karte   Werkzeug (kein add_test): Spielstand mit Figur, Band und Story-Bits.
add_executable(probe_r41_zombiefresser_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r41_zombiefresser_karte.c)
target_link_libraries(probe_r41_zombiefresser_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r41_zombiefresser_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r41_zombiefresser_karte PRIVATE m)
endif()

#   integration_r41_zombiefresser_<teil>   echte exe (Messkopie re15_pc_r41i_<teil>_*), RE15_ZOMBIEFRESSER_LOG + RE15_STATE_LOG:
#     tuer2000   ECHTER WEG: Debug-Sprung 2070, Wegplan durch die Tuer nach ROOM2000 auf den Oststeg (Leon, Standard-KI)
#     elza2001   ECHTER WEG als Elza: Debug-Sprung 2071 + Spawn-Bit 3:0x45, Wegplan durch die Tuer nach ROOM2001
#     leon3000   Spielstand Leon ROOM3000 (4:9, 4:0xC6) + CONTINUE, 1200 vor dem Zombie (-9900,-14800)
#     leon3010   Spielstand Leon ROOM3010 (4:0x0A) + CONTINUE, 1100 vor dem Zombie (-9100,-19800)
#     re15_3000  wie leon3000 mit RE1.5-KI (Clip 0x12 statt RE2-Clip 8)
#   Je Teil: INIT 1/7/0 f10e=4002 (RE2) bzw. grid 84 (RE1.5); WECKT mit d < 4000; STEHT AUF <= 16 Bilder danach;
#   Zombie aufrecht (st=1 ss1=1 bzw. 2, g=00); Angriff (gr=1 oder Spieler-HP sinkt); NIE ss1=8 (Fressen); ferne
#   Zombies liegen den ganzen Lauf.
if(TARGET re15_pc)
    foreach(_teil tuer2000 elza2001 leon3000 leon3010 re15_3000)
        add_test(NAME integration_r41_zombiefresser_${_teil}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r41_zombiefresser_karte>
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_zombiefresser_wd
                         -DTEIL=${_teil}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_zombiefresser.cmake)
        set_tests_properties(integration_r41_zombiefresser_${_teil} PROPERTIES TIMEOUT 600)
    endforeach()
endif()
