cmake_minimum_required(VERSION 3.16)
# =============================================================================
# Runde 38 Spur J "rekorder" MIT DER ECHTEN EXE (analysis/befunde_runde38/J_rekorder.md).
#
# Nutzer: "Nutze beim Rekorder Rätsel beim reinsprechen den Sound Irons_Rekorder_Sound.wav aus dem synchro Ordner, und
# nachdem er fertig abgespielt ist, machst du den Piep Sound für Erfolgreiche Eingabe und den Tür öffnen Sound. Das
# zusammen mit der erfolgreich message auf den Panel."
#
#   L  Leon ROOM1100 ECHTER WEG (Panel per RE15_FIRE_AOT=2 -> sub07, msg 1, "Ja", Cursor ON/OFF -> SYSTEM READY/SAY
#      HELLO, Cursor Mikrofon -> sub02), Nachrichten langsam. MUSS: Aufnahme im Takt von Message_on 4 (rk.log, Lader-Zeile),
#      im Mitschnitt gefunden (Korrelation >= 0.8, 79..80 Ticks); Piep (auf.log) im Takt des ersten Cut_chg(11) ACCEPTED,
#      Aufschliessen +15, nur diese zwei Toene, msg 2 ohne zweite Folge; Mitschnitt: Ende der Aufnahme VOR dem Key-On des
#      Pieps, Piep mit der Lautstaerke des Mischers, Key-On auf einem Tick-Anfang und EINEN Tick frueher gemischt als die
#      Aufnahme (s. j_mitschnitt); Bild Piep-Takt+1 zeigt ACCEPTED (Cut 11), Bild des Piep-Takts noch leer (Cut 12) - die
#      Anzeige wechselt im selben Bild, in dem der Piep zu hoeren ist; Tuer-Platz 1 frei.
#   S  Leon schnell (msg 4 und Wegwerf-Abfrage so schnell wie moeglich): sub02 WARTET an @0x00C96, bis die Aufnahme zu
#      Ende ist ("frei" ein Bild nach "stimme zu Ende"), Piep 5 Bilder danach; Mitschnitt: Ende vor dem Piep.
#   E  Elza ROOM1101 (RE15_PSELECT_AUTO_SWITCH): dieselbe Datei aus synchro/STAGE1/room1100, dieselbe Folge.
#   T  Tuer: Panel per RE15_FORCE_EVENT=7 (= Ereignis des Panel-Platzes 2, sub00 @0x00B14 `.. 18 07`), danach Tuer-Platz 1
#      per RE15_FIRE_AOT=1 -> ROOM1110 wird geladen.
# Ton ohne Geraet: RE15_AUDIO_CAP_SYNC. Auswertung: probe_r38_rekorder cap / bild.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PROBE=<probe> -DRE15_WAV=<wav> -DRE15_BSS=<ROOM110.BSS> -DWORKDIR=<dir>
#               -P test_r38_rekorder.cmake
# =============================================================================
foreach(_v RE15_PC_EXE RE15_PROBE RE15_WAV RE15_BSS)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r38_rekorder: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "r38_rekorder: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r37_codepanel.cmake)
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r38j_haken_*.exe" "${_exe_dir}/re15_pc_r38j_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe_kopie "${_exe_dir}/re15_pc_r38j_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(j_lauf _name _endbild)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 500
        RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_AUFSCHLIESS_LOG=auf.log RE15_REKORDER_LOG=rk.log
        RE15_CUT_SYNC_LOG=cut.log RE15_MSG_LOG=1
        ${ARGN}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r38_rekorder[${_name}]: debug.log fehlt (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(FIND "${_dbg}" "EXIT_AT: Bild ${_endbild}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r38_rekorder[${_name}]: Endbild ${_endbild} nicht erreicht (exit=${_rv}, ${WORKDIR})")
    endif()
    foreach(_f auf rk cut)
        set(_t "")
        if(EXISTS "${WORKDIR}/${_f}.log")
            file(READ "${WORKDIR}/${_f}.log" _t)
        endif()
        set(_${_f} "${_t}" PARENT_SCOPE)
    endforeach()
    set(_dbg "${_dbg}" PARENT_SCOPE)
    set(_wd "${WORKDIR}" PARENT_SCOPE)
endfunction()

function(j_muss _name _text _muster)
    string(REGEX MATCH "${_muster}" _m "${_text}")
    if(NOT _m)
        message(FATAL_ERROR "r38_rekorder[${_name}]: erwartet /${_muster}/ - fehlt")
    endif()
endfunction()
function(j_nicht _name _text _muster)
    string(REGEX MATCH "${_muster}" _m "${_text}")
    if(_m)
        message(FATAL_ERROR "r38_rekorder[${_name}]: /${_muster}/ darf nicht vorkommen: '${_m}'")
    endif()
endfunction()

# Folge aus den Logs: _s (Anforderung der Aufnahme), _p (Piep), Pruefungen der Reihenfolge im Skript-Takt.
function(j_folge _name _raum)
    string(REGEX MATCH "F([0-9]+) raum=${_raum} stimme an: sub02 @0x00C94" _m "${_rk}")
    if(NOT _m)
        message(FATAL_ERROR "r38_rekorder[${_name}]: keine Aufnahme angefordert (rk.log: ${_rk})")
    endif()
    set(_s ${CMAKE_MATCH_1})
    string(REGEX MATCH "F([0-9]+) raum=${_raum} ACCEPTED: sub02 @0x00C9C Cut_chg 11 -> Piep \\+ Aufschliessen" _m "${_rk}")
    if(NOT _m)
        message(FATAL_ERROR "r38_rekorder[${_name}]: kein ACCEPTED-Anker (rk.log: ${_rk})")
    endif()
    set(_acc ${CMAKE_MATCH_1})
    string(REGEX MATCH "F([0-9]+) raum=${_raum} nachricht=2 art=KARTE_AUF satz=1\\(PIEP\\) bild=\\+0" _m "${_auf}")
    if(NOT _m)
        message(FATAL_ERROR "r38_rekorder[${_name}]: Piep fehlt (auf.log: ${_auf})")
    endif()
    set(_p ${CMAKE_MATCH_1})
    if(NOT _p EQUAL _acc)
        message(FATAL_ERROR "r38_rekorder[${_name}]: Piep F${_p} nicht im Takt des ACCEPTED F${_acc}")
    endif()
    math(EXPR _p15 "${_p} + 15")
    j_muss(${_name} "${_auf}" "F${_p15} raum=${_raum} nachricht=2 art=KARTE_AUF satz=2\\(AUF_E\\) bild=\\+15")
    string(REGEX MATCHALL "satz=" _toene "${_auf}")
    list(LENGTH _toene _nt)
    if(NOT _nt EQUAL 2)
        message(FATAL_ERROR "r38_rekorder[${_name}]: ${_nt} Toene statt 2 (Piep + Aufschliessen): ${_auf}")
    endif()
    # erster Cut_chg(11) nach der Anforderung = Piep-Takt (debug.log "[scd F<n>] Cut_chg(11)")
    string(REGEX MATCHALL "\\[scd F[0-9]+\\] Cut_chg\\(11\\)" _c11 "${_dbg}")
    set(_erst -1)
    foreach(_z IN LISTS _c11)
        string(REGEX MATCH "F([0-9]+)" _mm "${_z}")
        if(CMAKE_MATCH_1 GREATER ${_s} AND _erst EQUAL -1)
            set(_erst ${CMAKE_MATCH_1})
        endif()
    endforeach()
    if(NOT _erst EQUAL _p)
        message(FATAL_ERROR "r38_rekorder[${_name}]: erstes Cut_chg(11) F${_erst}, Piep F${_p} - nicht derselbe Takt")
    endif()
    j_muss(${_name} "${_rk}" "raum=${_raum} Message_on 2 @0x00CBA: Piep/Aufschliessen schon am ACCEPTED gespielt")
    j_muss(${_name} "${_dbg}" "\\[voice\\] clip loaded: Irons_Rekorder_Sound.wav \\(32000 Hz x1")
    set(_s ${_s} PARENT_SCOPE)
    set(_p ${_p} PARENT_SCOPE)
    message(STATUS "r38_rekorder[${_name}]: Aufnahme F${_s}, Piep F${_p} = erstes ACCEPTED, Aufschliessen F${_p15}")
endfunction()

# Mitschnitt: Aufnahme finden, Ende vor dem Piep, Takt-Versatz von Aufnahme und Piep.
#
# Strang ton Stufe 1 Runde 3 (Pruefung Runde 2, R1 und G2) - DREI Aenderungen, jede mit Grund:
#  (1) Der Piep wird nicht mehr ueber den Bandanteil 1380..1530 Hz > 0,3 je Tick gesucht, sondern als Key-On des Satzes
#      der Mini-Bank per Kreuzkorrelation (probe_r38_rekorder.c, Kopf ueber piep_bezug_t). Der Bandanteil ist ein
#      Verhaeltnis zur Musik darunter: im Lauf S (Piep 5 Spielbilder nach dem Ende der Aufnahme) lag er im Key-On-Tick
#      bei 0,308, seit die Musik unter gesprochenen Saetzen 8 dB zuruecktritt, davor unter 0,3 - der Riegel nannte
#      Tick 708 statt 709 und wurde rot, obwohl der Piep auf demselben Abtastwert beginnt.
#  (2) Takt-Versatz: frueher "Aufnahme und Piep gleich". Das stimmte nur, weil das alte Mass den Key-On-Tick des Pieps
#      nicht sah (weicher Einsatz der Tonprobe: erst der Folgetick lag ueber 0,3). Am Key-On selbst gemessen liegt
#      der Piep EINEN Tick frueher als die Aufnahme, auf dem Tick-Anfang. Grund im Code:
#        Piep     re15_rekorder_r38_pc (ACCEPTED) -> re15_aufschliessen_r36_notice -> re15_audio_re2_karte_se ->
#                 se_play_layers: Satz 1 der Mini-Bank = `00 00 13 01`, byte3 & 0x1f = 1 < 0x10 -> Direkt-Zweig
#                 (FUN_80045024 "if ((byte3 & 0x1f) < 0x10)": sofortiges SsUtKeyOnV @0x8004522c) - die Stimme ist
#                 sofort aktiv und steht im naechsten gerenderten Tick ab dessen erstem Abtastwert.
#        Aufnahme stimme_anfordern (rekorder_r38.c) reiht SCD_AUDIO_VOICE_ON ein; re15_audio_tick (audio_pc.c) leert die
#                 Warteschlange erst NACH audio_callback, also nachdem der Tick dieses Bilds schon gerendert ist -
#                 die Aufnahme beginnt einen Tick spaeter.
#      Gemessen in L, S und E, mit abgesenkter und mit voller Musik (Gegenprobe-exe ohne Absenkung): immer -1.
#  (3) Die Schwelle der Korrelation war wirkungslos: `if(_k VERSION_LESS 0.8)` vergleicht Versionsnummern, 0.544 gilt
#      dabei als groesser als 0.8 (544 > 8). Jetzt Zahlenvergleich. Die Schwelle 0,8 heisst: die Aufnahme traegt in
#      ihrem Fenster mindestens 64 % der Energie des Mitschnitts (alles andere - Musik, Geraeusche - liegt mindestens
#      2,5 dB darunter). Ohne die Absenkung der Musik (Stand Runde 1) misst der Riegel 0,53 .. 0,61 und faellt.
#      Nur fuer Gegenproben: -DRE15_REK_KORR_MIN=<wert> setzt die Schwelle herab, damit ein Lauf mit voller Musik bis
#      zum Piep-Mass kommt (kein ctest-Aufruf setzt das).
if(NOT DEFINED RE15_REK_KORR_MIN)
    set(RE15_REK_KORR_MIN 0.8)
endif()
function(j_mitschnitt _name _s _p)
    execute_process(COMMAND "${RE15_PROBE}" cap "${_wd}/cap.raw" "${RE15_WAV}"
                    OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
    if(NOT _r EQUAL 0)
        message(FATAL_ERROR "r38_rekorder[${_name}]: cap-Werkzeug exit=${_r}: ${_o}")
    endif()
    string(REGEX MATCH "Korrelation ([0-9.]+)" _m "${_o}")
    set(_k ${CMAKE_MATCH_1})
    string(REGEX MATCH "WAV ANFANG Tick ([0-9]+) \\(\\+([0-9]+)\\) ENDE letzter Tick ([0-9]+)" _m "${_o}")
    if(NOT _m)
        message(FATAL_ERROR "r38_rekorder[${_name}]: Aufnahme nicht gefunden: ${_o}")
    endif()
    set(_ta ${CMAKE_MATCH_1})
    set(_tarest ${CMAKE_MATCH_2})
    set(_te ${CMAKE_MATCH_3})
    if(_k LESS ${RE15_REK_KORR_MIN})
        message(FATAL_ERROR "r38_rekorder[${_name}]: Korrelation ${_k} < ${RE15_REK_KORR_MIN} - die Aufnahme geht im Mitschnitt unter: ${_o}")
    endif()
    string(REGEX MATCH "PIEP KEYON Tick ([0-9]+) \\(([+-][0-9]+)\\) Korrelation ([0-9.]+) Verstaerkung ([0-9.]+) = ([0-9.]+) des Solls -> (PEGEL-[A-Z]+)" _m "${_o}")
    if(NOT _m)
        message(FATAL_ERROR "r38_rekorder[${_name}]: Piep nicht gefunden: ${_o}")
    endif()
    set(_tp ${CMAKE_MATCH_1})
    set(_tprest ${CMAKE_MATCH_2})
    set(_tpq ${CMAKE_MATCH_5})
    if(NOT CMAKE_MATCH_6 STREQUAL "PEGEL-OK")
        message(FATAL_ERROR "r38_rekorder[${_name}]: am besten passenden Versatz liegt nicht der Piep der Mini-Bank (Verstaerkung ${_tpq} des Solls, erlaubt 0.80 .. 1.20): ${_o}")
    endif()
    # Key-On auf dem Tick-Anfang; +-30 Abtastwerte = eine Periode des Tons (die Korrelation eines Tons hat Nebengipfel im
    # Abstand seiner Periode, die Musik darunter kann den hoechsten um eine Periode verschieben).
    if(_tprest GREATER 30 OR _tprest LESS -30)
        message(FATAL_ERROR "r38_rekorder[${_name}]: Key-On des Pieps ${_tprest} Abtastwerte neben dem Tick-Anfang (erlaubt +-30): ${_o}")
    endif()
    math(EXPR _len "${_te} - ${_ta} + 1")
    if(_len LESS 79 OR _len GREATER 81 OR NOT _tarest EQUAL 0)
        message(FATAL_ERROR "r38_rekorder[${_name}]: Aufnahme ${_len} Ticks ab +${_tarest} (soll 79..81 ab Tick-Anfang): ${_o}")
    endif()
    if(NOT _tp GREATER _te)
        message(FATAL_ERROR "r38_rekorder[${_name}]: Key-On des Pieps (Tick ${_tp}) nicht NACH dem Ende der Aufnahme (Tick ${_te})")
    endif()
    math(EXPR _va "${_ta} - ${_s}")
    math(EXPR _vp "${_tp} - ${_p}")
    math(EXPR _vp_soll "${_va} - 1")
    if(NOT _vp EQUAL _vp_soll)
        message(FATAL_ERROR "r38_rekorder[${_name}]: Takt-Versatz Piep ${_vp}, Soll ${_vp_soll} = Aufnahme ${_va} - 1 (Direkt-Zweig gegen Warteschlange, s. Kopf)")
    endif()
    message(STATUS "r38_rekorder[${_name}]: Mitschnitt Aufnahme Tick ${_ta}..${_te} (Korrelation ${_k}), Piep Key-On Tick ${_tp} (${_tprest}), Verstaerkung ${_tpq} des Solls, Versatz Aufnahme ${_va} / Piep ${_vp}")
endfunction()

function(j_bild _name _ppm _soll)
    if(NOT EXISTS "${_ppm}")
        message(FATAL_ERROR "r38_rekorder[${_name}]: Bild fehlt: ${_ppm}")
    endif()
    execute_process(COMMAND "${RE15_PROBE}" bild "${_ppm}" "${RE15_BSS}" OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
    string(REGEX MATCH "ANZEIGE Cut ([0-9]+)" _m "${_o}")
    if(NOT _r EQUAL 0 OR NOT CMAKE_MATCH_1 EQUAL _soll)
        message(FATAL_ERROR "r38_rekorder[${_name}]: ${_ppm} zeigt nicht Cut ${_soll}: ${_o}")
    endif()
    message(STATUS "r38_rekorder[${_name}]: ${_o}")
endfunction()

macro(_seg _a _n _k)
    math(EXPR _e "${_a} + ${_n} - 1")
    foreach(_f RANGE ${_a} ${_e})
        list(APPEND _pad "${_f}:${_k}")
    endforeach()
endmacro()

# Eingaben (gemessen, Dossier §2/§4): Runde 41 Spur O: statt Aktionstaste + Viereck auf msg 1 und msg 3 "Ja" das USE des Spielers
# (RE15_USE_TEST F300, Elza F430: das Panel steht vor dem Cursor-Weg), Cursor LEFT 15 / DOWN 19,
# Viereck 406 (ON/OFF), RIGHT 30 / DOWN 16, Viereck 498 (Mikrofon) - _o verschiebt alles (Elza).
macro(_weg _o)
    set(_pad "")
    math(EXPR _x "370 + ${_o}")
    _seg(${_x} 15 L)
    math(EXPR _x "385 + ${_o}")
    _seg(${_x} 19 D)
    math(EXPR _x "406 + ${_o}")
    list(APPEND _pad "${_x}:A")
    math(EXPR _x "450 + ${_o}")
    _seg(${_x} 30 R)
    math(EXPR _x "480 + ${_o}")
    _seg(${_x} 16 D)
    math(EXPR _x "498 + ${_o}")
    list(APPEND _pad "${_x}:A")
endmacro()

# --- L: Leon langsam ------------------------------------------------------------------------------------------------
_weg(0)
foreach(_f RANGE 530 800 10)
    list(APPEND _pad "${_f}:A")
endforeach()
list(JOIN _pad "," _pad_s)
j_lauf(l 820 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_AUDIO_CAP_SYNC=cap.raw
       RE15_DEBUG_JUMP=1100@120 RE15_SET_FLAG=3:0x67 RE15_GIVE=0x44:1 "RE15_USE_TEST=0x44@300#1100"
       "RE15_PAD_AT=${_pad_s}" "RE15_EXIT_AT=820#1100" "RE15_FRAMEDUMP=660-700/1:fd_")
j_muss(l "${_dbg}" "\\[msgtext\\] room=1100 id=4 \"You've used the Minidisc Player w/ Disc.\"")
j_muss(l "${_dbg}" "\\[msgtext\\] room=1100 id=2 \"You've opened the lock.\"")
j_folge(l 1100)
j_mitschnitt(l ${_s} ${_p})
math(EXPR _p1 "${_p} + 1")
string(LENGTH "000000${_p}" _lp)
math(EXPR _lp "${_lp} - 6")
string(SUBSTRING "000000${_p}" ${_lp} 6 _fp)
string(LENGTH "000000${_p1}" _lp)
math(EXPR _lp "${_lp} - 6")
string(SUBSTRING "000000${_p1}" ${_lp} 6 _fp1)
j_bild(l_piep_takt "${_wd}/fd_${_fp}.ppm" 12)
j_bild(l_piep_bild "${_wd}/fd_${_fp1}.ppm" 11)
j_muss(l "${_cut}" "F${_p1} room=1100 bg=1100#11 ")
j_muss(l "${_rk}" "raum=1100 frei: sub02 @0x00C96 Cut_chg 12 nach 0 Wartetakten")
j_muss(l "${_rk}" "raum=1100 Tuer frei: Platz 1 aktiv 1 typ 1")
file(REMOVE "${_wd}/cap.raw")

# --- S: Leon schnell (sub02 wartet) ---------------------------------------------------------------------------------
_weg(0)
_seg(500 21 A)
list(APPEND _pad "522:A")
_seg(524 27 A)
list(APPEND _pad "552:A" "556:A" "560:A" "564:A" "568:A")
foreach(_f RANGE 660 760 10)
    list(APPEND _pad "${_f}:A")
endforeach()
list(JOIN _pad "," _pad_s)
j_lauf(s 780 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_AUDIO_CAP_SYNC=cap.raw
       RE15_DEBUG_JUMP=1100@120 RE15_SET_FLAG=3:0x67 RE15_GIVE=0x44:1 "RE15_USE_TEST=0x44@300#1100"
       "RE15_PAD_AT=${_pad_s}" "RE15_EXIT_AT=780#1100")
j_folge(s 1100)
string(REGEX MATCH "F([0-9]+) raum=1100 stimme zu Ende: ([0-9]+) Bilder" _m "${_rk}")
set(_ende ${CMAKE_MATCH_1})
string(REGEX MATCH "F([0-9]+) raum=1100 frei: sub02 @0x00C96 Cut_chg 12 nach ([0-9]+) Wartetakten" _m "${_rk}")
set(_frei ${CMAKE_MATCH_1})
set(_wt ${CMAKE_MATCH_2})
if(NOT _m OR _wt LESS 1)
    message(FATAL_ERROR "r38_rekorder[s]: sub02 hat nicht auf die Aufnahme gewartet (rk.log: ${_rk})")
endif()
math(EXPR _frei_soll "${_ende} + 1")
math(EXPR _piep_soll "${_frei} + 5")
if(NOT _frei EQUAL _frei_soll OR NOT _p EQUAL _piep_soll)
    message(FATAL_ERROR "r38_rekorder[s]: Ende F${_ende}, frei F${_frei}, Piep F${_p} (soll frei = Ende+1, Piep = frei+5)")
endif()
j_mitschnitt(s ${_s} ${_p})
message(STATUS "r38_rekorder[s]: ${_wt} Wartetakte an @0x00C96, Aufnahme zu Ende F${_ende}, frei F${_frei}, Piep F${_p}")
file(REMOVE "${_wd}/cap.raw")

# --- E: Elza ROOM1101 ------------------------------------------------------------------------------------------------
_weg(130)
foreach(_f RANGE 660 930 10)
    list(APPEND _pad "${_f}:A")
endforeach()
list(JOIN _pad "," _pad_s)
j_lauf(e 950 SDL_ASSERT=always_ignore RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
       RE15_AUDIO_CAP_SYNC=cap.raw RE15_DEBUG_JUMP=1100@250 RE15_SET_FLAG=3:0x67 RE15_GIVE=0x44:1
       "RE15_USE_TEST=0x44@430#1101" "RE15_PAD_AT=${_pad_s}" "RE15_EXIT_AT=950#1101")
j_muss(e "${_dbg}" "\\[pl\\] Spieler-Familie PL04")
j_muss(e "${_dbg}" "\\[msgtext\\] room=1101 id=4 \"You've used the Minidisc Player w/ Disc.\"")
j_folge(e 1101)
j_mitschnitt(e ${_s} ${_p})
j_muss(e "${_rk}" "raum=1101 Tuer frei: Platz 1 aktiv 1 typ 1")
file(REMOVE "${_wd}/cap.raw")

# --- T: Tuer nach dem Oeffnen benutzbar ------------------------------------------------------------------------------
_weg(0)
foreach(_f RANGE 530 800 10)
    list(APPEND _pad "${_f}:A")
endforeach()
list(JOIN _pad "," _pad_s)
j_lauf(t 900 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_NOAUDIO=1
       RE15_DEBUG_JUMP=1100@120 RE15_SET_FLAG=3:0x67 RE15_GIVE=0x44:1 "RE15_USE_TEST=0x44@300#1100"
       "RE15_FIRE_AOT=1@860#1100" "RE15_PAD_AT=${_pad_s}" "RE15_EXIT_AT=900")
j_muss(t "${_rk}" "raum=1100 Tuer frei: Platz 1 aktiv 1 typ 1")
j_muss(t "${_dbg}" "\\[aot\\] DOOR FIRE slot=1 ")
j_muss(t "${_dbg}" "\\[room\\] PC loaded room1110.rdt")

file(REMOVE "${_exe_kopie}")
message(STATUS "r38_rekorder: L (Leon langsam), S (Leon schnell, Warten), E (Elza ROOM1101), T (Tuer -> ROOM1110) bestanden")
