# Spur E (Runde 36) — zwanzig Gegner-Plaetze je Raum wie das Original (RE15_ACTOR_MAX 16 -> 21).
# Dossier analysis/befunde_runde36/E_gegnerzahl.md, Werkzeug re15_port/tools/r36_e/sce_em_slot_zensus.py.
# Belege: Gegner-Feld 0x800acc2c / 20 Plaetze (FUN_80037250 `sltiu v0,v0,0x14` @0x800372b4; naechstes Feld bei
# 0x800af33c @0x80039b38/@0x80039b40), Sce_em_set ohne Grenzpruefung (@0x800420f8/@0x80042118).
#
# RIEGEL (Unit, echte RDTs ueber scd_room_reenter): Teile feld raum1030 raum1031 raum1040 raum1041 raum20b0 kein_platz.
add_executable(test_r36_gegnerzahl ${CMAKE_CURRENT_SOURCE_DIR}/test_r36_gegnerzahl.c)
target_link_libraries(test_r36_gegnerzahl PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_gegnerzahl PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_gegnerzahl PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_teil feld raum1030 raum1031 raum1040 raum1041 raum20b0 kein_platz)
    add_test(NAME unit_r36_gegnerzahl_${_teil} COMMAND test_r36_gegnerzahl ${_teil})
    set_tests_properties(unit_r36_gegnerzahl_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

# MESS-WERKZEUG (kein add_test): Speicherkarte mit Stand vor einer Original-Tuer + Tot-Bits (bank:bit[-bit]).
add_executable(probe_r36_gegnerzahl_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r36_gegnerzahl_karte.c)
target_link_libraries(probe_r36_gegnerzahl_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_gegnerzahl_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

# INTEGRATION (echte exe): Spielstand + CONTINUE, Tuer 1030 <-> 1040, Aktoren 16..20 im Zielraum.
# Arbeitsordner r36_gegnerzahl_wd (nie gleich einem Target-Namen: Linux-exe ohne Endung).
if(TARGET re15_pc)
    add_test(NAME integration_r36_gegnerzahl
        COMMAND ${CMAKE_COMMAND}
            -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
            -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_gegnerzahl_karte>
            -DWORKDIR=${CMAKE_BINARY_DIR}/r36_gegnerzahl_wd
            -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r36_gegnerzahl.cmake)
    set_tests_properties(integration_r36_gegnerzahl PROPERTIES TIMEOUT 900)
endif()
