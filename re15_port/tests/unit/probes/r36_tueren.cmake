# Runde 36 Spur G "tueren" (analysis/befunde_runde36/G_tueren.md). GEPINNT -> add_test je Teil von probe_r36_tueren:
#   fahrstuhl      Punkt 1a: S013/S040/S057 -> RE2-Fahrstuhltuer DOOR27 V0 (RE2-Datei; seit Runde 37 Spur C Port-Archiv P27F
#                  = DOOR27 mit glatten Fluegeln); Maschine: zwei Fluegel
#                  schieben gegenlaeufig (Gegenprobe DOOR25 = ein Blatt hebt sich); echter Durchgang 10C0 -> 1080
#   schott         Punkt 1b: S164/S165 -> DOOR26 V0 zwei Fluegel (Gegenprobe V3 einer); echter Durchgang 2090 -> 20A0
#   aufschliessen  Punkt 3: KARTSE.VBS = RE2-Wellen bytegleich, RE2-Belegbytes (ROOM2110 sub08, EXE @0x800515f4/f8),
#                  Folgen, echter Skriptweg ROOM10D0 (Code) / ROOM3010 (Green Keycard) / ROOM1140 (Riegel)
#   textur         Punkt 2: P0CD-Blatt fein >= 0.095 / form >= 0.30 / hell <= 100, Faser waagerecht (gy > 2 gx),
#                  Glas < 0,8 x Holz, gezeigt = gemalt (ROOM10C04-Zeilenprofil, +-10) an 5 Zeilen
#                  (vorher, dieselbe C-Rechnung: fein 0.0814, form 0.1824, hell 114.5, gy/gx 0.44)
add_executable(probe_r36_tueren probe_r36_tueren.c)
target_link_libraries(probe_r36_tueren PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r36_tueren PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(probe_r36_tueren PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2"
    RE15_ASSET_SHARED_DIR="${CMAKE_SOURCE_DIR}/shared_assets"
    RE15_RE2_INFO_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon")
add_test(NAME unit_r36_tueren_fahrstuhl     COMMAND probe_r36_tueren fahrstuhl)
add_test(NAME unit_r36_tueren_schott        COMMAND probe_r36_tueren schott)
add_test(NAME unit_r36_tueren_aufschliessen COMMAND probe_r36_tueren aufschliessen)
add_test(NAME unit_r36_tueren_textur        COMMAND probe_r36_tueren textur)
set_tests_properties(unit_r36_tueren_fahrstuhl unit_r36_tueren_schott unit_r36_tueren_aufschliessen unit_r36_tueren_textur
                     PROPERTIES TIMEOUT 180)
if(NOT WIN32)
    target_link_libraries(probe_r36_tueren PRIVATE m)
endif()

# integration_r36_tueren: echte exe (PC-Spieler KARTSE.VBS, Tuersequenz-Laeufer) - tests/integration/test_r36_tueren.cmake
if(TARGET re15_pc)
    add_test(NAME integration_r36_tueren
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r36_tueren_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r36_tueren.cmake)
    set_tests_properties(integration_r36_tueren PROPERTIES TIMEOUT 900)
endif()
