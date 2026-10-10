# Runde 36 / Spur J "szenenleon" — Dossier analysis/befunde_runde36/J_szenenleon.md, Code engine/src/szenenleon_r36.c
# (+ gen/szl11b0_szene.inc, tools/r36_j/szene11b0_bauen.py), Konstanten include/re15_szenenleon_r36.h.
#
#   test_r36_szenenleon  RIEGEL (Teil ueber das Argument), echte VM + Spielschritt wie platform/pc/main.c:
#     unit_r36_szenenleon_programm     Erweiterung/Tuerprogramm statisch: Laengen -> Evt_end, Set(9,84) zuerst, ein Se_on
#                                      Port-Bank 0x3A Satz 0x0A nach Clip 15 + Sleep 20 (Kendo-Form RE2 ROOM1010 sub09),
#                                      Tuer zu (Zelle (3,0x0C) + Platz 8), Schwanz `47 05 01 00`, msg 22 == 11B1 msg 12
#     unit_r36_szenenleon_schub        Punkt 1: Plc_dest Modus 0x13 (Sub @0x80052864) fuer Leon UND Marvin, Clips
#                                      0x11/0x12/0x11-rueck, Container Wort 0x0201 -> von Leon >= 1300 in +z geschoben,
#                                      Ende 0x0081, Schiebe-Ton snd0 Satz 7
#     unit_r36_szenenleon_erweiterung  Punkt 2: Umleitung @0x017EA nach dem Schieben, msg 20, Marvin an der Tuer, Clip 15 +
#                                      Se_on 0x3A/0x0A, Tuer zu, zurueck, msg 21, dann erst Aot_on 5 (ROOM1260),
#                                      Kameras 6 -> 10 -> 8 -> 10 -> 6
#     unit_r36_szenenleon_tuer         Punkt 2 danach: (9,84) -> Zelle solid, Platz 8 Text 22 "The door is locked.",
#                                      RE2-Ton wie Elzas (0x11B1,12), Leon kommt nicht durch; ohne (9,84) Original;
#                                      ROOM11B1 unberuehrt
#     unit_r36_szenenleon_abgang10f0   Punkt 3: ROOM10F0 nach Zeile 23 kein Cut_chg, Cut 2 bis zum Ende, Marvin/Ada erst
#                                      ausserhalb des Bildes geparkt, ein Tuerton danach
#     unit_r36_szenenleon_irons        Punkt 4: Irons tot -> +0x1b8 = 0x12, Kopf-Akku 0/0 an zwei Standpunkten;
#                                      Gegenprobe lebend: Kopf folgt Leon
add_executable(test_r36_szenenleon ${CMAKE_CURRENT_LIST_DIR}/../test_r36_szenenleon.c)
target_link_libraries(test_r36_szenenleon PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_szenenleon PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_szenenleon PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r36_szenenleon PRIVATE m)
endif()
foreach(_teil programm schub erweiterung tuer abgang10f0 irons)
    add_test(NAME unit_r36_szenenleon_${_teil} COMMAND test_r36_szenenleon ${_teil})
    set_tests_properties(unit_r36_szenenleon_${_teil} PROPERTIES TIMEOUT 240)
endforeach()

#   integration_r36_szenenleon  echte exe: A Szene ROOM11B0 + Erweiterung (Umleitung, msg 20, Se_on 0x3A ->
#                               RE2-Schlossbank SCHLOSS1010 ueber audio_pc.c, msg 21, ROOM1260); B Werkstatt-Tuer mit
#                               (9,84): Ereignis 23 + Tuer-zu-Ton wie Elzas (11B1 msg 12) (test_r36_szenenleon.cmake)
if(TARGET re15_pc)
    add_test(NAME integration_r36_szenenleon
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r36_szenenleon_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r36_szenenleon.cmake)
    set_tests_properties(integration_r36_szenenleon PROPERTIES TIMEOUT 900)
endif()
