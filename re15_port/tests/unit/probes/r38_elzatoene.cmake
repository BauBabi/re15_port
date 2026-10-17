# Spur H (Runde 38) "elzatoene" — Elza: Schuss-Ton statt Platzhaltertext ROOM1041, Roys Zombie-Stoehnen beim Zugehen
# und Johns Schuesse (korrigierter RE2-Skript-Pistolenschuss) in ROOM1021. Dossier analysis/befunde_runde38/H_elzatoene.md.
#
# RIEGEL (Unit): misst die MECHANIK am echten VM (RDT laden, scd_room_reenter, Anker-Scan, Opcode-/Message_on-Haken).
#   gunshot1041 stoehnen1021 schuss1021 bank re2beleg aufnahme signatur
add_executable(test_r38_elzatoene ${CMAKE_CURRENT_LIST_DIR}/../test_r38_elzatoene.c)
target_link_libraries(test_r38_elzatoene PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_elzatoene PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_elzatoene PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_teil gunshot1041 stoehnen1021 schuss1021 bank re2beleg aufnahme signatur)
    add_test(NAME unit_r38_elzatoene_${_teil} COMMAND test_r38_elzatoene ${_teil})
    set_tests_properties(unit_r38_elzatoene_${_teil} PROPERTIES TIMEOUT 240)
endforeach()

# AUSWERTE-WERKZEUG (kein add_test): Kreuzkorrelation eines Tons (ELZASE.VBS-Satz oder snd1-Satz eines RDT, so dekodiert
# und getaktet wie der Mischer: re15_vag_adpcm_decode + re15_vab_note2pitch2, Schritt = pitch << 4) mit einem
# RE15_AUDIO_CAP_SYNC-Mitschnitt an einem Raum-Bild.
add_executable(probe_r38_elzatoene_kreuz ${CMAKE_CURRENT_LIST_DIR}/../probe_r38_elzatoene_kreuz.c)
target_link_libraries(probe_r38_elzatoene_kreuz PRIVATE re15_engine m)   # m: sqrt (glibc-libm; mingw hat sie in msvcrt, darum unter Windows unsichtbar - Linux-Release v0.8.26 brach hier ab)
target_include_directories(probe_r38_elzatoene_kreuz PRIVATE ${CMAKE_SOURCE_DIR}/include)

# INTEGRATION (echte exe, Elza-Spiel ueber Spielstand + CONTINUE + Tuer + Gehen): je Nutzer-Punkt ein Lauf, gemessen in
# elza.log (Anker), state.log (Nachrichten) und im RE15_AUDIO_CAP_SYNC-Mitschnitt (Kreuzkorrelation je Ton).
foreach(_teil gunshot1041 szene1021)
    add_test(NAME integration_r38_elzatoene_${_teil}
        COMMAND ${CMAKE_COMMAND}
            -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
            -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_elza_karte>
            -DRE15_KREUZ_TOOL=$<TARGET_FILE:probe_r38_elzatoene_kreuz>
            -DRE15_ELZASE=${CMAKE_SOURCE_DIR}/shared_assets/RE2/ELZASE.VBS
            -DRE15_RDT1021=${CMAKE_SOURCE_DIR}/shared_assets/PSX/STAGE1/ROOM1021.RDT
            -DWORKDIR=${CMAKE_BINARY_DIR}/r38_elzatoene_wd
            -DTEIL=${_teil}
            -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_elzatoene.cmake)
    set_tests_properties(integration_r38_elzatoene_${_teil} PROPERTIES TIMEOUT 1200)
endforeach()
