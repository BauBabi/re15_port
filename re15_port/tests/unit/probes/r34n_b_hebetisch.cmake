# Runde 34 Nacht / Spur B — Hebetisch ROOM1150/1151: Cursor-Bedienung statt Direktoeffnung.
# Dossier: analysis/befunde_runde34_nacht/B_hebetisch.md, Konstanten: include/re15_hebetisch_cursor.h
#
#   probe_r34n_b_messung   MESS-SONDE (kein add_test): Halte-Stelle For 15 @0x0FC0/@0x0F9E in sub04,
#                          Cut_old-Ziel mit/ohne vorgeschaltetes Cut 4, Kuppel-Huelle unter Cut 4 mit
#                          Engine-Projektion, 11F0-Cursor unter Cut 10 von ROOM11F0.
#   probe_r34n_b_cursor    RIEGEL (je ROOM1150 und ROOM1151, Teil-Riegel ueber das Argument):
#     unit_r34n_b_halt          R1 Spielerweg haelt sub04 vor dem For, Cursorzustand, Start (160,119)
#     unit_r34n_b_kuppel        R2 Huelle im Modul = Engine-Projektion der Kuppel, Treffertest
#     unit_r34n_b_ablauf        R3 Fehldruck-Text, D-Pad, Kuppeldruck -> Klick + Fahrt, Items wie bisher
#     unit_r34n_b_cut_old       R4 nach dem Durchlauf mit Cursor zurueck in die Raumkamera
#     unit_r34n_b_harness       R5 scd_event_fire(4) ohne Aktion (RE15_FIRE_AOT) -> kein Halt
#     unit_r34n_b_cursor_bytes  R6 eingebackene Cursor-Bytes == ROOM11F0.RDT
#     unit_r34n_b_abbruch       R7 CROSS -> Original-Aufraeumbytes von sub04, erneut aktivierbar
#     unit_r34n_b_kreuztext     R8 Text mit CROSS schliessen bricht nicht ab
#   integration_r34n_b_cursor   echte exe: Lade-Weg 1150/1151, Nutzerweg bis zum Ende, Abbruch,
#                               Inventar im Cursor, Harness-Gegenprobe (test_r34n_b_cursor.cmake)
add_executable(probe_r34n_b_messung ${CMAKE_CURRENT_LIST_DIR}/../probe_r34n_b_messung.c)
target_link_libraries(probe_r34n_b_messung PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_b_messung PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r34n_b_messung PRIVATE m)
endif()

add_executable(probe_r34n_b_cursor ${CMAKE_CURRENT_LIST_DIR}/../probe_r34n_b_cursor.c)
target_link_libraries(probe_r34n_b_cursor PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_b_cursor PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r34n_b_cursor PRIVATE m)
endif()
foreach(_r34n_b_teil halt kuppel ablauf cut_old harness abbruch kreuztext)
    add_test(NAME unit_r34n_b_${_r34n_b_teil} COMMAND probe_r34n_b_cursor ${_r34n_b_teil})
    set_tests_properties(unit_r34n_b_${_r34n_b_teil} PROPERTIES TIMEOUT 120)
endforeach()
add_test(NAME unit_r34n_b_cursor_bytes COMMAND probe_r34n_b_cursor bytes)
set_tests_properties(unit_r34n_b_cursor_bytes PROPERTIES TIMEOUT 60)

if(TARGET re15_pc AND TARGET probe_r30_granate_karte)
    add_test(NAME integration_r34n_b_cursor
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r30_granate_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r34n_b_cursor_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r34n_b_cursor.cmake)
    set_tests_properties(integration_r34n_b_cursor PROPERTIES TIMEOUT 900)
endif()
