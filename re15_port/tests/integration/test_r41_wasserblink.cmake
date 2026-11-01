# =============================================================================
# KEIN BLINKEN IM KANALWASSER VON ROOM2000/2001 (Runde 41, Spur W "wasserblink").
#
# Dossier analysis/befunde_runde41/W_wasserblink.md. Nutzer 2026-11-01: "Das blinken im Wasser ist auf jeden Fall
# fehlerhaft und quatsch" (Marke ROOM2000 Cut 2, F444). Ursache: engine/src/raumlicht.c Fall 0x2000 (Commit 4100d2aed),
# ausgedachte Glanzpunkte im 4-Bild-Takt. Die Raumdaten setzen im Kanal kein Licht. Der Deckentropfen Effekt 6 Sub 0
# (Routine 13 @0x80017990; Start nur im Repo-Baum, main00 @0x0177C `04 ff 18 07`) ist ein ESP-Effekt und haengt im
# Port bei y -10000 (raumlicht.c, Dossier OFFEN 4), ausserhalb des Kanal-Fensters unten. Laesst der Port die Tropfen
# fallen, faellt Punkt 5 (-14476,-23171) bei x 118..125 in dieses Fenster, und der Riegel ist anzupassen. Fall 0x2000 entfernt.
#
#   TEIL=leon / elza   Debug-Sprung 2000 (Elza -> 2001), Spawn (-7000,-25659) Band 1: die RVD-Zonen 0->1 (@0x0028C) und
#                      1->2 (@0x002C8) bringen die Kamera auf Cut 2 = Blick der Marke. Bildserie F300..F419:
#                      - Kanal-Fenster x 104..230, y 100..200 (Leon steht links davon) springt in keinem Bildwechsel
#                        um mehr als 8 (master aa88a1b6a: 16 von 119 Wechseln, groesster Sprung 131),
#                      - Bild F360 im Fenster gleich dem Lauf mit RE15_KEIN_RAUMLICHT=1 (Raumlicht traegt dort nichts bei),
#                      - der Wasserlauf der Spur N laeuft weiter ("[wasserlauf41] ROOM200x: 6 Strahlen").
#   TEIL=gegen         Gegenprobe: ROOM50B0 Cut 2 traegt den Raumlicht-Schein weiter; Bild F150 mit und ohne
#                      RE15_KEIN_RAUMLICHT unterscheidet sich im Bodenband x 0..130, y 165..200 um >= 20 — der Messweg
#                      (RE15_FRAMEDUMP) sieht, was der Raumlicht-Pfad zeichnet.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r41_wasserblink> -DTEIL=<leon|elza|gegen> -DWORKDIR=<dir>
#               -P test_r41_wasserblink.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
foreach(_v RE15_PC_EXE RE15_TOOL)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r41_wasserblink: ${_v} fehlt: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR OR NOT TEIL)
    message(FATAL_ERROR "r41_wasserblink: WORKDIR/TEIL fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_basis}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte "${_exe_dir}/re15_pc_r41w_${TEIL}_*${_exe_ext}")
foreach(_k IN LISTS _alte)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r41w_${TEIL}_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

set(_fehler "")
macro(_lauf _name _timeout)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv ${_timeout}
        SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
        ${ARGN}
        "${_exe}")
    set(_dbg "")
    if(EXISTS "${WORKDIR}/debug.log")
        file(READ "${WORKDIR}/debug.log" _dbg)
    else()
        set(_fehler "${_fehler}\n[${_name}] kein debug.log (exit=${_rv})")
    endif()
endmacro()
function(_muss _text _wort _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(_p LESS 0)
        set(_fehler "${_fehler}\n${_was}: '${_wort}' fehlt im debug.log" PARENT_SCOPE)
    endif()
endfunction()

if("${TEIL}" STREQUAL "leon" OR "${TEIL}" STREQUAL "elza")
    if("${TEIL}" STREQUAL "elza")
        set(_fig RE15_PSELECT_AUTO_SWITCH=1)
        set(_raum 2001)
    else()
        set(_fig "")
        set(_raum 2000)
    endif()
    set(_ort RE15_DEBUG_JUMP=2000@250 "RE15_PLAYER_POS=-7000,-25659,1976,1")

    _lauf(an 300 ${_fig} ${_ort} "RE15_FRAMEDUMP=300-419/1:w_" "RE15_EXIT_AT=425#${_raum}")
    set(_dir_an "${WORKDIR}")
    _muss("${_dbg}" "loaded room${_raum}.rdt" "[an] Raum")
    _muss("${_dbg}" "[pri] cut=2 " "[an] Kamera Cut 2 ueber die RVD-Zonen")
    _muss("${_dbg}" "[wasserlauf41] ROOM${_raum}: 6 Strahlen, Lauf ueber den Weg Effekt 0x0F" "[an] Wasserlauf der Spur N")
    _muss("${_dbg}" "EXIT_AT: Bild 425 in Raum ${_raum}" "[an] Lauf zu Ende")
    if("${TEIL}" STREQUAL "elza")
        _muss("${_dbg}" "Spieler-Familie PL04" "[an] Elza gewaehlt")
    endif()

    _lauf(aus 300 ${_fig} ${_ort} RE15_KEIN_RAUMLICHT=1 "RE15_FRAMEDUMP=360:w_360.ppm" "RE15_EXIT_AT=365#${_raum}")
    set(_dir_aus "${WORKDIR}")

    file(GLOB _bilder "${_dir_an}/w_*.ppm")
    list(LENGTH _bilder _n)
    if(_n LESS 120)
        set(_fehler "${_fehler}\n[an] nur ${_n} von 120 Bildern")
    else()
        list(SORT _bilder)
        execute_process(COMMAND "${RE15_TOOL}" flacker 104 230 100 200 8 0 ${_bilder}
                        TIMEOUT 120 RESULT_VARIABLE _r1 OUTPUT_VARIABLE _o1)
        message(STATUS "r41_wasserblink[${TEIL} flacker]:\n${_o1}")
        if(NOT _r1 EQUAL 0)
            set(_fehler "${_fehler}\n[flacker] das Kanalwasser blinkt:\n${_o1}")
        endif()
        if(EXISTS "${_dir_aus}/w_360.ppm")
            execute_process(COMMAND "${RE15_TOOL}" gleich 104 230 100 200 0 "${_dir_an}/w_000360.ppm" "${_dir_aus}/w_360.ppm"
                            TIMEOUT 60 RESULT_VARIABLE _r2 OUTPUT_VARIABLE _o2)
            message(STATUS "r41_wasserblink[${TEIL} gleich]:\n${_o2}")
            if(NOT _r2 EQUAL 0)
                set(_fehler "${_fehler}\n[gleich] Raumlicht zeichnet im Kanal:\n${_o2}")
            endif()
        else()
            set(_fehler "${_fehler}\n[aus] kein Bild F360")
        endif()
    endif()
elseif("${TEIL}" STREQUAL "gegen")
    _lauf(mit 300 RE15_DEBUG_JUMP=50B0@250 RE15_FORCE_CUT=2 "RE15_FRAMEDUMP=150:g.ppm" "RE15_EXIT_AT=160#50B0")
    set(_dir_mit "${WORKDIR}")
    _muss("${_dbg}" "loaded room50b0.rdt" "[mit] Raum 50B0")
    _lauf(ohne 300 RE15_DEBUG_JUMP=50B0@250 RE15_FORCE_CUT=2 RE15_KEIN_RAUMLICHT=1 "RE15_FRAMEDUMP=150:g.ppm" "RE15_EXIT_AT=160#50B0")
    set(_dir_ohne "${WORKDIR}")
    if(EXISTS "${_dir_mit}/g.ppm" AND EXISTS "${_dir_ohne}/g.ppm")
        execute_process(COMMAND "${RE15_TOOL}" gleich 0 130 165 200 19 "${_dir_mit}/g.ppm" "${_dir_ohne}/g.ppm"
                        TIMEOUT 60 RESULT_VARIABLE _r3 OUTPUT_VARIABLE _o3)
        message(STATUS "r41_wasserblink[gegen]:\n${_o3}")
        if(_r3 EQUAL 0)
            set(_fehler "${_fehler}\n[gegen] Raumlicht 50B0 im Bild nicht zu sehen (Unterschied < 20) — Messweg blind:\n${_o3}")
        endif()
    else()
        set(_fehler "${_fehler}\n[gegen] Bild F150 fehlt")
    endif()
else()
    set(_fehler "unbekannter TEIL ${TEIL}")
endif()

file(REMOVE "${_exe}")
if(NOT _fehler STREQUAL "")
    message(FATAL_ERROR "r41_wasserblink[${TEIL}] FEHLER:${_fehler}")
endif()
message(STATUS "r41_wasserblink[${TEIL}]: OK")
