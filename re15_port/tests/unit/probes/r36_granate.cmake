# Runde 36 Spur D — Granate: (1) kein Feuer an Zombies bei der Explosiv-Granate (NUTZER-VORGABE; Form: die
# beiden Flammen-Spawns des RE2-Spreng-Russ FUN_8010640C @0x80106418-44/@0x80106450-74 entfallen, Brand-
# Verkohlung FUN_80106128 unveraendert), (2) Wurf-Aussetzer ("Granate wird manchmal nicht geworfen").
#
# Dossier: analysis/befunde_runde36/D_granate.md
#
#   unit_r36_granate            echte Engine: tests/unit/test_r36_granate.c — [1] feuer: Zaehlung der Flammen-
#                               Slots (Id 8) nach HE-Tod = 0 / nach Brand-Tod > 0, Zerreissen + Russ-Tinten
#                               bleiben; [2] wurf: N Abzuege = N Granaten im Spielschritt.
#   integration_r36_granate     echte exe (tests/integration/test_r36_granate.cmake): HE-Wurf an die Zombies in
#                               ROOM1140 mit RE15_FX_LOG (kein id=8 nach der Explosion), Brand-Wurf (id=8 > 0),
#                               Wurfserie (SPAWN-Zeilen = Abzuege).
add_executable(test_r36_granate
    ${CMAKE_CURRENT_LIST_DIR}/../test_r36_granate.c)
target_link_libraries(test_r36_granate PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_granate PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/platform/pc/src)
target_compile_definitions(test_r36_granate PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r36_granate PRIVATE m)
endif()
add_test(NAME unit_r36_granate COMMAND test_r36_granate alle)
set_tests_properties(unit_r36_granate PROPERTIES TIMEOUT 300)

if(TARGET re15_pc)
    add_test(NAME integration_r36_granate
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r36_granate_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r36_granate.cmake)
    set_tests_properties(integration_r36_granate PROPERTIES TIMEOUT 1200)
endif()
