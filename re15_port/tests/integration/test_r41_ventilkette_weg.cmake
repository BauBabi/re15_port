# =============================================================================
# VENTILKETTE, Nachbesserung 3 (Runde 41 Spur C, Abnahme 2 Maengel N1/N2/N4; Nachtraege 7/9/10 des Nutzers): die ganze Kette
# in ROOM2030 und der Weg nach der Flut zu Fuss an der echten exe, je Figur (FIGUR=leon -> ROOM20x0, FIGUR=elza -> 20x1).
# Herleitung: Dossier analysis/befunde_runde41/C_ventilkette.md §0d; Konstanten include/re15_ventilkette_r41.h.
# Wegplan-Treiber RE15_WEGPLAN (dachkiste_r36.c: echte Pad-Bits aus Spieler-/Ziellage, Aktionstaste, kein RE15_FIRE_AOT);
# die zwei Raetsel in 2060 (Cursor-Steuerung) mit RE15_INPUT_SCRIPT (Spielbild-Achse). Glieder:
#
#   v1  2030 (Ankunft an der Ost-Tuer, Ring Band 3): Pult ohne Druck (Text 40) -> Treppe Slot 8 (@0x01B8E) hinab ->
#       Steg-Zelle 12 gesperrt (u0 0xff, Bruecke oben) -> Ventil 13 (Text 41 -> Ja -> 42, Bit 101) -> Treppe hinauf ->
#       Pult "Lower the bridge?" (43) -> Ja -> Cut 4, Bruecke obj 0 auf -1800, Bit 102 -> Treppe hinab -> Zelle 12 offen.
#   v2a 2030 (Nachbesserung 5, Nachtrag 16 Folge 4; vom Ring, Bits 101/102): Treppe -> Steg -> Sued-Landeplatz -> Ventil 09
#       OHNE die Sicherung im Ventilkasten 2010 = Text 49 "It's locked." (Original ROOM20A0 msg 1 @0x203C), kein Zaehler ->
#       Sicherung aufheben (Original Slot 9 @0x01BAA -> sub06: msg 2 -> Ja -> Set(3,108) @0x01FF2 -> msg 3) -> Ventil 09 bleibt
#       "It's locked." (die Sicherung in der Hand gibt die Ventile nicht frei), Bit (9,125) bleibt 0.
#   v2b 2030 (Sued-Landeplatz, Bits 101/102 + (3,108) + (9,125) = Sicherung im Ventilkasten): richtige Folge 09/07/02/01 zu
#       Fuss, je Drehung die RE2-Dreh-Animation (RBJ von RE2 ROOM1080, sub08 @0x0210A..@0x0221E, Clip 17 je Drehung 5x) ->
#       Warnung 46 im Wortlaut des Nachtrags 19 -> Zaehler -> zu Fuss (Steg, Treppe, Ring) -> Ost-Tuer -> ROOM20B0 ("Zaehler lief
#       beim Verlassen ... Bits 103 + 100"), keine Flut.
#   v2c 2030 (wie v2b; vormals zweite Haelfte von v2a): die Folge des Plakats 09/01/02/07 zu Fuss (Sued, Nord, Nord, Sued) ->
#       nach der vierten Drehung "FALSCH: zurueckgesetzt" + Text 45, (5,0x10) = 0, kein Zaehler.
#   v2d Dokument "Technician's Note" (Slot 20; seit Nachbesserung 5 ein Zettel an der Suedwand neben dem Schrank mit der
#       Sicherung, Platz x -5100..-4600 z 7400..8400): Aktion -> Leser (Titel, 2 Seiten, RECHTS blaettert, wie
#       integration_r38_tagebuch) -> "has been filed" -> Bit (9,107); Framedump der Textseite.
#   v2p Plakat (Slot 21, Suedwand): Aktion -> Nahaufnahme (Vollbild, Pad-Sperre), Viereck schliesst; die Bildmitte ist im
#       offenen Zustand das helle Blatt (ppm-Werkzeug von test_r41_ventilkette), danach wieder der Raum.
#       Die Pad-Sperre (2,7) zieht player_mode auf 2 - dort gibt der Wegplan-Treiber keine Tasten (dachkiste_r36.c @353);
#       v2d/v2p laufen deshalb mit RE15_PRESS (feste Bilder) an der Stelle (RE15_PLAYER_POS).
#   g1  2060: Sicherung einsetzen = Original sub18 (msg 3/4, Ja, Cut 8, Cursor obj 4) -> Zone 7 + SQUARE -> sub19
#       Set(3,144) @0x0169E, msg 5.
#   g2  2060: Generator = Original sub12 (msg 0, Ja, Cut 7, Cursor obj 3) -> Schalter 1 (Zone 3) + 3 (Zone 5) ->
#       Loesung sub01 @0x01296..@0x012A6 -> sub13 + Set(3,126) @0x012AE, msg 2 "It seems that the generator is working now."
#   k1  2010 Ventil-Sicherungskasten (Nachbesserung 5, Nachtrag 16 Folgen 1/2; Slot 8, Ereignis 20, Ostwand x 5400): mit der
#       aufgehobenen Sicherung (3,108) Text 40 -> Frage 41 -> Ja -> Set(9,125) -> Standbild der Ventile (ROOM2030 Cut 7, Vollbild;
#       Framedump == BSS/ROOM2030/BG07.BSS ueber das Werkzeug "bss") + Aufschliess-Ton -> Bestaetigen schliesst -> Text 42.
#       Das Standbild steht unter der Pad-Sperre (2,7) (player_mode 2, der Wegplan gibt dort keine Tasten) -> RE15_PRESS.
#   k2  nach der Flut, die Sicherung steckt im Ventilkasten (Nachtrag 16 Folgen 3/6, ganzer Weg zu Fuss): 20B0 -> Kanal ->
#       Westgang -> Tuer slot 2 (@0x019A2) -> 2010 -> Kasten: Text 43 -> Ja -> Set(9,125)=0, Text 44 -> Tuer slot 0 (nach der Flut
#       auf 20B0 umgelenkt) -> Leiter -> 2050 -> 2020 -> 2030 Nordgalerie -> 2060 -> Generator-Kasten = Original sub18 (msg 3,
#       Frage msg 4 "Will you use the Fuse?").
#   k3  2060 mit der Sicherung im Ventilkasten (Bit 125): Generator-Kasten Slot 9 = Text 3 des Else-Zweigs @0x010A6, keine Frage,
#       kein Set(3,144).
#   w1  nach der Flut, ohne Strom: 20B0 (nach der Szene) -> Kanal (Sprung hinab / Aufstieg) -> Leiter -> 2050 (Kisten-
#       Raetsel (3,109) gesetzt, Zelle 51 frei) -> 2020 (Kurbel (3,113)/(3,219) gesetzt) -> 2030 Nordgalerie
#       ("unter Wasser (y -7200)") -> 2060.
#   w3  nach dem Strom: 2060 -> 2030 Nordgalerie ("Strom (3,126) -> Wasser knapp unter der Absperrung y -5350") -> 2020 -> 2050 ->
#       Leiter hinab -> ROOM20B0 (die Nordgalerie hat keinen Lift; den Ring erreicht man ueber 20B0).
#   w4  nach dem Strom: 20B0 (Leiter-Ankunft) -> Kanal -> Ost-Tuer slot 3 (offen, Nachtrag 7) -> 2030 Ring (trocken) ->
#       Lift B (Slot 7, Ereignis 4, (3,220) 0 -> 1) -> Suedgalerie -> Tuer slot 1 -> 2080 -> Treppe -> Rechner = ORIGINAL
#       sub02 (msg 0, Cut 7, (3,245)), kein Port-Programm -> Tuer slot 1 nach 2090 -> ROOM2090.
# Gegner sind nicht Gegenstand des Riegels: Spinnen 2030 (7,155..158) und 2060 (7,122)/(7,123) als getoetet (Kill-Bit
# pc[7] der Sce_em_set, EM-Status-Gate FUN_800420a0 @0x80042120-38); in 2050/2020/2060/2080 GEGNERWEG (R36-Konvention).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_R41C_WERKZEUG=<test_r41_ventilkette> -DRE15_R41C_PSX=<RE15_ASSETS_PATH>
#               -DWORKDIR=<dir> -DFIGUR=leon|elza -P test_r41_ventilkette_weg.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_ventilkette_weg: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_ventilkette_weg: WORKDIR fehlt")
endif()
if(NOT RE15_R41C_WERKZEUG OR NOT EXISTS "${RE15_R41C_WERKZEUG}")
    message(FATAL_ERROR "r41_ventilkette_weg: RE15_R41C_WERKZEUG (test_r41_ventilkette, ppm-Mittel) fehlt: '${RE15_R41C_WERKZEUG}'")
endif()
if(NOT RE15_R41C_PSX OR NOT EXISTS "${RE15_R41C_PSX}/BSS")
    message(FATAL_ERROR "r41_ventilkette_weg: RE15_R41C_PSX (RE15_ASSETS_PATH, BSS-Baum fuer k1) fehlt: '${RE15_R41C_PSX}'")
endif()
if(NOT FIGUR STREQUAL "leon" AND NOT FIGUR STREQUAL "elza")
    message(FATAL_ERROR "r41_ventilkette_weg: FIGUR muss leon oder elza sein ('${FIGUR}')")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake), Praefix je Figur.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41cw${FIGUR}_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r41cw${FIGUR}_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r41cw${FIGUR}_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(_fehler _was)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_ventilkette_weg[${FIGUR}]${_was}")
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
# Folge ALLER Wegplan-Lesungen von FLAG (Bank,Bit) muss genau <_soll> sein (Liste, z.B. "0;1").
function(_flagfolge _wp _name _bank _bit _soll)
    string(REGEX MATCHALL "FLAG [(]${_bank},${_bit}[)] = [01]" _fl "${_wp}")
    set(_ist "")
    foreach(_e IN LISTS _fl)
        string(REGEX REPLACE ".* = " "" _w "${_e}")
        list(APPEND _ist "${_w}")
    endforeach()
    if(NOT "${_ist}" STREQUAL "${_soll}")
        _fehler("[${_name}]: FLAG (${_bank},${_bit}) gelesen '${_ist}', erwartet '${_soll}'")
    endif()
endfunction()

# Figur: Leon = Titel-Bild-Haken, Elza = Spielerwahl wie integration_r36_dachkiste; Raum-Endziffer 0/1.
if(FIGUR STREQUAL "leon")
    set(_env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1
             RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
    set(S 0)
else()
    set(_env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1
             RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
    set(S 1)
endif()

macro(_lauf _name _timeout _plan)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_wp_arg "")
    if(NOT "${_plan}" STREQUAL "")
        file(WRITE "${WORKDIR}/wegplan_plan.txt" "${_plan}")
        set(_wp_arg "RE15_WEGPLAN=${WORKDIR}/wegplan_plan.txt" RE15_WEGPLAN_LOG=wegplan.txt)
    endif()
    re15_start_spiel(_rv ${_timeout} ${_env} ${_wp_arg} ${ARGN} "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        _fehler("[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    set(_wp "")
    if(EXISTS "${WORKDIR}/wegplan.txt")
        file(READ "${WORKDIR}/wegplan.txt" _wp)
    endif()
    if(FIGUR STREQUAL "elza")
        _muss("${_dbg}" "Spieler-Familie PL04" ${_name} "Elza gewaehlt (RE15_PSELECT_AUTO_SWITCH)")
    endif()
endmacro()

# Zustand nach der Flut (alles, was die Glieder davor setzen; Original-Raetsel ausserhalb der Kette als geloest):
# 101/102 Ventil 13 + Bruecke, (3,108) Sicherung, 103/100 Flut + Leiter, (3,235) 20B0-Szene gelaufen, 106 Pult 2050,
# (3,109) Kisten 2050, (3,114)/(3,113)/(3,219) Hinweis 2010 / Kurbel 2020 / Tuer 2020->2030; Spinnen 2030.
set(_nach "9:101,9:102,3:108,9:103,9:100,3:235,9:106,3:109,3:114,3:113,3:219,7:155,7:156,7:157,7:158")

# ---- v1: Bruecke zu Fuss -----------------------------------------------------------------------------------------------
_lauf(v1 420 "RAUM 203${S}
WARTE 30
FLAG 9 101
FLAG 9 102
FLAG 3 108
GEHE 6575 -9675
GEHE 350 -9675
BLICK 1024
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 120
AKTION
WARTE 60
GEHE -6800 -9675
GEHE -6350 -7700 80
BLICK 0
WARTE 20
AKTION
WARTE 300
GEHE -1050 -7700
GEHE -1050 -7000 100
ZELLE 0 12
GEHE -1500 -6400
BLICK 2048
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 150
AKTION
WARTE 60
FLAG 9 101
GEHE -1900 -7700 80
BLICK 2048
WARTE 20
AKTION
WARTE 300
GEHE -6800 -9675
GEHE 350 -9675
BLICK 1024
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 240
FLAG 9 102
AKTION
WARTE 120
AKTION
WARTE 30
GEHE -6800 -9675
GEHE -6350 -7700 80
BLICK 0
WARTE 20
AKTION
WARTE 300
GEHE -1050 -7700
ZELLE 0 12
FLAG 9 102
ENDE
" RE15_DEBUG_JUMP=2030@250 RE15_SET_FLAG=7:155,7:156,7:157 "RE15_PLAYER_POS=6550,-100,2048,3" "RE15_EXIT_AT=3900#203${S}")
_muss("${_dbg}" "Ereignis 24 -> Port-Programm 2030_pult_ohne" v1 "Pult ohne Druck")
_muss("${_dbg}" "[msgtext] room=203${S} id=40 \"A control panel for the bridge." v1 "Text 40")
_muss("${_wp}" "ZELLE r0[12] LAUFZEIT: x -2050..-50 z -5950..5450 typ 1 u0 0xff" v1 "Steg-Zelle 12 mit der Bruecke oben gesperrt")
_danach("${_dbg}" "Port-Programm 2030_v13_frage" "[msgtext] room=203${S} id=42" v1 "Ventil 13 geoeffnet (Text 42)")
_flagfolge("${_wp}" v1 9 101 "0;1")
_danach("${_dbg}" "Port-Programm 2030_pult_senken" "Cut_chg(4)" v1 "Bruecke faehrt unter Cut 4")
_muss("${_dbg}" "obj=0x00 m=1" v1 "Member_set der Bruecke (obj 0)")
_flagfolge("${_wp}" v1 9 102 "0;1;1")
_muss("${_wp}" "ZELLE r0[12] LAUFZEIT: x -2050..-50 z -5950..5450 typ 1 u0 0x00" v1 "Steg-Zelle 12 nach dem Senken offen")
_darf_nicht("${_dbg}" "2030_ventil" v1 "kein Ventilraetsel in v1")
message(STATUS "r41_ventilkette_weg[${FIGUR}] v1: Pult 40 -> Ventil 13 -> Pult 43 -> Bruecke unten (Bit 102), Steg offen")

# ---- v2a: Ventile ohne die Sicherung im Ventilkasten 2010 = "It's locked." - auch mit der Sicherung in der Hand -------------
# (Nachbesserung 5, Nachtrag 16 Folge 4). Ventil-Plaetze (Kopf re15_ventilkette_r41.h "Ventil-RAEDER"): 09 (-2650) Suedwand
# z 7900 -> Spieler z 7280 Blick +z (3072); Steg ueber x -1050 (Zelle 12). Gemessen 2026-10-25 (Messlauf nb5 v2an, Leon): letzte
# Lesung F2065 -> Ende F2200.
_lauf(v2a 420 "RAUM 203${S}
WARTE 30
FLAG 3 108
FLAG 9 125
GEHE 6575 -9675
GEHE -6800 -9675
GEHE -6350 -7700 80
BLICK 0
WARTE 20
AKTION
WARTE 300
GEHE -1050 -7700
GEHE -1050 6000
GEHE -1050 6900 100
GEHE -2650 7280 100
BLICK 3072
WARTE 10
AKTION
WARTE 120
AKTION
WARTE 60
GEHE -4700 7150 100
BLICK 2048
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 150
AKTION
WARTE 60
FLAG 3 108
GEHE -2650 7280 100
BLICK 3072
WARTE 10
AKTION
WARTE 120
AKTION
WARTE 60
FLAG 3 108
FLAG 9 125
ENDE
" RE15_DEBUG_JUMP=2030@250 RE15_SET_FLAG=7:155,7:156,7:157,9:101,9:102 "RE15_PLAYER_POS=6550,-100,2048,3"
      "RE15_EXIT_AT=2200#203${S}")
_muss("${_dbg}" "Ereignis 22 -> Port-Programm 2030_ventile_vorher" v2a "Ventil 09 ohne Kasten-Sicherung")
_muss("${_dbg}" "[msgtext] room=203${S} id=49 \"It's locked.\"" v2a "Text 49 = Original ROOM20A0 msg 1 @0x203C (Nachtrag 16 Folge 4)")
_danach("${_dbg}" "[msgtext] room=203${S} id=49" "[msgtext] room=203${S} id=2 \"Will you take the Fuse?\"" v2a "danach die Sicherung (Original sub06)")
_danach("${_dbg}" "id=2 \"Will you take the Fuse?\"" "[msgtext] room=203${S} id=3 \"You've taken the Fuse.\"" v2a "Sicherung genommen")
_danach("${_dbg}" "You've taken the Fuse." "[msgtext] room=203${S} id=49 \"It's locked.\"" v2a "mit der Sicherung in der Hand weiter verschlossen")
_flagfolge("${_wp}" v2a 3 108 "0;1;1")
_flagfolge("${_wp}" v2a 9 125 "0;0")
_darf_nicht("${_dbg}" "Port-Programm 2030_ventil_09" v2a "ohne Kasten-Sicherung keine Ventil-Frage")
_darf_nicht("${_dbg}" "Zaehler laeuft" v2a "kein Zaehler")
message(STATUS "r41_ventilkette_weg[${FIGUR}] v2a: Ventil 09 ohne Kasten-Sicherung = 49 \"It's locked.\" -> Sicherung aufgehoben -> weiter 49, Bit 125 = 0")

# ---- v2b: richtige Folge 09/07/02/01 zu Fuss -> Warnung -> Zaehler -> Flucht zur Ost-Tuer -> 20B0 -------------------------
# Nachbesserung 5: je Ja die RE2-Dreh-Animation (345 Takte; der Wegplan ruht waehrend player_mode 2) -> WARTE 450; gemessen
# 2026-10-24 (Messlauf nb5 v2bc, Leon): Warnung zu F4483, Countdown ab F4500, Ost-Tuer zu Fuss, 20B0.
_lauf(v2b 600 "RAUM 203${S}
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
FLAG 5 29
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
RAUM 20B${S}
WARTE 30
ENDE
" RE15_DEBUG_JUMP=2030@250 RE15_SET_FLAG=7:155,7:156,7:157,9:101,9:102,3:108,9:125 "RE15_PLAYER_POS=-2650,7280,3072,1"
      "RE15_EXIT_AT=200#20B${S}")
_muss("${_dbg}" "bindet das RBJ von RE2 ROOM1080 (VENTIL1080.RBJ" v2b "RE2-Ventil-RBJ fuer 203${S} gebunden")
string(REGEX MATCHALL "Plc_motion[(]entity=0, motion=17, flags=0x00[)]" _m17 "${_dbg}")
list(LENGTH _m17 _n_m17)
if(NOT _n_m17 EQUAL 20)
    _fehler("[v2b]: ${_n_m17} Mal Clip 17 statt 20 (4 Drehungen x 5, RE2 ROOM1080 sub08 For 2 @0x02178, For 2 @0x021AE, einzeln @0x021CE)")
endif()
_danach("${_dbg}" "Ventil 09 gedreht (Drehung 1/4)" "Ventil 07 gedreht (Drehung 2/4)" v2b "09, dann 07")
_danach("${_dbg}" "Ventil 07 gedreht (Drehung 2/4)" "Ventil 02 gedreht (Drehung 3/4)" v2b "dann 02 (Nordwand)")
_danach("${_dbg}" "Ventil 02 gedreht (Drehung 3/4)" "Ventil 01 gedreht (Drehung 4/4)" v2b "dann 01")
_danach("${_dbg}" "Ventil 01 gedreht (Drehung 4/4)" "[ventilkette41] Folge 09 07 02 01 -> RICHTIG: Warnung, Zaehler, Flut" v2b "richtige Folge")
_danach("${_dbg}" "RICHTIG: Warnung" "[msgtext] room=203${S} id=46 \"WARNING: The water will rise in one minute. Please leave the room immediately!\"" v2b "Warnung 46 im Wortlaut des Nachtrags 19")
_danach("${_dbg}" "id=46" "[ventilkette41] Zaehler laeuft: 1800 Takte" v2b "Zaehler gestartet")
_flagfolge("${_wp}" v2b 5 29 "1")
_danach("${_dbg}" "Zaehler laeuft: 1800 Takte" "[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750) target_cut=11" v2b "Ost-Tuer zu Fuss erreicht")
string(TOLOWER "20B${S}" _k20b0)
_muss("${_dbg}" "loaded room${_k20b0}.rdt" v2b "20B0 geladen")
_muss("${_dbg}" "[ventilkette41] Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100" v2b "Flut gelaufen + Leiter frei")
_darf_nicht("${_dbg}" "FLUT: Wasser steigt" v2b "der Weg zur Tuer passt in die 60 s")
_darf_nicht("${_dbg}" "ERTRUNKEN" v2b "nicht ertrunken")
_darf_nicht("${_dbg}" "-> FALSCH: zurueckgesetzt" v2b "keine falsche Folge")
message(STATUS "r41_ventilkette_weg[${FIGUR}] v2b: 09/07/02/01 zu Fuss mit RE2-Dreh-Animation (20 x Clip 17) -> RICHTIG -> 46 (Nachtrag 19) -> Zaehler -> zu Fuss zur Ost-Tuer -> 20B0 (103+100)")

# ---- v2c: die Folge des Plakats 09/01/02/07 = falsch -> zurueck (Nachtrag 13; Sicherung im Ventilkasten) ----------------------
# Gemessen 2026-10-25 (Messlauf nb5 v2c, Leon): FALSCH + Text 45 nach der vierten Drehung, letzte Lesung F4766 -> Ende F4900.
_lauf(v2c 600 "RAUM 203${S}
WARTE 30
GEHE -2650 7280 100
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
GEHE -1350 -7880 100
BLICK 1024
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
GEHE 150 -7880 100
BLICK 1024
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
GEHE -1050 -7400 100
GEHE -1050 -6400
GEHE -1050 6000
GEHE -1050 6900 100
GEHE 2600 7280 100
BLICK 3072
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
FLAG 5 19
AKTION
WARTE 60
FLAG 5 16
FLAG 5 29
ENDE
" RE15_DEBUG_JUMP=2030@250 RE15_SET_FLAG=7:155,7:156,7:157,9:101,9:102,3:108,9:125 "RE15_PLAYER_POS=-2650,7280,3072,1"
      "RE15_EXIT_AT=4900#203${S}")
_danach("${_dbg}" "[msgtext] room=203${S} id=55 \"Valve 09, red wheel. Will you turn it?\"" "Ventil 09 gedreht (Drehung 1/4)" v2c "Ventil 09: Frage 55 -> gedreht")
_danach("${_dbg}" "Ventil 09 gedreht (Drehung 1/4)" "[msgtext] room=203${S} id=50 \"Valve 01, black wheel. Will you turn it?\"" v2c "Ventil 01 (Nordwand): Frage 50")
_danach("${_dbg}" "Ventil 01 gedreht (Drehung 2/4)" "[msgtext] room=203${S} id=51 \"Valve 02, green wheel. Will you turn it?\"" v2c "Ventil 02: Frage 51")
_danach("${_dbg}" "Ventil 02 gedreht (Drehung 3/4)" "[msgtext] room=203${S} id=53 \"Valve 07, black wheel. Will you turn it?\"" v2c "Ventil 07: Frage 53")
_danach("${_dbg}" "Ventil 07 gedreht (Drehung 4/4)" "[ventilkette41] Folge 09 01 02 07 -> FALSCH: zurueckgesetzt" v2c "Pruefung erst nach der vierten Drehung")
_danach("${_dbg}" "FALSCH: zurueckgesetzt" "[msgtext] room=203${S} id=45 \"The valves turned back to their original position.\"" v2c "Text 45")
# (5,0x13) und (5,0x10): das Programm 2030_ventile_falsch setzt die Drehungen sofort zurueck (vor dem Text 45), die
# vierte Drehung selbst belegt die Zeile "Ventil 07 gedreht (Drehung 4/4)".
_flagfolge("${_wp}" v2c 5 19 "0")
_flagfolge("${_wp}" v2c 5 16 "0")
_flagfolge("${_wp}" v2c 5 29 "0")
_darf_nicht("${_dbg}" "Zaehler laeuft" v2c "falsche Folge: kein Zaehler")
_darf_nicht("${_dbg}" "room=203${S} id=46" v2c "falsche Folge: keine Warnung")
message(STATUS "r41_ventilkette_weg[${FIGUR}] v2c: Plakat-Folge 09/01/02/07 zu Fuss (Kasten-Sicherung gesteckt) -> FALSCH, Text 45, zurueck")

# ---- v2d: Dokument "Technician's Note" (Slot 20, Item 0x4E) -> Leser -> abgelegt, Bit (9,107) ----------------------------
# (Framedump-Pfade relativ: die exe laeuft im Laufordner; ${WORKDIR} setzt erst das Makro _lauf.)
# Nachbesserung 5 (Abnahme 4 Mangel 1): der Zettel haengt an der Suedwand (Mitte (-4850,-3950,7880)); Spieler (-4850,7280)
# Blick +z -> Vorwaertspunkt (-4850,7900) im Platz x -5100..-4600 z 7400..8400 (Wandflaeche z 7900 +- 500). Leser wie
# integration_r38_tagebuch: Viereck oeffnet, RECHTS x3 bis zur Ende-Stellung (Titel + 2 Seiten, max_page 2), Viereck
# schliesst, Viereck bestaetigt "has been filed".
_lauf(v2d 180 "" RE15_DEBUG_JUMP=2030@250 RE15_SET_FLAG=7:155,7:156,7:157,9:101,9:102 "RE15_PLAYER_POS=-4850,7280,3072,1"
      "RE15_PRESS=square@330,right@480,right@510,right@540,square@570,square@680"
      "RE15_FRAMEDUMP=500:f_dok_500.ppm" "RE15_EXIT_AT=820#203${S}")
_muss("${_dbg}" "[ventilkette41] ROOM203${S}: Ventile 01/02/03/07/08/09 (Slots 14-19), Plakat Slot 21, Dokument Slot 20 (Item 0x4E)" v2d "Dokument-Zone angelegt")
_muss("${_dbg}" "[ventilkette41] Dokument 6 \"Technician's Note\" abgelegt: Bit (9,107) = 1" v2d "Dokument gelesen und abgelegt (Bit 107)")
if(NOT EXISTS "${WORKDIR}/f_dok_500.ppm")
    _fehler("[v2d]: Framedump der Textseite F500 fehlt")
endif()
message(STATUS "r41_ventilkette_weg[${FIGUR}] v2d: Dokument Technician's Note -> Leser -> abgelegt (9,107)")

# ---- v2p: Plakat-Nahaufnahme (Slot 21) ---------------------------------------------------------------------------------
_lauf(v2p 180 "" RE15_DEBUG_JUMP=2030@250 RE15_SET_FLAG=7:155,7:156,7:157,9:101,9:102 "RE15_PLAYER_POS=-3650,7280,3072,1"
      "RE15_PRESS=square@330,square@420" "RE15_FRAMEDUMP=380-460/80:f_plakat_"
      "RE15_EXIT_AT=480#203${S}")
_danach("${_dbg}" "Ereignis 23 -> Port-Programm 2030_plakat" "[ventilkette41] Plakat-Nahaufnahme offen" v2p "Plakat oeffnet")
_danach("${_dbg}" "Plakat-Nahaufnahme offen" "[ventilkette41] Plakat-Nahaufnahme zu" v2p "Viereck schliesst")
# Bildmitte (110..210, 90..150): offen (F380) das helle Blatt, zu (F460) der Raum. Seit Nachbesserung 5 (Nachtrag 18 Punkt 1)
# die Nahaufnahme im Stil des Spiels: Papier (200,193,170), Mittel der Flaeche im erzeugten Bild 168/160/142 (C_belege/
# nb5_plakat.png) - die Schwellen Rot >= 120 / Gruen >= 110 gelten unveraendert.
foreach(_pb 380 460)
    if(NOT EXISTS "${WORKDIR}/f_plakat_000${_pb}.ppm")
        _fehler("[v2p]: Framedump F${_pb} fehlt")
    endif()
    execute_process(COMMAND "${RE15_R41C_WERKZEUG}" ppm "${WORKDIR}/f_plakat_000${_pb}.ppm" 110 90 210 150 OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
    string(REGEX MATCH "MITTEL ([0-9]+) ([0-9]+) ([0-9]+)" _m "${_o}")
    if(NOT _m)
        _fehler("[v2p]: ppm-Werkzeug F${_pb}: ${_o}")
    endif()
    message(STATUS "r41_ventilkette_weg[${FIGUR}] v2p: Bildmitte F${_pb} = ${CMAKE_MATCH_1} ${CMAKE_MATCH_2} ${CMAKE_MATCH_3}")
    if(_pb EQUAL 380 AND (CMAKE_MATCH_1 LESS 120 OR CMAKE_MATCH_2 LESS 110))
        _fehler("[v2p]: F380 zeigt nicht das helle Plakat-Blatt in der Bildmitte (${_o})")
    endif()
    if(_pb EQUAL 460 AND NOT CMAKE_MATCH_1 LESS 100)
        _fehler("[v2p]: F460 zeigt noch das Plakat (${_o}) - Viereck hat nicht geschlossen")
    endif()
endforeach()
message(STATUS "r41_ventilkette_weg[${FIGUR}] v2p: Plakat -> Nahaufnahme (helles Blatt) -> Viereck schliesst")

# ---- g1/g2: 2060 Sicherung + Generator (Original-Raetsel) ---------------------------------------------------------------
_lauf(g1 240 "" RE15_FLAG_CENSUS=1 RE15_DEBUG_JUMP=2060@250 RE15_SET_FLAG=3:108,7:122,7:123 "RE15_PLAYER_POS=-4000,3800,3072,0"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330
      "RE15_INPUT_SCRIPT=A0.1,W3,A0.1,W3,A0.1,W3,A0.1,W3,R0.2,W0.5,A0.1,W3,A0.1,W3"
      "RE15_EXIT_AT=1100#206${S}")
_muss("${_dbg}" "[msgtext] room=206${S} id=4 \"Will you use the Fuse?\"" g1 "Frage am Sicherungskasten (sub18)")
_danach("${_dbg}" "Will you use the Fuse?" "Cut_chg(8)" g1 "Einsetz-Ansicht Cut 8")
_danach("${_dbg}" "Cut_chg(8)" "WRITE Set  zone=3 idx=144 op=1" g1 "Sicherung eingesetzt: sub19 Set(3,144) @0x0169E")
_muss("${_dbg}" "[msgtext] room=206${S} id=5 \"You've used the Fuse.\"" g1 "Text 5")
_lauf(g2 240 "" RE15_FLAG_CENSUS=1 RE15_DEBUG_JUMP=2060@250 RE15_SET_FLAG=3:108,3:144,7:122,7:123 "RE15_PLAYER_POS=-3200,3400,0,0"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330
      "RE15_INPUT_SCRIPT=A0.1,W3,A0.1,W3,A0.1,W3,A0.1,W3,A0.1,W3,L0.77,W0.5,D0.57,W0.5,A0.1,W1.5,R1.0,W0.5,A0.1,W5"
      "RE15_EXIT_AT=1300#206${S}")
_muss("${_dbg}" "[msgtext] room=206${S} id=0 \"The generator does not appear to be on." g2 "Frage am Generator (sub12)")
_danach("${_dbg}" "The generator does not appear" "Cut_chg(7)" g2 "Schalter-Ansicht Cut 7")
_danach("${_dbg}" "Cut_chg(7)" "WRITE Set  zone=5 idx=6 op=1" g2 "Schalter 1 (Zone 3, sub08)")
_danach("${_dbg}" "zone=5 idx=6 op=1" "WRITE Set  zone=5 idx=8 op=1" g2 "Schalter 3 (Zone 5, sub10)")
_danach("${_dbg}" "zone=5 idx=8 op=1" "WRITE Set  zone=3 idx=126 op=1" g2 "Loesung 1,0,1,0 -> Set(3,126) @0x012AE")
_muss("${_dbg}" "[msgtext] room=206${S} id=2 \"It seems that the generator is working now.\"" g2 "Text 2 (sub13)")
message(STATUS "r41_ventilkette_weg[${FIGUR}] g1/g2: Sicherung (3,144) + Generator (3,126) ueber die Original-Raetsel")

# ---- k1: Ventil-Sicherungskasten 2010 - Sicherung einsetzen (Nachbesserung 5, Nachtrag 16 Folgen 1/2) --------------------
# Spieler (4950,-2770) Blick +x -> Vorwaertspunkt (5570,-2770) im Platz x 4900..5900 z -3470..-2070 (Wandflaeche x 5400 +- 500).
# Tasten wie der Messlauf nb5 bx2 (2026-10-24 21:32, Leon): F300 Ereignis 20 -> Text 40 (2 Seiten), F520 Frage 41, F600 Ja ->
# Bit 125, Standbild + Aufschliess-Ton ab F603, F680 Bestaetigen -> Standbild zu (nach 77 Bildern), Text 42, F760 Text 42 zu.
# Framedumps F560 (Frage ueber dem Raum) und F640 (Standbild); Flaeche x 40..280 y 20..120 (ueber dem Nachrichtenfenster).
_lauf(k1 180 "" RE15_FLAG_CENSUS=1 RE15_DEBUG_JUMP=2010@250 RE15_SET_FLAG=3:108 "RE15_PLAYER_POS=4950,-2770,0,0"
      "RE15_PRESS=square@300,square@350,square@400,square@450,square@520,square@600,square@680,square@760"
      "RE15_FRAMEDUMP=560-640/80:f_kasten_" "RE15_EXIT_AT=840#201${S}")
_muss("${_dbg}" "[ventilkette41] ROOM201${S}: Ventil-Sicherungskasten Slot 8 (x 4900..5900 z -3470..-2070), Sicherung fehlt" k1 "Kasten angelegt")
_muss("${_dbg}" "[ventilkette41] ROOM201${S} Ereignis 20 -> Port-Programm 2010_kasten_frage" k1 "Aktion am Kasten (mit (3,108))")
_muss("${_dbg}" "[msgtext] room=201${S} id=40 \"A fuse box for the valve controls." k1 "Text 40")
_danach("${_dbg}" "id=40 \"A fuse box" "[msgtext] room=201${S} id=41 \"Will you use the Fuse?\"" k1 "Frage 41")
_danach("${_dbg}" "id=41 \"Will you use the Fuse?\"" "WRITE Set  zone=9 idx=125 op=1" k1 "Ja -> Set(9,125) (`22 09 7d 01`)")
_danach("${_dbg}" "zone=9 idx=125 op=1" "[ventilkette41] Standbild der Ventile offen (ROOM2030 Cut 7) + Aufschliess-Ton" k1 "Standbild + Ton")
_danach("${_dbg}" "Standbild der Ventile offen" "[ventilkette41] Standbild der Ventile zu" k1 "Bestaetigen schliesst das Standbild")
_danach("${_dbg}" "Standbild der Ventile zu" "[msgtext] room=201${S} id=42 \"You've used the Fuse.\"" k1 "Text 42 nach dem Standbild")
# Das Standbild ist der Hintergrund ROOM2030 Cut 7: der Framedump F640 wird Pixel fuer Pixel mit der dekodierten BSS-Scheibe
# verglichen (Werkzeug "bssdiff", dieselbe Dekodierkette wie ventilkette_r41_pc.c standbild_laden): GLEICH = Anteil der Pixel
# mit Abweichung <= 4 in Prozent. Gegenprobe F560 (Raum 2010 mit der Frage). Gemessen 2026-10-25 (Riegel-Lauf lauf_dfe7a2ea_k1,
# Software-Renderer): F640 ABWEICHUNG 0 GLEICH 100, F560 ABWEICHUNG 26 GLEICH 13; Messlauf nb5 bx2 (beschleunigt): F640 100,
# F560/F700 9. (Ein Flaechenmittel taugt als Gegenprobe nicht: F560 25/25/20 gegen 19/20/16 im selben Lauf.)
set(_bss07 "${RE15_R41C_PSX}/BSS/ROOM2030/BG07.BSS")
if(NOT EXISTS "${_bss07}")
    _fehler("[k1]: ${_bss07} fehlt (RE15_R41C_PSX = RE15_ASSETS_PATH der exe)")
endif()
function(_gleich _bild _out)
    set(_ppm "${WORKDIR}/f_kasten_000${_bild}.ppm")
    if(NOT EXISTS "${_ppm}")
        _fehler("[k1]: Framedump F${_bild} fehlt")
    endif()
    execute_process(COMMAND "${RE15_R41C_WERKZEUG}" bssdiff "${_bss07}" "${_ppm}" 0 0 320 240 OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
    string(REGEX MATCH "ABWEICHUNG ([0-9]+) GLEICH ([0-9]+)" _m "${_o}")
    if(NOT _m)
        _fehler("[k1]: Werkzeug bssdiff F${_bild}: ${_o}")
    endif()
    set(${_out} "${CMAKE_MATCH_2}" PARENT_SCOPE)
endfunction()
_gleich(640 _g_640)
_gleich(560 _g_560)
message(STATUS "r41_ventilkette_weg[${FIGUR}] k1: gleich BSS 2030 Cut 7: F640 ${_g_640} %, F560 ${_g_560} %")
if(_g_640 LESS 95)
    _fehler("[k1]: F640 ist nicht das Standbild ROOM2030 Cut 7 (nur ${_g_640} % der Pixel gleich, erwartet >= 95)")
endif()
if(_g_560 GREATER 50)
    _fehler("[k1]: Gegenprobe ungueltig - F560 (Raum 2010) gleicht schon dem Standbild (${_g_560} % gleich)")
endif()
message(STATUS "r41_ventilkette_weg[${FIGUR}] k1: Kasten 2010 -> 40 -> 41 -> Ja -> Bit 125 -> Standbild 2030 Cut 7 (${_g_640} % gleich) + Ton -> 42")

# ---- k2: nach der Flut die Sicherung aus dem Ventilkasten holen und zum Generator 2060 - ganzer Weg zu Fuss -----------------
# (Nachbesserung 5, Nachtrag 16 Folgen 3/6). Zustand nach der Flut (_nach) + Bit 125 (die Sicherung steckt). Tueren: 20B0 slot 2
# Door_aot_set @0x019A2 rect (-27600,-3650,1000,2000) -> ROOM2010 (-4600,0,-4950); 2010 slot 0 @0x00746 rect (-5800,-5800,
# 1000,1750) -> ROOM2000 (-26450,-1800,-2700) Cut 14, nach der Flut umgelenkt auf ROOM20B0 Cut 10. Gemessen 2026-10-25
# (Messlauf nb5 k2m, Leon): 43 -> Ja -> Set(9,125)=0 -> 44, zurueck, Leiter, 2050, 2020, 2030 (y -7200), 2060 F1028 msg 4.
_lauf(k2 600 "RAUM 20B${S}
WARTE 30
FLAG 9 125
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
GEHE -26150 -2650 80
BLICK 2048
WARTE 10
AKTION
RAUM 201${S}
WARTE 30
GEHE 4950 -4950 150
GEHE 4950 -2770 100
BLICK 0
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 150
AKTION
WARTE 150
AKTION
WARTE 60
FLAG 9 125
GEHE 4950 -4950 150
GEHE -4300 -4950 120
BLICK 2048
WARTE 10
AKTION
RAUM 20B${S}
WARTE 30
FLAG 9 125
GEHE -26150 10300 80
BLICK 2048
WARTE 10
AKTION
RAUM 205${S}
GEGNERWEG
WARTE 30
GEHE 9100 -6300 150
GEHE -6850 -6300 150
GEHE -6850 2500 150
GEHE -5150 3600 120
BLICK 3072
WARTE 10
AKTION
RAUM 202${S}
GEGNERWEG
WARTE 30
GEHE -1300 -3400 120
GEHE -1300 2300 150
GEHE -13800 2500 200
GEHE -14200 2900 100
BLICK 3072
WARTE 30
AKTION
RAUM 203${S}
WARTE 30
GEHE -6874 -9664
GEHE -6955 -7368
BLICK 2048
WARTE 10
AKTION
RAUM 206${S}
GEGNERWEG
WARTE 30
GEHE 6000 3950 150
GEHE 6000 -6900 150
GEHE -4500 -6900 150
GEHE -4500 3400 150
GEHE -4000 3800 100
BLICK 3072
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 150
FLAG 9 125
ENDE
" RE15_FLAG_CENSUS=1 RE15_DEBUG_JUMP=20B0@250 "RE15_SET_FLAG=${_nach},9:125,7:122,7:123" "RE15_PLAYER_POS=-20620,-9705,2048,1"
      "RE15_EXIT_AT=1100#206${S}")
_darf_nicht("${_wp}" "KLEMME" k2 "der Weg klemmte")
_muss("${_dbg}" "[aot] DOOR FIRE slot=2 rect=(-27100,-2650,hw=500,hh=1000) target_cut=0 spawn=(-4600,0,-4950)" k2 "20B0 Westgang Tuer slot 2 nach 2010")
string(TOLOWER "201${S}" _k2010)
_muss("${_dbg}" "loaded room${_k2010}.rdt" k2 "2010 geladen")
_muss("${_dbg}" "[ventilkette41] ROOM201${S}: Ventil-Sicherungskasten Slot 8 (x 4900..5900 z -3470..-2070), Sicherung steckt (Bit 125)" k2 "Kasten mit Sicherung")
_muss("${_dbg}" "[ventilkette41] ROOM201${S}: Tuer Slot 0 -> ROOM20B0 Cut 10 (Ziel ROOM2000)" k2 "2010-Tuer nach der Flut auf 20B0 umgelenkt")
_danach("${_dbg}" "Ereignis 20 -> Port-Programm 2010_kasten_drin" "[msgtext] room=201${S} id=43 \"The fuse is in place." k2 "Text 43 (ROOM2060 msg 6 + ROOM2030 msg 2)")
_danach("${_dbg}" "room=201${S} id=43" "WRITE Set  zone=9 idx=125 op=0" k2 "Ja -> Set(9,125)=0")
_danach("${_dbg}" "zone=9 idx=125 op=0" "[msgtext] room=201${S} id=44 \"You've taken the Fuse.\"" k2 "Text 44")
_danach("${_dbg}" "room=201${S} id=44" "[ventilkette41] Ventil-Sicherungskasten 2010: Sicherung genommen -> Bit (9,125) = 0" k2 "Bit 125 = 0")
_danach("${_dbg}" "Sicherung genommen" "[aot] DOOR FIRE slot=0 rect=(-5300,-4925,hw=500,hh=875) target_cut=10 spawn=(-26450,-1800,-2700)" k2 "zurueck nach 20B0")
_danach("${_dbg}" "rect=(-5300,-4925,hw=500,hh=875)" "[aot] DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500) target_cut=0 spawn=(9100,-5400,4100)" k2 "Leiter nach 2050")
_danach("${_dbg}" "rect=(-27100,10300,hw=500,hh=500)" "[aot] DOOR FIRE slot=1 rect=(-5150,4450,hw=1000,hh=500) target_cut=0 spawn=(11350,-1800,-3150)" k2 "2050 slot 1 nach 2020")
_danach("${_dbg}" "rect=(-5150,4450,hw=1000,hh=500)" "spawn=(-450,-16200,-9700)" k2 "2020 slot 1 nach 2030 Nordgalerie")
_muss("${_dbg}" "[ventilkette41] ROOM203${S}: unter Wasser (y -7200)" k2 "2030 ohne Strom geflutet")
_danach("${_dbg}" "spawn=(-450,-16200,-9700)" "[aot] DOOR FIRE slot=3 rect=(-7700,-7250,hw=500,hh=900) target_cut=0 spawn=(7600,0,3950)" k2 "2030 slot 3 nach 2060")
string(TOLOWER "206${S}" _k2060k)
_muss("${_dbg}" "loaded room${_k2060k}.rdt" k2 "2060 geladen")
_darf_nicht("${_dbg}" "Generator-Kasten Slot 9 = Text 3" k2 "ohne Bit 125 gilt im Generator-Kasten wieder das Original")
_danach("${_dbg}" "loaded room${_k2060k}.rdt" "[msgtext] room=206${S} id=3 \"One of the fuses is missing...\"" k2 "2060 Kasten: Original sub18 msg 3")
_danach("${_dbg}" "room=206${S} id=3" "[msgtext] room=206${S} id=4 \"Will you use the Fuse?\"" k2 "2060 Kasten: Frage msg 4 (Sicherung einsetzbar)")
_flagfolge("${_wp}" k2 9 125 "1;0;0;0")
message(STATUS "r41_ventilkette_weg[${FIGUR}] k2: nach der Flut zu Fuss 20B0 -> 2010 Kasten (43 -> Ja -> 44, Bit 125 = 0) -> 20B0 -> 2050 -> 2020 -> 2030 -> 2060 -> \"Will you use the Fuse?\"")

# ---- k3: Generator-Kasten 2060, waehrend die Sicherung im Ventilkasten steckt (Bit 125) ---------------------------------------
# Gemessen 2026-10-24 (Messlauf nb5 k3m, Leon): Slot 9 = Text 3 "One of the fuses is missing..." (Else-Zweig @0x010A6), keine
# Frage msg 4, kein Set(3,144). Eingaben wie g1 (A0.1 = Aktion), Spielbild-Achse.
_lauf(k3 180 "" RE15_FLAG_CENSUS=1 RE15_DEBUG_JUMP=2060@250 RE15_SET_FLAG=3:108,9:125,7:122,7:123 "RE15_PLAYER_POS=-4000,3800,3072,0"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W3,A0.1,W3"
      "RE15_EXIT_AT=600#206${S}")
_muss("${_dbg}" "[ventilkette41] ROOM206${S}: Sicherung steckt im Ventilkasten 2010 (Bit 125) -> Generator-Kasten Slot 9 = Text 3 (@0x010A6)" k3 "Kasten-Sperre angelegt")
_muss("${_dbg}" "[msgtext] room=206${S} id=3 \"One of the fuses is missing...\"" k3 "Text 3")
_darf_nicht("${_dbg}" "id=4 \"Will you use the Fuse?\"" k3 "keine Frage ohne Sicherung in der Hand")
_darf_nicht("${_dbg}" "zone=3 idx=144 op=1" k3 "kein Set(3,144)")
message(STATUS "r41_ventilkette_weg[${FIGUR}] k3: 2060 mit Bit 125 -> Generator-Kasten Text 3, keine Frage")

# ---- w1: nach der Flut, ohne Strom: 20B0 -> 2050 -> 2020 -> 2030 Nordgalerie -> 2060 -------------------------------------
_lauf(w1 420 "RAUM 20B${S}
WARTE 30
FLAG 9 103
FLAG 3 126
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
GEHE -26150 10300 80
BLICK 2048
WARTE 10
AKTION
RAUM 205${S}
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
RAUM 202${S}
GEGNERWEG
WARTE 30
GEHE -1300 -3400 120
GEHE -1300 2300 150
GEHE -13800 2500 200
GEHE -14200 2900 100
BLICK 3072
WARTE 30
AKTION
RAUM 203${S}
WARTE 30
FLAG 3 126
GEHE -6874 -9664
GEHE -6955 -7368
BLICK 2048
WARTE 10
AKTION
RAUM 206${S}
WARTE 30
ENDE
" RE15_DEBUG_JUMP=20B0@250 "RE15_SET_FLAG=${_nach}" "RE15_PLAYER_POS=-20620,-9705,2048,1" "RE15_EXIT_AT=60#206${S}")
_muss("${_dbg}" "[aot] DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500) target_cut=0 spawn=(9100,-5400,4100)" w1 "20B0-Leiter nach 2050 (frei, Bit 100)")
_muss("${_dbg}" "[aot] DOOR FIRE slot=1 rect=(-5150,4450,hw=1000,hh=500) target_cut=0 spawn=(11350,-1800,-3150)" w1 "2050 slot 1 nach 2020")
_muss("${_dbg}" "spawn=(-450,-16200,-9700)" w1 "2020 slot 1 nach 2030 Nordgalerie")
_muss("${_dbg}" "[ventilkette41] ROOM203${S}: unter Wasser (y -7200)" w1 "2030 ohne Strom geflutet")
_muss("${_dbg}" "[aot] DOOR FIRE slot=3 rect=(-7700,-7250,hw=500,hh=900) target_cut=0 spawn=(7600,0,3950)" w1 "2030 Nordgalerie slot 3 nach 2060")
string(TOLOWER "206${S}" _k2060)
_muss("${_dbg}" "loaded room${_k2060}.rdt" w1 "2060 geladen")
_flagfolge("${_wp}" w1 3 126 "0;0")
message(STATUS "r41_ventilkette_weg[${FIGUR}] w1: 20B0 -> Kanal -> Leiter -> 2050 -> 2020 -> 2030 Nordgalerie (geflutet) -> 2060")

# ---- w3: nach dem Strom: 2060 -> 2030 Nordgalerie (Pegel knapp unter der Absperrung) -> 2020 -> 2050 -> 20B0 -------------
_lauf(w3 420 "RAUM 206${S}
GEGNERWEG
WARTE 30
FLAG 3 126
GEHE -4500 3400 150
GEHE -4500 -6900 150
GEHE 6000 -6900 150
GEHE 6000 3950 150
GEHE 7600 3950 120
BLICK 0
WARTE 10
AKTION
RAUM 203${S}
WARTE 30
FLAG 3 126
AOTS
GEHE -6874 -9664
GEHE -450 -9700 120
BLICK 1024
WARTE 10
AKTION
RAUM 202${S}
GEGNERWEG
WARTE 30
GEHE -13800 2500 200
GEHE -1300 2300 150
GEHE -1300 -3400 120
GEHE 11400 -3000 120
BLICK 1024
WARTE 10
AKTION
RAUM 205${S}
GEGNERWEG
WARTE 30
GEHE -6850 2500 150
GEHE -6850 -6300 150
GEHE 9100 -6300 150
GEHE 9100 4300 120
BLICK 3072
WARTE 10
AKTION
RAUM 20B${S}
WARTE 30
ENDE
" RE15_DEBUG_JUMP=2060@250 "RE15_SET_FLAG=${_nach},3:144,3:126,7:122,7:123" "RE15_PLAYER_POS=-3200,3400,0,0" "RE15_EXIT_AT=60#20B${S}")
_muss("${_dbg}" "spawn=(-6950,-16200,-7200)" w3 "2060 slot 0 nach 2030 Nordgalerie")
_muss("${_dbg}" "[ventilkette41] ROOM203${S}: Strom (3,126) -> Wasser knapp unter der Absperrung y -5350 (Schachtoeffnung), Treppe Slot 8 -> Sperrtext 48, Ring frei" w3 "2030 mit Strom knapp unter der Absperrung (Nachtraege 7/8/12)")
_darf_nicht("${_dbg}" "unter Wasser (y -7200)" w3 "mit Strom nicht mehr ueber dem Ring geflutet")
_muss("${_dbg}" "pos=(0,-5350,0)" w3 "Wasserflaeche obj 4 auf -5350 (Ringboden -5400 + 50)")
_muss("${_dbg}" "spawn=(-14200,-1800,2450)" w3 "2030 slot 2 nach 2020")
_muss("${_dbg}" "spawn=(-5150,-5400,3450)" w3 "2020 slot 0 nach 2050")
_muss("${_dbg}" "[aot] DOOR FIRE slot=0 rect=(9100,5700,hw=1000,hh=1000) target_cut=9" w3 "2050-Leiter hinab (nach 20B0, Bit 103)")
_muss("${_dbg}" "loaded room${_k20b0}.rdt" w3 "20B0 geladen")
message(STATUS "r41_ventilkette_weg[${FIGUR}] w3: 2060 -> 2030 Nordgalerie (Pegel -5350) -> 2020 -> 2050 -> 20B0")

# ---- w4: nach dem Strom: 20B0 -> Ost-Tuer -> 2030 Ring -> Lift B -> 2080 -> Rechner (Original) -> 2090 ---------------------
_lauf(w4 480 "RAUM 20B${S}
WARTE 30
FLAG 9 103
FLAG 3 126
GEHE -26000 -9629 150
GEHE -25700 -9629 80
BLICK 0
WARTE 10
AKTION
WARTE 90
GEHE -22250 -9629 150
BLICK 0
WARTE 10
AKTION
WARTE 120
GEHE -21000 -6400 150
GEHE 5000 -6400 80
BLICK 0
WARTE 10
AKTION
RAUM 203${S}
WARTE 30
AOTS
GEHE 6550 9300 150
GEHE -7000 9300 150
GEHE -7000 3600 150
GEHE -9800 3600 100
BLICK 2048
WARTE 20
FLAG 3 220
AKTION
WARTE 400
FLAG 3 220
GEHE -7000 3600 150
GEHE -7000 6400 120
BLICK 2048
WARTE 10
AKTION
RAUM 208${S}
GEGNERWEG
WARTE 30
FLAG 3 245
BLICK 0
WARTE 10
AKTION
WARTE 200
GEHE -15500 -16500 150
GEHE -11500 -16500 150
GEHE -11300 -7350 150
BLICK 3072
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 150
AKTION
WARTE 450
FLAG 3 245
GEHE -10541 -8106 150
GEHE -10496 -24008 150
GEHE -9750 -25650 100
BLICK 0
WARTE 10
AKTION
RAUM 209${S}
WARTE 30
ENDE
" RE15_DEBUG_JUMP=20B0@250 "RE15_SET_FLAG=${_nach},3:144,3:126" "RE15_PLAYER_POS=-26450,10250,1024,1" "RE15_EXIT_AT=60#209${S}")
_darf_nicht("${_dbg}" "Ring-Tuer Slot 3 nach 2030 gesperrt" w4 "mit Strom ist die 20B0-Ost-Tuer offen (Nachtrag 7)")
_muss("${_dbg}" "[aot] DOOR FIRE slot=3 rect=(5950,-6400,hw=500,hh=1500) target_cut=0 spawn=(6550,-5400,-100)" w4 "20B0 slot 3 @0x019C2 auf den Ring")
_muss("${_dbg}" "[ventilkette41] ROOM203${S}: Strom (3,126) -> Wasser knapp unter der Absperrung y -5350" w4 "2030 mit Strom, Ring trocken")
_flagfolge("${_wp}" w4 3 220 "0;1")
_muss("${_dbg}" "[aot] DOOR FIRE slot=1 rect=(-7700,6400,hw=500,hh=900) target_cut=0 spawn=(-23200,-9000,-16700)" w4 "Suedgalerie slot 1 @0x01AEA nach 2080")
_darf_nicht("${_dbg}" "Port-Programm 2080" w4 "Rechner 2080 ohne Port-Programm (Nachtrag 7)")
_muss("${_dbg}" "[msgtext] room=208${S} id=0 \"From here maybe I can control the water level." w4 "Original-Frage am Rechner (sub02 @0x00C86)")
_danach("${_dbg}" "From here maybe I can control the water level." "Cut_chg(7)" w4 "Wasser 2080 sinkt (sub02 Cut 7)")
_darf_nicht("${_dbg}" "[msgtext] room=208${S} id=40" w4 "kein Port-Text 2080 Id 40 (entfallen)")
_flagfolge("${_wp}" w4 3 245 "0;1")
_muss("${_dbg}" "[aot] DOOR FIRE slot=1 rect=(-9150,-25650,hw=500,hh=500) target_cut=0" w4 "Tuer slot 1 @0x00BC2 nach 2090 (erst nach dem Rechner)")
string(TOLOWER "209${S}" _k2090)
_muss("${_dbg}" "loaded room${_k2090}.rdt" w4 "2090 geladen")
message(STATUS "r41_ventilkette_weg[${FIGUR}] w4: 20B0 -> Ost-Tuer -> 2030 Ring (trocken) -> Lift B -> Suedgalerie -> 2080 -> Rechner (Original) -> 2090")

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_ventilkette_weg[${FIGUR}]: v1 v2a v2b v2c v2d v2p g1 g2 k1 k2 k3 w1 w3 w4 bestanden")
