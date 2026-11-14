# =============================================================================
# Runde 42 "story" (v0.8.31), Stufe STAGE2 - DURCHLAEUFE in der echten exe (RE15_WEGPLAN: Gehen, Blick, Aktionstaste wie der
# Spieler; Raumwechsel nur ueber die Tueren/Leitern des Spiels, Einstieg je Lauf ueber das Debug-Menue RE15_DEBUG_JUMP mit dem
# Zustand, den das vorige Glied hinterlaesst). Konstanten/Belege include/re15_story2_r42.h, Code engine/src/story2_r42.c.
# PORT-STORY nach dem Konzept (3.2 L1/L1b, 4.2 E1, 6b; K-01..K-03, K-07, K-08).
#
# TEIL leon | elza (Raeume xxx0 bzw. xxx1, Elza ueber die Spieler-Auswahl):
#   Glied 1 "ablass": 2080 vor dem Alligator (keine Flags) -> Treppe -> Gang -> Rechner -> Ja -> Wasser sinkt -> Hinweis msg 52
#            "Next to the computer... there's a keyhole." -> zum Fuss der Leiter -> Leiter hinab -> 2090 geladen.
#   Glied 2 "schluessel": 2090 nach dem Kampf (Alligator tot: 110/111/114/115/116 wie die Runde-41-Glieder) -> zum Schluessel ->
#            Aufnahme -> Hinweis msg 47 (die Uhr liegt daneben) -> Uhr aufnehmen (Bit 90) -> Leiter hinauf -> 2080 -> Gang ->
#            Schluesselloch -> msg 54 + (9,113)/(9,119) -> Wegwerf-Abfrage -> Szene L1/E1 (Zeilen 43..48/51) -> Bit 118 ->
#            zurueck zur Leiter -> 2090 (Tor nicht mehr gesperrt) -> Leiter hinauf -> 2080: Begleiter wartet, KEINE zweite Szene,
#            kein Schluesselloch -> Leiter hinab -> 2090.
# TEIL tueren | tueren_elza:
#   Lauf 1 Zeitbomben-Weg: 2040 Halle ((3,146)/(3,53)) -> Rost-Tuer -> Treppe -> Galerie -> Tuer nach 2070 "unlocked" (msg 6,
#            Bit 122) -> 2070 Nordgang -> Tuer nach 11A0 "It's locked from the other side." (msg 1), (3,151) bleibt 0, kein 11A0.
#   Lauf 2 Gegenseite: 11A0 Kanal (Ankunft wie ueber 20A0) -> Tuer nach 2070 "unlocked" (msg 11, (3,151), Bit 121) -> 2070 ->
#            Tuer nach 2040 "It's locked from the other side." (Bit 122 = 0) -> Tuer nach 11A0 (Abkuerzung) -> 11A0.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -DTEIL=<teil> -P test_r42_story2.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r42_story2: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r42_story2: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL leon)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r42s2_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r42s2_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

function(_fehler _was)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r42_story2[${TEIL}]${_was}")
endfunction()
function(_muss _text _wort _name _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(_p LESS 0)
        _fehler("[${_name}]: ${_was} - '${_wort}' fehlt")
    endif()
endfunction()
function(_darf_nicht _text _wort _name _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(NOT _p LESS 0)
        _fehler("[${_name}]: ${_was} - '${_wort}' steht im Log")
    endif()
endfunction()
function(_danach _text _wort1 _wort2 _name _was)
    string(FIND "${_text}" "${_wort1}" _p1)
    if(_p1 LESS 0)
        _fehler("[${_name}]: ${_was} - '${_wort1}' fehlt")
    endif()
    string(SUBSTRING "${_text}" ${_p1} -1 _rest)
    string(FIND "${_rest}" "${_wort2}" _p2)
    if(_p2 LESS 0)
        _fehler("[${_name}]: ${_was} - '${_wort2}' steht nicht nach '${_wort1}'")
    endif()
endfunction()
function(_zaehle _text _wort _out)
    string(REGEX MATCHALL "${_wort}" _m "${_text}")
    list(LENGTH _m _n)
    set(${_out} ${_n} PARENT_SCOPE)
endfunction()

set(_leon RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
set(_elza RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
          RE15_PSELECT_AUTO_SWITCH=1)

macro(_lauf _name _timeout _plan)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(WRITE "${WORKDIR}/wegplan_plan.txt" "${_plan}")
    re15_start_spiel(_rv ${_timeout} ${ARGN} "RE15_WEGPLAN=${WORKDIR}/wegplan_plan.txt" RE15_WEGPLAN_LOG=wegplan.txt
                     RE15_STATE_LOG=state.log "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        _fehler("[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    set(_wp "")
    if(EXISTS "${WORKDIR}/wegplan.txt")
        file(READ "${WORKDIR}/wegplan.txt" _wp)
    endif()
endmacro()

# Weg im 2080 vom Fuss der Leiter (Ankunft aus 2090: (-9950,-14400,-25700)) zum Rechner und zurueck (Form test_r41_ventilkette_weg w4)
set(_zum_rechner "GEHE -10496 -24008 150
GEHE -10541 -8106 150
GEHE -11300 -7350 150
BLICK 3072
WARTE 10")
set(_zur_leiter "GEHE -10541 -8106 150
GEHE -10496 -24008 150
GEHE -9750 -25650 100
BLICK 0
WARTE 10
AKTION")
# 2090: vom Leiter-Ankunftspunkt (-8200,-25550) zur Leiter hinauf (Tuer Slot 0 @0x00996 Rechteck x -9400..-8400 z -26000..-25000)
set(_hinauf "GEHE -8700 -25500 100
BLICK 2048
WARTE 10
AKTION")

if(TEIL STREQUAL "leon" OR TEIL STREQUAL "elza")
    if(TEIL STREQUAL "leon")
        set(_wer ${_leon})
        set(S 0)
        set(_npc "Marvin")
        set(_letzte 48)
    else()
        set(_wer ${_elza})
        set(S 1)
        set(_npc "John \\+ Sherry")
        set(_letzte 51)
    endif()
    # ---- Glied 1: ablass ----------------------------------------------------------------------------------------------------
    _lauf(ablass 420 "RAUM 208${S}
WARTE 30
BLICK 0
WARTE 10
AKTION
WARTE 200
GEHE -15500 -16500 150
GEHE -11500 -16500 150
GEHE -11400 -11500 150
GEHE -11300 -7350 150
BLICK 3072
WARTE 10
AKTION
WARTE 90
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 400
AKTION
WARTE 60
AKTION
WARTE 60
FLAG 3 245
${_zur_leiter}
RAUM 209${S}
WARTE 30
ENDE
" ${_wer} RE15_DEBUG_JUMP=208${S}@250 "RE15_EXIT_AT=60#209${S}")
    _muss("${_dbg}" "[msgtext] room=208${S} id=0 \"From here maybe I can control the water level." ablass "Original-Frage am Rechner (sub02 @0x0C86)")
    _muss("${_dbg}" "Rechner (Ereignis 2) = sub02 mit Hinweis msg 52" ablass "Hinweis in den Ja-Zweig vor das Endif @0x0CEE")
    _danach("${_dbg}" "Cut_chg(7)" "[msgtext] room=208${S} id=52 \"Next to the computer... there's a keyhole.\"" ablass
            "K-01: Hinweis nach dem Ablassen (Wasser-Einstellung Cut 7)")
    _muss("${_wp}" "FLAG (3,245) = 1" ablass "(3,245) = 1 (Wasser abgelassen)")
    _muss("${_dbg}" "loaded room209${S}.rdt" ablass "Leiter hinab nach 209${S}")
    message(STATUS "r42_story2[${TEIL}] ablass: Rechner 208${S} -> Wasser sinkt -> Hinweis msg 52 -> Leiter -> 209${S}")

    # ---- Glied 2: schluessel, Schluesselloch, Szene, Wiederbetreten ---------------------------------------------------------
    _lauf(schluessel 900 "RAUM 209${S}
WARTE 30
GEHE -900 -23629 150
BLICK 0
WARTE 10
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
FLAG 9 112
FLAG 9 90
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
FLAG 9 90
GEHE -8200 -25550 150
${_hinauf}
RAUM 208${S}
WARTE 30
${_zum_rechner}
AKTION
WARTE 60
AKTION
WARTE 40
AKTION
WARTE 40
AKTION
WARTE 2700
FLAG 9 113
FLAG 9 119
FLAG 9 118
${_zur_leiter}
RAUM 209${S}
WARTE 30
GEHE -8200 -25550 150
${_hinauf}
RAUM 208${S}
WARTE 30
${_zum_rechner}
AKTION
WARTE 120
${_zur_leiter}
RAUM 209${S}
WARTE 30
ENDE
" ${_wer} RE15_DEBUG_JUMP=209${S}@250 "RE15_SET_FLAG=9:110,9:111,9:114,9:115,9:116,3:245" "RE15_EXIT_AT=60#209${S}!5")
    string(TOLOWER "209${S}" _k90)
    _muss("${_dbg}" "Taschenuhr (Item 0x3F, Platz 16, obj 0) neben dem Schluessel" schluessel "K-08: Uhr neben dem Schluessel, Puls")
    _muss("${_dbg}" "[msgtext] room=${_k90} id=47 \"There's something else lying next to it...\"" schluessel "K-08: Hinweis beim Schluessel")
    _muss("${_wp}" "FLAG (9,112) = 1" schluessel "Schluessel genommen")
    _muss("${_wp}" "FLAG (9,90) = 1" schluessel "Uhr genommen (Taken-Bit 90 @0x00A12)")
    _muss("${_dbg}" "Schluesselloch neben dem Rechner = Platz 4" schluessel "Schluesselloch angelegt")
    if(TEIL STREQUAL "leon")
        _muss("${_dbg}" "[msgtext] room=2080 id=54 \"Leon used the key.\"" schluessel "K-01: msg 54")
    else()
        _muss("${_dbg}" "[msgtext] room=2081 id=54 \"Elza used the key.\"" schluessel "K-01: msg 54")
    endif()
    _muss("${_wp}" "FLAG (9,113) = 1" schluessel "Tor 2090<->20A0 offen (Bit 113)")
    _muss("${_wp}" "FLAG (9,119) = 1" schluessel "Bit 119")
    _muss("${_wp}" "FLAG (9,118) = 1" schluessel "Szene gesehen (Bit 118)")
    foreach(_id RANGE 43 ${_letzte})
        _muss("${_dbg}" "[msgtext] room=208${S} id=${_id} " schluessel "Zeile ${_id}")
    endforeach()
    _zaehle("${_dbg}" "-> Szene (L1|E1)" _n_szene)
    if(NOT _n_szene EQUAL 1)
        _fehler("[schluessel]: Szene ${_n_szene} mal gestartet (soll genau 1)")
    endif()
    _zaehle("${_dbg}" "\\[msgtext\\] room=208${S} id=43 " _n_43)
    if(NOT _n_43 EQUAL 1)
        _fehler("[schluessel]: Zeile 43 ${_n_43} mal (soll 1 - keine zweite Szene)")
    endif()
    _muss("${_dbg}" "Begleiter bleibt zurueck (K-07)" schluessel "K-07: Szenen-Ende")
    if(NOT _dbg MATCHES "ROOM208${S}: ${_npc} wartet")
        _fehler("[schluessel]: Wiederbetreten 208${S}: '${_npc} wartet' fehlt (K-07)")
    endif()
    _zaehle("${_dbg}" "Schluesselloch neben dem Rechner = Platz 4" _n_loch)
    if(NOT _n_loch EQUAL 1)
        _fehler("[schluessel]: Schluesselloch ${_n_loch} mal angelegt (soll 1 - nach Bit 119 nicht mehr)")
    endif()
    _zaehle("${_dbg}" "loaded room${_k90}.rdt" _n_90)
    if(_n_90 LESS 3)
        _fehler("[schluessel]: 209${S} nur ${_n_90} mal geladen (soll >= 3: Start, nach der Szene, am Ende)")
    endif()
    # nach Bit 113 ist das Tor 2090 nicht mehr gesperrt: genau EIN "Tor gesperrt" (der erste Aufbau vor dem Schluesselloch)
    _zaehle("${_dbg}" "ROOM209${S} Slot 1 \\(Tor\\) gesperrt" _n_tor)
    if(NOT _n_tor EQUAL 1)
        _fehler("[schluessel]: 'Slot 1 (Tor) gesperrt' ${_n_tor} mal (soll 1: nach dem Schluesselloch offen)")
    endif()
    message(STATUS "r42_story2[${TEIL}] schluessel: Schluessel + Hinweis + Uhr -> Leiter -> Schluesselloch -> Szene einmal -> Begleiter wartet -> Tor offen")
elseif(TEIL STREQUAL "tueren" OR TEIL STREQUAL "tueren_elza")
    if(TEIL STREQUAL "tueren")
        set(_wer ${_leon})
        set(S 0)
        set(_name Leon)
    else()
        set(_wer ${_elza})
        set(S 1)
        set(_name Elza)
    endif()
    _lauf(bombe 480 "RAUM 204${S}
WARTE 30
GEGNERWEG
GEHE -14626 1672
GEHE -14715 -5290
GEHE 4961 -5418
GEHE 5061 -7194
GEHE 5020 -10382
GEHE -1342 -10490
GEHE -1356 -13782
BLICK 1024
WARTE 10
AKTION
WARTE 200
GEHE -2132 -19815
BLICK 2048
WARTE 10
AKTION
WARTE 120
GEHE -19646 -19806
BLICK 2048
WARTE 10
FLAG 9 122
AKTION
WARTE 60
AKTION
WARTE 40
FLAG 9 122
AKTION
RAUM 207${S}
WARTE 30
GEGNERWEG
GEHE -16000 -19300
GEHE -18471 -19256
BLICK 2048
WARTE 10
AKTION
WARTE 60
AKTION
WARTE 40
AKTION
WARTE 90
FLAG 3 151
ENDE
" ${_wer} RE15_DEBUG_JUMP=204${S}@250 "RE15_SET_FLAG=3:146,3:53,3:172,7:98,7:99" "RE15_PLAYER_POS=-12850,1600,2048,3"
      "RE15_EXIT_AT=900#207${S}")
    _muss("${_dbg}" "[aot] DOOR FIRE slot=2 rect=(-2400,-19800,hw=400,hh=1000)" bombe "Rost-Tuer/Galerie slot 2 @0x012DC")
    _muss("${_dbg}" "[msgtext] room=204${S} id=6 \"${_name} unlocked the door.\"" bombe "K-03: 2040 schliesst von der Galerie auf")
    _muss("${_wp}" "FLAG (9,122) = 0" bombe "Bit 122 vor dem Druck 0")
    _muss("${_wp}" "FLAG (9,122) = 1" bombe "Bit 122 nach dem Druck 1")
    _muss("${_dbg}" "loaded room207${S}.rdt" bombe "Durchgang nach 207${S}")
    _muss("${_dbg}" "[msgtext] room=207${S} id=1 \"It's locked from the other side.\"" bombe "K-02: Tuer nach 11A0 von der Korridorseite zu")
    _darf_nicht("${_dbg}" "\"You've opened the lock.\"" bombe "kein Riegel mehr auf der 2070-Seite (sub02 @0x0CE8)")
    _muss("${_wp}" "FLAG (3,151) = 0" bombe "(3,151) bleibt 0")
    string(TOLOWER "11A${S}" _k11)
    _darf_nicht("${_dbg}" "loaded room${_k11}.rdt" bombe "die Zeitbombe fuehrt nicht mehr nach 11A0 (am Alligator vorbei)")
    message(STATUS "r42_story2[${TEIL}] bombe: 204${S} Galerie aufgeschlossen -> 207${S} -> 'It's locked from the other side.' -> kein 11A${S}")

    _lauf(gegenseite 420 "RAUM 11A${S}
WARTE 30
GEGNERWEG
GEHE 6500 -2900
BLICK 1024
WARTE 10
FLAG 3 151
AKTION
WARTE 60
AKTION
WARTE 40
FLAG 3 151
FLAG 9 121
AKTION
RAUM 207${S}
WARTE 30
GEGNERWEG
GEHE 3000 -19250
GEHE 4500 -19250
BLICK 0
WARTE 10
AKTION
WARTE 60
AKTION
WARTE 40
GEHE -16000 -19300
GEHE -18471 -19256
BLICK 2048
WARTE 10
AKTION
RAUM 11A${S}
WARTE 30
ENDE
" ${_wer} RE15_DEBUG_JUMP=11A${S}@250 "RE15_PLAYER_POS=6500,-2600,1024,0" "RE15_EXIT_AT=60#11A${S}!3")
    _muss("${_dbg}" "[msgtext] room=${_k11} id=11 \"${_name} unlocked the door.\"" gegenseite "K-02: 11A0 schliesst auf")
    _muss("${_wp}" "FLAG (3,151) = 1" gegenseite "(3,151) = 1")
    _muss("${_wp}" "FLAG (9,121) = 1" gegenseite "Bit 121 = 1")
    _muss("${_dbg}" "[aot] DOOR FIRE slot=0 rect=(6500,-3850,hw=1100,hh=450) target_cut=12 spawn=(-18650,0,-19300)" gegenseite
          "Tuer = Door_aot_set @0x00F5A")
    _muss("${_dbg}" "[msgtext] room=207${S} id=1 \"It's locked from the other side.\"" gegenseite "K-03: Tuer nach 2040 ohne Bit 122 zu")
    _muss("${_dbg}" "[aot] DOOR FIRE slot=4 rect=(-19400,-19250,hw=500,hh=1000) target_cut=7 spawn=(6500,0,-3200)" gegenseite
          "Abkuerzung 2070 -> 11A0")
    _zaehle("${_dbg}" "loaded room${_k11}.rdt" _n11)
    if(_n11 LESS 2)
        _fehler("[gegenseite]: 11A${S} nur ${_n11} mal geladen (soll 2: Start + Abkuerzung)")
    endif()
    message(STATUS "r42_story2[${TEIL}] gegenseite: 11A${S} aufgeschlossen -> 207${S} -> 2040 zu -> Abkuerzung zurueck nach 11A${S}")
else()
    _fehler(": unbekannter TEIL '${TEIL}'")
endif()
file(REMOVE "${_exe_kopie}")
