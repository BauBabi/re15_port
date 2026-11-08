# Runde 41 Spur AN: Annette-Szene ROOM5090 (include/re15_annette_r41.h, analysis/befunde_runde41/AN_annette.md).
# Eigene Datei (GLOB in tests/unit/CMakeLists.txt), damit die gemeinsame CMakeLists unberuehrt bleibt.
add_executable(test_r41_annette ${CMAKE_CURRENT_SOURCE_DIR}/test_r41_annette.c)
target_link_libraries(test_r41_annette PRIVATE re15_engine re15_test_support m)
target_include_directories(test_r41_annette PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_annette PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_AN_AUSZUG="${CMAKE_SOURCE_DIR}/tools/r41_annette/re2_6141_liegen.bin")
add_test(NAME unit_r41_annette COMMAND test_r41_annette)
set_tests_properties(unit_r41_annette PROPERTIES TIMEOUT 120)
if(TARGET re15_pc)
    # Echter Lauf (exe): Leon mit Tor + Zugstart (Szene, danach Kampf), Leon ohne Zugstart (keine Frau), Leiche beim Wiedereintritt
    # (NB3), Elza, Zugstart 6030.
    add_test(NAME integration_r41_annette
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_annette_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_annette.cmake)
    set_tests_properties(integration_r41_annette PROPERTIES TIMEOUT 900)
    # Integration A2 (Gegenpruefung Welle 2 Befund 3): 5090 mit (3,89)/(9,133) wie im echten Weg - AN-Szene, Riesenform 1000 HP
    # (K5c), Rueckstoss, Sturz, G5-Auftritt, Leiche hp -1 (tests/integration/test_r41_annette_g5.cmake; Renderer der exe, kein
    # Software-Renderer wegen des K5c-Absturzes in SDL_BlitTriangle_Slow).
    add_test(NAME integration_r41_annette_g5
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_annette_g5_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_annette_g5.cmake)
    set_tests_properties(integration_r41_annette_g5 PROPERTIES TIMEOUT 900)
    # Nachbesserung 5 (Abnahme 3 A2, Nutzerentscheid 2026-11-03 RE2-Regel): Begleiter in der Schusslinie, ROOM10D0 Marvin + Zombie,
    # der Schuss geht durch den Begleiter auf den Gegner, der Begleiter bleibt unveraendert.
    add_test(NAME integration_r41_begleiter_schusslinie
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_begleiter_schusslinie_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_begleiter_schusslinie.cmake)
    set_tests_properties(integration_r41_begleiter_schusslinie PROPERTIES TIMEOUT 300)
endif()
