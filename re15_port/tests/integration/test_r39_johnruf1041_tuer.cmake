cmake_minimum_required(VERSION 3.16)
# =============================================================================
# RUNDE 39 SPUR P "johnruf1041", NACHBESSERUNG 1 (Nutzer-Nachtrag 12) MIT DER ECHTEN EXE, Elza, Spielstand + CONTINUE,
# echte Tueren (RE15_WEGPLAN). Dossier analysis/befunde_runde39/P_johnruf1041.md Abschnitt 8, Konstanten
# include/re15_tuersperre_r39.h. Die Unit-Riegel unit_r39_johnruf1041_tuer* pruefen die Mechanik am Engine-Code.
#   TEIL t1041   ROOM1041 in der Panikphase (Bank 9 Bit 97): Tuer -> ROOM1031 zeigt "I have to rescue John! He is in
#                danger!" (msg 41), Stimme main41.wav nach der Ordnerregel, KEIN Raumwechsel
#   TEIL t1041o  ROOM1041 ohne Panikphase (nach Johns Rettung: Bit 97 = 0, (4,0xF0) = 0): Tuer -> ROOM1031 wechselt den Raum
#   TEIL t1051   ROOM1051 in der Panikphase: Tuer -> ROOM10A1 zeigt den Satz (msg 40), KEIN Raumwechsel
#   TEIL t1051o  ROOM1051 ohne Panikphase: Tuer -> ROOM10A1 wechselt den Raum
#   TEIL t11b1   ROOM11B1 mit Strom (4,0xF3) (Nachtrag 12c, Nutzer-Korrektur "11B1, nicht 10B1"): Parkplatz-Tuer -> ROOM11C1
#                zeigt "The door is locked." (Original-msg 12), KEIN Raumwechsel
#   TEIL t11b1d  ROOM11B1 ohne Strom: Original-Text msg 14 ("It's too dark..."), keine Port-Sperre
#   TEIL t10b1v  ROOM10B1 vor der Sherry-Szene (ueber die echte Tuer 1261 -> 10B1 Cut 7): Kanaldeckel -> Text msg 40
#   TEIL t10b1s  ROOM10B1 mit (3,0x82) (Szene "John, wait!" steht an): Szene msg 0..5, am Ende Deckel-Text statt ROOM1261,
#                nach dem Text Pad frei, Balken weg, Elza laeuft
#   TEIL t10b1n  ROOM10B1 nach der Szene / Sherry-Suche ((3,0x83) (3,0x41) (3,0x46)): Kanaldeckel -> Text, kein Raumwechsel
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_ELZA=<probe_r36_elza_karte> -DWORKDIR=<dir> -DTEIL=<teil>
#               -P test_r39_johnruf1041_tuer.cmake
# =============================================================================
foreach(_v RE15_PC_EXE RE15_KARTE_ELZA WORKDIR TEIL)
    if(NOT ${_v})
        message(FATAL_ERROR "r39_johnruf1041_tuer: ${_v} fehlt")
    endif()
endforeach()
foreach(_v RE15_PC_EXE RE15_KARTE_ELZA)
    if(NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r39_johnruf1041_tuer: ${_v} existiert nicht: '${${_v}}'")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Keine RE15_*-Variablen des Aufrufers erben (VERTRAG §2; Muster test_r39_johnruf1041.cmake).
execute_process(COMMAND "${CMAKE_COMMAND}" -E environment OUTPUT_VARIABLE _envdump)
string(REGEX MATCHALL "(^|\n)RE15_[A-Za-z0-9_]*=" _geerbt "${_envdump}")
foreach(_g ${_geerbt})
    string(REGEX REPLACE "^\n?(RE15_[A-Za-z0-9_]*)=$" "\\1" _n "${_g}")
    unset(ENV{${_n}})
endforeach()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_lauf "${WORKDIR}/lauf_${TEIL}_${_lauf_id}")
if(AUSWERTEN)
    set(_lauf "${AUSWERTEN}")
endif()

# Eigene exe-Kopie mit Praefix JE TEST (und Teil) neben der exe (Asset-Wurzel = exe-Verzeichnis).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe_kopie "${_exe_dir}/re15_pc_r39pt_${TEIL}_${_lauf_id}${_exe_ext}")

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

set(_satz "I have to rescue John! He is in danger!")
set(_deckel " The lid is firmly closed.It's impossible for me to open it.")
set(_basis "3:0x5F;4:0xF1;4:0xEC;3:0x78")       # Elza nach Roys Tagebuch (wie integration_r39_johnruf1041_a)
set(_extra "")
set(_plan_tuer "WARTE 30
GEGNERWEG
FLAG 9 97
")
# Tuer 1041 -> 1031: Rechteck x -20400 B 1500 / z -4900 T 2200 (ROOM1041 main00 @0x010B8); Vorwaertspunkt 620 vor Elza
set(_plan_1041 "GEHE -20800 -3800 120
BLICK 0
AKTION
WARTE 150
AKTION
WARTE 60
ENDE
")
# Tuer 1051 -> 10A1: Rechteck x 16700 B 1000 / z -14700 T 2000 (ROOM1051 main00 @0x00B5A), am Rollo vorbei
set(_plan_1051 "GEHE 15000 -12500 150
GEHE 16450 -13700 60
BLICK 0
AKTION
WARTE 150
AKTION
WARTE 60
ENDE
")
# Kanaldeckel 10B1: Rechteck x -22120 B 1500 / z 3560 T 2100 (ROOM10B1 main00 @0x016F2); Ankunft aus 1261 (-21159,3289)
set(_plan_10b1 "RAUM 10B1
WARTE 30
GEGNERWEG
BLICK 3072
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 60
ENDE
")
# Parkplatz-Tuer 11B1 -> 11C1: Rechteck x -26730 B 2500 / z -29940 T 3500 (ROOM11B1 main00 @0x00F9C); Lage und Blick wie
# integration_r39_parkplatz11c0_e (dort fuehrte derselbe Druck vor Nachtrag 12c nach ROOM11C1)
set(_plan_11b1 "RAUM 11B1
WARTE 30
AKTION
WARTE 150
AKTION
WARTE 60
ENDE
")
if(TEIL STREQUAL "t1041")
    set(_karte "1041;-25150;-3800;0;${_basis};4:0xF0;4:5;4:4;9:97")
    set(_plan "RAUM 1041\n${_plan_tuer}${_plan_1041}")
    set(_ende "RE15_EXIT_AT=480#1041")
elseif(TEIL STREQUAL "t1041o")
    set(_karte "1041;-25150;-3800;0;${_basis};4:5;4:4")
    set(_plan "RAUM 1041\n${_plan_tuer}${_plan_1041}")
    set(_ende "RE15_EXIT_AT=300")   # nach 1031 folgt mit diesen Flags die Auto-Tuer Slot 18 -> 1241
elseif(TEIL STREQUAL "t1051")
    set(_karte "1051;15000;-10000;3072;${_basis};4:0xF0;3:0x79;9:97")
    set(_plan "RAUM 1051\n${_plan_tuer}${_plan_1051}")
    set(_ende "RE15_EXIT_AT=540#1051")
elseif(TEIL STREQUAL "t1051o")
    set(_karte "1051;15000;-10000;3072;${_basis};3:0x79")
    set(_plan "RAUM 1051\n${_plan_tuer}${_plan_1051}")
    set(_ende "RE15_EXIT_AT=120#10a1")
elseif(TEIL STREQUAL "t11b1")
    set(_karte "11B1;-25480;-26000;1024;${_basis};4:0xF3")
    set(_plan "${_plan_11b1}")
    set(_ende "RE15_EXIT_AT=480#11b1")
elseif(TEIL STREQUAL "t11b1d")
    set(_karte "11B1;-25480;-26000;1024;${_basis}")
    set(_plan "${_plan_11b1}")
    set(_ende "RE15_EXIT_AT=480#11b1")
elseif(TEIL STREQUAL "t10b1v")
    set(_karte "1261;-20250;-6700;2048;${_basis};4:0xF0")
    set(_plan "${_plan_10b1}")
    set(_ende "RE15_EXIT_AT=420#10b1")
    set(_extra "RE15_FIRE_AOT=2@40#1261")   # echte Tuer ROOM1261 main00 @0x0116A Slot 2 -> 10B1 Cut 7
elseif(TEIL STREQUAL "t10b1n")
    set(_karte "1261;-20250;-6700;2048;${_basis};4:0xF0;3:0x83;3:0x41;3:0x46")
    set(_plan "${_plan_10b1}")
    set(_ende "RE15_EXIT_AT=420#10b1")
    set(_extra "RE15_FIRE_AOT=2@40#1261")
elseif(TEIL STREQUAL "t10b1s")
    set(_karte "1261;-20250;-6700;2048;${_basis};4:0xF0;3:0x82")
    # RAUM wartet, bis die Szene vorbei und der Text zu ist (player_mode != 2); dann laeuft Elza Richtung Deckel.
    set(_plan "RAUM 10B1
WARTE 30
FLAG 2 7
FLAG 1 27
GEHE -21000 3000 100
WARTE 30
ENDE
")
    set(_ende "RE15_EXIT_AT=1500#10b1")
    # Text-Druecke im Raster 30 ab Bild 760 (der Deckel-Text oeffnet im Lauf n6b in Bild 733; zwei Seiten)
    set(_pr "")
    foreach(_f RANGE 760 940 30)
        list(APPEND _pr "square@${_f}")
    endforeach()
    string(REPLACE ";" "," _pr "${_pr}")
    set(_extra "RE15_FIRE_AOT=2@40#1261" "RE15_PRESS=${_pr}")
else()
    message(FATAL_ERROR "r39_johnruf1041_tuer: TEIL '${TEIL}' unbekannt")
endif()

if(NOT AUSWERTEN)
    file(MAKE_DIRECTORY "${_lauf}")
    file(WRITE "${_lauf}/wegplan.txt" "${_plan}")
    string(REPLACE "," ";" _kargs "${_karte}")
    execute_process(COMMAND "${RE15_KARTE_ELZA}" "${_lauf}/re15_card.mcr" ${_kargs}
                    RESULT_VARIABLE _rvk OUTPUT_QUIET ERROR_QUIET)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${_lauf}/re15_card.mcr")
        message(FATAL_ERROR "r39_johnruf1041_tuer[${TEIL}]: Spielstand nicht geschrieben (${_rvk})")
    endif()
    file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
    set(WORKDIR "${_lauf}")
    re15_start_spiel(_rv 1800
        RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
        RE15_MSG_LOG=1 RE15_STATE_LOG=state.txt
        "RE15_WEGPLAN=${_lauf}/wegplan.txt" RE15_WEGPLAN_LOG=wegplan_log.txt
        "${_ende}" ${_extra}
        "${_exe_kopie}")
    file(REMOVE "${_exe_kopie}")
    message(STATUS "r39_johnruf1041_tuer[${TEIL}]: exit=${_rv}, ${_lauf}")
endif()

if(NOT EXISTS "${_lauf}/debug.log")
    message(FATAL_ERROR "r39_johnruf1041_tuer[${TEIL}]: kein debug.log (${_lauf})")
endif()
file(READ "${_lauf}/debug.log" _d)
string(FIND "${_d}" "EXIT_AT: Bild" _p)
if(_p LESS 0)
    message(FATAL_ERROR "r39_johnruf1041_tuer[${TEIL}]: Endbild nicht erreicht - Lauf vorher abgerissen (${_lauf})")
endif()
pruef(_d "CONTINUE: resumed in room")
set(_wl "")
if(EXISTS "${_lauf}/wegplan_log.txt")
    file(READ "${_lauf}/wegplan_log.txt" _wl)
endif()

if(TEIL STREQUAL "t1041")
    pruef(_d "ROOM1041: Panikphase (Bit 97) -> Tuer Slot 1 -> Text msg 41")
    pruef(_d "[msgtext] room=1041 id=41 \"${_satz}\"")
    # Nachtrag 13: einfacher Raumtext ohne Stimme (wie Leons Tuertexte in der Panikphase)
    pruef_nicht(_d "[voice] ROOM1041 msg 41:")
    pruef_nicht(_d "clip loaded: main41.wav")
    pruef_nicht(_d "[aot] DOOR FIRE slot=1")
    pruef_nicht(_d "loaded room1031.rdt")
    pruef(_wl "Befehl 9 beendet")                            # zweite AKTION (Text weg) + WARTE 60: Elza wieder frei
elseif(TEIL STREQUAL "t1041o")
    pruef_nicht(_d "[tuersperre]")
    pruef(_d "[aot] DOOR FIRE slot=1 rect=(-19650,-3800,hw=750,hh=1100) target_cut=8")
    pruef(_d "loaded room1031.rdt")
    pruef_nicht(_d "id=41 \"${_satz}\"")
elseif(TEIL STREQUAL "t1051")
    pruef(_d "ROOM1051: Panikphase (Bit 97) -> Tuer Slot 4 -> Text msg 40")
    pruef(_d "[msgtext] room=1051 id=40 \"${_satz}\"")
    pruef_nicht(_d "[voice] ROOM1051 msg 40:")              # Nachtrag 13: ohne Stimme
    pruef_nicht(_d "clip loaded: main40.wav")
    pruef_nicht(_d "[aot] DOOR FIRE slot=4")
    pruef_nicht(_d "loaded room10a1.rdt")
    pruef(_wl "Befehl 10 beendet")
elseif(TEIL STREQUAL "t1051o")
    pruef_nicht(_d "[tuersperre]")
    pruef(_d "[aot] DOOR FIRE slot=4 rect=(17200,-13700,hw=500,hh=1000) target_cut=0")
    pruef(_d "loaded room10a1.rdt")
elseif(TEIL STREQUAL "t11b1")
    pruef(_d "ROOM11B1: Slot 1 -> Text msg 12 (immer zu)")
    pruef(_d "[msgtext] room=11b1 id=12 ")
    pruef(_d "The door is locked.")
    pruef_nicht(_d "[aot] DOOR FIRE slot=1")
    pruef_nicht(_d "loaded room11c1.rdt")
    pruef(_wl "Befehl 5 beendet")                            # zweite AKTION (Text weg) + WARTE 60: Elza wieder frei
elseif(TEIL STREQUAL "t11b1d")
    pruef_nicht(_d "[tuersperre]")
    pruef(_d "[msgtext] room=11b1 id=14 ")
    pruef_nicht(_d "[aot] DOOR FIRE slot=1")
    pruef_nicht(_d "loaded room11c1.rdt")
elseif(TEIL STREQUAL "t10b1v" OR TEIL STREQUAL "t10b1n")
    pruef(_d "loaded room10b1.rdt")
    pruef(_d "ROOM10B1: Slot 1 -> Text msg 40 (immer zu)")
    pruef(_d "[msgtext] room=10b1 id=40 \"${_deckel}\"")
    pruef_nicht(_d "[aot] DOOR FIRE slot=1")
    pruef_nicht(_d "loaded room1261.rdt")                     # nur der Start liegt in 1261 (CONTINUE), nie ein Rueckweg
    pruef_nicht(_d "[voice] ROOM10B1 msg 40")
elseif(TEIL STREQUAL "t10b1s")
    pruef(_d "ROOM10B1 sub01 @0x01824 Evt_exec sub04 -> Programm SZENE10B1")
    pruef(_d "[msgtext] room=10b1 id=0 \"Elza: John, wait!\"")
    pruef(_d "[msgtext] room=10b1 id=5 \"John: If that's the case, we can go through the sewer tunnels. Follow me.\"")
    string(FIND "${_d}" "[msgtext] room=10b1 id=5 " _p5)
    string(FIND "${_d}" "[msgtext] room=10b1 id=40 \"${_deckel}\"" _p40)
    set(_folge FALSE)
    if(_p5 GREATER -1 AND _p40 GREATER _p5)
        set(_folge TRUE)
    endif()
    pruef_wahr(_folge "Deckel-Text NACH \"Follow me.\" (sub04 @0x01A8E Aot_on Slot 1 -> MESSAGE)")
    pruef_nicht(_d "[aot] DOOR FIRE slot=1")
    pruef_nicht(_d "loaded room1261.rdt")
    pruef(_wl "FLAG (2,7) = 0")
    pruef(_wl "FLAG (1,27) = 0")
    # Elza laeuft nach dem Text: GEHE begonnen, letzte "beendet"-Zeile mit pm=0 an einer anderen Lage als (-20303,1579)
    # (Szenen-Standplatz sub04 @0x0198A Pos_set)
    pruef(_wl "GEHE (-21000,3000)")
    string(REGEX MATCHALL "[^\n]*beendet[^\n]*" _bl "${_wl}")
    list(LENGTH _bl _nbl)
    set(_lief FALSE)
    if(_nbl GREATER 0)
        list(GET _bl -1 _zl)
        message(STATUS "     letzte Zeile: ${_zl}")
        if(_zl MATCHES "pm=0" AND NOT _zl MATCHES "spieler=\\(-20303,1579\\)")
            set(_lief TRUE)
        endif()
    endif()
    pruef_wahr(_lief "nach dem Text Steuerung zurueck: Elza laeuft (pm=0, Lage nicht mehr der Szenen-Standplatz)")
endif()

list(LENGTH _fehler _nf)
if(_nf GREATER 0)
    message(FATAL_ERROR "r39_johnruf1041_tuer[${TEIL}]: ${_nf} Fehler: ${_fehler} (${_lauf})")
endif()
message(STATUS "r39_johnruf1041_tuer[${TEIL}]: alles gruen (${_lauf})")
