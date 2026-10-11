# =============================================================================
# Runde 36 Spur R "dachkiste" — ECHTE exe. Dossier: analysis/befunde_runde36/R_dachkiste.md
#
# Befund der Spur H (H_ziel1190.md 2.2/2.4): die Kiste obj 1 auf dem DACH von ROOM1090/1091 ist die einzige
# Stufe zum Tor nach ROOM1100 (Tuer slot 1 @0x0213A verlangt Band 6 = Kistenoberkante). Seit Spur A
# (objfall_r36.c, FUN_8002c444) kann eine Kiste fallen; faellt diese, waere das Tor unerreichbar. Nutzer-
# Wortlaut der Spur A: "Das macht ein Fortschritt im Spiel unmoeglich" — ein vom Port erzeugter Softlock ist
# ausgeschlossen.
#
# AM ORIGINAL GEMESSEN (DuckStation-GDB, Auslieferungs-EXE, R_belege/orig_lauf5+6/log.txt): JEDER Schub an der
# Dach-Kiste ist gesperrt — der erste Schubschritt FUN_8002cabc(Spieler,obj,1) @0x8002bfa4 setzt die Kiste
# auf Spieler-z - 1350 (= -15558), FUN_8003b558(obj,2) @0x8002bfb0 trifft die Bruestungszelle r3[4]
# (0x80123984: x -7256..4624 z -18560..-16403, u0 0xff) bzw. beim Schub nach Westen die Westwand r3[2]
# (x -9200..-6878), Ruecksetzung @0x8002c0b8-d4; 0 Fall-Ausloeser, Kiste bleibt (-5960,-9000,-15482).
#
# DIESER RIEGEL misst dasselbe an der exe ueber den ECHTEN WEG (RE15_PUSH_LOG = Messschrieb der Schubkette,
# engine/src/dachkiste_r36.c; TREFFER-ZELLE = Gegenstueck zum Original-Haltepunkt FUN_8003da78 @0x8003b79c):
#   leon  ROOM10F0 (Szene (9,71) gesehen) -> Tritt schieben, hochklettern, Tuer slot 1 -> ROOM1090 ->
#         Korridor x -6878..-4814 nach Norden -> drei Schuebe gegen die Kiste:
#         MUSS: "DOOR FIRE slot=1 ... spawn=(-13600,-9000,1300)", "loaded room1090.rdt", >= 2x
#         "room=1090 p=1 obj=1 SCHUB-GESPERRT ... ziel=(-5960,-155xx)" mit TREFFER-ZELLE der Bruestung
#         "x -7256..4624 z -18560..-16403", KEIN "SCHUB-OK" fuer obj 1 in 1090, KEIN "p=1 AUSLOESER",
#         letzte Kistenlage "p1 (-5960,-9000,-15482) b5".
#   elza  derselbe Weg mit Elza (RE15_PSELECT_AUTO_SWITCH=1): ROOM10F1 -> ROOM1091, dieselben MUSS-Zeilen.
#   west  Leon, Sprung ROOM1090, Spieler in der Tasche oestlich der Kiste ((-4610,-15155) Blick 2112 = West):
#         MUSS: >= 1x "SCHUB-GESPERRT ... ziel=(-603x,-15482)" mit TREFFER-ZELLE der Westwand
#         "x -9200..-6878", kein SCHUB-OK, kein AUSLOESER, Kiste bleibt.
#
# Er DISKRIMINIERT: ein Port, der den Schub annimmt (SCHUB-OK) oder die Kiste fallen laesst (AUSLOESER,
# Kistenlage != Record), faellt durch.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r36_dachkiste.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r36_dachkiste: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r36_dachkiste: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_basis}")

# Eigener exe-Name neben der exe (Muster test_r34n_b_cursor.cmake: fremde `taskkill /IM re15_pc.exe`).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r36r_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r36r_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

function(_fehler _was)
    file(REMOVE "${_exe}")
    message(FATAL_ERROR "r36_dachkiste: ${_was}")
endfunction()

# Ein Lauf im eigenen Unterordner; debug.log/push.txt/objfall.txt bleiben zur Nachschau liegen.
macro(_lauf _name _timeout)
    set(_sub "${_basis}/${_name}")
    file(MAKE_DIRECTORY "${_sub}")
    set(WORKDIR "${_sub}")
    re15_start_spiel(_rv ${_timeout} ${ARGN} "${_exe}")
    if("${_rv}" STREQUAL "1" OR "${_rv}" STREQUAL "-1" OR NOT EXISTS "${_sub}/debug.log")
        message(STATUS "r36_dachkiste[${_name}]: Lauf ohne Ergebnis (exit=${_rv}) -> EIN Wiederholungsversuch")
        file(REMOVE "${_sub}/debug.log" "${_sub}/push.txt" "${_sub}/objfall.txt" "${_sub}/climb.txt")
        re15_start_spiel(_rv ${_timeout} ${ARGN} "${_exe}")
    endif()
    if(NOT EXISTS "${_sub}/debug.log")
        _fehler("${_name}: kein debug.log (exit=${_rv}, ${_sub})")
    endif()
    file(READ "${_sub}/debug.log" _dbg)
    set(_push "")
    if(EXISTS "${_sub}/push.txt")
        file(READ "${_sub}/push.txt" _push)
    endif()
    set(_fall "")
    if(EXISTS "${_sub}/objfall.txt")
        file(READ "${_sub}/objfall.txt" _fall)
    endif()
endmacro()

macro(_muss _text _var _was)
    string(FIND "${${_var}}" "${_text}" _pos)
    if(_pos LESS 0)
        _fehler("${_was} — fehlt: '${_text}'")
    endif()
endmacro()

# ⛔ Makro-Argumente werden TEXTUELL eingesetzt und im Makro ein zweites Mal ausgewertet (gemessen 2026-10-11: ein
# "\\(" im Aufruf wurde zur Gruppe "(", der Riegel fand den gesperrten Schub nicht): Regex-Argumente deshalb ohne
# Backslash — Klammerausdruecke [(] [)] [.] bzw. "." fuer eckige Klammern; Klartext ueber string(FIND).
macro(_darf_nicht _regex _var _was)
    string(REGEX MATCH "${_regex}" _m "${${_var}}")
    if(NOT "${_m}" STREQUAL "")
        _fehler("${_was} — gefunden: '${_m}'")
    endif()
endmacro()

macro(_darf_nicht_text _text _var _was)
    string(FIND "${${_var}}" "${_text}" _pos)
    if(NOT _pos LESS 0)
        _fehler("${_was} — gefunden: '${_text}'")
    endif()
endmacro()

# Kiste obj 1 (Dach, Band 5) im Raum <_raum>: nur gesperrte Schuebe, Treffer-Zelle <_zelle>, kein Fall, Record-Lage.
macro(_kiste_bleibt _name _raum _ziel_regex _zelle)
    string(REGEX MATCHALL "room=${_raum} p=1 obj=1 SCHUB-GESPERRT[^\n]*ziel=${_ziel_regex}[^\n]*band=5\n  TREFFER-ZELLE r[0-9]+.[0-9]+.: ${_zelle}" _ges "${_push}")
    list(LENGTH _ges _n_ges)
    if(_n_ges LESS 1)
        _fehler("${_name}: kein gesperrter Schub an obj 1 in ROOM${_raum} mit Ziel ${_ziel_regex} und Treffer-Zelle '${_zelle}' (push.txt: ${_push})")
    endif()
    _darf_nicht("room=${_raum} p=1 obj=1 SCHUB-OK[^\n]*" _push "${_name}: der Port nimmt einen Schub an der Dach-Kiste an (Original: jeder Schub gesperrt @0x8002bfb8)")
    _darf_nicht("room=${_raum} p=1 AUSLOESER[^\n]*" _fall "${_name}: Fall-Ausloeser der Dach-Kiste (Original: 0)")
    _darf_nicht("room=${_raum} p=1 (zust|BODEN|LANDUNG)[^\n]*" _fall "${_name}: Fallzustand der Dach-Kiste (Original: keiner)")
    string(REGEX MATCHALL "room=${_raum} spieler=[^\n]*\\| p1 \\([-0-9]+,[-0-9]+,[-0-9]+\\) b[0-9]" _lagen "${_fall}")
    list(LENGTH _lagen _n_lagen)
    if(_n_lagen LESS 1)
        _fehler("${_name}: keine Kistenlage p1 in objfall.txt (ROOM${_raum})")
    endif()
    list(GET _lagen -1 _letzte)
    string(REGEX MATCH "p1 \\(-5960,-9000,-15482\\) b5" _rec "${_letzte}")
    if("${_rec}" STREQUAL "")
        _fehler("${_name}: Kiste obj 1 nicht mehr auf dem Record (-5960,-9000,-15482) Band 5: '${_letzte}'")
    endif()
    message(STATUS "r36_dachkiste ${_name}: ${_n_ges} gesperrte Schuebe (Ziel ${_ziel_regex}, Zelle ${_zelle}), 0 angenommen, 0 Ausloeser, Kiste auf dem Record")
endmacro()

set(_grund RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
           RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
           RE15_CLIMB_TRACE=climb.txt RE15_OBJFALL_LOG=objfall.txt RE15_PUSH_LOG=push.txt
           RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1)

# Echter Weg (BASIS=spiel: die Zeitachse beginnt in JEDEM Raum neu, derselbe Text laeuft in 10F0/10F1 und
# dann in 1090/1091): 10F0-Teil = Tritt nach Osten schieben, hochklettern (A), Tuer slot 1 (A) [R_belege/
# lauf_l1]; 1090-Teil = die Wiederholung endet an der Saeule r1[2] (x -5282, Blick 32), danach R0.35 (Blick
# ~1088, Nord), U8 den Korridor hinauf bis an die Kiste (Spieler z -14132) = erster Schub, U1.5/U1.5 = zwei
# weitere Schuebe (Zaehler obj+0x8C laeuft nach dem Loslassen neu, @0x8002bf74).
set(_weg "W10,U2.6,W1,L0.37,W0.3,U0.5,W0.3,R0.37,W0.3,U0.55,W0.3,R0.37,W0.5,U10.5,W1,A0.1,W2.5,U0.6,W0.3,A0.1,W3,W10,R0.35,W0.3,U8,W0.5,U1.5,W0.5,U1.5,W2")
set(_bruestung "x -7256[.][.]4624 z -18560[.][.]-16403")
set(_westwand  "x -9200[.][.]-6878 z -18524[.][.]265")

# ---- leon: ROOM10F0 -> Tritt -> Tuer -> ROOM1090 -> Korridor -> Schub nach Norden -------------------------
_lauf(leon 400 ${_grund} RE15_SET_FLAG=9:71 RE15_DEBUG_JUMP=10F0@250 "RE15_PLAYER_POS=-7125,4950,0"
      "RE15_INPUT_SCRIPT=${_weg}" "RE15_EXIT_AT=2000#1090")
_muss("loaded room10f0.rdt" _dbg "leon: ROOM10F0 nicht geladen")
_darf_nicht_text("[cut10f0] ROOM10F0: Szene gestartet" _dbg "leon: Szene 10F0 lief trotz (9,71)=1")
_muss("[aot] DOOR FIRE slot=1 rect=(-4750,-1750,hw=600,hh=500) target_cut=0 spawn=(-13600,-9000,1300)" _dbg "leon: Tuer slot 1 nach ROOM1090 nicht ausgeloest (Tritt/Klettern/Aktion)")
_muss("loaded room1090.rdt" _dbg "leon: ROOM1090 nicht geladen")
_kiste_bleibt(leon 1090 "[(]-5960,-155[0-9][0-9][)]" "${_bruestung}")

# ---- elza: derselbe Weg, ROOM10F1 -> ROOM1091 -------------------------------------------------------------
_lauf(elza 400 ${_grund} RE15_PSELECT_AUTO_SWITCH=1 RE15_DEBUG_JUMP=10F0@250 "RE15_PLAYER_POS=-7125,4950,0"
      "RE15_INPUT_SCRIPT=${_weg}" "RE15_EXIT_AT=2000#1091")
_muss("Spieler-Familie PL04" _dbg "elza: Elza nicht gewaehlt (RE15_PSELECT_AUTO_SWITCH)")
_muss("loaded room10f1.rdt" _dbg "elza: ROOM10F1 nicht geladen")
_muss("[aot] DOOR FIRE slot=1 rect=(-4750,-1750,hw=600,hh=500) target_cut=0 spawn=(-13600,-9000,1300)" _dbg "elza: Tuer slot 1 nach ROOM1091 nicht ausgeloest")
_muss("loaded room1091.rdt" _dbg "elza: ROOM1091 nicht geladen")
_kiste_bleibt(elza 1091 "[(]-5960,-155[0-9][0-9][)]" "${_bruestung}")

# ---- west: Leon in der Tasche oestlich der Kiste, Schub nach Westen (Westwand r3[2]) ------------------------
_lauf(west 200 ${_grund} RE15_DEBUG_JUMP=1090@250 "RE15_PLAYER_POS=-4610,-15155,2112,5"
      "RE15_INPUT_SCRIPT=W2,U2,W0.5,U1.5,W0.5,U1.5,W1" "RE15_EXIT_AT=450#1090")
_muss("loaded room1090.rdt" _dbg "west: ROOM1090 nicht geladen")
_kiste_bleibt(west 1090 "[(]-60[0-9][0-9],-15482[)]" "${_westwand}")

file(REMOVE "${_exe}")
message(STATUS "r36_dachkiste: OK — Dach-Kiste ROOM1090/1091: jeder Schub gesperrt wie im Original, kein Fall, Tor-Stufe bleibt")
