# Runde 36 (2026-10-10), Spur P "story" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde36/P_story.md, Konstanten include/re15_story_r36.h,
# Code engine/src/story_r36.c (+ gen/story_r36_texte.inc aus tools/r36_p/texte_bauen.py).
# Haken: scd_room_setup.c (Installer-Ende), msg_common.c (Ende re15_msg_load_room_block,
# re15_dialog_open_mask), platform/pc/main.c (Boot-/CONTINUE-Block).
#
#   unit_r36_story_roy          P1 Roys Leiche ROOM1020 erst mit Strom (4,0xF3): Prop + Platz + echter Druck
#   unit_r36_story_zelle        P2 ROOM1210 Text-Platz = Elzas Zellentuer-Platz ROOM1211, msg 20, echter Druck
#   unit_r36_story_schuppen     P3 ROOM1091 Text-Platz = Leons Feuer-Platz ROOM1090, msg 20, echter Druck
#   unit_r36_story_texte        P4 jede umgeschriebene Nachricht: Original an der Id, neu nach dem Laden,
#                               Kopf/Schwanz/Dauer = Original, Rest des Raums bytegleich
#   unit_r36_story_sweep        P4 alle RDTs beider Szenarien: anderer Name nur an belegten Stellen, kein
#                               alter Begegnungs-Wortlaut
#   unit_r36_story_reihenfolge  Tuer-Weg und Boot-Weg enden beide mit Overrides + Id 20
#   integration_r36_story       echte exe: Spielstand + CONTINUE (Boot-Weg) und Tuer-Eintritt (Tuer-Weg),
#                               Leon und Elza, Text-Protokoll RE15_MSG_LOG (test_r36_story.cmake)
add_executable(test_r36_story ${CMAKE_CURRENT_LIST_DIR}/../test_r36_story.c)
target_link_libraries(test_r36_story PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_story PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_story PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r36_story PRIVATE m)
endif()
foreach(_teil roy zelle schuppen texte sweep reihenfolge)
    add_test(NAME unit_r36_story_${_teil} COMMAND test_r36_story ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r36_story_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

# MESS-WERKZEUG (kein add_test): Speicherkarte mit einem Stand je Punkt (Leon/Elza), fuer die echte exe.
#   probe_r36_story_karte <kartendatei> <stand> [strom]
add_executable(probe_r36_story_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r36_story_karte.c)
target_link_libraries(probe_r36_story_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_story_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
