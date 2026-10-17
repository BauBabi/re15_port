# =============================================================================
# RIEGEL ELZA ROOM1171 MIT DER ECHTEN EXE (Runde 38 Spur I, Nutzer: "Elza stuck in ihrer Cutscene im ROOM 1170").
#
# Dossier analysis/befunde_runde38/I_heliport1171.md, Konstanten include/re15_heliport1171_r38.h.
# Die Unit-Riegel unit_r38_heliport1171_* messen die Mechanik Bild fuer Bild; dieser Riegel prueft den ECHTEN Weg
# mit Elza in der exe (Lade-Weg + Tueren), in beiden KI-Modi:
#   weg_re2   Spielstand ROOM1131 (Elza, (3,0xC2)=1 wie nach ROOM1141 sub02 @0x00C20) vor der Flurtuer Slot 3
#             (@0x008EE, -> ROOM1171 Cut 8), Aktionstaste -> ROOM1171; die Selbst-Tuer Slot 5 (@0x012F0, Cut 0,
#             = der Weg vom Dach-Treppenhaus auf das Helipad) per RE15_FIRE_AOT -> Wiedereintritt Szenario 0 ->
#             sub07 @0x016C4 -> sub02. Standard-KI (RE2).
#   weg_re15  derselbe Weg mit RE15_AI_FLAVOR=re15.
#   laden     Spielstand direkt in ROOM1171 (Szenario 0 beim Laden) — der Lade-Weg.
# Erwartet: Griff (gr=1, HP bleibt 100), Opfer-Clips 0 -> 1 -> 2 der Kraehenbank, danach Modus 6 (gr=0, mo=1),
# Texte msg 0/1/3, `Aot_on 01` @0x0158E -> Tuer Slot 1 -> ROOM10B1; die Ereignis-Kraehen laden die RE1.5-Bank
# (kein "RE2 EM021 loaded", Muster 0x26 nach Herkunft).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_elza_karte> -DWORKDIR=<dir> -DTEIL=<teil>
#               -P test_r38_heliport1171.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.16)   # Policies NEW (CMP0012/CMP0054/CMP0007) auch unter dem Container-CMake 3.28 im Skriptmodus wie auf dem Host (Memory reai-v2-linux-test-portabilitaet; Linux-Release v0.8.26)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r38_heliport1171: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r38_heliport1171: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR OR NOT TEIL)
    message(FATAL_ERROR "r38_heliport1171: WORKDIR/TEIL fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
# Eigener exe-Name neben der exe (Muster test_r34n_b_cursor.cmake: local_build.sh-Kills treffen nur re15_pc.exe).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r38i_${TEIL}_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r38i_${TEIL}_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(WORKDIR "${WORKDIR}/${TEIL}_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")

set(_flavor "")
set(_weg "")
if(TEIL STREQUAL "weg_re2" OR TEIL STREQUAL "weg_re15")
    set(_karte 1131 2700 15300 1024 3:194)
    set(_weg RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=30 RE15_INPUT_SCRIPT=A0.5
             "RE15_FIRE_AOT=5@60#1171")
elseif(TEIL STREQUAL "laden")
    set(_karte 1171 2300 14365 1024 3:194)
else()
    message(FATAL_ERROR "r38_heliport1171: unbekannter TEIL '${TEIL}'")
endif()
if(TEIL STREQUAL "weg_re15")
    set(_flavor RE15_AI_FLAVOR=re15)
endif()

execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${_karte}
                WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
    message(FATAL_ERROR "r38_heliport1171[${TEIL}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
endif()

re15_start_spiel(_rv 540
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
    RE15_EVT_TRACE=1 RE15_MSG_LOG=1 RE15_STATE_LOG=state.log
    "RE15_EXIT_AT=60#10b1"
    ${_weg} ${_flavor}
    "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")
if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.log")
    message(FATAL_ERROR "r38_heliport1171[${TEIL}]: kein debug.log/state.log (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)

function(_muss _text _was _warum)
    string(FIND "${_text}" "${_was}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r38_heliport1171[${TEIL}]: fehlt '${_was}' - ${_warum} (exit=${_rv}, ${WORKDIR})")
    endif()
endfunction()

if(_weg)
    _muss("${_dbg}" "DOOR FIRE slot=3" "Flurtuer ROOM1131 Slot 3 (@0x008EE) nicht benutzt")
    _muss("${_dbg}" "PC loaded room1171.rdt" "ROOM1171 nicht geladen")
    _muss("${_dbg}" "DOOR FIRE slot=5" "Selbst-Tuer Slot 5 (@0x012F0) nicht gefeuert")
endif()
_muss("${_dbg}" "room=1171 Evt_exec sub=2" "sub02 (Evt_exec @0x01704) nicht gestartet")
# Texte in der Reihenfolge des Skripts: msg 0 @0x014BA, msg 1 @0x0151E, msg 3 @0x01586
string(FIND "${_dbg}" "[msg] room=1171 id=0 " _m0)
string(FIND "${_dbg}" "[msg] room=1171 id=1 " _m1)
string(FIND "${_dbg}" "[msg] room=1171 id=3 " _m3)
if(_m0 LESS 0 OR _m1 LESS _m0 OR _m3 LESS _m1)
    message(FATAL_ERROR "r38_heliport1171[${TEIL}]: Texte 0/1/3 nicht in Folge (${_m0}/${_m1}/${_m3}) - die Szene "
        "haengt (sub02 @0x013F4 wartet auf (5,0x1C)?) (${WORKDIR})")
endif()
_muss("${_dbg}" "DOOR FIRE slot=1" "Aot_on 01 @0x0158E hat die Tuer nach ROOM10B1 nicht gefeuert")
_muss("${_dbg}" "PC loaded room10b1.rdt" "Abstieg nach ROOM10B1 fehlt")
_muss("${_dbg}" "EXIT_AT: Bild 60 in Raum 10b1" "Lauf vor dem Endbild abgerissen")
string(FIND "${_dbg}" "RE2 EM021 loaded" _re2bank)
if(NOT _re2bank LESS 0)
    message(FATAL_ERROR "r38_heliport1171[${TEIL}]: die Ereignis-Kraehen haben die RE2-Bank geladen (main.c-Weiche "
        "nach Herkunft fehlt) (${WORKDIR})")
endif()

# state.log bis zum Raumwechsel (Bildzaehler springt auf 0): HP, Griff, Opfer-Clips, Modus 6
file(STRINGS "${WORKDIR}/state.log" _zeilen)
set(_vor -1)
set(_hp_min 1000)
set(_griff 0)
set(_wurf 0)
set(_modus6 0)
set(_in1171 0)
set(_frei10b1 0)
set(_nach 0)
foreach(_z IN LISTS _zeilen)
    if(_nach)                      # ROOM10B1: Elza wieder steuerbar (pm=0)?
        if(_z MATCHES " pm=0 ")
            set(_frei10b1 1)
        endif()
        continue()
    endif()
    if(NOT _z MATCHES "^F([0-9]+) .*PL\\(-?[0-9]+,-?[0-9]+,rot=-?[0-9]+,hp=(-?[0-9]+)\\) .* mo=([0-9]+) .* gr=([0-9])")
        continue()
    endif()
    set(_f ${CMAKE_MATCH_1})
    set(_hp ${CMAKE_MATCH_2})
    set(_mo ${CMAKE_MATCH_3})
    set(_gr ${CMAKE_MATCH_4})
    if(_z MATCHES " t=21 st=4 " )
        set(_in1171 1)
    endif()
    if(NOT _in1171)
        continue()
    endif()
    if(_vor GREATER _f)            # Raumwechsel nach ROOM10B1
        set(_nach 1)
        if(_z MATCHES " pm=0 ")
            set(_frei10b1 1)
        endif()
        continue()
    endif()
    set(_vor ${_f})
    if(_hp LESS _hp_min)
        set(_hp_min ${_hp})
    endif()
    if(_gr EQUAL 1 AND _mo EQUAL 0)
        set(_griff 1)
    endif()
    if(_griff AND _gr EQUAL 1 AND _mo EQUAL 2)
        set(_wurf 1)
    endif()
    if(_wurf AND _gr EQUAL 0 AND _mo EQUAL 1)
        set(_modus6 1)
    endif()
endforeach()
if(NOT _in1171)
    message(FATAL_ERROR "r38_heliport1171[${TEIL}]: keine Ereignis-Kraehe (t=21 st=4) im state.log (${WORKDIR})")
endif()
if(NOT _hp_min EQUAL 100)
    message(FATAL_ERROR "r38_heliport1171[${TEIL}]: Elza verliert HP in der Szene (min ${_hp_min}) - der Rueckgriff "
        "@0x80115248-c8 schreibt keine HP")
endif()
if(NOT (_griff AND _wurf AND _modus6))
    message(FATAL_ERROR "r38_heliport1171[${TEIL}]: Opfer-Folge unvollstaendig (Griff ${_griff}, Wurf-Clip ${_wurf}, "
        "Modus 6 ${_modus6}) (${WORKDIR})")
endif()
if(NOT _frei10b1)
    message(FATAL_ERROR "r38_heliport1171[${TEIL}]: nach dem Abstieg ist Elza in ROOM10B1 nicht steuerbar (kein pm=0) "
        "(${WORKDIR})")
endif()
message(STATUS "r38_heliport1171[${TEIL}]: OK (Szene bis ROOM10B1, HP 100, Griff/Wurf/Modus 6, danach frei) ${WORKDIR}")
