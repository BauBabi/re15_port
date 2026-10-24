# =============================================================================
# VENTILKETTE, Nachbesserung 2 (Runde 41 Spur C, Abnahme 1 Mangel N1, Orchestrator-Entscheid C-E2b 2026-10-24 07:49):
# der Weg zum Rechner ROOM2080 NACH der Flut (Bits 103+100, ohne 107) zu Fuss an der echten exe. Herleitung und
# Tuergraph: Dossier analysis/befunde_runde41/C_ventilkette.md §0c, Werkzeug tools/r41_ventilkette/tuergraph_nach_flut.py,
# Ausgabe C_belege/nb2_tuergraph_nach_flut.txt. Die Glieder des Wegs (Wegplan-Treiber RE15_WEGPLAN, dachkiste_r36.c:
# echte Pad-Bits aus Spieler-/Ziellage, Aktionstaste, kein RE15_FIRE_AOT):
#
#   a  Leon: ROOM2040 Halle (Ankunft aus 2060 (-12850,-5400,1600)) mit benutzter Zeitbombe ((3,146) ROOM1110 @0x00D32,
#      (3,53) ROOM2040 sub02 @0x01624 -> Zelle 19 frei) -> durch die Rost-Tuer -> Treppe sce 13 (@0x0133C, mo 221) ->
#      Galerie Band 5 -> Tuer slot 2 (@0x012DC) -> slot 4 (@0x0131C) -> ROOM2070 Nordgang -> Riegel slot 7 ("You've
#      opened the lock.", (3,151) @0x00CE4) -> slot 4 -> ROOM11A0 -> Aufstieg aus dem Kanal -> Steg -> Ost-Tuer slot 4:
#      ORIGINAL-SPERRE main00 @0x00FEA Ck(3,206)=0 -> @0x00FEE Text msg 2 "It's locked from the other side."; KEIN
#      Raumwechsel nach 20A0, (3,206) bleibt 0 (setzt nur ROOM20A0/20A1 sub10 @0x01FF0).
#   b  Elza: derselbe Weg in ROOM2041 / 2071 / 11A1.
#   c  Gegenprobe Leon: ROOM11A0 mit (3,206)=1 -> dieselbe Aktion -> DOOR FIRE slot 4 -> ROOM20A0 (die einzige Sperre).
#   d  Leon: ROOM2050 nach dem Kisten-Raetsel ((3,109), sub02 @0x0221C; seit M1 erst nach dem Pult = Bit 106) ->
#      Zelle 51 frei (sub00 @0x01FEE `37 0r 33 00`) -> Tasche -> slot 1 (@0x01C6A) -> ROOM2020.
#   e  Leon: ROOM2030 Nordgalerie nach der Flut (Ankunft aus 2020, (3,219)) -> slot 3 (@0x01B4C) -> ROOM2060.
#   f  Leon: ROOM2080 von der 2090-Tuer (Ankunft (-9950,-14400,-25700) Band 8) zum Rechner -> Port-Programm
#      2080_rechner_erw -> msg 40 -> Bit 107 (der Rest des C-E2b-Wegs, falls (3,206) je 1 waere).
# Das Ergebnis (OFFEN fuer den Orchestrator): der Weg bricht an der ORIGINAL-Sperre in a/b ab; es gibt keine Port-Sperre
# der Runde 41 auf ihm. Fallen a/b gruen aus, ist genau dieser Befund belegt; aendert jemand die Sperre, faellt der
# Riegel und der Befund ist neu zu bewerten.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_ventilkette_weg.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_ventilkette_weg: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_ventilkette_weg: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake), Praefix re15_pc_r41cw_.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41cw_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r41cw_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r41cw_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(_fehler _was)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_ventilkette_weg${_was}")
endfunction()
function(_muss _text _wort _name _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(_p LESS 0)
        _fehler("[${_name}]: ${_was} — '${_wort}' fehlt")
    endif()
endfunction()
function(_darf_nicht _text _wort _name _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(NOT _p LESS 0)
        _fehler("[${_name}]: ${_was} — '${_wort}' steht im Log")
    endif()
endfunction()
function(_danach _text _wort1 _wort2 _name _was)
    string(FIND "${_text}" "${_wort1}" _p1)
    if(_p1 LESS 0)
        _fehler("[${_name}]: ${_was} — '${_wort1}' fehlt")
    endif()
    string(SUBSTRING "${_text}" ${_p1} -1 _rest)
    string(FIND "${_rest}" "${_wort2}" _p2)
    if(_p2 LESS 0)
        _fehler("[${_name}]: ${_was} — '${_wort2}' steht nicht nach '${_wort1}'")
    endif()
endfunction()

set(_leon RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1
          RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
set(_elza RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1
          RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)

macro(_lauf _name _timeout _plan)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(WRITE "${WORKDIR}/wegplan_plan.txt" "${_plan}")
    re15_start_spiel(_rv ${_timeout} ${ARGN} "RE15_WEGPLAN=${WORKDIR}/wegplan_plan.txt" RE15_WEGPLAN_LOG=wegplan.txt
                     "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        _fehler("[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    set(_wp "")
    if(EXISTS "${WORKDIR}/wegplan.txt")
        file(READ "${WORKDIR}/wegplan.txt" _wp)
    endif()
endmacro()

# Wegplan 2040 Halle -> Galerie -> 2070 Nordgang -> 11A0 -> Ost-Tuer (Raumnummern je Szenario)
function(_weg_plan _out _r2040 _r2070 _r11a0)
    set(${_out} "RAUM ${_r2040}
WARTE 30
FLAG 3 53
GEHE -14750 1700 150
GEHE -14750 -5400 150
GEHE 5050 -5400 150
GEHE 5050 -7200 150
GEHE 5050 -10500 150
GEHE -1350 -10500 150
GEHE -1350 -13950 100
BLICK 1024
WARTE 10
AKTION
WARTE 150
GEHE -2300 -19800 120
BLICK 2048
WARTE 10
AKTION
WARTE 90
GEHE -19750 -19800 150
BLICK 2048
WARTE 10
AKTION
RAUM ${_r2070}
WARTE 30
FLAG 3 151
GEHE -18600 -19250 150
BLICK 2048
WARTE 10
AKTION
WARTE 90
AKTION
WARTE 60
FLAG 3 151
AKTION
WARTE 60
AKTION
RAUM ${_r11a0}
WARTE 30
FLAG 3 206
GEHE 6500 -600 100
BLICK 3072
WARTE 10
AKTION
WARTE 120
GEHE 28700 1300 100
BLICK 0
WARTE 10
AKTION
WARTE 120
AKTION
WARTE 90
FLAG 3 206
ENDE
" PARENT_SCOPE)
endfunction()

macro(_weg_pruefen _name _r2040 _r2070 _r11a0)
    string(TOLOWER "${_r2070}" _k2070)
    string(TOLOWER "${_r11a0}" _k11a0)
    _muss("${_dbg}" "[aot] DOOR FIRE slot=2 rect=(-2400,-19800,hw=400,hh=1000) target_cut=11 spawn=(-3900,-9000,-19850)" ${_name} "Galerie-Tuer slot 2 @0x012DC nach der Treppe (Band 5)")
    _muss("${_dbg}" "[aot] DOOR FIRE slot=4 rect=(-20450,-19800,hw=500,hh=1000) target_cut=9 spawn=(4650,0,-19200)" ${_name} "Tuer slot 4 @0x0131C nach 2070 Nordgang")
    _muss("${_dbg}" "[msgtext] room=${_k2070} id=0 \"You've opened the lock.\"" ${_name} "Riegel im Nordgang (sub02 @0x00CE4)")
    _muss("${_dbg}" "[aot] DOOR FIRE slot=4 rect=(-19400,-19250,hw=500,hh=1000) target_cut=7 spawn=(6500,0,-3200)" ${_name} "Tuer 2070 slot 4 @0x00AFA nach 11A0")
    _muss("${_dbg}" "loaded room${_k11a0}.rdt" ${_name} "11A0 geladen")
    _muss("${_dbg}" "[msgtext] room=${_k11a0} id=2 \"It's locked from the other side.\"" ${_name} "ORIGINAL-Sperre 11A0 slot 4 (@0x00FEA Ck(3,206)=0 -> @0x00FEE msg 2)")
    _danach("${_dbg}" "loaded room${_k11a0}.rdt" "[msgtext] room=${_k11a0} id=2" ${_name} "der Satz kommt in 11A0 nach dem Aufstieg")
    _darf_nicht("${_dbg}" "loaded room20a" ${_name} "kein Raumwechsel nach 20A0 (Tuer erst mit (3,206))")
    _muss("${_wp}" "FLAG (3,53) = 1" ${_name} "Zeitbombe als benutzt gesetzt")
    _muss("${_wp}" "FLAG (3,151) = 1" ${_name} "(3,151) nach dem Riegel = 1")
    string(FIND "${_wp}" "FLAG (3,206) = 1" _p206)
    if(NOT _p206 LESS 0)
        _fehler("[${_name}]: (3,206) wurde 1 — die Sperre waere offen")
    endif()
    _muss("${_wp}" "FLAG (3,206) = 0" ${_name} "(3,206) bleibt 0")
    _muss("${_dbg}" "EXIT_AT: Bild" ${_name} "Endbild in 11A0 erreicht")
endmacro()

# ---- a: Leon 2040 -> 2070 -> 11A0 -> Sperre ------------------------------------------------------------------------
_weg_plan(_pa 2040 2070 11A0)
_lauf(a 420 "${_pa}" ${_leon} RE15_DEBUG_JUMP=2040@250 RE15_SET_FLAG=9:103,9:100,3:146,3:53
      "RE15_PLAYER_POS=-12850,1600,1024,3" "RE15_EXIT_AT=1000#11A0")
_weg_pruefen(a 2040 2070 11A0)
message(STATUS "r41_ventilkette_weg a (Leon): 2040 Halle -> Treppe -> Galerie -> 2070 Nordgang -> 11A0 -> 'It's locked from the other side.' (Original-Sperre (3,206))")

# ---- b: Elza 2041 -> 2071 -> 11A1 -> Sperre ------------------------------------------------------------------------
_weg_plan(_pb 2041 2071 11A1)
_lauf(b 420 "${_pb}" ${_elza} RE15_DEBUG_JUMP=2040@250 RE15_SET_FLAG=9:103,9:100,3:146,3:53
      "RE15_PLAYER_POS=-12850,1600,1024,3" "RE15_EXIT_AT=1000#11A1")
_muss("${_dbg}" "Spieler-Familie PL04" b "Elza gewaehlt")
_weg_pruefen(b 2041 2071 11A1)
message(STATUS "r41_ventilkette_weg b (Elza): dieselbe Sperre in 11A1")

# ---- c: Gegenprobe (3,206)=1 -> Tuer nach 20A0 ------------------------------------------------------------------------
_lauf(c 150 "RAUM 11A0
WARTE 30
FLAG 3 206
GEHE 28700 1300 100
BLICK 0
WARTE 10
AKTION
WARTE 60
AKTION
RAUM 20A0
WARTE 30
ENDE
" ${_leon} RE15_DEBUG_JUMP=11A0@250 RE15_SET_FLAG=9:103,9:100,3:151,3:206 "RE15_PLAYER_POS=27000,1300,0,1"
      "RE15_EXIT_AT=60#20A0")
_muss("${_wp}" "FLAG (3,206) = 1" c "(3,206) gesetzt")
_muss("${_dbg}" "[aot] DOOR FIRE slot=4 rect=(29000,1300,hw=500,hh=1100) target_cut=8 spawn=(25800,-3600,-800)" c "mit (3,206) feuert 11A0 slot 4 @0x01006")
_muss("${_dbg}" "loaded room20a0.rdt" c "20A0 geladen")
_darf_nicht("${_dbg}" "[msgtext] room=11a0 id=2" c "kein Sperrsatz mit (3,206)")

# ---- d: 2050 nach dem Kisten-Raetsel -> 2020 -----------------------------------------------------------------------
_lauf(d 150 "RAUM 2050
GEGNERWEG
WARTE 30
ZELLE 0 51
GEHE 9100 -6300 150
GEHE -6850 -6300 150
GEHE -6850 2500 150
GEHE -5150 3600 120
BLICK 3072
WARTE 10
AKTION
RAUM 2020
WARTE 30
ENDE
" ${_leon} RE15_DEBUG_JUMP=2050@250 RE15_SET_FLAG=9:103,9:100,9:106,3:109 "RE15_PLAYER_POS=9100,4100,1024,3"
      "RE15_EXIT_AT=60#2020")
_muss("${_wp}" "ZELLE r0[51] LAUFZEIT: x -7550..-6150 z -4500..850 typ 1 u0 0x00" d "Zelle 51 nach (3,109) frei (sub00 @0x01FEE)")
_muss("${_dbg}" "[aot] DOOR FIRE slot=1 rect=(-5150,4450,hw=1000,hh=500) target_cut=0 spawn=(11350,-1800,-3150)" d "2050 slot 1 @0x01C6A nach 2020")
_muss("${_dbg}" "loaded room2020.rdt" d "2020 geladen")

# ---- e: 2030 Nordgalerie nach der Flut -> 2060 ----------------------------------------------------------------------
_lauf(e 150 "RAUM 2030
WARTE 30
GEHE -6943 -9687 120
GEHE -6950 -7250 120
BLICK 2048
WARTE 10
AKTION
RAUM 2060
WARTE 30
ENDE
" ${_leon} RE15_DEBUG_JUMP=2030@250 RE15_SET_FLAG=9:103,9:100,7:155,7:156,7:157,7:158,3:219
      "RE15_PLAYER_POS=-450,-9700,3072,9" "RE15_EXIT_AT=60#2060")
_muss("${_dbg}" "[ventilkette41] ROOM2030: unter Wasser (y -7200)" e "2030 geflutet")
_muss("${_dbg}" "[aot] DOOR FIRE slot=3 rect=(-7700,-7250,hw=500,hh=900) target_cut=0 spawn=(7600,0,3950)" e "2030 slot 3 @0x01B4C (Nordgalerie) nach 2060")
_muss("${_dbg}" "loaded room2060.rdt" e "2060 geladen")

# ---- f: 2080 von der 2090-Tuer zum Rechner -> Bit 107 -----------------------------------------------------------------
_lauf(f 200 "RAUM 2080
WARTE 30
FLAG 9 107
GEHE -10500 -24000 150
GEHE -10500 -8000 150
GEHE -11185 -7300 80
BLICK 3072
WARTE 10
AKTION
WARTE 120
AKTION
WARTE 120
AKTION
WARTE 480
AKTION
WARTE 60
FLAG 9 107
ENDE
" ${_leon} RE15_DEBUG_JUMP=2080@240 RE15_SET_FLAG=9:103,9:100,3:206 "RE15_PLAYER_POS=-9950,-25700,2048,8"
      "RE15_EXIT_AT=1700#2080")
_muss("${_dbg}" "[ventilkette41] ROOM2080 Ereignis 2 -> Port-Programm 2080_rechner_erw" f "Rechner von der 2090-Tuer aus")
_danach("${_dbg}" "Port-Programm 2080_rechner_erw" "Cut_chg(7)" f "Wasser 2080 sinkt (sub02 @0x00CC2)")
_muss("${_dbg}" "[msgtext] room=2080 id=40 \"The treatment pool is being drained as well.\"" f "Text 40")
_muss("${_wp}" "FLAG (9,107) = 1" f "Bit 107 gesetzt")

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_ventilkette_weg: Glieder 2050->2020, 2030N->2060, 2040->2070->11A0 (Leon+Elza) gangbar; Ende an der ORIGINAL-Sperre 11A0 slot 4 (3,206); mit (3,206) -> 20A0; 2080 ab 2090 -> Rechner -> Bit 107")
