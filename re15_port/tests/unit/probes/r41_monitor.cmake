# Runde 41 Spur Monitor (Zweig r41/sichel_szene): die Hangar-Szene als Monitorbild im Monitor Room 5030/5031 (include/re15_monitor_r41.h,
# engine/src/monitor_r41.c). Nutzer 2026-11-15 "100%ig kein Boss Kampf ... Leon oder Elza sehen Birkin auf dem Monitor".
#   unit_r41_monitor                    effekt / ablauf (c)(d)(e) Leon + Elza / hangar (b) / speichern (b)(c)
#   unit_r41_monitor_gegen_kein_bild    RE15_KEIN_MONITORBILD=1 -> Abschnitt "ablauf" muss ROT sein (c)
#   unit_r41_monitor_gegen_hangar_alt   RE15_HANGAR_IM_RAUM=1 (alter Ausloeser) -> Abschnitt "hangar" muss ROT sein (b)
#   unit_r41_monitor_gegen_stelle       Rueckweg 300 neben die Stelle -> Abschnitt "stelle" muss ROT sein (e)
#   unit_r41_monitor_gegen_sherry       (3,0x2F) = 1 vorab -> Abschnitt "sherry" muss ROT sein (d)
#   integration_r41_monitor             echte exe: Leon 5030 + Elza 5031 (Sherry-Ereignis msg 3 danach), Gegenprobe ohne Monitorbild
add_executable(test_r41_monitor ${CMAKE_CURRENT_LIST_DIR}/../test_r41_monitor.c)
target_link_libraries(test_r41_monitor PRIVATE re15_engine re15_test_support m)
target_include_directories(test_r41_monitor PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_monitor PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r41_monitor COMMAND test_r41_monitor alles)
set_tests_properties(unit_r41_monitor PROPERTIES TIMEOUT 120)
add_test(NAME unit_r41_monitor_gegen_kein_bild COMMAND test_r41_monitor ablauf gegen)
set_tests_properties(unit_r41_monitor_gegen_kein_bild PROPERTIES TIMEOUT 120 ENVIRONMENT "RE15_KEIN_MONITORBILD=1")
add_test(NAME unit_r41_monitor_gegen_hangar_alt COMMAND test_r41_monitor hangar gegen)
set_tests_properties(unit_r41_monitor_gegen_hangar_alt PROPERTIES TIMEOUT 120 ENVIRONMENT "RE15_HANGAR_IM_RAUM=1")
add_test(NAME unit_r41_monitor_gegen_stelle COMMAND test_r41_monitor stelle gegen)
set_tests_properties(unit_r41_monitor_gegen_stelle PROPERTIES TIMEOUT 120)
add_test(NAME unit_r41_monitor_gegen_sherry COMMAND test_r41_monitor sherry gegen)
set_tests_properties(unit_r41_monitor_gegen_sherry PROPERTIES TIMEOUT 120)
if(TARGET re15_pc)
    add_test(NAME integration_r41_monitor
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_monitor_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_monitor.cmake)
    set_tests_properties(integration_r41_monitor PROPERTIES TIMEOUT 900)
endif()
