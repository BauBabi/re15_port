cmake_minimum_required(VERSION 3.16)   # cmake -P: ohne diese Zeile gelten im Linux-Container (CMake 3.28) die alten Policies (IN_LIST, if(TRUE)) - Muster der aelteren Integrationsskripte, Memory reai-v2-linux-test-portabilitaet
# =============================================================================
# HANGAR NACH DER SZENE: WIEDEREINTRITT UND LADEN (Runde 41 Rueckbau Monitorbild + Bank-9-Bit 38; seit Runde 42 "hangar_aussen" mit
# dem neuen Ausloeser und dem leeren Gang, birkin_formen.c Kopf "Runde 42").
#
# Seit Runde 42 (PORT-SZENE, Nutzer 16.11.2026) laeuft die Szene nur beim Betreten durch die Nordtuer aus dem Monitorraum 5030/5031 und
# endet ohne Kampf: Birkin entfernt, Spider-Man liegt tot. Danach ist der Gang leer - beim Wiedereintritt im selben Prozess und nach dem
# Laden eines Spielstands ("Szene gesehen" = Bank-9-Bit 38 im Spielstand). Bis 98391e747 stand Birkin dann als Angreifer da.
#
# Je Figur (Leon 50A0/5030, Elza 50A1/5031) zwei Prozesse auf EINER Karte, Affen per Kill-Flag 8:42/8:43 aus:
#   Lauf 1  Debug-Sprung 5030 an die Tuer, Aktionstaste -> Gang -> Szene. Wegplan: an die Nordtuer, zurueck in den Monitorraum,
#           wieder in den Gang (Wiedereintritt), dort Speichern in Bild 400 (RE15_SAVE_TEST_AGAIN), Prozessende.
#           Verlangt: genau eine "Szene Start" und "Szene Ende"; im zweiten Gang-Besuch "ohne Birkin (..., Szene gesehen 1)" und kein
#           Typ 0x30/0x2B; gespeichert im Gang.
#   Lauf 2  NEUER Prozess, CONTINUE aus Slot 0 (im Gang geladen), dann Debug-Sprung in den Monitorraum und durch die Nordtuer in den
#           Gang. Verlangt: keine "Szene Start", kein Typ 0x30/0x2B (weder im geladenen Gang noch nach dem Betreten), zweimal "ohne Birkin".
# GEGENPROBE (gemessen, scratchpad hangar_aussen/BERICHT_r1.md): exe von 98391e747 -> ROT (keine Szene aus der Nordtuer; nach dem
#   Laden Birkin in seiner KI).
#
# Testhaken (kein Spielverhalten): RE15_SAVE_TEST_AGAIN / RE15_CARD_AUTO / RE15_CARD_SLOT / RE15_SAVE_TEST_EXIT_AFTER (main.c, Form
# test_save_counter_pin.cmake), RE15_CONTINUE_TEST, RE15_DEBUG_JUMP, RE15_PLAYER_POS, RE15_INPUT_SCRIPT, RE15_WEGPLAN(_LOG),
# RE15_SET_FLAG, RE15_STATE_LOG, RE15_HANGAR_LOG, RE15_EXIT_AT.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_hangar_laden.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_hangar_laden: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_hangar_laden: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41hangarladen_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41hangarladen_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# Binaer-sicheres Lesen (debug.log enthaelt Binaerbytes, file(STRINGS) verschluckt dann still den Rest - test_save_counter_pin.cmake).
function(_zahl _datei _text _out)
    set(${_out} 0 PARENT_SCOPE)
    if(NOT EXISTS "${_datei}")
        return()
    endif()
    file(READ "${_datei}" _lh HEX)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(_n 0)
    string(FIND "${_lh}" "${_h}" _p)
    while(NOT _p LESS 0)
        math(EXPR _n "${_n} + 1")
        math(EXPR _s "${_p} + 2")
        string(SUBSTRING "${_lh}" ${_s} -1 _lh)
        string(FIND "${_lh}" "${_h}" _p)
    endwhile()
    set(${_out} ${_n} PARENT_SCOPE)
endfunction()
# Figurenzeilen (Typ 0x30/0x2B) je Abschnitt des state.log (Abschnitt = Ruecksprung des Bildzaehlers = Raumwechsel).
function(_figuren _datei _abschnitt _out _n_ab)
    file(STRINGS "${_datei}" _zl REGEX "^F[0-9]+ ")
    set(_ab 0)
    set(_vor -1)
    set(_n 0)
    foreach(_z IN LISTS _zl)
        if(_z MATCHES "^F([0-9]+) ")
            if(CMAKE_MATCH_1 LESS _vor)
                math(EXPR _ab "${_ab} + 1")
            endif()
            set(_vor ${CMAKE_MATCH_1})
        endif()
        if(_ab EQUAL _abschnitt AND _z MATCHES " t=(30|2b) ")
            math(EXPR _n "${_n} + 1")
        endif()
    endforeach()
    set(${_out} ${_n} PARENT_SCOPE)
    set(${_n_ab} ${_ab} PARENT_SCOPE)
endfunction()

set(_f "")

function(figur _name _raum _monitor _extra)
    string(TOLOWER "${_raum}" _rl)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    set(_g "")
    # Monitorraum-Tuer ROOM5030 main00 Slot 3 @0x00EC6 (Mitte (-15300,-26300)), Nordtuer ROOM50A0 main00 Slot 3 @0x0156A.
    file(WRITE "${WORKDIR}/weg1.txt" "RAUM ${_raum}\nGEHE -12117 4600\nBLICK 3072\nAKTION\nRAUM ${_monitor}\nGEHE -15300 -25800\n"
                                     "BLICK 1024\nAKTION\nRAUM ${_raum}\nENDE\n")
    file(WRITE "${WORKDIR}/weg2.txt" "RAUM ${_monitor}\nGEHE -15300 -25800\nBLICK 1024\nAKTION\nRAUM ${_raum}\nENDE\n")

    # --- Lauf 1: Szene, Nordtuer hinaus und wieder herein, Speichern ---------------------------------------------------------------
    re15_start_spiel(_rv1 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_HANGAR_LOG=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_SET_FLAG=8:42,8:43
        RE15_DEBUG_JUMP=5030@20
        RE15_PLAYER_POS=-15300,-26300,1024
        "RE15_INPUT_SCRIPT=W1,A0.2"
        "RE15_WEGPLAN=${WORKDIR}/weg1.txt" RE15_WEGPLAN_LOG=wegplan1.txt
        RE15_STATE_LOG=state1.log
        RE15_SAVE_TEST_AGAIN=400 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_SAVE_TEST_EXIT_AFTER=1
        "${_exe_kopie}")
    if(EXISTS "${WORKDIR}/debug.log")
        file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug1.log")
    endif()
    _zahl("${WORKDIR}/debug1.log" "[hangar] ROOM${_raum} Szene Start: Nordtuer aus ${_monitor}" _s1)
    _zahl("${WORKDIR}/debug1.log" "[hangar] ROOM${_raum} Szene Ende: Cut 0, Birkin entfernt" _e1)
    _zahl("${WORKDIR}/debug1.log" "[hangar] ROOM${_raum} ohne Birkin (hier besiegt 0, Kraftraum besiegt 0, Szene gesehen 1)" _w1)
    _zahl("${WORKDIR}/debug1.log" "[save] saved (room ${_rl})" _sv)
    _figuren("${WORKDIR}/state1.log" 4 _fig1 _ab1)   # 0 Start, 1 Monitorraum, 2 Gang (Szene), 3 Monitorraum, 4 Gang (Wiedereintritt)
    if(NOT _s1 EQUAL 1 OR NOT _e1 EQUAL 1 OR NOT _w1 EQUAL 1 OR NOT _sv OR NOT _ab1 EQUAL 4 OR _fig1 GREATER 0
       OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        set(_f "${_f}\n ${_name}: Lauf 1 - Szene ${_s1}x / Ende ${_e1}x (Soll 1/1), Wiedereintritt leer ${_w1}x (Soll 1), Figurenzeilen im 2. Besuch ${_fig1} (Soll 0), Raumwechsel ${_ab1} (Soll 4), gespeichert ${_sv} (exit=${_rv1}, ${WORKDIR})"
            PARENT_SCOPE)
        return()
    endif()

    # --- Lauf 2: neuer Prozess, Laden im Gang, durch den Monitorraum erneut betreten ------------------------------------------------
    re15_start_spiel(_rv2 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_HANGAR_LOG=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_DEBUG_JUMP=5030@60
        RE15_PLAYER_POS=-15300,-26300,1024
        "RE15_WEGPLAN=${WORKDIR}/weg2.txt" RE15_WEGPLAN_LOG=wegplan2.txt
        RE15_STATE_LOG=state2.log
        "RE15_EXIT_AT=200#${_raum}!2"
        "${_exe_kopie}")
    if(EXISTS "${WORKDIR}/debug.log")
        file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug2.log")
    endif()
    _zahl("${WORKDIR}/debug2.log" "CONTINUE: resumed in room ${_rl}" _geladen)
    _zahl("${WORKDIR}/debug2.log" "EXIT_AT: Bild 200 in Raum ${_rl}" _ende)
    _zahl("${WORKDIR}/debug2.log" "Szene Start" _s2)
    _zahl("${WORKDIR}/debug2.log" "[hangar] ROOM${_raum} ohne Birkin (hier besiegt 0, Kraftraum besiegt 0, Szene gesehen 1)" _w2)
    file(STRINGS "${WORKDIR}/state2.log" _zl2 REGEX " t=(30|2b) ")
    list(LENGTH _zl2 _n2)
    if(NOT _geladen OR NOT _ende)
        string(APPEND _g "\n ${_name}: Lauf 2 unvollstaendig (geladen ${_geladen}, Endbild ${_ende}, exit=${_rv2}, ${WORKDIR})")
    else()
        if(_s2 GREATER 0)
            string(APPEND _g "\n ${_name}: Lauf 2 - die Hangar-Szene laeuft nach dem Laden erneut (${_s2}x)")
        endif()
        if(NOT _w2 EQUAL 2)
            string(APPEND _g "\n ${_name}: Lauf 2 - 'ohne Birkin (..., Szene gesehen 1)' ${_w2}x statt 2x (Laden + Betreten)")
        endif()
        if(_n2 GREATER 0)
            string(APPEND _g "\n ${_name}: Lauf 2 - ${_n2} state-Zeilen mit Birkin/Spider-Man nach dem Laden/Betreten")
        endif()
    endif()
    message(STATUS "r41_hangar_laden ${_name}: Lauf 1 exit=${_rv1} (Szene ${_s1}x, Wiedereintritt leer ${_w1}x), Lauf 2 exit=${_rv2} "
                   "(Szene ${_s2}x, leer ${_w2}x, Figurenzeilen ${_n2})${_g}")
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

figur(leon 50A0 5030 "")
figur(elza 50A1 5031 "RE15_TITLE_SHOT_CHAR=1")

file(REMOVE "${_exe_kopie}")
if(_f)
    message(FATAL_ERROR "r41_hangar_laden:${_f}")
endif()
message(STATUS "r41_hangar_laden: OK (Leon 50A0 + Elza 50A1, Laufverzeichnis ${_wurzel})")
