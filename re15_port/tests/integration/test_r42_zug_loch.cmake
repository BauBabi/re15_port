# =============================================================================
# integration_r42_zug_loch_<leon|elza> — Runde 42 Strang "finale", Stufe 2: Loch in Wagen B nach dem Zugstart (include/re15_zug_r42.h).
#
# Echte exe, echter Tuerweg: Debug-Sprung in Wagen A (5090/5091, (3,89)=1, (3,42)=1 und das Zugstart-Bit der Figur), zur Tuer
# Slot 0 (main00 @0x0104E, Ziel Wagen B (24700,-9250) Cut 9 = Tuer in DENSELBEN Raum), dann durch Wagen B nach Westen.
# Geprueft:
#   * beide Raumladungen (Sprung, Tuer) tauschen genau einmal: "Cut_replace(8,10): Zonen nach 8/10 = 0/2, Kamera-AOTs ... = 0/2"
#     - vor dem Fix (re15_rdt_zonen_auffrischen) tauschte die Tuer die Zonen ZURUECK (Messung: "= 2/0", Kamera 8)
#   * die Kamera zeigt im Lochbereich Cut 10 (Zone 9->10 @0x454 statt 9->8), nie Cut 8
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r42_zug_loch: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r42_zug_loch: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL leon)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")
string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}_${TEIL}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "[.]exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r42zl_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r42zl_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
if(TEIL STREQUAL "elza")
    set(_raum 5091)
    set(_bit "9:130")
    set(_extra RE15_PSELECT_AUTO_SWITCH=1)
else()
    set(_raum 5090)
    set(_bit "9:133")
    set(_extra "")
endif()
file(WRITE "${WORKDIR}/plan.txt" "RAUM ${_raum}
WARTE 30
GEHE 300 5000 200
BLICK 2048
AKTION
WARTE 220
GEHE 19500 -9250 200
GEHE 13800 -9250 200
WARTE 30
ENDE
")
re15_start_spiel(_rv 300 RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500
    RE15_PSELECT_AUTO=1 ${_extra} RE15_DEBUG_JUMP=${_raum}@250 "RE15_SET_FLAG=3:89,3:42,${_bit}" "RE15_PLAYER_POS=1500,5000,2048,0"
    "RE15_WEGPLAN=${WORKDIR}/plan.txt" RE15_WEGPLAN_LOG=wegplan.txt RE15_CAM_TRACE=1 "RE15_EXIT_AT=500#${_raum}!1" "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")
set(_f "")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r42_zug_loch[${TEIL}]: kein debug.log (exit=${_rv})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
string(REGEX MATCHALL "Cut_replace[(]8,10[)]: Zonen nach 8/10 = [0-9]+/[0-9]+, Kamera-AOTs nach 8/10 = [0-9]+/[0-9]+" _tausch "${_dbg}")
list(LENGTH _tausch _nt)
if(NOT _nt EQUAL 2)
    string(APPEND _f "\n ${_nt} statt 2 Cut_replace(8,10) (Sprung + Tuer)")
endif()
foreach(_t ${_tausch})
    if(NOT _t MATCHES "= 0/2, Kamera-AOTs nach 8/10 = 0/2")
        string(APPEND _f "\n Tausch nicht 0/2: '${_t}' (Tuer in denselben Raum tauscht zurueck?)")
    endif()
endforeach()
if(NOT _dbg MATCHES "room=${_raum} req[(]cam_id[)]=10 shown[(]s_last[)]=10 wv0A=10 pending=0 auto=1 pl=[(]1[6-9][0-9][0-9][0-9],-9250[)]")
    string(APPEND _f "\n Kamera 10 im Lochbereich (x 16000..19999) nie gezeigt")
endif()
if(_dbg MATCHES "room=${_raum} req[(]cam_id[)]=8 ")
    string(APPEND _f "\n Kamera 8 angefordert (Wagen B ohne Loch)")
endif()
if(_f)
    message(FATAL_ERROR "r42_zug_loch[${TEIL}]:${_f}\n (${WORKDIR})")
endif()
message(STATUS "r42_zug_loch[${TEIL}]: Loch in Wagen B nach Sprung und Tuer, Kamera 10 (${WORKDIR})")
