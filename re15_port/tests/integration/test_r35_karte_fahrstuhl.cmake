# Runde 35 Spur G "karte", Nachbesserung 1 (Abnahme 0, M1/M2) — RIEGEL auf die BEWEGUNG des
# Spieler-Markers in der Fahrstuhlkabine ROOM1080, am ECHTEN Weg mit der echten exe.
# Nutzer 2026-10-03: "Beim Elevator ROOM 1080 bewegt sich auf der Map der Player Cursor nicht."
#
# Je Lauf: Spielstand (Slot 0) vor der Fahrstuhltuer des Etagenraums, Titel -> LOAD GAME,
# Aktionstaste an der Tuer (Tuer-AOT ROOM1040 @0x1096 bzw. ROOM1120 @0xCB6) -> Tuersequenz ->
# ROOM1080, dort LAUFEN (Eingabeskript), dann Statusschirm -> MAP, Abzug des Framebuffers,
# Marker im Kabinenkasten suchen (probe_r35_karte marker). Lagen wie Abnahme 0 (lauf.sh F1_*).
#   1F aus ROOM1040: steh / vor / rechts / links    -> Blatt 2, gemalter Innenraum x110..117 y135..142
#   3F aus ROOM1120: rechts / links                 -> Blatt 4, gemalter Innenraum x128..135 y138..145
# Soll: Marker-Mitte in JEDER Lage im gemalten Innenraum, ueber die Lagen x-Spanne >= 6 px und
# y-Spanne >= 6 px (1F), 3F x-Spanne >= 6 px; Richtung 180 Grad (Welt-West = Karte-Ost).
# ROT am Stand 154a73c1 (x 113..118 / y 144..146) und am Stand 8fee1bb4 (x 113..115 / y 138..140)
# - beide gemessen, G_karte.md Nachbesserung 1.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PROBE=<probe_r35_karte> -DWORKDIR=<dir>
#               -P test_r35_karte_fahrstuhl.cmake
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r35_fahrstuhl: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_PROBE OR NOT EXISTS "${RE15_PROBE}")
    message(FATAL_ERROR "r35_fahrstuhl: RE15_PROBE fehlt: '${RE15_PROBE}'")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r35g_fahrstuhl_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r35g_fahrstuhl_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_fehler "")
# fahrt(<name> <kartenargs> <skript> <fenster x0;y0;x1;y1>) -> setzt _mx_<name>, _my_<name>
function(fahrt _name _kartenargs _skript _fenster)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log" "${WORKDIR}/karte.bmp")
    execute_process(COMMAND "${RE15_PROBE}" karte "re15_card.mcr" ${_kartenargs}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r35_fahrstuhl[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    # Karte 230 Bilder nach dem Eintritt in 1080 (das Laufen ist dann laengst vorbei),
    # Abzug im 60. Kartenbild, Ende 420 Bilder nach dem Eintritt.
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=${_skript}"
        "RE15_INV_OPEN_AT=230#1080"
        RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=60
        "RE15_EXIT_AT=420#1080"
        "${_exe_kopie}")
    set(_lage "?")
    if(EXISTS "${WORKDIR}/debug.log")
        file(STRINGS "${WORKDIR}/debug.log" _walk REGEX "pl pos=\\(")
        list(LENGTH _walk _nw)
        if(_nw GREATER 0)
            list(GET _walk -1 _lz)
            string(REGEX MATCH "pl pos=\\([-0-9]+,[-0-9]+,[-0-9]+\\)" _lage "${_lz}")
        endif()
        file(STRINGS "${WORKDIR}/debug.log" _geladen REGEX "PC loaded room1080")
        if(NOT _geladen)
            message(FATAL_ERROR "r35_fahrstuhl[${_name}]: ROOM1080 nie geladen (Tuerweg) - ${WORKDIR}")
        endif()
    endif()
    if(NOT EXISTS "${WORKDIR}/karte.bmp")
        message(FATAL_ERROR "r35_fahrstuhl[${_name}]: kein Kartenabzug (exit=${_rv}, ${WORKDIR})")
    endif()
    string(REPLACE ";" " " _f "${_fenster}")
    separate_arguments(_fa UNIX_COMMAND "${_f}")
    execute_process(COMMAND "${RE15_PROBE}" marker "${WORKDIR}/karte.bmp" ${_fa}
                    RESULT_VARIABLE _rvm OUTPUT_VARIABLE _outm)
    string(STRIP "${_outm}" _outm)
    message(STATUS "r35_fahrstuhl[${_name}]: ${_lage} -> ${_outm}")
    if(NOT _rvm EQUAL 0 OR NOT _outm MATCHES "^MARKER ([0-9]+) ([0-9]+)")
        message(FATAL_ERROR "r35_fahrstuhl[${_name}]: kein Marker im Abzug (${WORKDIR})")
    endif()
    set(_mx_${_name} ${CMAKE_MATCH_1} PARENT_SCOPE)
    set(_my_${_name} ${CMAKE_MATCH_2} PARENT_SCOPE)
endfunction()

# Spielstaende vor der Fahrstuhltuer (Tuer-Datensaetze ROOM1040 @0x1096 r(-22936,-11000,2300,1400),
# ROOM1120 @0xCB6 r(300,5900,2000,1000)); flag:4:243 = Reservestrom an (ROOM11F0 sub18 @0x016F6
# `22 04 f3 01`), ohne Strom faehrt der Fahrstuhl nicht.
set(_k1 "1040;-21786;-10500;3072;besucht:1040:-21786:-10500;flag:4:243")
set(_k3 "1120;1300;6400;1024;besucht:1120:1300:7300;flag:4:243")
set(_tuer "W1,A0.3,W1,A0.3")
set(_i1 "102;127;126;151")   # Suchfenster um die 1F-Kabine (Kasten x109..118 y134..143)
set(_i3 "120;130;144;154")   # Suchfenster um die 3F-Kabine (Kasten x127..136 y137..146)
fahrt(f1_steh   "${_k1}" "${_tuer}"              "${_i1}")
fahrt(f1_vor    "${_k1}" "${_tuer},U2"           "${_i1}")
fahrt(f1_rechts "${_k1}" "${_tuer},R0.6,U2"      "${_i1}")
fahrt(f1_links  "${_k1}" "${_tuer},L0.6,U2"      "${_i1}")
fahrt(f3_rechts "${_k3}" "${_tuer},R0.6,U2"      "${_i3}")
fahrt(f3_links  "${_k3}" "${_tuer},L0.6,U2"      "${_i3}")
file(REMOVE "${_exe_kopie}")

# Urteil
macro(pruefe _was _bed)
    if(${_bed})
        message(STATUS "r35_fahrstuhl: PASS ${_was}")
    else()
        message(STATUS "r35_fahrstuhl: FAIL ${_was}")
        set(_fehler "${_fehler}\n  ${_was}")
    endif()
endmacro()
foreach(_n f1_steh f1_vor f1_rechts f1_links)
    set(_drin FALSE)
    if(_mx_${_n} GREATER_EQUAL 110 AND _mx_${_n} LESS_EQUAL 117 AND
       _my_${_n} GREATER_EQUAL 135 AND _my_${_n} LESS_EQUAL 142)
        set(_drin TRUE)
    endif()
    pruefe("${_n}: Marker (${_mx_${_n}},${_my_${_n}}) im gemalten 1F-Innenraum x110..117 y135..142" _drin)
endforeach()
foreach(_n f3_rechts f3_links)
    set(_drin FALSE)
    if(_mx_${_n} GREATER_EQUAL 128 AND _mx_${_n} LESS_EQUAL 135 AND
       _my_${_n} GREATER_EQUAL 138 AND _my_${_n} LESS_EQUAL 145)
        set(_drin TRUE)
    endif()
    pruefe("${_n}: Marker (${_mx_${_n}},${_my_${_n}}) im gemalten 3F-Innenraum x128..135 y138..145" _drin)
endforeach()
set(_xs ${_mx_f1_steh} ${_mx_f1_vor} ${_mx_f1_rechts} ${_mx_f1_links})
set(_ys ${_my_f1_steh} ${_my_f1_vor} ${_my_f1_rechts} ${_my_f1_links})
list(SORT _xs COMPARE NATURAL)
list(SORT _ys COMPARE NATURAL)
list(GET _xs 0 _x0)
list(GET _xs -1 _x1)
list(GET _ys 0 _y0)
list(GET _ys -1 _y1)
math(EXPR _sx "${_x1} - ${_x0}")
math(EXPR _sy "${_y1} - ${_y0}")
set(_ok FALSE)
if(_sx GREATER_EQUAL 6)
    set(_ok TRUE)
endif()
pruefe("1F: Marker wandert quer x ${_x0}..${_x1} = ${_sx} px (soll >= 6)" _ok)
set(_ok FALSE)
if(_sy GREATER_EQUAL 6)
    set(_ok TRUE)
endif()
pruefe("1F: Marker wandert laengs y ${_y0}..${_y1} = ${_sy} px (soll >= 6)" _ok)
math(EXPR _s3 "${_mx_f3_rechts} - ${_mx_f3_links}")
set(_ok FALSE)
if(_s3 GREATER_EQUAL 6)
    set(_ok TRUE)
endif()
pruefe("3F: Marker wandert quer ${_mx_f3_links}..${_mx_f3_rechts} = ${_s3} px (soll >= 6, rechts = Karte-Ost)" _ok)
set(_ok FALSE)
if(_mx_f1_rechts GREATER _mx_f1_links AND _my_f1_vor LESS _my_f1_steh)
    set(_ok TRUE)
endif()
pruefe("1F: 180 Grad - Welt-West (rechts gelaufen) = Karte-Ost, vorwaerts (-z) = Karte-oben" _ok)

if(NOT "${_fehler}" STREQUAL "")
    message(FATAL_ERROR "r35_fahrstuhl: ROT:${_fehler}")
endif()
message(STATUS "r35_fahrstuhl: alle Pruefungen OK")
