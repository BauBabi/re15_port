# Runde 30 Nachschliff, Spur cut-blitz: Kamerawechsel schaltet Hintergrund und Projektion im
# SELBEN Bild (Original: Present FUN_8002137c @0x80021558 Apply + @0x80021560 Sprung hinter
# DrawOTag/Pufferwechsel; Apply FUN_80021bbc schaltet BG @0x80021d2c/@0x80021e34, H @0x80021e6c
# und Blickmatrix @0x80021e8c in einem Zug).
#
#   integration_r30_cut_blitz   echte exe, zwei Laeufe (ROOM1150 zu Fuss ueber zwei Zonen;
#                               ROOM1170 SCD Cut_chg), Messschiene RE15_CUT_SYNC_LOG: in jedem
#                               Bild mit 3D traegt die Projektion den Cut des Hintergrunds
#                               (ausser unter voller Tuerblende), und die Wechsel fanden statt.
# Dossier: analysis/befunde_runde30/nachschliff-cut-blitz.md
if(TARGET re15_pc)
    add_test(NAME integration_r30_cut_blitz
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r30_cut_blitz_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r30_cut_blitz.cmake)
    set_tests_properties(integration_r30_cut_blitz PROPERTIES TIMEOUT 600)
endif()
