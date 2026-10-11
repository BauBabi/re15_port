# Runde 36 (2026-10-11), Spur O "pri" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde36/O_pri.md, Regeln: include/re15_pri_schatten_r36.h.
#
#   unit_r36_pri_absperrung  Punkt 1: ROOM10F0 Cut 2 traegt die Absperrung des Nutzers (pri/STAGE1/10F0/
#                            02_02.png, Lage x=33 y=56): ueber die ECHTEN Leser (re15_pri_msk_section_offset /
#                            re15_pri_parse_section / re15_tim_parse) wird Marvins Standplatz der Szene
#                            (RE15_CUT10F0_MARVIN_X/Z) im Koerperkasten von der Scheibenkante verdeckt
#                            (Original-Urteil re15_pri_mask_occludes), die gemessenen Wegpunkte VOR der
#                            Scheibe (x=4568, Zellenflaeche 4100 + Radius 450) bleiben frei.
#   unit_r36_pri_schatten    Punkt 2: die OT-Regel des Figurenschattens (AVSZ4 ZSF4=256 @0x80066c7c/80,
#                            srl 4 @0x8001b2d8; Gleichstand = Einhaenge-Reihenfolge) an den Bucket-Grenzen,
#                            und am echten ROOM1100.MSK: am Wandstandplatz (-15100,-18500) Cut 2 bzw.
#                            (-15100,-16900) Cut 1 liegt KEINE Maske ueber dem Schattenquad des Spielers
#                            (die Masken am Wandfuss liegen hinter ihm); der Zombie hinter der Ecke (F595,
#                            (-20275,-10713)) bleibt in Cut 2 voll verdeckt.
#   integration_r36_pri_schatten  (nur mit re15_pc) die echte exe: ROOM1100 Cut 2, Spieler (-15100,-18500),
#                            Framedump Bild 150 mit und ohne Masken (RE15_NO_PRI) — im Schattenfenster des
#                            Spielers aendert sich kein Pixel (vorher: Bodenpixel der Wandmaske ueber dem
#                            halbtransparenten Schatten).
add_executable(test_r36_pri ${CMAKE_CURRENT_LIST_DIR}/../test_r36_pri.c)
target_link_libraries(test_r36_pri PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_pri PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_pri PRIVATE RE15_PORT_SRC_DIR="${CMAKE_SOURCE_DIR}")
if(NOT WIN32)
    target_link_libraries(test_r36_pri PRIVATE m)
endif()
foreach(_teil absperrung schatten)
    add_test(NAME unit_r36_pri_${_teil} COMMAND test_r36_pri ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r36_pri_${_teil} PROPERTIES TIMEOUT 120)
endforeach()

# MESS-WERKZEUG (kein add_test): Pixel mit Kanaldifferenz > Schwelle in einem Bildrechteck zweier Framedumps.
add_executable(probe_r36_ppm_diff ${CMAKE_CURRENT_LIST_DIR}/../probe_r36_ppm_diff.c)

if(TARGET re15_pc)
    add_test(NAME integration_r36_pri_schatten
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r36_ppm_diff>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r36_pri_schatten_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r36_pri_schatten.cmake)
    set_tests_properties(integration_r36_pri_schatten PROPERTIES TIMEOUT 400)
endif()
