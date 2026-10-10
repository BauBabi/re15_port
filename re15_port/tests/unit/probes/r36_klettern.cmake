# Runde 36 / Spur A — BLOCKER "auf die verschiebbaren Leitern/Tritte klettern" (z.B. ROOM10F0).
# Dossier: analysis/befunde_runde36/A_klettern.md
#
#   probe_r36_klettern_10f0     Diagnose (Argument diag) + Riegel (pin / pin_start):
#     unit_r36_klettern_10f0_endlage    Tritt obj 0 (Typ 4, Flags 0x0001 = pc[6..7]|1 @0x80040990-a4)
#                                       in der Endlage (-4850,0,-850), Flag (3,102)=1; ACTION vor dem
#                                       Tritt -> Substate 9 (@0x8002d604), KIND 0x81 (@0x8002d614/18),
#                                       Aufstieg endet auf der Oberkante obj.y-2*box_hy, Band 0 -> 1
#                                       (@0x80038200-08), Standobjekt = Tritt (@0x80031d20)
#     unit_r36_klettern_10f0_startlage  dasselbe in der Startlage (-6050,0,4950), Flag (3,102)=0
add_executable(probe_r36_klettern_10f0
    ${CMAKE_CURRENT_SOURCE_DIR}/probe_r36_klettern_10f0.c)
target_link_libraries(probe_r36_klettern_10f0 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_klettern_10f0 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r36_klettern_10f0 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

add_test(NAME unit_r36_klettern_10f0_endlage COMMAND probe_r36_klettern_10f0 pin)
add_test(NAME unit_r36_klettern_10f0_startlage COMMAND probe_r36_klettern_10f0 pin_start)
set_tests_properties(unit_r36_klettern_10f0_endlage unit_r36_klettern_10f0_startlage
                     PROPERTIES TIMEOUT 120)

# ECHTER WEG mit der exe (tests/integration/test_r36_klettern.cmake): Leon ROOM10F0 (Sprung, Szene gesehen),
# Elza ROOM10F1 (RE15_GOTO_ROOM = Tuer-Spawn, Weg durch den Raum), Leon ROOM10F0 ERSTES Betreten (Szene
# der Runde 35 laeuft zu Ende, dann Tritt): Tritt verschieben -> hochklettern -> oben ankommen -> Tuer
# nach ROOM1090/1091 von oben. Laufzeit ~5 min (Lauf C enthaelt die ~95 s lange Szene).
if(TARGET re15_pc)
    add_test(NAME integration_r36_klettern
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r36_klettern_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r36_klettern.cmake)
    set_tests_properties(integration_r36_klettern PROPERTIES TIMEOUT 1200)
endif()
