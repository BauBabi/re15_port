# Runde 38 / Spur C "werkstatt11b0" — Dossier analysis/befunde_runde38/C_werkstatt11b0.md, Code
# engine/src/werkstatt11b0_r38.c + platform/pc/src/werkstatt11b0_pc.c (Konstanten include/re15_werkstatt11b0_r38.h),
# Text: tools/r36_j/szene11b0_bauen.py -> engine/src/gen/szl11b0_szene.inc.
#
#   test_r38_werkstatt11b0  RIEGEL (Teil ueber das Argument), echte VM + Spielschritt wie platform/pc/main.c:
#     unit_r38_werkstatt11b0_text       Punkt 1: msg 20 = "Marvin: But, hold on..." in der Form der Original-Zeilen
#                                       (Kopf msg 12 @0x1BD0, Komma/Auslassung `18 00`/`57 57 57` wie msg 4 @0x19CD),
#                                       Programm unveraendert, im Spiel dekodiert
#     unit_r38_werkstatt11b0_takt       Punkt 2: 11B0 Satz 7 @0xE4A0 leer, Quelle ROOM1171 Satz 7 @0x6EF8 == ROOM1090
#                                       (Ton 4 + VAG 4 bytegleich), Zustaendigkeit nur 11B0/Satz 7, Szene: 8 Anforderungen
#                                       im Original-Takt (Clip 0x12 Bild 1, 25 Bilder Abstand, Phase 7)
#     unit_r38_werkstatt11b0_uebergabe  Punkt 3: Marvins Bank je Bild wie main.c: Phase 1->2 Bank 1 Clip 5, Phase 7
#                                       Loco Clip 0x11 Bild 0 (= Keyframe von Phase 6), danach Bank 1 Clip 1, nie Rueckfall
#                                       auf die Containerbank; Gegenprobe alte Regel = tiefe Schiebehaltung
add_executable(test_r38_werkstatt11b0 ${CMAKE_CURRENT_LIST_DIR}/../test_r38_werkstatt11b0.c)
target_link_libraries(test_r38_werkstatt11b0 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_werkstatt11b0 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_werkstatt11b0 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r38_werkstatt11b0 PRIVATE m)
endif()
foreach(_teil text takt uebergabe)
    add_test(NAME unit_r38_werkstatt11b0_${_teil} COMMAND test_r38_werkstatt11b0 ${_teil})
    set_tests_properties(unit_r38_werkstatt11b0_${_teil} PROPERTIES TIMEOUT 240)
endforeach()

#   unit_r38_werkstatt11b0_ton  Punkt 2 mit dem ECHTEN audio_pc.c + werkstatt11b0_pc.c (RE15_AUDIO_CAP_SYNC, kein
#                               SDL-Geraet, Muster probe_r34_plattform_ton): 11B0 Satz 7 ohne Ergaenzung stumm, mit
#                               Ergaenzung Ton; ROOM1090 eigener Satz 7 Ton; beide Fenster Sample fuer Sample gleich;
#                               11B1 und 11B0 Satz 0 ohne Ergaenzung
if(TARGET SDL2::SDL2-static)
    add_executable(probe_r38_werkstatt11b0_ton
        ${CMAKE_CURRENT_LIST_DIR}/../probe_r38_werkstatt11b0_ton.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/audio_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/asset_root_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/werkstatt11b0_pc.c)
    target_link_libraries(probe_r38_werkstatt11b0_ton PRIVATE re15_engine SDL2::SDL2-static)
    target_include_directories(probe_r38_werkstatt11b0_ton PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/platform/pc/src)
    target_compile_definitions(probe_r38_werkstatt11b0_ton PRIVATE
        RE15_PLATFORM_PC
        RE15_ASSET_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_CD_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
    if(NOT WIN32)
        target_link_libraries(probe_r38_werkstatt11b0_ton PRIVATE m)
    endif()
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r38_werkstatt11b0_ton_wd)
    add_test(NAME unit_r38_werkstatt11b0_ton COMMAND probe_r38_werkstatt11b0_ton)
    set_tests_properties(unit_r38_werkstatt11b0_ton PROPERTIES TIMEOUT 120
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r38_werkstatt11b0_ton_wd)
endif()

#   integration_r38_werkstatt11b0  echte exe (tests/integration/test_r38_werkstatt11b0.cmake): A Text msg 20 im Spiel,
#                               8 gekeyte Schiebe-Toene aus der ROOM1171-Bank im Original-Takt, Uebergabebild Phase 7
#                               (anim.txt: Loco Clip 0x11 Bild 0, danach Bank 1 Clip 1, nie Rueckfall); B Mess-Haken
#                               RE15_W11B0_SCHUB_STUMM: Ton-Mitschnitt A/B bis zum ersten Ton gleich, danach verschieden
if(TARGET re15_pc)
    add_test(NAME integration_r38_werkstatt11b0
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r38_werkstatt11b0_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_werkstatt11b0.cmake)
    set_tests_properties(integration_r38_werkstatt11b0 PROPERTIES TIMEOUT 900)
endif()
