# =============================================================================
# Runde 42 "story" (v0.8.31), Stufe STAGE2 - DURCHLAEUFE in der echten exe (RE15_WEGPLAN: Gehen, Blick, Aktionstaste, Zielen/Feuern
# wie der Spieler; Raumwechsel nur ueber die Tueren/Leitern des Spiels). Konstanten/Belege include/re15_story2_r42.h, Code
# engine/src/story2_r42.c. PORT-STORY nach dem Konzept (3.2 L1/L1b, 4.2 E1, 6b; K-01..K-03, K-07, K-08).
#
# Nachbesserung 1 (Pruefung 1 der Stufe, Befund 3 "Alligator-Tod gesetzt, nicht gespielt; Tor nach 20A0 ungeprueft; Gegenseite per
# RE15_PLAYER_POS"): die frueheren Glieder ablass/schluessel/gegenseite sind je Figur ZWEI Glieder mit durchgereichtem Zustand.
#
# TEIL kette | kette_elza (Raeume xxx0 bzw. xxx1, Elza ueber die Spieler-Auswahl):
#   Glied 1 "kette" - EIN Spielstand vom Einstieg (Debug-Menue in 208x vor dem Alligator, KEINE Flags) bis 20Ax:
#     Treppe -> Gang -> Rechner -> Ja -> Wasser sinkt -> Hinweis msg 52 "Next to the computer... there's a keyhole." -> Leiter hinab
#     -> 209x: Kampf (Auftritt, Zielen/Feuern, Kanister-Szene in 208x, Rueckkehr, Lauf zum Kanister, Tod) -> Schluessel bei der
#     Leiche (Wegplan GEHE_OBJ auf obj 4) -> Hinweis msg 47 -> Uhr (Bit 90) -> Leiter hinauf -> 208x -> Schluesselloch -> msg 54 +
#     (9,113)/(9,119) -> Wegwerf-Abfrage -> Szene L1/E1 (Begleiter von Suedwesten, Spieler danach an seinen Platz; bei Elza Tuerton
#     2030 und Sherry aus der Nische oben an der Treppe) -> Bit 118 -> Leiter -> 209x -> Leiter -> 208x: Begleiter wartet, keine
#     zweite Szene -> Leiter -> 209x -> Rampe (Kletterzelle) -> Tor -> 20Ax; am Ende liest der Wegplan die Flags aus.
#     Die Kanister-Szene legt den 208x-Platz am Rechner schon VOR dem Tod an (Raumaufbau mit (3,245) = 1); die Szene L1/E1 startet
#     erst mit dem Schluessel.
#   Glied 2 "gegenseite" - mit GENAU den Flags, die Glied 1 in 20Ax ausgibt (RE15_SET_FLAG aus dessen Wegplan-Log), an der Tuer zu
#     11Ax: Original sub10 @0x01FE6 -> 11Ax-Ankunft am Steg (28150,1250) -> Steg hinab -> Tuer nach 207x schliesst auf (msg 11,
#     (3,151), Bit 121) -> 207x -> Tuer nach 204x "It's locked from the other side." (Bit 122 = 0) -> Abkuerzung -> 11Ax.
#     Das Raetsel in 20Ax zwischen Tor und Tuer (Leiter, Rolladen, Kiste) ist nicht Gegenstand der Stufe.
# TEIL tueren | tueren_elza:
#   Lauf bombe Zeitbomben-Weg: 2040 Halle ((3,146)/(3,53)) -> Rost-Tuer -> Treppe -> Galerie -> Tuer nach 2070 "unlocked" (msg 6,
#            Bit 122) -> 2070 Nordgang -> Tuer nach 11A0 "It's locked from the other side." (msg 1), (3,151) bleibt 0, kein 11A0.
#            (Der Einstieg in die Halle mit benutzter Zeitbombe bleibt ein Debug-Menue-Einstieg mit (3,146)/(3,53) wie
#            integration_r41_ventilkette_sperre a/b - der Weg dorthin ist nicht Gegenstand der Stufe.)
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
    set(TEIL kette)
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

if(TEIL STREQUAL "kette" OR TEIL STREQUAL "kette_elza")
    if(TEIL STREQUAL "kette")
        set(_wer ${_leon})
        set(S 0)
        set(_npc "Marvin")
        set(_name Leon)
        set(_letzte 48)
    else()
        set(_wer ${_elza})
        set(S 1)
        set(_npc "John \\+ Sherry")
        set(_name Elza)
        set(_letzte 51)
    endif()
    string(TOLOWER "209${S}" _k90)
    string(TOLOWER "20A${S}" _ka0)
    string(TOLOWER "11A${S}" _k11)
    # ---- Glied 1 "kette": EIN Spielstand vom Einstieg in 208x (Debug-Menue, vor dem Alligator, KEINE Flags) bis 20Ax. Alles zu Fuss
    # mit der Aktionstaste und dem Zielen/Feuern des Spielers (Wegplan SCHIESS); kein RE15_SET_FLAG, kein RE15_PLAYER_POS, keine
    # Messhaken am Alligator (RE15_K2080_GATOR_HP / RE15_K2080_RUECK_KANISTER / RE15_GB_TEST bleiben aus). Testmittel: Pistolen-
    # munition (RE15_GIVE=3:250, Waffe 3 ausgeruestet), Spieler-HP gehalten (RE15_R39_MESS_SPIELER_HP=100).
    # Kampf: Schiessen bis zur Kanister-Szene (Bit 115, Schwelle HP < 25000 @0x80101ebc-dc), dann zum Kanister im Nordbecken laufen
    # (der Alligator folgt dem Spieler und nimmt ihn dabei auf - Runde 41 G2 Punkt 8, kein Anschwimmen) und weiter schiessen bis zum
    # Tod (Bit 111). Rampe 2090: Kletterzelle an der Suedkante (z -18750, Blick Nord, Aktionstaste; Form test_r41_gatorfix rampe_oben).
    _lauf(kette 2300 "RAUM 208${S}
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
GEHE -6000 -21000 150
GEHE -6000 -17500 150
BLICK 3072
WARTE 10
SCHIESS 9 115 12000
WARTE 60
GEHE -6000 -17500 150
GEHE -5100 -13900 150
BLICK 3072
WARTE 10
SCHIESS 9 111 12000
WARTE 120
FLAG 9 111
OBJ 4
GEHE_OBJ 250
BLICK_OBJ
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
GEHE -6000 -21000 150
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
WARTE 3000
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
GEHE -3000 -25000 150
GEHE 4500 -21000 150
GEHE 4500 -18750 100
BLICK 3072
WARTE 10
AKTION
WARTE 90
GEHE 6600 -16350 100
BLICK 0
WARTE 10
AKTION
RAUM 20A${S}
WARTE 30
FLAG 9 90
FLAG 9 110
FLAG 9 111
FLAG 9 112
FLAG 9 113
FLAG 9 114
FLAG 9 115
FLAG 9 116
FLAG 9 117
FLAG 9 118
FLAG 9 119
FLAG 9 120
FLAG 9 121
FLAG 9 122
FLAG 3 151
FLAG 3 206
FLAG 3 245
ENDE
" ${_wer} RE15_DEBUG_JUMP=208${S}@250 RE15_GIVE=3:250 RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 "RE15_EXIT_AT=300#20A${S}")
    set(_wp1 "${_wp}")
    # Ablassen (K-01)
    _muss("${_dbg}" "[msgtext] room=208${S} id=0 \"From here maybe I can control the water level." kette "Original-Frage am Rechner (sub02 @0x0C86)")
    _danach("${_dbg}" "Cut_chg(7)" "[msgtext] room=208${S} id=52 \"Next to the computer... there's a keyhole.\"" kette
            "K-01: Hinweis nach dem Ablassen (Wasser-Einstellung Cut 7)")
    _muss("${_wp}" "FLAG (3,245) = 1" kette "(3,245) = 1 (Wasser abgelassen)")
    # Kampf: echter Tod, keine gesetzten Todes-Bits
    _darf_nicht("${_dbg}" "MESSHAKEN" kette "kein Messhaken am Alligator")
    _muss("${_dbg}" "AUSLOESER geschwaecht" kette "Kanister-Szene durch Treffer ausgeloest (HP-Schwelle @0x80101ebc-dc)")
    _muss("${_wp}" "SCHIESS: (9,115) = 1 nach" kette "Kanister-Szene gelaufen (Bit 115)")
    _muss("${_wp}" "SCHIESS: (9,111) = 1 nach" kette "Alligator im Kampf getoetet (Bit 111 durch den Tod, nicht gesetzt)")
    _danach("${_dbg}" "SZENE (" "Water Key (Item 0x34, Bits (9,111)/(9,112)) bei der Leiche" kette
            "Schluessel liegt nach dem ECHTEN Tod bei der Leiche (gatorszene_r41.c key_bei_leiche)")
    # Schluessel + Uhr (K-08, Befund 6)
    _muss("${_dbg}" "neben dem Schluessel im Bild (Kamera" kette "K-08 / Befund 6: Uhr in der Einstellung des Schluessels sichtbar")
    _muss("${_dbg}" "[msgtext] room=${_k90} id=47 \"There's something else lying next to it...\"" kette "K-08: Hinweis beim Schluessel")
    _muss("${_wp}" "FLAG (9,112) = 1" kette "Schluessel genommen")
    _muss("${_wp}" "FLAG (9,90) = 1" kette "Uhr genommen (Taken-Bit 90 @0x00A12)")
    # Schluesselloch + Szene (K-01, L1/E1, K-07; Befund 1/4)
    _muss("${_dbg}" "[msgtext] room=208${S} id=54 \"${_name} used the key.\"" kette "K-01: msg 54")
    _darf_nicht("${_dbg}" "Schluesselloch ohne Schluessel" kette "der Spieler kam mit dem Schluessel")
    _muss("${_wp}" "FLAG (9,113) = 1" kette "Tor 2090<->20A0 offen (Bit 113)")
    _muss("${_wp}" "FLAG (9,119) = 1" kette "Bit 119")
    _muss("${_wp}" "FLAG (9,118) = 1" kette "Szene gesehen (Bit 118)")
    foreach(_id RANGE 43 ${_letzte})
        _muss("${_dbg}" "[msgtext] room=208${S} id=${_id} " kette "Zeile ${_id}")
    endforeach()
    _zaehle("${_dbg}" "-> Szene (L1|E1)" _n_szene)
    if(NOT _n_szene EQUAL 1)
        _fehler("[kette]: Szene ${_n_szene} mal gestartet (soll genau 1)")
    endif()
    _zaehle("${_dbg}" "\\[msgtext\\] room=208${S} id=43 " _n_43)
    if(NOT _n_43 EQUAL 1)
        _fehler("[kette]: Zeile 43 ${_n_43} mal (soll 1 - keine zweite Szene)")
    endif()
    _danach("${_dbg}" "Begleiter erscheint bei (-13400,-17200)" "Ankunft Begleiter bei (-11" kette "Befund 1: Begleiter von Suedwesten an N")
    _danach("${_dbg}" "Ankunft Begleiter bei (-11" "Ankunft Spieler bei (-10" kette "Befund 1: Spieler geht nach P, nachdem der Begleiter steht")
    if(TEIL STREQUAL "kette_elza")
        _danach("${_dbg}" "Tuer-Ton 2030" "Sherry erscheint bei (-15800,-16400)" kette "Befund 4: erst der Tuerton, dann Sherry oben an der Treppe")
        _danach("${_dbg}" "Sherry erscheint bei (-15800,-16400)" "Ankunft Sherry bei (-10" kette "Befund 4: Sherry laeuft vor das Paar (S)")
    endif()
    _muss("${_dbg}" "Begleiter bleibt zurueck (K-07)" kette "K-07: Szenen-Ende")
    if(NOT _dbg MATCHES "ROOM208${S}: ${_npc} wartet")
        _fehler("[kette]: Wiederbetreten 208${S}: '${_npc} wartet' fehlt (K-07)")
    endif()
    # Tor 209x -> 20Ax
    _muss("${_dbg}" "[aot] DOOR FIRE slot=1 rect=(7250,-16350,hw=500,hh=1750)" kette "Tor 209${S} Slot 1 @0x009B6 feuert")
    _muss("${_dbg}" "loaded room${_ka0}.rdt" kette "durch das Tor 209${S} -> 20A${S}")
    message(STATUS "r42_story2[${TEIL}] kette: Ablassen -> Kampf (echter Tod) -> Schluessel + Uhr -> Schluesselloch -> Szene einmal -> Begleiter wartet -> Rampe -> Tor -> 20A${S}")

    # ---- Glied 2 "gegenseite": mit dem Zustand, den Glied 1 hinterlaesst (die FLAG-Ausgabe nach dem Betreten von 20Ax wird hier
    # zu RE15_SET_FLAG), an der Tuer zu 11Ax in 20Ax (Original-Raetsel des Raums dazwischen - Leiter, Rolladen, Kiste - ist nicht
    # Gegenstand der Stufe): Tuer -> Original sub10 @0x01FE6 (Set(3,206), "You've opened the lock.") -> 11Ax-Ankunft (28150,1250) ->
    # Steg hinab -> Tuer nach 207x schliesst auf (msg 11, (3,151), Bit 121) -> 207x -> Tuer nach 204x "It's locked from the other
    # side." -> Tuer nach 11Ax (Abkuerzung) -> 11Ax.
    string(FIND "${_wp1}" "RAUM 20A${S} geladen" _p20)
    if(_p20 LESS 0)
        _fehler("[gegenseite]: Glied 1 endete nicht in 20A${S}")
    endif()
    string(SUBSTRING "${_wp1}" ${_p20} -1 _wp1_ende)
    string(REGEX MATCHALL "FLAG \\(([0-9]+),([0-9]+)\\) = 1" _gesetzt "${_wp1_ende}")
    set(_flags "")
    foreach(_f IN LISTS _gesetzt)
        string(REGEX REPLACE "FLAG \\(([0-9]+),([0-9]+)\\) = 1" "\\1:\\2" _bb "${_f}")
        list(APPEND _flags "${_bb}")
    endforeach()
    list(REMOVE_DUPLICATES _flags)
    string(REPLACE ";" "," _flags "${_flags}")
    foreach(_soll 9:90 9:111 9:112 9:113 9:118 9:119 3:245)
        if(NOT ",${_flags}," MATCHES ",${_soll},")
            _fehler("[gegenseite]: Zustand aus Glied 1 ohne ${_soll} ('${_flags}')")
        endif()
    endforeach()
    message(STATUS "r42_story2[${TEIL}] gegenseite: Zustand aus Glied 1 = ${_flags}")
    _lauf(gegenseite 600 "RAUM 20A${S}
WARTE 30
BLICK 3072
WARTE 10
FLAG 3 206
AKTION
WARTE 60
AKTION
WARTE 60
FLAG 3 206
AKTION
RAUM 11A${S}
WARTE 30
GEHE 6500 300 150
BLICK 1024
WARTE 10
AKTION
WARTE 90
GEHE 6500 -2900 100
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
" ${_wer} RE15_DEBUG_JUMP=20A${S}@250 "RE15_SET_FLAG=${_flags}" "RE15_PLAYER_POS=25800,-1000,3072,2" "RE15_EXIT_AT=200#11A${S}!3")
    _muss("${_dbg}" "[msgtext] room=${_ka0} id=2 \"You've opened the lock.\"" gegenseite "20A${S} sub10 @0x01FE6 schliesst die Tuer zu 11A${S} auf")
    _muss("${_dbg}" "[aot] DOOR FIRE slot=2 rect=(25800,50,hw=1000,hh=500) target_cut=9 spawn=(28150,-1800,1250)" gegenseite
          "Tuer 20A${S} -> 11A${S} (Ankunft am Steg x 28150)")
    _muss("${_dbg}" "verworfen - wartet in 208${S}" gegenseite "Befund 8: Mod-Begleiter in 11A${S} verworfen, solange sie in 208${S} warten")
    _muss("${_dbg}" "[msgtext] room=${_k11} id=11 \"${_name} unlocked the door.\"" gegenseite "K-02: 11A${S} schliesst auf")
    _muss("${_wp}" "FLAG (3,151) = 1" gegenseite "(3,151) = 1")
    _muss("${_wp}" "FLAG (9,121) = 1" gegenseite "Bit 121 = 1")
    _muss("${_dbg}" "[aot] DOOR FIRE slot=0 rect=(6500,-3850,hw=1100,hh=450) target_cut=12 spawn=(-18650,0,-19300)" gegenseite
          "Tuer = Door_aot_set @0x00F5A")
    _muss("${_dbg}" "[msgtext] room=207${S} id=1 \"It's locked from the other side.\"" gegenseite "K-03: Tuer nach 2040 ohne Bit 122 zu")
    _muss("${_dbg}" "[aot] DOOR FIRE slot=4 rect=(-19400,-19250,hw=500,hh=1000) target_cut=7 spawn=(6500,0,-3200)" gegenseite
          "Abkuerzung 207${S} -> 11A${S}")
    _zaehle("${_dbg}" "loaded room${_k11}.rdt" _n11)
    if(_n11 LESS 2)
        _fehler("[gegenseite]: 11A${S} nur ${_n11} mal geladen (soll 2: aus 20A${S} + Abkuerzung aus 207${S})")
    endif()
    message(STATUS "r42_story2[${TEIL}] gegenseite: 20A${S} -> 11A${S} (Ankunft x 28150) aufgeschlossen -> 207${S} -> 2040 zu -> Abkuerzung zurueck nach 11A${S}")
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

else()
    _fehler(": unbekannter TEIL '${TEIL}'")
endif()
file(REMOVE "${_exe_kopie}")
