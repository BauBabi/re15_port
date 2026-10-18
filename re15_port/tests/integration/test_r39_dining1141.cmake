cmake_minimum_required(VERSION 3.16)
# =============================================================================
# RUNDE 39 SPUR J "dining1141" MIT DER ECHTEN EXE. Dossier analysis/befunde_runde39/J_dining1141.md, Konstanten
# include/re15_dining_r39.h. Die Unit-Riegel unit_r39_dining1141_* pruefen die Mechanik am Engine-Code; dieser Riegel
# prueft den Nutzerweg mit Elza an der exe (Debug-Sprung nach ROOM1131, danach nur echte Tueren per RE15_WEGPLAN):
#   1131 (Dining Room Key benutzen, Tuer) -> 1141 Phase H (Klopfen + help01..05, Elzas Reaktion) -> Gemaelde "ja" ->
#   Rolladen, Kamera vorne auf Elza, Tuer auf/zu, Kamera auf die Tuer mit John -> Szene (Glasbruch vor msg 6, Blick ab
#   msg 9) -> Tuersperre msg 41 -> Kampfende per Messhaken RE15_R38F_GORILLAS_TOT (Kill-Flags (7,0xCB)/(7,0xCC), KEINE
#   Schusszahl: Gorilla-Energie zieht Spur E nach, Hinweis des Orchestrators) -> msg 51/52 -> Johns Abgang -> Dachtuer
#   -> 1171 (Musik beim Wechsel, Punkt 9).
# Geprueft: debug.log, RE15_R39J_LOG (Messschiene engine/src/dining_r39.c), wegplan_log.txt und der Ton-Mitschnitt
# (Werkzeug test_r39_dining1141 pegel). Bilder je 10 Takte per RE15_FRAMEDUMP (RE15_WINDOW_SCALE=1) im Arbeitsordner.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_R39J_WERKZEUG=<test_r39_dining1141> -DWORKDIR=<dir> -P test_r39_dining1141.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r39_dining1141: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_R39J_WERKZEUG OR NOT EXISTS "${RE15_R39J_WERKZEUG}")
    message(FATAL_ERROR "r39_dining1141: RE15_R39J_WERKZEUG fehlt: '${RE15_R39J_WERKZEUG}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r39_dining1141: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_lauf "${WORKDIR}/lauf_${_lauf_id}")
if(AUSWERTEN)
    set(_lauf "${AUSWERTEN}")
endif()

# Eigene exe-Kopie mit Praefix JE TEST neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r39j_d_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r39j_d_${_lauf_id}${_exe_ext}")
if(NOT AUSWERTEN)
    file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
endif()

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
macro(pruef_wahr _bed _text)
    if(${_bed})
        message(STATUS "ok   ${_text}")
    else()
        list(APPEND _fehler "${_text}")
        message(STATUS "FAIL ${_text}")
    endif()
endmacro()

# Wegplan (geschlossen geregelt, engine/src/dachkiste_r36.c): 1131 Schluessel + Tuer (Druckzahl wie integration_r38_
# dining1141 D), 1141 erst stehen (Klopfen, Hilferuf, Reaktion), Gemaelde (1+3 Drucke), Szene abwarten (WARTE zaehlt nur
# freie Bilder), Doppeltuer im Kampf (msg 41), Warten bis der Messhaken die Gorillas toetet und Gespraech + Abgang durch
# sind, Dachtuer.
set(_plan "RAUM 1131
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
WARTE 420
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
AKTION
RAUM 1171
WARTE 300
ENDE
")

if(NOT AUSWERTEN)
    file(MAKE_DIRECTORY "${_lauf}")
    file(WRITE "${_lauf}/wegplan.txt" "${_plan}")
    set(WORKDIR "${_lauf}")
    # Nach-Szene-Flags der 1021-Szene wie integration_r38_dining1141 (3,0x5F) (4,0xF0) (4,0xF1) (4,0xEC) (3,0x78).
    re15_start_spiel(_rv 3000
        RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
        RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_EVT_TRACE=1 RE15_SPAWN_DIAG=1 RE15_BGM_CTL_DEBUG=1 RE15_BGM_FADE_DEBUG=1
        RE15_STATE_LOG=state.txt RE15_AUDIO_CAP_SYNC=cap.raw RE15_R39J_LOG=r39.txt
        "RE15_WEGPLAN=${_lauf}/wegplan.txt" RE15_WEGPLAN_LOG=wegplan_log.txt
        "RE15_EXIT_AT=300#1171"
        "RE15_SET_FLAG=3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78" "RE15_GIVE=0x43:1" "RE15_DEBUG_JUMP=1131@gp"
        "RE15_PLAYER_POS=-1300,-13900,0" "RE15_R38F_GORILLAS_TOT=600" "RE15_R38F_GORILLAS_STARR=1"
        "RE15_FRAMEDUMP=0-6000/10:fd_"
        "${_exe_kopie}")
    file(REMOVE "${_exe_kopie}")
    message(STATUS "r39_dining1141: exit=${_rv}, ${_lauf}")
endif()

if(NOT EXISTS "${_lauf}/debug.log")
    message(FATAL_ERROR "r39_dining1141: kein debug.log (${_lauf})")
endif()
file(READ "${_lauf}/debug.log" _d)
string(FIND "${_d}" "EXIT_AT: Bild" _p)
if(_p LESS 0)
    message(FATAL_ERROR "r39_dining1141: Endbild in 1171 nicht erreicht - Lauf vorher abgerissen (${_lauf})")
endif()
# Abschnitt 1141 = zwischen "loaded room1141.rdt" und "loaded room1171.rdt"
string(FIND "${_d}" "loaded room1141.rdt" _a)
string(FIND "${_d}" "loaded room1171.rdt" _b)
set(_ok_ab FALSE)
if(_a GREATER -1 AND _b GREATER _a)
    set(_ok_ab TRUE)
endif()
pruef_wahr(_ok_ab "Weg 1131 -> 1141 -> 1171 (beide Raeume geladen)")
if(NOT _ok_ab)
    message(FATAL_ERROR "r39_dining1141: Weg unvollstaendig: ${_fehler}")
endif()
math(EXPR _len "${_b} - ${_a}")
string(SUBSTRING "${_d}" ${_a} ${_len} _r)

# Runde 38 (unveraendert): Schluessel benutzt, Tuer offen
pruef(_d "ROOM1131: Dining Room Key benutzt -> Tuer offen, Flag (9,66) = 1")
# Punkte 1 und 3: weder Zombie-Stoehnen/Krachen (Bank 0x3C = RE2 ROOM1060) noch "Help me out." (0x3D, main42)
pruef_nicht(_r "Se_on bank=0x3C")
pruef_nicht(_r "Se_on bank=0x3D")
pruef_nicht(_r "SCD Se_on: bank=60 ")
pruef_nicht(_r "SCD Se_on: bank=61 ")
pruef_nicht(_r "clip loaded: main42.wav")
pruef(_r "ROOM1141 Phase H: John auf dem Dach (geparkt -30000,-30000)")
# Punkt 4: Klopfen (RE2 DOOR0F) und Hilferufe aus den Nutzer-Dateien
pruef(_r "Klopfbank RE2 DOOR0F geladen")
pruef(_r "(RE2 DOOR0F Klopfen)")
pruef(_r "(Stimme help03.wav, Id 55)")
pruef(_r "(Stimme help01.wav, Id 53)")
pruef(_r "[voice] clip loaded: help03.wav")
pruef(_r "[voice] clip loaded: help01.wav")
# Punkt 5: Elzas Reaktion ist die ERSTE Nachricht in 1141 und kommt nach dem ersten Hilferuf
string(FIND "${_r}" "[msg] room=1141 id=" _pm)
string(FIND "${_r}" "(Stimme help03.wav" _ph)
set(_b5 FALSE)
if(_pm GREATER -1 AND _ph GREATER -1 AND _pm GREATER _ph)
    string(SUBSTRING "${_r}" ${_pm} 23 _erste)
    if(_erste MATCHES "id=50 ")
        set(_b5 TRUE)
    endif()
endif()
pruef_wahr(_b5 "Punkt 5: erste Nachricht in 1141 = msg 50 (Elzas Reaktion), nach dem ersten Hilferuf")
# Punkt 6: beide Glasbruch-Saetze vor "What was that?!" (msg 6)
string(FIND "${_r}" "(RE2 GLAS1090 Glasbruch)" _pg REVERSE)
string(FIND "${_r}" "[msg] room=1141 id=6 " _p6)
set(_b6 FALSE)
if(_pg GREATER -1 AND _p6 GREATER _pg)
    set(_b6 TRUE)
endif()
pruef_wahr(_b6 "Punkt 6: Glasbruch vor msg 6 \"What was that?!\"")
# Runde 38 (unveraendert): Szene ab msg 40, Tuersperre msg 41, Kampfende, Tuer-Toene
pruef(_r "[msg] room=1141 id=40")
pruef(_r "[msgtext] room=1141 id=41")
pruef(_r "MESSHAKEN RE15_R38F_GORILLAS_TOT")
pruef(_r "Se_on bank=14 id=0")
pruef(_r "Se_on bank=14 id=1")
# Punkt 8: msg 51 vor msg 52, beide nach dem Kampfende
string(FIND "${_r}" "MESSHAKEN RE15_R38F_GORILLAS_TOT" _pk)
string(FIND "${_r}" "[msg] room=1141 id=51 " _p51)
string(FIND "${_r}" "[msg] room=1141 id=52 " _p52)
set(_b8 FALSE)
if(_pk GREATER -1 AND _p51 GREATER _pk AND _p52 GREATER _p51)
    set(_b8 TRUE)
endif()
pruef_wahr(_b8 "Punkt 8: nach dem Kampf msg 51 \"Puuuh...\" und dann msg 52 \"Elza, follow me...\"")

# Messschiene RE15_R39J_LOG: Kamera, Johns Lage/Blick/Sub/Clip je Bild in 1141
file(STRINGS "${_lauf}/r39.txt" _zeilen)
set(_erst_da "")
set(_cut6_john "")
set(_cut7_seit "")
set(_blick_n 0)
set(_blick_aus 0)
set(_b51_blick 0)
set(_wink 0)
set(_geste15 0)
set(_b67 0)
foreach(_z IN LISTS _zeilen)
    if(NOT _z MATCHES "^F([0-9]+) cam=([0-9]+) anz=([0-9]+) msg=([01])/([0-9]+) john=[01] \\((-?[0-9]+),(-?[0-9]+)\\) rot=[0-9]+ st=[0-9]+ s1=([0-9]+) s2=[0-9]+ mo=(-?[0-9]+) .* b67=([01]) .* blick=(-?[0-9]+)$")
        continue()
    endif()
    set(_f ${CMAKE_MATCH_1})
    set(_anz ${CMAKE_MATCH_3})
    set(_ma ${CMAKE_MATCH_4})
    set(_mid ${CMAKE_MATCH_5})
    set(_jx ${CMAKE_MATCH_6})
    set(_jz ${CMAKE_MATCH_7})
    set(_s1 ${CMAKE_MATCH_8})
    set(_mo ${CMAKE_MATCH_9})
    set(_b67z ${CMAKE_MATCH_10})
    set(_bl ${CMAKE_MATCH_11})
    if(_bl LESS 0)
        math(EXPR _bl "-(${_bl})")
    endif()
    if(_anz EQUAL 7 AND _cut7_seit STREQUAL "")
        set(_cut7_seit ${_f})
    endif()
    # Punkt 2: das erste Bild, in dem John nicht mehr geparkt ist
    if(_erst_da STREQUAL "" AND _jx GREATER -29000)
        set(_erst_da "F${_f} anz=${_anz} (${_jx},${_jz})")
    endif()
    if(_cut6_john STREQUAL "" AND _anz EQUAL 6 AND NOT _erst_da STREQUAL "")
        set(_cut6_john "F${_f} (${_jx},${_jz})")
    endif()
    # Punkt 7: Blick waehrend msg 9 und 10 (msg 11 endet mit Johns Lauf in die Ecke, Original sub02 @0x00F8E)
    if(_ma EQUAL 1 AND (_mid EQUAL 9 OR _mid EQUAL 10))
        math(EXPR _blick_n "${_blick_n} + 1")
        if(_bl GREATER 160)
            math(EXPR _blick_aus "${_blick_aus} + 1")
        endif()
    endif()
    if(_ma EQUAL 1 AND _mid EQUAL 51 AND _bl LESS 129)
        set(_b51_blick 1)
    endif()
    if(_ma EQUAL 1 AND _mid EQUAL 51 AND _s1 EQUAL 0 AND _mo EQUAL 15)
        set(_geste15 1)
    endif()
    if(_ma EQUAL 1 AND _mid EQUAL 52 AND _s1 EQUAL 2 AND _mo EQUAL 6)
        set(_wink 1)
    endif()
    if(_b67z EQUAL 1)
        set(_b67 1)
    endif()
endforeach()
message(STATUS "     John erstmals im Raum: ${_erst_da}; erstes Bild Cut 6 danach: ${_cut6_john}; Cut 7 ab F${_cut7_seit}")
set(_b2a FALSE)
if(_erst_da MATCHES "anz=7 \\(-7350,-1300\\)")
    set(_b2a TRUE)
endif()
pruef_wahr(_b2a "Punkt 2: John kommt innen vor die Dachtuer, waehrend die Kamera vorne auf Elza steht (Cut 7)")
set(_b2b FALSE)
if(_cut6_john MATCHES "\\(-7350,-1300\\)")
    set(_b2b TRUE)
endif()
pruef_wahr(_b2b "Punkt 2: Kamera auf die Tuer (Cut 6): John steht schon da")
set(_b7 FALSE)
if(_blick_n GREATER 50 AND _blick_aus EQUAL 0)
    set(_b7 TRUE)
endif()
pruef_wahr(_b7 "Punkt 7: John blickt waehrend msg 9/10 zu Elza (${_blick_aus} von ${_blick_n} Bildern ueber 160 Einheiten)")
pruef_wahr(_b51_blick "Punkt 8: John blickt bei msg 51 zu Elza")
pruef_wahr(_geste15 "Punkt 8: Geste Clip 15 zu msg 51")
pruef_wahr(_wink "Punkt 8: Heranwinken Sub 2 Clip 6 zu msg 52")
pruef_wahr(_b67 "Abgang: (9,67) = 1 John hat 1141 verlassen")

# Punkt 9: Musik beim Wechsel 1141 -> 1171 (Werkzeug liest debug.log + cap.raw)
execute_process(COMMAND "${RE15_R39J_WERKZEUG}" pegel "${_lauf}" RESULT_VARIABLE _rp OUTPUT_VARIABLE _op ERROR_VARIABLE _ep)
message(STATUS "${_op}")
set(_b9 FALSE)
if(_rp EQUAL 0)
    set(_b9 TRUE)
endif()
pruef_wahr(_b9 "Punkt 9: Pegel nach dem Raumwechsel (Werkzeug pegel)")

list(LENGTH _fehler _nf)
if(_nf GREATER 0)
    message(FATAL_ERROR "r39_dining1141 D: ${_nf} Fehler: ${_fehler} (${_lauf})")
endif()
message(STATUS "r39_dining1141 D: alle Pruefungen gruen (${_lauf})")
