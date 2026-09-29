# Runde 32 (2026-09-29) - Tor ROOM1170 zu dunkel. Dossier: analysis/befunde_runde32/tor_helligkeit.md
# GEPINNT:
#   unit_r32_tor_hell        (nur Engine) Tormaschine setzt Fluegel/Pfosten mit Flag 0x1000 auf
#                            (0x1a80 / 0x1280 = DOOR2B-Blatt @Datei 0x5022; BK 136 @0x800142bc),
#                            Schild-Eckfarbe 107 mit dem Port-Lichtcode (Gegenprobe BK 68 -> 73),
#                            Schild-Texel x 107/128 = gemaltes Schild auf 3 %.
#   integration_r32_tor_hell (echte exe, beschleunigter Renderer, RE15_WINDOW_SCALE=1,
#                            RE15_TUER_TEST=1, RE15_TUER_BOGEN -> S000_anfang.ppm = Bild 20, im
#                            Standbild 16..129 pixelgleich mit Bild 100): Schild-Rechteck gezeigt /
#                            gemalt auf 3 %; Rohrflaeche mit Eckfarbe 148 hellt auf (F/Texel >= 1,10,
#                            door_scene_pc.c tri_psx).
add_executable(probe_r32_tor probe_r32_tor.c)
target_link_libraries(probe_r32_tor PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r32_tor PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r32_tor PRIVATE
    RE15_RE2_DOOR_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon/COMMON/DOOR")
add_test(NAME unit_r32_tor_hell COMMAND probe_r32_tor)
set_tests_properties(unit_r32_tor_hell PROPERTIES TIMEOUT 30)

if(TARGET re15_pc)
    add_test(NAME integration_r32_tor_hell
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DPRUEFER=$<TARGET_FILE:probe_r32_tor>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r32_tor_hell_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r32_tor_hell.cmake)
    set_tests_properties(integration_r32_tor_hell PROPERTIES TIMEOUT 120)
endif()
