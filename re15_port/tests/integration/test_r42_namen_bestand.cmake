# =============================================================================
# Runde 42 Strang "namen", Stufe 7 (Nachtrag 19.11.2026) - DURCHLAEUFE im echten Spiel mit UNVERAENDERTEM BESTAND, geprueft am
# TONMITSCHNITT:
#   TEIL 6030  Leon, Zug ROOM6030, Wagen-Gespraech (msg 0, 1, 21, 2, 3, 22, 4, 32, 5, 6, 7, 23, 24, 8)
#   TEIL 3020  Leon, 3000 -> 3010 -> ROOM3020 Fensterszene (msg 0, 1, 2, 7, 8) bis zum Tausch nach 30C0
#   TEIL 6031  Elza, Zug ROOM6031, Wagen-Szene E6 (msg 0, 1, 2, 12, 13) - Strang fabrikplan Stufe 3: msg 1/12 mit dem neuen Wortlaut
#              ("A woman ..."/"She said ...") spielen die Aufnahmen des Nutzers vom 20.11.2026 an ihrem neuen Untertitel
#
# Luecke aus dem Pruefbericht der Stufe 6 (gelb 3 und 4): integration_r42_story6_leon und _story3b_leon laufen aus einer Ersatz-Wurzel
# mit je einer ersetzten Datei, integration_r42_story3a_* laeuft ohne Ton; den Auslieferungsstand deckte kein Durchlauf ab. Hier
# liegt die exe-Kopie NEBEN der gebauten exe, alle Aufnahmen kommen aus <Repo>/synchro wie sie sind (nur gelesen).
#
# Erwartung (include/re15_altaufnahme_r42.h, PORT-WAHL, Entscheid des Orchestrators 19.11.2026: "stumm bleibt nur, was einen ANDEREN
# Satz spricht; eine Aufnahme, die den neuen Satz woertlich oder bis auf ein Wort spricht, spielt"):
#   6030  msg 21 (main21.wav, "... We made it down here ..." unter "... We came down here ...") SPIELT, an seinem Untertitel
#         msg 2, 3, 22, 4, 5 (anderer Satz, Abdruck des Tags v0.8.30) sind STUMM - im ganzen Mitschnitt nicht zu finden
#         msg 0, 1, 6, 7, 23, 24, 8 (nicht betroffen) spielen, an ihrem Untertitel (Negativkontrolle); msg 32 hat keine Datei
#   3020  msg 2 (main02.wav, "Leon, give me a hand." unter "Leon, give me a hand!") SPIELT, an seinem Untertitel
#         msg 0, 1 (anderer Satz) sind STUMM; msg 7, 8 haben keine Datei
#         Negativkontrolle: die Stimmen der Ankunft in ROOM3000 (msg 2..8) spielen
# STAND SEIT Strang fabrikplan Stufe 2 (Nutzer 20.11.2026 ~18:30: "Ok, saemtliche Sprachaufnahmen liegen vor.", Commit ce5ec405f;
# Aufnahmeliste FEHLENDE_AUFNAHMEN_2026-11-20.md Teile A, B, C) - die Erwartung oben ist abgeloest:
#   6030  msg 2, 3, 22, 4, 5 (Teil A) und 21 (Teil B) tragen die NEUE Aufnahme des Nutzers und SPIELEN an ihrem Untertitel;
#         msg 32 (Teil C) hat jetzt eine Datei und SPIELT an ihrem Untertitel
#   3020  msg 0, 1 (Teil A) tragen die neue Aufnahme und SPIELEN; msg 7, 8 (Teil C) haben jetzt eine Datei und SPIELEN;
#         msg 2 bleibt die Aufnahme des Tags (spricht den neuen Satz schon, keine neue Aufnahme noetig) und spielt
# Eine Datei mit dem Abdruck des Tags (alte Aufnahme) oder eine fehlende Datei ist bei diesen Saetzen jetzt ein FEHLER.
#
# Ton: RE15_AUDIO_CAP_SYNC (audio_pc.c re15_audio_tick: je Spielbild genau 1470 Stereo-Abtastwerte durch dieselbe Mix-Kette wie
# der Geraete-Callback, bitgleich wiederholbar). Messung: tests/unit/werkzeug_r42_tonmess.c (normierte Kreuzkorrelation jeder
# Aufnahme gegen den ganzen Mitschnitt; "spielt" = Spitze >= 0,25 und Abstand >= 4; Lage = capTick aus "[voice] Start").
# Wege und Vorgaben wie tests/integration/test_r42_story6.cmake TEIL leon bzw. test_r42_story3a.cmake TEIL kette (dort begruendet),
# Ende nach der Szene.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -DTEIL=<6030|3020|6031> -DRE15_SYNCHRO=<Repo>/synchro
#               -DRE15_TONMESS=<werkzeug_r42_tonmess> -P test_r42_namen_bestand.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r42_namen_bestand: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r42_namen_bestand: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL 6030)
endif()
if(NOT RE15_SYNCHRO OR NOT IS_DIRECTORY "${RE15_SYNCHRO}")
    message(FATAL_ERROR "r42_namen_bestand: RE15_SYNCHRO fehlt/kein Ordner: '${RE15_SYNCHRO}'")
endif()
if(NOT RE15_TONMESS OR NOT EXISTS "${RE15_TONMESS}")
    message(FATAL_ERROR "r42_namen_bestand: RE15_TONMESS fehlt/existiert nicht: '${RE15_TONMESS}'")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/altaufnahmen_r42.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}_${TEIL}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r42nb_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r42nb_${_lauf_id}")
endif()

set(_fehler "")
macro(fail _t)
    list(APPEND _fehler "${_t}")
    message(STATUS "FAIL r42_namen_bestand[${TEIL}]: ${_t}")
endmacro()
macro(ok _t)
    message(STATUS "ok   r42_namen_bestand[${TEIL}]: ${_t}")
endmacro()
macro(pruefe _text _muster)
    string(FIND "${_dbg}" "${_muster}" _p)
    if(_p LESS 0)
        fail("${_text} (fehlt: '${_muster}')")
    else()
        ok("${_text}")
    endif()
endmacro()
macro(pruefe_nicht _text _muster)
    string(FIND "${_dbg}" "${_muster}" _p)
    if(_p LESS 0)
        ok("${_text}")
    else()
        fail("${_text} (gefunden: '${_muster}')")
    endif()
endmacro()
macro(pruefe_re _text _regex)
    if(_dbg MATCHES "${_regex}")
        ok("${_text}")
    else()
        fail("${_text} (fehlt: /${_regex}/)")
    endif()
endmacro()
function(_fertig)
    file(REMOVE "${_exe_kopie}")
    file(REMOVE "${WORKDIR}/ton.raw")                 # rund 20 MB je Lauf
    list(LENGTH _fehler _nf)
    if(_nf GREATER 0)
        message(FATAL_ERROR "r42_namen_bestand[${TEIL}]: ${_nf} Fehler (${WORKDIR})")
    endif()
    message(STATUS "r42_namen_bestand[${TEIL}]: alle Pruefungen ok (${WORKDIR})")
endfunction()

# Bestand unveraendert: neben der exe darf kein eigenes synchro/ liegen (der Basis-Wurzelsucher asset_root_pc.c rr_init nimmt das
# exe-Verzeichnis zuerst - so bauen die anderen Durchlaeufe ihre Ersatz-Wurzel)
if(EXISTS "${_exe_dir}/synchro")
    message(FATAL_ERROR "r42_namen_bestand: neben der exe liegt ein eigenes synchro/ (${_exe_dir}/synchro) - das waere nicht der Bestand")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# --- Erwartung je Aufnahme aus dem Stand der Datei im Baum (Liste _mess fuer das Messwerkzeug)
set(_mess "")
macro(_tick _out _raum _id)
    set(${_out} "")
    if(_dbg MATCHES "\\[voice\\] Start ROOM${_raum} Id ${_id} [^\n]*capTick ([0-9]+)")
        set(${_out} "@${CMAKE_MATCH_1}")
    endif()
endmacro()
macro(_datei _out _raum _id)
    string(SUBSTRING "${_raum}" 0 1 _stage)
    if(${_id} LESS 10)
        set(${_out} "${RE15_SYNCHRO}/STAGE${_stage}/room${_raum}/main0${_id}.wav")
    else()
        set(${_out} "${RE15_SYNCHRO}/STAGE${_stage}/room${_raum}/main${_id}.wav")
    endif()
endmacro()
macro(_soll_spielt _raum _id _warum)
    _datei(_f ${_raum} ${_id})
    if(NOT EXISTS "${_f}")
        fail("${_raum} msg ${_id}: Datei fehlt im Bestand (${_f})")
    else()
        pruefe("Log: ${_raum} msg ${_id} startet die Stimme (${_warum})" "[voice] Start ROOM${_raum} Id ${_id} ")
        pruefe_nicht("Log: ${_raum} msg ${_id} gilt nicht als alte Aufnahme" "ROOM${_raum} msg ${_id}: synchro/")
        _tick(_t ${_raum} ${_id})
        list(APPEND _mess "SPIELT${_t}=${_f}")
    endif()
endmacro()
# fast woertlich (Stufe 7): steht nicht in der Tabelle, spielt - mit der Aufnahme des Tags oder einer ersetzten Datei
macro(_fast_woertlich _raum _id)
    re15_altaufnahme_stand(_st "${RE15_SYNCHRO}" ${_raum} ${_id})
    re15_altaufnahme_fastwoertlich_stand(_fw "${RE15_SYNCHRO}" ${_raum} ${_id})
    if(NOT _st STREQUAL "KEINE")
        fail("${_raum} msg ${_id} steht in der Tabelle der veralteten Aufnahmen (Stand ${_st}) - Stufe 7: sie spricht den Untertitel woertlich oder bis auf ein Wort und spielt")
    else()
        ok("${_raum} msg ${_id} steht nicht in der Tabelle der veralteten Aufnahmen")
    endif()
    # Strang fabrikplan Stufe 2: 6030 msg 21 hat der Nutzer neu aufgenommen (Teil B) -> NEU verlangt; 3020 msg 2 spricht den
    # neuen Satz schon ("keine neue Aufnahme noetig") -> ALT oder NEU, beide spielen
    if("${_raum}" STREQUAL "6030" AND NOT _fw STREQUAL "NEU")
        fail("${_raum} msg ${_id}: Stand der Datei '${_fw}' - seit dem 20.11.2026 liegt dort die neue Aufnahme des Nutzers (Teil B)")
    elseif(_fw STREQUAL "ALT")
        _soll_spielt(${_raum} ${_id} "Bestand: Aufnahme des Tags v0.8.30, fast woertlich")
    elseif(_fw STREQUAL "NEU")
        _soll_spielt(${_raum} ${_id} "Bestand: Datei ersetzt")
    else()
        fail("${_raum} msg ${_id}: Stand der Datei '${_fw}' (erwartet ALT oder NEU)")
    endif()
    message(STATUS "     r42_namen_bestand[${TEIL}]: Bestand - ${_raum} msg ${_id} ${_fw} (spielt)")
endmacro()
# veraltet (anderer Satz, Teil A der Aufnahmeliste): seit dem 20.11.2026 liegt die NEUE Aufnahme des Nutzers im Bestand und spielt
# an ihrem Untertitel; die alte Aufnahme (Abdruck des Tags, stumm) oder eine fehlende Datei ist jetzt ein Fehler
macro(_veraltet _raum _id)
    re15_altaufnahme_stand(_st "${RE15_SYNCHRO}" ${_raum} ${_id})
    if(_st STREQUAL "NEU")
        _soll_spielt(${_raum} ${_id} "Bestand: neue Aufnahme des Nutzers 20.11.2026")
    elseif(_st STREQUAL "ALT")
        fail("${_raum} msg ${_id}: im Bestand liegt noch die alte Aufnahme (Abdruck des Tags v0.8.30) - seit dem 20.11.2026 muss dort die neue Aufnahme des Nutzers liegen")
    elseif(_st STREQUAL "FEHLT")
        fail("${_raum} msg ${_id}: keine Datei im Bestand - seit dem 20.11.2026 muss dort die neue Aufnahme des Nutzers liegen")
    else()
        fail("${_raum} msg ${_id} steht nicht in der Tabelle der veralteten Aufnahmen")
    endif()
    message(STATUS "     r42_namen_bestand[${TEIL}]: Bestand - ${_raum} msg ${_id} ${_st}")
endmacro()

if(TEIL STREQUAL "6030")
    file(WRITE "${WORKDIR}/plan.txt" "RAUM 6000
WARTE 30
GEHE 1700 -10100 150
BLICK 1024
AKTION
RAUM 6030
WARTE 40
GEHE -19100 -20900 200
WARTE 2700
ENDE
")
    # Ende 2300 Bilder nach dem Eintritt in 6030: das Gespraech endet mit msg 8 (gemessen: Untertitel bei Raumbild 1891, Lauf
    # video/s6_6030_jetzt, zeiten_jetzt.txt), danach rund 400 Bilder Luft.
    re15_start_spiel(_rv 1200 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
        RE15_STIMME_LOG=1 RE15_SE_DEBUG=1 RE15_WEGPLAN_LOG=wegplan.txt RE15_STATE_LOG=state.log "RE15_PLAYER_POS=1693,-9900,1024,2"
        "RE15_AUDIO_CAP_SYNC=${WORKDIR}/ton.raw" RE15_DEBUG_JUMP=6000@250 "RE15_SET_FLAG=3:48,3:49,3:100,3:239"
        "RE15_FIRE_AOT=5@300#6000" "RE15_WEGPLAN=${WORKDIR}/plan.txt" "RE15_EXIT_AT=2300#6030" "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        fail("kein debug.log (exit=${_rv})")
        _fertig()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    pruefe("Ton: bildgenauer Mitschnitt des Mischers" "[audio] RE15_AUDIO_CAP_SYNC -> ")
    pruefe("exe-Kopie liegt neben der gebauten exe (Bestand aus dem Repo)" "[asset] exe-dir: ${_exe_dir}")
    pruefe("Wagen-Programm L21" "[story80] ROOM6030 Evt_exec sub02 -> Programm")
    pruefe_re("Zeilen 0, 1, 21, 2, 3, 22, 4, 32, 5, 6, 7, 23, 24, 8 in dieser Folge"
        "room=6030 id=0 .*room=6030 id=1 .*room=6030 id=21 .*room=6030 id=2 .*room=6030 id=3 .*room=6030 id=22 .*room=6030 id=4 .*room=6030 id=32 .*room=6030 id=5 .*room=6030 id=6 .*room=6030 id=7 .*room=6030 id=23 .*room=6030 id=24 .*room=6030 id=8 ")
    pruefe("Untertitel msg 21" "[msgtext] room=6030 id=21 \"John: Name's John. This is Sherry. We came down here with a girl named Elza.\"")
    pruefe("Ende nach dem Gespraech" "EXIT_AT: Bild 2300 in Raum 6030")
    _fast_woertlich(6030 21)
    foreach(_m 2 3 22 4 5)
        _veraltet(6030 ${_m})
    endforeach()
    foreach(_m 0 1 6 7 23 24 8)                       # Negativkontrolle: die uebrigen Stimmen des Gespraechs
        _soll_spielt(6030 ${_m} "nicht betroffen")
    endforeach()
    _soll_spielt(6030 32 "neue Datei des Nutzers 20.11.2026, Teil C")    # vorher "keine Aufnahme"
    pruefe_nicht("Log: msg 32 nicht 'keine Aufnahme'" "ROOM6030 msg 32: keine Aufnahme")
elseif(TEIL STREQUAL "3020")
    # Weg und Vorgaben: tests/integration/test_r42_story3a.cmake (_basis, _weg_vor_30c0), hier MIT Ton und nur bis zum Tausch.
    file(WRITE "${WORKDIR}/plan.txt" "RAUM 3000
WARTE 30
GEHE -5555 -17520 300
BLICK 0
AKTION
RAUM 3010
WARTE 30
GEHE -22790 10744 250
GEHE -19040 -14756 250
GEHE -8990 -20006 250
GEHE -7340 -18356 250
GEHE -6500 -18300 250
BLICK 0
AKTION
RAUM 3020
WARTE 30
GEHE -19500 -19000 300
BLICK 3656
LAUF 2 7 90
RAUM 30C0
WARTE 60
ENDE
")
    re15_start_spiel(_rv 1500 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
        RE15_EQUIP=3 "RE15_GIVE=21:200"
        "RE15_SET_FLAG=9:90,9:111,9:112,9:113,9:118,9:119,3:245,3:206,3:151,9:121,9:120,7:64,7:65,7:66,7:67,7:68,7:69,7:70,7:113,7:114"
        RE15_GOTO_ROOM=3000 RE15_WEGPLAN_LOG=wegplan.txt RE15_STIMME_LOG=1 RE15_SE_DEBUG=1 RE15_STATE_LOG=state.log
        "RE15_AUDIO_CAP_SYNC=${WORKDIR}/ton.raw" RE15_START_ZUSTAND=70 "RE15_WEGPLAN=${WORKDIR}/plan.txt" "RE15_EXIT_AT=120#30C0"
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        fail("kein debug.log (exit=${_rv})")
        _fertig()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    pruefe("Ton: bildgenauer Mitschnitt des Mischers" "[audio] RE15_AUDIO_CAP_SYNC -> ")
    pruefe("exe-Kopie liegt neben der gebauten exe (Bestand aus dem Repo)" "[asset] exe-dir: ${_exe_dir}")
    pruefe("Fensterszene 3020" "ROOM3020: Fensterszene = sub02 mit Zeilen 7/8")
    pruefe_re("Zeilen 0, 1, 2, 7, 8 in dieser Folge"
        "msgtext] room=3020 id=0 .*msgtext] room=3020 id=1 .*msgtext] room=3020 id=2 .*msgtext] room=3020 id=7 .*msgtext] room=3020 id=8 ")
    pruefe("Untertitel msg 2" "[msgtext] room=3020 id=2 \"Marvin: Leon, give me a hand!\"")
    pruefe("Tausch Leon -> Marvin erreicht (Ende der Szene)" "Tausch Leon -> Marvin")
    pruefe_re("Ende in 30C0" "EXIT_AT: Bild 120 in Raum 30[cC]0")
    _fast_woertlich(3020 2)
    _veraltet(3020 0)
    _veraltet(3020 1)
    foreach(_m 7 8)                                   # vorher "keine Aufnahme" (Untertitel mit Textdauer)
        _soll_spielt(3020 ${_m} "neue Datei des Nutzers 20.11.2026, Teil C")
        pruefe_nicht("Log: msg ${_m} nicht 'keine Aufnahme'" "ROOM3020 msg ${_m}: keine Aufnahme")
    endforeach()
    foreach(_m 2 3 4 5 6 7 8)                         # Negativkontrolle: die Stimmen der Ankunft in ROOM3000
        _soll_spielt(3000 ${_m} "nicht betroffen")
    endforeach()
elseif(TEIL STREQUAL "6031")
    # Strang fabrikplan Stufe 3 (NUTZERWUNSCH 20.11.2026, gewaehlt "Ada bringt Marvin"; Konzept K-17): Elzas Wagen-Szene E6 mit den
    # NEUEN Untertiteln msg 1 "A woman ... she" / msg 12 "She said ... her" - die Aufnahmen des Nutzers vom 20.11.2026 (ce5ec405f,
    # Whisper: genau dieser Wortlaut) SPIELEN an IHREM Untertitel. Weg und Vorgaben wie test_r42_story6.cmake TEIL elza (dort
    # begruendet), Ende nach der Szene (msg 13 startet dort um Raumbild 870 und steht 5,95 s).
    file(WRITE "${WORKDIR}/plan.txt" "RAUM 6001
WARTE 30
GEHE 1700 -10100 150
BLICK 1024
AKTION
RAUM 6031
WARTE 40
GEHE -21000 -21000 250
GEHE -24300 -21200 250
WARTE 1500
ENDE
")
    re15_start_spiel(_rv 1200 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
        RE15_PSELECT_AUTO_SWITCH=1 RE15_STIMME_LOG=1 RE15_SE_DEBUG=1 RE15_WEGPLAN_LOG=wegplan.txt RE15_STATE_LOG=state.log
        "RE15_PLAYER_POS=1693,-9900,1024,2" "RE15_AUDIO_CAP_SYNC=${WORKDIR}/ton.raw" RE15_DEBUG_JUMP=6001@250
        "RE15_SET_FLAG=3:48,3:49" "RE15_WEGPLAN=${WORKDIR}/plan.txt" "RE15_EXIT_AT=1400#6031" "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        fail("kein debug.log (exit=${_rv})")
        _fertig()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    pruefe("Ton: bildgenauer Mitschnitt des Mischers" "[audio] RE15_AUDIO_CAP_SYNC -> ")
    pruefe("exe-Kopie liegt neben der gebauten exe (Bestand aus dem Repo)" "[asset] exe-dir: ${_exe_dir}")
    pruefe("E6: Wagen-Programm" "ROOM6031 Evt_exec sub02 -> Programm (270 B): Wagen-Szene E6")
    pruefe_re("E6: Zeilen 0, 1, 2, 12, 13 in dieser Folge"
        "msgtext] room=6031 id=0 .*msgtext] room=6031 id=1 .*msgtext] room=6031 id=2 .*msgtext] room=6031 id=12 .*msgtext] room=6031 id=13 ")
    # der NEUE Untertitel, und die Stimme startet an genau diesem Untertitel (hoechstens drei Logzeilen danach). Klammern als [[] / []],
    # nicht mit Backslash: pruefe_re ist ein Makro, CMake wertet die Escapes beim Einsetzen ein zweites Mal aus.
    pruefe_re("msg 1: neuer Untertitel, Stimme startet an ihm"
        "msgtext[]] room=6031 id=1 \"Sherry: A woman brought Marvin down here[.]Sherry: Then she went back into the fire[.]\"\n([^\n]*\n)?([^\n]*\n)?([^\n]*\n)?[[]voice[]] Start ROOM6031 Id 1 ")
    pruefe_re("msg 12: neuer Untertitel, Stimme startet an ihm"
        "msgtext[]] room=6031 id=12 \"Sherry: She said she had to find someone[.] She told us not to wait for her[.]\"\n([^\n]*\n)?([^\n]*\n)?([^\n]*\n)?[[]voice[]] Start ROOM6031 Id 12 ")
    pruefe_nicht("kein 'A policeman' mehr" "Sherry: A policeman")
    pruefe_nicht("kein 'He said he had to find someone' mehr" "Sherry: He said")
    pruefe_re("Ende nach der Szene" "EXIT_AT: Bild 1400 in Raum 6031")
    _veraltet(6031 1)                                 # Teil A: neue Aufnahme des Nutzers, kein Abdruck des Tags
    _soll_spielt(6031 12 "neue Datei des Nutzers 20.11.2026, Teil C")
    pruefe_nicht("Log: msg 12 nicht 'keine Aufnahme'" "ROOM6031 msg 12: keine Aufnahme")
    _veraltet(6031 2)                                 # Teil A wie msg 1 (Wortlaut unveraendert "Back? Why?")
    foreach(_m 0 13)                                  # Negativkontrolle: die uebrigen Stimmen der Szene
        _soll_spielt(6031 ${_m} "nicht betroffen")
    endforeach()
else()
    fail("unbekannter TEIL ${TEIL}")
    _fertig()
endif()

# --- Messung am Tonmitschnitt
if(NOT EXISTS "${WORKDIR}/ton.raw")
    fail("kein Tonmitschnitt (${WORKDIR}/ton.raw)")
    _fertig()
endif()
execute_process(COMMAND "${RE15_TONMESS}" "${WORKDIR}/ton.raw" ${_mess}
                RESULT_VARIABLE _trv OUTPUT_VARIABLE _tout ERROR_VARIABLE _terr)
file(WRITE "${WORKDIR}/tonmess.txt" "${_tout}${_terr}")
message(STATUS "Tonmitschnitt gemessen (${WORKDIR}/tonmess.txt):\n${_tout}")
if(NOT _trv EQUAL 0)
    fail("Tonmitschnitt: mindestens eine Aufnahme spielt/schweigt nicht wie erwartet (werkzeug_r42_tonmess -> ${_trv})")
else()
    ok("Tonmitschnitt: die neuen Aufnahmen des Nutzers, die fast woertliche und die nicht betroffenen Stimmen spielen an ihrem Untertitel")
endif()
_fertig()
