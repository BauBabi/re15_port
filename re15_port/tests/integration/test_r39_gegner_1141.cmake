# =============================================================================
# Runde 39 Spur E "gegner", Nachbesserung 2 — die Gorilla-Regel (HP x2, Flinch erst nach 36 Energie) an der ECHTEN exe
# in ROOM1141 mit ELZA (der zweite 0x27-Raum des Zensus neben ROOM11C0), echter Weg 1131 -> Tuer -> 1141 (Phase H,
# Gemaelde, Szene BEFREIUNG: die Gorillas brechen ein), dann der Kampf mit Elzas SIG P228 bis Spielbild 4500 (im Messlauf
# haben beide Gorillas ihren ersten Flinch bei F3129 / F3780 hinter sich; 600 Bilder Spielraum fuer Szenen-Aenderungen). Tod beider Gorillas -> Kill-Flags -> sub01 (4,0xF2)=0 -> Johns
# Abgang misst unit_r39_gegner_1141_ablauf1141 (echte Engine-Wege, 31 Treffer je Gorilla); der lange Exe-Kampf steht im
# Dossier (Versuche 2-6: der stehende Skript-Schuetze ist dafuer kein stabiler Riegel, Befund E).
#
# Nutzer-Nachtrag 3 (AUFTRAG.md 2026-10-18 14:25): "Zu 2. ja, wir machen das Gorilla Verhalten generell so." —
# HP 2 x 180 (Zeile 0x27 @0x8011f514, Saat `sh v0,154(a1)` @0x80117104) = 360; Flinch erst, wenn seit dem letzten
# Flinch 3 x 12 = 36 Energie abgezogen sind (Pistole 12 @0x8006ee44; Elzas P228 = Waffe 4 = 12 @0x8006ee48).
# Dossier analysis/befunde_runde39/E_gegner.md "Nachbesserung 2"; Code engine/src/gegner_r39.c (re15_r39_affen_regel).
#
# Aufbau wie der Messlauf n1141_m des Dossiers (Muster integration_r38_dining1141_d bis zum Gemaelde): Elza (Figur 4,
# RE15_PSELECT_AUTO(_SWITCH)), Nach-1021-Flags, Dining Room Key 0x43, P228 0x04, RE15_DEBUG_JUMP=1131@gp, Wegplan
# 1131 Schluessel -> Tuer -> 1141 Phase H -> Gemaelde "ja" -> BEFREIUNG. Ab Spielbild 2760 schiesst ein STEHENDER
# Skript-Schuetze (RE15_INPUT_SCRIPT: R1 gehalten, alle sechs Druecke neu zielen).
# Mess-Aufbau = Messlauf n1141_m des Dossiers (bis F4500 bildgleich): Messhaken RE15_R39_MESS_SPIELER_HP=100 (gegner_r39.c,
# nur mit Env; haelt die Spieler-HP >= 50 und damit den Luft-Finisher fern, Commit nur bei player.hp < 50 @0x80118cf4 —
# die Gorilla-Seite bleibt unabhaengig davon, wie oft Elza gebissen wird) und 99 Schuss im Magazin (RE15_GIVE; bis F4500
# fallen 24 Schuss, kein Nachladen; erwartet: 20 Treffer-Zeilen, 6 x FLINCH = 6 Flinch-Beginne). Gemessen wird die Gorilla-Seite: Energie und Zucken.
#
# Geprueft: (a) gegner_r39.log: Messhaken aktiv; jeder 0x27-INIT in ROOM1141 mit hp=360 (regel 1), keiner mit 180;
# (b) jede Treffer-Zeile folgt der Schwelle (FLINCH nur bei akku >= 36, OHNE ZUCKEN nur darunter) und mindestens ein
# Gorilla zeigt das Muster OHNE akku=12, OHNE akku=24, FLINCH akku=36 (gemessen n1141_m: beide, Platz 2 F3081/F3105/F3129,
# Platz 3 F3513/F3756/F3780); (c) state.txt: in jedem Bild mit Clip 7 in Zustand 2 (Flinch) ging ein FLINCH-Eintrag voraus —
# kein Flinch ohne Schwelle; (d) EXIT_AT erreicht. Haengt am Runde-38-F-Ablauf von ROOM1141 (Wegplan bis zum Gemaelde);
# aendert eine spaetere Spur die Szene, ist der Wegplan bei der Integration nachzuziehen.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r39_gegner_1141.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.16)
set(_tag "r39_gegner_1141")
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "${_tag}: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "${_tag}: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_dir "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_dir}")

# Eigene Messkopie neben der exe (Asset-Wurzel = exe-Ordner), Praefix NUR dieses Tests.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r39e_1141_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r39e_1141_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# Keine Umgebung des Aufrufers erben.
foreach(_v RE15_AI_FLAVOR RE15_R39_GEGNER_LOG RE15_R39_MESS_SPIELER_HP RE15_FRAMEDUMP RE15_INPUT_SCRIPT RE15_INPUT_SCRIPT_START
           RE15_INPUT_SCRIPT_BASIS RE15_PLAYER_POS RE15_FORCE_EVENT RE15_WEGPLAN RE15_GIVE RE15_EQUIP RE15_SET_FLAG
           RE15_DEBUG_JUMP RE15_NOAUDIO RE15_SOFTWARE_RENDER RE15_R38F_GORILLAS_TOT RE15_R38F_GORILLAS_STARR RE15_EXIT_AT)
    unset(ENV{${_v}})
endforeach()

# Wegplan = integration_r38_dining1141_d bis zum Gemaelde (1131: Schluessel + Tuer; 1141: Phase H, Gemaelde "ja").
# Runde 39 Spur J (bei der Integration nachgezogen, Merge in r39/dining1141 Nachbesserung 2, wie oben vorgesehen: "aendert eine
# spaetere Spur die Szene, ist der Wegplan bei der Integration nachzuziehen"): seit J Nachbesserung 1 (Nutzer-Nachtrag 4) oeffnet
# das Gemaelde nur den Rolladen; die Szene mit dem Gorilla-Einbruch startet erst am Riegel der Dachtuer (Ereignis 5, main00
# @0x00A9A -> sub05 @0x00FC8). Darum nach dem Gemaelde wie integration_r39_dining1141_d durch die Rolladen-Oeffnung zum Riegel
# (Vorwaerts-Punkt (-7250,-1000), Blick +Z), druecken und msg 12 schliessen. Seit Nutzer-Nachtrag 10 ohne Elzas Reaktion in 1141:
# die Szene endet bei F3438 (Messlauf scratchpad j39n2/e_mess2: pm 0 ab F3438, Elza wieder bei (-7998,-10284); mit Reaktion
# e_mess F3655, vorher ueber das Gemaelde ~F2700) - Schuetze 65 Bilder nach dem Szenenende wie zuvor, gleiche Kampfdauer
# 1740 Bilder (START 2760 -> 3500, EXIT 4500 -> 5240).
file(WRITE "${_dir}/wegplan.txt" "RAUM 1131
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
# Schuetze: 10 s warten (die Gorillas kommen heran), dann Bloecke "R1 los, 0,6 s zielen, 6 x (Quadrat 0,1 s, 0,3 s)"
# (dieselbe Zeichenkette wie im Messlauf n1141_m; nach EXIT_AT ungenutzt).
set(_block "W0.1,M0.6")
foreach(_i RANGE 1 6)
    string(APPEND _block ",MA0.1,M0.3")
endforeach()
set(_skript "W10")
foreach(_i RANGE 1 80)
    string(APPEND _skript ",${_block}")
endforeach()

set(WORKDIR "${_dir}")
set(_env RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
         RE15_MSG_LOG=1 RE15_STATE_LOG=state.txt RE15_AUDIO_CAP_SYNC=cap.raw RE15_R39_GEGNER_LOG=1
         "RE15_WEGPLAN=${_dir}/wegplan.txt" RE15_WEGPLAN_LOG=wegplan_log.txt
         RE15_SET_FLAG=3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78 RE15_GIVE=0x43:1,0x04:99,0x15:60 RE15_EQUIP=4
         RE15_DEBUG_JUMP=1131@gp RE15_PLAYER_POS=-1300,-13900,0 RE15_R39_MESS_SPIELER_HP=100
         "RE15_INPUT_SCRIPT=${_skript}" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=3500
         "RE15_EXIT_AT=5240")
re15_start_spiel(_rv 900 ${_env} "${_exe_kopie}")
file(REMOVE "${_dir}/cap.raw")
file(REMOVE "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    message(FATAL_ERROR "${_tag}: re15_pc exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
endif()
foreach(_d debug.log state.txt gegner_r39.log)
    if(NOT EXISTS "${_dir}/${_d}")
        message(FATAL_ERROR "${_tag}: kein ${_d} in ${_dir}")
    endif()
endforeach()
file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
if(NOT _ex)
    message(FATAL_ERROR "${_tag}: RE15_EXIT_AT nicht erreicht — Haenger? ${_dir}")
endif()

set(_fehler "")
# ---- (a) Messhaken + INIT ----
file(STRINGS "${_dir}/gegner_r39.log" _mh REGEX "^messhaken RE15_R39_MESS_SPIELER_HP=100 aktiv")
if(NOT _mh)
    string(APPEND _fehler " (a)Messhaken-nicht-aktiv")
endif()
file(STRINGS "${_dir}/gegner_r39.log" _init360 REGEX "^affen init slot=[0-9]+ raum=1141 typ=27 hp=360 .*regel 1\\)")
file(STRINGS "${_dir}/gegner_r39.log" _init180 REGEX "^affen init slot=[0-9]+ raum=1141 typ=27 hp=180 ")
list(LENGTH _init360 _n360)
if(_n360 LESS 2 OR _init180)
    string(APPEND _fehler " (a)INIT:${_n360}x360/180=[${_init180}]")
endif()
# ---- (b) Schwelle je Treffer-Zeile, Muster je Gorilla ----
file(STRINGS "${_dir}/gegner_r39.log" _tr REGEX "^affen treffer F[0-9]+ slot=[0-9]+ raum=1141 ")
list(LENGTH _tr _ntr)
set(_nfl 0)
set(_noz 0)
set(_slots "")
foreach(_l IN LISTS _tr)
    if(NOT _l MATCHES "slot=([0-9]+) raum=1141 hp=(-?[0-9]+) akku=(-?[0-9]+) -> (FLINCH|OHNE ZUCKEN)")
        string(APPEND _fehler " (b)Zeile?[${_l}]")
        continue()
    endif()
    set(_s ${CMAKE_MATCH_1})
    set(_a ${CMAKE_MATCH_3})
    set(_v "${CMAKE_MATCH_4}")
    if(_v STREQUAL "FLINCH")
        math(EXPR _nfl "${_nfl} + 1")
        if(_a LESS 36)
            string(APPEND _fehler " (b)FLINCH-bei-akku${_a}")
        endif()
        if(_a EQUAL 36)
            list(APPEND _slots "${_s}F36")
        endif()
    else()
        math(EXPR _noz "${_noz} + 1")
        if(NOT _a LESS 36)
            string(APPEND _fehler " (b)OHNE-bei-akku${_a}")
        endif()
        list(APPEND _slots "${_s}O${_a}")
    endif()
endforeach()
# Muster je Gorilla-Platz; verlangt fuer mindestens EINEN Platz (welcher Gorilla zuerst dreimal getroffen wird, haengt am
# Kampfverlauf; im Messlauf n1141_m zeigen es beide). Die Regel selbst prueft jede Zeile oben.
set(_muster_plaetze "")
foreach(_s 2 3)
    set(_voll 1)
    foreach(_m O12 O24 F36)
        list(FIND _slots "${_s}${_m}" _p)
        if(_p LESS 0)
            set(_voll 0)
        endif()
    endforeach()
    if(_voll)
        list(APPEND _muster_plaetze ${_s})
    endif()
endforeach()
if(NOT _muster_plaetze)
    string(APPEND _fehler " (b)kein-Gorilla-mit-OHNE12/OHNE24/FLINCH36")
endif()
# ---- (c) state.txt: jeder Flinch-Beginn (Zustand 2 + Clip 7, Bild davor nicht) hat einen FLINCH-Eintrag ----
file(STRINGS "${_dir}/state.txt" _zl)
set(_fl_state 0)
foreach(_s 2 3)
    set(_alt${_s} 0)
endforeach()
foreach(_z IN LISTS _zl)
    if(NOT _z MATCHES "\\[1 t=49 ")
        continue()          # nur ROOM1141 (John ist im Raum)
    endif()
    foreach(_s 2 3)
        set(_jetzt 0)
        if(_z MATCHES "\\[${_s} t=27 st=2 [^@]* mo=7 ")
            set(_jetzt 1)
        endif()
        if(_jetzt AND NOT _alt${_s})
            math(EXPR _fl_state "${_fl_state} + 1")
        endif()
        set(_alt${_s} ${_jetzt})
    endforeach()
endforeach()
if(NOT _fl_state EQUAL _nfl)
    string(APPEND _fehler " (c)Flinch-Beginne-im-state.txt=${_fl_state}/FLINCH-Eintraege=${_nfl}")
endif()
if(_fehler)
    message(FATAL_ERROR "${_tag}:${_fehler} (Treffer ${_ntr}: FLINCH ${_nfl} / OHNE ${_noz}; ${_dir})")
endif()
message(STATUS "${_tag}: OK — INIT 360 x ${_n360}, ${_ntr} Treffer-Zeilen (FLINCH ${_nfl} nur bei akku >= 36, OHNE ZUCKEN ${_noz}), Flinch-Beginne im state.txt ${_fl_state}; ${_dir}")
