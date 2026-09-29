# Runde 34 (Granaten), Spur C (Plattform) — analysis/befunde_runde34_granaten/bau_c.md
#
#   unit_r34_plattform   was ohne Fenster pruefbar ist (platform/pc/src/fx_plattform_pc.c, OHNE SDL):
#                        C1 Takt (ESP-Tick hinter dem Spielschritt, Freigabe/Pause), C4 Haken-Bindung +
#                        Ton-Weiche (FUN_80045024 / RE2 0x0113/0x0112 -> ARMS10/11 Satz 10, EDH-Bytes),
#                        C2 TEX.TIM-Effektseiten (Paletten 481/483/492, Seitenpixel == gepinnte
#                        file-route-Blaetter) und Zeichen-Helfer (Sichtbarkeit, wpos, CLUT/TPAGE, defW/H),
#                        C3 Licht-Latch gegen K2 LICHT_LESER, C8 Harness-Parse, C7 RE2-Part-Tinte.
#                        Rueckgabe 0 = gruen, sonst Nummer der ersten verletzten Pruefung.
add_executable(probe_r34_plattform
    ${CMAKE_CURRENT_LIST_DIR}/../probe_r34_plattform.c
    ${CMAKE_SOURCE_DIR}/platform/pc/src/fx_plattform_pc.c)
target_link_libraries(probe_r34_plattform PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34_plattform PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/platform/pc/src)
target_compile_definitions(probe_r34_plattform PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
# libm (Runde-30-Befund probes/r30_irons-diary-welt.cmake): unter Linux nicht implizit
if(NOT WIN32)
    target_link_libraries(probe_r34_plattform PRIVATE m)
endif()
add_test(NAME unit_r34_plattform COMMAND probe_r34_plattform)
set_tests_properties(unit_r34_plattform PROPERTIES TIMEOUT 120)

#   unit_r34_plattform_ton   dieselbe Ton-Weiche mit dem ECHTEN audio_pc.c (RE15_AUDIO_CAP_SYNC, kein
#                        SDL-Geraet, Muster test_r31_tuer_ton): ARMS10/ARMS11 Satz 10 (Zusatzbaenke, E9)
#                        und die ESP-Codes 0x010A0601 (ARMS09 Satz 0x0A) / 0x04080001 (CORE Satz 8)
#                        KLINGEN (PCM-Energie), leere Saetze / Bank 0 bleiben stumm.
if(TARGET SDL2::SDL2-static)
    add_executable(probe_r34_plattform_ton
        ${CMAKE_CURRENT_LIST_DIR}/../probe_r34_plattform_ton.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/fx_plattform_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/audio_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/asset_root_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/skeleton_trig_pc.c)
    target_link_libraries(probe_r34_plattform_ton PRIVATE re15_engine SDL2::SDL2-static)
    target_include_directories(probe_r34_plattform_ton PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/platform/pc/src)
    target_compile_definitions(probe_r34_plattform_ton PRIVATE
        RE15_PLATFORM_PC
        RE15_ASSET_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_CD_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
    if(NOT WIN32)
        target_link_libraries(probe_r34_plattform_ton PRIVATE m)
    endif()
    add_test(NAME unit_r34_plattform_ton COMMAND probe_r34_plattform_ton)
    set_tests_properties(unit_r34_plattform_ton PROPERTIES TIMEOUT 120
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()
