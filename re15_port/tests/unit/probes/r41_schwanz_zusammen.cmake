# Runde 41 Schwanz r9 (Zweig r41/sichel_szene): Riegel "Schwanz haengt nicht frei in der Luft" - Nutzer 2026-11-15 "nicht mehr sauber
# connected". Auswerter probe_r41_schwanz_zusammen (kein add_test) + Szenenlauf integration_r41_schwanz_zusammen (drei exe-Laeufe).
add_executable(probe_r41_schwanz_zusammen ${CMAKE_CURRENT_LIST_DIR}/../probe_r41_schwanz_zusammen.c)
# Runde 5 (Nutzer 2026-11-15 "Oberseite ... wieder eher rot als dunkel" / "NUR DIE UNTERSEITE WAR KOMISCH MIT DEN ABSCHNITTEN"): Farbregel 8g im Spiel
add_executable(probe_r41_schwanz_farbe ${CMAKE_CURRENT_LIST_DIR}/../probe_r41_schwanz_farbe.c)

if(TARGET re15_pc)
    add_test(NAME integration_r41_schwanz_zusammen
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PRUEF_TOOL=$<TARGET_FILE:probe_r41_schwanz_zusammen>
                     -DRE15_FARB_TOOL=$<TARGET_FILE:probe_r41_schwanz_farbe>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_schwanz_zusammen_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_schwanz_zusammen.cmake)
    set_tests_properties(integration_r41_schwanz_zusammen PROPERTIES TIMEOUT 1500)
endif()

# Release-Bau v0.8.30 (release/logs/v0.8.30_20261117-204645/03_linux.log Z. 1903-1923): beide Auswerter rufen libm
# (lround/acos/ceil/atan2/sincos/sqrt) und binden weder re15_engine noch m - unter mingw liefert msvcrt die Funktionen mit,
# im Linux-Container brach der Link mit "undefined reference to `lround'" ab.
if(NOT WIN32)
    target_link_libraries(probe_r41_schwanz_zusammen PRIVATE m)
    target_link_libraries(probe_r41_schwanz_farbe PRIVATE m)
endif()
