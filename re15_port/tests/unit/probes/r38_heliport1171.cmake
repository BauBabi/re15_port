# Spur I (Runde 38) "heliport1171" — Elza haengt in ihrer Szene ROOM1171: sub02 wartet @0x013F4 auf (5,0x1C), den
# im Original NUR die Ereignis-Kraehe beim Rueckgriff setzt (FLIGHT-2 HOVER @0x801152c4, SET 0x8004ef90 auf 0x800b1028).
# Port: Kraehen-Flagwort = Spiel-Bank 5, Ereignis-Kraehe immer RE1.5-Hirn/-Bank, Rueckgriff-Opfermaschine
# (Hook A [0x21][1] 0x80115b80) mit Wurf in Modus 6. Dossier analysis/befunde_runde38/I_heliport1171.md.
#
# RIEGEL (Unit): echte RDTs, scd_room_reenter, Bilder in main.c-Reihenfolge — misst die MECHANIK in beiden KI-Modi.
add_executable(test_r38_heliport1171 ${CMAKE_CURRENT_LIST_DIR}/../test_r38_heliport1171.c)
target_link_libraries(test_r38_heliport1171 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_heliport1171 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_heliport1171 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_teil banken zensus flagwort szene_re2 szene_re15 danach leon1170)
    add_test(NAME unit_r38_heliport1171_${_teil} COMMAND test_r38_heliport1171 ${_teil})
    set_tests_properties(unit_r38_heliport1171_${_teil} PROPERTIES TIMEOUT 240)
endforeach()
