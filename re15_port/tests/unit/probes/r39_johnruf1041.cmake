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
add_executable(test_r39_johnruf1041 ${CMAKE_CURRENT_LIST_DIR}/../test_r39_johnruf1041.c)
target_link_libraries(test_r39_johnruf1041 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_johnruf1041 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_johnruf1041 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_johnruf1041 PRIVATE m)
endif()
foreach(_teil texte programme stimme xa ablauf musik ruf1131 leon)
    add_test(NAME unit_r39_johnruf1041_${_teil} COMMAND test_r39_johnruf1041 ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r39_johnruf1041_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
