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
#      Nachbesserung 1 (A2): an der Tuer "Schliessen" < "Cut_chg(2)" < "-> RE2-Tuerbank" < "Abschliessen" <
#      "-> RE2-Schlossbank" mit streng steigenden Bildnummern ([szl-r36] ROOM11B0 F<n> ...)
#   B  (9,84)=1 ((3,0x83)/(4,0xCD) gesetzt), Leon oestlich vor der Tuer mit Blick nach Westen, Aktionstaste:
#      "[szl-r36] Werkstatt-Tuer verschlossen (9,84): Ereignis 23 im Init-Lauf", tuerse.log "raum=11B1 nachricht=12
#      art=M"; state.log: kein Bild mit Cut 8 (offen gemalt), vor der Tuer Cut 2 (geschlossen gemalt)
#   C  wie B von Westen: kein Bild mit Cut 7, vor der Tuer Cut 0
#   D  Spielstand nach der Szene + CONTINUE (RE15_KARTE_TOOL = probe_r36_szenenleon_karte): Boot-/CONTINUE-Weg in
#      main.c, Ereignis 23 im Init-Lauf, kein Bild mit Cut 8; E Gegenprobe ohne (9,84): Cut 8 (offen) wie im Original
#
# RE15_TITLE_SHOT(_AF): Titel-Autovorlauf (Muster test_r30_cut_blitz.cmake). SDL_AUDIODRIVER=dummy: Mischer ohne Geraet (die Bank-Weiche laeuft und loggt mit RE15_SE_DEBUG).
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
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
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
    "[room] PC loaded room1260.rdt")
string(FIND "${_dbg}" "[schlossse]" _p)
if(NOT _p LESS 0)
    message(FATAL_ERROR "r36_szenenleon[A]: SCHLOSS1010 nicht geladen (shared_assets/RE2/SCHLOSS1010.*)")
endif()
# Nachbesserung 1 (Nachtrag abends, M2/M4): an der Tuer ERST schliessen (Clip 20 + Tuerbild 2 + RE2-Tuer-zu-Ton aus der
# Tuerbank), DANN abschliessen — Reihenfolge im Log UND Bildnummern streng steigend.
_reihenfolge("${_dbg}" A2
    "sub06 @0x017EA Aot_on 5 -> erst die Erweiterung"
    "Schliessen: Plc_motion(0,20)"
    "Cut_chg(2)"
    "-> RE2-Tuerbank"
    "Tuer-zu-Ton: Se_on Port-Bank 0x0E Satz 1"
    "Abschliessen: Plc_motion(0,15)"
    "-> RE2-Schlossbank SCHLOSS1010"
    "Abschliess-Ton: Se_on Port-Bank 0x3A Satz 0x0A"
    "[msg] room=11b0 id=21")
set(_vorbild -1)
foreach(_m "Schliessen: Plc_motion" "Tuer-zu-Ton: Se_on" "Abschliessen: Plc_motion" "Abschliess-Ton: Se_on")
    string(REGEX MATCH "\\[szl-r36\\] ROOM11B0 F([0-9]+) \\+0x[0-9A-F]+ ${_m}" _treffer "${_dbg}")
    if(NOT _treffer)
        message(FATAL_ERROR "r36_szenenleon[A2]: keine Bildnummer fuer '${_m}'")
    endif()
    set(_bild "${CMAKE_MATCH_1}")
    if(NOT _bild GREATER _vorbild)
        message(FATAL_ERROR "r36_szenenleon[A2]: '${_m}' in Bild ${_bild} nicht nach Bild ${_vorbild}")
    endif()
    message(STATUS "r36_szenenleon[A2]: F${_bild} ${_m}")
    set(_vorbild ${_bild})
endforeach()
string(FIND "${_dbg}" "Tuerbank DOOR1A nicht geladen" _p)
if(NOT _p LESS 0)
    message(FATAL_ERROR "r36_szenenleon[A2]: Tuerbank DOOR1A nicht geladen")
endif()
message(STATUS "r36_szenenleon[A]: Szene + Erweiterung (Schliessen < Tuer-zu-Ton < Abschliessen < Abschliess-Ton) + Raumwechsel ROOM1260 in Ordnung")

# --- B: die verschlossene Werkstatt-Tuer ------------------------------------------------------------------
set(WORKDIR "${_basis}_b")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rv 200
    RE15_NO_INTRO=1 SDL_AUDIODRIVER=dummy RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=11b0@250 "RE15_SET_FLAG=4:0xf3,3:0x82,3:0x83,4:0xcd,9:0x54" "RE15_PLAYER_POS=-20900,-16230,2048"
    "RE15_TUERSE_LOG=${WORKDIR}/tuerse.log" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=300
    "RE15_INPUT_SCRIPT=U1.5,W0.5,A0.2,W2" "RE15_STATE_LOG=${WORKDIR}/state.log" "RE15_EXIT_AT=480#11b0"
    "${_exe_kopie}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r36_szenenleon[B]: kein debug.log (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
_reihenfolge("${_dbg}" B "JUMP -> 11b GARAGE" "[szl-r36] ROOM11B0 Werkstatt-Tuer verschlossen (9,84): Ereignis 23 im Init-Lauf")
if(NOT EXISTS "${WORKDIR}/tuerse.log")
    message(FATAL_ERROR "r36_szenenleon[B]: kein Tuer-zu-Ton an der Werkstatt-Tuer (tuerse.log fehlt)")
endif()
file(READ "${WORKDIR}/tuerse.log" _ts)
string(FIND "${_ts}" "raum=11B1 nachricht=12 art=M weg=AOT" _p)
if(_p LESS 0)
    message(FATAL_ERROR "r36_szenenleon[B]: Tuer-zu-Ton nicht wie Elzas (11B1 msg 12): ${_ts}")
endif()

# Nachbesserung 1 (M1/M4): das Tuerbild. Die Werkstatt-Tuer ist nur im Hintergrund: Bild 2 (Ost) / 0 (West) geschlossen,
# 8 / 7 offen gemalt (gleiche Kamera, RDT @0x00A0 == @0x0160, @0x0060 == @0x0140). Mit (9,84) darf in keinem Bild ein
# OFFEN-Bild gezeigt werden; vor der Tuer steht Leon im geschlossenen Bild. (Der Vorraum vor dem Sprung zeigt nur 0/1.)
function(_tuerbild _name _state _verboten _soll)
    if(NOT EXISTS "${_state}")
        message(FATAL_ERROR "r36_szenenleon[${_name}]: kein state.log")
    endif()
    file(STRINGS "${_state}" _zeilen)
    list(LENGTH _zeilen _n)
    set(_offen 0)
    foreach(_z IN LISTS _zeilen)
        string(FIND "${_z}" "cam=${_verboten} " _p)
        if(NOT _p LESS 0)
            math(EXPR _offen "${_offen} + 1")
        endif()
    endforeach()
    list(GET _zeilen -1 _letzte)
    string(FIND "${_letzte}" "cam=${_soll} " _p)
    if(_offen GREATER 0 OR _p LESS 0)
        message(FATAL_ERROR "r36_szenenleon[${_name}]: Tuerbild falsch — ${_offen} Bilder mit Cut ${_verboten} (offen), "
                            "letztes Bild: ${_letzte}")
    endif()
    message(STATUS "r36_szenenleon[${_name}]: ${_n} Bilder, keines mit Cut ${_verboten} (offen), vor der Tuer Cut ${_soll} (zu)")
endfunction()
_tuerbild(B "${WORKDIR}/state.log" 8 2)
message(STATUS "r36_szenenleon[B]: Werkstatt-Tuer verschlossen und geschlossen gemalt, Text-Platz + RE2-Ton wie Elzas Tuer")

# --- C: dieselbe Tuer von Westen (Bild 0 statt 7) -----------------------------------------------------------------
set(WORKDIR "${_basis}_c")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rv 200
    RE15_NO_INTRO=1 SDL_AUDIODRIVER=dummy RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=11b0@250 "RE15_SET_FLAG=4:0xf3,3:0x82,3:0x83,4:0xcd,9:0x54" "RE15_PLAYER_POS=-23300,-16300,0"
    "RE15_STATE_LOG=${WORKDIR}/state.log" "RE15_EXIT_AT=120#11b0"
    "${_exe_kopie}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r36_szenenleon[C]: kein debug.log (exit=${_rv}, ${WORKDIR})")
endif()
_tuerbild(C "${WORKDIR}/state.log" 7 0)

# --- D/E: Spielstand + CONTINUE in ROOM11B0 (Boot-/CONTINUE-Weg in main.c, NICHT scd_room_reenter) ----------------
# D: nach der Szene ((9,84)=1) -> Ereignis 23 im Init-Lauf auch auf diesem Weg, nie Cut 8.
# E: Gegenprobe ohne (9,84) -> Original, Leon oestlich der Tuer sieht Cut 8 (offen): beweist, dass die Messung D die
#    offene Tuer erkennen WUERDE.
if(RE15_KARTE_TOOL AND EXISTS "${RE15_KARTE_TOOL}")
    foreach(_fall D E)
        set(WORKDIR "${_basis}_${_fall}")
        file(MAKE_DIRECTORY "${WORKDIR}")
        # gespeicherter Cut = der Cut beim Speichern (CONTINUE setzt ihn nach sub00 wieder, main.c "FE-4 CONTINUE"):
        # nach der Szene Ost Cut 2 (Tuer zu), ohne (9,84) Ost Cut 8 (Tuer offen, Leon sub00 @0x01188)
        if(_fall STREQUAL "D")
            set(_karg ost cut=2)
        else()
            set(_karg ost offen cut=8)
        endif()
        execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${_karg}
                        WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
        if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
            message(FATAL_ERROR "r36_szenenleon[${_fall}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
        endif()
        re15_start_spiel(_rv 180
            RE15_NO_INTRO=1 SDL_AUDIODRIVER=dummy RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
            RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
            "RE15_STATE_LOG=${WORKDIR}/state.log" "RE15_EXIT_AT=150#11b0"
            "${_exe_kopie}")
        if(NOT EXISTS "${WORKDIR}/debug.log")
            message(FATAL_ERROR "r36_szenenleon[${_fall}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
        endif()
        file(READ "${WORKDIR}/debug.log" _dbg)
        string(FIND "${_dbg}" "Werkstatt-Tuer verschlossen (9,84): Ereignis 23 im Init-Lauf" _p23)
        if(_fall STREQUAL "D")
            if(_p23 LESS 0)
                message(FATAL_ERROR "r36_szenenleon[D]: Ereignis 23 lief auf dem CONTINUE-Weg nicht im Init-Lauf")
            endif()
            _tuerbild(D "${WORKDIR}/state.log" 8 2)
        else()
            file(READ "${WORKDIR}/state.log" _st)
            string(FIND "${_st}" "cam=8 " _p8)
            if(NOT _p23 LESS 0 OR _p8 LESS 0)
                message(FATAL_ERROR "r36_szenenleon[E]: Gegenprobe ohne (9,84) zeigt nicht das Original (Cut 8 offen)")
            endif()
            message(STATUS "r36_szenenleon[E]: Gegenprobe ohne (9,84): Cut 8 (offen) wie im Original — die Messung sieht es")
        endif()
    endforeach()
else()
    message(FATAL_ERROR "r36_szenenleon: RE15_KARTE_TOOL fehlt ('${RE15_KARTE_TOOL}')")
endif()
file(REMOVE "${_exe_kopie}")
