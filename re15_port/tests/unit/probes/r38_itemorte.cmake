# Runde 38 (2026-10-16), Spur K "itemorte" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde38/K_itemorte.md, Konstanten include/re15_itemorte_r38.h (+ gen/itemorte_r38_lage.inc).
#
#   unit_r38_itemorte_lage      das gezeichnete Netz liegt im Nutzer-Cut auf der Marke: Loescher ROOM1030 Cut 0 (Mitte +-1 px,
#                               15 vor der triangulierten Pfeilerflaeche), Schluessel 10F0/10F1 Cut 5 (jeder sichtbare Texel in
#                               der Marke, fuellt sie; Licht flach wie RE2 @0x80055320)
#   unit_r38_itemorte_raum      Raumstart 1030 (Zone Slot 14, Monitor-Satz -> 20), 1031/1050 nichts, 10F0/10F1 (Zone Slot 8,
#                               Satz msg 4 -> 15), genommen -> nichts, Installer idempotent
#   unit_r38_itemorte_aufnahme  echter Druck -> Modal -> Ja (Bits, Inventar, Modell weg); danach antwortet der verschobene Satz
#   unit_r38_itemorte_sicht     Sichtbarkeitsregel + Mechanik (Cut_replace 3->10, Anker-Viereck, Masken, Sehstrahl)
add_executable(test_r38_itemorte ${CMAKE_CURRENT_LIST_DIR}/../test_r38_itemorte.c)
target_link_libraries(test_r38_itemorte PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_itemorte PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r38_itemorte PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r38_itemorte PRIVATE m)
endif()
foreach(_teil lage raum aufnahme sicht)
    add_test(NAME unit_r38_itemorte_${_teil} COMMAND test_r38_itemorte ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r38_itemorte_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

#   integration_r38_itemorte  ECHTE exe am Lade-Weg (Spielstand + CONTINUE, Kartenwerkzeug probe_r37_feuerloescher_karte):
#                             Bilder mit/ohne Taken-Bit im Nutzer-Cut (Unterschied = der Gegenstand, in der Marke), Aufnahme
#                             per Eingabeskript (Modal, Ja, danach der verschobene Satz), verdeckte Cuts (1030 Cut 10 nach
#                             dem Rolltor-Tausch, 10F0/10F1 Cut 7) ohne Unterschied, Cut 6 mit; Prison Key auch in der
#                             Spiel-Fenstergroesse (Skala 3); Elza 1031 kein Loescher, 1091 der leere am Boden (Runde 37
#                             unveraendert), 1050 kein Loescher mehr
if(TARGET re15_pc)
    add_test(NAME integration_r38_itemorte
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r37_feuerloescher_karte>
                     -DRE15_BILD_TOOL=$<TARGET_FILE:probe_r34n_e_bild>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r38_itemorte_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_itemorte.cmake)
    set_tests_properties(integration_r38_itemorte PROPERTIES TIMEOUT 1800)
endif()
