# =============================================================================
# Runde 41 Spur G "kanister2080" — die echte exe (Dossier analysis/befunde_runde41/G_kanister2080.md).
#
# Lauf "ankunft"  (Leon) ROOM2080 per Sprung mit Bit 110 (Spur F: 2090-Szene gesehen) und (3,245): der Installer spawnt die 8
#                 Zombies (Sce_em_set Typ 0x16 Etage 8, y -14400) und spielt beim ersten Betreten den Tuer-Ton (DOOR06 Satz 1,
#                 Door_exit @0x800141f0), Bit 114.
# Lauf "kanister" (Leon) ROOM2080 OHNE Bit 110 in Cut 5 (= Gas_Can.bmp): kein Zombie, der Kanister steht an der Reling - die
#                 Messzeile des Zeichners traegt Lage (-6745,-14400,-17625) und den Bildkasten in Cut 5, der die Markenmitte
#                 (167,156) enthaelt und vom Fuss (y ~155) bis ueber y 80 reicht.
# Lauf "szene"    (Leon) ROOM2090 mitten im Kampf (RE15_GB_TEST=5), Bit 110, Messhaken RE15_K2080_GATOR_HP=24990 (Schwelle
#                 HP < 25000 mit EINEM echten Pistolentreffer statt 43): Ausloeser -> ROOM2080 (Marvin 0x40, 8 Zombies) ->
#                 Tuer-Ton -> 8 Kopfschuesse mit 8 x "Kopf ab" (RE2 FUN_801092C4) -> Aufheben -> Wurf (RE2 ROOM7020 sub13) ->
#                 Aufschlag -> Rueckkehr ROOM2090 (Tot-Bits 8 von 8) -> Kanister im Becken; Schnitte in 2080 in der Folge
#                 2 -> 5 -> 7 (state.log cam=), Nachricht 42 im Bild (state.log msg id=42).
# Lauf "elza"     dasselbe mit Elza (Spielerwahl RE15_PSELECT_AUTO_SWITCH): ROOM2091 -> ROOM2081, John 0x49.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_kanister2080.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_kanister2080: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_kanister2080: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
# EIGENER EXE-NAME (Muster test_r34n_b_cursor.cmake): nur die eigene Kopie wird geloescht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe_kopie "${_exe_dir}/re15_pc_r41g_${_lauf_id}.exe")
if(NOT WIN32)
    set(_exe_kopie "${_exe_dir}/re15_pc_r41g_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

function(r41g_fehler _text)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_kanister2080: ${_text}")
endfunction()

set(_basisenv RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore)
set(_leon RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
set(_elza RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)

# ---------------------------------------------------------------- ankunft (echter Tuerweg 2090 -> 2080, Ton gemischt)
set(WORKDIR "${_basis}/ankunft")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rv 300 RE15_NO_INTRO=1 RE15_AUDIO_CAP_SYNC=cap.raw RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore ${_leon}
    RE15_DEBUG_JUMP=2090@250 "RE15_SET_FLAG=9:110,3:245" RE15_KEIN_GATOR=1 "RE15_PLAYER_POS=-8900,-25244,2048"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=W1.0,A0.2,W6.0"
    RE15_SPAWN_DIAG=1 "RE15_EXIT_AT=90#2080" "${_exe_kopie}")
file(REMOVE "${WORKDIR}/cap.raw")
if(NOT _rv EQUAL 0)
    r41g_fehler("Lauf ankunft exe exit=${_rv}")
endif()
file(STRINGS "${WORKDIR}/debug.log" _td REGEX "\\[tuer\\] Sequenz Archiv [0-9]+ DOOR16 ")
if(NOT _td)
    r41g_fehler("Lauf ankunft: kein Tuerweg 2090 -> 2080 (Leiter DOOR16, Tuer Slot 0 @ROOM2090 0x0996)")
endif()
file(STRINGS "${WORKDIR}/debug.log" _sp REGEX "\\[k2080\\] F[0-9]+ ROOM2080 nach Bit 110: 8 lebende Zombies \\(Tot-Bits 236\\.\\.243\\)")
if(NOT _sp)
    r41g_fehler("Lauf ankunft: keine Zeile 'ROOM2080 nach Bit 110: 8 lebende Zombies'")
endif()
file(STRINGS "${WORKDIR}/debug.log" _tt REGEX "\\[k2080\\] F[0-9]+ Tuer-Ton DOOR06 Satz 1 .*erstes Betreten nach Bit 110, Bit 114 = 1")
file(STRINGS "${WORKDIR}/debug.log" _tb REGEX "\\[k2080\\] F[0-9]+ Tuer-Ton: DOOR06-Tonteil 19608 B in der Tuerbank, Satz 1")
file(STRINGS "${WORKDIR}/debug.log" _ts REGEX "\\[k2080\\] F[0-9]+ Tuer-Ton: [1-8] SE-Stimmen aktiv")
if(NOT _tt OR NOT _tb OR NOT _ts)
    r41g_fehler("Lauf ankunft: Tuer-Ton nicht gespielt (Anstoss ${_tt} / Tuerbank ${_tb} / Stimmen ${_ts})")
endif()
file(STRINGS "${WORKDIR}/debug.log" _em REGEX "\\[spawn-diag\\] Sce_em_set type=0x16 behavior=0x02 slot=[0-7] pos=\\(-?[0-9]+,-14400,-?[0-9]+\\)")
list(LENGTH _em _n_em)
if(NOT _n_em EQUAL 8)
    r41g_fehler("Lauf ankunft: ${_n_em} Zombie-Spawns auf Band 8 statt 8")
endif()
message(STATUS "r41_kanister2080 ankunft: Tuerweg 2090 -> 2080, Tuer-Ton gemischt (${_ts}), 8 Zombies Typ 0x16 auf y -14400")

# ---------------------------------------------------------------- kanister
set(WORKDIR "${_basis}/kanister")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rv 300 ${_basisenv} ${_leon}
    RE15_DEBUG_JUMP=2080@250 "RE15_PLAYER_POS=-12000,-24000,0,8" RE15_FORCE_CUT=5 RE15_SPAWN_DIAG=1
    "RE15_EXIT_AT=40#2080" "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    r41g_fehler("Lauf kanister exe exit=${_rv}")
endif()
file(STRINGS "${WORKDIR}/debug.log" _em REGEX "\\[spawn-diag\\] Sce_em_set type=0x16")
if(_em)
    r41g_fehler("Lauf kanister: Zombies ohne Bit 110")
endif()
file(STRINGS "${WORKDIR}/debug.log" _kz REGEX "\\[k2080\\] F[0-9]+ Kanister pos=\\(-6745,-14400,-17625\\) Cut 5 Kasten=")
if(NOT _kz)
    r41g_fehler("Lauf kanister: keine Zeichner-Zeile des Kanisters in Cut 5")
endif()
list(GET _kz -1 _k)
if(NOT _k MATCHES "Kasten=\\((-?[0-9]+)\\.\\.(-?[0-9]+),(-?[0-9]+)\\.\\.(-?[0-9]+)\\) Dreiecke ([0-9]+)")
    r41g_fehler("Lauf kanister: Zeile unlesbar: ${_k}")
endif()
set(_x0 ${CMAKE_MATCH_1})
set(_x1 ${CMAKE_MATCH_2})
set(_y0 ${CMAKE_MATCH_3})
set(_y1 ${CMAKE_MATCH_4})
set(_tri ${CMAKE_MATCH_5})
if(_x0 GREATER 167 OR _x1 LESS 167 OR _y1 LESS 150 OR _y0 GREATER 80 OR _tri LESS 30)
    r41g_fehler("Lauf kanister: Kasten ${_x0}..${_x1},${_y0}..${_y1} (${_tri} Dreiecke) deckt die Marke (167,156) nicht vom Fuss bis y 80")
endif()
message(STATUS "r41_kanister2080 kanister: Cut 5 Kasten ${_x0}..${_x1},${_y0}..${_y1}, ${_tri} Dreiecke, keine Zombies")

# ---------------------------------------------------------------- szene (Leon) / elza
foreach(_wer leon elza)
    set(WORKDIR "${_basis}/szene_${_wer}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    if(_wer STREQUAL "leon")
        set(_wahl ${_leon})
        set(_r80 2080)
        set(_r90 2090)
        set(_npc "Marvin 0x40")
    else()
        set(_wahl ${_elza})
        set(_r80 2081)
        set(_r90 2091)
        set(_npc "John 0x49")
    endif()
    if(_wer STREQUAL "leon")
        set(_tonenv RE15_NO_INTRO=1 RE15_AUDIO_CAP_SYNC=cap.raw RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore)
    else()
        set(_tonenv ${_basisenv})
    endif()
    re15_start_spiel(_rv 600 ${_tonenv} ${_wahl}
        RE15_DEBUG_JUMP=2090@250 "RE15_SET_FLAG=9:110,3:245" RE15_GB_TEST=5 "RE15_PLAYER_POS=6500,-25000,3072"
        "RE15_GIVE=3:15" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=W2.0,M0.3,MA0.1,M0.3,MA0.1,W1.0"
        RE15_STATE_LOG=state.log "RE15_EXIT_AT=120#${_r90}" "${_exe_kopie}")
    file(REMOVE "${WORKDIR}/cap.raw")
    if(NOT _rv EQUAL 0)
        r41g_fehler("Lauf szene_${_wer} exe exit=${_rv}")
    endif()
    if(_wer STREQUAL "leon")
        file(STRINGS "${WORKDIR}/debug.log" _as REGEX "\\[k2080\\] F[0-9]+ Aufschlag: [1-8] SE-Stimmen aktiv")
        file(STRINGS "${WORKDIR}/debug.log" _ts REGEX "\\[k2080\\] F[0-9]+ Tuer-Ton: [1-8] SE-Stimmen aktiv")
        if(NOT _as OR NOT _ts)
            r41g_fehler("Lauf szene_leon: Aufschlag-/Tuer-Ton nicht gemischt (${_as} / ${_ts})")
        endif()
    endif()
    foreach(_muster
            "AUSLOESER geschwaecht .*-> ROOM${_r80}"
            "ROOM${_r80} SZENE \\(${_npc}, 8 lebende Zombies"
            "Tuer-Ton DOOR06 Satz 1 \\(Door_exit @0x800141f0\\) - Szene"
            "Kanister aufgehoben"
            "Wurf: Start .* -> Landung \\(-?[0-9]+,-?[0-9]+,-?[0-9]+\\) nach [0-9]+ Bildern"
            "Aufschlag im Wasser"
            "ROOM${_r90} RUECKKEHR: .*Zone-7-Tot-Bits 236\\.\\.243 gesetzt: 8 von 8"
            "ROOM${_r90} Kanister im Becken")
        file(STRINGS "${WORKDIR}/debug.log" _z REGEX "\\[k2080\\] F[0-9]+ ${_muster}")
        if(NOT _z)
            r41g_fehler("Lauf szene_${_wer}: keine Zeile '${_muster}'")
        endif()
    endforeach()
    file(STRINGS "${WORKDIR}/debug.log" _ka REGEX "\\[k2080\\] F[0-9]+ Kopf ab: Slot [0-9]+ \\(Part 8 Flagwort 0x4[A-F]")
    list(LENGTH _ka _n_ka)
    file(STRINGS "${WORKDIR}/debug.log" _sch REGEX "\\[k2080\\] F[0-9]+ Schuss [0-9]+: Treffer Slot [0-9]+ .*hp=-1 state=3 zeile=5")
    list(LENGTH _sch _n_sch)
    if(NOT _n_ka EQUAL 8 OR NOT _n_sch EQUAL 8)
        r41g_fehler("Lauf szene_${_wer}: ${_n_sch} Kopfschuesse (zeile 5) und ${_n_ka} x Kopf ab statt je 8")
    endif()
    # Schnittfolge in 2080 (Spieler auf dem Parkplatz -23200,-16700): erste Bilder je Cut
    file(STRINGS "${WORKDIR}/state.log" _st REGEX "PL\\(-23200,-16700,")
    set(_folge "")
    set(_letzt "")
    set(_msg42 0)
    foreach(_l IN LISTS _st)
        if(_l MATCHES " cam=([0-9]+) ")
            set(_c ${CMAKE_MATCH_1})
            if(NOT _c STREQUAL _letzt)
                string(APPEND _folge "${_c} ")
                set(_letzt ${_c})
            endif()
        endif()
        if(_l MATCHES "id=42 ")
            set(_msg42 1)
        endif()
    endforeach()
    if(NOT _folge MATCHES "^2 5 7 ")
        r41g_fehler("Lauf szene_${_wer}: Schnittfolge in ${_r80} '${_folge}' statt '2 5 7'")
    endif()
    if(NOT _msg42)
        r41g_fehler("Lauf szene_${_wer}: Nachricht 42 nie im Bild (state.log msg id=42)")
    endif()
    message(STATUS "r41_kanister2080 szene_${_wer}: ${_npc}, 8 Kopfschuesse, Schnitte '${_folge}', Nachricht 42, Kanister im Becken")
endforeach()

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_kanister2080: alle Laeufe gruen")
