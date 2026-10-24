# =============================================================================
# Runde 41 Spur K "stage3szenen_b" - SPIELFIGUR SHERRY mit der echten exe (Dossier analysis/befunde_runde41/K_stage3szenen_b.md
# K1). Nutzer: "Ab 30C1 wird es richtig wild... Sherrys Charakter Modell ist voellig falsch und kaputt. Ausserdem sollte sie
# keine Waffe haben."
#
# Weg: Elza-Neuspiel (Spielerwahl), Sprung nach ROOM3061; dort schreibt der Messhaken RE15_WORKVAR_AT genau das, was die
# Kriech-Szene schreibt (ROOM3061 @0x011F6 `24 10 0f 00`), und RE15_FIRE_AOT nimmt die Schacht-Tuer Slot 3 (@0x00EF8, wie
# `47 03` @0x011FA) -> Raumlader -> ROOM30C1 Cut 10 als Sherry. Dann in 30C1 (Bildzaehler je Raum):
#   Bild 300..309  hoch          = Gehen   (Original K_belege/k1_orig_tempo_30c1.txt: 50 je Bild)
#   Bild 330..341  hoch + Kreuz  = Rennen  (Original k1_orig_rennen_30c1.txt: 120 je Bild)
#   Bild 400..439  R1            = kein Zielen (Waffe 0; @0x80032014/@0x80032430/...)
# Ausgewertet von test_r41_stage3szenen_b sherry <debug.log> <state.txt>: Koerper-/Waffen-Zeilen des Raumladers, Knochenlage
# Bild 60 (Kind), Schritte je Bild, Zielen gesperrt.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_WERKZEUG=<test_r41_stage3szenen_b> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
cmake_minimum_required(VERSION 3.16)
set(_tag "r41_stage3szenen_b_sherry")
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
set(_exe "${_exe_dir}/re15_pc_r41ks_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

set(_pad "")
foreach(_f RANGE 300 309)
    list(APPEND _pad "${_f}:U")
endforeach()
foreach(_f RANGE 330 341)
    list(APPEND _pad "${_f}:UX")
endforeach()
foreach(_f RANGE 400 439)
    list(APPEND _pad "${_f}:M")
endforeach()
string(REPLACE ";" "," _pad "${_pad}")

foreach(_versuch 1 2)
    file(REMOVE "${WORKDIR}/debug.log" "${WORKDIR}/state.txt")
    re15_start_spiel(_rv 400
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
        "RE15_SET_FLAG=4:244" "RE15_DEBUG_JUMP=3060@250"
        "RE15_WORKVAR_AT=0x10:15@30#3061" "RE15_FIRE_AOT=3@60#3061"
        "RE15_PAD_AT=${_pad}" RE15_STATE_LOG=state.txt
        "RE15_EXIT_AT=460#30C1"
        "${_exe}")
    if(EXISTS "${WORKDIR}/debug.log" AND EXISTS "${WORKDIR}/state.txt")
        file(STRINGS "${WORKDIR}/debug.log" _ende REGEX "EXIT_AT: Bild 460")
        if(_ende)
            break()
        endif()
    endif()
    message(STATUS "${_tag}: Lauf ${_versuch} ohne Ende-Zeile (exit=${_rv}) -> Wiederholung")
endforeach()
file(REMOVE "${_exe}")
if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.txt")
    message(FATAL_ERROR "${_tag}: kein debug.log/state.txt (exit=${_rv}, ${WORKDIR})")
endif()
execute_process(COMMAND "${RE15_WERKZEUG}" sherry "${WORKDIR}/debug.log" "${WORKDIR}/state.txt"
                RESULT_VARIABLE _ar OUTPUT_VARIABLE _ao ERROR_VARIABLE _ae)
message(STATUS "${_ao}${_ae}")
if(NOT _ar EQUAL 0)
    message(FATAL_ERROR "${_tag}: Auswertung fehlgeschlagen (${WORKDIR})")
endif()
