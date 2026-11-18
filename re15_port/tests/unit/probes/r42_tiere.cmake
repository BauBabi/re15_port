# Runde 42 Strang "gore", Stufe 2 "Tiere und Bosse" (BAUPLAN gore_matrix U5, U7, U16, U4, U3, U9, U6, U8; Nutzerregel 16.11.2026):
#   Kraehe zerfaellt bei Pistole (80 %) und Schrot/SPAS (Disc), Spinne platzt mit Beinen am Kadaver, Hund an der Granate = RE2
#   genau, Birkin-Menschform 300 HP, Gorilla 180 HP, Riesenform mit RE2-G4-Schaden (nicht 5090/5091), Beretta-Feuerstoss = EIN
#   Treffer gegen Disc-Zeilen, Gitterhaende nicht anschiessbar (mit Vorpruefung des Durchgangs).
#   unit_r42_tiere       alle Abschnitte
#   unit_r42_tiere_aus   eigener Prozess mit RE15_KEIN_80PROZENT_GORE=1
add_executable(test_r42_tiere
    ${CMAKE_CURRENT_LIST_DIR}/../test_r42_tiere.c)
target_link_libraries(test_r42_tiere PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_tiere PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/platform/pc/src)
target_compile_definitions(test_r42_tiere PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r42_tiere PRIVATE m)
endif()
add_test(NAME unit_r42_tiere COMMAND test_r42_tiere alle)
set_tests_properties(unit_r42_tiere PROPERTIES TIMEOUT 300)
add_test(NAME unit_r42_tiere_aus COMMAND test_r42_tiere aus)
set_tests_properties(unit_r42_tiere_aus PROPERTIES TIMEOUT 120)

# Stufe 2 Runde 2 (Pruefbericht s2 r1 Gelb 2): der Durchgang durch die Gitterhaende im ECHTEN Spiel (kein Schuss)
if(TARGET re15_pc)
    add_test(NAME integration_r42_gitter
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_TOOL=$<TARGET_FILE:test_r42_tiere>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r42_gitter_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_gitter.cmake)
    set_tests_properties(integration_r42_gitter PROPERTIES TIMEOUT 900)
endif()
