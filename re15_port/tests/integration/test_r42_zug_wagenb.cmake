# =============================================================================
# integration_r42_zug_wagenb_<leon|elza> — Runde 42 Strang "finale", Stufe 4: Wagen B nach dem Zugstart OHNE Loch.
#
# Stufe 2 hatte beim Raumaufbau nach dem Zugstart Cut_replace(8,10) gesetzt (ROOM509 Bild 10, Boden aufgerissen) - man lief ueber das
# Loch hinweg. NUTZER 18.11.2026: "Aber natuerlich soll man nicht einfach ueber das Loch laufen koennen." und "Finden wir keinen Weg,
# muss es halt raus...". Gemessen (Bericht s4 r1): ein Loch ab dem Anfahren laesst keinen Weg nach Wagen C - neben dem Loch
# (x 16300..18500, z -6300..-11900) bleiben 580 bzw. 830 bis zu den Waenden SCA #11 @0x8FC / #12 @0x908, der Spieler braucht 900
# (PR 450, re15_collision.c), und bei x 17975..18500 lassen die Baenke SCA #17 @0x944 / #18 @0x950 nur z -10925..-7575 frei, ganz im
# Loch; Wagen B ist der einzige Weg A -> C (main00 @0x0104E / @0x0108E). Darum ist das Loch wieder entfernt.
#
# Echte exe, echter Tuerweg wie bisher: Debug-Sprung in Wagen A (5090/5091, (3,89)=1, (3,42)=1 und das Zugstart-Bit der Figur), zur
# Tuer Slot 0 (main00 @0x0104E, Ziel Wagen B (24700,-9250) Cut 9 = Tuer in DENSELBEN Raum), dann durch Wagen B nach Westen.
# Geprueft:
#   * kein Cut_replace(8,10) (Messzeile in op_cut_replace) - weder beim Sprung noch an der Tuer
#   * im frueheren Lochbereich (x 16000..19999) zeigt die Kamera Bild 8 (Zone 9->8 @0x454), nie Bild 10
#   * der Weg nach Westen ist frei: die Figur erreicht x <= 14500 (Zone 8->7 @0x418, Kamera 7)
# Gegenprobe: exe 1789c5e5e (Stufe 3, mit Loch) -> rot (2 x Cut_replace, Kamera 10).
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r42_zug_wagenb: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r42_zug_wagenb: WORKDIR fehlt")
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
    set(_exe_kopie "${_exe_dir}/re15_pc_r42zb_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r42zb_${_lauf_id}")
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
    message(FATAL_ERROR "r42_zug_wagenb[${TEIL}]: kein debug.log (exit=${_rv})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
if(NOT _dbg MATCHES "DOOR FIRE slot=0 [^\n]*target_cut=9")
    string(APPEND _f "\n Tuer Wagen A -> B (Slot 0, Cut 9) nicht benutzt - der Lauf prueft Wagen B nicht")
endif()
string(REGEX MATCHALL "Cut_replace[(]8,10[)]" _tausch "${_dbg}")
list(LENGTH _tausch _nt)
if(NOT _nt EQUAL 0)
    string(APPEND _f "\n ${_nt} x Cut_replace(8,10) - das Loch (Bild 10) ist noch da")
endif()
if(_dbg MATCHES "room=${_raum} req[(]cam_id[)]=10 ")
    string(APPEND _f "\n Kamera 10 (Boden aufgerissen) angefordert")
endif()
if(NOT _dbg MATCHES "room=${_raum} req[(]cam_id[)]=8 [^\n]*pl=[(]1[6-9][0-9][0-9][0-9],-9250[)]")
    string(APPEND _f "\n Kamera 8 im frueheren Lochbereich (x 16000..19999) nie gezeigt")
endif()
if(NOT _dbg MATCHES "room=${_raum} req[(]cam_id[)]=7 [^\n]*pl=[(]1[0-4][0-9][0-9][0-9],-9250[)]")
    string(APPEND _f "\n Kamera 7 westlich des frueheren Lochs (x <= 14999) nie erreicht - Weg nach Wagen C versperrt?")
endif()
if(_f)
    message(FATAL_ERROR "r42_zug_wagenb[${TEIL}]:${_f}\n (${WORKDIR})")
endif()
message(STATUS "r42_zug_wagenb[${TEIL}]: Wagen B nach dem Zugstart Bild 8, kein Loch, Weg nach Westen frei (${WORKDIR})")
