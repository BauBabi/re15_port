# Spur K (Runde 37) "roy" — Elzas Roy (NPC-Typ 0x47): Modell aus EMD/CDEMD1.EMS (FUN_80022300 @0x800223ac,
# Elza-Bit bltz @0x80022334), Animationen ueber den allgemeinen NPC-Weg (RBJ-Marker-Binder FUN_8001b3f8 -> ROOM1011
# Record 1; Loco-/eigene Bank des CDEMD1-Blobs in ROOM1021), keine Schleife ueber das Clip-Ende. Elliot (Leon, 1170)
# unveraendert. Dossier analysis/befunde_runde37/K_roy.md.
#
# RIEGEL (Unit): misst die Mechanik am echten VM (RDT laden, scd_room_reenter, Bilder in main.c-Reihenfolge).
#   toc weiche szene1011 szene1021 leon1170 — und `test_r37_roy spur <anim.log> <raum>` fuer die Integration.
add_executable(test_r37_roy ${CMAKE_CURRENT_LIST_DIR}/../test_r37_roy.c)
target_link_libraries(test_r37_roy PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_roy PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r37_roy PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_teil toc weiche szene1011 szene1021 leon1170)
    add_test(NAME unit_r37_roy_${_teil} COMMAND test_r37_roy ${_teil})
    set_tests_properties(unit_r37_roy_${_teil} PROPERTIES TIMEOUT 240)
endforeach()

# INTEGRATION (echte exe): Roy-Lader, Zeichenweg und RE15_ANIM_TRACE je Szene; Leon-Gegenprobe fuer Elliot.
foreach(_teil szene1011 szene1021 leon1170)
    add_test(NAME integration_r37_roy_${_teil}
        COMMAND ${CMAKE_COMMAND}
            -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
            -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_elza_karte>
            -DRE15_SPUR_TOOL=$<TARGET_FILE:test_r37_roy>
            -DWORKDIR=${CMAKE_BINARY_DIR}/r37_roy_wd
            -DTEIL=${_teil}
            -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_roy.cmake)
    set_tests_properties(integration_r37_roy_${_teil} PROPERTIES TIMEOUT 1200)
endforeach()
