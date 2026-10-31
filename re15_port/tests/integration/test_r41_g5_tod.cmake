# =============================================================================
# G5 FRISST DEN SPIELER, GAME OVER, TITEL. Runde 41 Spur R, Abnahme 2.
#
# Vorher lief hier eine Endlosschleife: der Devour schlug bei gesundem Spieler zu (Polaritaet `bgtz` @0x801008e0 / @0x80103e80
# verkehrt), der Spieler stand in Opfer-Modus 4 (re15_player_is_dead() = 0), die Game-Over-FSM startete nie, und das Kauen senkte das
# Gewicht nicht, also fiel auch das Ende-Gate @0x80101d14 nicht. Das Bild kaute ewig, keine Eingabe wirkte.
#
# Original: Biss FUN_800401d4(40, 1) @0x80104118 / @0x80104170 toetet nie, klemmt auf HP 0 @0x800402b0. Devour nur bei HP <= 0, dann
# FUN_800401d4(500, 0): Todes-Latch 0x04000000 @0x800402d8, Flow-Manager @0x800266c8-34 startet das DIE-Overlay mitten im Fressen.
# RE1.5: Game-Over-FSM FUN_8001500c fuer Kommando 3, 6, 7 (@0x80015014-30).
#
# Je ein Lauf Leon (ROOM5090) und Elza (ROOM5091), kein Messhaken: Rakete bis zum G5, danach keine Taste. Geprueft:
#   1  HP faellt ueber die Bisse auf 0, der Tod kommt erst danach (state.log)
#   2  finisher.log: FSM-START, BLACKBG, YOUDIED, mit Latch 0 (Latch der Gator-Schiene) bei isdead 0 = G5-Latch traegt die FSM
#   3  Titel: zweiter Spiel-Boot erreicht (RE15_BOOT_EXIT_AT=2, debug.log/stderr "[flow] BOOT_EXIT_AT")
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_g5_tod.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_g5_tod: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_g5_tod: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41g5tod_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41g5tod_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_f "")
function(ein_lauf _name _raum)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_extra "")
    if(_raum STREQUAL "5091")
        set(_extra "RE15_TITLE_SHOT_CHAR=1")
    endif()
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=500,-9175,2048,0"
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=A0.1,W1,U8.5,MA7.2,W260"
        RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log RE15_GIVE=18:200 RE15_EQUIP=18
        RE15_FINISHER_LOG=1 RE15_BOOT_EXIT_AT=2 "RE15_EXIT_AT=7000#${_raum}"
        "${_exe_kopie}")
    set(_g "")
    if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.log")
        set(_f "${_f}\n ${_name}: kein Protokoll (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    if(NOT EXISTS "${WORKDIR}/finisher.log")
        string(APPEND _g "\n ${_name}: finisher.log fehlt, Game-Over-FSM nie gestartet (Endlosschleife im Fressen)")
    else()
        file(READ "${WORKDIR}/finisher.log" _fin)
        foreach(_mark FSM-START BLACKBG YOUDIED)
            if(NOT _fin MATCHES "${_mark}")
                string(APPEND _g "\n ${_name}: ${_mark} fehlt in finisher.log")
            endif()
        endforeach()
        if(NOT _fin MATCHES "FSM-START[^\n]*isdead=0")
            string(APPEND _g "\n ${_name}: FSM startete nicht im Fressen (isdead != 0 bei FSM-START)")
        endif()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    if(NOT _dbg MATCHES "BOOT_EXIT_AT: boot #2")
        string(APPEND _g "\n ${_name}: Titel nicht erreicht (kein zweiter Spiel-Boot, exit=${_rv})")
    endif()
    # HP-Verlauf: vor dem Tod muss HP 0 stehen, kein Sprung von > 0 nach < 0 (Biss a1 = 1 @0x80104170 toetet nie)
    file(STRINGS "${WORKDIR}/state.log" _zeilen REGEX "PL\\(")
    set(_vor 100)
    set(_null 0)
    set(_tod 0)
    set(_tod_bei 0)
    foreach(_z IN LISTS _zeilen)
        if(_z MATCHES "PL\\(-?[0-9]+,-?[0-9]+,rot=-?[0-9]+,hp=(-?[0-9]+)\\)")
            set(_hp "${CMAKE_MATCH_1}")
            if(_hp EQUAL 0)
                set(_null 1)
            endif()
            if(_hp LESS 0 AND NOT _tod)
                set(_tod 1)
                set(_tod_bei "${_vor}")
            endif()
            set(_vor "${_hp}")
        endif()
    endforeach()
    if(NOT _tod)
        string(APPEND _g "\n ${_name}: Spieler stirbt nicht")
    elseif(NOT _null OR NOT _tod_bei EQUAL 0)
        string(APPEND _g "\n ${_name}: Tod bei HP ${_tod_bei} statt 0 (Devour verlangt HP <= 0, `bgtz` @0x80103e80)")
    endif()
    message(STATUS "r41_g5_tod ${_name}: HP 0 gesehen ${_null}, Tod bei HP ${_tod_bei}, exit ${_rv}")
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

ein_lauf(leon 5090)
ein_lauf(elza 5091)
file(REMOVE "${_exe_kopie}")

if(_f)
    message(FATAL_ERROR "r41_g5_tod: FEHLER${_f}")
endif()
message(STATUS "r41_g5_tod: alle Pruefungen gruen")
