# =============================================================================
# SCHEIBEN-RIEGEL MIT DER ECHTEN EXE (Runde 36 Spur H "ziel1190").
#
# NUTZER-VORGABE (AUFTRAG.md Runde 36): die Saetze "This target has a surprisingly large number of bullet
# holes." / "This target does not have many bullet holes" gehoeren HINTEN an die Zielscheiben (beim
# Anklicken), nicht vorne an die Knoepfe; bei Elza (ROOM1191) sind es die anderen zwei Scheiben.
# Dossier analysis/befunde_runde36/H_ziel1190.md, Konstanten include/re15_ziel1190.h.
#
# Der Engine-Riegel unit_r36_ziel1190 prueft die Mechanik auf den Raumdaten; dieser Riegel prueft, was nur
# die exe zeigt: den LADE-Weg (Boot/CONTINUE ohne scd_room_reenter -> main.c-Installer), den echten
# Laufweg vom Durchgang am Tresenende zu jeder Scheibe (Eingabeskript wie die Messlaeufe des Dossiers,
# Abschnitt 1.4) und die Charakterwahl ueber den Spielstand (Elza = Charakter 4 -> ROOM1191, PL04).
#
# Neun Laeufe, jeweils Spielstand aus probe_r36_ziel1190_karte, CONTINUE, Eingabeskript ab Spielbild 150:
#   L0..L3  Leon ROOM1190, Scheibe 0..3 hinten anklicken -> Nachricht 6 / 7 / 6 / 7
#   E0..E3  Elza ROOM1191, Scheibe 0..3 hinten anklicken -> Nachricht 7 / 6 / 7 / 6 (vertauscht), PL04 geladen
#   K       Leon ROOM1190 mit Strom ((4,243)), 620 vor Knopf 0 (Lage -3180,-24500 Blick -x), Aktion ->
#           die ORIGINAL-Nachricht 0 ("There is a switch here. Push it?") und KEIN Scheiben-Satz
# Geprueft wird das LOG (debug.log): die Zeilen "[ziel1190] ROOM<raum> Scheibe <n> angeklickt (Platz <17+n>)
# -> Nachricht <id>" (ziel_1190.c re15_ziel1190_show, work_vars[0] = Platz @0x80042f3c) bzw.
# "[msg] room=1190 id=0" (scd_vm.c op_message_on unter RE15_MSG_LOG).
#
# Laufskript (Dossier 1.4): "XU3.5,L0.36667,U<t>,R0.36667,U1,A0.1,W6", t = 6.05 / 4.45 / 2.85 / 1.25 fuer
# Scheibe 0..3; Aktion bei Spielbild 150 + 30*(3.5+0.36667+t+0.36667+1) = 489 / 441 / 393 / 345, Ende
# jeweils 200 Bilder spaeter (EXIT_AT). RE15_SOFTWARE_RENDER=1 dient nur der Robustheit des Testhakens.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_ziel1190_karte> -DWORKDIR=<dir>
#               -P test_r36_ziel1190_scheiben.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r36_ziel1190_scheiben: RE15_PC_EXE fehlt/existiert nicht: ${RE15_PC_EXE}")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r36_ziel1190_scheiben: RE15_KARTE_TOOL fehlt: ${RE15_KARTE_TOOL}")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r36_ziel1190_scheiben: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# EIGENER EXE-NAME (Muster test_r34n_b_cursor.cmake): local_build.sh beendet beim Bau JEDE re15_pc.exe der
# Maschine; die Kopie liegt neben der exe (Asset-Wurzel = exe-Verzeichnis) und traegt einen eigenen Namen.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r36h_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r36h_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(_hex_von _text _out)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(${_out} "${_h}" PARENT_SCOPE)
endfunction()

function(_hat _log _text _out)
    _hex_von("${_text}" _h)
    string(FIND "${_log}" "${_h}" _p)
    if(_p LESS 0)
        set(${_out} 0 PARENT_SCOPE)
    else()
        set(${_out} 1 PARENT_SCOPE)
    endif()
endfunction()

# Ein Lauf: Karte schreiben, exe starten, debug.log als HEX nach _out_hex.
#   _raum    1190 | 1191      _karte  weitere Argumente fuer das Kartenwerkzeug (Liste)
#   _skript  RE15_INPUT_SCRIPT   _ende  RE15_EXIT_AT-Bild
function(ziel_lauf _name _raum _karte _skript _ende _out_hex)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}" ${_karte}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r36_ziel1190_scheiben[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_MSG_LOG=1
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=150
        "RE15_INPUT_SCRIPT=${_skript}"
        "RE15_EXIT_AT=${_ende}#${_raum}"
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r36_ziel1190_scheiben[${_name}]: kein debug.log (exit=${_rv})")
    endif()
    file(READ "${WORKDIR}/debug.log" _lh HEX)
    _hat("${_lh}" "CONTINUE: resumed in room ${_raum}" _ok)
    if(NOT _ok)
        message(FATAL_ERROR "r36_ziel1190_scheiben[${_name}]: der Spielstand in ROOM${_raum} wurde nicht "
                            "geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    _hat("${_lh}" "EXIT_AT: Bild" _ok)
    if(NOT _ok)
        message(FATAL_ERROR "r36_ziel1190_scheiben[${_name}]: das Endbild wurde nicht erreicht - der Lauf "
                            "ist vorher abgerissen (exit=${_rv}, ${WORKDIR})")
    endif()
    set(${_out_hex} "${_lh}" PARENT_SCOPE)
endfunction()

set(_fehler "")

# ---- L0..L3 / E0..E3: Scheiben hinten --------------------------------------------------------------
set(_t_0 "6.05")
set(_t_1 "4.45")
set(_t_2 "2.85")
set(_t_3 "1.25")
set(_ende_0 689)
set(_ende_1 641)
set(_ende_2 593)
set(_ende_3 545)
foreach(_raum 1190 1191)
    foreach(_n 0 1 2 3)
        # Leon: Obj 0+2 viele (6), 1+3 wenige (7); Elza vertauscht (ziel_1190.c re15_ziel1190_viele).
        math(EXPR _gerade "${_n} % 2")
        if(_raum STREQUAL "1190")
            if(_gerade EQUAL 0)
                set(_soll 6)
            else()
                set(_soll 7)
            endif()
        else()
            if(_gerade EQUAL 0)
                set(_soll 7)
            else()
                set(_soll 6)
            endif()
        endif()
        math(EXPR _platz "17 + ${_n}")
        if(_raum STREQUAL "1190")
            set(_name "L${_n}")
        else()
            set(_name "E${_n}")
        endif()
        ziel_lauf("${_name}" "${_raum}" "" "XU3.5,L0.36667,U${_t_${_n}},R0.36667,U1,A0.1,W6" "${_ende_${_n}}" _lh)
        set(_z "[ziel1190] ROOM${_raum} Scheibe ${_n} angeklickt (Platz ${_platz}) -> Nachricht ${_soll}")
        _hat("${_lh}" "${_z}" _ok)
        if(NOT _ok)
            string(APPEND _fehler "${_name}: fehlt: ${_z}\n")
        endif()
        _hat("${_lh}" "[msg] room=${_raum} id=0" _knopf)
        if(_knopf)
            string(APPEND _fehler "${_name}: Knopf-Nachricht 0 statt Scheiben-Text\n")
        endif()
        set(_pl 1)
        if(_raum STREQUAL "1191")
            _hat("${_lh}" "[pl] Spieler-Familie PL04" _pl)
            if(NOT _pl)
                string(APPEND _fehler "${_name}: Elza (PL04) nicht geladen\n")
            endif()
        endif()
        message(STATUS "r36_ziel1190_scheiben[${_name}]: ROOM${_raum} Scheibe ${_n} -> Nachricht ${_soll} gesehen=${_ok} Knopf=${_knopf} PL04=${_pl}")
    endforeach()
endforeach()

# ---- K: Knopf 0 mit Strom = Original-Nachricht 0, kein Scheiben-Satz ------------------------------
ziel_lauf("K" "1190" "strom;pos=-3180,-24500,2048" "A0.1,W3" 400 _lh)
_hat("${_lh}" "[msg] room=1190 id=0" _ok)
if(NOT _ok)
    string(APPEND _fehler "K: Original-Nachricht 0 am Knopf 0 fehlt\n")
endif()
_hat("${_lh}" "angeklickt" _falsch)
if(_falsch)
    string(APPEND _fehler "K: Scheiben-Satz am Knopf\n")
endif()
message(STATUS "r36_ziel1190_scheiben[K]: Knopf 0 msg0=${_ok} Scheiben-Satz=${_falsch}")

file(REMOVE "${_exe_kopie}")
if(NOT _fehler STREQUAL "")
    message(FATAL_ERROR "r36_ziel1190_scheiben FAIL:\n${_fehler}")
endif()
message(STATUS "r36_ziel1190_scheiben: OK (9 Laeufe)")
