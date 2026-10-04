# Runde 35 Spur G "karte" — Fahrstuhl-Cursor ROOM1080, ROOM11F0/1200 auf der Karte,
# ROOM1230 zeigt die Karte von ROOM11E0, ROOM1210 Korridor + Tueren.
# Dossier: analysis/befunde_runde35/G_karte.md
#
#   probe_r35_karte messung   MESS-SONDE (kein add_test): echter Kartenpfad je Raum/Position
add_executable(probe_r35_karte ${CMAKE_CURRENT_LIST_DIR}/../test_r35_karte.c)
target_link_libraries(probe_r35_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r35_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r35_karte PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(probe_r35_karte PRIVATE m)
endif()
#
#   RIEGEL (je Nutzer-Punkt, misst die MECHANIK am echten Kartenpfad):
#     unit_r35_karte_fahrstuhl  Punkt 1: Etagenraeume setzen Bank 3 Bit 54/55/56; Blatt 2/3/4 je
#                               Kabinen-Etage, aktuell nur rect 9/4/0; Marker in allen 4 Ecken im
#                               GEMALTEN Kabinen-Innenraum und 180 Grad gedreht; Rueckfall ohne Bit
#     unit_r35_karte_b2         Punkt 2: 11F0 -> rect 1, 1200 -> rect 2 (nur diese aktuell), Marker
#                               auf der gemalten Flaeche; 11E0 bleibt rect 0
#     unit_r35_karte_r1230      Punkt 3: 1230 (und 1180) an 6 Punkten -> Blatt 0 rect 0, Marker auf
#                               dem gemalten Gang; NICHT Blatt 1; 10A0 Band 4 -> Blatt 0 rect 6
#     unit_r35_karte_r1210      Punkt 4: 1210 -> rect 3 (T-Korridor), 1220-Zellen -> rect 4/6/5/7/8;
#                               jede der 6 Tuer-AOTs von 1210 hat eine sichtbare Marke auf
#                               gemalter Wand <= 3 px neben ihrer Projektion (Zeile @0x800769b8)
foreach(_r35g_teil fahrstuhl b2 r1230 r1210)
    add_test(NAME unit_r35_karte_${_r35g_teil} COMMAND probe_r35_karte ${_r35g_teil})
    set_tests_properties(unit_r35_karte_${_r35g_teil} PROPERTIES TIMEOUT 120)
endforeach()
#
#   integration_r35_karte    ECHTE exe, Spielstand + LOAD GAME + MAP + Framebuffer-Abzug, Auswertung
#                            des BILDES (rot nur im erwarteten Rechteck, gelbe Tuerbalken):
#                            1230 / 11F0 / 1200 / 1210 und der Nutzerweg 1120 -> Fahrstuhl 1080
#                            (tests/integration/test_r35_karte.cmake)
if(TARGET re15_pc)
    add_test(NAME integration_r35_karte
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PROBE=$<TARGET_FILE:probe_r35_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r35_karte_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r35_karte.cmake)
    set_tests_properties(integration_r35_karte PROPERTIES TIMEOUT 1500)
endif()
#
#   integration_r35_karte_fahrstuhl   NACHBESSERUNG 1 (Abnahme 0, M1/M2): BEWEGUNGSWEITE des
#                            Fahrstuhl-Markers am ECHTEN Weg (Etagenraum -> Tuer -> ROOM1080 ->
#                            laufen -> MAP -> Abzug), 4 Lagen auf 1F + 2 auf 3F; Soll je Achse
#                            >= 6 px im gemalten Innenraum (tests/integration/test_r35_karte_fahrstuhl.cmake)
if(TARGET re15_pc)
    add_test(NAME integration_r35_karte_fahrstuhl
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PROBE=$<TARGET_FILE:probe_r35_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r35_karte_fahrstuhl_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r35_karte_fahrstuhl.cmake)
    set_tests_properties(integration_r35_karte_fahrstuhl PROPERTIES TIMEOUT 1500)
endif()
