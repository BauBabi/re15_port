# Runde 37 Spur C Nachbesserung 1 (analysis/befunde_runde37/C_fahrstuhl.md "Nachbesserung 1") - echte exe:
#   A  Nachtrag 5a: Lastenaufzug ROOM3080 (Sprung, erste Fahrt mit Ada) -> Aot_on -> Tuersequenz Port-Archiv P2DS
#      Variante 5 (451 Bilder, 2 Se_on) VOR dem Laden von ROOM4000.
#   B  Nachtrag 5b: ROOM2030 Lift A, echter Weg (Generator (3,126), auf der Plattform Aktionstaste per Eingabeskript):
#      Anker KLICK + FAHRT_KURZ; Ton = Differenz der Aufnahmen mit/ohne Aktionstaste (RE15_AUDIO_CAP_SYNC, Auswertung
#      probe_r37_fahrstuhl_n1 kapsel): Stopp-Schlag 90 + 20 Takte nach Fahrtbeginn, Dauer = Wellenlaenge.
#   C  dasselbe Lift B: KLICK + FAHRT, Stopp-Schlag 180 + 20.
# Eingabe nur ueber RE15_INPUT_SCRIPT (ersetzt die Tasten-Bits ganz): ein angeschlossener (virtueller) Controller
# veraendert den Lauf sonst (gemessen 2026-10-12: Spieler lief bei Bild 120..210 von der Plattform).
cmake_minimum_required(VERSION 3.16)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_fahrstuhl_n1: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_PROBE_N1 OR NOT EXISTS "${RE15_PROBE_N1}")
    message(FATAL_ERROR "r37_fahrstuhl_n1: RE15_PROBE_N1 fehlt/existiert nicht: '${RE15_PROBE_N1}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r37_fahrstuhl_n1: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37c_n1_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r37c_n1_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(n1_lauf _name _out_dbg)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 300 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 ${ARGN} "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r37_fahrstuhl_n1[${_name}]: debug.log fehlt (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    set(${_out_dbg} "${_dbg}" PARENT_SCOPE)
endfunction()

function(n1_muss _name _text _muster)
    string(REGEX MATCH "${_muster}" _m "${_text}")
    if(NOT _m)
        message(FATAL_ERROR "r37_fahrstuhl_n1[${_name}]: erwartet /${_muster}/ - fehlt")
    endif()
endfunction()

function(n1_vor _name _text _a _b)
    string(FIND "${_text}" "${_a}" _pa)
    string(FIND "${_text}" "${_b}" _pb)
    if(_pa LESS 0 OR _pb LESS 0 OR NOT _pa LESS _pb)
        message(FATAL_ERROR "r37_fahrstuhl_n1[${_name}]: [${_a}] (${_pa}) nicht vor [${_b}] (${_pb})")
    endif()
endfunction()

# Ton-Auswertung: Differenz der Aufnahmen mit/ohne Aktionstaste
function(n1_kapsel _name _mit _ohne _satz)
    execute_process(COMMAND "${RE15_PROBE_N1}" kapsel "${_mit}" "${_ohne}" ${_satz}
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 120)
    message(STATUS "r37_fahrstuhl_n1[${_name}] ${_out}")
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "r37_fahrstuhl_n1[${_name}]: Ton-Auswertung ROT (${_rv}) ${_err}")
    endif()
    file(REMOVE "${_mit}" "${_ohne}")
endfunction()

set(_titel RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
set(_ohne_tasten "RE15_INPUT_SCRIPT=W1")
set(_aktion RE15_INPUT_SCRIPT=A0.1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=200)

# A: Lastenaufzug ROOM3080 -> ROOM4000 (erste Fahrt, Ada-Szene, Aot_on @0x00A22)
n1_lauf(a _dbg ${_titel} "RE15_AUDIO_CAP_SYNC=${_basis}_a.raw" ${_ohne_tasten} RE15_DEBUG_JUMP=3080@120 "RE15_EXIT_AT=60#4000")
file(REMOVE "${_basis}_a.raw")
n1_muss(a "${_dbg}" "\\[aot\\] DOOR FIRE slot=0 rect=\\(0,0,hw=0,hh=0\\) target_cut=1 spawn=\\(-7600,0,-6760\\)")
n1_muss(a "${_dbg}" "\\[tuer\\] Sequenz Archiv 2 DOOR2D Port-Archiv P2DS Variante 5 Bit7 0 Tuer 45 Seite S000 T112")
n1_muss(a "${_dbg}" "\\[tuer\\] Sequenz fertig: 451 Bilder \\+ 1 Warten, 2 Se_on")
n1_vor(a "${_dbg}" "Port-Archiv P2DS Variante 5" "PC loaded room4000.rdt")

# B: Lift A (Platz 5, Mitte (6600,12100), Band 3), Stand 620 davor mit Blick +x
set(_lift_a ${_titel} RE15_DEBUG_JUMP=2030@120 RE15_PLAYER_POS=5980,12100,0,3 RE15_SET_FLAG=3:126 RE15_EXIT_AT=420)
n1_lauf(b_ohne _dbg ${_lift_a} ${_ohne_tasten} "RE15_AUDIO_CAP_SYNC=${_basis}_b_ohne.raw")
n1_lauf(b_mit _dbg ${_lift_a} ${_aktion} "RE15_AUDIO_CAP_SYNC=${_basis}_b_mit.raw")
n1_muss(b "${_dbg}" "\\[lift2030\\] Anker @0x01E30 -> Satz 0")
n1_muss(b "${_dbg}" "\\[lift2030\\] Anker @0x01E54 -> Satz 2")
n1_kapsel(b "${_basis}_b_mit.raw" "${_basis}_b_ohne.raw" 2)

# C: Lift B (Platz 7, Mitte (-9800,3600), Band 3)
set(_lift_b ${_titel} RE15_DEBUG_JUMP=2030@120 RE15_PLAYER_POS=-10032,3600,0,3 RE15_SET_FLAG=3:126 RE15_EXIT_AT=520)
n1_lauf(c_ohne _dbg ${_lift_b} ${_ohne_tasten} "RE15_AUDIO_CAP_SYNC=${_basis}_c_ohne.raw")
n1_lauf(c_mit _dbg ${_lift_b} ${_aktion} "RE15_AUDIO_CAP_SYNC=${_basis}_c_mit.raw")
n1_muss(c "${_dbg}" "\\[lift2030\\] Anker @0x01F0A -> Satz 0")
n1_muss(c "${_dbg}" "\\[lift2030\\] Anker @0x01F2E -> Satz 1")
n1_kapsel(c "${_basis}_c_mit.raw" "${_basis}_c_ohne.raw" 1)

file(REMOVE "${_exe_kopie}")
message(STATUS "r37_fahrstuhl_n1: A (3080 -> 4000 P2DS V5), B (Lift A Ton), C (Lift B Ton) bestanden")
