# =============================================================================
# Runde 41 Spur G "kanister2080", Nachbesserung 4 (Abnahme G 3, Mangel M2): die Zeichnung der RE2-Kern-CLUT-Zeilen 485..498
# (platform/pc/src/gator_fx_r41_pc.c) darf nicht davon abhaengen, ob in diesem Prozess schon ROOM2090/2091 betreten wurde.
#
# Vorher gemessen (Abnahme 3 Abschnitt 5): derselbe Granatwerfer-Schuss (Id 15, Runde 35) in ROOM2080, gleiche Lage (-9950,-25700)
# Gier 2907 Cut 4 - ohne 2090-Besuch "gezeichnet G 0 (kern485 0 raum 0)", nach dem Leiterweg aus 2090 "gezeichnet G 6 (kern485 6
# raum 0)" (Muendungsblitz 0x01002000 = Kern-Bank 1 Zeile 492, Granatkoerner 0x020c0a00 = Zeile 495).
# Jetzt zeichnet der Zeichner Kern-485-Quads nur fuer Plaetze der R1-Folge des Alligators (Matrix-Zeiger +0x6C `sw a2,108(t0)`
# @0x8001cd6c = Fontaene Part 6/7 bzw. Spritzring-Kanister; re2fx_platz_quelle / re15_gr2_fx_quelle_r1) - alle anderen laesst der
# Port in jedem Raum aus wie re2fx_pc.c (Kommentar N5).
#
# Zwei Laeufe (RE15_GATOR_FX_LOG=1):
#   direkt    RE15_DEBUG_JUMP=2080, Schuss aus der Lage des Abnahme-Laufs gl_2080_direkt
#   nach2090  RE15_DEBUG_JUMP=2090 (ohne Alligator), Leiter DOOR16 nach 2080, derselbe Schuss (Lauf gl_2080_nach2090)
# Gefordert: in beiden Laeufen Kern-485-Quads in ROOM2080 vorhanden (der Schuss fand statt) und in KEINEM ROOM2080-Bild gezeichnet
# ("gezeichnet G 0", alle als "kern485 fremd" gezaehlt); die Folge der Quad-Zahlen (Quads, kern480, kern485) nach dem Schuss ist in
# beiden Laeufen gleich; im Lauf nach2090 war die Raum-ESP ROOM40A0 registriert (Atlas geladen) und wurde beim Verlassen ausgetragen.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_kanister2080_raumfremd.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_kanister2080_raumfremd: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_kanister2080_raumfremd: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe_kopie "${_exe_dir}/re15_pc_r41gr_${_lauf_id}.exe")
if(NOT WIN32)
    set(_exe_kopie "${_exe_dir}/re15_pc_r41gr_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

function(r41gr_fehler _text)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_kanister2080_raumfremd: ${_text}")
endfunction()

unset(ENV{RE15_GB_TEST})
unset(ENV{RE15_GATOR_KANISTER})
unset(ENV{RE15_KEIN_GATOR})
unset(ENV{RE15_K2080_WURF_GIER})
unset(ENV{RE15_K2080_GATOR_HP})

set(_basisenv RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
              "RE15_SET_FLAG=9:110,3:245" "RE15_GIVE=15:6" RE15_EQUIP=15 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1
              RE15_GATOR_FX_LOG=1)

foreach(_lauf direkt nach2090)
    set(WORKDIR "${_basis}/${_lauf}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    if(_lauf STREQUAL "direkt")
        re15_start_spiel(_rv 300 ${_basisenv} RE15_DEBUG_JUMP=2080@250 "RE15_PLAYER_POS=-9950,-25700,2907,8" RE15_FORCE_CUT=4
            "RE15_INPUT_SCRIPT=W6.0,M0.6,MA1.5,M1.0,W4" "RE15_EXIT_AT=400#2080" "${_exe_kopie}")
    else()
        re15_start_spiel(_rv 300 ${_basisenv} RE15_DEBUG_JUMP=2090@250 RE15_KEIN_GATOR=1 "RE15_PLAYER_POS=-8900,-25244,2048"
            "RE15_INPUT_SCRIPT=W1.0,A0.2,W6.0,M0.6,MA1.5,M1.0,W4" "RE15_EXIT_AT=300#2080" "${_exe_kopie}")
    endif()
    if(NOT _rv EQUAL 0)
        r41gr_fehler("Lauf ${_lauf}: exe exit=${_rv}")
    endif()
    file(STRINGS "${WORKDIR}/debug.log" _z REGEX "\\[gator-fx\\] F[0-9]+ ROOM2080 R1 0 reg40A0 0 ")
    set(_folge "")
    set(_vorhanden 0)
    foreach(_l IN LISTS _z)
        if(NOT _l MATCHES "Quads ([0-9]+): kern480 ([0-9]+) kern485 ([0-9]+) raum [0-9]+ glas [0-9]+ rest [0-9]+ \\| gezeichnet G ([0-9]+) \\(kern485 ([0-9]+) raum ([0-9]+)\\).* \\| kern485 fremd ([0-9]+) ")
            r41gr_fehler("Lauf ${_lauf}: Messzeile unlesbar: '${_l}'")
        endif()
        if(NOT CMAKE_MATCH_4 EQUAL 0)
            r41gr_fehler("Lauf ${_lauf}: Kern-485-Quads in ROOM2080 gezeichnet (Nebenwirkung M2): '${_l}'")
        endif()
        if(NOT CMAKE_MATCH_7 EQUAL CMAKE_MATCH_3)
            r41gr_fehler("Lauf ${_lauf}: kern485 ${CMAKE_MATCH_3} != fremd ${CMAKE_MATCH_7}: '${_l}'")
        endif()
        if(CMAKE_MATCH_3 GREATER 0)
            set(_vorhanden 1)
        endif()
        list(APPEND _folge "${CMAKE_MATCH_1}/${CMAKE_MATCH_2}/${CMAKE_MATCH_3}")
    endforeach()
    if(NOT _vorhanden)
        r41gr_fehler("Lauf ${_lauf}: keine Kern-485-Quads in ROOM2080 - der Schuss hat nicht stattgefunden (Riegel prueft nichts)")
    endif()
    set(_folge_${_lauf} "${_folge}")
    list(LENGTH _folge _n)
    message(STATUS "r41_kanister2080_raumfremd ${_lauf}: ${_n} ROOM2080-Bilder mit RE2-Effekten, gezeichnet G 0 in allen")
endforeach()

# Im Lauf nach2090 war die Raum-ESP ROOM40A0 registriert und wurde beim Wechsel nach 2080 ausgetragen
file(STRINGS "${_basis}/nach2090/debug.log" _reg REGEX "\\[gator-fx\\] F[0-9]+ ROOM2090: Raum-ESP ROOM40A0 -> re2fx_register_raum rc=0")
file(STRINGS "${_basis}/nach2090/debug.log" _aus REGEX "\\[gator-fx\\] F[0-9]+ ROOM2080: Raum-ESP ROOM40A0 ausgetragen")
file(STRINGS "${_basis}/nach2090/debug.log" _atl REGEX "\\[gator-fx\\] GATOR40A0.ESP [0-9]+ B .*Atlas Slot 54 ok")
if(NOT _reg OR NOT _aus OR NOT _atl)
    r41gr_fehler("Lauf nach2090: 2090-Besuch nicht belegt (Registrierung '${_reg}', Austragen '${_aus}', Atlas '${_atl}')")
endif()
# Derselbe Schuss: die Quad-Folge ab dem ersten Bild mit Kern-485-Quads gleich
foreach(_lauf direkt nach2090)
    set(_ab "")
    set(_an 0)
    foreach(_e IN LISTS _folge_${_lauf})
        if(NOT _an AND _e MATCHES "/[1-9][0-9]*$")
            set(_an 1)
        endif()
        if(_an)
            list(APPEND _ab "${_e}")
        endif()
    endforeach()
    list(LENGTH _ab _n)
    if(_n GREATER 12)
        list(SUBLIST _ab 0 12 _ab)
    endif()
    set(_ab_${_lauf} "${_ab}")
endforeach()
if(NOT "${_ab_direkt}" STREQUAL "${_ab_nach2090}")
    r41gr_fehler("Quad-Folge nach dem Schuss verschieden: direkt '${_ab_direkt}' / nach2090 '${_ab_nach2090}'")
endif()

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_kanister2080_raumfremd: derselbe Schuss mit/ohne 2090-Besuch gleich (Quads/kern480/kern485: ${_ab_direkt}), Kern-485 nie gezeichnet")
