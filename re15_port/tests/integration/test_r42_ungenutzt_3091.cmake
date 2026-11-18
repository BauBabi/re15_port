# =============================================================================
# Runde 42 Strang "rest", Stufe 3, Runde 2 (Pruefung s3_r1 G3): die AUSGELIEFERTE neue 3091-Szene (U-01) mit der echten exe.
# integration_r41_stage3szenen_b_3091 laesst per Messhaken RE15_U42_ORIGINAL die alte Mod-Szene laufen (Clipvergleich mit dem
# DuckStation-GDB-Lauf); dieser Lauf prueft die neue Fassung ohne Haken im echten Spiel.
#
# Weg wie integration_r41_stage3szenen_b_3091 (Elza-Neuspiel, work_vars[0x10] = 0x0F = ROOM3061 @0x011F6, Sprung nach 3091,
# Sicherung (4,8) und Riegelschluessel (9,21) gesetzt, Sherry vor der Riegeltuer Slot 1, VIERECK -> sub02 -> Evt_exec 7).
# Geprueft im debug.log:
#   - ungenutzt_r42.c hat sub07 ersetzt (Zeile "[ungenutzt42] ... ROOM3091 sub07: Blick zur Hoftuer (-24506,1500)")
#   - Elza (Aktor 2) und John (Aktor 1) drehen nach den Schlaegen zum Tonort (-24506,1500) = Disc ROOM3091 Se_on @0x1778
#   - Zeilenfolge 22, 23, 24, 33 (D309 #21), 25..28, 34 (D309 #24), 36 (D309 #25), 29, 30
#   - danach geht es wie in der Mod in die Lobby 30C1 (Tuer S198, Aot_on Slot 0 @0x1DAE)
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
cmake_minimum_required(VERSION 3.16)
set(_tag "r42_ungenutzt_3091")
foreach(_v RE15_PC_EXE WORKDIR)
    if(NOT ${_v})
        message(FATAL_ERROR "${_tag}: ${_v} fehlt")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

execute_process(COMMAND "${CMAKE_COMMAND}" -E environment OUTPUT_VARIABLE _umg)
string(REGEX MATCHALL "(^|\n)RE15_[A-Za-z0-9_]*=" _umg_namen "${_umg}")
foreach(_n ${_umg_namen})
    string(REGEX REPLACE "[\n=]" "" _n "${_n}")
    unset(ENV{${_n}})
endforeach()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe "${_exe_dir}/re15_pc_r42u_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

# Viereck bei Bild 300/310/320, dann Kreuz alle 30 Bilder (Form integration_r41_stage3szenen_b_3091); die drei neuen Zeilen
# verlaengern die Szene, daher bis Bild 4600.
set(_pad "300:A" "310:A" "320:A")
foreach(_f RANGE 360 4600 30)
    list(APPEND _pad "${_f}:X")
endforeach()
string(REPLACE ";" "," _pad "${_pad}")

foreach(_versuch 1 2)
    file(REMOVE "${WORKDIR}/debug.log")
    re15_start_spiel(_rv 480
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
        "RE15_SET_FLAG=4:8,9:21" "RE15_WORKVAR_AT=0x10:15@200" "RE15_DEBUG_JUMP=3090@250"
        "RE15_PLAYER_POS=-24665,1000,3072"
        "RE15_PAD_AT=${_pad}" RE15_MSG_LOG=1
        "RE15_EXIT_AT=30#30C1"
        "${_exe}")
    if(EXISTS "${WORKDIR}/debug.log")
        file(STRINGS "${WORKDIR}/debug.log" _ende REGEX "EXIT_AT: Bild 30 in Raum 30c1")
        if(_ende)
            break()
        endif()
    endif()
    message(STATUS "${_tag}: Lauf ${_versuch} ohne Ende-Zeile (exit=${_rv}) -> Wiederholung")
endforeach()
file(REMOVE "${_exe}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "${_tag}: kein debug.log (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)

set(_fehler 0)
macro(pruefe _was _muster)
    string(FIND "${_dbg}" "${_muster}" _pos)
    if(_pos LESS 0)
        message(STATUS "FAIL ${_tag}: ${_was} (fehlt: '${_muster}')")
        math(EXPR _fehler "${_fehler} + 1")
    else()
        message(STATUS "ok   ${_tag}: ${_was}")
    endif()
endmacro()
macro(pruefe_re _was _re)
    string(REGEX MATCH "${_re}" _m "${_dbg}")
    if(NOT _m)
        message(STATUS "FAIL ${_tag}: ${_was} (Muster '${_re}')")
        math(EXPR _fehler "${_fehler} + 1")
    else()
        message(STATUS "ok   ${_tag}: ${_was}")
    endif()
endmacro()

pruefe("neue Szene (kein Messhaken)" "ROOM3091 sub07: Blick zur Hoftuer (-24506,1500), Zeilen 33 (John), 34 (Elza), 36 (John)")
pruefe("Elza dreht zum Tonort (Disc @0x1778)" "Plc_dest(slot=2 mode=0x09 dest=(-24506,1500))")
pruefe("John dreht zum Tonort (Disc @0x1778)" "Plc_dest(slot=1 mode=0x09 dest=(-24506,1500))")
pruefe_re("Zeilen 22, 23, 24, 33, 25..28, 34, 36, 29, 30 in dieser Folge"
    "msgtext] room=3091 id=22 .*msgtext] room=3091 id=23 .*msgtext] room=3091 id=24 .*msgtext] room=3091 id=33 .*msgtext] room=3091 id=25 .*msgtext] room=3091 id=28 .*msgtext] room=3091 id=34 .*msgtext] room=3091 id=36 .*msgtext] room=3091 id=29 .*msgtext] room=3091 id=30 ")
pruefe("msg 33" "[msgtext] room=3091 id=33 \"John: Hey, let's get going. If we stay here, they'll come swarming back.\"")
pruefe("msg 34" "[msgtext] room=3091 id=34 \"Elza: Hey... you'll be all right, won't you?\"")
pruefe("msg 36" "[msgtext] room=3091 id=36 \"John: Leave it to me!\"")
pruefe("danach Lobby 30C1 wie in der Mod" "EXIT_AT: Bild 30 in Raum 30c1")
string(FIND "${_dbg}" "Messhaken RE15_U42_ORIGINAL" _haken)
if(NOT _haken LESS 0)
    message(STATUS "FAIL ${_tag}: Messhaken RE15_U42_ORIGINAL war aktiv")
    math(EXPR _fehler "${_fehler} + 1")
endif()
if(_fehler GREATER 0)
    message(FATAL_ERROR "${_tag}: ${_fehler} Fehler (${WORKDIR})")
endif()
message(STATUS "${_tag}: 0 Fehler")
