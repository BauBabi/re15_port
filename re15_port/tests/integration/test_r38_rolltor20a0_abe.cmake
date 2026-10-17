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

# Ein Lauf: _fall = Messflaeche des Werkzeugs, _raum, _pos = RE15_PLAYER_POS (x,z,rot,band), _cut.
function(abe_lauf _fall _raum _pos _cut)
    set(WORKDIR "${_basis}_${_fall}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 600
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_TITLE_CONFIRM_MS=1500
        RE15_PSELECT_AUTO=1 "RE15_DEBUG_JUMP=${_raum}@250" "RE15_PLAYER_POS=${_pos}" "RE15_FORCE_CUT=${_cut}"
        RE15_STATE_LOG=state.txt "RE15_FRAMEDUMP=300-300/1:fd_" "RE15_EXIT_AT=310#${_raum}"
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/fd_000300.ppm")
        set(_fehler_${_fall} "\n  ${_fall}: kein Framedump F300 (exit ${_rv})" PARENT_SCOPE)
        return()
    endif()
    file(STRINGS "${WORKDIR}/state.txt" _st REGEX "^F300 ")
    if(NOT _st MATCHES "cam=${_cut} ")
        set(_fehler_${_fall} "\n  ${_fall}: F300 nicht in Cut ${_cut}: ${_st}" PARENT_SCOPE)
        return()
    endif()
    execute_process(COMMAND "${RE15_ABE_TOOL}" --fall "${_fall}" "${WORKDIR}/fd_000300.ppm"
                    RESULT_VARIABLE _rvw OUTPUT_VARIABLE _outw OUTPUT_STRIP_TRAILING_WHITESPACE TIMEOUT 60)
    message(STATUS "r38_rolltor20a0_abe[${_fall}] ROOM${_raum} ${_outw}")
    if(NOT _rvw EQUAL 0)
        set(_fehler_${_fall} "\n  ${_fall}: ${_outw}" PARENT_SCOPE)
    endif()
endfunction()

abe_lauf(5060c4 5060 "-20926,-25850,0,0" 4)
string(APPEND _fehler "${_fehler_5060c4}")
abe_lauf(20a0c2 20A0 "8750,-22932,0,2" 2)
string(APPEND _fehler "${_fehler_20a0c2}")
abe_lauf(4090c0 4090 "-21000,-24600,0,0" 0)
string(APPEND _fehler "${_fehler_4090c0}")

file(REMOVE "${_exe_kopie}")
if(_fehler)
    message(FATAL_ERROR "r38_rolltor20a0_abe: FEHLER${_fehler}")
endif()
message(STATUS "r38_rolltor20a0_abe: OK (ABR 0 ROOM5060 Cut 4, ABR 2 ROOM20A0 Cut 2, ABR 3 ROOM4090 Cut 0 = Original)")
