cmake_minimum_required(VERSION 3.16)
# =============================================================================
# STAGE-2-SPINNEN IM ECHTLAUF (Runde 38, Spur R "spinnen").
#
# Nutzer (AUFTRAG.md Z. 68-70): "die Spinnen ... fliegen fast alle in der Luft", "verschiessen kein Gift ... der einzige
# Grund fuer das Gegengift Item", "die Minispinnen ... fliegen weitestgehend in der Luft und greifen einen nicht an".
# Dossier analysis/befunde_runde38/R_spinnen.md (1.6, 2.8, 2.9, 4.4). Die Mechanik misst unit_r38_spinnen im
# Spielschritt; dieser Riegel misst die ECHTE exe mit der Messschiene RE15_SPINNEN_LOG:
#
#   Lauf L  Leon, ROOM2030 (Debug-Sprung, Spieler (6550,-5400,2000) auf dem Steg, Blick 3072), Schrotflinte
#           (RE15_GIVE=8:30, RE15_EQUIP=8), Feuerskript auf der Spielzeitachse: Spinne 1 stirbt, die RE2-Babys (Typ 0x26)
#           krabbeln los; Ende Bild 900 in ROOM2030.
#   Lauf E  derselbe Lauf als ELZA (RE15_TITLE_SHOT_CHAR=1 -> PL04, Raum ROOM2031 = Leon-RDT + 1).
#
# Auswerter `test_r38_spinnen pruef_log <log> <raum>`: Babys entstehen, kein Baby am Boden ueber dem SCA-Boden bu
# (FUN_8001c6e8 ab dem eigenen Band; Wandpass FUN_8003567C @0x801000c0), Bisse > 0, keine Adultspinne ueber dem Boden,
# Gift-Bit 0x2 am Spieler, Messgroesse am Spieler gegengeprueft.
#
# Gemessen 2026-10-17 (Endstand 96eca34b, Lauf m5_2030 / e_2031 ueber 2400 Bilder): Babys 0 von 19575 Boden-Bildern
# schwebend (vorher 14485 von 20025, m2_2030), bis Bild 900: 7 Bisse, Gift gesetzt. Gegenprobe ohne den Baby-Schwanz:
# unit_r38_spinnen C 2 FAIL (R_belege/gegenprobe_baby_schwanz.txt).
# Nachbesserung 1 (Mangel 1 der Abnahme 0): der Auswerter prueft zusaetzlich die GIFT-TAKTE (gz 0 -> 0x78 mit HP -2):
# ohne fremden HP-Abzug im Fenster +-25 keine Treffer-Reaktion (Schiene pmo/phf) und Periode 121 (Original
# @0x80031e44-ec8 schreibt nur HP + Zaehler). Endstand L/E: 6 Takte, 5 sauber, 0 Reaktionen, 3 Perioden = 121.
# Gegenprobe ohne den Haken in game_step_common.c: 3 von 3 Takten reagierten, Perioden 2 x != 121 -> FAIL
# (R_belege/gegenprobe_nb1_echtlauf.txt).
#
# Eigener exe-Name (Muster test_r37_bissarm_echtlauf.cmake), SDL_ASSERT=always_ignore.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r38_spinnen> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r38_spinnen_echtlauf: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
    message(FATAL_ERROR "r38_spinnen_echtlauf: RE15_TOOL fehlt: '${RE15_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r38_spinnen_echtlauf: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r38r_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r38r_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

set(_fehler "")

function(r38r_lauf _name _raum)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 300
        SDL_ASSERT=always_ignore RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${ARGN}
        RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=6550,2000,3072,3
        RE15_GIVE=8:30 RE15_EQUIP=8
        "RE15_INPUT_SCRIPT=M1,MA0.2,M1.2,MA0.2,M1.2,MA0.2,M1.2,MA0.2,M1.2,MA0.2,M1.2,MA0.2,W40"
        RE15_INPUT_SCRIPT_START=20 RE15_INPUT_SCRIPT_BASIS=spiel
        RE15_SPINNEN_LOG=spinnen.log
        "RE15_EXIT_AT=900#${_raum}"
        "${_exe}")
    if(NOT EXISTS "${WORKDIR}/spinnen.log")
        set(_fehler "${_fehler}\n[${_name}] kein spinnen.log (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    execute_process(COMMAND "${RE15_TOOL}" pruef_log "${WORKDIR}/spinnen.log" "${_raum}"
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 120 RESULT_VARIABLE _rp OUTPUT_VARIABLE _op)
    message(STATUS "r38_spinnen_echtlauf[${_name}] exe exit=${_rv}\n${_op}")
    if(NOT _rp EQUAL 0)
        set(_fehler "${_fehler}\n[${_name}] pruef_log exit=${_rp}\n${_op}" PARENT_SCOPE)
    endif()
endfunction()

r38r_lauf(L 2030)
r38r_lauf(E 2031 RE15_TITLE_SHOT_CHAR=1)

file(REMOVE "${_exe}")
if(NOT _fehler STREQUAL "")
    message(FATAL_ERROR "r38_spinnen_echtlauf: ${_fehler}")
endif()
message(STATUS "r38_spinnen_echtlauf: L (Leon ROOM2030) und E (Elza ROOM2031) bestanden")
