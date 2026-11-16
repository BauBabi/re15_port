cmake_minimum_required(VERSION 3.16)   # cmake -P: ohne diese Zeile gelten im Linux-Container (CMake 3.28) die alten Policies (IN_LIST, if(TRUE)) - Muster der aelteren Integrationsskripte, Memory reai-v2-linux-test-portabilitaet
# =============================================================================
# HANGAR-SZENE NUR AUS DEM MONITORRAUM (Runde 42 "hangar_aussen", Zweig r42/hangar_aussen; birkin_formen.c Kopf "Runde 42").
#
# PORT-SZENE nach Gestaltung des Nutzers (16.11.2026, woertlich): "Bei Szene aussen muss das aber vom Monitor Raum aus erfolgen, wenn
# man zurueck laeuft, sonst fragt man sich - wenn er Richtung Kamera zum anderen Ausgang hin geht wo er denn hin ist..."
#   - Ausloeser NUR beim Betreten des Gangs 50A0/50A1 durch die Nordtuer aus dem Monitorraum 5030/5031 (Tuersatz ROOM5030 main00
#     Slot 3 @0x00EC6, Eintritts-Schnitt 0), Szene noch nicht gesehen (Bank-9-Bit 38), Birkin nicht im Kraftraum besiegt (3,0x49).
#   - Durch die Westtuer (aus 4070/4071, ROOM4070 main00 Slot 1 @0x011FE, Eintritts-Schnitt 11) nie eine Szene.
#   - Szeneninhalt wie 98391e747; Ende ohne Kampf: Birkin entfernt, Spider-Man liegt tot (Zustand 7), Kamera Cut 0 der Nordtuer.
#
# Je Figur (Leon 50A0/5030/4070, Elza 50A1/5031/4071) zwei Prozesse, Affen per Kill-Flag 8:42/8:43 aus (Weg frei, wie alle Hangar-Laeufe):
#   Lauf 1  Debug-Sprung 4070 an die Tuer (Band 8), Aktionstaste -> Westtuer -> Gang. Wegplan (RE15_WEGPLAN, geschlossen geregelt):
#           durch den Gang nach Norden, Nordtuer -> Monitorraum, zurueck durch die Nordtuer -> Szene -> nach Sueden.
#           Verlangt: (a) "betreten aus 407?: keine Szene" und im ersten Gang-Besuch kein Typ 0x30/0x2B; (b) danach genau eine
#           "Szene Start: Nordtuer aus 503?" und "Szene Ende: Cut 0, Birkin entfernt, Spider-Man liegt"; (c) im zweiten Gang-Besuch ab
#           der ersten Leichenzeile (t=2b st=7) bis zum Ende kein Typ 0x30 und genau EINE unveraenderte Leiche (mindestens 300 Bilder);
#           (d) Kontrolle an der Nordtuer: erste Leichenzeile mit dem Spieler an der Tuer (z > 4000), danach pm=0 und der Spieler
#           geht nach Sueden (z < -20000).
#   Lauf 2  Bank 3 Bit 0x49 (Kraftraum-Birkin besiegt), Debug-Sprung 5030 an die Tuer, Aktionstaste -> Gang. Verlangt: keine Szene,
#           "ohne Birkin (hier besiegt 0, Kraftraum besiegt 1, Szene gesehen 0)", kein Typ 0x30/0x2B im Gang.
# GEGENPROBE (gemessen, scratchpad hangar_aussen/BERICHT_r1.md): exe von 98391e747 -> ROT (von Westen laeuft die Szene, im Gang danach
# Birkin in seiner KI; mit 3:73 ohne Merkerzeile).
#
# Testhaken (kein Spielverhalten): RE15_DEBUG_JUMP, RE15_PLAYER_POS, RE15_INPUT_SCRIPT, RE15_WEGPLAN(_LOG), RE15_SET_FLAG,
# RE15_STATE_LOG, RE15_HANGAR_LOG, RE15_EXIT_AT.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r42_hangar_aussen.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r42_hangar_aussen: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r42_hangar_aussen: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r42hangaraussen_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r42hangaraussen_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# Binaer-sicheres Lesen (debug.log enthaelt Binaerbytes - test_save_counter_pin.cmake): Position des Textes oder -1.
function(_pos _datei _text _out)
    set(${_out} -1 PARENT_SCOPE)
    if(NOT EXISTS "${_datei}")
        return()
    endif()
    file(READ "${_datei}" _lh HEX)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    string(FIND "${_lh}" "${_h}" _p)
    set(${_out} ${_p} PARENT_SCOPE)
endfunction()
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

set(_f "")

function(figur _name _gang _monitor _west _extra)
    string(SUBSTRING "${_west}" 0 4 _wl)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_g "")

    # Wegplan: Gang (von Westen) -> Nordtuer -> Monitorraum -> Nordtuer -> (Szene) -> nach Sueden. Punkte im Gang (ROOM50A0 SCA):
    # Suedgang z -26600..-23400, Halle x -12400..-2700, Nordgang x -13580..-11060; Nordtuer ROOM50A0 main00 Slot 3 @0x0156A,
    # Monitorraum-Tuer ROOM5030 main00 Slot 3 @0x00EC6 (Mitte (-15300,-26300)).
    file(WRITE "${WORKDIR}/weg.txt" "RAUM ${_gang}\nGEHE -24900 -24900\nGEHE -11500 -24900\nGEHE -11700 -11000\nGEHE -12300 3000\n"
                                    "GEHE -12117 4600\nBLICK 3072\nAKTION\nRAUM ${_monitor}\nGEHE -15300 -25800\nBLICK 1024\nAKTION\n"
                                    "RAUM ${_gang}\nWARTE 15\nGEHE -11700 -11000\nGEHE -9000 -22000\nGEHE -11500 -24900\nENDE\n")

    # --- Lauf 1 ------------------------------------------------------------------------------------------------------------------
    re15_start_spiel(_rv1 420
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_HANGAR_LOG=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_SET_FLAG=8:42,8:43
        RE15_DEBUG_JUMP=4070@20
        RE15_PLAYER_POS=-12950,-18950,1024,8
        "RE15_INPUT_SCRIPT=W1,A0.2"
        "RE15_WEGPLAN=${WORKDIR}/weg.txt" RE15_WEGPLAN_LOG=wegplan1.txt
        RE15_STATE_LOG=state1.log
        "RE15_EXIT_AT=850#${_gang}!4"
        "${_exe_kopie}")
    if(EXISTS "${WORKDIR}/debug.log")
        file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug1.log")
    endif()
    set(_d1 "${WORKDIR}/debug1.log")
    _pos("${_d1}" "[hangar] ROOM${_gang} betreten aus ${_wl}: keine Szene" _pw)
    _pos("${_d1}" "[hangar] ROOM${_gang} Szene Start: Nordtuer aus ${_monitor}" _ps)
    _pos("${_d1}" "[hangar] ROOM${_gang} Szene Ende: Cut 0, Birkin entfernt, Spider-Man liegt" _pe)
    _zahl("${_d1}" "Szene Start" _ns)
    _pos("${_d1}" "EXIT_AT: Bild 850" _px)
    set(_kette "West ${_pw} Start ${_ps} (${_ns}x) Ende ${_pe} Exit ${_px}")
    if(_pw LESS 0 OR _ps LESS _pw OR _pe LESS _ps OR NOT _ns EQUAL 1 OR _px LESS 0)
        string(APPEND _g "\n ${_name}: Lauf 1 - Ablauf unvollstaendig/falsch (${_kette}, exit=${_rv1}, ${WORKDIR})")
    endif()
    # state1.log: Abschnitte an den Ruecksprungen des Bildzaehlers (0 Start, 1 4070, 2 Gang von Westen, 3 Monitorraum, 4 Gang)
    file(STRINGS "${WORKDIR}/state1.log" _zl REGEX "^F[0-9]+ ")
    set(_ab 0)
    set(_vor -1)
    set(_fig2 0)
    set(_leiche "")
    set(_leiche_n 0)
    set(_leiche_abw 0)
    set(_birkin_nach 0)
    set(_tuer_z "-")
    set(_frei 0)
    set(_sued 0)
    foreach(_z IN LISTS _zl)
        if(_z MATCHES "^F([0-9]+) ")
            if(CMAKE_MATCH_1 LESS _vor)
                math(EXPR _ab "${_ab} + 1")
            endif()
            set(_vor ${CMAKE_MATCH_1})
        endif()
        if(_ab EQUAL 2 AND _z MATCHES " t=(30|2b) ")
            math(EXPR _fig2 "${_fig2} + 1")
        endif()
        if(_ab EQUAL 4)
            string(REGEX MATCHALL "\\[[0-9]+ t=2b [^]]*\\]" _sm "${_z}")
            if(_leiche STREQUAL "")
                if(_z MATCHES "\\[[0-9]+ t=2b st=7 ")
                    list(GET _sm 0 _leiche)
                    set(_leiche_n 1)
                    string(REGEX MATCH "PL\\(-?[0-9]+,(-?[0-9]+)," _m "${_z}")
                    set(_tuer_z "${CMAKE_MATCH_1}")
                endif()
            else()
                list(LENGTH _sm _nsm)
                if(_nsm EQUAL 1 AND _sm STREQUAL _leiche)
                    math(EXPR _leiche_n "${_leiche_n} + 1")
                else()
                    math(EXPR _leiche_abw "${_leiche_abw} + 1")
                endif()
                if(_z MATCHES " t=30 ")
                    math(EXPR _birkin_nach "${_birkin_nach} + 1")
                endif()
                if(_z MATCHES " pm=0 ")
                    set(_frei 1)
                endif()
                if(_z MATCHES "PL\\(-?[0-9]+,(-?[0-9]+),")
                    if(CMAKE_MATCH_1 LESS -20000)
                        set(_sued 1)
                    endif()
                endif()
            endif()
        endif()
    endforeach()
    if(NOT _ab EQUAL 4)
        string(APPEND _g "\n ${_name}: Lauf 1 - ${_ab} Raumwechsel statt 4 (Weg nicht zu Ende gefahren?)")
    endif()
    if(_fig2 GREATER 0)
        string(APPEND _g "\n ${_name}: Lauf 1 - ${_fig2} state-Zeilen mit Birkin/Spider-Man im Gang von Westen")
    endif()
    if(_leiche_n LESS 300 OR _leiche_abw GREATER 0 OR _birkin_nach GREATER 0)
        string(APPEND _g "\n ${_name}: Lauf 1 - nach der Szene: Leiche ${_leiche_n} Bilder unveraendert, ${_leiche_abw} abweichend, Birkin in ${_birkin_nach} Zeilen (${_leiche})")
    endif()
    if(_tuer_z STREQUAL "-" OR _tuer_z LESS 4000 OR NOT _frei OR NOT _sued)
        string(APPEND _g "\n ${_name}: Lauf 1 - Kontrolle nicht an der Nordtuer oder nicht nach Sueden gegangen (z beim Ende ${_tuer_z}, pm=0 ${_frei}, Sueden ${_sued})")
    endif()

    # --- Lauf 2: Kraftraum-Birkin besiegt (Bank 3 Bit 0x49) ----------------------------------------------------------------------
    re15_start_spiel(_rv2 200
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_HANGAR_LOG=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_SET_FLAG=8:42,8:43,3:73
        RE15_DEBUG_JUMP=5030@20
        RE15_PLAYER_POS=-15300,-26300,1024
        "RE15_INPUT_SCRIPT=W1,A0.2"
        RE15_STATE_LOG=state2.log
        "RE15_EXIT_AT=300#${_gang}"
        "${_exe_kopie}")
    if(EXISTS "${WORKDIR}/debug.log")
        file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug2.log")
    endif()
    _pos("${WORKDIR}/debug2.log" "[hangar] ROOM${_gang} ohne Birkin (hier besiegt 0, Kraftraum besiegt 1, Szene gesehen 0)" _pm)
    _pos("${WORKDIR}/debug2.log" "Szene Start" _s2)
    _pos("${WORKDIR}/debug2.log" "EXIT_AT: Bild 300 in Raum" _px2)
    file(STRINGS "${WORKDIR}/state2.log" _zl2 REGEX " t=(30|2b) ")
    list(LENGTH _zl2 _n2)
    if(_pm LESS 0 OR NOT _s2 LESS 0 OR _n2 GREATER 0 OR _px2 LESS 0)
        string(APPEND _g "\n ${_name}: Lauf 2 (Kraftraum besiegt) - Merkerzeile ${_pm}, Szene ${_s2}, Birkin/Spider-Man-Zeilen ${_n2}, Ende ${_px2} (exit=${_rv2})")
    endif()

    message(STATUS "r42_hangar_aussen ${_name}: Lauf 1 exit=${_rv1} (${_kette}; Gang von Westen ${_fig2} Figurenzeilen, Leiche ${_leiche_n} Bilder "
                   "${_leiche}, Spieler bei Kontrolle z ${_tuer_z}), Lauf 2 exit=${_rv2} (Merker ${_pm}, Figuren ${_n2})${_g}")
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

figur(leon 50A0 5030 4070 "")
figur(elza 50A1 5031 4071 "RE15_TITLE_SHOT_CHAR=1")

file(REMOVE "${_exe_kopie}")
if(_f)
    message(FATAL_ERROR "r42_hangar_aussen:${_f}")
endif()
message(STATUS "r42_hangar_aussen: OK (Leon 50A0 + Elza 50A1, Laufverzeichnis ${_wurzel})")
