# =============================================================================
# RUNDE 37 SPUR O (szene11c0) — ADA/MARVIN-SZENE ROOM11C0 MIT DER ECHTEN EXE.
#
# Dossier analysis/befunde_runde37/O_szene11c0.md. Weg wie integration_r35_cut1150 Lauf A / integration_r36_szene1150:
# Spielstand (Karte aus probe_r35_cut1150_karte) + CONTINUE in ROOM1130 vor der Tuer zu Irons' Buero, Aktionstaste ->
# Tuer -> ROOM1150 -> Szene -> Montage 1130 -> 1040 -> 1030 -> 11C0; Ende in ROOM11C0 Bild 505 (vor dem Schnitt zurueck).
# Mess-Haken: RE15_S11C0_LOG (engine/src/szene11c0_r37.c, je Bild Gier/Kopf-Ziel/Kopf-Ort beider Figuren + Untertitel),
# RE15_NECK_TRACE (skeleton_common.c, der gezeichnete Kopf). Auswertung: test_r37_szene11c0 auswerten <lauf>.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r35_cut1150_karte> -DRE15_AUSWERTER=<test_r37_szene11c0>
#               -DWORKDIR=<dir> -P test_r37_szene11c0.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.16)
foreach(_v RE15_PC_EXE RE15_KARTE_TOOL RE15_AUSWERTER)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r37_szene11c0: ${_v} fehlt: '${${_v}}'")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte "${_exe_dir}/re15_pc_r37o_haken_*")
foreach(_k IN LISTS _alte)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_ext "${RE15_PC_EXE}" EXT)
set(_exe "${_exe_dir}/re15_pc_r37o_haken_${_lauf_id}${_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "1130" nach10f0 ersteszene
                WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
    message(FATAL_ERROR "r37_szene11c0: Kartenwerkzeug exit=${_rvk}\n${_outk}")
endif()
re15_start_spiel(_rv 600
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=W1,A0.2,W200"
    RE15_S11C0_LOG=s11c0.log RE15_NECK_TRACE=neck_trace.log RE15_NECK_LOG=neck.log
    "RE15_EXIT_AT=505#11C0"
    "${_exe}")
file(REMOVE "${_exe}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r37_szene11c0: kein debug.log (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
foreach(_m "CONTINUE: resumed in room 1130" "Montage 11C0 (Programm 4)" "EXIT_AT: Bild 505 in Raum 11c0")
    string(FIND "${_dbg}" "${_m}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r37_szene11c0: fehlt im debug.log: '${_m}' (${WORKDIR})")
    endif()
endforeach()
# in 11C0 dreht sich niemand per Plc_dest Modus 9 (Gespraech ohne Koerperdrehung); der Lauf ist Modus 5 zur Tuer
string(FIND "${_dbg}" "Montage 11C0 (Programm 4)" _p11)
string(SUBSTRING "${_dbg}" ${_p11} -1 _dbg11)
string(FIND "${_dbg11}" "mode=0x09" _p9)
if(NOT _p9 LESS 0)
    message(FATAL_ERROR "r37_szene11c0: in ROOM11C0 noch ein Plc_dest Modus 9 (Koerperdrehung im Gespraech)")
endif()
string(FIND "${_dbg11}" "Plc_dest(slot=4 mode=0x05 dest=(-25100,17250))" _p5)
if(_p5 LESS 0)
    message(FATAL_ERROR "r37_szene11c0: Marvins Lauf zur Tuer (Plc_dest Modus 5) fehlt")
endif()
execute_process(COMMAND "${RE15_AUSWERTER}" auswerten "${WORKDIR}"
                TIMEOUT 120 RESULT_VARIABLE _rva OUTPUT_VARIABLE _outa ERROR_VARIABLE _erra)
message(STATUS "${_outa}")
if(NOT _rva EQUAL 0)
    message(FATAL_ERROR "r37_szene11c0: Auswertung exit=${_rva} (${WORKDIR})\n${_erra}")
endif()
message(STATUS "r37_szene11c0: OK - einander zugewandt, Ada ohne Drehung, Marvin nur beim Losrennen, Sprecher sieht Partner an")
