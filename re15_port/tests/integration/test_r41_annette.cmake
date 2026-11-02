# =============================================================================
# Runde 41 Spur AN: Annette-Szene im hinteren Zugwagen (include/re15_annette_r41.h, analysis/befunde_runde41/AN_annette.md).
#
# Vier echte Laeufe (exe, kein Messhaken ausser RE15_SET_FLAG fuer den Spielstand). 5090/5091 wie integration_r41_birkin_pistole:
# Debug-Sprung in Wagen B, Tuer 2 nach Wagen C, laufen bis Cut 12 (Kampfstart-Ausloeser sub01 @0x01290).
#   leon_tor   (3,89)=1 (Tor, ROOM6000 sub07 @0x011B4) und (9,133)=1 (Zugstart 6030 gesehen): Modell RE2/ANNETTE_4D.EMD, Szene
#              nach RE2 ROOM6141 sub02 in der Reihenfolge Szenenstart (Krach) -> Sturz -> gelandet -> Hieb -> Treffer (Blut-Textur
#              nachgeladen) -> Schnitt weg/liegt/Schrei -> Kampfstart sub04; Toene 0/1/2 der Mini-Bank; danach die Riesenform
#              freigegeben (grid 0x13 = sub04 Member_set @0x0130A), Annette bleibt liegen (hp -1).
#   leon_ohne_zug (3,89)=1, (9,133)=0: die Frau im Kittel wird in 5090 verworfen (erst nach dem Zugstart), keine Szene.
#   elza       5091 mit (3,89)=1, (9,133)=1: keine Frau im Kittel auf der Disc, keine Szene.
#   zug6030    Fuehrerstand 6030 (Debug-Sprung = Eintritt Cut 0 @0x8001d818 -> sub00 Case 0 -> sub07) mit (3,89)=1, (4,23)=1:
#              Zugstart-Szene statt sub05 (sub07 @0x01132), Toene Zug an (3) vor Krach (0), Annette geht ab und ist versteckt
#              (Pos_set-Form 6000 @0x0100A), (9,133) gesetzt.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_annette.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_annette: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_annette: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41annette_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41annette_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_f "")
function(ein_lauf _name _raum _flag _pos _exit)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_extra "")
    if(_raum STREQUAL "5091")
        set(_extra "RE15_TITLE_SHOT_CHAR=1")
    endif()
    if(_flag)
        list(APPEND _extra "RE15_SET_FLAG=${_flag}")
    endif()
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=${_pos}"
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=A0.1,W1,U8.5,W40"
        RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log
        "RE15_EXIT_AT=${_exit}#${_raum}"
        "${_exe_kopie}")
    set(_g "")
    if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.log")
        set(_f "${_f}\n ${_name}: kein Protokoll (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    file(STRINGS "${WORKDIR}/state.log" _frau REGEX " t=4d st=")
    if(_name STREQUAL "leon_tor")
        if(NOT _dbg MATCHES "Ersatzmodell RE2/ANNETTE_4D.EMD")
            string(APPEND _g "\n ${_name}: Modell RE2/ANNETTE_4D.EMD nicht geladen")
        endif()
        set(_pos 0)
        foreach(_schritt "Ton 0 " "Szenenstart statt sub04" "Ton 1 " "Sturz:" "gelandet, Drohen:" "Ton 2 " "Hieb \\(RE2"
                         "Treffer:" "Schnitt weg, liegt, Schrei" "Kampfstart \\(sub04\\), Form frei")
            string(REGEX MATCH "\\[annette\\] F[0-9]+ ${_schritt}" _m "${_dbg}")
            if(NOT _m)
                string(APPEND _g "\n ${_name}: Schritt '${_schritt}' fehlt im debug.log")
                continue()
            endif()
            string(FIND "${_dbg}" "${_m}" _p)
            if(_p LESS _pos)
                string(APPEND _g "\n ${_name}: Schritt '${_schritt}' in falscher Reihenfolge")
            endif()
            set(_pos ${_p})
        endforeach()
        if(NOT _dbg MATCHES "Ersatzmodell RE2/ANNETTE_4D_BLUT.EMD")
            string(APPEND _g "\n ${_name}: Blut-Textur (ANNETTE_4D_BLUT.EMD) nach dem Treffer nicht geladen")
        endif()
        file(STRINGS "${WORKDIR}/state.log" _form REGEX " t=30 st=[0-9]+ ss1=[0-9]+ ss2=[0-9]+ ss3=[0-9]+ g=13 ")
        list(LENGTH _form _nf)
        if(_nf EQUAL 0)
            string(APPEND _g "\n ${_name}: Riesenform nach der Szene nie freigegeben (kein t=30 mit g=13)")
        endif()
        list(LENGTH _frau _na)
        if(_na EQUAL 0)
            string(APPEND _g "\n ${_name}: keine Frau im Kittel im state.log")
        else()
            list(GET _frau -1 _letzte)
            if(NOT _letzte MATCHES "hp=-1")
                string(APPEND _g "\n ${_name}: Annette am Ende nicht hp -1: ${_letzte}")
            endif()
        endif()
    elseif(_name STREQUAL "zug6030")
        set(_pos 0)
        foreach(_schritt "ROOM6030 Zugstart-Szene statt sub05" "Ton 3 " "Ton 0 ")
            string(REGEX MATCH "\\[annette\\] F[0-9]+ ${_schritt}" _m "${_dbg}")
            if(NOT _m)
                string(APPEND _g "\n ${_name}: Schritt '${_schritt}' fehlt im debug.log")
                continue()
            endif()
            string(FIND "${_dbg}" "${_m}" _p)
            if(_p LESS _pos)
                string(APPEND _g "\n ${_name}: Schritt '${_schritt}' in falscher Reihenfolge")
            endif()
            set(_pos ${_p})
        endforeach()
        list(LENGTH _frau _na)
        if(_na EQUAL 0)
            string(APPEND _g "\n ${_name}: Annette nie im state.log (Sce_em_set @0x010F8 im Programm)")
        else()
            list(GET _frau -1 _letzte)
            if(NOT _letzte MATCHES "8646")
                string(APPEND _g "\n ${_name}: Annette am Ende nicht versteckt (z 8646): ${_letzte}")
            endif()
        endif()
    else()
        if(_dbg MATCHES "\\[annette\\] F[0-9]+ Szenenstart")
            string(APPEND _g "\n ${_name}: Szene gestartet, erwartet keine")
        endif()
        list(LENGTH _frau _na)
        if(NOT _na EQUAL 0)
            string(APPEND _g "\n ${_name}: Frau im Kittel steht im Wagen (${_na} Zeilen), erwartet keine")
        endif()
        if(_name STREQUAL "leon_ohne_zug" AND NOT _dbg MATCHES "Frau im Kittel \\(0x4D, @0x0122A\\) verworfen")
            string(APPEND _g "\n ${_name}: Verwerfen der 0x4D nicht protokolliert")
        endif()
    endif()
    message(STATUS "r41_annette ${_name}: exit=${_rv}, Zeilen Frau ${_na}")
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

ein_lauf(leon_tor 5090 "3:89,9:133" "500,-9175,2048,0" 1500)
ein_lauf(leon_ohne_zug 5090 "3:89" "500,-9175,2048,0" 900)
ein_lauf(elza 5091 "3:89,9:133" "500,-9175,2048,0" 900)
ein_lauf(zug6030 6030 "3:89,4:23" "-3873,-23487,0,0" 1300)
file(REMOVE "${_exe_kopie}")
if(_f)
    message(FATAL_ERROR "r41_annette:${_f}")
endif()
message(STATUS "r41_annette: alle vier Laeufe wie erwartet")
