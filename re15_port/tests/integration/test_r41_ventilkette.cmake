# =============================================================================
# VENTILKETTE MIT DER ECHTEN EXE (Runde 41 Spur C): der echte Weg 2030 -> Ost-Tuer -> ROOM20B0 und die Leiter 2000.
#
# Dossier analysis/befunde_runde41/C_ventilkette.md, Konstanten include/re15_ventilkette_r41.h. Der Unit-Riegel
# unit_r41_ventilkette misst die Mechanik Takt fuer Takt (auch Zaehler-Ende -> Flut -> Tod); dieser Riegel misst, was
# nur die exe zeigt: den Eingabepfad (Aktionstaste an den Ventilen, Ja/Nein, Warnung), den Zaehlerstart im Spiel-Takt,
# die umgelenkte Ost-Tuer (echter Tuer-Feuerpfad re15_aot_fire_slot -> Raumlader) und den Raumwechsel nach ROOM20B0
# mit dem Setzen der Bits 103 + 100; dazu die Leiter in 2000 als Sperrtext (kein Raumwechsel nach 2050).
#
#   A  ROOM2030 per Debug-Sprung, RE15_PLAYER_POS (6550,-100) Blick -x (2048) = auf die Ventile 04/05/06 (der
#      Debug-Sprung selbst stellt den Spieler mit Blick +x = auf die Ost-Tuer; gemessen 2026-10-23 21:52: Aktion bei
#      F330 feuerte die Tuer Slot 0 nach 2070). Gemessen 21:56 (RE15_STATE_LOG): F330 Aktion -> Frage 45 oeffnet F331
#      (zwei Seiten + Ja/Nein); F453 + F531 spulen/blaettern, F654 = Ja -> 45 zu F655, Warnung 46 oeffnet F657;
#      F777 schliesst die Warnung (im Flut-Lauf: Druck 120 Bilder nach dem Oeffnen schliesst) -> Marker -> Zaehler.
#      F900 RE15_FIRE_AOT=0 (die umgelenkte Ost-Tuer feuert ueber den echten Tuerpfad) -> ROOM20B0 geladen,
#      "Zaehler lief beim Verlassen" (Bits 103 + 100), Framedump F970 (20B0 Eintrittsszene), Ende F1040.
#   B  ROOM2000 per Debug-Sprung + RE15_PLAYER_POS vor die Leiter (-26000,10300, Blick -x; Vorwaertspunkt 620 -> x -26620
#      liegt im Leiter-Rechteck x -27600..-26600 @0x0169E, bei -25900 nicht - gemessen 22:12): F330 Aktion ->
#      Sperrtext 40, KEIN Raumwechsel nach ROOM2050 bis zum Ende (F520).
#   C  Die Flut am Spiel (gemessen 2026-10-23 22:00, Dossier §4 "Flut am Spiel", C_belege/flut_echt_sheet.png):
#      Spieler (6550,-100) Blick +z (3072; Vorwaertspunkt trifft keinen Platz), die drei Spinnen als Spielzustand
#      "schon getoetet" ueber ihre Kill-Bits (Sce_em_set pc[7] = 0x9b/0x9c/0x9d @0x01C18/@0x01C2C/@0x01C40, EM-Status-
#      Gate FUN_800420a0 @0x80042120-38, Zone 7 = STAGE2) - kein Haken, kein Spielverhalten. F330 RE15_FIRE_AOT=14
#      (der Ventil-Platz, Ereignis 26) -> Frage 45; Druecke alle 4 s ab F450: F696 Ja (46 oeffnet F699), F819 zu
#      -> Marker F820 -> Sleep 1800 -> F2621 Flut (Cut 4, Pad-Sperre), Wasser 50 je Takt ab y 0: Fuesse (-5400)
#      bei F2728 (dort EIN Spritzer, Wasser-Schritt-Effekt 0x06050D00 @0x800456b8 - Ereignis statt der frueheren
#      Rate ohne Beleg), -6300 = 900 ueber den Fuessen bei F2746 -> hp -1 + blaue Blende
#      (fade-log kick ch=0 value=0x5000), Todeskette (pst 7 bei F2859). Riegel: Zaehler-Ende 1800 +- 3 Bilder nach dem
#      Schliessen, Tod 125 +- 2 Bilder nach dem Flutbeginn, Blende, Wasser -6300 / Spieler -5400.
#
# Die Spinnen (main00 Switch auf den Eintritts-Cut, Case 0 @0x01C12) greifen in A nach wenigen Sekunden an; A bleibt
# deshalb unter 35 s im Raum (Zaehler-Start reicht); C laeuft ohne sie (Kill-Bits, s. o.).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_ventilkette.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_ventilkette: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_ventilkette: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41c_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r41c_haken_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r41c_haken_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(_muss _text _wort _name _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r41_ventilkette[${_name}]: ${_was} — '${_wort}' fehlt im debug.log")
    endif()
endfunction()
function(_darf_nicht _text _wort _name _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(NOT _p LESS 0)
        message(FATAL_ERROR "r41_ventilkette[${_name}]: ${_was} — '${_wort}' steht im debug.log")
    endif()
endfunction()

# ---- A: 2030 Ventile -> Zaehler -> Ost-Tuer -> 20B0 ---------------------------------------------------------
set(WORKDIR "${_basis}_a")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rva 300
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=2030@240
    "RE15_PLAYER_POS=6550,-100,2048"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330
    "RE15_INPUT_SCRIPT=A0.1,W4,A0.1,W2.5,A0.1,W4,A0.1,W4,A0.1,W4"
    RE15_MSG_LOG=1
    "RE15_FIRE_AOT=0@900#2030"
    "RE15_FRAMEDUMP=970:${WORKDIR}/f20b0_970.ppm"
    "RE15_EXIT_AT=1040#20B0"
    "${RE15_PC_EXE}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r41_ventilkette[a]: kein debug.log (exit=${_rva}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
_muss("${_dbg}" "JUMP -> 203 TREATMENT POOL (ROOM2030)" a "Sprung nach 2030")
_muss("${_dbg}" "[ventilkette41] ROOM2030: Bruecke obj 0 oben y -5400" a "Bruecke oben beim Raumstart")
_muss("${_dbg}" "[ventilkette41] ROOM2030 Ereignis 26 -> Port-Programm 2030_ventile" a "Aktion an den Ventilen startet das Programm")
_muss("${_dbg}" "[msg] room=2030 id=45" a "Frage 45")
_muss("${_dbg}" "[msg] room=2030 id=46" a "Warnung 46 (Ja gewaehlt)")
_muss("${_dbg}" "[ventilkette41] Zaehler laeuft: 1800 Takte, Ost-Tuer -> 20B0, Wasser geparkt" a "Zaehler im Spiel-Takt")
_muss("${_dbg}" "[ventilkette41] ROOM2030: Tuer Slot 0 -> ROOM20B0 Cut 11 (Zaehler laeuft)" a "Ost-Tuer umgelenkt")
_muss("${_dbg}" "[fire-aot] slot=0" a "Ost-Tuer gefeuert (echter Tuerpfad)")
_muss("${_dbg}" "[esp] room 20B0:" a "ROOM20B0 geladen (Umlenkung wirksam)")
_muss("${_dbg}" "[ventilkette41] Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100" a "Flut gelaufen + Leiter frei")
_darf_nicht("${_dbg}" "[esp] room 2070:" a "ohne Umlenkung waere die Ost-Tuer nach ROOM2070 gegangen")
_muss("${_dbg}" "EXIT_AT: Bild" a "Endbild in ROOM20B0 erreicht")
if(NOT EXISTS "${WORKDIR}/f20b0_970.ppm")
    message(FATAL_ERROR "r41_ventilkette[a]: Framedump F970 fehlt")
endif()

# ---- B: 2000 Leiter = Sperrtext, kein Raumwechsel ------------------------------------------------------------
set(WORKDIR "${_basis}_b")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rvb 240
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=2000@240
    "RE15_PLAYER_POS=-26000,10300,2048"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330
    "RE15_INPUT_SCRIPT=A0.1,W2,A0.1,W3"
    RE15_MSG_LOG=1
    "RE15_FRAMEDUMP=360:${WORKDIR}/f2000_360.ppm"
    "RE15_EXIT_AT=520#2000"
    "${RE15_PC_EXE}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r41_ventilkette[b]: kein debug.log (exit=${_rvb}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbgb)
_muss("${_dbgb}" "[ventilkette41] ROOM2000: Leiter Slot 4 (@0x0169E) -> Sperrtext 40" b "Leiter als Sperrtext angelegt")
_muss("${_dbgb}" "[msgtext] room=2000 id=40" b "Aktion an der Leiter zeigt Text 40 (sce-1-Weg loggt [msgtext])")
_darf_nicht("${_dbgb}" "[esp] room 2050:" b "die Leiter darf nicht nach ROOM2050 fuehren")
_darf_nicht("${_dbgb}" "[aot] DOOR FIRE slot=4" b "Slot 4 darf keine Tuer mehr sein")
_muss("${_dbgb}" "EXIT_AT: Bild" b "Endbild in ROOM2000 erreicht")

# ---- C: 2030 Zaehler abgelaufen -> Flut ueber die Ringebene -> Ertrinken --------------------------------------
set(WORKDIR "${_basis}_c")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rvc 420
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=2030@240
    "RE15_PLAYER_POS=6550,-100,3072"
    "RE15_SET_FLAG=7:155,7:156,7:157"
    "RE15_FIRE_AOT=14@330#2030"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=450
    "RE15_INPUT_SCRIPT=A0.1,W4,A0.1,W4,A0.1,W4,A0.1,W4,A0.1,W4,A0.1,W4,A0.1,W4,A0.1,W4"
    RE15_MSG_LOG=1 RE15_FADE_LOG=1 RE15_STATE_LOG=state.txt
    "RE15_FRAMEDUMP=2750:${WORKDIR}/f2030_2750.ppm"
    "RE15_EXIT_AT=2960#2030"
    "${RE15_PC_EXE}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r41_ventilkette[c]: kein debug.log (exit=${_rvc}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbgc)
_muss("${_dbgc}" "[fire-aot] slot=14" c "Ventil-Platz (Ereignis 26) gefeuert")
_muss("${_dbgc}" "[ventilkette41] ROOM2030 Ereignis 26 -> Port-Programm 2030_ventile" c "Programm gestartet")
_muss("${_dbgc}" "[msg] room=2030 id=46" c "Warnung 46 (Ja gewaehlt)")
_muss("${_dbgc}" "[ventilkette41] Zaehler laeuft: 1800 Takte, Ost-Tuer -> 20B0, Wasser geparkt" c "Zaehler gestartet")
_muss("${_dbgc}" "[ventilkette41] FLUT: Wasser steigt" c "Zaehler abgelaufen -> Flut")
_muss("${_dbgc}" "room=2030 CUT 0 -> 4" c "Flut unter Cut 4 (Draufsicht)")
_muss("${_dbgc}" "[ventilkette41] ERTRUNKEN: Wasser y -6300, Spieler y -5400 -> hp -1, blaue Blende" c "Ertrinken 900 ueber den Fuessen auf dem Ring")
string(FIND "${_dbgc}" "[ventilkette41] SPRITZER: Wasser y -5400 erreicht die Fuesse (Spieler y -5400)" _p_spritz)
string(FIND "${_dbgc}" "[ventilkette41] ERTRUNKEN:" _p_tot)
if(_p_spritz LESS 0 OR _p_tot LESS 0 OR NOT _p_spritz LESS _p_tot)
    message(FATAL_ERROR "r41_ventilkette[c]: kein Spritzer beim Erreichen der Fuesse vor dem Ertrinken (Spritzer ${_p_spritz}, tot ${_p_tot})")
endif()
string(REGEX MATCHALL "SPRITZER: Wasser y" _spritz_alle "${_dbgc}")
list(LENGTH _spritz_alle _n_spritz)
if(NOT _n_spritz EQUAL 1)
    message(FATAL_ERROR "r41_ventilkette[c]: ${_n_spritz} Spritzer-Ereignisse statt genau einem (Ereignis, keine Rate)")
endif()
_muss("${_dbgc}" "kick ch=0 value=0x5000" c "blaue Blende Kanal 0 (level 0x5000)")
_darf_nicht("${_dbgc}" "[esp] room 2070:" c "kein Raumwechsel waehrend der Flut")
_muss("${_dbgc}" "EXIT_AT: Bild" c "Endbild erreicht")
# Zeiten aus dem Zustandslog (Bildnummern des Spiels, RE15_STATE_LOG): Schliessen der Warnung = letzter Wechsel der
# Pause-Maske FFFF0007 -> 00000007 vor der Flut; Flut = erstes Bild mit cam=4; Tod = erstes Bild mit hp=-1.
if(NOT EXISTS "${WORKDIR}/state.txt")
    message(FATAL_ERROR "r41_ventilkette[c]: state.txt fehlt")
endif()
file(STRINGS "${WORKDIR}/state.txt" _st REGEX "^F[0-9]+ pad=.*PL\\(6550,-100,rot=3072")
set(_f_zu -1)
set(_f_flut -1)
set(_f_tot -1)
set(_pf_vorher "")
foreach(_z IN LISTS _st)
    string(REGEX MATCH "^F([0-9]+) " _m "${_z}")
    set(_f "${CMAKE_MATCH_1}")
    string(REGEX MATCH "pf=([0-9A-F]+)" _m2 "${_z}")
    set(_pf "${CMAKE_MATCH_1}")
    if(_f_flut LESS 0 AND _pf_vorher STREQUAL "FFFF0007" AND _pf STREQUAL "00000007")
        set(_f_zu "${_f}")
    endif()
    set(_pf_vorher "${_pf}")
    if(_f_flut LESS 0 AND _z MATCHES " cam=4 ")
        set(_f_flut "${_f}")
    endif()
    if(_f_tot LESS 0 AND _z MATCHES "hp=-1\\)")
        set(_f_tot "${_f}")
    endif()
endforeach()
message(STATUS "r41_ventilkette[c]: Warnung zu F${_f_zu}, Flut F${_f_flut}, tot F${_f_tot}")
if(_f_zu LESS 0 OR _f_flut LESS 0 OR _f_tot LESS 0)
    message(FATAL_ERROR "r41_ventilkette[c]: Bildnummern nicht gefunden (zu ${_f_zu}, Flut ${_f_flut}, tot ${_f_tot})")
endif()
math(EXPR _d_zaehler "${_f_flut} - ${_f_zu}")
math(EXPR _d_tod "${_f_tot} - ${_f_flut}")
if(_d_zaehler LESS 1798 OR _d_zaehler GREATER 1804)
    message(FATAL_ERROR "r41_ventilkette[c]: Zaehler ${_d_zaehler} Bilder (erwartet 1800 +- 3: Sleep 1800 '09 0a 08 07', 60 s bei 30 Takten)")
endif()
if(_d_tod LESS 123 OR _d_tod GREATER 127)
    message(FATAL_ERROR "r41_ventilkette[c]: Tod ${_d_tod} Bilder nach dem Flutbeginn (erwartet 125 +- 2: 126 Schritte x 50 = 6300 = -5400 - 900, der erste Schritt im Marker-Takt)")
endif()
if(NOT EXISTS "${WORKDIR}/f2030_2750.ppm")
    message(FATAL_ERROR "r41_ventilkette[c]: Framedump F2750 fehlt")
endif()

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_ventilkette: A (2030 -> Ost-Tuer -> 20B0, Bits 103+100), B (Leiter 2000 = Text 40) und C (Zaehler ${_d_zaehler} -> Flut -> tot nach ${_d_tod}) bestanden")
