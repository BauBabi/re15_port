# Strang r42/karte (v0.8.31): Karte STAGE2-6 so sauber wie STAGE1.
# Eigene Datei, damit die parallelen r42-Straenge nicht dieselbe CMakeLists editieren.
add_executable(probe_r42_karte_bestand probe_r42_karte_bestand.c)
target_link_libraries(probe_r42_karte_bestand PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r42_karte_bestand PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r42_karte_bestand PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

# RIEGEL: Karte STAGE2 (Blatt 6) + STAGE3 (Blatt 7) - Zone, Marker auf der Kachel, Mitlaufen, Tuermarken.
add_executable(test_r42_karte_st23 test_r42_karte_st23.c)
target_link_libraries(test_r42_karte_st23 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_karte_st23 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_karte_st23 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r42_karte_st23 COMMAND test_r42_karte_st23)
set_tests_properties(unit_r42_karte_st23 PROPERTIES TIMEOUT 120)

# RIEGEL Stufe 2: Karte STAGE4 (Blatt 8), STAGE5 (Blaetter 9-11), STAGE6 (Blatt 12).
add_executable(test_r42_karte_st456 test_r42_karte_st456.c)
target_link_libraries(test_r42_karte_st456 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_karte_st456 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_karte_st456 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r42_karte_st456 COMMAND test_r42_karte_st456)
set_tests_properties(unit_r42_karte_st456 PROPERTIES TIMEOUT 120)

# RIEGEL Stufe 2 Runde 2: der aktuelle Raum ist auf dem Blatt GEZEICHNET (Op-Liste), jede Zone aller Blaetter.
add_executable(test_r42_karte_sichtbar test_r42_karte_sichtbar.c)
target_link_libraries(test_r42_karte_sichtbar PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_karte_sichtbar PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_karte_sichtbar PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r42_karte_sichtbar COMMAND test_r42_karte_sichtbar)
set_tests_properties(unit_r42_karte_sichtbar PROPERTIES TIMEOUT 120)

# RIEGEL Stufe 3 Runde 2: an jeder ECHTEN Ankunftslage (Tuer-Spawn) steht der Punkt auf der eigenen GEMALTEN Kachel
# (Blatt 6-12, hergeleitete Zeilen); Pruefgroesse = Texel aus DATA/MAP0x.PIX, Zeilen direkt aus PSX.EXE.
add_executable(test_r42_karte_ankunft test_r42_karte_ankunft.c)
target_link_libraries(test_r42_karte_ankunft PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_karte_ankunft PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_karte_ankunft PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_EXE_PFAD="${CMAKE_SOURCE_DIR}/../info/Re1.5/PSX.EXE")
add_test(NAME unit_r42_karte_ankunft COMMAND test_r42_karte_ankunft)
set_tests_properties(unit_r42_karte_ankunft PROPERTIES TIMEOUT 180)
