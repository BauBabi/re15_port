# =============================================================================
# test_r41_gatorfix.cmake - Runde 41 Spur G2 "gatorfix", exe-Riegel (Dossier analysis/befunde_runde41/G2_gatorfix.md).
#
# TEIL=gang_montage|gang_montage_elza  Punkt 3: Leiter 2080 -> 2090-Ankunftsszene -> Montage 2080 (Programm 2) -> zurueck. state.log: die fuenf
#     Zombies der Montage laufen in den Montage-Bildern (Spieler an der Leiter-Ankunft PL(-9950,-25700)) je >= 1000 naeher an die Leiter
#     (Auswerter test_r41_gatorfix ganglog montage). Vorher (Bau 789f6e681, Lauf v_ank_leon): 8 Zombies, 4 davon >= 1000, kleinster Weg -452.
# TEIL=gang_szene|gang_szene_elza  Punkt 4: Kampf 2090 mit dem Ausloese-Messhaken RE15_K2080_GATOR_HP=24990 (ein echter Treffer) -> Marvin/John-
#     Szene in 2080/2081. state.log: drei Zombies, jeder naeher an der Leiter, wenn er stirbt, und noch >= 2000 entfernt (ganglog szene); debug.log:
#     letzter Schuss <= 90 Bilder nach dem Szenenbeginn. Vorher: der Spieler stand am Treppenfuss, 8 stehende Zombies, letzter Schuss F351.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r41_gatorfix> -DWORKDIR=<dir> -DTEIL=<teil> -P test_r41_gatorfix.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_gatorfix: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
    message(FATAL_ERROR "r41_gatorfix: RE15_TOOL fehlt/existiert nicht: '${RE15_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_gatorfix: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_dir "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_dir}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe_kopie "${_exe_dir}/re15_pc_r41g2_${_lauf_id}.exe")
if(NOT WIN32)
    set(_exe_kopie "${_exe_dir}/re15_pc_r41g2_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(WORKDIR "${_dir}")

function(g2_fehler _text)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_gatorfix: Lauf ${TEIL} - ${_text} (${_dir})")
endfunction()

foreach(_v RE15_GB_TEST RE15_GATOR_KANISTER RE15_KEIN_GATOR RE15_K2080_WURF_GIER RE15_K2080_GATOR_HP RE15_SET_FLAG RE15_FIRE_AOT RE15_G2_WURF_CUT)
    unset(ENV{${_v}})
endforeach()

if(TEIL MATCHES "elza$")
    set(_r2080 2081)
    set(_r2090 2091)
    set(_wahl RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
else()
    set(_r2080 2080)
    set(_r2090 2090)
    set(_wahl RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
endif()

if(TEIL MATCHES "^gang_montage")
    re15_start_spiel(_rv 600
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 ${_wahl}
        RE15_SET_FLAG=3:245 RE15_STATE_LOG=state.log RE15_DEBUG_JUMP=${_r2080}@250 "RE15_FIRE_AOT=1@40#${_r2080}"
        "RE15_EXIT_AT=150#${_r2080}" "${_exe_kopie}")   # Montage-Besuch F1..F156 (der erste 2080-Besuch endet mit der Leiter bei ~F45)
elseif(TEIL MATCHES "^gang_szene")
    set(_schuss "W6.0")
    foreach(_i RANGE 1 20)
        string(APPEND _schuss ",M0.6,MA0.1,M0.3,MA0.1,W3.0")
    endforeach()
    re15_start_spiel(_rv 600
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 ${_wahl}
        RE15_DEBUG_JUMP=${_r2090}@250 "RE15_SET_FLAG=9:110,3:245" "RE15_GIVE=3:60" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_schuss}" RE15_STATE_LOG=state.log "RE15_PLAYER_POS=-6000,-17500,3072"
        "RE15_EXIT_AT=200#${_r2080}" "${_exe_kopie}")
else()
    g2_fehler("unbekannter TEIL")
endif()
file(REMOVE "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    g2_fehler("exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT)")
endif()
if(TEIL MATCHES "^gang_montage")
    set(_modus montage)
else()
    set(_modus szene)
endif()
execute_process(COMMAND "${RE15_TOOL}" ganglog "${_dir}/state.log" ${_modus} RESULT_VARIABLE _gr OUTPUT_VARIABLE _go ERROR_VARIABLE _ge)
string(REGEX MATCH "ganglog [a-z]+: [0-9]+ Bilder an der Leiter[^\n]*" _gz "${_go}")
if(NOT _gr EQUAL 0)
    g2_fehler("Gang zur Leiter: ${_gz}")
endif()
if(_modus STREQUAL "szene")
    file(STRINGS "${_dir}/debug.log" _sl REGEX "\\[k2080\\] F[0-9]+ Schuss [0-9]+: Treffer")
    list(LENGTH _sl _n)
    if(NOT _n EQUAL 3)
        g2_fehler("${_n} Treffer statt 3")
    endif()
    list(GET _sl 2 _l3)
    string(REGEX MATCH "F([0-9]+) Schuss" _m "${_l3}")
    if(CMAKE_MATCH_1 GREATER 90)
        g2_fehler("letzter Schuss F${CMAKE_MATCH_1} > 90 Bilder nach dem Szenenbeginn")
    endif()
    message(STATUS "r41_gatorfix ${TEIL}: ${_gz}; letzter Schuss F${CMAKE_MATCH_1}")
else()
    message(STATUS "r41_gatorfix ${TEIL}: ${_gz}")
endif()
