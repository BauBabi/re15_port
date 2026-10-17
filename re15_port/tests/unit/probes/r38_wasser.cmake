# Runde 38 Spur N "wasser" — der Wasser-Effekt (Bild + Ton), wenn Leon oder Elza in ROOM1260 und den folgenden
# Raeumen durch flaches Wasser gehen. Dossier analysis/befunde_runde38/N_wasser.md; Code engine/src/wasser_r38.c,
# include/re15_wasser_r38.h, Haken in game_step_common.c (Bildanfang + Fusstakt), platform/pc/main.c (Part-Matrizen
# des Zeichners) und scd_vm.c (op_aot_set: Breite/Tiefe vorzeichenlos).
#
#   unit_r38_wasser_<fall>   Mechanik ohne exe am echten Raum (RDT + ESP-Bank + PL00/PL00W01), Faelle:
#                            pfuetze (Effekt 6 Sub 5, Anker = Original-GDB-Wert, Tonsatz 0x17), zeitfolge (Bildwort des
#                            gezeichneten Bilds), trocken (kein Effekt, 0x1d), stempel (ROOM2090: Effekt 6 Sub 3, Part a1-1,
#                            Gier 0x800, Watsatz 0x1a, +0x88 geloescht), lauf (a0 = 1 -> 0x18), aotbreite (sce-8-Zone
#                            Breite 38950 stempelt), zensus (FLR 0x97 an den sechs Flecken von ROOM1260).
#   integration_r38_wasser   echte exe (Messkopie): Leon ROOM1260 und Elza ROOM1261 gehen in den Fleck P0 —
#                            RE15_WASSER_LOG zweig=boden mit dem Original-Anker, RE15_FX_LOG zeichnet id=6 sub=5,
#                            [foot] VAG 7 (Wasser) im Fleck und VAG 1 (Beton) davor; ROOM2090 Watschritt (Stempel);
#                            echte Tuerwege 10B0 -> 1260 (Leon) und 10B1 -> 1261 (Elza) in den Fleck P5.
add_executable(test_r38_wasser ${CMAKE_CURRENT_SOURCE_DIR}/test_r38_wasser.c)
target_link_libraries(test_r38_wasser PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_wasser PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_wasser PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r38_wasser PRIVATE m)
endif()
foreach(_fall pfuetze zeitfolge trocken stempel lauf aotbreite zensus)
    add_test(NAME unit_r38_wasser_${_fall} COMMAND test_r38_wasser ${_fall})
    set_tests_properties(unit_r38_wasser_${_fall} PROPERTIES TIMEOUT 120)
endforeach()

if(TARGET re15_pc)
    add_test(NAME integration_r38_wasser
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r38_wasser_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_wasser.cmake)
    set_tests_properties(integration_r38_wasser PROPERTIES TIMEOUT 900)
endif()
