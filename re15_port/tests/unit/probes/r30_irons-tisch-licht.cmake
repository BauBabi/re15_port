# =============================================================================
# Runde 30 Nachschliff, Spur tischlicht — Irons Diary (obj 5) und Memory Card (obj 6) waren in
# Cut 6 fast schwarz (Lichtsatz Cut 6, ROOM1150.RDT @0x00488, ambient 40,40,24). PORT-WAHL:
# nur diese zwei Props nehmen den Lichtsatz von Cut 2 (@0x003E8). Messtabelle:
# include/re15_irons_tisch.h; Dossier analysis/befunde_runde30/irons-diary-welt.md Abschnitt 10.
#
#   unit_r30_irons_tisch_licht          die Wahl gilt NUR fuer obj 5/6 in 1150/1151; der Satz
#                                       ist vorhanden (beide RDTs @0x3E8, byte-gleich)
#   integration_r30_irons_tisch_licht   echte exe, vier Framedumps (Cut 6/Cut 2, mit/ohne
#                                       Props): Helligkeit beider Props in Cut 6 innerhalb der
#                                       gemalten Umgebung; Cut 6 nicht dunkler als Cut 2
# =============================================================================
add_executable(test_r30_irons_tisch_licht ${CMAKE_CURRENT_LIST_DIR}/../test_r30_irons_tisch_licht.c)
target_link_libraries(test_r30_irons_tisch_licht PRIVATE re15_engine re15_test_support)
target_include_directories(test_r30_irons_tisch_licht PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r30_irons_tisch_licht PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r30_irons_tisch_licht COMMAND test_r30_irons_tisch_licht
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r30_irons_tisch_licht PROPERTIES TIMEOUT 60)

# AUSWERTER (kein add_test) fuer den Licht-Riegel: vergleicht vier Framedumps.
add_executable(probe_r30_irons_tisch_licht ${CMAKE_CURRENT_LIST_DIR}/../probe_r30_irons_tisch_licht.c)

if(TARGET re15_pc AND TARGET probe_r30_irons_tisch_karte)
    add_test(NAME integration_r30_irons_tisch_licht
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r30_irons_tisch_karte>
                     -DRE15_LICHT_TOOL=$<TARGET_FILE:probe_r30_irons_tisch_licht>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r30_irons_tisch_licht_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r30_irons_tisch_licht.cmake)
    set_tests_properties(integration_r30_irons_tisch_licht PROPERTIES TIMEOUT 400)
endif()
