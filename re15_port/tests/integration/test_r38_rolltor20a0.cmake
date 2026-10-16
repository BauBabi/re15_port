# =============================================================================
# ROLLADEN ROOM20A0/20A1 MIT DER ECHTEN EXE (Runde 38 Spur W).
#
# Nutzer 2026-10-16: "in ROOM 20A0 the shutter that you have to open to push the box down is somehow DIRECTLY in
# the camera. I cannot see anything... this is not like this in the original...."
# Dossier analysis/befunde_runde38/W_rolltor20a0.md, Code engine/src/objpoly_r38.c (+ main.c Prop-Schleife).
#
# Drei Laeufe, je eine Messkopie der exe unter eigenem Namen (Muster test_r34n_b_cursor.cmake), echter Weg ueber
# RE15_WEGPLAN (geschlossen geregelt, dachkiste_r36.c): Sprung vor die Leiter (Band 2), Leiter = Door_aot_set slot 1
# @0x01766 -> Cut 12, Schalter = Aot_set slot 3 @0x017E8 -> Frage -> Ja -> sub07 @0x01E40 (Rolladen obj 1 hoch).
#   A  Leon  ROOM20A0: nach Sueden in die Zone "12 -> 11" -> Cut 11, Framedumps F700/F850/F1000.
#   B  Elza  ROOM20A1 (Spielerwahl RE15_PSELECT_AUTO_SWITCH): derselbe Weg, dieselben Pruefungen.
#   C  Leon  ROOM20A0: Kiste obj 0 von Sueden durch die Oeffnung schieben -> Fall (Kisten-Ablauf bleibt loesbar).
# Pruefungen A/B: Cut 11 in den Dumpbildern (RE15_STATE_LOG cam=11), Rolladen y -10800, RE15_OBJPOLY_LOG fuer Cut 11
# obj 1 = Original-Messung (gezeichnet 6, nclip 20, otz 16, gpu 6 — W_belege/orig1_log.txt), und im Bild ist der
# Raumhintergrund von Cut 11 zu sehen (Werkzeug test_r38_rolltor20a0 --bild ... 11 0: Anteil der Pixel gleich dem
# dekodierten BSS-Bild; vor dem Fix zugedeckt, gemessen im Dossier).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_BILD_TOOL=<test_r38_rolltor20a0> -DRE15_BSS=<ROOM20A.BSS>
#               -DDATEN=<tests/integration/r38_rolltor20a0> -DWORKDIR=<dir> -P test_r38_rolltor20a0.cmake
# =============================================================================
foreach(_v RE15_PC_EXE RE15_BILD_TOOL RE15_BSS DATEN)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r38_rolltor20a0: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "r38_rolltor20a0: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Schwelle des Hintergrund-Anteils in Cut 11 bei Toleranz 0 (Pixel == dekodiertes BSS-Bild; gemessen 2026-10-16,
# Dossier §7): vorher (Rolladen deckt zu) 0.1305 (nur die linke Wand frei), nachher 0.9424 (Rest = Figur, Kiste,
# Raum-Objekte). Schwelle dazwischen.
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

# Ein Lauf. _raum = 20A0/20A1, _plan = Wegplan-Datei, _ende = Bild, _extra = weitere KEY=WERT.
function(rolladen_lauf _name _raum _plan _ende)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(READ "${DATEN}/${_plan}" _wp)
    string(REPLACE "RAUM 20A0" "RAUM ${_raum}" _wp "${_wp}")
    file(WRITE "${WORKDIR}/wegplan_eingabe.txt" "${_wp}")
    re15_start_spiel(_rv 600
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
        RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
        "RE15_DEBUG_JUMP=${_raum}@250" "RE15_PLAYER_POS=16500,-21050,3072,2"
        "RE15_WEGPLAN=${WORKDIR}/wegplan_eingabe.txt" RE15_WEGPLAN_LOG=wegplan.txt
        RE15_STATE_LOG=state.txt RE15_OBJPOLY_LOG=objpoly.txt RE15_OBJFALL_LOG=objfall.txt
        "RE15_EXIT_AT=${_ende}#${_raum}"
        ${ARGN}
        "${_exe_kopie}")
    set(_rv_${_name} "${_rv}" PARENT_SCOPE)
endfunction()

# Pruefung A/B: Cut 11 mit offenem Rolladen.
function(pruefe_cut11 _name _raum)
    set(WORKDIR "${_basis}_${_name}")
    set(_f "")
    if(NOT EXISTS "${WORKDIR}/state.txt" OR NOT EXISTS "${WORKDIR}/objpoly.txt")
        set(_fehler_${_name} "${_name}: state.txt/objpoly.txt fehlt (exit ${_rv_${_name}})" PARENT_SCOPE)
        return()
    endif()
    file(STRINGS "${WORKDIR}/wegplan.txt" _wp_obj REGEX "obj1=\\(7942,-10800,-20510\\)")
    list(LENGTH _wp_obj _n_offen)
    if(_n_offen EQUAL 0)
        string(APPEND _f "\n  ${_name}: Rolladen obj 1 nie auf (7942,-10800,-20510) (sub07 @0x01E40 nicht gelaufen?)")
    endif()
    foreach(_bild 700 850 1000)
        file(STRINGS "${WORKDIR}/state.txt" _st REGEX "^F${_bild} ")
        if(NOT _st MATCHES " cam=11 ")
            string(APPEND _f "\n  ${_name}: Bild ${_bild} nicht in Cut 11: ${_st}")
            continue()
        endif()
        set(_ppm "${WORKDIR}/fd_000${_bild}.ppm")
        if(_bild GREATER 999)
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
        message(STATUS "r38_rolltor20a0[${_name}] Bild ${_bild} Cut 11: Raumhintergrund-Anteil ${_anteil}")
        if(_anteil LESS _bg_min)
            string(APPEND _f "\n  ${_name}: Bild ${_bild} zugedeckt — Hintergrund-Anteil ${_anteil} < ${_bg_min}")
        endif()
    endforeach()
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

# A: Leon ROOM20A0
rolladen_lauf(a_leon 20A0 wp_cut11.txt 1000 "RE15_FRAMEDUMP=700-1000/150:fd_")
pruefe_cut11(a_leon 20A0)
string(APPEND _fehler "${_fehler_a_leon}")

# B: Elza ROOM20A1
rolladen_lauf(b_elza 20A1 wp_cut11.txt 1000 "RE15_FRAMEDUMP=700-1000/150:fd_" RE15_PSELECT_AUTO_SWITCH=1)
file(STRINGS "${_basis}_b_elza/wegplan.txt" _elza_raum REGEX "RAUM 20A1 geladen")
if(NOT _elza_raum)
    fehler("b_elza: ROOM20A1 nie geladen (Elza-Variante)")
endif()
pruefe_cut11(b_elza 20A1)
string(APPEND _fehler "${_fehler_b_elza}")

# C: Leon, Kiste faellt durch die Oeffnung
rolladen_lauf(c_kiste 20A0 wp_kiste.txt 1450)
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
message(STATUS "r38_rolltor20a0: OK (A Leon 20A0, B Elza 20A1, C Kiste)")
