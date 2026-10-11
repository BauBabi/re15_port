# =============================================================================
# FIGURENSCHATTEN GEGEN DIE WANDMASKEN MIT DER ECHTEN EXE (Runde 36, Spur O: ROOM1100).
#
# Dossier analysis/befunde_runde36/O_pri.md §2, Regeln include/re15_pri_schatten_r36.h.
# Nutzer-Befund: "ROOM 1100: Leon wird mit einer Halbtransparenz ueberdeckt mit Boden." Gemessen war
# es der halbtransparente Spielerschatten (ABR2-Quad, FUN_8001b064), den die Wandmasken (Bodenstreifen
# des Zellenpolsters) zublitteten, weil der PC alle Schatten vor allen Masken malte. Zwei Laeufe, Sprung
# nach ROOM1100 Cut 2, Spieler (-15100,-18500) an der Westwand, Framedump Bild 150:
#   A  mit Masken, B  ohne Masken (RE15_NO_PRI=1).
# Im SCHATTENFENSTER des Spielers (Bildrechteck um seine Fuesse, x 40..120 y 185..240 bei Fensterskalierung 1)
# darf sich zwischen A und B kein Pixel um mehr als die Atlas-Rundung aendern (Schwelle 24, s. probe_r36_ppm_diff;
# der Schattenblob subtrahiert bis 96) — die Masken am Wandfuss liegen HINTER dem Schatten (Bucket-Regel) und
# werden vor ihm gezeichnet. Gegenkontrolle: ausserhalb des Fensters (der Zombie hinter der Ecke, Bildrechteck
# x 125..145 y 120..145) MUESSEN sich Pixel aendern, sonst hat Lauf A keine Masken gezeichnet und das Gruen
# waere leer. Vor der Aenderung (alte Zeichenreihenfolge + Zellenpolster) lagen 85 Punkte > 24 im Bild, die
# Diagonale am Fuss davon im Schattenfenster (Dossier O_pri.md §2.2b).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PPM_TOOL=<probe_r36_ppm_diff> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r36_pri_schatten: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_PPM_TOOL OR NOT EXISTS "${RE15_PPM_TOOL}")
    message(FATAL_ERROR "r36_pri_schatten: RE15_PPM_TOOL fehlt: '${RE15_PPM_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r36_pri_schatten: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; local_build.sh killt re15_pc.exe).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r36o_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r36o_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

function(schatten_lauf _name _extra)
    set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/f.ppm" "${WORKDIR}/debug.log")
    re15_start_spiel(_rv 180
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        RE15_DEBUG_JUMP=1100@250 "RE15_PLAYER_POS=-15100,-18500,-1024" RE15_FORCE_CUT=2
        "RE15_FRAMEDUMP=150:f.ppm" "RE15_EXIT_AT=150#1100"
        ${_extra}
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/f.ppm")
        message(FATAL_ERROR "r36_pri_schatten[${_name}]: kein Framedump (exit=${_rv}, ${WORKDIR})")
    endif()
    set(_ppm_${_name} "${WORKDIR}/f.ppm" PARENT_SCOPE)
endfunction()

schatten_lauf(a "")
schatten_lauf(b "RE15_NO_PRI=1")
file(REMOVE "${_exe_kopie}")

# probe_r36_ppm_diff <a> <b> x0 y0 x1 y1 schwelle -> "n=<Pixel mit Kanaldifferenz > schwelle> max=<..>"
function(ppm_diff _out _x0 _y0 _x1 _y1)
    execute_process(COMMAND "${RE15_PPM_TOOL}" "${_ppm_a}" "${_ppm_b}" ${_x0} ${_y0} ${_x1} ${_y1} 24
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _o TIMEOUT 60)
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "r36_pri_schatten: PPM-Werkzeug exit=${_rv}: ${_o}")
    endif()
    string(REGEX MATCH "n=([0-9]+)" _m "${_o}")
    set(${_out} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

ppm_diff(_n_schatten 40 185 120 240)
ppm_diff(_n_zombie   125 120 145 145)
message(STATUS "r36_pri_schatten: geaenderte Pixel Schattenfenster=${_n_schatten} Zombie-Ecke=${_n_zombie}")
if(NOT _n_zombie GREATER 0)
    message(FATAL_ERROR "r36_pri_schatten: Lauf A zeichnet keine Masken (Zombie-Ecke unveraendert) - leeres Gruen")
endif()
if(NOT _n_schatten EQUAL 0)
    message(FATAL_ERROR "r36_pri_schatten: ${_n_schatten} Pixel im Schattenfenster des Spielers von den Wandmasken ueberdeckt")
endif()
message(STATUS "r36_pri_schatten: OK")
