# =============================================================================
# VENTILKETTE MIT DER ECHTEN EXE (Runde 41 Spur C): der echte Weg 2030 -> Ost-Tuer -> ROOM20B0 und die Leiter 2000.
#
# Dossier analysis/befunde_runde41/C_ventilkette.md, Konstanten include/re15_ventilkette_r41.h. Der Unit-Riegel
# unit_r41_ventilkette misst die Mechanik Takt fuer Takt (auch Zaehler-Ende -> Flut -> Tod); dieser Riegel misst, was
# nur die exe zeigt: den Eingabepfad (Aktionstaste an den Ventilen, Ja/Nein, Warnung), den Zaehlerstart im Spiel-Takt,
# die umgelenkte Ost-Tuer (echter Tuer-Feuerpfad re15_aot_fire_slot -> Raumlader) und den Raumwechsel nach ROOM20B0
# mit dem Setzen der Bits 103 + 100; dazu die Leiter in 2000 als Sperrtext (kein Raumwechsel nach 2050).
#
#   A  ROOM2030 per Debug-Sprung (Ost-Tuer-Spawn (6550,-5400,-100), Blick -x = auf die Ventile 04/05/06):
#      F330 Aktion -> Frage 45 Seite 1 (Schreibmaschine ~3 s), F453 Seite 2, F531 Ja, Warnung 46 (~3 s), F654 zu;
#      Zaehler laeuft -> Tuer Slot 0 -> 20B0; F720 RE15_FIRE_AOT=0 (die Ost-Tuer feuert) -> ROOM20B0 geladen,
#      "Zaehler lief beim Verlassen" (Bits 103 + 100), Framedump F790 (20B0 Eintrittsszene).
#      (Gemessen am Pult-Lauf 2050: ein Druck waehrend der Schreibmaschine spult nur vor, erst der naechste schliesst.)
#   B  ROOM2000 per Debug-Sprung + RE15_PLAYER_POS vor die Leiter (-25900,10300, Blick -x): F330 Aktion ->
#      Sperrtext 40, KEIN Raumwechsel nach ROOM2050 bis zum Ende (F520).
#
# ⛔ Die Spinnen von 2030 (main00 Switch auf den Eintritts-Cut, Case 0 @0x01C12: drei Spinnen am Ring) greifen den
# Spieler nach wenigen Sekunden an; Lauf A bleibt deshalb unter 10 s im Raum. Der 60-s-Zaehler bis zur Flut wird im
# Unit-Riegel gemessen (dort ohne Gegner-KI).
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
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41c_haken_*.exe")
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
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330
    "RE15_INPUT_SCRIPT=A0.1,W4,A0.1,W2.5,A0.1,W4,A0.1,W4"
    RE15_MSG_LOG=1
    "RE15_FIRE_AOT=0@720#2030"
    "RE15_FRAMEDUMP=790:${WORKDIR}/f20b0_790.ppm"
    "RE15_EXIT_AT=860#20B0"
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
if(NOT EXISTS "${WORKDIR}/f20b0_790.ppm")
    message(FATAL_ERROR "r41_ventilkette[a]: Framedump F790 fehlt")
endif()

# ---- B: 2000 Leiter = Sperrtext, kein Raumwechsel ------------------------------------------------------------
set(WORKDIR "${_basis}_b")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rvb 240
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=2000@240
    "RE15_PLAYER_POS=-25900,10300,2048"
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
_darf_nicht("${_dbgb}" "[esp] room 2050:" b "die Leiter darf nicht nach ROOM2050 fuehren")
_darf_nicht("${_dbgb}" "[aot] DOOR FIRE slot=4" b "Slot 4 darf keine Tuer mehr sein")
_muss("${_dbgb}" "EXIT_AT: Bild" b "Endbild in ROOM2000 erreicht")

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_ventilkette: A (2030 -> Ost-Tuer -> 20B0, Bits 103+100) und B (Leiter 2000 = Text 40) bestanden")
