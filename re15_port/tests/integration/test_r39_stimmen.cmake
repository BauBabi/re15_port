# Runde 39 Spur A "stimmen" — INTEGRATION mit der echten exe am ECHTEN Weg, je Nutzer-Punkt ein TEIL
# (Dossier analysis/befunde_runde39/A_stimmen.md, Zuordnung include/re15_stimmen_r39.h):
#   1050  Punkt 1: Debug-Sprung ROOM1000 -> Tuer -> ROOM1050 -> zur 10A0-Tuer -> Quadrat (Weg test_r35_inventar1050
#         Lauf A): Ada-Ruf-Szene msg 22/23/24 laden main09/10/11.wav, Mitschnitt traegt die Dateien im Zeilenfenster
#   1150  Punkt 2: Spielstand (probe_r35_cut1150_karte 1130 nach10f0 ersteszene) + CONTINUE in ROOM1130, Tuer ->
#         ROOM1150 (Weg test_r35_cut1150 Lauf A): Irons' Tod msg 22..28 laden main15..21.wav (im Ton), msg 29 ohne Datei
#   11b0  Punkt 3: Spielstand (probe_r36_elza_karte 11c0 ...) + CONTINUE in ROOM11C0, Tuer Slot 0 -> ROOM11B0, Szene
#         bis ROOM1260: msg 20/21 laden main13/14.wav (im Ton), msg 1..12 weiter main01..12 (Regel), keine Sperrzeile
#   1171  Punkt 4: Elza (Karte Figur 4), Spielstand ROOM1131 + CONTINUE, Tuer -> ROOM1171, Dachszene bis ROOM10B1 (Weg
#         test_r38_heliport1171 weg_re2): msg 3 laedt main02.wav (im Ton), msg 0/1 weiter main00/01, msg 2 nie
# Mass je Zeile (probe_r39_stimmen_auswertung fit): korr >= 0,5 ohne BGM-Fenster; unter BGM (1150 Irons-Zeilen) tragen
# Pegelfaktor-Median 0,5..4,5 und Segment-Konsistenz >= 0,6 den Nachweis (Dossier §5: gespielter Clip haelt den Faktor
# ueber alle Segmente, ein fehlender streut um 0). Vorher-Werte (Dossier §3): korr 0,006..0,10, Konsistenz < 0,5.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_AUSWERT_TOOL=<probe_r39_stimmen_auswertung> -DWORKDIR=<dir> -DTEIL=<teil>
#               [-DRE15_KARTE_1150=<probe_r35_cut1150_karte>] [-DRE15_KARTE_ELZA=<probe_r36_elza_karte>]
#         -P test_r39_stimmen.cmake
cmake_minimum_required(VERSION 3.16)
set(_tag "r39_stimmen_${TEIL}")
foreach(_v RE15_PC_EXE RE15_AUSWERT_TOOL WORKDIR TEIL)
    if(NOT ${_v})
        message(FATAL_ERROR "${_tag}: ${_v} fehlt")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Keine RE15_*-Variablen des Aufrufers erben (VERTRAG §2): alles, was die Umgebung traegt, wird vorher geloescht.
execute_process(COMMAND "${CMAKE_COMMAND}" -E environment OUTPUT_VARIABLE _envdump)
string(REGEX MATCHALL "(^|\n)RE15_[A-Za-z0-9_]*=" _geerbt "${_envdump}")
foreach(_g ${_geerbt})
    string(REGEX REPLACE "^\n?(RE15_[A-Za-z0-9_]*)=$" "\\1" _n "${_g}")
    unset(ENV{${_n}})
endforeach()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_dir "${WORKDIR}/lauf_${TEIL}_${_lauf_id}")
file(MAKE_DIRECTORY "${_dir}")
set(WORKDIR "${_dir}")
# Messkopie unter eigenem Namen neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe "${_exe_dir}/re15_pc_r39a_${TEIL}_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

macro(fail)
    file(REMOVE "${_exe}")
    message(FATAL_ERROR "${_tag}: ${ARGN} (${WORKDIR})")
endmacro()

function(zeilen _datei _regex _out)
    file(STRINGS "${_datei}" _zz REGEX "${_regex}")
    list(LENGTH _zz _n)
    set(${_out} ${_n} PARENT_SCOPE)
endfunction()

macro(karte _tool)
    if(NOT ${_tool} OR NOT EXISTS "${${_tool}}")
        fail("Kartenwerkzeug ${_tool} fehlt: '${${_tool}}'")
    endif()
    execute_process(COMMAND "${${_tool}}" "re15_card.mcr" ${ARGN}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        fail("Kartenwerkzeug exit=${_rvk}: ${_outk}")
    endif()
endmacro()

set(_gemeinsam RE15_NO_INTRO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1
               RE15_STATE_LOG=state.log RE15_AUDIO_CAP_SYNC=cap.raw)
set(_karten RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0)

if(TEIL STREQUAL "1050")
    set(_lauf ${_gemeinsam} RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        RE15_SET_FLAG=3:121 RE15_DEBUG_JUMP=1000@120 RE15_PLAYER_POS=22230,-13400,0,0
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_INPUT_SCRIPT=U0.8,W14
        RE15_PRESS=square@150 "RE15_EXIT_AT=600#1050")
    set(_abschnitt 2)                        # Abschnitte: Startraum (bis zum Sprung F120), 1000, 1050
    set(_zeilen "22;9;23;10;24;11")          # msg;datei
    set(_texte "22;Woman: Hello\\? Anyone\\? Please, get me out of here!"
               "23;Leon: Another civilian survivor\\."
               "24;Leon: I have to help her!")
    set(_mindest_korr 0.5)
elseif(TEIL STREQUAL "1150")
    karte(RE15_KARTE_1150 1130 nach10f0 ersteszene)
    set(_lauf ${_gemeinsam} ${_karten}
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=W1,A0.2,W200"
        "RE15_EXIT_AT=1250#1150")
    set(_abschnitt 1)                        # Abschnitte: 1130, 1150
    set(_zeilen "22;15;23;16;24;17;25;18;26;19;27;20;28;21")
    set(_texte "22;Leon: Sir!" "25;Irons: Leon\\.\\.\\. I\\.\\.\\. I'm proud to have an officer as dependable as you!"
               "28;Irons: Get them out of here\\.\\.\\. Leon\\.\\.\\." "29;Leon: SIR, Sir\\?!")
    set(_mindest_korr 0.12)                  # BGM MAIN1E liegt ueber Irons' leisen Zeilen (Dossier §5)
elseif(TEIL STREQUAL "11b0")
    karte(RE15_KARTE_ELZA 11c0 -25100 15000 1024 3:0x43 3:0x82 4:0xf3)
    set(_lauf ${_gemeinsam} ${_karten} "RE15_FIRE_AOT=0@60#11c0" "RE15_EXIT_AT=30#1260")
    set(_abschnitt 1)                        # Abschnitte: 11C0, 11B0, 1260
    set(_zeilen "20;13;21;14;1;1;12;12")
    set(_texte "20;Marvin: But, hold on\\.\\.\\." "21;Marvin: I just want to make sure we don't have any unwanted followers\\.")
    set(_mindest_korr 0.5)
elseif(TEIL STREQUAL "1171")
    karte(RE15_KARTE_ELZA 1131 2700 15300 1024 3:194)
    set(_lauf ${_gemeinsam} ${_karten} RE15_EVT_TRACE=1
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=30 RE15_INPUT_SCRIPT=A0.5
        "RE15_FIRE_AOT=5@60#1171" "RE15_EXIT_AT=60#10b1")
    set(_abschnitt 1)                        # Abschnitte: 1131, 1171, 10B1
    set(_zeilen "3;2;0;0;1;1")
    set(_texte "3;John: Alright, let's get down from here!")
    set(_mindest_korr 0.5)
else()
    fail("unbekannter TEIL '${TEIL}'")
endif()

re15_start_spiel(_rv 600 ${_lauf} "${_exe}")
if("${_rv}" STREQUAL "1" OR "${_rv}" STREQUAL "-1" OR NOT EXISTS "${WORKDIR}/debug.log")
    message(STATUS "${_tag}: Lauf ohne Ergebnis (exit=${_rv}) -> EIN Wiederholungsversuch")
    file(GLOB _alt "${WORKDIR}/cap.raw" "${WORKDIR}/debug.log" "${WORKDIR}/state.log")
    if(_alt)
        file(REMOVE ${_alt})
    endif()
    re15_start_spiel(_rv 600 ${_lauf} "${_exe}")
endif()
file(REMOVE "${_exe}")
foreach(_f debug.log state.log cap.raw)
    if(NOT EXISTS "${WORKDIR}/${_f}")
        message(FATAL_ERROR "${_tag}: ${_f} fehlt (exit=${_rv}, ${WORKDIR})")
    endif()
endforeach()
set(_dbg "${WORKDIR}/debug.log")
string(TOUPPER "${TEIL}" _raum)

if(TEIL STREQUAL "1171")
    zeilen("${_dbg}" "Spieler-Familie PL04" _elza)
    if(NOT _elza GREATER 0)
        message(FATAL_ERROR "${_tag}: nicht mit Elza gelaufen (keine PL04-Zeile)")
    endif()
    zeilen("${_dbg}" "\\[voice\\] ROOM1171 msg 2 " _m2)
    if(NOT _m2 EQUAL 0)
        message(FATAL_ERROR "${_tag}: msg 2 wurde angefordert (${_m2} Zeilen) - das Skript zeigt sie nie")
    endif()
endif()
if(TEIL STREQUAL "1150")
    zeilen("${_dbg}" "\\[voice\\] ROOM1150 msg 29: keine Aufnahme" _m29)
    if(NOT _m29 EQUAL 1)
        message(FATAL_ERROR "${_tag}: msg 29 ohne Datei muss genau einmal gemeldet werden (${_m29})")
    endif()
endif()
if(TEIL STREQUAL "11b0")
    zeilen("${_dbg}" "\\[voice\\] ROOM11B0 msg 1[34] stumm" _sperre)
    if(NOT _sperre EQUAL 0)
        message(FATAL_ERROR "${_tag}: Sperrzeile in der Szene (${_sperre}) - msg 13/14 duerfen dort nicht vorkommen")
    endif()
endif()

# Texte der Szene (RE15_MSG_LOG [msgtext])
foreach(_t ${_texte})
    string(REGEX REPLACE "^([0-9]+);(.*)$" "\\1" _id "${_t}")
    string(REGEX REPLACE "^([0-9]+);(.*)$" "\\2" _re "${_t}")
    zeilen("${_dbg}" "\\[msgtext\\] room=${TEIL} id=${_id} \"${_re}\"" _n)
    if(NOT _n GREATER 0)
        message(FATAL_ERROR "${_tag}: Text msg ${_id} fehlt im Log: ${_re}")
    endif()
endforeach()

# Lader-Zeilen + Mitschnitt je Zeile
set(_fit_args "")
set(_zl ${_zeilen})
while(_zl)
    list(GET _zl 0 _msg)
    list(GET _zl 1 _datei)
    list(REMOVE_AT _zl 0 1)
    string(REGEX REPLACE "^([0-9])$" "0\\1" _dd "${_datei}")
    zeilen("${_dbg}" "\\[voice\\] clip loaded: main${_dd}\\.wav" _geladen)
    if(NOT _geladen GREATER 0)
        message(FATAL_ERROR "${_tag}: '[voice] clip loaded: main${_dd}.wav' fehlt (msg ${_msg})")
    endif()
    if(NOT _msg EQUAL _datei)
        zeilen("${_dbg}" "\\[voice\\] ROOM${_raum} msg ${_msg} <- main${_dd}\\.wav \\(Runde 39 Spur A" _zu)
        if(NOT _zu EQUAL 1)
            message(FATAL_ERROR "${_tag}: Zuordnungszeile msg ${_msg} <- main${_dd}.wav fehlt/doppelt (${_zu})")
        endif()
    endif()
    get_filename_component(_synchro "${CMAKE_CURRENT_LIST_DIR}/../../../synchro" ABSOLUTE)
    set(_wav "${_synchro}/STAGE1/room${_raum}/main${_dd}.wav")
    if(NOT EXISTS "${_wav}")
        message(FATAL_ERROR "${_tag}: Nutzer-Datei fehlt: ${_wav}")
    endif()
    list(APPEND _fit_args "msg:${_msg}" "${_wav}")
endwhile()
execute_process(COMMAND "${RE15_AUSWERT_TOOL}" fit "${WORKDIR}/cap.raw" "${WORKDIR}/state.log" ${_abschnitt} ${_fit_args}
                TIMEOUT 300 RESULT_VARIABLE _rvf OUTPUT_VARIABLE _fit)
message(STATUS "${_tag}: Auswertung\n${_fit}")
if(NOT _rvf EQUAL 0)
    message(FATAL_ERROR "${_tag}: Auswertung exit=${_rvf}")
endif()
string(REGEX MATCHALL "zeile F[0-9]+ [^\n]*" _zz "${_fit}")
list(LENGTH _zz _nz)
list(LENGTH _fit_args _na)
math(EXPR _soll "${_na} / 2")
if(NOT _nz EQUAL _soll)
    message(FATAL_ERROR "${_tag}: ${_nz} von ${_soll} Zeilen ausgewertet")
endif()
set(_bericht "")
foreach(_z ${_zz})
    string(REGEX MATCH "zeile (F[0-9]+) ([^:]+): lag (-?[0-9]+) korr (-?[0-9.]+) faktor (-?[0-9.]+) median (-?[0-9.]+) konsistenz ([0-9]+)/([0-9]+)" _m "${_z}")
    if(NOT _m)
        message(FATAL_ERROR "${_tag}: unlesbare Auswertezeile: ${_z}")
    endif()
    set(_bild "${CMAKE_MATCH_1}")
    set(_wav "${CMAKE_MATCH_2}")
    set(_korr "${CMAKE_MATCH_4}")
    set(_med "${CMAKE_MATCH_6}")
    set(_k "${CMAKE_MATCH_7}")
    set(_kn "${CMAKE_MATCH_8}")
    if(_kn EQUAL 0)
        message(FATAL_ERROR "${_tag}: ${_wav}: keine Segmente mit Stimmenergie")
    endif()
    math(EXPR _kons100 "100 * ${_k} / ${_kn}")
    if(_korr LESS _mindest_korr)
        message(FATAL_ERROR "${_tag}: ${_wav} ab ${_bild}: korr ${_korr} < ${_mindest_korr} - die Datei liegt nicht im Ton")
    endif()
    if(_med LESS 0.5 OR _med GREATER 4.5)
        message(FATAL_ERROR "${_tag}: ${_wav} ab ${_bild}: Pegelfaktor-Median ${_med} ausserhalb 0,5..4,5 (Lader hebt hoechstens 4x)")
    endif()
    if(_kons100 LESS 60)
        message(FATAL_ERROR "${_tag}: ${_wav} ab ${_bild}: Segment-Konsistenz ${_k}/${_kn} < 60 %")
    endif()
    string(APPEND _bericht " ${_wav}@${_bild} korr ${_korr} median ${_med} kons ${_k}/${_kn};")
endforeach()
message(STATUS "${_tag}: OK -${_bericht}")
