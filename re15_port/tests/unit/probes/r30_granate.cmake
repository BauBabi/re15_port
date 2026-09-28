# Runde 30, Nachtrag K — Handgranate (Item 0x09) im Hebetisch von Irons' Buero (ROOM1150/1151).
#
# Dossier: analysis/befunde_runde30/nachtrag-granate.md, Konstanten: include/re15_granate.h
#
#   unit_r30_granate          Modell = die 19 Waffen-Flaechen aus PLD/PL00W09.PLW (Punkte,
#                             Normalen, UV, TIM Byte fuer Byte); Sitz gegen Fachboden, Sicherung
#                             und GESCHLOSSENE Kuppel aus ROOM1150.RDT UND ROOM1151.RDT; Fahrten:
#                             erst Sicherung, dann Granate, jede mit eigenem Flag, "No" bei einer
#                             laesst die andere unberuehrt, je Fahrt hoechstens ein Modal je
#                             Gegenstand; Negativ-Kontrolle Flag (9,56) vor dem Raumstart
#   integration_r30_granate_laden   echte exe: Lade-Weg 1150/1151, Negativ-Kontrolle,
#                             Reihenfolge im Spiel, CHECK-Foto von Item 0x09
add_executable(probe_r30_granate ${CMAKE_CURRENT_LIST_DIR}/../probe_r30_granate.c)
target_link_libraries(probe_r30_granate PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_granate PRIVATE ${CMAKE_SOURCE_DIR}/include)
# hypotf/lroundf aus <math.h>: unter Linux liegt libm nicht implizit dabei (Runde-30-Befund
# probes/r30_irons-diary-welt.cmake)
if(NOT WIN32)
    target_link_libraries(probe_r30_granate PRIVATE m)
endif()
add_test(NAME unit_r30_granate COMMAND probe_r30_granate)
set_tests_properties(unit_r30_granate PROPERTIES TIMEOUT 120)

# MESS-WERKZEUG (kein add_test): schreibt die Speicherkarte fuer den Lade-Riegel.
add_executable(probe_r30_granate_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r30_granate_karte.c)
target_link_libraries(probe_r30_granate_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_granate_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(TARGET re15_pc)
    add_test(NAME integration_r30_granate_laden
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r30_granate_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r30_granate_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r30_granate_laden.cmake)
    set_tests_properties(integration_r30_granate_laden PROPERTIES TIMEOUT 600)
endif()
