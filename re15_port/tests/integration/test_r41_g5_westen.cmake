# =============================================================================
# G5 UND ARME SCHIEBEN DEN SPIELER NICHT IN DIE WAND. Runde 41 Spur R, Abnahme 3 (analysis/befunde_runde41/R_abnahme_3.md N1, N2).
#
# Vorher: Leon im Westen des Wagens, ohne Feuer. Der Koerper drueckte ihn gegen die Diagonale #30 (ROOM5090), der Handler LAB_8003cb9c lief bei
# 740 Einheiten Tiefe ueber (`mult v1,a2` @0x8003cf88) und warf ihn hinein, von dort in Box 29 und Box 22 (z -27668). Dort wirkte keine Eingabe
# mehr, der G5 traf ihn nicht und die Pistole nicht den G5: Softlock (Laeufe z_w4, p_falle). Spieler und Arm: Box 29, 388 Bilder (z_fern).
#
# Original: RE2-Klemme FUN_8003567c, Bytes am Kopf von re15_collision_constrain_re2. Kein Messhaken: Rakete bis zum G5, dann die Eingaben der Abnahme.
# Geprueft aus state.log, an JEDEM Bildende:
#   1  Der Spieler-Mittelpunkt liegt in keiner der Zellen #22, #23, #28, #29, #30
#   2  Zum Schluss wirkt die Eingabe: ab Bild 1600 bewegt sich der Spieler um mindestens 300 Einheiten
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_g5_westen.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_g5_westen: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_g5_westen: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41westen_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41westen_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_f "")
function(ein_lauf _name _raum _skript)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_extra "")
    if(_raum STREQUAL "5091")
        set(_extra "RE15_TITLE_SHOT_CHAR=1")
    endif()
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=500,-9175,2048,0"
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=${_skript}"
        RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log RE15_GIVE=18:200 RE15_EQUIP=18
        "RE15_EXIT_AT=2300#${_raum}"
        "${_exe_kopie}")
    set(_g "")
    if(NOT EXISTS "${WORKDIR}/state.log")
        set(_f "${_f}\n ${_name}: kein Protokoll (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    file(STRINGS "${WORKDIR}/state.log" _zeilen REGEX "PL\\(")
    set(_n 0)
    set(_wand 0)
    set(_erste "")
    set(_minx 999999)
    set(_maxx -999999)
    set(_minz 999999)
    set(_maxz -999999)
    set(_raum_an 0)
    foreach(_z IN LISTS _zeilen)
        if(NOT _z MATCHES "^F([0-9]+) .*PL\\((-?[0-9]+),(-?[0-9]+),rot=")
            continue()
        endif()
        set(_b "${CMAKE_MATCH_1}")
        set(_x "${CMAKE_MATCH_2}")
        set(_zz "${CMAKE_MATCH_3}")
        # erst ab dem Sprung in den Raum (Spieler auf dem Bahnsteig)
        if(_zz GREATER -30000 AND _zz LESS -17000)
            set(_raum_an 1)
        endif()
        if(NOT _raum_an)
            continue()
        endif()
        math(EXPR _n "${_n} + 1")
        set(_fest "")
        if(_zz LESS -26884 AND _x GREATER -5673 AND _x LESS 28067)
            set(_fest "#22")
        elseif(_zz GREATER_EQUAL -19870 AND _x GREATER -5673 AND _x LESS 28057)
            set(_fest "#23")
        elseif(_x GREATER_EQUAL 4050 AND _x LESS 20225 AND _zz GREATER_EQUAL -21700 AND _zz LESS -19700)
            set(_fest "#28")
        elseif(_x GREATER_EQUAL 5300 AND _x LESS 20250 AND _zz GREATER_EQUAL -27200 AND _zz LESS -25200)
            set(_fest "#29")
        elseif(_x GREATER_EQUAL -425 AND _x LESS 5300 AND _zz GREATER_EQUAL -27200 AND _zz LESS -25200)
            # #30 Typ 6: fest unter der Linie (-425,-27200)-(5300,-25200)
            math(EXPR _l "(${_zz} + 27200) * 5725")
            math(EXPR _r "(${_x} + 425) * 2000")
            if(_l LESS _r)
                set(_fest "#30")
            endif()
        endif()
        if(_fest)
            math(EXPR _wand "${_wand} + 1")
            if(NOT _erste)
                set(_erste "F${_b} (${_x},${_zz}) ${_fest}")
            endif()
        endif()
        if(_b GREATER_EQUAL 1600)
            if(_x LESS _minx)
                set(_minx "${_x}")
            endif()
            if(_x GREATER _maxx)
                set(_maxx "${_x}")
            endif()
            if(_zz LESS _minz)
                set(_minz "${_zz}")
            endif()
            if(_zz GREATER _maxz)
                set(_maxz "${_zz}")
            endif()
        endif()
    endforeach()
    if(_n LESS 1500)
        string(APPEND _g "\n ${_name}: nur ${_n} Bilder im Raum")
    endif()
    if(_wand GREATER 0)
        string(APPEND _g "\n ${_name}: ${_wand} Bildenden in fester Zelle, erstes ${_erste}")
    endif()
    math(EXPR _weg "${_maxx} - ${_minx} + ${_maxz} - ${_minz}")
    if(_weg LESS 300)
        string(APPEND _g "\n ${_name}: Eingabe wirkt nicht mehr, Weg ab F1600 nur ${_weg}")
    endif()
    message(STATUS "r41_g5_westen ${_name}: ${_n} Bilder, in fester Zelle ${_wand}, Weg ab F1600 ${_weg}")
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

# Eingaben der Abnahme: z_w4 (Leon im Westen ohne Feuer) und z_fern (Suedwand im Armbereich), danach laufen, drehen, rennen
set(_tasten ",W15.5,U2,W1,D2,W1,L0.5,U2,R1.1,XU2,W10")
ein_lauf(leon_w4 5090 "A0.1,W1,U8.5,MA7.2,W4.2,XU1.9${_tasten}")
ein_lauf(elza_w4 5091 "A0.1,W1,U8.5,MA7.2,W4.2,XU2.2${_tasten}")
ein_lauf(leon_fern 5090 "A0.1,W1,U8.5,MA7.2,W4.2,R1.08,XU2.4${_tasten}")
file(REMOVE "${_exe_kopie}")

if(_f)
    message(FATAL_ERROR "r41_g5_westen: FEHLER${_f}")
endif()
message(STATUS "r41_g5_westen: alle Pruefungen gruen")
