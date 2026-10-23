# Runde 40 "untersuchen1120" — Text-Plaetze ROOM1120/1121 msg 1 (Bild), 3 (Fenster), 4 (Aktenschrank).
# Herleitung include/re15_untersuchen_r40.h, Bilder analysis/untersuchungstexte_2026-10-23/.
#
# Mechanik-Riegel: echte ROOM1120/1121.RDT, Raumaufbau wie im Spiel (Installer-Block), Plaetze 5..7 in Satzform
# @0x00CF8, Aktion vor jedem Gegenstand oeffnet den RDT-Text, Telefon/Tueren unveraendert, ROOM1130 ohne Plaetze.
add_executable(test_r40_untersuchen1120 test_r40_untersuchen1120.c)
target_link_libraries(test_r40_untersuchen1120 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r40_untersuchen1120 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r40_untersuchen1120 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r40_untersuchen1120 COMMAND test_r40_untersuchen1120)
set_tests_properties(unit_r40_untersuchen1120 PROPERTIES TIMEOUT 120)
