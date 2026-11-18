# integration_r42_lageplaene3_weg — Runde 42 Strang lageplaene_nach: der Revierplan an der Tuerkamera der Lobby, am ECHTEN
# Weg der exe fuer BEIDE Figuren.
#
# Leon (1030): Start (-15000,-14000) Blick Ost in Kamera 4 (dort kommt er aus dem Westflur ueber RVD 5->4 an), geht die Lobby
#   entlang nach Osten zur Ausgangstuer nach 1050 (Door_aot_set Slot 0 @0x01C6A).
# Elza (1031): Start an ihrem Spielbeginn (-8200,-22500) Blick 1024 (Rueckkehrtuer Slot 19 @0x02082 next (-8200,0,-22500)
#   rot 1024), Intro gesehen ((3,0xC1),(3,0x7D)), dreht nach Sueden, geht vor dem Tresen vorbei nach Osten zur Tuer.
# Geprueft im debug.log:
#   a) der Raumstart hat den Plan angelegt ("[kartenitems] ROOMxxxx: Karte Blatt 2"),
#   b) auf dem Weg schaltet die Kamera auf 2 ([walk] ... cut=2), BEVOR der Plan genommen wird - Kamera 2 ist die Tuerkamera
#      (RVD 1->2 @0x00294 / 3->2 @0x0030C, Kopf re15_kartenitems_r36.h RUNDE 42 NACHARBEIT),
#   c) die Aktionstaste im Streifen zwischen Bank und Tuer startete das Port-Programm (Ereignis 28), nicht das Klettern
#      (kein " mo=23"),
#   d) Meldung 31 ("You've taken a / police station map.") lief.
# Wege gemessen 2026-11-18 (Gehen ~73 Einheiten/Bild; Leon blieb bei z -14300 an der Bank haengen, SCA-Zelle 14 @0x01AB8
# z -14744 + Spielerradius -> Weg auf z -14000): Leon F300 cut=2 bei x -1425, Halt (1930,-14000); Elza F390 cut=2 bei x 911,
# Halt (1586,-13941).
# Gegenprobe: auf bce9c91a8 (Plan an der Rueckwand) und aaea5f8c2 startet die Aktion dort kein Ereignis 28 -> c) d) rot.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_schluessel_karte> -DWORKDIR=<dir> -P test_r42_lageplaene3_weg.cmake

foreach(_v RE15_PC_EXE RE15_KARTE_TOOL)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "lageplaene3_weg: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "lageplaene3_weg: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

function(hexof _text _out)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(${_out} "${_h}" PARENT_SCOPE)
endfunction()

# name | raum | x | z | blick | cut | skript | ende | kartenargumente ("+"-getrennt)
set(_faelle
    "leon|1030|-15000|-14000|0|4|W2,U7.75,W1,A0.1,W3,A0.1,W3,A0.1,W3|760|"
    "elza|1031|-8200|-22500|1024|0|W2,R0.7,U3.78,R0.35,U4.5,W1,A0.1,W3,A0.1,W3,A0.1,W3|950|elza+flag:3:0xC1+flag:3:0x7D")

set(_fehler "")
foreach(_f ${_faelle})
    string(REPLACE "|" ";" _t "${_f}")
    list(GET _t 0 _n)
    list(GET _t 1 _raum)
    list(GET _t 2 _x)
    list(GET _t 3 _z)
    list(GET _t 4 _rot)
    list(GET _t 5 _cut)
    list(GET _t 6 _skript)
    list(GET _t 7 _ende)
    list(LENGTH _t _nt)
    set(_argl "")
    if(_nt GREATER 8)
        list(GET _t 8 _args)
        string(REPLACE "+" ";" _argl "${_args}")
    endif()
    set(WORKDIR "${_basis}_${_n}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}" ${_x} ${_z} ${_rot} "cut:${_cut}" ${_argl}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        string(APPEND _fehler "${_n}: Kartenwerkzeug exit=${_rvk} ${_outk}\n")
        continue()
    endif()
    re15_start_spiel(_rv 300 RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1
                     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
                     RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=${_skript}"
                     "RE15_EXIT_AT=${_ende}#${_raum}" "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        string(APPEND _fehler "${_n}: kein debug.log (exit=${_rv})\n")
        continue()
    endif()
    file(READ "${WORKDIR}/debug.log" _lh HEX)
    hexof("CONTINUE: resumed in room" _h0)
    hexof("[kartenitems] ROOM${_raum}: Karte Blatt 2 " _h1)
    hexof(" cut=2 mo=" _hc2)
    hexof("[kartenitems] ROOM${_raum} Ereignis 28 -> Port-Programm Karte Blatt 2" _h2)
    string(TOLOWER "${_raum}" _raum_k)
    hexof("[msg] room=${_raum_k} id=31" _h3)
    hexof(" mo=23" _hk)
    foreach(_p h0 h1 hc2 h2 h3 hk)
        string(FIND "${_lh}" "${_${_p}}" _pos_${_p})
    endforeach()
    message(STATUS "lageplaene3_weg ${_n} ROOM${_raum}: geladen ${_pos_h0}, Plan ${_pos_h1}, erste Kamera 2 ${_pos_hc2}, "
                   "Ereignis ${_pos_h2}, Meldung 31 ${_pos_h3}, Klettern ${_pos_hk} (${WORKDIR})")
    if(_pos_h0 LESS 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: Spielstand nicht geladen (exit=${_rv})\n")
    endif()
    if(_pos_h1 LESS 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: kein Revierplan angelegt\n")
    endif()
    if(_pos_h2 LESS 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: Aktion vor der Ostwand startete das Karten-Programm nicht (Ereignis 28)\n")
    elseif(_pos_hc2 LESS 0 OR NOT _pos_hc2 LESS _pos_h2)
        string(APPEND _fehler "${_n} ROOM${_raum}: Kamera 2 (Tuerkamera) kam auf dem Weg nicht vor der Aufnahme\n")
    endif()
    if(_pos_h3 LESS 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: Meldung 31 (\"You've taken a / police station map.\") lief nicht\n")
    endif()
    if(NOT _pos_hk LESS 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: die Aktion startete das Klettern (Bewegung 230/231)\n")
    endif()
endforeach()

if(_fehler)
    message(FATAL_ERROR "lageplaene3_weg: der echte Weg nimmt den Revierplan nicht an der Tuerkamera.\n${_fehler}")
endif()
message(STATUS "lageplaene3_weg: Leon 1030 und Elza 1031 gehen zur Ausgangstuer, Kamera 2 zeigt den Plan, Aktion nimmt ihn")
