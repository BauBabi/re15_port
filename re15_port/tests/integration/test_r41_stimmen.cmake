# =============================================================================
# RUNDE 41 STIMMEN MIT DER ECHTEN EXE: Port-Option "VOICE  ON|OFF" (include/re15_stimme_option.h).
#
# TEIL=menue
#   (1) Abzuege (RE15_CONFIG_SHOT): Tab 0 mit einer Datei ohne voice-Zeile -> Pruefzeile "VOICE ON"; Tab 10 (TOP, Cursor
#       auf VOICE) mit "voice=off" -> "Schirm 10 Cursor 5, VOICE OFF". Die VOICE-Zeile (x 226..305, y 225..238) ist
#       zwischen beiden verschieden. Tab 10 mit ON gegen Tab 10 mit OFF: die Beschriftung (x 231..305, y 227..238) ist
#       verschieden, der ganze Schirm darueber (y 0..224) und die AI/ULTIMATE-Felder (x 14..218) sind pixelgleich.
#   (2) Echte Tastenkette im OPTIONS-Schirm: Start TOP Cursor 0, RIGHT x5 (CONFIG SOUND EXIT AI ULTIMATE VOICE),
#       Kreis/Quadrat -> "voice=off" sofort in re15_options.cfg, LEFT x3 auf EXIT, Kreis beendet. Zweiter Lauf von dieser
#       Datei: Start auf VOICE, Kreis -> ON, die Datei ist danach Byte fuer Byte die Ausgangsdatei.
# TEIL=szene (deterministischer Ton RE15_AUDIO_CAP_SYNC, neues Spiel: Vorspann ROOM1240 + Helipad ROOM1170 bis Bild 3000,
#   beide Raeume mit Aufnahmen in synchro/):
#   (3) ON: >= 10 Aufnahmen geladen. OFF ("voice=off"): keine Aufnahme geladen, jede Zeile meldet "VOICE OFF ... stumm",
#       stimme.log (Message_on je Zeile: Raum, Nachricht, Kanal, Wartebilder, Bild) ist zeilengleich mit ON und laeuft=0
#       ueberall -> jede Zeile oeffnet im selben Bild wie mit Aufnahme, nichts haengt, nichts faellt weg, die Szene endet.
#       Mitschnitt ON gegen OFF verschieden (die Stimmen), OFF nicht stumm (Musik und Geraeusche bleiben).
#   (4) Umschalten mitten in der ersten Aufnahme (Messhaken RE15_VOICE_AUS_NACH=30, derselbe Schalter wie im Menue):
#       die Aufnahme endet ("laufende Aufnahme ROOM1240 msg 0 endet"), im Bild danach laeuft keine Stimme und die
#       Nachricht 0 ist noch offen; danach keine weitere Aufnahme, stimme.log zeilengleich mit OFF; der Mitschnitt ist ab
#       dem Bild nach dem Umschalten bitgleich mit OFF und bis zum Umschalt-Bild bitgleich mit ON (die Aufnahme lief
#       vorher mindestens 30 Bilder).
#   (5) Irons' Tod ROOM1150 (Spielstand probe_r35_cut1150_karte + CONTINUE in 1130, Tuer; Weg integration_r39_stimmen_1150),
#       deterministischer Ton: dort sind Aufnahmen LAENGER als ihre Bilder, ON wartet an mindestens einer Zeile auf das
#       Ende der Aufnahme. OFF: keine Aufnahme, an keiner Zeile gewartet, jede Zeile oeffnet genau um die bis dahin bei ON
#       gewarteten Bilder frueher (Bild_ON - Bild_OFF = Summe der Wartebilder bis zu dieser Zeile) = Textdauer, als gaebe
#       es keine Aufnahme; die Szene laeuft bis Bild 1250.
#       Runde 42 namen Stufe 5 R2: ROOM1150 msg 23 und 27 haben zwei Seiten (tools/r42_namen/porttexte_s5r2.py). Ohne Stimme
#       wartet die naechste Zeile (msg 24, msg 28), bis die zweiseitige Zeile von selbst endet (msg_common.c
#       re15_msg_seiten_halten; Original FUN_80027e68 @0x80027e74-8c bricht eine laufende Nachricht nie ab). Dort muss OFF
#       warten, sonst nirgends; Bild_ON - Bild_OFF = Summe der ON-Wartebilder - Summe der OFF-Wartebilder.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r41_optionen> -DRE15_TOOL2=<test_r41_stimmen>
#               -DRE15_KARTE_1150=<probe_r35_cut1150_karte> -DWORKDIR=<dir> -DTEIL=menue|szene -P test_r41_stimmen.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
foreach(_v RE15_PC_EXE RE15_TOOL RE15_TOOL2)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "stimmen_r41: ${_v} fehlt: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "stimmen_r41: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Keine RE15_*-Variablen des Aufrufers erben.
execute_process(COMMAND "${CMAKE_COMMAND}" -E environment OUTPUT_VARIABLE _envdump)
string(REGEX MATCHALL "(^|\n)RE15_[A-Za-z0-9_]*=" _geerbt "${_envdump}")
foreach(_g ${_geerbt})
    string(REGEX REPLACE "^\n?(RE15_[A-Za-z0-9_]*)=$" "\\1" _n "${_g}")
    unset(ENV{${_n}})
endforeach()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
set(_exe "${_exe_dir}/re15_pc_r41stimme_${TEIL}_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")
set(_fehler "")

function(log_lesen _dir _out)
    if(EXISTS "${_dir}/debug.log")
        file(READ "${_dir}/debug.log" _t)
    else()
        set(_t "")
    endif()
    set(${_out} "${_t}" PARENT_SCOPE)
endfunction()

function(bmp_pruefen _art _a _b _x0 _y0 _x1 _y1 _out)
    execute_process(COMMAND "${RE15_TOOL}" "bmp_${_art}" "${_a}" "${_b}" ${_x0} ${_y0} ${_x1} ${_y1}
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _o)
    string(STRIP "${_o}" _o)
    message(STATUS "stimmen_r41[bmp_${_art} ${_x0},${_y0}..${_x1},${_y1}]: ${_o} (rv=${_rv})")
    set(${_out} "${_rv}" PARENT_SCOPE)
endfunction()

if(TEIL STREQUAL "menue")
    # ---- (1) Abzuege -----------------------------------------------------------------------------------------------
    set(WORKDIR "${_basis}_shots")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(WRITE "${WORKDIR}/opt.cfg" "volume=50\nsound=stereo\n")
    file(WRITE "${WORKDIR}/opt_off.cfg" "volume=50\nsound=stereo\nvoice=off\n")
    re15_start_spiel(_rv 120 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 "RE15_OPTIONS_FILE=${WORKDIR}/opt.cfg"
        RE15_CONFIG_TEST=1 RE15_CONFIG_SHOT=top.bmp RE15_CONFIG_TAB=0 "${_exe}")
    log_lesen("${WORKDIR}" _log_top)
    file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug_top.log")
    re15_start_spiel(_rv 120 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 "RE15_OPTIONS_FILE=${WORKDIR}/opt_off.cfg"
        RE15_CONFIG_TEST=1 RE15_CONFIG_SHOT=voice_off.bmp RE15_CONFIG_TAB=10 "${_exe}")
    log_lesen("${WORKDIR}" _log_off)
    file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug_off.log")
    re15_start_spiel(_rv 120 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 "RE15_OPTIONS_FILE=${WORKDIR}/opt.cfg"
        RE15_CONFIG_TEST=1 RE15_CONFIG_SHOT=voice_on.bmp RE15_CONFIG_TAB=10 "${_exe}")
    log_lesen("${WORKDIR}" _log_on)
    file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug_on.log")
    string(FIND "${_log_top}" "ULTIMATE 1.5 OFF, Schirm 0 Cursor 0, VOICE ON" _p_top)
    string(FIND "${_log_off}" "Schirm 10 Cursor 5, VOICE OFF" _p_off)
    string(FIND "${_log_on}" "Schirm 10 Cursor 5, VOICE ON" _p_on)
    message(STATUS "stimmen_r41[1]: Pruefzeilen TOP ${_p_top}, OFF ${_p_off}, ON ${_p_on}")
    if(_p_top LESS 0 OR _p_off LESS 0 OR _p_on LESS 0)
        set(_fehler "${_fehler}\n[1] VOICE-Pruefzeile fehlt oder VOICE nicht aus re15_options.cfg gelesen (TOP ${_p_top}, OFF ${_p_off}, ON ${_p_on})")
    endif()
    if(EXISTS "${WORKDIR}/top.bmp" AND EXISTS "${WORKDIR}/voice_off.bmp" AND EXISTS "${WORKDIR}/voice_on.bmp")
        bmp_pruefen(anders "${WORKDIR}/top.bmp" "${WORKDIR}/voice_off.bmp" 226 225 305 238 _a)
        if(NOT _a EQUAL 0)
            set(_fehler "${_fehler}\n[1] VOICE-Zeile (226..305, 225..238) zwischen Tab 0/ON und Tab 10/OFF gleich - keine Zeile")
        endif()
        bmp_pruefen(anders "${WORKDIR}/voice_on.bmp" "${WORKDIR}/voice_off.bmp" 231 227 305 238 _a2)
        if(NOT _a2 EQUAL 0)
            set(_fehler "${_fehler}\n[1] Beschriftung ON und OFF (231..305, 227..238) gleich")
        endif()
        bmp_pruefen(gleich "${WORKDIR}/voice_on.bmp" "${WORKDIR}/voice_off.bmp" 0 0 319 224 _g)
        if(NOT _g EQUAL 0)
            set(_fehler "${_fehler}\n[1] Schirm oberhalb des Streifens (y 0..224) haengt an VOICE")
        endif()
        bmp_pruefen(gleich "${WORKDIR}/voice_on.bmp" "${WORKDIR}/voice_off.bmp" 14 225 218 238 _g2)
        if(NOT _g2 EQUAL 0)
            set(_fehler "${_fehler}\n[1] AI-/ULTIMATE-Felder (14..218) zwischen VOICE ON und OFF verschieden")
        endif()
    else()
        set(_fehler "${_fehler}\n[1] Abzug fehlt (top.bmp/voice_off.bmp/voice_on.bmp)")
    endif()

    # ---- (2) Tastenkette -----------------------------------------------------------------------------------------
    set(WORKDIR "${_basis}_tasten")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_ausgang "volume=50\nsound=stereo\n")
    file(WRITE "${WORKDIR}/opt.cfg" "${_ausgang}")
    re15_start_spiel(_rv 180 RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1
        "RE15_OPTIONS_FILE=${WORKDIR}/opt.cfg" RE15_CONFIG_TEST=1 RE15_CONFIG_TAB=0
        "RE15_INPUT_SCRIPT=W5,R0.1,W0.4,R0.1,W0.4,R0.1,W0.4,R0.1,W0.4,R0.1,W0.5,A0.1,W0.5,L0.1,W0.4,L0.1,W0.4,L0.1,W0.5,A0.1,W3"
        RE15_INPUT_SCRIPT_START=0 "${_exe}")
    log_lesen("${WORKDIR}" _log_t1)
    file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug_t1.log")
    file(READ "${WORKDIR}/opt.cfg" _cfg1)
    string(FIND "${_log_t1}" "[optionen] VOICE AUS" _pt1)
    message(STATUS "stimmen_r41[2]: nach Kreis auf VOICE: Datei '${_cfg1}', Logzeile ${_pt1}")
    if(NOT _cfg1 STREQUAL "volume=50\nsound=stereo\nvoice=off\n" OR _pt1 LESS 0)
        set(_fehler "${_fehler}\n[2] RIGHT x5 + Kreis schreibt nicht 'voice=off' (Datei '${_cfg1}', Logzeile ${_pt1})")
    endif()
    re15_start_spiel(_rv 180 RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1
        "RE15_OPTIONS_FILE=${WORKDIR}/opt.cfg" RE15_CONFIG_TEST=1 RE15_CONFIG_TAB=10
        "RE15_INPUT_SCRIPT=W5,A0.1,W0.5,L0.1,W0.4,L0.1,W0.4,L0.1,W0.5,A0.1,W3" RE15_INPUT_SCRIPT_START=0 "${_exe}")
    log_lesen("${WORKDIR}" _log_t2)
    file(READ "${WORKDIR}/opt.cfg" _cfg2)
    string(FIND "${_log_t2}" "[optionen] VOICE AN" _pt2)
    message(STATUS "stimmen_r41[2]: zurueck auf ON: Datei '${_cfg2}', Logzeile ${_pt2}")
    if(NOT _cfg2 STREQUAL "${_ausgang}" OR _pt2 LESS 0)
        set(_fehler "${_fehler}\n[2] zurueck auf ON: Datei nicht bitgleich mit der Ausgangsdatei ('${_cfg2}', Logzeile ${_pt2})")
    endif()

elseif(TEIL STREQUAL "szene")
    # Ein Lauf: neues Spiel (Titel-Haken), Vorspann 1240, Helipad 1170 bis Bild <ende>. Ergebnis: debug.log, stimme.log,
    # cap.raw im Ordner _basis_<name>.
    function(szene_lauf _name _cfg _ende)
        set(WORKDIR "${_basis}_${_name}")
        file(MAKE_DIRECTORY "${WORKDIR}")
        file(WRITE "${WORKDIR}/opt.cfg" "${_cfg}")
        re15_start_spiel(_rv 600 RE15_NO_INTRO=1 RE15_FPS=240 RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=60
            "RE15_EXIT_AT=${_ende}#1170" RE15_STIMME_LOG=1 "RE15_OPTIONS_FILE=${WORKDIR}/opt.cfg" ${ARGN} "${_exe}")
        log_lesen("${WORKDIR}" _l)
        string(FIND "${_l}" "EXIT_AT: Bild ${_ende} in Raum 1170" _pe)
        if(_pe LESS 0)
            message(FATAL_ERROR "stimmen_r41[${_name}]: Szene nicht bis ROOM1170 Bild ${_ende} gelaufen (exit=${_rv}, ${WORKDIR})")
        endif()
        set(_s "")
        if(EXISTS "${WORKDIR}/stimme.log")
            file(READ "${WORKDIR}/stimme.log" _s)
        endif()
        string(REGEX MATCHALL "clip loaded" _cl "${_l}")
        list(LENGTH _cl _ncl)
        string(REGEX MATCHALL "VOICE OFF: ROOM[0-9A-F]+ msg [0-9]+ bleibt stumm" _st "${_l}")
        list(LENGTH _st _nst)
        set(_log_${_name} "${_l}" PARENT_SCOPE)
        set(_stimme_${_name} "${_s}" PARENT_SCOPE)
        set(_clips_${_name} ${_ncl} PARENT_SCOPE)
        set(_stumm_${_name} ${_nst} PARENT_SCOPE)
        message(STATUS "stimmen_r41[${_name}]: ${_ncl} Aufnahmen geladen, ${_nst} Zeilen stumm")
    endfunction()
    function(cap_vergleich _a _b _out)
        execute_process(COMMAND "${RE15_TOOL2}" cap_diff "${_basis}_${_a}/cap.raw" "${_basis}_${_b}/cap.raw"
                        OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
        string(STRIP "${_o}" _o)
        message(STATUS "stimmen_r41[cap ${_a}/${_b}]: ${_o}")
        set(${_out} "${_o}" PARENT_SCOPE)
    endfunction()
    set(_cfg_on "volume=50\nsound=stereo\n")
    set(_cfg_off "volume=50\nsound=stereo\nvoice=off\n")

    # ---- (3) ON gegen OFF --------------------------------------------------------------------------------------------
    szene_lauf(on "${_cfg_on}" 3000 RE15_AUDIO_CAP_SYNC=cap.raw)
    szene_lauf(off "${_cfg_off}" 3000 RE15_AUDIO_CAP_SYNC=cap.raw)
    if(_clips_on LESS 10)
        set(_fehler "${_fehler}\n[3] ON: nur ${_clips_on} Aufnahmen geladen - der Lauf misst keine Szene mit Aufnahme")
    endif()
    if(NOT _clips_off EQUAL 0 OR _stumm_off LESS 10)
        set(_fehler "${_fehler}\n[3] OFF: ${_clips_off} Aufnahmen geladen (soll 0), ${_stumm_off} Zeilen stumm (soll >= 10)")
    endif()
    string(REGEX MATCHALL "\n" _nz "${_stimme_off}")
    list(LENGTH _nz _nzeilen)
    if(_nzeilen LESS 10 OR NOT _stimme_on STREQUAL _stimme_off)
        set(_fehler "${_fehler}\n[3] stimme.log OFF (${_nzeilen} Zeilen) nicht zeilengleich mit ON:\nON:\n${_stimme_on}\nOFF:\n${_stimme_off}")
    endif()
    string(FIND "${_stimme_off}" "laeuft=1" _l1)
    if(NOT _l1 LESS 0)
        set(_fehler "${_fehler}\n[3] OFF: Sprachkanal lief an einer Zeile (laeuft=1)")
    endif()
    cap_vergleich(on off _c)
    string(REGEX MATCH "abweichend=([0-9]+)" _m "${_c}")
    set(_abw ${CMAKE_MATCH_1})
    string(REGEX MATCH "energie_b=([0-9]+)" _m "${_c}")
    set(_eoff ${CMAKE_MATCH_1})
    if(NOT _abw OR _abw LESS 1 OR NOT _eoff OR _eoff LESS 1)
        set(_fehler "${_fehler}\n[3] Mitschnitt: ON und OFF gleich (abweichend '${_abw}') oder OFF stumm (Energie '${_eoff}')")
    endif()

    # ---- (4) Umschalten waehrend der ersten Aufnahme -----------------------------------------------------------------
    szene_lauf(um "${_cfg_on}" 3000 RE15_AUDIO_CAP_SYNC=cap.raw RE15_VOICE_AUS_NACH=30)
    string(REGEX MATCH "VOICE OFF nach 30 Bildern laufender Aufnahme \\(Rest ([0-9]+) Bilder, Nachricht 0 offen 1, capTick ([0-9]+)\\)" _m "${_log_um}")
    set(_rest ${CMAKE_MATCH_1})
    set(_tick ${CMAKE_MATCH_2})
    if(NOT _m OR _rest LESS 1)
        set(_fehler "${_fehler}\n[4] kein Umschalten mitten in der Aufnahme mit offener Nachricht 0 (Rest '${_rest}')")
    endif()
    string(FIND "${_log_um}" "VOICE OFF: laufende Aufnahme ROOM1240 msg 0 endet" _pend)
    string(FIND "${_log_um}" "Bild 1 nach dem Umschalten - Stimme laeuft 0, Nachricht 0 offen 1" _pb1)
    if(_pend LESS 0 OR _pb1 LESS 0)
        set(_fehler "${_fehler}\n[4] Aufnahme endet nicht (${_pend}) oder im Bild danach laeuft sie noch / Untertitel zu (${_pb1})")
    endif()
    if(NOT _clips_um EQUAL 1 OR _stumm_um LESS 10)
        set(_fehler "${_fehler}\n[4] nach dem Umschalten: ${_clips_um} Aufnahmen geladen (soll 1), ${_stumm_um} Zeilen stumm")
    endif()
    if(NOT _stimme_um STREQUAL _stimme_off)
        set(_fehler "${_fehler}\n[4] stimme.log nach dem Umschalten nicht zeilengleich mit OFF:\n${_stimme_um}")
    endif()
    if(_tick)
        cap_vergleich(um off _c1)
        string(REGEX MATCH "letzte_abweichung=(-?[0-9]+)" _m "${_c1}")
        set(_letzte ${CMAKE_MATCH_1})
        cap_vergleich(um on _c2)
        string(REGEX MATCH "erste_abweichung=(-?[0-9]+)" _m "${_c2}")
        set(_erste ${CMAKE_MATCH_1})
        string(REGEX MATCH "erste_abweichung=(-?[0-9]+)" _m "${_c1}")
        set(_start ${CMAKE_MATCH_1})
        message(STATUS "stimmen_r41[4]: Umschalten capTick ${_tick}; UM/OFF verschieden ${_start}..${_letzte}; UM/ON erste Abweichung ${_erste} (= Umschalt-Bild)")
        if(NOT _letzte EQUAL _tick)
            set(_fehler "${_fehler}\n[4] Ton nach dem Umschalten nicht bitgleich mit OFF (letzte Abweichung ${_letzte}, Umschalt-Bild ${_tick})")
        endif()
        math(EXPR _min_start "${_tick} - 30")
        if(_start LESS 0 OR _start GREATER ${_min_start} OR NOT _erste EQUAL _tick)
            set(_fehler "${_fehler}\n[4] Mitschnitt: Aufnahme beginnt nicht vor dem Umschalten (UM/OFF ab ${_start}) oder UM weicht nicht genau ab dem Umschalt-Bild von ON ab (ab ${_erste})")
        endif()
    endif()

    # ---- (5) Irons' Tod ROOM1150: Aufnahmen laenger als ihre Bilder ------------------------------------------------
    if(NOT RE15_KARTE_1150 OR NOT EXISTS "${RE15_KARTE_1150}")
        message(FATAL_ERROR "stimmen_r41: RE15_KARTE_1150 fehlt: '${RE15_KARTE_1150}'")
    endif()
    function(irons_lauf _name _cfg)
        set(WORKDIR "${_basis}_${_name}")
        file(MAKE_DIRECTORY "${WORKDIR}")
        file(WRITE "${WORKDIR}/opt.cfg" "${_cfg}")
        execute_process(COMMAND "${RE15_KARTE_1150}" re15_card.mcr 1130 nach10f0 ersteszene
                        WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rk OUTPUT_QUIET)
        if(NOT _rk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
            message(FATAL_ERROR "stimmen_r41[${_name}]: Kartenwerkzeug exit=${_rk}")
        endif()
        re15_start_spiel(_rv 600 RE15_NO_INTRO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_AUDIO_CAP_SYNC=cap.raw
            RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_INPUT_SCRIPT_BASIS=spiel
            RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=W1,A0.2,W200" "RE15_EXIT_AT=1250#1150" RE15_STIMME_LOG=1
            "RE15_OPTIONS_FILE=${WORKDIR}/opt.cfg" "${_exe}")
        log_lesen("${WORKDIR}" _l)
        string(FIND "${_l}" "EXIT_AT: Bild 1250 in Raum 1150" _pe)
        if(_pe LESS 0)
            message(FATAL_ERROR "stimmen_r41[${_name}]: Szene nicht bis ROOM1150 Bild 1250 gelaufen (exit=${_rv}, ${WORKDIR})")
        endif()
        file(STRINGS "${WORKDIR}/stimme.log" _z REGEX "^raum=1150 ")
        string(REGEX MATCHALL "clip loaded" _cl "${_l}")
        list(LENGTH _cl _ncl)
        set(_zeilen_${_name} "${_z}" PARENT_SCOPE)
        set(_clips_${_name} ${_ncl} PARENT_SCOPE)
    endfunction()
    irons_lauf(irons_on "${_cfg_on}")
    irons_lauf(irons_off "${_cfg_off}")
    list(LENGTH _zeilen_irons_on _n_on)
    list(LENGTH _zeilen_irons_off _n_off)
    message(STATUS "stimmen_r41[5]: ROOM1150 Zeilen ON ${_n_on} (${_clips_irons_on} Aufnahmen), OFF ${_n_off} (${_clips_irons_off} Aufnahmen)")
    if(_n_on LESS 6 OR NOT _n_on EQUAL _n_off)
        set(_fehler "${_fehler}\n[5] ROOM1150: Zeilenzahl ON ${_n_on} / OFF ${_n_off} (soll gleich, >= 6)")
    elseif(_clips_irons_on LESS 6 OR NOT _clips_irons_off EQUAL 0)
        set(_fehler "${_fehler}\n[5] ROOM1150: Aufnahmen ON ${_clips_irons_on} (soll >= 6), OFF ${_clips_irons_off} (soll 0)")
    else()
        set(_summe 0)
        set(_gewartet_on 0)
        set(_halt_off 24 28)   # Runde 42 namen Stufe 5 R2: die Zeilen hinter den zweiseitigen msg 23/27 (s. Kopf (5))
        math(EXPR _letzte "${_n_on} - 1")
        foreach(_i RANGE 0 ${_letzte})
            list(GET _zeilen_irons_on ${_i} _zon)
            list(GET _zeilen_irons_off ${_i} _zoff)
            string(REGEX MATCH "nachricht=([0-9]+)  laeuft=([0-9]+) rest=[0-9]+  ([0-9]+) gewartet  bild=([0-9]+)" _m "${_zon}")
            set(_mon ${CMAKE_MATCH_1})
            set(_won ${CMAKE_MATCH_3})
            set(_bon ${CMAKE_MATCH_4})
            string(REGEX MATCH "nachricht=([0-9]+)  laeuft=([0-9]+) rest=[0-9]+  ([0-9]+) gewartet  bild=([0-9]+)" _m "${_zoff}")
            set(_moff ${CMAKE_MATCH_1})
            set(_loff ${CMAKE_MATCH_2})
            set(_woff ${CMAKE_MATCH_3})
            set(_boff ${CMAKE_MATCH_4})
            if("${_won}" STREQUAL "" OR "${_boff}" STREQUAL "")
                set(_fehler "${_fehler}\n[5] stimme.log-Zeile unlesbar: ON '${_zon}' / OFF '${_zoff}'")
                break()
            endif()
            math(EXPR _summe "${_summe} + ${_won} - ${_woff}")
            # Runde 42 namen Stufe 5 R2: OFF wartet nur (und muss) hinter den zweiseitigen msg 23/27 (s. Kopf (5))
            list(FIND _halt_off "${_mon}" _darf_halten)
            set(_woff_ok 0)
            if(_darf_halten LESS 0 AND _woff EQUAL 0)
                set(_woff_ok 1)
            elseif(_darf_halten GREATER -1 AND _woff GREATER 0)
                set(_woff_ok 1)
            endif()
            if(_won GREATER 0)
                math(EXPR _gewartet_on "${_gewartet_on} + 1")
            endif()
            math(EXPR _diff "${_bon} - ${_boff}")
            message(STATUS "stimmen_r41[5]: msg ${_mon}: ON Bild ${_bon} (${_won} gewartet), OFF Bild ${_boff} (${_woff} gewartet, Kanal ${_loff}), Abstand ${_diff} = Summe ${_summe}")
            if(NOT _mon EQUAL _moff OR NOT _woff_ok OR NOT _loff EQUAL 0 OR NOT _diff EQUAL _summe)
                set(_fehler "${_fehler}\n[5] ROOM1150 msg ${_mon}/${_moff}: OFF Bild ${_boff} (gewartet ${_woff}, Kanal ${_loff}), ON Bild ${_bon} - Abstand ${_diff} statt Summe der Wartebilder ${_summe}")
            endif()
        endforeach()
        if(_gewartet_on LESS 1)
            set(_fehler "${_fehler}\n[5] ROOM1150 ON wartet an keiner Zeile - der Lauf misst das Warten nicht")
        endif()
    endif()
else()
    message(FATAL_ERROR "stimmen_r41: TEIL '${TEIL}' unbekannt (menue|szene)")
endif()

file(REMOVE "${_exe}")
if(_fehler)
    message(FATAL_ERROR "stimmen_r41[${TEIL}]: ROT${_fehler}")
endif()
message(STATUS "stimmen_r41[${TEIL}]: GRUEN")
