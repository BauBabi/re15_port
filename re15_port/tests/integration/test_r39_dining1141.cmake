cmake_minimum_required(VERSION 3.16)
# =============================================================================
# RUNDE 39 SPUR J "dining1141" MIT DER ECHTEN EXE. Dossier analysis/befunde_runde39/J_dining1141.md, Konstanten
# include/re15_dining_r39.h. Die Unit-Riegel unit_r39_dining1141_* pruefen die Mechanik am Engine-Code; dieser Riegel
# prueft den Nutzerweg mit Elza an der exe (Debug-Sprung nach ROOM1131, danach nur echte Tueren per RE15_WEGPLAN):
#   1131 (Dining Room Key benutzen, Tuer) -> 1141 Phase H (Klopfen + help01..05, Elzas Reaktion) -> Gemaelde "ja" ->
#   Rolladen, danach frei laufen (Nachbesserung 1 N4.1) -> Riegel der Dachtuer (msg 12) -> Elza rennt zur Kamera Cut 7,
#   dreht sich zur Tuer, hebt die Waffe (N4.2; hier ohne Schusswaffe = Messer) -> Tuer auf/zu, Kamera auf die Tuer mit
#   John -> Szene (Glasbruch vor msg 6, Blick ab msg 9) -> Tuersperre msg 41 -> Kampf, John schiesst ab und an (N4.3) ->
#   Kampfende per Messhaken RE15_R38F_GORILLAS_TOT (Kill-Flags (7,0xCB)/(7,0xCC), KEINE Schusszahl) -> msg 51/52 mit
#   Heranwinken aus ROOM1171 (M2, Nutzer-Nachtrag 7), John springt nicht (M3) -> Johns Abgang -> Dachtuer -> 1171 (Musik
#   beim Wechsel, Punkt 9).
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
# dining1141 D), 1141 erst stehen (Klopfen, Hilferuf, Reaktion), Gemaelde (1+3 Drucke), nach dem Rolladen 200 freie
# Bilder (N4.1), durch die Rolladen-Oeffnung zur Dachtuer, Riegel (msg 12 schliessen), Szene abwarten (WARTE zaehlt nur
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
WARTE 200
GEHE -7350 -8500 150
GEHE -7350 -7600 150
GEHE -7250 -1000 80
BLICK 3072
AKTION
WARTE 40
AKTION
WARTE 40
AKTION
WARTE 60
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

# Runde 38 (unveraendert, aus integration_r38_dining1141 D uebernommen): Schluessel gegeben + benutzt, Tuer offen
pruef(_d "[give] item 67 x1")
pruef(_d "ROOM1131: Dining Room Key benutzt -> Tuer offen, Flag (9,66) = 1")
pruef(_r "ROOM1141 Gemaelde (Ereignis 6) in Phase H -> Programm ROLLTOR39")
# Nachbesserung 1 N4.2: die Szene startet am Riegel der Dachtuer (sub05 msg 12), Elza hebt die Waffe (ohne Schusswaffe
# im Inventar das Messer), N4.3: John schiesst; N6: Zeilen klein geschrieben
pruef(_r "ROOM1141 Riegel der Dachtuer (Ereignis 5) in Phase H -> Programm ENTRIEGELN39")
pruef(_r "[msg] room=1141 id=12 ")
pruef(_r "Dachtuer entriegelt: Elza hebt Waffe 1 (vorher 1, Messer)")
pruef(_r "Elza Plc_motion(2,6) -> W-Bank-Clip 6")
pruef(_r "John schiesst (Schuss 2)")
pruef(_r "He is in trouble, I have to help him!")
pruef(_r "we end up as gorilla meal....")
pruef_nicht(_r "He is in Trouble")
pruef_nicht(_r "as Gorilla meal")
pruef(_r "Se_on bank=0x3B id=12 -> Runde 38 Spur F")
pruef(_r "Se_on bank=0x3B id=10 -> Runde 38 Spur F")
pruef(_r "Se_on bank=0x3B id=11 -> Runde 38 Spur F")
pruef(_r "[msg] room=1141 id=11")
pruef_nicht(_r "[msg] room=1141 id=0 ")
pruef_nicht(_r "[msg] room=1141 id=1 ")
pruef_nicht(_r "[msg] room=1141 id=2 ")
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
# Punkt 6: der Glasbruch klingt am ENDE von Johns Sprachzeile zu msg 5 (Warte-Zeile msg 58), nicht mitten im Satz
string(REGEX MATCH "GLAS1090 Glasbruch\\) F([0-9]+)" _gm "${_r}")
set(_glas_f "${CMAKE_MATCH_1}")
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
set(_geste15 0)
set(_b67 0)
set(_cut7_frei 0)
set(_m5_ende "")
set(_frei_nach_rolladen 0)
set(_entriegelt_z "")
set(_heben "")
set(_heben_vor_john FALSE)
set(_schuesse 0)
set(_treffer 0)
set(_geste17 0)
set(_zielt_auf_elza 0)
set(_sprung_max 0)
set(_sprung_f "")
set(_vor_jx "")
set(_vor_jz "")
set(_vor_anz "")
set(_vor_s1 0)
foreach(_z IN LISTS _zeilen)
    # CMake kennt nur CMAKE_MATCH_1..9: Hauptteil mit 8 Gruppen, b67/blick einzeln.
    if(NOT _z MATCHES "^F([0-9]+) cam=[0-9]+ anz=([0-9]+) msg=([01])/([0-9]+) john=[01] \\((-?[0-9]+),(-?[0-9]+)\\) rot=[0-9]+ st=[0-9]+ s1=([0-9]+) s2=[0-9]+ mo=(-?[0-9]+) ")
        continue()
    endif()
    set(_f ${CMAKE_MATCH_1})
    set(_anz ${CMAKE_MATCH_2})
    set(_ma ${CMAKE_MATCH_3})
    set(_mid ${CMAKE_MATCH_4})
    set(_jx ${CMAKE_MATCH_5})
    set(_jz ${CMAKE_MATCH_6})
    set(_s1 ${CMAKE_MATCH_7})
    set(_mo ${CMAKE_MATCH_8})
    if(NOT _z MATCHES " b67=([01]) ")
        continue()
    endif()
    set(_b67z ${CMAKE_MATCH_1})
    if(NOT _z MATCHES " blick=(-?[0-9]+)$")
        continue()
    endif()
    set(_bl ${CMAKE_MATCH_1})
    if(_bl LESS 0)
        math(EXPR _bl "-(${_bl})")
    endif()
    if(NOT _z MATCHES " 6a=([01]) .* pm=([0-9]+) c3=([01]) w=(-?[0-9]+) sch=([0-9]+)/([0-9]+) ")
        continue()
    endif()
    set(_6a ${CMAKE_MATCH_1})
    set(_pm ${CMAKE_MATCH_2})
    set(_c3 ${CMAKE_MATCH_3})
    set(_w ${CMAKE_MATCH_4})
    set(_schuesse ${CMAKE_MATCH_5})
    set(_treffer ${CMAKE_MATCH_6})
    if(NOT _z MATCHES " elza=\\((-?[0-9]+),(-?[0-9]+)\\) ")
        continue()
    endif()
    set(_ez ${CMAKE_MATCH_2})
    # N4.1: nach dem Rolladen frei laufen, solange die Dachtuer verriegelt ist
    if(_6a EQUAL 1 AND _c3 EQUAL 0 AND _pm EQUAL 0)
        math(EXPR _frei_nach_rolladen "${_frei_nach_rolladen} + 1")
    endif()
    if(_c3 EQUAL 1 AND _entriegelt_z STREQUAL "")
        set(_entriegelt_z ${_ez})
    endif()
    # N4.2: Waffe heben (W-Clip 6 Schusswaffe / 13 Messer-DRAW) bei Kamera Cut 7, bevor John im Raum ist
    if(_heben STREQUAL "" AND (_w EQUAL 6 OR _w EQUAL 13))
        set(_heben "F${_f} anz=${_anz} w=${_w} elza_z=${_ez}")
        if(_anz EQUAL 7 AND _jx LESS -29000)
            set(_heben_vor_john TRUE)
        endif()
    endif()
    # M2 / Nachtrag 7: Heranwinken (0,2) zu msg 52 (ROOM1171 @0x014CE, geliehen); nie Sub 2 Clip 6 (Zielen) waehrend msg 51/52
    if(_ma EQUAL 1 AND _mid EQUAL 52 AND _s1 EQUAL 0 AND _mo EQUAL 2)
        set(_geste17 1)
    endif()
    if(_ma EQUAL 1 AND (_mid EQUAL 51 OR _mid EQUAL 52) AND _s1 EQUAL 2 AND _mo EQUAL 6)
        math(EXPR _zielt_auf_elza "${_zielt_auf_elza} + 1")
    endif()
    # M3: John springt nicht (Schritt je Bild ohne Kameraschnitt und ohne Lauf-Sub 4/5/7/8, nicht geparkt)
    # (Lauf-Sub im Bild ODER im Bild davor: der letzte Laufschritt faellt ins Ankunftsbild, in dem Sub 5 schon zu 6 wird)
    set(_geht FALSE)
    foreach(_sx ${_s1} ${_vor_s1})
        if(_sx EQUAL 4 OR _sx EQUAL 5 OR _sx EQUAL 7 OR _sx EQUAL 8)
            set(_geht TRUE)
        endif()
    endforeach()
    if(NOT _vor_jx STREQUAL "" AND _jx GREATER -29000 AND _vor_jx GREATER -29000 AND _anz EQUAL _vor_anz AND NOT _geht)
        math(EXPR _dx "${_jx} - ${_vor_jx}")
        math(EXPR _dz "${_jz} - ${_vor_jz}")
        if(_dx LESS 0)
            math(EXPR _dx "-(${_dx})")
        endif()
        if(_dz LESS 0)
            math(EXPR _dz "-(${_dz})")
        endif()
        if(_dx GREATER _sprung_max)
            set(_sprung_max ${_dx})
            set(_sprung_f "F${_f}")
        endif()
        if(_dz GREATER _sprung_max)
            set(_sprung_max ${_dz})
            set(_sprung_f "F${_f}")
        endif()
    endif()
    set(_vor_jx ${_jx})
    set(_vor_jz ${_jz})
    set(_vor_anz ${_anz})
    set(_vor_s1 ${_s1})
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
    if(_ma EQUAL 1 AND _mid EQUAL 5)
        set(_m5_ende ${_f})
    endif()
    if(_ma EQUAL 1 AND _mid EQUAL 51 AND _bl LESS 129)
        set(_b51_blick 1)
    endif()
    if(_ma EQUAL 1 AND _mid EQUAL 51 AND _s1 EQUAL 0 AND _mo EQUAL 15)
        set(_geste15 1)
    endif()
    if(_b67z EQUAL 1)
        set(_b67 1)
        if(NOT _anz EQUAL 7)
            set(_cut7_frei 1)
        endif()
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
set(_b6b FALSE)
if(NOT _glas_f STREQUAL "" AND NOT _m5_ende STREQUAL "")
    math(EXPR _gd "${_glas_f} - ${_m5_ende}")
    if(_gd GREATER -4)
        set(_b6b TRUE)
    endif()
endif()
pruef_wahr(_b6b "Punkt 6: Glasbruch (F${_glas_f}) erst am Ende von msg 5 (zuletzt offen F${_m5_ende}), nicht mitten in Johns Satz")
pruef_wahr(_b51_blick "Punkt 8: John blickt bei msg 51 zu Elza")
pruef_wahr(_geste15 "Punkt 8: Geste Clip 15 zu msg 51")
pruef_wahr(_geste17 "Punkt 8 / M2 / Nachtrag 7: Heranwinken (0,2) zu msg 52 (ROOM1171 sub02 @0x014CE)")
pruef(_r "John winkt heran: RBJ ROOM1171 Record 1 Clip 2 geliehen (96 Bilder")
set(_bm2 FALSE)
if(_zielt_auf_elza EQUAL 0)
    set(_bm2 TRUE)
endif()
pruef_wahr(_bm2 "M2/N4.4: John richtet im Gespraech nie die Waffe auf Elza (Sub 2 Clip 6: ${_zielt_auf_elza} Bilder)")
set(_bm3 FALSE)
if(_sprung_max LESS 41)
    set(_bm3 TRUE)
endif()
pruef_wahr(_bm3 "M3: John springt nie im Bild (groesster Schritt ohne Schnitt/Lauf ${_sprung_max} bei ${_sprung_f}; Abnahme: 138)")
set(_b41 FALSE)
if(_frei_nach_rolladen GREATER 100)
    set(_b41 TRUE)
endif()
pruef_wahr(_b41 "N4.1: nach dem Rolladen frei laufen, Dachtuer noch verriegelt (${_frei_nach_rolladen} freie Bilder)")
message(STATUS "     entriegelt bei Elza z=${_entriegelt_z}; Waffe hoch: ${_heben}")
set(_b42 FALSE)
if(_heben_vor_john AND _heben MATCHES "elza_z=-1[0-9][0-9][0-9][0-9]" AND _entriegelt_z GREATER -2000)
    set(_b42 TRUE)
endif()
pruef_wahr(_b42 "N4.2: entriegelt an der Tuer, gerannt (Waffe hoch bei z < -10000), Waffe hoch bei Cut 7 bevor John da ist")
set(_b43 FALSE)
if(_schuesse GREATER 1 AND _treffer GREATER 0)
    set(_b43 TRUE)
endif()
pruef_wahr(_b43 "N4.3: John schiesst (${_schuesse} Schuesse, ${_treffer} Treffer)")
pruef_wahr(_b67 "Abgang: (9,67) = 1 John hat 1141 verlassen")
pruef_wahr(_cut7_frei "nach dem Abgang verlaesst die Kamera Cut 7 (Cut_old/Cut_auto, Runde 38 Fortsetzung 4)")

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
