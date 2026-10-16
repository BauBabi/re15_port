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
