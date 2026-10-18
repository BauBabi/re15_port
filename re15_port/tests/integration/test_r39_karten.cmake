cmake_minimum_required(VERSION 3.16)
# =============================================================================
# KARTEN DER POLIZEISTATION MIT DER ECHTEN EXE (Runde 39, Spur G "karten").
#
# Dossier analysis/befunde_runde39/G_karten.md. Nutzer (AUFTRAG.md):
#   1. "Bei den Maps des Police Departement passt es noch nicht so ganz. [...] map3F_correct.png, map2F_correct.png,
#      map1F_correct.png, mapB2_correct.png. Dort ist immer links, wie es bei uns aussieht und rechts wie es eigentlich
#      aussehen sollte. Keine Doppelwaende, Tuer immer genau auf Wand platziert, treppen nicht in der Wand platziert usw.
#      [...] Die Karte von B1 ist noch ziemlicher schrott."
#   2. "In ROOM 1090 ist Leons Positionsmarker der Map ausserhalb des aktuellen Raums."
#
# Lauf S (Sweep): Spielstand in ROOM1190, LOAD GAME, Kartenschirm, RE15_MAP_SHOT_SWEEP zieht alle 13 Blaetter ab.
#   - Blaetter 1..4 (B2/1F/2F/3F): Klassenbild gegen die SOLL-Haelfte der Nutzer-Vorlage (probe_r39_karten sollbild,
#     r39_karten_soll.inc) - 0 Abweichungen;
#   - Blaetter 0..4: keine Doppelwand (Koerper|Wand|Wand|Koerper) - probe_r39_karten doppelwand.
# Lauf M (1090): Spielstand am Punkt des Nutzers (befund.log F1126..F1351: R1090 (-12528,-9000,959), obere Ebene),
#   Kartenschirm (Blatt 2F): der Marker-Ring steht im ROT des aktuellen Raums (7x7 um die Ring-Mitte: rot >= 8, kein
#   Gruen eines fremden Raums). Vorher: Ring (186,83) im gruenen Nachbarraum (linke Haelfte von map2F_correct.png).
# Lauf K (Nachbesserung 2, Abnahme 1 N1): der Weg der Abnahme - Leon laeuft aus dem ROOM1210-Flur durch die Oeffnung in
#   die Suedwest-Kammer; der Ring steht in ihrer Zeichnung (x187..200 y113..128), ringsum ROT, Kammer geschlossen, keine
#   Doppelwand. Vorher: Ring (191,117) im Blau links der Flurwand x201. Blatt 1 im Sweep: die Kammer (x186..201 y113..129)
#   wird statt gegen die Vorlage gegen ihre Zeichnung aus der SCA verglichen (probe_r39_karten sollbild, kammer_soll).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PROBE=<probe_r39_karten> -DRE15_PROBE36=<probe_r36_karte>
#               -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
foreach(_v RE15_PC_EXE RE15_PROBE RE15_PROBE36)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r39_karten: ${_v} fehlt: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "r39_karten: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis); Praefix nur DIESES Tests. Endung wie die Quelle.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
set(_exe_kopie "${_exe_dir}/re15_pc_r39g_karten_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(_basis_wd "${WORKDIR}/lauf_${_lauf_id}")
set(_fehler "")

function(karten_lauf _name _oeffnen_raum _karte_args)
    set(WORKDIR "${_basis_wd}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    execute_process(COMMAND "${RE15_PROBE36}" karte "re15_card.mcr" ${_karte_args}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r39_karten[${_name}]: Spielstand nicht geschrieben (exit=${_rvk})\n${_outk}")
    endif()
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        "RE15_INV_OPEN_AT=40#${_oeffnen_raum}"
        ${ARGN}
        "${_exe_kopie}")
    set(_rv_${_name} ${_rv} PARENT_SCOPE)
endfunction()

# ---- S: Sweep aller Blaetter aus ROOM1190 ----------------------------------------------------------------------
karten_lauf(S 1190 "1190;3174;0;-11182;0" RE15_MAP_SHOT_SWEEP=s)
foreach(_s 00 01 02 03 04)
    if(NOT EXISTS "${_basis_wd}_S/s${_s}.bmp")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r39_karten[S]: kein Abzug s${_s}.bmp (exit=${_rv_S}, ${_basis_wd}_S)")
    endif()
endforeach()
foreach(_s 1 2 3 4)
    execute_process(COMMAND "${RE15_PROBE}" sollbild "${_basis_wd}_S/s0${_s}.bmp" ${_s}
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _out)
    string(STRIP "${_out}" _out)
    message(STATUS "r39_karten[S] Blatt ${_s}: ${_out}")
    if(NOT _rv EQUAL 0)
        set(_fehler "${_fehler} soll-${_s}")
    endif()
endforeach()
foreach(_s 0 1 2 3 4)
    execute_process(COMMAND "${RE15_PROBE}" doppelwand "${_basis_wd}_S/s0${_s}.bmp"
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _out)
    string(STRIP "${_out}" _out)
    message(STATUS "r39_karten[S] Blatt ${_s}: ${_out}")
    if(NOT _rv EQUAL 0)
        set(_fehler "${_fehler} doppelwand-${_s}")
    endif()
endforeach()

# ---- M: ROOM1090 obere Ebene am Punkt des Nutzers ----------------------------------------------------------------
karten_lauf(M 1090 "1090;-12528;-9000;959;0;besucht:1090:-12528:959:5"
            RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=60 "RE15_EXIT_AT=200#1090")
if(NOT EXISTS "${_basis_wd}_M/karte.bmp")
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r39_karten[M]: kein Kartenabzug (exit=${_rv_M}, ${_basis_wd}_M)")
endif()
execute_process(COMMAND "${RE15_PROBE36}" marker "${_basis_wd}_M/karte.bmp" 150 50 250 130
                RESULT_VARIABLE _rvm OUTPUT_VARIABLE _outm)
string(STRIP "${_outm}" _outm)
message(STATUS "r39_karten[M] ${_outm}")
if(_rvm EQUAL 0 AND _outm MATCHES "^MARKER ([0-9]+) ([0-9]+)")
    set(_mx ${CMAKE_MATCH_1})
    set(_my ${CMAKE_MATCH_2})
    math(EXPR _x0 "${_mx} - 3")
    math(EXPR _y0 "${_my} - 3")
    math(EXPR _x1 "${_mx} + 3")
    math(EXPR _y1 "${_my} + 3")
    execute_process(COMMAND "${RE15_PROBE36}" farben "${_basis_wd}_M/karte.bmp" ${_x0} ${_y0} ${_x1} ${_y1}
                    RESULT_VARIABLE _rvf OUTPUT_VARIABLE _outf)
    string(STRIP "${_outf}" _outf)
    message(STATUS "r39_karten[M] um den Ring: ${_outf}")
    if(_outf MATCHES "rot=([0-9]+) besucht=([0-9]+)")
        if(CMAKE_MATCH_1 LESS 8 OR NOT CMAKE_MATCH_2 EQUAL 0)
            set(_fehler "${_fehler} 1090-marker-ausserhalb")
        endif()
    else()
        set(_fehler "${_fehler} 1090-farben")
    endif()
else()
    set(_fehler "${_fehler} 1090-kein-marker")
endif()

# ---- T: Teilzustaende (Nachbesserung 1, Abnahme-Mangel M3) ------------------------------------------------------
# Je Ort ALLEIN besucht und aktuell (kein Nachbar gezeichnet, kein Etagenplan): die Doppelwand-Korrektur darf die eigene
# Wandlinie nur loeschen, wenn der Nachbar mit der Ersatzlinie gezeichnet wird. Vorher (Stand 40328436, Abnahme 0):
# ROOM1000 Nord-WC ohne Westwand x207 + Unterkante y138, 1070 ohne Westwand x196, 1010 Zone 0 ohne beide Seitenwaende.
foreach(_t "TN;1000;650;50" "TW;1070;160;6145" "TZ;1010;3650;-3950")
    list(GET _t 0 _tn)
    list(GET _t 1 _tr)
    list(GET _t 2 _tx)
    list(GET _t 3 _tz)
    karten_lauf(${_tn} ${_tr} "${_tr};${_tx};0;${_tz};0;besucht:${_tr}:${_tx}:${_tz}"
                RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=60 "RE15_EXIT_AT=200#${_tr}")
    if(NOT EXISTS "${_basis_wd}_${_tn}/karte.bmp")
        set(_fehler "${_fehler} teil-${_tn}-kein-abzug")
        continue()
    endif()
    execute_process(COMMAND "${RE15_PROBE}" offen "${_basis_wd}_${_tn}/karte.bmp"
                    RESULT_VARIABLE _rvo OUTPUT_VARIABLE _outo)
    string(STRIP "${_outo}" _outo)
    message(STATUS "r39_karten[${_tn}] ROOM${_tr} allein: ${_outo}")
    if(NOT _rvo EQUAL 0)
        set(_fehler "${_fehler} teil-${_tn}-offen")
    endif()
endforeach()

# ---- K: ROOM1210 Suedwest-Kammer (Nachbesserung 2, Abnahme 1 Mangel N1) = der Weg der Abnahme ------------------
# Spielstand im Flur (-19500,0,-21700), Blick West, ROOM11E0 + ROOM1210 besucht; LOAD GAME; Leon laeuft durch die Oeffnung
# (Flur-Westwand @0x01BC8 endet z -20746, Kammer-Ostwand @0x01C40 beginnt z -22520) in die Kammer; Kartenschirm bei Bild
# 200. VORHER (Abnahme 1): debug.log pos=(-25500,0,-21700), Ring (191,117) im Blau links der Flurwand x201.
# Soll: Ring im Innenraum der Kammer-Zeichnung x187..200 y113..128, ringsum ROT (aktuell), keine offene Raumseite.
karten_lauf(K 1210 "1210;-19500;0;-21700;2048;besucht:11E0:0:0;besucht:1210:-19500:-21700"
            RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT=W1,U4,W5 "RE15_INV_OPEN_AT=200#1210"
            RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=60 "RE15_EXIT_AT=400#1210")
if(NOT EXISTS "${_basis_wd}_K/karte.bmp")
    set(_fehler "${_fehler} kammer-kein-abzug")
else()
    set(_kx "")
    if(EXISTS "${_basis_wd}_K/debug.log")
        file(STRINGS "${_basis_wd}_K/debug.log" _kpos REGEX "pl pos=\\(-?[0-9]+,-?[0-9]+,-?[0-9]+\\)")
        if(_kpos)
            list(GET _kpos -1 _kletzt)
            if(_kletzt MATCHES "pl pos=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\)")
                set(_kx ${CMAKE_MATCH_1})
                set(_kz ${CMAKE_MATCH_3})
            endif()
        endif()
    endif()
    message(STATUS "r39_karten[K] letzte Spielerlage: x=${_kx} z=${_kz}")
    if("${_kx}" STREQUAL "" OR NOT _kx LESS -22200)
        set(_fehler "${_fehler} kammer-nicht-erreicht")
    endif()
    execute_process(COMMAND "${RE15_PROBE36}" marker "${_basis_wd}_K/karte.bmp" 175 60 235 140
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    string(STRIP "${_outk}" _outk)
    message(STATUS "r39_karten[K] ${_outk}")
    if(_rvk EQUAL 0 AND _outk MATCHES "^MARKER ([0-9]+) ([0-9]+)")
        set(_mx ${CMAKE_MATCH_1})
        set(_my ${CMAKE_MATCH_2})
        if(_mx LESS 187 OR _mx GREATER 200 OR _my LESS 113 OR _my GREATER 128)
            set(_fehler "${_fehler} kammer-ring-ausserhalb(${_mx},${_my})")
        endif()
        math(EXPR _x0 "${_mx} - 3")
        math(EXPR _y0 "${_my} - 3")
        math(EXPR _x1 "${_mx} + 3")
        math(EXPR _y1 "${_my} + 3")
        execute_process(COMMAND "${RE15_PROBE36}" farben "${_basis_wd}_K/karte.bmp" ${_x0} ${_y0} ${_x1} ${_y1}
                        RESULT_VARIABLE _rvf OUTPUT_VARIABLE _outf)
        string(STRIP "${_outf}" _outf)
        message(STATUS "r39_karten[K] um den Ring: ${_outf}")
        if(_outf MATCHES "rot=([0-9]+) besucht=([0-9]+)")
            if(CMAKE_MATCH_1 LESS 8 OR NOT CMAKE_MATCH_2 EQUAL 0)
                set(_fehler "${_fehler} kammer-ring-nicht-im-rot")
            endif()
        else()
            set(_fehler "${_fehler} kammer-farben")
        endif()
    else()
        set(_fehler "${_fehler} kammer-kein-marker")
    endif()
    # offen: Kasten um Kammer + Durchgang (die vorbestehenden Kerben (187,71)/(187,77) am Tuerbalken 11E0<->1210 = Hinweis
    # der Abnahme 1, nicht Teil von N1); doppelwand: ganzes Blatt.
    foreach(_pr "offen;184;110;203;131" "doppelwand")
        list(GET _pr 0 _prn)
        list(SUBLIST _pr 1 -1 _pra)
        execute_process(COMMAND "${RE15_PROBE}" ${_prn} "${_basis_wd}_K/karte.bmp" ${_pra}
                        RESULT_VARIABLE _rvp OUTPUT_VARIABLE _outp)
        string(STRIP "${_outp}" _outp)
        message(STATUS "r39_karten[K] ${_prn}: ${_outp}")
        if(NOT _rvp EQUAL 0)
            set(_fehler "${_fehler} kammer-${_prn}")
        endif()
    endforeach()
endif()

file(REMOVE "${_exe_kopie}")
if(NOT "${_fehler}" STREQUAL "")
    message(FATAL_ERROR "r39_karten: ROT bei:${_fehler}")
endif()
message(STATUS "r39_karten: OK")
