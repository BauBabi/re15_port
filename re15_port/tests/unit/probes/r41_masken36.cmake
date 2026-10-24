# Runde 41 (2026-10-24), Spur M "masken36" — Riegel, zaehlen zur Suite. Dossier: analysis/befunde_runde41/M_masken36.md.
#
# Punkt 1 (AUFTRAG.md): "Die von dir erstellten Masken fuer STAGE3-6 sind relativ unbrauchbar, weil die ueberall Loecher
# aufweisen." -> "danach kannst du deine Masken schliessen in der naechsten Version."
#   unit_r41_masken36_loecher  Mechanik des Schliessens an allen 216 Generator-Sektionen (test_r41_masken36.c): eigener Atlas,
#                             den der Lader nimmt; offene Kerben (Kuenstler-Atlas deckend + farbgleich direkt an der Kante
#                             eines alten Rechtecks, nicht gezeichnet) NACHHER <= 30 % von VORHER (gemessen 18,4 %, vor NB1 25,1 %), Gegenprobe VORHER > 10000; Blit.
add_executable(test_r41_masken36 ${CMAKE_CURRENT_LIST_DIR}/../test_r41_masken36.c)
target_link_libraries(test_r41_masken36 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_masken36 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_masken36 PRIVATE RE15_PORT_SRC_DIR="${CMAKE_SOURCE_DIR}")
if(NOT WIN32)
    target_link_libraries(test_r41_masken36 PRIVATE m)
endif()
add_test(NAME unit_r41_masken36_loecher COMMAND test_r41_masken36 loecher WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_masken36_loecher PROPERTIES TIMEOUT 600)
# Punkt 2 (AUFTRAG.md): "Ich habe diverse neue PRIs fuer STAGE2 erstellt. die sollen alle sauber eingebaut werden!"
#   unit_r41_masken36_kurbel  ROOM2020 Cut 3 an der Kurbel (Pos_set @0x0A8C (3650,-1800,-2925)): die Sichtlinie Kamera ->
#                             Leon kreuzt die solide Zelle der Suedreling (Leon steht HINTER ihr), das Original hat in Cut 3
#                             keine Masken (NULL-Sektion), die Reling-Maske verdeckt die Beine nach dem Original-Urteil.
add_test(NAME unit_r41_masken36_kurbel COMMAND test_r41_masken36 kurbel WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
#   integration_r41_masken36_stage2  echte exe (Leon und Elza): ROOM2020 Cut 3 an der Kurbel - die Reling-Maske verdeckt die
#                             Beine (Leon steht HINTER der Reling); an der strengsten VOR-Stelle 0 Figurpunkte uebermalt,
#                             an der HINTER-Stelle >= Haelfte verdeckt (11A0, 2030, 2040, 2050, 2080); C = probe_bg_dump.
if(TARGET re15_pc)
    add_test(NAME integration_r41_masken36_stage2
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r37_masken36_ppm>
                     -DRE15_BG_TOOL=$<TARGET_FILE:probe_bg_dump>
                     -DCD_ROOT=${CMAKE_SOURCE_DIR}/shared_assets/PSX
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_masken36_s2_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_stage2.cmake)
    set_tests_properties(integration_r41_masken36_stage2 PROPERTIES TIMEOUT 1500)
endif()

# Nachbesserung 1 (Abnahme 0, Mangel 1): ausgeschlossene Generator-Objekte der Stages 3..6 mit der Tiefe ihrer Kollisionszelle
#   integration_r41_masken36_zelle  echte exe (Leon): je Objekt eine HINTER-Stelle (die wieder aufgenommenen Rechtecke verdecken
#                             Leon: H >= Haelfte bzw. H > 0) und eine DAVOR-Stelle (Leon ueberlappt sie, 0 Punkte uebermalt);
#                             Szenen test_r41_masken36_zelle_szenen.cmake, Laeufer wie integration_r41_masken36_stage2.
if(TARGET re15_pc)
    add_test(NAME integration_r41_masken36_zelle
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r37_masken36_ppm>
                     -DRE15_BG_TOOL=$<TARGET_FILE:probe_bg_dump>
                     -DCD_ROOT=${CMAKE_SOURCE_DIR}/shared_assets/PSX
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_masken36_zelle_wd
                     -DSZENEN=${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_zelle_szenen.cmake
                     -DPRAEFIX=r41m3
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_stage2.cmake)
    set_tests_properties(integration_r41_masken36_zelle PROPERTIES TIMEOUT 1500)
endif()

# Nachbesserung 2 (Abnahme 1, Mangel 1, Weg (a)): Generator-Rechtecke mit Zellen-Modell (tests/unit/gen_r41_masken36_zellen.inc,
# tools/maske/zellen3d_r41.py) - der Ausschluss urteilt fuer sie in 3D (Koerper der Modellzelle vor dem Figurpunkt).
#   unit_r41_masken36_zellen3d  jede Modell-Zeile byte-genau im RDT, MSK-Tiefe == Zeile, Engine zeichnet sie; Ausschluss MIT 3D-Urteil
#                             == Engine-Tabelle in jedem Modell-Cut, Gegenprobe OHNE 3D-Urteil: >= 125 Rechtecke fielen weg (gemessen 125),
#                             alle mit Modell-Zeile. Modelle auch aus der Zuordnung von Hand (tools/maske/zuordnung_r41.json, 16 Objekte).
add_test(NAME unit_r41_masken36_zellen3d COMMAND test_r37_masken36 zellen3d WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_masken36_zellen3d PROPERTIES TIMEOUT 1500)
#   integration_r41_masken36_zellen3d  echte exe: die in NB2 gezeichneten Objekte (ROOM6000 Lok/Traeger, 5050 C1, 3040 C0/C3, X-Kiste
#                             5020 C6, Treppenraum 5020 C0, Trennwand 30A0 C1, Kistenstapel 5090 C13)
#                             verdecken Leon HINTER ihnen (H >= Haelfte bzw. H > 0) und uebermalen ihn DAVOR nicht (H == 0);
#                             Szenen test_r41_masken36_zellen3d_szenen.cmake, Laeufer wie integration_r41_masken36_stage2.
if(TARGET re15_pc)
    add_test(NAME integration_r41_masken36_zellen3d
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r37_masken36_ppm>
                     -DRE15_BG_TOOL=$<TARGET_FILE:probe_bg_dump>
                     -DCD_ROOT=${CMAKE_SOURCE_DIR}/shared_assets/PSX
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_masken36_zellen3d_wd
                     -DSZENEN=${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_zellen3d_szenen.cmake
                     -DPRAEFIX=r41m4
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_stage2.cmake)
    set_tests_properties(integration_r41_masken36_zellen3d PROPERTIES TIMEOUT 1500)
endif()
