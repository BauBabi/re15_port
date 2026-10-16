cmake_minimum_required(VERSION 3.16)
# =============================================================================
# RUNDE 38 SPUR F "dining1141" MIT DER ECHTEN EXE. Dossier analysis/befunde_runde38/F_dining1141.md, Konstanten
# include/re15_dining_r38.h. Die Unit-Riegel unit_r38_dining1141_* pruefen die Mechanik am Engine-Code; dieser Riegel
# prueft den Nutzerweg an der exe (Debug-Sprung in den ERSTEN Raum, danach nur echte Tueren per RE15_WEGPLAN):
#
#   W  ELZA 1021 -> 1011 -> 1021: Roys Leiche zeigt msg 40 (Hinweis), Tuer Slot 1 -> ROOM1011, der Dining Room Key
#      liegt an Roys Liegestelle und wird aufgenommen ((9,64)), Tuer Slot 0 -> ROOM1021: dort steht kein John
#   Z  ELZA 1131 ohne Schluessel: Tuer -> msg 40 "It's locked...", kein Raumwechsel
#   D  ELZA 1131 -> 1141 -> 1171 mit Schluessel: msg 41, Wegwerf-Abfrage, Tuer -> 1141; Phase H (John ab Bild 0 hinter
#      dem Rolladen, Klopfen/Help/Musik als Ton, keine Nachricht); Gemaelde -> Rolladen-Ton, Szene ohne msg 0/1/2,
#      Tuersperre msg 41 an der Doppeltuer; Kampfende (Messhaken RE15_R38F_GORILLAS_TOT) -> Johns Abgang in Cut 7;
#      Dachtuer -> ROOM1171. Zustands-Log: John nie gesprungen, beim Parken hinter der Kamera von Cut 7.
#   L  LEON 1140: Gemaelde -> Rolladen-Ton 0x3B 0x0C/0x0A/0x0B (Punkt 3)
#
# Druckzahlen je Abfrage (AKTION + WARTE 60) gemessen mit `test_r38_dining1141 takt`: Gemaelde 1+3, Tuer 1131 mit
# Schluessel 1+2 (msg 41 + Wegwerf-Abfrage; an der exe gemessen 1+3, darum 5 Drucke — ein zusaetzlicher Druck geht
# durch die nun offene Tuer, die restlichen Zeilen laufen in 1141 am Eintrittspunkt ins Leere), Leiche 1021 1+4. Ein Druck zu viel loest den Satz neu aus (Leiche: sub06
# @0x02216 stellt Slot 6 wieder scharf).
# Bilder je Lauf per RE15_FRAMEDUMP (RE15_WINDOW_SCALE=1) und Ton-Mitschnitt RE15_AUDIO_CAP_SYNC im Arbeitsordner;
# geprueft werden debug.log, state.txt und wegplan_log.txt.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> [-DTEIL=W|Z|D|L] -P test_r38_dining1141.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r38_dining1141: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r38_dining1141: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL "WZDL")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# EIGENER EXE-NAME neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r38f_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r38f_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

set(_fehler "")
macro(pruef _name _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(_p LESS 0)
        list(APPEND _fehler "${_name}: fehlt '${_muster}'")
        message(STATUS "FAIL ${_name}: fehlt '${_muster}'")
    else()
        message(STATUS "ok   ${_name}: '${_muster}'")
    endif()
endmacro()
macro(pruef_nicht _name _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(NOT _p LESS 0)
        list(APPEND _fehler "${_name}: unerwartet '${_muster}'")
        message(STATUS "FAIL ${_name}: unerwartet '${_muster}'")
    else()
        message(STATUS "ok   ${_name}: nicht da '${_muster}'")
    endif()
endmacro()
macro(pruef_wahr _name _bed _text)
    if(${_bed})
        message(STATUS "ok   ${_name}: ${_text}")
    else()
        list(APPEND _fehler "${_name}: ${_text}")
        message(STATUS "FAIL ${_name}: ${_text}")
    endif()
endmacro()

# Ein Lauf: Wegplan schreiben, exe starten, debug.log + wegplan_log.txt lesen.
#   _figur  ELZA|LEON   _plan  Wegplan-Text   _extra  weitere KEY=WERT (Liste)
function(f_lauf _name _figur _ende _plan _extra _dbg_out _wp_out)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(WRITE "${WORKDIR}/wegplan.txt" "${_plan}")
    set(_figur_env RE15_PSELECT_AUTO=1)
    if(_figur STREQUAL "ELZA")
        list(APPEND _figur_env RE15_PSELECT_AUTO_SWITCH=1)
    endif()
    re15_start_spiel(_rv 3000
        RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500 ${_figur_env}
        RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_EVT_TRACE=1 RE15_SPAWN_DIAG=1 RE15_BGM_CTL_DEBUG=1
        RE15_STATE_LOG=state.txt RE15_AUDIO_CAP_SYNC=cap.raw
        "RE15_WEGPLAN=${WORKDIR}/wegplan.txt" RE15_WEGPLAN_LOG=wegplan_log.txt
        "RE15_EXIT_AT=${_ende}"
        ${_extra}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r38_dining1141[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    set(_wp "")
    if(EXISTS "${WORKDIR}/wegplan_log.txt")
        file(READ "${WORKDIR}/wegplan_log.txt" _wp)
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r38_dining1141[${_name}]: Endbild nicht erreicht - Lauf vorher abgerissen (exit=${_rv}, ${WORKDIR})")
    endif()
    message(STATUS "r38_dining1141[${_name}]: exit=${_rv}, ${WORKDIR}")
    set(${_dbg_out} "${_dbg}" PARENT_SCOPE)
    set(${_wp_out} "${_wp}" PARENT_SCOPE)
endfunction()

# Nach-Szene-Flags der 1021-Szene (E13, analysis/story_leon_elza/ablauf_elza.md): (3,0x5F) Leiche/Szene gelaufen
# (ROOM1021 sub02 @0x01FE4), (4,0xF0) John-Szene 1141 offen (@0x01FE0), (4,0xF1) erster 1011-Besuch (ROOM1011 sub02
# @0x00AFE), (4,0xEC)/(3,0x78) Prison key genommen (ROOM1011 sub06 @0x00E2C/@0x00E30).
set(_nach1021 "RE15_SET_FLAG=3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78")

# --- W: 1021 -> 1011 -> 1021 ------------------------------------------------------------------------------------
# Leiche Slot 6 main00 @0x01E2C (x -4850..-3850, z -10100..-9100): Vorwaerts-Punkt (-4350,-9600) von (-3730,-9600)
# Blick -X. Tuer Slot 1 @0x01CA2 (x -3700..-2200, z -19600..-17300) -> ROOM1011 Spawn (3650,0,6900); zurueck ueber
# ROOM1011 Slot 0 @0x008AE (x 4150..5150, z 5900..7900) -> ROOM1021 Spawn (-4000,0,-18000).
if(TEIL MATCHES "W")
    f_lauf(w ELZA "900#1021" "RAUM 1021
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
GEHE -3900 -18450 100
BLICK 0
AKTION
RAUM 1011
WARTE 20
GEHE -1380 7000 120
GEHE -1380 7450 80
BLICK 2048
AKTION
WARTE 90
AKTION
WARTE 90
AKTION
WARTE 60
FLAG 9 64
GEHE 3600 6900 100
BLICK 0
AKTION
RAUM 1021
WARTE 120
ENDE
" "${_nach1021};RE15_DEBUG_JUMP=1021@gp;RE15_PLAYER_POS=-3730,-9600,2048;RE15_FRAMEDUMP=0-900/15:fd_" _w _wpw)
    pruef(W _w "[msg] room=1021 id=40")
    pruef(W _w "[dining] ROOM1011: Dining Room Key (Item 0x43, Bit (9,64)) Prop obj 6 bei (-2000,0,7450), Zone Slot 14")
    pruef(W _wpw "FLAG (9,64) = 1")
    # zurueck in 1021: kein John (Original: Spawn nur bei (4,0x60)==1, main00 @0x01DF6)
    string(FIND "${_w}" "loaded room1021.rdt" _p1)
    string(FIND "${_w}" "loaded room1021.rdt" _p2 REVERSE)
    pruef_wahr(W "_p2 GREATER _p1" "ROOM1021 zweimal geladen (Hin- und Rueckweg ueber Tueren)")
    string(SUBSTRING "${_w}" ${_p2} -1 _w_zurueck)
    pruef_nicht(W _w_zurueck "type=0x49")
endif()

# --- Z: 1131 ohne Schluessel ----------------------------------------------------------------------------------
# Tuer Slot 0 main00 @0x0088E (x -800..200, z -15900..-11900); Standplatz = Ruecksprung-Spawn aus 1141
# (-1300,0,-13950), Blick +X -> Vorwaerts-Punkt (-680,-13900).
if(TEIL MATCHES "Z")
    f_lauf(z ELZA "400#1131" "RAUM 1131
AKTION
WARTE 60
AKTION
WARTE 60
ENDE
" "${_nach1021};RE15_DEBUG_JUMP=1131@gp;RE15_PLAYER_POS=-1300,-13900,0;RE15_FRAMEDUMP=0-400/20:fd_" _z _wpz)
    pruef(Z _z "ROOM1131 Slot 0 (Tuer -> ROOM1141) gesperrt: Ereignis 29")
    pruef(Z _z "ROOM1131: Tuer zum Besprechungsraum verschlossen (kein Dining Room Key im Inventar)")
    # (die Tuertexte zeigt der Tuer-Handler direkt, re15_scd_show_message — ohne "[msg]"-Zeile; Zustands-Log zeigt id 40)
    file(READ "${_basis}_z/state.txt" _zst)
    pruef(Z _zst "msg(a=1 fsm=1 id=40")
    pruef_nicht(Z _z "loaded room1141.rdt")
endif()

# --- D: 1131 -> 1141 -> 1171 ----------------------------------------------------------------------------------
# 1141: Eintritt (-7600,0,-17600); Gemaelde Aot slot 3 sub00 @0x00B88 (x -9000..-8000) von (-8150,-10250) Blick -X
# (Wandabstand 468: SCA-Westwand bis x -8600); Doppeltuer Slot 0 @0x00A52 (x -9100..-8100, z -19750..-15750) von
# (-7900,-17750) Blick -X; Dachtuer Slot 1 @0x00A7A/@0x00AB2 (z -750..250) von (-7250,-1000) Blick +Z.
# Kampf: Gorillas eingefroren (RE15_R38F_GORILLAS_STARR) und nach 600 freien Kampfbildern tot (RE15_R38F_GORILLAS_TOT).
if(TEIL MATCHES "D")
    f_lauf(d ELZA "60#1171" "RAUM 1131
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
BLICK 0
AKTION
RAUM 1141
WARTE 600
GEHE -7600 -12000 150
GEHE -8150 -10250 80
BLICK 2048
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 120
GEHE -7900 -15000 150
GEHE -7900 -17750 80
BLICK 2048
AKTION
WARTE 45
AKTION
WARTE 20
GEHE -5500 -19000 150
WARTE 700
GEHE -6000 -12000 150
GEHE -7350 -7600 150
GEHE -7250 -1000 80
BLICK 3072
AKTION
RAUM 1171
WARTE 30
ENDE
" "${_nach1021};RE15_GIVE=0x43:1;RE15_DEBUG_JUMP=1131@gp;RE15_PLAYER_POS=-1300,-13900,0;RE15_R38F_GORILLAS_TOT=600;RE15_R38F_GORILLAS_STARR=1;RE15_FRAMEDUMP=90-6000/15:fd_" _d _wpd)
    pruef(D _d "[give] item 67 x1")
    pruef(D _d "ROOM1131: Dining Room Key benutzt -> Tuer offen, Flag (9,66) = 1")
    file(READ "${_basis}_d/state.txt" _dst)
    pruef(D _dst "msg(a=1 fsm=1 id=41")
    pruef(D _d "loaded room1141.rdt")
    pruef(D _d "ROOM1141 Phase H: John im Dachgang hinter dem Rolladen (-7124,0,-6150) Blick 1024")
    pruef(D _d "Se_on bank=0x3C id=10 -> Runde 38 Spur F")
    pruef(D _d "Se_on bank=0x3D id=0 -> Runde 38 Spur F (Stimme main42.wav)")
    pruef(D _d "[voice] clip loaded: main42.wav")
    pruef(D _d "Se_on bank=0x3B id=12 -> Runde 38 Spur F")
    pruef(D _d "Se_on bank=0x3B id=10 -> Runde 38 Spur F")
    pruef(D _d "Se_on bank=0x3B id=11 -> Runde 38 Spur F")
    pruef(D _d "ROOM1141 Gemaelde (Ereignis 6) in Phase H -> Programm BEFREIUNG")
    pruef(D _d "[msg] room=1141 id=40")
    pruef(D _d "[msg] room=1141 id=11")
    pruef_nicht(D _d "[msg] room=1141 id=0 ")
    pruef_nicht(D _d "[msg] room=1141 id=1 ")
    pruef_nicht(D _d "[msg] room=1141 id=2 ")
    pruef(D _d "MESSHAKEN RE15_R38F_GORILLAS_TOT")
    pruef(D _d "Se_on bank=14 id=0 -> RE2-Tuerbank")
    pruef(D _d "Se_on bank=14 id=1 -> RE2-Tuerbank")
    pruef(D _d "loaded room1171.rdt")
    # Erste Nachricht in 1141 ist die Gemaelde-Frage (msg 13): in Phase H erscheint KEIN Text (Help/Klopfen nur Ton).
    string(FIND "${_d}" "[msg] room=1141 id=" _pm)
    if(NOT _pm LESS 0)
        string(SUBSTRING "${_d}" ${_pm} 24 _erste)
        pruef_wahr(D "\"${_erste}\" MATCHES \"id=13 \"" "erste Nachricht in 1141 = msg 13 (Gemaelde), vorher kein Text")
    endif()
    # Zustands-Log 1141: John (t=49) ab Bild 0 da, nie gesprungen, beim Parken hinter Cut 7 (z > -9613).
    set(_st "${_basis}_d/state.txt")
    if(EXISTS "${_st}")
        file(STRINGS "${_st}" _zeilen)
        set(_in1141 0)
        set(_erst "")
        set(_px "")
        set(_pz "")
        set(_sprung 0)
        set(_park "")
        set(_sperre 0)
        foreach(_z IN LISTS _zeilen)
            # Tuersperre im Kampf: msg 41 offen, waehrend John (t=49) im Raum ist = ROOM1141 (in 1131 gibt es keinen John)
            if(_z MATCHES "msg\\(a=1 fsm=1 id=41 " AND _z MATCHES "\\[1 t=49 ")
                set(_sperre 1)
            endif()
            if(_z MATCHES "^F0 ")
                set(_in1141 0)
            endif()
            if(_z MATCHES "\\[1 t=49 [^@]*@\\((-?[0-9]+),(-?[0-9]+),")
                set(_x ${CMAKE_MATCH_1})
                set(_zz ${CMAKE_MATCH_2})
                string(REGEX MATCH "cam=([0-9]+)" _c "${_z}")
                set(_cam ${CMAKE_MATCH_1})
                if(_erst STREQUAL "" AND _x EQUAL -7124 AND _zz EQUAL -6150)
                    set(_erst "${_z}")
                    set(_in1141 1)
                endif()
                if(_in1141)
                    if(_x EQUAL -30000 AND _park STREQUAL "")
                        set(_park "cam=${_cam} vorher=(${_px},${_pz})")
                        if(NOT _cam EQUAL 7 OR _pz LESS -9613)
                            set(_sprung 99)
                        endif()
                    elseif(NOT _x EQUAL -30000 AND NOT _px STREQUAL "")
                        math(EXPR _d1 "${_x} - (${_px})")
                        math(EXPR _d2 "${_zz} - (${_pz})")
                        if(_d1 LESS 0)
                            math(EXPR _d1 "-(${_d1})")
                        endif()
                        if(_d2 LESS 0)
                            math(EXPR _d2 "-(${_d2})")
                        endif()
                        math(EXPR _s "${_d1} + ${_d2}")
                        if(_s GREATER 400)
                            math(EXPR _sprung "${_sprung} + 1")
                            message(STATUS "     Sprung: ${_z}")
                        endif()
                    endif()
                    if(NOT _x EQUAL -30000)
                        set(_px ${_x})
                        set(_pz ${_zz})
                    endif()
                endif()
            endif()
        endforeach()
        string(SUBSTRING "${_erst}" 0 4 _erst_bild)
        pruef_wahr(D "\"${_erst_bild}\" MATCHES \"^F[01] \"" "John steht ab dem ersten Bild in 1141 bei (-7124,-6150) (${_erst_bild})")
        pruef_wahr(D "_sprung EQUAL 0" "John springt nie und wird hinter der Kamera von Cut 7 geparkt (${_park})")
        pruef_wahr(D "NOT \"${_park}\" STREQUAL \"\"" "John wurde beim Abgang geparkt (${_park})")
        pruef_wahr(D "_sperre EQUAL 1" "Doppeltuer im Kampf -> msg 41 \"I cannot leave him alone with those monsters.\"")
    else()
        list(APPEND _fehler "D: kein state.txt")
    endif()
endif()

# --- L: Leon 1140 Rolladen-Ton ----------------------------------------------------------------------------------
if(TEIL MATCHES "L")
    f_lauf(l LEON "800#1140" "RAUM 1140
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 450
ENDE
" "RE15_SET_FLAG=2:0;RE15_DEBUG_JUMP=1140@gp;RE15_PLAYER_POS=-8150,-10250,2048;RE15_FRAMEDUMP=0-800/20:fd_" _l _wpl)
    pruef(L _l "Se_on bank=0x3B id=12 -> Runde 38 Spur F (ROOM1131-snd0 Rolladen)")
    pruef(L _l "Se_on bank=0x3B id=10 -> Runde 38 Spur F (ROOM1131-snd0 Rolladen)")
    pruef(L _l "Se_on bank=0x3B id=11 -> Runde 38 Spur F (ROOM1131-snd0 Rolladen)")
    pruef(L _l "[dining] Port-Bank 0x3B geladen")
    pruef_nicht(L _l "Phase H")
endif()

file(REMOVE "${_exe_kopie}")
list(LENGTH _fehler _nf)
if(_nf GREATER 0)
    string(REPLACE ";" "\n  " _fl "${_fehler}")
    message(FATAL_ERROR "r38_dining1141: ${_nf} Fehler:\n  ${_fl}")
endif()
message(STATUS "r38_dining1141: alle Pruefungen gruen (TEIL ${TEIL})")
