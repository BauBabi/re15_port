# Runde 41 (2026-11-13), Zwischensequenz Raum 114 (ROOM1141, John und Elza) — Riegel, zaehlen zur Suite.
# Herleitung include/re15_zws114_r41.h, Code engine/src/zws114_r41.c + platform/pc/src/zws114_r41_pc.c, Zeilen im Programm
# ENTRIEGELN39 (tools/r39_dining/programme_bauen.py), Film shared_assets/PSX/MOVIE/R114GORI.STR (tools/r41_zws114/film_bauen.py).
#
#   unit_r41_zws114_datei     R114GORI.STR: Form wie CAPCOM.STR, 367 Bilder 320x240 mit den Port-Dekodern, CD-XA-Ton = Filmlaenge
#   unit_r41_zws114_schrei    ROOM11C0 snd1 Satz 4 = 37 Takte, Satz 2 = 16 Takte (Laenge aus der Bank)
#   unit_r41_zws114_programm  ENTRIEGELN39-Bytes (msg 58/61/Sleep 20/msg 6, Sleep 86/msg 60/msg 7, kein Glas) + Haken
#   unit_r41_zws114_ablauf    ROOM1141 am VM: msg 5 -> Schrei -> Tonlaenge + Sleep 20 -> msg 6 -> Drehen + Sleep 86 -> Film -> msg 7 -> msg 8,
#                             harter Schnitt in den Film, danach Einblenden 240..0
#   unit_r41_zws114_blende    RE2-Einblenden nach dem Film (ROOM1080 sub14 Sce_fade_set @0x0252A): Kanal 2, ABR 2, RGB 7, -0x800
# Echte exe: integration_r39_dining1141_d (tests/integration/test_r39_dining1141.cmake) prueft Schrei vor msg 6 und den Film
# zwischen msg 6 und msg 7 am Lauf.
add_executable(test_r41_zws114 ${CMAKE_CURRENT_LIST_DIR}/../test_r41_zws114.c)
target_link_libraries(test_r41_zws114 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_zws114 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_zws114 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_zws114 PRIVATE m)
endif()
foreach(_teil datei schrei programm ablauf blende)
    add_test(NAME unit_r41_zws114_${_teil} COMMAND test_r41_zws114 ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_zws114_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
