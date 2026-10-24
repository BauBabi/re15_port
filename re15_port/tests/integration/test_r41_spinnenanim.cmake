cmake_minimum_required(VERSION 3.16)
# =============================================================================
# SCHACHTSPINNE ROOM2060 IM ECHTLAUF (Runde 41, Spur L "spinnenanim").
#
# Nutzer (AUFTRAG.md Z. 47): "Die Spinnen Animation beim herausklettern vom Gitter und runter klettern an der Wand
# stimmen noch nicht ganz." Dossier analysis/befunde_runde41/L_spinnenanim.md. Die Mechanik misst unit_r41_spinnenanim
# im Spielschritt; dieser Riegel misst die ECHTE exe: die Messschiene RE15_SPINNENANIM_LOG schreibt je Takt die Matrix,
# die der Gegner-Zeichner in platform/pc/main.c WIRKLICH benutzt (nach Drehung und Massstab), und das gezeichnete Bild.
#
#   Lauf L2  Leon, ROOM2060 (Debug-Sprung, Spieler an der Tuerlage aus 2030), ab Raumbild 150 gleitet der Spieler auf
#            (6000,3000) = Cut 1 (RE15_POCC_TP, dieselbe Lage wie die Original-Messung), KI = Standard (RE2).
#   Lauf L15 derselbe Lauf mit RE15_AI_FLAVOR=re15 (RE1.5-Gehirn, anim_set-Zwilling re15_aspider_anim).
#   Lauf E2  derselbe Lauf als ELZA (RE15_TITLE_SHOT_CHAR=1, ROOM2061 = gleiche Sce_em_set-Zeilen, R38 1.1).
#
# Auswerter `test_r41_spinnenanim pruef_log <log>`: Takte 62..133 der Spinne e0 gegen die Original-Tabelle
# (Figur-Matrix +0x20, gezeichnetes Bild +0x95-1, Ueberblendung +0x8f vor dem Takt).
#
# Eigener exe-Name je Lauf (Muster test_r38_spinnen_echtlauf.cmake), SDL_ASSERT=always_ignore, Endung der exe aus
# RE15_PC_EXE (Linux ohne .exe).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r41_spinnenanim> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_spinnenanim: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
    message(FATAL_ERROR "r41_spinnenanim: RE15_TOOL fehlt: '${RE15_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_spinnenanim: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41l_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r41l_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

set(_fehler "")

function(r41l_lauf _name _sprung _raum)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 300
        SDL_ASSERT=always_ignore RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${ARGN}
        RE15_DEBUG_JUMP=${_sprung}@100 "RE15_POCC_TP=150,6000,3000,1024"
        RE15_SPINNENANIM_LOG=spinnenanim.log
        "RE15_EXIT_AT=250#${_raum}"
        "${_exe}")
    if(NOT EXISTS "${WORKDIR}/spinnenanim.log")
        set(_fehler "${_fehler}\n[${_name}] kein spinnenanim.log (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    execute_process(COMMAND "${RE15_TOOL}" pruef_log "${WORKDIR}/spinnenanim.log"
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 120 RESULT_VARIABLE _rp OUTPUT_VARIABLE _op)
    message(STATUS "r41_spinnenanim[${_name}] exe exit=${_rv}\n${_op}")
    if(NOT _rp EQUAL 0)
        set(_fehler "${_fehler}\n[${_name}] pruef_log exit=${_rp}\n${_op}" PARENT_SCOPE)
    endif()
endfunction()

r41l_lauf(L2 2060 2060 RE15_AI_FLAVOR=re2 RE15_AI_MODELS=re2)
r41l_lauf(L15 2060 2060 RE15_AI_FLAVOR=re15 RE15_AI_MODELS=re2)
r41l_lauf(E2 2060 2061 RE15_TITLE_SHOT_CHAR=1 RE15_AI_FLAVOR=re2 RE15_AI_MODELS=re2)   # Elza: Sprungziel 2060 -> Raum 2061 (Leon-RDT + 1, wie R38 Lauf E)

file(REMOVE "${_exe}")
if(NOT _fehler STREQUAL "")
    message(FATAL_ERROR "r41_spinnenanim: ${_fehler}")
endif()
message(STATUS "r41_spinnenanim: L2 (Leon RE2-KI), L15 (Leon RE1.5-KI) und E2 (Elza ROOM2061) bestanden")
