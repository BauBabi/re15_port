cmake_minimum_required(VERSION 3.16)
# =============================================================================
# RUNDE 37 SPUR L (re2zombie) — NUTZER-BEISPIEL MIT DER ECHTEN EXE: die Zombies im Gang nach Irons' Tod heben wie in RE2 die
# Arme (Arme-hoch-Gang +0x4 = 0x201, EXEC[2] @0x80102260) und greifen aus diesem Gang.
#
# Dossier analysis/befunde_runde37/L_re2zombie.md. Weg wie integration_r37_zombieaktiv (Spur H): Spielstand (Karte aus
# probe_r36_gegnerzahl_karte) in ROOM1150 vor der Tuer Slot 0 @0x00D5E nach ROOM1130, Montage gesehen + CONTINUE,
# Aktionstaste -> Tuer -> ROOM1130 (Nachspawn Programm 10), Leon steht 1100 Bilder.
#   - state.log (letzter Raumbesuch): mindestens ein Zombie wechselt aus dem Gang (ss1=1) in den Arme-hoch-Gang (ss1=2)
#     bei +0x1F0 < 0x7D0 (Produzent (2) @0x80101EC4-F10; RE2-Original gemessen 1999/1987) und
#   - aus dem Arme-hoch-Gang wird gegriffen (ss1=2 -> ss1=3; RE2-Original: Griff aus 0x201 bei 1180/1152).
# Vorher (Stand bis Runde 37): kein Zombie erreicht ss1=2 vor Leon (unit_r37_re2zombie_raum1130 Gegenprobe Maske 0).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_gegnerzahl_karte> -DWORKDIR=<dir> -P test_r37_re2zombie.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_re2zombie: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r37_re2zombie: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte "${_exe_dir}/re15_pc_r37l_haken_*")
foreach(_k IN LISTS _alte)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_ext "${RE15_PC_EXE}" EXT)
set(_exe "${_exe_dir}/re15_pc_r37l_haken_${_lauf_id}${_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "1150" "-17150" "-11960" "0"
                        "9:71" "3:94" "9:73-76" "4:4-5" "4:15" "7:0xd3-0xd7" "7:0xc6-0xca"
                WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
    message(FATAL_ERROR "r37_re2zombie: Kartenwerkzeug exit=${_rvk}\n${_outk}")
endif()
re15_start_spiel(_rv 900
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=W1,A0.2,W2"
    RE15_STATE_LOG=state.log
    "RE15_EXIT_AT=1100#1130"
    "${_exe}")
file(REMOVE "${_exe}")
if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.log")
    message(FATAL_ERROR "r37_re2zombie: kein debug.log/state.log (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
foreach(_m "CONTINUE: resumed in room 1150" "Nachspawn 1130 (Programm 10)" "EXIT_AT: Bild 1100 in Raum 1130")
    string(FIND "${_dbg}" "${_m}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r37_re2zombie: fehlt im debug.log: '${_m}' (${WORKDIR})")
    endif()
endforeach()

# Zeilen des LETZTEN Raumbesuchs (ROOM1130): ab der letzten Zeile "F1 ".
file(STRINGS "${WORKDIR}/state.log" _zeilen REGEX "^F[0-9]+ ")
set(_ab 0)
set(_i 0)
foreach(_z IN LISTS _zeilen)
    if(_z MATCHES "^F1 ")
        set(_ab ${_i})
    endif()
    math(EXPR _i "${_i} + 1")
endforeach()
set(_rx "\\[([0-9]+) t=([0-9a-f]+) st=([0-9]+) ss1=([0-9]+) ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=[0-9]+ af=[0-9]+ stun=-?[0-9]+ d=([0-9]+) @")
set(_wechsel 0)
set(_wechsel_nah 0)
set(_griff_aus_201 0)
set(_d_liste "")
set(_i 0)
foreach(_z IN LISTS _zeilen)
    if(_i GREATER_EQUAL _ab)
        string(REPLACE ";" "," _z "${_z}")
        string(REGEX MATCHALL "${_rx}" _es "${_z}")
        foreach(_e IN LISTS _es)
            if(_e MATCHES "${_rx}")
                set(_s ${CMAKE_MATCH_1})
                set(_st ${CMAKE_MATCH_3})
                set(_s1 ${CMAKE_MATCH_4})
                set(_d ${CMAKE_MATCH_5})
                if(DEFINED _vor_${_s})
                    if(_st EQUAL 1 AND _s1 EQUAL 2 AND NOT _vor_${_s} STREQUAL "1/2")
                        math(EXPR _wechsel "${_wechsel} + 1")
                        list(APPEND _d_liste "${_d}")
                        if(_d LESS 2000 AND _vor_${_s} STREQUAL "1/1")
                            math(EXPR _wechsel_nah "${_wechsel_nah} + 1")
                        endif()
                    endif()
                    if(_st EQUAL 1 AND _s1 EQUAL 3 AND _vor_${_s} STREQUAL "1/2")
                        math(EXPR _griff_aus_201 "${_griff_aus_201} + 1")
                    endif()
                endif()
                set(_vor_${_s} "${_st}/${_s1}")
            endif()
        endforeach()
    endif()
    math(EXPR _i "${_i} + 1")
endforeach()
message(STATUS "r37_re2zombie: ROOM1130 Wechsel in den Arme-hoch-Gang ${_wechsel} (davon aus dem Gang bei +0x1F0 < 0x7D0: "
               "${_wechsel_nah}; Abstaende ${_d_liste}), Griffe aus dem Arme-hoch-Gang ${_griff_aus_201}")
if(_wechsel_nah LESS 1)
    message(FATAL_ERROR "r37_re2zombie: kein Zombie wechselt vor Leon (+0x1F0 < 0x7D0) in den Arme-hoch-Gang (Produzent (2) "
                        "@0x80101EC4-F10) — ${WORKDIR}")
endif()
if(_griff_aus_201 LESS 1)
    message(FATAL_ERROR "r37_re2zombie: kein Griff aus dem Arme-hoch-Gang (ss1 2 -> 3; DECISION[2] Block G @0x80102114-1D0) — ${WORKDIR}")
endif()
message(STATUS "r37_re2zombie: OK")
