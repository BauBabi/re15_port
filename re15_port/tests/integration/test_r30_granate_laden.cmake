# =============================================================================
# GRANATEN-RIEGEL MIT DER ECHTEN EXE (Runde 30, Nachtrag K: Handgranate im Hebetisch).
#
# Dossier analysis/befunde_runde30/nachtrag-granate.md, Konstanten include/re15_granate.h.
# Der Engine-Riegel unit_r30_granate prueft Modell, Sitz und die Fahrten Bild fuer Bild; dieser
# Riegel prueft, was nur die exe zeigt: den LADE-Weg (Boot/CONTINUE geht nicht durch
# scd_room_reenter; Original EIN Raumlader FUN_800396fc, `jal 0x800396fc` @0x8001d5ac LOAD und
# @0x8001d988 Tuer), die Reihenfolge der Modale im Spiel und das CHECK-Foto.
#
# Fuenf Laeufe, jeweils Spielstand aus probe_r30_granate_karte, CONTINUE, Hebetisch per
# RE15_FIRE_AOT=1@90 ausloesen (Aot slot 1 -> sub04), Ende RE15_EXIT_AT=280:
#   A  ROOM1150, Sicherung schon genommen (9,53): Granate im Pool, gezeichnet (oid=0x07), und
#      ihr Modal geht in der Fahrt auf
#   B  ROOM1150, (9,53) und (9,56): KEINE Granaten-Zeile (NEGATIV-KONTROLLE: Flag vor dem Anlegen)
#   C  ROOM1150, nichts genommen: das Sicherungs-Modal geht auf, das der Granate NICHT (es wartet
#      hinter dem unbeantworteten Sicherungs-Modal — "erst Sicherung, dann Granate")
#   D  ROOM1151 (Elza), (9,53): wie A
#   E  ROOM1150, Granate in Inventarplatz 0, RE15_INV_CHECK_SHOT: das CHECK-Foto von Item 0x09
#      wird an das Fenster des RE1.5-Schirms geladen (ITPS-Block 0x09 @0x1B000 traegt crect
#      (0,489) / prect (832,256); Foto-Lader DEBUG.BIN @0x800c0258, LoadImage @0x800c0280/@0x800c02a0)
#
# RE15_SOFTWARE_RENDER=1 dient nur der Robustheit des Testhakens; geprueft wird das LOG. Ob die
# Granate im Fach SICHTBAR ist, belegt die Framedump-Abnahme im Dossier.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe> -DWORKDIR=<dir>
#               -P test_r30_granate_laden.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "granate_laden: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "granate_laden: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "granate_laden: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

function(_hex_von _text _out)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(${_out} "${_h}" PARENT_SCOPE)
endfunction()

# Ein Lauf: Karte schreiben, exe starten, debug.log als HEX nach _out_hex.
#   _raum     1150 | 1151
#   _karte    Argumente fuer das Kartenwerkzeug (Liste)
#   _check    1 = Statusschirm + CHECK auf Platz 0 statt Hebetisch
function(granate_lauf _name _raum _karte _check _out_hex)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}" ${_karte}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "granate_laden[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    if(_check)
        re15_start_spiel(_rv 180
            RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
            RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
            RE15_INV_SHOT=inv.bmp          # Schirm auf in F30, Abzug + Ende in F140
            RE15_INV_CHECK_SHOT=1          # CHECK auf Inventarplatz 0 = die Granate
            "${RE15_PC_EXE}")
    else()
        re15_start_spiel(_rv 180
            RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
            RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
            "RE15_FIRE_AOT=1@90#${_raum}"    # Hebetisch ausloesen: Aot slot 1 -> sub04
            "RE15_EXIT_AT=280#${_raum}"      # Modal erst in der Ruhe oben, Bild 247 (Runde 31)
            "${RE15_PC_EXE}")
    endif()
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "granate_laden[${_name}]: kein debug.log (exit=${_rv})")
    endif()
    file(READ "${WORKDIR}/debug.log" _lh HEX)
    _hex_von("CONTINUE: resumed in room ${_raum}" _h)
    string(FIND "${_lh}" "${_h}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "granate_laden[${_name}]: der Spielstand in ROOM${_raum} wurde nicht "
                            "geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    if(_check)
        _hex_von("[inv] acceptance shot" _h)
    else()
        _hex_von("EXIT_AT: Bild" _h)
    endif()
    string(FIND "${_lh}" "${_h}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "granate_laden[${_name}]: das Endbild wurde nicht erreicht - der Lauf "
                            "ist vorher abgerissen (exit=${_rv}, ${WORKDIR})")
    endif()
    set(${_out_hex} "${_lh}" PARENT_SCOPE)
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

set(_z_boot   "[granate] Boot-Weg: Prop obj_id=7 im Pool")
set(_z_render "oid=0x07 pos=")
set(_z_modal  "[granate] Modal auf")
set(_z_smodal "[sicherung] Modal auf")

# --- A: Sicherung genommen -> Granate liegt und ihr Modal geht auf ------------
granate_lauf(a 1150 "sicherung" 0 _log)
foreach(_z IN ITEMS "${_z_boot}" "${_z_render}" "${_z_modal}")
    _hat("${_log}" "${_z}" _ja)
    if(NOT _ja)
        message(FATAL_ERROR "granate_laden[A]: nach CONTINUE in ROOM1150 fehlt '${_z}' - die "
                            "Granate liegt am Lade-Weg nicht im Hebetisch oder ihr Modal geht in "
                            "der Fahrt nicht auf (re15_granate_install am Boot-Weg in main.c?)")
    endif()
endforeach()

# --- B: beide genommen -> keine Granate (Negativ-Kontrolle) --------------------
granate_lauf(b 1150 "sicherung;genommen" 0 _log)
foreach(_z IN ITEMS "${_z_boot}" "${_z_render}" "${_z_modal}")
    _hat("${_log}" "${_z}" _ja)
    if(_ja)
        message(FATAL_ERROR "granate_laden[B]: die Granate ist im Spielstand GENOMMEN (9,56), "
                            "trotzdem steht '${_z}' im Log - das Anlegen laeuft vor dem Restore "
                            "der Flags")
    endif()
endforeach()

# --- C: nichts genommen -> erst die Sicherung, die Granate wartet -------------
granate_lauf(c 1150 "" 0 _log)
_hat("${_log}" "${_z_boot}" _boot)
_hat("${_log}" "${_z_smodal}" _smodal)
_hat("${_log}" "${_z_modal}" _gmodal)
if(NOT _boot OR NOT _smodal OR _gmodal)
    message(FATAL_ERROR "granate_laden[C]: erwartet Granate im Pool (${_boot}), Sicherungs-Modal "
                        "auf (${_smodal}) und KEIN Granaten-Modal, solange das der Sicherung "
                        "unbeantwortet ist (${_gmodal})")
endif()

# --- D: ROOM1151 (Elza) --------------------------------------------------------
granate_lauf(d 1151 "sicherung" 0 _log)
foreach(_z IN ITEMS "${_z_boot}" "${_z_render}" "${_z_modal}")
    _hat("${_log}" "${_z}" _ja)
    if(NOT _ja)
        message(FATAL_ERROR "granate_laden[D]: nach CONTINUE in ROOM1151 fehlt '${_z}'")
    endif()
endforeach()

# --- E: CHECK-Foto der Granate -------------------------------------------------
granate_lauf(e 1150 "sicherung;fach0" 1 _log)
_hat("${_log}" "[inv] CHECK-Foto Item 0x09: crect (0,489) prect (832,256) 56x72 -> Foto und CLUT geladen" _ja)
if(NOT _ja)
    message(FATAL_ERROR "granate_laden[E]: der CHECK auf der Granate laedt kein Foto in das "
                        "Fenster (832,256) / CLUT (0,489) des RE1.5-Schirms")
endif()

message(STATUS "granate_laden: OK - Lade-Weg 1150/1151, Negativ-Kontrolle, Reihenfolge "
               "Sicherung vor Granate, CHECK-Foto geladen")
