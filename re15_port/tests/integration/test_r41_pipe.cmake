# Runde 41 Spur "pipe" — integration_r41_pipe (echte exe). Registrierung: tests/unit/probes/r41_pipe.cmake.
#
# Nutzer 2026-11-16: "Eigentlich sollte die Pipe so aehnlich funktionieren wie ein Messer, aber mit mehr Reichweite."
# Je Figur zwei Laeufe mit GLEICHER Aufstellung, nur die Waffe wechselt (Messer = Rueckfall Id 1, Rohr = Id 2 per
# RE15_GIVE/RE15_EQUIP). Der Zombie wird per RE15_GEGNER_POS 2800 vor dem Spieler gehalten (Blick aufeinander), RE2-KI,
# Eingabe R1 halten + ein Schlag. Messhaken RE15_PIPE_LOG schreibt je Nahkampftreffer eine Zeile.
#   leon   ROOM1140, Spieler (-7600,-17600) rot 0 (+x), Zombie Slot 2 bei (-4800,-17600) rot 2048
#   elza   ROOM1181 (Sprung 1180, Elza-Bit), Spieler (-3758,-7436) rot 1024 (-z), Zombie Slot 3 bei (-3758,-10236) rot 3072
# Gemessen (scratchpad Sweep 2026-11-16): Leon Messer trifft bis 2700, Rohr bis 3000; Elza Messer bis 2500..2700, Rohr
# 2800. Erwartet je Figur: Messer KEINE Trefferzeile, Rohr mindestens eine mit waffe=2 slot=<Zombie>; Bank W02 mit 14 Clips
# (die Messerbank, Original-W02 waere die 11-Clip-Schusswaffen-Vorlage); Rohr laedt die Messer-Ton-Bank ARMS01 (wf.log count=10).
cmake_minimum_required(VERSION 3.16)
set(_tag "r41_pipe")
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "${_tag}: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "${_tag}: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")
set(_basis "${WORKDIR}")

function(pipe_lauf _figur _waffe _out)
    string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _id)
    set(_dir "${_basis}/lauf_${_figur}_w${_waffe}_${_id}")
    file(MAKE_DIRECTORY "${_dir}")
    set(WORKDIR "${_dir}")
    if(_figur STREQUAL "leon")
        set(_env RE15_DEBUG_JUMP=1140@250 "RE15_PLAYER_POS=-7600,-17600,0" "RE15_GEGNER_POS=2,-4800,-17600,2048@1#1140"
                 "RE15_EXIT_AT=330#1140")
    else()
        set(_env RE15_PSELECT_AUTO_SWITCH=1 RE15_DEBUG_JUMP=1180@250 "RE15_PLAYER_POS=-3758,-7436,1024"
                 "RE15_GEGNER_POS=3,-3758,-10236,3072@1#1181" "RE15_EXIT_AT=330#1181")
    endif()
    if(_waffe EQUAL 2)
        list(APPEND _env "RE15_GIVE=2:255")
    endif()
    re15_start_spiel(_rv 240 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
                     RE15_WINDOW_SCALE=1 RE15_AI_FLAVOR=re2 "RE15_EQUIP=${_waffe}" RE15_INPUT_SCRIPT_BASIS=spiel
                     RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=W9|W1.5,M1,MA0.2,M1.5,W1" RE15_WAFFEN_LOG=wf.log
                     RE15_PIPE_LOG=pipe.log ${_env} "${RE15_PC_EXE}")
    if(NOT EXISTS "${_dir}/debug.log" OR NOT _rv EQUAL 0)
        message(STATUS "${_tag}: ${_figur} w${_waffe} ohne Ergebnis (exit=${_rv}) -> EIN Wiederholungsversuch")
        file(REMOVE "${_dir}/debug.log" "${_dir}/wf.log" "${_dir}/pipe.log")
        re15_start_spiel(_rv 240 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
                         RE15_WINDOW_SCALE=1 RE15_AI_FLAVOR=re2 "RE15_EQUIP=${_waffe}" RE15_INPUT_SCRIPT_BASIS=spiel
                         RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=W9|W1.5,M1,MA0.2,M1.5,W1" RE15_WAFFEN_LOG=wf.log
                         RE15_PIPE_LOG=pipe.log ${_env} "${RE15_PC_EXE}")
    endif()
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "${_tag}: ${_figur} w${_waffe} exit=${_rv}, ${_dir}")
    endif()
    file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
    file(STRINGS "${_dir}/debug.log" _pin REGEX "\\[r37a\\] RE15_GEGNER_POS: Akteur")
    if(NOT _ex OR NOT _pin)
        message(FATAL_ERROR "${_tag}: ${_figur} w${_waffe}: EXIT_AT oder Gegner-Halter fehlt (${_dir})")
    endif()
    set(_treffer "")
    if(EXISTS "${_dir}/pipe.log")
        file(STRINGS "${_dir}/pipe.log" _treffer REGEX "^\\[pipe\\] F[0-9]+ waffe=${_waffe} ")
    endif()
    list(LENGTH _treffer _n)
    set(${_out} "${_n};${_dir}" PARENT_SCOPE)
endfunction()

foreach(_figur leon elza)
    pipe_lauf(${_figur} 1 _m)
    pipe_lauf(${_figur} 2 _r)
    list(GET _m 0 _nm)
    list(GET _m 1 _dm)
    list(GET _r 0 _nr)
    list(GET _r 1 _dr)
    if(NOT _nm EQUAL 0)
        message(FATAL_ERROR "${_tag}: ${_figur}: das MESSER trifft auf 2800 (${_nm} Treffer) — Messer-Reichweite veraendert? ${_dm}")
    endif()
    if(_nr LESS 1)
        message(FATAL_ERROR "${_tag}: ${_figur}: das ROHR trifft auf 2800 nicht — keine groessere Reichweite (${_dr})")
    endif()
    file(STRINGS "${_dr}/debug.log" _bank REGEX "\\[equip\\] W-bank -> W02 \\(Clips 14")
    if(NOT _bank)
        message(FATAL_ERROR "${_tag}: ${_figur}: Rohr-Bank W02 nicht mit 14 Clips geladen (${_dr})")
    endif()
    # Messer-Ton-Bank: ARMS01.EDH traegt 10 EDT-Saetze (pBAV 40 / 4), ARMS02 (Pistolen-Platzhalter) 8 -> "count=10".
    file(STRINGS "${_dr}/wf.log" _ton REGEX "arms_rec=5 bank=1\\(geladen=1\\) count=10")
    if(NOT _ton)
        message(FATAL_ERROR "${_tag}: ${_figur}: Schwung-Ton (Satz 5) nicht aus der Messer-Bank ARMS01 (${_dr})")
    endif()
    message(STATUS "${_tag}: ${_figur}: Messer 0 Treffer, Rohr ${_nr} Treffer auf 2800, Bank W02 14 Clips")
endforeach()
message(STATUS "${_tag}: OK")
