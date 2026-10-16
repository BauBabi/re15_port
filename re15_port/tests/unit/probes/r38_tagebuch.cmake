# Runde 38 Spur G "tagebuch" — Roys Diary statt der Code-Nachricht (Elza, ROOM1011) + Kartenkette Communications
# Room -> Prison Room. Dossier analysis/befunde_runde38/G_tagebuch.md, Belege include/re15_tagebuch_r38.h,
# Satz re15_port/tools/r38_g/tagebuch_satz.py (FILE30_*.TIM unter shared_assets/RE2/FILES).
#
# RIEGEL (Unit, je Nutzer-Punkt die MECHANIK):
#   unit_r38_tagebuch_dokument  Tabelle (Item 0x4D, Bildsatz 30, max_page 5, H 176, "Roys Diary"), FILE30-Bytes ==
#                               Satz (FNV), Papier == FILE16, "0513" gruen ueber re15_re2doc_pixel (TEX.TIM Zeile 482)
#   unit_r38_tagebuch_ablauf    echte VM + Statusschirm: Roy untersuchen -> sub07 parkt an msg 18 @0x00E64, Leser
#                               statt Nachricht, Liste, "filed", Bit 68, Kette erst am Evt_end @0x00E82 (Blatt 3/9 ->
#                               1/3, zeitgesteuert, schliesst); zweites Untersuchen: nur lesen
#   unit_r38_tagebuch_karte     Ziele in der normalen Karte bis Prison Key (9,83) / John frei (4,0xFC); Leon unveraendert
#   unit_r38_tagebuch_leon      Gegenprobe ROOM1010 (Leon): nichts
add_executable(test_r38_tagebuch ${CMAKE_CURRENT_LIST_DIR}/../test_r38_tagebuch.c)
target_link_libraries(test_r38_tagebuch PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_tagebuch PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_tagebuch PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r38_tagebuch PRIVATE m)
endif()
foreach(_teil dokument ablauf karte leon)
    add_test(NAME unit_r38_tagebuch_${_teil} COMMAND test_r38_tagebuch ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r38_tagebuch_${_teil} PROPERTIES TIMEOUT 240)
endforeach()
