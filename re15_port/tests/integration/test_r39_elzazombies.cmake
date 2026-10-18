# test_r39_elzazombies.cmake — Runde 39 Spur Q "elzazombies": Elza, Zombie-Durchbrueche in der Panikphase an der ECHTEN
# exe (Dossier analysis/befunde_runde39/Q_elzazombies.md, Mechanik include/re15_elzazombies_r39.h). Aufruf:
#   cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE=<probe_r39_elzazombies_karte> -DRE15_AUSWERTER=<test_r39_elzazombies>
#         -DWORKDIR=<dir> -DTEIL=<kette|knall|treppe|ohne> -P test_r39_elzazombies.cmake
#
#   kette   Elza-Spielstand ROOM1041 (Intro durch: (3,125),(3,193); 1011 besucht: (3,116); 10E1/1071 nie betreten = alle
#           leben), CONTINUE, (9,97) bei Bild 200 (= Spur P beim 1041-Ruf). debug.log: Flanke -> Umzug (10E1 4, 1071 5) ->
#           Knall in 1041 -> ROOM1061 (+4 Gegner, Cut 2) -> Knall -> ROOM1031 (+5, Cut 7) -> Original-Se_on -> +3 Kriecher ->
#           Cut 12 -> ROOM1041 -> "Rueckkehr beendet, Steuerung frei"; je Spur-Zeile: jede Kopie >= 400 naeher an der Kamera;
#           Spieler-HP 100 in allen Schnitten; nach der Rueckkehr steht Elza an der gemerkten Stelle.
#   knall   derselbe Lauf zweimal mit RE15_AUDIO_CAP_SYNC, einmal RE15_R39Q_KNALL_STUMM: die Differenz = zwei hoerbare
#           Tuerknaelle (Auftakt 1041, Ende 1061) — test_r39_elzazombies auswerten_ton.
#   treppe  ECHTER WEG: Elza ROOM1041 an der Tuer zu 1061 mit (9,95) (Montage gelaufen), Aktion -> Tuer (Slot 2 @0x010D8)
#           -> ROOM1061 Etage 0 -> Nachspawn (Kopien an der Westseite, gehalten) -> Treppe A (Etage 0 -> 2) -> Treppe B
#           (2 -> 4) -> frei bei der Ankunft (25424,24568), kleinster Abstand >= 4000, alle Kopien an der Westseite
#           (x <= 21100); Bild der Ankunft (ankunft.ppm).
#   ohne    Leon ROOM1040 mit (9,97)-Flanke: keine Kette; Elza-Spielstand mit (9,97) schon gesetzt (Laden): keine Flanke.
#
# Parallel-/Linux-fest: exe-Kopie mit eigenem Praefix je TEIL, eigener Laufordner je Lauf, Umgebung nur ueber
# re15_start_spiel (spiel_lauf.cmake), keine .exe-/Backslash-Pfade.
cmake_minimum_required(VERSION 3.16)
foreach(_v RE15_PC_EXE RE15_KARTE RE15_AUSWERTER WORKDIR TEIL)
    if(NOT DEFINED ${_v})
        message(FATAL_ERROR "r39_elzazombies: ${_v} fehlt")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

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

# ------------------------------------------------------------------------------------------------------------------
if(TEIL STREQUAL "kette")
    # Ende im ZWEITEN 1041-Besuch: der erste verlaesst 1041 bei Bild ~306 (Tuer des Auftakts), Bild 400 erreicht nur die
    # Rueckkehr (Bildzaehler je Raum).
    lauf(kette "${_elza_1041}" 900 RE15_NOAUDIO=1 RE15_SET_FLAG_AT=9:97@200 "RE15_EXIT_AT=400#1041")
    set(_pos 0)
    foreach(_m
            "Flanke (9,97) 0->1 in ROOM1041"
            "Umzug: 10E1 lebend 4 -> 1061, 1071 lebend 5 -> 1031 Cut 7; (9,95)=1"
            "Folge #1 K ROOM1041"
            "PC loaded room1061"
            "S ROOM1061 T"
            "C ROOM1061 T"
            "K ROOM1061"
            "PC loaded room1031"
            "C ROOM1031 T"
            "K ROOM1031"
            "PC loaded room1041"
            "Rueckkehr beendet, Steuerung frei")
        finde("${_m}" ${_pos} _p)
        if(_p LESS 0)
            fail("in der Folge fehlt '${_m}' (nach Stelle ${_pos})")
            break()
        endif()
        set(_pos ${_p})
    endforeach()
    if(_pos GREATER 0)
        ok("Folge Flanke -> Umzug -> Knall 1041 -> 1061 (Spawn, Cut) -> Knall -> 1031 (Cut 7, Se_on) -> 1041 -> Steuerung frei")
    endif()
    foreach(_m "S ROOM1061 T105 4" "C ROOM1061 T105 2" "S ROOM1031 T281 5" "C ROOM1031 T281 7" "K ROOM1031 T457 2" "C ROOM1031 T489 12")
        string(FIND "${_dbg}" "${_m}" _p)
        if(_p LESS 0)
            # Takte koennen sich unter Last nicht verschieben (je Spielbild), aber der Text muss passen:
            string(REGEX REPLACE " T[0-9]+ " " T[0-9]+ " _rx "${_m}")
            string(REGEX MATCH "${_rx}" _mm "${_dbg}")
            if(NOT _mm)
                fail("Folge-Eintrag '${_m}' fehlt")
            endif()
        endif()
    endforeach()
    ok("1061: +4 Kopien, Cut 2; 1031: +5 Kopien Cut 7, Original-Knall, Cut 12")
    # Spur-Zeilen: jede Kopie naeher an der Kamera
    string(REGEX MATCHALL "ROOM10[36]1 Cut [0-9]+ Bild [0-9]+: Aktor [0-9]+ Bit [0-9]+ Abstand zur Kamera [0-9]+ -> [0-9]+" _sp "${_dbg}")
    set(_n 0)
    set(_nah 0)
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
            elseif(_raum STREQUAL "1031" AND _cut EQUAL 7 AND _bit GREATER_EQUAL 128 AND _bit LESS_EQUAL 132)
                set(_zaehlt 1)
            elseif(_raum STREQUAL "1031" AND _cut EQUAL 12 AND (_bit EQUAL 133 OR _bit EQUAL 144 OR _bit EQUAL 145))
                set(_zaehlt 1)
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
    if(_n EQUAL 12 AND _nah EQUAL 12)
        ok("alle 12 Kopien (4 in 1061 Cut 2, 5 in 1031 Cut 7, 3 Kriecher Cut 12) >= 400 naeher an der Kamera")
    else()
        fail("Kopien-Spuren: ${_nah} von ${_n} naeher (erwartet 12 von 12)")
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
        ok("Spieler-HP 100 in allen drei Schnitten")
    else()
        fail("Spieler-HP: ${_hp}")
    endif()
    finde("PC loaded room1041" 100 _pr)
    string(SUBSTRING "${_dbg}" ${_pr} -1 _rueck)
    if(_rueck MATCHES "\\[walk\\] F[0-9]+ cut=0 [^\n]*pl pos=\\(-21008,0,-13134\\) rot=2112")
        ok("Rueckkehr: Elza an der gemerkten Stelle (-21008,0,-13134) Gier 2112 Cut 0")
    else()
        fail("Rueckkehr-Stelle nicht gefunden")
    endif()

# ------------------------------------------------------------------------------------------------------------------
elseif(TEIL STREQUAL "knall")
    lauf(laut "${_elza_1041}" 900 RE15_AUDIO_CAP_SYNC=cap.raw RE15_SET_FLAG_AT=9:97@200 "RE15_EXIT_AT=530#1031")
    set(_laut "${_dir}/cap.raw")
    lauf(stumm "${_elza_1041}" 900 RE15_AUDIO_CAP_SYNC=cap.raw RE15_R39Q_KNALL_STUMM=1 RE15_SET_FLAG_AT=9:97@200
         "RE15_EXIT_AT=530#1031")
    set(_stumm "${_dir}/cap.raw")
    execute_process(COMMAND "${RE15_AUSWERTER}" auswerten_ton "${_laut}" "${_stumm}" TIMEOUT 120
                    RESULT_VARIABLE _rva OUTPUT_VARIABLE _outa)
    message(STATUS "${_outa}")
    if(_rva EQUAL 0)
        ok("zwei hoerbare Tuerknaelle (Differenz der Mitschnitte)")
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
elseif(TEIL STREQUAL "ohne")
    lauf(leon "1040;-21008;0;-13134;2112;0" 600 RE15_NOAUDIO=1 RE15_SET_FLAG_AT=9:97@200 "RE15_EXIT_AT=500#1040")
    string(FIND "${_dbg}" "[elzazombies-r39]" _p)
    string(FIND "${_dbg}" "PC loaded room" _pl)
    if(_p LESS 0 AND _pl LESS 0)
        ok("Leon ROOM1040: (9,97)-Flanke startet nichts (keine Zeile, kein Raumwechsel)")
    else()
        fail("Leon: Montage-Spur im Log (${_p}) oder Raumwechsel (${_pl})")
    endif()
    lauf(laden "${_elza_1041};9:97" 600 RE15_NOAUDIO=1 "RE15_EXIT_AT=500#1041")
    string(FIND "${_dbg}" "Flanke (9,97)" _p)
    string(FIND "${_dbg}" "PC loaded room" _pl)
    if(_p LESS 0 AND _pl LESS 0)
        ok("Elza mit (9,97) im Spielstand: Laden ist keine Flanke, keine Kette")
    else()
        fail("Laden mit (9,97): Flanke (${_p}) oder Raumwechsel (${_pl})")
    endif()
else()
    message(FATAL_ERROR "r39_elzazombies: TEIL '${TEIL}' unbekannt")
endif()

if(_fehler)
    list(LENGTH _fehler _nf)
    message(FATAL_ERROR "${_tag}: ${_nf} Fehler")
endif()
message(STATUS "${_tag}: alles gruen")
