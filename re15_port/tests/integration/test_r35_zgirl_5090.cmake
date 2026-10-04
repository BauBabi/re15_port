# =============================================================================
# ENDKAMPF ROOM5090 NACH EINER SELBST-TUER (Runde 35 Spur C, Nachbesserung 1 M2b,
# analysis/befunde_runde35/C_zgirl.md).
#
# Seit U1 (aot_common.c) steigt jede Selbst-Tuer neu ein (@0x8001d988 `jal 0x800396fc`). In ROOM5090
# kommt man nur so zum Boss: Einstieg aus ROOM6030 Slot 3 im Nordwagen, dann Selbst-Tueren 0 und 2.
# Beim Wiedereintritt spawnt sub00 @0x0124A den Boss erneut (Typ 0x30 -> 0x36). Abnahme 0 mass:
# HP 0 nach der Tuer, Tod beim Kampfstart ohne Treffer (M1). Original: Sce_em_set @0x800421e0
# `sw zero,4(s0)` -> RE2-G5-Main @0x80100164 liest +0x4 -> Tabelle @0x801055CC[0] = Konstruktor
# @0x801003CC -> HP 600 (@0x801003fc / @0x80100400).
#
# Ein Lauf, ECHTER Weg: RE15_DEBUG_JUMP=5090@gp, Leon vor Tuer 2 (Rechteck (-550,-9175,hw 450,hh 1975),
# Vorwaertspunkt 620 vor dem Spieler @0x80042bd0), Aktionstaste per Eingabeskript, danach geradeaus
# laufen bis zum Kampfstart (sub04 Member_set(0x0c,0x13) @0x130A). Geprueft:
#   1  debug.log: DOOR FIRE slot=2 -> Cut 14 und danach ein ZWEITER Sce_em_set type=0x36
#   2  state.log: nach der Tuer Boss-HP 600 am Ende, nie < 0 (Leon schiesst nicht; das Spawn-Bild darf 0 zeigen)
#   3  state.log: der Boss laeuft an (x von -9000 = [T0] @0x801011d0 um >= 1500 nach vorn)
# RE15_SOFTWARE_RENDER=1 nur fuer die Robustheit; Bilder belegt das Dossier (Framedump).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r35_zgirl_5090.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r35_zgirl_5090: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r35_zgirl_5090: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r35c_5090_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r35c_5090_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

re15_start_spiel(_rv 300
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=5090@gp "RE15_PLAYER_POS=500,-9175,2048,0"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=A0.1,W1,U12"
    RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log
    "RE15_EXIT_AT=900#5090"
    "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")

set(_f "")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r35_zgirl_5090: kein debug.log (exit=${_rv})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
string(FIND "${_dbg}" "DOOR FIRE slot=2 " _p_tuer)
if(_p_tuer LESS 0 OR NOT _dbg MATCHES "DOOR FIRE slot=2 [^\n]*target_cut=14 ")
    string(APPEND _f "\n Tuer 2 -> Cut 14 nicht gefeuert")
else()
    string(SUBSTRING "${_dbg}" ${_p_tuer} -1 _nach)
    if(NOT _nach MATCHES "Sce_em_set type=0x36 ")
        string(APPEND _f "\n kein zweiter Sce_em_set type=0x36 nach der Tuer (Wiedereintritt fehlt)")
    endif()
endif()
if(NOT _dbg MATCHES "EXIT_AT: Bild 900 in Raum 5090")
    string(APPEND _f "\n exe erreichte Bild 900 in ROOM5090 nicht (exit=${_rv})")
endif()

if(NOT EXISTS "${WORKDIR}/state.log")
    string(APPEND _f "\n kein state.log")
else()
    # Nur der Raum-Abschnitt (state.log beginnt nach dem Sprung neu bei F1, davor liegt der Vorspann).
    file(STRINGS "${WORKDIR}/state.log" _zeilen REGEX "t=36 st=")
    list(LENGTH _zeilen _n)
    if(_n LESS 600)
        string(APPEND _f "\n nur ${_n} Bilder mit dem Boss in state.log")
    else()
        set(_hp600 0)
        set(_tot 0)
        set(_xmax -99999)
        set(_letzte_hp "")
        foreach(_z IN LISTS _zeilen)
            string(REGEX MATCH "t=36 st=[^]]*@\\((-?[0-9]+),(-?[0-9]+),r[0-9]+\\)\\] hp=(-?[0-9]+)" _m "${_z}")
            if(NOT _m)
                continue()
            endif()
            set(_x "${CMAKE_MATCH_1}")
            set(_hp "${CMAKE_MATCH_3}")
            set(_letzte_hp "${_hp}")
            if(_hp EQUAL 600)
                set(_hp600 1)
            elseif(_hp600 AND _hp LESS 0)
                set(_tot 1)
            endif()
            if(_hp600 AND _x GREATER _xmax)
                set(_xmax "${_x}")
            endif()
        endforeach()
        if(NOT _letzte_hp EQUAL 600)
            string(APPEND _f "\n Boss-HP am Ende ${_letzte_hp}, erwartet 600 (Konstruktor @0x801003fc nach dem Neuspawn)")
        endif()
        if(_tot)
            string(APPEND _f "\n Boss-HP fiel nach 600 unter 0 (Tod ohne Treffer)")
        endif()
        if(_xmax LESS -7500)
            string(APPEND _f "\n Boss laeuft nicht an (x max ${_xmax}, Start -9000 @0x801011d0)")
        endif()
        message(STATUS "r35_zgirl_5090: ${_n} Bilder, Boss-HP am Ende ${_letzte_hp}, x max ${_xmax}")
    endif()
endif()

if(_f)
    message(FATAL_ERROR "r35_zgirl_5090: FEHLER${_f}")
endif()
message(STATUS "r35_zgirl_5090: alle Pruefungen gruen")
