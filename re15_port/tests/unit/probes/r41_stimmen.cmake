# Runde 41 Stimmen (2026-11-17) — neue Port-Option "VOICE  ON|OFF" im OPTIONS-Schirm (Standard ON).
# Belege und Zensus der Stimmen-Quellen: include/re15_stimme_option.h, Bericht scratchpad stimmen_option/BERICHT_r1.md.
# Code: engine/src/stimme_option.c; Haken: lautstaerke_r41.c (voice=off in re15_options.cfg), audio_pc.c (Tor in
# re15_voice_play, Stopp-Hook re15_audio_stimme_stoppen, Messhaken RE15_VOICE_AUS_NACH), main.c (Zeile + Cursor 5),
# scd_vm.c (stimme.log + bild=).
#
#   unit_stimmen_r41                      A Schalter/Parser/Messhaken, B Options-Datei (ON = bisherige Datei bitgleich,
#                                         OFF = + "voice=off"), C Stopp-Hook genau beim Wechsel ON -> OFF, D was stumm
#                                         wird (synchro-Dialog) und was bleibt (Tuerton 1141)
#   integration_stimmen_r41_menue         echte exe: VOICE-Zeile im TOP-Schirm (Abzug + Pruefzeile), Wert aus
#                                         re15_options.cfg, echte Tastenkette TOP -> RIGHT x5 -> Kreis schreibt voice=off,
#                                         zweiter Kreis schreibt die Datei bitgleich zurueck
#   integration_stimmen_r41_szene         echte exe, Vorspann 1240 + Helipad 1170 mit Aufnahmen, deterministischer
#                                         Mitschnitt: OFF laedt/spielt keine Aufnahme, jede Zeile oeffnet im selben Bild
#                                         wie bei ON; Umschalten waehrend main00: Stimme endet im selben Bild, Untertitel
#                                         bleibt offen, danach ist der Ton bitgleich mit OFF; Irons' Tod ROOM1150 (Aufnahmen
#                                         laenger als ihre Bilder): ON wartet, OFF oeffnet jede Zeile um genau die
#                                         Wartebilder frueher = Textdauer
add_executable(test_r41_stimmen ${CMAKE_CURRENT_LIST_DIR}/../test_r41_stimmen.c)
target_link_libraries(test_r41_stimmen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_stimmen PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(test_r41_stimmen PRIVATE m)
endif()
add_test(NAME unit_stimmen_r41 COMMAND test_r41_stimmen WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_stimmen_r41 PROPERTIES TIMEOUT 60)

if(TARGET re15_pc AND TARGET probe_r35_cut1150_karte)
    add_test(NAME integration_stimmen_r41_menue
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_TOOL=$<TARGET_FILE:test_r41_optionen>
                     -DRE15_TOOL2=$<TARGET_FILE:test_r41_stimmen>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_stimmen_menue_wd
                     -DTEIL=menue
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_stimmen.cmake)
    set_tests_properties(integration_stimmen_r41_menue PROPERTIES TIMEOUT 900)
    add_test(NAME integration_stimmen_r41_szene
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_TOOL=$<TARGET_FILE:test_r41_optionen>
                     -DRE15_TOOL2=$<TARGET_FILE:test_r41_stimmen>
                     -DRE15_KARTE_1150=$<TARGET_FILE:probe_r35_cut1150_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_stimmen_szene_wd
                     -DTEIL=szene
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_stimmen.cmake)
    set_tests_properties(integration_stimmen_r41_szene PROPERTIES TIMEOUT 1500)
endif()
