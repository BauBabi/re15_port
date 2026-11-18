# integration_r42_lageplaene2_weg — Runde 42 Strang lageplaene, Stufe 2: die Plaene STAGE2..6 am ECHTEN Weg der exe.
#
# Je Gebiet ein Lauf (Lade-Weg CONTINUE, Speicherkarte von probe_r36_schluessel_karte mit Boden-y und Figur): die Figur geht
# auf dem Gehweg zum Plakat, dreht zur Wand, Aktion -> Frage -> Ja -> Meldung. Geprueft im debug.log:
#   a) der Raumstart hat das Plakat angelegt ("[kartenitems] ROOMxxxx: Karte Blatt <b>"),
#   b) die Aktionstaste startete das Port-Programm ("Ereignis 28 -> Port-Programm"), NICHT die Klettersonde (kein Bewegungs-
#      wert 230/231 = Kletter-Substate 9 in den [walk]-Zeilen; gemessen 2026-11-18 mit Plakat im Spielerband: Leon kletterte in
#      2000 auf y -3600, FUN_8002d2c0 @0x8002d370),
#   c) die Meldung 31 ("You've taken / the <Plan>.") lief (RE15_MSG_LOG: "[msg] room=xxxx id=31", scd_vm.c Message_on).
# Wege (gemessen in den Videos 2026-11-18, Gehen ~72 Einheiten/Bild, Drehen 96/Bild):
#   kanal  Leon 2000 (4200,-1800,-20350) Blick 2048, U1.1 -> x 1725, R0.36 -> Blick 3104 (Nordwand)
#   fabrik Elza 30C1 (-7600,0,-2500) Blick 3072, U1.2 -> z 200 (Nordwand)
#   ubahn  Leon 6000 (4500,-3600,-400) Blick 2048, U0.85 -> x 2550 (Rueckwand des Absatzes); Flag (3,0x64) = Annettes
#          Ankunftsszene (6000 sub00 @0x00F44 Ck(3,0x64)==0) schon gesehen
# Gegenprobe: auf aaea5f8c2 gibt es in diesen Raeumen kein Plakat -> a) bis c) rot.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_schluessel_karte> -DWORKDIR=<dir> -P test_r42_lageplaene2_weg.cmake

foreach(_v RE15_PC_EXE RE15_KARTE_TOOL)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "lageplaene2_weg: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "lageplaene2_weg: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

function(hexof _text _out)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(${_out} "${_h}" PARENT_SCOPE)
endfunction()

# name | raum | x | z | blick | cut | blatt | skript | kartenargumente ("+"-getrennt)
set(_faelle
    "kanal|2000|4200|-20350|2048|1|6|W2,U1.1,W0.5,R0.36,W1,A0.1,W3,A0.1,W4,A0.1,W3|y:-1800"
    "fabrik|30C1|-7600|-2500|3072|1|7|W2,U1.2,W1,A0.1,W3,A0.1,W4,A0.1,W3|elza"
    "ubahn|6000|4500|-400|2048|0|12|W2,U0.85,W1,A0.1,W3,A0.1,W4,A0.1,W3|y:-3600+flag:3:0x64")

set(_fehler "")
foreach(_f ${_faelle})
    string(REPLACE "|" ";" _t "${_f}")
    list(GET _t 0 _n)
    list(GET _t 1 _raum)
    list(GET _t 2 _x)
    list(GET _t 3 _z)
    list(GET _t 4 _rot)
    list(GET _t 5 _cut)
    list(GET _t 6 _blatt)
    list(GET _t 7 _skript)
    list(GET _t 8 _args)
    set(WORKDIR "${_basis}_${_n}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    string(REPLACE "+" ";" _argl "${_args}")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}" ${_x} ${_z} ${_rot} "cut:${_cut}" ${_argl}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        string(APPEND _fehler "${_n}: Kartenwerkzeug exit=${_rvk} ${_outk}\n")
        continue()
    endif()
    re15_start_spiel(_rv 240 RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1
                     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
                     RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=${_skript}"
                     "RE15_EXIT_AT=720#${_raum}" "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        string(APPEND _fehler "${_n}: kein debug.log (exit=${_rv})\n")
        continue()
    endif()
    file(READ "${WORKDIR}/debug.log" _lh HEX)
    hexof("CONTINUE: resumed in room" _h0)
    hexof("[kartenitems] ROOM${_raum}: Karte Blatt ${_blatt} " _h1)
    hexof("[kartenitems] ROOM${_raum} Ereignis 28 -> Port-Programm Karte Blatt ${_blatt}" _h2)
    string(TOLOWER "${_raum}" _raum_k)
    hexof("[msg] room=${_raum_k} id=31" _h3)
    hexof(" mo=23" _hk)
    foreach(_p h0 h1 h2 h3 hk)
        string(FIND "${_lh}" "${_${_p}}" _pos_${_p})
    endforeach()
    message(STATUS "lageplaene2_weg ${_n} ROOM${_raum}: geladen ${_pos_h0}, Plakat ${_pos_h1}, Ereignis ${_pos_h2}, Meldung 31 ${_pos_h3}, Klettern ${_pos_hk} (${WORKDIR})")
    if(_pos_h0 LESS 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: Spielstand nicht geladen (exit=${_rv})\n")
    endif()
    if(_pos_h1 LESS 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: kein Plakat Blatt ${_blatt} angelegt\n")
    endif()
    if(_pos_h2 LESS 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: Aktion vor der Wand startete das Karten-Programm nicht (Ereignis 28)\n")
    endif()
    if(_pos_h3 LESS 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: Meldung 31 (\"You've taken ...\") lief nicht\n")
    endif()
    if(NOT _pos_hk LESS 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: die Aktion startete das Klettern (Bewegung 230/231)\n")
    endif()
endforeach()

if(_fehler)
    message(FATAL_ERROR "lageplaene2_weg: der echte Weg nimmt den Plan nicht.\n${_fehler}")
endif()
message(STATUS "lageplaene2_weg: Kanalisation (Leon 2000), Fabrik (Elza 30C1), U-Bahn (Leon 6000) am echten Weg genommen")
