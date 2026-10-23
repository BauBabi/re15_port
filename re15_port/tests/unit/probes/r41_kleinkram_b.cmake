# Runde 41 Spur E "kleinkram_b" — Dossier: analysis/befunde_runde41/E_kleinkram_b.md
#   unit_r41_kleinkram_b_11b1_{programm,zu,original}
#       Punkt 3: Elzas Werkstatt-Tuer ROOM11B1 bleibt dauerhaft zu (engine/src/werkstatt11b1_r41.c): Programm = belegte
#       Form (Umkehr von sub00 @0x0110E..@0x0111A, Text-Platz @0x0112A Byte fuer Byte), im Init-Lauf hinter sub00 bei
#       (3,0x83)=1: Zelle (3,0x0C) u0=ff fl=03, Platz 8 msg 12, RVD-Zonen ohne Cut 7/8, Eingangs-Cut 0, Schacht bleibt
#       Tuer; Lauf nach Westen blockiert, Aktionstaste -> msg 12 ohne Raumwechsel; ohne (3,0x83) und bei Leon (11B0)
#       unveraendert.
add_executable(test_r41_kleinkram_b_11b1 ${CMAKE_CURRENT_LIST_DIR}/../test_r41_kleinkram_b_11b1.c)
target_link_libraries(test_r41_kleinkram_b_11b1 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_kleinkram_b_11b1 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_kleinkram_b_11b1 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_kleinkram_b_11b1 PRIVATE m)
endif()
foreach(_teil programm zu original)
    add_test(NAME unit_r41_kleinkram_b_11b1_${_teil} COMMAND test_r41_kleinkram_b_11b1 ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_kleinkram_b_11b1_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
#   probe_r41_kleinkram_b_karte mess|pin
#       Punkt 2: Karte B1 — Marker vor der Ostkammer-Tuer von ROOM1190 ("vordere Tuer der Waffen Armory") bleibt auf der
#       Ostkammer-Marke (146,109) und wird nicht zur Hallen-Marke (127,115) gezogen (tuer_anziehen: Marke ueber die
#       SPAWN-Zone der Tuer statt "irgendeine Zone des Zielraums"); Hallen-Tuer, Ausgaenge in 1190, ROOM1180 ohne Strom.
#       Im Gegensatz zu probe_r39_karten wird die VM im Init-Lauf getickt (Tuer-AOTs vorhanden, Zug wirksam).
add_executable(probe_r41_kleinkram_b_karte ${CMAKE_CURRENT_LIST_DIR}/../test_r41_kleinkram_b_karte.c)
target_link_libraries(probe_r41_kleinkram_b_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r41_kleinkram_b_karte PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/..)
target_compile_definitions(probe_r41_kleinkram_b_karte PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(probe_r41_kleinkram_b_karte PRIVATE m)
endif()
add_test(NAME unit_r41_kleinkram_b_karte_pin COMMAND probe_r41_kleinkram_b_karte pin WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_kleinkram_b_karte_pin PROPERTIES TIMEOUT 300)
#   unit_r41_kleinkram_b_kurbel_{laenge,schleife}
#       Punkt 1: Kurbel ROOM2020/2021 — Spieler-Clip im Kommandowort 4 (FUN_80030660 @0x800306a4 -> 0x80050cb8) laeuft
#       mit Plc_flg(0,4) als Schleife (Umlauf 39 -> 0) statt 138 Bilder Bild 39 zu halten; ohne Flag Halten; Laenge aus
#       dem Raum-RBJ-Record (Marker-Bit 0) bzw. PL00 (engine/src/plcloop_r41.c).
add_executable(test_r41_kleinkram_b_kurbel ${CMAKE_CURRENT_LIST_DIR}/../test_r41_kleinkram_b_kurbel.c)
target_link_libraries(test_r41_kleinkram_b_kurbel PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_kleinkram_b_kurbel PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_kleinkram_b_kurbel PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_kleinkram_b_kurbel PRIVATE m)
endif()
foreach(_teil laenge schleife)
    add_test(NAME unit_r41_kleinkram_b_kurbel_${_teil} COMMAND test_r41_kleinkram_b_kurbel ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_kleinkram_b_kurbel_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
