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
