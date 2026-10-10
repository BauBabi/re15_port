# =============================================================================
# RUNDE 36 SPUR J MIT DER ECHTEN EXE: die ROOM11B0-Szene (Leon/Ada/Marvin, sub06) samt Erweiterung und danach die
# verschlossene Werkstatt-Tuer. Die Unit-Riegel unit_r36_szenenleon_* messen die Mechanik Bild fuer Bild in der VM;
# dieser Haken prueft, was nur die exe zeigt: Debug-Sprung -> Original-Szene -> Umleitung @0x017EA -> der Schloss-Ton
# laeuft ueber den Se_on-Zweig von audio_pc.c in die RE2-Bank SCHLOSS1010 (shared_assets/RE2) -> Raumwechsel ROOM1260;
# danach die Tuer: Aktionstaste -> Elzas Text-Satz und der RE2-Tuer-zu-Ton (lock_se_common.c).
#
# Laeufe:
#   A  RE15_DEBUG_JUMP=11b0, (3,0x43)/(3,0x82)/(4,0xF3): "[szl-r36] ... Plc_dest Modus 0x13", "Schub-Sub zu Ende",
#      "sub06 @0x017EA Aot_on 5 -> erst die Erweiterung", msg 20, "Se_on bank=0x3A id=10 -> RE2-Schlossbank
#      SCHLOSS1010", msg 21, dann ROOM1260 — in dieser Reihenfolge, und KEIN "[schlossse] ... fehlt"
#   B  (9,84)=1 ((3,0x83)/(4,0xCD) gesetzt), Leon oestlich vor der Tuer mit Blick nach Westen, Aktionstaste:
#      "[szl-r36] Werkstatt-Tuer verschlossen (9,84): Ereignis 23", tuerse.log "raum=11B1 nachricht=12 art=M"
#
# SDL_AUDIODRIVER=dummy: Mischer ohne Geraet (die Bank-Weiche laeuft und loggt mit RE15_SE_DEBUG).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r36_szenenleon.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r36_szenenleon: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r36_szenenleon: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name (Muster test_r34n_b_cursor.cmake): ein parallel bauender Baum darf diesen Lauf nicht treffen.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r36j_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r36j_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

function(_reihenfolge _text _name)
    set(_vor -1)
    foreach(_w IN LISTS ARGN)
        string(FIND "${_text}" "${_w}" _p)
        if(_p LESS 0)
            message(FATAL_ERROR "r36_szenenleon[${_name}]: fehlt im debug.log: '${_w}'")
        endif()
        if(_p LESS_EQUAL _vor)
            message(FATAL_ERROR "r36_szenenleon[${_name}]: falsche Reihenfolge bei '${_w}'")
        endif()
        set(_vor ${_p})
    endforeach()
endfunction()

# --- A: die Szene mit Erweiterung ------------------------------------------------------------------------
set(WORKDIR "${_basis}_a")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rv 400
    RE15_NO_INTRO=1 SDL_AUDIODRIVER=dummy RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_DEBUG_JUMP=11b0@250 "RE15_SET_FLAG=3:0x43,3:0x82,4:0xf3"
    RE15_MSG_LOG=1 RE15_SE_DEBUG=1 "RE15_EXIT_AT=20#1260"
    "${_exe_kopie}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r36_szenenleon[A]: kein debug.log (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
_reihenfolge("${_dbg}" A
    "JUMP -> 11b GARAGE"
    "[szl-r36] ROOM11B0 Spieler: Plc_dest Modus 0x13"
    "[szl-r36] ROOM11B0 Spieler: Schub-Sub zu Ende"
    "[szl-r36] ROOM11B0 sub06 @0x017EA Aot_on 5 -> erst die Erweiterung"
    "[msg] room=11b0 id=20"
    "-> RE2-Schlossbank SCHLOSS1010"
    "[msg] room=11b0 id=21"
    "STAGE1/ROOM1260.RDT")
string(FIND "${_dbg}" "[schlossse]" _p)
if(NOT _p LESS 0)
    message(FATAL_ERROR "r36_szenenleon[A]: SCHLOSS1010 nicht geladen (shared_assets/RE2/SCHLOSS1010.*)")
endif()
message(STATUS "r36_szenenleon[A]: Szene + Erweiterung + Schloss-Ton + Raumwechsel ROOM1260 in Ordnung")

# --- B: die verschlossene Werkstatt-Tuer ------------------------------------------------------------------
set(WORKDIR "${_basis}_b")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rv 200
    RE15_NO_INTRO=1 SDL_AUDIODRIVER=dummy RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_DEBUG_JUMP=11b0@250 "RE15_SET_FLAG=4:0xf3,3:0x82,3:0x83,4:0xcd,9:0x54" "RE15_PLAYER_POS=-20900,-16230,2048"
    "RE15_TUERSE_LOG=${WORKDIR}/tuerse.log" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=300
    "RE15_INPUT_SCRIPT=U1.5,W0.5,A0.2,W2" "RE15_EXIT_AT=480#11b0"
    "${_exe_kopie}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r36_szenenleon[B]: kein debug.log (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
_reihenfolge("${_dbg}" B "JUMP -> 11b GARAGE" "[szl-r36] ROOM11B0 Werkstatt-Tuer verschlossen (9,84): Ereignis 23")
if(NOT EXISTS "${WORKDIR}/tuerse.log")
    message(FATAL_ERROR "r36_szenenleon[B]: kein Tuer-zu-Ton an der Werkstatt-Tuer (tuerse.log fehlt)")
endif()
file(READ "${WORKDIR}/tuerse.log" _ts)
string(FIND "${_ts}" "raum=11B1 nachricht=12 art=M weg=AOT" _p)
if(_p LESS 0)
    message(FATAL_ERROR "r36_szenenleon[B]: Tuer-zu-Ton nicht wie Elzas (11B1 msg 12): ${_ts}")
endif()
message(STATUS "r36_szenenleon[B]: Werkstatt-Tuer verschlossen, Text-Platz + RE2-Ton wie Elzas Tuer")
file(REMOVE "${_exe_kopie}")
