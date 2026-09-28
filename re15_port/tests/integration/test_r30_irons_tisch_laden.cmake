# =============================================================================
# IRONS-TISCH-LADE-PIN (Runde 30, Thema irons-diary-welt).
#
# Buch (obj 5) und Memory Card (obj 6) auf Irons' Schreibtisch muessen auch dann da sein,
# wenn ROOM1150/1151 der STARTRAUM eines geladenen Spielstands ist. Der Boot-/CONTINUE-Weg
# in platform/pc/main.c geht NICHT durch scd_room_reenter (gemessen an der Sicherung,
# analysis/befunde_runde30/sicherung.md §2.4: 25 Bilder pixelgleich mit dem Lauf ohne
# Prop); re15_irons_tisch_install steht deshalb an BEIDEN Wegen, jeweils direkt hinter
# re15_sicherung_install.
#
# ORIGINAL: EIN Raumlader FUN_800396fc mit zwei Aufrufern,
#     8001d5ac: jal 0x800396fc      ; Session-Start / LOAD
#     8001d988: jal 0x800396fc      ; Tuer
#
# DER RIEGEL, drei Laeufe mit der ECHTEN exe (CONTINUE, Cut 2 erzwungen, Ende Bild 120):
#   A  ROOM1150, nichts genommen: das debug.log MUSS tragen
#         CONTINUE: resumed
#         [irons-tisch] Boot-Weg: Prop obj_id=5 ...   und   ... obj_id=6 ...
#         [prop-render] pi=5 oid=0x05                und   [prop-render] pi=6 oid=0x06
#         die erste Zustandszeile (RE15_IRONS_LOG) mit zone7=1 zone8=1 prop5=1 prop6=1
#   B  ROOM1150, beides genommen (Bits (9,54)/(9,55) im Spielstand): KEINE Boot-Weg-Zeile,
#      KEIN Zeichnen von obj 5/6, Zustand flag54=1 flag55=1 zone7=0 zone8=0 prop5=-1 prop6=-1
#      (die Flags werden VOR dem Anlegen zurueckgeladen)
#   C  ROOM1151 (Elza-Variante), nichts genommen: wie A
#
# Er DISKRIMINIERT: ohne den Aufruf am Lade-Weg fehlen in A und C die Boot-Weg-Zeilen und
# das Zeichnen; laeuft das Anlegen vor dem Flag-Restore, traegt B die Props.
# RE15_SOFTWARE_RENDER=1 dient nur der Robustheit des Testhakens; geprueft wird das LOG.
# ⛔ Die Zeile "[prop-render] pi=5/6" steht VOR der Tiefen-Klemme: dieser Riegel sagt NICHTS
# darueber, ob die Props zu SEHEN sind (Gegenpruefer-Mutation: Klemme weg -> 0 Pixel, dieser
# Riegel gruen). Das SICHTBARE Ergebnis haelt integration_r30_irons_tisch_bild fest.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe> -DWORKDIR=<dir>
#               -P test_r30_irons_tisch_laden.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "irons_tisch_laden: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "irons_tisch_laden: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "irons_tisch_laden: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

function(hexof _text _out)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(${_out} "${_h}" PARENT_SCOPE)
endfunction()
hexof("CONTINUE: resumed" _hex_continue)
hexof("EXIT_AT: Bild" _hex_exit)
hexof("[irons-tisch] Boot-Weg: Prop obj_id=5" _hex_boot5)
hexof("[irons-tisch] Boot-Weg: Prop obj_id=6" _hex_boot6)
hexof("[prop-render] pi=5 oid=0x05" _hex_render5)
hexof("[prop-render] pi=6 oid=0x06" _hex_render6)
hexof("oid=0x05" _hex_oid5)
hexof("oid=0x06" _hex_oid6)
hexof("[irons-tisch] F" _hex_zustand)
hexof("flag54=0 flag55=0 zone7=1 zone8=1 prop5=1 prop6=1" _hex_da)
hexof("flag54=1 flag55=1 zone7=0 zone8=0 prop5=-1 prop6=-1" _hex_weg)

function(tisch_lauf _name _raum _karten_arg _out_hex)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}" ${_karten_arg}
                    WORKING_DIRECTORY "${WORKDIR}"
                    TIMEOUT 60
                    RESULT_VARIABLE _rvk
                    OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "irons_tisch_laden[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 180
        RE15_NO_INTRO=1
        RE15_NOAUDIO=1
        RE15_SOFTWARE_RENDER=1
        RE15_CONTINUE_TEST=1
        RE15_CARD_AUTO=1
        RE15_CARD_SLOT=0
        RE15_IRONS_LOG=1
        RE15_FORCE_CUT=2
        "RE15_EXIT_AT=120#${_raum}"
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "irons_tisch_laden[${_name}]: kein debug.log (exit=${_rv})")
    endif()
    file(READ "${WORKDIR}/debug.log" _lh HEX)
    string(FIND "${_lh}" "${_hex_continue}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "irons_tisch_laden[${_name}]: der Lauf hat nicht geladen "
                            "(keine CONTINUE-Zeile, exit=${_rv})")
    endif()
    string(FIND "${_lh}" "${_hex_exit}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "irons_tisch_laden[${_name}]: Bild 120 in ROOM${_raum} nicht "
                            "erreicht (keine EXIT_AT-Zeile, exit=${_rv})")
    endif()
    set(${_out_hex} "${_lh}" PARENT_SCOPE)
endfunction()

# erste Zustandszeile (RE15_IRONS_LOG) aus dem HEX-Log
function(erste_zustandszeile _lh _out)
    string(FIND "${_lh}" "${_hex_zustand}" _p)
    if(_p LESS 0)
        set(${_out} "" PARENT_SCOPE)
        return()
    endif()
    string(SUBSTRING "${_lh}" ${_p} 360 _z)
    set(${_out} "${_z}" PARENT_SCOPE)
endfunction()

foreach(_fall a1150 c1151)
    if(_fall STREQUAL "a1150")
        set(_raum 1150)
    else()
        set(_raum 1151)
    endif()
    tisch_lauf(${_fall} ${_raum} "" _log)
    foreach(_h _hex_boot5 _hex_boot6)
        string(FIND "${_log}" "${${_h}}" _p)
        if(_p LESS 0)
            message(FATAL_ERROR
                "irons_tisch_laden[${_fall}]: nach dem Laden in ROOM${_raum} liegt ${_h} "
                "NICHT im Prop-Pool — re15_irons_tisch_install fehlt am Boot-/CONTINUE-Weg "
                "(Original: FUN_800396fc, @0x8001d5ac LOAD und @0x8001d988 Tuer).")
        endif()
    endforeach()
    foreach(_h _hex_render5 _hex_render6)
        string(FIND "${_log}" "${${_h}}" _p)
        if(_p LESS 0)
            message(FATAL_ERROR
                "irons_tisch_laden[${_fall}]: ${_h} wird im Cut 2 nie gezeichnet.")
        endif()
    endforeach()
    erste_zustandszeile("${_log}" _z)
    string(FIND "${_z}" "${_hex_da}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR
            "irons_tisch_laden[${_fall}]: die erste Zustandszeile meldet nicht "
            "'flag54=0 flag55=0 zone7=1 zone8=1 prop5=1 prop6=1' — Zonen oder Props fehlen.")
    endif()
endforeach()

tisch_lauf(b1150 1150 "genommen" _log_b)
foreach(_h _hex_boot5 _hex_boot6 _hex_oid5 _hex_oid6)
    string(FIND "${_log_b}" "${${_h}}" _p)
    if(NOT _p LESS 0)
        message(FATAL_ERROR
            "irons_tisch_laden[B]: beides ist im Spielstand GENOMMEN (Bits (9,54)/(9,55)), "
            "trotzdem steht ${_h} im Log — das Anlegen laeuft vor dem Restore der Flags.")
    endif()
endforeach()
erste_zustandszeile("${_log_b}" _z)
string(FIND "${_z}" "${_hex_weg}" _p)
if(_p LESS 0)
    message(FATAL_ERROR
        "irons_tisch_laden[B]: die erste Zustandszeile meldet nicht "
        "'flag54=1 flag55=1 zone7=0 zone8=0 prop5=-1 prop6=-1'.")
endif()

message(STATUS "irons_tisch_laden: OK — am Lade-Weg liegen Buch und Karte in ROOM1150 und "
               "ROOM1151 samt Zonen und werden gezeichnet; genommen bleiben beide weg")
