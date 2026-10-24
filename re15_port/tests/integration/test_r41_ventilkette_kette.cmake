# =============================================================================
# VENTILKETTE IN EINEM EXE-LAUF (Runde 41 Spur C, Nachbesserung 1 — Abnahme 0, Mangel M2; Nachbesserung 4 — Abnahme 3,
# Mangel 1: auf die Kette der Nachbesserungen 3/4 umgestellt; Nachbesserung 5 - Abnahme 4 Mangel 3: Bedingung der Ventile ist
# seit Nachtrag 16 die Sicherung im Ventilkasten 2010 (9,125), nach jedem Ja laeuft die RE2-Dreh-Animation (345 Takte, der
# Wegplan ruht waehrend player_mode 2) -> WARTE 450): 2030 Sued-Landeplatz (Bruecke unten, Sicherung genommen =
# Bedingung der Nachtraege 9/10) -> die richtige Ventil-Folge 09/07/02/01 ZU FUSS (Nachtrag 13) -> Warnung -> ZU FUSS ueber
# Steg, Treppe und Ring durch die Ost-Tuer -> ROOM20B0 -> Auswasch-Szene des Originals bis zur Freigabe ((3,235)=1, Balken
# weg) -> zu Fuss durch den Kanal an die Leiter -> ROOM2050. Dossier analysis/befunde_runde41/C_ventilkette.md §0b M2, §0e.
#
# Orchestrator-Entscheid C-E1 (AUFTRAG.md 2026-10-24 05:25): Name integration_r41_ventilkette_kette, registriert in
# probes/r41_ventilkette.cmake NUR, wenn das Modul der Spur A im Baum liegt (engine/src/wasser20b0_r41.c); im Baum der
# Spur C erscheint stattdessen beim Configure die Statuszeile "uebersprungen: Spur A fehlt".
# ABHAENGIGKEIT SPUR A: die Szene laeuft im Port nur mit dem AUTO-Haken der Spur A (Zweig r41/wasser20b0,
# aot_common.c `re15_wasser20b0_r41_auto_aot`: FUN_800436a8 wird @0x8001ce1c ohne Szenen-Gate gerufen). Ohne ihn
# ueberspringt re15_aot_scan waehrend der Szene die AUTO-Zone Slot 7 (@0x01DF2 sce 3 Ereignis 5, aot_common.c
# `if (in_cinematic && a->type != RE15_AOT_TYPE_CAM_SWITCH) continue;`), sub05 @0x01F5C startet nie und der Spieler
# steht unter der Pad-Sperre von sub02 ((2,7) @0x01EF8 `22 02 07 01`, (1,28) @0x01EFC `22 01 1c 01`) im Westgang
# (Abnahme 0 L3, F109..F1100).
# Ist der Riegel registriert (Spur A im Baum), MUSS die ganze Kette durchlaufen; startet sub05 nicht, ist das ein FAIL.
# Von Hand im Baum der Spur C (cmake -P, ohne den Haken im Quelltext): der Teil bis zur 20B0-Ankunft wird voll geprueft
# ("loaded room20b0.rdt", "Bits 103 + 100"), der Teil ab der Szene meldet "uebersprungen: Haken der Spur A fehlt".
#
# Gemessen 2026-10-24 05:53-05:55 mit dem wortgleichen Haken der Spur A (temporaerer Messbau, nicht committet;
# scratchpad r41c_nb1/mess/kette3; damals noch der Platzhalter am Ring): Warnung 46 -> Zaehler -> Ost-Tuer zu Fuss
# (`[aot] DOOR FIRE slot=0 ... target_cut=11`) -> loaded room20b0.rdt -> sub02: Cut 11 (@0x01F00), Plc_dest Modus 5
# (-15920,-6355) (@0x01F24) -> Zone Slot 7 -> `thread start ... first_op=0x46` = sub05 @0x01F5C (Aot_reset) ->
# Cut 7 (@0x01F6A), Cut 15 (@0x01F9C), Cut 6 (@0x01FB6) -> `WRITE Set zone=3 idx=235 op=1` (@0x01FBC) ->
# `letterbox closed -> gameplay` -> Wegplan: Kante Mittelgang (-21150,-9629) Blick West + Aktion = Sprung in den
# Kanal (SCA ROOM20B0 Zelle 18 x -25200..-21650 Band 1 = Kanalrand, WATER-Zone Slot 5), Kanal (-24600,-9629) Blick
# West + Aktion = Aufstieg auf den Westgang, Leiter (-26000,10300) Blick West + Aktion -> `[aot] DOOR FIRE slot=4
# ... spawn=(9100,-5400,4100)` -> loaded room2050.rdt (kein Sperrtext: Bit 100 gesetzt beim Verlassen von 2030).
# Die Kill-Bits der drei Spinnen (Sce_em_set pc[7] 0x9b/0x9c/0x9d @0x01C18/@0x01C2C/@0x01C40) stehen fuer den
# Spielzustand "schon getoetet" (Messmittel wie integration_r41_ventilkette C / Abnahme 0 L3).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_SRC=<re15_port> -DWORKDIR=<dir> -P test_r41_ventilkette_kette.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_ventilkette_kette: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_SRC OR NOT EXISTS "${RE15_SRC}/engine/src/aot_common.c")
    message(FATAL_ERROR "r41_ventilkette_kette: RE15_SRC (re15_port) fehlt: '${RE15_SRC}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_ventilkette_kette: WORKDIR fehlt")
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

# Eigener exe-Name (Praefix re15_pc_r41ck_; Teil A raeumt re15_pc_r41c_haken_*, Teil B re15_pc_r41cb_*).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41ck_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r41ck_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r41ck_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

function(_fehler _was)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_ventilkette_kette: ${_was}")
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

# Ein Lauf der Kette; _exit = RE15_EXIT_AT. EIN Wegplan fuer beide Raeume: 2030 die Ventile und die Flucht, dann RAUM 20B0
# (wartet, bis der Raum geladen UND der Spieler frei ist = Ende der Szene), Kanal, Leiter, RAUM 2050. Setzt _dbg/_wp/_dbg20/
# _sub05 im Aufrufer.
macro(_kette_lauf _name _exit _timeout)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(WRITE "${WORKDIR}/kette.txt"
"RAUM 2030
WARTE 30
GEHE -2650 7280 100
BLICK 3072
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
GEHE 2600 7280 100
BLICK 3072
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
GEHE -1050 6900 100
GEHE -1050 6000
GEHE -1050 -6400
GEHE -1050 -7400 100
GEHE 150 -7880 100
BLICK 1024
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
GEHE -1350 -7880 100
BLICK 1024
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
AKTION
WARTE 30
GEHE -1900 -7700 80
BLICK 2048
WARTE 20
AKTION
WARTE 300
GEHE -6800 -9675
GEHE 6575 -9675
GEHE 6550 -200
BLICK 0
WARTE 10
AKTION
RAUM 20B0
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
    re15_start_spiel(_rv ${_timeout}
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        RE15_MSG_LOG=1 RE15_FLAG_CENSUS=1
        RE15_DEBUG_JUMP=2030@250
        "RE15_SET_FLAG=7:155,7:156,7:157,9:101,9:102,3:108,9:125"
        "RE15_PLAYER_POS=-2650,7280,3072,1"
        "RE15_WEGPLAN=${WORKDIR}/kette.txt" RE15_WEGPLAN_LOG=wegplan.txt
        "RE15_EXIT_AT=${_exit}"
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        _fehler("[${_name}] kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    set(_wp "")
    if(EXISTS "${WORKDIR}/wegplan.txt")
        file(READ "${WORKDIR}/wegplan.txt" _wp)
    endif()
    set(_dbg20 "")
    set(_sub05 "")
    string(FIND "${_dbg}" "[room] PC loaded room20b0.rdt" _p20)
    if(NOT _p20 LESS 0)
        string(SUBSTRING "${_dbg}" ${_p20} -1 _dbg20)
        string(REGEX MATCH "thread start slot=[0-9]+ first_op=0x46" _sub05 "${_dbg20}")
    endif()
endmacro()

# Ohne den Haken der Spur A haengt die Szene (Spieler nie frei) -> dann nur bis zur 20B0-Ankunft (200 Bilder in 20B0).
if(_haken)
    _kette_lauf(voll "60#2050" 720)
else()
    _kette_lauf(bis20b0 "200#20B0" 600)
endif()

# ---- 2030: richtige Folge zu Fuss -> Warnung -> Flucht zu Fuss -> Ost-Tuer -> 20B0 (ohne RE15_FIRE_AOT) ----------------
_danach("${_dbg}" "Ventil 09 gedreht (Drehung 1/4)" "Ventil 07 gedreht (Drehung 2/4)" "09, dann 07 (Suedwand)")
_danach("${_dbg}" "Ventil 07 gedreht (Drehung 2/4)" "Ventil 02 gedreht (Drehung 3/4)" "dann 02 (Nordwand)")
_danach("${_dbg}" "Ventil 02 gedreht (Drehung 3/4)" "Ventil 01 gedreht (Drehung 4/4)" "dann 01")
_danach("${_dbg}" "Ventil 01 gedreht (Drehung 4/4)" "[ventilkette41] Folge 09 07 02 01 -> RICHTIG: Warnung, Zaehler, Flut" "richtige Folge")
_muss("${_dbg}" "[ventilkette41] ROOM2030 Ereignis 26 -> Port-Programm 2030_ventile_flut" "Warnung + Zaehler (Ereignis 26)")
_muss("${_dbg}" "[msg] room=2030 id=46" "Warnung 46")
_muss("${_dbg}" "[ventilkette41] ROOM2030: Tuer Slot 0 -> ROOM20B0 Cut 11 (Zaehler laeuft)" "Ost-Tuer umgelenkt")
_darf_nicht("${_dbg}" "[fire-aot]" "die Ost-Tuer muss zu Fuss (Aktionstaste) feuern, nicht per RE15_FIRE_AOT")
_muss("${_dbg}" "[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750) target_cut=11" "Ost-Tuer zu Fuss gefeuert")
_danach("${_dbg}" "[aot] DOOR FIRE slot=0 rect=(7350,-200" "[room] PC loaded room20b0.rdt" "ROOM20B0 geladen")
_danach("${_dbg}" "[room] PC loaded room20b0.rdt" "Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100" "Bits 103 + 100 beim Verlassen")
_darf_nicht("${_dbg}" "FLUT: Wasser steigt" "der Weg zur Tuer passt in die 60 s")
if(NOT _haken)
    _muss("${_dbg20}" "EXIT_AT: Bild 200 in Raum 20b0" "200 Bilder in ROOM20B0")
    file(REMOVE "${_exe_kopie}")
    message(STATUS "r41_ventilkette_kette: 2030 richtige Folge 09/07/02/01 zu Fuss -> Warnung -> Ost-Tuer zu Fuss -> ROOM20B0, Bits 103 + 100; Teil ab der Szene uebersprungen: Haken der Spur A fehlt (aot_common.c re15_wasser20b0_r41_auto_aot)")
    return()
endif()
# ---- Szene sub02 -> Zone Slot 7 -> sub05 ------------------------------------------------------------------------
_danach("${_dbg}" "[room] PC loaded room20b0.rdt" "Plc_dest(slot=0 mode=0x05 dest=(-15920,-6355)" "sub02: Plc_dest Modus 5 (@0x01F24)")
if("${_sub05}" STREQUAL "")
    _fehler("sub05 @0x01F5C (Zone Slot 7, AUTO) startet nicht - der AUTO-Haken der Spur A (aot_common.c re15_wasser20b0_r41_auto_aot, FUN_800436a8 @0x8001ce1c) fehlt oder wirkt nicht (Haken im Quelltext: ${_haken})")
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
_muss("${_dbg20}" "EXIT_AT: Bild 60 in Raum 2050" "Endbild in ROOM2050")

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_ventilkette_kette: 2030 richtige Folge zu Fuss -> Ost-Tuer zu Fuss -> 20B0 Szene sub02/sub05 -> (3,235)=1 -> frei -> Kanal -> Leiter -> ROOM2050 in einem Lauf (Haken der Spur A: ${_haken})")
