# =============================================================================
# HELLIGKEITSPULS DES FEUERLOESCHERS MIT DER ECHTEN EXE (Runde 41, Spur D "kleinkram_a").
#
# Dossier analysis/befunde_runde41/D_kleinkram_a.md §2, Belege include/re15_blink_r41.h.
# Leon ROOM1030 Cut 0 (Nutzerbild fire_extinguisher.bmp = Cut 0, Runde 38 Spur K: Loescher-Huelle x 38..48,
# y 111..122), Bildserie RE15_FRAMEDUMP ueber eine Pulsperiode (60 Spielbilder, alle 4 Bilder):
#   - Mittelhelligkeit der Loescher-Huelle schwankt (Hub >= 6, gemessen 10,5 bei Spitze 0xBE/0x80 = +48 %
#     des beleuchteten Modells; Obergrenze 40 gegen Fremdeinfluss),
#   - die Pfeilerflaeche daneben (x 24..34, y 111..122, Hintergrund) bleibt konstant (Hub <= 1).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r41_kleinkram_a> -DWORKDIR=<dir>
#               -P test_r41_kleinkram_a_blink.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_kleinkram_a_blink: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
    message(FATAL_ERROR "r41_kleinkram_a_blink: RE15_TOOL fehlt: '${RE15_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_kleinkram_a_blink: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte "${_exe_dir}/re15_pc_r41d_blink_*${_exe_ext}")
foreach(_k IN LISTS _alte)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r41d_blink_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

re15_start_spiel(_rv 400
    SDL_ASSERT=always_ignore RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=1030@250 RE15_PLAYER_POS=-11500,-19500,2048
    "RE15_FRAMEDUMP=400-460/4:fl_"
    "RE15_EXIT_AT=470#1030"
    "${_exe}")
file(REMOVE "${_exe}")

set(_fehler "")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r41_kleinkram_a_blink: kein debug.log (exit=${_rv})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
string(FIND "${_dbg}" "[feuerloescher] ROOM1030: Loescher an der Wand obj 5" _p)
if(_p LESS 0)
    set(_fehler "${_fehler}\n[lauf] Loescher in ROOM1030 nicht installiert")
endif()
file(GLOB _bilder "${WORKDIR}/fl_*.ppm")
list(LENGTH _bilder _n)
if(_n LESS 15)
    set(_fehler "${_fehler}\n[lauf] nur ${_n} Bilder (exit=${_rv})")
else()
    list(SORT _bilder)
    execute_process(COMMAND "${RE15_TOOL}" ppm_puls 38 48 111 122 6 40 ${_bilder}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 120 RESULT_VARIABLE _rp OUTPUT_VARIABLE _op)
    message(STATUS "r41_kleinkram_a_blink[loescher]:\n${_op}")
    if(NOT _rp EQUAL 0)
        set(_fehler "${_fehler}\n[loescher] kein Helligkeitshub ueber die Pulsperiode:\n${_op}")
    endif()
    execute_process(COMMAND "${RE15_TOOL}" ppm_puls 24 34 111 122 0 1 ${_bilder}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 120 RESULT_VARIABLE _rq OUTPUT_VARIABLE _oq)
    message(STATUS "r41_kleinkram_a_blink[pfeiler]:\n${_oq}")
    if(NOT _rq EQUAL 0)
        set(_fehler "${_fehler}\n[pfeiler] Hintergrund daneben nicht konstant:\n${_oq}")
    endif()
endif()

if(NOT _fehler STREQUAL "")
    message(FATAL_ERROR "r41_kleinkram_a_blink FEHLER:${_fehler}")
endif()
message(STATUS "r41_kleinkram_a_blink: OK (Loescher pulsiert, Pfeiler konstant)")
