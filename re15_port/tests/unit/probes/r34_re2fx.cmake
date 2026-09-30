# Runde 34 (Granaten) Spur D — die RE2-FX-Maschine (Saeure-/Brand-Aufschlag, Bodenfeuer).
#
# Dossier: analysis/befunde_runde34_granaten/bau_d.md, Code: engine/src/re2_fx.c, include/re2_fx.h
#
#   unit_r34_re2fx          Registrierung CORE00.ESP, Spawner, Pumpe, Saeure-Bildfolge X..X+4 mit Lage
#                           (O-VB1), Brand 2 Kinder + 3 Flammen gegen den RE2-Strom, Landung -> Op 19,
#                           Applier-Tor step[0x16] >= 16 / X > 0x1000, Lebensdauer, Folgeflammen, Wand
#                           (Op 50 / Op 64); je mit Negativ-Kontrollen
#   unit_r34_re2fx_knochen  O-VB1: die GL-Knochenbasis B aus info/re2leon PL01.PLD + PL01W09.PLW
add_executable(probe_r34_re2fx ${CMAKE_CURRENT_LIST_DIR}/../probe_r34_re2fx.c)
target_link_libraries(probe_r34_re2fx PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34_re2fx PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r34_re2fx PRIVATE m)
endif()
add_test(NAME unit_r34_re2fx COMMAND probe_r34_re2fx)
set_tests_properties(unit_r34_re2fx PROPERTIES TIMEOUT 120)

add_executable(probe_r34_re2fx_knochen ${CMAKE_CURRENT_LIST_DIR}/../probe_r34_re2fx_knochen.c)
target_link_libraries(probe_r34_re2fx_knochen PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34_re2fx_knochen PRIVATE ${CMAKE_SOURCE_DIR}/include)
# Die RE2-Spielerdateien liegen nur unter info/re2leon (Repo-Wurzel = eine Ebene ueber re15_port).
get_filename_component(_r34_repo_root "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)
target_compile_definitions(probe_r34_re2fx_knochen PRIVATE RE15_REPO_ROOT="${_r34_repo_root}")
if(NOT WIN32)
    target_link_libraries(probe_r34_re2fx_knochen PRIVATE m)
endif()
add_test(NAME unit_r34_re2fx_knochen COMMAND probe_r34_re2fx_knochen)
set_tests_properties(unit_r34_re2fx_knochen PROPERTIES TIMEOUT 120)

# O9 / Paket 6: die Aufschlag-Kinder offscreen (VRAM-Modell, PSX-ABR je Texel) -> PPM-Bilder +
# Quad-/Ausschnitt-Listen fuer den Katalog-Vergleich (tools/re2fx_katalog.py --vergleich <dir>).
add_executable(probe_r34_re2fx_bild ${CMAKE_CURRENT_LIST_DIR}/../probe_r34_re2fx_bild.c)
target_link_libraries(probe_r34_re2fx_bild PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34_re2fx_bild PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r34_re2fx_bild PRIVATE m)
endif()
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/r34_re2fx_bild)
add_test(NAME unit_r34_re2fx_bild COMMAND probe_r34_re2fx_bild ${CMAKE_BINARY_DIR}/r34_re2fx_bild)
set_tests_properties(unit_r34_re2fx_bild PROPERTIES TIMEOUT 120)

# Nachbesserung N2 (Gegenpruefung M1/M2): die RE1.5-Raumabbildung von FUN_8004fba0 (re2fx_boden) mit echt
# geladenen Raeumen ROOM1140/1150/1170, ohne Boden-Haken (Wand/frei/Band-2-Zelle/Prop, Negativ-Kontrollen).
add_executable(probe_r34_re2fx_raum ${CMAKE_CURRENT_LIST_DIR}/../probe_r34_re2fx_raum.c)
target_link_libraries(probe_r34_re2fx_raum PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34_re2fx_raum PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r34_re2fx_raum PRIVATE m)
endif()
add_test(NAME unit_r34_re2fx_raum COMMAND probe_r34_re2fx_raum)
set_tests_properties(unit_r34_re2fx_raum PROPERTIES TIMEOUT 120)

# Nachbesserung N5 (Gegenpruefung M7/M8): der PC-Zeichner platform/pc/src/re2fx_pc.c ohne SDL - die sechs
# render_pc.c-APIs sind Attrappen in der Sonde; Slot-Masse (512 x 1280), Seiten-/CLUT-Zeilen-Abbildung,
# Texel gegen das PSX-VRAM-Modell, Reihenfolge, Mischmodus je ABR, Auslassen fremder CLUT-Zeilen.
add_executable(probe_r34_re2fx_pc ${CMAKE_CURRENT_LIST_DIR}/../probe_r34_re2fx_pc.c
                                  ${CMAKE_SOURCE_DIR}/platform/pc/src/re2fx_pc.c)
target_link_libraries(probe_r34_re2fx_pc PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34_re2fx_pc PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/platform/pc/src)
if(NOT WIN32)
    target_link_libraries(probe_r34_re2fx_pc PRIVATE m)
endif()
add_test(NAME unit_r34_re2fx_pc COMMAND probe_r34_re2fx_pc)
set_tests_properties(unit_r34_re2fx_pc PROPERTIES TIMEOUT 120)
