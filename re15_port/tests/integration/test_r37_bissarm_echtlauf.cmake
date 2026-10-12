cmake_minimum_required(VERSION 3.16)
# =============================================================================
# DER DRANHAENGENDE ARM IM ECHTLAUF (Runde 37, Spur J "bissarm").
#
# Nutzer: "fehlt in unseren Port teilweise noch der dranhaengende Arm wenn man vom Zombie gebissen
# wird." Dossier analysis/befunde_runde37/J_bissarm.md Abschnitte 4/5. Die Unit-Riegel
# unit_r37_bissarm_re2_* (Hand gegen die RE2-RAM-Fixture) und unit_r37_bissarm_klemme_* (ein
# Wandklemm-Durchgang je Bild) messen die Mechanik in der Sonde. Dieser Riegel misst die echte exe:
#
#   Lauf A  ROOM1140 Cut 1, EIN Greifer: Kill-Flags Zone 7 Bits 211..214 (Sce_em_set pc[7], Gate
#           @0x80042120-38) -> nur Satz 4 spawnt (Typ 0x11 bei (200,-19600)); Leon (-1000,-19600)
#           Blick 2048 = Ruecken zum Greifer -> Griff von HINTEN, Browning ausgeruestet.
#   Lauf B  ROOM1010 Cut 6 (Weg aus Abschnitt 1.2): Leon (3700,0,2300), 0,3 s rueckwaerts -> Griff
#           durch Slot 1 (Typ 0x10) von vorn.
#   Lauf C  wie A, aber ELZA (Charakter 4, PL04) — Opfer-Bezug: die exe liest PLD/PL00.PLD (Leons
#           Knochen) ueber re15_asset_read_file und legt Elzas Schulter-Differenz an (Lauf A: 0).
#
# Je Lauf zwei Auswerter (test_r37_bissarm):
#   pruef_armlog   RE15_BISSARM_LOG: der NPC-Zeichner (platform/pc/main.c) ruft im Griff
#                  re15_bissarm_r37_handkontakt; in jedem Bild liegt die gezeichnete Hand hoechstens
#                  |rest|+3 vom RE2-Zielpunkt und in >= 20 Biss-Bildern (Clip 12, P3 @0x80102838)
#                  liegen beide Haende darauf; Opfer-Bezug Leon = 0 in jeder Zeile, Elza |Y| >= 50.
#   pruef_griffweg RE15_STATE_LOG: im Halten (Greifer-Clip 11/12) steht Leon <= 2000 vom Greifer,
#                  im ganzen Griff kein Sprung > 1200 und nie (0,0) (RE2 FUN_8003567c: EIN Durchgang
#                  `jal 0x8004c1bc` @0x800356d0 vom alten Standpunkt `sh` @0x800356f4-f8).
#
# Gemessen 2026-10-12 (Bau dieser Spur): A 103 Griffbilder, 50 Biss-Bilder mit beiden Haenden auf
# dem Ziel, Halten max 1128; B 103 / 54 / max 917; C 103 / 58, Opfer-Bezug |Y| mittel 116.
# Gegenprobe mit dem Bau VOR dem Fix (419380dc): A ohne bissarm.log (Haken fehlt), B Leon 89 Bilder
# auf (0,0), 81 Halte-Bilder > 2000 (bis 5571).
#
# Eigener exe-Name (Muster test_r36_kleidung_bild.cmake), SDL_ASSERT=always_ignore (RDP-Haenger
# an WIN_AddDisplay, Runde 36 B_abnahme_0.md Abschnitt 5).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r37_bissarm> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_bissarm_echtlauf: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
    message(FATAL_ERROR "r37_bissarm_echtlauf: RE15_TOOL fehlt: '${RE15_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r37_bissarm_echtlauf: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37j_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r37j_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

set(_fehler "")

# Karte schreiben (Slot 0) -> CONTINUE -> Lauf bis Bild _ende; danach beide Auswerter.
function(r37j_lauf _name _ende _raum _opfer)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    execute_process(COMMAND "${RE15_TOOL}" karte "re15_card.mcr" ${ARGN}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _ok)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r37_bissarm_echtlauf[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_ok}")
    endif()
    message(STATUS "r37_bissarm_echtlauf[${_name}] ${_ok}")
    if(_name STREQUAL "B")
        re15_start_spiel(_rv 300
            SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
            RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
            RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=30 "RE15_INPUT_SCRIPT=D0.3,W20"
            RE15_STATE_LOG=state.log RE15_BISSARM_LOG=bissarm.log
            "RE15_EXIT_AT=${_ende}#${_raum}"
            "${_exe}")
    else()
        re15_start_spiel(_rv 300
            SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
            RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
            RE15_STATE_LOG=state.log RE15_BISSARM_LOG=bissarm.log
            "RE15_EXIT_AT=${_ende}#${_raum}"
            "${_exe}")
    endif()
    if(NOT EXISTS "${WORKDIR}/state.log")
        message(FATAL_ERROR "r37_bissarm_echtlauf[${_name}]: state.log fehlt (exit=${_rv})")
    endif()
    message(STATUS "r37_bissarm_echtlauf[${_name}] exe exit=${_rv}")
    set(_f_lokal "${_fehler}")
    foreach(_teil pruef_armlog pruef_griffweg)
        if(_teil STREQUAL "pruef_armlog")
            set(_args "${WORKDIR}/bissarm.log" 20 ${_opfer})
        else()
            set(_args "${WORKDIR}/state.log" 2000 30)
        endif()
        execute_process(COMMAND "${RE15_TOOL}" ${_teil} ${_args}
                        TIMEOUT 60 RESULT_VARIABLE _rva OUTPUT_VARIABLE _outa)
        message(STATUS "r37_bissarm_echtlauf[${_name}] ${_outa}")
        if(NOT _rva EQUAL 0)
            string(APPEND _f_lokal " ${_name}:${_teil}")
        endif()
    endforeach()
    set(_fehler "${_f_lokal}" PARENT_SCOPE)
endfunction()

r37j_lauf(A 200 1140 leon 1140 0 -1000 0 -19600 2048 100 1 3 "7:211,7:212,7:213,7:214")
r37j_lauf(B 200 1010 leon 1010 0 3700 0 2300 0 100 6 0 "-")
r37j_lauf(C 200 1140 elza 1140 4 -1000 0 -19600 2048 100 1 3 "7:211,7:212,7:213,7:214")

file(REMOVE "${_exe}")
if(_fehler)
    message(FATAL_ERROR "r37_bissarm_echtlauf: ROT —${_fehler}")
endif()
message(STATUS "r37_bissarm_echtlauf: OK — im Griff liegen die gezeichneten Zombie-Haende auf dem RE2-Kontaktpunkt (Leon und Elza), das Opfer bleibt am Greifer")
