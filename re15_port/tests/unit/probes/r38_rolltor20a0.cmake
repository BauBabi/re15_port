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

#   integration_r38_rolltor20a0   echte exe (Messkopie re15_pc_r38w_haken_*), echter Weg (Leiter, Schalter, Ja):
#                                 A Leon ROOM20A0 und B Elza ROOM20A1 — Rolladen auf y -10800, Cut 11, Framedump
#                                 (RE15_WINDOW_SCALE=1): Anteil Raumhintergrund im Bild hoch (vorher zugedeckt),
#                                 RE15_OBJPOLY_LOG Cut 11 obj 1 = Original (NCLIP 20, OTZ<64 16, GPU 6);
#                                 C Leon: Kiste durch die Oeffnung schieben -> faellt (Ausloeser + Landung).
if(TARGET re15_pc)
    add_test(NAME integration_r38_rolltor20a0
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_BILD_TOOL=$<TARGET_FILE:test_r38_rolltor20a0>
                     -DRE15_BSS=${RE15_ASSETS_PATH}/STAGE2/ROOM20A.BSS
                     -DDATEN=${CMAKE_SOURCE_DIR}/tests/integration/r38_rolltor20a0
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r38_rolltor20a0_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_rolltor20a0.cmake)
    set_tests_properties(integration_r38_rolltor20a0 PROPERTIES TIMEOUT 1200)
endif()
