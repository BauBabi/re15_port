# Runde 41 (2026-10-24), Spur F "gatorszene" - Riegel. Dossier: analysis/befunde_runde41/F_gatorszene.md,
# Konstanten include/re15_gatorszene_r41.h, Code engine/src/gatorszene_r41.c (+ platform/pc/src/gatorszene_r41_pc.c).
# Haken: scd_vm.c (Spawn-Verwerfen, Ereignis, Takt), scd_room_setup.c (Installer), enemy_ai_boss_gator.c (Boss-Takt),
# platform/pc/main.c (Prop-Lader, Icon/Bild, PC-Init), audio_pc.c (BGM-Weiche 0xFF3C -> RE2 MAIN1A), Item-Namen/-Texte,
# item_discard_common.c (Wegwerf-Stelle 2090 msg 43).
#
#   unit_r41_gatorszene_spinnen     keine Spinne 0x25 in 2090/2091; Gegenprobe ohne Verwerfen: 2
#   unit_r41_gatorszene_programme   Opcode-Laengen; P1 Cut 1 + Knall + Sleep 45; P2 8 Zombies oben (Band 8) + Cut 2; P3 Poll + Bit 110
#   unit_r41_gatorszene_leiche      Leiche/Schluessel an der Szenen-Lage (leiche_lage.py), Kind-Relation + Neigung, Blinken
#   unit_r41_gatorszene_texte       Tor-Texte = ROOM20A0 msg 1/2 bytegleich; Name "Water Key"
#   unit_r41_gatorszene_tor         Nachtrag 21: Tor 2090/20A0 zu bis (9,113), Aufschliessen mit dem Water Key, Wegwerf-Abfrage
#   unit_r41_gatorszene_schluessel  Alligator tot -> Schluessel im Wasser bei der Leiche, Aufnahme -> 0x34 + (9,112)
#   unit_r41_gatorszene_tod         Nachbesserung 1 (M2): die zwei Todeslagen der Abnahme 0 (Todesrolle Cut 0, R1 Cut 1, R1 mit
#                                   Sprung-Cut 0 -> Ersatz-Zonenkamera, Elza): Schluessel im Bild, vor der Leiche, im Wasser,
#                                   zur Kamera gekippt, Blink-Spitze unter der Kappung 0x80
#   unit_r41_gatorszene_szene       die ganze Szene (Leon) mit dem echten Boss-Takt und der RE2-EM23-Bank
#   unit_r41_gatorszene_szene_elza  dieselbe mit Elza (2091/2081)
#   integration_r41_gatorszene      echte exe (test_r41_gatorszene.cmake): Leiter 2080 -> 2090, Szene, Folge-Zeilen (Leon + Elza);
#                                   Nachbesserung 1: Kamera-Uebergabe Cut 0 (M1); _tod/_tod_elza: Todesrolle, Schluessel sichtbar
#                                   und pulsierend (Bildvergleich mit/ohne Schluessel ueber test_r41_gatorszene bilddiff, M2)
add_executable(test_r41_gatorszene ${CMAKE_CURRENT_LIST_DIR}/../test_r41_gatorszene.c)
target_link_libraries(test_r41_gatorszene PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_gatorszene PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_gatorszene PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r41_gatorszene PRIVATE m)
endif()
foreach(_teil spinnen programme leiche texte tor schluessel tod szene szene_elza)
    add_test(NAME unit_r41_gatorszene_${_teil} COMMAND test_r41_gatorszene ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_gatorszene_${_teil} PROPERTIES TIMEOUT 600)
endforeach()

if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_gatorszene.cmake)
    foreach(_teil leon elza tod tod_elza)
        add_test(NAME integration_r41_gatorszene_${_teil}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_TOOL=$<TARGET_FILE:test_r41_gatorszene>
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_gatorszene_${_teil}_wd
                         -DTEIL=${_teil}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_gatorszene.cmake)
        set_tests_properties(integration_r41_gatorszene_${_teil} PROPERTIES TIMEOUT 900)
    endforeach()
endif()
