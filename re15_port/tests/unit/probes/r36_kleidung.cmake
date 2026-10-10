# Spur B (Runde 36) — "kleidung": Leons zerrissene/blutige Kleidung (Blut-Decal-Wundsystem
# FUN_80037edc @0x80037edc, Phase 4 der Opfer-FSM @0x8010a4e8, Dispatcher @0x8010a580-5b0).
# Dossier analysis/befunde_runde36/B_kleidung.md; Code include/re15_kleidung_r36.h + engine/src/kleidung_r36.c.
#
# Riegel:
#   unit_r36_kleidung_griff_re2 / _re15  ROOM1140 auf dem echten game_step-Weg: der 3. ueberlebte Griff
#                                        (von hinten, Helper 0x8010a208 (0,10)(4,50)(7,50)) stempelt
#                                        Panel 4/7 Stufe 1 — unter BEIDEN Gehirnen (RE2 = Standard).
#   unit_r36_kleidung_hund               RE2-Hund: Boden-Biss stempelt ein Panel aus 05 07 04 06 (+0x32).
#   unit_r36_kleidung_kraehe             RE2-Kraehe: Griff-Eintritt stempelt die 0x801161e8-Baender.
#   unit_r36_kleidung_lut                Panel-LUT @0x80074208: Leon Zeile 0, Elza Zeile 4 (nicht 1).
#   integration_r36_kleidung_bild        echte exe: Spielstand mit Wunden -> CONTINUE -> Framedump:
#                                        Leon UND Elza zeigen die Decals (Pixel-Differenz im Kasten).
add_executable(test_r36_kleidung ${CMAKE_CURRENT_SOURCE_DIR}/test_r36_kleidung.c)
target_link_libraries(test_r36_kleidung PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_kleidung PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_kleidung PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_fl re2 re15)
    add_test(NAME unit_r36_kleidung_griff_${_fl} COMMAND test_r36_kleidung griff ${_fl})
    set_tests_properties(unit_r36_kleidung_griff_${_fl} PROPERTIES TIMEOUT 240)
endforeach()
foreach(_teil hund kraehe lut)
    add_test(NAME unit_r36_kleidung_${_teil} COMMAND test_r36_kleidung ${_teil})
    set_tests_properties(unit_r36_kleidung_${_teil} PROPERTIES TIMEOUT 240)
endforeach()

if(TARGET re15_pc)
    add_test(NAME integration_r36_kleidung_bild
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:test_r36_kleidung>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r36_kleidung_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r36_kleidung_bild.cmake)
    set_tests_properties(integration_r36_kleidung_bild PROPERTIES TIMEOUT 900)
endif()
