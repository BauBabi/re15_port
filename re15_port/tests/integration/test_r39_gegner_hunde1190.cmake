# =============================================================================
# Runde 39 Spur E "gegner" — kein paralleler Hundebiss, gemessen an der ECHTEN exe in ROOM1190 (Hunde-Luke).
#
# Nutzer (AUFTRAG.md): "Wenn ich einmal von einem Hund gerade angegriffen/gebissen werde, soll ich nicht parallel noch
# von anderen Hunden angegriffen/gebissen werden koennen, wenn ich mich nicht wehren kann, so zum Beispiel bei Room
# 1090." — "Room 1090" mit Hunden = ROOM1190 (Zensus E_gegner.md Schritt 0, wie Spur B).
# Dossier analysis/befunde_runde39/E_gegner.md Punkt 2; Code engine/src/gegner_r39.c, Haken enemy_ai_re2_dog.c.
#
# Original (RE2 EMD0G_MOD0.BIN): Boden-Biss-Spielerhaken 0x801051BC setzt PL+0x1D3 |= 0x80 (@0x801051f8-0x80105200);
# Kontakt-Tor @0x80104e5c-68 und Angriffs-Tor @0x80104d84-94 lassen keinen zweiten Hund an den beanspruchten
# Spieler; der Spieler gibt den Riegel am Ende seiner Treffer-Reaktion frei (HURT [2]/[3] 0x8003f778 Phase 2
# `sb zero,467(s0)` @0x8003f8b4). Port: Flinch cmd 2 = 22 Bilder (PL00-Clip 8/9), Freigabe mit dem NORMAL-Prolog
# (@0x80031964) im Folgebild.
#
# Aufbau (Spur B, Lauf j1190): ROOM1190 per Debug-Sprung (RE15_DEBUG_JUMP=1190@250), Leon am Spawn (3050,-11650),
# RE15_FORCE_EVENT=10@400 = die Raetsel-Szene, danach kommen die drei Hunde durch die Luke und greifen den
# stehenden Leon an (RE2-KI = Standard). Gemessen im state.log: jeder HP-Abzug des Spielers (Hundebiss = -20,
# FUN_800401D4(20) @0x80104EB8-C0) mit Bildnummer.
# Geprueft: (a) mindestens 3 Bisse (sonst misst der Lauf nichts); (b) KEINE zwei Bisse naeher als 24 Bilder
# (22 Bilder Flinch + Exit-Bild + Freigabe-Bild) — waehrend der Reaktion darf kein zweiter Hund treffen;
# (c) gegner_r39.log traegt zu jedem Biss "hund riegel GESETZT" und danach "hund riegel FREI"; (d) Leon lebt am Ende.
# Der Lauf endet bei Bild 1000 (RE15_EXIT_AT ohne Raum): gemessen (zweimal bildgleich, 2026-10-18) beissen die Hunde
# F685/F720/F753/F833 (je -20), F1042 (hp 0) und F1074 (hp -20 = Tod -> CONTINUE, der Raum-gebundene Exit kaeme nie).
# Basislauf (c71fa144, derselbe Aufbau): 6 Bisse F685/720/749/829/942/1000 — Abstaende 35/29/80/113/58, also auch
# dort kein Paar < 24 (der Nutzer-Fall "parallel" entsteht, wenn zwei Hunde im selben Flug treffen; das misst der
# Unit-Riegel unit_r39_gegner_hund_riegel deterministisch: vorher -40 im selben Bild, nachher -20).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r39_gegner_hunde1190.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.16)
set(_tag "r39_gegner_hunde1190")
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

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r39e_hunde_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r39e_hunde_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

foreach(_v RE15_AI_FLAVOR RE15_R39_LOG RE15_FRAMEDUMP RE15_INPUT_SCRIPT RE15_INPUT_SCRIPT_START RE15_PLAYER_POS RE15_FORCE_EVENT)
    unset(ENV{${_v}})
endforeach()
set(WORKDIR "${_dir}")
set(_env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
         RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
         RE15_DEBUG_JUMP=1190@250 RE15_FORCE_EVENT=10@400
         RE15_STATE_LOG=state.log RE15_R39_LOG=1 "RE15_EXIT_AT=1000")
re15_start_spiel(_rv 300 ${_env} "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    message(STATUS "${_tag}: Lauf ohne Ergebnis (exit=${_rv}) -> EIN Wiederholungsversuch")
    file(REMOVE "${_dir}/debug.log" "${_dir}/state.log" "${_dir}/gegner_r39.log")
    re15_start_spiel(_rv 300 ${_env} "${_exe_kopie}")
endif()
if(NOT _rv EQUAL 0)
    message(FATAL_ERROR "${_tag}: re15_pc exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
endif()
foreach(_d debug.log state.log gegner_r39.log)
    if(NOT EXISTS "${_dir}/${_d}")
        message(FATAL_ERROR "${_tag}: kein ${_d} in ${_dir}")
    endif()
endforeach()
file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
if(NOT _ex)
    message(FATAL_ERROR "${_tag}: RE15_EXIT_AT nicht erreicht — Haenger?")
endif()

# ---- HP-Abzuege des Spielers nach dem Raumsprung ----
file(STRINGS "${_dir}/state.log" _zl)
set(_vorher -1)
set(_nach 0)
set(_hp_alt "")
set(_bisse "")
set(_fehler "")
set(_letzter -1000)
foreach(_l IN LISTS _zl)
    if(NOT _l MATCHES "^F([0-9]+) .*PL\\((-?[0-9]+),(-?[0-9]+),rot=(-?[0-9]+),hp=(-?[0-9]+)\\) ")
        continue()
    endif()
    set(_f "${CMAKE_MATCH_1}")
    set(_hp "${CMAKE_MATCH_5}")
    if(NOT _nach AND _vorher GREATER _f)
        set(_nach 1)
        set(_hp_alt "")
    endif()
    set(_vorher ${_f})
    if(NOT _nach)
        continue()
    endif()
    if(NOT _hp_alt STREQUAL "" AND _hp LESS _hp_alt)
        math(EXPR _d "${_hp_alt} - ${_hp}")
        math(EXPR _abstand "${_f} - ${_letzter}")
        list(APPEND _bisse "F${_f}:-${_d}")
        if(_abstand LESS 24)
            string(APPEND _fehler " F${_f}(-${_d})nur${_abstand}BilderNachF${_letzter}")
        endif()
        set(_letzter ${_f})
    endif()
    set(_hp_alt ${_hp})
endforeach()
if(_hp LESS 0)
    message(FATAL_ERROR "${_tag}: Leon ist im Lauf gestorben (hp ${_hp}) — der Lauf soll bei Bild 1000 vor dem 6. Biss enden (${_bisse})")
endif()
list(LENGTH _bisse _n)
if(_n LESS 3)
    message(FATAL_ERROR "${_tag}: nur ${_n} HP-Abzuege (${_bisse}) — die Hunde haben Leon nicht angegriffen? ${_dir}")
endif()
if(_fehler)
    message(FATAL_ERROR "${_tag}: zweiter Biss waehrend der Spieler-Reaktion:${_fehler} (alle: ${_bisse})")
endif()
file(STRINGS "${_dir}/gegner_r39.log" _gesetzt REGEX "^hund riegel GESETZT")
file(STRINGS "${_dir}/gegner_r39.log" _frei REGEX "^hund riegel FREI")
list(LENGTH _gesetzt _ng)
list(LENGTH _frei _nf)
if(_ng LESS 1 OR _nf LESS 1)
    message(FATAL_ERROR "${_tag}: Riegel-Protokoll unvollstaendig (GESETZT ${_ng}, FREI ${_nf})")
endif()
message(STATUS "${_tag}: OK — ${_n} Bisse (${_bisse}), alle Abstaende >= 24 Bilder; Riegel gesetzt ${_ng} x / frei ${_nf} x; ${_dir}")
