cmake_minimum_required(VERSION 3.16)
# =============================================================================
# LEON IM STREIFEN JENSEITS DER GEMALTEN ECKE MIT DER ECHTEN EXE (Runde 39, Spur L: ROOM1100 Cut 1).
#
# Dossier analysis/befunde_runde39/L_optik.md (Punkt 1). Nutzer (AUFTRAG.md): "Ich habe noch mal einen Marker mit F9
# gemacht. Leon ist immer noch vom Boden ueberdeckt an den einen Streifen." Marke befund_1100_F741_marke1.bmp: Leon
# (-16007,0,-23658) rot 3023, Cut 1, im Quergang hinter der Ecke der Westwand. Die Port-Masken der Westwand (Quader bis
# zur SCA-Zelle z -23200) reichten 5..7 Bildspalten ueber die GEMALTE Ecke (Spalte 173 bzw. 171 im Sockelband) hinaus und
# malten dort Boden/Rueckwand des hinteren Gangs ueber ihn. Seit optik_r39.c werden sie beim Laden an der gemalten Wand
# gekappt.
#
# Drei Laeufe (Sprung nach ROOM1100, RE15_FORCE_CUT=1, Zombie 1 hinter die Kamera, Framedump Bild 150):
#   A  mit Masken, Leon an der Lage der Marke
#   B  ohne Masken (RE15_NO_PRI=1), sonst wie A
#   C  mit Masken, Leon hinter die Kamera gesetzt (-14000,-11000; Kameraort z -13608) = die Szene OHNE Leon
# Fenster (Zeilen 40..170): STREIFEN x 166..172 (jenseits der gemalten Ecke), WAND x 173..180.
#   A gegen C im Streifen > 0  -> Leon wird dort GEZEICHNET (sonst waere das Gruen leer)
#   A gegen B im Streifen <= 1 -> die Masken malen dort nichts ueber ihn (gemessen 1 = Spalte 171 im Sockelband, in dem
#                                 die Wand ab 171 beginnt; vorher 134 Punkte)
#   A gegen B in der Wand > 0  -> die Masken sind an (sie decken ihn hinter der Wand; gemessen 95)
#   A gegen C in der Wand == 0 -> hinter der gemalten Wand bleibt er vollstaendig verdeckt
# Schwelle 24 wie probe_r36_ppm_diff (Atlas-Rundung < 8).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PPM_TOOL=<probe_r36_ppm_diff> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r39_optik_marke: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_PPM_TOOL OR NOT EXISTS "${RE15_PPM_TOOL}")
    message(FATAL_ERROR "r39_optik_marke: RE15_PPM_TOOL fehlt: '${RE15_PPM_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r39_optik_marke: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; Praefix nur DIESES Tests, kein Glob ueber fremde
# Kopien). Endung wie die Quelle (Linux ohne .exe).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r39l_marke_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r39l_marke_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(_basis_wd "${WORKDIR}")

function(marke_lauf _name _pos)
    set(WORKDIR "${_basis_wd}/lauf_${_lauf_id}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/f.ppm" "${WORKDIR}/debug.log")
    re15_start_spiel(_rv 180
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_AUDIODRIVER=dummy
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        RE15_DEBUG_JUMP=1100@250 "RE15_PLAYER_POS=${_pos}" RE15_FORCE_CUT=1
        "RE15_GEGNER_POS=1,-14000,-11000,1024@5#1100"
        "RE15_FRAMEDUMP=150:f.ppm" "RE15_EXIT_AT=150#1100"
        ${ARGN}
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/f.ppm")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r39_optik_marke[${_name}]: kein Framedump (exit=${_rv}, ${WORKDIR})")
    endif()
    set(_ppm_${_name} "${WORKDIR}/f.ppm" PARENT_SCOPE)
    set(_dbg_${_name} "${WORKDIR}/debug.log" PARENT_SCOPE)
endfunction()

marke_lauf(a "-16007,-23658,3023")
marke_lauf(b "-16007,-23658,3023" RE15_NO_PRI=1)
marke_lauf(c "-14000,-11000,3023")
file(REMOVE "${_exe_kopie}")

# Der Haken muss gelaufen sein (sonst misst der Test den alten Stand).
file(READ "${_dbg_a}" _dbg)
string(FIND "${_dbg}" "[optik_r39] ROOM1100 Cut 1: Westwand an der gemalten Ecke gekappt" _p)
if(_p LESS 0)
    message(FATAL_ERROR "r39_optik_marke: Lader-Zeile '[optik_r39] ROOM1100 Cut 1 ...' fehlt im debug.log (${_dbg_a})")
endif()

# probe_r36_ppm_diff <a> <b> x0 y0 x1 y1 schwelle -> "n=<Pixel mit Kanaldifferenz > schwelle> max=<..>"
function(ppm_diff _out _a _b _x0 _y0 _x1 _y1)
    execute_process(COMMAND "${RE15_PPM_TOOL}" "${_a}" "${_b}" ${_x0} ${_y0} ${_x1} ${_y1} 24
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _o TIMEOUT 60)
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "r39_optik_marke: PPM-Werkzeug exit=${_rv}: ${_o}")
    endif()
    string(REGEX MATCH "n=([0-9]+)" _m "${_o}")
    set(${_out} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

ppm_diff(_s_ac "${_ppm_a}" "${_ppm_c}" 166 40 173 171)
ppm_diff(_s_ab "${_ppm_a}" "${_ppm_b}" 166 40 173 171)
ppm_diff(_w_ab "${_ppm_a}" "${_ppm_b}" 173 40 181 171)
ppm_diff(_w_ac "${_ppm_a}" "${_ppm_c}" 173 40 181 171)
message(STATUS "r39_optik_marke: Streifen x166..172: Leon gezeichnet (A/C)=${_s_ac}, Maske ueber Leon (A/B)=${_s_ab} | "
               "Wand x173..180: Maske an (A/B)=${_w_ab}, Leon sichtbar (A/C)=${_w_ac}")
if(NOT _s_ac GREATER 0)
    message(FATAL_ERROR "r39_optik_marke: Leon liegt nicht im Streifen (A/C = 0) - leeres Gruen")
endif()
if(NOT _w_ab GREATER 0)
    message(FATAL_ERROR "r39_optik_marke: keine Maske aktiv in der Wand (A/B = 0) - leeres Gruen")
endif()
if(_s_ab GREATER 1)
    message(FATAL_ERROR "r39_optik_marke: ${_s_ab} Punkte der Westwand-Masken liegen jenseits der gemalten Ecke ueber Leon (Soll <= 1)")
endif()
if(NOT _w_ac EQUAL 0)
    message(FATAL_ERROR "r39_optik_marke: ${_w_ac} Punkte von Leon durch die gemalte Wand sichtbar (Soll 0)")
endif()
message(STATUS "r39_optik_marke: OK")
