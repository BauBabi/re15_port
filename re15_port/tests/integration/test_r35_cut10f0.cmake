# =============================================================================
# SZENE ROOM10F0 MIT DER ECHTEN EXE (Runde 35, Spur K): der Abnahmeweg des Auftrags — Spielstand +
# CONTINUE in ROOM10D0 vor der Tuer zum Communication Room, Aktionstaste, echter Eintritt durch die
# Tuer, Szene laeuft im Zielraum GENAU EINMAL (zweiter Lauf mit gesetztem Flag (9,71): keine Szene).
#
# Dossier analysis/befunde_runde35/K_cut10f0.md, Konstanten include/re15_cut10f0.h. Die Unit-Riegel
# unit_r35_cut10f0_* pruefen die Mechanik Bild fuer Bild in der VM; dieser Riegel prueft, was nur die
# exe zeigt: Lade-Weg, Tuersequenz -> Raumwechsel, Gestenblock-Leihe (main.c), Port-Bank-Tuerknall,
# Kartenhinweis-Kette ueber den Statusschirm (menu_common.c) und das Szenen-Ende mit MAIN01-Anstoss.
#
# Laeufe (Karte aus probe_r35_cut10f0_karte, Spieler (1900,0,-7000) Gierung 2048 vor der Tuer, CONTINUE,
# Eingabeskript auf der Spielbild-Zeitachse ab Bild 60):
#   A  ROOM10D0 -> Tuer -> ROOM10F0: "[cut10f0] ROOM10F0: Szene gestartet", Animationsblock von ROOM11B0
#      geliehen, Nachrichten 6..23 in dieser Reihenfolge, 2x Se_on Bank 14 (0x0E) Satz 1, "Szene zu Ende",
#      Hinweis 1 -> 2 (Folge) -> schliessen, danach Cut 2 (Leon im Bild)
#   B  wie A mit (9,71)=1 im Spielstand: kein "[cut10f0]", keine Nachricht 6..23, kein geliehener Block
#
# RE15_SOFTWARE_RENDER=1 dient nur der Robustheit des Testhakens; geprueft wird das LOG (debug.log).
# SDL_AUDIODRIVER=dummy, damit die BGM-Weiche auch ohne Audio-Endpunkt laeuft und "[bgm] ... entry=FF01"
# geschrieben wird (ohne Geraet kehrt re15_audio_start_room_bgm vorher zurueck).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r35_cut10f0_karte> -DWORKDIR=<dir>
#               -P test_r35_cut10f0.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r35_cut10f0: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r35_cut10f0: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r35_cut10f0: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name (Muster test_r34n_b_cursor.cmake): local_build.sh eines parallel bauenden Baums darf
# diesen Lauf nicht treffen.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r35k_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r35k_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(szene_lauf _name _karte _raum _ende _out_debug)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${_karte}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r35_cut10f0[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    #  F60 Aktionstaste an der Tuer (Tuersequenz, Raumwechsel), danach nur warten
    re15_start_spiel(_rv 600
        RE15_NO_INTRO=1 SDL_AUDIODRIVER=dummy RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=A0.2,W5"
        RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_CAM_TRACE=1
        "RE15_EXIT_AT=${_ende}#10f0"
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r35_cut10f0[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(FIND "${_dbg}" "CONTINUE: resumed in room ${_raum}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r35_cut10f0[${_name}]: Spielstand in ROOM${_raum} nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r35_cut10f0[${_name}]: Endbild in ROOM10F0 nicht erreicht - Tuer nicht genommen oder "
                            "Lauf abgerissen (exit=${_rv}, ${WORKDIR})")
    endif()
    set(${_out_debug} "${_dbg}" PARENT_SCOPE)
endfunction()

function(_pos _text _wort _out)
    string(FIND "${_text}" "${_wort}" _p)
    set(${_out} ${_p} PARENT_SCOPE)
endfunction()

# --- A: erster Eintritt -> Szene --------------------------------------------------------------------
szene_lauf(a "" 10d0 2700 _dbg)
_pos("${_dbg}" "[cut10f0] ROOM10F0: Szene gestartet" _ps)
if(_ps LESS 0)
    message(FATAL_ERROR "r35_cut10f0[A]: Szene nach dem Tuereintritt nicht gestartet")
endif()
_pos("${_dbg}" "Animationsblock von ROOM11B0 geliehen" _pr)
if(_pr LESS 0)
    message(FATAL_ERROR "r35_cut10f0[A]: Gestenblock von ROOM11B0 nicht geliehen (main.c-Haken)")
endif()
# Nachrichten 6..23 in dieser Reihenfolge
set(_vorher -1)
foreach(_id RANGE 6 23)
    _pos("${_dbg}" "[msg] room=10f0 id=${_id} " _pm)
    if(_pm LESS 0)
        message(FATAL_ERROR "r35_cut10f0[A]: Nachricht ${_id} nie angezeigt")
    endif()
    if(NOT _pm GREATER _vorher)
        message(FATAL_ERROR "r35_cut10f0[A]: Nachricht ${_id} vor ihrer Vorgaengerin")
    endif()
    set(_vorher ${_pm})
endforeach()
string(REGEX MATCHALL "\\[se\\] SCD Se_on: bank=14 id=1" _knall "${_dbg}")
list(LENGTH _knall _nk)
if(NOT _nk EQUAL 2)
    message(FATAL_ERROR "r35_cut10f0[A]: Tuerknall (Se_on Bank 0x0E Satz 1) ${_nk}x statt 2x")
endif()
_pos("${_dbg}" "[cut10f0] Szene zu Ende" _pe)
if(_pe LESS 0)
    message(FATAL_ERROR "r35_cut10f0[A]: Szene nicht zu Ende gelaufen (Faden haengt)")
endif()
_pos("${_dbg}" "Folge-Hinweis 1 -> 2" _pf)
_pos("${_dbg}" "[hint] F" _ph)
if(_pf LESS 0 OR _ph LESS 0)
    message(FATAL_ERROR "r35_cut10f0[A]: Kartenhinweis-Kette ROOM11C0 -> ROOM1150 nicht gelaufen")
endif()
string(REGEX MATCH "\\[hint\\] F[0-9]+ schliessen" _hs "${_dbg}")
if(NOT _hs)
    message(FATAL_ERROR "r35_cut10f0[A]: Hinweis-Schirm hat sich nicht geschlossen")
endif()
# Audio: die BGM-Weiche liefert nach der Szene MAIN01 (0xFF01) — mit dem Dummy-Treiber geloggt
_pos("${_dbg}" "entry=FF01 -> MAIN01" _pb)
if(_pb LESS 0)
    message(FATAL_ERROR "r35_cut10f0[A]: nach der Szene kein BGM-Start mit entry=FF01 (MAIN01)")
endif()
if(NOT _pb GREATER _pe)
    message(FATAL_ERROR "r35_cut10f0[A]: MAIN01 vor dem Szenen-Ende")
endif()
# Ende: Kamera wieder auf Cut 2 (Leon im Bild), nicht auf Cut 0 der Abgangsszene (letzte cam-trace-Zeile)
string(REGEX MATCHALL "\\[cam-trace\\] F[0-9]+ room=10f0 [^\n]*" _cts "${_dbg}")
list(LENGTH _cts _ncts)
if(_ncts EQUAL 0)
    message(FATAL_ERROR "r35_cut10f0[A]: keine cam-trace-Zeile in ROOM10F0")
endif()
list(GET _cts -1 _letzt)
string(REGEX MATCH "shown\\(s_last\\)=([0-9]+)" _m "${_letzt}")
if(NOT "${CMAKE_MATCH_1}" STREQUAL "2")
    message(FATAL_ERROR "r35_cut10f0[A]: nach der Szene steht die Kamera nicht auf Cut 2 (${_letzt})")
endif()
# Tuerknall wirklich auf die RE2-Tuerbank geleitet (audio_pc.c, nicht "VERWORFEN")
string(REGEX MATCHALL "\\[se\\] Se_on bank=14 id=1 -> RE2-Tuerbank" _tb "${_dbg}")
list(LENGTH _tb _ntb)
if(NOT _ntb EQUAL 2)
    message(FATAL_ERROR "r35_cut10f0[A]: Port-Bank 0x0E ${_ntb}x auf die RE2-Tuerbank geleitet statt 2x")
endif()

# --- B: Szene schon gesehen -> keine Szene -----------------------------------------------------------
szene_lauf(b "gesehen" 10d0 300 _dbg)
_pos("${_dbg}" "[cut10f0]" _ps)
_pos("${_dbg}" "[msg] room=10f0 id=6 " _pm)
_pos("${_dbg}" "Animationsblock von ROOM11B0 geliehen" _pr)
if(NOT (_ps LESS 0 AND _pm LESS 0 AND _pr LESS 0))
    message(FATAL_ERROR "r35_cut10f0[B]: mit (9,71)=1 lief die Szene trotzdem (cut10f0 ${_ps}, msg ${_pm}, rbj ${_pr})")
endif()

# --- C: CONTINUE direkt IN ROOM10F0 mit ausstehender Szene (alter Spielstand) -> Lade-Weg (main.c Boot) ---
szene_lauf(c "in10f0" 10f0 400 _dbg)
_pos("${_dbg}" "[cut10f0] ROOM10F0: Szene gestartet" _ps)
_pos("${_dbg}" "Animationsblock von ROOM11B0 geliehen" _pr)
_pos("${_dbg}" "[msg] room=10f0 id=6 " _pm)
if(_ps LESS 0 OR _pr LESS 0 OR _pm LESS 0)
    message(FATAL_ERROR "r35_cut10f0[C]: CONTINUE in ROOM10F0: Szene ${_ps}, Leihe ${_pr}, msg 6 ${_pm} (Boot-Weg)")
endif()

file(REMOVE "${_exe_kopie}")
message(STATUS "r35_cut10f0: OK - Eintritt durch die Tuer, Szene genau einmal, Hinweiskette, MAIN01, Boot-Weg")
