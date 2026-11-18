# Runde 42 Strang "birkinfilm" (2026-11-18), Stufe 1: RE2-Film "Birkins Mutation" in Leons erstem Treffen mit Annette (ROOM6000).
# Herleitung include/re15_birkinfilm_r42.h + include/re15_zws_r41.h ("ZWEITE BLENDENART"), Code engine/src/birkinfilm_r42.c,
# engine/src/zws_r41.c, platform/pc/src/zws_r41_pc.c, platform/pc/src/audio_pc.c; Werkzeuge tools/r42_birkinfilm/.
#
#   unit_r42_birkinfilm_datei     R409BIRK.STR = R409.STR ohne Sektoren [11630,13460), Rest bytegleich, 3927 Bilder 320x160, Ton
#   unit_r42_birkinfilm_xa        R409_XA09/11/14.XAS = Lesefenster aus VOICE04.XAS (STAGE4.BIN @0x384), Zuordnung Sprachkanal
#   unit_r42_birkinfilm_programm  Programm = sub06 + Einschub vor @0x01156, Texte 40..42, Filmtabelle (Blendenart 4090, y 40)
#   unit_r42_birkinfilm_ablauf    ROOM6000 am VM: Zeilen 0,1,2,40,41,42,3; Film einmal; Blenden 64 ab / 64 auf wie RE2 ROOM4090
#   unit_r42_birkinfilm_einmal    zweites Betreten ((3,100)=1): kein Film
add_executable(test_r42_birkinfilm ${CMAKE_CURRENT_LIST_DIR}/../test_r42_birkinfilm.c)
target_link_libraries(test_r42_birkinfilm PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_birkinfilm PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_birkinfilm PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2"
    RE15_INFO_RE2_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon")
if(NOT WIN32)
    target_link_libraries(test_r42_birkinfilm PRIVATE m)
endif()
foreach(_teil datei xa programm ablauf einmal)
    add_test(NAME unit_r42_birkinfilm_${_teil} COMMAND test_r42_birkinfilm ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_birkinfilm_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
