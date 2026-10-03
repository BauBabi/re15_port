# Runde 35 / Spur E — inventar1050: Inventar nach der Ada-Szene ROOM1050, Kampfmesser als Rueckfall,
# Selbstgespraech wie ROOM1170. Dossier: analysis/befunde_runde35/E_inventar1050.md,
# Konstanten: include/re15_messer.h, include/re15_adaruf.h.
#
#   test_r35_inventar1050  RIEGEL Punkt 2 (Messer), Teil ueber das Argument:
#     unit_r35_inventar1050_start       Startinventar ohne Messer, 25c8 0x80, Waffe 1
#     unit_r35_inventar1050_elza        Elza-Tabelle @0x80074bc4 -> leer, Waffe 1
#     unit_r35_inventar1050_ablegen     Ablegen im Statusschirm -> Commit 0x80 -> Waffe 1
#     unit_r35_inventar1050_altstand    alter Spielstand (Messer Platz 0) -> Messer weg, 0x80, Waffe 1
#     unit_r35_inventar1050_altpistole  alter Stand mit Pistole -> Pistole Platz 0, Waffe 3
#     unit_r35_inventar1050_kiste       Messer aus der Kiste eines alten Stands entfernt
#     unit_r35_inventar1050_breit       breite Waffe ohne Ausruestung: 25c8 bleibt 0x80 (RE2)
#     unit_r35_inventar1050_reserve     Munition in Platz 0 = Reserve (RE2)
#     (karte <pfad> = Werkzeug fuer den Integrationsriegel)
#   Punkte 1 und 3 (Szene, Pad-Bit, Inventar nach der Szene) misst der vorhandene Riegel
#   unit_r34n_d_adaruf_szene/_raster (tests/unit/test_r34n_d_adaruf.c, in Runde 35 umgestellt).
#   integration_r35_inventar1050  ECHTE exe: (A) Tuer ROOM1000 -> ROOM1050 -> Szene -> START = Inventar
#     offen, Blick zur Tuer, Clip 18/17, kein Clip 19; (B) Neues Spiel -> Messerstich ohne Waffe;
#     (C) CONTINUE mit altem Stand (Messer Platz 0) -> Messer entfernt, Stich mit dem Messer.
add_executable(test_r35_inventar1050 ${CMAKE_CURRENT_SOURCE_DIR}/test_r35_inventar1050.c)
target_link_libraries(test_r35_inventar1050 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_inventar1050 PRIVATE ${CMAKE_SOURCE_DIR}/include)
foreach(_r35e_teil start elza ablegen altstand altpistole kiste breit reserve)
    add_test(NAME unit_r35_inventar1050_${_r35e_teil} COMMAND test_r35_inventar1050 ${_r35e_teil})
    set_tests_properties(unit_r35_inventar1050_${_r35e_teil} PROPERTIES TIMEOUT 60)
endforeach()

if(TARGET re15_pc)
    add_test(NAME integration_r35_inventar1050
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:test_r35_inventar1050>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r35_inventar1050_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r35_inventar1050.cmake)
    set_tests_properties(integration_r35_inventar1050 PROPERTIES TIMEOUT 900)
endif()
