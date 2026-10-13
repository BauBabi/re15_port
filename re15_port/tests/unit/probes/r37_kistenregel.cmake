# Runde 37 (2026-10-12), Spur Q "kistenregel" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde37/Q_kistenregel.md, Regel include/re15_kistenregel_r37.h, Tabelle
# engine/src/kistenregel_r37.c (Haken scd_vm.c op_ck, "Runde 37 Spur Q").
#
#   unit_r37_kistenregel_tabelle     die 20 Ck-Stellen (K1..K11) gegen die ausgelieferten RDTs: `21 bank bit wert` am
#                                    Datei-Offset, fertig-Bit wird im Raum gesetzt; keine Zeile fuer reine Skript-
#                                    Gegenstaende (Crank/Time Bomb/Fuse/Latch Key) und fremde Stellen desselben Bits
#   unit_r37_kistenregel_raumstart   je Fall der Platz beim Raumstart: KISTE -> Original-Zweig "nicht im Besitz"
#                                    (Raumtext), INVENTAR -> Benutzungs-Platz wie bisher, FERTIG + Kiste -> Zweig
#                                    "fertig" wie im Original, nie genommen -> unveraendert
#   unit_r37_kistenregel_ablauf      Druck auf den Platz + Bild-Ablauf (Ja auf jede Frage): Nachrichten-Folge KISTE
#                                    gegen INVENTAR (1090: msg 7 ohne Frage 8 / Loeschen; Kartenleser msg 8 / msg 9 ...)
#   unit_r37_kistenregel_spielstand  Kiste + Bits reisen mit re15_savedata; Herausnehmen (echter Tausch) -> Benutzung
add_executable(test_r37_kistenregel ${CMAKE_CURRENT_LIST_DIR}/../test_r37_kistenregel.c)
target_link_libraries(test_r37_kistenregel PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_kistenregel PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r37_kistenregel PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r37_kistenregel PRIVATE m)
endif()
foreach(_teil tabelle raumstart ablauf spielstand)
    add_test(NAME unit_r37_kistenregel_${_teil} COMMAND test_r37_kistenregel ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r37_kistenregel_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

#   integration_r37_kistenregel  ECHTE exe am Lade-Weg (Spielstand + CONTINUE, Kartenwerkzeug der Spur I; Lauf I: echte Box
#                                ROOM1150 + Sprung nach 1090
#                                probe_r37_feuerloescher_karte mit kiste:/give:): 1090 Loescher in der Kiste -> nur
#                                msg 7, keine Loesch-Frage; im Inventar -> Frage + Loeschen (Gegenprobe); Kartenleser
#                                10D0 / Elza 11E1, Zelle 1211 (Elza), 3010, 4030 je Kiste gegen Inventar
if(TARGET re15_pc)
    add_test(NAME integration_r37_kistenregel
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r37_feuerloescher_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r37_kistenregel_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_kistenregel.cmake)
    set_tests_properties(integration_r37_kistenregel PROPERTIES TIMEOUT 1800)
endif()
