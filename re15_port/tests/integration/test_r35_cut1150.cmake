# =============================================================================
# IRONS-TODESSZENE + KNALL-MONTAGE MIT DER ECHTEN EXE (Runde 35, Spur L).
#
# Dossier analysis/befunde_runde35/L_cut1150.md, Konstanten include/re15_irons_tod.h, include/re15_tuer1060.h.
# Die Unit-Riegel unit_r35_cut1150_* pruefen jeden Schritt einzeln ueber die VM; dieser Riegel prueft den
# ECHTEN Weg: Spielstand (Karte aus probe_r35_cut1150_karte) + CONTINUE in ROOM1130 vor der Tuer zu Irons'
# Buero, Aktionstaste -> Tuer -> ROOM1150: die Szene startet beim Betreten, die Kette laeuft durch alle
# Raeume (1130 -> 1040 -> 1030 -> 11C0 -> 1150) und gibt die Steuerung zurueck; dazu die 1060-Sperre.
#
# Laeufe (debug.log-Marken [irons-tod] / [tuer1060] / [room], Flags aus dem Spielstand):
#   A  1130 -> 1150, alle Zombies in 1140 und 1070 leben: Szene, Montage 1130, 1040, 1030, 11C0, Rueckkehr
#   B  1130 -> 1150, 1140 und 1070 leer: Szene, direkt 1040 (ohne 1130), 1030 nur Cut 6, 11C0, Rueckkehr
#   C  1060 Etage 0, (9,71)=1: Aktion an der Tuer -> Text msg 1, kein Raumwechsel
#   D  1060 Etage 0, (9,73)=1: Aktion -> ROOM1040
#
# RE15_SOFTWARE_RENDER=1 dient nur der Robustheit des Hakens; geprueft wird das LOG. Das BILD belegt die
# Framedump-Abnahme im Dossier.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r35_cut1150_karte> -DWORKDIR=<dir>
#               -P test_r35_cut1150.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r35_cut1150: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r35_cut1150: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r35_cut1150: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name (test_r34n_b_cursor.cmake: local_build.sh beendet fremde re15_pc.exe).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r35l_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r35l_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

# Ein Lauf: Karte schreiben, exe starten (CONTINUE, Eingabeskript ab Bild 60), debug.log zurueckgeben.
function(it_lauf _name _raum _karte _skript _ende _ende_raum _timeout _out_debug)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}" ${_karte}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r35_cut1150[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv ${_timeout}
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
        "RE15_INPUT_SCRIPT=${_skript}"
        RE15_STATE_LOG=state.log
        "RE15_EXIT_AT=${_ende}#${_ende_raum}"
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r35_cut1150[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(FIND "${_dbg}" "CONTINUE: resumed in room ${_raum}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r35_cut1150[${_name}]: Spielstand in ROOM${_raum} nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r35_cut1150[${_name}]: Endbild nicht erreicht - Lauf vorher abgerissen (exit=${_rv}, ${WORKDIR})")
    endif()
    set(${_out_debug} "${_dbg}" PARENT_SCOPE)
endfunction()

function(it_muss _name _dbg _text)
    string(FIND "${_dbg}" "${_text}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r35_cut1150[${_name}]: fehlt im debug.log: '${_text}'")
    endif()
endfunction()
function(it_darf_nicht _name _dbg _text)
    string(FIND "${_dbg}" "${_text}" _p)
    if(NOT _p LESS 0)
        message(FATAL_ERROR "r35_cut1150[${_name}]: unerwartet im debug.log: '${_text}'")
    endif()
endfunction()
# Reihenfolge zweier Marken im Log.
function(it_vor _name _dbg _a _b)
    string(FIND "${_dbg}" "${_a}" _pa)
    string(FIND "${_dbg}" "${_b}" _pb)
    if(_pa LESS 0 OR _pb LESS 0 OR NOT _pa LESS _pb)
        message(FATAL_ERROR "r35_cut1150[${_name}]: '${_a}' (${_pa}) muss vor '${_b}' (${_pb}) liegen")
    endif()
endfunction()

# --- A: ganze Kette, alle Zombies leben --------------------------------------------------------
#  F90 Aktion an der Tuer (Standplatz aus der Karte: (-5900,16350) Blick 3072 vor Slot 2 @0x008CE)
# RE15_EXIT_AT zaehlt je Raum (der Bildzaehler startet bei jedem Raumladen neu): 1500 liegt ueber der
# Szenenlaenge des ERSTEN 1150-Besuchs (1283 Bilder, Unit-Riegel) und wird erst beim RUECKKEHR-Besuch erreicht.
it_lauf(a 1130 "nach10f0;ersteszene" "W1,A0.2,W200" 1500 1150 600 _dbg)
it_muss(A "${_dbg}" "[room] PC loaded room1150.rdt")
it_muss(A "${_dbg}" "Szene startet (Programm 0)")
it_muss(A "${_dbg}" "Umzug: 1140 lebend 5 -> 1130 (9,74)=1; 1070 lebend 5 -> 1030 (9,76)=1")
it_muss(A "${_dbg}" "Signal (5,12): Irons' Arm faellt")
it_muss(A "${_dbg}" "Montage 1130 (Programm 1)")
it_muss(A "${_dbg}" "Montage 1040 (Programm 2)")
it_muss(A "${_dbg}" "Montage 1030 (Programm 3)")
it_muss(A "${_dbg}" "Montage 11C0 (Programm 4)")
it_muss(A "${_dbg}" "Rueckkehr: Programm 5")
it_muss(A "${_dbg}" "Rueckkehr beendet, Steuerung frei")
it_vor(A "${_dbg}" "Szene startet" "Montage 1130")
it_vor(A "${_dbg}" "Montage 1130" "Montage 1040")
it_vor(A "${_dbg}" "Montage 1040" "Montage 1030")
it_vor(A "${_dbg}" "Montage 1030" "Montage 11C0")
it_vor(A "${_dbg}" "Montage 11C0" "Rueckkehr: Programm 5")
it_darf_nicht(A "${_dbg}" "Kette unterbrochen")
it_darf_nicht(A "${_dbg}" "KEIN Ereignis-Slot")
# der Schnitt nach 11C0 trifft den richtigen Raum (nicht 11B0)
it_muss(A "${_dbg}" "[room] PC loaded room11c0.rdt")
it_darf_nicht(A "${_dbg}" "[room] PC loaded room11b0.rdt")
# Steuerung am Ende frei: state.log letzte Zeile pm=0, Raum 1150
file(READ "${_basis}_a/state.log" _st)
string(REGEX MATCH "F[0-9]+ [^\n]*pm=([0-9])[^\n]*\n$" _letzte "${_st}")
if(NOT CMAKE_MATCH_1 STREQUAL "0")
    message(FATAL_ERROR "r35_cut1150[A]: am Ende ist der Spieler nicht frei (pm=${CMAKE_MATCH_1})")
endif()

# --- B: 1140 und 1070 leer: ohne 1130, ohne Cut-7-Teil ------------------------------------------
it_lauf(b 1130 "nach10f0;ersteszene;tot1140;tot1070" "W1,A0.2,W200" 1500 1150 600 _dbg)
it_muss(B "${_dbg}" "Umzug: 1140 lebend 0 -> 1130 (9,74)=0; 1070 lebend 0 -> 1030 (9,76)=0")
it_darf_nicht(B "${_dbg}" "Montage 1130")
it_darf_nicht(B "${_dbg}" "[room] PC loaded room1130.rdt")
it_muss(B "${_dbg}" "Montage 1040 (Programm 2)")
it_muss(B "${_dbg}" "Montage 1030 (Programm 3)")
it_muss(B "${_dbg}" "Montage 11C0 (Programm 4)")
it_muss(B "${_dbg}" "Rueckkehr beendet, Steuerung frei")
it_darf_nicht(B "${_dbg}" "Kette unterbrochen")

# --- C: 1060 gesperrt ((9,71)=1, (9,73)=0): Text, kein Raumwechsel ----------------------------
it_lauf(c 1060 "nach10f0;ersteszene" "W1,A0.2,W3" 240 1060 240 _dbg)
it_muss(C "${_dbg}" "[tuer1060] ROOM1060 Slot 2 -> Text-Platz msg 1")
it_darf_nicht(C "${_dbg}" "[room] PC loaded room1040.rdt")

# --- D: 1060 frei ((9,73)=1): Tuer nach 1040 -----------------------------------------------------
it_lauf(d 1060 "nach10f0;ersteszene;gesehen" "W1,A0.2,W3" 240 1040 240 _dbg)
it_darf_nicht(D "${_dbg}" "[tuer1060]")
it_muss(D "${_dbg}" "[room] PC loaded room1040.rdt")

file(REMOVE "${_exe_kopie}")
message(STATUS "r35_cut1150: OK - Kette A/B, Sperre C, Freigabe D")
