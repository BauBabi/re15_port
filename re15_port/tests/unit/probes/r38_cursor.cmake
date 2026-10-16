# Runde 38 Spur A "cursor" (analysis/befunde_runde38/A_cursor.md): der "komische Schatten" des Cursors.
# Ursache: der PC-Zeichner reihte die RUECKSEITIGE Unterseite (y=+923, 4 Dreiecke mit dem vollen Muster) des
# Cursor-Kastens ein; sie schien durch die durchsichtige Oberseite. Original: NCLIP-Verwurf MAC0 < 0 in allen vier
# Objekt-Zeichnern (FUN_800254a0 @0x800255a8, FUN_800256b0 @0x800257c0, FUN_80022f0c @0x80023010, FUN_80023098
# @0x80023190). Mechanik: include/re15_nclip_r38.h; Haken main.c (Raum-Prop-Schleife) + hebetisch_cursor_pc.c.
#   unit_r38_cursor_nclip    Praedikat (psx-spx NCLIP woertlich, < 0 verworfen, = 0 gezeichnet, Saettigung, Zaehler)
#   unit_r38_cursor_kasten   ROOM11F0-Cursor unter Cut 10 an 25 Lagen: Oberseite gezeichnet, Unterseite verworfen;
#                            Gegenprobe: ohne Verwurf 4 Unterseiten-Dreiecke im Oberseiten-Umriss
#   unit_r38_cursor_raeume   Zensus aller 23 Cursor-Raeume (gleiche Kasten-Bauart, Unterseite gegensinnig gewunden)
#   integration_r38_cursor   echte exe: ROOM11F0 Cut 10 (gegen das DuckStation-Bild), ROOM1150 Hebetisch-Cursor
#                            (Lade-Weg), ROOM10D0 Ziffernfeld; RE15_TRILOG + Framedump (test_r38_cursor.cmake)
# Eigene Datei (nicht tests/unit/CMakeLists.txt), s. probes/README.md.
add_executable(probe_r38_cursor ${CMAKE_CURRENT_LIST_DIR}/../probe_r38_cursor.c)
target_link_libraries(probe_r38_cursor PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r38_cursor PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r38_cursor PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(probe_r38_cursor PRIVATE m)
endif()
add_test(NAME unit_r38_cursor_nclip  COMMAND probe_r38_cursor nclip)
add_test(NAME unit_r38_cursor_kasten COMMAND probe_r38_cursor kasten)
add_test(NAME unit_r38_cursor_raeume COMMAND probe_r38_cursor raeume)
set_tests_properties(unit_r38_cursor_nclip unit_r38_cursor_kasten unit_r38_cursor_raeume PROPERTIES TIMEOUT 120)

if(TARGET re15_pc AND TARGET probe_r30_granate_karte)
    add_test(NAME integration_r38_cursor
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PROBE=$<TARGET_FILE:probe_r38_cursor>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r30_granate_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r38_cursor_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_cursor.cmake)
    set_tests_properties(integration_r38_cursor PROPERTIES TIMEOUT 900)
endif()
