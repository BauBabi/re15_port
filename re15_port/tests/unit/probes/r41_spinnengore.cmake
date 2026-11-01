# Runde 41 Spur S "spinnengore" — Nutzer (2026-11-01): "die spinnen platzen bei magnum Schuss z.b. immer noch nicht...";
# Regel: RE2-Gehirn = Angriffsmuster/Treffer, Gore = Maximum aus RE1.5, RE2 und 80%-Videos.
#
# Dossier: analysis/befunde_runde41/S_spinnengore.md; Code: engine/src/spinnengore_r41.c, include/re15_spinnengore_r41.h,
# Haken in enemy_ai_re2_spider.c (HURT/DEATH), enemy_ai_common.c (RE1.5 case 3), actor_common.c (Platz leeren),
# platform/pc/main.c (Tinte), Messschiene spinnen_r38.c ({G..}-Klammer).
#
#   unit_r41_spinnengore   Byte-Pins STAGE2.BIN (@0x80118f38, @0x80114220, @0x80113c98, @0x80113b74) und die Mechanik
#                          beider Gehirne im Spielschritt (Platzen, Beine, Schalter RE15_KEIN_80PROZENT_GORE, Platz leeren).
add_executable(test_r41_spinnengore ${CMAKE_CURRENT_LIST_DIR}/../test_r41_spinnengore.c)
target_link_libraries(test_r41_spinnengore PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_spinnengore PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_spinnengore PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_spinnengore PRIVATE m)
endif()
add_test(NAME unit_r41_spinnengore COMMAND test_r41_spinnengore)
set_tests_properties(unit_r41_spinnengore PROPERTIES TIMEOUT 120)

#   integration_r41_spinnengore   echte exe, Messschiene RE15_SPINNEN_LOG: Leon ROOM2070 Redhawk (RE2-KI = Standard),
#                                 Elza ROOM2071 Redhawk, Leon ROOM2070 Remington (Bein), Leon ROOM2070 Redhawk mit
#                                 RE15_KEIN_80PROZENT_GORE=1 (Gegenprobe: kein Platzen). Auswerter
#                                 `test_r41_spinnengore pruef_log <log> platzt|bein|aus`.
if(TARGET re15_pc)
    add_test(NAME integration_r41_spinnengore
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_TOOL=$<TARGET_FILE:test_r41_spinnengore>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_spinnengore_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_spinnengore.cmake)
    set_tests_properties(integration_r41_spinnengore PROPERTIES TIMEOUT 900)
endif()

#   probe_r41_gore_inventur   MESSSONDE (kein ctest): Gore-Erreichbarkeit je Typ unter RE2- und RE1.5-KI (Dossier Inventur).
add_executable(probe_r41_gore_inventur ${CMAKE_CURRENT_LIST_DIR}/../probe_r41_gore_inventur.c)
target_link_libraries(probe_r41_gore_inventur PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r41_gore_inventur PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r41_gore_inventur PRIVATE m)
endif()
