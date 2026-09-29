# Runde 31 - Tueren (analysis/befunde_runde31/tueren_04_bau.md).
# GEPINNT -> add_test je Teil von probe_r31_tueren:
#   viereck    40-B-Tuersaetze ROOM4030/4031 (Punkte pc+6..21, Nutzlast pc+22, FUN_80014368),
#              Installation + echter Durchgang im Spielschritt nach ROOM4040 / ROOM4080.
#   maschine   Tuermaschine gegen den Katalog-Simulator fuer alle 43 benutzten Archiv-Varianten
#              (Kopien shared_assets/RE2/DOOR, je Bild Pruefsumme; Skript 0 verteilt die Variante).
#   zuordnung  Port-Tabelle gegen die echten Door_aot_set; Kreuz-Raum-Tuer S042 stellt im Spielschritt
#              die Anfrage (DOOR13 V1, Spender DOOR07), nicht abgedeckte nicht, Tor/Intro unveraendert.
add_executable(probe_r31_tueren probe_r31_tueren.c)
target_link_libraries(probe_r31_tueren PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r31_tueren PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(probe_r31_tueren PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_r31_viereck   COMMAND probe_r31_tueren viereck)
add_test(NAME unit_r31_maschine  COMMAND probe_r31_tueren maschine)
add_test(NAME unit_r31_zuordnung COMMAND probe_r31_tueren zuordnung)
set_tests_properties(unit_r31_viereck unit_r31_maschine unit_r31_zuordnung PROPERTIES TIMEOUT 120)

# Ton ueberlebt den Raumwechsel: braucht das ECHTE audio_pc.c + SDL2 (wie test_rotor_bgm_pin),
# nicht die Audio-Stubs aus re15_test_support.
if(TARGET SDL2::SDL2-static)
    add_executable(test_r31_tuer_ton
        test_r31_tuer_ton.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/audio_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/asset_root_pc.c)
    target_link_libraries(test_r31_tuer_ton PRIVATE re15_engine SDL2::SDL2-static)
    target_include_directories(test_r31_tuer_ton PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/platform/pc/src)
    target_compile_definitions(test_r31_tuer_ton PRIVATE
        RE15_PLATFORM_PC
        RE15_ASSET_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_CD_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
    add_test(NAME unit_r31_tuer_ton COMMAND test_r31_tuer_ton)
    set_tests_properties(unit_r31_tuer_ton PROPERTIES TIMEOUT 60)
endif()
