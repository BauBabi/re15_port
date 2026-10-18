cmake_minimum_required(VERSION 3.16)
# =============================================================================
# SCHLUESSEL AN DER PINNWAND ROOM10F0/10F1 "MAXIMAL HALB SO GROSS" MIT DER ECHTEN EXE (Runde 39, Spur M "schluessel2").
#
# Dossier analysis/befunde_runde39/M_schluessel2.md. NUTZER-VORGABE (AUFTRAG.md Nachtrag 5, woertlich): "Und die Schlüssel
# im Communicationsroom sind immer noch zu groß, die sollten maximal halb so groß sein." Werkzeug
# tools/r38_itemorte/itemorte_export.py -> gen/itemorte_r38_lage.inc (RE15_R39M_*): groesster Massstab, bei dem die
# gezeichnete Huelle (stetig) UND das GTE-Viereck (ganzzahlige Bildschirm-Ecken, FUN_80022f0c RTPT @0x80022fb0 -> SXY ins
# GPU-Polygon @0x80022fd8) in Breite und Hoehe hoechstens halb so gross sind wie der Stand Spur L.
#
# Weg: ECHTER Lade-Weg (Spielstand mit probe_r37_feuerloescher_karte, RE15_CONTINUE_TEST + RE15_CARD_AUTO, wie
# integration_r38_itemorte k_lauf), Fensterskala 3 (960x720, so spielt der Nutzer), Cut 5 (Nutzer-Cut key_location.bmp),
# Bild 100, Leon ROOM10F0 (Parking Lot Key) und Elza ROOM10F1 (Prison Key, Karte "elza"), je mit / ohne Taken-Bit.
#   R  RASTER: Messschiene RE15_TRILOG (render_pc.c) im Fenster der Marke: die Ecken der letzten 4 Dreiecke (ein Bild) ==
#      RE15_R39M_*_GTE_X0/Y0/B/H aus gen/itemorte_r38_lage.inc (Park 2 x 1, Prison 3 x 2; Spur L war 6 x 3 / 6 x 4)
#   H  HALB: Unterschied mit/ohne (probe_r38_itemorte_kontrast, |dR|+|dG|+|dB| > 30) im Pinnwand-Fenster: Huelle Breite
#      UND Hoehe <= halbe Spur-L-Messung (Abnahme L0 bei 960x720: Leon 14 x 8, Elza 16 x 10 -> <= 7 x 4 bzw. 8 x 5)
#   S  SICHTBAR: Pixel und Sum|dL| >= ~60 % der Messung 2026-10-18 (Leon 9 / 403, Elza 16 / 748), AUSSEN == 0
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KONTRAST_TOOL=<probe_r38_itemorte_kontrast>
#               -DRE15_KARTE_TOOL=<probe_r37_feuerloescher_karte> -DRE15_LAGE_INC=<gen/itemorte_r38_lage.inc>
#               -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
foreach(_v RE15_PC_EXE RE15_KONTRAST_TOOL RE15_KARTE_TOOL RE15_LAGE_INC)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r39_schluessel2: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "r39_schluessel2: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# keine Umgebung des Aufrufers erben
foreach(_v RE15_WINDOW_SCALE RE15_FRAMEDUMP RE15_TRILOG RE15_INPUT_SCRIPT RE15_INPUT_SCRIPT_START RE15_INPUT_SCRIPT_BASIS
           RE15_PLAYER_POS RE15_DEBUG_JUMP RE15_GOTO_ROOM RE15_FORCE_CUT RE15_SET_FLAG RE15_EXIT_AT RE15_UVPHASE
           RE15_PSELECT_AUTO RE15_PSELECT_AUTO_SWITCH RE15_TITLE_SHOT RE15_TITLE_SHOT_AF RE15_SOFTWARE_RENDER)
    unset(ENV{${_v}})
endforeach()

# Soll-Ecken aus dem erzeugten Kopf (eine Quelle: das Werkzeug)
file(STRINGS "${RE15_LAGE_INC}" _inc REGEX "^#define RE15_R39M_(PARK|PRISON)_GTE_(X0|Y0|B|H) ")
foreach(_z IN LISTS _inc)
    if(_z MATCHES "^#define (RE15_R39M_[A-Z]+_GTE_[A-Z0-9]+) +(-?[0-9]+)")
        set(${CMAKE_MATCH_1} "${CMAKE_MATCH_2}")
    endif()
endforeach()
foreach(_v RE15_R39M_PARK_GTE_X0 RE15_R39M_PARK_GTE_Y0 RE15_R39M_PARK_GTE_B RE15_R39M_PARK_GTE_H
           RE15_R39M_PRISON_GTE_X0 RE15_R39M_PRISON_GTE_Y0 RE15_R39M_PRISON_GTE_B RE15_R39M_PRISON_GTE_H)
    if("${${_v}}" STREQUAL "")
        message(FATAL_ERROR "r39_schluessel2: ${_v} nicht in ${RE15_LAGE_INC}")
    endif()
endforeach()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis); Praefix nur DIESES Tests, Endung wie die Quelle.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r39m_schl2_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r39m_schl2_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(_basis_wd "${WORKDIR}")
set(_fehler "")

# Ein Lauf: Spielstand schreiben, exe starten (CONTINUE), Framedump Bild 100, debug.log (+ tri.log) pruefen.
function(lauf _name _raum _trilog)
    set(WORKDIR "${_basis_wd}/lauf_${_lauf_id}_${_name}")
    file(REMOVE_RECURSE "${WORKDIR}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${ARGN}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r39_schluessel2[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    set(_tri "")
    if(_trilog)
        set(_tri "RE15_TRILOG=tri.log@133,77,144,86")
    endif()
    re15_start_spiel(_rv 400
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=3
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 RE15_INPUT_SCRIPT=W3
        RE15_EXIT_AT=120 "RE15_FRAMEDUMP=100:f.ppm" ${_tri}
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/f.ppm" OR NOT EXISTS "${WORKDIR}/debug.log")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r39_schluessel2[${_name}]: kein Framedump/debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(TOLOWER "${_raum}" _raum_kl)
    string(FIND "${_dbg}" "CONTINUE: resumed in room ${_raum_kl}" _p)
    if(_p LESS 0)
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r39_schluessel2[${_name}]: Spielstand in ROOM${_raum} nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    set(_ppm_${_name} "${WORKDIR}/f.ppm" PARENT_SCOPE)
    set(_dbg_${_name} "${_dbg}" PARENT_SCOPE)
    set(_tri_${_name} "${WORKDIR}/tri.log" PARENT_SCOPE)
endfunction()

macro(pruef _name _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(_p LESS 0)
        list(APPEND _fehler "${_name}: fehlt '${_muster}'")
        message(STATUS "FAIL ${_name}: fehlt '${_muster}'")
    else()
        message(STATUS "ok   ${_name}: '${_muster}'")
    endif()
endmacro()

# R: Ecken der letzten 4 eingereihten Dreiecke (= ein Bild; der Schluessel sind 4 Dreiecke um die Mitte) gegen den Kopf.
function(raster _name _trilog _x0 _y0 _b _h)
    if(NOT EXISTS "${_trilog}")
        set(_fehler "${_fehler};${_name}: kein tri.log" PARENT_SCOPE)
        return()
    endif()
    file(STRINGS "${_trilog}" _zeilen REGEX "tri")
    list(LENGTH _zeilen _n)
    if(_n LESS 4)
        set(_fehler "${_fehler};${_name}: nur ${_n} Dreiecke im Fenster der Marke" PARENT_SCOPE)
        return()
    endif()
    math(EXPR _ab "${_n} - 4")
    list(SUBLIST _zeilen ${_ab} 4 _letzte)
    set(_mnx 100000)
    set(_mxx -100000)
    set(_mny 100000)
    set(_mxy -100000)
    foreach(_z IN LISTS _letzte)
        string(REGEX MATCHALL "\\((-?[0-9]+),(-?[0-9]+)\\)uv" _paare "${_z}")
        foreach(_pp IN LISTS _paare)
            string(REGEX MATCH "\\((-?[0-9]+),(-?[0-9]+)\\)" _m "${_pp}")
            set(_x "${CMAKE_MATCH_1}")
            set(_y "${CMAKE_MATCH_2}")
            if(_x LESS _mnx)
                set(_mnx ${_x})
            endif()
            if(_x GREATER _mxx)
                set(_mxx ${_x})
            endif()
            if(_y LESS _mny)
                set(_mny ${_y})
            endif()
            if(_y GREATER _mxy)
                set(_mxy ${_y})
            endif()
        endforeach()
    endforeach()
    math(EXPR _bb "${_mxx} - ${_mnx}")
    math(EXPR _hh "${_mxy} - ${_mny}")
    message(STATUS "r39_schluessel2[${_name}]: GTE-Viereck x ${_mnx}..${_mxx} y ${_mny}..${_mxy} (${_bb} x ${_hh}), Soll (${_x0},${_y0}) ${_b} x ${_h}")
    if(NOT _mnx EQUAL _x0 OR NOT _mny EQUAL _y0 OR NOT _bb EQUAL _b OR NOT _hh EQUAL _h)
        set(_fehler "${_fehler};${_name}: GTE-Viereck (${_mnx},${_mny}) ${_bb} x ${_hh} statt (${_x0},${_y0}) ${_b} x ${_h}" PARENT_SCOPE)
    endif()
endfunction()

# H + S: probe_r38_itemorte_kontrast <mit> <ohne> x0 y0 x1 y1 -> "FENSTER ..: N Pixel, SUMDL S, Huelle x..y.. | AUSSEN: n"
function(mass _name _mit _ohne _min_px _min_dl _max_b _max_h)
    execute_process(COMMAND "${RE15_KONTRAST_TOOL}" "${_mit}" "${_ohne}" 393 225 437 263
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _o TIMEOUT 60)
    string(STRIP "${_o}" _o)
    message(STATUS "r39_schluessel2[${_name}]: ${_o}")
    if(NOT _o MATCHES "FENSTER [^:]*: ([0-9]+) Pixel, SUMDL ([0-9]+), Huelle x([0-9]+)\\.\\.([0-9]+) y([0-9]+)\\.\\.([0-9]+) \\| AUSSEN: ([0-9]+)")
        set(_fehler "${_fehler};${_name}: Werkzeug-Ausgabe unlesbar oder nichts gezeichnet '${_o}'" PARENT_SCOPE)
        return()
    endif()
    set(_px "${CMAKE_MATCH_1}")
    set(_dl "${CMAKE_MATCH_2}")
    math(EXPR _b "${CMAKE_MATCH_4} - ${CMAKE_MATCH_3} + 1")
    math(EXPR _h "${CMAKE_MATCH_6} - ${CMAKE_MATCH_5} + 1")
    set(_aussen "${CMAKE_MATCH_7}")
    set(_f "")
    if(_px LESS _min_px OR _dl LESS _min_dl)
        list(APPEND _f "${_name}: ${_px} Pixel / Sum|dL| ${_dl} (Soll >= ${_min_px} / ${_min_dl}) - nicht sichtbar")
    endif()
    if(_b GREATER _max_b OR _h GREATER _max_h)
        list(APPEND _f "${_name}: Huelle ${_b} x ${_h} (Soll <= ${_max_b} x ${_max_h}) - nicht halb so gross")
    endif()
    if(NOT _aussen EQUAL 0)
        list(APPEND _f "${_name}: ${_aussen} Pixel ausserhalb des Fensters")
    endif()
    if(_f)
        set(_fehler "${_fehler};${_f}" PARENT_SCOPE)
    else()
        message(STATUS "ok   ${_name}: Huelle ${_b} x ${_h} <= ${_max_b} x ${_max_h}, ${_px} Pixel / Sum|dL| ${_dl} >= ${_min_px} / ${_min_dl}, aussen 0")
    endif()
endfunction()

lauf(leon_mit  10F0 1 10F0 0 4000 1024 cut:5 flag:9:71 flag:3:94)
lauf(leon_ohne 10F0 0 10F0 0 4000 1024 cut:5 flag:9:71 flag:3:94 flag:9:82)
lauf(elza_mit  10F1 1 10F1 0 4000 1024 cut:5 elza)
lauf(elza_ohne 10F1 0 10F1 0 4000 1024 cut:5 elza flag:9:83)
pruef(L _dbg_leon_mit "[schluessel] ROOM10F0: PARKING LOT KEY (Item 0x45, Bit (9,82)) Prop obj 4 bei (1358,-2495,-3400) rot (3072,0,2048), Zone Slot 8")
pruef(E _dbg_elza_mit "[pl] Spieler-Familie PL04")
pruef(E _dbg_elza_mit "[schluessel] ROOM10F1: PRISON KEY (Item 0x42, Bit (9,83)) Prop obj 4 bei (1358,-2495,-3400) rot (1024,0,0), Zone Slot 8")

raster(R_park   "${_tri_leon_mit}" ${RE15_R39M_PARK_GTE_X0} ${RE15_R39M_PARK_GTE_Y0} ${RE15_R39M_PARK_GTE_B} ${RE15_R39M_PARK_GTE_H})
raster(R_prison "${_tri_elza_mit}" ${RE15_R39M_PRISON_GTE_X0} ${RE15_R39M_PRISON_GTE_Y0} ${RE15_R39M_PRISON_GTE_B} ${RE15_R39M_PRISON_GTE_H})

# Grenzen: halbe Spur-L-Messung bei 960x720 (Abnahme L0: Leon 14 x 8, Elza 16 x 10), abgerundet. Mindestwerte ~60 % der
# Messung 2026-10-18 (Dossier "Messung nachher": Leon 9 Pixel / Sum|dL| 403, Elza 16 / 748).
mass(H_leon "${_ppm_leon_mit}" "${_ppm_leon_ohne}" 5 240 7 4)
mass(H_elza "${_ppm_elza_mit}" "${_ppm_elza_ohne}" 9 450 8 5)
file(REMOVE "${_exe_kopie}")

if(_fehler)
    string(REPLACE ";" "\n  " _f "${_fehler}")
    message(FATAL_ERROR "r39_schluessel2: FEHLER\n  ${_f}")
endif()
message(STATUS "r39_schluessel2: OK")
