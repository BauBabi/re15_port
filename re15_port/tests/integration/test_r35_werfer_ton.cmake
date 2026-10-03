# Runde 35 Spur B, Nachbesserung 1 (Mangel M2 der Abnahme 0) — Leerschuss des Raketenwerfers AM MISCHER.
#
# Dossier analysis/befunde_runde35/B_werfer.md §8.2. Registrierung: tests/unit/probes/r35_werfer.cmake.
# Zwei Laeufe der echten exe, ROOM1000 ohne Gegner, Leon (21850,-13400) Blick -x, RE15_GIVE=18:1, RE15_EQUIP=18,
# RE15_AUDIO_CAP_SYNC (audio_pc.c: je Spielbild RE15_AUDIO_RATE/30 = 1470 Stereo-Abtastungen s16 = 5880 Byte
# durch dieselbe Mischkette, deterministisch):
#   MIT   "M0.6,MA0.3,M1.6,MA0.2,M2.0,W1"  — erster Abzug feuert die einzige Rakete, zweiter Abzug (Bild ~76) leer
#   OHNE  "M0.6,MA0.3,M1.6,M0.2,M2.0,W1"   — derselbe Ablauf ohne den zweiten Abzug
# Gemessen wird:
#   * wf.log MIT: genau eine Zeile "re2arms ARMS11 satz=1" (RE2-Leerzweig @0x80043868/94-9c), OHNE: keine;
#   * die Mitschnitte sind bis zu einem Bild D bytegleich und weichen ab D in >= 2 Bildern ab — der Leerschuss
#     ist HOERBAR. Vor der Nachbesserung (RE1.5-Klick ARMS12 Satz 1 = ff ff ff ff) waren beide Mitschnitte
#     bytegleich (Abnahme 0 §2.5: "Raketenwerfer ohne jede Abweichung").
# Ein Lauf ohne Ergebnis (exit 1/-1 = von aussen beendet) wird EINMAL wiederholt.

cmake_minimum_required(VERSION 3.16)
set(_tag "r35_werfer_ton")
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "${_tag}: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "${_tag}: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_wurzel}")

function(r35t_lauf _name _skript)
    set(_dir "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${_dir}")
    set(WORKDIR "${_dir}")
    set(_env RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_AUDIO_CAP_SYNC=cap.raw
             RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=1000@250 "RE15_PLAYER_POS=21850,-13400,2048"
             RE15_AI_FLAVOR=re2 "RE15_GIVE=18:1" "RE15_EQUIP=18"
             RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_skript}"
             RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log RE15_WPN_DBG=1 "RE15_EXIT_AT=150#1000")
    re15_start_spiel(_rv 240 ${_env} "${RE15_PC_EXE}")
    if("${_rv}" STREQUAL "1" OR "${_rv}" STREQUAL "-1" OR NOT EXISTS "${_dir}/wf.log")
        message(STATUS "${_tag} [${_name}]: Lauf ohne Ergebnis (exit=${_rv}) -> EIN Wiederholungsversuch")
        file(REMOVE "${_dir}/debug.log" "${_dir}/state.log" "${_dir}/wf.log" "${_dir}/cap.raw")
        re15_start_spiel(_rv 240 ${_env} "${RE15_PC_EXE}")
    endif()
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "${_tag} [${_name}]: re15_pc.exe exit=${_rv}, ${_dir}")
    endif()
    foreach(_d debug.log wf.log cap.raw)
        if(NOT EXISTS "${_dir}/${_d}")
            message(FATAL_ERROR "${_tag} [${_name}]: kein ${_d} in ${_dir}")
        endif()
    endforeach()
    file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
    if(NOT _ex)
        message(FATAL_ERROR "${_tag} [${_name}]: RE15_EXIT_AT nicht erreicht — Haenger?")
    endif()
endfunction()

r35t_lauf(mit  "M0.6,MA0.3,M1.6,MA0.2,M2.0,W1")
r35t_lauf(ohne "M0.6,MA0.3,M1.6,M0.2,M2.0,W1")

# 1) Log: der Leerzweig ruft im MIT-Lauf genau einmal RE2 ARMS11 Satz 1, im OHNE-Lauf nie.
file(STRINGS "${_wurzel}/mit/wf.log"  _m REGEX "re2arms ARMS11 satz=1$")
file(STRINGS "${_wurzel}/ohne/wf.log" _o REGEX "re2arms ARMS11 satz=1$")
list(LENGTH _m _nm)
list(LENGTH _o _no)
if(NOT _nm EQUAL 1 OR NOT _no EQUAL 0)
    message(FATAL_ERROR "${_tag}: 're2arms ARMS11 satz=1' MIT=${_nm} (erwartet 1), OHNE=${_no} (erwartet 0)")
endif()
file(STRINGS "${_wurzel}/mit/wf.log" _schuss REGEX "re2fx code=0x01140001")
if(NOT _schuss)
    message(FATAL_ERROR "${_tag}: der erste Abzug hat keine Rakete gezuendet (kein 0x01140001) — Ablauf stimmt nicht")
endif()

# 2) Mischer: erstes abweichendes Bild D, davor bytegleich, ab D mindestens zwei abweichende Bilder.
set(_fb 5880)
file(SIZE "${_wurzel}/mit/cap.raw"  _sm)
file(SIZE "${_wurzel}/ohne/cap.raw" _so)
if(_sm LESS _so)
    set(_s ${_sm})
else()
    set(_s ${_so})
endif()
math(EXPR _nf "${_s} / ${_fb}")
if(_nf LESS 100)
    message(FATAL_ERROR "${_tag}: Mitschnitt zu kurz (${_nf} Bilder)")
endif()
set(_d -1)
set(_nabw 0)
math(EXPR _letzt "${_nf} - 1")
foreach(_f RANGE 0 ${_letzt})
    math(EXPR _ofs "${_f} * ${_fb}")
    file(READ "${_wurzel}/mit/cap.raw"  _a OFFSET ${_ofs} LIMIT ${_fb} HEX)
    file(READ "${_wurzel}/ohne/cap.raw" _b OFFSET ${_ofs} LIMIT ${_fb} HEX)
    if(NOT _a STREQUAL _b)
        if(_d EQUAL -1)
            set(_d ${_f})
        endif()
        math(EXPR _nabw "${_nabw} + 1")
    endif()
endforeach()
if(_d EQUAL -1)
    message(FATAL_ERROR "${_tag}: Mitschnitte MIT/OHNE bytegleich ueber ${_nf} Bilder — der Leerschuss ist STUMM (Mangel M2)")
endif()
if(_nabw LESS 2)
    message(FATAL_ERROR "${_tag}: nur ${_nabw} abweichendes Bild ab ${_d} — zu kurz fuer das Sample ARMS11 Satz 1")
endif()
message(STATUS "${_tag}: OK — Leerschuss hoerbar: erstes abweichendes Bild ${_d} von ${_nf}, ${_nabw} abweichende Bilder; wf.log ARMS11 satz=1 MIT=1 OHNE=0")
