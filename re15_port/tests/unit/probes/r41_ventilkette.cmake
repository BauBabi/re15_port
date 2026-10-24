# Runde 41 Spur C "ventilkette" — STAGE2-Kette: Leiter 2000 (Text 40), Bruecke + Pult + Ventil 13, das Ventilraetsel 2030
# (sechs Ventile, Reihenfolge, Dokument + Plakat; Nachtrag 13), Zaehler/Flut/Ertrinken, 20B0 statt 2000, Schaltpult 2050
# (80 V) sperrt den Hebel; mit dem Strom (Generator 2060, (3,126)) steht 2030 knapp unter der Absperrung (Nachtraege 7/8/12),
# der Rechner 2080 bleibt Original.
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

# Echter Weg mit der exe (nur PC-Ziel): A Sprung nach 2030 (Sued-Landeplatz), die richtige Ventil-Folge zu Fuss -> Warnung,
# Zaehler, Tuer Slot 0 feuert -> ROOM20B0 geladen (Umlenkung), Bits 103 + 100; B 2000 mit Leiter-Text; C Zaehler abgelaufen
# -> Flut -> tot (Nachbesserung 1, M4: beschleunigter Renderer, Farbe der Ertrink-Blende ueber das ppm-Werkzeug von
# test_r41_ventilkette); D Ventil ohne Kasten-Sicherung = Text 49 "It's locked.". Nachbesserung 5: mit der RE2-Dreh-Animation
# laeuft A in 2030 bis F4700 (Countdown-Ausschnitt F4440/F4560/F4680) + 60 Bilder in 20B0, C bis F6600; neu E (Abnahme 4
# Mangel 2: Plakat waehrend des Zaehlers gesperrt, Flut-Sperre bis zum Tod) bis F6450.
if(RE15_BUILD_PC AND TARGET re15_pc)
    add_test(NAME integration_r41_ventilkette
             COMMAND ${CMAKE_COMMAND}
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_R41C_WERKZEUG=$<TARGET_FILE:test_r41_ventilkette>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_ventilkette
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_ventilkette.cmake)
    set_tests_properties(integration_r41_ventilkette PROPERTIES TIMEOUT 1500 RUN_SERIAL TRUE)
endif()

# Teil B (dritte Fortsetzung 2026-10-24): die Glieder, die Teil A nicht faehrt — Pult 2050 -> 80 V -> Hebel = Original
# sub04 (Leon + Elza), Hebel vorher gesperrt, 2030-Pult ohne Ventil 13, Leitern 20B0 <-> 2050 nach der Flut, 2030 nach der
# Flut ohne Strom (y -7200) und mit Strom (3,126) (y -5350, knapp unter der Absperrung; Nachtraege 7/8/12), Ring-Zugaenge,
# Elza-Leiter 2001. Eigener exe-Kopie-Praefix re15_pc_r41cb_.
if(RE15_BUILD_PC AND TARGET re15_pc)
    add_test(NAME integration_r41_ventilkette_b
             COMMAND ${CMAKE_COMMAND}
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_ventilkette_b
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_ventilkette_b.cmake)
    set_tests_properties(integration_r41_ventilkette_b PROPERTIES TIMEOUT 1200 RUN_SERIAL TRUE)
endif()

# Nachbesserung 1 (Abnahme 0, M2; Orchestrator-Entscheid C-E1 2026-10-24), seit Nachbesserung 4 auf der heutigen Kette: die
# ganze Kette in EINEM Lauf - 2030 richtige Ventil-Folge zu Fuss -> Ost-Tuer zu Fuss -> 20B0-Szene sub02/sub05 -> (3,235)=1
# -> frei -> Kanal -> Leiter -> 2050. Braucht den AUTO-Haken der Spur A (aot_common.c re15_wasser20b0_r41_auto_aot) -> NUR
# registriert, wenn A's Modul im Baum liegt (ohne den Haken prueft der Riegel von Hand nur bis zur 20B0-Ankunft).
if(RE15_BUILD_PC AND TARGET re15_pc)
    if(EXISTS ${CMAKE_SOURCE_DIR}/engine/src/wasser20b0_r41.c)
        add_test(NAME integration_r41_ventilkette_kette
                 COMMAND ${CMAKE_COMMAND}
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_SRC=${CMAKE_SOURCE_DIR}
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_ventilkette_kette
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_ventilkette_kette.cmake)
        set_tests_properties(integration_r41_ventilkette_kette PROPERTIES TIMEOUT 900 RUN_SERIAL TRUE)
    else()
        message(STATUS "integration_r41_ventilkette_kette uebersprungen: Spur A fehlt (engine/src/wasser20b0_r41.c, AUTO-Haken re15_wasser20b0_r41_auto_aot)")
    endif()
endif()

# Nachbesserung 3/4/5 (Abnahme 2 N1/N2/N4, Abnahme 3 Maengel 3/4, Abnahme 4; Nachtraege 7/9/12/13/16/18/19): die Kette in 2030
# und der Weg nach der Flut zu Fuss, je Figur - v1 Bruecke, v2a Ventil ohne Kasten-Sicherung = 49 "It's locked." (auch mit der
# Sicherung in der Hand), v2b richtige Folge mit RE2-Dreh-Animation -> Warnung -> Flucht in 60 s -> 20B0, v2c Plakat-Folge =
# falsch -> zurueck, v2d Dokument -> Leser -> Bit 107, v2p Plakat-Nahaufnahme, k1 Ventilkasten 2010 (Standbild + Ton),
# k2 nach der Flut Sicherung aus dem Kasten -> zu Fuss bis 2060, k3 Generator-Kasten 2060 mit Bit 125 = Text 3,
# g1/g2 Sicherung + Generator 2060 (Original-Raetsel, (3,126)), w1 20B0 -> 2050 -> 2020 -> 2030 Nordgalerie (geflutet) -> 2060,
# w3 2060 -> 2030 (Pegel -5350) -> 2020 -> 2050 -> 20B0, w4 20B0 -> Ost-Tuer -> 2030 Ring -> Lift B -> 2080 -> Rechner
# (Original) -> 2090. Eigene exe-Praefixe re15_pc_r41cw<figur>_.
if(RE15_BUILD_PC AND TARGET re15_pc)
    foreach(_r41c_fig leon elza)
        set(_r41c_name integration_r41_ventilkette_weg)
        if(_r41c_fig STREQUAL "elza")
            set(_r41c_name integration_r41_ventilkette_weg_elza)
        endif()
        add_test(NAME ${_r41c_name}
                 COMMAND ${CMAKE_COMMAND}
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_R41C_WERKZEUG=$<TARGET_FILE:test_r41_ventilkette>
                         -DRE15_R41C_PSX=${RE15_ASSETS_PATH}
                         -DFIGUR=${_r41c_fig}
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_ventilkette_weg_${_r41c_fig}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_ventilkette_weg.cmake)
        set_tests_properties(${_r41c_name} PROPERTIES TIMEOUT 3600 RUN_SERIAL TRUE)
    endforeach()
endif()

# Nachtrag 6 (C-E2c widerrufen): die ORIGINAL-Sperre ROOM11A0/11A1 slot 4 ((3,206), @0x00FEA/@0x00FEE) bleibt — 2040 Halle ->
# 2070 Nordgang -> 11A0 bis zur Sperre (Leon + Elza), Gegenprobe (3,206)=1 -> 20A0 (vormals Laeufe a/b/c von _weg).
if(RE15_BUILD_PC AND TARGET re15_pc)
    add_test(NAME integration_r41_ventilkette_sperre
             COMMAND ${CMAKE_COMMAND}
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_ventilkette_sperre
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_ventilkette_sperre.cmake)
    set_tests_properties(integration_r41_ventilkette_sperre PROPERTIES TIMEOUT 1500 RUN_SERIAL TRUE)
endif()
