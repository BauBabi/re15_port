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
