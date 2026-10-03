# =============================================================================
# RUNDE 35 SPUR E MIT DER ECHTEN EXE — Inventar nach der Ada-Szene ROOM1050, Messer-Rueckfall,
# Selbstgespraech wie ROOM1170. Dossier analysis/befunde_runde35/E_inventar1050.md.
#
# Die Unit-Riegel (unit_r34n_d_adaruf_*, unit_r35_inventar1050_*) messen die Mechanik Takt fuer Takt;
# dieser Riegel misst den Nutzerweg in der exe:
#   A  Titel -> Debug-Sprung ROOM1000 -> Tuer -> ROOM1050 -> vor zur 10A0-Tuer -> Quadrat = Ada-Szene
#      -> START nach dem Szenenende: Statusschirm OFFEN (Punkt 1, Pad-Bit 0x01000000 geloescht);
#      waehrend Leons Zeilen Gierung zur Tuer (+X), Clip 18 und 17, nie Clip 19 (Punkt 3).
#   B  Neues Spiel: Startinventar ohne Messer, nichts ausgeruestet (mg=-1), Zielen + Quadrat = Messer-
#      stich (W01-Bank, Zielclip 7) (Punkt 2).
#   C  CONTINUE mit einem ALTEN Spielstand (Messer in Platz 0, ausgeruestet; Karte vom Werkzeug
#      `test_r35_inventar1050 karte`): Messer wird beim Laden entfernt, Stich trotzdem mit dem Messer.
#
# RE15_SOFTWARE_RENDER=1 nur fuer die Robustheit des Hakens; geprueft werden debug.log und das
# Zustandslog (RE15_STATE_LOG), die Bilder belegt die Framedump-Abnahme im Dossier.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<test_r35_inventar1050> -DWORKDIR=<dir>
#               -P test_r35_inventar1050.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r35_inventar1050: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r35_inventar1050: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r35_inventar1050: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name (Begruendung test_r34n_b_cursor.cmake: local_build.sh-Fallback beendet sonst jede
# re15_pc.exe der Maschine); Kopie neben der exe (Asset-Wurzel = exe-Verzeichnis).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r35e_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r35e_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

# Ein Lauf; liefert debug.log und state.log als Text.
function(e_lauf _name _out_dbg _out_state)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/debug.log" "${WORKDIR}/state.log")
    if(_name STREQUAL "c")
        execute_process(COMMAND "${RE15_KARTE_TOOL}" karte "re15_card.mcr"
                        WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                        RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
        if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
            message(FATAL_ERROR "r35_inventar1050[c]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
        endif()
    endif()
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
        RE15_STATE_LOG=state.log RE15_INV_DBG=1
        ${ARGN}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.log")
        message(FATAL_ERROR "r35_inventar1050[${_name}]: debug.log/state.log fehlt (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    file(READ "${WORKDIR}/state.log" _st)
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r35_inventar1050[${_name}]: Endbild nicht erreicht (exit=${_rv}, ${WORKDIR})")
    endif()
    set(${_out_dbg} "${_dbg}" PARENT_SCOPE)
    set(${_out_state} "${_st}" PARENT_SCOPE)
endfunction()

function(e_muss _name _text _wort)
    string(FIND "${_text}" "${_wort}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r35_inventar1050[${_name}]: erwartet '${_wort}' - fehlt")
    endif()
endfunction()

set(_start_soll "[messer] Startinventar Charakter 0: 03 x15 15 x50 | Ausruest-Platz 0x80, Waffe 1")

# --- A: Szene + Inventar danach ---------------------------------------------------------------
e_lauf(a _dbg _st
    RE15_SET_FLAG=3:121 RE15_DEBUG_JUMP=1000@120 RE15_PLAYER_POS=22230,-13400,0,0
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_INPUT_SCRIPT=U0.8,W14
    RE15_PRESS=square@150,start@560 RE15_EXIT_AT=620)
e_muss(a "${_dbg}" "${_start_soll}")
e_muss(a "${_dbg}" "[adaruf] Ereignis 13: Szene startet")
# Zustandsbild direkt vor dem START-Druck (ROOM1050, Bildzaehler ab dem Raumwechsel)
string(REGEX MATCH "\nF559 [^\n]* pf=([0-9A-F]+) pm=([0-9]+)" _m "${_st}")
if(NOT _m)
    message(FATAL_ERROR "r35_inventar1050[a]: Zustandszeile F559 fehlt")
endif()
set(_pf "${CMAKE_MATCH_1}")
set(_pm "${CMAKE_MATCH_2}")
math(EXPR _padbit "0x${_pf} & 0x01000000")
if(NOT _padbit EQUAL 0 OR NOT _pm EQUAL 0)
    message(FATAL_ERROR "r35_inventar1050[a]: nach der Szene pf=${_pf} pm=${_pm} - Pad-Bit 0x01000000 "
                        "steht noch (Punkt 1: Inventar gesperrt)")
endif()
string(REGEX MATCH "\\[invdbg\\] F(5[6-9][0-9]|6[0-2][0-9]) stage=[0-9]+ open=1" _auf "${_dbg}")
if(NOT _auf)
    message(FATAL_ERROR "r35_inventar1050[a]: START F560 nach der Szene oeffnet den Statusschirm NICHT")
endif()
# Leons Zeilen: jede Zeile mit Clip 18/17 hat Gierung zur Tuer (|Gierung| <= 192 = 2 x 0x60 @0x800313d4)
string(REGEX MATCHALL "rot=[0-9]+,hp=[-0-9]+\\) pst=[0-9]+ ps1=[0-9]+ ps2=[0-9]+ mo=1[78] " _gesten "${_st}")
list(LENGTH _gesten _ng)
if(_ng LESS 100)
    message(FATAL_ERROR "r35_inventar1050[a]: zu wenige Gestenbilder mit Clip 18/17 (${_ng})")
endif()
set(_falsch 0)
foreach(_g IN LISTS _gesten)
    string(REGEX MATCH "rot=([0-9]+)," _r "${_g}")
    set(_rot "${CMAKE_MATCH_1}")
    math(EXPR _rm "${_rot} % 4096")
    if(_rm GREATER 192 AND _rm LESS 3904)
        math(EXPR _falsch "${_falsch} + 1")
    endif()
endforeach()
if(_falsch GREATER 0)
    message(FATAL_ERROR "r35_inventar1050[a]: ${_falsch} von ${_ng} Gestenbildern NICHT zur Tuer gewandt "
                        "(Punkt 3: Drehung zur Kamera?)")
endif()
string(REGEX MATCH " mo=19 " _c19 "${_st}")
if(_c19)
    message(FATAL_ERROR "r35_inventar1050[a]: Clip 19 (Gespraechsgeste) kommt vor - Punkt 3")
endif()
message(STATUS "r35_inventar1050[a]: Szene, pf=${_pf} pm=${_pm} vor START, Statusschirm offen, ${_ng} Gestenbilder zur Tuer, kein Clip 19")

# --- B: Neues Spiel, Messerstich ohne Waffe --------------------------------------------------------
e_lauf(b _dbg _st
    RE15_DEBUG_JUMP=1000@120 RE15_PLAYER_POS=22230,-13400,0,0
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100
    "RE15_INPUT_SCRIPT=W1,M1,MA0.1,M0.6,MA0.1,M0.6,MA0.1,M0.6,W1" RE15_EXIT_AT=300)
e_muss(b "${_dbg}" "${_start_soll}")
e_muss(b "${_dbg}" "W-bank -> W01")
string(REGEX MATCHALL "mg=-?[0-9]+ " _mg "${_st}")
list(REMOVE_DUPLICATES _mg)
if(NOT _mg STREQUAL "mg=-1 ")
    message(FATAL_ERROR "r35_inventar1050[b]: ausgeruestet sollte nichts sein (mg=-1), gefunden: ${_mg}")
endif()
string(REGEX MATCHALL "pad=8800 [^\n]* ac=7 " _stich "${_st}")
list(LENGTH _stich _ns)
if(_ns LESS 3)
    message(FATAL_ERROR "r35_inventar1050[b]: Messerstich (Zielclip 7 bei R1+Quadrat) nicht gefunden (${_ns})")
endif()
message(STATUS "r35_inventar1050[b]: Startinventar ohne Messer, mg=-1, ${_ns} Stichbilder (W01, Clip 7)")

# --- C: alter Spielstand mit Messer -> Messer entfernt, Stich mit dem Messer ----------------------
e_lauf(c _dbg _st
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
    "RE15_INPUT_SCRIPT=W1,M1,MA0.1,M0.6,MA0.1,M0.6,MA0.1,M0.6,W1" "RE15_EXIT_AT=260#1000")
e_muss(c "${_dbg}" "CONTINUE: resumed in room 1000")
e_muss(c "${_dbg}" "[messer] alter Spielstand: 1 Messer entfernt, Ausruest-Platz 0x80, Waffe 1")
string(REGEX MATCHALL "mg=-?[0-9]+ " _mg "${_st}")
list(REMOVE_DUPLICATES _mg)
if(NOT _mg STREQUAL "mg=-1 ")
    message(FATAL_ERROR "r35_inventar1050[c]: nach dem Laden sollte nichts ausgeruestet sein (mg=-1): ${_mg}")
endif()
string(REGEX MATCHALL "pad=8800 [^\n]* ac=7 " _stich "${_st}")
list(LENGTH _stich _ns)
if(_ns LESS 3)
    message(FATAL_ERROR "r35_inventar1050[c]: Messerstich nach dem Laden nicht gefunden (${_ns})")
endif()
message(STATUS "r35_inventar1050[c]: alter Stand geladen, Messer entfernt, ${_ns} Stichbilder")
message(STATUS "r35_inventar1050: OK")
