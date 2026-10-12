cmake_minimum_required(VERSION 3.16)
# =============================================================================
# RUNDE 37 SPUR H (zombieaktiv) — NUTZER-BEISPIEL MIT DER ECHTEN EXE.
#
# Dossier analysis/befunde_runde37/H_zombieaktiv.md. Weg: Spielstand (Karte aus probe_r36_gegnerzahl_karte) in ROOM1150
# vor der Tuer Slot 0 @0x00D5E nach ROOM1130, Montage nach Irons' Tod gesehen ((9,71) (3,94) (9,73)-(9,76), Rolltor/Tor
# offen, umgezogene Records tot) + CONTINUE, Aktionstaste -> Tuer -> ROOM1130 -> Nachspawn 1130 (Programm 10). Leon steht.
#   - debug.log: "Nachspawn 1130 (Programm 10)"
#   - state.log, Besuch ROOM1130: Bild 7 = der zweite KI-Takt nach der Blende (die KI ruht bis Bild 6, Pause-Wort
#     0xFF000000 @0x8001cc5c-6c): alle fuenf Montage-Zombies in Zustand 1 / Unterzustand 1 = GANG (RE2-Original: Takt 2
#     nach dem Spawn, Deskriptor 6 @0x80100ab8-abc)
#   - Bild 306 (300 Bilder spaeter): jeder >= 300 x 15 = 4500 naeher an Leon (RE2-Original kleinste Rate 15,4 je Takt)
# Vorher (v0.8.24, Dossier M1): in 1800 Bildern erreicht KEINER den Gang.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_gegnerzahl_karte> -DWORKDIR=<dir> -P test_r37_zombieaktiv.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_zombieaktiv: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r37_zombieaktiv: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte "${_exe_dir}/re15_pc_r37h_haken_*")
foreach(_k IN LISTS _alte)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_ext "${RE15_PC_EXE}" EXT)
set(_exe "${_exe_dir}/re15_pc_r37h_haken_${_lauf_id}${_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "1150" "-17150" "-11960" "0"
                        "9:71" "3:94" "9:73-76" "4:4-5" "4:15" "7:0xd3-0xd7" "7:0xc6-0xca"
                WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
    message(FATAL_ERROR "r37_zombieaktiv: Kartenwerkzeug exit=${_rvk}\n${_outk}")
endif()
re15_start_spiel(_rv 600
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=W1,A0.2,W2"
    RE15_STATE_LOG=state.log
    "RE15_EXIT_AT=320#1130"
    "${_exe}")
file(REMOVE "${_exe}")
if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.log")
    message(FATAL_ERROR "r37_zombieaktiv: kein debug.log/state.log (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
foreach(_m "CONTINUE: resumed in room 1150" "Nachspawn 1130 (Programm 10)" "EXIT_AT: Bild 320 in Raum 1130")
    string(FIND "${_dbg}" "${_m}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r37_zombieaktiv: fehlt im debug.log: '${_m}' (${WORKDIR})")
    endif()
endforeach()

# die Zeilen F7 und F306 des LETZTEN Raumbesuchs (ROOM1130; der 1150-Teil endet vor Bild 100)
file(STRINGS "${WORKDIR}/state.log" _z7 REGEX "^F7 ")
file(STRINGS "${WORKDIR}/state.log" _z306 REGEX "^F306 ")
list(LENGTH _z7 _n7)
list(LENGTH _z306 _n306)
if(_n7 LESS 1 OR _n306 LESS 1)
    message(FATAL_ERROR "r37_zombieaktiv: state.log ohne Bild 7/306 (${_n7}/${_n306})")
endif()
list(GET _z7 -1 _l7)
list(GET _z306 -1 _l306)
# CMake-Listen: ein '[' ohne ']' in einem Element verhindert das Trennen an ';' (Klammer-Semantik) -> ersetzen
string(REPLACE "[" "<" _l7 "${_l7}")
string(REPLACE "]" ">" _l7 "${_l7}")
string(REPLACE "[" "<" _l306 "${_l306}")
string(REPLACE "]" ">" _l306 "${_l306}")
set(_rx "<([0-9]+) t=([0-9a-f]+) st=([0-9]+) ss1=([0-9]+) ss2=[0-9]+ ss3=[0-9]+ g=([0-9a-f]+) mo=[0-9]+ af=[0-9]+ stun=-?[0-9]+ d=([0-9]+) @")
string(REGEX MATCHALL "${_rx}" _e7 "${_l7}")
string(REGEX MATCHALL "${_rx}" _e306 "${_l306}")
set(_n 0)
set(_gang 0)
set(_nah 0)
foreach(_e IN LISTS _e7)
    string(REGEX MATCH "${_rx}" _x "${_e}")
    set(_slot ${CMAKE_MATCH_1})
    set(_typ ${CMAKE_MATCH_2})
    set(_st ${CMAKE_MATCH_3})
    set(_ss ${CMAKE_MATCH_4})
    set(_g ${CMAKE_MATCH_5})
    set(_d7 ${CMAKE_MATCH_6})
    if(NOT _typ MATCHES "^(10|11|16)$")
        continue()
    endif()
    math(EXPR _n "${_n} + 1")
    if(_st EQUAL 1 AND _ss EQUAL 1)
        math(EXPR _gang "${_gang} + 1")
    endif()
    set(_d306 -1)
    foreach(_f IN LISTS _e306)
        string(REGEX MATCH "${_rx}" _y "${_f}")
        if(CMAKE_MATCH_1 EQUAL _slot)
            set(_d306 ${CMAKE_MATCH_6})
        endif()
    endforeach()
    math(EXPR _weg "${_d7} - ${_d306}")
    message(STATUS "r37_zombieaktiv: Slot ${_slot} Typ ${_typ} grid ${_g}: Bild 7 Zustand ${_st}/${_ss} d=${_d7}, Bild 306 d=${_d306} (${_weg} naeher)")
    if(_d306 GREATER_EQUAL 0 AND _weg GREATER_EQUAL 4500)
        math(EXPR _nah "${_nah} + 1")
    endif()
endforeach()
if(NOT _n EQUAL 5)
    message(FATAL_ERROR "r37_zombieaktiv: ${_n} statt 5 Montage-Zombies in ROOM1130 (Bild 7: ${_l7})")
endif()
if(NOT _gang EQUAL 5)
    message(FATAL_ERROR "r37_zombieaktiv: nur ${_gang} von 5 in Bild 7 im Gang (RE2: Deskriptor 6 = Gang ab Takt 2)")
endif()
if(NOT _nah EQUAL 5)
    message(FATAL_ERROR "r37_zombieaktiv: nur ${_nah} von 5 nach 300 Bildern >= 4500 naeher an Leon")
endif()
message(STATUS "r37_zombieaktiv: OK — 5/5 im Gang ab Bild 7, 5/5 >= 4500 naeher nach 300 Bildern (${WORKDIR})")
