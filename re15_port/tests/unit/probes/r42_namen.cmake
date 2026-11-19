# Runde 42 Strang "namen" (2026-11-18), Stufe 1: Sprechernamen auf jeder Seite gesprochener Untertitel (Annette im Birkin-Film
# ROOM6000, Leon ROOM6030 msg 4). Belege im Kopf von tests/unit/test_r42_namen.c, Werkzeuge tools/r42_birkinfilm/szene_bauen.py und
# tools/r42_story/texte_stage6.py.
#
#   unit_r42_namen_texte  6000/6030 wie im Spiel geladen: jede Seite einer Nachricht mit Namenskopf traegt den Kopf
#   unit_r42_namen_zeilen  Stufe 4: neue/geaenderte Untertitel je Seite <= 2 Zeilen, keine Ein-Wort-Zeile, <= 271 px, mit Namen
#   unit_r42_namen_seitenwechsel  Stufe 4: Seite 2 zweiseitiger Saetze (6000 48/49/50, 10D0 18/24/25) beim ersten Wort der Aufnahme
#   unit_r42_namen_standzeit  Stufe 4: 6030 msg 4 und msg 32 stehen im Wagen-Programm so lange wie die Original-Zeile msg 0
#   unit_r42_namen_alle_zeilen  Stufe 5: jede Nachricht jedes Raums wie im Spiel geladen (RDT + Port-Texte des Raumaufbaus) - je
#                         Seite hoechstens 2 Zeilen (tools/r42_namen/zweizeilig_s5.py, lesbar_s5.py)
#   unit_r42_namen_quellen_zeilen  Stufe 5: jede Byte-Tabelle in engine/src (.c, gen/.inc) als Nachricht gelesen - je Seite
#                         hoechstens 2 Zeilen (Port-Texte, die erst bei einem Ereignis eingesetzt werden; ohne Inventar)
#   unit_r42_namen_darstellung  Stufe 5 Runde 2: jede Nachricht jedes Raums ueber Message_on GEZEIGT (Full-Text-Raeume 1170/1240/1241
#                         eingeschlossen) - je gezeigtem Fenster hoechstens 2 Zeilen, keine Zeile ueber den Bildrand
#   unit_r42_namen_ohne_stimme  Stufe 5 Runde 2: zweiseitige Zeilen der RDT-Skripte ohne Stimme (VOICE OFF) - jede Seite sichtbar,
#                         die letzte mindestens so lange wie ihr Nachlauf (Pflicht 30E1 14, 5011 3, 11B0 5)
#   unit_r42_namen_fortsetzung  Stufe 2: Mod-Fortsetzungszeilen 10D0/1150/2000 am Satzende der Aufnahme, jede mit Namen; Elza-
#                         Raeume 10D1/1151/2001 mitgeprueft (include/re15_namen_r42.h, tools/r42_namen/fortsetzung_bauen.py)
add_executable(test_r42_namen ${CMAKE_CURRENT_LIST_DIR}/../test_r42_namen.c)
target_link_libraries(test_r42_namen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_namen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_namen PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_SYNCHRO_DIR="${CMAKE_SOURCE_DIR}/../synchro" RE15_ENGINE_SRC_DIR="${CMAKE_SOURCE_DIR}/engine/src")
if(NOT WIN32)
    target_link_libraries(test_r42_namen PRIVATE m)
endif()
foreach(_teil texte fortsetzung zeilen seitenwechsel standzeit alle_zeilen quellen_zeilen darstellung ohne_stimme)
    add_test(NAME unit_r42_namen_${_teil} COMMAND test_r42_namen ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_namen_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
# Runde 2: ohne_stimme laeuft wie VOICE OFF (die Engine-Variable g_re15_voice_laeuft bleibt 0; RE15_VOICE=0 dokumentiert den Fall)
set_tests_properties(unit_r42_namen_ohne_stimme PROPERTIES ENVIRONMENT "RE15_VOICE=0")

# Stufe 6 (2026-11-19): veraltete Aufnahmen bleiben stumm, bis die neue Aufnahme da ist (include/re15_altaufnahme_r42.h,
# tools/r42_namen/altaufnahmen_bauen.py -> engine/src/gen/altaufnahme_r42_tabelle.inc, Haken audio_pc.c re15_voice_load_clip).
#   unit_r42_namen_altaufnahme        Tabelle der 15 Saetze gegen den ganzen Bestand synchro/ (Liste beim Configure): nur diese 15
#                                     Pfade mit dem Abdruck des Tags v0.8.30 gelten als alt, keine andere Datei, kein Doppel;
#                                     Untertitel neu; ein geaendertes Byte -> nicht alt; keine Umleitung beruehrt die Saetze
#   unit_r42_namen_altlader           ECHTER Lader (audio_pc.c) + RE15_AUDIO_CAP_SYNC: die 15 Zeilen stumm (Mitschnitt exakt 0),
#                                     alle anderen Dateien in 3020/30E0/6030/6031 spielen (korr >= 0,9)
#   unit_r42_namen_altlader_ersatz    derselbe Lader aus einer Ersatz-Wurzel (Kopie der Probe + eigenes synchro/ mit vier ersetzten
#                                     Dateien, tests/unit/r42_namen_altlader_ersatz.cmake): ersetzte Dateien spielen, der Rest stumm
file(GLOB_RECURSE _r42_alt_bestand LIST_DIRECTORIES false RELATIVE "${CMAKE_SOURCE_DIR}/../synchro" "${CMAKE_SOURCE_DIR}/../synchro/*.wav")
list(SORT _r42_alt_bestand)
string(REPLACE ";" "\n" _r42_alt_bestand_txt "${_r42_alt_bestand}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/r42_namen_alt_bestand.txt" "${_r42_alt_bestand_txt}\n")
target_compile_definitions(test_r42_namen PRIVATE RE15_R42_ALT_BESTAND="${CMAKE_CURRENT_BINARY_DIR}/r42_namen_alt_bestand.txt")
add_test(NAME unit_r42_namen_altaufnahme COMMAND test_r42_namen altaufnahme WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r42_namen_altaufnahme PROPERTIES TIMEOUT 300)

# Testwerkzeug: "ersetzte" Aufnahme aus einer vorhandenen bauen (auch fuer integration_r42_story3b_leon / _story6_leon)
add_executable(werkzeug_r42_altaufnahme_ersatz ${CMAKE_CURRENT_LIST_DIR}/../werkzeug_r42_altaufnahme_ersatz.c)

if(TARGET SDL2::SDL2-static)
    add_executable(probe_r42_namen_altlader
        ${CMAKE_CURRENT_LIST_DIR}/../probe_r42_namen_altlader.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/audio_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/asset_root_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/werkstatt11b0_pc.c)
    target_link_libraries(probe_r42_namen_altlader PRIVATE re15_engine SDL2::SDL2-static)
    target_include_directories(probe_r42_namen_altlader PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/platform/pc/src)
    target_compile_definitions(probe_r42_namen_altlader PRIVATE
        RE15_PLATFORM_PC
        RE15_ASSET_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_CD_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2"
        RE15_SYNCHRO_DIR="${CMAKE_SOURCE_DIR}/../synchro")
    if(NOT WIN32)
        target_link_libraries(probe_r42_namen_altlader PRIVATE m)
    endif()
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r42_namen_altlader_wd)
    add_test(NAME unit_r42_namen_altlader COMMAND probe_r42_namen_altlader)
    set_tests_properties(unit_r42_namen_altlader PROPERTIES TIMEOUT 300
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r42_namen_altlader_wd)
    add_test(NAME unit_r42_namen_altlader_ersatz
             COMMAND ${CMAKE_COMMAND} -DPROBE=$<TARGET_FILE:probe_r42_namen_altlader>
                     -DWERKZEUG=$<TARGET_FILE:werkzeug_r42_altaufnahme_ersatz>
                     -DSYNCHRO=${CMAKE_SOURCE_DIR}/../synchro
                     -DWURZEL=${CMAKE_CURRENT_BINARY_DIR}/r42_namen_altlader_ersatz
                     -P ${CMAKE_CURRENT_LIST_DIR}/../r42_namen_altlader_ersatz.cmake)
    set_tests_properties(unit_r42_namen_altlader_ersatz PROPERTIES TIMEOUT 300)
endif()
