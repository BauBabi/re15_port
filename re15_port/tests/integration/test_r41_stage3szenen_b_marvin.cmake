# =============================================================================
# Runde 41 Spur K "stage3szenen_b", Nachbesserung 1 (M2/M3) - SPIELFIGUR MARVIN 0x0D mit der echten exe
# (Dossier analysis/befunde_runde41/K_stage3szenen_b.md NB1.M2/NB1.M3).
#
# Original: FUN_800314b0 bindet bei JEDEM Neuladen EDD (@0x8003154c) und EMR (@0x80031578) der neuen PLD und setzt Waffe :=
# DEBUG.BIN @0x800c00d4[Modell] (@0x80031680/@0x80031690), HP := 100 (@0x80031710/@0x80031718), Status := 0 (@0x80031720).
# Weg: Leon-Neuspiel mit Pistole (RE15_EQUIP=3) und Gift bei Bild 100 (RE15_R39_GIFT_AT, Messhaken zustand_r39.c), Sprung
# nach ROOM3020; dort schreibt RE15_WORKVAR_AT, was sub02 schreibt (`24 10 0d 00` @0x00D7A), und RE15_FIRE_AOT nimmt die
# Tuer Slot 2 (`47 02` @0x00D7E, Door_aot_set @0x00B6E -> ROOM30C0) -> Raumlader -> Marvin.
# Ausgewertet von test_r41_stage3szenen_b marvinlauf <debug.log>: Raumlader-Zeilen (Waffe/HP/Status vorher -> nachher,
# Koerper PL0D) und die Knochenlage Bild 60 in 30C0 (Knochen 10: Marvin 485, Leon 422).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_WERKZEUG=<test_r41_stage3szenen_b> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
cmake_minimum_required(VERSION 3.16)
set(_tag "r41_stage3szenen_b_marvin")
foreach(_v RE15_PC_EXE RE15_WERKZEUG WORKDIR)
    if(NOT ${_v})
        message(FATAL_ERROR "${_tag}: ${_v} fehlt")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Keine Umgebungsvariablen des Aufrufers erben (VERTRAG §2).
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
set(_exe "${_exe_dir}/re15_pc_r41km_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

foreach(_versuch 1 2)
    file(REMOVE "${WORKDIR}/debug.log")
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500
        "RE15_EQUIP=3" "RE15_R39_GIFT_AT=100"
        "RE15_DEBUG_JUMP=3020@250" "RE15_WORKVAR_AT=0x10:13@30#3020" "RE15_FIRE_AOT=2@60#3020"
        "RE15_EXIT_AT=90#30C0"
        "${_exe}")
    if(EXISTS "${WORKDIR}/debug.log")
        file(STRINGS "${WORKDIR}/debug.log" _ende REGEX "EXIT_AT: Bild 90")
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
execute_process(COMMAND "${RE15_WERKZEUG}" marvinlauf "${WORKDIR}/debug.log"
                RESULT_VARIABLE _ar OUTPUT_VARIABLE _ao ERROR_VARIABLE _ae)
message(STATUS "${_ao}${_ae}")
if(NOT _ar EQUAL 0)
    message(FATAL_ERROR "${_tag}: Auswertung fehlgeschlagen (${WORKDIR})")
endif()
