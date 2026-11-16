# Runde 41 Optionen (2026-11-16) — "AI MIXED" aus dem OPTIONS-Schirm, neue Option "ULTIMATE 1.5" (Disc-Bilder/-Modelle).
# Belege: include/re15_ultimate15.h, Bericht scratchpad optionen/BERICHT_r1.md. Code: engine/src/ultimate15.c,
# platform/pc/src/ultimate15_pc.c; Haken: main.c (Menue, Ladestellen, Prop-Tausch), inv_render_pc.c, item_icon_common.c,
# itps_common.c, render_pc.c (Modal-Bild-Cache), lautstaerke_r41.c (ultimate= in re15_options.cfg),
# enemy_ai_re2_zombie.c (Env "mixed" -> RE2).
#
#   unit_optionen_r41                     A KI "mixed" -> RE2, B Schalter + Datei-Rundreise (AUS = bisherige Datei
#                                         bitgleich), C Inventar (Zuordnungsregel an den echten Dateien, Puffer, Leser
#                                         AUS = heute / AN = Disc), D Welt (Prop-Zensus, Disc-Modelle lesbar + anders)
#   integration_optionen_r41_menue        echte exe: AI-Schirm ohne dritte Zeile (Rechteck y 89..106 == TOP-Schirm),
#                                         MIXED-Index 2 -> RE2, ULTIMATE-Zeile aus re15_options.cfg (ON/OFF), echte
#                                         Tastenkette TOP cur 4 + Kreis schreibt ultimate=on
#   integration_optionen_r41_bilder       echte exe: Inventar-Raster mit Startausruestung AUS vs AN verschieden,
#                                         ROOM1020 per Sprung: Disc-Weltmodell fuer obj 4/5 nur bei AN
add_executable(test_r41_optionen ${CMAKE_CURRENT_LIST_DIR}/../test_r41_optionen.c)
target_link_libraries(test_r41_optionen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_optionen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_optionen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_optionen PRIVATE m)
endif()
add_test(NAME unit_optionen_r41 COMMAND test_r41_optionen WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_optionen_r41 PROPERTIES TIMEOUT 120)

if(TARGET re15_pc)
    add_test(NAME integration_optionen_r41_menue
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_TOOL=$<TARGET_FILE:test_r41_optionen>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_optionen_menue_wd
                     -DTEIL=menue
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_optionen.cmake)
    set_tests_properties(integration_optionen_r41_menue PROPERTIES TIMEOUT 900)
    add_test(NAME integration_optionen_r41_bilder
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_TOOL=$<TARGET_FILE:test_r41_optionen>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_optionen_bilder_wd
                     -DTEIL=bilder
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_optionen.cmake)
    set_tests_properties(integration_optionen_r41_bilder PROPERTIES TIMEOUT 1200)
endif()
