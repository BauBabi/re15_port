cmake_minimum_required(VERSION 3.16)
# =============================================================================
# RUNDE 41 SPUR E "kleinkram_b", NACHBESSERUNG 1 (Abnahme 0 Hinweis H3): die drei Nutzer-Punkte an der ECHTEN exe.
# Dossier analysis/befunde_runde41/E_kleinkram_b.md ("Nachbesserung 1"), Auswerter tests/unit/test_r41_kleinkram_b_exe.c.
#   TEIL kurbel          Punkt 1: Leon, ECHTER Weg ROOM2050 Tuer Slot 1 (Tuerfahrt) -> ROOM2020 (11350,-3150) -> nach
#                        Westen (Kamera 0 -> 2 -> 3 wie der Nutzer-Lauf im befund.log) -> Kurbel-Zone Slot 3 (@0x009AE)
#                        -> "Will you use the Crank?" Ja -> sub02: zwischen Pos_set @0x0A8C und Plc_ret @0x0B16 steht Leon
#                        jedes Bild bei (3650,-3018), Raum-RBJ Clip 0 laeuft als Schleife (Plc_flg(0,4) @0x0AA0,
#                        0x80050cb8 @0x80050dbc-c8): >= 3 Umlaeufe, kein Standbild auf Slot 39.
#   TEIL karte           Punkt 2: Leon im Gang ROOM1230 an der Ostkammer-Tuer (Slot 4 @0x00CE6, Ankunft (19515,-12109))
#                        -> Kartenschirm B1: Marker bleibt auf der Ostkammer-Marke (x >= 143; vorher 130 = Hallen-Seite).
#   TEIL werkstatt_szene Punkt 3: Elza, ECHTER Weg ROOM1261 Tuer Slot 0 (@0x0112A) -> ROOM11B1, Szene sub03 (msg 10/11):
#                        Port-Programm hinter sub00, Kamera nie Cut 8 (offenes Tuerblatt), am Ende Cut 2.
#   TEIL werkstatt_tuer  Punkt 3: Elza in ROOM11B1 vor der Werkstatt-Tuer: Lauf nach Westen blockiert (x >= -20950),
#                        Aktionstaste -> msg 12 "The door is locked." (@0x179F), kein Raumwechsel.
# NACHBESSERUNG 2 (Abnahme 1 Maengel 1-3, master-AUFTRAG E-N1/E-N2):
#   TEIL kante           E-N1: Leon geht in ROOM2020 genau auf die gemeinsame Kante x = 9000 der RVD-Zonen z1 @0x016C
#                        (Cut 0 -> 2) / z5 @0x01BC (Cut 2 -> 0): angeforderter Cut wechselt jedes Bild (wie das Original,
#                        GDB o_cam9000: FUN_800142f4 je Durchlauf), das Bild STEHT (Present @0x80021558/@0x80021560: kein
#                        DrawOTag, kein Pufferwechsel) - alle Abzuege ab dem Stand pixelgleich.
#   TEIL geh             E-N2: Kurbel im GEHEN (Abnahme 1 R2): kein Spieler-Bild zwischen Message_on 0 @0x0A5C und 1
#                        @0x0A62 (Spiel-Task @0x80020de4 vor Nachrichten-Takt @0x80020f3c), Pos_set @0x0A8C im ersten Bild
#                        nach dem Schliessen der Frage (Auswahl-Message_on kehrt sofort zurueck @0x80040520), Bild gehalten.
#   TEIL renn            Kurbel im RENNEN (Abnahme 1 R4): Renn-Stopp an der Kurbel (0x80032778 @0x8003293c/@0x8003295c).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_WERKZEUG=<test_r41_kleinkram_b_exe> -DWORKDIR=<dir> -DTEIL=<teil>
#               -P test_r41_kleinkram_b.cmake
# =============================================================================
foreach(_v RE15_PC_EXE RE15_WERKZEUG WORKDIR TEIL)
    if(NOT ${_v})
        message(FATAL_ERROR "r41_kleinkram_b: ${_v} fehlt")
    endif()
endforeach()
foreach(_v RE15_PC_EXE RE15_WERKZEUG)
    if(NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r41_kleinkram_b: ${_v} existiert nicht: '${${_v}}'")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Keine RE15_*-Variablen des Aufrufers erben (VERTRAG §2; Muster test_r39_johnruf1041_tuer.cmake).
execute_process(COMMAND "${CMAKE_COMMAND}" -E environment OUTPUT_VARIABLE _envdump)
string(REGEX MATCHALL "(^|\n)RE15_[A-Za-z0-9_]*=" _geerbt "${_envdump}")
foreach(_g ${_geerbt})
    string(REGEX REPLACE "^\n?(RE15_[A-Za-z0-9_]*)=$" "\\1" _n "${_g}")
    unset(ENV{${_n}})
endforeach()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_lauf "${WORKDIR}/lauf_${TEIL}_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe_kopie "${_exe_dir}/re15_pc_r41e_${TEIL}_${_lauf_id}${_exe_ext}")

set(_fehler "")
macro(pruef _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(_p LESS 0)
        list(APPEND _fehler "fehlt '${_muster}'")
        message(STATUS "FAIL fehlt '${_muster}'")
    else()
        message(STATUS "ok   '${_muster}'")
    endif()
endmacro()
macro(pruef_nicht _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(NOT _p LESS 0)
        list(APPEND _fehler "unerwartet '${_muster}'")
        message(STATUS "FAIL unerwartet '${_muster}'")
    else()
        message(STATUS "ok   nicht da '${_muster}'")
    endif()
endmacro()
# _bed = NAME einer Variablen mit TRUE/FALSE (ein Ausdruck mit Leerzeichen kaeme im Makro als EIN Argument an)
macro(pruef_wahr _bed _text)
    if(${_bed})
        message(STATUS "ok   ${_text}")
    else()
        list(APPEND _fehler "${_text}")
        message(STATUS "FAIL ${_text}")
    endif()
endmacro()

set(_gemein RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1)
if(TEIL STREQUAL "kurbel")
    # Lauf t2_weg2050 des Dossiers: links drehen 0,36 s (3072 -> 2016), 3,8 s gehen (x 11350 -> 2914), dann auf der Stelle
    # links drehen und Aktion abwechselnd (je 1 Bild); Ausloesung an der Kurbel-Zone Slot 3 (@0x009AE), Dialog per PRESS.
    set(_skript "L0.36,U3.8")
    foreach(_i RANGE 1 8)
        string(APPEND _skript ",LA0.034,L0.034")
    endforeach()
    string(APPEND _skript ",W2.0")
    set(_env RE15_DEBUG_JUMP=2050@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 "RE15_FIRE_AOT=1@30#2050"
             "RE15_INPUT_SCRIPT=${_skript}" RE15_INPUT_SCRIPT_START=60 RE15_INPUT_SCRIPT_BASIS=spiel
             RE15_PRESS=square@260,square@300,square@340,square@380
             RE15_SCD_TRACE=1 RE15_ANIM_TRACE=anim.txt RE15_STATE_LOG=state.txt "RE15_EXIT_AT=600#2020")
elseif(TEIL STREQUAL "karte")
    # Abnahme 0 §4.1 Lauf "U0.8 / U1.0 / U1.5": Gang ROOM1230, Strom (4,0xF3), Ostkammer-Tuer offen (3,0x89), Hunde tot
    # (7,0x88)/(7,0x89); Lauf endet an der Tuer (96,12779), dann START + L1 = Kartenschirm Blatt 0 (B1).
    set(_env RE15_DEBUG_JUMP=1230@240 RE15_PLAYER_POS=96,14500,1024,0 RE15_SET_FLAG=4:0xf3,3:0x89,7:0x88,7:0x89
             RE15_INPUT_SCRIPT=U1.0,W0.2,S0.1,W1.0,Q0.1 RE15_INPUT_SCRIPT_START=300 RE15_INPUT_SCRIPT_BASIS=spiel
             RE15_STATE_LOG=state.txt RE15_INV_FB_SHOT=map.bmp RE15_INV_FB_SHOT_AT=120 RE15_MAP_SHOT_PAGE=0
             RE15_EXIT_AT=560)
elseif(TEIL STREQUAL "werkstatt_szene")
    # Abnahme 0 §5.1 Lauf D1: Zustand nach 10B1 sub04 @0x0195E (3,0x83) und 2001 @0x01958 (3,0x46); Tuer 1261 Slot 0.
    set(_env RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=1261@240 RE15_SET_FLAG=3:0x83,3:0x46,4:0xf3
             RE15_PLAYER_POS=27650,9450,1024,1 RE15_PRESS=square@300,square@310
             RE15_SCD_TRACE=1 RE15_MSG_LOG=1 RE15_STATE_LOG=state.txt "RE15_EXIT_AT=700#11b1")
elseif(TEIL STREQUAL "werkstatt_tuer")
    # Abnahme 0 §5.2 Lauf D2: Elza oestlich der Tuer, Blick -x, 1,5 s gehen, dann Aktionstaste.
    set(_env RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=11b1@240 RE15_SET_FLAG=3:0x83,4:0xf3
             RE15_PLAYER_POS=-20200,-16230,2048,0 "RE15_INPUT_SCRIPT=U1.5,W0.3,A0.1" RE15_INPUT_SCRIPT_START=262
             RE15_INPUT_SCRIPT_BASIS=spiel RE15_STATE_LOG=state.txt RE15_MSG_LOG=1 RE15_EXIT_AT=380)
elseif(TEIL STREQUAL "kante")
    # Lauf EN1w des Dossiers: (9375,-3200) Blick 2048 (-x), 5 Bilder Gehen (75 je Bild) -> x 9000 ab Bild 64.
    set(_env RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_PLAYER_POS=9375,-3200,2048,1 RE15_INPUT_SCRIPT=U0.1667
             RE15_INPUT_SCRIPT_START=60 RE15_INPUT_SCRIPT_BASIS=spiel RE15_STATE_LOG=state.txt RE15_FRAMEDUMP=66-90/1:fd_
             "RE15_EXIT_AT=92#2020")
elseif(TEIL STREQUAL "geh")
    set(_env RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_PLAYER_POS=2600,-1500,1024,1
             "RE15_INPUT_SCRIPT=U0.3,UA0.1,U0.1,UA0.1,U0.1,UA0.1,U0.1,UA0.1,U0.1,UA0.1,U1.0" RE15_INPUT_SCRIPT_START=300
             RE15_INPUT_SCRIPT_BASIS=spiel RE15_PRESS=square@380,square@430,square@470,square@510 RE15_SCD_TRACE=1
             RE15_MOTRACE=1 RE15_STATE_LOG=state.txt RE15_KAMERAHALT_LOG=1 "RE15_EXIT_AT=420#2020")
elseif(TEIL STREQUAL "renn")
    set(_env RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_PLAYER_POS=2600,-800,1024,1
             "RE15_INPUT_SCRIPT=XU0.2,XUA0.1,XU0.1,XUA0.1,XU0.1,XUA0.1,XU0.1,XUA0.1,XU0.1,XUA0.1,XU1.0"
             RE15_INPUT_SCRIPT_START=300 RE15_INPUT_SCRIPT_BASIS=spiel RE15_PRESS=square@380,square@430,square@470,square@510
             RE15_SCD_TRACE=1 RE15_STATE_LOG=state.txt "RE15_EXIT_AT=430#2020")
else()
    message(FATAL_ERROR "r41_kleinkram_b: TEIL '${TEIL}' unbekannt")
endif()

file(MAKE_DIRECTORY "${_lauf}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(WORKDIR "${_lauf}")
re15_start_spiel(_rv 600 ${_gemein} ${_env} "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")
message(STATUS "r41_kleinkram_b[${TEIL}]: exit=${_rv}, ${_lauf}")

if(NOT EXISTS "${_lauf}/debug.log")
    message(FATAL_ERROR "r41_kleinkram_b[${TEIL}]: kein debug.log (${_lauf})")
endif()
file(READ "${_lauf}/debug.log" _d)
string(FIND "${_d}" "EXIT_AT: Bild" _p)
if(_p LESS 0)
    message(FATAL_ERROR "r41_kleinkram_b[${TEIL}]: Endbild nicht erreicht - Lauf vorher abgerissen (${_lauf})")
endif()

if(TEIL STREQUAL "kurbel")
    pruef(_d "[fire-aot] slot=1")
    pruef(_d "loaded room2020.rdt")
    pruef(_d "@0x0A5C op=0x2B")                   # Message_on 0 "It's a square hole."
    pruef(_d "@0x0A7E op=0x29")                   # Cut_chg 3 nach dem Ja
    pruef(_d "@0x0A9C op=0x3F")                   # Plc_motion(0,0,0)
    pruef(_d "@0x0B16 op=0x42")                   # Plc_ret
    pruef(_d "EXIT_AT: Bild 600 in Raum 2020")
    execute_process(COMMAND "${RE15_WERKZEUG}" kurbel "${_lauf}/debug.log" "${_lauf}/state.txt" "${_lauf}/anim.txt"
                    RESULT_VARIABLE _rw OUTPUT_VARIABLE _ow ERROR_VARIABLE _ew)
    message(STATUS "${_ow}${_ew}")
    set(_ok FALSE)
    if(_rw EQUAL 0)
        set(_ok TRUE)
    endif()
    pruef_wahr(_ok "Kurbel: Lage (3650,-3018) bis Plc_ret, Schleife >= 3 Umlaeufe, kein Standbild")
elseif(TEIL STREQUAL "karte")
    pruef(_d "loaded room1230.rdt")
    set(_ok FALSE)
    if(EXISTS "${_lauf}/map.bmp")
        set(_ok TRUE)
    endif()
    pruef_wahr(_ok "Kartenschirm-Abzug map.bmp")
    execute_process(COMMAND "${RE15_WERKZEUG}" segment "${_lauf}/state.txt" OUTPUT_VARIABLE _os)
    message(STATUS "${_os}")
    if(EXISTS "${_lauf}/map.bmp")
        execute_process(COMMAND "${RE15_WERKZEUG}" marker "${_lauf}/map.bmp" RESULT_VARIABLE _rm OUTPUT_VARIABLE _om)
        message(STATUS "${_om}")
        set(_mx -1)
        set(_my -1)
        if(_om MATCHES "MARKER ([0-9]+) ([0-9]+) ([0-9]+)")
            set(_mx ${CMAKE_MATCH_1})
            set(_my ${CMAKE_MATCH_2})
        endif()
        # Ostkammer-Marke (146,109) re15_map_zones.h:987, gemessen (145,107); Hallen-Marke (127,115), vorher (130,112).
        set(_ok FALSE)
        if(_mx GREATER_EQUAL 143 AND _mx LESS_EQUAL 149)
            set(_ok TRUE)
        endif()
        pruef_wahr(_ok "Marker x ${_mx} auf der Ostkammer-Seite (143..149)")
        set(_ok FALSE)
        if(_my GREATER_EQUAL 103 AND _my LESS_EQUAL 111)
            set(_ok TRUE)
        endif()
        pruef_wahr(_ok "Marker y ${_my} (103..111)")
    endif()
elseif(TEIL STREQUAL "werkstatt_szene")
    pruef(_d "loaded room11b1.rdt")
    pruef(_d "[wk11b1-r41] ROOM11B1 (3,0x83)=1")
    pruef(_d "[msgtext] room=11b1 id=10 ")        # "Elza: Sherry!?" (sub03 @0x01480)
    pruef(_d "[msgtext] room=11b1 id=11 ")        # "Where has she gone!?" (@0x014CE)
    pruef(_d "@0x14E6 op=0x42")                   # Plc_ret der Szene
    execute_process(COMMAND "${RE15_WERKZEUG}" segment "${_lauf}/state.txt" OUTPUT_VARIABLE _os)
    message(STATUS "${_os}")
    set(_kams "")
    set(_letzte -1)
    if(_os MATCHES "kameras=([0-9,]+) letzte_kamera=([0-9]+)")
        set(_kams ",${CMAKE_MATCH_1},")
        set(_letzte ${CMAKE_MATCH_2})
    endif()
    string(FIND "${_kams}" ",8," _p8)
    string(FIND "${_kams}" ",7," _p7)
    set(_ok FALSE)
    if(_p8 LESS 0 AND _p7 LESS 0 AND NOT _kams STREQUAL "")
        set(_ok TRUE)
    endif()
    pruef_wahr(_ok "ROOM11B1 nie Cut 7/8 (offenes Tuerblatt): ${_kams}")
    set(_ok FALSE)
    if(_letzte EQUAL 2)
        set(_ok TRUE)
    endif()
    pruef_wahr(_ok "am Ende Cut 2 (Tuerblatt zu): ${_letzte}")
elseif(TEIL STREQUAL "werkstatt_tuer")
    pruef(_d "[wk11b1-r41] ROOM11B1 (3,0x83)=1")
    pruef(_d "[msgtext] room=11b1 id=12 ")
    pruef(_d "The door is locked.")
    pruef(_d "EXIT_AT: Bild 380 in Raum 11b1")
    execute_process(COMMAND "${RE15_WERKZEUG}" segment "${_lauf}/state.txt" OUTPUT_VARIABLE _os)
    message(STATUS "${_os}")
    set(_mnx -99999)
    if(_os MATCHES "min_x=(-?[0-9]+)")
        set(_mnx ${CMAKE_MATCH_1})
    endif()
    # Zellkante x -21410 (SCA (3,0x0C) @0x00EC8: x -22150 + w 740) + Radius = -20942 gemessen.
    set(_ok FALSE)
    if(_mnx GREATER_EQUAL -20950)
        set(_ok TRUE)
    endif()
    pruef_wahr(_ok "Elza an der Trennwand blockiert (min x ${_mnx} >= -20950)")
endif()

if(TEIL STREQUAL "kante")
    pruef(_d "loaded room2020.rdt")
    pruef(_d "(gehalten)")
    execute_process(COMMAND "${RE15_WERKZEUG}" kamwechsel "${_lauf}/state.txt" 66 90 OUTPUT_VARIABLE _ok_)
    message(STATUS "${_ok_}")
    set(_kw 0)
    if(_ok_ MATCHES "KAMWECHSEL ([0-9]+)")
        set(_kw ${CMAKE_MATCH_1})
    endif()
    set(_ok FALSE)
    if(_kw GREATER_EQUAL 20)
        set(_ok TRUE)
    endif()
    pruef_wahr(_ok "angeforderter Cut wechselt auf der Kante jedes Bild wie im Original (${_kw} Wechsel in 24 Bildern)")
    execute_process(COMMAND "${RE15_WERKZEUG}" gleich "${_lauf}/fd_" 66 90 RESULT_VARIABLE _rg OUTPUT_VARIABLE _og)
    message(STATUS "${_og}")
    set(_ok FALSE)
    if(_rg EQUAL 0)
        set(_ok TRUE)
    endif()
    pruef_wahr(_ok "Bild steht (Present @0x80021560: kein DrawOTag, kein Pufferwechsel): Abzuege F66..F90 pixelgleich")
elseif(TEIL STREQUAL "geh" OR TEIL STREQUAL "renn")
    pruef(_d "loaded room2020.rdt")
    pruef(_d "@0x0A62 op=0x2B")
    pruef(_d "@0x0A8C op=0x32")
    set(_modus renn)
    if(TEIL STREQUAL "geh")
        set(_modus takt)
    endif()
    execute_process(COMMAND "${RE15_WERKZEUG}" ${_modus} "${_lauf}/debug.log" "${_lauf}/state.txt"
                    RESULT_VARIABLE _rw OUTPUT_VARIABLE _ow ERROR_VARIABLE _ew)
    message(STATUS "${_ow}${_ew}")
    set(_ok FALSE)
    if(_rw EQUAL 0)
        set(_ok TRUE)
    endif()
    pruef_wahr(_ok "Kurbel-Anlauf ${TEIL}: Bildfolge wie das Original (Auswerter ${TEIL})")
endif()

if(_fehler)
    string(REPLACE ";" "\n  " _f "${_fehler}")
    message(FATAL_ERROR "r41_kleinkram_b[${TEIL}] DURCHGEFALLEN (${_lauf}):\n  ${_f}")
endif()
message(STATUS "r41_kleinkram_b[${TEIL}] BESTANDEN")
