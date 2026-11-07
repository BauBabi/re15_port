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
#
# NACHBESSERUNG 4 (Abnahme 2 M-A, analysis/befunde_runde41/form/STUFE3_abnahme_2.md; Dossier STUFE3.md "Nachbesserung 4"): die
# Rueckstoss-Szene hielt fest Cut 12; im echten Wagenweg war das Taumeln 16 Bilder fast schwarz (Form 1240 vor dem Auge) bzw. 7,5 s
# hinter Leons Ruecken (Spieler weicht vorher 2 s zurueck). Teile (TEIL): leon, elza (Raum 5091), zurueck_leon, zurueck_elza (nach dem
# Kampfstart 2 s Zurueckweichen, Tastenskript D2.0 wie die Abnahme-Laeufe v5090/v5091). Zusaetzlich zu 1-5 je Taumelbild (state.log:
# Form st 2/3, Clip 13, HP 1, bis zum Sturz-Clip 12):
#   6a Form im Bild: sichtbare Bildpunkte der Form >= 768 (1 % des Bildes 320 x 240)
#   6b Spielerkoerper: sichtbare Bildpunkte des Spielers (TIM-Slot 0) <= 25600 (ein Drittel, Pruefmass Abnahme 2 M-A)
#   6c Spielerverdeckung: verdeckter Anteil der Form (bedeckt - sichtbar) / bedeckt <= 500 Promille
#   6d fast schwarz: Anteil Bildpunkte mit Luma < 24 <= 600 Promille (Pruefmass Abnahme 2 M-A, Werkzeug probe_r41_form_kamera_bild)
# Sichtbar/bedeckt je Slot aus der Messschiene RE15_SLOTDECKUNG (render_pc.c: die gezeichneten Dreiecke in Zeichenfolge samt
# Vordergrund-Masken in einen Kennungspuffer), NICHT aus dem Kugelmodell der Schnittwahl. Form-Slot = der Figuren-Slot 1..18 (main.c:
# 0..18 Figuren/Objekte, ab 19 Effekte) mit der groessten bedeckten Flaeche in den Taumelbildern.
# Gegenprobe (gemessen, Dossier NB4-5): exe 2e3713c74 -> rot ("kein deck.log", die Messschiene fehlt dort); exe mit birkin_formen.c von
# 2e3713c74 und der Messschiene (fester Cut 12) -> leon/elza 6d rot (F1487-F1502: 776-1000 Promille fast schwarz), zurueck_leon 231 und
# zurueck_elza 94 Verstoesse 6a/6b/6c (Form 0 Bildpunkte sichtbar, Leon bis 56690 Bildpunkte, Form bis 1000 Promille verdeckt).
# Renderer: der des Nutzers (kein RE15_SOFTWARE_RENDER, wie die Abnahme-2-Messung); gemessen: der Software-Renderer dunkelt Cut 11 um
# 40-50 Promille ab (zurueck_elza max 599 statt 544 Promille fast schwarz) und maskiert Bildfehler (Memory reai-v2-visual-verify-gdigrab).
#
#
# NACHBESSERUNG 4 RUNDE 2 (Abnahme 3 N1/N2/E1, analysis/befunde_runde41/form/STUFE3_abnahme_3.md): die Nahsicht-Wache nach dem
# Bildmass machte die Kampfkamera VOR der Szene schlechter (gleiches Spiel je Bild): Spieler wartet 24 s (Lauf k5090) 38 statt 7
# Bilder > 600 Promille fast schwarz (Cut 12 mit der Form 1660-2160 vor dem Auge, Kurzschnitte), Spieler weicht 2 s zurueck (v5090)
# 28 Bilder dunkler Aussenblick Cut 14 statt 0. Die Riegel prueften nur die Taumelbilder. Neu:
#   7  Kampfkamera: je Bild von F1150 (Framedump-Beginn; Wagen C ab ~F946, der Eintritt in Cut 14 ist die Zonenkamera der Tuer) bis
#      vor die Rueckstoss-Szene bzw. bis zum Laufende Anteil fast schwarzer Bildpunkte (Luma < 24, wie 6d). Hoechstzahl Bilder
#      > 600 Promille: leon/elza/zurueck_* 0, warten_* 9; dazu kein Bild ueber 650 Promille. Beides = Stand 2e3713c74 (Achsenregel),
#      gemessen in diesem Riegel (Fenster 320x240): warten_leon 9 Bilder > 600, max 622 (F1889), warten_elza 1, max 619; leon/elza/
#      zurueck_* 0, max 596 (Abnahme 3 mit 640x480 und echter HP: kv_leon 7, max 618). Runde 1 (Wache nach dem Bildmass): warten 38,
#      max 701-707; zurueck 28, max 773-775. Die 650 ist PORT-WAHL zwischen beiden Messungen.
#   Teile warten_leon/warten_elza: Tastenskript wie leon/elza, aber 24 s warten statt 2 s (Abnahme 2/3 Lauf k5090), echte Spieler-HP
#   (ohne Messhaken; Leon faellt bis F1931 auf HP 10), Laufende F1931 wie k5090. Die Form wird dort bis F1931 nur einmal getroffen,
#   die Pruefungen 3 und 5 (Kampfdauer, Rueckstoss) gelten nur in den anderen vier Teilen.
# Gegenprobe (gemessen, Dossier STUFE3.md "Nachbesserung 4 Runde 2"): exe b9ab865af (Wache nach dem Bildmass) -> warten_leon/
# warten_elza und zurueck_leon/zurueck_elza rot in Pruefung 7.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> [-DRE15_FORM_HP_HEADER=<re15_birkin_formen.h>] [-DTEIL=leon|elza|zurueck_leon|
#         zurueck_elza|warten_leon|warten_elza] [-DRE15_BILD_TOOL=<probe_r41_form_kamera_bild>] -P test_r41_form_wagenweg.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_form_wagenweg: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_form_wagenweg: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL leon)
endif()
if(NOT TEIL MATCHES "^(leon|elza|zurueck_leon|zurueck_elza|warten_leon|warten_elza)$")
    message(FATAL_ERROR "r41_form_wagenweg: TEIL '${TEIL}' unbekannt")
endif()
set(_raum 5090)
set(_figur Leon)
if(TEIL MATCHES "elza")
    set(_raum 5091)
    set(_figur Elza)
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
set(_kampf_max 0)          # Pruefung 7: Bilder > 600 Promille fast schwarz vor der Szene (s. Kopf)
set(_warten 0)
if(TEIL MATCHES "^warten")
    set(_warten 1)
    set(_exit_bild 1931)   # = Abnahme 2/3 Lauf k5090 (RE15_EXIT_AT=1931#5090)
    set(_kampf_max 9)      # = Stand 2e3713c74 in diesem Riegel (warten_leon 9 Bilder > 600, max 622; s. Kopf Pruefung 7)
endif()
math(EXPR _limit_s "120 + ${_exit_bild} / 20")

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41wagenweg_${TEIL}_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41wagenweg_${TEIL}_${_lauf_id}.exe")
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
if(_warten)
    string(APPEND _skript "A0.1000,W0.6000,U4.9000,W24")   # Nachbesserung 4 Runde 2: 24 s warten (Abnahme 2/3 Lauf k5090)
else()
    string(APPEND _skript "A0.1000,W0.6000,U4.9000,W2")
endif()
if(TEIL MATCHES "^zurueck")
    string(APPEND _skript ",D2.0")       # Nachbesserung 4: 2 s zurueckweichen (Abnahme 2 v5090/v5091)
endif()
math(EXPR _ma_n "${_exit_bild} / 120 + 2")
foreach(_i RANGE 1 ${_ma_n})
    string(APPEND _skript ",MA4")
endforeach()

set(_figur_env "")
set(_haken_env RE15_R39_MESS_SPIELER_HP=100)
if(_warten)
    set(_haken_env "")       # echte Spieler-HP wie k5090
endif()
if(_raum EQUAL 5091)
    set(_figur_env RE15_TITLE_SHOT_CHAR=1)
endif()
re15_start_spiel(_rv ${_limit_s}
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_figur_env}
    RE15_SLOTDECKUNG=deck.log "RE15_FRAMEDUMP=1150-2000/1:rs_"
    RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=27200,4900,2048,0" "RE15_SET_FLAG=3:89"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=6 "RE15_INPUT_SCRIPT=${_skript}"
    RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log "RE15_GIVE=3:400,21:250" RE15_EQUIP=3
    ${_haken_env}
    "RE15_EXIT_AT=${_exit_bild}#${_raum}"
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
if(_treffer LESS _treffer_min AND NOT _warten)
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
elseif((_form_alt STREQUAL "" OR NOT _form_alt GREATER 0) AND NOT _warten)
    string(APPEND _f "\n keine Rueckstoss-Szene (Clip 13, HP 1) bis F${_exit_bild}, Form steht bei HP '${_form_alt}'")
endif()
if(_pl_treffer LESS 1)
    string(APPEND _f "\n Kampf: ${_figur} wird zwischen dem ersten Treffer (F${_erster_treffer}) und dem Kampfende (Rueckstoss F${_rueck}) nicht getroffen")
endif()

# --- 6 Kamera der Rueckstoss-Szene (Nachbesserung 4) ---
set(_taumel "")
foreach(_z IN LISTS _zeilen)
    if(_z MATCHES "^F([0-9]+) pad=" AND _z MATCHES "t=30 st=[23] ss1=[0-9]+ ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=13 [^]]*\\] hp=1( |$)")
        string(REGEX MATCH "^F([0-9]+)" _m "${_z}")
        list(APPEND _taumel "${CMAKE_MATCH_1}")
    endif()
endforeach()
list(LENGTH _taumel _taumel_n)
set(_k "")
set(_kam_info "")
if(_warten)
    set(_kam_info "keine Szene bis F${_exit_bild} (Teil warten)")
elseif(_taumel_n LESS 60)
    string(APPEND _f "\n Kamera: nur ${_taumel_n} Taumelbilder (Clip 13, HP 1) im state.log, erwartet >= 60")
elseif(NOT EXISTS "${WORKDIR}/deck.log")
    string(APPEND _f "\n Kamera: kein deck.log (Messschiene RE15_SLOTDECKUNG fehlt in dieser exe)")
else()
    file(STRINGS "${WORKDIR}/deck.log" _deck REGEX "^F[0-9]+ cam=")
    foreach(_d IN LISTS _deck)
        if(_d MATCHES "^F([0-9]+) ")
            set(_dk_${CMAKE_MATCH_1} "${_d}")
        endif()
    endforeach()
    # Form-Slot: Figuren-Slot 1..18 mit der groessten bedeckten Flaeche ueber die Taumelbilder
    foreach(_s RANGE 1 18)
        set(_sum_${_s} 0)
    endforeach()
    foreach(_t IN LISTS _taumel)
        if(DEFINED _dk_${_t})
            string(REGEX MATCHALL " s[0-9]+=[0-9]+/[0-9]+" _sl "${_dk_${_t}}")
            foreach(_e IN LISTS _sl)
                if(_e MATCHES " s([0-9]+)=([0-9]+)/([0-9]+)" AND CMAKE_MATCH_1 GREATER 0 AND CMAKE_MATCH_1 LESS 19)
                    math(EXPR _sum_${CMAKE_MATCH_1} "${_sum_${CMAKE_MATCH_1}} + ${CMAKE_MATCH_3}")
                endif()
            endforeach()
        endif()
    endforeach()
    set(_fslot -1)
    set(_fsum 0)
    foreach(_s RANGE 1 18)
        if(_sum_${_s} GREATER _fsum)
            set(_fsum ${_sum_${_s}})
            set(_fslot ${_s})
        endif()
    endforeach()
    # fast schwarz aus der Framedump-Serie
    list(GET _taumel 0 _t0)
    list(GET _taumel -1 _t1)
    if(RE15_BILD_TOOL AND EXISTS "${RE15_BILD_TOOL}")
        execute_process(COMMAND "${RE15_BILD_TOOL}" "${WORKDIR}/rs_" ${_t0} ${_t1} RESULT_VARIABLE _rvb OUTPUT_VARIABLE _outb ERROR_VARIABLE _errb)
        string(REGEX MATCHALL "F[0-9]+ dunkel=[0-9]+" _dl "${_outb}")
        foreach(_e IN LISTS _dl)
            if(_e MATCHES "F([0-9]+) dunkel=([0-9]+)")
                set(_dn_${CMAKE_MATCH_1} ${CMAKE_MATCH_2})
            endif()
        endforeach()
    else()
        string(APPEND _f "\n Kamera: RE15_BILD_TOOL fehlt (probe_r41_form_kamera_bild)")
    endif()
    set(_fehl_n 0)
    set(_ohne 0)
    set(_dn_n 0)
    set(_min_form 999999)
    set(_max_pl 0)
    set(_max_deck 0)
    set(_max_dn 0)
    set(_kams "")
    foreach(_t IN LISTS _taumel)
        if(DEFINED _dn_${_t})
            math(EXPR _dn_n "${_dn_n} + 1")
            if(_dn_${_t} GREATER _max_dn)
                set(_max_dn ${_dn_${_t}})
            endif()
            if(_dn_${_t} GREATER 600)
                math(EXPR _fehl_n "${_fehl_n} + 1")
                if(_fehl_n LESS 7)
                    string(APPEND _k "\n   F${_t}: ${_dn_${_t}} Promille fast schwarz (> 600)")
                endif()
            endif()
        endif()
        if(NOT DEFINED _dk_${_t})
            math(EXPR _ohne "${_ohne} + 1")
            continue()
        endif()
        set(_fv 0)
        set(_fb 0)
        set(_pv 0)
        string(REGEX MATCH "cam=(-?[0-9]+)" _m "${_dk_${_t}}")
        set(_cam "${CMAKE_MATCH_1}")
        list(FIND _kams "${_cam}" _kpos)
        if(_kpos LESS 0)
            list(APPEND _kams "${_cam}")
        endif()
        if(_dk_${_t} MATCHES " s${_fslot}=([0-9]+)/([0-9]+)")
            set(_fv ${CMAKE_MATCH_1})
            set(_fb ${CMAKE_MATCH_2})
        endif()
        if(_dk_${_t} MATCHES " s0=([0-9]+)/")
            set(_pv ${CMAKE_MATCH_1})
        endif()
        set(_deck 0)
        if(_fb GREATER 0)
            math(EXPR _deck "(${_fb} - ${_fv}) * 1000 / ${_fb}")
        endif()
        if(_fv LESS _min_form)
            set(_min_form ${_fv})
        endif()
        if(_pv GREATER _max_pl)
            set(_max_pl ${_pv})
        endif()
        if(_deck GREATER _max_deck)
            set(_max_deck ${_deck})
        endif()
        set(_grund "")
        if(_fv LESS 768)
            string(APPEND _grund " Form sichtbar ${_fv} < 768")
        endif()
        if(_pv GREATER 25600)
            string(APPEND _grund " ${_figur} ${_pv} > 25600")
        endif()
        if(_deck GREATER 500)
            string(APPEND _grund " Form zu ${_deck} Promille verdeckt")
        endif()
        if(_grund)
            math(EXPR _fehl_n "${_fehl_n} + 1")
            if(_fehl_n LESS 7)
                string(APPEND _k "\n   F${_t} Cut ${_cam}:${_grund}")
            endif()
        endif()
    endforeach()
    math(EXPR _mit "${_taumel_n} - ${_ohne}")
    math(EXPR _mit_min "${_taumel_n} * 9 / 10")
    if(_mit LESS _mit_min)
        string(APPEND _f "\n Kamera: nur ${_mit} von ${_taumel_n} Taumelbildern im deck.log")
    endif()
    if(_dn_n LESS 20)
        string(APPEND _f "\n Kamera: nur ${_dn_n} Framedump-Bilder in den Taumelbildern F${_t0}-F${_t1}")
    endif()
    if(_fehl_n GREATER 0)
        string(APPEND _f "\n Kamera der Rueckstoss-Szene: ${_fehl_n} Verstoesse in F${_t0}-F${_t1}${_k}")
    endif()
    set(_kam_info "Taumeln F${_t0}-F${_t1} (${_taumel_n} Bilder, Schnitte ${_kams}), Form-Slot ${_fslot} min sichtbar ${_min_form}, ${_figur} max ${_max_pl}, Form max ${_max_deck} Promille verdeckt, max ${_max_dn} Promille fast schwarz (${_dn_n} Bilder)")
endif()
# --- 7 Kampfkamera vor der Szene (Nachbesserung 4 Runde 2) ---
set(_k7_bis ${_exit_bild})
if(NOT _rueck STREQUAL "")
    math(EXPR _k7_bis "${_rueck} - 1")
endif()
set(_k7_info "")
if(RE15_BILD_TOOL AND EXISTS "${RE15_BILD_TOOL}")
    execute_process(COMMAND "${RE15_BILD_TOOL}" "${WORKDIR}/rs_" 1150 ${_k7_bis} RESULT_VARIABLE _rv7 OUTPUT_VARIABLE _out7 ERROR_VARIABLE _err7)
    string(REGEX MATCHALL "F[0-9]+ dunkel=[0-9]+" _dl7 "${_out7}")
    list(LENGTH _dl7 _n7)
    set(_ueber7 0)
    set(_max7 0)
    set(_max7_f "")
    set(_k7 "")
    foreach(_e IN LISTS _dl7)
        if(_e MATCHES "F([0-9]+) dunkel=([0-9]+)")
            if(CMAKE_MATCH_2 GREATER _max7)
                set(_max7 ${CMAKE_MATCH_2})
                set(_max7_f ${CMAKE_MATCH_1})
            endif()
            if(CMAKE_MATCH_2 GREATER 600)
                math(EXPR _ueber7 "${_ueber7} + 1")
                if(_ueber7 LESS 9)
                    string(APPEND _k7 "\n   F${CMAKE_MATCH_1}: ${CMAKE_MATCH_2} Promille fast schwarz")
                endif()
            endif()
        endif()
    endforeach()
    math(EXPR _n7_min "(${_k7_bis} - 1150) * 9 / 10")
    if(_n7 LESS _n7_min)
        string(APPEND _f "\n Kampfkamera: nur ${_n7} Framedump-Bilder in F1150-F${_k7_bis}, erwartet >= ${_n7_min}")
    endif()
    if(_max7 GREATER 650)
        string(APPEND _f "
 Kampfkamera vor der Szene (F1150-F${_k7_bis}): F${_max7_f} ${_max7} Promille fast schwarz, erlaubt 650")
    endif()
    if(_ueber7 GREATER _kampf_max)
        string(APPEND _f "\n Kampfkamera vor der Szene (F1150-F${_k7_bis}): ${_ueber7} Bilder > 600 Promille fast schwarz, erlaubt ${_kampf_max}${_k7}")
    endif()
    set(_k7_info "F1150-F${_k7_bis} ${_n7} Bilder, ${_ueber7} > 600 Promille fast schwarz (erlaubt ${_kampf_max}), max ${_max7} (F${_max7_f})")
else()
    string(APPEND _f "\n Kampfkamera: RE15_BILD_TOOL fehlt (probe_r41_form_kamera_bild)")
endif()

message(STATUS "r41_form_wagenweg[${TEIL}]: Kamera ${_kam_info}")
message(STATUS "r41_form_wagenweg[${TEIL}]: Kampfkamera ${_k7_info}")
message(STATUS "r41_form_wagenweg: Soll-HP ${_form_hp_soll}, Wagen B ${_hp_b}, Wagen C ${_hp_c} (F${_bild_c}), "
               "Treffer ${_treffer} (Abzug ${_abzug}, groesster ${_max_einzel}) ab F${_erster_treffer}, Rueckstoss F${_rueck}, Form-HP zuletzt ${_form_alt}, ${_figur} getroffen ${_pl_treffer}x, "
               "exit=${_rv}, Lauf ${WORKDIR}")
if(_f)
    message(FATAL_ERROR "r41_form_wagenweg[${TEIL}]: FEHLER${_f}")
endif()
file(GLOB _ppm "${WORKDIR}/rs_*.ppm")
if(_ppm)
    file(REMOVE ${_ppm})
endif()
message(STATUS "r41_form_wagenweg[${TEIL}]: alle Pruefungen gruen")
