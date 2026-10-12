# Runde 37 Spur D "itemband" (2026-10-12) — Items auf erhoehten Flaechen nur vom eigenen Band aufnehmbar.
# Dossier: analysis/befunde_runde37/D_itemband.md. Belege: include/re15_itemband_r37.h.
#   (1) AOT-Scan FUN_80042bac @0x80042cac-ccc: rec[2] (Item_aot_set pc[4]) == Spieler+0x82 fuer JEDEN Satz vor dem
#       sce-Dispatch — auch ITEM (sce 9 -> 0x80043328).
#   (2) Member_set 0x12 -> FUN_8004116c Tabelle 0x80010c8c[18] -> @0x80041228 sb a2,130(a0) = 0x800ACAD6 =
#       Kollisionsband des Spielers (Lift ROOM3050 sub04 @0x023D0 -> Band 0).
#
#   unit_r37_itemband   Z  Zensus: 206 RDTs / 40674 Opcodes / 164 Item_aot_set, genau die 12 Saetze mit floor != 0,
#                          keiner mit Bit 0x80; 22 Schreiber Member 0x12 (alle 0x34), genau die 17 am Spieler.
#                       G  jeder der 153 Item-Saetze mit Geometrie: echter Handler + re15_aot_scan -> Modal GENAU bei
#                          Spielerband == floor; die 12 erhoehten bis zum Taken-Bit; Gegenprobe Sperr-Zaehler.
#                       M  echte Subs: ROOM3050 sub04/sub02 (Band 0/4), ROOM2030 sub02/sub04 (6/3/9/3), ROOM50D0
#                          sub02/sub04 (3/0); Gegenprobe Figur-Slot laesst das Band stehen.
add_executable(test_r37_itemband ${CMAKE_CURRENT_LIST_DIR}/../test_r37_itemband.c)
target_link_libraries(test_r37_itemband PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_itemband PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r37_itemband PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r37_itemband PRIVATE m)
endif()
add_test(NAME unit_r37_itemband COMMAND test_r37_itemband
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r37_itemband PROPERTIES TIMEOUT 600)

# INTEGRATION (echte exe, RE15_WEGPLAN geschlossen geregelt + RE15_ITEMBAND_LOG): je erhoehtem Item das FALSCHE Band an der
# Item-Stelle (GESPERRT, Taken-Bit 0) und der VORGESEHENE Weg (FREI auf dem Item-Band, Taken-Bit 1):
#   3050 oben (Band 4) / Weg Fall -> Lift (Member 0x12 4 -> 0) -> Kerbe (3,93) -> Klettern -> Band 1; 3051 Elza (Band 4);
#   3010 unter dem Steg (Band 0) / Klettern an r0[4] -> Band 1; 2090 echte Tuer aus 20A0 -> Plattform Band 1;
#   2040 Band 5 / Halle Band 3 (Tor-Raetsel (3,146)+(3,53) gesetzt); 2030 SPAS-12 Band 3 / Lift A ((3,126) Strom gesetzt,
#   Member 0x12 3 -> 6); 2030 Schrot Band 3 / Treppe slot 8 Abstieg 3 -> 1. Skript tests/integration/test_r37_itemband.cmake.
add_test(NAME integration_r37_itemband
    COMMAND ${CMAKE_COMMAND}
        -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
        -DWORKDIR=${CMAKE_BINARY_DIR}/r37_itemband_wd
        -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_itemband.cmake)
set_tests_properties(integration_r37_itemband PROPERTIES TIMEOUT 3000)
