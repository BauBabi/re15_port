cmake_minimum_required(VERSION 3.16)
# =============================================================================
# DIE 53 NUTZER-MASKEN 11A0 + STAGE2 AN DER ECHTEN EXE (Runde 41, Spur M "masken36", Punkt 2).
#
# Nutzer: "Ich habe diverse neue PRIs fuer STAGE2 erstellt. die sollen alle sauber eingebaut werden!" (Masken aus den
# Freistellungen in master 5924939d, Lage/Ebene be59e679). Dossier analysis/befunde_runde41/M_masken36.md Punkt 2.
# Je Szene zwei Framedumps (Bild 120, RE15_WINDOW_SCALE=1, Leon per RE15_DEBUG_JUMP + RE15_PLAYER_POS, RE15_FORCE_CUT):
#   A  mit Masken, B  ohne Masken (RE15_NO_PRI=1); C = Hintergrund des Cuts aus probe_bg_dump (der figurlose Spielrahmen
#   ist bitgleich zu diesem Bild, gemessen ROOM3050 Cut 2: 0 Abweichung). probe_r37_masken36_ppm im Figurfenster:
#   S = Figurpunkte |B - C| > 24, H = davon uebermalt |A - B| > 24.
#   VOR (Leon an der begehbaren Stelle, die die Maskenflaeche am staerksten VOR der Standlinie der Freistellung
#        ueberdeckt): S > 50 und H == 0.
#   HINTER (staerkste Ueberdeckung HINTER der Standlinie): S > 50 und H >= S/2.
#   KURBEL ROOM2020 Cut 3 (Pos_set @0x0A8C (3650,-1800,-2925)): Leon steht HINTER der Suedreling (Riegel
#        unit_r41_masken36_kurbel: Sichtlinie kreuzt die solide Zelle x -400..13100 z -2550..-2050), die Reling-Maske
#        verdeckt seine Beine: S > 50 und H > 0 (gemessen 86 von 779).
# Dazu muss das Log fuer den Cut "masks=<n> fg_atlas=1" (n > 0) zeigen.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PPM_TOOL=<probe_r37_masken36_ppm> -DRE15_BG_TOOL=<probe_bg_dump>
#               -DCD_ROOT=<shared_assets/PSX> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
foreach(_v RE15_PC_EXE RE15_PPM_TOOL RE15_BG_TOOL CD_ROOT)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r41_masken36_stage2: ${_v} fehlt: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_masken36_stage2: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41m2_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41m2_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(_basis_wd "${WORKDIR}")

# Hintergruende der Stages 1 und 2 (C-Bilder)
set(_bg "${_basis_wd}/bg_${_lauf_id}")
file(MAKE_DIRECTORY "${_bg}")
foreach(_st 1 2)
    execute_process(COMMAND "${RE15_BG_TOOL}" "${CD_ROOT}" "${_bg}" ${_st} RESULT_VARIABLE _rv OUTPUT_QUIET ERROR_QUIET TIMEOUT 300)
    if(NOT _rv EQUAL 0)
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r41_masken36_stage2: probe_bg_dump Stage ${_st} exit=${_rv}")
    endif()
endforeach()

# Szenen: name raum(hex) cut x z band art(V/H/K) fenster x0 y0 x1 y1 [E = Elza]  (Lagen/Fenster: Dossier Punkt 2)
include("${CMAKE_CURRENT_LIST_DIR}/test_r41_masken36_szenen.cmake")

set(_fehler "")
set(_n 0)
foreach(_sz IN LISTS R41M_SZENEN)
    string(REPLACE " " ";" _f "${_sz}")
    list(GET _f 0 _name)
    list(GET _f 1 _raum)
    list(GET _f 2 _cut)
    list(GET _f 3 _x)
    list(GET _f 4 _z)
    list(GET _f 5 _band)
    list(GET _f 6 _art)
    list(GET _f 7 _x0)
    list(GET _f 8 _y0)
    list(GET _f 9 _x1)
    list(GET _f 10 _y1)
    list(LENGTH _f _fl)
    set(_figur "")
    set(_zr "${_raum}")
    if(_fl GREATER 11)
        list(GET _f 11 _wer)
        if(_wer STREQUAL "E")     # Elza: der Sprung laedt ihre Raumvariante ROOMxxx1 (Zwilling-Masken, re15_pri_masken_raum)
            set(_figur "RE15_TITLE_SHOT_CHAR=1")
            string(REGEX REPLACE ".$" "1" _zr "${_raum}")
        endif()
    endif()
    set(_ppm "")
    foreach(_mit a b)
        set(WORKDIR "${_basis_wd}/lauf_${_lauf_id}_${_name}_${_mit}")
        file(MAKE_DIRECTORY "${WORKDIR}")
        file(REMOVE "${WORKDIR}/f.ppm" "${WORKDIR}/debug.log")
        set(_extra "")
        if(_mit STREQUAL "b")
            set(_extra "RE15_NO_PRI=1")
        endif()
        re15_start_spiel(_rv 180
            RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_AUDIODRIVER=dummy
            RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
            "RE15_DEBUG_JUMP=${_raum}@250" "RE15_PLAYER_POS=${_x},${_z},0,${_band}" "RE15_FORCE_CUT=${_cut}"
            "RE15_FRAMEDUMP=120:f.ppm" "RE15_EXIT_AT=120#${_zr}"
            ${_extra} ${_figur}
            "${_exe_kopie}")
        if(NOT EXISTS "${WORKDIR}/f.ppm")
            file(REMOVE "${_exe_kopie}")
            message(FATAL_ERROR "r41_masken36_stage2[${_name} ${_mit}]: kein Framedump (exit=${_rv}, ${WORKDIR})")
        endif()
        list(APPEND _ppm "${WORKDIR}/f.ppm")
        if(_mit STREQUAL "a")
            file(READ "${WORKDIR}/debug.log" _log)
            string(TOLOWER "loaded room${_zr}.rdt" _zl)
            string(FIND "${_log}" "${_zl}" _zp)
            if(_zp LESS 0)
                list(APPEND _fehler "${_name}: ROOM${_zr} nicht geladen")
            else()
                string(SUBSTRING "${_log}" ${_zp} -1 _log)
                string(REGEX MATCH "\\[pri\\] cut=${_cut} pri_offset=0x[0-9A-F]+ masks=([0-9]+) fg_atlas=([0-9])" _m "${_log}")
                if(NOT _m OR CMAKE_MATCH_1 EQUAL 0 OR NOT CMAKE_MATCH_2 EQUAL 1)
                    list(APPEND _fehler "${_name}: Cut ${_cut} ohne gezeichnete Masken '${_m}'")
                endif()
            endif()
        endif()
    endforeach()
    list(GET _ppm 0 _pa)
    list(GET _ppm 1 _pb)
    string(SUBSTRING "${_raum}" 0 3 _r3)
    if(_cut LESS 10)
        set(_cc "0${_cut}")
    else()
        set(_cc "${_cut}")
    endif()
    set(_pc "${_bg}/ROOM${_r3}${_cc}.ppm")
    execute_process(COMMAND "${RE15_PPM_TOOL}" "${_pa}" "${_pb}" "${_pc}" ${_x0} ${_y0} ${_x1} ${_y1} 24
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _o TIMEOUT 60)
    string(REGEX MATCH "S=([0-9]+) H=([0-9]+)" _m "${_o}")
    set(_s "${CMAKE_MATCH_1}")
    set(_h "${CMAKE_MATCH_2}")
    message(STATUS "r41_masken36_stage2[${_name}] ROOM${_zr} Cut ${_cut} (${_x},${_z}) Band ${_band} ${_art}: Figurpunkte ${_s}, uebermalt ${_h}")
    math(EXPR _n "${_n} + 1")
    if(NOT _rv EQUAL 0 OR "${_s}" STREQUAL "")
        list(APPEND _fehler "${_name}: PPM-Werkzeug exit=${_rv} '${_o}'")
    elseif(NOT _s GREATER 50)
        list(APPEND _fehler "${_name}: Leon im Figurfenster nicht zu sehen (S=${_s}) - leeres Gruen")
    elseif(_art STREQUAL "V" AND NOT _h EQUAL 0)
        list(APPEND _fehler "${_name}: VOR der Freistellung ${_h} von ${_s} Figurpunkten uebermalt")
    elseif(_art STREQUAL "H")
        math(EXPR _h2 "${_h} * 2")
        if(_h2 LESS _s)
            list(APPEND _fehler "${_name}: HINTER der Freistellung nur ${_h} von ${_s} verdeckt (Soll >= Haelfte)")
        endif()
    elseif(_art STREQUAL "K" AND NOT _h GREATER 0)
        list(APPEND _fehler "${_name}: Kurbel - die Reling-Maske verdeckt Leons Beine nicht (H=${_h})")
    endif()
endforeach()
file(REMOVE "${_exe_kopie}")
if(_fehler)
    string(REPLACE ";" "\n  " _ft "${_fehler}")
    message(FATAL_ERROR "r41_masken36_stage2: ${_n} Szenen, Fehler:\n  ${_ft}")
endif()
message(STATUS "r41_masken36_stage2: ${_n} Szenen OK")
