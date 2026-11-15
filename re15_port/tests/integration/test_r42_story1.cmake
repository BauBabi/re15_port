# =============================================================================
# Runde 42 "story" (v0.8.31), Stufe STAGE1 - DURCHLAEUFE der Szene vor der Leiter 11A0/11A1 in der echten exe (RE15_WEGPLAN:
# Gehen, Blick, Aktionstaste wie der Spieler; Raumwechsel nur ueber die Tueren/Leitern des Spiels). Konstanten/Belege
# include/re15_story1_r42.h, Code engine/src/story1_r42.c. PORT-STORY nach dem Konzept (3.2 L3, 4.2 E2, 6b; K-07, K-08, F-26).
#
# Zustand = das Ende der Stufe STAGE2 (integration_r42_story2_kette/_kette_elza): Glied 1 gibt in 20Ax die Flags
# 9:90 9:111 9:112 9:113 9:118 9:119 3:245 aus, Glied 2 setzt 3:206 (20Ax sub10), 3:151 + 9:121 (11Ax schliesst auf) - genau diese
# Flags; die Uhr (Bit 90) liegt dann im Inventar (RE15_GIVE=63:1, Item 0x3F). Weiter wie Glied 2 nach der Abkuerzung: von der
# 207x-Tuer (Slot 4 @0x0AD6/@0x0AFA) nach 11Ax, im unteren Gang zur Kante, Aktion (hinaufsteigen) -> Szene; danach hinab und wieder
# hinauf (keine zweite Szene), durch den Abschnitt zur Leiter (Slot 1 @0x00F7C) -> 300x.
# Der volle Weg vom Einstieg in 208x bis hierher laeuft in integration_r42_story2_kette/_kette_elza (Glied 2 geht dort weiter bis
# 300x, mit dem Inventar aus Glied 1).
#
# TEIL leiter            Leon mit Uhr: Szene L3 (Marvin), Zeilen 3..6, keine Rueckgabe (F-19: Leon behaelt die Uhr)
# TEIL leiter_elza       Elza mit Uhr: Szene E2 mit Rueckgabe (6b, F-26), Zeilen 3..10, Uhr aus dem Inventar
# TEIL leiter_elza_ohne  Elza ohne Uhr (Bit 90 = 0, nichts im Inventar): Szene E2 ohne die Zeilen 6..9
# In allen: Mod-Begleiter in 11Ax verworfen (K-07), Bit 120 = 1, Abgang (Nachbesserung 1): Begleiter an die Kante, Abblende,
#   >= 30 Bilder Schwarz, oben im Abschnitt in Kamera 2 Richtung Leiter, erst in Kamera 3 entfernt, nie im Kanten-Sub; 300x unveraendert:
#   Vergleich mit dem Stand vor der Stufe (fa2d88eb1, dieselbe Route ab 207x) - dieselben Spawns (RE15_SPAWN_DIAG) und dieselbe
#   Zeilenfolge (3000: 2..8, 3001: 2..9).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -DTEIL=<teil> -P test_r42_story1.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r42_story1: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r42_story1: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL leiter)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/r42_leiter_pruefung.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r42s1_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r42s1_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

function(_fehler _was)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r42_story1[${TEIL}]${_was}")
endfunction()

set(_leon RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
set(_elza RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
          RE15_PSELECT_AUTO_SWITCH=1)
set(_ende_stage2 "9:90,9:111,9:112,9:113,9:118,9:119,3:245,3:206,3:151,9:121")

if(TEIL STREQUAL "leiter")
    set(S 0)
    set(_wer ${_leon} RE15_GIVE=63:1)
    set(_flags "${_ende_stage2}")
    set(_uhr 1)
elseif(TEIL STREQUAL "leiter_elza")
    set(S 1)
    set(_wer ${_elza} RE15_GIVE=63:1)
    set(_flags "${_ende_stage2}")
    set(_uhr 1)
elseif(TEIL STREQUAL "leiter_elza_ohne")
    set(S 1)
    set(_wer ${_elza})
    string(REPLACE "9:90," "" _flags "${_ende_stage2}")
    set(_uhr 0)
else()
    _fehler(": unbekannter TEIL")
endif()

set(WORKDIR "${_basis}_${TEIL}")
file(MAKE_DIRECTORY "${WORKDIR}")
r42_leiter_plan(_weg ${S})
file(WRITE "${WORKDIR}/wegplan_plan.txt" "RAUM 207${S}
WARTE 30
GEHE -18471 -19256 150
BLICK 2048
WARTE 10
AKTION
${_weg}ENDE
")
re15_start_spiel(_rv 1200 ${_wer} RE15_DEBUG_JUMP=207${S}@250 "RE15_SET_FLAG=${_flags}" "RE15_PLAYER_POS=-17000,-19256,2048,0"
                 "RE15_WEGPLAN=${WORKDIR}/wegplan_plan.txt" RE15_WEGPLAN_LOG=wegplan.txt RE15_STATE_LOG=state.log RE15_SPAWN_DIAG=1
                 "RE15_EXIT_AT=1100#300${S}" "${_exe_kopie}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    _fehler(": kein debug.log (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
set(_wp "")
if(EXISTS "${WORKDIR}/wegplan.txt")
    file(READ "${WORKDIR}/wegplan.txt" _wp)
endif()
r42_leiter_pruefen("${_dbg}" "${_wp}" ${S} ${_uhr} _bericht)
file(REMOVE "${_exe_kopie}")
message(STATUS "r42_story1[${TEIL}]: ${_bericht}")
