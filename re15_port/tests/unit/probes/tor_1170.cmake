# Tor ROOM1170 - RE2-Tuersequenz (analysis/tor_1170/07_modell.md, 09_sequenz.md).
# GEPINNT -> add_test: Skriptmaschine Bild fuer Bild gegen den Katalog-Simulator (DOOR2E + Tor),
# RotMatrix @0x8008e1f4, Tuer-Zuordnung (Intro-Uebergabe Slot 3 bleibt aussen vor).
add_executable(unit_door_seq unit_door_seq.c)
target_link_libraries(unit_door_seq PRIVATE re15_engine re15_test_support)
target_include_directories(unit_door_seq PRIVATE
    ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(unit_door_seq PRIVATE
    RE15_RE2_EXE_PATH="${CMAKE_SOURCE_DIR}/../info/re2leon/PSX.EXE"
    RE15_RE2_DOOR_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon/COMMON/DOOR")
add_test(NAME unit_door_seq COMMAND unit_door_seq)

# Anfrage an beiden Varianten des Tors (ROOM1170 + Elzas ROOM1171), Intro-Uebergabe nicht.
add_executable(probe_tor_1170_anfrage probe_tor_1170_anfrage.c)
target_link_libraries(probe_tor_1170_anfrage PRIVATE re15_engine re15_test_support)
target_include_directories(probe_tor_1170_anfrage PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_tor_1170_anfrage PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_tor_1170_anfrage COMMAND probe_tor_1170_anfrage)
set_tests_properties(unit_tor_1170_anfrage PROPERTIES TIMEOUT 30)
