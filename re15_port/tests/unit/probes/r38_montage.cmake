# Runde 38 (2026-10-16), Spur O "montage" — Montage nach Irons' Tod: in jedem Schnitt, in dem Zombies durchbrechen,
# gehen sie auf die Kamera zu wie die Zombies hinter dem Rolltor (ROOM1040 Cut 1); der Rolltor-Schnitt bleibt unveraendert.
# Dossier analysis/befunde_runde38/O_montage.md, Mechanik include/re15_montage_r38.h + engine/src/montage_r38.c,
# Programme engine/src/irons_tod_1150.c.
#
#   unit_r38_montage_kameras     Kamera-Tabelle = RID-Bytes der ausgelieferten RDTs; Parkplatz Cut 12 nachgerechnet
#   unit_r38_montage_s1130_re2   ROOM1130 Cut 0, KI RE2 (Standard): alle fuenf >= 1000 naeher an der Kamera, Einzelstopp
#                                nur hinter der Kamera, Spieler-HP unveraendert; Gegenprobe v0.8.25: keiner kommt naeher
#   unit_r38_montage_s1130_re15  dasselbe unter der RE1.5-KI (Menue)
#   unit_r38_montage_s1030_re2   ROOM1030 Cut 7 (Kopien aus 1070) + Cut 12 (Tor): alle naeher; Spieler nach dem Schnitt auf
#                                Cut 12 auf der Kameraachse; Gegenprobe v0.8.25
#   unit_r38_montage_s1030_re15  dasselbe unter der RE1.5-KI
#   unit_r38_montage_rolltor     ROOM1040 Cut 1: mit/ohne Aenderung Bild fuer Bild dieselben Lagen, 5 durchs Tor
add_executable(test_r38_montage ${CMAKE_CURRENT_LIST_DIR}/../test_r38_montage.c)
target_link_libraries(test_r38_montage PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_montage PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_montage PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r38_montage PRIVATE m)
endif()
foreach(_teil kameras s1130_re2 s1130_re15 s1030_re2 s1030_re15 rolltor)
    add_test(NAME unit_r38_montage_${_teil} COMMAND test_r38_montage ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r38_montage_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
