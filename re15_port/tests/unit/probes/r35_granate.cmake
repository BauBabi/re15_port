# Runde 35 Spur A — Granate: Wand/Kontakt im Flug (RE2 FUN_8001ED9C), RE2-Reichweite der Explosion
# (Op 47 Box @0x80010918), RE2-Explosionston (0x01110001 -> ARMS0F Satz 10), Brutalitaet (RE2 DEATH[9][1]
# = 0x80108BEC), Explosion an Birkin und Alligator.
#
# Dossier: analysis/befunde_runde35/A_granate.md
#
#   unit_r35_granate            echte Engine: tests/unit/test_r35_granate.c (Abschnitte wand, reichweite,
#                               sound, gore, bosse, strecke) — linkt fx_plattform_pc.c fuer die Ton-Weiche (wie
#                               unit_r34_plattform, ohne SDL). Abschnitt strecke (Nachbesserung 1): Zellform
#                               Typ 1-9 gegen die RE1.5-Handler, Quadrantenliste, ROOM11C0-Raute, ROOM1220
#                               duenne Zellenfront in 38 Wurfphasen, Wurfbild.
#   integration_r35_granate     echte exe (tests/integration/test_r35_granate.cmake): MITTE-Wurf gegen die
#                               Wand in ROOM1140 (Explosion in der Zelle, kein Durchflug), TIEF-Wurf an den
#                               Zombie (RE2-Ton ARMS0F, Zerreissen statt Sturz), RE15_FORCE_EXPLOSION am
#                               ARMIERTEN Birkin (ROOM5090 G5, sub04) und am Alligator (ROOM2090), HOCH-Wurf
#                               gegen die duenne Zellenfront ROOM1220 in zwei Phasen, MITTE-Wurf aus dem
#                               Rechteck der Raute in ROOM11C0.
add_executable(test_r35_granate
    ${CMAKE_CURRENT_LIST_DIR}/../test_r35_granate.c
    ${CMAKE_SOURCE_DIR}/platform/pc/src/fx_plattform_pc.c)
target_link_libraries(test_r35_granate PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_granate PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/platform/pc/src)
target_compile_definitions(test_r35_granate PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r35_granate PRIVATE m)
endif()
add_test(NAME unit_r35_granate COMMAND test_r35_granate alle)
set_tests_properties(unit_r35_granate PROPERTIES TIMEOUT 300)

if(TARGET re15_pc)
    add_test(NAME integration_r35_granate
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r35_granate_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r35_granate.cmake)
    set_tests_properties(integration_r35_granate PROPERTIES TIMEOUT 1200)
endif()
