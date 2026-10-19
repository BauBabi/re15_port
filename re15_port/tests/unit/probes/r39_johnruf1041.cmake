# Runde 39 (2026-10-19), Spur P "johnruf1041" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde39/P_johnruf1041.md, Konstanten include/re15_johnruf_r39.h, Code engine/src/johnruf_r39.c,
# Programme engine/src/gen/johnruf_r39_programme.inc (tools/r39_johnruf/programme_bauen.py).
# Haken (je 1-2 Zeilen, "Runde 39 Spur P"): scd_room_setup.c (Installer am Blockende), platform/pc/main.c (Boot-/CONTINUE-
# Weg), scd_vm.c (Opcode 0x5F, op_message_on), game_step_common.c (Tick), audio_pc.c (Weiche, Sperre, Stimmen-Ordner,
# CD_VOL), map_hint_common.c (Eintrag 1041 -> 1141), tests/unit/test_r36_kartenbits.c k_port[] (Bit 97).
#
#   unit_r39_johnruf1041_texte      msg 40 = Wortlaut des Nutzers, Form ROOM1041 msg 2
#   unit_r39_johnruf1041_programme  RUF1041/RUF1131: Reihenfolge, Lautstaerken, Original-Zeilen bytegleich (ROOM1041 sub11)
#   unit_r39_johnruf1041_stimme     (1041|1131, Id 53..57) -> room1141/help01..05, sonst unveraendert
#   unit_r39_johnruf1041_xa         Opcode 0x5F mit PC-Schranke, CD_VOL-Rechnung, Rueckstellung je Raum
#   unit_r39_johnruf1041_ablauf     ROOM1041 am VM: Rufe leise, Drehung, msg 40, Karte 1141, Bit 97, Steuerung, kein 2. Mal
#   unit_r39_johnruf1041_musik      Weiche/Sperre, Kampf, Rettung -> Bit 97 aus + Tabellen-Musik, Laden in der Phase
#   unit_r39_johnruf1041_ruf1131    1131: ein Ruf lauter, einmal je Phase, nicht ohne Phase / nach der Rettung
#   unit_r39_johnruf1041_leon       Leon ROOM1040 unveraendert
# Nachbesserung 1 (Nutzer-Nachtrag 12; include/re15_tuersperre_r39.h, engine/src/tuersperre_r39.c, gen/tuersperre_r39.inc):
#   unit_r39_johnruf1041_tuertexte  Satz 1041/1051 = Nutzer-Wortlaut in Erzaehlform; 10B1 = RE2 ROOM21A0 msg 4; Ids; Stimme
#   unit_r39_johnruf1041_tuer1041   Tuer -> ROOM1031: ohne Phase Raumwechsel, in der Phase Text 41; Beginn in 1041 sperrt sofort
#   unit_r39_johnruf1041_tuer1051   Tuer -> ROOM10A1: ohne Phase Raumwechsel, in der Phase Text 40; Leon 1050 unveraendert
#   unit_r39_johnruf1041_tuer10b1   Kanaldeckel -> ROOM1261 in allen Zustaenden Text 40; Leiter bleibt; Leon 10B0 unveraendert
#   unit_r39_johnruf1041_tuerszene  Sherry-Szene sub04: Text statt Raumwechsel; ohne Huelle bliebe die Pad-Sperre stehen
add_executable(test_r39_johnruf1041 ${CMAKE_CURRENT_LIST_DIR}/../test_r39_johnruf1041.c)
target_link_libraries(test_r39_johnruf1041 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_johnruf1041 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_johnruf1041 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_RE2_RDT_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon/PL0/RDT")
if(NOT WIN32)
    target_link_libraries(test_r39_johnruf1041 PRIVATE m)
endif()
foreach(_teil texte programme stimme xa ablauf musik ruf1131 leon tuertexte tuer1041 tuer1051 tuer10b1 tuerszene)
    add_test(NAME unit_r39_johnruf1041_${_teil} COMMAND test_r39_johnruf1041 ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r39_johnruf1041_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

# INTEGRATION (echte exe, Elza, Spielstand + CONTINUE, echte Tueren; tests/integration/test_r39_johnruf1041.cmake):
#   integration_r39_johnruf1041_a  1021 -> 1041: Rufe leise (Pegel Xa 50/120 im Mitschnitt), msg 40 + main40, Drehung zur
#                                  Treppe, Karte 1141 (FB_SHOT), Bit 97 + MAIN01; -> 1061: MAIN01 laeuft durch
#   integration_r39_johnruf1041_b  Spielstand 1121 in der Panikphase: MAIN01 nach dem Laden; 1131 Ruf lauter (Pegel Xa 100),
#                                  einmal; 1141 unveraendert (MAIN16, Rufe 120); Kampfende -> Bit 97 aus; 1171 MAIN32
if(TARGET re15_pc AND TARGET probe_r36_elza_karte)
    foreach(_teil a b)
        add_test(NAME integration_r39_johnruf1041_${_teil}
            COMMAND ${CMAKE_COMMAND}
                -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                -DRE15_WERKZEUG=$<TARGET_FILE:test_r39_johnruf1041>
                -DRE15_KARTE_ELZA=$<TARGET_FILE:probe_r36_elza_karte>
                -DRE15_SYNCHRO=${CMAKE_SOURCE_DIR}/../synchro
                -DWORKDIR=${CMAKE_BINARY_DIR}/r39_johnruf1041_wd
                -DTEIL=${_teil}
                -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_johnruf1041.cmake)
        set_tests_properties(integration_r39_johnruf1041_${_teil} PROPERTIES TIMEOUT 3600)
    endforeach()
endif()

# INTEGRATION Nachbesserung 1 (Nutzer-Nachtrag 12; tests/integration/test_r39_johnruf1041_tuer.cmake), echte exe, Elza:
#   integration_r39_johnruf1041_tuer_t1041   1041 in der Panikphase: Tuer -> 1031 = Satz msg 41, kein Raumwechsel
#   integration_r39_johnruf1041_tuer_t1041o  1041 ohne Panikphase (nach der Rettung): Tuer -> ROOM1031
#   integration_r39_johnruf1041_tuer_t1051   1051 in der Panikphase: Tuer -> 10A1 = Satz msg 40, kein Raumwechsel
#   integration_r39_johnruf1041_tuer_t1051o  1051 ohne Panikphase: Tuer -> ROOM10A1
#   integration_r39_johnruf1041_tuer_t10b1v  10B1 vor der Sherry-Szene: Kanaldeckel = Text, kein Raumwechsel
#   integration_r39_johnruf1041_tuer_t10b1s  10B1 Sherry-Szene: Deckel-Text statt ROOM1261, danach Steuerung zurueck
#   integration_r39_johnruf1041_tuer_t10b1n  10B1 nach der Szene / Sherry-Suche: Kanaldeckel = Text, kein Raumwechsel
# (der Beginn der Phase IN 1041 am echten Weg 1021 -> 1041 steht in integration_r39_johnruf1041_a)
if(TARGET re15_pc AND TARGET probe_r36_elza_karte)
    foreach(_teil t1041 t1041o t1051 t1051o t10b1v t10b1s t10b1n)
        add_test(NAME integration_r39_johnruf1041_tuer_${_teil}
            COMMAND ${CMAKE_COMMAND}
                -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                -DRE15_KARTE_ELZA=$<TARGET_FILE:probe_r36_elza_karte>
                -DWORKDIR=${CMAKE_BINARY_DIR}/r39_johnruf1041_tuer_wd
                -DTEIL=${_teil}
                -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_johnruf1041_tuer.cmake)
        set_tests_properties(integration_r39_johnruf1041_tuer_${_teil} PROPERTIES TIMEOUT 1800)
    endforeach()
endif()
