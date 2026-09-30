# =============================================================================
# HEBETISCH-CURSOR MIT DER ECHTEN EXE (Runde 34 Nacht, Spur B: Irons' Buero ROOM1150/1151).
#
# Dossier analysis/befunde_runde34_nacht/B_hebetisch.md (§9), Konstanten include/re15_hebetisch_cursor.h.
# Die Unit-Riegel unit_r34n_b_* pruefen die Mechanik Takt fuer Takt; dieser Riegel prueft, was nur die
# exe zeigt (Auflage 8 der Gegenpruefung): den echten Eingabepfad (Aktionstaste am Tisch, D-Pad,
# SQUARE, CROSS, START), den LADE-Weg (Boot/CONTINUE geht nicht durch scd_room_reenter, Auflage 2;
# Original EIN Raumlader FUN_800396fc, `jal 0x800396fc` @0x8001d5ac LOAD und @0x8001d988 Tuer), die
# Schranken von Menue und Text (Auflage 1) und den Nutzerweg bis zum Ende.
#
# Vier Laeufe, Spielstand aus probe_r30_granate_karte (Spieler (-22250,0,-18500) rot 0 = Westseite des
# Tischs, Blick auf das Modell), CONTINUE, Eingabeskript auf der Spielbild-Zeitachse ab Bild 60:
#   A  ROOM1150 NUTZERWEG: Aktion am Tisch -> Cursor; SQUARE daneben -> "Nothing happened."; Text mit
#      CROSS zu (KEIN Abbruch); D-Pad auf die Kuppel; SQUARE -> Klick + Kuppel auf; Sicherung Yes,
#      Granate Yes; Fahrt zu Ende, Parklage, zurueck in die Raumkamera
#   B  ROOM1151 ABBRUCH: Cursor, CROSS -> Aufraeumbytes (Parklage, Raumkamera), erneute Aktion ->
#      Cursor wieder
#   C  ROOM1150 INVENTAR im Cursor: START, D-Pad + SQUARE im Menue (EXIT), danach RIGHT 9 Takte ->
#      Cursor steht bis zum Schliessen still, dann genau 9 Schritte, kein Text, kein Klick, kein Abbruch
#   D  ROOM1150 HARNESS-GEGENPROBE: RE15_FIRE_AOT=1@90 (re15_aot_fire_slot -> scd_event_fire direkt)
#      bringt KEINEN Cursor, die Fahrt laeuft wie bisher bis in die Ruhe oben
#
# RE15_SOFTWARE_RENDER=1 dient nur der Robustheit des Testhakens; geprueft wird das LOG
# (RE15_HEBETISCH_CURSOR_LOG, RE15_HEBETISCH_LOG, debug.log). Das BILD belegt die Framedump-Abnahme
# im Dossier (§9.5).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r30_granate_karte> -DWORKDIR=<dir>
#               -P test_r34n_b_cursor.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r34n_b_cursor: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r34n_b_cursor: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r34n_b_cursor: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# ⛔ EIGENER EXE-NAME (gemessen 2026-09-30): zwei Laeufe dieses Hakens endeten mitten im Spiel mit
# exit=1 (Bild ~300 bzw. ~500, kein Absturzeintrag), waehrend dieselben Laeufe unter einem anderen
# exe-Namen (tools/r34n_b/lauf_laden.sh) 5 von 5 durchliefen. Ursache: local_build.sh faellt ohne
# powershell/cygpath im Minimal-PATH auf `taskkill //F //IM re15_pc.exe` zurueck (local_build.sh:298-299)
# und beendet damit JEDE re15_pc.exe der Maschine, sobald ein paralleler Agent baut. Die Kopie liegt
# neben der exe (Asset-Wurzel = exe-Verzeichnis) und traegt einen eigenen Namen.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r34nb_haken_*.exe")   # Reste abgebrochener Laeufe
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r34nb_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

# Ein Lauf: Karte schreiben, exe starten. Liefert cursor.log / debug.log / hebetisch.log als Text.
function(cursor_lauf _name _raum _karte _skript _ende _extra _out_cursor _out_debug _out_heb)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log" "${WORKDIR}/cursor.log"
                "${WORKDIR}/hebetisch.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}" ${_karte}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r34n_b_cursor[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
        "RE15_INPUT_SCRIPT=${_skript}"
        RE15_HEBETISCH_CURSOR_LOG=cursor.log RE15_HEBETISCH_LOG=hebetisch.log
        "RE15_EXIT_AT=${_ende}#${_raum}"
        ${_extra}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r34n_b_cursor[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(FIND "${_dbg}" "CONTINUE: resumed in room ${_raum}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r34n_b_cursor[${_name}]: Spielstand in ROOM${_raum} nicht geladen "
                            "(exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r34n_b_cursor[${_name}]: Endbild nicht erreicht - Lauf vorher "
                            "abgerissen (exit=${_rv}, ${WORKDIR})")
    endif()
    set(_cur "")
    if(EXISTS "${WORKDIR}/cursor.log")
        file(READ "${WORKDIR}/cursor.log" _cur)
    endif()
    set(_heb "")
    if(EXISTS "${WORKDIR}/hebetisch.log")
        file(READ "${WORKDIR}/hebetisch.log" _heb)
    endif()
    set(${_out_cursor} "${_cur}" PARENT_SCOPE)
    set(${_out_debug} "${_dbg}" PARENT_SCOPE)
    set(${_out_heb} "${_heb}" PARENT_SCOPE)
endfunction()

# Anzahl der cursor.log-Zeilen mit einem Ereigniswort (das Log hat keine Klammern/Semikola).
function(_anzahl _text _wort _out)
    string(REGEX MATCHALL "F[0-9]+ raum=[0-9A-F]+ ${_wort} [^\n]*" _m "${_text}")
    list(LENGTH _m _n)
    set(${_out} ${_n} PARENT_SCOPE)
endfunction()

# --- A: Nutzerweg bis zum Ende (ROOM1150) ------------------------------------------------------
#  F90 Aktion am Tisch | F117 SQUARE daneben | F180 CROSS schliesst den Text | F198..219 D-Pad
#  (18x RIGHT+DOWN, 4x DOWN) | F229 SQUARE auf der Kuppel | ab F367 achtmal SQUARE (Yes/Yes)
cursor_lauf(a 1150 ""
    "W1,A0.2,W0.7,A0.1,W2,X0.1,W0.5,RD0.6,D0.13,W0.3,A0.1,W4.5,A0.1,W1,A0.1,W1,A0.1,W1,A0.1,W1,A0.1,W1,A0.1,W1,A0.1,W1,A0.1,W8"
    900 "" _cur _dbg _heb)
_anzahl("${_cur}" verlangt _nv)
_anzahl("${_cur}" aktiv _nak)
_anzahl("${_cur}" nichts _nni)
_anzahl("${_cur}" kuppel _nku)
_anzahl("${_cur}" abbruch _nab)
if(NOT (_nv EQUAL 1 AND _nak EQUAL 1))
    message(FATAL_ERROR "r34n_b_cursor[A]: Aktion am Tisch nach CONTINUE: erwartet genau eine "
        "Cursor-Sitzung (verlangt ${_nv}, aktiv ${_nak}) - Install am Boot-Weg / Haken in game_step?")
endif()
if(NOT _nni EQUAL 1)
    message(FATAL_ERROR "r34n_b_cursor[A]: SQUARE neben der Kuppel: erwartet genau einen "
        "\"Nothing happened.\" (${_nni})")
endif()
if(NOT _nab EQUAL 0)
    message(FATAL_ERROR "r34n_b_cursor[A]: der Text wurde mit CROSS geschlossen und hat den Cursor "
        "ABGEBROCHEN (${_nab}) - Auflage 1/(c)7")
endif()
if(NOT _nku EQUAL 1)
    message(FATAL_ERROR "r34n_b_cursor[A]: SQUARE auf der Kuppel: erwartet genau einen Treffer (${_nku})")
endif()
string(REGEX MATCH "kuppel [^\n]*treffer=1[^\n]*klick=1 text=1" _kz "${_cur}")
if(NOT _kz)
    message(FATAL_ERROR "r34n_b_cursor[A]: Kuppeldruck ohne treffer=1 / klick=1 / text=1")
endif()
foreach(_z IN ITEMS "[sicherung] Yes: genommen" "[granate] Yes: genommen" "[sicherung] Fahrt zu Ende")
    string(FIND "${_dbg}" "${_z}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r34n_b_cursor[A]: nach dem Kuppeldruck fehlt '${_z}' - der normale "
            "Ablauf von sub04 lief nicht durch")
    endif()
endforeach()
# nach dem Fahrtende zurueck in die Raumkamera (Cut_old @0x10B2), nicht Cut 4
string(FIND "${_dbg}" "[sicherung] Fahrt zu Ende" _pe)
string(FIND "${_dbg}" "[pri] cut=" _pl REVERSE)
if(_pl LESS _pe)
    message(FATAL_ERROR "r34n_b_cursor[A]: nach dem Fahrtende kein Kamerawechsel (Cut_old @0x10B2?)")
endif()
string(SUBSTRING "${_dbg}" ${_pl} 12 _pri_letzt)
if(_pri_letzt STREQUAL "[pri] cut=4 ")
    message(FATAL_ERROR "r34n_b_cursor[A]: nach dem Fahrtende steht die Kamera auf Cut 4")
endif()
string(REGEX MATCH "F[0-9]+ y=-20224[^\n]*\n$" _park "${_heb}")
if(NOT _park)
    message(FATAL_ERROR "r34n_b_cursor[A]: Plattform am Ende nicht in der Parklage y=-20224 (@0x109E)")
endif()

# --- B: Abbruch mit CROSS (ROOM1151, Sicherung schon genommen) -----------------------------------
#  F90 Aktion | F117 CROSS | F150 erneute Aktion
cursor_lauf(b 1151 "sicherung" "W1,A0.2,W0.7,X0.1,W1,A0.2,W2" 230 "" _cur _dbg _heb)
_anzahl("${_cur}" verlangt _nv)
_anzahl("${_cur}" aktiv _nak)
_anzahl("${_cur}" abbruch _nab)
if(NOT (_nab EQUAL 1 AND _nv EQUAL 2 AND _nak EQUAL 2))
    message(FATAL_ERROR "r34n_b_cursor[B]: erwartet Cursor, CROSS-Abbruch, erneute Aktion -> Cursor "
        "wieder (verlangt ${_nv}, aktiv ${_nak}, abbruch ${_nab})")
endif()
string(REGEX MATCH "F([0-9]+) raum=1151 abbruch" _m "${_cur}")
math(EXPR _f_nach "${CMAKE_MATCH_1} + 2")
string(REGEX MATCH "F${_f_nach} y=(-?[0-9]+)" _m "${_heb}")
if(NOT CMAKE_MATCH_1 EQUAL -20224)
    message(FATAL_ERROR "r34n_b_cursor[B]: 2 Bilder nach dem Abbruch (F${_f_nach}) steht die Plattform "
        "nicht in der Parklage (y=${CMAKE_MATCH_1}, Pos_set @0x109E)")
endif()
string(FIND "${_dbg}" "[granate] Modal auf" _pg)
if(NOT _pg LESS 0)
    message(FATAL_ERROR "r34n_b_cursor[B]: der Abbruch hat die Granaten-Aufnahme ausgeloest")
endif()

# --- C: Inventar im Cursor (ROOM1150) --------------------------------------------------------------
#  F90 Aktion | F111 START | D-Pad im Menue | SQUARE auf EXIT | danach RIGHT 9 Takte
cursor_lauf(c 1150 "" "W1,A0.2,W0.5,S0.1,W2,R1,D1,A0.1,W2,R0.3,W1" 330 "" _cur _dbg _heb)
_anzahl("${_cur}" nichts _nni)
_anzahl("${_cur}" kuppel _nku)
_anzahl("${_cur}" abbruch _nab)
if(NOT (_nni EQUAL 0 AND _nku EQUAL 0 AND _nab EQUAL 0))
    message(FATAL_ERROR "r34n_b_cursor[C]: Eingaben im Menue haben den Cursor bedient (Text ${_nni}, "
        "Kuppel ${_nku}, Abbruch ${_nab}) - Auflage 1")
endif()
string(REGEX MATCH "x=-17754 z=22684[^\n]*\n$" _endlage "${_cur}")
if(NOT _endlage)
    message(FATAL_ERROR "r34n_b_cursor[C]: nach Menue und 9 Takten RIGHT steht der Cursor nicht bei "
        "x=-19554+9*200=-17754 (Menue-D-Pad durchgesickert oder Menue nicht zu?)")
endif()

# --- D: Harness-Gegenprobe RE15_FIRE_AOT (ROOM1150) ------------------------------------------------
cursor_lauf(d 1150 "" "W8" 280 "RE15_FIRE_AOT=1@90#1150" _cur _dbg _heb)
_anzahl("${_cur}" verlangt _nv)
_anzahl("${_cur}" aktiv _nak)
if(NOT (_nv EQUAL 0 AND _nak EQUAL 0))
    message(FATAL_ERROR "r34n_b_cursor[D]: RE15_FIRE_AOT hat den Cursor gebracht (Armieren nur am "
        "Spielerweg)")
endif()
string(FIND "${_heb}" "ruht=1" _pr)
if(_pr LESS 0)
    message(FATAL_ERROR "r34n_b_cursor[D]: ohne Cursor erreicht die Fahrt die Ruhe oben nicht")
endif()

file(REMOVE "${_exe_kopie}")
message(STATUS "r34n_b_cursor: OK - Nutzerweg 1150 bis zum Ende, Abbruch + erneut 1151, Inventar im "
               "Cursor, Harness ohne Cursor")
