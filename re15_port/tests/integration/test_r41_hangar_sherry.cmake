cmake_minimum_required(VERSION 3.16)   # cmake -P: Policies wie die aelteren Integrationsskripte (Memory reai-v2-linux-test-portabilitaet)
# =============================================================================
# HANGAR-SZENE: BIRKIN RUFT EINMAL "SHERRY" (Runde 1 "sherry_ruf", Zweig r41/sichel_szene).
#
# Nutzer 2026-11-16: "Ah, und ich moechte das der 4 Beiner nicht mehr "Sherry" ruft, denn das ist wirklich weird! Allerdings kann
# 2 Beiner Birkin mit Schwanz waehrend er Richtung Kamera laeuft nachdem er den Spiderman weg gehauen hat noch mal Sherry rufen."
# NUTZERWUNSCH (Gestaltung, die Szene gibt es nur im 80%-Video): im ersten Gangtakt (Videobild h119 = BF_H_GANG, WALK-Clip 1 Bild 16)
# ruft die Szene re15_bf_ruf_hook -> re15_audio_birkin_ruf = Raum-Ton 10 aus der snd1-Bank von STAGE3/ROOM3070.RDT
# (EDT-Satz 10 `00 00 b2 16`, VAG 12 sha1 d19adfc8010d = 40-%-Disc ROOM3070 VAG 12 @0x16640; der 0x30 spielt ihn ueber
# FUN_800453d0(10), STAGE3 @0x8011a744/48). Belege: birkin_bahnsteig.c bb_bild_ton, audio_pc.c re15_audio_birkin_ruf.
#
# Je Figur (Leon 50A0, Elza 50A1) ein Lauf mit Ton (RE15_AUDIO_CAP_SYNC: Audioschicht aktiv, kein Geraet). Seit dem Merge in
# r42/hangar_aussen startet die Szene nur beim Betreten durch die Nordtuer aus dem Monitorraum (Lauf wie integration_r42_hangar_aussen
# Lauf 2 ohne 3:73: Debug-Sprung 5030 an die Tuer, Aktionstaste):
#   verlangt  "Szene Start" < genau EIN "[hangar] ROOM50A? Sherry-Ruf h119" < "Szene Ende";
#             "[birkinruf] STAGE3/ROOM3070.RDT snd1 geladen: Satz 10 = 00 00 b2 16" (die Bank ist da, der Satz ist der Disc-Satz);
#             Messzeile "[hangar-takt] h119 ... B clip 1 bild 16" (Birkin geht in diesem Takt los).
# Bild fuer Bild unveraendert (gemessen, scratchpad sherry_ruf/BERICHT_r1.md): state.log und alle [hangar-takt]-Zeilen bitgleich zu
# d413862a4 (r42/hangar_aussen vor dem Merge); im Mitschnitt liegt die Welle genau einmal, im alten Stand nie.
# GEGENPROBE (gemessen): exe von d413862a4 -> ROT (keine Sherry-Ruf-Zeile, keine Bank-Zeile).
#
# Testhaken (kein Spielverhalten): RE15_SET_FLAG, RE15_DEBUG_JUMP, RE15_PLAYER_POS, RE15_INPUT_SCRIPT, RE15_HANGAR_LOG,
# RE15_AUDIO_CAP_SYNC, RE15_STATE_LOG, RE15_EXIT_AT.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_hangar_sherry.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_hangar_sherry: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_hangar_sherry: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41hangarsherry_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41hangarsherry_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# Binaer-sicheres Lesen (debug.log enthaelt Binaerbytes): Position des Textes oder -1 / Anzahl.
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
set(_weg "W1,A0.2,W25")   # an der Nordtuer des Monitorraums: Aktionstaste -> Gang -> Szene (integration_r42_hangar_aussen Lauf 2)

function(figur _name _raum _extra)
    string(TOLOWER "${_raum}" _rl)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_g "")
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_HANGAR_LOG=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        "RE15_AUDIO_CAP_SYNC=${WORKDIR}/ton.raw"
        RE15_SET_FLAG=8:42,8:43
        RE15_DEBUG_JUMP=5030@20
        RE15_PLAYER_POS=-15300,-26300,1024
        "RE15_INPUT_SCRIPT=${_weg}"
        RE15_STATE_LOG=state.log
        "RE15_EXIT_AT=500#${_rl}"
        "${_exe_kopie}")
    set(_d "${WORKDIR}/debug.log")
    _pos("${_d}" "[hangar] ROOM${_raum} Szene Start" _p1)
    _pos("${_d}" "[hangar] ROOM${_raum} Sherry-Ruf h119 " _p2)
    _pos("${_d}" "[hangar] ROOM${_raum} Szene Ende" _p3)
    _zahl("${_d}" "Sherry-Ruf h" _n)
    _pos("${_d}" "[birkinruf] STAGE3/ROOM3070.RDT snd1 geladen: Satz 10 = 00 00 b2 16" _bank)
    _pos("${_d}" "[hangar-takt] h119 F" _takt)
    set(_kette "Start ${_p1} Ruf ${_p2} Ende ${_p3}, Rufe ${_n}, Bank ${_bank}, exit=${_rv}, ${WORKDIR}")
    if(_p1 LESS 0 OR _p2 LESS _p1 OR _p3 LESS _p2 OR NOT _n EQUAL 1)
        string(APPEND _g "\n ${_name}: Sherry-Ruf nicht genau einmal zwischen Szenenstart und -ende im Gangtakt h119 (${_kette})")
    endif()
    if(_bank LESS 0)
        string(APPEND _g "\n ${_name}: Ruf-Bank ROOM3070 snd1 Satz 10 (00 00 b2 16) nicht geladen (${_kette})")
    endif()
    if(EXISTS "${_d}")
        file(STRINGS "${_d}" _tz REGEX "^\\[hangar-takt\\] h119 F[0-9]+ B ")
    else()
        set(_tz "")
    endif()
    if(NOT _tz MATCHES " B clip 1 bild 16 ")
        string(APPEND _g "\n ${_name}: im Takt h119 geht Birkin nicht los (Messzeile '${_tz}', Soll 'B clip 1 bild 16')")
    endif()
    message(STATUS "r41_hangar_sherry ${_name}: ${_kette}")
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

figur(leon 50A0 "")
figur(elza 50A1 "RE15_TITLE_SHOT_CHAR=1")

file(REMOVE "${_exe_kopie}")
if(NOT _f STREQUAL "")
    message(FATAL_ERROR "r41_hangar_sherry: ROT${_f}")
endif()
message(STATUS "r41_hangar_sherry: GRUEN (Leon 50A0 und Elza 50A1: genau ein Sherry-Ruf im Gangtakt h119, Bank ROOM3070 Satz 10)")
