# --- Runde 33 (2026-09-29), Thema S "Speichern nur mit Memory Card" (RE2-Farbband-Ablauf).
#     Dossier: analysis/befunde_runde33/speichern_memory_card.md
#     RE2-Vorbild: AOT-Typ 9 -> 0x80051AB0, Farbband 0x1E nur im Inventar (FUN_800696CC @0x80051b34),
#     ohne Farbband Meldung (0x100,0) @0x8009EFCC, mit Farbband Meldung (0x100,1) @0x8009F01F +
#     Ja/Nein, JA -> Speicherbildschirm (@0x80051bd0-dc). Port: Memory Card (0x21) statt Farbband,
#     KEIN Verbrauch (PORT-WAHL, Nutzerwunsch).
#
#   unit_r33_speichern          Engine-Riegel: Texte an allen 16 Speicherstellen (RDT-Anfang +
#                               RE2-Schluss), ROOM1150 ohne Karte / Karte nur in der Kiste /
#                               mit Karte JA / mit Karte NEIN, Skript-Freeze waehrend des Textes.
#   integration_r33_speichern   echte exe (Spielstand in ROOM1150, echtes Untersuchen des Telefons
#                               ueber AOT-Slot 3, Tasten ueber RE15_PAD_AT): ohne Karte kein
#                               Speichern; mit Karte + JA gespeichert, Karte danach noch da (Log
#                               UND geschriebener Kartenblock); mit Karte + NEIN kein Speichern.
add_executable(probe_r33_speichern probe_r33_speichern.c)
target_link_libraries(probe_r33_speichern PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r33_speichern PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r33_speichern PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r33_speichern COMMAND probe_r33_speichern)
set_tests_properties(unit_r33_speichern PROPERTIES TIMEOUT 60)

# Kartenwerkzeug (kein eigener Test): Spielstand ROOM1150 vor dem Telefon, optional mit Memory Card.
add_executable(probe_r33_speichern_karte probe_r33_speichern_karte.c)
target_link_libraries(probe_r33_speichern_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r33_speichern_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(TARGET re15_pc)
    add_test(NAME integration_r33_speichern
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r33_speichern_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r33_speichern_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r33_speichern.cmake)
    set_tests_properties(integration_r33_speichern PROPERTIES TIMEOUT 600)
endif()
