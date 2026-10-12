# Runde 37 Spur E "codepanel" (analysis/befunde_runde37/E_codepanel.md). GEPINNT -> add_test je Teil von probe_r37_codepanel:
#   anker     Original-Bytes (sub19-Schwanz/sub17-Cut/sub01-Flag in 10D0/10D1/11E0/11E1/1230/1231, RE1.5 ROOM1100 sub02,
#             RE2 ROOM2110/4070 sub08), Kunst LAMPE2130.TIM == RE2 room2110/esp16.tim, Port-Tabelle
#   kamera    E2: echter Skriptweg (Cursor + Quadrat, Codes 4312/0513/5632), Piep + Aufschliessen bei Panel-Cut, Kamera
#             erst danach zurueck; Gegenprobe ohne Haken; Nachricht frueh zu -> sub19 wartet
#   lampe     E1 (Nachtrag 3/3b): die rot gemalte Lampe leuchtet rot bis zum Piep-Bild, ab dem naechsten die andere
#             gemalte Lampe in ihrer Farbe (10D0/10D1 blau, SECUGUARD gruen), Lage/Kante/Zellfolge, falscher Code
#             bleibt rot
#   zeichner  E1 im Bild: echter PC-Zeichner (codepanel_r37_pc.c einkompiliert) auf dem echten MDEC-Hintergrund
# Eigene Datei (nicht tests/unit/CMakeLists.txt), s. probes/README.md.
add_executable(probe_r37_codepanel
    ${CMAKE_CURRENT_SOURCE_DIR}/probe_r37_codepanel.c
    ${CMAKE_SOURCE_DIR}/platform/pc/src/codepanel_r37_pc.c)
target_link_libraries(probe_r37_codepanel PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r37_codepanel PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src
    ${CMAKE_SOURCE_DIR}/platform/pc/src)
target_compile_definitions(probe_r37_codepanel PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2"
    RE15_RE2_RDT_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon/PL0/RDT")
add_test(NAME unit_r37_codepanel_anker    COMMAND probe_r37_codepanel anker)
add_test(NAME unit_r37_codepanel_kamera   COMMAND probe_r37_codepanel kamera)
add_test(NAME unit_r37_codepanel_lampe    COMMAND probe_r37_codepanel lampe)
add_test(NAME unit_r37_codepanel_zeichner COMMAND probe_r37_codepanel zeichner)
set_tests_properties(unit_r37_codepanel_anker unit_r37_codepanel_kamera unit_r37_codepanel_lampe unit_r37_codepanel_zeichner
                     PROPERTIES TIMEOUT 300)
if(NOT WIN32)
    target_link_libraries(probe_r37_codepanel PRIVATE m)
endif()

# integration_r37_codepanel: echte exe (Lampe im Bild, Kamera-Halt, Toene) - tests/integration/test_r37_codepanel.cmake
#   B ROOM10D0 echter Weg (Kartenleser, Ja, Code 4312 per Cursor), C ROOM1230 SECUGUARD, D ROOM10D0 falscher Code.
if(TARGET re15_pc)
    add_test(NAME integration_r37_codepanel
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PROBE=$<TARGET_FILE:probe_r37_codepanel>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r37_codepanel_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_codepanel.cmake)
    set_tests_properties(integration_r37_codepanel PROPERTIES TIMEOUT 900)
endif()
