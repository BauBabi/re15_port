# =============================================================================
# EFFEKT-QUADER UND GPU-GRENZE IM BESCHLEUNIGTEN RENDERER (Integration Runde 41, Z2: Dossier Q OFFEN 1, Entscheid des Orchestrators).
#
# ROOM20B0/20B1 nach der Szene, Figur im Westgang (Wegplan des Ketten-Riegels ab RAUM 20B0): Effekt 0x0f sub 7 steht kurz vor der Kamera. Das Original baut die Quader ohne Klemme
# (FUN_800534c4, Belege include/re15_gpu_grenze_r41.h), die GPU liest 11 Bit je Koordinate und zeichnet nichts ueber 1023 x 511 (psx-spx "Vertex", "Size Restriction").
#
# Geprueft wird an der exe OHNE RE15_SOFTWARE_RENDER, was der Spieler sieht:
#   - fx.log: die Quader ueber der Grenze werden verworfen (Zeilen "-> gpu-grenze"),
#   - Bild: zwischen der ersten und der letzten Verwerfung bleibt der Hintergrund stehen. Das erste Bild zaehlt nicht: dort liegt ein 393 breiter Quader UNTER der Grenze, den auch die Hardware zeichnet.
#   - Bild: der Wassereffekt derselben Szene bleibt (fuenf Bilder hinter der letzten Verwerfung, kleine Quader id=15).
#
# Gemessen 2026-10-31, Mittel je Kanal in Zehntel Graustufen gegen das Bild fuenf vor der ersten Verwerfung, RE15_WINDOW_SCALE=1 (Elza Laeufe int_qr/ve1, ne1; Leon im Riegel je Wert hoechstens 1 daneben):
#   ohne Grenze F756..F767: 294 410 440 703 694 692 326 722 307 327 243 253
#   mit Grenze  F756..F767:  25  40  40  99  97  95 113  98  66  10  22  28
#   Wasser F772 mit und ohne Grenze 94, Verwerfungen 34 in F755..F767, Leon und Elza.
#
# Der Framedump liest ueber SDL_RenderReadPixels zurueck, derselbe Messweg wie alle Bild-Riegel; gegen gdigrab auf das echte Fenster gemessen: Dossier Q_restpunkte.md, Abschnitt Integration Q+R.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_N30_WERKZEUG=<test_r41_nachtrag30> -DWORKDIR=<dir> -DFIGUR=<leon|elza> -P test_r41_restpunkte_effektquader.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_effektquader: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_N30_WERKZEUG OR NOT EXISTS "${RE15_N30_WERKZEUG}")
    message(FATAL_ERROR "r41_effektquader: RE15_N30_WERKZEUG fehlt: '${RE15_N30_WERKZEUG}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_effektquader: WORKDIR fehlt")
endif()

if(FIGUR STREQUAL "elza")
    set(_raum 20B1)
    set(_rk 20b1)
    set(_r50 2051)
    set(_wahl RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
    set(_fam PL04)
    set(_pref re15_pc_r41qg_e_)
else()
    set(_raum 20B0)
    set(_rk 20b0)
    set(_r50 2050)
    set(_wahl RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
    set(_fam PL00)
    set(_pref re15_pc_r41qg_l_)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte "${_exe_dir}/${_pref}*")
foreach(_k IN LISTS _alte)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/${_pref}${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/${_pref}${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")

function(_fehler _was)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_effektquader[${FIGUR}]: ${_was} (${WORKDIR})")
endfunction()

file(WRITE "${WORKDIR}/kette.txt"
"RAUM ${_raum}
WARTE 30
FLAG 3 235
GEHE -21150 -9629 80
BLICK 2048
WARTE 10
AKTION
WARTE 90
GEHE -24600 -9629 150
BLICK 2048
WARTE 10
AKTION
WARTE 120
GEHE -26000 10300 100
BLICK 2048
WARTE 10
AKTION
RAUM ${_r50}
WARTE 30
ENDE
")
re15_start_spiel(_rv 300
    SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
    ${_wahl}
    RE15_DEBUG_JUMP=${_rk}@250 "RE15_SET_FLAG=9:100,9:103"
    "RE15_WEGPLAN=${WORKDIR}/kette.txt" RE15_WEGPLAN_LOG=wegplan.txt
    RE15_FX_LOG=fx.log "RE15_FRAMEDUMP=745-790/1:f" RE15_EXIT_AT=800
    "${_exe_kopie}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    _fehler("kein debug.log (exit=${_rv})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
string(FIND "${_dbg}" "[pl] Spieler-Familie ${_fam} " _p)
if(_p LESS 0)
    _fehler("Spielfigur nicht ${_fam} (exit=${_rv})")
endif()
string(FIND "${_dbg}" "EXIT_AT: Bild 800 in Raum ${_rk}" _p)
if(_p LESS 0)
    _fehler("Lauf endet nicht in Bild 800 in Raum ${_rk} (exit=${_rv})")
endif()
if(NOT EXISTS "${WORKDIR}/fx.log")
    _fehler("kein fx.log")
endif()

# ---- Verwerfungen ------------------------------------------------------------------------------------------------
file(STRINGS "${WORKDIR}/fx.log" _weg REGEX "-> gpu-grenze ")
list(LENGTH _weg _n)
if(_n LESS 20)
    _fehler("nur ${_n} Quader verworfen, gemessen 34: die Grenze greift im Effekt-Zeichner nicht")
endif()
set(_erst 999999)
set(_letzt 0)
foreach(_z IN LISTS _weg)
    if(NOT _z MATCHES "F=([0-9]+)")
        _fehler("Zeile ohne Bild: ${_z}")
    endif()
    if(CMAKE_MATCH_1 LESS _erst)
        set(_erst ${CMAKE_MATCH_1})
    endif()
    if(CMAKE_MATCH_1 GREATER _letzt)
        set(_letzt ${CMAKE_MATCH_1})
    endif()
endforeach()
math(EXPR _ref "${_erst} - 5")
math(EXPR _wasser "${_letzt} + 5")
if(_ref LESS 745 OR _wasser GREATER 790)
    _fehler("Verwerfungen F${_erst}..F${_letzt} ausserhalb des Framedumps 745..790")
endif()
message(STATUS "r41_effektquader[${FIGUR}]: ${_n} Quader verworfen in F${_erst}..F${_letzt}")

function(_bild _f _out)
    string(LENGTH "${_f}" _l)
    math(EXPR _null "6 - ${_l}")
    string(REPEAT "0" ${_null} _v)
    set(${_out} "${WORKDIR}/f${_v}${_f}.ppm" PARENT_SCOPE)
endfunction()
_bild(${_ref} _ref_bild)
if(NOT EXISTS "${_ref_bild}")
    _fehler("Framedump fehlt: ${_ref_bild}")
endif()

# ---- Bild: zwischen den Verwerfungen steht der Hintergrund ----------------------------------------------------------
math(EXPR _ab "${_erst} + 1")
set(_max 0)
set(_reihe "")
foreach(_f RANGE ${_ab} ${_letzt})
    _bild(${_f} _b)
    execute_process(COMMAND "${RE15_N30_WERKZEUG}" diff "${_ref_bild}" "${_b}" 0 0 320 240 OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
    if(NOT _r EQUAL 0 OR NOT _o MATCHES "^DIFF ([0-9]+)")
        _fehler("Bildvergleich F${_f}: ${_o}")
    endif()
    string(APPEND _reihe " ${CMAKE_MATCH_1}")
    if(CMAKE_MATCH_1 GREATER _max)
        set(_max ${CMAKE_MATCH_1})
    endif()
endforeach()
message(STATUS "r41_effektquader[${FIGUR}]: Abweichung F${_ab}..F${_letzt} gegen F${_ref}:${_reihe}")
if(_max GREATER 150)
    _fehler("Quader ueber der Grenze im Bild: Abweichung bis ${_max} (mit Grenze gemessen hoechstens 113, ohne 243..722)")
endif()

# ---- Bild: der gewollte Wassereffekt bleibt ---------------------------------------------------------------------------
_bild(${_wasser} _w)
execute_process(COMMAND "${RE15_N30_WERKZEUG}" diff "${_ref_bild}" "${_w}" 0 0 320 240 OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
if(NOT _r EQUAL 0 OR NOT _o MATCHES "^DIFF ([0-9]+)")
    _fehler("Bildvergleich Wasser: ${_o}")
endif()
set(_d ${CMAKE_MATCH_1})
file(STRINGS "${WORKDIR}/fx.log" _w_z REGEX "^id=15 .* F=${_wasser}$")
list(LENGTH _w_z _nw)
message(STATUS "r41_effektquader[${FIGUR}]: Wasser F${_wasser}: ${_nw} Quader id=15, Abweichung ${_d}")
if(_nw LESS 1 OR _d LESS 60)
    _fehler("der Wassereffekt fehlt: F${_wasser} ${_nw} Quader, Abweichung ${_d} (gemessen 94)")
endif()

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_effektquader[${FIGUR}]: OK")
