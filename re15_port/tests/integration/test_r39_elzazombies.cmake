# test_r39_elzazombies.cmake — Runde 39 Spur Q "elzazombies": Elza, Zombie-Durchbrueche in der Panikphase an der ECHTEN
# exe (Dossier analysis/befunde_runde39/Q_elzazombies.md, Mechanik include/re15_elzazombies_r39.h). Aufruf:
#   cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE=<probe_r39_elzazombies_karte> -DRE15_AUSWERTER=<test_r39_elzazombies>
#         -DWORKDIR=<dir> -DTEIL=<kette|knall|treppe|durch|ohne|leon|altstand|oben> -P test_r39_elzazombies.cmake
#
#   kette   Elza-Spielstand ROOM1041 (Intro durch: (3,125),(3,193); 1011 besucht: (3,116); 10E1 und 1071 nie betreten = alle
#           leben), CONTINUE, (9,97) bei Bild 200 (= Spur P beim 1041-Ruf). debug.log: Flanke -> Umzug (10E1 4, 1071 5) ->
#           Tuerknall in 1041 -> ROOM1061 (+4 Gegner, Cut 2) -> Tuerknall Satz 0 -> ROOM1031 (Nachtrag 12: +5 Kopien der
#           1071-Zombies, erstes Bild Cut 7, Original-Knall @0x02862, +3 Kriecher, Cut 12) -> ROOM1041 -> "Rueckkehr beendet,
#           Steuerung frei"; je Spur-Zeile: jede Kopie >= 400 naeher an der Kamera; Spieler-HP 100 in allen drei Schnitten;
#           nach der Rueckkehr steht Elza an der gemerkten Stelle.
#   knall   derselbe Lauf zweimal mit RE15_AUDIO_CAP_SYNC, einmal RE15_R39Q_KNALL_STUMM: die Differenz = zwei hoerbare
#           Knaelle (Tuerknall Auftakt 1041, Tuerknall Ende 1061 vor der 1071-Tuer) — test_r39_elzazombies auswerten_ton.
#   durch   Nachtrag 11, ECHTER WEG: Elza ROOM1061 Etage 0 an der Tuer nach 1041, (9,95) + lebende 1061-Kopien + Panikphase
#           (9,97) (H1-Entscheid), Aktion -> ROOM1041 -> Rolltor-Durchbruch (Cut 1, Knall, Rolltor hoch, 4 Kopien durch und
#           naeher an die Kamera, HP 100) -> Elza am Eintrittspunkt mit Steuerung; Bild durchbruch.ppm.
#   leon    Nachtrag 12, ECHTER WEG Leon: Spielstand ROOM1130 vor Irons' Tuer, Tuer -> Szene -> Montage 1130 -> Treppenhaus
#           ROOM1060 (+4 Kopien der 10E0-Zombies im Raumaufbau, erstes Bild Cut 2, Knall-Folge) -> 1040 -> 1030; Kopien naeher an
#           der Kamera, HP 100, erstes Bild des 1060-Besuchs (state.log) Cut 2 mit vier Zombies, Pad-Sperre in Bild 10.
#   treppe  ECHTER WEG: Elza ROOM1041 an der Tuer zu 1061 mit (9,95) (Montage gelaufen), Aktion -> Tuer (Slot 2 @0x010D8)
#           -> ROOM1061 Etage 0 -> Nachspawn (Kopien an der Westseite, gehalten) -> Treppe A (Etage 0 -> 2) -> Treppe B
#           (2 -> 4) -> frei bei der Ankunft (25424,24568), kleinster Abstand >= 4000, alle Kopien an der Westseite
#           (x <= 21100); Bild der Ankunft (ankunft.ppm).
#   ohne    Leon ROOM1040 mit (9,97)-Flanke: keine Kette; Elza-Spielstand mit (9,97) schon gesetzt (Laden): keine Flanke;
#           H1: Weg wie TEIL durch OHNE (9,97) (John gerettet): kein Durchbruch, Elza mit Steuerung an der Tuer.
#   altstand NACHBESSERUNG 3, Mangel 1 der Abnahme 2, ECHTER WEG: Karte der Abnahme M10 (Leon nach Irons' Tod, (9,71),(3,94),(9,73),
#           alle vier 10E0-Zombies getoetet, Kopie-Bits und (9,96) nie gesetzt = Spielstand v0.8.22..v0.8.26), Tuer 1040 -> 1060:
#           kein Nachspawn, Bild 90 des 1060-Besuchs ohne Gegner (= v0.8.26-exe, Abnahme 2 M10). GEGENPROBE dieselbe Karte mit
#           (9,96)=1: Nachspawn mit vier Kopien (der Riegel sieht den Unterschied).
#   oben    NACHBESSERUNG 3, Mangel 2 + ORCHESTRATOR-ENTSCHEID E1, ECHTER WEG von oben (Tuer Etage 8 = Ankunft aus ROOM1120/1121
#           Slot 0 @0x00C76, Treppe B 8 -> 6, Treppe A 6 -> 4; Skript der Abnahme M9) fuer Leon (1060, Zustand nach seiner Montage
#           mit (9,96)) und Elza (1061, (9,95)): Nachspawn "Ankunft von oben" mit vier Kopien an der Ostseite (x >= 25500), im
#           ganzen Besuch keine nicht initialisierte Kopie (state.log nie " st=0 "), gehalten und sichtbar in Schnitt 2
#           (cam=2 mit g=20), frei am Ende der Treppe A (z 25368) mit kleinstem Abstand >= 4591 (= gemessen von unten), kein Griff
#           (gr=0, Spieler-HP 100 bis Bild 700); Bild ankunft_oben.ppm (Bild 480, Schnitt 2 waehrend Treppe A).
#
# Parallel-/Linux-fest: exe-Kopie mit eigenem Praefix je TEIL, eigener Laufordner je Lauf, Umgebung nur ueber
# re15_start_spiel (spiel_lauf.cmake) — geerbte RE15_*-Variablen des Aufrufers (jede Schreibweise) loescht der Block
# nach dem include vor dem ersten Lauf (VERTRAG Runde 39 Abschnitt 2, Nachbesserung 1) —, keine .exe-/Backslash-Pfade.
cmake_minimum_required(VERSION 3.16)
foreach(_v RE15_PC_EXE RE15_KARTE RE15_AUSWERTER WORKDIR TEIL)
    if(NOT DEFINED ${_v})
        message(FATAL_ERROR "r39_elzazombies: ${_v} fehlt")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Keine Umgebungsvariablen des Aufrufers erben (VERTRAG Runde 39 Abschnitt 2; Abnahme 0 Mangel 1: mit geerbtem
# RE15_NOAUDIO=1 schrieb TEIL knall keinen Mitschnitt, "[audio] RE15_NOAUDIO set -> audio disabled" audio_pc.c:1955).
# spiel_lauf.cmake setzt und loescht nur die UEBERGEBENEN Paare; alles andere ginge an Kartenwerkzeug, exe und Auswerter.
# Muster test_r39_stimmen.cmake / test_r39_heliport1171.cmake, hier ohne Ruecksicht auf die Gross-/Kleinschreibung:
# Windows liest Umgebungsnamen ohne sie (gemessen: "re15_nb1=1" -> $ENV{RE15_NB1} = 1; "re15_scd_trace=1" schaltet in
# test_r39_elzazombies den SCD-Trace ein, 15 -> 325 Zeilen), "cmake -E environment" listet den Namen aber so, wie er
# gesetzt wurde. Danach Selbstpruefung: bleibt einer stehen, bricht der Riegel ab statt mit fremder Umgebung zu messen.
function(r39q_geerbte_namen _out)
    execute_process(COMMAND "${CMAKE_COMMAND}" -E environment OUTPUT_VARIABLE _envdump)
    string(REGEX MATCHALL "(^|\n)[Rr][Ee]15_[A-Za-z0-9_]*=" _treffer "${_envdump}")
    set(_namen "")
    foreach(_t IN LISTS _treffer)
        string(REGEX REPLACE "^\n?([Rr][Ee]15_[A-Za-z0-9_]*)=$" "\\1" _n "${_t}")
        list(APPEND _namen "${_n}")
    endforeach()
    set(${_out} "${_namen}" PARENT_SCOPE)
endfunction()
r39q_geerbte_namen(_geerbt)
foreach(_n IN LISTS _geerbt)
    unset(ENV{${_n}})
endforeach()
r39q_geerbte_namen(_rest)
if(_rest)
    message(FATAL_ERROR "r39_elzazombies[${TEIL}]: geerbte Umgebung laesst sich nicht loeschen: ${_rest}")
endif()
if(_geerbt)
    message(STATUS "r39_elzazombies[${TEIL}]: geerbte Umgebung geloescht: ${_geerbt}")
endif()

set(_tag "r39_elzazombies[${TEIL}]")
set(_basisdir "${WORKDIR}_${TEIL}")
file(MAKE_DIRECTORY "${_basisdir}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe "${_exe_dir}/re15_pc_r39elzazombies_${TEIL}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

set(_fehler "")
macro(fail _text)
    list(APPEND _fehler "${_text}")
    message(STATUS "FAIL ${_tag}: ${_text}")
endmacro()
macro(ok _text)
    message(STATUS "ok   ${_tag}: ${_text}")
endmacro()

# Ein Lauf: Karte schreiben, exe starten (CONTINUE), debug.log lesen (-> _dbg), Laufordner -> _dir.
function(lauf _name _kartenargs _timeout)
    set(WORKDIR "${_basisdir}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log" "${WORKDIR}/cap.raw" "${WORKDIR}/ankunft.ppm")
    execute_process(COMMAND "${RE15_KARTE}" "re15_card.mcr" ${_kartenargs}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "${_tag}/${_name}: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv ${_timeout} RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
                     RE15_CARD_SLOT=0 ${ARGN} "${_exe}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "${_tag}/${_name}: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _d)
    set(_dbg "${_d}" PARENT_SCOPE)
    set(_dir "${WORKDIR}" PARENT_SCOPE)
    message(STATUS "${_tag}/${_name}: exit=${_rv}, ${WORKDIR}")
endfunction()

# Position eines Musters im Log ab _ab (-1 = nicht gefunden) -> _pos
function(finde _muster _ab _out)
    string(SUBSTRING "${_dbg}" ${_ab} -1 _rest)
    string(FIND "${_rest}" "${_muster}" _p)
    if(_p LESS 0)
        set(${_out} -1 PARENT_SCOPE)
    else()
        math(EXPR _p "${_p} + ${_ab}")
        set(${_out} ${_p} PARENT_SCOPE)
    endif()
endfunction()

set(_elza_1041 "1041;-21008;0;-13134;2112;0;3:125;3:193;3:116")
# (4,0xF0) = 240: Johns Szene in 1021 gelaufen (ROOM1021 sub02 @0x01FE0) - die echte Vorbedingung der Panikphase. Seit
# Spur P in der Integration (Runde 39) startet damit ihr Ruf RUF1041 beim Betreten von 1041 und setzt (9,97) beim
# Kartenschluss selbst (_elza_1041_p: die ECHTE Kette); ohne (4,0xF0) loescht Spur P ein von aussen gesetztes Bit 97 im
# selben Bild ("John gerettet"). Fuer Laeufe, die bildgleich sein muessen (knall: Differenz zweier Mitschnitte), setzt
# _flanke_ohne_ruf (4,0xF0) und (9,97) erst im Spiel: kein RUF1041 (beim Eintritt fehlte F0), keine Rettung (F0 steht);
# der Kartenschirm des Rufs schliesst nach Menue-Schritten, nicht Spielbildern (Lauf 12:52: F514 gegen F515).
set(_elza_1041_p "${_elza_1041};4:240;9:64")   # 9:64 Herzschluessel: RUF1041 startet erst damit (Nachtrag 17)
set(_flanke_ohne_ruf "RE15_SET_FLAG_AT=4:240,9:97@200")

# ------------------------------------------------------------------------------------------------------------------
if(TEIL STREQUAL "kette")
    # ECHTE KETTE P -> Q (Integration Runde 39): der Ruf der Spur P (RUF1041, Karte) setzt (9,97) beim Kartenschluss
    # (Lauf 12:29: Bild 432), Spur Q sieht die Flanke und startet die Montage. Ende im ZWEITEN 1041-Besuch: der erste
    # verlaesst 1041 bei Bild ~540, Bild 900 erreicht nur die Rueckkehr (Bildzaehler je Raum). Spielstand: 10E1 und 1071
    # nie betreten = 4 + 5 Zombies leben (Nachtrag 12: 1071-Schritt).
    lauf(kette "${_elza_1041_p}" 900 RE15_NOAUDIO=1 "RE15_EXIT_AT=900#1041")
    set(_pos 0)
    foreach(_m
            "Flanke (9,97) 0->1 in ROOM1041"
            "Umzug: 10E1 lebend 4 -> 1061; 1071 lebend 5 -> 1031 Cut 7; (9,95)=1"
            "Folge #1 K ROOM1041"
            "PC loaded room1061"
            "S ROOM1061 T"
            "C ROOM1061 T"
            "K ROOM1061"
            "PC loaded room1031"
            "S ROOM1031 T"
            "C ROOM1031 T"
            "K ROOM1031 T"
            "S ROOM1031 T"
            "C ROOM1031 T"
            "PC loaded room1041"
            "Rueckkehr beendet, Steuerung frei")
        finde("${_m}" ${_pos} _p)
        if(_p LESS 0)
            fail("in der Folge fehlt '${_m}' (nach Stelle ${_pos})")
            break()
        endif()
        math(EXPR _pos "${_p} + 1")
    endforeach()
    if(_pos GREATER 0)
        ok("Folge Flanke -> Umzug (10E1 4, 1071 5) -> Knall 1041 -> 1061 (Spawn, Cut) -> Knall -> 1031 (Kopien, Cut 7, Original-Knall, Kriecher, Cut 12) -> 1041 -> Steuerung frei")
    endif()
    # Satz/Wert je Eintrag (Takte sind Spielbilder; unter Last verschieben sie sich nicht, der Text muss passen):
    #   1061: +4 Kopien, Cut 2, am Ende Tuerknall Satz 0 (die 1071-Tuer, Leons 1040-Ende mit (9,76)=1)
    #   1031: +5 Kopien im Raumaufbau, erstes Bild Cut 7, Original-Knall @0x02862 (2), +3 Kriecher, Cut 12
    foreach(_m "S ROOM1061 T105 4" "C ROOM1061 T105 2" "K ROOM1061 T237 0" "S ROOM1031 T281 5" "C ROOM1031 T281 7"
               "K ROOM1031 T457 2" "S ROOM1031 T467 3" "C ROOM1031 T489 12")
        string(FIND "${_dbg}" "${_m}" _p)
        if(_p LESS 0)
            string(REGEX REPLACE " T[0-9]+ " " T[0-9]+ " _rx "${_m}")
            string(REGEX MATCH "${_rx}" _mm "${_dbg}")
            if(NOT _mm)
                fail("Folge-Eintrag '${_m}' fehlt")
            endif()
        endif()
    endforeach()
    ok("1061: +4 Kopien, Cut 2, Tuerknall Satz 0; 1031: +5 Kopien Cut 7, Original-Knall, +3 Kriecher, Cut 12 (Nachtrag 12)")
    if(_dbg MATCHES "ROOM10[36]1 Cut [0-9]+ Bild [0-9]+: Aktor [0-9]+ Bit 1(28|29|30|31) ")
        fail("eine 1041-Kopie (Bits 128..131) in der Montage")
    endif()
    # Spur-Zeilen: jede Kopie naeher an der Kamera (1061 Cut 2: Bits 124..127; 1031 Cut 7: Leons 1030-Cut-7-Bits 47,48,49,75,76;
    # 1031 Cut 12: Kriecher 133/144/145)
    string(REGEX MATCHALL "ROOM10[36]1 Cut [0-9]+ Bild [0-9]+: Aktor [0-9]+ Bit [0-9]+ Abstand zur Kamera [0-9]+ -> [0-9]+" _sp "${_dbg}")
    set(_n 0)
    set(_nah 0)
    set(_n7 0)
    foreach(_z IN LISTS _sp)
        if(_z MATCHES "ROOM(10[36]1) Cut ([0-9]+) Bild [0-9]+: Aktor [0-9]+ Bit ([0-9]+) Abstand zur Kamera ([0-9]+) -> ([0-9]+)")
            set(_raum "${CMAKE_MATCH_1}")
            set(_cut "${CMAKE_MATCH_2}")
            set(_bit "${CMAKE_MATCH_3}")
            set(_d0 "${CMAKE_MATCH_4}")
            set(_d1 "${CMAKE_MATCH_5}")
            set(_zaehlt 0)
            if(_raum STREQUAL "1061" AND _bit GREATER_EQUAL 124 AND _bit LESS_EQUAL 127)
                set(_zaehlt 1)
            elseif(_raum STREQUAL "1031" AND _cut EQUAL 12 AND (_bit EQUAL 133 OR _bit EQUAL 144 OR _bit EQUAL 145))
                set(_zaehlt 1)
            elseif(_raum STREQUAL "1031" AND _cut EQUAL 7 AND (_bit EQUAL 47 OR _bit EQUAL 48 OR _bit EQUAL 49 OR _bit EQUAL 75 OR _bit EQUAL 76))
                set(_zaehlt 1)
                math(EXPR _n7 "${_n7} + 1")
            endif()
            if(_zaehlt)
                math(EXPR _n "${_n} + 1")
                math(EXPR _diff "${_d0} - ${_d1}")
                if(_diff GREATER_EQUAL 400)
                    math(EXPR _nah "${_nah} + 1")
                else()
                    fail("ROOM${_raum} Cut ${_cut} Bit ${_bit}: nur ${_diff} naeher an der Kamera (${_d0} -> ${_d1})")
                endif()
            endif()
        endif()
    endforeach()
    if(_n EQUAL 12 AND _nah EQUAL 12 AND _n7 EQUAL 5)
        ok("alle 12 (4 Kopien in 1061 Cut 2, 5 Kopien der 1071-Zombies in 1031 Cut 7, 3 Kriecher in 1031 Cut 12) >= 400 naeher an der Kamera")
    else()
        fail("Kopien-Spuren: ${_nah} von ${_n} naeher, davon Cut 7: ${_n7} (erwartet 12 von 12, Cut 7: 5)")
    endif()
    string(REGEX MATCHALL "Ende Bild [0-9]+: Spieler-HP min [0-9]+" _hp "${_dbg}")
    list(LENGTH _hp _nhp)
    set(_hp_ok 1)
    foreach(_z IN LISTS _hp)
        if(NOT _z MATCHES "HP min 100$")
            set(_hp_ok 0)
        endif()
    endforeach()
    if(_nhp EQUAL 3 AND _hp_ok)
        ok("Spieler-HP 100 in allen drei Schnitten (1061 Cut 2, 1031 Cut 7, 1031 Cut 12)")
    else()
        fail("Spieler-HP: ${_hp}")
    endif()
    # Rueckkehr an die beim Montage-Start gemerkte Stelle (Spur P dreht Elza beim Ruf zur Treppe - die Gier kommt aus dem Lauf)
    string(REGEX MATCH "Montage startet in ROOM1041: Rueckkehr \\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\) Gier ([0-9]+) Cut ([0-9]+)"
           _mm "${_dbg}")
    if(NOT _mm)
        fail("Montage-Start mit gemerkter Stelle nicht gefunden")
    else()
        set(_gx "${CMAKE_MATCH_1}")
        set(_gy "${CMAKE_MATCH_2}")
        set(_gz "${CMAKE_MATCH_3}")
        set(_gg "${CMAKE_MATCH_4}")
        set(_gc "${CMAKE_MATCH_5}")
        finde("PC loaded room1041" 100 _pr)
        string(SUBSTRING "${_dbg}" ${_pr} -1 _rueck)
        if(_rueck MATCHES "\\[walk\\] F[0-9]+ cut=${_gc} [^\n]*pl pos=\\(${_gx},${_gy},${_gz}\\) rot=${_gg}")
            ok("Rueckkehr: Elza an der gemerkten Stelle (${_gx},${_gy},${_gz}) Gier ${_gg} Cut ${_gc}")
        else()
            fail("Rueckkehr-Stelle (${_gx},${_gy},${_gz}) Gier ${_gg} Cut ${_gc} nicht gefunden")
        endif()
    endif()

# ------------------------------------------------------------------------------------------------------------------
elseif(TEIL STREQUAL "knall")
    # Ende in ROOM1031 bei Bild 300: beide Knaelle liegen davor (1041, 1061); der 1031-Schritt dauert ~357 Bilder (Lauf k5),
    # Bild 530 erreichte er nach Nachtrag 11 nicht mehr (Tuer zurueck nach 1041 vorher -> kein Ende, Zeitueberschreitung).
    # bildgleiche Flanke ohne den Ruf der Spur P (sein Kartenschirm schliesst nach Menue-Schritten, s. _flanke_ohne_ruf)
    lauf(laut "${_elza_1041}" 600 RE15_AUDIO_CAP_SYNC=cap.raw ${_flanke_ohne_ruf} "RE15_EXIT_AT=300#1031")
    set(_laut "${_dir}/cap.raw")
    lauf(stumm "${_elza_1041}" 600 RE15_AUDIO_CAP_SYNC=cap.raw RE15_R39Q_KNALL_STUMM=1 ${_flanke_ohne_ruf}
         "RE15_EXIT_AT=300#1031")
    set(_stumm "${_dir}/cap.raw")
    execute_process(COMMAND "${RE15_AUSWERTER}" auswerten_ton "${_laut}" "${_stumm}" TIMEOUT 120
                    RESULT_VARIABLE _rva OUTPUT_VARIABLE _outa)
    message(STATUS "${_outa}")
    if(_rva EQUAL 0)
        ok("zwei hoerbare Knaelle (Differenz der Mitschnitte)")
    else()
        fail("Knall-Auswertung exit=${_rva}")
    endif()

# ------------------------------------------------------------------------------------------------------------------
elseif(TEIL STREQUAL "treppe")
    # Das Eingabeskript laeuft auf der Spielbild-Achse, die je Raum neu zaehlt: das erste "A0.2" loest in 1041 die Tuer
    # aus (Elza steht im Tuer-Rechteck Slot 2, Blick Ost); in 1061 laeuft dasselbe Skript von vorn: "A0.2" am Tuer-Spawn
    # ohne Wirkung, danach der Weg zu Treppe A, Aufstieg 0 -> 2, Weg zu Treppe B, Aufstieg 2 -> 4 (Dossier 4.2).
    lauf(treppe "1041;-20700;0;-13000;0;0;3:125;3:193;3:116;9:95;7:224;7:225;7:226;7:227" 900 RE15_NOAUDIO=1
         RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
         "RE15_INPUT_SCRIPT=A0.2,W0.3,R0.034,U2.1,W0.3,L0.4,W0.3,U0.4,W0.3,A0.2,W2.5,U0.6,W0.3,L0.334,W0.3,U1.8,W0.3,L0.367,W0.3,U0.6,W0.3,A0.2,W4"
         "RE15_FRAMEDUMP=500:ankunft.ppm" "RE15_EXIT_AT=560#1061")
    finde("PC loaded room1061" 0 _p1)
    finde("Nachspawn 1061 (Programm E)" 0 _p2)
    if(_p1 GREATER_EQUAL 0 AND _p2 GREATER _p1)
        ok("Tuer 1041 -> 1061, Nachspawn im Raumaufbau")
    else()
        fail("kein Nachspawn nach der Tuer (Tuer ${_p1}, Nachspawn ${_p2})")
    endif()
    string(REGEX MATCH "Spielerin auf Etage 4 bei \\(([0-9-]+),([0-9-]+)\\) -> Kopien frei, kleinster Abstand ([0-9]+)" _m "${_dbg}")
    if(NOT _m)
        fail("keine Freigabe auf Etage 4")
    else()
        set(_fz "${CMAKE_MATCH_2}")
        set(_dm "${CMAKE_MATCH_3}")
        if(_fz EQUAL 24568 AND _dm GREATER_EQUAL 4000)
            ok("frei erst bei der Ankunft (z ${_fz} = Treppenende), kleinster Abstand ${_dm} >= 4000")
        else()
            fail("Freigabe bei z ${_fz} (erwartet 24568 = Treppenende) mit kleinstem Abstand ${_dm}")
        endif()
    endif()
    string(REGEX MATCHALL "ROOM1061: Kopie Aktor [0-9]+ Bit [0-9]+ bei \\([0-9-]+,[0-9-]+\\)" _ks "${_dbg}")
    set(_nk 0)
    set(_west 0)
    foreach(_z IN LISTS _ks)
        if(_z MATCHES "bei \\(([0-9-]+),([0-9-]+)\\)")
            math(EXPR _nk "${_nk} + 1")
            if(CMAKE_MATCH_1 LESS_EQUAL 21100)
                math(EXPR _west "${_west} + 1")
            endif()
        endif()
    endforeach()
    if(_nk EQUAL 4 AND _west EQUAL 4)
        ok("alle vier Kopien standen bei der Ankunft an der Westseite (x <= 21100) — gehalten waehrend Etage 0 und 2")
    else()
        fail("Kopien bei der Ankunft: ${_west} von ${_nk} an der Westseite")
    endif()
    if(EXISTS "${_dir}/ankunft.ppm")
        ok("Bild der Ankunft: ${_dir}/ankunft.ppm")
    else()
        fail("ankunft.ppm fehlt")
    endif()

# ------------------------------------------------------------------------------------------------------------------
elseif(TEIL STREQUAL "durch")
    # Nachtrag 11: Zustand nach der Montage ((9,95), 10E1-Quellen still, 1041-Kopien still, die vier 1061-Kopien leben =
    # nicht besiegt), Elza in ROOM1061 Etage 0 an der Tuer nach 1041 (Slot 2 @0x00D52, Rechteck (26600,24300,1000,2200)),
    # Blick Ost, Aktion -> Tuer -> ROOM1041 -> Rolltor-Durchbruch. Das Skript laeuft je Raum von vorn ("A" in 1041 faellt in
    # die Szene, Riegel).
    lauf(durch "1061;26200;0;25300;0;7;3:125;3:193;3:116;4:240;9:95;9:97;7:224;7:225;7:226;7:227;7:128;7:129;7:130;7:131" 900
         RE15_NOAUDIO=1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=A0.2,W20"
         "RE15_FRAMEDUMP=260:durchbruch.ppm" "RE15_EXIT_AT=500#1041")
    set(_pos 0)
    foreach(_m
            "Nachspawn 1061 (Programm E)"
            "DOOR FIRE slot=2"
            "PC loaded room1041"
            "Durchbruch 1041: 4 Zombies aus 1061 (nicht besiegt) -> hinter das Rolltor"
            "C ROOM1041 T0 1"
            "Durchbruch 1041 startet (Programm G): Eintritt (-21008,0,-13134) Gier 2112 Cut 5, Rolltor faehrt hoch"
            "K ROOM1041 T"
            "alle durch das Rolltor"
            "Durchbruch beendet, Steuerung frei")
        finde("${_m}" ${_pos} _p)
        if(_p LESS 0)
            fail("in der Folge fehlt '${_m}' (nach Stelle ${_pos})")
            break()
        endif()
        set(_pos ${_p})
    endforeach()
    if(_pos GREATER 0)
        ok("Tuer 1061 -> 1041 -> Umzug der 4 -> erstes Bild Cut 1 -> Knall -> alle durchs Rolltor -> Steuerung frei")
    endif()
    string(REGEX MATCHALL "ROOM1041 Cut 1 Bild [0-9]+: Aktor [0-9]+ Bit 1(28|29|30|31) Abstand zur Kamera [0-9]+ -> [0-9]+" _sp "${_dbg}")
    set(_n 0)
    set(_nah 0)
    foreach(_z IN LISTS _sp)
        if(_z MATCHES "Abstand zur Kamera ([0-9]+) -> ([0-9]+)")
            math(EXPR _n "${_n} + 1")
            math(EXPR _diff "${CMAKE_MATCH_1} - ${CMAKE_MATCH_2}")
            if(_diff GREATER_EQUAL 400)
                math(EXPR _nah "${_nah} + 1")
            endif()
        endif()
    endforeach()
    if(_n EQUAL 4 AND _nah EQUAL 4)
        ok("Cut 1: alle 4 Durchgebrochenen >= 400 naeher an der Kamera")
    else()
        fail("Cut 1: ${_nah} von ${_n} Kopien naeher (erwartet 4 von 4)")
    endif()
    if(_dbg MATCHES "ROOM1041 Cut 1 Ende Bild [0-9]+: Spieler-HP min 100")
        ok("Spieler-HP 100 waehrend der Szene")
    else()
        fail("Spieler-HP waehrend der Szene nicht 100")
    endif()
    finde("Durchbruch beendet" 0 _pe)
    string(SUBSTRING "${_dbg}" ${_pe} -1 _danach)
    if(_danach MATCHES "\\[walk\\] F[0-9]+ cut=5 [^\n]*pl pos=\\(-21008,0,-13134\\) rot=2112")
        ok("danach: Elza am Eintrittspunkt (-21008,0,-13134) Gier 2112, Kamera Cut 5")
    else()
        fail("Eintrittspunkt nach der Szene nicht gefunden")
    endif()
    if(EXISTS "${_dir}/durchbruch.ppm")
        ok("Bild der Szene: ${_dir}/durchbruch.ppm")
    else()
        fail("durchbruch.ppm fehlt")
    endif()

# ------------------------------------------------------------------------------------------------------------------
elseif(TEIL STREQUAL "ohne")
    lauf(leon "1040;-21008;0;-13134;2112;0" 600 RE15_NOAUDIO=1 RE15_SET_FLAG_AT=9:97@200 "RE15_EXIT_AT=500#1040")
    string(FIND "${_dbg}" "[elzazombies-r39]" _p)
    string(FIND "${_dbg}" "PC loaded room" _pl)
    if(_p LESS 0 AND _pl LESS 0)
        ok("Leon ROOM1040: (9,97)-Flanke startet nichts (keine Zeile, kein Raumwechsel)")
    else()
        fail("Leon: Montage-Spur im Log (${_p}) oder Raumwechsel (${_pl})")
    endif()
    lauf(laden "${_elza_1041_p};9:97" 600 RE15_NOAUDIO=1 "RE15_EXIT_AT=500#1041")
    string(FIND "${_dbg}" "Flanke (9,97)" _p)
    string(FIND "${_dbg}" "PC loaded room" _pl)
    if(_p LESS 0 AND _pl LESS 0)
        ok("Elza mit (9,97) im Spielstand: Laden ist keine Flanke, keine Kette")
    else()
        fail("Laden mit (9,97): Flanke (${_p}) oder Raumwechsel (${_pl})")
    endif()
    # H1-ENTSCHEID (Nachbesserung 2): nach Johns Rettung ((9,97)=0, Spur P) bricht in 1041 nichts mehr durch. Derselbe echte Weg
    # wie TEIL durch (Tuer 1061 -> 1041, vier 1061-Kopien leben), nur OHNE (9,97): kein Durchbruch, keine Szene, Elza steht mit
    # Steuerung an der Tuer (Cut 5).
    lauf(gerettet "1061;26200;0;25300;0;7;3:125;3:193;3:116;9:95;7:224;7:225;7:226;7:227;7:128;7:129;7:130;7:131" 600
         RE15_NOAUDIO=1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=A0.2,W20" "RE15_EXIT_AT=200#1041")
    finde("PC loaded room1041" 0 _p1)
    finde("Durchbruch 1041 entfaellt: Panikphase vorbei ((9,97)=0)" 0 _p2)
    string(FIND "${_dbg}" "Durchbruch 1041 startet" _p3)
    if(_p1 GREATER_EQUAL 0 AND _p2 GREATER _p1 AND _p3 LESS 0)
        ok("John gerettet ((9,97)=0): Tuer 1061 -> 1041, kein Rolltor-Durchbruch (H1)")
    else()
        fail("gerettet: Raum ${_p1}, 'entfaellt' ${_p2}, 'startet' ${_p3}")
    endif()
    string(SUBSTRING "${_dbg}" ${_p1} -1 _danach)
    if(_danach MATCHES "\\[walk\\] F[0-9]+ cut=5 [^\n]*pl pos=\\(-21008,0,-13134\\)")
        ok("gerettet: Elza an der Tuer (-21008,0,-13134), Kamera Cut 5, ohne Szene")
    else()
        fail("gerettet: Elza nicht an der Tuer / nicht Cut 5")
    endif()
# ------------------------------------------------------------------------------------------------------------------
elseif(TEIL STREQUAL "leon")
    # Nachtrag 12, ECHTER WEG Leon: Spielstand ROOM1130 vor der Tuer zu Irons' Buero (Standplatz von probe_r35_cut1150_karte:
    # (-5900,16350) Blick 2048 vor Slot 2 @0x008CE), (9,71) 10F0-Szene + (3,94) erste Irons-Szene, 10E0 nie betreten = alle vier
    # Medical-Room-Zombies leben. Aktion -> Tuer -> ROOM1150 -> Szene -> Montage 1130 -> TREPPENHAUS 1060 -> 1040 -> 1030; Ende im
    # 5. Bild von ROOM1030 (Muster integration_r35_cut1150 Laeufe E/F). state.log fuer das erste Bild des 1060-Besuchs.
    lauf(leon "1130;-5900;0;16350;2048;0;9:71;3:94" 900 RE15_NOAUDIO=1
         RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=W1,A0.2,W200"
         RE15_STATE_LOG=state.log "RE15_EXIT_AT=5#1030")
    set(_pos 0)
    foreach(_rx
            "Leon: Umzug 10E0 lebend 4 -> 1060"
            "Folge #[0-9]+ K ROOM1150 T[0-9]+ Knall Satz 0"
            "PC loaded room1130"
            "Folge #[0-9]+ K ROOM1130 T[0-9]+ Knall Satz 0"
            "PC loaded room1060"
            "Folge #[0-9]+ S ROOM1060 T[0-9]+ Spawn \\+ 4 \\(Raumaufbau"
            "Folge #[0-9]+ C ROOM1060 T[0-9]+ Kamera Cut 2 \\(Raumaufbau"
            "Montage 1060 \\(Runde 39 Spur Q, Programm L\\)"
            "Folge #[0-9]+ K ROOM1060 T[0-9]+ Knall Satz 1"
            "PC loaded room1040"
            "Folge #[0-9]+ S ROOM1040 T[0-9]+ Spawn \\+ 5 \\(Raumaufbau"
            "Folge #[0-9]+ C ROOM1040 T[0-9]+ Kamera Cut 1 \\(Raumaufbau"
            "PC loaded room1030")
        string(SUBSTRING "${_dbg}" ${_pos} -1 _rest)
        string(REGEX MATCH "${_rx}" _mm "${_rest}")
        if(NOT _mm)
            fail("in der Folge fehlt '${_rx}' (nach Stelle ${_pos})")
            break()
        endif()
        string(FIND "${_rest}" "${_mm}" _p)
        math(EXPR _pos "${_pos} + ${_p} + 1")
    endforeach()
    if(_pos GREATER 0 AND NOT _fehler)
        ok("Leon: Umzug 10E0 -> 1060; 1130 -> Tuerknall Satz 0 -> 1060 (+4 im Raumaufbau, erstes Bild Cut 2) -> Knall Satz 1 -> 1040 (+5, Cut 1) -> 1030")
    endif()
    string(REGEX MATCHALL "ROOM1060 Cut 2 Bild [0-9]+: Aktor [0-9]+ Bit [0-9]+ Abstand zur Kamera [0-9]+ -> [0-9]+" _sp "${_dbg}")
    set(_n 0)
    set(_nah 0)
    foreach(_z IN LISTS _sp)
        if(_z MATCHES "Bit (12[4-7]) Abstand zur Kamera ([0-9]+) -> ([0-9]+)")
            math(EXPR _n "${_n} + 1")
            math(EXPR _diff "${CMAKE_MATCH_2} - ${CMAKE_MATCH_3}")
            if(_diff GREATER_EQUAL 400)
                math(EXPR _nah "${_nah} + 1")
            else()
                fail("ROOM1060 Cut 2 Bit ${CMAKE_MATCH_1}: nur ${_diff} naeher an der Kamera")
            endif()
        endif()
    endforeach()
    if(_n EQUAL 4 AND _nah EQUAL 4)
        ok("ROOM1060 Cut 2: alle 4 Kopien >= 400 naeher an der Kamera (wie Elzas 1061)")
    else()
        fail("ROOM1060 Cut 2: ${_nah} von ${_n} Kopien naeher (erwartet 4 von 4)")
    endif()
    if(_dbg MATCHES "ROOM1060 Cut 2 Ende Bild [0-9]+: Spieler-HP min 100")
        ok("Spieler-HP 100 im Treppenhaus-Schnitt")
    else()
        fail("Spieler-HP im Treppenhaus-Schnitt nicht 100")
    endif()
    if(_dbg MATCHES "Montage 1130 \\(Programm 1\\)" AND _dbg MATCHES "Montage 1040 \\(Programm 2\\)" AND _dbg MATCHES "Montage 1030 \\(Programm 3\\)")
        ok("Leons uebrige Montage-Schritte 1130/1040/1030 laufen wie bisher")
    else()
        fail("Leons Montage-Schritte 1130/1040/1030 nicht alle im Log")
    endif()
    # state.log: die ERSTE Zeile jedes Raumbesuchs (Bildzaehler faellt zurueck); Besuch 3 = ROOM1060 (1 = 1150, 2 = 1130): Cut 2
    # und schon die vier Zombies (Spawn im Raumaufbau, Runde 36 Spur K K3); Bild 10 des Besuchs: Pad-Sperre (2,7) im Pause-Wort.
    if(EXISTS "${_dir}/state.log")
        file(STRINGS "${_dir}/state.log" _st)
        set(_letzt -1)
        set(_besuch 0)
        set(_erst "")
        set(_riegel "")
        foreach(_z IN LISTS _st)
            if(_z MATCHES "^F([0-9]+) ")
                set(_f ${CMAKE_MATCH_1})
                if(_f LESS _letzt)
                    math(EXPR _besuch "${_besuch} + 1")
                    if(_besuch EQUAL 3)
                        string(REGEX MATCHALL " t=[0-9a-f]+ st=" _g "${_z}")
                        list(LENGTH _g _ng)
                        string(REGEX MATCH "cam=([0-9]+)" _c "${_z}")
                        set(_erst "F${_f} ${_c} Gegner ${_ng}")
                    endif()
                endif()
                if(_besuch EQUAL 3 AND _f EQUAL 10)
                    string(REGEX MATCH "pf=([0-9A-F]+)" _riegel "${_z}")
                endif()
                set(_letzt ${_f})
            endif()
        endforeach()
        if(_erst MATCHES "cam=2 Gegner 4$")
            ok("erstes Bild des 1060-Besuchs: Cut 2 mit den vier Zombies (${_erst})")
        else()
            fail("erstes Bild des 1060-Besuchs: '${_erst}' (erwartet cam=2, 4 Gegner)")
        endif()
        if(_riegel MATCHES "^pf=01")
            ok("1060 Bild 10: Pad-Sperre (2,7) gesetzt (${_riegel})")
        else()
            fail("1060 Bild 10 ohne Pad-Sperre: '${_riegel}'")
        endif()
    else()
        fail("state.log fehlt")
    endif()

# ------------------------------------------------------------------------------------------------------------------
elseif(TEIL STREQUAL "altstand")
    # Nachbesserung 3, Mangel 1: alter Leon-Spielstand (Karte der Abnahme M10), Tuer 1040 -> 1060 (Slot 2 @0x010D8).
    foreach(_fall alt neu)
        if(_fall STREQUAL "alt")
            set(_k "1040;-20700;0;-13000;0;0;9:71;3:94;9:73;7:224;7:225;7:226;7:227")
        else()
            set(_k "1040;-20700;0;-13000;0;0;9:71;3:94;9:73;9:96;7:224;7:225;7:226;7:227")
        endif()
        lauf(altstand_${_fall} "${_k}" 300 RE15_NOAUDIO=1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
             "RE15_INPUT_SCRIPT=A0.2,W5" RE15_STATE_LOG=state.log "RE15_EXIT_AT=100#1060")
        if(NOT _dbg MATCHES "PC loaded room1060")
            fail("${_fall}: Tuer 1040 -> 1060 nicht durchschritten")
            continue()
        endif()
        # state.log: Bild 90 des ZWEITEN Raumbesuchs (1 = 1040, 2 = 1060; der Bildzaehler faellt beim Raumwechsel zurueck)
        set(_g90 -1)
        if(EXISTS "${_dir}/state.log")
            file(STRINGS "${_dir}/state.log" _st)
            set(_letzt -1)
            set(_besuch 1)
            foreach(_z IN LISTS _st)
                if(_z MATCHES "^F([0-9]+) ")
                    set(_f ${CMAKE_MATCH_1})
                    if(_f LESS _letzt)
                        math(EXPR _besuch "${_besuch} + 1")
                    endif()
                    if(_besuch EQUAL 2 AND _f EQUAL 90)
                        string(REGEX MATCHALL " t=[0-9a-f]+ st=" _g "${_z}")
                        list(LENGTH _g _g90)
                    endif()
                    set(_letzt ${_f})
                endif()
            endforeach()
        endif()
        if(_fall STREQUAL "alt")
            if(_dbg MATCHES "kein Nachspawn 1060: Umzug 10E0 -> 1060 nie gelaufen" AND NOT _dbg MATCHES "Leon: Nachspawn 1060"
               AND _g90 EQUAL 0)
                ok("alter Spielstand ((9,73)=1, ohne (9,96)): ROOM1060 Bild 90 ohne Gegner = v0.8.26 (Abnahme 2 M10)")
            else()
                fail("alter Spielstand: ROOM1060 Bild 90 mit ${_g90} Gegnern (erwartet 0, kein Nachspawn)")
            endif()
        else()
            if(_dbg MATCHES "Leon: Nachspawn 1060 \\(Programm E\\)" AND _g90 EQUAL 4)
                ok("Gegenprobe (9,96)=1: Nachspawn mit ${_g90} Kopien (der Riegel unterscheidet die Staende)")
            else()
                fail("Gegenprobe (9,96)=1: Bild 90 mit ${_g90} Gegnern (erwartet 4)")
            endif()
        endif()
    endforeach()

# ------------------------------------------------------------------------------------------------------------------
elseif(TEIL STREQUAL "oben")
    # Nachbesserung 3, Mangel 2 + E1: Ankunft von OBEN. Karte = Ankunft aus ROOM1120/1121 (Tuer Slot 0 @0x00C76: Raum 0x06,
    # (26000,-14400,25300) Gier 2048, Cut 0); Skript der Abnahme 2 M9/M11: Treppe B 8 -> 6, quer zur Westtreppe, Treppe A 6 -> 4.
    set(_skript "W0.5,U0.67,W0.3,L0.35,W0.3,U0.45,W0.3,A0.2,W5,R0.35,W0.3,U3.0,W0.3,R0.35,W0.3,U0.5,W0.3,A0.2,W8")
    foreach(_wer leon elza)
        if(_wer STREQUAL "leon")
            set(_raum 1060)
            set(_k "1060;26000;-14400;25300;2048;0;9:71;3:94;9:73;9:96;9:74;9:75;9:76;7:224;7:225;7:226;7:227")
            set(_ns "Leon: Nachspawn 1060 \\(Programm E\\): Ankunft von oben, Kopien an der Ostseite")
        else()
            set(_raum 1061)
            set(_k "1061;26000;-14400;25300;2048;0;3:125;3:193;3:116;9:95;7:224;7:225;7:226;7:227")
            set(_ns "Nachspawn 1061 \\(Programm E\\): Ankunft von oben, Kopien an der Ostseite")
        endif()
        lauf(oben_${_wer} "${_k}" 600 RE15_NOAUDIO=1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=30
             "RE15_INPUT_SCRIPT=${_skript}" RE15_STATE_LOG=state.log "RE15_FRAMEDUMP=480:ankunft_oben.ppm"
             "RE15_EXIT_AT=700#${_raum}")
        if(_dbg MATCHES "${_ns}")
            ok("${_wer}: Nachspawn von oben (Ostseite)")
        else()
            fail("${_wer}: kein Nachspawn 'Ankunft von oben' im debug.log")
        endif()
        string(REGEX MATCHALL "ROOM${_raum}: Kopie Aktor [0-9]+ Bit [0-9]+ bei \\([0-9-]+,[0-9-]+\\)" _ks "${_dbg}")
        set(_nk 0)
        set(_ost 0)
        foreach(_z IN LISTS _ks)
            if(_z MATCHES "bei \\(([0-9-]+),([0-9-]+)\\)")
                math(EXPR _nk "${_nk} + 1")
                if(CMAKE_MATCH_1 GREATER_EQUAL 25500)
                    math(EXPR _ost "${_ost} + 1")
                endif()
            endif()
        endforeach()
        if(_nk EQUAL 4 AND _ost EQUAL 4)
            ok("${_wer}: alle vier Kopien standen bei der Ankunft an der Ostseite (x >= 25500)")
        else()
            fail("${_wer}: Kopien bei der Ankunft: ${_ost} von ${_nk} an der Ostseite")
        endif()
        string(REGEX MATCH "ROOM${_raum}: Spielerin auf Etage 4 bei \\(([0-9-]+),([0-9-]+)\\) -> Kopien frei, kleinster Abstand ([0-9]+)" _m "${_dbg}")
        if(_m AND CMAKE_MATCH_2 EQUAL 25368 AND CMAKE_MATCH_3 GREATER_EQUAL 4591)
            ok("${_wer}: frei erst am Ende der Treppe A (z ${CMAKE_MATCH_2}), kleinster Abstand ${CMAKE_MATCH_3} >= 4591 (= von unten)")
        else()
            fail("${_wer}: Freigabe '${_m}' (erwartet z 25368, Abstand >= 4591)")
        endif()
        set(_st0 0)
        set(_sicht 0)
        set(_griff 0)
        set(_hp 0)
        if(EXISTS "${_dir}/state.log")
            file(STRINGS "${_dir}/state.log" _st)
            foreach(_z IN LISTS _st)
                if(_z MATCHES " t=[0-9a-f]+ st=0 ")
                    math(EXPR _st0 "${_st0} + 1")
                endif()
                if(_z MATCHES " cam=2 " AND _z MATCHES " g=20 ")
                    math(EXPR _sicht "${_sicht} + 1")
                endif()
                if(_z MATCHES " gr=1")
                    math(EXPR _griff "${_griff} + 1")
                endif()
                if(_z MATCHES "PL\\([-0-9]+,[-0-9]+,rot=[0-9]+,hp=([-0-9]+)\\)" AND NOT CMAKE_MATCH_1 EQUAL 100)
                    math(EXPR _hp "${_hp} + 1")
                endif()
            endforeach()
        else()
            fail("${_wer}: state.log fehlt")
        endif()
        if(_st0 EQUAL 0 AND _sicht GREATER_EQUAL 10)
            ok("${_wer}: keine nicht initialisierte Kopie (0 Bilder mit st=0); ${_sicht} Bilder in Schnitt 2 gehalten und initialisiert")
        else()
            fail("${_wer}: ${_st0} Bilder mit nicht initialisierter Kopie (st=0), ${_sicht} Bilder in Schnitt 2 gehalten (erwartet 0 / >= 10)")
        endif()
        if(_griff EQUAL 0 AND _hp EQUAL 0)
            ok("${_wer}: kein Griff, Spieler-HP 100 bis Bild 700")
        else()
            fail("${_wer}: ${_griff} Bilder gegriffen, ${_hp} Bilder mit HP != 100")
        endif()
        if(EXISTS "${_dir}/ankunft_oben.ppm")
            ok("${_wer}: Bild ${_dir}/ankunft_oben.ppm")
        else()
            fail("${_wer}: ankunft_oben.ppm fehlt")
        endif()
    endforeach()

else()
    message(FATAL_ERROR "r39_elzazombies: TEIL '${TEIL}' unbekannt")
endif()

# Messkopie wieder entfernen (Muster test_r39_heliport1171.cmake:93), sonst liegen nach jedem Lauf fuenf Kopien neben der
# exe, die der Nutzer startet. file(REMOVE) bricht bei einer noch gesperrten Datei nicht ab (gemessen 2026-10-19 an einer
# laufenden Kopie: cmake exit 0, Datei bleibt) - der Riegel kann dadurch nicht rot werden.
file(REMOVE "${_exe}")

if(_fehler)
    list(LENGTH _fehler _nf)
    message(FATAL_ERROR "${_tag}: ${_nf} Fehler")
endif()
message(STATUS "${_tag}: alles gruen")
