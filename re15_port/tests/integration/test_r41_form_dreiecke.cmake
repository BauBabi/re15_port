# =============================================================================
# DREIECKS-LISTE DER RIESENFORM, Runde 41 Form K5g3, Orchestrator-Entscheid G3 (2026-11-14).
#
# Die Riesenform mit dem Spalt B4 (Netze 10/16/21/22 fein geteilt, vom Nutzer abgenommen) hat rund 2,2-mal so viele Dreiecke wie
# K5e_f1. Die PC-Liste der texturierten Dreiecke (render_pc.c TEXTRI_QUEUE_MAX) verwarf bis jetzt STILL alles, was ueber die Grenze
# ging. G3: Grenze 8192 -> 16384 (Port-Puffer, kein Spielwert) und ein Ueberlauf-Zaehler, der im debug.log steht
# ("[tri-liste] ... verworfen=N", Schlusszeile "[tri-liste] ENDE ..." aus re15_testhaken_ende).
#
# Laeufe (eigene exe-Kopie, Modell aus RE15_FORM_ASSET_ROOT ueber RE15_RE2_ASSET_ROOT; fehlt die TIM dort, nimmt das Spiel die aus
# shared_assets/RE2):
#   kampf   ROOM5090 wie integration_r41_birkin_pistole (Debug-Sprung, laufen, Zielen + Feuer gehalten): die Form kommt, wird
#           getroffen (HP < 150), die Kamera steht dabei auch auf Schnitt 11.
#   s11     ROOM5090, Form per Messhaken RE15_BB_POSE in Clip 12 (abgespielt) an der Bild-2-Lage, Kamera RE15_FORCE_CUT=11,
#           Spielerin im Bild, schiesst (Magazin faellt).
#   v5080   ROOM5080, Form Clip 12 (abgespielt) von vorn (Gier 2095), Kamera Schnitt 1, Spielerin im Bild, schiesst.
# Gefordert je Lauf: Form geladen, Schlusszeile da, verworfen=0, keine Zeile UEBERLAUF; mit Variante dazu hoechststand >= 6000
# (K5e_f1 erreicht dort hoechstens rund 5000 - Beleg, dass wirklich das grosse Modell gezeichnet wurde).
# GEGENPROBE (muss ROT sein): s11 mit dem Messhaken RE15_TRILISTE_GRENZE=2000 -> die Pruefung muss verworfen > 0 und UEBERLAUF sehen.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> [-DRE15_FORM_ASSET_ROOT=<ordner mit BAHNSTEIG_FORM.EMD>|-] -P test_r41_form_dreiecke.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_form_dreiecke: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_form_dreiecke: WORKDIR fehlt")
endif()
if(NOT DEFINED RE15_FORM_ASSET_ROOT)
    # Seit dem R9-Einbau (15.11.2026, K5g3 + rechte Hand Variante 1) ist das Spiel-Asset selbst die zu pruefende Form
    # (vorher: K5/k5g/K5g3/geo, die Geometrie-Stufe vor dem Einbau).
    get_filename_component(RE15_FORM_ASSET_ROOT
        "${CMAKE_CURRENT_LIST_DIR}/../../shared_assets/RE2" ABSOLUTE)
endif()
set(_variante 0)
if(NOT RE15_FORM_ASSET_ROOT STREQUAL "-" AND EXISTS "${RE15_FORM_ASSET_ROOT}/BAHNSTEIG_FORM.EMD")
    set(_variante 1)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41dreiecke_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41dreiecke_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_feuer "")
foreach(_i RANGE 1 60)
    string(APPEND _feuer ",MA4")
endforeach()

# Ein Lauf; Ergebnis in <name>_fehler (leer = gruen) und <name>_text.
function(ein_lauf _name _raum _exit _skript)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_extra ${ARGN})
    if(_variante)
        list(APPEND _extra "RE15_RE2_ASSET_ROOT=${RE15_FORM_ASSET_ROOT}")
    endif()
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        RE15_DEBUG_JUMP=${_raum}@250 RE15_BIRKIN_FORM=bahnsteig RE15_SPAWN_DIAG=1
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=${_skript}"
        RE15_STATE_LOG=state.log "RE15_GIVE=3:400,21:250" RE15_EQUIP=3
        "RE15_EXIT_AT=${_exit}#${_raum}"
        ${_extra}
        "${_exe_kopie}")
    set(_g "")
    if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.log")
        set(${_name}_fehler "\n ${_name}: kein Protokoll (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    if(NOT _dbg MATCHES "Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips")
        string(APPEND _g "\n ${_name}: Riesenform nicht geladen")
    endif()
    set(_hoch -1)
    set(_verw -1)
    if(_dbg MATCHES "\\[tri-liste\\] ENDE hoechststand=([0-9]+) \\(F[0-9]+\\) verworfen=([0-9]+) bilder_mit_verlust=([0-9]+) grenze=([0-9]+)")
        set(_hoch "${CMAKE_MATCH_1}")
        set(_verw "${CMAKE_MATCH_2}")
        set(_grenze "${CMAKE_MATCH_4}")
    else()
        string(APPEND _g "\n ${_name}: keine Schlusszeile [tri-liste] ENDE (Zaehler fehlt oder Lauf nicht bis EXIT_AT)")
    endif()
    if(NOT _verw EQUAL 0)
        string(APPEND _g "\n ${_name}: ${_verw} Dreiecke verworfen (Liste voll)")
    endif()
    if(_dbg MATCHES "\\[tri-liste\\] UEBERLAUF")
        string(APPEND _g "\n ${_name}: Zeile UEBERLAUF im debug.log")
    endif()
    if(_variante AND _hoch LESS 6000)
        string(APPEND _g "\n ${_name}: hoechststand ${_hoch} < 6000 - das grosse Modell wurde nicht gezeichnet")
    endif()
    # Schuesse: das Magazin (mg=) faellt
    file(STRINGS "${WORKDIR}/state.log" _mg REGEX " mg=[0-9]+ ")
    set(_mg_max -1)
    set(_mg_min 99999)
    set(_cam11 0)
    set(_form_min 9999)
    foreach(_z IN LISTS _mg)
        if(_z MATCHES " mg=([0-9]+) ")
            if(CMAKE_MATCH_1 GREATER _mg_max)
                set(_mg_max "${CMAKE_MATCH_1}")
            endif()
            if(CMAKE_MATCH_1 LESS _mg_min)
                set(_mg_min "${CMAKE_MATCH_1}")
            endif()
        endif()
        if(_z MATCHES " cam=11 ")
            set(_cam11 1)
        endif()
        if(_z MATCHES "t=30 st=[0-9]+ [^]]*\\] hp=(-?[0-9]+)")
            if(CMAKE_MATCH_1 LESS _form_min)
                set(_form_min "${CMAKE_MATCH_1}")
            endif()
        endif()
    endforeach()
    math(EXPR _schuesse "${_mg_max} - ${_mg_min}")
    if(_schuesse LESS 5)
        string(APPEND _g "\n ${_name}: kaum Schuesse (Magazin ${_mg_max} -> ${_mg_min})")
    endif()
    if(_name STREQUAL "kampf")
        if(_form_min GREATER_EQUAL 150)
            string(APPEND _g "\n kampf: die Form wurde nicht getroffen (HP-Minimum ${_form_min})")
        endif()
        if(NOT _cam11)
            string(APPEND _g "\n kampf: Kamera nie auf Schnitt 11")
        endif()
    endif()
    set(${_name}_fehler "${_g}" PARENT_SCOPE)
    set(${_name}_text "hoechststand ${_hoch}, verworfen ${_verw}, Schuesse ${_schuesse}, Form-HP-Minimum ${_form_min}, Schnitt 11 ${_cam11}"
        PARENT_SCOPE)
endfunction()

set(_f "")
ein_lauf(kampf 5090 2600 "A0.1,W1,U8.5${_feuer}" "RE15_PLAYER_POS=500,-9175,2048,0")
ein_lauf(s11 5090 1300 "A0.1,W1${_feuer}" "RE15_PLAYER_POS=4000,-24300,1024,0"
    "RE15_BB_POSE=941,-23776,3874,12,-1,-1" RE15_FORCE_CUT=11)
ein_lauf(v5080 5080 1300 "A0.1,W1${_feuer}" "RE15_PLAYER_POS=-23000,-14000,1024,0"
    "RE15_BB_POSE=-20711,-15984,2095,12,-1,-1" RE15_FORCE_CUT=1)
ein_lauf(gegenprobe 5090 1300 "A0.1,W1${_feuer}" "RE15_PLAYER_POS=4000,-24300,1024,0"
    "RE15_BB_POSE=941,-23776,3874,12,-1,-1" RE15_FORCE_CUT=11 RE15_TRILISTE_GRENZE=2000)
file(REMOVE "${_exe_kopie}")

foreach(_n kampf s11 v5080)
    message(STATUS "r41_form_dreiecke ${_n}: ${${_n}_text}")
    string(APPEND _f "${${_n}_fehler}")
endforeach()
message(STATUS "r41_form_dreiecke gegenprobe (Grenze 2000, muss rot sein): ${gegenprobe_text}")
if(NOT gegenprobe_fehler MATCHES "Dreiecke verworfen" OR NOT gegenprobe_fehler MATCHES "UEBERLAUF")
    string(APPEND _f "\n GEGENPROBE STUMPF: mit Grenze 2000 meldet die Pruefung keinen Ueberlauf (${gegenprobe_fehler})")
endif()
if(_f)
    message(FATAL_ERROR "r41_form_dreiecke: FEHLER${_f}")
endif()
message(STATUS "r41_form_dreiecke: alle Pruefungen gruen (Variante ${_variante}: ${RE15_FORM_ASSET_ROOT})")
