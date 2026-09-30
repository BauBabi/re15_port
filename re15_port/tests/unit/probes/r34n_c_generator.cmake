# Runde 34 Nacht, Spur C — Generator ROOM11F0/11F1 (analysis/befunde_runde34_nacht/C_generator.md).
# Nutzer 2026-09-30: nach einem Schalter frei bewegen, "Ausser ganz am Ende" (Endsperre, bis der
# Zeiger auf 80 steht + 30 Ruhebilder, RE2 ROOM2130 @0x0171C); zwei gruene Lampen (oben = linke
# Spalte 1,0,1,0,1 @0x012BE..0x012CE, unten = rechte Spalte 0,1,0,1,0 @0x012D2..0x012E2).
# Der PC-Zeichner platform/pc/src/panel_lampen_pc.c wird ECHT einkompiliert (Stubs fuer
# Framebuffer und RE2-Leser in der Sonde), damit Teil I das gezeichnete Ergebnis prueft.
# EIGENE Datei (nicht die gemeinsame CMakeLists.txt), s. probes/README.md.
add_executable(probe_r34n_c_generator
    ${CMAKE_CURRENT_SOURCE_DIR}/probe_r34n_c_generator.c
    ${CMAKE_SOURCE_DIR}/platform/pc/src/panel_lampen_pc.c)
target_link_libraries(probe_r34n_c_generator PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_c_generator PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src
    ${CMAKE_SOURCE_DIR}/platform/pc/src)
target_compile_definitions(probe_r34n_c_generator PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2"
    RE15_RE2_RDT_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon/PL0/RDT")
add_test(NAME unit_r34n_c_generator COMMAND probe_r34n_c_generator)
set_tests_properties(unit_r34n_c_generator PROPERTIES TIMEOUT 240)
