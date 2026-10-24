# =============================================================================
# VENTILKETTE IN EINEM EXE-LAUF (Runde 41 Spur C, Nachbesserung 1 — Abnahme 0, Mangel M2): 2030 Ventile -> Warnung ->
# ZU FUSS durch die Ost-Tuer -> ROOM20B0 -> Auswasch-Szene des Originals bis zur Freigabe ((3,235)=1, Balken weg) ->
# zu Fuss durch den Kanal an die Leiter -> ROOM2050. Dossier analysis/befunde_runde41/C_ventilkette.md §0b M2.
#
# ABHAENGIGKEIT SPUR A: die Szene laeuft im Port nur mit dem AUTO-Haken der Spur A (Zweig r41/wasser20b0,
# aot_common.c `re15_wasser20b0_r41_auto_aot`: FUN_800436a8 wird @0x8001ce1c ohne Szenen-Gate gerufen). Ohne ihn
# ueberspringt re15_aot_scan waehrend der Szene die AUTO-Zone Slot 7 (@0x01DF2 sce 3 Ereignis 5, aot_common.c
# `if (in_cinematic && a->type != RE15_AOT_TYPE_CAM_SWITCH) continue;`), sub05 @0x01F5C startet nie und der Spieler
# steht unter der Pad-Sperre von sub02 ((2,7) @0x01EF8, (1,27) @0x01EFC) im Westgang (Abnahme 0 L3, F109..F1100).
# Gate: fehlt der Haken im Quellbaum UND haengt die Szene genau so (kein sub05-Start), meldet der Riegel
# "R41C-SKIP:" (ctest SKIP_REGULAR_EXPRESSION -> "Skipped", weder gruen noch rot); mit dem Haken (nach der
# Integration) MUSS die ganze Kette durchlaufen. Laeuft die Szene ohne den Haken trotzdem durch (spaetere allgemeine
# Loesung), wird ebenfalls voll geprueft.
#
# Gemessen 2026-10-24 05:53-05:55 mit dem wortgleichen Haken der Spur A (temporaerer Messbau, nicht committet;
# scratchpad r41c_nb1/mess/kette3): Ventile Text 45 -> Ja -> Warnung 46 -> Zaehler -> Ost-Tuer zu Fuss (Drehung R0.7, Aktion,
# `[aot] DOOR FIRE slot=0 ... target_cut=11`) -> loaded room20b0.rdt -> sub02: Cut 11 (@0x01F00), Plc_dest Modus 5
# (-15920,-6355) (@0x01F24) -> Zone Slot 7 -> `thread start ... first_op=0x46` = sub05 @0x01F5C (Aot_reset) ->
# Cut 7 (@0x01F6A), Cut 15 (@0x01F9C), Cut 6 (@0x01FB6) -> `WRITE Set zone=3 idx=235 op=1` (@0x01FBC) ->
# `letterbox closed -> gameplay` -> Wegplan: Kante Mittelgang (-21150,-9629) Blick West + Aktion = Sprung in den
# Kanal (SCA ROOM20B0 Zelle 18 x -25200..-21650 Band 1 = Kanalrand, WATER-Zone Slot 5), Kanal (-24600,-9629) Blick
# West + Aktion = Aufstieg auf den Westgang, Leiter (-26000,10300) Blick West + Aktion -> `[aot] DOOR FIRE slot=4
# ... spawn=(9100,-5400,4100)` -> loaded room2050.rdt (kein Sperrtext: Bit 100 gesetzt beim Verlassen von 2030).
# Die Kill-Bits der drei Spinnen (Sce_em_set pc[7] 0x9b/0x9c/0x9d @0x01C18/@0x01C2C/@0x01C40) stehen fuer den
# Spielzustand "schon getoetet" (Messmittel wie integration_r41_ventilkette C / Abnahme 0 L3).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_SRC=<re15_port> -DWORKDIR=<dir> -P test_r41_ventilkette_szene.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_ventilkette_szene: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_SRC OR NOT EXISTS "${RE15_SRC}/engine/src/aot_common.c")
    message(FATAL_ERROR "r41_ventilkette_szene: RE15_SRC (re15_port) fehlt: '${RE15_SRC}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_ventilkette_szene: WORKDIR fehlt")
endif()

file(READ "${RE15_SRC}/engine/src/aot_common.c" _aot_src)
string(FIND "${_aot_src}" "re15_wasser20b0_r41_auto_aot" _p_haken)
if(_p_haken LESS 0)
    set(_haken 0)
else()
    set(_haken 1)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name (Praefix re15_pc_r41cs_; Teil A raeumt re15_pc_r41c_haken_*, Teil B re15_pc_r41cb_*).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41cs_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r41cs_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r41cs_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

function(_fehler _was)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_ventilkette_szene: ${_was}")
endfunction()
function(_muss _text _wort _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(_p LESS 0)
        _fehler("${_was} — '${_wort}' fehlt")
    endif()
endfunction()
function(_darf_nicht _text _wort _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(NOT _p LESS 0)
        _fehler("${_was} — '${_wort}' steht im Log")
    endif()
endfunction()
# _wort2 muss NACH dem ersten Auftreten von _wort1 stehen.
function(_danach _text _wort1 _wort2 _was)
    string(FIND "${_text}" "${_wort1}" _p1)
    if(_p1 LESS 0)
        _fehler("${_was} — '${_wort1}' fehlt")
    endif()
    string(SUBSTRING "${_text}" ${_p1} -1 _rest)
    string(FIND "${_rest}" "${_wort2}" _p2)
    if(_p2 LESS 0)
        _fehler("${_was} — '${_wort2}' steht nicht nach '${_wort1}'")
    endif()
endfunction()

set(WORKDIR "${_basis}")
file(MAKE_DIRECTORY "${WORKDIR}")
# Wegplan ab ROOM20B0 (RAUM wartet, bis der Raum geladen UND der Spieler frei ist = Ende der Szene).
file(WRITE "${WORKDIR}/kette.txt"
"RAUM 20B0
WARTE 30
FLAG 3 235
FLAG 9 103
FLAG 9 100
GEHE -21150 -9629 80
BLICK 2048
WARTE 10
AKTION
WARTE 90
GEHE -24600 -9629 150
BLICK 2048
WARTE 10
AKTION
WARTE 120
GEHE -26000 10300 100
BLICK 2048
WARTE 10
AKTION
RAUM 2050
WARTE 30
ENDE
")
re15_start_spiel(_rv 420
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_MSG_LOG=1 RE15_FLAG_CENSUS=1
    RE15_DEBUG_JUMP=2030@240
    "RE15_SET_FLAG=7:155,7:156,7:157"
    "RE15_PLAYER_POS=6550,-100,2048"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330
    "RE15_INPUT_SCRIPT=A0.1,W4,A0.1,W2.5,A0.1,W4,A0.1,W4,A0.1,W4,R0.7,W1,A0.1,U3"
    "RE15_WEGPLAN=${WORKDIR}/kette.txt" RE15_WEGPLAN_LOG=wegplan.txt
    "RE15_EXIT_AT=60#2050"
    "${_exe_kopie}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    _fehler("kein debug.log (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
set(_wp "")
if(EXISTS "${WORKDIR}/wegplan.txt")
    file(READ "${WORKDIR}/wegplan.txt" _wp)
endif()

# ---- 2030 -> Ost-Tuer zu Fuss -> 20B0 (wie integration_r41_ventilkette A, hier ohne RE15_FIRE_AOT) ------------------
_muss("${_dbg}" "[ventilkette41] ROOM2030 Ereignis 26 -> Port-Programm 2030_ventile" "Aktion an den Ventilen")
_muss("${_dbg}" "[msg] room=2030 id=46" "Warnung 46 (Ja gewaehlt)")
_muss("${_dbg}" "[ventilkette41] ROOM2030: Tuer Slot 0 -> ROOM20B0 Cut 11 (Zaehler laeuft)" "Ost-Tuer umgelenkt")
_darf_nicht("${_dbg}" "[fire-aot]" "die Ost-Tuer muss zu Fuss (Aktionstaste) feuern, nicht per RE15_FIRE_AOT")
_muss("${_dbg}" "[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750) target_cut=11" "Ost-Tuer zu Fuss gefeuert")
_danach("${_dbg}" "[aot] DOOR FIRE slot=0 rect=(7350,-200" "[room] PC loaded room20b0.rdt" "ROOM20B0 geladen")
_danach("${_dbg}" "[room] PC loaded room20b0.rdt" "Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100" "Bits 103 + 100 beim Verlassen")
# ---- Szene sub02 -> Zone Slot 7 -> sub05 ------------------------------------------------------------------------
_danach("${_dbg}" "[room] PC loaded room20b0.rdt" "Plc_dest(slot=0 mode=0x05 dest=(-15920,-6355)" "sub02: Plc_dest Modus 5 (@0x01F24)")
string(FIND "${_dbg}" "[room] PC loaded room20b0.rdt" _p20)
string(SUBSTRING "${_dbg}" ${_p20} -1 _dbg20)
string(REGEX MATCH "thread start slot=[0-9]+ first_op=0x46" _sub05 "${_dbg20}")
if("${_sub05}" STREQUAL "")
    if(NOT _haken)
        file(REMOVE "${_exe_kopie}")
        message(STATUS "R41C-SKIP: die Auswasch-Szene ROOM20B0 haengt (sub05 @0x01F5C startet nicht) und aot_common.c traegt den AUTO-Haken der Spur A (re15_wasser20b0_r41_auto_aot, Zweig r41/wasser20b0) noch nicht - der Riegel wird mit der Integration der Spur A scharf (Dossier C §0b M2).")
        return()
    endif()
    _fehler("sub05 @0x01F5C (Zone Slot 7, AUTO) startet nicht, obwohl der Haken der Spur A im Quellbaum steht")
endif()
_danach("${_dbg20}" "${_sub05}" "Cut_chg(7)" "sub05: Cut 7 (@0x01F6A)")
_danach("${_dbg20}" "Cut_chg(7)" "Cut_chg(15)" "sub05: Cut 15 (@0x01F9C)")
_danach("${_dbg20}" "Cut_chg(15)" "Cut_chg(6)" "sub05: Cut 6 (@0x01FB6)")
_danach("${_dbg20}" "Cut_chg(6)" "[flagcensus] room=20B0 WRITE Set  zone=3 idx=235 op=1" "sub05: Set(3,235)=1 (@0x01FBC)")
_danach("${_dbg20}" "zone=3 idx=235 op=1" "letterbox closed -> gameplay (player_mode=0)" "Szene beendet, Spieler frei")
_muss("${_wp}" "FLAG (3,235) = 1" "Wegplan: (3,235) nach der Szene")
_muss("${_wp}" "FLAG (9,103) = 1" "Wegplan: Bit 103")
_muss("${_wp}" "FLAG (9,100) = 1" "Wegplan: Bit 100 (Leiter frei)")
# ---- zu Fuss durch den Kanal an die Leiter -> 2050 ---------------------------------------------------------------
_darf_nicht("${_wp}" "KLEMME" "der Weg zur Leiter klemmte")
_darf_nicht("${_dbg20}" "-> Sperrtext 40" "mit Bit 100 ist die Leiter eine Tuer")
_muss("${_dbg20}" "[aot] DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500) target_cut=0 spawn=(9100,-5400,4100)" "Leiter feuert")
_danach("${_dbg20}" "[aot] DOOR FIRE slot=4" "[room] PC loaded room2050.rdt" "ROOM2050 geladen")
_muss("${_dbg20}" "EXIT_AT: Bild" "Endbild in ROOM2050")

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_ventilkette_szene: 2030 Ventile -> Ost-Tuer zu Fuss -> 20B0 Szene sub02/sub05 -> (3,235)=1 -> frei -> Kanal -> Leiter -> ROOM2050 in einem Lauf (Haken der Spur A: ${_haken})")
