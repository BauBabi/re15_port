# =============================================================================
# HALBTRANSPARENTE RAUM-OBJEKTE MIT DER ECHTEN EXE (Runde 38 Spur W, Nachbesserung 1 — Abnahme-Mangel M1).
#
# Abnahme 0 (analysis/befunde_runde38/W_abnahme_0.md M1): ROOM5060/5061 Cut 4 — der Original-Polygon-Weg zeichnete
# ein Halbviereck der Glasscheibe obj 4 (Attribut 0x0012) DECKEND dunkel; das Original mischt es halbtransparent.
# Behoben mit Parameter 4 der Objekt-Zeichner (ABE = Attribut & 2 @0x8002c2b4), ABR aus der Texpage (MD1-Seite ODER
# (Attribut & 0xC) << 3, FUN_80025940/FUN_80025a98) und der STP-Regel je Texel (engine/src/objabe_r38.c,
# platform/pc/src/objabe_r38_pc.c). Dossier analysis/befunde_runde38/W_rolltor20a0.md §10.
#
# Drei Laeufe mit einer Messkopie der exe (Muster test_r38_rolltor20a0.cmake), Debug-Sprung, Spieler an die Stelle,
# an der das ORIGINAL gemessen wurde (DuckStation, W_belege/orig_abe_*_log.txt), Kamera-Schnitt erzwungen,
# Framedump F300 (RE15_WINDOW_SCALE=1); Werkzeug test_r38_rolltor20a0_abe --fall vergleicht den Mittelwert einer
# Messflaeche mit dem Original-Bildpuffer:
#   5060c4  ABR 0 (B/2+F/2)  Glasscheibe obj 4, Cut 4 — Original (31.6,15.3,26.1); deckend (6.3,0.5,6.6) und
#                            v0.8.25 (47.2,30.5,36.6) liegen ausserhalb der Toleranz 6.
#   20a0c2  ABR 2 (B-F)      Wasser obj 4 (Typ 3), Cut 2 — Original (0.4,0.4,0.3); v0.8.25 deckend (11.8,14.3,11.7).
#   4090c0  ABR 3 (B+F/4)    Nebel obj 0-2, Cut 0 — Original (140.5,139.4,147.9); v0.8.25 (56.5,55.7,55.1).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_ABE_TOOL=<test_r38_rolltor20a0_abe> -DWORKDIR=<dir>
#               -P test_r38_rolltor20a0_abe.cmake
# =============================================================================
foreach(_v RE15_PC_EXE RE15_ABE_TOOL)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r38_rolltor20a0_abe: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "r38_rolltor20a0_abe: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r38w_abe_*.exe" "${_exe_dir}/re15_pc_r38w_abe_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe_kopie "${_exe_dir}/re15_pc_r38w_abe_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_fehler "")

# Ein Lauf: _fall = Messflaeche des Werkzeugs, _raum, _pos = RE15_PLAYER_POS (x,z,rot,band), _cut, ARGN = weitere
# KEY=WERT. Framedump F270..F330 jedes zweite Bild; gewertet wird jedes Bild, dessen Zustand UND Vorbild-Zustand den
# Ziel-Schnitt tragen (das Umschaltbild zeigt noch den alten Schnitt, Dossier §7c); mindestens 3, alle im Rahmen.
function(abe_lauf _fall _raum _pos _cut)
    set(WORKDIR "${_basis}_${_fall}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 600
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_TITLE_CONFIRM_MS=1500
        RE15_PSELECT_AUTO=1 "RE15_DEBUG_JUMP=${_raum}@250" "RE15_PLAYER_POS=${_pos}" "RE15_FORCE_CUT=${_cut}"
        RE15_STATE_LOG=state.txt "RE15_FRAMEDUMP=270-330/2:fd_" "RE15_EXIT_AT=332#${_raum}"
        ${ARGN}
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/state.txt")
        set(_fehler_${_fall} "\n  ${_fall}: state.txt fehlt (exit ${_rv})" PARENT_SCOPE)
        return()
    endif()
    file(STRINGS "${WORKDIR}/state.txt" _st REGEX "^F(2[6-9][0-9]|3[0-3][0-9]) ")
    foreach(_z IN LISTS _st)
        if(_z MATCHES "^F([0-9]+) .* cam=([0-9]+) ")
            set(_cam_${CMAKE_MATCH_1} ${CMAKE_MATCH_2})
        endif()
    endforeach()
    set(_n 0)
    set(_f "")
    foreach(_bild RANGE 270 330 2)
        math(EXPR _vor "${_bild} - 1")
        set(_ppm "${WORKDIR}/fd_000${_bild}.ppm")
        if(NOT EXISTS "${_ppm}" OR NOT "${_cam_${_bild}}" STREQUAL "${_cut}" OR NOT "${_cam_${_vor}}" STREQUAL "${_cut}")
            continue()
        endif()
        execute_process(COMMAND "${RE15_ABE_TOOL}" --fall "${_fall}" "${_ppm}"
                        RESULT_VARIABLE _rvw OUTPUT_VARIABLE _outw OUTPUT_STRIP_TRAILING_WHITESPACE TIMEOUT 60)
        math(EXPR _n "${_n} + 1")
        if(NOT _rvw EQUAL 0)
            string(APPEND _f "\n  ${_fall} F${_bild}: ${_outw}")
        elseif(_n EQUAL 1)
            message(STATUS "r38_rolltor20a0_abe[${_fall}] ROOM${_raum} F${_bild} ${_outw}")
        endif()
    endforeach()
    message(STATUS "r38_rolltor20a0_abe[${_fall}] ROOM${_raum}: ${_n} Bilder in Cut ${_cut} gewertet")
    if(_n LESS 3)
        string(APPEND _f "\n  ${_fall}: nur ${_n} Bilder in Cut ${_cut} (mindestens 3; exit ${_rv})")
    endif()
    set(_fehler_${_fall} "${_f}" PARENT_SCOPE)
endfunction()

abe_lauf(5060c4 5060 "-20926,-25850,0,0" 4)
string(APPEND _fehler "${_fehler_5060c4}")
# Spinne (Akteur 1, Sce_em_set-Lage (8197,-22772)) steht neben der Messstelle, greift sofort an und stoesst Leon in
# die Zone 2->1 (gemessen: Leon (6745,-22773) hp 80, Zustand cam=1). Das Original wurde ohne Gegner gemessen (Aktiv-
# Bits geloescht) — hier wird sie vom ersten Bild in 20A0 an hinter die Kamera von Cut 2 ((3672,-4986,-25416) Blick
# nach +x) gestellt, an die Sprung-Lage des Originals (-2800,-22200) (Messhaken RE15_GEGNER_POS, Runde 37 Spur A).
abe_lauf(20a0c2 20A0 "8750,-22932,0,2" 2 "RE15_GEGNER_POS=1,-2800,-22200,0@0#20A0")
string(APPEND _fehler "${_fehler_20a0c2}")
abe_lauf(4090c0 4090 "-21000,-24600,0,0" 0)
string(APPEND _fehler "${_fehler_4090c0}")

file(REMOVE "${_exe_kopie}")
if(_fehler)
    message(FATAL_ERROR "r38_rolltor20a0_abe: FEHLER${_fehler}")
endif()
message(STATUS "r38_rolltor20a0_abe: OK (ABR 0 ROOM5060 Cut 4, ABR 2 ROOM20A0 Cut 2, ABR 3 ROOM4090 Cut 0 = Original)")
