cmake_minimum_required(VERSION 3.16)
# =============================================================================
# Runde 37 Spur D "itemband" — ECHTE exe. Dossier: analysis/befunde_runde37/D_itemband.md.
#
# Nutzer (AUFTRAG.md #8): "Items auf erhoehten Flaechen lassen sich vom falschen Stockwerk aufnehmen, z.B. die Super
# Redhawk in ROOM3050 ohne die Kiste." Original (Belege include/re15_itemband_r37.h):
#   AOT-Scan FUN_80042bac @0x80042cac-ccc: rec[2] (Item_aot_set pc[4]) == Spieler+0x82 fuer JEDEN Satz vor dem sce-Dispatch;
#   Member_set 0x12 -> @0x80041228 `sb a2,130(a0)` = 0x800ACAD6 = Kollisionsband (Lift ROOM3050 sub04 @0x023D0 -> Band 0).
# Am Original gemessen (D_belege/orig_r36/o3050_item_log.txt, D_belege/orig_*): 3050 Band 4/0 -> keine Aufnahme, Band 1 ->
# Aufnahme; Lift -> Band 0.
#
# Laeufe (RE15_WEGPLAN = geschlossen geregelter Messlauf, engine/src/dachkiste_r36.c; RE15_ITEMBAND_LOG = Messschrieb des
# Gates, engine/src/itemband_r37.c). Je Item: FALSCHES Band an der Item-Stelle -> GESPERRT + Taken-Bit 0; VORGESEHENER Weg
# -> FREI auf dem Item-Band + Taken-Bit 1.
#   oben3050    Leon obere Ebene (Band 4) an der Super Redhawk (slot 6 @0x01EC6 floor 1): 3x GESPERRT, (9,92) = 0.
#   weg3050     Kiste schieben -> Fall -> Lift (MEMBER 0x12 4 -> 0) -> Kiste in die Kerbe ((3,93) = 1) -> Klettern ->
#               FREI auf Band 1 -> (9,92) = 1.
#   elza3051    dasselbe wie oben3050 mit Elza (ROOM3051 @0x01EC6 byte-gleich): GESPERRT, (9,92) = 0.
#   unten3010   Band 0 unter dem Steg (SHOTGUN SHELLS slot 5 @0x02094 floor 1): GESPERRT, (9,20) = 0.
#   weg3010     Klettern an r0[4] (Band 0 -> 1) -> FREI auf Band 1 -> (9,20) = 1.
#   weg2090     Tuer 20A0 slot 0 @0x01746 -> 2090 Plattform Band 1 -> FREI (Pocket Watch slot 5 @0x00A12) -> (9,90) = 1.
#               Mit Bits (9,110)+(9,113) (Querbezug Runde 41 Spur F, s. Lauf weg2090).
#   galerie2040 Band 5 an der Head of Akuma (slot 7 @0x01364 floor 3): GESPERRT, (9,91) = 0.
#   weg2040     Halle Band 3 (Tor-Raetsel (3,146)/(3,53) als geloest gesetzt: sub00 schliesst Zelle 19 sonst per Sca_id_set)
#               -> Nordgang -> FREI auf Band 3 -> (9,91) = 1.
#   spas2030    Lift A (sub02, MEMBER 0x12 3 -> 6) -> SPAS-12 (slot 11 @0x01BD6 floor 6) FREI auf Band 6 -> (9,109) = 1.
#   spasb3      Band 3 an der SPAS-12: GESPERRT, (9,109) = 0.
#   schrot2030  Treppe slot 8 @0x01B8E West-Haelfte (Abstieg 3 -> 1) -> Band-1-Gang -> SHOTGUN SHELLS (slot 10 @0x01BC0)
#               FREI auf Band 1 -> (9,108) = 1.
#   schrotb3    Band 3 (Hallenhoehe) an den Schrotpatronen: GESPERRT, (9,108) = 0.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r37_itemband.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_itemband: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r37_itemband: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_basis}")

# Eigener exe-Name neben der exe (Muster test_r34n_b_cursor.cmake: fremde `taskkill /IM re15_pc.exe`).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37d_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r37d_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

function(_fehler _was)
    file(REMOVE "${_exe}")
    message(FATAL_ERROR "r37_itemband: ${_was}")
endfunction()

set(_grund RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
           RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_STAIR_DBG=1
           RE15_CLIMB_TRACE=climb.txt RE15_ITEMBAND_LOG=itemband.txt RE15_WEGPLAN_LOG=wegplan.txt)

# Ein Lauf im eigenen Unterordner mit eigenem Wegplan (Zeilen als Liste); debug.log/itemband.txt/wegplan.txt bleiben liegen.
macro(_lauf _name _timeout _plan)
    set(_sub "${_basis}/${_name}")
    file(MAKE_DIRECTORY "${_sub}")
    string(REPLACE ";" "\n" _plan_text "${${_plan}}")
    file(WRITE "${_sub}/wegplan_eingabe.txt" "${_plan_text}\n")
    set(WORKDIR "${_sub}")
    re15_start_spiel(_rv ${_timeout} ${_grund} "RE15_WEGPLAN=${_sub}/wegplan_eingabe.txt" ${ARGN} "${_exe}")
    if("${_rv}" STREQUAL "1" OR "${_rv}" STREQUAL "-1" OR NOT EXISTS "${_sub}/debug.log")
        message(STATUS "r37_itemband[${_name}]: Lauf ohne Ergebnis (exit=${_rv}) -> EIN Wiederholungsversuch")
        file(REMOVE "${_sub}/debug.log" "${_sub}/itemband.txt" "${_sub}/wegplan.txt" "${_sub}/climb.txt")
        re15_start_spiel(_rv ${_timeout} ${_grund} "RE15_WEGPLAN=${_sub}/wegplan_eingabe.txt" ${ARGN} "${_exe}")
    endif()
    if(NOT EXISTS "${_sub}/debug.log")
        _fehler("${_name}: kein debug.log (exit=${_rv}, ${_sub})")
    endif()
    file(READ "${_sub}/debug.log" _dbg)
    set(_item "")
    if(EXISTS "${_sub}/itemband.txt")
        file(READ "${_sub}/itemband.txt" _item)
    endif()
    set(_wp "")
    if(EXISTS "${_sub}/wegplan.txt")
        file(READ "${_sub}/wegplan.txt" _wp)
    endif()
endmacro()

macro(_muss _text _var _was)
    string(FIND "${${_var}}" "${_text}" _pos)
    if(_pos LESS 0)
        _fehler("${_was} — fehlt: '${_text}'")
    endif()
endmacro()

macro(_darf_nicht_text _text _var _was)
    string(FIND "${${_var}}" "${_text}" _pos)
    if(NOT _pos LESS 0)
        _fehler("${_was} — gefunden: '${_text}'")
    endif()
endmacro()

# Letzter Wert des Wegplan-FLAG-Eintrags (Bank, Bit) muss <_soll> sein.
macro(_flag_ende _name _bank _bit _soll)
    string(REGEX MATCHALL "FLAG [(]${_bank},${_bit}[)] = [01]" _fl "${_wp}")
    list(LENGTH _fl _n_fl)
    if(_n_fl LESS 2)
        _fehler("${_name}: FLAG (${_bank},${_bit}) nicht zweimal gelesen (Wegplan nicht zu Ende? wegplan.txt: ${_wp})")
    endif()
    list(GET _fl -1 _fl_letzt)
    if(NOT "${_fl_letzt}" STREQUAL "FLAG (${_bank},${_bit}) = ${_soll}")
        _fehler("${_name}: am Ende '${_fl_letzt}', erwartet 'FLAG (${_bank},${_bit}) = ${_soll}'")
    endif()
endmacro()

# Mindestens <_n> GESPERRT-Zeilen fuer Slot/floor bei Spielerband <_band>, und keine FREI-Zeile.
macro(_gesperrt _name _raum _slot _floor _band _n)
    string(REGEX MATCHALL "room=${_raum} ITEM slot ${_slot} floor ${_floor} [^\n]*spieler-band ${_band} [^\n]*-> GESPERRT" _ges "${_item}")
    list(LENGTH _ges _n_ges)
    if(_n_ges LESS ${_n})
        _fehler("${_name}: nur ${_n_ges} gesperrte Druecke an slot ${_slot} (floor ${_floor}) bei Band ${_band}, erwartet >= ${_n} (itemband.txt: ${_item})")
    endif()
    _darf_nicht_text("-> FREI" _item "${_name}: Aufnahme vom falschen Band")
endmacro()

macro(_frei _name _raum _slot _floor)
    string(REGEX MATCH "room=${_raum} ITEM slot ${_slot} floor ${_floor} [^\n]*spieler-band ${_floor} [^\n]*-> FREI" _fr "${_item}")
    if("${_fr}" STREQUAL "")
        _fehler("${_name}: keine Aufnahme (FREI) an slot ${_slot} auf Band ${_floor} (itemband.txt: ${_item})")
    endif()
    _darf_nicht_text("-> GESPERRT" _item "${_name}: auf dem vorgesehenen Weg gesperrt")
endmacro()

# ---- ROOM3050: der Nutzer-Fall ------------------------------------------------------------------------------------
set(_wp_oben3050 "OBJ 1" "RAUM 3050" "WARTE 30" "FLAG 9 92" "GEHE -6450 1523" "GEHE -6450 -20750" "GEHE -11880 -22540"
    "GEHE -16650 -22540" "GEHE -22720 -22540" "GEHE -24070 -22782 100" "BLICK 1024" "WARTE 20" "AKTION" "WARTE 60"
    "AKTION" "WARTE 60" "AKTION" "WARTE 100" "FLAG 9 92" "ENDE")
_lauf(oben3050 500 _wp_oben3050 RE15_DEBUG_JUMP=3050@250 "RE15_PLAYER_POS=-3596,1523,1024,4" "RE15_EXIT_AT=1250")
_muss("loaded room3050.rdt" _dbg "oben3050: ROOM3050 nicht geladen")
_gesperrt(oben3050 3050 6 1 4 3)
_flag_ende(oben3050 9 92 0)
message(STATUS "r37_itemband oben3050: Super Redhawk von Band 4 gesperrt (@0x80042ccc), (9,92) = 0")

set(_wp_weg3050 "OBJ 1" "RAUM 3050" "WARTE 30" "FLAG 3 82" "FLAG 3 93" "FLAG 9 92" "GEHE -6450 1523" "GEHE -6450 -20750"
    "GEHE -11880 -20500" "GEHE -16650 -20300 120" "SCHIEB 2048 X <= -24040 1500" "WARTE 40" "GEHE -22720 -22540"
    "GEHE -24058 -22150 100" "SCHIEB 3072 Z >= -17000 900" "WARTE 200" "GEHE -16650 -20300" "GEHE -11880 -20500"
    "GEHE -6450 -20750" "GEHE -6450 -8150" "GEHE -4400 -8150 120" "GEHE -1900 -8150 150" "BLICK 0" "WARTE 20" "AKTION"
    "WARTE 400" "FLAG 3 82" "GEHE -4400 -8150 120" "GEHE -8000 -8150" "GEHE -8000 -14200" "GEHE -20000 -14200"
    "GEHE -24058 -14250 100" "SCHIEB 1024 Z <= -22200 1500" "WARTE 60" "FLAG 3 93" "BLICK 1024" "WARTE 20" "AKTION"
    "WARTE 150" "GEHE -24058 -22900 100" "BLICK 1024" "WARTE 20" "AKTION" "WARTE 60" "AKTION" "WARTE 60" "AKTION"
    "WARTE 100" "FLAG 9 92" "FLAG 3 93" "ENDE")
_lauf(weg3050 900 _wp_weg3050 RE15_DEBUG_JUMP=3050@250 "RE15_PLAYER_POS=-3596,1523,1024,4" "RE15_EXIT_AT=4250")
_muss("MEMBER 0x12 Spieler: Kollisionsband 4 -> 0 (@0x80041228) y=0" _item "weg3050: der Lift sub04 (@0x023D0) setzt das Band nicht auf 0")
_muss("FLAG (3,82) = 1" _wp "weg3050: Lift sub04 nicht gefahren ((3,82))")
_muss("FLAG (3,93) = 1" _wp "weg3050: Kiste nicht in der Kerbe slot 7 @0x01F06 (Latch (3,93) sub01 @0x02184-2196)")
_frei(weg3050 3050 6 1)
_flag_ende(weg3050 9 92 1)
message(STATUS "r37_itemband weg3050: Fall -> Lift (Band 4 -> 0) -> Kerbe (3,93) -> Klettern -> Band 1 FREI -> (9,92) = 1")

string(REPLACE "RAUM 3050" "RAUM 3051" _wp_elza3051 "${_wp_oben3050}")
_lauf(elza3051 500 _wp_elza3051 RE15_PSELECT_AUTO_SWITCH=1 RE15_DEBUG_JUMP=3050@250 "RE15_PLAYER_POS=-3596,1523,1024,4"
      "RE15_EXIT_AT=1250")
_muss("loaded room3051.rdt" _dbg "elza3051: ROOM3051 (Elza) nicht geladen")
_gesperrt(elza3051 3051 6 1 4 3)
_flag_ende(elza3051 9 92 0)
message(STATUS "r37_itemband elza3051: Super Redhawk (ROOM3051, Elza) von Band 4 gesperrt, (9,92) = 0")

# ---- ROOM3010: Steg (Band 1) ----------------------------------------------------------------------------------------
set(_wp_unten3010 "OBJ 0" "RAUM 3010" "GEGNERWEG" "WARTE 30" "FLAG 9 20" "BLICK 0" "WARTE 20" "AKTION" "WARTE 60"
    "AKTION" "WARTE 60" "AKTION" "WARTE 100" "FLAG 9 20" "ENDE")
_lauf(unten3010 400 _wp_unten3010 RE15_DEBUG_JUMP=3010@250 "RE15_PLAYER_POS=-10700,844,0,0" "RE15_EXIT_AT=500")
_muss("loaded room3010.rdt" _dbg "unten3010: ROOM3010 nicht geladen")
_gesperrt(unten3010 3010 5 1 0 3)
_flag_ende(unten3010 9 20 0)
message(STATUS "r37_itemband unten3010: Schrotpatronen auf dem Steg von Band 0 (darunter) gesperrt, (9,20) = 0")

set(_wp_weg3010 "OBJ 0" "RAUM 3010" "GEGNERWEG" "WARTE 30" "FLAG 9 20" "GEHE -10400 -11100 120" "BLICK 3072" "WARTE 20"
    "AKTION" "WARTE 150" "GEHE -10700 -8000" "GEHE -10700 844 100" "BLICK 0" "WARTE 20" "AKTION" "WARTE 60" "AKTION"
    "WARTE 60" "AKTION" "WARTE 100" "FLAG 9 20" "ENDE")
_lauf(weg3010 500 _wp_weg3010 RE15_DEBUG_JUMP=3010@250 "RE15_PLAYER_POS=-7400,-18400,3072,0" "RE15_EXIT_AT=1050")
file(READ "${_sub}/climb.txt" _climb3010)
string(REGEX MATCH "room=3010 [^\n]*band=0 sub=9" _kl3010 "${_climb3010}")
if("${_kl3010}" STREQUAL "")
    _fehler("weg3010: kein Klettern (Substate 9) an r0[4] — climb.txt")
endif()
_frei(weg3010 3010 5 1)
_flag_ende(weg3010 9 20 1)
message(STATUS "r37_itemband weg3010: Klettern an r0[4] (Band 0 -> 1) -> FREI -> (9,20) = 1")

# ---- ROOM2090: Plattform (Band 1) ueber die echte Tuer aus 20A0 -------------------------------------------------------
set(_wp_weg2090 "OBJ 0" "RAUM 20A0" "GEGNERWEG" "WARTE 30" "BLICK 3072" "WARTE 20" "AKTION" "RAUM 2090" "GEGNERWEG"
    "WARTE 30" "FLAG 9 90" "BLICK 2048" "WARTE 20" "AKTION" "WARTE 60" "AKTION" "WARTE 60" "AKTION" "WARTE 100"
    "FLAG 9 90" "ENDE")
# Querbezug Runde 41 Spur F (Nachbesserung 2, Muster Orchestrator-Entscheid F-E1 "neue Regel widerlegt alten Riegel"):
# Nutzer (Nachtrag 21) "man kommt ueber die Leiter zum Aligator, nicht ueber das Tor. Das Tor soll verschlossen sein, und dann
# mit den Schluessel den man vom Aligator bekommt geoeffnet werden!" - das Tor 20A0 slot 0 @0x01746 ist bis Bit (9,113)
# "Tor aufgeschlossen" ein sce-3-Ereignis ('It's locked.', kein DOOR FIRE), und das erste Betreten von 2090 startet bis
# Bit (9,110) "Szene gesehen" die Ankunftsszene (VERTRAG 1.1). Der Weg ueber das Tor besteht also nur im Zustand nach
# Szene und Schluessel -> RE15_SET_FLAG=9:110,9:113. Gemessen 2026-10-25 (Dossier F_gatorszene.md N2.M1): ohne die Bits
# 'ROOM20A0 Slot 0 (Tor) gesperrt', 'Folge Z ROOM20A0 F84', kein DOOR FIRE; mit den Bits DOOR FIRE slot=0, 2090 geladen,
# 'ITEM slot 5 floor 1 item 0x3F tk 90 spieler-band 1 ... -> FREI', FLAG (9,90) 0 -> 1.
_lauf(weg2090 500 _wp_weg2090 RE15_SET_FLAG=9:110,9:113 RE15_DEBUG_JUMP=20A0@250 "RE15_PLAYER_POS=4950,4100,3072,2"
      "RE15_EXIT_AT=500#2090")
_muss("[aot] DOOR FIRE slot=0 rect=(4950,5000,hw=1500,hh=500) target_cut=2 spawn=(6300,-1800,-16250)" _dbg "weg2090: Tuer 20A0 slot 0 @0x01746 nicht ausgeloest")
_muss("loaded room2090.rdt" _dbg "weg2090: ROOM2090 nicht geladen")
_frei(weg2090 2090 5 1)
_flag_ende(weg2090 9 90 1)
message(STATUS "r37_itemband weg2090: Tuer 20A0 -> Plattform Band 1 -> Pocket Watch FREI -> (9,90) = 1")

# ---- ROOM2040: Halle (Band 3) -----------------------------------------------------------------------------------------
set(_wp_galerie2040 "OBJ 0" "RAUM 2040" "GEGNERWEG" "WARTE 30" "FLAG 9 91" "BLICK 1024" "WARTE 20" "AKTION" "WARTE 60"
    "AKTION" "WARTE 60" "AKTION" "WARTE 100" "FLAG 9 91" "ENDE")
_lauf(galerie2040 400 _wp_galerie2040 RE15_DEBUG_JUMP=2040@250 "RE15_PLAYER_POS=2200,-13800,1024,5" "RE15_EXIT_AT=500")
_gesperrt(galerie2040 2040 7 3 5 3)
_flag_ende(galerie2040 9 91 0)
message(STATUS "r37_itemband galerie2040: Head of Akuma von Band 5 gesperrt, (9,91) = 0")

set(_wp_weg2040 "OBJ 0" "RAUM 2040" "GEGNERWEG" "WARTE 30" "FLAG 9 91" "GEHE 5050 -5375" "GEHE 5050 -10000"
    "GEHE 2200 -10500" "GEHE 2200 -13800 100" "BLICK 1024" "WARTE 20" "AKTION" "WARTE 60" "AKTION" "WARTE 60" "AKTION"
    "WARTE 100" "FLAG 9 91" "ENDE")
_lauf(weg2040 500 _wp_weg2040 RE15_SET_FLAG=3:146,3:53 RE15_DEBUG_JUMP=2040@250 "RE15_PLAYER_POS=21700,-5400,2048,3"
      "RE15_EXIT_AT=1000")
_frei(weg2040 2040 7 3)
_flag_ende(weg2040 9 91 1)
message(STATUS "r37_itemband weg2040: Halle Band 3 -> Nordgang -> Head of Akuma FREI -> (9,91) = 1")

# ---- ROOM2030: Lift A (Band 6) und Treppe (Band 1) -------------------------------------------------------------------
set(_wp_spas2030 "OBJ 1" "RAUM 2030" "GEGNERWEG" "WARTE 30" "FLAG 9 109" "FLAG 3 222" "GEHE 6600 12000 100" "BLICK 3072"
    "WARTE 20" "AKTION" "WARTE 400" "FLAG 3 222" "GEHE 6600 13700 120" "GEHE 2000 14500" "GEHE -650 14500 100" "BLICK 0"
    "WARTE 20" "AKTION" "WARTE 60" "AKTION" "WARTE 60" "AKTION" "WARTE 100" "FLAG 9 109" "ENDE")
_lauf(spas2030 600 _wp_spas2030 RE15_SET_FLAG=3:126 RE15_DEBUG_JUMP=2030@250 "RE15_PLAYER_POS=6550,-100,3072,3"
      "RE15_EXIT_AT=1450")
_muss("MEMBER 0x12 Spieler: Kollisionsband 3 -> 6 (@0x80041228) y=-10800" _item "spas2030: Lift A sub02 (@0x01E66) setzt das Band nicht auf 6")
_frei(spas2030 2030 11 6)
_flag_ende(spas2030 9 109 1)
message(STATUS "r37_itemband spas2030: Lift A (Band 3 -> 6) -> SPAS-12 FREI -> (9,109) = 1")

set(_wp_spasb3 "OBJ 1" "RAUM 2030" "GEGNERWEG" "WARTE 30" "FLAG 9 109" "BLICK 0" "WARTE 20" "AKTION" "WARTE 60" "AKTION"
    "WARTE 60" "AKTION" "WARTE 100" "FLAG 9 109" "ENDE")
_lauf(spasb3 400 _wp_spasb3 RE15_DEBUG_JUMP=2030@250 "RE15_PLAYER_POS=-650,14500,0,3" "RE15_EXIT_AT=500")
_gesperrt(spasb3 2030 11 6 3 3)
_flag_ende(spasb3 9 109 0)
message(STATUS "r37_itemband spasb3: SPAS-12 von Band 3 gesperrt, (9,109) = 0")

set(_wp_schrot2030 "OBJ 1" "RAUM 2030" "GEGNERWEG" "WARTE 30" "FLAG 9 108" "GEHE 6575 -9675" "GEHE -6800 -9675"
    "GEHE -6350 -7700 80" "BLICK 0" "WARTE 20" "AKTION" "WARTE 300" "GEHE -1050 -7700" "GEHE -1050 7100"
    "GEHE 4300 7100 100" "BLICK 0" "WARTE 20" "AKTION" "WARTE 60" "AKTION" "WARTE 60" "AKTION" "WARTE 100" "FLAG 9 108"
    "ENDE")
_lauf(schrot2030 600 _wp_schrot2030 RE15_SET_FLAG=9:101,9:102 RE15_DEBUG_JUMP=2030@250 "RE15_PLAYER_POS=6550,-100,1024,3" "RE15_EXIT_AT=1650")
# RE15_SET_FLAG 9:101,9:102 = Runde 41 Spur C: die Bruecke (obj 0 @0x01B6C) steht bis zum Pult-Raetsel oben und die Steg-Zelle 12
# ist gesperrt (re15_ventilkette_r41.h); dieser Lauf prueft die Band-Regel, nicht die Bruecke -> Bruecke unten vorgesetzt.
set(_stair "")
if(EXISTS "${_sub}/stair_dbg.log")
    file(READ "${_sub}/stair_dbg.log" _stair)
endif()
_muss("[stair] START dir=DOWN cur_band=3 target_band=1" _stair "schrot2030: Treppe slot 8 @0x01B8E fuehrt nicht von Band 3 auf Band 1")
_frei(schrot2030 2030 10 1)
_flag_ende(schrot2030 9 108 1)
message(STATUS "r37_itemband schrot2030: Treppe (Band 3 -> 1) -> Gang -> Schrotpatronen FREI -> (9,108) = 1")

set(_wp_schrotb3 "OBJ 1" "RAUM 2030" "GEGNERWEG" "WARTE 30" "FLAG 9 108" "BLICK 0" "WARTE 20" "AKTION" "WARTE 60"
    "AKTION" "WARTE 60" "AKTION" "WARTE 100" "FLAG 9 108" "ENDE")
_lauf(schrotb3 400 _wp_schrotb3 RE15_DEBUG_JUMP=2030@250 "RE15_PLAYER_POS=4300,7100,0,3" "RE15_EXIT_AT=500")
_gesperrt(schrotb3 2030 10 1 3 3)
_flag_ende(schrotb3 9 108 0)
message(STATUS "r37_itemband schrotb3: Schrotpatronen von Band 3 gesperrt, (9,108) = 0")

file(REMOVE "${_exe}")
message(STATUS "r37_itemband: alle Laeufe bestanden")
