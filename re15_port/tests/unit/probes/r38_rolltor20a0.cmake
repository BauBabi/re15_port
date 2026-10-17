# Runde 38 Spur W — ROOM20A0/20A1: der geoeffnete Rolladen (obj 1) steht nicht mehr vor der Kamera von Cut 11
# (Nutzer 2026-10-16: "the shutter that you have to open to push the box down is somehow DIRECTLY in the camera.
# I cannot see anything... this is not like this in the original....").
#
# Dossier: analysis/befunde_runde38/W_rolltor20a0.md; Code: engine/src/objpoly_r38.c, include/re15_objpoly_r38.h,
# platform/pc/src/objpoly_r38_pc.c, Haken in platform/pc/main.c (Prop-Schleife: NCLIP @0x80023190 / OTZ<64
# @0x80023228 / GPU-Groessengrenze statt des Port-Ausschnitts `_vz < 64`).
#
#   unit_r38_rolltor20a0          Mechanik ohne exe (echte ROOM20A0/20A1.RDT): 48 Vierecke des Rolladens in Cut 11
#                                 gegen die ORIGINAL-Messung (DuckStation-GDB, Haltepunkte in FUN_80023098) —
#                                 NCLIP-Vorzeichen, OTZ-Schranke, GPU-Dreiecke, SXY +-3; Bildbedeckung neu 0 Pixel,
#                                 alter Weg >= 90 % (Negativkontrolle = Befund); Cut 12 unveraendert; 20A1 byte-gleich.
add_executable(test_r38_rolltor20a0 ${CMAKE_CURRENT_LIST_DIR}/../test_r38_rolltor20a0.c)
target_link_libraries(test_r38_rolltor20a0 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_rolltor20a0 PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(test_r38_rolltor20a0 PRIVATE m)
endif()
add_test(NAME unit_r38_rolltor20a0 COMMAND test_r38_rolltor20a0)
set_tests_properties(unit_r38_rolltor20a0 PROPERTIES TIMEOUT 120)

#   unit_r38_rolltor20a0_windung  Nebenwirkung des NCLIP im PC-Prop-Weg: Zensus der Windung (Objektraum (v1-v0)x(v2-v0)
#                                 gegen die gespeicherten Normalen) ueber alle Objekt-Modelle aller RDTs = Original-
#                                 Konvention (>= 99 %); JEDES PORT-EIGENE Modell ebenso (NCLIP gilt seit dem Merge von
#                                 r38/cursor fuer alle Objekte; Feuerloescher R37 von Spur A neu gewickelt); EIN NCLIP-
#                                 Weg: der Verwurf der Prop-Schleife laeuft ueber Spur As re15_nclip_r38 (Zaehler).
add_executable(test_r38_rolltor20a0_windung ${CMAKE_CURRENT_LIST_DIR}/../test_r38_rolltor20a0_windung.c)
target_link_libraries(test_r38_rolltor20a0_windung PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_rolltor20a0_windung PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(test_r38_rolltor20a0_windung PRIVATE m)
endif()
add_test(NAME unit_r38_rolltor20a0_windung COMMAND test_r38_rolltor20a0_windung)
set_tests_properties(unit_r38_rolltor20a0_windung PROPERTIES TIMEOUT 300)

#   probe_r38_rolltor20a0_karte   Werkzeug (kein Test): Speicherkarte mit Spielstand in ROOM20A0 (Leon) / ROOM20A1 (Elza)
#                                 vor der Leiter, fuer den LADE-Weg (Titel -> CONTINUE) des exe-Riegels.
add_executable(probe_r38_rolltor20a0_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r38_rolltor20a0_karte.c)
target_link_libraries(probe_r38_rolltor20a0_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r38_rolltor20a0_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

#   integration_r38_rolltor20a0   echte exe (Messkopie re15_pc_r38w_haken_*), echter Weg (Leiter, Schalter, Ja):
#                                 A Leon ROOM20A0 (Debug-Sprung vor die Leiter), B Elza ROOM20A1 und D Leon ROOM20A0
#                                 (beide Spielstand + CONTINUE) — Rolladen auf y -10800, Cut 11, Framedump
#                                 (RE15_WINDOW_SCALE=1): Anteil Raumhintergrund im Bild hoch (vorher zugedeckt),
#                                 RE15_OBJPOLY_LOG Cut 11 obj 1 = Original (NCLIP 20, OTZ<64 16, GPU 6);
#                                 C Leon: Kiste durch die Oeffnung schieben -> faellt (Ausloeser + Landung).
if(TARGET re15_pc)
    add_test(NAME integration_r38_rolltor20a0
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r38_rolltor20a0_karte>
                     -DRE15_BILD_TOOL=$<TARGET_FILE:test_r38_rolltor20a0>
                     -DRE15_BSS=${RE15_ASSETS_PATH}/STAGE2/ROOM20A.BSS
                     -DDATEN=${CMAKE_SOURCE_DIR}/tests/integration/r38_rolltor20a0
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r38_rolltor20a0_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_rolltor20a0.cmake)
    set_tests_properties(integration_r38_rolltor20a0 PROPERTIES TIMEOUT 1200)
endif()

# ---- Nachbesserung 1 (Abnahme-Mangel M1): halbtransparente Raum-Objekte (Attribut-Bit 0x2) ------------------------
#   unit_r38_rolltor20a0_abe      Regeln gegen den Disassembler (ABE @0x8002c2b4, ABR = MD1-Seite | (Attribut & 0xC)
#                                 << 3 @0x800259c4/@0x80025b24, Flach @0x8002c2f4 / Typ 3), gegen die ORIGINAL-
#                                 Laufzeitmessung (DuckStation-GDB: 5060 obj 2-5, 20A0 obj 4-6, 4090 obj 0-2, Code
#                                 0x3e + Texpage-ABR je Polygon), STP-Teilung echter Texturen (11B0 obj 0 gemischt,
#                                 5060 obj 4), SDL-Mischung gegen die GPU-Formel (<= 1 Stufe). Werkzeug --fall fuer:
#   integration_r38_rolltor20a0_abe  echte exe: ROOM5060 Cut 4 (ABR 0, der Abnahme-Fall), ROOM20A0 Cut 2 (ABR 2,
#                                 Wasser Typ 3), ROOM4090 Cut 0 (ABR 3, Nebel) — Mittelwert der Messflaeche = Original-
#                                 Bildpuffer (W_belege/orig_abe_*_buf0.png).
add_executable(test_r38_rolltor20a0_abe ${CMAKE_CURRENT_LIST_DIR}/../test_r38_rolltor20a0_abe.c)
target_link_libraries(test_r38_rolltor20a0_abe PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_rolltor20a0_abe PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(test_r38_rolltor20a0_abe PRIVATE m)
endif()
add_test(NAME unit_r38_rolltor20a0_abe COMMAND test_r38_rolltor20a0_abe)
set_tests_properties(unit_r38_rolltor20a0_abe PROPERTIES TIMEOUT 120)
if(TARGET re15_pc)
    add_test(NAME integration_r38_rolltor20a0_abe
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_ABE_TOOL=$<TARGET_FILE:test_r38_rolltor20a0_abe>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r38_rolltor20a0_abe_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_rolltor20a0_abe.cmake)
    set_tests_properties(integration_r38_rolltor20a0_abe PROPERTIES TIMEOUT 1200)
endif()
