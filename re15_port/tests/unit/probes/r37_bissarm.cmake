# Spur J (Runde 37) — "bissarm": beim Zombie-Biss fehlt teilweise der dranhaengende Arm.
# Dossier analysis/befunde_runde37/J_bissarm.md; Code include/re15_bissarm_r37.h + engine/src/bissarm_r37.c.
#
# Riegel:
#   unit_r37_bissarm_re2_vorn / _hinten   (A) Handkontakt: die GEZEICHNETE Hand des Hybrid-Greifers liegt relativ zu
#                                         Leons Schultergelenk dort, wo sie im RE2-ORIGINAL liegt (RAM-Fixture
#                                         J_belege/re2_*_frames.bin, SLUS-00748_4.sav, Biss-Loop P3 @0x8010F838 =
#                                         EMOVL10_S0 +0x2838). Ohne Handkontakt Mittel >= 150 (gemessen 222/223),
#                                         mit <= 12 (gemessen vorn 9/3, hinten 8/2), ab Biss-Bild 4 jedes Bild <= 6.
#   unit_r37_bissarm_klemme_*             (B) Wandklemme im Griff = EIN Durchgang je Bild vom Stand des Vorbilds
#                                         (RE2 FUN_8003567c `jal 0x8004c1bc` @0x800356d0, alter Standpunkt
#                                         `sh` @0x800356f4-f8): ROOM1010 an der Schreibtisch-Wand (vorher 16 bzw.
#                                         80 Fehlbilder, Leon auf (0,0)/festgehalten), ROOM1140 freie Flaeche.
#   integration_r37_bissarm_echtlauf      echte exe: ROOM1140 Griff von hinten (ein Greifer per Kill-Flags) und
#                                         ROOM1010 Cut 6 — Haende auf dem RE2-Ziel (RE15_BISSARM_LOG), Leon am
#                                         Greifer (RE15_STATE_LOG).
# Die Fixtures liegen unter analysis/ (Muster r22_10f0 / r30_*); fehlen sie, meldet die Sonde SKIP (77).
add_executable(test_r37_bissarm ${CMAKE_CURRENT_SOURCE_DIR}/test_r37_bissarm.c)
target_link_libraries(test_r37_bissarm PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_bissarm PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r37_bissarm PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

foreach(_s vorn hinten)
    add_test(NAME unit_r37_bissarm_re2_${_s}
             COMMAND test_r37_bissarm pruef_re2
                     ${CMAKE_SOURCE_DIR}/../analysis/befunde_runde37/J_belege/re2_${_s}_frames.bin ${_s})
    set_tests_properties(unit_r37_bissarm_re2_${_s} PROPERTIES TIMEOUT 300 SKIP_RETURN_CODE 77)
endforeach()

# pruef_klemme <raum> <px> <pz> <rot> <vorn|hinten>: ROOM1010 = Stand des Nutzerwegs aus Abschnitt 1.2 (F57
# PL(3392,3279)), Blick 1024 bzw. 0; ROOM1140 = freie Flaeche (-1000,-19600), dort darf sich nichts aendern.
# ROOM1220 (duenne Wand) ist NICHT dabei: dort schiebt schon die erste Platzierung Leon durch die Wand, vor wie
# nach dem Fix (Abschnitt OFFEN).
foreach(_k "1010a;1010;3392;3279;1024;vorn" "1010b;1010;3392;3279;0;vorn"
           "1140_vorn;1140;-1000;-19600;0;vorn" "1140_hinten;1140;-1000;-19600;0;hinten")
    list(GET _k 0 _n)
    list(SUBLIST _k 1 5 _a)
    add_test(NAME unit_r37_bissarm_klemme_${_n} COMMAND test_r37_bissarm pruef_klemme ${_a})
    set_tests_properties(unit_r37_bissarm_klemme_${_n} PROPERTIES TIMEOUT 300 SKIP_RETURN_CODE 77)
endforeach()

if(TARGET re15_pc)
    add_test(NAME integration_r37_bissarm_echtlauf
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_TOOL=$<TARGET_FILE:test_r37_bissarm>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r37_bissarm_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_bissarm_echtlauf.cmake)
    set_tests_properties(integration_r37_bissarm_echtlauf PROPERTIES TIMEOUT 900)
endif()
