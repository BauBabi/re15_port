# Runde 38 (2026-10-16), Spur O "montage" — Montage nach Irons' Tod: in jedem Schnitt, in dem Zombies durchbrechen,
# gehen sie auf die Kamera zu wie die Zombies hinter dem Rolltor (ROOM1040 Cut 1); der Rolltor-Schnitt bleibt unveraendert.
# Dossier analysis/befunde_runde38/O_montage.md, Mechanik include/re15_montage_r38.h + engine/src/montage_r38.c,
# Programme engine/src/irons_tod_1150.c.
#
#   unit_r38_montage_kameras     Kamera-Tabelle = RID-Bytes der ausgelieferten RDTs; Parkplatz Cut 12 nachgerechnet;
#                                Parkplatz 1030 (Cut 7) = Mitte BLK-Zone 7 hinter SCA 5 auf der Achse, in Zone 7, ohne Zelle
#   unit_r38_montage_s1130_re2   ROOM1130 Cut 0, KI RE2 (Standard): alle fuenf >= 1000 naeher an der Kamera und in BEIDEN
#                                Schnitthaelften je >= 400 (Radius) — "ueber die Schnittdauer"; Einzelstopp nur hinter der
#                                Kamera, Navigator-Ziel = Spieler, HP unveraendert; Gegenprobe v0.8.25 ROT
#   unit_r38_montage_s1130_re15  dasselbe unter der RE1.5-KI (Menue; Haelften nur gemeldet)
#   unit_r38_montage_s1030_re2   ROOM1030 Cut 7 (Kopien aus 1070) + Cut 12 (Tor): alle naeher, in beiden Haelften; die Zombies
#                                hinter dem Tor warten waehrend Cut 7 (Baustein 5); wer aufrecht ans Tor kommt, kriecht
#                                (Kriech-Regel sub06/sub07); nur Cut 7/12; Spieler auf der Kameraachse Cut 12; Gegenprobe ROT
#   unit_r38_montage_s1030_re15  dasselbe unter der RE1.5-KI (Haelften nur gemeldet)
#   unit_r38_montage_s1030_ohne7_re2/_re15  1070 leer ((9,76)=0): die Tuer zeigt gleich Cut 12 — dieselben Cut-12-Kriterien;
#                                Gegenprobe v0.8.25 ROT ueber die Haelften (die aufrechten stehen nach der Haelfte am Tor)
#   unit_r38_montage_rolltor     ROOM1040 Cut 1: mit/ohne Aenderung Bild fuer Bild dieselben Lagen, 5 durchs Tor
#   integration_r38_montage      ECHTE exe ueber den echten Weg (Karte + CONTINUE in 1130 + Tuer -> 1150 -> Szene ->
#                                Montage bis 11C0): dieselben Kriterien aus den [montage-r38]-Zeilen des debug.log
add_executable(test_r38_montage ${CMAKE_CURRENT_LIST_DIR}/../test_r38_montage.c)
target_link_libraries(test_r38_montage PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_montage PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_montage PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r38_montage PRIVATE m)
endif()
foreach(_teil kameras s1130_re2 s1130_re15 s1030_re2 s1030_re15 s1030_ohne7_re2 s1030_ohne7_re15 rolltor)
    add_test(NAME unit_r38_montage_${_teil} COMMAND test_r38_montage ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r38_montage_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

# INTEGRATION (echte exe): Arbeitsordner r38_montage_lauf (nicht der Target-Name, Linux ohne .exe).
add_test(NAME integration_r38_montage
    COMMAND ${CMAKE_COMMAND}
        -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
        -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r35_cut1150_karte>
        -DRE15_AUSWERTER=$<TARGET_FILE:test_r38_montage>
        -DWORKDIR=${CMAKE_BINARY_DIR}/r38_montage_lauf
        -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_montage.cmake)
set_tests_properties(integration_r38_montage PROPERTIES TIMEOUT 900)
