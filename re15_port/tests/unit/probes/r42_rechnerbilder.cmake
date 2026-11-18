# Runde 42 Strang "gegenstaende", Stufe 2 - Riegel. Belege include/re15_rechnerbilder_r42.h (Rechner-Vollbilder, Standbild-Weg),
# include/re15_gegenstaende_r42.h (Kartenbild 0x47), tools/r42_gegenstaende/michaelkarte.py -> engine/src/gen/gegenstaende_r42_karte.inc.
#
#   unit_r42_rechnerbilder_karte    ITPS-Block 0x47: nur das Namensfeld neu ("MICHAEL"), sonst bytegleich
#   unit_r42_rechnerbilder_r305     3050/3051 sub15: Cut 10 (Bildschirm) zu msg 6, dann Cut 2
#   unit_r42_rechnerbilder_r505     5050/5051 Rechner: Cut 4/5/4 zu msg 1/2/3 beim Einschalten, Cut 5/3 bei "Registration complete."
#   unit_r42_rechnerbilder_s2080    2080/2081 Wasser ablassen: Bild 9 als Standbild, Kamera nie 9, dann Cut 7/3
#   unit_r42_rechnerbilder_leiche9  1050 Leiche (Leon): Cut 9 zu Text 5, dann Cut 3
add_executable(test_r42_rechnerbilder ${CMAKE_CURRENT_LIST_DIR}/../test_r42_rechnerbilder.c)
target_link_libraries(test_r42_rechnerbilder PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_rechnerbilder PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_rechnerbilder PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_rechnerbilder PRIVATE m)
endif()
foreach(_teil karte r305 r505 s2080 leiche9)
    add_test(NAME unit_r42_rechnerbilder_${_teil} COMMAND test_r42_rechnerbilder ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_rechnerbilder_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
