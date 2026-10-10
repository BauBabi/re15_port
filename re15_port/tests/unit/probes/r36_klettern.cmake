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

#   probe_r36_klettern_zensus   Der Wortlaut ist PLURAL ("Leitern/Tritte"): ALLE Typ-4-Objekte ohne
#                               Bit 0x100 (Zensus A_belege/zensus_obj_typ4_flags.txt) in der VM, je
#                               Raum (Leon + Elza), je Lage (Flag 0/1), von vier Seiten:
#                               10F0/10F1 (Tritt, (3,102)), 11A0/11A1 (Kiste, (5,6)), 2040/2041
#                               (obj 1/2 Tritt, statisch), 2060/2061 (Tritt, (3,172)), 20A0/20A1
#                               (Kiste, (3,204)), 5020/5021 (Kiste, (4,203)); Riegel = je Objekt
#                               mindestens eine Seite mit Substate 9 / KIND 0x81 / oben angekommen.
#                               Dazu (Nachbesserung 1, Abnahme 0 Mangel 1) die Null-Box ROOM3091
#                               obj 1 (@0x01508, h 0): FUN_8002da4c vergleicht vorzeichenlos
#                               (@0x8002da8c-c0), bei h 0 / Rand -112 (@0x8002d398) Treffer genau
#                               bei |dx| >= 112 UND |dz| >= 112 — Riegel: Wahrheitstafel 12 Punkte,
#                               achsgerade 4/4 ohne Objekt-Treffer, Querversatz +-300 8/8 Start.
add_executable(probe_r36_klettern_zensus
    ${CMAKE_CURRENT_SOURCE_DIR}/probe_r36_klettern_zensus.c)
target_link_libraries(probe_r36_klettern_zensus PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_klettern_zensus PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r36_klettern_zensus PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r36_klettern_zensus COMMAND probe_r36_klettern_zensus pin)
set_tests_properties(unit_r36_klettern_zensus PROPERTIES TIMEOUT 300)

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
