# Runde 41 Schwanz r9 (Zweig r41/sichel_szene): Riegel "Schwanz haengt nicht frei in der Luft" - Nutzer 2026-11-15 "nicht mehr sauber
# connected". Auswerter probe_r41_schwanz_zusammen (kein add_test) + Szenenlauf integration_r41_schwanz_zusammen (drei exe-Laeufe).
add_executable(probe_r41_schwanz_zusammen ${CMAKE_CURRENT_LIST_DIR}/../probe_r41_schwanz_zusammen.c)

if(TARGET re15_pc)
    add_test(NAME integration_r41_schwanz_zusammen
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PRUEF_TOOL=$<TARGET_FILE:probe_r41_schwanz_zusammen>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_schwanz_zusammen_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_schwanz_zusammen.cmake)
    set_tests_properties(integration_r41_schwanz_zusammen PROPERTIES TIMEOUT 900)
endif()
