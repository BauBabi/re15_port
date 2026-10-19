cmake_minimum_required(VERSION 3.16)
# =============================================================================
# RUNDE 39 SPUR J "dining1141", NACHBESSERUNG 3 (Abnahme 2 Mangel M1, Nutzer-Punkt 8 "Alles natuerlich mit passenden
# Animationen"): zielt Elza noch, wenn ENTRIEGELN39 nach dem Tod der Gorillas auf Cut 8 schneidet, stand sie dort 11 Bilder
# lang mit der Waffe auf John (Abnahme 2 Lauf R/R2 state.txt F6296..F6306 pst=1 mo=215; gemergter Stand Lauf v_m2 F4086..F4096).
# Echte exe, echter Weg mit Elza (Debug-Sprung nach ROOM1131, danach nur echte Tueren per RE15_WEGPLAN wie
# integration_r39_dining1141_d bis zum Riegel der Dachtuer, dann stehen), Browning im Inventar und ausgeruestet; Elza haelt
# R1 (RE15_INPUT_SCRIPT, Spielbilder 3500..4200) ueber Kampfende und Schnitt hinweg; Kampfende per Messhaken
# RE15_R38F_GORILLAS_TOT=600 mit eingefrorenen Gorillas (Kill-Flags (7,0xCB)/(7,0xCC), KEINE Schusszahl).
# Geprueft vom Werkzeug test_r39_dining1141 zielen (debug.log + state.txt):
#   Vorbedingung: Elza zielt beim Kampfende und haelt R1 bis nach dem Schnitt
#   Form ROOM11C0 sub03 @0x01B06..@0x01B18: Pad-Sperre >= 29 Bilder VOR dem Schnitt auf Cut 8
#   unter der Sperre senkt sie die Waffe (FUN_80030444 kappt das Held-Wort @0x80030514/@0x8003051c, HOLD @0x800331ec ->
#   aca5a = 3 @0x80033200), kein Bild in Cut 8 mit erhobener Waffe, im Schnittbild neutral
# Dossier analysis/befunde_runde39/J_dining1141.md Abschnitt 10, Code engine/src/dining_r39.c (re15_dining_r39_ziel_senken),
# Haken engine/src/player_common.c, Programm tools/r39_dining/programme_bauen.py (baue_entriegeln39).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_R39J_WERKZEUG=<test_r39_dining1141> -DWORKDIR=<dir> [-DAUSWERTEN=<lauf>]
#         -P test_r39_dining1141_zielen.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r39_dining1141_zielen: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_R39J_WERKZEUG OR NOT EXISTS "${RE15_R39J_WERKZEUG}")
    message(FATAL_ERROR "r39_dining1141_zielen: RE15_R39J_WERKZEUG fehlt: '${RE15_R39J_WERKZEUG}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r39_dining1141_zielen: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")
# Runde 39 Spur J Nachbesserung 3 (Abnahme 2 H4, VERTRAG Abschnitt 2): keine RE15_*-Variable des Aufrufers erben.
include("${CMAKE_CURRENT_LIST_DIR}/r39_dining1141_umgebung.cmake")
r39j_umgebung_leeren()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_lauf "${WORKDIR}/lauf_${_lauf_id}")
if(AUSWERTEN)
    set(_lauf "${AUSWERTEN}")
endif()

# Eigene exe-Kopie mit Praefix JE TEST (re15_pc_r39j_z_), nur die eigenen alten Kopien weg.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r39j_z_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r39j_z_${_lauf_id}${_exe_ext}")
if(NOT AUSWERTEN)
    file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
endif()

# Wegplan = integration_r39_dining1141_d bis zum Riegel der Dachtuer (1131 Schluessel + Tuer; 1141 Gemaelde "ja", Rolladen,
# frei laufen, Riegel, msg 12 schliessen), danach bleibt Elza stehen (Lage nach der Szene (-7998,-10284), Lauf v_m2).
set(_plan "RAUM 1131
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
BLICK 0
AKTION
RAUM 1141
WARTE 600
GEHE -7600 -12000 150
GEHE -8150 -10250 80
BLICK 2048
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 200
GEHE -7350 -8500 150
GEHE -7350 -7600 150
GEHE -7250 -1000 80
BLICK 3072
AKTION
WARTE 40
AKTION
WARTE 40
AKTION
WARTE 120
ENDE
")

if(NOT AUSWERTEN)
    file(MAKE_DIRECTORY "${_lauf}")
    file(WRITE "${_lauf}/wegplan.txt" "${_plan}")
    set(WORKDIR "${_lauf}")
    # Szenenende in 1141 ~ Bild 3456 (Lauf v_m2), Messhaken 600 freie Kampfbilder spaeter (F4056), Schnitt ~F4086: R1 von
    # Bild 3500 an 23,4 s (702 Bilder, bis ~F4202) - das Fenster deckt Kampfende und Schnitt auch bei verschobenem Ablauf.
    re15_start_spiel(_rv 1500
        RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
        RE15_MSG_LOG=1 RE15_EVT_TRACE=1 RE15_STATE_LOG=state.txt RE15_R39J_LOG=r39.txt
        "RE15_WEGPLAN=${_lauf}/wegplan.txt" RE15_WEGPLAN_LOG=wegplan_log.txt
        "RE15_EXIT_AT=4210#1141"
        "RE15_SET_FLAG=3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78" "RE15_GIVE=0x43:1,0x03:15,0x15:50" "RE15_EQUIP=3"
        "RE15_DEBUG_JUMP=1131@gp" "RE15_PLAYER_POS=-1300,-13900,0"
        "RE15_R38F_GORILLAS_TOT=600" "RE15_R38F_GORILLAS_STARR=1"
        "RE15_INPUT_SCRIPT=M23.4" "RE15_INPUT_SCRIPT_BASIS=spiel" "RE15_INPUT_SCRIPT_START=3500"
        "RE15_FRAMEDUMP=4040-4130/2:fd_"
        "${_exe_kopie}")
    file(REMOVE "${_exe_kopie}")
    message(STATUS "r39_dining1141_zielen: exit=${_rv}, ${_lauf}")
endif()

if(NOT EXISTS "${_lauf}/debug.log" OR NOT EXISTS "${_lauf}/state.txt")
    message(FATAL_ERROR "r39_dining1141_zielen: debug.log/state.txt fehlt (${_lauf})")
endif()
file(STRINGS "${_lauf}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
if(NOT _ex)
    message(FATAL_ERROR "r39_dining1141_zielen: RE15_EXIT_AT nicht erreicht (Lauf abgerissen?) ${_lauf}")
endif()
execute_process(COMMAND "${RE15_R39J_WERKZEUG}" zielen "${_lauf}" RESULT_VARIABLE _rz OUTPUT_VARIABLE _oz ERROR_VARIABLE _ez)
message(STATUS "${_oz}")
if(NOT _rz EQUAL 0)
    message(FATAL_ERROR "r39_dining1141_zielen: Werkzeug zielen meldet Fehler (rv=${_rz}) - ${_lauf}")
endif()
message(STATUS "r39_dining1141_zielen: alle Pruefungen gruen (${_lauf})")
