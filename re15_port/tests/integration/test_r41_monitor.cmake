# Runde 41 Spur Monitor - echte exe (kein Messhaken ausser Sprung/Lage/Skript): die Hangar-Szene als Monitorbild im Monitor Room.
# Nutzer 2026-11-15: "diese Szene wo der Birkin den Spiderman schmeisst, ist 100%ig kein Boss Kampf ... Leon oder Elza sehen Birkin
# auf dem Monitor". Code engine/src/monitor_r41.c, Raumprobe tests/unit/test_r41_monitor.c.
#
#   L  Leon, ROOM5030, Eintrittslage aus 5000 (Door ROOM5000 slot 0 @0x007FE: (-13846,-5474) Gier 798):
#      genau ein Monitorbild (Start -> Hangar 50A0 mit Szene bis h134 -> zurueck), der Spieler steht danach an derselben Stelle
#      mit derselben Gier, Pad-Sperre aus, und er laeuft auf Eingabe los (Steuerung frei) - (c)(e).
#   E  Elza, ROOM5031: dasselbe, danach laeuft Elza in die Sherry-Zone (AOT Slot 6, sub00 @0x010E8) und das Original-Ereignis
#      sub03 laeuft: Monitorbild Cut 11 (Effekt (1,0x1E) @0x011A8) und "Elza: Sherry!" (msg 3 @0x011EC) - (d).
#   G  Gegenprobe: Lauf L mit RE15_KEIN_MONITORBILD=1 - die Pruefung von L MUSS rot sein.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_monitor.cmake
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_monitor: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_monitor: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")
string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

function(lauf _name _dir_out)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
        RE15_STATE_LOG=state.log RE15_HANGAR_LOG=1 RE15_INPUT_SCRIPT_BASIS=spiel
        ${ARGN}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r41_monitor[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    set(${_dir_out} "${WORKDIR}" PARENT_SCOPE)
endfunction()

# Pruefung eines Laufs: _mon = 5030/5031, _hg = 50A0/50A1; _fehler_out = leer, wenn alles stimmt.
function(pruefe _dir _mon _hg _x _z _gier _fehler_out)
    set(_f "")
    file(STRINGS "${_dir}/debug.log" _start REGEX "^\\[monitor\\] ROOM${_mon} Monitor Start")
    list(LENGTH _start _n_start)
    if(NOT _n_start EQUAL 1)
        string(APPEND _f "\n  ${_mon}: ${_n_start} Monitor-Starts (Soll genau 1)")
    endif()
    file(STRINGS "${_dir}/debug.log" _sz REGEX "^\\[hangar\\] ROOM${_hg} Szene Start")
    list(LENGTH _sz _n_sz)
    if(NOT _n_sz EQUAL 1)
        string(APPEND _f "\n  ${_hg}: Szene ${_n_sz}-mal gestartet (Soll 1)")
    endif()
    file(STRINGS "${_dir}/debug.log" _h134 REGEX "^\\[hangar-takt\\] h134 F")
    if(NOT _h134)
        string(APPEND _f "\n  ${_hg}: Szene nicht bis h134 gespielt")
    endif()
    file(STRINGS "${_dir}/debug.log" _bild REGEX "^\\[monitor\\] ROOM${_hg} Hangar geladen: .*Effekt 1")
    if(NOT _bild)
        string(APPEND _f "\n  ${_hg}: kein Monitorbild mit Effekt (1,0x1E)")
    endif()
    set(_soll "Spieler (${_x},0,${_z}) Gier ${_gier} hp 100 Zustand 1")
    file(STRINGS "${_dir}/debug.log" _ende REGEX "^\\[monitor\\] ROOM${_mon} Monitor Ende: Phase 0, ")
    if(NOT _ende)
        string(APPEND _f "\n  ${_mon}: kein Monitor-Ende")
    else()
        list(GET _ende 0 _e)
        string(FIND "${_e}" "${_soll}" _p)
        if(_p LESS 0)
            string(APPEND _f "\n  ${_mon} (e): Stelle nach dem Monitorbild '${_e}' (Soll '${_soll}')")
        endif()
        if(NOT _e MATCHES "Zonen 1, Pad-Sperre 0, Effekt 0")
            string(APPEND _f "\n  ${_mon} (e): Ende nicht frei: '${_e}'")
        endif()
    endif()
    # Steuerung frei: nach dem Ende laeuft der Spieler mit der Skript-Eingabe von der Stelle weg (state.log, letzte Zeile)
    file(STRINGS "${_dir}/state.log" _letzte REGEX "^F[0-9]+ pad=")
    list(LENGTH _letzte _nl)
    if(_nl GREATER 0)
        math(EXPR _il "${_nl} - 1")
        list(GET _letzte ${_il} _z1)
        if(_z1 MATCHES "PL\\(${_x},${_z},")
            string(APPEND _f "\n  ${_mon} (e): Spieler bewegt sich nach dem Monitorbild nicht ('${_z1}')")
        endif()
    endif()
    set(${_fehler_out} "${_f}" PARENT_SCOPE)
endfunction()

# L - Leon
lauf(l _dl RE15_DEBUG_JUMP=5030@20 RE15_PLAYER_POS=-13846,-5474,798 "RE15_INPUT_SCRIPT=W1|W1|W1|W4,U1.5,W4" "RE15_EXIT_AT=330#5030!3")
pruefe("${_dl}" 5030 50A0 -13846 -5474 798 _fl)
# E - Elza (Sherry-Ereignis danach)
lauf(e _de RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=5030@20 RE15_PLAYER_POS=-13846,-5474,1900 "RE15_INPUT_SCRIPT=W1|W1|W1|W3,U4,W20"
     "RE15_EXIT_AT=700#5031!3")
pruefe("${_de}" 5031 50A1 -13846 -5474 1900 _fe)
file(STRINGS "${_de}/state.log" _sherry REGEX "msg\\(a=1 fsm=1 id=3 ")
file(STRINGS "${_de}/state.log" _cut11 REGEX " cam=11 ")
if(NOT _sherry OR NOT _cut11)
    string(APPEND _fe "\n  5031 (d): Sherry-Ereignis nach dem Monitorbild nicht gelaufen (msg 3 @0x011EC / Cut 11 @0x011A6 fehlen)")
endif()
# G - Gegenprobe ohne Monitorbild: die Pruefung von L muss rot sein
lauf(g _dg RE15_KEIN_MONITORBILD=1 RE15_DEBUG_JUMP=5030@20 RE15_PLAYER_POS=-13846,-5474,798 "RE15_INPUT_SCRIPT=W1|W4,U1.5,W4"
     "RE15_EXIT_AT=200#5030")
pruefe("${_dg}" 5030 50A0 -13846 -5474 798 _fg)
set(_fehler "")
if(_fl)
    string(APPEND _fehler "\n Leon:${_fl}")
endif()
if(_fe)
    string(APPEND _fehler "\n Elza:${_fe}")
endif()
if(NOT _fg)
    string(APPEND _fehler "\n Gegenprobe ohne Monitorbild war GRUEN - die Pruefung sieht das Monitorbild nicht")
else()
    message(STATUS "r41_monitor: Gegenprobe ohne Monitorbild rot wie verlangt:${_fg}")
endif()
if(_fehler)
    message(FATAL_ERROR "r41_monitor:${_fehler}\n Laeufe ${_basis}_*")
endif()
message(STATUS "r41_monitor: OK - Leon 5030 und Elza 5031 je ein Monitorbild, Stelle gleich, Steuerung frei, Sherry-Ereignis danach (${_basis}_*)")
