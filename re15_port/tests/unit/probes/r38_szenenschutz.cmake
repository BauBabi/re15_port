# Runde 38 Spur B "szenenschutz" — kein Angriff auf den Spieler, solange eine Szene ihn haelt (Nutzer: "Das Leon schon in
# Cutscenes angegriffen werden kann, wo er sich noch nicht mal bewegen kann ist mist ... ROOM 1190 ... 11D0 ... Es sei
# den es ist Teil der Cutscene wie bei Elza Room 1170.").
#
# Dossier: analysis/befunde_runde38/B_szenenschutz.md; Code: engine/src/szenenschutz_r38.c, include/re15_szenenschutz_r38.h,
# Haken in enemy_ai_common.c (re15_enemy_ai_run_all Kopf/je Gegner/Ende, re15_player_is_grabbed) und
# enemy_ai_re2_dog.c (Sperrzeit-Tor an Entscheid/STALK/RUN-P3, Abzug im ACTIVE-Schwanz).
#
#   unit_r38_szenenschutz         Mechanik ohne exe: Praedikat, Haken-Folge + Sicherheitsnetz, ROOM1190 RE2-Hund
#                                 Sperrzeit 0x78 (@0x801115e8-ec) -> Commit >= 120 Bilder nach der Landung,
#                                 gehalten (player_mode 2) kein Treffer / frei Biss (RE2 + RE1.5), Gegenprobe ROOM1171
#                                 Szenen-Kraehe greift den gehaltenen Spieler.
add_executable(test_r38_szenenschutz ${CMAKE_CURRENT_LIST_DIR}/../test_r38_szenenschutz.c)
target_link_libraries(test_r38_szenenschutz PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_szenenschutz PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_szenenschutz PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r38_szenenschutz PRIVATE m)
endif()
add_test(NAME unit_r38_szenenschutz COMMAND test_r38_szenenschutz)
set_tests_properties(unit_r38_szenenschutz PROPERTIES TIMEOUT 120)

#   unit_r38_szenenschutz_alle    Zensus ALLER Gegnertypen (Dossier 2.5/2.6) in beiden Gehirnen (RE2-Standard, RE1.5):
#                                 gehalten kein HP-Verlust/Griff ohne Sicherheitsnetz, frei greift derselbe Aufbau an.
add_executable(test_r38_szenenschutz_alle ${CMAKE_CURRENT_LIST_DIR}/../test_r38_szenenschutz_alle.c)
target_link_libraries(test_r38_szenenschutz_alle PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_szenenschutz_alle PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_szenenschutz_alle PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r38_szenenschutz_alle PRIVATE m)
endif()
add_test(NAME unit_r38_szenenschutz_alle COMMAND test_r38_szenenschutz_alle)
set_tests_properties(unit_r38_szenenschutz_alle PROPERTIES TIMEOUT 300)

#   integration_r38_szenenschutz  echte exe (Messkopie re15_pc_r38b_szene_*): ROOM1190 (Hebel-Szene sub10) und ROOM11D0
#                                 (Aufnahme-Szene sub03/sub02) mit Leon, Standard-KI: kein HP-Verlust und kein Hund im
#                                 Angriff (st1/3) solange pm=2, danach Biss; 1190 erster Biss >= 30 Bilder nach dem
#                                 Szenenende (vorher 13). Gegenprobe ROOM1171 mit Elza: RE1.5-KI Szenen-Kraehe greift
#                                 waehrend pm=2 (gr=1), Standard-KI kein geschuetzter Akteur im Raum.
if(TARGET re15_pc)
    add_test(NAME integration_r38_szenenschutz
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r38_szenenschutz_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_szenenschutz.cmake)
    set_tests_properties(integration_r38_szenenschutz PROPERTIES TIMEOUT 1200)
endif()
