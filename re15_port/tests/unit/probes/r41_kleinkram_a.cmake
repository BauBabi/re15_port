# Runde 41 (2026-10-23), Spur D "kleinkram_a" — Riegel, zaehlen zur Suite.
# Dossier: analysis/befunde_runde41/D_kleinkram_a.md. Code: engine/src/lautstaerke_r41.c + platform/pc/src/
# lautstaerke_r41_pc.c (Lautstaerke-Option), engine/src/blink_r41.c (Helligkeitspuls aufnehmbarer Port-
# Gegenstaende, API fuer Spur F), engine/src/giftkugel_r41.c (RE2-Giftkugel 20 HP, Danger + Treffer = Tod).
# Haken: audio_pc.c (Endstufe), main.c (SOUND-Schirm, Start, Prop-Lichtkontext), spinnen_r38.c (Routine 25),
# game_step_common.c (Messschiene RE15_R41_HP_AT), feuerloescher_r37.c (Puls-Registrierung).
#
#   unit_r41_kleinkram_a                 A Lautstaerke (Parser, Q15, Klemme, Arbeitskopie @0x8002f244-4c/@0x8002f2c0/
#                                        @0x8002f27c, Datei-Rundreise), B Blink (wert(n) == Titelpuls FUN_801028ec,
#                                        Tabelle, Lichtkontext), C Giftkugel (20 HP @0x800200dc, Danger -> Zustand 3,
#                                        Tore, RE1.5-Gehirn unberuehrt)
#   integration_r41_kleinkram_a_lautstaerke  echte exe: zwei CAP_SYNC-Mitschnitte (re15_options.cfg 100 % / 50 %,
#                                        Schuss + Giftkugel in ROOM2030) -> RMS halbiert; Options-Schirm-Shot mit
#                                        VOLUME-Zeile (dritte Kachel y 89 = Pixel der ersten Kachel, Ziffern rechts)
#   integration_r41_kleinkram_a_blink    echte exe: ROOM1030 Cut 0, Bildserie ueber eine Pulsperiode -> Helligkeits-
#                                        hub am Loescher, Pfeiler daneben konstant
#   integration_r41_kleinkram_a_gift     echte exe: Leon ROOM2030 (RE2-Gehirn), HP 15 (Danger) -> erster Giftklumpen
#                                        = Tod (Zustand 3, HP < 0), Todesfolge, Raum-Neuladen; Elza ROOM2031 ebenso
add_executable(test_r41_kleinkram_a ${CMAKE_CURRENT_LIST_DIR}/../test_r41_kleinkram_a.c)
target_link_libraries(test_r41_kleinkram_a PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_kleinkram_a PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(test_r41_kleinkram_a PRIVATE m)
endif()
add_test(NAME unit_r41_kleinkram_a COMMAND test_r41_kleinkram_a WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_kleinkram_a PROPERTIES TIMEOUT 300)

if(TARGET re15_pc)
    add_test(NAME integration_r41_kleinkram_a_lautstaerke
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_TOOL=$<TARGET_FILE:test_r41_kleinkram_a>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_kleinkram_a_laut_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_kleinkram_a_lautstaerke.cmake)
    set_tests_properties(integration_r41_kleinkram_a_lautstaerke PROPERTIES TIMEOUT 1800)
    add_test(NAME integration_r41_kleinkram_a_blink
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_TOOL=$<TARGET_FILE:test_r41_kleinkram_a>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_kleinkram_a_blink_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_kleinkram_a_blink.cmake)
    set_tests_properties(integration_r41_kleinkram_a_blink PROPERTIES TIMEOUT 1200)
    add_test(NAME integration_r41_kleinkram_a_gift
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_kleinkram_a_gift_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_kleinkram_a_gift.cmake)
    set_tests_properties(integration_r41_kleinkram_a_gift PROPERTIES TIMEOUT 1800)
endif()
