# =============================================================================
# HANGAR-/LABOR-BIRKIN, Runde 41 Spur SF (analysis/befunde_runde41/SF_sichel.md).
#
# Nutzer 2026-11-02 (NUTZER_ENTSCHEIDE E17): der Birkin im Hangar 50A0 und im Labor 50E0 ist derselbe; das Modell
# EMD/EM30_HANGAR.EMD (seit v0.8.30 Sichel-Form, Zweig r41/sichel_v8: V8, tools/r41_sichel/v8_kette.sh; vorher V7 v7_kette.sh bzw.
# tools/birkin_formen.py) traegt eine Schwanzkette (Knochen 16-22); V8 hat weiter 23 Meshes / 23 Knochen / 21 Clips (riegel_v8 S8). Je ein kurzer Lauf mit
# Debug-Sprung, kein Messhaken ausser der Kamera in 50A0 (RE15_FORCE_CUT=4: Cut 12 der Szene hat ein Ankerviereck
# x -25000..-21000, in dem der Birkin nicht steht; Cut 4 zeigt ihn). Geprueft aus debug.log / enemy_dbg.log:
#   1  50A0 und 50E0: "EM30: Ersatzmodell EMD/EM30_HANGAR.EMD" und "EM30 loaded: 23 meshes, 23 bones, 21 clips"
#   2  50A0: der Zeichner zeichnet 23 Teile (Bones 23, Meshes 23) - kein Rohr-Tausch, Schwanzglieder mit eigenem Knochen
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_sichel_form.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_sichel_form: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_sichel_form: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41sichel_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41sichel_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_f "")
function(ein_lauf _raum _pos _extra)
    set(WORKDIR "${_wurzel}/${_raum}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 180
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=${_pos}" RE15_ENEMY_DBG=1 ${_extra}
        "RE15_EXIT_AT=700#${_raum}"
        "${_exe_kopie}")
    set(_g "")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        set(_f "${_f}\n ${_raum}: kein debug.log (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    if(NOT _dbg MATCHES "EM30: Ersatzmodell EMD/EM30_HANGAR.EMD \\(Raum ${_raum}\\)")
        string(APPEND _g "\n ${_raum}: Ersatzmodell EM30_HANGAR nicht gewaehlt")
    endif()
    if(NOT _dbg MATCHES "EM30 loaded: 23 meshes, 23 bones, 21 clips")
        string(APPEND _g "\n ${_raum}: EM30_HANGAR nicht mit Schwanzkette geladen (23 Meshes/Knochen, 21 Clips)")
    endif()
    if(_raum STREQUAL "50A0")
        set(_ed "")
        if(EXISTS "${WORKDIR}/enemy_dbg.log")
            file(READ "${WORKDIR}/enemy_dbg.log" _ed)
        endif()
        if(NOT _ed MATCHES "ZEICHNE Typ 0x30: 23 Teile \\(Bones 23, Meshes 23\\)")
            string(APPEND _g "\n ${_raum}: Zeichner zeichnet nicht 23 Teile des 0x30 (enemy_dbg.log)")
        endif()
    endif()
    message(STATUS "r41_sichel_form ${_raum}: exit=${_rv}${_g}")
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

ein_lauf(50A0 "-3000,-15000,2048,0" "RE15_FORCE_CUT=4")
ein_lauf(50E0 "-25450,-14700,0,0" "")

file(REMOVE "${_exe_kopie}")
if(_f)
    message(FATAL_ERROR "r41_sichel_form:${_f}")
endif()
message(STATUS "r41_sichel_form: OK (50A0 + 50E0, Laufverzeichnis ${_wurzel})")
