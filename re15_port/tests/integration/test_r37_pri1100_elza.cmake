cmake_minimum_required(VERSION 3.16)
# =============================================================================
# ELZAS ROOM1101 AUF DEM ECHTEN TUERWEG (Runde 37, Spur A, Nachbesserung 1 — Abnahme A#0 M1).
#
# Dossier analysis/befunde_runde37/A_pri1100.md §8.1. Elzas ROOM1101.RDT ist bytegleich zu ROOM1100.RDT, Zombie 1
# liegt an derselben Ecke (Datei 0x9FE: -16453,0,-23817), der Raum liegt auf Elzas Pflichtweg 10D1 -> 1101. Der Port
# suchte die Masken mit der VOLLEN Raum-Id (MASKS/ROOM1101.*), die es nicht gibt: "[pri] cut=1 pri_offset=0x60C
# masks=0" und Kopf, Arm und Oberkoerper von Zombie 1 lagen ueber der Westwand (Wandfenster 163..168 Punkte).
# Jetzt: re15_pri_masken_raum(0x1101) = 0x1100 (Kamerasaetze + Hintergrund je Cut gleich, VERTRAG.md 1.7).
#
# Weg wie die Abnahme: als Elza (RE15_TITLE_SHOT_CHAR=1) Sprung nach ROOM10D1, in den Tuer-AOT
# (Door_aot_set @ROOM10D0.RDT+0x11D6 -> Raum 0x10 (-20975,0,-25000)), Quadrat, in 1101 nach Osten (U2.5); die Kamera
# wechselt SELBST auf Cut 1 (kein FORCE_CUT, kein Sprung in den Raum). Bild 300 in ROOM1101, drei Laeufe:
#   A  mit Masken
#   B  ohne Masken (RE15_NO_PRI=1)                         -> der Zombie liegt wirklich im Wandfenster
#   C  mit Masken, Zombie 1 hinter die Kamera gesetzt (RE15_GEGNER_POS=1,-14000,-11000,1024@5#1101)
# Pruefung: A laedt die Leon-Masken ("[pri] ROOM1101: Masken der Leon-Variante MASKS/ROOM1100.MSK", Cut 1 mit
# Masken und Atlas), im Wandfenster x 168..191 y 126..149 ist A gleich C (0 Punkte > 24) und B nicht (> 0).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PPM_TOOL=<probe_r36_ppm_diff> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_pri1100_elza: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_PPM_TOOL OR NOT EXISTS "${RE15_PPM_TOOL}")
    message(FATAL_ERROR "r37_pri1100_elza: RE15_PPM_TOOL fehlt: '${RE15_PPM_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r37_pri1100_elza: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis). Eigenes Praefix, damit integration_r37_pri1100_marke
# (raeumt re15_pc_r37a_haken_* auf) parallel laufen kann.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37a_elza_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r37a_elza_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(_basis_wd "${WORKDIR}")

function(elza_lauf _name)
    set(WORKDIR "${_basis_wd}/lauf_${_lauf_id}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/f.ppm" "${WORKDIR}/debug.log")
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_AUDIODRIVER=dummy
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_TITLE_SHOT_CHAR=1
        RE15_DEBUG_JUMP=10D0@250 "RE15_PLAYER_POS=-19300,24000,2048" RE15_PRESS=square@60
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=200 "RE15_INPUT_SCRIPT=U2.5,W10"
        "RE15_FRAMEDUMP=300:f.ppm" "RE15_EXIT_AT=300#1101"
        ${ARGN}
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/f.ppm")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r37_pri1100_elza[${_name}]: kein Framedump (exit=${_rv}, ${WORKDIR})")
    endif()
    set(_ppm_${_name} "${WORKDIR}/f.ppm" PARENT_SCOPE)
    set(_log_${_name} "${WORKDIR}/debug.log" PARENT_SCOPE)
endfunction()

elza_lauf(a)
elza_lauf(b RE15_NO_PRI=1)
elza_lauf(c "RE15_GEGNER_POS=1,-14000,-11000,1024@5#1101")
file(REMOVE "${_exe_kopie}")

# Der Lauf A muss als Elza in ROOM1101 stehen und dort die Leon-Masken geladen haben.
if(NOT EXISTS "${_log_a}")
    message(FATAL_ERROR "r37_pri1100_elza: debug.log von Lauf A fehlt (${_log_a})")
endif()
file(READ "${_log_a}" _log)
if(NOT _log MATCHES "PC loaded room1101\\.rdt")
    message(FATAL_ERROR "r37_pri1100_elza: Lauf A hat ROOM1101 nicht geladen (Elza-Tuerweg 10D1 -> 1101)")
endif()
if(NOT _log MATCHES "\\[pri\\] ROOM1101: Masken der Leon-Variante MASKS/ROOM1100\\.MSK")
    message(FATAL_ERROR "r37_pri1100_elza: ROOM1101 laedt die Masken der Leon-Variante nicht (re15_pri_masken_raum)")
endif()
string(REGEX MATCH "\\[pri\\] cut=1 pri_offset=0x60C masks=([0-9]+) fg_atlas=1" _m "${_log}")
if(NOT _m OR NOT CMAKE_MATCH_1 GREATER 0)
    message(FATAL_ERROR "r37_pri1100_elza: Cut 1 von ROOM1101 ohne Masken/Atlas (vorher: masks=0 fg_atlas=0)")
endif()
set(_n_masken "${CMAKE_MATCH_1}")

# probe_r36_ppm_diff <a> <b> x0 y0 x1 y1 schwelle -> "n=<Pixel mit Kanaldifferenz > schwelle> max=<..>"
function(ppm_diff _out _a _b)
    execute_process(COMMAND "${RE15_PPM_TOOL}" "${_a}" "${_b}" 168 126 192 150 24
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _o TIMEOUT 60)
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "r37_pri1100_elza: PPM-Werkzeug exit=${_rv}: ${_o}")
    endif()
    string(REGEX MATCH "n=([0-9]+)" _m "${_o}")
    set(${_out} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

ppm_diff(_n_a "${_ppm_a}" "${_ppm_c}")
ppm_diff(_n_b "${_ppm_b}" "${_ppm_c}")
message(STATUS "r37_pri1100_elza: ROOM1101 Cut 1 masks=${_n_masken}; Wandfenster x168..191 y126..149, Punkte > 24 "
               "gegen 'ohne Zombie': mit Masken=${_n_a} ohne Masken=${_n_b}")
if(NOT _n_b GREATER 0)
    message(FATAL_ERROR "r37_pri1100_elza: ohne Masken liegt kein Zombie im Wandfenster - leeres Gruen")
endif()
if(NOT _n_a EQUAL 0)
    message(FATAL_ERROR "r37_pri1100_elza: ${_n_a} Punkte von Zombie 1 ueber der Westwand gezeichnet (Elza, ROOM1101)")
endif()
message(STATUS "r37_pri1100_elza: OK")
