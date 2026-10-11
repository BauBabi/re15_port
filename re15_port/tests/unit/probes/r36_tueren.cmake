# Runde 36 Spur G "tueren" (analysis/befunde_runde36/G_tueren.md). GEPINNT -> add_test je Teil von probe_r36_tueren:
#   fahrstuhl      Punkt 1a: S013/S040/S057 -> RE2-Fahrstuhltuer DOOR27 V0 (RE2-Datei); Maschine: zwei Fluegel
#                  schieben gegenlaeufig (Gegenprobe DOOR25 = ein Blatt hebt sich); echter Durchgang 10C0 -> 1080
#   schott         Punkt 1b: S164/S165 -> DOOR26 V0 zwei Fluegel (Gegenprobe V3 einer); echter Durchgang 2090 -> 20A0
#   aufschliessen  Punkt 3: KARTSE.VBS = RE2-Wellen bytegleich, RE2-Belegbytes (ROOM2110 sub08, EXE @0x800515f4/f8),
#                  Folgen, echter Skriptweg ROOM10D0 (Code) / ROOM3010 (Green Keycard) / ROOM1140 (Riegel)
add_executable(probe_r36_tueren probe_r36_tueren.c)
target_link_libraries(probe_r36_tueren PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_tueren PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(probe_r36_tueren PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2"
    RE15_RE2_INFO_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon")
add_test(NAME unit_r36_tueren_fahrstuhl     COMMAND probe_r36_tueren fahrstuhl)
add_test(NAME unit_r36_tueren_schott        COMMAND probe_r36_tueren schott)
add_test(NAME unit_r36_tueren_aufschliessen COMMAND probe_r36_tueren aufschliessen)
set_tests_properties(unit_r36_tueren_fahrstuhl unit_r36_tueren_schott unit_r36_tueren_aufschliessen
                     PROPERTIES TIMEOUT 180)
