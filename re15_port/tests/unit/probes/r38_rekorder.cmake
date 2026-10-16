# Runde 38 Spur J "rekorder" (analysis/befunde_runde38/J_rekorder.md). GEPINNT -> add_test je Teil von probe_r38_rekorder:
#   anker   Original-Bytes ROOM1100/1101 sub02 (@0x00C90..0x00CC7, @0x00CC8 Tuer), sub01 @0x00C2C (Mikrofon), sub08
#           (SYSTEM READY / SAY HELLO); RE2 ROOM2110 sub08 (@0x01B5C Piep + @0x01B6C Anzeige im selben Takt, @0x01B8C
#           Sleep 15, @0x01B90 Aufschliessen); Runde-36-Stellen 1100/2, 1101/2 = KARTE_AUF; Port-Konstanten == Bytes
#   datei   synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav (PCM16 mono 32000 Hz, 79,4 Bilder) und die Bindung Id 40
#   ablauf  echter Skriptweg headless mit nachgebildetem Stimmen-Kanal: Aufnahme im Takt von Message_on 4, Piep erst nach
#           ihrem Ende im Takt des ersten ACCEPTED, Aufschliessen +15, msg 2 ohne zweite Folge, Tuer frei; Leon langsam/
#           schnell, Elza ROOM1101, Gegenprobe ohne Haken (= vorher), ohne Ton (kein Haenger)
# Eigene Datei (nicht tests/unit/CMakeLists.txt), s. probes/README.md.
add_executable(probe_r38_rekorder ${CMAKE_CURRENT_SOURCE_DIR}/probe_r38_rekorder.c)
target_link_libraries(probe_r38_rekorder PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r38_rekorder PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(probe_r38_rekorder PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_RE2_RDT_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon/PL0/RDT"
    RE15_REPO_DIR="${CMAKE_SOURCE_DIR}/..")
if(NOT WIN32)
    target_link_libraries(probe_r38_rekorder PRIVATE m)
endif()
add_test(NAME unit_r38_rekorder_anker  COMMAND probe_r38_rekorder anker)
add_test(NAME unit_r38_rekorder_datei  COMMAND probe_r38_rekorder datei)
add_test(NAME unit_r38_rekorder_ablauf COMMAND probe_r38_rekorder ablauf)
set_tests_properties(unit_r38_rekorder_anker unit_r38_rekorder_datei unit_r38_rekorder_ablauf PROPERTIES TIMEOUT 300)

# integration_r38_rekorder: echte exe (Ton-Mitschnitt, Framedumps, Logs) - tests/integration/test_r38_rekorder.cmake
#   L Leon ROOM1100 echter Weg (Panel, Ja, ON/OFF, Mikrofon) langsam, S Leon schnell (sub02 wartet), E Elza ROOM1101,
#   T Tuer nach dem Oeffnen benutzbar (ROOM1110).
if(TARGET re15_pc)
    add_test(NAME integration_r38_rekorder
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PROBE=$<TARGET_FILE:probe_r38_rekorder>
                     -DRE15_WAV=${CMAKE_SOURCE_DIR}/../synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav
                     -DRE15_BSS=${CMAKE_SOURCE_DIR}/shared_assets/PSX/STAGE1/ROOM110.BSS
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r38_rekorder_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_rekorder.cmake)
    set_tests_properties(integration_r38_rekorder PROPERTIES TIMEOUT 1200)
endif()
