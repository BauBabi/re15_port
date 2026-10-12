cmake_minimum_required(VERSION 3.16)
# =============================================================================
# ZOMBIE HINTER DER WESTWAND MIT DER ECHTEN EXE (Runde 37, Spur A: ROOM1100 Cut 1).
#
# Dossier analysis/befunde_runde37/A_pri1100.md. Nutzer (AUFTRAG.md #1): "Room 1100 das Problem ist immer noch
# da, siehe Zombie auf den F9 Marker." Marke befund_1100_F892_marke1.bmp: Leon (-14914,0,-25645) rot 4488,
# Cut 1, Zombie 1 liegt an der Suedecke (-16157,0,-23876) rot 1024 — Kopf und Arm stecken HINTER der Westwand
# (der Kopf in der Zelle Suedblock) und wurden ueber die Wand gemalt (Kachel-Maximum t=161, Schluessel 10378).
# Marke befund_1100_F240_marke1.bmp: derselbe Zombie an seiner Spawnlage (-16453,0,-23817), nur der Kopf schwebte.
#
# Je Szene drei Laeufe (Sprung nach ROOM1100, RE15_FORCE_CUT=1, Framedump Bild 150):
#   A  mit Masken, Zombie 1 an der Lage der Marke
#   B  ohne Masken (RE15_NO_PRI=1), sonst wie A
#   C  mit Masken, Zombie 1 hinter die Kamera gesetzt (RE15_GEGNER_POS ...,-14000,-11000 — z groesser als der
#      Kameraort z=-13608, also nicht im Bild) = das Wandfenster OHNE Zombie
# Im WANDFENSTER rechts der Ecke (x 168..191, y 126..149; ganz in der Silhouette, 576 von 576 Maskenpunkten)
#   A gegen C: 0 Punkte mit Kanaldifferenz > 24 (Schwelle wie probe_r36_ppm_diff: Atlas-Rundung < 8)
#             -> kein Teil des Zombies wird ueber die Wand gemalt
#   B gegen C: > 0 Punkte -> der Zombie liegt wirklich hinter diesem Wandstueck (sonst waere das Gruen leer)
# Vorher (v0.8.24-Masken): A gegen C im Fenster = Kopf und Arm (Dossier 1.2: 80 bzw. 82 Leckpunkte).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PPM_TOOL=<probe_r36_ppm_diff> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_pri1100: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_PPM_TOOL OR NOT EXISTS "${RE15_PPM_TOOL}")
    message(FATAL_ERROR "r37_pri1100: RE15_PPM_TOOL fehlt: '${RE15_PPM_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r37_pri1100: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; local_build.sh killt re15_pc.exe). Endung wie
# die Quelle (Linux ohne .exe).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37a_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r37a_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(_basis_wd "${WORKDIR}")

function(marke_lauf _name)
    set(WORKDIR "${_basis_wd}/lauf_${_lauf_id}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/f.ppm" "${WORKDIR}/debug.log")
    re15_start_spiel(_rv 180
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_AUDIODRIVER=dummy
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        RE15_DEBUG_JUMP=1100@250 "RE15_PLAYER_POS=-14914,-25645,392" RE15_FORCE_CUT=1
        "RE15_FRAMEDUMP=150:f.ppm" "RE15_EXIT_AT=150#1100"
        ${ARGN}
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/f.ppm")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r37_pri1100[${_name}]: kein Framedump (exit=${_rv}, ${WORKDIR})")
    endif()
    set(_ppm_${_name} "${WORKDIR}/f.ppm" PARENT_SCOPE)
endfunction()

set(_f892 "RE15_GEGNER_POS=1,-16157,-23876,1024@5#1100")
set(_weg  "RE15_GEGNER_POS=1,-14000,-11000,1024@5#1100")
marke_lauf(f892_a ${_f892})
marke_lauf(f892_b ${_f892} RE15_NO_PRI=1)
marke_lauf(spawn_a)
marke_lauf(spawn_b RE15_NO_PRI=1)
marke_lauf(ohne_c ${_weg})
file(REMOVE "${_exe_kopie}")

# probe_r36_ppm_diff <a> <b> x0 y0 x1 y1 schwelle -> "n=<Pixel mit Kanaldifferenz > schwelle> max=<..>"
function(ppm_diff _out _a _b)
    execute_process(COMMAND "${RE15_PPM_TOOL}" "${_a}" "${_b}" 168 126 192 150 24
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _o TIMEOUT 60)
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "r37_pri1100: PPM-Werkzeug exit=${_rv}: ${_o}")
    endif()
    string(REGEX MATCH "n=([0-9]+)" _m "${_o}")
    set(${_out} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

ppm_diff(_n_f892_a   "${_ppm_f892_a}"  "${_ppm_ohne_c}")
ppm_diff(_n_f892_b   "${_ppm_f892_b}"  "${_ppm_ohne_c}")
ppm_diff(_n_spawn_a  "${_ppm_spawn_a}" "${_ppm_ohne_c}")
ppm_diff(_n_spawn_b  "${_ppm_spawn_b}" "${_ppm_ohne_c}")
message(STATUS "r37_pri1100: Wandfenster x168..191 y126..149, Punkte > 24 gegen 'ohne Zombie': "
               "F892 mit Masken=${_n_f892_a} ohne Masken=${_n_f892_b} | Spawnlage (F240) mit Masken=${_n_spawn_a} ohne Masken=${_n_spawn_b}")
if(NOT _n_f892_b GREATER 0 OR NOT _n_spawn_b GREATER 0)
    message(FATAL_ERROR "r37_pri1100: ohne Masken liegt kein Zombie im Wandfenster - leeres Gruen")
endif()
if(NOT _n_f892_a EQUAL 0)
    message(FATAL_ERROR "r37_pri1100: Szene F892: ${_n_f892_a} Punkte von Zombie 1 ueber der Westwand gezeichnet")
endif()
if(NOT _n_spawn_a EQUAL 0)
    message(FATAL_ERROR "r37_pri1100: Szene F240 (Spawnlage): ${_n_spawn_a} Punkte von Zombie 1 ueber der Westwand gezeichnet")
endif()
message(STATUS "r37_pri1100: OK")
