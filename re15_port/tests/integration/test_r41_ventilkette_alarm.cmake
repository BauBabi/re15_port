# =============================================================================
# RE2-ALARM SUB_25 IN 2030/2031 MIT DER ECHTEN EXE (Runde 41 Spur C, Nachbesserung 6 — Abnahme 5 Mangel 1; Nachtrag 18 Punkt 4
# "Alarm, so wie am Spiel Ende immer", Nachtrag 25 (o) "Ja, RE2-Alarm SUB_25", Orchestrator-Entscheid C-E4).
#
# Form (include/re15_ventilkette_r41.h "Alarm", Dossier analysis/befunde_runde41/C_ventilkette.md §0g B6-2): RE2 ROOM6040 @0x022E8
# `57 00 04 06 11 25 00 80` -> Raum-Eintrag Sub-Byte 0xA5 (FUN_8005b9f4; Flags FUN_8005a444: SEQ0 spielt, SEQ#2 stumm), Lautstaerke
# ROOM6040 sub23 @0x038BA `51 01 00 01 64 40` (FUN_8005b6f0: ProgAtr[0].mvol 99). Im Port: Warntext 46 -> re15_audio_r41_alarm(1)
# (audio_pc.c, Haken "Runde 41 Spur C, C-E4") laedt shared_assets/RE2/SOUND/SUB_25.BGM in den SUB-Platz; Flut/Tod -> aus.
#
# Gemessen mit RE15_AUDIO_CAP_SYNC (bildgenaue Aufnahme ohne SDL-Geraet, 1470 Stereo-Bilder je Spieltakt):
#   A  Leon, 2030: richtige Folge 09/07/02/01 zu Fuss -> Warntext 46 -> Ring -> Flut (Plan wie integration_r41_ventilkette C).
#   B  derselbe Lauf mit RE15_RE2_ASSET_ROOT -> SOUND/SUB_25.BGM = der 4-Byte-Stummel der RE1.5-CD (shared_assets/PSX/SOUND):
#      der Alarm laedt NICHT ("nicht lesbar"), sonst bitgleich. A - B = der Alarm allein.
#      Erwartet (Messlauf nb6 al_flut_leon / al_flut_leon_ohne 2026-10-25): vor dem Alarm-Takt bitgleich (max |A-B| = 0), waehrend
#      des Alarms RMS(A-B) 667..718 (>= 300 verlangt), ab dem Alarm-Ende + 120 Takte wieder bitgleich (Hall abgeklungen; gemessen letzte Abweichung Ende + 78).
#   C  Elza, 2031, derselbe Plan: Alarm an/aus in 2031; die Aufnahme ab ihrem Alarm-Takt korreliert mit A - B (gemessen 0,571,
#      Gegenprobe 300 Takte vorher ~0; verlangt >= 0,3 bzw. <= 0,1).
#   Je Lauf: der Alarm geht MIT dem Warntext an und im Bild des Flut-Beginns aus - das ist das Bild, in dem der Countdown RE2s
#   abgelaufenes " 0^00=00" zeigt (Bild 1800, @0x80026284).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_R41C_WERKZEUG=<test_r41_ventilkette> -DRE15_SRC=<re15_port> -DWORKDIR=<dir>
#         -P test_r41_ventilkette_alarm.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
foreach(_v RE15_PC_EXE RE15_R41C_WERKZEUG RE15_SRC)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r41_ventilkette_alarm: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_ventilkette_alarm: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_basis}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41calarm_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r41calarm_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r41calarm_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# RE2-Wurzel des Zwillings: nur SOUND/SUB_25.BGM = der 4-Byte-Stummel der RE1.5-CD; alles andere faellt auf shared_assets/RE2 zurueck
# (asset_root_pc.c re15_pc_read_re2: erst RE15_RE2_ASSET_ROOT, dann <shared>/RE2).
set(_ohne "${_basis}/re2_ohne_alarm")
file(MAKE_DIRECTORY "${_ohne}/SOUND")
file(COPY_FILE "${RE15_SRC}/shared_assets/PSX/SOUND/SUB_25.BGM" "${_ohne}/SOUND/SUB_25.BGM")
file(SIZE "${_ohne}/SOUND/SUB_25.BGM" _stummel)
if(NOT _stummel LESS 64)
    message(FATAL_ERROR "r41_ventilkette_alarm: shared_assets/PSX/SOUND/SUB_25.BGM ist kein Stummel (${_stummel} B) - Zwilling ungueltig")
endif()

function(_fehler _t)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_ventilkette_alarm${_t}")
endfunction()
function(_muss _text _wort _name _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(_p LESS 0)
        _fehler("[${_name}]: ${_was} — '${_wort}' fehlt im debug.log")
    endif()
endfunction()

macro(_lauf _name _figur _plan)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(WRITE "${WORKDIR}/plan.txt" "${_plan}")
    if("${_figur}" STREQUAL "leon")
        set(_env RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
        set(_raum 2030)
    else()
        set(_env RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
        set(_raum 2031)
    endif()
    re15_start_spiel(_rv 900
        RE15_NO_INTRO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 ${_env}
        RE15_DEBUG_JUMP=2030@250
        "RE15_PLAYER_POS=-2650,7280,3072,1"
        "RE15_SET_FLAG=7:155,7:156,7:157,9:101,9:102,3:108,9:125"
        "RE15_WEGPLAN=${WORKDIR}/plan.txt" RE15_WEGPLAN_LOG=wegplan.txt
        RE15_AUDIO_CAP_SYNC=cap.raw RE15_BGM_CTL_DEBUG=1
        ${ARGN}
        "RE15_EXIT_AT=6450#${_raum}"
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/cap.raw")
        _fehler("[${_name}]: kein debug.log/cap.raw (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
endmacro()

# Plan = integration_r41_ventilkette C: die richtige Folge 09/07/02/01 zu Fuss (Sued, Sued, Steg, Nord, Nord), Warnung weg,
# ueber Steg und Treppe auf den Ring (6550,-200), dort stehen bleiben -> Flut.
function(_plan _s _out)
    set(${_out} "RAUM 203${_s}
WARTE 30
GEHE -2650 7280 100
BLICK 3072
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
GEHE 2600 7280 100
BLICK 3072
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
GEHE -1050 6900 100
GEHE -1050 6000
GEHE -1050 -6400
GEHE -1050 -7400 100
GEHE 150 -7880 100
BLICK 1024
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
GEHE -1350 -7880 100
BLICK 1024
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
AKTION
WARTE 30
GEHE -1900 -7700 80
BLICK 2048
WARTE 20
AKTION
WARTE 300
GEHE -6800 -9675
GEHE 6575 -9675
GEHE 6550 -200
BLICK 3072
ENDE
" PARENT_SCOPE)
endfunction()
_plan(0 _p0)
_plan(1 _p1)

# Alarm-Takte aus dem Log (Zeilen von ventilkette_r41_pc.c) und die Reihenfolge Warntext -> Flut -> aus.
function(_alarm_takte _text _raum _name _an _aus)
    string(REGEX MATCH "RE2-Alarm SUB_25 an \\(ROOM${_raum}, Warntext 46\\) geladen capTick ([0-9]+)" _m1 "${_text}")
    set(_t_an "${CMAKE_MATCH_1}")
    string(REGEX MATCH "RE2-Alarm SUB_25 aus \\(Flut/Tod\\) capTick ([0-9]+) F([0-9]+)" _m2 "${_text}")
    set(_t_aus "${CMAKE_MATCH_1}")
    set(_f_aus "${CMAKE_MATCH_2}")
    if(NOT _m1 OR NOT _m2)
        _fehler("[${_name}]: Alarm an/aus fehlt im Log (an '${_m1}', aus '${_m2}')")
    endif()
    string(FIND "${_text}" "RE2-Alarm SUB_25 an (ROOM${_raum}" _p_an)
    string(FIND "${_text}" "[msgtext] room=${_raum} id=46 \"WARNING: The water will rise in one minute. Please leave the room immediately!\"" _p_46)
    string(FIND "${_text}" "FLUT: Wasser steigt" _p_flut)
    string(FIND "${_text}" "RE2-Alarm SUB_25 aus (Flut/Tod)" _p_aus)
    if(_p_46 LESS 0 OR _p_flut LESS 0 OR NOT _p_an LESS _p_flut OR NOT _p_flut LESS _p_aus)
        _fehler("[${_name}]: Reihenfolge Alarm an / Warntext 46 / Flut / Alarm aus falsch (${_p_an} ${_p_46} ${_p_flut} ${_p_aus})")
    endif()
    # der Alarm geht mit dem Warntext an: hoechstens 60 Takte vor der Textzeile (Schreibmaschine) und nicht vor der richtigen Folge
    string(FIND "${_text}" "Folge 09 07 02 01 -> RICHTIG" _p_r)
    if(_p_r LESS 0 OR NOT _p_r LESS _p_an)
        _fehler("[${_name}]: Alarm vor der richtigen Folge")
    endif()
    # Alarm aus im Bild des Flut-Beginns = Bild des abgelaufenen Countdowns " 0^00=00" (RE2 @0x80026284)
    string(REGEX MATCH "Countdown RE2-Form Bild 1800 \" 0\\^00=00\" Farbe 2 \\(x 104 y 32\\) F([0-9]+)" _m3 "${_text}")
    if(NOT _m3 OR NOT CMAKE_MATCH_1 STREQUAL _f_aus)
        _fehler("[${_name}]: Alarm-Ende (F${_f_aus}) nicht im Bild des abgelaufenen Countdowns ('${_m3}')")
    endif()
    set(${_an} "${_t_an}" PARENT_SCOPE)
    set(${_aus} "${_t_aus}" PARENT_SCOPE)
endfunction()

# ---- A: Leon mit dem Alarm -------------------------------------------------------------------------------------------------
_lauf(a leon "${_p0}")
_muss("${_dbg}" "[bgm] Sce_bgm_control slot=1 op=0 (part=1 vol=100 pan=64)" a "Lautstaerke wie RE2 ROOM6040 sub23 @0x038BA `51 01 00 01 64 40`")
_alarm_takte("${_dbg}" 2030 a _a_an _a_aus)
set(_a_cap "${WORKDIR}/cap.raw")
# ---- B: Zwilling ohne SUB_25 ----------------------------------------------------------------------------------------------
_lauf(b leon "${_p0}" "RE15_RE2_ASSET_ROOT=${_ohne}")
_muss("${_dbg}" "[bgm] SUB_25.BGM nicht lesbar" b "Zwilling: SUB_25 laedt nicht")
_muss("${_dbg}" "RE2-Alarm SUB_25 an (ROOM2030, Warntext 46) NICHT geladen capTick ${_a_an}" b "Zwilling: gleicher Alarm-Takt wie A")
set(_b_cap "${WORKDIR}/cap.raw")
execute_process(COMMAND "${RE15_R41C_WERKZEUG}" capdiff "${_a_cap}" "${_b_cap}" ${_a_an} ${_a_aus} OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
string(STRIP "${_o}" _o)
message(STATUS "r41_ventilkette_alarm[ab]: Alarm an ${_a_an} aus ${_a_aus}: ${_o}")
if(NOT _r EQUAL 0 OR NOT _o MATCHES "^CAPDIFF VOR ([0-9]+) WAEHREND ([0-9]+)\\.[0-9] NACH ([0-9]+) ")
    _fehler("[ab]: capdiff: ${_o}")
endif()
set(_vor "${CMAKE_MATCH_1}")
set(_waehrend "${CMAKE_MATCH_2}")
set(_nach "${CMAKE_MATCH_3}")
if(NOT _vor EQUAL 0)
    _fehler("[ab]: vor dem Alarm nicht bitgleich (max |A-B| ${_vor}) - der Zwilling misst nicht den Alarm allein")
endif()
if(_waehrend LESS 300)
    _fehler("[ab]: waehrend des Alarms RMS(A-B) ${_waehrend} < 300 - kein hoerbarer Alarm")
endif()
if(NOT _nach EQUAL 0)
    _fehler("[ab]: nach dem Alarm-Ende (+120 Takte) nicht wieder bitgleich (max |A-B| ${_nach}) - der Alarm laeuft weiter")
endif()
# ---- C: Elza (2031) -------------------------------------------------------------------------------------------------------
_lauf(c elza "${_p1}")
_muss("${_dbg}" "Spieler-Familie PL04" c "Elza gewaehlt")
_alarm_takte("${_dbg}" 2031 c _c_an _c_aus)
execute_process(COMMAND "${RE15_R41C_WERKZEUG}" capkorr "${WORKDIR}/cap.raw" "${_a_cap}" "${_b_cap}" ${_c_an} ${_a_an} 600
                OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
string(STRIP "${_o}" _o)
message(STATUS "r41_ventilkette_alarm[c]: Elza Alarm an ${_c_an} aus ${_c_aus}: ${_o}")
if(NOT _r EQUAL 0 OR NOT _o MATCHES "^CAPKORR (-?[0-9.]+) KONTROLLE (-?[0-9.]+)")
    _fehler("[c]: capkorr: ${_o}")
endif()
set(_k "${CMAKE_MATCH_1}")
set(_k0 "${CMAKE_MATCH_2}")
if(_k LESS 0.3 OR _k0 GREATER 0.1)
    _fehler("[c]: Elzas Aufnahme traegt den Alarm nicht (Korrelation ${_k}, Gegenprobe ${_k0}; verlangt >= 0.3 / <= 0.1)")
endif()
file(REMOVE "${_exe_kopie}")
message(STATUS "r41_ventilkette_alarm: RE2-Alarm SUB_25 mit dem Warntext an, mit der Flut aus (Leon A-B: vor ${_vor}, waehrend ${_waehrend}, nach ${_nach}; Elza ${_k})")
