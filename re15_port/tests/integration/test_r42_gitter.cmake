# Runde 42 Strang "gore", Stufe 2 Runde 2 (Pruefbericht s2 r1 Gelb 2): integration_r42_gitter - kommt man im ECHTEN Spiel
# ohne Schuss an den Gitterhaenden (Typ 0x1a, ROOM1210/1211) vorbei? BAUPLAN U8 verlangt diese Pruefung VOR dem Bau ("Schuesse
# gehen durch die Arme"); der Unit-Riegel 200 setzte die Spielerlage je Bild neu (kein Koerperstoss der Arme, keine z-Wand).
# Registrierung: tests/unit/probes/r42_tiere.cmake.
#
# LAEUFE (eine echte re15_pc.exe je Lauf, Titel-Autostart, RE2-KI, nur Pad OBEN 35 s, kein Zielen, kein Schuss):
#   W1 Leon ROOM1210 Westseite x -20700   W2 Flurmitte x -19500   W3 Ostseite x -17100   (Start z -6000, Blick 1024)
#   E2 Elza ROOM1211 Flurmitte x -19500 (RE15_TITLE_SHOT_CHAR=1). Die Arme stehen in ROOM1211 hinter `Ck(3,44,1)` (RDT @0x01E6E,
#      Sce_em_set 0x1a ab @0x01E86); der Raumsprung setzt das Flag nicht -> Messhaken RE15_SET_FLAG=3:44 (gemessen 2026-11-18: ohne
#      das Flag lud ROOM1211 kein EM01A, der Lauf pruefte nichts).
# AUSWERTER test_r42_tiere pruef_gitter: HP immer 100, mindestens ein Arm wach, der Spieler kommt hinter den letzten Arm.
# Das ist eine Vorbedingung, kein Umbau: der Riegel ist auf jedem Stand gruen (Gegenprobe entfaellt bewusst, im Bericht).
# Ein Lauf ohne state.log (Startfehler) wird EINMAL wiederholt.
cmake_minimum_required(VERSION 3.16)
set(_tag "r42_gitter")
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "${_tag}: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
    message(FATAL_ERROR "${_tag}: RE15_TOOL fehlt: '${RE15_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "${_tag}: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r42git_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r42git_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

set(_fehler "")
function(r42g_lauf _name _raum _x)
    set(WORKDIR "${_basis}_${_name}")
    foreach(_versuch 1 2)
        file(REMOVE_RECURSE "${WORKDIR}")
        file(MAKE_DIRECTORY "${WORKDIR}")
        re15_start_spiel(_rv 300
            RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_AI_FLAVOR=re2
            RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${ARGN}
            RE15_DEBUG_JUMP=${_raum}@250
            RE15_PLAYER_POS=${_x},-6000,1024
            RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=5 RE15_INPUT_SCRIPT=U35
            RE15_STATE_LOG=state.log
            "RE15_EXIT_AT=900#${_raum}"
            "${_exe}")
        if(EXISTS "${WORKDIR}/state.log")
            break()
        endif()
    endforeach()
    if(NOT EXISTS "${WORKDIR}/state.log")
        set(_fehler "${_fehler}\n[${_name}] kein state.log (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    execute_process(COMMAND "${RE15_TOOL}" pruef_gitter "${WORKDIR}/state.log"
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 120 RESULT_VARIABLE _rp OUTPUT_VARIABLE _op)
    message(STATUS "${_tag}[${_name}] exit=${_rv}\n${_op}")
    if(NOT _rp EQUAL 0)
        set(_fehler "${_fehler}\n[${_name}] pruef_gitter exit=${_rp}\n${_op}" PARENT_SCOPE)
    endif()
endfunction()

r42g_lauf(W1 1210 -20700)
r42g_lauf(W2 1210 -19500)
r42g_lauf(W3 1210 -17100)
r42g_lauf(E2 1211 -19500 RE15_TITLE_SHOT_CHAR=1 RE15_SET_FLAG=3:44)

file(REMOVE "${_exe}")
if(NOT _fehler STREQUAL "")
    message(FATAL_ERROR "${_tag}:${_fehler}")
endif()
message(STATUS "${_tag}: alle Laeufe gruen")
