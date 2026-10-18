# test_r39_elza.cmake — Runde 39 Spur H "elza": die drei Nutzer-Punkte an der ECHTEN exe (Dossier
# analysis/befunde_runde39/H_elza.md). Aufruf:
#   cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_FL=<probe_r37_feuerloescher_karte> -DRE15_KARTE_ELZA=<probe_r36_elza_karte>
#         -DWORKDIR=<dir> -DTEIL=<hand|karte|loescher> -P test_r39_elza.cmake
#
#   hand      Neues Spiel Elza (Titel + Figurenwahl) bis ROOM1031 sub13: ab dem Bild des Plc_motion(0,11) @0x029A4
#             (Plc_flg 0x0004) bis vor Plc_motion(0,0) @0x029BE (Plc_flg 0x0080) steht der Spieler je Tick auf Motion 11
#             (RE15_STATE_LOG, Kommandowort 4), nie auf 210/200 (die Eintritts-Latch); danach Motion 0 (Clip 0 rueckwaerts).
#   karte     Roys Tagebuch (Elza ROOM1011): die Kartenkette wird SOFORT nach dem Leser angefordert — der Hinweis-Schirm
#             beginnt, BEVOR sub07 hinter dem Message_on weiterlaeuft (kein Plc_motion @0x0E6A / Plc_ret @0x0E76 vor dem
#             Hinweis-Beginn); nach dem Hinweis laeuft der Faden bis zum Evt_end.
#   loescher  Elza ROOM1091 neben dem leeren Loescher (1880,-1300): Viereck mit Blick 1024 und 3072 startet KEINEN Aufstieg
#             (RE15_CLIMB_TRACE: kein sub=9), Blick 0 zeigt weiter msg 29 "It's an empty fire extinguisher."
#
# Parallel-/Linux-fest: exe-Kopie mit eigenem Praefix je TEIL, eigener Laufordner je Lauf, Umgebung nur ueber
# re15_start_spiel (spiel_lauf.cmake), keine .exe-/Backslash-Pfade.
cmake_minimum_required(VERSION 3.16)
foreach(_v RE15_PC_EXE RE15_KARTE_FL RE15_KARTE_ELZA WORKDIR TEIL)
    if(NOT DEFINED ${_v})
        message(FATAL_ERROR "r39_elza: ${_v} fehlt")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

set(_tag "r39_elza[${TEIL}]")
set(_basisdir "${WORKDIR}_${TEIL}")
file(MAKE_DIRECTORY "${_basisdir}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe "${_exe_dir}/re15_pc_r39elza_${TEIL}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

set(_fehler "")
macro(fail _text)
    list(APPEND _fehler "${_text}")
    message(STATUS "FAIL ${_tag}: ${_text}")
endmacro()
macro(ok _text)
    message(STATUS "ok   ${_tag}: ${_text}")
endmacro()

# Ein Lauf: Karte (optional) schreiben, exe starten, debug.log lesen (-> _dbg), Laufordner -> _dir.
function(lauf _name _kartentool _kartenargs _timeout)
    set(WORKDIR "${_basisdir}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log" "${WORKDIR}/state.txt" "${WORKDIR}/climb.txt")
    if(NOT "${_kartentool}" STREQUAL "")
        execute_process(COMMAND "${_kartentool}" "re15_card.mcr" ${_kartenargs}
                        WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
        if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
            message(FATAL_ERROR "${_tag}/${_name}: Kartenwerkzeug exit=${_rvk}\n${_outk}")
        endif()
    endif()
    re15_start_spiel(_rv ${_timeout} ${ARGN} "${_exe}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "${_tag}/${_name}: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _d)
    set(_dbg "${_d}" PARENT_SCOPE)
    set(_dir "${WORKDIR}" PARENT_SCOPE)
    message(STATUS "${_tag}/${_name}: exit=${_rv}, ${WORKDIR}")
endfunction()

# ------------------------------------------------------------------------------------------------------------------
if(TEIL STREQUAL "hand")
    # Neues Spiel Elza: Titel (Bestaetigung nach 1500 ms) + Figurenwahl (RE15_PSELECT_AUTO + _SWITCH = Elza), Montage
    # ROOM1241, Erzaehler, Selbsttuer, sub13 in ROOM1031; Ende bei Bild 560 des ROOM1031-Blocks.
    lauf(hand "" "" 900
         RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500
         RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_SCD_TRACE=1
         "RE15_STATE_LOG=state.txt" "RE15_EXIT_AT=560#1031")
    string(FIND "${_dbg}" "PC loaded room1031.rdt" _p)
    if(_p LESS 0)
        fail("ROOM1031 nicht erreicht")
    else()
        # Bild des Plc_motion(0,11)+Plc_flg(0x04) @0x029A4/@0x029A8 und des Plc_motion(0,0)+Plc_flg(0x80) @0x029BE/@0x029C2
        # im ROOM1031-Teil des Logs (nach dem letzten Raumwechsel).
        string(SUBSTRING "${_dbg}" ${_p} -1 _teil1031)
        string(REGEX MATCH "\\[scd F([0-9]+)\\] Plc_flg\\(subop=0, mask=0x0004\\) → slot=0" _m4 "${_teil1031}")
        set(_f_auf "${CMAKE_MATCH_1}")
        string(REGEX MATCH "\\[scd F([0-9]+)\\] Plc_flg\\(subop=0, mask=0x0080\\) → slot=0" _m8 "${_teil1031}")
        set(_f_ab "${CMAKE_MATCH_1}")
        if(NOT _m4 OR NOT _m8)
            fail("Plc_flg 0x0004 / 0x0080 im ROOM1031-Log nicht gefunden")
        else()
            ok("Plc_motion(0,11) bei F${_f_auf}, Plc_motion(0,0) rueckwaerts bei F${_f_ab}")
            # STATE_LOG: letzter Block (Zaehler springt beim Raumwechsel auf 0) = ROOM1031; je Tick " mo=<motion> ".
            file(STRINGS "${_dir}/state.txt" _zeilen)
            set(_block "")
            foreach(_z IN LISTS _zeilen)
                if(_z MATCHES "^F0 ")
                    set(_block "")
                endif()
                list(APPEND _block "${_z}")
            endforeach()
            set(_n11 0)
            set(_nfalsch 0)
            set(_n0 0)
            set(_beispiel "")
            math(EXPR _f_auf1 "${_f_auf} + 1")
            math(EXPR _f_ab1 "${_f_ab} - 1")
            math(EXPR _f_ab5 "${_f_ab} + 5")
            foreach(_z IN LISTS _block)
                if(_z MATCHES "^F([0-9]+) .* pst=([0-9]+) .* mo=([0-9]+) ")
                    set(_f "${CMAKE_MATCH_1}")
                    set(_pst "${CMAKE_MATCH_2}")
                    set(_mo "${CMAKE_MATCH_3}")
                    if(_f GREATER_EQUAL _f_auf1 AND _f LESS_EQUAL _f_ab1)
                        if(_mo EQUAL 11 AND _pst EQUAL 4)
                            math(EXPR _n11 "${_n11} + 1")
                        else()
                            math(EXPR _nfalsch "${_nfalsch} + 1")
                            if(_beispiel STREQUAL "")
                                set(_beispiel "${_z}")
                            endif()
                        endif()
                    elseif(_f GREATER_EQUAL _f_ab AND _f LESS_EQUAL _f_ab5)
                        if(_mo EQUAL 0 AND _pst EQUAL 4)
                            math(EXPR _n0 "${_n0} + 1")
                        endif()
                    endif()
                endif()
            endforeach()
            if(_n11 GREATER_EQUAL 30 AND _nfalsch EQUAL 0)
                ok("F${_f_auf1}..F${_f_ab1}: ${_n11} Ticks Motion 11 / Kommandowort 4, keiner auf 210/200 (Eintritts-Latch)")
            else()
                fail("F${_f_auf1}..F${_f_ab1}: ${_n11} Ticks Motion 11, ${_nfalsch} andere — z.B. '${_beispiel}'")
            endif()
            if(_n0 GREATER_EQUAL 5)
                ok("F${_f_ab}..F${_f_ab5}: Motion 0 (Clip 0 rueckwaerts = senken), ${_n0} Ticks")
            else()
                fail("nach Plc_motion(0,0): nur ${_n0} Ticks Motion 0")
            endif()
        endif()
    endif()

# ------------------------------------------------------------------------------------------------------------------
elseif(TEIL STREQUAL "karte")
    # Elza vor dem liegenden Roy (wie integration_r38_tagebuch_lesen): Viereck @60 untersuchen, RECHTS x6 blaettern,
    # Viereck schliesst, Viereck bestaetigt "filed".
    lauf(karte "${RE15_KARTE_ELZA}" "1011;-1900;6830;3072;4:0xF1;4:0xFA;3:0x74" 900
         RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
         RE15_MSG_LOG=1 RE15_SCD_TRACE=1
         "RE15_PRESS=square@60,right@200,right@230,right@260,right@290,right@320,right@350,square@380,square@480"
         "RE15_EXIT_AT=1100#1011")
    string(REGEX MATCH "\\[tagebuch\\] Leser zu: Bit \\(9,68\\)=1, Kartenkette sofort \\(Runde 39 Spur H\\): Eintrag 3 \\(ROOM10F1 Blatt 3 Rechteck 9\\) -> Folge 4 \\(ROOM1211 Blatt 1 Rechteck 3\\)" _zu "${_dbg}")
    string(REGEX MATCH "\\[hint\\] F([0-9]+) begin" _hb "${_dbg}")
    set(_f_hint "${CMAKE_MATCH_1}")
    string(REGEX MATCH "\\[tagebuch\\] Hinweis zu: Faden laeuft hinter dem Message_on weiter" _hz "${_dbg}")
    # sub07 hinter dem Message_on: Plc_motion @0x0E6A (op 0x3F), Plc_ret @0x0E76 (op 0x42), Evt_end @0x0E82 (op 0x01)
    string(REGEX MATCH "\\[scd\\] F([0-9]+) slot=[0-9]+ @0x0E6A op=0x3F" _pm "${_dbg}")
    set(_f_pm "${CMAKE_MATCH_1}")
    string(REGEX MATCH "\\[scd\\] F([0-9]+) slot=[0-9]+ @0x0E76 op=0x42" _pr "${_dbg}")
    set(_f_pr "${CMAKE_MATCH_1}")
    string(REGEX MATCH "\\[scd\\] F([0-9]+) slot=[0-9]+ @0x0E82 op=0x01" _ee "${_dbg}")
    set(_f_ee "${CMAKE_MATCH_1}")
    string(REGEX MATCH "\\[hint\\] F([0-9]+) schliessen" _hs "${_dbg}")
    set(_f_hs "${CMAKE_MATCH_1}")
    if(NOT _zu)
        fail("Kartenkette wurde nicht sofort nach dem Leser angefordert ('[tagebuch] Leser zu ... Kartenkette sofort' fehlt)")
    else()
        ok("Leser zu -> Kartenkette sofort angefordert")
    endif()
    if(NOT _hb OR NOT _pm OR NOT _pr OR NOT _ee OR NOT _hs OR NOT _hz)
        fail("Ablauf unvollstaendig: hint-begin='${_hb}' plc_motion='${_pm}' plc_ret='${_pr}' evt_end='${_ee}' schliessen='${_hs}' hinweis-zu='${_hz}'")
    else()
        if(_f_hint LESS _f_pm AND _f_hs LESS_EQUAL _f_pm AND _f_pm LESS _f_pr AND _f_pr LESS_EQUAL _f_ee)
            ok("Hinweis F${_f_hint}..F${_f_hs} VOR Plc_motion F${_f_pm} / Plc_ret F${_f_pr} / Evt_end F${_f_ee}: Karte kommt, bevor das Spiel zurueckgegeben wird")
        else()
            fail("Reihenfolge: hint F${_f_hint}, schliessen F${_f_hs}, Plc_motion F${_f_pm}, Plc_ret F${_f_pr}, Evt_end F${_f_ee}")
        endif()
    endif()
    string(FIND "${_dbg}" "[msgtext] room=1011 id=18" _p18)
    if(NOT _p18 LESS 0)
        fail("die Code-Nachricht msg 18 erschien")
    endif()

# ------------------------------------------------------------------------------------------------------------------
elseif(TEIL STREQUAL "loescher")
    # Spielstand neben dem leeren Loescher (Lauf C des Riegels r37_feuerloescher: Zombies 0xF8..0xFB erledigt).
    foreach(_rot 1024 3072 0)
        lauf("rot${_rot}" "${RE15_KARTE_FL}" "1091;1880;-1300;${_rot};y:-1800;cut:6;elza;flag:7:0xf8;flag:7:0xf9;flag:7:0xfa;flag:7:0xfb" 400
             RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
             RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=W1,A0.3,W3"
             RE15_MSG_LOG=1 "RE15_CLIMB_TRACE=climb.txt" "RE15_EXIT_AT=200")
        string(FIND "${_dbg}" "CONTINUE: resumed in room 1091" _pr)
        if(_pr LESS 0)
            fail("rot ${_rot}: Spielstand nicht geladen")
            continue()
        endif()
        string(FIND "${_dbg}" "[feuerloescher] ROOM1091: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300)" _pl)
        if(_pl LESS 0)
            fail("rot ${_rot}: Loescher nicht installiert")
        endif()
        if(NOT EXISTS "${_dir}/climb.txt")
            fail("rot ${_rot}: kein climb.txt")
            continue()
        endif()
        file(STRINGS "${_dir}/climb.txt" _kl REGEX " sub=[1-9]")
        list(LENGTH _kl _nkl)
        file(STRINGS "${_dir}/climb.txt" _pad REGEX " pad=8000 ")
        list(LENGTH _pad _npad)
        if(_npad LESS 1)
            fail("rot ${_rot}: Viereck (pad=8000) kam im Trace nicht an")
        endif()
        if(_nkl EQUAL 0)
            ok("rot ${_rot}: kein Aufstieg (sub bleibt 0 in allen ${_npad} Viereck-Bildern)")
        else()
            list(GET _kl 0 _erste)
            fail("rot ${_rot}: Aufstieg gestartet — '${_erste}'")
        endif()
        if(_rot EQUAL 0)
            string(FIND "${_dbg}" "[msgtext] room=1091 id=29" _p29)
            if(_p29 LESS 0)
                fail("rot 0: Untersuchen-Nachricht msg 29 fehlt")
            else()
                ok("rot 0: msg 29 'It's an empty fire extinguisher.' (Untersuchen-Platz unveraendert)")
            endif()
        endif()
    endforeach()
else()
    message(FATAL_ERROR "${_tag}: TEIL '${TEIL}' unbekannt (hand|karte|loescher)")
endif()

file(REMOVE "${_exe}")
if(_fehler)
    message(FATAL_ERROR "${_tag}: ${_fehler}")
endif()
message(STATUS "${_tag}: OK")
