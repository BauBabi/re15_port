cmake_minimum_required(VERSION 3.16)
# =============================================================================
# VORDERGRUND-MASKEN DER STAGES 3..6 MIT DER ECHTEN EXE (Runde 37, Spur P "masken36").
#
# Dossier analysis/befunde_runde37/P_masken36.md. Orchestrator-Auftrag P (Regression 53741fe5): die Generator-Masken
# (R15M ohne eigenen Atlas) wurden seit 2026-09-08 fuer niemanden gezeichnet (fg_atlas=0). Jetzt zeichnet der Lader sie
# mit dem BSS-Atlas des Cuts (re15_pri_atlas_quelle), ohne die Rechtecke, die eine Figur VOR jedem Objekt uebermalen
# (engine/src/masken36_r37.c). Je Stage eine Szene HINTER einem Vordergrund-Objekt und eine DAVOR (Lagen aus dem
# Szenenfinder, an der exe gemessen; Leon per RE15_PLAYER_POS, Kamera per RE15_FORCE_CUT, Bild 120 im Raum):
#   A  mit Masken, Leon an der Lage
#   B  ohne Masken (RE15_NO_PRI=1) - so sah das Bild vor Spur P aus (gemessen: vorher-exe == B in allen 18 Szenen)
#   C  ohne Masken, Leon ausserhalb des Regionsvierecks des Cuts (Akteur-Cull FUN_8002c18c) = Bild ohne Figur
# probe_r37_masken36_ppm im Figurfenster: S = Figurpunkte (|B - C| > 24), H = davon von den Masken uebermalt (|A - B| > 24).
#   HINTER: S > 50 und H >= S/2 (gemessen 57..91 %), DAVOR: S > 50 und H == 0. Dazu muss das Log fuer den Cut
#   "masks=<n> fg_atlas=1" (n > 0) und die Zeile der Spur P zeigen (vorher: fg_atlas=0).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PPM_TOOL=<probe_r37_masken36_ppm> -DWORKDIR=<dir> -DSTUFE=<3..6> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_masken36: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_PPM_TOOL OR NOT EXISTS "${RE15_PPM_TOOL}")
    message(FATAL_ERROR "r37_masken36: RE15_PPM_TOOL fehlt: '${RE15_PPM_TOOL}'")
endif()
if(NOT WORKDIR OR NOT STUFE)
    message(FATAL_ERROR "r37_masken36: WORKDIR/STUFE fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Szenen: raum cut  hinter_x hinter_z  davor_cut davor_x davor_z  ohne_x ohne_z (davor_ohne_x davor_ohne_z)  Fenster hinter / davor
if(STUFE EQUAL 3)
    set(_raum 3050)
    set(_cut 2)
    set(_h -22000 -20768)
    set(_vcut 2)
    set(_v -20400 -14368)
    set(_c -23996 1523)
    set(_vc -23996 1523)
    set(_wh -9 122 55 206)
    set(_wv 222 127 277 213)
elseif(STUFE EQUAL 4)
    set(_raum 4040)
    set(_cut 4)
    set(_h -23200 -922)
    set(_vcut 7)
    set(_v -7200 -20022)
    set(_c -26400 17150)
    set(_vc -26400 17150)
    set(_wh 172 65 194 123)
    set(_wv 143 84 177 190)
elseif(STUFE EQUAL 5)
    set(_raum 5020)
    set(_cut 9)
    set(_h -13133 -21625)
    set(_vcut 9)
    set(_v -12133 -13325)
    set(_c -10000 -23150)
    set(_vc -10000 -23150)
    set(_wh 114 116 139 190)
    set(_wv 118 3 195 272)
elseif(STUFE EQUAL 6)
    set(_raum 6010)
    set(_cut 12)
    set(_h -18612 -7920)
    set(_vcut 12)
    set(_v -14412 -5720)
    set(_c -27015 -25588)
    set(_vc -27015 -25588)
    set(_wh 187 81 217 176)
    set(_wv 246 27 318 284)
else()
    message(FATAL_ERROR "r37_masken36: STUFE ${STUFE} unbekannt")
endif()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; local_build.sh killt re15_pc.exe). Endung wie die Quelle.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37p${STUFE}_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r37p${STUFE}_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(_basis_wd "${WORKDIR}")

function(szene_lauf _name _cutnr _x _z)
    set(WORKDIR "${_basis_wd}/lauf_${_lauf_id}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/f.ppm" "${WORKDIR}/debug.log")
    re15_start_spiel(_rv 180
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_AUDIODRIVER=dummy
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        "RE15_DEBUG_JUMP=${_raum}@250" "RE15_PLAYER_POS=${_x},${_z},0" "RE15_FORCE_CUT=${_cutnr}"
        "RE15_FRAMEDUMP=120:f.ppm" "RE15_EXIT_AT=120#${_raum}"
        ${ARGN}
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/f.ppm")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r37_masken36[${STUFE} ${_name}]: kein Framedump (exit=${_rv}, ${WORKDIR})")
    endif()
    set(_ppm_${_name} "${WORKDIR}/f.ppm" PARENT_SCOPE)
    set(_log_${_name} "${WORKDIR}/debug.log" PARENT_SCOPE)
endfunction()

list(GET _h 0 _hx)
list(GET _h 1 _hz)
list(GET _v 0 _vx)
list(GET _v 1 _vz)
list(GET _c 0 _cx)
list(GET _c 1 _cz)
list(GET _vc 0 _vcx)
list(GET _vc 1 _vcz)
szene_lauf(h_a ${_cut}  ${_hx} ${_hz})
szene_lauf(h_b ${_cut}  ${_hx} ${_hz} RE15_NO_PRI=1)
szene_lauf(h_c ${_cut}  ${_cx} ${_cz} RE15_NO_PRI=1)
szene_lauf(v_a ${_vcut} ${_vx} ${_vz})
szene_lauf(v_b ${_vcut} ${_vx} ${_vz} RE15_NO_PRI=1)
if(_vcut EQUAL _cut AND _vcx EQUAL _cx AND _vcz EQUAL _cz)
    set(_ppm_v_c "${_ppm_h_c}")
else()
    szene_lauf(v_c ${_vcut} ${_vcx} ${_vcz} RE15_NO_PRI=1)
endif()
file(REMOVE "${_exe_kopie}")

# Log: der Cut zeichnet Masken mit Atlas, ueber den Weg der Spur P
function(log_pruefen _name _cutnr)
    file(READ "${_log_${_name}}" _log)
    string(REGEX MATCH "\\[pri\\] cut=${_cutnr} pri_offset=0x[0-9A-F]+ masks=([0-9]+) fg_atlas=([0-9])" _m "${_log}")
    if(NOT _m OR CMAKE_MATCH_1 EQUAL 0 OR NOT CMAKE_MATCH_2 EQUAL 1)
        message(FATAL_ERROR "r37_masken36[${STUFE} ${_name}]: ROOM${_raum} Cut ${_cutnr} ohne gezeichnete Masken: '${_m}'")
    endif()
    string(FIND "${_log}" "Cut ${_cutnr}: Port-Masken ohne eigenen Atlas -> BSS-Atlas des Cuts (Runde 37 Spur P)" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r37_masken36[${STUFE} ${_name}]: Logzeile der Spur P fehlt")
    endif()
    message(STATUS "r37_masken36[${STUFE} ${_name}]: ${_m}")
endfunction()
log_pruefen(h_a ${_cut})
log_pruefen(v_a ${_vcut})

function(sh_messen _out_s _out_h _a _b _c)
    execute_process(COMMAND "${RE15_PPM_TOOL}" "${_a}" "${_b}" "${_c}" ${ARGN} 24
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _o TIMEOUT 60)
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "r37_masken36: PPM-Werkzeug exit=${_rv}: ${_o}")
    endif()
    string(REGEX MATCH "S=([0-9]+) H=([0-9]+)" _m "${_o}")
    set(${_out_s} "${CMAKE_MATCH_1}" PARENT_SCOPE)
    set(${_out_h} "${CMAKE_MATCH_2}" PARENT_SCOPE)
endfunction()
sh_messen(_hs _hh "${_ppm_h_a}" "${_ppm_h_b}" "${_ppm_h_c}" ${_wh})
sh_messen(_vs _vh "${_ppm_v_a}" "${_ppm_v_b}" "${_ppm_v_c}" ${_wv})
message(STATUS "r37_masken36[Stage ${STUFE}] ROOM${_raum}: HINTER Cut ${_cut} (${_hx},${_hz}) Figurpunkte ${_hs}, verdeckt ${_hh} | "
               "DAVOR Cut ${_vcut} (${_vx},${_vz}) Figurpunkte ${_vs}, verdeckt ${_vh}")
if(NOT _hs GREATER 50 OR NOT _vs GREATER 50)
    message(FATAL_ERROR "r37_masken36[${STUFE}]: Leon ist im Figurfenster nicht zu sehen - leeres Gruen")
endif()
math(EXPR _h2 "${_hh} * 2")
if(_h2 LESS _hs)
    message(FATAL_ERROR "r37_masken36[${STUFE}]: HINTER dem Objekt nur ${_hh} von ${_hs} Figurpunkten verdeckt (Soll >= Haelfte)")
endif()
if(NOT _vh EQUAL 0)
    message(FATAL_ERROR "r37_masken36[${STUFE}]: VOR dem Objekt ${_vh} Figurpunkte uebermalt")
endif()
message(STATUS "r37_masken36[${STUFE}]: OK")
