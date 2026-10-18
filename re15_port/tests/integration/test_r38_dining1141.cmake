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
#   R  ELZA 1021 mit der ECHTEN Szene (Nachbesserung 1 M1): danach Roys Leiche im SELBEN Besuch -> msg 40
#   A  ELZA Johns Abgang (Nachbesserung 1 M3): Elza auf Johns Bahn -> er wartet; sie geht in den Dachgang -> Abgang, Elza
#      unverrueckt (kein Schub), John geparkt, Dachtuer -> 1171
#   S  ELZA wie D bis zur Dachtuer, ZWEI Laeufe (mit/ohne Johns Bild), jedes Bild von 1141 zurueckgelesen und verglichen:
#      John erscheint/verschwindet in keinem Bild ohne Bewegung (Nachtrag 2; Werkzeug test_r38_dining1141 sicht)
#
# Druckzahlen je Abfrage (AKTION + WARTE 60) gemessen mit `test_r38_dining1141 takt`: Gemaelde 1+3, Tuer 1131 mit
# Doppeltuer im Kampf 1+1 mit WARTE 90 (zwei Zeilen; nach WARTE 45 lief die Schrift noch, der Druck beschleunigte
# nur — gemessen Lauf int_d2: msg 41 blieb ab Bild 2930 offen), Schluessel 1+2 (msg 41 + Wegwerf-Abfrage; an der exe gemessen 1+3, darum 5 Drucke — ein zusaetzlicher Druck geht
# durch die nun offene Tuer, die restlichen Zeilen laufen in 1141 am Eintrittspunkt ins Leere), Leiche 1021 1+4. Ein Druck zu viel loest den Satz neu aus (Leiche: sub06
# @0x02216 stellt Slot 6 wieder scharf).
# Bilder je Lauf per RE15_FRAMEDUMP (RE15_WINDOW_SCALE=1) und Ton-Mitschnitt RE15_AUDIO_CAP_SYNC im Arbeitsordner;
# geprueft werden debug.log, state.txt und wegplan_log.txt.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> [-DTEIL=W|Z|D|L|S|R|A] [-DRE15_R38F_WERKZEUG=<test_r38_dining1141>] -P test_r38_dining1141.cmake
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
# Nur auswerten: -DAUSWERTEN=<vorhandene Lauf-Basis ohne _<teil>> liest die Dateien eines frueheren Laufs neu ein.
if(AUSWERTEN)
    set(_basis "${AUSWERTEN}")
endif()

# EIGENER EXE-NAME neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
# Je TEIL ein eigener Praefix: parallele Laeufe verschiedener Teile raeumen sich nicht gegenseitig die Kopie weg.
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r38f_haken_${TEIL}_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r38f_haken_${TEIL}_${_lauf_id}${_exe_ext}")
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
    # _bed = NAME einer Variablen mit TRUE/FALSE (Ausdruecke vorher mit if() auswerten — ein ${...} mit Leerzeichen
    # bleibt im Makro EIN Argument, if("a GREATER b") waere immer falsch).
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
    set(_rv "ausgewertet")
    if(NOT AUSWERTEN)
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
    endif()
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
    set(_b FALSE)
    if(_p2 GREATER _p1)
        set(_b TRUE)
    endif()
    pruef_wahr(W _b "ROOM1021 zweimal geladen (Hin- und Rueckweg ueber Tueren)")
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
    # Runde 39 Spur J (AUFTRAG Runde 39, ROOM1141 Punkte 1-9): der Weg D laeuft jetzt mit John durch die Tuer, Klopfen +
    # help01..05, Elzas Reaktion, Glasbruch, Johns Zeilen 51/52 — ersetzt durch integration_r39_dining1141_d, das auch
    # die weiter gueltigen Pruefungen dieses Teils traegt (Schluessel, msg 40/41/11, Rolladen-Ton, Tuer-Toene, Cut 7).
    message(STATUS "r38_dining1141[D]: ersetzt durch integration_r39_dining1141_d (Runde 39 Spur J)")
elseif(FALSE)
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
WARTE 90
AKTION
WARTE 30
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
        set(_b FALSE)
        if("${_erste}" MATCHES "id=13 ")
            set(_b TRUE)
        endif()
        pruef_wahr(D _b "erste Nachricht in 1141 = msg 13 (Gemaelde), vorher kein Text (${_erste})")
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
        set(_fvor -1)
        set(_fertig 0)
        set(_cam_frei "")
        foreach(_z IN LISTS _zeilen)
            # Tuersperre im Kampf: msg 41 offen, waehrend John (t=49) im Raum ist = ROOM1141 (in 1131 gibt es keinen John)
            if(_z MATCHES "msg\\(a=1 fsm=1 id=41 " AND _z MATCHES "\\[1 t=49 ")
                set(_sperre 1)
            endif()
            # Raumwechsel = der Bildzaehler beginnt neu (main.c setzt frame_count beim Eintritt auf 0): nach 1141 Schluss.
            if(_z MATCHES "^F([0-9]+) ")
                set(_fn ${CMAKE_MATCH_1})
                if(_fn LESS _fvor AND _in1141)
                    set(_in1141 0)
                    set(_fertig 1)
                endif()
                set(_fvor ${_fn})
            endif()
            if(_fertig)
                continue()
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
                    # Fortsetzung 4 (Dossier 4.3): nach dem Parken muss die Kamera Cut 7 wieder verlassen (Cut_old, Form
                    # ROOM1150 @0x010F2) — Lauf int_d3 blieb auf Cut 7, Elza lief hinter der Kamera zur Dachtuer.
                    if(NOT _park STREQUAL "" AND _x EQUAL -30000 AND NOT _cam EQUAL 7 AND _cam_frei STREQUAL "")
                        set(_cam_frei "${_z}")
                    endif()
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
        set(_b FALSE)
        if("${_erst_bild}" MATCHES "^F[01] ")
            set(_b TRUE)
        endif()
        pruef_wahr(D _b "John steht ab dem ersten Bild in 1141 bei (-7124,-6150) (${_erst_bild})")
        set(_b FALSE)
        if(_sprung EQUAL 0)
            set(_b TRUE)
        endif()
        pruef_wahr(D _b "John springt nie und wird hinter der Kamera von Cut 7 geparkt (${_park}, Spruenge ${_sprung})")
        set(_b FALSE)
        if(NOT "${_park}" STREQUAL "")
            set(_b TRUE)
        endif()
        pruef_wahr(D _b "John wurde beim Abgang geparkt (${_park})")
        set(_b FALSE)
        if(NOT "${_cam_frei}" STREQUAL "")
            set(_b TRUE)
        endif()
        string(SUBSTRING "${_cam_frei}" 0 120 _cam_frei_kurz)
        pruef_wahr(D _b "nach dem Abgang verlaesst die Kamera Cut 7 (Cut_old): ${_cam_frei_kurz}")
        set(_b FALSE)
        if(_sperre EQUAL 1)
            set(_b TRUE)
        endif()
        pruef_wahr(D _b "Doppeltuer im Kampf -> msg 41 (I cannot leave him alone with those monsters.)")
    else()
        list(APPEND _fehler "D: kein state.txt")
    endif()
endif()

# --- S: Sichtbarkeit per Framedump (Nachtrag 2, Fortsetzung 4, Dossier 4.4) -----------------------------------------
# "Ich möchte keine plötzlich rein poppenden Charaktere haben..." -> in keinem Bild von ROOM1141 erscheint John ohne
# vorherige Bewegung ins Bild. Zwei gleiche Laeufe des Weges D bis nach Johns Abgang und Elzas Gang zur Dachtuer (ohne
# Eintritt in 1171): A mit John, B mit RE15_R38F_JOHN_AUS=1 (John weder Mesh noch Schatten, Spiel unveraendert). Beide
# lesen mit RE15_R38F_SICHT=sicht_ JEDES Bild von 1141 fertig komponiert zurueck (dining_r38_pc.c, Weg von RE15_FRAMEDUMP).
# Das Werkzeug `test_r38_dining1141 sicht` vergleicht A/B Bild fuer Bild: die verschiedenen Pixel sind Johns sichtbare
# Pixel; Regeln R0-R3 im Kopf von teil_sicht (test_r38_dining1141.c). Die Bilder (2 x ~1 GB) werden nach gruener Pruefung
# geloescht, sicht.txt (je Bild Kamera, Pixel, Kasten, Lagen) bleibt.
if(TEIL MATCHES "S")
    if(NOT AUSWERTEN AND (NOT RE15_R38F_WERKZEUG OR NOT EXISTS "${RE15_R38F_WERKZEUG}"))
        message(FATAL_ERROR "r38_dining1141[S]: RE15_R38F_WERKZEUG fehlt/existiert nicht: '${RE15_R38F_WERKZEUG}'")
    endif()
    set(_weg_s "RAUM 1131
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
WARTE 90
AKTION
WARTE 30
GEHE -5500 -19000 150
WARTE 700
WARTE 900
GEHE -6000 -12000 150
GEHE -7350 -7600 150
GEHE -7250 -1000 80
BLICK 3072
WARTE 60
ENDE
")
    set(_env_s "${_nach1021};RE15_GIVE=0x43:1;RE15_DEBUG_JUMP=1131@gp;RE15_PLAYER_POS=-1300,-13900,0;RE15_R38F_GORILLAS_TOT=600;RE15_R38F_GORILLAS_STARR=1;RE15_R38F_SICHT=sicht_")
    # Runde 39 Spur J: Ende 5800 statt 4450 - Tuer-Auftritt (+~250 Bilder) und Johns Zeilen 51/52 (+~350) verlaengern 1141
    # (gemessen Lauf n1: Elza an der Dachtuer F5602).
    f_lauf(sa ELZA "5800#1141" "${_weg_s}" "${_env_s}" _sa _wpsa)
    f_lauf(sb ELZA "5800#1141" "${_weg_s}" "${_env_s};RE15_R38F_JOHN_AUS=1" _sb _wpsb)
    pruef(S _sa "ROOM1141 Phase H: John auf dem Dach (geparkt")   # Runde 39 Spur J Punkt 2 (dining_r39.c)
    pruef(S _sa "MESSHAKEN RE15_R38F_GORILLAS_TOT")
    pruef(S _sa "[r38f-sicht] F")
    pruef(S _sb "[r38f-sicht] F")
    if(NOT AUSWERTEN OR RE15_R38F_WERKZEUG)
        execute_process(COMMAND "${RE15_R38F_WERKZEUG}" sicht "${_basis}_sa" "${_basis}_sb" sicht_
                        RESULT_VARIABLE _rv_s OUTPUT_VARIABLE _out_s ERROR_VARIABLE _out_s TIMEOUT 900)
        message(STATUS "${_out_s}")
        set(_b FALSE)
        if("${_rv_s}" STREQUAL "0")
            set(_b TRUE)
        endif()
        pruef_wahr(S _b "Sichtbarkeit je Bild (test_r38_dining1141 sicht, exit ${_rv_s})")
        if(_b)
            foreach(_l sa sb)
                file(GLOB _ppm "${_basis}_${_l}/sicht_*.ppm")
                if(_ppm)
                    file(REMOVE ${_ppm})
                endif()
            endforeach()
        endif()
    endif()
endif()

# --- R: die ECHTE 1021-Szene, danach Roys Leiche im SELBEN Besuch (Nachbesserung 1 M1) ------------------------------
# Abnahme 0 E1a: nach der Szene liess sich Roys Leiche nicht untersuchen (Zone Slot 6 nur im Raumaufbau, main00 @0x01E24);
# msg 40 kam erst nach Verlassen und Wiederbetreten. Stand vor der Szene = nach ROOM1211 (ablauf_elza.md E12, wie E1a):
# (4,0xF1) (3,0x74) (4,0xEC) (4,0xFC) (4,0x60) (3,0xC9) (3,0x2C) (3,0xBA) + Tot-Bits der Zombies 1021/1011. Start im
# Westteil wie vom 1041-Eintritt, in die Begehzone Slot 5 (main00 @0x01DE2, x -13100..-11000) gehen -> Szene; danach
# zu Fuss an die Leiche (Vorwaerts-Punkt (-4350,-9600) von (-3730,-9600) Blick -X) und druecken (Leiche 1+4).
if(TEIL MATCHES "R")
    f_lauf(r ELZA "2600#1021" "RAUM 1021
GEHE -12050 -7100 150
WARTE 30
GEHE -3730 -9600 100
BLICK 2048
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
ENDE
" "RE15_SET_FLAG=4:0xF1,3:0x74,4:0xEC,4:0xFC,4:0x60,3:0xC9,3:0x2C,3:0xBA,7:0xAA,7:0xAB,7:0xAD,7:0xB5,7:0xB8,7:0xB9;RE15_DEBUG_JUMP=1021@gp;RE15_PLAYER_POS=-15500,-7100,0;RE15_FRAMEDUMP=1700-2600/15:fd_" _r _wpr)
    pruef(R _r "ROOM1021 Ereignis 2 (Szene sub02) -> Programm SZENE1021")
    pruef(R _r "[msg] room=1021 id=0 ")
    pruef(R _r "[msg] room=1021 id=8 ")
    pruef(R _r "[msg] room=1021 id=40")
    # im SELBEN Besuch: msg 40 nach dem Ende der Szene (msg 8), 1021 nur EINMAL geladen
    string(FIND "${_r}" "[msg] room=1021 id=8 " _p8)
    string(FIND "${_r}" "[msg] room=1021 id=40" _p40)
    set(_b FALSE)
    if(_p8 GREATER -1 AND _p40 GREATER _p8)
        set(_b TRUE)
    endif()
    pruef_wahr(R _b "msg 40 (Hinweis) nach dem Ende der Szene (msg 8)")
    string(FIND "${_r}" "loaded room1021.rdt" _q1)
    string(FIND "${_r}" "loaded room1021.rdt" _q2 REVERSE)
    set(_b FALSE)
    if(_q1 GREATER -1 AND _q1 EQUAL _q2)
        set(_b TRUE)
    endif()
    pruef_wahr(R _b "ROOM1021 nur EINMAL geladen: der Hinweis kam im Besuch der Szene, ohne Verlassen und Wiederbetreten")
endif()

# --- A: Johns Abgang mit Elza in seiner Bahn und im Dachgang (Nachbesserung 1 M3) ----------------------------------
# Abnahme 0 E10/E5: Johns Abgangslauf schob Elza (Koerper-Schub FUN_8002aec4). Weg wie D bis nach der Szene; dann steht
# Elza auf Johns sichtbarer Bahn (E10-Lage (-7418,-14952)), waehrend die Gorillas sterben (Messhaken nach 600 freien
# Kampfbildern) -> John WARTET; danach geht sie nach Norden in den Dachgang: beim Verlassen der Bahn (z > -8875) beginnt
# der Abgang, sie steht dann auf Johns (unsichtbarem) Weg — er laeuft ohne Schub durch (Bit 0x2); danach weiter zur
# Dachtuer -> 1171. Zustands-Log: Cut 7 erst NACH dem Verlassen der Bahn, Elza waehrend Cut 7 unverrueckt, John geparkt.
if(TEIL MATCHES "A")
    # Runde 39 Spur J (Punkt 8): nach dem Tod der Gorillas steht Elza fuer Johns Zeilen an einer festen Lage ausserhalb seiner
    # Bahn (Pos_set wie ROOM1141 sub02 @0x00C32); Warten/Schub dieses Teils prueft unit_r39_dining1141_phase_h.
    message(STATUS "r38_dining1141[A]: ersetzt durch unit_r39_dining1141_phase_h / integration_r39_dining1141_d (Runde 39 Spur J)")
elseif(FALSE)
    f_lauf(a ELZA "60#1171" "RAUM 1131
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
GEHE -7418 -14952 100
BLICK 3072
WARTE 1200
GEHE -7400 -9600 100
GEHE -7342 -4062 100
WARTE 120
GEHE -7250 -1000 80
BLICK 3072
AKTION
RAUM 1171
WARTE 30
ENDE
" "${_nach1021};RE15_GIVE=0x43:1;RE15_DEBUG_JUMP=1131@gp;RE15_PLAYER_POS=-1300,-13900,0;RE15_R38F_GORILLAS_TOT=600;RE15_R38F_GORILLAS_STARR=1;RE15_FRAMEDUMP=90-9000/15:fd_" _a _wpa)
    pruef(A _a "ROOM1141 Gemaelde (Ereignis 6) in Phase H -> Programm BEFREIUNG")
    pruef(A _a "MESSHAKEN RE15_R38F_GORILLAS_TOT")
    pruef(A _a "Se_on bank=14 id=0 -> RE2-Tuerbank")
    pruef(A _a "loaded room1171.rdt")
    # Zeitpunkt des Messhakens (Bild in 1141) aus debug.log
    string(REGEX MATCH "\\[dining\\] F([0-9]+) MESSHAKEN RE15_R38F_GORILLAS_TOT" _m "${_a}")
    set(_f_tot "${CMAKE_MATCH_1}")
    set(_st "${_basis}_a/state.txt")
    if(EXISTS "${_st}" AND NOT "${_f_tot}" STREQUAL "")
        file(STRINGS "${_st}" _zeilen)
        set(_in1141 0)
        set(_fvor -1)
        set(_fertig 0)
        set(_c7 "")            # erstes Bild mit cam=7 nach dem Messhaken
        set(_vor_c7 "")        # Elza im Bild davor
        set(_c7_lage "")       # Elza waehrend Cut 7 (erste Zeile)
        set(_c7_abw 0)         # Zeilen mit cam=7, in denen Elza woanders steht
        set(_in_bahn_n 0)      # Bilder nach dem Messhaken mit Elza in der Bahn und ohne Cut 7
        set(_park 0)
        set(_nach_c7 "")       # Elza in der ersten Zeile nach Cut 7
        set(_ex "")
        set(_ez "")
        foreach(_z IN LISTS _zeilen)
            if(_z MATCHES "^F([0-9]+) ")
                set(_fn ${CMAKE_MATCH_1})
                if(_fn LESS _fvor AND _in1141)
                    set(_fertig 1)
                endif()
                set(_fvor ${_fn})
            endif()
            if(_fertig)
                break()
            endif()
            if(_z MATCHES "\\[1 t=49 ")
                set(_in1141 1)
            endif()
            if(NOT _in1141 OR _fn LESS _f_tot)
                continue()
            endif()
            string(REGEX MATCH "PL\\((-?[0-9]+),(-?[0-9]+)," _pl "${_z}")
            set(_ex ${CMAKE_MATCH_1})
            set(_ez ${CMAKE_MATCH_2})
            string(REGEX MATCH "cam=([0-9]+)" _c "${_z}")
            set(_cam ${CMAKE_MATCH_1})
            if(_z MATCHES "@\\(-30000,-30000,")
                set(_park 1)
            endif()
            if(_cam EQUAL 7)
                if(_c7 STREQUAL "")
                    set(_c7 ${_fn})
                    set(_c7_lage "${_ex},${_ez}")
                elseif(NOT "${_ex},${_ez}" STREQUAL "${_c7_lage}")
                    math(EXPR _c7_abw "${_c7_abw} + 1")
                endif()
            else()
                if(_c7 STREQUAL "")
                    set(_vor_c7 "${_ex},${_ez}")
                    if(_ex GREATER_EQUAL -8450 AND _ex LESS_EQUAL -6450 AND _ez GREATER_EQUAL -22400 AND _ez LESS_EQUAL -8875)
                        math(EXPR _in_bahn_n "${_in_bahn_n} + 1")
                    endif()
                elseif(_nach_c7 STREQUAL "")
                    set(_nach_c7 "${_ex},${_ez}")
                endif()
            endif()
        endforeach()
        message(STATUS "     A: Messhaken F${_f_tot}, Cut 7 ab F${_c7}, Elza davor (${_vor_c7}), in Cut 7 (${_c7_lage}), danach (${_nach_c7}), Bilder in der Bahn ${_in_bahn_n}")
        set(_b FALSE)
        if(_in_bahn_n GREATER 300)
            set(_b TRUE)
        endif()
        pruef_wahr(A _b "Gorillas tot, Elza auf Johns Bahn: ${_in_bahn_n} Bilder ohne Abgang (John wartet)")
        set(_b FALSE)
        if(NOT "${_c7}" STREQUAL "" AND NOT "${_vor_c7}" STREQUAL "")
            string(REPLACE "," ";" _v "${_vor_c7}")
            list(GET _v 1 _vz)
            if(_vz GREATER -8875)
                set(_b TRUE)
            endif()
        endif()
        pruef_wahr(A _b "Abgang (Cut 7 ab F${_c7}) erst, nachdem Elza die Bahn verlassen hat (Elza davor ${_vor_c7}, Nordrand -8875)")
        # Im Ausloesebild geht Elza noch (sie verlaesst die Bahn zu Fuss): der Schritt vom letzten Bild vor Cut 7 ist ein
        # Gehschritt, kein Schub (Schub der Abnahme: 856..4202 Einheiten). Waehrend Cut 7 und danach: exakt dieselbe Lage.
        set(_schritt 99999)
        if(NOT "${_vor_c7}" STREQUAL "" AND NOT "${_c7_lage}" STREQUAL "")
            string(REPLACE "," ";" _v "${_vor_c7}")
            string(REPLACE "," ";" _w "${_c7_lage}")
            list(GET _v 0 _vx)
            list(GET _v 1 _vz)
            list(GET _w 0 _wx)
            list(GET _w 1 _wz)
            math(EXPR _dx "${_wx} - (${_vx})")
            math(EXPR _dz "${_wz} - (${_vz})")
            if(_dx LESS 0)
                math(EXPR _dx "-(${_dx})")
            endif()
            if(_dz LESS 0)
                math(EXPR _dz "-(${_dz})")
            endif()
            math(EXPR _schritt "${_dx} + ${_dz}")
        endif()
        set(_b FALSE)
        if(_c7_abw EQUAL 0 AND _schritt LESS_EQUAL 150 AND "${_nach_c7}" STREQUAL "${_c7_lage}")
            set(_b TRUE)
        endif()
        pruef_wahr(A _b "Elza waehrend des ganzen Abgangs unverrueckt: davor (${_vor_c7}, Gehschritt ${_schritt}) -> Cut 7 (${_c7_lage}, ${_c7_abw} Abweichungen) = danach (${_nach_c7})")
        pruef_wahr(A _park "John beim Abgang geparkt")
    else()
        list(APPEND _fehler "A: kein state.txt oder kein Messhaken")
    endif()
endif()

# --- L: Leon 1140 Rolladen-Ton ----------------------------------------------------------------------------------
if(TEIL MATCHES "L")
    # Leons Gemaelde-Zone (Slot 5) feuert im Port Ereignis 5 ("The switch is not responding...", sub05): sub00 @0x00B14
    # setzt Slot 5 -> sub03, sub01 @0x00C7A `46 05 03 31 ff 00 18 05` stellt ihn jedes Bild auf sub05 (gemessen
    # test_r38_dining1141 takt: "1140 Slot 5 ... Ereignis 5", Lauf int_l2: msg 1 -> msg 5). Der Rolladen hebt sich bei Leon
    # nur ueber Ereignis 3 = sub03 (Strom (2,0), scd_vm.c s_power_gates); gefeuert wie im Vorher-Lauf v1140 per
    # RE15_FORCE_EVENT, Antwort "ja" per RE15_PRESS.
    f_lauf(l LEON "700#1140" "RAUM 1140
WARTE 600
ENDE
" "RE15_SET_FLAG=2:0;RE15_GOTO_ROOM=1140;RE15_FORCE_EVENT=3@60;RE15_PRESS=square@120,square@180,square@240;RE15_FRAMEDUMP=0-700/20:fd_" _l _wpl)
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
