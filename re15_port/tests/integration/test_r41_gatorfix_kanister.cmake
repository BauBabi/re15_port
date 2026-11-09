# =============================================================================
# test_r41_gatorfix_kanister.cmake - Runde 41 Integration, Nutzerbefund 2026-11-09 Punkt 2 (exe-Riegel).
#
# Nutzer woertlich: "Ich musste diverse runden mit dem Aligator drehen, bis er letzendlich den Behälter aufnahm. Eigentlich sollte er den Behälter
# aufnehmen, sobald die Position erreicht ist, wo der Behälter liegt."
#
# TEIL=kanister_vorbei|kanister_vorbei_elza  zwei Laeufe am echten Weg (Kampf 2090/2091 mit dem Ausloese-Messhaken RE15_K2080_GATOR_HP=24990,
#     Marvin/John-Szene in 2080/2081, Rueckkehr mit dem liegenden Kanister an der Aufschlagstelle (-6870,-12651), RE15_G2_TRAG_LOG=1):
#     (a) pendel_innen: Rueckkehr-Lage -4000,-22000 Blick Nord, Figur pendelt im Suedbecken (der Alligator folgt durch den Westkanal und
#         dasht ueber die Zonenlinie z -14500 - ueber den Kanister);
#     (b) stehen_ost: Rueckkehr-Lage -4800,-12651 Blick Ost, Figur steht 2070 oestlich des Kanisters (Biss ueber dem Kanister).
#     Auswerter test_r41_gatorfix kanpass: Aufnahme <= erstes Bild mit dem Kanister im Fenster (LURK/CHASE/LUNGE/RE2_DASH) + 1.
#     Vorher (Stand 0b86e36c6, Laeufe des Untersuchers gator_nb/kanister/runs, kanpass): (a) Aufnahme F884 gegen Fensterbild F594,
#     (b) F1020 gegen F252 - rot.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r41_gatorfix> -DWORKDIR=<dir> -DTEIL=<teil> -P test_r41_gatorfix_kanister.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.16)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_gatorfix_kanister: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
    message(FATAL_ERROR "r41_gatorfix_kanister: RE15_TOOL fehlt/existiert nicht: '${RE15_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_gatorfix_kanister: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_basis}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe_kopie "${_exe_dir}/re15_pc_r41kan_${_lauf_id}.exe")
if(NOT WIN32)
    set(_exe_kopie "${_exe_dir}/re15_pc_r41kan_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

foreach(_v RE15_GB_TEST RE15_GATOR_KANISTER RE15_KEIN_GATOR RE15_K2080_WURF_GIER RE15_K2080_GATOR_HP RE15_SET_FLAG RE15_FIRE_AOT RE15_G2_WURF_CUT
           RE15_K2080_RUECK_KANISTER RE15_K2080_RUECK_LAGE RE15_G2_KAN_PHASEN_ALT)
    unset(ENV{${_v}})
endforeach()

if(TEIL MATCHES "elza$")
    set(_r2090 2091)
    set(_wahl RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
elseif(TEIL STREQUAL "kanister_vorbei")
    set(_r2090 2090)
    set(_wahl RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
else()
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_gatorfix_kanister: unbekannter TEIL '${TEIL}'")
endif()

# Abschnitte je Raum (RE15_INPUT_SCRIPT_BASIS=spiel): 2090 erster Besuch (Ausloeseschuss), 2080 (Szene), 2090 Rueckkehr (Weg)
set(_v "W1|W6.0,M0.6,MA0.1,M0.3,MA0.1|W1|")
set(_laeufe a b)
set(_weg_a "${_v}W1.6,XU3.0,L0.711,XU3.0,L0.711,XU3.0,L0.711,XU3.0,L0.711,XU3.0,L0.711,XU3.0,L0.711,XU3.0,W60")
set(_lage_a "RE15_K2080_RUECK_LAGE=-4000,-22000,3072,2")
set(_ende_a 3600)
set(_weg_b "${_v}W100")
set(_lage_b "RE15_K2080_RUECK_LAGE=-4800,-12651,0,2")
set(_ende_b 3000)
set(_fehl "")
set(_bericht "")
foreach(_l IN LISTS _laeufe)
    set(WORKDIR "${_basis}/${_l}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 900
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 ${_wahl}
        RE15_DEBUG_JUMP=${_r2090}@250 "RE15_SET_FLAG=9:110,3:245" "RE15_GIVE=3:1" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_weg_${_l}}" RE15_STATE_LOG=state.log RE15_G2_TRAG_LOG=1
        "RE15_PLAYER_POS=-6000,-17500,3072" ${_lage_${_l}} "RE15_EXIT_AT=${_ende_${_l}}#${_r2090}" "${_exe_kopie}")
    if(NOT _rv EQUAL 0)
        list(APPEND _fehl "Lauf ${_l}: exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT)")
        continue()
    endif()
    execute_process(COMMAND "${RE15_TOOL}" kanpass "${WORKDIR}" RESULT_VARIABLE _kr OUTPUT_VARIABLE _ko ERROR_VARIABLE _ke)
    string(REGEX MATCH "kanpass: Kanister[^\n]*" _kz "${_ko}")
    string(APPEND _bericht "  Lauf ${_l}: ${_kz}\n")
    if(NOT _kr EQUAL 0)
        list(APPEND _fehl "Lauf ${_l}: ${_ko}")
    endif()
endforeach()
file(REMOVE "${_exe_kopie}")
message(STATUS "r41_gatorfix_kanister ${TEIL}:\n${_bericht}")
if(_fehl)
    message(FATAL_ERROR "r41_gatorfix_kanister: Lauf ${TEIL} - ${_fehl} (${_basis})")
endif()
message(STATUS "r41_gatorfix_kanister: ${TEIL} OK (${_basis})")
