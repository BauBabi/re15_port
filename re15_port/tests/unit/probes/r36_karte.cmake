# Runde 36 Spur M "karte" — Treppen 10A0 auf B1, Marker ROOM11D0 an der Tuer, ROOM1190 in zwei
# Bereichen, Tuer 11B0<->11C0 waagerecht, Teilbereiche ROOM1110 erst nach Betreten besucht,
# Fahrstuhl-Kabine nur auf der Etage besucht, von der aus man eingestiegen ist.
# Dossier: analysis/befunde_runde36/M_karte.md
#
#   probe_r36_karte messung   MESS-SONDE (kein add_test): echter Kartenpfad je Punkt
add_executable(probe_r36_karte ${CMAKE_CURRENT_LIST_DIR}/../test_r36_karte.c)
target_link_libraries(probe_r36_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r36_karte PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(probe_r36_karte PRIVATE m)
endif()
#
#   RIEGEL (je Nutzer-Punkt, misst die MECHANIK am echten Kartenpfad, Tuerdurchgang = Trigger im
#   Quellraum + Laden des Zielraums am Spawn der Tuer aus g_aot):
#     unit_r36_karte_treppe      P1: 1230 -> 10A0 B1: genau zwei Treppen auf Blatt 0 rect 6, je auf
#                                der Projektion eines Band-4-STAIR-AOT, Sprossen auf gemalter Flaeche;
#                                vorher (nur 1F) keine; B2/1F unveraendert je zwei
#     unit_r36_karte_marker11d0  P2: 1230 -> 11D0: Marker <= 6 px unter der Tuermarke, Sprung <= 8 px
#     unit_r36_karte_r1190       P3: Haupthalle -> NUR rect 1, Ostkammer -> NUR rect 2, getrennte
#                                Besucht-Bits; 11A0 nicht mehr auf B1
#     unit_r36_karte_tuer11c0    P4: Marke 11B0<->11C0 waagerecht, alle 5 Punkte auf der gemalten Wand
#     unit_r36_karte_teile1110   P5: kleine Raeume erst nach Betreten besucht; Speichern/Laden; alter Stand
#     unit_r36_karte_fahrstuhl   P6: Kabine nur auf der Einstiegs-Etage besucht (1080 und 4020)
foreach(_r36m_teil treppe marker11d0 r1190 tuer11c0 teile1110 fahrstuhl)
    add_test(NAME unit_r36_karte_${_r36m_teil} COMMAND probe_r36_karte ${_r36m_teil})
    set_tests_properties(unit_r36_karte_${_r36m_teil} PROPERTIES TIMEOUT 120)
endforeach()
#
#   integration_r36_karte    ECHTE exe: Spielstand + LOAD GAME + Tuer per Aktionstaste + MAP +
#                            Framebuffer-Abzug (RE15_INV_FB_SHOT, RE15_WINDOW_SCALE=1), Auswertung
#                            des BILDES je Punkt (tests/integration/test_r36_karte.cmake)
if(TARGET re15_pc)
    add_test(NAME integration_r36_karte
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PROBE=$<TARGET_FILE:probe_r36_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r36_karte_lauf
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r36_karte.cmake)
    set_tests_properties(integration_r36_karte PROPERTIES TIMEOUT 1500)
endif()
