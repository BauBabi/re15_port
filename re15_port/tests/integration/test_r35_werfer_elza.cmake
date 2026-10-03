# Runde 35 Spur B, Nachbesserung 1 (Anmerkung A2 / M6 der Abnahme 0) — Elza (PL04) mit dem Granatwerfer an der ECHTEN exe.
#
# Dossier analysis/befunde_runde35/B_werfer.md §8.6. Registrierung: tests/unit/probes/r35_werfer.cmake.
# Start-Rezept der Abnahme 0 §2.3: RE15_TITLE_CONFIRM_MS=1500 + RE15_PSELECT_AUTO=1 + RE15_PSELECT_AUTO_SWITCH=1 (die
# Spielerwahl schaltet auf Elza), RE15_DEBUG_JUMP=1000@250, Leon-Position der Freie-Bahn-Laeufe (21850,-13400) Blick -x,
# RE15_GIVE=15:6 / RE15_EQUIP=15, Abzug "M0.6,MA1.5,M1.0,W4". Gemessen wird:
#   * debug.log "[pl] Spieler-Familie PL04" (Elza) und "[equip] W-bank -> W0F (Clips 14" (Elzas Standard-Bank; Leons
#     PL00W0F hat 11 Clips und den Waffenrahmen, Elza nicht — unit_r35_werfer 95);
#   * wf.log: 5 x "RE2SPAWN a0=020c0a00 ... ofs=(120,1200,0)" (RE2-Versatz @0x80044c18-40 ohne Rahmen) und mindestens
#     eine Explosion "re2fx code=0x01110001" (Op 47 Sub 12).
cmake_minimum_required(VERSION 3.16)
set(_tag "r35_werfer_elza")
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
set(WORKDIR "${_dir}")
set(_env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
         RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=1000@250 "RE15_PLAYER_POS=21850,-13400,2048" RE15_AI_FLAVOR=re2
         "RE15_GIVE=15:6" "RE15_EQUIP=15" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1
         "RE15_INPUT_SCRIPT=M0.6,MA1.5,M1.0,W4" RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log RE15_WPN_DBG=1
         "RE15_EXIT_AT=150#1000")
re15_start_spiel(_rv 240 ${_env} "${RE15_PC_EXE}")
if("${_rv}" STREQUAL "1" OR "${_rv}" STREQUAL "-1" OR NOT EXISTS "${_dir}/wf.log")
    message(STATUS "${_tag}: Lauf ohne Ergebnis (exit=${_rv}) -> EIN Wiederholungsversuch")
    file(REMOVE "${_dir}/debug.log" "${_dir}/state.log" "${_dir}/wf.log")
    re15_start_spiel(_rv 240 ${_env} "${RE15_PC_EXE}")
endif()
if(NOT _rv EQUAL 0)
    message(FATAL_ERROR "${_tag}: re15_pc.exe exit=${_rv}, ${_dir}")
endif()
file(STRINGS "${_dir}/debug.log" _pl REGEX "\\[pl\\] Spieler-Familie PL04")
file(STRINGS "${_dir}/debug.log" _bk REGEX "\\[equip\\] W-bank -> W0F \\(Clips 14")
file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
file(STRINGS "${_dir}/wf.log" _sp REGEX "RE2SPAWN a0=020c0a00 .*ofs=\\(120,1200,0\\)")
file(STRINGS "${_dir}/wf.log" _xp REGEX "re2fx code=0x01110001")
list(LENGTH _sp _nsp)
list(LENGTH _xp _nxp)
if(NOT _pl)
    message(FATAL_ERROR "${_tag}: kein '[pl] Spieler-Familie PL04' — Elza nicht im Spiel")
endif()
if(NOT _bk)
    message(FATAL_ERROR "${_tag}: keine Bank W0F mit 14 Clips")
endif()
if(NOT _ex)
    message(FATAL_ERROR "${_tag}: RE15_EXIT_AT nicht erreicht — Haenger?")
endif()
if(_nsp LESS 5 OR _nxp LESS 1)
    message(FATAL_ERROR "${_tag}: RE2SPAWN 020c0a00 ofs=(120,1200,0) ${_nsp} (>= 5), Explosionen ${_nxp} (>= 1)")
endif()
message(STATUS "${_tag}: OK — Elza PL04, W0F 14 Clips, ${_nsp} Runden mit ofs=(120,1200,0), ${_nxp} Explosionen")
