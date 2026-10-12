# Runde 37 (2026-10-12), Spur I "feuerloescher" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde37/I_feuerloescher.md, Konstanten include/re15_feuerloescher_r37.h.
#
#   unit_r37_feuerloescher_modell     MD1 stehend/liegend + TIM + Text (Kopf, Masse, Normalen, Z-Ordnung, CLUT 0)
#   unit_r37_feuerloescher_raum       Raumstart 1050/1051/1090/1091/1000/1001 je Zustand (Prop, Zonen, Spind)
#   unit_r37_feuerloescher_aufnahme   echter Druck 1050 -> Modal -> Ja: (9,0x86), (3,0x85) @0x00D0C, Modell weg
#   unit_r37_feuerloescher_loeschen   1090 sub06 -> Neuladen -> sub03 msg 9 -> Wegwerf-Abfrage Ja/Nein -> Loescher
#   unit_r37_feuerloescher_elza       1091 Druck -> msg 29; Messpunkt Runde 36 -> weiter msg 20
#   unit_r37_feuerloescher_spind      1000 Spind -> msg 0, kein Modal
#   unit_r37_feuerloescher_spielstand Flags + Inventar reisen mit dem Spielstand
#   unit_r37_feuerloescher_leer       Nachtrag 4c: behaltener Loescher heisst "Empty Fire Extinguisher" (Name/CHECK-Text,
#                                     Abfrage-Name), nur Leon nach (3,0x81), auch nach Speichern/Laden
add_executable(test_r37_feuerloescher ${CMAKE_CURRENT_LIST_DIR}/../test_r37_feuerloescher.c)
target_link_libraries(test_r37_feuerloescher PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_feuerloescher PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r37_feuerloescher PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r37_feuerloescher PRIVATE m)
endif()
foreach(_teil modell raum aufnahme loeschen elza spind spielstand leer)
    add_test(NAME unit_r37_feuerloescher_${_teil} COMMAND test_r37_feuerloescher ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r37_feuerloescher_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

# MESS-WERKZEUG (kein add_test): Speicherkarte an frei gewaehlter Stelle (mit Hoehe y:, Flags, Inventar, Figur)
# fuer Laeufe mit der echten exe (CONTINUE-Weg).
add_executable(probe_r37_feuerloescher_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r37_feuerloescher_karte.c)
target_link_libraries(probe_r37_feuerloescher_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r37_feuerloescher_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

#   integration_r37_feuerloescher  ECHTE exe am Lade-Weg: Leon nimmt den Loescher an der Wand ROOM1050 (Modal, Ja,
#                                  (3,0x85)), Sprung nach ROOM1090, Feuer loeschen, Wegwerf-Ja -> Loescher liegt
#                                  (Ende-zu-Ende); Tuer-Weg 1050 -> 1090 mit Flags -> liegt; Elza ROOM1091 -> liegt +
#                                  msg 29; Spind ROOM1000 -> "Nothing unusual."; Bilder je Lauf (RE15_FRAMEDUMP)
if(TARGET re15_pc)
    add_test(NAME integration_r37_feuerloescher
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r37_feuerloescher_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r37_feuerloescher_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_feuerloescher.cmake)
    set_tests_properties(integration_r37_feuerloescher PROPERTIES TIMEOUT 1800)
endif()
