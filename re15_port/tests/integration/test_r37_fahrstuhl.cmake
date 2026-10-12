# Runde 37 Spur C "fahrstuhl" (analysis/befunde_runde37/C_fahrstuhl.md) - echte exe:
#   A  Einstiegsseiten S013/S040/S057 spielen das Port-Archiv P27F (Revier-Fahrstuhl), Bogen-Bilder entstehen.
#   B  echter Einstieg: ROOM10C0 mit Strom (4,0xF3), vor der Fahrstuhltuer Aktionstaste -> Sequenz P27F S040 vor
#      dem Laden von ROOM1080.
#   C  Ausstieg Revier-Kabine (Leon): ROOM1080 Knopf 2F (sub08) -> Fahrt -> Sequenz P27C S028 VOR room10c0, mit Ton
#      (DOOR27-Tonteil: "Bild 70 Se_on", 1 Se_on).
#   D  Ausstieg Labor-Kabine: ROOM4020 Knopf sub08 -> Sequenz DOOR27 V0 (RE2-Datei) S222 VOR room5000.
#   E  Elza: ROOM1081 Knopf 3F (sub09) -> Sequenz P27C S029 VOR room1121.
cmake_minimum_required(VERSION 3.16)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_fahrstuhl: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r37_fahrstuhl: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37c_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r37c_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(c_lauf _name _out_dbg)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}/bogen")
    re15_start_spiel(_rv 300 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 ${ARGN} "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r37_fahrstuhl[${_name}]: debug.log fehlt (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    file(REMOVE "${WORKDIR}/cap.raw")
    set(${_out_dbg} "${_dbg}" PARENT_SCOPE)
endfunction()

function(c_muss _name _text _muster)
    string(REGEX MATCH "${_muster}" _m "${_text}")
    if(NOT _m)
        message(FATAL_ERROR "r37_fahrstuhl[${_name}]: erwartet /${_muster}/ - fehlt")
    endif()
endfunction()

# Reihenfolge: Text A muss VOR Text B im Protokoll stehen
function(c_vor _name _text _a _b)
    string(FIND "${_text}" "${_a}" _pa)
    string(FIND "${_text}" "${_b}" _pb)
    if(_pa LESS 0 OR _pb LESS 0 OR NOT _pa LESS _pb)
        message(FATAL_ERROR "r37_fahrstuhl[${_name}]: [${_a}] (${_pa}) nicht vor [${_b}] (${_pb})")
    endif()
endfunction()

set(_titel RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)

# A: Seiten
c_lauf(a _dbg RE15_NOAUDIO=1 RE15_TUER_SCHNELL=1 RE15_TUER_SEITE=S013,S040,S057 "RE15_TUER_BOGEN=${_basis}_a/bogen")
foreach(_s "S013 ROOM1040" "S040 ROOM10C0" "S057 ROOM1120")
    c_muss(a "${_dbg}" "\\[tuer-seite\\] ${_s} DOOR27 V0 Spender FF Port-Archiv P27F")
endforeach()
string(REGEX MATCHALL "\\[tuer\\] Sequenz Archiv 2 DOOR27 Port-Archiv P27F Variante 0" _seq "${_dbg}")
list(LENGTH _seq _nseq)
if(NOT _nseq EQUAL 3)
    message(FATAL_ERROR "r37_fahrstuhl[a]: ${_nseq} P27F-Sequenzen statt 3")
endif()
foreach(_s S013 S040 S057)
    if(NOT EXISTS "${_basis}_a/bogen/${_s}_mitte.ppm")
        message(FATAL_ERROR "r37_fahrstuhl[a]: Bogen-Bild ${_s}_mitte.ppm fehlt")
    endif()
endforeach()

# B: echter Einstieg 2F (Spielerlage vor der Tuer, Blick zur Tuer, Aktionstaste)
c_lauf(b _dbg ${_titel} RE15_NOAUDIO=1 RE15_SET_FLAG=4:243 RE15_DEBUG_JUMP=10C0@120 RE15_PLAYER_POS=1500,7000,1024,0
       "RE15_PAD_AT=150:A,160:A,170:A" "RE15_EXIT_AT=60#1080")
c_muss(b "${_dbg}" "\\[tuer\\] Sequenz Archiv 2 DOOR27 Port-Archiv P27F Variante 0 Bit7 0 Tuer 39 Seite S040 T017")
c_vor(b "${_dbg}" "Port-Archiv P27F Variante 0" "PC loaded room1080.rdt")

# C: Ausstieg Revier-Kabine mit Ton
c_lauf(c _dbg ${_titel} RE15_AUDIO_CAP_SYNC=cap.raw RE15_DEBUG_JUMP=1080@120 RE15_FORCE_EVENT=8@200 "RE15_EXIT_AT=400#10C0")
c_muss(c "${_dbg}" "\\[aot\\] DOOR FIRE slot=1 rect=\\(0,0,hw=0,hh=0\\)")
c_muss(c "${_dbg}" "\\[tuer\\] Sequenz Archiv 2 DOOR27 Port-Archiv P27C Variante 0 Bit7 0 Tuer 39 Seite S028 T017")
c_muss(c "${_dbg}" "\\[tuer\\] Bild 70 Se_on Tonkopf-Eintrag 0")
c_muss(c "${_dbg}" "\\[tuer\\] Sequenz fertig: 271 Bilder \\+ 1 Warten, 1 Se_on")
c_vor(c "${_dbg}" "Port-Archiv P27C Variante 0" "PC loaded room10c0.rdt")

# D: Ausstieg Labor-Kabine
c_lauf(d _dbg ${_titel} RE15_NOAUDIO=1 RE15_DEBUG_JUMP=4020@120 RE15_FORCE_EVENT=8@200 "RE15_EXIT_AT=400#5000")
c_muss(d "${_dbg}" "\\[tuer\\] Sequenz Archiv 2 DOOR27 Variante 0 Bit7 0 Tuer 39 Seite S222 T115")
c_vor(d "${_dbg}" "Seite S222 T115" "PC loaded room5000.rdt")

# E: Elza, Revier-Kabine 3F
c_lauf(e _dbg RE15_NOAUDIO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
       RE15_GOTO_ROOM=1081 RE15_FORCE_EVENT=9@150 "RE15_EXIT_AT=200#1121")
c_muss(e "${_dbg}" "Elza-Bit=1")
c_muss(e "${_dbg}" "\\[tuer\\] Sequenz Archiv 2 DOOR27 Port-Archiv P27C Variante 0 Bit7 0 Tuer 39 Seite S029 T018")
c_vor(e "${_dbg}" "Seite S029 T018" "PC loaded room1121.rdt")

file(REMOVE "${_exe_kopie}")
message(STATUS "r37_fahrstuhl: A (Seiten P27F), B (Einstieg 2F), C (Ausstieg 1080 + Ton), D (Ausstieg 4020), E (Elza 1081) bestanden")
