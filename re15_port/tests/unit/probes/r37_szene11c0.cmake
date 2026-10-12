# Runde 37 / Spur O "szene11c0" — Ada/Marvin-Szene ROOM11C0 nach Irons' Tod (Dossier analysis/befunde_runde37/O_szene11c0.md).
# Code: Programm 4 k_p_11c0 in engine/src/irons_tod_1150.c; Rechenhilfe + Mess-Haken engine/src/szene11c0_r37.c.
#
#   test_r37_szene11c0   RIEGEL (Teil ueber das Argument):
#     unit_r37_szene11c0_aufstellung  Anfangs-Gieren = Peilungen auf den Partner (re15_s11c0_gier, FUN_8001a6d4-Form);
#                                     Vorbild ROOM10D0 @0x01ABC..@0x01AF6 (Dir_set = Peilung +-17), Adas Record 475
#                                     @0x01770; kein Plc_dest Modus 9 im Gespraech, ein Lauf erst nach Marvins letzter
#                                     Zeile, danach EINE Drehung Adas nach dem Signal "Ada folgt Marvin" (Nachtrag 12),
#                                     Knall nur Koepfe
#     unit_r37_szene11c0_ablauf       die Szene im Spieltakt: einander zugewandt ab dem ersten Bild, im Gespraech dreht
#                                     sich niemand, Marvin nur beim Losrennen (< 120 Grad), je Zeile Sprecher und
#                                     Zuhoerer zum Partner (Kegel 0x60 @0x80076c41), Ada dreht sich dem laufenden Marvin
#                                     nach (< 90 Grad) und ihr Kopf folgt ihm, Schnitt nach 1150 Cut 7
add_executable(test_r37_szene11c0 ${CMAKE_CURRENT_SOURCE_DIR}/test_r37_szene11c0.c)
target_link_libraries(test_r37_szene11c0 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_szene11c0 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r37_szene11c0 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_teil aufstellung ablauf)
    add_test(NAME unit_r37_szene11c0_${_teil} COMMAND test_r37_szene11c0 ${_teil})
    set_tests_properties(unit_r37_szene11c0_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

# INTEGRATION (echte exe): Spielstand + CONTINUE in ROOM1130, Tuer -> ROOM1150 -> Szene -> Montage bis ROOM11C0 Bild 505,
# Mess-Haken RE15_S11C0_LOG + RE15_NECK_TRACE (der GEZEICHNETE Kopf); Auswertung = test_r37_szene11c0 auswerten <lauf>:
# einander zugewandt im ersten Bild, im Gespraech keine Drehung, Marvin nur beim Losrennen, je Zeile Koerper und
# gezeichneter Kopf-Blick von Sprecher und Zuhoerer zum Partner (<= 8,4 Grad), Ada dreht sich dem laufenden Marvin nach.
add_test(NAME integration_r37_szene11c0
    COMMAND ${CMAKE_COMMAND}
        -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
        -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r35_cut1150_karte>
        -DRE15_AUSWERTER=$<TARGET_FILE:test_r37_szene11c0>
        -DWORKDIR=${CMAKE_BINARY_DIR}/r37_szene11c0_lauf
        -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_szene11c0.cmake)
set_tests_properties(integration_r37_szene11c0 PROPERTIES TIMEOUT 900)
