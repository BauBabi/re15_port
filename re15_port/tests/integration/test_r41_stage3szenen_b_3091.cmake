# =============================================================================
# Runde 41 Spur K "stage3szenen_b" - die ROOM3091-SZENE mit der echten exe (Dossier analysis/befunde_runde41/K_stage3szenen_b.md
# K2). Nutzer: "Die Cutscene im ROOM 3091 hat STAENDIG falsche Animationen...."
#
# Weg: Elza-Neuspiel, im ersten Raum work_vars[0x10] = 0x0F (RE15_WORKVAR_AT, = ROOM3061 @0x011F6), Sprung nach 3091 ->
# der Raumlader bindet SHERRY als Spielfigur (wie FUN_800396fc @0x80039788); Sicherung oben (4,8) und Riegelschluessel (9,21)
# gesetzt (die Vorbedingungen aus sub05 @0x016AA / sub06 @0x016C2); dann die ECHTE Riegeltuer Slot 1 per RE15_FIRE_AOT
# (= Aot-Pfad des Untersuchens) -> sub01 -> sub02 -> Ck(9,21) -> sub07. Nachrichten mit Kreuz alle 30 Bilder.
# Ausgewertet von test_r41_stage3szenen_b szene <anim.txt> <debug.log>: Binder-Zeilen (Spieler rec2 / John rec1 / Elza-NPC
# rec2) und die Sub-0-Clipfolge je Aktor gegen das ORIGINAL (DuckStation-GDB, K_belege/k2_orig_3091_gdb_lauf2.log.gz).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_WERKZEUG=<test_r41_stage3szenen_b> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
cmake_minimum_required(VERSION 3.16)
set(_tag "r41_stage3szenen_b_3091")
foreach(_v RE15_PC_EXE RE15_WERKZEUG WORKDIR)
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
set(_exe "${_exe_dir}/re15_pc_r41kz_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

# Kreuz alle 30 Bilder ab Bild 270 (nach dem Sprung bei 250; der Bildzaehler laeuft je Raum neu, 3091 bis ~3100).
set(_pad "")
foreach(_f RANGE 270 3600 30)
    list(APPEND _pad "${_f}:X")
endforeach()
string(REPLACE ";" "," _pad "${_pad}")

foreach(_versuch 1 2)
    file(REMOVE "${WORKDIR}/debug.log" "${WORKDIR}/anim.txt")
    re15_start_spiel(_rv 900
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
        "RE15_SET_FLAG=4:8,9:21" "RE15_WORKVAR_AT=0x10:15@200" "RE15_DEBUG_JUMP=3090@250"
        "RE15_FIRE_AOT=1@300#3091"
        "RE15_PAD_AT=${_pad}" RE15_ANIM_TRACE=anim.txt
        "RE15_EXIT_AT=30#30C1"
        "${_exe}")
    if(EXISTS "${WORKDIR}/debug.log" AND EXISTS "${WORKDIR}/anim.txt")
        file(STRINGS "${WORKDIR}/debug.log" _ende REGEX "EXIT_AT: Bild 30 in Raum 30c1")
        if(_ende)
            break()
        endif()
    endif()
    message(STATUS "${_tag}: Lauf ${_versuch} ohne Ende-Zeile (exit=${_rv}) -> Wiederholung")
endforeach()
file(REMOVE "${_exe}")
if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/anim.txt")
    message(FATAL_ERROR "${_tag}: kein debug.log/anim.txt (exit=${_rv}, ${WORKDIR})")
endif()
execute_process(COMMAND "${RE15_WERKZEUG}" szene "${WORKDIR}/anim.txt" "${WORKDIR}/debug.log"
                RESULT_VARIABLE _ar OUTPUT_VARIABLE _ao ERROR_VARIABLE _ae)
message(STATUS "${_ao}${_ae}")
if(NOT _ar EQUAL 0)
    message(FATAL_ERROR "${_tag}: Auswertung fehlgeschlagen (${WORKDIR})")
endif()
