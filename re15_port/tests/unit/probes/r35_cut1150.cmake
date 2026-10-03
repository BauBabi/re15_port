# Spur L (Runde 35) — Tuer ROOM1060->1040, Irons-Todesszene ROOM1150, Knall-Montage 1130/1040/1030,
# Schluss-Schnitt ROOM11C0. Dossier analysis/befunde_runde35/L_cut1150.md, Werkzeuge
# re15_port/tools/r35_l/, Code include/re15_tuer1060.h + engine/src/tuer1060_1040.c,
# include/re15_irons_tod.h + engine/src/irons_tod_1150.c (Haken: scd_room_setup.c, scd_vm.c, main.c).
#
# RIEGEL (Unit): ueber die ECHTEN Haken (Installation beim Raumaufbau, Weiche in scd_event_fire, Reseed-
# Sperre, Signal-Takt). Teile: tuer1060 programme zaehlung szene montage_1130 montage_1040 montage_1030
# montage_11c0 rueckkehr totenpose knallbank.
add_executable(test_r35_cut1150 ${CMAKE_CURRENT_SOURCE_DIR}/test_r35_cut1150.c)
target_link_libraries(test_r35_cut1150 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_cut1150 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r35_cut1150 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_teil tuer1060 programme zaehlung szene montage_1130 montage_1040 montage_1030 montage_11c0 rueckkehr totenpose tot_bleibt_tot knallbank)
    add_test(NAME unit_r35_cut1150_${_teil} COMMAND test_r35_cut1150 ${_teil})
    set_tests_properties(unit_r35_cut1150_${_teil} PROPERTIES TIMEOUT 240)
endforeach()

# MESS-WERKZEUG (kein add_test): Speicherkarte mit Stand in ROOM1130 vor der Tuer zu ROOM1150 fuer den
# CONTINUE-Lauf der echten exe (Flags aus dem GELADENEN Spielstand).
#   probe_r35_cut1150_karte <kartendatei> [nach10f0] [ersteszene] [tot1140] [tot1070] [gesehen]
add_executable(probe_r35_cut1150_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r35_cut1150_karte.c)
target_link_libraries(probe_r35_cut1150_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r35_cut1150_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

# INTEGRATION (echte exe): Spielstand + CONTINUE in ROOM1130, Weg durch die Tuer nach ROOM1150, die
# ganze Kette bis zur Rueckkehr (debug.log-Marken [irons-tod], Raumfolge, Flags), plus die 1060-Sperre.
add_test(NAME integration_r35_cut1150
    COMMAND ${CMAKE_COMMAND}
        -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
        -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r35_cut1150_karte>
        -DWORKDIR=${CMAKE_BINARY_DIR}/r35_cut1150
        -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r35_cut1150.cmake)
set_tests_properties(integration_r35_cut1150 PROPERTIES TIMEOUT 900)
