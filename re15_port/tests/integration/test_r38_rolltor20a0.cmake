# =============================================================================
# ROLLADEN ROOM20A0/20A1 MIT DER ECHTEN EXE (Runde 38 Spur W).
#
# Nutzer 2026-10-16: "in ROOM 20A0 the shutter that you have to open to push the box down is somehow DIRECTLY in
# the camera. I cannot see anything... this is not like this in the original...."
# Dossier analysis/befunde_runde38/W_rolltor20a0.md, Code engine/src/objpoly_r38.c (+ main.c Prop-Schleife).
#
# Vier Laeufe, eine Messkopie der exe unter eigenem Namen (Muster test_r34n_b_cursor.cmake), echter Weg ueber
# RE15_WEGPLAN (geschlossen geregelt, dachkiste_r36.c): vor der Leiter (Band 2), Leiter = Door_aot_set slot 1
# @0x01766 -> Cut 12, Schalter = Aot_set slot 3 @0x017E8 -> Frage -> Ja -> sub07 @0x01E40 (Rolladen obj 1 hoch),
# dann nach Sueden in die Zone "12 -> 11" -> Cut 11.
#   A  Leon  ROOM20A0, Eintritt per Debug-Sprung (RE15_DEBUG_JUMP + RE15_PLAYER_POS vor die Leiter).
#   B  Elza  ROOM20A1, Eintritt per Spielstand + CONTINUE (Karte aus probe_r38_rolltor20a0_karte, Figur 4).
#   D  Leon  ROOM20A0, Eintritt per Spielstand + CONTINUE.
#   C  Leon  ROOM20A0 (Sprung): Kiste obj 0 von Sueden durch die Oeffnung schieben -> Fall (Kisten-Ablauf loesbar).
# Pruefungen A/B/D: Rolladen y -10800; alle Framedumps (F600..F1100 je 50) in Cut 11 (RE15_STATE_LOG cam=11 im Bild
# und im Vorbild — das Umschaltbild zeigt noch den alten Schnitt; mindestens 3) zeigen den Raumhintergrund
# (Werkzeug test_r38_rolltor20a0 --bild ... 11 0: Anteil der Pixel gleich dem dekodierten BSS-Bild >= 0.80;
# vor dem Fix 0.1305, nachher 0.94); RE15_OBJPOLY_LOG Cut 11 obj 1 =
# Original-Messung (gezeichnet 6, nclip 20, otz 16, gpu 6 — W_belege/orig1_log.txt) in jedem Bild.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r38_rolltor20a0_karte>
#               -DRE15_BILD_TOOL=<test_r38_rolltor20a0> -DRE15_BSS=<ROOM20A.BSS>
#               -DDATEN=<tests/integration/r38_rolltor20a0> -DWORKDIR=<dir> -P test_r38_rolltor20a0.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.16)   # Policies NEW (CMP0012/CMP0054/CMP0007) auch unter dem Container-CMake 3.28 im Skriptmodus wie auf dem Host (Memory reai-v2-linux-test-portabilitaet; Linux-Release v0.8.26)
foreach(_v RE15_PC_EXE RE15_KARTE_TOOL RE15_BILD_TOOL RE15_BSS DATEN)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r38_rolltor20a0: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "r38_rolltor20a0: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Schwelle des Hintergrund-Anteils in Cut 11 bei Toleranz 0 (Pixel == dekodiertes BSS-Bild; gemessen 2026-10-16,
# Dossier §6/§7): vorher (Rolladen deckt zu) 0.1305 (nur die linke Wand frei), nachher 0.9424..0.9493 (Rest = Figur,
# Kiste, Raum-Objekte). Schwelle dazwischen.
set(_bg_min 0.80)

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r38w_haken_*.exe" "${_exe_dir}/re15_pc_r38w_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe_kopie "${_exe_dir}/re15_pc_r38w_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_fehler "")
macro(fehler _text)
    string(APPEND _fehler "\n  ${_text}")
endmacro()

# Ein Lauf. _weg = sprung | karte, _raum = 20A0/20A1, _plan = Wegplan-Datei, _ende = Bild, ARGN = weitere KEY=WERT.
function(rolladen_lauf _name _weg _raum _plan _ende)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(READ "${DATEN}/${_plan}" _wp)
    string(REPLACE "RAUM 20A0" "RAUM ${_raum}" _wp "${_wp}")
    file(WRITE "${WORKDIR}/wegplan_eingabe.txt" "${_wp}")
    if(_weg STREQUAL "karte")
        file(REMOVE "${WORKDIR}/re15_card.mcr")
        execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}"
                        WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
        if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
            set(_rv_${_name} "Karte exit ${_rvk}: ${_outk}" PARENT_SCOPE)
            return()
        endif()
        message(STATUS "r38_rolltor20a0[${_name}] ${_outk}")
        set(_eintritt RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0)
    else()
        set(_eintritt RE15_PSELECT_AUTO=1 "RE15_DEBUG_JUMP=${_raum}@250" "RE15_PLAYER_POS=16500,-21050,3072,2")
    endif()
    re15_start_spiel(_rv 600
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_TITLE_CONFIRM_MS=1500
        ${_eintritt}
        "RE15_WEGPLAN=${WORKDIR}/wegplan_eingabe.txt" RE15_WEGPLAN_LOG=wegplan.txt
        RE15_STATE_LOG=state.txt RE15_OBJPOLY_LOG=objpoly.txt RE15_OBJFALL_LOG=objfall.txt
        "RE15_EXIT_AT=${_ende}#${_raum}"
        ${ARGN}
        "${_exe_kopie}")
    set(_rv_${_name} "${_rv}" PARENT_SCOPE)
endfunction()

# Pruefung A/B/D: Cut 11 mit offenem Rolladen.
function(pruefe_cut11 _name _raum)
    set(WORKDIR "${_basis}_${_name}")
    set(_f "")
    if(NOT EXISTS "${WORKDIR}/state.txt" OR NOT EXISTS "${WORKDIR}/objpoly.txt" OR NOT EXISTS "${WORKDIR}/wegplan.txt")
        set(_fehler_${_name} "\n  ${_name}: state/objpoly/wegplan fehlt (exit ${_rv_${_name}})" PARENT_SCOPE)
        return()
    endif()
    file(STRINGS "${WORKDIR}/wegplan.txt" _geladen REGEX "RAUM ${_raum} geladen")
    if(NOT _geladen)
        string(APPEND _f "\n  ${_name}: ROOM${_raum} nie geladen")
    endif()
    file(STRINGS "${WORKDIR}/wegplan.txt" _wp_obj REGEX "obj1=\\(7942,-10800,-20510\\)")
    list(LENGTH _wp_obj _n_offen)
    if(_n_offen EQUAL 0)
        string(APPEND _f "\n  ${_name}: Rolladen obj 1 nie auf (7942,-10800,-20510) (sub07 @0x01E40 nicht gelaufen?)")
    endif()
    set(_n_cut11 0)
    foreach(_bild RANGE 600 1100 50)
        # Bild N zeigt noch den Schnitt des Vorbilds: im Umschaltbild (Zustand F649 cam=12, F650 cam=11) liegt im
        # Framedump F650 noch der Hintergrund von Cut 12 (gemessen 2026-10-17, Dossier §9.4: Anteil 0.0042, Bild =
        # Cut-12-Ansicht). Gewertet wird darum nur ein Bild, dessen Zustand UND Vorbild-Zustand cam=11 tragen.
        math(EXPR _vorbild "${_bild} - 1")
        file(STRINGS "${WORKDIR}/state.txt" _st REGEX "^F${_bild} ")
        file(STRINGS "${WORKDIR}/state.txt" _st_vor REGEX "^F${_vorbild} ")
        if(NOT _st MATCHES " cam=11 " OR NOT _st_vor MATCHES " cam=11 ")
            continue()
        endif()
        if(_bild LESS 1000)
            set(_ppm "${WORKDIR}/fd_000${_bild}.ppm")
        else()
            set(_ppm "${WORKDIR}/fd_00${_bild}.ppm")
        endif()
        if(NOT EXISTS "${_ppm}")
            string(APPEND _f "\n  ${_name}: Framedump ${_ppm} fehlt")
            continue()
        endif()
        execute_process(COMMAND "${RE15_BILD_TOOL}" --bild "${_ppm}" "${RE15_BSS}" 11 0
                        RESULT_VARIABLE _rvb OUTPUT_VARIABLE _outb TIMEOUT 60)
        if(NOT _outb MATCHES "BG-ANTEIL ([0-9.]+)")
            string(APPEND _f "\n  ${_name}: Bildwerkzeug: ${_outb}")
            continue()
        endif()
        set(_anteil "${CMAKE_MATCH_1}")
        math(EXPR _n_cut11 "${_n_cut11} + 1")
        message(STATUS "r38_rolltor20a0[${_name}] Bild ${_bild} Cut 11: Raumhintergrund-Anteil ${_anteil}")
        if(_anteil LESS _bg_min)
            string(APPEND _f "\n  ${_name}: Bild ${_bild} zugedeckt — Hintergrund-Anteil ${_anteil} < ${_bg_min}")
        endif()
    endforeach()
    if(_n_cut11 LESS 3)
        string(APPEND _f "\n  ${_name}: nur ${_n_cut11} Dumpbilder in Cut 11 (erwartet >= 3)")
    endif()
    file(STRINGS "${WORKDIR}/objpoly.txt" _op REGEX "room=${_raum} cut=11 obj=1 ")
    list(LENGTH _op _n_op)
    if(_n_op LESS 100)
        string(APPEND _f "\n  ${_name}: nur ${_n_op} OBJPOLY-Zeilen fuer Cut 11 obj 1")
    endif()
    set(_n_falsch 0)
    foreach(_z IN LISTS _op)
        if(NOT _z MATCHES "gezeichnet=6 nclip=20 otz=16 gpu=6$")
            math(EXPR _n_falsch "${_n_falsch} + 1")
            set(_bsp "${_z}")
        endif()
    endforeach()
    if(_n_falsch GREATER 0)
        string(APPEND _f "\n  ${_name}: ${_n_falsch} OBJPOLY-Zeilen Cut 11 obj 1 != Original (6/20/16/6), z. B. ${_bsp}")
    endif()
    message(STATUS "r38_rolltor20a0[${_name}] OBJPOLY Cut 11 obj 1: ${_n_op} Bilder, Abweichungen ${_n_falsch}")
    set(_fehler_${_name} "${_f}" PARENT_SCOPE)
endfunction()

# A: Leon ROOM20A0, Debug-Sprung
rolladen_lauf(a_leon sprung 20A0 wp_cut11.txt 1100 "RE15_FRAMEDUMP=600-1100/50:fd_")
pruefe_cut11(a_leon 20A0)
string(APPEND _fehler "${_fehler_a_leon}")

# B: Elza ROOM20A1, Spielstand + CONTINUE
rolladen_lauf(b_elza karte 20A1 wp_cut11.txt 1100 "RE15_FRAMEDUMP=600-1100/50:fd_")
pruefe_cut11(b_elza 20A1)
string(APPEND _fehler "${_fehler_b_elza}")
if(EXISTS "${_basis}_b_elza/debug.log")
    file(STRINGS "${_basis}_b_elza/debug.log" _elza REGEX "Spieler-Familie PL04")
    if(NOT _elza)
        fehler("b_elza: nicht mit Elza gespielt (keine Zeile 'Spieler-Familie PL04' im debug.log)")
    endif()
endif()

# D: Leon ROOM20A0, Spielstand + CONTINUE
rolladen_lauf(d_leon_karte karte 20A0 wp_cut11.txt 1100 "RE15_FRAMEDUMP=600-1100/50:fd_")
pruefe_cut11(d_leon_karte 20A0)
string(APPEND _fehler "${_fehler_d_leon_karte}")

# C: Leon, Kiste faellt durch die Oeffnung
rolladen_lauf(c_kiste sprung 20A0 wp_kiste.txt 1450)
set(_wd "${_basis}_c_kiste")
if(EXISTS "${_wd}/objfall.txt")
    file(STRINGS "${_wd}/objfall.txt" _ausl REGEX "room=20A0 p=0 AUSLOESER")
    file(STRINGS "${_wd}/objfall.txt" _land REGEX "room=20A0 p=0 LANDUNG")
    if(NOT _ausl OR NOT _land)
        fehler("c_kiste: Kiste obj 0 faellt nicht (AUSLOESER/LANDUNG fehlen in objfall.txt)")
    else()
        list(GET _land 0 _land0)
        message(STATUS "r38_rolltor20a0[c_kiste] ${_land0}")
    endif()
else()
    fehler("c_kiste: objfall.txt fehlt (exit ${_rv_c_kiste})")
endif()

file(REMOVE "${_exe_kopie}")
if(_fehler)
    message(FATAL_ERROR "r38_rolltor20a0: FEHLER${_fehler}")
endif()
message(STATUS "r38_rolltor20a0: OK (A Leon 20A0 Sprung, B Elza 20A1 Karte, D Leon 20A0 Karte, C Kiste)")
