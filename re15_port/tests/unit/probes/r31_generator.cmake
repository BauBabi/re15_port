# Runde 31 (2026-09-29), Thema G "Generator ROOM11F0": die Abnahme des Raetsels (Evt_exec
# sub18 @0x012E6 + Set(4,238,1) @0x012EA, Meldung "Power supply OK.", Bestaetigungston,
# Licht) erst, wenn der Leistungszeiger auf 80 steht und 30 Bilder stand — RE2 ROOM2130.RDT
# sub04: ewhile @0x01708, sleep 30 @0x0171C/@0x0171D, cmp(var5==80) @0x01752. Waehrend Fahrt
# und Ruhe Eingabesperre wie Bank 2 Bit 7 (@0x01110..@0x01818).
#
# Nutzer: "warte erst bis der zeiger final auf 80 steht, bevor du mit ok das abnimmst,
# das Licht anschaltest etc."
#
# EIGENE Datei (nicht die gemeinsame CMakeLists.txt), s. probes/README.md.
add_executable(r31_generator ${CMAKE_CURRENT_SOURCE_DIR}/r31_generator.c)
target_link_libraries(r31_generator PRIVATE re15_engine re15_test_support)
target_include_directories(r31_generator PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(r31_generator PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r31_generator COMMAND r31_generator)
set_tests_properties(unit_r31_generator PROPERTIES TIMEOUT 240)
