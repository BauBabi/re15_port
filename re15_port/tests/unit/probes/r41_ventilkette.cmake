# Runde 41 Spur C "ventilkette" — STAGE2-Kette: Leiter 2000 (Text 40), Bruecke + Pult + Ventil 13 + Ventile 2030,
# Zaehler/Flut/Ertrinken, 20B0 statt 2000, Schaltpult 2050 (80 V) sperrt den Hebel, Rechner 2080 entleert 2030.
# Herleitung include/re15_ventilkette_r41.h, Dossier analysis/befunde_runde41/C_ventilkette.md.
#
# Mechanik-Riegel (echte RDTs, Leon + Elza, Raumaufbau wie im Spiel, Aktion vor jeder Stelle, Takt fuer Takt):
add_executable(test_r41_ventilkette test_r41_ventilkette.c)
target_link_libraries(test_r41_ventilkette PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_ventilkette PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r41_ventilkette PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r41_ventilkette COMMAND test_r41_ventilkette)
set_tests_properties(unit_r41_ventilkette PROPERTIES TIMEOUT 300)

# Echter Weg mit der exe (nur PC-Ziel): Sprung nach 2030 (Ost-Tuer), Ventile -> Frage -> Ja -> Warnung, Zaehler,
# Tuer Slot 0 feuert -> ROOM20B0 geladen (Umlenkung), Szene sub02 (Cut 11), Bits 103 + 100; zweiter Lauf: 2000 mit
# Leiter-Text, dritter Lauf: Zaehler abgelaufen -> Flut -> tot.
if(RE15_BUILD_PC AND TARGET re15_pc)
    add_test(NAME integration_r41_ventilkette
             COMMAND ${CMAKE_COMMAND}
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_ventilkette
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_ventilkette.cmake)
    set_tests_properties(integration_r41_ventilkette PROPERTIES TIMEOUT 900 RUN_SERIAL TRUE)
endif()

# Teil B (dritte Fortsetzung 2026-10-24): die Glieder, die Teil A nicht faehrt — Pult 2050 -> 80 V -> Hebel = Original
# sub04 (Leon + Elza), Hebel vorher gesperrt, 2030-Pult ohne Ventil 13, Leitern 20B0 <-> 2050 nach der Flut, 2030 mit
# Bit 107 ohne Wasser (Gegenprobe ohne 107), Elza-Leiter 2001. Eigener exe-Kopie-Praefix re15_pc_r41cb_.
if(RE15_BUILD_PC AND TARGET re15_pc)
    add_test(NAME integration_r41_ventilkette_b
             COMMAND ${CMAKE_COMMAND}
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_ventilkette_b
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_ventilkette_b.cmake)
    set_tests_properties(integration_r41_ventilkette_b PROPERTIES TIMEOUT 1200 RUN_SERIAL TRUE)
endif()
