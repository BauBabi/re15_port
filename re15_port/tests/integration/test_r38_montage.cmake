# =============================================================================
# RUNDE 38 SPUR O (montage) — MONTAGE NACH IRONS' TOD MIT DER ECHTEN EXE: in jedem Schnitt, in dem Zombies durchbrechen,
# gehen sie auf die Kamera zu (wie die Zombies hinter dem Rolltor ROOM1040 Cut 1, der unveraendert bleibt).
#
# Dossier analysis/befunde_runde38/O_montage.md. Weg wie integration_r37_szene11c0 / integration_r35_cut1150 Lauf A:
# Spielstand (Karte aus probe_r35_cut1150_karte "1130 nach10f0 ersteszene") + CONTINUE in ROOM1130 vor der Tuer zu
# Irons' Buero, Aktionstaste -> Tuer -> ROOM1150 -> Szene -> Montage 1130 -> 1040 -> 1030 -> 11C0; Ende in ROOM11C0
# Bild 60. Mess-Haken: die [montage-r38]-Zeilen von engine/src/montage_r38.c (je Schnitt und Zombie Abstand zur
# Kamera-Bodenprojektion am Anfang/Ende, Kriech-Bit, Endlage; je Schnitt Spielerlage, HP, Navigator-Ziel) und
# "[irons-tod] Bilanz 1040". Auswertung: test_r38_montage auswerten <lauf> = dieselben Kriterien wie die Unit-Teile.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r35_cut1150_karte> -DRE15_AUSWERTER=<test_r38_montage>
#               -DWORKDIR=<dir> -P test_r38_montage.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.16)
foreach(_v RE15_PC_EXE RE15_KARTE_TOOL RE15_AUSWERTER)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r38_montage: ${_v} fehlt: '${${_v}}'")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte "${_exe_dir}/re15_pc_r38o_haken_*")
foreach(_k IN LISTS _alte)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_ext "${RE15_PC_EXE}" EXT)
set(_exe "${_exe_dir}/re15_pc_r38o_haken_${_lauf_id}${_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "1130" nach10f0 ersteszene
                WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
    message(FATAL_ERROR "r38_montage: Kartenwerkzeug exit=${_rvk}\n${_outk}")
endif()
re15_start_spiel(_rv 600
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=W1,A0.2,W200"
    "RE15_EXIT_AT=60#11C0"
    "${_exe}")
file(REMOVE "${_exe}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r38_montage: kein debug.log (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
foreach(_m "CONTINUE: resumed in room 1130" "Montage 1130 (Programm 1)" "Montage 1040 (Programm 2)" "Montage 1030 (Programm 3)"
           "EXIT_AT: Bild 60 in Raum 11c0")
    string(FIND "${_dbg}" "${_m}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r38_montage: fehlt im debug.log: '${_m}' (${WORKDIR})")
    endif()
endforeach()
execute_process(COMMAND "${RE15_AUSWERTER}" auswerten "${WORKDIR}"
                TIMEOUT 120 RESULT_VARIABLE _rva OUTPUT_VARIABLE _outa ERROR_VARIABLE _erra)
message(STATUS "${_outa}")
if(NOT _rva EQUAL 0)
    message(FATAL_ERROR "r38_montage: Auswertung exit=${_rva} (${WORKDIR})\n${_erra}")
endif()
message(STATUS "r38_montage: OK - 1130 Cut 0, 1030 Cut 7 und Cut 12: alle Zombies auf die Kamera zu; Rolltor 1040 Cut 1 wie vorher")
