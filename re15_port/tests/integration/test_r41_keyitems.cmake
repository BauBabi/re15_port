cmake_minimum_required(VERSION 3.16)
# =============================================================================
# RUNDE 41 SPUR O "keyitems": Key Items ueber das Inventar an der ECHTEN exe. Dossier analysis/befunde_runde41/O_keyitems.md.
#   TEIL kurbel        Leon, ECHTER Weg ROOM2050 Tuer Slot 1 -> ROOM2020, gehen, START, ITEM, Kurbel, USE: Inventar zu, erstes Bild
#                      an der Kurbel ist schon das Kurbeln, kein Text, keine Frage, Plc_motion Flags 0x40 (RE2 ROOM1110 @0x02FC1).
#   TEIL elza          dasselbe mit Elza in ROOM2021.
#   TEIL renn / geh    START im Rennen / Gehen: nach dem USE dieselbe Lage wie aus dem Stand: Sollwert (3650,-3018) im Auswerter
#                      (Original Stand-Anlauf o_stand1 n90). Der Bildvergleich ist Messung im Dossier, kein Teil dieses Riegels.
#   TEIL west / nord / ost  Stand mit Blick West, Nord, Ost (Gier 2048, 1024, 0): dieselbe Lage nach dem USE.
#   TEIL falsch        USE fern der Stelle: "You can't use it here.", Kurbel bleibt, Leon bleibt.
#   TEIL aktion        Aktionstaste an der Stelle: msg 0, keine Frage, Antwort Nein, Kurbel bleibt.
#   TEIL karte         ROOM10D0 Blue Keycard USE: msg 9 "You've used the Blue Keycard." ohne msg 7.
#   TEIL karte_aktion  ROOM10D0 Aktionstaste: Beschreibung endet vor "Will you operate the card reader?".
#   TEIL aufnahme      ROOM2010: "Will you take the Crank?" Ja -> Eintrag im Inventar, Name und CHECK-Text aus dem Port.
#   TEIL bombe         ROOM2040: USE Zeitbombe -> Inventar zu, ohne Text und Frage das Zuender-Feld (Set(5,0) @0x016FE, Cut_chg 14 @0x016FC).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_WERKZEUG=<test_r41_keyitems_exe> -DWORKDIR=<dir> -DTEIL=<teil> -P test_r41_keyitems.cmake
# =============================================================================
foreach(_v RE15_PC_EXE RE15_WERKZEUG WORKDIR TEIL)
    if(NOT ${_v})
        message(FATAL_ERROR "r41_keyitems: ${_v} fehlt")
    endif()
endforeach()
foreach(_v RE15_PC_EXE RE15_WERKZEUG)
    if(NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r41_keyitems: ${_v} existiert nicht: '${${_v}}'")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Keine RE15_*-Variablen des Aufrufers erben (VERTRAG §2).
execute_process(COMMAND "${CMAKE_COMMAND}" -E environment OUTPUT_VARIABLE _envdump)
string(REGEX MATCHALL "(^|\n)RE15_[A-Za-z0-9_]*=" _geerbt "${_envdump}")
foreach(_g ${_geerbt})
    string(REGEX REPLACE "^\n?(RE15_[A-Za-z0-9_]*)=$" "\\1" _n "${_g}")
    unset(ENV{${_n}})
endforeach()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_lauf "${WORKDIR}/lauf_${TEIL}_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe_kopie "${_exe_dir}/re15_pc_r41o_${TEIL}_${_lauf_id}${_exe_ext}")

set(_fehler "")
macro(pruef _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(_p LESS 0)
        list(APPEND _fehler "fehlt '${_muster}'")
        message(STATUS "FAIL fehlt '${_muster}'")
    else()
        message(STATUS "ok   '${_muster}'")
    endif()
endmacro()
macro(pruef_nicht _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(NOT _p LESS 0)
        list(APPEND _fehler "unerwartet '${_muster}'")
        message(STATUS "FAIL unerwartet '${_muster}'")
    else()
        message(STATUS "ok   nicht da '${_muster}'")
    endif()
endmacro()

set(_gemein RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
            RE15_KEYITEMS_LOG=1 RE15_STATE_LOG=state.txt)
# Leon: Platz 0 Browning, 1 Patronen, 2 Kurbel (Flag -> Eintrag): START, ITEM, runter auf Platz 2, Auswahl, USE.
# Tasten ab Bild 260 (Sprung @240): RE15_PRESS zaehlt je Raum neu und traefe sonst auch das Titelbild davor und startete damit ein Neues Spiel.
if(TEIL STREQUAL "kurbel")
    set(_env RE15_DEBUG_JUMP=2050@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 "RE15_FIRE_AOT=1@30#2050"
             "RE15_INPUT_SCRIPT=L0.36,U3.8,W2.0" RE15_INPUT_SCRIPT_START=60 RE15_INPUT_SCRIPT_BASIS=spiel
             RE15_PRESS=start@190,square@215,down@230,square@240,square@255 RE15_SCD_TRACE=1 "RE15_EXIT_AT=520#2020")
elseif(TEIL STREQUAL "elza")
    # Elza: Inventar leer, Kurbel auf Platz 0
    set(_env RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=2021@240 RE15_SET_FLAG=3:0x72 RE15_PLAYER_POS=2914,-3388,2016,1
             RE15_PRESS=start@260,square@285,square@310,square@325 RE15_SCD_TRACE=1 "RE15_EXIT_AT=580#2021")
elseif(TEIL STREQUAL "renn")
    # Gang z -3388 nach Westen (Weg von 2050, Riegel kleinkram_b): F266 x 3617, F268 x 3221 (Lauf m_renn), START F267 mit dem
    # Vorwaerts-Punkt im Rechteck x 2200..3000 (@0x009AE). Bilder >= 260: die liegen sicher im Raum (Sprung @240).
    set(_env RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_PLAYER_POS=5000,-3388,2016,1
             "RE15_INPUT_SCRIPT=XU1.0" RE15_INPUT_SCRIPT_START=260 RE15_INPUT_SCRIPT_BASIS=spiel
             RE15_PRESS=start@267,square@292,down@307,square@317,square@332 RE15_SCD_TRACE=1 "RE15_EXIT_AT=560#2020")
elseif(TEIL STREQUAL "geh")
    # Wie TEIL renn, gehend: F266 x 3282 (Lauf m_geh).
    set(_env RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_PLAYER_POS=3800,-3388,2016,1
             "RE15_INPUT_SCRIPT=U1.0" RE15_INPUT_SCRIPT_START=260 RE15_INPUT_SCRIPT_BASIS=spiel
             RE15_PRESS=start@266,square@291,down@306,square@316,square@331 RE15_SCD_TRACE=1 "RE15_EXIT_AT=560#2020")
elseif(TEIL STREQUAL "west")
    set(_env RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_PLAYER_POS=3300,-3750,2048,1
             RE15_PRESS=start@260,square@285,down@300,square@310,square@325 RE15_SCD_TRACE=1 "RE15_EXIT_AT=580#2020")
elseif(TEIL STREQUAL "nord")
    # Blick Nord: 620 voraus = (2600,-3638), im Rechteck x 2200..3000, z -4150..-3350 (@0x009AE). Bilder >= 260 wie TEIL renn.
    set(_env RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_PLAYER_POS=2600,-3018,1024,1
             RE15_PRESS=start@260,square@285,down@300,square@310,square@325 RE15_SCD_TRACE=1 "RE15_EXIT_AT=560#2020")
elseif(TEIL STREQUAL "ost")
    # Blick Ost: 620 voraus = (2320,-3388), im selben Rechteck.
    set(_env RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_PLAYER_POS=1700,-3388,0,1
             RE15_PRESS=start@260,square@285,down@300,square@310,square@325 RE15_SCD_TRACE=1 "RE15_EXIT_AT=560#2020")
elseif(TEIL STREQUAL "falsch")
    # Gang z -3388, x 5000: 620 voraus = x 4380, weit ausserhalb des Rechtecks @0x009AE ((2600,-1500) schoebe der Koerper-Schub auf die Stelle).
    set(_env RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_PLAYER_POS=5000,-3388,2016,1
             RE15_PRESS=start@260,square@285,down@300,square@310,square@325 RE15_SCD_TRACE=1 RE15_EXIT_AT=460)
elseif(TEIL STREQUAL "aktion")
    set(_env RE15_DEBUG_JUMP=2020@240 RE15_SET_FLAG=3:0x72 RE15_EQUIP=3 RE15_PLAYER_POS=2914,-3388,2016,1
             RE15_PRESS=square@260,square@330 RE15_MSG_LOG=1 RE15_SCD_TRACE=1 RE15_EXIT_AT=460)
elseif(TEIL STREQUAL "karte")
    # Leser Slot 1 @0x0103E Rechteck x 400..1200, z -6150..-5350; der Gang beginnt bei x 1278 (Koerper-Schub): Blick West, 620 voraus = x 658.
    set(_env RE15_DEBUG_JUMP=10d0@240 RE15_GIVE=0x38:1 RE15_SET_FLAG=9:52 RE15_EQUIP=3 RE15_PLAYER_POS=1278,-5750,2048,0
             RE15_PRESS=start@260,square@285,down@300,square@310,square@325 RE15_MSG_LOG=1 RE15_EXIT_AT=460)
elseif(TEIL STREQUAL "karte_aktion")
    set(_env RE15_DEBUG_JUMP=10d0@240 RE15_GIVE=0x38:1 RE15_SET_FLAG=9:52 RE15_EQUIP=3 RE15_PLAYER_POS=1278,-5750,2048,0
             RE15_PRESS=square@260,square@340,square@420 RE15_MSG_LOG=1 RE15_EXIT_AT=500)
elseif(TEIL STREQUAL "aufnahme")
    # Platz 4 sub00 @0x00880 Rechteck x 2700..3700, z -1550..-550; frei ab x 3618 (Koerper-Schub): Blick West, 620 voraus = x 2998.
    # Frage, Ja, "You've taken the Crank.", START, ITEM.
    set(_env RE15_DEBUG_JUMP=2010@240 RE15_EQUIP=3 RE15_PLAYER_POS=3618,-1050,2048,0 RE15_MSG_LOG=1
             RE15_PRESS=square@260,square@320,square@360,square@400,start@440,square@465,down@480,square@490,left@505,square@515
             RE15_EXIT_AT=630)
elseif(TEIL STREQUAL "bombe")
    # Platz 8 @0x01456 Rechteck x 4050..6050, z -6600..-5600 Band 3; frei ab z -5168 (Koerper-Schub): Blick Nord, 620 voraus = z -5788.
    set(_env RE15_DEBUG_JUMP=2040@240 RE15_SET_FLAG=3:0x92 RE15_EQUIP=3 RE15_PLAYER_POS=5000,-5168,1024,3
             RE15_PRESS=start@260,square@285,down@300,square@310,square@325 RE15_MSG_LOG=1 RE15_SCD_TRACE=1 RE15_EXIT_AT=460)
else()
    message(FATAL_ERROR "r41_keyitems: TEIL '${TEIL}' unbekannt")
endif()

file(MAKE_DIRECTORY "${_lauf}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(WORKDIR "${_lauf}")
re15_start_spiel(_rv 600 ${_gemein} ${_env} "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")
message(STATUS "r41_keyitems[${TEIL}]: exit=${_rv}, ${_lauf}")

if(NOT EXISTS "${_lauf}/debug.log")
    message(FATAL_ERROR "r41_keyitems[${TEIL}]: kein debug.log (${_lauf})")
endif()
file(READ "${_lauf}/debug.log" _d)
string(FIND "${_d}" "EXIT_AT: Bild" _p)
if(_p LESS 0)
    message(FATAL_ERROR "r41_keyitems[${TEIL}]: Endbild nicht erreicht - Lauf vorher abgerissen (${_lauf})")
endif()

if(TEIL MATCHES "^(kurbel|elza|renn|geh|west|nord|ost)$")
    pruef(_d "Frage 1 entfaellt: Ja (USE)")
    pruef(_d "motion=0, flags=0x40)")
    pruef(_d "@0x0A8C op=0x32")                    # Pos_set
    pruef(_d "Gegenstand 0x33 verlaesst das Inventar")
    pruef_nicht(_d "Frage 1 entfaellt: Nein")
    # Pos_set @0x0A8C und Plc_motion @0x0A9C im selben Skripttakt: der Sleep 20 @0x0A98 wartet nicht (RE2 ROOM1110 sub13 -> sub15)
    string(REGEX MATCH "\\[scd\\] F([0-9]+) slot=[0-9]+ @0x0A8C" _m "${_d}")
    set(_f_pos "${CMAKE_MATCH_1}")
    string(REGEX MATCH "\\[scd\\] F([0-9]+) slot=[0-9]+ @0x0A9C" _m "${_d}")
    set(_f_mot "${CMAKE_MATCH_1}")
    message(STATUS "Pos_set F${_f_pos}, Plc_motion F${_f_mot}")
    if(NOT _f_pos OR NOT "${_f_pos}" STREQUAL "${_f_mot}")
        list(APPEND _fehler "Pos_set F${_f_pos} und Plc_motion F${_f_mot} nicht im selben Takt")
    endif()
    execute_process(COMMAND "${RE15_WERKZEUG}" kurbel "${_lauf}/state.txt"
                    RESULT_VARIABLE _rw OUTPUT_VARIABLE _ow ERROR_VARIABLE _ew)
    message(STATUS "${_ow}${_ew}")
    if(NOT _rw EQUAL 0)
        list(APPEND _fehler "Auswerter kurbel")
    endif()
elseif(TEIL STREQUAL "falsch")
    pruef(_d "USE 0x33 ROOM2020: keine Stelle")
    pruef_nicht(_d "USE-Lauf")
    pruef_nicht(_d "Plc_motion")
    pruef_nicht(_d "verlaesst das Inventar")
elseif(TEIL STREQUAL "aktion")
    pruef(_d "Frage 1 entfaellt: Nein (Aktionstaste)")
    pruef(_d "@0x0A5C op=0x2B")                    # Message_on 0 "It's a square hole."
    pruef_nicht(_d "@0x0A7E op=0x29")              # kein Ja-Zweig
    pruef_nicht(_d "Plc_motion")
    pruef_nicht(_d "verlaesst das Inventar")
elseif(TEIL STREQUAL "karte")
    pruef(_d "Frage 7 entfaellt: Ja (USE)")
    pruef(_d "[msg] room=10d0 id=9")
elseif(TEIL STREQUAL "aufnahme")
    pruef(_d "[msg] room=2010 id=0")
    pruef(_d "Gegenstand 0x33 ins Inventar")
    pruef(_d "[msg] room=2010 id=1")
elseif(TEIL STREQUAL "bombe")
    pruef(_d "Gegenstand 0x35 ins Inventar")
    pruef(_d "Frage 5 entfaellt: Ja (USE)")
    pruef(_d "Cut_chg(14)")
    pruef_nicht(_d "[msg] room=2040 id=0")
    pruef_nicht(_d "[msg] room=2040 id=5")
elseif(TEIL STREQUAL "karte_aktion")
    pruef(_d "Frage 7: nur Beschreibung")
    pruef_nicht(_d "[msg] room=10d0 id=9")
endif()

if(_fehler)
    message(FATAL_ERROR "r41_keyitems[${TEIL}]: ${_fehler}")
endif()
file(REMOVE_RECURSE "${_lauf}")
message(STATUS "r41_keyitems[${TEIL}]: bestanden")
