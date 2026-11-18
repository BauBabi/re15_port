# Runde 42 Strang "gegenstaende" (v0.8.31, Konzept K-10), Stufe 1 - Riegel. Konstanten/Belege include/re15_gegenstaende_r42.h, Code
# engine/src/gegenstaende_r42.c, Texte tools/r42_gegenstaende/texte.py -> engine/src/gen/gegenstaende_r42_texte.inc.
#
#   unit_r42_gegenstaende_stellen   je Fundstelle Zone/Menge/Bit/Rechteck, Vorrang-Satz umgezogen, Bit gesetzt -> keine Zone
#   unit_r42_gegenstaende_python    2040/2041 Colt Python x6 Bit 91 statt Head of Akuma
#   unit_r42_gegenstaende_leiche    1050 Leiche wie 1051: Ereignis 5 -> Text 5 -> Aufnahme Glock 18 x15 Bit 40; 1051 unveraendert
#   unit_r42_gegenstaende_mutter    Mutter kein Heilmittel; Pult 2030 ohne Mutter -> Text 56; USE -> (9,43) + Text 58; danach Panel
#   unit_r42_gegenstaende_granaten  ZENSUS (Nacharbeit gelb G1): alle RDT-Item_aot_set beider Fassungen, alle Raeume x Eintritts-Cuts
#                                   durch die Installer, alle direkten Vergabestellen im Quelltext, Startausruestung -> <= 2 Granaten
#   unit_r42_gegenstaende_fund      Fundstellen mit Text (gelb G2/G3/G7): Spind 1071, Kisten 3010/3011, Spalt 5021
#   unit_r42_gegenstaende_texte     gelb G4: Name "Detonator", CHECK Python / Magnum mit Python
#   unit_r42_gegenstaende_ultimate  gelb G5: Aufnahme-Bild der Kapseln = Inventar-Kachel des Modus (AUS Mod, AN Disc)
#   unit_r42_gegenstaende_bilder    Item-Bilder der Kapseln 0x1E..0x20 = Inventar-Kachel 2-fach statt Platzhalter
add_executable(test_r42_gegenstaende ${CMAKE_CURRENT_LIST_DIR}/../test_r42_gegenstaende.c)
target_link_libraries(test_r42_gegenstaende PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_gegenstaende PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_gegenstaende PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_gegenstaende PRIVATE m)
endif()
foreach(_teil stellen python leiche mutter granaten bilder fund texte ultimate)
    add_test(NAME unit_r42_gegenstaende_${_teil} COMMAND test_r42_gegenstaende ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_gegenstaende_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
