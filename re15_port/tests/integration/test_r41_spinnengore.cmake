cmake_minimum_required(VERSION 3.16)
# =============================================================================
# SPINNEN-PLATZEN IM ECHTLAUF (Runde 41, Spur S "spinnengore").
#
# Nutzer (2026-11-01): "die spinnen platzen bei magnum Schuss z.b. immer noch nicht..." Dossier
# analysis/befunde_runde41/S_spinnengore.md. Gemessen vorher: RE2-Gehirn besitzt jede 0x25, +0x93 = 0x41 beim
# Redhawk-Tod, Teilschicht aus (ta=0). Nachher: Teile 0/19 platzen, alle Beine fliegen (sh 0x80001, fl 0x7fff8).
#
#   ECHTER WEG: Debug-Sprung nach ROOM2070 (Leon) bzw. ROOM2071 (Elza, RE15_TITLE_SHOT_CHAR=1), Waffe per
#   RE15_GIVE/RE15_EQUIP (die Super Redhawk liegt im Spiel in ROOM3050/3051 @RDT 0x1ec6), Zielen + Feuern im Wechsel
#   (Zielhilfe FUN_8003703c waehlt die naechste Spinne). KI = Standard (kein RE15_AI_FLAVOR).
#   Lauf L7    Leon, Redhawk          -> "platzt"
#   Lauf E7    Elza, Redhawk          -> "platzt"
#   Lauf L8    Leon, Remington        -> "bein"   (RE2 trennt mit Zeile 7 ein Bein ab, FUN_80105BF0)
#   Lauf L7AUS Leon, Redhawk, RE15_KEIN_80PROZENT_GORE=1 -> "aus" (Gegenprobe: ohne die Hebung kein Platzen)
#
# Eigener exe-Name je Lauf (Muster test_r41_spinnenanim.cmake), SDL_ASSERT=always_ignore ueber spiel_lauf.cmake.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r41_spinnengore> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_spinnengore: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
    message(FATAL_ERROR "r41_spinnengore: RE15_TOOL fehlt: '${RE15_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_spinnengore: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41s_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r41s_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

set(_fehler "")
set(_skript "M0.3,MA0.1,M0.8,MA0.1,M0.8,MA0.1,M0.8,MA0.1,M0.8,MA0.1,M0.8,MA0.1,M0.8,MA0.1,M0.8,MA0.1,M1,W10")

function(r41s_lauf _name _raum _waffe _art)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${ARGN}
        RE15_DEBUG_JUMP=${_raum}@250
        RE15_GIVE=${_waffe}:60 RE15_EQUIP=${_waffe}
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=5 "RE15_INPUT_SCRIPT=${_skript}"
        RE15_SPINNEN_LOG=sp.log
        "RE15_EXIT_AT=400#${_raum}"
        "${_exe}")
    if(NOT EXISTS "${WORKDIR}/sp.log")
        set(_fehler "${_fehler}\n[${_name}] kein sp.log (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    execute_process(COMMAND "${RE15_TOOL}" pruef_log "${WORKDIR}/sp.log" ${_art}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 120 RESULT_VARIABLE _rp OUTPUT_VARIABLE _op)
    message(STATUS "r41_spinnengore[${_name}] exe exit=${_rv}\n${_op}")
    if(NOT _rp EQUAL 0)
        set(_fehler "${_fehler}\n[${_name}] pruef_log ${_art} exit=${_rp}\n${_op}" PARENT_SCOPE)
    endif()
endfunction()

r41s_lauf(L7 2070 7 platzt)
r41s_lauf(E7 2071 7 platzt RE15_TITLE_SHOT_CHAR=1)
r41s_lauf(L8 2070 8 bein)
r41s_lauf(L7AUS 2070 7 aus RE15_KEIN_80PROZENT_GORE=1)

file(REMOVE "${_exe}")
if(NOT _fehler STREQUAL "")
    message(FATAL_ERROR "r41_spinnengore: ${_fehler}")
endif()
message(STATUS "r41_spinnengore: L7 (Leon), E7 (Elza), L8 (Bein) und L7AUS (Gegenprobe) bestanden")
