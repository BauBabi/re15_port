# =============================================================================
# Runde 42 Strang "rest", Stufe 5 (U-08, NUTZERWUNSCH 18.11.2026 "ja, warum nicht?"): Elza geht durch die Tuer 4001 -> 401.
# Herleitung include/re15_ungenutzt_r42.h (U-08), Code engine/src/ungenutzt_r42.c re15_ungenutzt_r42_tuer_sce.
#
# Durchgehender Lauf mit der echten exe (Elza-Neuspiel, Memory Card per RE15_GIVE=0x21:1):
#   ROOM4001 (Debug-Sprung 400, Eintrittsszene und Gitter-Ablaeufe vorbei: (3,0x21) sub01 @0x0137C, (4,20) sub00 @0x0120C,
#   (3,0x91) @0x01214, (3,0xAF) @0x012A6), vor der Tuer Slot 1 (-15608,-3524) Blick -x, Viereck
#   -> Tuer Disc @0x1012 / Mod @0x1084 -> ROOM4011 (-21044,-24000)
#   -> Kiste Slot 9 (sub02 msg 6 -> Kistenbild), mit Kreuz schliessen
#   -> Rechner Slot 10 (sub03 msg 7 -> "Will you use the Memory Card?"), NEIN (kein Schreiben auf die Karte)
#   -> Funde Slot 7/8 (First Aid Spray Bit (9,11), Shotgun Shells Bit (9,12))
#   -> Tuer Slot 0 (Mod 4011 @0x04D2) zurueck nach ROOM4001 (-15608,-3524)
# Wege und Bildzahlen sind im Echtlauf gemessen (Zustandsprotokoll: 75 Einheiten je Bild vorwaerts, 96 je Bild Drehung).
# Geprueft im debug.log:
#   - "[ungenutzt42] ... ROOM4001 Slot 1 @0x1084: Tuer nach 401 offen" und DOOR FIRE slot=1 nach (-21044,0,-24000)
#   - in 4011: ITEMBOX (msg 6), SAVEPOINT (msg 7) mit Frage und Antwort NEIN, Bits (9,11)/(9,12) 0 -> 1
#   - DOOR FIRE slot=0 nach (-15608,0,-3524), danach Raum 4001 (EXIT_AT)
#   - kein "Security Room"-Text (msg 14, "[msgtext] room=4001 id=14") in 4001 - auf dem alten Stand steht er dort
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
cmake_minimum_required(VERSION 3.16)
set(_tag "r42_ungenutzt_401")
foreach(_v RE15_PC_EXE WORKDIR)
    if(NOT ${_v})
        message(FATAL_ERROR "${_tag}: ${_v} fehlt")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

execute_process(COMMAND "${CMAKE_COMMAND}" -E environment OUTPUT_VARIABLE _umg)
string(REGEX MATCHALL "(^|\n)RE15_[A-Za-z0-9_]*=" _umg_namen "${_umg}")
foreach(_n ${_umg_namen})
    string(REGEX REPLACE "[\n=]" "" _n "${_n}")
    unset(ENV{${_n}})
endforeach()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe "${_exe_dir}/re15_pc_r42u401_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

# Abschnitt 0 = Startraum bis zum Sprung (Bild 250), 1 = ROOM4001, 2 = ROOM4011 (RE15_INPUT_SCRIPT_BASIS=spiel, '|' je Raum).
#   Kiste:   8 Bilder vor (-21644), 11 Bilder links (Blick -z), 7 Bilder vor -> (-21623,-24518), Viereck, Kreuz schliesst
#   Rechner: 22 rechts (+z), 42 vor, 11 links (-x), 41 vor -> (-24572,-21410), Viereck x3 (Seiten), rechts = NEIN, Viereck
#   Funde:   11 rechts, 53 vor, 11 links, 15 vor (Kollision haelt bei -24752), Viereck x5 (Aufheben + Bestaetigen, je Fund)
#   Tuer:    21 rechts (+x), 40 vor, 11 rechts (-z), 86 vor, 11 links (+x) -> Vorwaertspunkt im Rechteck x -21300..-20400
set(_kiste   "W2,U0.2667,L0.3667,U0.2333,W0.5,A0.1,W3,X0.1,W2")
set(_rechner "R0.7333,W0.3,U1.4,W0.3,L0.3667,W0.3,U1.3667,W0.5,A0.1,W2.5,A0.1,W2.5,A0.1,W3,R0.1,W0.5,A0.1,W2")
set(_funde   "R0.3667,W0.3,U1.7667,W0.3,L0.3667,W0.3,U0.5,W0.5,A0.1,W3,A0.1,W3,A0.1,W3,A0.1,W3,A0.1,W3")
set(_tuer    "R0.7,W0.3,U1.3333,W0.3,R0.3667,W0.3,U2.8667,W0.3,L0.3667,W0.5,A0.2,W8")
set(_skript "W12|W1.5,A0.2,W8|${_kiste},${_rechner},${_funde},${_tuer}")

foreach(_versuch 1 2)
    file(REMOVE "${WORKDIR}/debug.log")
    re15_start_spiel(_rv 480
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
        "RE15_SET_FLAG=3:33,4:20,3:145,3:175" "RE15_DEBUG_JUMP=4000@250"
        "RE15_PLAYER_POS=-15608,-3524,2048" "RE15_GIVE=0x21:1"
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_skript}"
        RE15_MSG_LOG=1 "RE15_FLAG_WATCH=9:11,9:12"
        "RE15_EXIT_AT=200#4001"
        "${_exe}")
    if(EXISTS "${WORKDIR}/debug.log")
        file(STRINGS "${WORKDIR}/debug.log" _ende REGEX "EXIT_AT: Bild 200 in Raum 4001")
        if(_ende)
            break()
        endif()
    endif()
    message(STATUS "${_tag}: Lauf ${_versuch} ohne Ende-Zeile (exit=${_rv}) -> Wiederholung")
endforeach()
file(REMOVE "${_exe}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "${_tag}: kein debug.log (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)

set(_fehler 0)
macro(pruefe _was _muster)
    string(FIND "${_dbg}" "${_muster}" _pos)
    if(_pos LESS 0)
        message(STATUS "FAIL ${_tag}: ${_was} (fehlt: '${_muster}')")
        math(EXPR _fehler "${_fehler} + 1")
    else()
        message(STATUS "ok   ${_tag}: ${_was}")
    endif()
endmacro()
macro(pruefe_re _was _re)
    string(REGEX MATCH "${_re}" _m "${_dbg}")
    if(NOT _m)
        message(STATUS "FAIL ${_tag}: ${_was} (Muster '${_re}')")
        math(EXPR _fehler "${_fehler} + 1")
    else()
        message(STATUS "ok   ${_tag}: ${_was}")
    endif()
endmacro()

pruefe("Elza (Spieler-Familie PL04)" "[pl] Spieler-Familie PL04")
pruefe("Haken: Mod-Satz @0x1084 wird zur Tuer (Disc @0x1012 sce 2)" "ROOM4001 Slot 1 @0x1084: Tuer nach 401 offen (sce 2 wie Disc @0x1012 statt 0), U-08")
pruefe("Tuer 4001 Slot 1 feuert nach 401 (Ankunft Disc/Mod-Nutzlast)" "[aot] DOOR FIRE slot=1 rect=(-16300,-3500,hw=800,hh=1100) target_cut=0 spawn=(-21044,0,-24000)")
pruefe("Kiste in 4011 (sub02 msg 6 -> Kistenbild)" "[msg] room=4011 id=6 ITEMBOX")
pruefe("Rechner in 4011 (sub03 msg 7 -> Speicher-Ablauf)" "[save] room=4011 msg=7 card_slot=0 -> FRAGE (Memory Card benutzen?)")
pruefe("Speicher-Frage beantwortet (NEIN)" "[save] Antwort: NEIN -> zurueck")
pruefe("Fund First Aid Spray (Bit (9,11))" "ROOM4011 (9,11) 0 -> 1")
pruefe("Fund Shotgun Shells (Bit (9,12))" "ROOM4011 (9,12) 0 -> 1")
pruefe("Tuer 4011 Slot 0 zurueck nach 400 (Mod 4011 @0x04D2)" "[aot] DOOR FIRE slot=0 rect=(-20850,-24100,hw=450,hh=700) target_cut=0 spawn=(-15608,0,-3524)")
pruefe_re("Reihenfolge Tuer hin, Kiste, Rechner, Funde, Tuer zurueck, 4001"
    "DOOR FIRE slot=1 .*room=4011 id=6 ITEMBOX.*room=4011 msg=7 .*ROOM4011 [(]9,11[)] 0 -> 1.*ROOM4011 [(]9,12[)] 0 -> 1.*DOOR FIRE slot=0 .*EXIT_AT: Bild 200 in Raum 4001")
string(FIND "${_dbg}" "[msgtext] room=4001 id=14 " _gesperrt)
if(NOT _gesperrt LESS 0)
    message(STATUS "FAIL ${_tag}: in 4001 kam msg 14 \"Security Room / It's locked.\"")
    math(EXPR _fehler "${_fehler} + 1")
endif()
if(_fehler GREATER 0)
    message(FATAL_ERROR "${_tag}: ${_fehler} Fehler (${WORKDIR})")
endif()
message(STATUS "${_tag}: 0 Fehler")
