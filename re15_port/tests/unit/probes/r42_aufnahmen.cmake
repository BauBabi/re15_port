# Runde 42 Strang "fabrikplan", Stufe 2 (Nachtrag 20.11.2026): die Sprachaufnahmen fuer v0.8.31 liegen vor (Commit ce5ec405f).
#   unit_r42_aufnahmen_v0831   ECHTER Lader (audio_pc.c re15_voice_load_clip ueber re15_audio_stimme, mit allen Umleitungen und der
#                              Stumm-Tabelle altaufnahme_r42) + RE15_AUDIO_CAP_SYNC: jeder der 75 Spiel-Saetze der Liste (Teile A, B,
#                              C; tests/unit/gen_r42_aufnahmen_v0831.inc) findet seine Datei, sie traegt keinen Abdruck der
#                              Stumm-Tabelle, der Sprachkanal laeuft und der Mitschnitt enthaelt genau diese Datei (korr >= 0,9).
#                              Gegenprobe: rot auf 1e5ab9f22 (vor dem Merge von master, ohne die Aufnahmen).
if(TARGET SDL2::SDL2-static)
    add_executable(probe_r42_aufnahmen_v0831
        ${CMAKE_CURRENT_LIST_DIR}/../probe_r42_aufnahmen_v0831.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/audio_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/asset_root_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/werkstatt11b0_pc.c)
    target_link_libraries(probe_r42_aufnahmen_v0831 PRIVATE re15_engine SDL2::SDL2-static)
    target_include_directories(probe_r42_aufnahmen_v0831 PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/platform/pc/src)
    target_compile_definitions(probe_r42_aufnahmen_v0831 PRIVATE
        RE15_PLATFORM_PC
        RE15_ASSET_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_CD_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2"
        RE15_SYNCHRO_DIR="${CMAKE_SOURCE_DIR}/../synchro")
    if(NOT WIN32)
        target_link_libraries(probe_r42_aufnahmen_v0831 PRIVATE m)
    endif()
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r42_aufnahmen_v0831_wd)
    add_test(NAME unit_r42_aufnahmen_v0831 COMMAND probe_r42_aufnahmen_v0831)
    set_tests_properties(unit_r42_aufnahmen_v0831 PROPERTIES TIMEOUT 600
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r42_aufnahmen_v0831_wd)
endif()
