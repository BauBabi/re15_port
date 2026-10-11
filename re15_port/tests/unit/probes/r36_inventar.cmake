# --- Runde 36 (2026-10-10), Spur I "inventar". Dossier: analysis/befunde_runde36/I_inventar.md
#     Nutzer: Memory Card beim Speichern -1; Memory Cards/Handgranaten bei 0 aus dem Inventar;
#     Debug-Menue-Sprung mit Elza in die xxx1-Raeume (z.B. ROOM1071).
#     Belege: RE2 MEM_CARD.BIN @0x801c118c-0x801c11e0 (Farbband-Abzug, bei 0 Platz weg + verdichten),
#     RE1.5 @0x8004aee4-af2c + FUN_8004dadc (Verbrauch/Verdichten), UNEQUIP @0x8004ad44-4c, Commit-Regel
#     @0x80046654-88 (0x80 -> Messer), Raumlader @0x800397e4/@0x800397ec (Elza-Bit + Basisindex).
#
#   unit_r36_inventar          Engine-Riegel: Block-Abzug == Live-Abzug (letzte Karte weg, verdichtet,
#                              Ausruest-Platz zieht mit), x3 -> x2, ohne Karte nichts; leere Granaten/
#                              Karten entfernt, Ablegen 0x80 -> Messer erst nach dem Wurf; Debug-Menue:
#                              alle 101 Sprung-Saetze Leon xxx0 / Elza xxx1, Datei vorhanden.
#   integration_r36_inventar   echte exe: Speichern mit 2 Karten -> 1, mit der letzten -> Platz weg
#                              (Log + geschriebener Kartenblock); letzte Handgranate werfen -> Platz weg,
#                              Messer erst nach dem Wurf; Debug-Sprung Elza -> ROOM1151, Leon -> ROOM1150.
add_executable(test_r36_inventar ${CMAKE_CURRENT_LIST_DIR}/../test_r36_inventar.c)
target_link_libraries(test_r36_inventar PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_inventar PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_inventar PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r36_inventar PRIVATE m)
endif()
add_test(NAME unit_r36_inventar COMMAND test_r36_inventar)
set_tests_properties(unit_r36_inventar PROPERTIES TIMEOUT 120)

# Kartenwerkzeug (kein eigener Test): Spielstand ROOM1150 vor dem Telefon, Memory Card x<n> in Platz 0.
add_executable(probe_r36_inventar_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r36_inventar_karte.c)
target_link_libraries(probe_r36_inventar_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_inventar_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r36_inventar_karte PRIVATE m)
endif()

if(TARGET re15_pc)
    add_test(NAME integration_r36_inventar
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_inventar_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r36_inventar_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r36_inventar.cmake)
    set_tests_properties(integration_r36_inventar PROPERTIES TIMEOUT 900)
endif()
