cmake_minimum_required(VERSION 3.16)
# =============================================================================
# Runde 37 Spur D "itemband", NACHBESSERUNG 1 — ECHTE exe. Dossier: analysis/befunde_runde37/D_itemband.md (Nachbesserung 1).
#
# Abnahme 0 (D_abnahme_0.md Maengel 1/2/4): Mit Member 0x12 = Kollisionsband (@0x80041228) nimmt Lift A in ROOM2030/2031
# den Spieler auf das Zielband mit, wo immer er beim Druck stand. Der Ausloeser (sub00 @0x01CE6 slot 5 / @0x01D20 slot 4,
# sat 0x31 = nur Punkt 620 voraus, FUN_80042bac @0x80042ef8) feuert auch aus dem Kabineneingang (Blick Sued) und knapp
# suedlich der oberen Kabine (Blick Nord); die Ausschiebung aus r0[49] / r0[33] setzte den Spieler dann hinter die
# Kabinenwand (10182 / 13768) — Softlock, SPAS-12 unerreichbar bzw. kein Ausgang (im Original identisch, Beta).
# PORT-WAHL nach RE2 Retail ROOM6030 (Retail-Fassung von ROOM50D0): Plattformzone sub00 @0x02B9C sat 0x41 (Mitte) +
# sub10 @0x02F70-7A `if Ck(5,0x22)==0 Evt_end` -> Lift A faehrt nur mit der Spielermitte im Ausloese-Rechteck
# (re15_itemband_r37_liftsperre, include/re15_itemband_r37.h (3)).
#
# Laeufe (RE15_WEGPLAN, Start an der Ankunft der 2070-Tuer (6550,-100) Band 3, Strom (3,126)):
#   liftauf2030   Anlauf von Norden bis (6600,10800), Blick Sued, AKTION -> KEINE Fahrt ((3,222) bleibt 0, Messschrieb
#                 "LIFT slot 5 ... mitte im Ausloeser nein -> GESPERRT"); in die Kabine (6600,12000), AKTION -> Fahrt
#                 (MEMBER 0x12 3 -> 6, (3,222) = 1); Kabine nach Sueden verlassen -> SPAS-12 FREI auf Band 6, (9,109) = 1.
#   liftab2030    aus der Kabine hoch, nach Sueden bis (6600,13300), Blick Nord, AKTION -> KEINE Fahrt ((3,222) bleibt 1);
#                 in die Kabine, AKTION -> Fahrt ab (MEMBER 0x12 6 -> 3, (3,222) = 0); nach Norden zur Tuer slot 0 ->
#                 DOOR FIRE slot=0 -> ROOM2070.
#   liftauf2031 / liftab2031   dasselbe mit Elza (RE15_PSELECT_AUTO_SWITCH=1, ROOM2031/2071).
# GEGENPROBE (Dossier NB1.6): derselbe Test gegen die exe VOR der Nachbesserung (05117c03) ist rot — schon am
# Verhalten ((3,222)-Folge 0;1;1 statt 0;0;1 bzw. 0;1;0;.. statt 0;1;1;0), nicht erst am Messschrieb.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r37_itemband_lift.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_itemband_lift: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r37_itemband_lift: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_basis}")

# Eigener exe-Name neben der exe (Muster test_r34n_b_cursor.cmake: fremde `taskkill /IM re15_pc.exe`).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37d_lift_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r37d_lift_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

function(_fehler _was)
    file(REMOVE "${_exe}")
    message(FATAL_ERROR "r37_itemband_lift: ${_was}")
endfunction()

set(_grund RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
           RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_STAIR_DBG=1
           RE15_CLIMB_TRACE=climb.txt RE15_ITEMBAND_LOG=itemband.txt RE15_WEGPLAN_LOG=wegplan.txt)

macro(_lauf _name _timeout _plan)
    set(_sub "${_basis}/${_name}")
    file(MAKE_DIRECTORY "${_sub}")
    string(REPLACE ";" "\n" _plan_text "${${_plan}}")
    file(WRITE "${_sub}/wegplan_eingabe.txt" "${_plan_text}\n")
    set(WORKDIR "${_sub}")
    re15_start_spiel(_rv ${_timeout} ${_grund} "RE15_WEGPLAN=${_sub}/wegplan_eingabe.txt" ${ARGN} "${_exe}")
    if("${_rv}" STREQUAL "1" OR "${_rv}" STREQUAL "-1" OR NOT EXISTS "${_sub}/debug.log")
        message(STATUS "r37_itemband_lift[${_name}]: Lauf ohne Ergebnis (exit=${_rv}) -> EIN Wiederholungsversuch")
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

macro(_muss_regex _re _var _was)
    string(REGEX MATCH "${_re}" _treffer "${${_var}}")
    if("${_treffer}" STREQUAL "")
        _fehler("${_was} — kein Treffer fuer '${_re}' (${_var}: ${${_var}})")
    endif()
endmacro()

# Folge ALLER Wegplan-Lesungen von FLAG (Bank,Bit) muss genau <_soll> sein (Liste, z.B. "0;0;1").
macro(_flagfolge _name _bank _bit _soll)
    string(REGEX MATCHALL "FLAG [(]${_bank},${_bit}[)] = [01]" _fl "${_wp}")
    set(_ist "")
    foreach(_e IN LISTS _fl)
        string(REGEX REPLACE ".* = " "" _w "${_e}")
        list(APPEND _ist "${_w}")
    endforeach()
    if(NOT "${_ist}" STREQUAL "${_soll}")
        _fehler("${_name}: FLAG (${_bank},${_bit}) gelesen '${_ist}', erwartet '${_soll}' (wegplan.txt: ${_wp})")
    endif()
endmacro()

# Aufnahme des Items (slot, floor) auf seinem eigenen Band; kein ITEM-Druck dieses Laufs gesperrt.
macro(_frei _name _raum _slot _floor)
    string(REGEX MATCH "room=${_raum} ITEM slot ${_slot} floor ${_floor} [^\n]*spieler-band ${_floor} [^\n]*-> FREI" _fr "${_item}")
    if("${_fr}" STREQUAL "")
        _fehler("${_name}: keine Aufnahme (FREI) an slot ${_slot} auf Band ${_floor} (itemband.txt: ${_item})")
    endif()
    string(REGEX MATCH "ITEM slot [^\n]*-> GESPERRT" _ig "${_item}")
    if(NOT "${_ig}" STREQUAL "")
        _fehler("${_name}: ITEM-Druck auf dem vorgesehenen Weg gesperrt: '${_ig}'")
    endif()
endmacro()

macro(_flag_ende _name _bank _bit _soll)
    string(REGEX MATCHALL "FLAG [(]${_bank},${_bit}[)] = [01]" _fl "${_wp}")
    list(LENGTH _fl _n_fl)
    if(_n_fl LESS 2)
        _fehler("${_name}: FLAG (${_bank},${_bit}) nicht zweimal gelesen (wegplan.txt: ${_wp})")
    endif()
    list(GET _fl -1 _fl_letzt)
    if(NOT "${_fl_letzt}" STREQUAL "FLAG (${_bank},${_bit}) = ${_soll}")
        _fehler("${_name}: am Ende '${_fl_letzt}', erwartet 'FLAG (${_bank},${_bit}) = ${_soll}'")
    endif()
endmacro()

# ---- Wegplaene (= analysis/befunde_runde37/D_belege/nb1/wp_auf_eingang.txt / wp_ab_sued.txt) ------------------------
set(_wp_auf "OBJ 1" "RAUM 2030" "GEGNERWEG" "WARTE 30" "FLAG 9 109" "FLAG 3 222" "GEHE 6600 10800 60" "BLICK 3072"
    "WARTE 20" "AKTION" "WARTE 400" "FLAG 3 222" "GEHE 6600 12000 100" "BLICK 3072" "WARTE 20" "AKTION" "WARTE 400"
    "FLAG 3 222" "GEHE 6600 13700 120" "GEHE 2000 14500" "GEHE -650 14500 100" "BLICK 0" "WARTE 20" "AKTION" "WARTE 60"
    "AKTION" "WARTE 60" "AKTION" "WARTE 100" "FLAG 9 109" "ENDE")
set(_wp_ab "OBJ 1" "RAUM 2030" "GEGNERWEG" "WARTE 30" "FLAG 3 222" "GEHE 6600 12000 100" "BLICK 3072" "WARTE 20" "AKTION"
    "WARTE 400" "FLAG 3 222" "GEHE 6600 13300 60" "BLICK 1024" "WARTE 20" "AKTION" "WARTE 400" "FLAG 3 222"
    "GEHE 6600 12000 100" "BLICK 1024" "WARTE 20" "AKTION" "WARTE 400" "FLAG 3 222" "GEHE 6600 2100 150"
    "GEHE 6550 -100 150" "BLICK 0" "WARTE 20" "AKTION" "RAUM 2070" "WARTE 30" "ENDE")

# ---- je Figur: aufwaerts aus dem Eingang, abwaerts von suedlich der oberen Kabine --------------------------------------
foreach(_fig leon elza)
    if(_fig STREQUAL "leon")
        set(_r 2030)
        set(_r_aus 2070)
        set(_fig_env "")
        set(_wp_auf_f "${_wp_auf}")
        set(_wp_ab_f "${_wp_ab}")
    else()
        set(_r 2031)
        set(_r_aus 2071)
        set(_fig_env RE15_PSELECT_AUTO_SWITCH=1)
        string(REPLACE "RAUM 2030" "RAUM 2031" _wp_auf_f "${_wp_auf}")
        string(REPLACE "RAUM 2030" "RAUM 2031" _wp_ab_f "${_wp_ab}")
        string(REPLACE "RAUM 2070" "RAUM 2071" _wp_ab_f "${_wp_ab_f}")
    endif()

    _lauf(liftauf${_r} 600 _wp_auf_f ${_fig_env} RE15_SET_FLAG=3:126 RE15_DEBUG_JUMP=2030@250
          "RE15_PLAYER_POS=6550,-100,3072,3" "RE15_EXIT_AT=1800#${_r}")
    _muss("loaded room${_r}.rdt" _dbg "liftauf${_r}: ROOM${_r} nicht geladen")
    # Verhalten zuerst: der Druck im Eingang faehrt NICHT ((3,222) bleibt 0), der Druck in der Kabine faehrt.
    _flagfolge(liftauf${_r} 3 222 "0;0;1")
    _frei(liftauf${_r} ${_r} 11 6)
    _flag_ende(liftauf${_r} 9 109 1)
    # Mechanismus: Sperre mit der Spielermitte ausserhalb (Eingang z 107xx/108xx), Fahrt mit der Mitte in der Kabine.
    _muss_regex("room=${_r} LIFT slot 5 sub 2 spieler=[(]6[0-9][0-9][0-9],-5400,10[78][0-9][0-9][)] band 3 mitte im Ausloeser nein -> GESPERRT" _item
                "liftauf${_r}: keine Sperre am Kabineneingang")
    _muss_regex("room=${_r} LIFT slot 5 sub 2 spieler=[(]6[0-9][0-9][0-9],-5400,1[12][0-9][0-9][0-9][)] band 3 mitte im Ausloeser ja -> FAEHRT" _item
                "liftauf${_r}: keine Fahrt aus der Kabine")
    _muss("room=${_r} MEMBER 0x12 Spieler: Kollisionsband 3 -> 6 (@0x80041228) y=-10800" _item
          "liftauf${_r}: Lift A sub02 (@0x01E66) setzt das Band nicht auf 6")
    message(STATUS "r37_itemband_lift liftauf${_r} (${_fig}): Eingang GESPERRT, Kabine FAEHRT (Band 3 -> 6), SPAS-12 FREI, (9,109) = 1")

    _lauf(liftab${_r} 600 _wp_ab_f ${_fig_env} RE15_SET_FLAG=3:126 RE15_DEBUG_JUMP=2030@250
          "RE15_PLAYER_POS=6550,-100,3072,3" "RE15_EXIT_AT=2600")
    _muss("loaded room${_r}.rdt" _dbg "liftab${_r}: ROOM${_r} nicht geladen")
    _flagfolge(liftab${_r} 3 222 "0;1;1;0")
    _muss("[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750)" _dbg "liftab${_r}: Tuer slot 0 nicht erreicht")
    _muss("loaded room${_r_aus}.rdt" _dbg "liftab${_r}: ROOM${_r_aus} nicht geladen")
    _muss_regex("room=${_r} LIFT slot 4 sub 2 spieler=[(]6[0-9][0-9][0-9],-10800,13[0-9][0-9][0-9][)] band 6 mitte im Ausloeser nein -> GESPERRT" _item
                "liftab${_r}: keine Sperre suedlich der oberen Kabine")
    _muss("room=${_r} MEMBER 0x12 Spieler: Kollisionsband 6 -> 3 (@0x80041228) y=-5400" _item
          "liftab${_r}: Lift A sub02 (@0x01EA8) setzt das Band nicht auf 3")
    message(STATUS "r37_itemband_lift liftab${_r} (${_fig}): suedlich der Kabine GESPERRT, Kabine FAEHRT (Band 6 -> 3), Tuer slot 0 -> ROOM${_r_aus}")
endforeach()

file(REMOVE "${_exe}")
message(STATUS "r37_itemband_lift: alle Laeufe bestanden")
