# Spur N (Runde 37) "elzaems" — das Modellarchiv JEDES Gegner-/NPC-Typs wie im Original: FUN_80022300 waehlt per
# `bltz` @0x80022334 auf DAT_800aca3c Bit 31 Datei 0x26 (CDEMD0, Leon) oder 0x27 (CDEMD1, Elza); dasselbe Bit ist
# SCD-Flag (1,0) (Bank-Tabelle @0x80074664[1]), vier Raumskripte schalten es fuer einen Birkin-Spawn um.
# Dossier analysis/befunde_runde37/N_elzaems.md.
#
# RIEGEL (Unit): toc (beide Archive, Unterschiede je Typ, CRC32 = debug.log-Werte), weiche (Elza-Bit-Zwilling),
#   raeume (Raum-Init am VM: Archiv je gespawntem Typ in 3071/50F1/5081/3091/1101 Elza und 5080/3070/50E0/1100 Leon),
#   griff (RE1.5-KI-Griff mit der Bank der Port-Weiche: Elzas Zombie-Hand auf IHRER Schulter, J-Messung 20..65),
#   opfer27 (0x27-Finisher an Elza ROOM1141: Opfer-Clip 0 aus CDEMD1 = 63 Bilder, Leon-Bank 70).
add_executable(test_r37_elzaems ${CMAKE_CURRENT_LIST_DIR}/../test_r37_elzaems.c)
target_link_libraries(test_r37_elzaems PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_elzaems PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r37_elzaems PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_teil toc weiche raeume griff opfer27)
    add_test(NAME unit_r37_elzaems_${_teil} COMMAND test_r37_elzaems ${_teil})
    set_tests_properties(unit_r37_elzaems_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

# INTEGRATION (echte exe, Lade-Weg ueber Spielstand + CONTINUE bzw. Tuer): debug.log-Zeile des Laders je Typ.
foreach(_teil elza5081 elza3071 leon5080 elza3091 elza1101 leon1100 elza11b1 elza5091 elza1181griff elza10c1kraehe
              leon10c0kraehe elza1141)
    add_test(NAME integration_r37_elzaems_${_teil}
        COMMAND ${CMAKE_COMMAND}
            -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
            -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_elza_karte>
            -DWORKDIR=${CMAKE_BINARY_DIR}/r37_elzaems_wd
            -DTEIL=${_teil}
            -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_elzaems.cmake)
    set_tests_properties(integration_r37_elzaems_${_teil} PROPERTIES TIMEOUT 1200)
endforeach()
