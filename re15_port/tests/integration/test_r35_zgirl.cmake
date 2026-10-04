# =============================================================================
# ZOMBIE-MAEDCHEN MIT DER ECHTEN EXE (Runde 35 Spur C, analysis/befunde_runde35/C_zgirl.md).
#
# Das Zombie-Maedchen (Typ 0x13, EM013) steht game-weit nur in ROOM4050/4051 main00 und wird dort per
# Switch(work_vars[0x0A]) @0x01E56 nach dem EINTRITTS-Cut gewaehlt: Cut 9 -> @0x01eb4 (-9900,0,1150),
# Cut 14 -> @0x01f5c (1600,0,4700). Cut 9/14 liefern nur die Selbst-Tueren 6 (@0x01c16) und 7
# (@0x01c36). Original FUN_8001d600: @0x8001d968 vergleicht nur die Stage, @0x8001d988
# `jal 0x800396fc` laedt jedes Tuerziel neu.
#
# Zwei Laeufe, ECHTER Weg (Aktionstaste an der Tuer, kein FIRE_AOT): RE15_DEBUG_JUMP=4050@gp mit
# RE15_PLAYER_POS vor der Tuer (Vorwaertspunkt 620 vor dem Spieler, @0x80042bd0, liegt in der Tuerflaeche)
# und RE15_PRESS=square:
#   A  Tuer 6 -> Cut 9: DOOR FIRE slot=6, Sce_em_set type=0x13 pos=(-9900,0,1150), Gegner in state.log,
#      Abstand sinkt, Spieler-HP sinkt (Griff) — Default-KI (RE2)
#   B  Tuer 7 -> Cut 14: DOOR FIRE slot=7, Sce_em_set type=0x13 pos=(1600,0,4700), Abstand sinkt
# Geprueft werden debug.log und RE15_STATE_LOG. RE15_SOFTWARE_RENDER=1 nur fuer die Robustheit; das
# BILD belegt der Framedump im Dossier (N3).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r35_zgirl.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r35_zgirl: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r35_zgirl: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r35c_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r35c_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_fehler "")

function(zgirl_lauf _name _pos _slot _cut _gx _gz _mit_griff)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/debug.log" "${WORKDIR}/state.log")
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        RE15_DEBUG_JUMP=4050@gp "RE15_PLAYER_POS=${_pos}"
        RE15_PRESS=square@60,square@62
        RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log
        "RE15_EXIT_AT=420#4050"
        "${_exe_kopie}")
    set(_f "")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        set(_fehler "${_fehler}\n[${_name}] kein debug.log (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    if(NOT _dbg MATCHES "DOOR FIRE slot=${_slot} [^\n]*target_cut=${_cut} ")
        string(APPEND _f "\n[${_name}] Tuer ${_slot} -> Cut ${_cut} nicht gefeuert")
    endif()
    if(NOT _dbg MATCHES "Sce_em_set type=0x13 behavior=0x00 slot=0 pos=\\(${_gx},0,${_gz}\\)")
        string(APPEND _f "\n[${_name}] kein Spawn type=0x13 pos=(${_gx},0,${_gz})")
    endif()
    if(NOT _dbg MATCHES "EXIT_AT: Bild 420 in Raum 4050")
        string(APPEND _f "\n[${_name}] exe erreichte Bild 420 in ROOM4050 nicht (exit=${_rv})")
    endif()
    if(EXISTS "${WORKDIR}/state.log")
        file(STRINGS "${WORKDIR}/state.log" _zeilen REGEX "t=13 st=")
        list(LENGTH _zeilen _n)
        if(_n LESS 300)
            string(APPEND _f "\n[${_name}] nur ${_n} Bilder mit dem Zombie-Maedchen in state.log")
        else()
            list(GET _zeilen 0 _erste)
            list(GET _zeilen -1 _letzte)
            string(REGEX MATCH " d=([0-9]+) @" _m "${_erste}")
            set(_d0 "${CMAKE_MATCH_1}")
            set(_dmin 99999)
            foreach(_z IN LISTS _zeilen)
                string(REGEX MATCH "t=13 st=1 [^]]* d=([0-9]+) @" _m "${_z}")
                if(_m AND CMAKE_MATCH_1 LESS _dmin)
                    set(_dmin "${CMAKE_MATCH_1}")
                endif()
            endforeach()
            math(EXPR _gewinn "${_d0} - ${_dmin}")
            if(_gewinn LESS 1500)
                string(APPEND _f "\n[${_name}] keine Annaeherung (d0=${_d0} dmin=${_dmin})")
            endif()
            string(REGEX MATCH "hp=(-?[0-9]+)\\) pst" _m "${_letzte}")
            set(_php "${CMAKE_MATCH_1}")
            if(_mit_griff AND NOT _php LESS 100)
                string(APPEND _f "\n[${_name}] Spieler-HP am Ende ${_php}, kein Griff-Schaden")
            endif()
            message(STATUS "r35_zgirl[${_name}]: ${_n} Bilder, d ${_d0} -> min ${_dmin}, Spieler-HP am Ende ${_php}")
        endif()
    else()
        string(APPEND _f "\n[${_name}] kein state.log")
    endif()
    set(_fehler "${_fehler}${_f}" PARENT_SCOPE)
endfunction()

zgirl_lauf(A_tuer6 "-9300,-23320,3072,0"  6  9 -9900 1150 1)
zgirl_lauf(B_tuer7 "-16830,-24750,2048,0" 7 14  1600 4700 0)

file(REMOVE "${_exe_kopie}")
if(_fehler)
    message(FATAL_ERROR "r35_zgirl: FEHLER${_fehler}")
endif()
message(STATUS "r35_zgirl: alle Pruefungen gruen")
