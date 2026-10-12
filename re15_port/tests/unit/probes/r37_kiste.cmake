# --- Runde 37 (2026-10-12), Spur M "kiste". Dossier: analysis/befunde_runde37/M_kiste.md
#     Nutzer (Nachtrag 9): Symbole in der Kiste so weit rechts wie moeglich, kein Name ueberdeckt sie;
#     beim Scrollen liegt der Kasten (Rahmen) layertechnisch UEBER den Items.

# Kartenwerkzeug (kein eigener Test): Spielstand vor der Kiste, Kiste mit den laengsten Namen gefuellt.
add_executable(probe_r37_kiste_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r37_kiste_karte.c)
target_link_libraries(probe_r37_kiste_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r37_kiste_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r37_kiste_karte PRIVATE m)
endif()

#   unit_r37_kiste   Engine-Riegel ueber die echte Op-Liste + Kisten-FSM: (A) Zensus ALLER Item-Ids 1..255 in drei
#                    Spielzustaenden (Anfang / alle Flags Leon / alle Flags Elza): kein gemaltes Namenspixel im Symbol,
#                    Symbol buendig am Fensterrand boxX+0xc5 (RE2 @0x80071238), Ziffern im Symbol unten links und ueber
#                    ihm (RE2 FUN_80072c6c @0x800aae2c, OT 3 ueber 6); (B) jedes Gleit-Bild RUNTER/HOCH inkl. Ring-Naht:
#                    die vier Rahmen-Teile liegen ueber allen Zeilen-Ops (RE2 Rahmen OT-Eintrag 2 ueber 6/3), jede
#                    Zeilen-Op im Panel, einlaufende Zeile scroll-1/scroll+5 vorhanden; (C) Band/Griff/Panel unveraendert.
add_executable(test_r37_kiste ${CMAKE_CURRENT_LIST_DIR}/../test_r37_kiste.c)
target_link_libraries(test_r37_kiste PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_kiste PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r37_kiste PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r37_kiste PRIVATE m)
endif()
add_test(NAME unit_r37_kiste COMMAND test_r37_kiste)
set_tests_properties(unit_r37_kiste PROPERTIES TIMEOUT 120)

# Bildwerkzeug (kein eigener Test): prueft Framedumps der exe gegen BOXPANEL.BIN (Rahmen unveraendert, kein Zeilen-
# Pixel ausserhalb des Listenfensters, Symbol buendig rechts mit freier Spalte davor, einlaufende Zeile sichtbar).
add_executable(probe_r37_kiste_bild ${CMAKE_CURRENT_LIST_DIR}/../probe_r37_kiste_bild.c)
if(NOT WIN32)
    target_link_libraries(probe_r37_kiste_bild PRIVATE m)
endif()

#   integration_r37_kiste   echte exe, echter Weg: Spielstand vor der Kiste (CONTINUE), Viereck oeffnet die Kiste ueber
#                           das Kisten-AOT, Box-Seite, RUNTER, HOCH; RE15_FRAMEDUMP jedes Bild F95..F130
#                           (RE15_WINDOW_SCALE=1) -> probe_r37_kiste_bild. Leon ROOM1150, Elza ROOM1151, Leon ROOM2010.
if(TARGET re15_pc)
    add_test(NAME integration_r37_kiste
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r37_kiste_karte>
                     -DRE15_BILD_TOOL=$<TARGET_FILE:probe_r37_kiste_bild>
                     -DRE15_BOXPANEL=${CMAKE_SOURCE_DIR}/shared_assets/RE2/BOXPANEL.BIN
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r37_kiste_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_kiste.cmake)
    set_tests_properties(integration_r37_kiste PROPERTIES TIMEOUT 900)
endif()
