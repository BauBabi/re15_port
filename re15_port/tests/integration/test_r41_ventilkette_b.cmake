# =============================================================================
# VENTILKETTE MIT DER ECHTEN EXE, Teil B (Runde 41 Spur C, dritte Fortsetzung 2026-10-24): die Glieder, die Teil A
# (test_r41_ventilkette.cmake: 2030 Ventile -> Zaehler -> Ost-Tuer -> 20B0, Leiter 2000, Flut -> Ertrinken) nicht
# faehrt. Gemessen 2026-10-24 00:26-00:29 (Dossier analysis/befunde_runde41/C_ventilkette.md §4 "Luecken am Spiel",
# Belegbilder C_belege/pult2050_loesung_sheet.png, pult2030_ohne_ventil.png, raum2030_leer_vergleich.png).
# Konstanten und Belege: include/re15_ventilkette_r41.h. Der Unit-Riegel unit_r41_ventilkette misst dieselben Glieder
# Takt fuer Takt am VM; dieser Riegel misst den Eingabepfad der exe (Aktionstaste, Steuerkreuz im Pult, Tuer-Feuerpfad,
# Raumlader).
#
#   D  Punkt 5, ein Lauf: ROOM2050 vor dem Pult (4900,-7100) Blick -z. Aktion -> Port-Programm 2050_pult_ein -> Text 43
#      -> "Will you push them?" (11F0 msg 1) -> Ja -> Cut 15 (sub09-Bytes @0x023F2). Cursor (Original sub05..08, 200 je
#      Bild): RIGHT 8 Bilder -> Hebel 3 (40 V), LEFT 36 -> Hebel 1 (70 V), RIGHT 60 -> Hebel 4 (80 V, Schalter 0xD) ->
#      30 Ruhebilder (RE2 ROOM2130 @0x0171C) -> Bit 106 + "Power supply OK." -> Ausstieg sub10 @0x0240C. Danach der
#      Hebel-Platz Slot 6 (@0x01E28, Ereignis 4) ueber RE15_FIRE_AOT (re15_aot_fire_slot = Pfad der Aktionstaste; der
#      Hebel liegt in der Nische hinter der Saeule, ein offenes Skript um die Wand waere geraten) -> KEIN Port-Programm,
#      Original sub04: Frage msg 0 -> Ja -> obj 4 y -1400 (@0x0230C ff.) -> Cut 4 -> For 180 Speed -20 -> obj 5 y -5000.
#   E  Punkt 5 vorher: Aktionstaste am Hebel (Nische (4300,-11150) Blick +x) ohne Bit 106 -> 2050_hebel_sperre, Text 40,
#      kein Cut 4, kein Member_set des Wassers.
#   E2 Nachbesserung 1 (Abnahme 0, M1): die drei Kisten liegen im Becken ((5,0..2)=1 ab F300 = die Bits, die sub01 beim
#      Einschieben setzt @0x0203C ff.) -> sub01 @0x0201A `46 06 03 31 ff 00 18 02` legt den Hebel-Platz Slot 6 auf
#      Ereignis 2 (= sub02 @0x0220E: Set(3,109), Speed -20 For 180 @0x0227C = Wasser steigt). Ohne Bit 106: Ereignis 2 ->
#      2050_hebel_sperre, Text 40, kein Cut 4, keine Kisten-/Wasser-Member_set (val -17700 @0x02244, -1400 @0x02260).
#   E3 Gegenprobe: dasselbe mit Bit 106 (Pult geloest) -> Original sub02: Frage msg 0 -> Ja -> Kisten obj 1 m0 -17700
#      (@0x02244), Cut 4 (@0x02270), obj 5 y -5000 (@0x02290) = das Kisten-Raetsel bleibt nach dem Pult loesbar.
#   E4 Elza (ROOM2051): E2 mit Elza.
#   F  Punkt 2 vorher: Bruecken-Pult 2030 (Ring (350,-9500) Blick -z) ohne Bit 101 -> 2030_pult_ohne, Text 40, keine
#      Fahrt (kein Cut_chg(4), kein Member_set der Planke obj 0). Spinnen als "schon getoetet" ueber ihre Kill-Bits
#      (Sce_em_set pc[7] 0x9b/0x9c/0x9d @0x01C18/@0x01C2C/@0x01C40, EM-Status-Gate FUN_800420a0 @0x80042120-38).
#   G  Punkt 4: ROOM20B0 nach der Szene ((3,235) = Set @0x01FBC) mit Bits 103 + 100 -> Leiter Slot 4 (@0x019E2) ist die
#      Tuer nach ROOM2050 (kein Sperrtext); in 2050 lenkt Slot 0 (Ziel ROOM2000) nach ROOM20B0 Cut 9 um.
#   H  Punkt 4: 2050-Leiter hinab (Slot 0 @0x01C4A, Ziel ROOM2000 Cut 10) mit Bit 103 -> ROOM20B0 Cut 9, nicht ROOM2000.
#   I  Nachtraege 7/8 (Nachbesserung 3): ROOM2030 mit Bit 103 ohne Strom -> Wasserflaechen obj 4/5 bei y -7200
#      (Gegenprobe); MIT Strom (3,126) (Setzer ROOM2060 sub01 @0x012AE) -> EINE Flaeche obj 4 bei (0,-5350,0) ueber der
#      Schachtoeffnung (Nachbesserung 4, Nachtrag 12 (k): knapp unter der Absperrung = Ringboden -5400 + 50 @0x00CC8), kein
#      obj 5, kein "unter Wasser"-Satz, Treppe Slot 8 = Sperrtext 48.
#   J  Elza (ROOM2051): D mit Elza bis "Power supply OK." und Original-Hebel.
#   K  Elza (ROOM2001): Leiter = Sperrtext 40 woertlich, kein Raumwechsel nach ROOM2051.
#   L/L2 Orchestrator-Entscheid C-E2 (Abnahme 0 M5), seit Nachtrag 7 nur "geflutet ohne Strom": ROOM20B0 nach der Szene
#      mit Bit 103 ohne (3,126) -> die Ost-Tuer Slot 3 @0x019C2 (Ziel 2030-Ring (6550,-5400,-100)) ist Sperrtext 41,
#      Aktionstaste -> Text, KEIN Raumwechsel; MIT Strom (3,126) feuert dieselbe Tuer und ROOM2030 wird geladen.
#   M/M2 dasselbe fuer ROOM2070 Slot 1 @0x00A5A (Text 40; mit Strom -> ROOM2030).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_ventilkette_b.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_ventilkette_b: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_ventilkette_b: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake). Praefix
# re15_pc_r41cb_ — NICHT re15_pc_r41c_haken_*, das raeumt Teil A beim Start weg.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41cb_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r41cb_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r41cb_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(_fehler _was)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_ventilkette_b${_was}")
endfunction()
function(_muss _text _wort _name _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(_p LESS 0)
        _fehler("[${_name}]: ${_was} — '${_wort}' fehlt im debug.log")
    endif()
endfunction()
function(_darf_nicht _text _wort _name _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(NOT _p LESS 0)
        _fehler("[${_name}]: ${_was} — '${_wort}' steht im debug.log")
    endif()
endfunction()
function(_muss_re _text _re _name _was)
    string(REGEX MATCH "${_re}" _m "${_text}")
    if("${_m}" STREQUAL "")
        _fehler("[${_name}]: ${_was} — Muster '${_re}' fehlt im debug.log")
    endif()
endfunction()
# Muster _wort2 muss NACH dem ersten Auftreten von _wort1 stehen.
function(_danach _text _wort1 _wort2 _name _was)
    string(FIND "${_text}" "${_wort1}" _p1)
    if(_p1 LESS 0)
        _fehler("[${_name}]: ${_was} — '${_wort1}' fehlt im debug.log")
    endif()
    string(SUBSTRING "${_text}" ${_p1} -1 _rest)
    string(FIND "${_rest}" "${_wort2}" _p2)
    if(_p2 LESS 0)
        _fehler("[${_name}]: ${_was} — '${_wort2}' steht nicht nach '${_wort1}'")
    endif()
endfunction()

# Grundumgebung (Leon): Titel-Bild-Haken wie Teil A; Elza-Lauf J setzt die Spielerwahl wie integration_r36_dachkiste.
set(_leon RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1
          RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
set(_elza RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1
          RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
# Pult 2050 (gemessen 00:26, Bildnummern im Dossier): Aktion F330 | F453 Text 43 zu | F531 Ja -> Cut 15 | RIGHT 8 |
# SQUARE (Hebel 3) | LEFT 36 | SQUARE (Hebel 1) | RIGHT 60 | SQUARE (Hebel 4) | F836 "Power supply OK." zu | F1022 Ja
# an der Original-Frage des Hebels (RE15_FIRE_AOT=6 bei F900).
set(_pult_skript "A0.1,W4,A0.1,W2.5,A0.1,W1,R0.25,W0.3,A0.1,W0.7,L1.2,W0.3,A0.1,W0.7,R2.0,W0.3,A0.1,W3,A0.1,W6.1,A0.1,W8")

macro(_lauf _name _timeout)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv ${_timeout} ${ARGN} "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        _fehler("[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
endmacro()

macro(_pult_pruefen _name _raum)
    _muss("${_dbg}" "[ventilkette41] ROOM${_raum}: Pult Slot 8 (@0x01D38) angelegt, Ereignis 9" ${_name} "Pult-Platz angelegt")
    _muss("${_dbg}" "[ventilkette41] ROOM${_raum} Ereignis 9 -> Port-Programm 2050_pult_ein" ${_name} "Aktion am Pult")
    _muss("${_dbg}" "[msg] room=${_raum} id=43" ${_name} "Beschreibung 43")
    _muss("${_dbg}" "[msg] room=${_raum} id=41" ${_name} "Frage 41 (11F0 msg 1)")
    _muss("${_dbg}" "Cut_chg(15)" ${_name} "Pult-Kamera Cut 15 (sub09)")
    _muss("${_dbg}" "[ventilkette41] Pult 2050: Hebel 3 -> EIN, Schalter 0x4, 40 V" ${_name} "Hebel 3")
    _muss("${_dbg}" "[ventilkette41] Pult 2050: Hebel 1 -> EIN, Schalter 0x5, 70 V" ${_name} "Hebel 1")
    _muss("${_dbg}" "[ventilkette41] Pult 2050: Hebel 4 -> EIN, Schalter 0xD, 80 V" ${_name} "Hebel 4 (80 V)")
    _muss("${_dbg}" "[ventilkette41] Pult 2050 GELOEST: 80 V -> Bit 106" ${_name} "Loesung nach den Ruhebildern")
    _muss("${_dbg}" "[msgtext] room=${_raum} id=42 \"Power supply OK.\"" ${_name} "Power supply OK. (11F0 msg 2, re15_scd_show_message loggt nur [msgtext])")
    _muss("${_dbg}" "[ventilkette41] Pult 2050 verlassen (Schalter 0xD, 80 V)" ${_name} "Ausstieg sub10")
    _danach("${_dbg}" "[fire-aot] slot=6" "[msg] room=${_raum} id=0" ${_name} "Hebel nach der Loesung = Original-Frage msg 0")
    _darf_nicht("${_dbg}" "2050_hebel_sperre" ${_name} "der Hebel darf nach der Loesung nicht gesperrt sein")
    _muss_re("${_dbg}" "obj=0x04 m=1 [^ ]+ val=-1400" ${_name} "Original sub04 Ja-Zweig: obj 4 y -1400 (@0x0230C ff.)")
    _danach("${_dbg}" "[fire-aot] slot=6" "Cut_chg(4)" ${_name} "Wasser steigt unter Cut 4 (@0x0231C)")
    _muss_re("${_dbg}" "obj=0x05 m=1 [^ ]+ val=-5000" ${_name} "Endlage nach For 180 (obj 5 y -5000)")
    _muss("${_dbg}" "EXIT_AT: Bild" ${_name} "Endbild erreicht")
endmacro()

# ---- D: 2050 Pult -> 80 V -> Hebel (Leon) --------------------------------------------------------------------
_lauf(d 300 ${_leon}
      RE15_DEBUG_JUMP=2050@240 "RE15_PLAYER_POS=4900,-7100,1024"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=${_pult_skript}"
      "RE15_FIRE_AOT=6@900#2050"
      "RE15_FRAMEDUMP=750:${_basis}_d/f2050_750.ppm"
      "RE15_EXIT_AT=1260#2050")
_pult_pruefen(d 2050)
if(NOT EXISTS "${_basis}_d/f2050_750.ppm")
    _fehler("[d]: Framedump F750 (Pult bei 80 V) fehlt")
endif()

# ---- E: Hebel vor dem Pult = Sperrtext 40 --------------------------------------------------------------------
_lauf(e 240 ${_leon}
      RE15_DEBUG_JUMP=2050@240 "RE15_PLAYER_POS=4300,-11150,0,3"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W4,A0.1,W3"
      "RE15_EXIT_AT=520#2050")
_muss("${_dbg}" "[ventilkette41] ROOM2050 Ereignis 4 -> Port-Programm 2050_hebel_sperre" e "Aktionstaste am Hebel ohne Bit 106")
_muss("${_dbg}" "[msgtext] room=2050 id=40" e "Sperrtext 40")
_darf_nicht("${_dbg}" "Cut_chg(4)" e "kein Wasser-Cut ohne Pult-Loesung")
_darf_nicht("${_dbg}" "val=-1400" e "kein Member_set des Wassers ohne Pult-Loesung")
_muss("${_dbg}" "EXIT_AT: Bild" e "Endbild erreicht")

# ---- E2/E3/E4: Kisten im Becken (Nachbesserung 1, M1) -----------------------------------------------------------
macro(_kisten_gesperrt _name _raum)
    _muss("${_dbg}" "[setflag-at] Frame 300: flag(5,2/0x02) = 1" ${_name} "Kisten-Bits (5,0..2) gesetzt")
    _muss("${_dbg}" "[ventilkette41] ROOM${_raum} Ereignis 2 -> Port-Programm 2050_hebel_sperre" ${_name} "Hebel mit Kisten (Ereignis 2, sub01 @0x0201A) ohne Bit 106 gesperrt")
    _muss("${_dbg}" "[msgtext] room=${_raum} id=40" ${_name} "Sperrtext 40")
    _darf_nicht("${_dbg}" "[msg] room=${_raum} id=0 " ${_name} "keine Original-Frage msg 0 (sub02 @0x0220E) ohne Pult")
    _darf_nicht("${_dbg}" "Cut_chg(4)" ${_name} "kein Wasser-Cut (@0x02270) ohne Pult")
    _darf_nicht("${_dbg}" "val=-17700" ${_name} "keine Kisten-Member_set (@0x02244) ohne Pult")
    _darf_nicht("${_dbg}" "val=-1400" ${_name} "kein Member_set des Wassers ohne Pult")
    _muss("${_dbg}" "EXIT_AT: Bild" ${_name} "Endbild erreicht")
endmacro()
_lauf(e2 240 ${_leon}
      RE15_DEBUG_JUMP=2050@240 "RE15_PLAYER_POS=4300,-11150,0,3" "RE15_SET_FLAG_AT=5:0,5:1,5:2@300"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W4,A0.1,W3"
      "RE15_FRAMEDUMP=360:${_basis}_e2/f2050_360.ppm"
      "RE15_EXIT_AT=520#2050")
_kisten_gesperrt(e2 2050)
_lauf(e3 300 ${_leon}
      RE15_DEBUG_JUMP=2050@240 "RE15_PLAYER_POS=4300,-11150,0,3" "RE15_SET_FLAG=9:106" "RE15_SET_FLAG_AT=5:0,5:1,5:2@300"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W4,A0.1,W10"
      "RE15_EXIT_AT=780#2050")
_darf_nicht("${_dbg}" "2050_hebel_sperre" e3 "mit Bit 106 bleibt der Kisten-Hebel das Original")
_muss("${_dbg}" "[msg] room=2050 id=0 " e3 "Original-Frage msg 0 (sub02 @0x0220E)")
_muss_re("${_dbg}" "obj=0x01 m=0 [^ ]+ val=-17700" e3 "Kisten steigen (sub02 @0x02244)")
_danach("${_dbg}" "val=-17700" "Cut_chg(4)" e3 "Wasser-Cut 4 (@0x02270)")
_muss_re("${_dbg}" "obj=0x05 m=1 [^ ]+ val=-5000" e3 "Endlage nach For 180 (@0x02290)")
_muss("${_dbg}" "EXIT_AT: Bild" e3 "Endbild erreicht")
_lauf(e4 240 ${_elza}
      RE15_DEBUG_JUMP=2050@250 "RE15_PLAYER_POS=4300,-11150,0,3" "RE15_SET_FLAG_AT=5:0,5:1,5:2@300"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W4,A0.1,W3"
      "RE15_EXIT_AT=520#2051")
_muss("${_dbg}" "Spieler-Familie PL04" e4 "Elza gewaehlt")
_muss("${_dbg}" "loaded room2051.rdt" e4 "ROOM2051 geladen")
_kisten_gesperrt(e4 2051)

# ---- F: Bruecken-Pult 2030 ohne Ventil 13 = Text 40, keine Fahrt ----------------------------------------------
_lauf(f 240 ${_leon}
      RE15_DEBUG_JUMP=2030@240 "RE15_PLAYER_POS=350,-9500,1024,3" "RE15_SET_FLAG=7:155,7:156,7:157"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W4,A0.1,W4,A0.1,W3"
      "RE15_EXIT_AT=700#2030")
_muss("${_dbg}" "[ventilkette41] ROOM2030: Bruecke obj 0 oben y -5400, Zelle 12 gesperrt" f "Bruecke oben")
_muss("${_dbg}" "[ventilkette41] ROOM2030 Ereignis 24 -> Port-Programm 2030_pult_ohne" f "Aktion am Pult ohne Ventil 13")
_muss("${_dbg}" "[msgtext] room=2030 id=40" f "Text 40 (Druck null, Ventil 13 zu)")
_darf_nicht("${_dbg}" "Cut_chg(4)" f "keine Fahrt-Kamera ohne Ventil 13")
_darf_nicht("${_dbg}" "obj=0x00 m=1" f "keine Endlage der Planke ohne Ventil 13")
_muss("${_dbg}" "EXIT_AT: Bild" f "Endbild erreicht")

# ---- G: 20B0 nach der Szene: Leiter -> 2050 -----------------------------------------------------------------
_lauf(g 240 ${_leon}
      RE15_DEBUG_JUMP=20B0@240 "RE15_PLAYER_POS=-26000,10300,2048" "RE15_SET_FLAG=3:235,9:103,9:100"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W6"
      "RE15_EXIT_AT=60#2050")
_darf_nicht("${_dbg}" "-> Sperrtext 40" g "mit Bit 100 bleibt die Leiter eine Tuer")
_muss("${_dbg}" "[aot] DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500) target_cut=0 spawn=(9100,-5400,4100)" g "Leiter feuert")
_muss("${_dbg}" "loaded room2050.rdt" g "ROOM2050 geladen")
_muss("${_dbg}" "[ventilkette41] ROOM2050: Tuer Slot 0 -> ROOM20B0 Cut 9 (Ziel ROOM2000)" g "2050-Leiter hinab lenkt nach 20B0 um")
_muss("${_dbg}" "EXIT_AT: Bild" g "Endbild in ROOM2050 erreicht")

# ---- H: 2050-Leiter hinab -> 20B0 statt 2000 ----------------------------------------------------------------
_lauf(h 240 ${_leon}
      RE15_DEBUG_JUMP=2050@240 "RE15_PLAYER_POS=9100,4300,3072" "RE15_SET_FLAG=3:235,9:103,9:100"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W6"
      "RE15_EXIT_AT=60#20B0")
_muss("${_dbg}" "[aot] DOOR FIRE slot=0 rect=(9100,5700,hw=1000,hh=1000) target_cut=9 spawn=(-26450,-1800,10250)" h "Leiter hinab feuert mit dem Zwillings-Cut 9")
_muss("${_dbg}" "loaded room20b0.rdt" h "ROOM20B0 geladen")
_darf_nicht("${_dbg}" "loaded room2000.rdt" h "nach der Flut nie ROOM2000")
_muss("${_dbg}" "EXIT_AT: Bild" h "Endbild in ROOM20B0 erreicht")

# ---- I: 2030 nach der Flut ohne Strom (Gegenprobe) und mit Strom (Relinghoehe, Nachtraege 7/8) ----------------
_lauf(i1 180 ${_leon}
      RE15_DEBUG_JUMP=2030@240 "RE15_SET_FLAG=9:101,9:102,9:103,9:100"
      "RE15_EXIT_AT=60#2030")
_muss("${_dbg}" "[ventilkette41] ROOM2030: unter Wasser (y -7200), Lift-Rufzonen 4/6 -> msg 1" i1 "Flut-Stand ohne Strom")
_muss_re("${_dbg}" "oid=0x04 pos=[(]-10800,-7200,-12400[)]" i1 "Wasserflaeche A bei y -7200")
_muss_re("${_dbg}" "oid=0x05 pos=[(]-10800,-7200,3800[)]" i1 "Wasserflaeche B bei y -7200")
_lauf(i2 180 ${_leon}
      RE15_DEBUG_JUMP=2030@240 "RE15_SET_FLAG=9:101,9:102,9:103,9:100,3:126"
      "RE15_EXIT_AT=60#2030")
_darf_nicht("${_dbg}" "unter Wasser" i2 "mit Strom steht 2030 nicht mehr ueber dem Ring unter Wasser")
_muss("${_dbg}" "[ventilkette41] ROOM2030: Strom (3,126) -> Wasser knapp unter der Absperrung y -5350 (Schachtoeffnung), Treppe Slot 8 -> Sperrtext 48, Ring frei" i2 "Pegel knapp unter der Absperrung (Nachtrag 12 (k))")
_muss_re("${_dbg}" "oid=0x04 pos=[(]0,-5350,0[)]" i2 "EINE Wasserflaeche (obj 4) auf -5350")
_darf_nicht("${_dbg}" "oid=0x04 pos=(-10800" i2 "keine Flutflaeche A mit Strom")
_darf_nicht("${_dbg}" "oid=0x05" i2 "keine zweite Flaeche mit Strom")
_muss("${_dbg}" "[ventilkette41] ROOM2030: Tuer Slot 0 -> ROOM20B0 Cut 11 (Ost-Tuer nach der Flut)" i2 "Ost-Tuer bleibt 20B0")
_muss_re("${_dbg}" "oid=0x00 pos=[(]-1075,-1800,-275[)]" i2 "Bruecke unten (Original-Lage @0x01B6C)")
_muss("${_dbg}" "EXIT_AT: Bild" i2 "Endbild erreicht")

# ---- J: Elza, ROOM2051 ---------------------------------------------------------------------------------------
_lauf(j 300 ${_elza}
      RE15_DEBUG_JUMP=2050@250 "RE15_PLAYER_POS=4900,-7100,1024"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=${_pult_skript}"
      "RE15_FIRE_AOT=6@900#2051"
      "RE15_EXIT_AT=1260#2051")
_muss("${_dbg}" "Spieler-Familie PL04" j "Elza gewaehlt (RE15_PSELECT_AUTO_SWITCH)")
_muss("${_dbg}" "loaded room2051.rdt" j "ROOM2051 geladen")
_pult_pruefen(j 2051)

# ---- K: Elza, ROOM2001: Leiter = Sperrtext 40 (Wortlaut-Vorgabe), kein Raumwechsel ----------------------------
_lauf(k 240 ${_elza}
      RE15_DEBUG_JUMP=2000@250 "RE15_PLAYER_POS=-26000,10300,2048"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W2,A0.1,W3"
      "RE15_EXIT_AT=520#2001")
_muss("${_dbg}" "Spieler-Familie PL04" k "Elza gewaehlt")
_muss("${_dbg}" "[ventilkette41] ROOM2001: Leiter Slot 4 (@0x0169E) -> Sperrtext 40" k "Leiter 2001 als Sperrtext")
_muss("${_dbg}" "[msgtext] room=2001 id=40 \"I cannot climb up the ladder because of the water pressure.\"" k "Text 40 woertlich")
_darf_nicht("${_dbg}" "loaded room2051.rdt" k "die Leiter darf nicht nach ROOM2051 fuehren")
_muss("${_dbg}" "EXIT_AT: Bild" k "Endbild in ROOM2001 erreicht")

# ---- L/L2/M/M2: Ring-Zugaenge des gefluteten 2030 (Entscheid C-E2) ---------------------------------------------
_lauf(l 240 ${_leon}
      RE15_DEBUG_JUMP=20B0@240 "RE15_PLAYER_POS=4900,-6400,0,1" "RE15_SET_FLAG=3:235,9:103,9:100"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W4,A0.1,W3"
      "RE15_EXIT_AT=520#20B0")
_muss("${_dbg}" "[ventilkette41] ROOM20B0: Ring-Tuer Slot 3 nach 2030 gesperrt (geflutet) -> Sperrtext 41" l "Ost-Tuer gesperrt")
_muss("${_dbg}" "[msgtext] room=20b0 id=41 \"I cannot open the door because of the water pressure.\"" l "Sperrtext 41")
_darf_nicht("${_dbg}" "DOOR FIRE slot=3" l "die Ost-Tuer darf im gefluteten Zustand nicht feuern")
_darf_nicht("${_dbg}" "loaded room2030.rdt" l "kein Raumwechsel in das geflutete 2030")
_muss("${_dbg}" "EXIT_AT: Bild" l "Endbild in ROOM20B0 erreicht")
_lauf(l2 240 ${_leon}
      RE15_DEBUG_JUMP=20B0@240 "RE15_PLAYER_POS=4900,-6400,0,1" "RE15_SET_FLAG=3:235,9:103,9:100,3:126"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W6"
      "RE15_EXIT_AT=60#2030")
_darf_nicht("${_dbg}" "Ring-Tuer Slot 3" l2 "mit Strom keine Sperre")
_muss("${_dbg}" "[aot] DOOR FIRE slot=3 rect=(5950,-6400" l2 "Ost-Tuer feuert mit Strom (Nachtrag 7)")
_muss("${_dbg}" "[ventilkette41] ROOM2030: Strom (3,126) -> Wasser knapp unter der Absperrung" l2 "Ring trocken (Pegel unter der Absperrung)")
_muss("${_dbg}" "loaded room2030.rdt" l2 "ROOM2030 geladen")
_muss("${_dbg}" "EXIT_AT: Bild" l2 "Endbild in ROOM2030 erreicht")
_lauf(m 240 ${_leon}
      RE15_DEBUG_JUMP=2070@240 "RE15_PLAYER_POS=4400,-6400,0,0" "RE15_SET_FLAG=9:103,9:100"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W4,A0.1,W3"
      "RE15_EXIT_AT=520#2070")
_muss("${_dbg}" "[ventilkette41] ROOM2070: Ring-Tuer Slot 1 nach 2030 gesperrt (geflutet) -> Sperrtext 40" m "Slot 1 gesperrt")
_muss("${_dbg}" "[ventilkette41] ROOM2070: Ring-Tuer Slot 2 nach 2030 gesperrt (geflutet) -> Sperrtext 40" m "Slot 2 gesperrt")
_muss("${_dbg}" "[msgtext] room=2070 id=40 \"I cannot open the door because of the water pressure.\"" m "Sperrtext 40")
_darf_nicht("${_dbg}" "loaded room2030.rdt" m "kein Raumwechsel in das geflutete 2030")
_muss("${_dbg}" "EXIT_AT: Bild" m "Endbild in ROOM2070 erreicht")
_lauf(m2 240 ${_leon}
      RE15_DEBUG_JUMP=2070@240 "RE15_PLAYER_POS=4400,-6400,0,0" "RE15_SET_FLAG=9:103,9:100,3:126"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W6"
      "RE15_EXIT_AT=60#2030")
_darf_nicht("${_dbg}" "Ring-Tuer Slot" m2 "mit Strom keine Sperre")
_muss("${_dbg}" "[aot] DOOR FIRE slot=1" m2 "Slot 1 feuert mit Strom (Nachtrag 7)")
_muss("${_dbg}" "loaded room2030.rdt" m2 "ROOM2030 geladen")
_muss("${_dbg}" "EXIT_AT: Bild" m2 "Endbild in ROOM2030 erreicht")

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_ventilkette_b: L/L2/M/M2 (Ring-Zugaenge 20B0/2070 gesperrt ohne Strom, offen mit (3,126)), D (Pult 2050 -> 80 V -> Hebel = Original), E (Hebel gesperrt), E2/E3/E4 (Kisten-Hebel ohne/mit Pult, Elza), F (2030-Pult ohne Ventil 13), G/H (Leitern 20B0 <-> 2050), I (2030 Flut ohne Strom / Pegel -5350 mit Strom), J (Elza 2051), K (Elza Leiter 2001) bestanden")
