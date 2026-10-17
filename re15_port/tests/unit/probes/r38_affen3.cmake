# Runde 38 Spur S "affen3" — die kleinen Affen (Typ 0x29, EM029) in Stage 3 treffen wie im Original.
# Nutzer: "In Stage 3 die kleinen Affen koennen einen nicht treffen, es passiert nichts".
# Dossier analysis/befunde_runde38/S_affen3.md, Code engine/src/affen3_r38.c + include/re15_affen3_r38.h
# (Haken: enemy_ai_common.c INIT/ACTIVE/Dispatch je 1 Zeile, affen_11c0.c Bank des Aktor-Typs 3 Zeilen).
#
#   unit_r38_affen3_typ      Typ 0x29 = der Blob, den der Original-Lader FUN_80022300 fuer 0x29 laedt (@0x80073000)
#   unit_r38_affen3_biss     Port ab dem Original-Zustand F137 der GDB-Spur o4: Commit, Treffer -5 im Fenster, Takt 57
#   unit_r38_affen3_flavor   dasselbe unter RE2-KI (Standard) und RE1.5-KI
#   unit_r38_affen3_quadrat  FUN_8001bff8 = Quadrat um den gezeichneten Knochenpunkt (Record 9 + 0x64)
#   unit_r38_affen3_schwer   schwerer Angriff Sub 6: -10 im Fenster {21..24,33,34}, Records 6/10
#   unit_r38_affen3_decke    angeschossener Affe: Sub 9 an die Decke, Kriechen/Jagd/Hieb wie GDB-Spur o7
#   unit_r38_affen3_griff    Griff von der Decke (Sub 15 + Spieler-Haken cmd 5) und Bisse danach wie GDB-Spur o8
#   unit_r38_affen3_sprung   Sprung Sub 7 / Todessprung Sub 8 (HP < 50 -> -500 -> cmd 6 -> Tod) wie GDB-Spur o9
#   integration_r38_affen3   echte exe, ROOM3040 per Debug-Sprung, Leon auf der Plattform: Bisse treffen (-5),
#                            RE2-KI (Standard) und RE1.5-KI
add_executable(test_r38_affen3 ${CMAKE_CURRENT_SOURCE_DIR}/test_r38_affen3.c)
target_link_libraries(test_r38_affen3 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_affen3 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_affen3 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_R38_PSX_EXE="${CMAKE_SOURCE_DIR}/../info/Re1.5/PSX.EXE")
if(NOT WIN32)
    target_link_libraries(test_r38_affen3 PRIVATE m)
endif()
foreach(_teil typ biss flavor quadrat schwer decke griff sprung)
    add_test(NAME unit_r38_affen3_${_teil} COMMAND test_r38_affen3 ${_teil})
    set_tests_properties(unit_r38_affen3_${_teil} PROPERTIES TIMEOUT 240)
endforeach()

if(TARGET re15_pc)
    add_test(NAME integration_r38_affen3
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r38_affen3_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_affen3.cmake)
    set_tests_properties(integration_r38_affen3 PROPERTIES TIMEOUT 900)
endif()
