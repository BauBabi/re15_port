cmake_minimum_required(VERSION 3.16)   # cmake -P: ohne diese Zeile gelten im Linux-Container (CMake 3.28) die alten Policies (IN_LIST, if(TRUE)) - Muster der aelteren Integrationsskripte, Memory reai-v2-linux-test-portabilitaet
# =============================================================================
# HANGAR: BIRKIN BLEIBT LIEGEN (Runde 1 "hangar_liegen", Zweig r41/sichel_szene; Kopf include/re15_birkin_formen.h).
#
# Nutzer 2026-11-16 zum Vorschlag "Zusammenbruch, aufstehen, weglaufen": "Das er aufsteht und wegrennt ist daemlich. Besser er bleibt
# einfach liegen..."
# Original (40-%-Disc STAGE5.BIN, selbst disassembliert): Todesphase 3 @0x8011aed4 wartet auf grid & 0xF == 2 (`lbu v0,9(a0)` /
# `andi v0,v0,0xf` / `bne` @0x8011aee0-ec) ohne anim_set - im Hangar (grid 0x10) kommt das Signal nie: Birkin liegt im letzten Bild
# von Clip 9. Das bleibt unveraendert. Neu ist nur der Merker "Hangar-Birkin besiegt" (Bank-9-Bit RE15_BF_BESIEGT_BIT = 39, PORT-WAHL),
# gesetzt beim Eintritt in den Tod (Phase 0 ausgefuehrt, +0x7 = 1 @0x8011ae38), gelesen beim Betreten: kein Birkin, keine Szene.
# Reihenfolge-Luecke: Bank 3 Bit 0x49 (Kraftraum-Birkin besiegt, ROOM50E0 sub01 @0x00B1A / 50F1 @0x00A04) -> ebenso leer.
#
# Je Figur (Leon 50A0, Elza 50A1) vier Prozesse auf EINER Karte:
#   Lauf 1  Debug-Sprung 50A0, Weg nach Sueden -> Szene; Pistole (RE15_GIVE/RE15_EQUIP), Birkins HP per Messhaken
#           RE15_HANGAR_BIRKIN_HP=1 (sonst 600 HP - der Lauf soll den Tod erreichen, nicht den Kampf ausspielen); Schuesse; Speichern
#           in Bild 1000. Verlangt: Szene Start/Ende, dann "Birkin besiegt"; ab dem ersten Bild in Phase 3 (ss3=3) bis zum Speichern
#           jede state-Zeile mit genau EINEM Typ 0x30 in st=3 ss3=3 mo=9, gleiches Bild, gleiche Lage und Gier (liegt, steht nicht auf).
#   Lauf 2  NEUER Prozess, CONTINUE aus Slot 0 (im Hangar geladen), Debug-Sprung in den Hangar (= erneutes Betreten), Weg nach Sueden.
#           Verlangt: keine Szene, kein Typ 0x30/0x2B, zweimal "[hangar] ROOM50A? ohne Birkin (hier besiegt 1, Kraftraum besiegt 0)".
#   Lauf 3  CONTINUE, Debug-Sprung in den Kraftraum (50E0 / Elza 50F0 -> 50F1). Verlangt: dort ein Typ 0x30 (Kraftraum unveraendert).
#   Lauf 4  Neues Spiel mit Bank 3 Bit 0x49 = 73, Hangar wie Lauf 1 ohne Schuesse. Verlangt: weder Szene noch Birkin.
# GEGENPROBE (gemessen, scratchpad hangar_liegen/BERICHT_r1.md): exe von 53fa8a984 -> ROT (kein Merker; nach dem Laden steht Birkin
# wieder da).
#
# Testhaken (kein Spielverhalten): RE15_HANGAR_BIRKIN_HP, RE15_GIVE, RE15_EQUIP, RE15_SET_FLAG, RE15_SAVE_TEST_AGAIN / RE15_CARD_AUTO /
# RE15_CARD_SLOT / RE15_SAVE_TEST_EXIT_AFTER, RE15_CONTINUE_TEST, RE15_DEBUG_JUMP, RE15_INPUT_SCRIPT(_BASIS), RE15_STATE_LOG,
# RE15_HANGAR_LOG.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_hangar_liegen.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_hangar_liegen: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_hangar_liegen: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41hangarliegen_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41hangarliegen_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# Binaer-sicheres Lesen (debug.log enthaelt Binaerbytes - test_save_counter_pin.cmake): Position des Textes oder -1.
function(_pos _datei _text _out)
    set(${_out} -1 PARENT_SCOPE)
    if(NOT EXISTS "${_datei}")
        return()
    endif()
    file(READ "${_datei}" _lh HEX)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    string(FIND "${_lh}" "${_h}" _p)
    set(${_out} ${_p} PARENT_SCOPE)
endfunction()
function(_zahl _datei _text _out)
    set(${_out} 0 PARENT_SCOPE)
    if(NOT EXISTS "${_datei}")
        return()
    endif()
    file(READ "${_datei}" _lh HEX)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(_n 0)
    string(FIND "${_lh}" "${_h}" _p)
    while(NOT _p LESS 0)
        math(EXPR _n "${_n} + 1")
        math(EXPR _s "${_p} + 2")
        string(SUBSTRING "${_lh}" ${_s} -1 _lh)
        string(FIND "${_lh}" "${_h}" _p)
    endwhile()
    set(${_out} ${_n} PARENT_SCOPE)
endfunction()

set(_f "")
# Weg wie integration_r41_hangar_laden (angeforderter Schnitt 4 -> Szene), dann 10 Schuesse (M = R1, A = Quadrat).
set(_weg "W0.5,UX3.6,W0.3,L0.27,W0.3,UX2.5")
set(_schuss "W8,M0.4,MA0.1,M0.4,MA0.1,M0.4,MA0.1,M0.4,MA0.1,M0.4,MA0.1,M0.4,MA0.1,M0.4,MA0.1,M0.4,MA0.1,M0.4,MA0.1,M0.4,MA0.1")

function(figur _name _raum _kraftsprung _extra)
    string(TOLOWER "${_raum}" _rl)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    set(_g "")

    # --- Lauf 1: Szene, Kampf, Tod, Liegen, Speichern ----------------------------------------------------------------------------
    re15_start_spiel(_rv1 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_HANGAR_LOG=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_SET_FLAG=8:42,8:43
        RE15_GIVE=3:60 RE15_EQUIP=3 RE15_HANGAR_BIRKIN_HP=1
        RE15_DEBUG_JUMP=50A0@20
        RE15_PLAYER_POS=-12406,4381,1024
        "RE15_INPUT_SCRIPT=${_weg},${_schuss},W30"
        RE15_STATE_LOG=state1.log
        RE15_SAVE_TEST_AGAIN=1000 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_SAVE_TEST_EXIT_AFTER=1
        "${_exe_kopie}")
    if(EXISTS "${WORKDIR}/debug.log")
        file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug1.log")
    endif()
    set(_d1 "${WORKDIR}/debug1.log")
    _pos("${_d1}" "[hangar] ROOM${_raum} Szene Start" _p1)
    _pos("${_d1}" "[hangar] ROOM${_raum} Szene Ende" _p2)
    _pos("${_d1}" "[hangar] ROOM${_raum} Birkin besiegt" _p3)
    _pos("${_d1}" "[save] saved (room ${_rl})" _p4)
    set(_kette "Start ${_p1} Ende ${_p2} besiegt ${_p3} gespeichert ${_p4}")
    set(_lauf1_ok 1)
    if(_p1 LESS 0 OR _p2 LESS _p1 OR _p3 LESS _p2 OR _p4 LESS 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        string(APPEND _g "\n ${_name}: Lauf 1 - Ablauf unvollstaendig oder in falscher Reihenfolge (${_kette}, exit=${_rv1}, ${WORKDIR})")
        set(_lauf1_ok 0)
    endif()
    # state1.log: ab dem ersten Bild in Phase 3 liegt genau ein 0x30 unveraendert (st=3 ss3=3 mo=9, gleiches Bild/Lage/Gier)
    file(STRINGS "${WORKDIR}/state1.log" _zl1 REGEX "^F[0-9]+ ")
    set(_liegt_ab "-")
    set(_liegt_n 0)
    set(_abw 0)
    set(_abw_z "")
    set(_bezug "")
    foreach(_z IN LISTS _zl1)
        string(REGEX MATCHALL "\\[[0-9]+ t=30 [^]]*\\]" _bk "${_z}")
        list(LENGTH _bk _nb)
        if(_bezug STREQUAL "")
            if(_nb EQUAL 1 AND _bk MATCHES " st=3 ss1=[0-9]+ ss2=[0-9]+ ss3=3 g=[0-9a-f]+ mo=9 ")
                # Bezug ohne Zaehler-/Abstandsfelder (stun, d): Bild, Lage, Gier
                string(REGEX REPLACE " stun=-?[0-9]+ d=[0-9]+ " " " _bezug "${_bk}")
                string(REGEX MATCH "^F([0-9]+)" _m "${_z}")
                set(_liegt_ab ${CMAKE_MATCH_1})
                set(_liegt_n 1)
            endif()
        else()
            string(REGEX REPLACE " stun=-?[0-9]+ d=[0-9]+ " " " _jetzt "${_bk}")
            if(NOT _nb EQUAL 1 OR NOT _jetzt STREQUAL _bezug)
                math(EXPR _abw "${_abw} + 1")
                if(_abw_z STREQUAL "")
                    string(SUBSTRING "${_z}" 0 40 _kopf)
                    set(_abw_z "${_kopf} ... ${_bk}")
                endif()
            else()
                math(EXPR _liegt_n "${_liegt_n} + 1")
            endif()
        endif()
    endforeach()
    if(_liegt_n LESS 300 OR _abw GREATER 0)
        string(APPEND _g "\n ${_name}: Lauf 1 - Birkin liegt nicht still (ab F${_liegt_ab} ${_liegt_n} Bilder liegend, ${_abw} abweichend; erste: ${_abw_z})")
    endif()

    set(_leer "-")
    set(_n3 "-")
    set(_rv2 "-")
    set(_rv3 "-")
    if(_lauf1_ok)
    # --- Lauf 2: neuer Prozess, Laden, Hangar erneut betreten ------------------------------------------------------------------
    re15_start_spiel(_rv2 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_HANGAR_LOG=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_DEBUG_JUMP=50A0@60
        RE15_PLAYER_POS=-12406,4381,1024
        RE15_INPUT_SCRIPT_BASIS=spiel
        "RE15_INPUT_SCRIPT=W3|${_weg},W12"
        RE15_STATE_LOG=state2.log
        "RE15_EXIT_AT=500#${_raum}!1"
        "${_exe_kopie}")
    if(EXISTS "${WORKDIR}/debug.log")
        file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug2.log")
    endif()
    set(_d2 "${WORKDIR}/debug2.log")
    _pos("${_d2}" "CONTINUE: resumed in room ${_rl}" _geladen)
    _pos("${_d2}" "AUTO-JUMP -> ROOM50A0" _sprung)
    _pos("${_d2}" "EXIT_AT: Bild 500 in Raum ${_rl}" _ende)
    _zahl("${_d2}" "[hangar] ROOM${_raum} ohne Birkin (hier besiegt 1, Kraftraum besiegt 0)" _leer)
    _pos("${_d2}" "Szene Start" _s2)
    if(_geladen LESS 0 OR _sprung LESS 0 OR _ende LESS 0)
        string(APPEND _g "\n ${_name}: Lauf 2 unvollstaendig (geladen ${_geladen}, Sprung ${_sprung}, Endbild ${_ende}, exit=${_rv2})")
    else()
        if(NOT _leer EQUAL 2)
            string(APPEND _g "\n ${_name}: Lauf 2 - '${_raum} ohne Birkin (hier besiegt 1, ...)' ${_leer}x statt 2x (Laden + Betreten)")
        endif()
        if(NOT _s2 LESS 0)
            string(APPEND _g "\n ${_name}: Lauf 2 - die Hangar-Szene laeuft nach dem Sieg erneut")
        endif()
        file(STRINGS "${WORKDIR}/state2.log" _zl2 REGEX " t=(30|2b) ")
        list(LENGTH _zl2 _n2)
        if(_n2 GREATER 0)
            string(APPEND _g "\n ${_name}: Lauf 2 - ${_n2} state-Zeilen mit Birkin/Spider-Man nach dem Laden/Betreten")
        endif()
    endif()

    # --- Lauf 3: Laden, Kraftraum: dort Birkin unveraendert -----------------------------------------------------------------------
    re15_start_spiel(_rv3 200
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_DEBUG_JUMP=${_kraftsprung}@60
        RE15_STATE_LOG=state3.log
        RE15_EXIT_AT=300
        "${_exe_kopie}")
    if(EXISTS "${WORKDIR}/debug.log")
        file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug3.log")
    endif()
    _pos("${WORKDIR}/debug3.log" "AUTO-JUMP -> ROOM${_kraftsprung}" _sprung3)
    file(STRINGS "${WORKDIR}/state3.log" _zl3 REGEX " t=30 ")
    list(LENGTH _zl3 _n3)
    if(_sprung3 LESS 0 OR _n3 EQUAL 0)
        string(APPEND _g "\n ${_name}: Lauf 3 - im Kraftraum (${_kraftsprung}) kein Birkin nach dem Hangar-Sieg (Sprung ${_sprung3}, Zeilen ${_n3})")
    endif()
    endif()   # _lauf1_ok

    # --- Lauf 4: Reihenfolge-Luecke - Kraftraum-Birkin schon besiegt (Bank 3 Bit 0x49 = 73) ------------------------------------
    re15_start_spiel(_rv4 200
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_HANGAR_LOG=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_SET_FLAG=8:42,8:43,3:73
        RE15_DEBUG_JUMP=50A0@20
        RE15_PLAYER_POS=-12406,4381,1024
        "RE15_INPUT_SCRIPT=${_weg},W8"
        RE15_STATE_LOG=state4.log
        RE15_EXIT_AT=560
        "${_exe_kopie}")
    if(EXISTS "${WORKDIR}/debug.log")
        file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug4.log")
    endif()
    _pos("${WORKDIR}/debug4.log" "[hangar] ROOM${_raum} ohne Birkin (hier besiegt 0, Kraftraum besiegt 1)" _luecke)
    _pos("${WORKDIR}/debug4.log" "Szene Start" _s4)
    file(STRINGS "${WORKDIR}/state4.log" _zl4 REGEX " t=(30|2b) ")
    list(LENGTH _zl4 _n4)
    if(_luecke LESS 0 OR NOT _s4 LESS 0 OR _n4 GREATER 0)
        string(APPEND _g "\n ${_name}: Lauf 4 (Kraftraum besiegt) - Merkerzeile ${_luecke}, Szene ${_s4}, Birkin/Spider-Man-Zeilen ${_n4}")
    endif()

    message(STATUS "r41_hangar_liegen ${_name}: Lauf 1 exit=${_rv1} (${_kette}, liegt ab F${_liegt_ab} ${_liegt_n} Bilder, ${_abw} abweichend), "
                   "Lauf 2 exit=${_rv2} (leer ${_leer}x), Lauf 3 exit=${_rv3} (Kraftraum-Birkin ${_n3} Zeilen), Lauf 4 exit=${_rv4}${_g}")
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

figur(leon 50A0 50E0 "")
figur(elza 50A1 50F0 "RE15_TITLE_SHOT_CHAR=1")

file(REMOVE "${_exe_kopie}")
if(_f)
    message(FATAL_ERROR "r41_hangar_liegen:${_f}")
endif()
message(STATUS "r41_hangar_liegen: OK (Leon 50A0 + Elza 50A1, Laufverzeichnis ${_wurzel})")
