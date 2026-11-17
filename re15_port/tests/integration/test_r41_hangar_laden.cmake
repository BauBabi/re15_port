cmake_minimum_required(VERSION 3.16)   # cmake -P: ohne diese Zeile gelten im Linux-Container (CMake 3.28) die alten Policies (IN_LIST, if(TRUE)) - Muster der aelteren Integrationsskripte, Memory reai-v2-linux-test-portabilitaet
# =============================================================================
# HANGAR-SZENE NACH DEM LADEN (Runde 41, Rueckbau des Monitorbilds auf den abgenommenen Stand 23f9da5c9, Zweig r41/sichel_szene).
#
# Nutzer-Entscheid 2026-11-15: Monitorbild "nein", Ablauf "Im Hangar, dann Kampf" = Stand 23f9da5c9. Einzige Neuerung: der Merker
# "Hangar-Szene gesehen" war in 23f9da5c9 nur prozessweit (analysis/befunde_runde41/H_hangar.md, 6. Offen Punkt 1: nach dem Laden lief
# die Szene erneut). Seit dem Rueckbau traegt ihn das Bank-9-Bit RE15_BF_HANGAR_BIT = 38 (PORT-WAHL, include/re15_birkin_formen.h),
# gesetzt am Szenenende (birkin_formen.c bf_hangar_beenden), gelesen beim Betreten (re15_bf_raumtick).
#
# Je Figur (Leon 50A0, Elza 50A1) zwei Prozesse auf EINER Karte, Weg wie integration_r41_schwanz_zusammen (Debug-Sprung 50A0, Affen per
# Kill-Flag 8:42/8:43 aus, Eingabeskript nach Sueden -> angeforderter Schnitt 4 -> Szene):
#   Lauf 1  Szene laeuft (Start + Ende), danach Speichern in Bild 390 (Szenenende gemessen F378/F379), Prozessende nach dem Speichern.
#   Lauf 2  NEUER Prozess, CONTINUE aus Slot 0, im Hangar geladen; in Bild 60 Debug-Sprung in den Hangar (= erneutes Betreten), derselbe
#           Weg nach Sueden wie in Lauf 1.
# Verlangt in Lauf 2:
#   (1) keine Zeile "[hangar] ... Szene Start" (keine zweite Szene), kein Spider-Man (Typ 0x2B) im state.log;
#   (2) Birkin wie beim Wiedereintritt (re15_bf_raumtick: bf_hangar_birkin_setzen + BF_H_FREI): ein Typ 0x30 in seiner KI (st=1) schon
#       im geladenen Raum UND nach dem erneuten Betreten.
# GEGENPROBE (gemessen, scratchpad hangar_rueckbau/BERICHT_r1.md): exe von 23f9da5c9 -> ROT, nach dem Betreten
#   "[hangar] ROOM50A0 Szene Start" (die Szene laeuft nach dem Laden erneut) und im geladenen Raum kein Birkin.
#
# Testhaken (kein Spielverhalten): RE15_SAVE_TEST_AGAIN / RE15_CARD_AUTO / RE15_CARD_SLOT / RE15_SAVE_TEST_EXIT_AFTER (main.c, Form
# test_save_counter_pin.cmake), RE15_CONTINUE_TEST, RE15_DEBUG_JUMP, RE15_INPUT_SCRIPT(_BASIS), RE15_STATE_LOG, RE15_HANGAR_LOG.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_hangar_laden.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_hangar_laden: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_hangar_laden: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41hangarladen_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41hangarladen_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# Binaer-sicheres Lesen (debug.log enthaelt Binaerbytes, file(STRINGS) verschluckt dann still den Rest - test_save_counter_pin.cmake).
function(_hat _datei _text _out)
    set(${_out} 0 PARENT_SCOPE)
    if(NOT EXISTS "${_datei}")
        return()
    endif()
    file(READ "${_datei}" _lh HEX)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    string(FIND "${_lh}" "${_h}" _p)
    if(NOT _p LESS 0)
        set(${_out} 1 PARENT_SCOPE)
    endif()
endfunction()

set(_f "")
set(_weg "W0.5,UX3.6,W0.3,L0.27,W0.3,UX2.5,W12")

function(figur _name _raum _extra)
    string(TOLOWER "${_raum}" _rl)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    set(_g "")

    # --- Lauf 1: Szene, dann Speichern -----------------------------------------------------------------------------------------
    re15_start_spiel(_rv1 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_HANGAR_LOG=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_SET_FLAG=8:42,8:43
        RE15_DEBUG_JUMP=50A0@20
        RE15_PLAYER_POS=-12406,4381,1024
        "RE15_INPUT_SCRIPT=${_weg},UX1.5,W10"
        RE15_STATE_LOG=state1.log
        RE15_SAVE_TEST_AGAIN=390 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_SAVE_TEST_EXIT_AFTER=1
        "${_exe_kopie}")
    if(EXISTS "${WORKDIR}/debug.log")
        file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug1.log")
    endif()
    _hat("${WORKDIR}/debug1.log" "[hangar] ROOM${_raum} Szene Start" _s1)
    _hat("${WORKDIR}/debug1.log" "[hangar] ROOM${_raum} Szene Ende" _e1)
    _hat("${WORKDIR}/debug1.log" "[save] saved (room ${_rl})" _sv)
    if(NOT _s1 OR NOT _e1 OR NOT _sv OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        set(_f "${_f}\n ${_name}: Lauf 1 ohne Szene/Speichern (Start ${_s1}, Ende ${_e1}, gespeichert ${_sv}, exit=${_rv1}, ${WORKDIR})"
            PARENT_SCOPE)
        return()
    endif()

    # --- Lauf 2: neuer Prozess, Laden, Hangar erneut betreten ------------------------------------------------------------------
    re15_start_spiel(_rv2 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_HANGAR_LOG=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_DEBUG_JUMP=50A0@60
        RE15_PLAYER_POS=-12406,4381,1024
        RE15_INPUT_SCRIPT_BASIS=spiel
        "RE15_INPUT_SCRIPT=W3|${_weg}"
        RE15_STATE_LOG=state2.log
        "RE15_EXIT_AT=500#${_raum}!1"
        "${_exe_kopie}")
    _hat("${WORKDIR}/debug.log" "CONTINUE: resumed in room ${_rl}" _geladen)
    _hat("${WORKDIR}/debug.log" "AUTO-JUMP -> ROOM50A0" _sprung)
    _hat("${WORKDIR}/debug.log" "EXIT_AT: Bild 500 in Raum ${_rl}" _ende)
    if(NOT _geladen OR NOT _sprung OR NOT _ende)
        set(_f "${_f}\n ${_name}: Lauf 2 unvollstaendig (geladen ${_geladen}, Sprung ${_sprung}, Endbild ${_ende}, exit=${_rv2}, ${WORKDIR})"
            PARENT_SCOPE)
        return()
    endif()
    # (1) keine zweite Szene
    _hat("${WORKDIR}/debug.log" "Szene Start" _s2)
    if(_s2)
        string(APPEND _g "\n ${_name}: die Hangar-Szene laeuft nach dem Laden erneut ('[hangar] ... Szene Start' in Lauf 2) - Bank-9-Bit 38 nicht im Spielstand/nicht gelesen")
    endif()
    # (1)/(2) state.log: Abschnitt A = geladener Raum, B = nach dem Debug-Sprung (Ruecksprung des Bildzaehlers)
    file(STRINGS "${WORKDIR}/state2.log" _zl REGEX "^F[0-9]+ ")
    set(_abschn 0)
    set(_vor -1)
    set(_birkin_a 0)
    set(_birkin_b 0)
    set(_sm 0)
    foreach(_z IN LISTS _zl)
        if(_z MATCHES "^F([0-9]+) ")
            if(CMAKE_MATCH_1 LESS _vor)
                set(_abschn 1)
            endif()
            set(_vor ${CMAKE_MATCH_1})
        endif()
        if(_z MATCHES " t=2b ")
            set(_sm 1)
        endif()
        if(_z MATCHES " t=30 st=1 ")
            if(_abschn EQUAL 0)
                set(_birkin_a 1)
            else()
                set(_birkin_b 1)
            endif()
        endif()
    endforeach()
    if(NOT _abschn EQUAL 1)
        string(APPEND _g "\n ${_name}: state2.log ohne Raumwechsel (Debug-Sprung nicht ausgefuehrt?)")
    endif()
    if(_sm)
        string(APPEND _g "\n ${_name}: Spider-Man (0x2B) nach dem Laden im Hangar")
    endif()
    if(NOT _birkin_a)
        string(APPEND _g "\n ${_name}: im geladenen Hangar kein Birkin in seiner KI (t=30 st=1) - erwartet wie beim Wiedereintritt")
    endif()
    if(NOT _birkin_b)
        string(APPEND _g "\n ${_name}: nach dem erneuten Betreten kein Birkin in seiner KI (t=30 st=1) - erwartet wie beim Wiedereintritt")
    endif()
    message(STATUS "r41_hangar_laden ${_name}: Lauf 1 exit=${_rv1}, Lauf 2 exit=${_rv2}, Szene nach Laden ${_s2}, Spider-Man ${_sm}, "
                   "Birkin geladen ${_birkin_a} / betreten ${_birkin_b}${_g}")
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

figur(leon 50A0 "")
figur(elza 50A1 "RE15_TITLE_SHOT_CHAR=1")

file(REMOVE "${_exe_kopie}")
if(_f)
    message(FATAL_ERROR "r41_hangar_laden:${_f}")
endif()
message(STATUS "r41_hangar_laden: OK (Leon 50A0 + Elza 50A1, Laufverzeichnis ${_wurzel})")
