cmake_minimum_required(VERSION 3.16)
# =============================================================================
# INVENTAR MIT DER ECHTEN EXE (Runde 39, Spur C). Dossier analysis/befunde_runde39/C_inventar.md, Konstanten
# include/re15_inventar_r39.h. NUTZER-VORGABEN (AUFTRAG.md Runde 39): (1) Spind ROOM1000 erst Handgun-Munition, dann
# "Nothing useful"; (2) Memory Cards stapelbar ("alles mit einer Zahl"); (3) Elza ohne Leons Bild/Karte im Status;
# (4) gelbe Karte bei Elza nach "discard" weg.
#
# Jeder Lauf: Spielstand per Kartenwerkzeug + CONTINUE (RE15_CONTINUE_TEST), Eingaben RE15_INPUT_SCRIPT / RE15_PRESS
# (Viereck = Bestaetigen, byte-true virtuell 0x4000 @0x80028570), Messschiene RE15_INV_LOG ([inv-r39] Inventar bei
# jeder Aenderung, [inv-r39-menu] Menue/Nachricht/Abfrage) + RE15_MSG_LOG ([msgtext]). Bildnummern aus den Messungen
# C_belege (lauf.sh).
#   spind    A Leon ROOM1000 vor dem Spind (20900,-1100) rot 3072, vier Aktionsdruecke (W1,A,W3.5,A,W4,A,W4,A):
#              Installer-Zeile (Slot 3 Item 0x15 x30 Bit (9,69), Text -> Slot 11), Modal -> Ja -> 15:50 -> 15:65
#              (30 im Satz, Nutzer-Halbierung), danach msg 0 "Nothing unusual." GENAU einmal (Druck 3 oeffnet, 4 schliesst)
#            B dasselbe mit (9,69)=1: "schon genommen", Inventar unveraendert, msg 0 beim ersten Druck
#   stapeln  C Leon ROOM1010 mit 21:3, Regal-Aufnahme Memory Card x3 -> 21:6 auf EINEM Platz (kein zweiter "21:")
#            D Leon ROOM1010 leer + 21:3 + 21:2, START -> ITEM -> Platz 0 -> COMBINE -> Platz 1 -> Ja -> 21:5, Platz 1 leer
#   status   E Elza ROOM1001, START, Framedump Bild 145: "[inv] Status-Bild ITEM/STPIC_04.TIM (Figur 4" und der Block
#              bei (14,26) == STPIC_04 (>= 90 % der Texel, Zeilen 0..79 = Driver License), nicht == STPIC_00 (< 50 %)
#            F Leon ROOM1000 dasselbe: STPIC_00 geladen, Block == STPIC_00
#   discard  G Elza ROOM11E1 mit Yellow Keycard + (9,0x8A): Leser -> Ja -> msg 9 -> Tastenfeld -> Code-Bits per
#              RE15_SET_FLAG_AT -> msg 5 -> Wegwerf-Abfrage (wegwerf=1, bereit) -> Ja -> Inventar ohne 39:1
#            H Leon ROOM11E0 mit (3,139) (wie nach ROOM1240 @0x0055A) + Karte: kein msg 7, keine Abfrage, Karte bleibt
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r37_feuerloescher_karte> -DRE15_BILD_TOOL=<probe_r39_inventar_bild>
#               -DRE15_ASSET_PSX_DIR=<shared_assets/PSX> -DWORKDIR=<dir> -DTEIL=spind|stapeln|status|discard
#               -P test_r39_inventar.cmake
# =============================================================================
foreach(_v RE15_PC_EXE RE15_KARTE_TOOL RE15_BILD_TOOL)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r39_inventar: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR OR NOT TEIL OR NOT RE15_ASSET_PSX_DIR)
    message(FATAL_ERROR "r39_inventar: WORKDIR/TEIL/RE15_ASSET_PSX_DIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/${TEIL}_${_lauf_id}")

# EIGENER EXE-NAME je Teil neben der exe (Asset-Wurzel = exe-Verzeichnis); alte Kopien NUR dieses Teils raeumen.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r39c_${TEIL}_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r39c_${TEIL}_${_lauf_id}${_exe_ext}")
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
# Anzahl der Vorkommen eines Musters muss == _soll sein.
macro(pruef_anzahl _name _text _muster _soll)
    string(REGEX MATCHALL "${_muster}" _treffer "${${_text}}")
    list(LENGTH _treffer _n)
    if(NOT _n EQUAL ${_soll})
        list(APPEND _fehler "${_name}: '${_muster}' ${_n}x (Soll ${_soll})")
        message(STATUS "FAIL ${_name}: '${_muster}' ${_n}x (Soll ${_soll})")
    else()
        message(STATUS "ok   ${_name}: '${_muster}' ${_n}x")
    endif()
endmacro()

# Ein Lauf: Karte schreiben, exe starten (CONTINUE), debug.log lesen. _extra = ;-Liste KEY=WERT.
function(c_lauf _name _raum _kartenargs _ende _extra _out)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${_kartenargs}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r39_inventar[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 400
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_MSG_LOG=1 RE15_INV_LOG=1
        "RE15_EXIT_AT=${_ende}"
        ${_extra}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r39_inventar[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(TOLOWER "${_raum}" _raum_kl)
    string(FIND "${_dbg}" "CONTINUE: resumed in room ${_raum_kl}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r39_inventar[${_name}]: Spielstand in ROOM${_raum} nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r39_inventar[${_name}]: Endbild nicht erreicht - Lauf vorher abgerissen (exit=${_rv}, ${WORKDIR})")
    endif()
    message(STATUS "r39_inventar[${_name}]: exit=${_rv}, ${WORKDIR}")
    set(${_out} "${_dbg}" PARENT_SCOPE)
endfunction()

# Ausweis-Block eines Dumps gegen eine STPIC: Trefferquote in Prozent -> _out.
function(c_bild _name _ppm _stpic _zeilen _out)
    execute_process(COMMAND "${RE15_BILD_TOOL}" "${_ppm}" "${RE15_ASSET_PSX_DIR}/ITEM/${_stpic}" 14 26 ${_zeilen} 0
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _o)
    string(STRIP "${_o}" _o)
    message(STATUS "r39_inventar[${_name}] ${_stpic}: ${_o}")
    if(NOT _o MATCHES "\\(([0-9]+)%\\)")
        set(${_out} -1 PARENT_SCOPE)
        return()
    endif()
    set(${_out} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

set(_skript_vier "W1,A0.1,W3.5,A0.1,W4,A0.1,W4,A0.1,W4")
set(_env_skript "RE15_INPUT_SCRIPT_BASIS=spiel;RE15_INPUT_SCRIPT_START=60;RE15_INPUT_SCRIPT=${_skript_vier}")

if(TEIL STREQUAL "spind")
    c_lauf(a 1000 "1000;20900;-1100;3072" 560 "${_env_skript}" _a)
    pruef(A1 _a "[inventar-r39] ROOM1000 Spind: Slot 3 Aufnahme Item 0x15 x30 Bit (9,69) (Form ROOM1001 @0x00C24), Raumtext msg 0 -> Slot 11")
    pruef(A2 _a "[inv-r39] F198 R1000 eq=128 inv=03:15,15:65,00:0")
    pruef_anzahl(A3 _a "\\[msgtext\\] room=1000 id=0 \"Nothing unusual\\.\"" 1)
    pruef_nicht(A4 _a "15:80")
    c_lauf(b 1000 "1000;20900;-1100;3072;flag:9:69" 300 "RE15_INPUT_SCRIPT_BASIS=spiel;RE15_INPUT_SCRIPT_START=60;RE15_INPUT_SCRIPT=W1,A0.1,W3.5,A0.1,W4" _b)
    pruef(B1 _b "[inventar-r39] ROOM1000 Spind: Munition schon genommen (9,69)=1 -> Slot 3 Raumtext msg 0")
    pruef(B2 _b "[msgtext] room=1000 id=0 \"Nothing unusual.\"")
    pruef_anzahl(B3 _b "\\[inv-r39\\] F[0-9]+ R1000" 1)
    pruef_nicht(B4 _b "Slot 3 Aufnahme Item 0x15")

elseif(TEIL STREQUAL "stapeln")
    c_lauf(c 1010 "1010;2800;-1300;0;give:0x21:3" 320 "${_env_skript}" _c)
    pruef(C1 _c "[inv-r39] F0 R1010 eq=128 inv=03:15,15:50,21:3,00:0")
    pruef(C2 _c "[inv-r39] F198 R1010 eq=128 inv=03:15,15:50,21:6,00:0")
    pruef_nicht(C3 _c "21:3,21:3")
    c_lauf(d 1010 "1010;2800;-1300;0;leer;give:0x21:3;give:0x21:2" 260
           "RE15_PRESS=start@120,square@139,square@149,right@158,square@160,right@162,square@164" _d)
    pruef(D1 _d "[inv-r39] F0 R1010 eq=128 inv=21:3,21:2,00:0")
    pruef(D2 _d "[inv-r39-menu] F160 offen=1 sub=3 st=7 cur=0 cur2=0 cmd=3")
    pruef(D3 _d "[inventar-r39] Kombinieren: Item 0x21 Platz 0 + 1 -> Aktion 5 (Selbst-Stapeln")
    pruef(D4 _d "[inv-r39] F164 R1010 eq=128 inv=21:5,00:0,00:0")

elseif(TEIL STREQUAL "status")
    c_lauf(e 1001 "1001;20900;-1100;3072;elza" 160 "RE15_PRESS=start@120;RE15_FRAMEDUMP=145:status.ppm" _e)
    pruef(E1 _e "[inv] Status-Bild ITEM/STPIC_04.TIM (Figur 4, Runde 39 Spur C)")
    pruef(E2 _e "[inv-r39-menu] F129 offen=1")
    c_bild(E3 "${_basis}_e/status.ppm" STPIC_04.TIM 80 _e04)
    c_bild(E4 "${_basis}_e/status.ppm" STPIC_00.TIM 80 _e00)
    if(_e04 LESS 90)
        list(APPEND _fehler "E3: Elza-Block == STPIC_04 nur ${_e04} % (Soll >= 90)")
    endif()
    if(_e00 GREATER 50 OR _e00 LESS 0)
        list(APPEND _fehler "E4: Elza-Block aehnelt STPIC_00 zu ${_e00} % (Soll < 50)")
    endif()
    c_lauf(f 1000 "1000;20900;-1100;3072" 160 "RE15_PRESS=start@120;RE15_FRAMEDUMP=145:status.ppm" _f)
    pruef(F1 _f "[inv] Status-Bild ITEM/STPIC_00.TIM (Figur 0, Runde 39 Spur C)")
    c_bild(F2 "${_basis}_f/status.ppm" STPIC_00.TIM 80 _f00)
    if(_f00 LESS 90)
        list(APPEND _fehler "F2: Leon-Block == STPIC_00 nur ${_f00} % (Soll >= 90)")
    endif()

elseif(TEIL STREQUAL "discard")
    # G: Bildnummern aus C_belege/m4_nachher2 (Leser-Prompt: Tippen bis Bild ~470, Ja-Nein sichtbar ab fsm=3).
    c_lauf(g 11E1 "11E1;10400;4250;0;elza;give:0x39:1;flag:9:0x8A" 1300
           "RE15_PRESS=square@100,square@500,square@600,square@700,square@1100;RE15_SET_FLAG_AT=5:13,5:14,5:15,5:16@800" _g)
    pruef(G1 _g "[inv-r39] F0 R11E1 eq=128 inv=03:15,15:50,39:1,00:0")
    pruef(G2 _g "[msgtext] room=11e1 id=7 \"A card reader.")
    pruef(G3 _g "[msgtext] room=11e1 id=9 \"You've used the Yellow Keycard.\"")
    pruef(G4 _g "[msgtext] room=11e1 id=5 \"You've opened the lock.\"")
    pruef(G5 _g "wegwerf=1 ")
    pruef(G6 _g "wegwerf=1 fsm=0 sel=0 bereit=1")
    pruef(G7 _g "R11E1 eq=128 inv=03:15,15:50,00:0,00:0")
    c_lauf(h 11E0 "11E0;10400;4250;0;give:0x39:1;flag:9:0x8A;flag:3:139" 400
           "RE15_PRESS=square@100,square@200,square@300" _h)
    pruef_nicht(H1 _h "[msgtext] room=11e0 id=7")
    pruef_nicht(H2 _h "wegwerf=1")
    pruef_anzahl(H3 _h "\\[inv-r39\\] F[0-9]+ R11E0" 1)
    pruef(H4 _h "inv=03:15,15:50,39:1,00:0")
else()
    message(FATAL_ERROR "r39_inventar: unbekannter TEIL '${TEIL}'")
endif()

if(_fehler)
    string(REPLACE ";" "\n  " _f "${_fehler}")
    message(FATAL_ERROR "r39_inventar[${TEIL}] FEHLER:\n  ${_f}")
endif()
message(STATUS "r39_inventar[${TEIL}]: alle Pruefungen gruen")
