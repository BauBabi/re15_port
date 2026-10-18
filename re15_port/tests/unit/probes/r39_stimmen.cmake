# Runde 39 (2026-10-18), Spur A "stimmen" — die Nutzer-Aufnahmen der Port-Szenen ROOM1050 (Ada-Ruf), ROOM1150
# (Irons' Tod), ROOM11B0 (Marvin "But, hold on..." / "unwanted followers") und ROOM1171 ("Alright, let's get down
# from here!") spielen: Tabelle (Raum, Nachricht) -> Datei in engine/src/stimmen_r39.c (Form = RE2 xa_on
# @0x80058028, Stimmnummer eigener Operand @0x80058044), Haken audio_pc.c re15_voice_load_clip "Runde 39 Spur A".
# Dossier analysis/befunde_runde39/A_stimmen.md.
#
# RIEGEL (Unit):
#   unit_r39_stimmen_tabelle   Tabelle gegen die ausgelieferten Daten: Dateien vorhanden (RIFF/WAVE), Message_on-Stellen
#                              der Szenen (k_ruf +0x1C/+0x50/+0x80, irons_tod Programm 0 OP_MSG 22..29, szl +0x0008/+0x010A,
#                              ROOM1171 @0x014BA/0x0151E/0x01586, ROOM11B0 @0x0126E/0x01298/0x0106C), Zuordnung je Punkt,
#                              Sperre (11B0 msg 13/14, 1171 msg 2), Ordnerregel sonst unberuehrt (1170 msg 3, 11B0 msg 1..12);
#                              1150 msg 29 <- main22 (Nutzer-Nachtrag 2, Nachbesserung 1)
#   unit_r39_stimmen_pegel     Nachbesserung 1 (NUTZER-VORGABE "ruhig lauter anheben"): Stufe 2 des Pegel-Bodens
#                              (include/re15_stimmen_pegel_r39.h, T=985 R=2040 C=21759 aus dem Zensus) am GANZEN Bestand
#                              synchro/STAGE*/room*/main*.wav (Liste beim Configure per GLOB): genau die 10 Irons-Dateien
#                              (room1150, alle am 4x-Deckel) werden angehoben (6,9..12,6x, Spitze <= C), alle anderen
#                              bleiben bitgleich (Leon 1150 main15/16/17/22), Stufe 2 ist idempotent
#   unit_r39_stimmen_lader     ECHTER Lader (audio_pc.c) + RE15_AUDIO_CAP_SYNC ohne SDL-Geraet: re15_audio_stimme(Raum, Id)
#                              -> Mitschnitt traegt die Nutzer-Datei (korr >= 0,9) je Tabellenzeile; Sperren stumm;
#                              Ordnerregel 11B0 msg 1, 1171 msg 0, 1170 msg 3 (Leon) unberuehrt
add_executable(test_r39_stimmen ${CMAKE_CURRENT_LIST_DIR}/../test_r39_stimmen.c)
target_link_libraries(test_r39_stimmen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_stimmen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_stimmen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_SYNCHRO_DIR="${CMAKE_SOURCE_DIR}/../synchro")
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r39_stimmen_tabelle_wd)
add_test(NAME unit_r39_stimmen_tabelle COMMAND test_r39_stimmen tabelle)
set_tests_properties(unit_r39_stimmen_tabelle PROPERTIES TIMEOUT 120
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r39_stimmen_tabelle_wd)
# Bestandsliste fuer den Pegel-Riegel (beim Configure; synchro/unused/ liegt nicht unter STAGE*/room*/ und bleibt draussen)
file(GLOB _r39_bestand LIST_DIRECTORIES false "${CMAKE_SOURCE_DIR}/../synchro/STAGE*/room*/main*.wav")
list(SORT _r39_bestand)
string(REPLACE ";" "\n" _r39_bestand_txt "${_r39_bestand}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/r39_stimmen_bestand.txt" "${_r39_bestand_txt}\n")
target_compile_definitions(test_r39_stimmen PRIVATE RE15_R39_BESTAND="${CMAKE_CURRENT_BINARY_DIR}/r39_stimmen_bestand.txt")
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r39_stimmen_pegel_wd)
add_test(NAME unit_r39_stimmen_pegel COMMAND test_r39_stimmen pegel)
set_tests_properties(unit_r39_stimmen_pegel PROPERTIES TIMEOUT 300
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r39_stimmen_pegel_wd)

if(TARGET SDL2::SDL2-static)
    add_executable(probe_r39_stimmen_lader
        ${CMAKE_CURRENT_LIST_DIR}/../probe_r39_stimmen_lader.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/audio_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/asset_root_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/werkstatt11b0_pc.c)
    target_link_libraries(probe_r39_stimmen_lader PRIVATE re15_engine SDL2::SDL2-static)
    target_include_directories(probe_r39_stimmen_lader PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/platform/pc/src)
    target_compile_definitions(probe_r39_stimmen_lader PRIVATE
        RE15_PLATFORM_PC
        RE15_ASSET_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_CD_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2"
        RE15_SYNCHRO_DIR="${CMAKE_SOURCE_DIR}/../synchro")
    if(NOT WIN32)
        target_link_libraries(probe_r39_stimmen_lader PRIVATE m)
    endif()
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r39_stimmen_lader_wd)
    add_test(NAME unit_r39_stimmen_lader COMMAND probe_r39_stimmen_lader)
    set_tests_properties(unit_r39_stimmen_lader PROPERTIES TIMEOUT 300
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r39_stimmen_lader_wd)
endif()

# AUSWERTE-WERKZEUG (kein add_test): Fit einer Nutzer-Datei gegen das Zeilenfenster eines RE15_AUDIO_CAP_SYNC-Mitschnitts
# (Abschnitte aus state.log, "msg:ID" = erstes Bild der Nachricht); Pegelfaktor-Median + Segment-Konsistenz.
add_executable(probe_r39_stimmen_auswertung ${CMAKE_CURRENT_LIST_DIR}/../probe_r39_stimmen_auswertung.c)
if(NOT WIN32)
    target_link_libraries(probe_r39_stimmen_auswertung PRIVATE m)   # sqrt/fabs (Linux-Release-Bau)
endif()

# INTEGRATION (echte exe am echten Weg, je Nutzer-Punkt ein Teil; tests/integration/test_r39_stimmen.cmake):
#   integration_r39_stimmen_1050   Ada-Ruf: msg 22/23/24 <- main09/10/11.wav, Texte, Dateien im Ton
#   integration_r39_stimmen_1150   Irons' Tod (Spielstand + CONTINUE 1130 -> Tuer): msg 22..29 <- main15..22.wav; Irons'
#                                  Zeilen >= 6x (Stufe 2), Stimme/Rest >= 0,45, kein geklemmter Abtastwert; Leon <= 2,1x
#   integration_r39_stimmen_11b0   Marvin/Ada (Spielstand + CONTINUE 11C0 -> Tuer): msg 20/21 <- main13/14.wav, 1/12 Regel
#   integration_r39_stimmen_1171   Elza, Dach (Spielstand + CONTINUE 1131 -> Tuer): msg 3 <- main02.wav, 0/1 Regel, 2 nie
if(TARGET re15_pc AND TARGET probe_r35_cut1150_karte AND TARGET probe_r36_elza_karte)
    foreach(_teil 1050 1150 11b0 1171)
        add_test(NAME integration_r39_stimmen_${_teil}
            COMMAND ${CMAKE_COMMAND}
                -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                -DRE15_AUSWERT_TOOL=$<TARGET_FILE:probe_r39_stimmen_auswertung>
                -DRE15_KARTE_1150=$<TARGET_FILE:probe_r35_cut1150_karte>
                -DRE15_KARTE_ELZA=$<TARGET_FILE:probe_r36_elza_karte>
                -DWORKDIR=${CMAKE_BINARY_DIR}/r39_stimmen_wd
                -DTEIL=${_teil}
                -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_stimmen.cmake)
        set_tests_properties(integration_r39_stimmen_${_teil} PROPERTIES TIMEOUT 1500)
    endforeach()
endif()
