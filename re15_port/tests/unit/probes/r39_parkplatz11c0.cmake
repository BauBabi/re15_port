# Runde 39 (2026-10-18), Spur O "parkplatz11c0" — Riegel, zaehlen zur Suite.
# Nutzer (AUFTRAG.md Nachtrag 9): ROOM11C0, waehrend der Gorilla-Kampf laeuft, zeigt die Tuer "I cannot leave Ada alone
# here with those monsters!" statt Leon hinauszulassen.
# Dossier: analysis/befunde_runde39/O_parkplatz11c0.md, Konstanten include/re15_parkplatz_r39.h, Code
# engine/src/parkplatz_r39.c, Programme engine/src/gen/parkplatz_r39_programme.inc (tools/r39_parkplatz/programme_bauen.py).
# Haken (je 1 Zeile, "Runde 39 Spur O"): scd_vm.c (op_evt_exec, re15_scd_show_message), scd_room_setup.c (Installer-Ende),
# platform/pc/main.c (Boot-/CONTINUE-Installer, Messhaken RE15_R39O_GORILLAS_STARR/_TOT).
#
#   unit_r39_parkplatz11c0_texte           msg 40 = Nutzer-Satz woertlich, Erzaehlform, Zeilenbreiten
#   unit_r39_parkplatz11c0_programme       SZENE/ENDE-Bytes, Kopf-Konstanten = RDT-Bytes, Anker, Evt_exec-Haken
#   unit_r39_parkplatz11c0_kampf           echte Szene ab dem Tuer-Spawn: Sperre ab Bild 1, Tuer -> msg 40 (+ Stimme), kein
#                                          Raumwechsel, Leon steuerbar; Gorillas tot -> Tuer zurueck, msg 7, Raumwechsel
#   unit_r39_parkplatz11c0_wiedereintritt  Kampfzustand beim Eintritt: Sperre vor dem ersten Bild, Ende wie oben
#   unit_r39_parkplatz11c0_nachher         nach dem Kampf: Tuer normal
#   unit_r39_parkplatz11c0_ende_gesetzt    (4,64)=0/(3,67)=1 (Abnahme 0 M1): keine Sperre, Tuer -> ROOM11B0
#   unit_r39_parkplatz11c0_echt            echter Kampfausgang (Abnahme 0 H2): Treffer bis zum Tod -> ENDE -> ROOM11B0
#   unit_r39_parkplatz11c0_elza            ROOM11C1 (Elza) unberuehrt
#   unit_r39_parkplatz11c0_fremd           geaendertes RDT: Anker greifen nicht, Original-Ablauf
#   integration_r39_parkplatz11c0_k|w|n|e|i echte exe (test_r39_parkplatz11c0.cmake): Speicherkarte 11B0 -> Tuer -> 11C0,
#                                          Szene, Kampf, Tuer im Kampf, Kampfende, Raumwechsel; Wiedereintritt im Kampf;
#                                          nach dem Kampf Tuer normal; Elza 11C1; I = (4,64)=0/(3,67)=1 ohne Sperre (M1)
add_executable(test_r39_parkplatz11c0 ${CMAKE_CURRENT_LIST_DIR}/../test_r39_parkplatz11c0.c)
target_link_libraries(test_r39_parkplatz11c0 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_parkplatz11c0 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_parkplatz11c0 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_parkplatz11c0 PRIVATE m)
endif()
foreach(_teil texte programme kampf wiedereintritt nachher ende_gesetzt echt elza fremd)
    add_test(NAME unit_r39_parkplatz11c0_${_teil} COMMAND test_r39_parkplatz11c0 ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r39_parkplatz11c0_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

# Je Teil ein eigener Test mit eigenem Arbeitsordner und eigener exe-Kopie (Praefix je Teil, Muster r38_dining1141).
if(TARGET re15_pc AND TARGET probe_r36_schluessel_karte AND
   EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_parkplatz11c0.cmake)
    foreach(_teil K W N E I)
        string(TOLOWER ${_teil} _klein)
        add_test(NAME integration_r39_parkplatz11c0_${_klein}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_schluessel_karte>
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r39_parkplatz11c0_${_klein}_wd
                         -DTEIL=${_teil}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_parkplatz11c0.cmake)
        set_tests_properties(integration_r39_parkplatz11c0_${_klein} PROPERTIES TIMEOUT 1800)
    endforeach()
endif()
