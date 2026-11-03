# =============================================================================
# RIESENFORM IM ECHTEN WAGENWEG, Runde 41 Spur Form, Nachbesserung 3 (AN-Abnahme 2 Mangel N1,
# .claude/worktrees/r41_annette/analysis/befunde_runde41/AN_abnahme_2.md; Dossier analysis/befunde_runde41/form/STUFE3.md).
#
# Befund: im Spielweg 6030-Tuer -> Wagen A -> B -> C kam die Riesenform mit HP 0 in Wagen C an (A 300, B 150, C 0); der erste
# Pistolentreffer startete die Rueckstoss-Szene, der Kampf fiel weg. Die bisherigen Integrationstests sahen das nicht, weil sie
# per Debug-Sprung in Wagen B beginnen (nur EINE Selbst-Tuer).
# Ursache: die Wagentueren fuehren in denselben Raum (scd_room_reenter, kein re15_enemy_reset). Sce_em_set legt die Form jedes Mal
# neu in Slot 2 an (+0x4 = 0 `sw zero,4(s0)` @0x800421e0, hp 0); der Eintritt von re15_bb_tick (Gegenstueck des G4-Ctors
# 0x8010062c, Tabelle @0x8010af60[0], HP `sh v0,342(s1)` @0x8010067c) lief nur bei neuem SLOT und blieb in Wagen C aus.
#
# Lauf (ein Lauf Leon, kein Debug-Sprung in Wagen B): Raumladung 5090 am Eintrittspunkt der 6030-Tuer (27200,4900) - der
# Debug-Sprung nimmt denselben Raumwechsel (re15_room_request_change -> re15_room_apply_pending) wie die 6030-Tuer -, dann
# zu Fuss Tuer A -> B (Slot 0, Schnitt 9) und B -> C (Slot 2, Schnitt 14), zum Kampfstart, Zielen und Feuer gehalten.
# Eingabe bis zum Kampfstart = Lauf kette_ohne der AN-Abnahme 2 (Tastenskript w8), nur die Wartezeit in Wagen C ist kuerzer.
# Geprueft aus state.log/debug.log:
#   1  der Weg: beide Wagentueren (DOOR FIRE slot=0 target_cut=9, slot=2 target_cut=14), drei Spawns des 0x30
#   2  HP der Form beim Eintritt in Wagen B UND C > 0 und = RE15_BB_FORM_HP (aus include/re15_birkin_formen.h gelesen)
#   3  der Kampf dauert: Treffer auf die Form im Kampffenster (Eintritt C bis Rueckstoss-Szene bzw. Laufende) >= RE15_BB_FORM_HP / 30
#      (mindestens 3); die HP steigen im Fenster nie (kein Ruecksetzen, der Schaden bleibt)
#   4  Leon wird im Kampf angegriffen: seine HP fallen mindestens einmal zwischen dem ersten Treffer auf die Form und dem Fensterende
#   5  Rueckstoss-Szene (Clip 13, HP 1): kommt sie, dann erst nach Abzug >= RE15_BB_FORM_HP - groesster Einzelabzug (die Form verliert
#      ihre ganzen HP bis auf den letzten Treffer; der Todesbild-Wert steht nicht im state.log, die Szene setzt sofort HP 1). Kommt
#      sie bis zum Laufende nicht, muss die Form noch leben (letzte HP > 0). Mit 150 HP kommt sie (F1486). Probelauf mit 1000 HP
#      (GR-Wert, nur der Kopf geaendert): C 1000, 64 Treffer, Leon 42x getroffen, keine Szene bis F5500, Form bei HP 40: Leon steht
#      ab ~F3250 an der Suedwestecke (-12,-26433) und zielt auf den naeheren 0x4D-Aktor (r41/form legt ihn aus der RDT an, (600,-25300),
#      Zustand 4, hp -1; Spur AN verwirft ihn bei (3,89) ohne (9,133)).
# Messhaken RE15_R39_MESS_SPIELER_HP=100 (engine/src/gegner_r39.c, KEIN Spielverhalten, nur mit Env): ein stehender
# Skript-Schuetze ueberlebt eine Form mit mehr HP nicht (Spur GR NB3: 1000 HP, Spieler tot F2100). Der Haken hebt die HP am
# Kopf jedes KI-Bildes; ein Abzug steht im state.log des Bildes, in dem er faellt (Pruefung 4 sieht ihn).
# Gegenprobe: exe 917195879 (vor der Behebung) -> Pruefungen 2 (C: HP 0), 3 und 4 rot.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> [-DRE15_FORM_HP_HEADER=<re15_birkin_formen.h>] -P test_r41_form_wagenweg.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_form_wagenweg: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_form_wagenweg: WORKDIR fehlt")
endif()
if(NOT RE15_FORM_HP_HEADER)
    set(RE15_FORM_HP_HEADER "${CMAKE_CURRENT_LIST_DIR}/../../include/re15_birkin_formen.h")
endif()
file(STRINGS "${RE15_FORM_HP_HEADER}" _hp_zeile REGEX "^#define RE15_BB_FORM_HP [0-9]+")
if(NOT _hp_zeile MATCHES "RE15_BB_FORM_HP ([0-9]+)")
    message(FATAL_ERROR "r41_form_wagenweg: RE15_BB_FORM_HP nicht in ${RE15_FORM_HP_HEADER}")
endif()
set(_form_hp_soll "${CMAKE_MATCH_1}")
math(EXPR _treffer_min "${_form_hp_soll} / 30")
if(_treffer_min LESS 3)
    set(_treffer_min 3)
endif()
# Endbild: erster Treffer F1237, 15 je Treffer. Gemessen: 150 HP -> 10 Treffer bis F1464, Szene F1486 (~25 Bilder je Treffer);
# 1000 HP (Probelauf mit dem GR-Wert) -> 64 Treffer bis F4500 (~51 Bilder je Treffer, Leon wird 31x getroffen und unterbrochen).
# 1500 + 4 x HP: 150 -> 2100, 1000 -> 5500.
math(EXPR _exit_bild "1500 + ${_form_hp_soll} * 4")
math(EXPR _limit_s "120 + ${_exit_bild} / 20")

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41wagenweg_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41wagenweg_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# Tastenskript w8 (AN-Abnahme 2, Lauf kette_ohne): Wagen A nach Westen, Tuer A -> B, Wagen B, Tuer B -> C, Wagen C bis zum
# Kampfstart; danach statt 24 s nur 2 s warten, dann Zielen + Feuer gehalten (MA4).
set(_skript "")
foreach(_i RANGE 1 7)
    string(APPEND _skript "A0.1000,W0.1000,")
endforeach()
string(APPEND _skript "U15.0000,")
foreach(_i RANGE 1 10)
    string(APPEND _skript "A0.1000,W0.1000,")
endforeach()
string(APPEND _skript "L0.0333,U2.1667,R0.0333,U10.7000,")
foreach(_i RANGE 1 9)
    string(APPEND _skript "A0.1000,W0.1000,")
endforeach()
string(APPEND _skript "A0.1000,W0.6000,U4.9000,W2")
math(EXPR _ma_n "${_exit_bild} / 120 + 2")
foreach(_i RANGE 1 ${_ma_n})
    string(APPEND _skript ",MA4")
endforeach()

re15_start_spiel(_rv ${_limit_s}
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=5090@250 "RE15_PLAYER_POS=27200,4900,2048,0" "RE15_SET_FLAG=3:89"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=6 "RE15_INPUT_SCRIPT=${_skript}"
    RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log "RE15_GIVE=3:400,21:250" RE15_EQUIP=3
    RE15_R39_MESS_SPIELER_HP=100
    "RE15_EXIT_AT=${_exit_bild}#5090"
    "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")

set(_f "")
if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.log")
    message(FATAL_ERROR "r41_form_wagenweg: kein Protokoll (exit=${_rv}) in ${WORKDIR}")
endif()

# --- 1 der Weg ---
file(READ "${WORKDIR}/debug.log" _dbg)
if(NOT _dbg MATCHES "DOOR FIRE slot=0 [^\n]*target_cut=9 spawn=\\(24700,0,-9250\\)")
    string(APPEND _f "\n Weg: Tuer A -> B (DOOR FIRE slot=0, Schnitt 9) fehlt")
endif()
if(NOT _dbg MATCHES "DOOR FIRE slot=2 [^\n]*target_cut=14 spawn=\\(24800,0,-23550\\)")
    string(APPEND _f "\n Weg: Tuer B -> C (DOOR FIRE slot=2, Schnitt 14) fehlt")
endif()
string(REGEX MATCHALL "Sce_em_set type=0x30 " _spawns "${_dbg}")
list(LENGTH _spawns _spawn_n)
if(_spawn_n LESS 3)
    string(APPEND _f "\n Weg: ${_spawn_n} Spawns des 0x30, erwartet 3 (A, B, C)")
endif()

# --- 2 bis 5 aus dem state.log ---
file(STRINGS "${WORKDIR}/state.log" _zeilen REGEX "^F[0-9]+ pad=")
set(_phase 0)          # 0 = vor 5090, 1 = Wagen A, 2 = Wagen B, 3 = Wagen C (Kampf), 4 = Rueckstoss
set(_hp_b "")
set(_hp_c "")
set(_bild_c "")
set(_form_alt "")
set(_treffer 0)
set(_abzug 0)
set(_max_einzel 0)
set(_hp_hoch "")
set(_erster_treffer "")
set(_pl_alt "")
set(_pl_treffer 0)
set(_rueck "")
foreach(_z IN LISTS _zeilen)
    if(NOT _z MATCHES "^F([0-9]+) pad=[0-9A-Fa-f]+ PL\\((-?[0-9]+),(-?[0-9]+),rot=-?[0-9]+,hp=(-?[0-9]+)\\)")
        continue()
    endif()
    set(_bild "${CMAKE_MATCH_1}")
    set(_px "${CMAKE_MATCH_2}")
    set(_pz "${CMAKE_MATCH_3}")
    set(_php "${CMAKE_MATCH_4}")
    set(_ist_form 0)
    if(_z MATCHES "\\[[0-9]+ t=30 st=([0-9]+) ss1=[0-9]+ ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=([0-9]+) [^]]*\\] hp=(-?[0-9]+)")
        set(_ist_form 1)
        set(_st "${CMAKE_MATCH_1}")
        set(_mo "${CMAKE_MATCH_2}")
        set(_fhp "${CMAKE_MATCH_3}")
    endif()
    if(_phase EQUAL 0)
        if(_ist_form)
            set(_phase 1)
        endif()
        continue()
    endif()
    if(_phase EQUAL 1 AND _px EQUAL 24700 AND _pz EQUAL -9250)
        set(_phase 2)
        if(_ist_form)
            set(_hp_b "${_fhp}")
        endif()
        continue()
    endif()
    if(_phase EQUAL 2 AND _px EQUAL 24800 AND _pz EQUAL -23550)
        set(_phase 3)
        set(_bild_c "${_bild}")
        if(_ist_form)
            set(_hp_c "${_fhp}")
            set(_form_alt "${_fhp}")
        endif()
        set(_pl_alt "${_php}")
        continue()
    endif()
    if(NOT _phase EQUAL 3)
        continue()
    endif()
    if(_ist_form)
        if((_st EQUAL 2 OR _st EQUAL 3) AND _mo EQUAL 13 AND _fhp EQUAL 1)
            set(_rueck "${_bild}")
            set(_phase 4)
            continue()
        endif()
        if(NOT _form_alt STREQUAL "" AND _fhp GREATER _form_alt AND _hp_hoch STREQUAL "")
            set(_hp_hoch "F${_bild} ${_form_alt} -> ${_fhp}")
        endif()
        if(NOT _form_alt STREQUAL "" AND _fhp LESS _form_alt)
            math(EXPR _treffer "${_treffer} + 1")
            math(EXPR _einzel "${_form_alt} - ${_fhp}")
            math(EXPR _abzug "${_abzug} + ${_einzel}")
            if(_einzel GREATER _max_einzel)
                set(_max_einzel "${_einzel}")
            endif()
            if(_erster_treffer STREQUAL "")
                set(_erster_treffer "${_bild}")
            endif()
        endif()
        set(_form_alt "${_fhp}")
    endif()
    if(NOT _erster_treffer STREQUAL "" AND NOT _pl_alt STREQUAL "" AND _php LESS _pl_alt)
        math(EXPR _pl_treffer "${_pl_treffer} + 1")
    endif()
    set(_pl_alt "${_php}")
endforeach()

if(NOT _hp_b STREQUAL "${_form_hp_soll}")
    string(APPEND _f "\n Wagen B: Form mit HP '${_hp_b}', erwartet ${_form_hp_soll} (RE15_BB_FORM_HP)")
endif()
if(_hp_c STREQUAL "" OR NOT _hp_c GREATER 0 OR NOT _hp_c EQUAL _form_hp_soll)
    string(APPEND _f "\n Wagen C (F${_bild_c}): Form mit HP '${_hp_c}', erwartet ${_form_hp_soll} (RE15_BB_FORM_HP, N1: 0)")
endif()
if(_treffer LESS _treffer_min)
    string(APPEND _f "\n Kampf: ${_treffer} Treffer auf die Form bis zur Rueckstoss-Szene bzw. F${_exit_bild}, erwartet >= ${_treffer_min}")
endif()
if(NOT _hp_hoch STREQUAL "")
    string(APPEND _f "\n Kampf: HP der Form steigen im Kampf (${_hp_hoch}), der Schaden bleibt nicht")
endif()
if(NOT _rueck STREQUAL "")
    math(EXPR _abzug_min "${_form_hp_soll} - ${_max_einzel}")
    if(_abzug LESS _abzug_min OR _abzug LESS 1)
        string(APPEND _f "\n Kampf: Rueckstoss-Szene F${_rueck} schon nach ${_abzug} HP Abzug, erwartet >= ${_abzug_min} (${_form_hp_soll} - Einzelabzug ${_max_einzel})")
    endif()
elseif(_form_alt STREQUAL "" OR NOT _form_alt GREATER 0)
    string(APPEND _f "\n keine Rueckstoss-Szene (Clip 13, HP 1) bis F${_exit_bild}, Form steht bei HP '${_form_alt}'")
endif()
if(_pl_treffer LESS 1)
    string(APPEND _f "\n Kampf: Leon wird zwischen dem ersten Treffer (F${_erster_treffer}) und dem Kampfende (Rueckstoss F${_rueck}) nicht getroffen")
endif()
message(STATUS "r41_form_wagenweg: Soll-HP ${_form_hp_soll}, Wagen B ${_hp_b}, Wagen C ${_hp_c} (F${_bild_c}), "
               "Treffer ${_treffer} (Abzug ${_abzug}, groesster ${_max_einzel}) ab F${_erster_treffer}, Rueckstoss F${_rueck}, Form-HP zuletzt ${_form_alt}, Leon getroffen ${_pl_treffer}x, "
               "exit=${_rv}, Lauf ${WORKDIR}")
if(_f)
    message(FATAL_ERROR "r41_form_wagenweg: FEHLER${_f}")
endif()
message(STATUS "r41_form_wagenweg: alle Pruefungen gruen")
