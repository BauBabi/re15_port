# =============================================================================
# IRONS-TISCH-LICHT-PIN (Runde 30 Nachschliff, Spur tischlicht).
#
# BEFUND: Irons Diary (obj 5) und Memory Card (obj 6) - beide portseitig auf Irons'
# Schreibtisch ergaenzt - waren in der Nahaufnahme Cut 6 fast schwarz: Mittel-RGB (17,16,8)
# und (17,17,10), waehrend der gemalte Tisch darum hell ist. Ursache: der ausgelieferte
# Lichtsatz von Cut 6, ROOM1150.RDT/ROOM1151.RDT @0x00488 (ambient 40,40,24).
#
# PORT-WAHL (keine Original-Adresse, include/re15_irons_tisch.h "LICHTSATZ DER ZWEI PROPS"):
# NUR diese zwei Props nehmen in jedem Cut den Lichtsatz von Cut 2 (@0x003E8, ambient
# 110,90,84) - der vorhandene Satz, der sie in beiden Cuts, in denen sie zu sehen sind (2, 6),
# am naechsten an die gemalte Umgebung bringt (Messtabelle im Kopf).
#
# DIESER RIEGEL faehrt die ECHTE exe viermal am Lade-Weg (CONTINUE, ROOM1150) und liest je EIN
# Bild ueber RE15_FRAMEDUMP (beschleunigter Renderer, KEIN SOFTWARE_RENDER, KEIN AUTOSHOT):
#   P6  Cut 6, nichts genommen     P60 Cut 6, beides genommen (Nullbild)
#   P2  Cut 2, nichts genommen     P20 Cut 2, beides genommen
# Spieler jeweils weit weg (-20500, -24500). probe_r30_irons_tisch_licht verlangt:
#   - Cut 6: Helligkeit von Buch und Karte INNERHALB der Spanne der gemalten Umgebung
#     [Y gemaltes Buch .. Y gemaltes Klemmbrett] im Nullbild desselben Cuts,
#   - je Prop: in Cut 6 nicht dunkler gegen die Malerei als in Cut 2 (dY6 >= dY2).
# NEGATIV-KONTROLLE (gemessen, Dossier Abschnitt 10): mit der exe VOR der Aenderung
# (Lichtsatz des aktiven Cuts, Cut 6 Y 15,6 / 16,5) ist der Riegel rot.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r30_irons_tisch_karte>
#               -DRE15_LICHT_TOOL=<probe_r30_irons_tisch_licht> -DWORKDIR=<dir>
#               -P test_r30_irons_tisch_licht.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "irons_tisch_licht: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "irons_tisch_licht: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT RE15_LICHT_TOOL OR NOT EXISTS "${RE15_LICHT_TOOL}")
    message(FATAL_ERROR "irons_tisch_licht: RE15_LICHT_TOOL fehlt: '${RE15_LICHT_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "irons_tisch_licht: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

function(hexof _text _out)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(${_out} "${_h}" PARENT_SCOPE)
endfunction()
hexof("CONTINUE: resumed in room 1150" _hex_continue)
hexof("-> bild.ppm" _hex_dump)
hexof("EXIT_AT: Bild" _hex_exit)

# Bild 100 im Raum (wie integration_r30_irons_tisch_bild: der Raum steht ab dort).
set(_bild 100)
math(EXPR _ende "${_bild} + 1")

function(licht_lauf _name _cut _genommen _out_ppm)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_karg "")
    if(_genommen)
        set(_karg "genommen")
    endif()
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "1150" ${_karg} "pos=-20500,-24500,0"
                    WORKING_DIRECTORY "${WORKDIR}"
                    TIMEOUT 60
                    RESULT_VARIABLE _rvk
                    OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "irons_tisch_licht[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 180
        RE15_NO_INTRO=1
        RE15_NOAUDIO=1
        RE15_CONTINUE_TEST=1
        RE15_CARD_AUTO=1
        RE15_CARD_SLOT=0
        RE15_IRONS_LOG=1
        "RE15_FORCE_CUT=${_cut}"
        "RE15_FRAMEDUMP=${_bild}:bild.ppm"
        "RE15_EXIT_AT=${_ende}#1150"
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "irons_tisch_licht[${_name}]: kein debug.log (exit=${_rv})")
    endif()
    file(READ "${WORKDIR}/debug.log" _lh HEX)
    string(FIND "${_lh}" "${_hex_continue}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "irons_tisch_licht[${_name}]: der Lauf hat den Spielstand in "
                            "ROOM1150 nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_lh}" "${_hex_dump}" _p)
    if(_p LESS 0 OR NOT EXISTS "${WORKDIR}/bild.ppm")
        message(FATAL_ERROR "irons_tisch_licht[${_name}]: kein Framedump von Bild ${_bild} "
                            "(exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_lh}" "${_hex_exit}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "irons_tisch_licht[${_name}]: Bild ${_ende} in ROOM1150 nicht "
                            "erreicht (keine EXIT_AT-Zeile, exit=${_rv}, ${WORKDIR})")
    endif()
    set(${_out_ppm} "${WORKDIR}/bild.ppm" PARENT_SCOPE)
endfunction()

licht_lauf(P6  6 FALSE _ppm_p6)
licht_lauf(P60 6 TRUE  _ppm_p60)
licht_lauf(P2  2 FALSE _ppm_p2)
licht_lauf(P20 2 TRUE  _ppm_p20)

execute_process(COMMAND "${RE15_LICHT_TOOL}" "${_ppm_p6}" "${_ppm_p60}" "${_ppm_p2}" "${_ppm_p20}"
                TIMEOUT 60
                RESULT_VARIABLE _rvb
                OUTPUT_VARIABLE _outb)
message(STATUS "irons_tisch_licht: Auswertung\n${_outb}")
if(NOT _rvb EQUAL 0)
    message(FATAL_ERROR
        "irons_tisch_licht: Buch/Karte in Cut 6 nicht in der Helligkeit der gemalten Umgebung "
        "(Auswerter exit=${_rvb}).\nDer Prop-Zeichner (main.c) muss fuer obj 5/6 in "
        "ROOM1150/1151 den Lichtsatz re15_irons_tisch_licht_cut() nehmen (Cut 2, RDT "
        "@0x003E8); mit dem Satz des aktiven Cuts 6 (@0x00488) sind beide fast schwarz.\n${_outb}")
endif()

message(STATUS "irons_tisch_licht: OK - Buch und Karte liegen in Cut 6 in der Helligkeit "
               "der gemalten Umgebung")
