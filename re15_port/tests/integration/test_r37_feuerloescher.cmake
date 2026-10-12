cmake_minimum_required(VERSION 3.16)
# =============================================================================
# FEUERLOESCHER MIT DER ECHTEN EXE (Runde 37, Spur I). Dossier analysis/befunde_runde37/I_feuerloescher.md,
# Konstanten include/re15_feuerloescher_r37.h. Die Unit-Riegel unit_r37_feuerloescher_* pruefen die Mechanik
# am Engine-Code; dieser Riegel prueft den Nutzerweg an der exe (Lade-Weg CONTINUE, Eingabeskript, Tuer-Weg):
#
#   A  ENDE-ZU-ENDE Leon: Spielstand ROOM1050 vor der Wand (17200,-22500) Blick Norden, Cut 6 -> Aktion ->
#      "Will you take the Fire Extinguisher?" -> Ja -> (3,0x85) (ROOM1000 sub01 @0x00D0C); Sprung (Debug-Menue)
#      nach ROOM1090 an den Feuer-Platz (300,-1800,-300) -> Aktion -> msg 7/8 -> Ja -> Selbst-Tuer (@0x02726) ->
#      Neueintritt OHNE Loescher (Leon traegt ihn) -> msg 9 -> Wegwerf-Abfrage -> Ja -> Loescher liegt
#      (2500,-1800,-1300) und wird gezeichnet
#   B  TUER-WEG Leon: Spielstand ROOM1050 vor der Notausgangs-Tuer mit (9,0x86)+(3,0x85)+(3,0x81), Loescher
#      abgelegt -> Aktion an der Tuer -> ROOM1090 (Tuer-Weg scd_room_reenter) -> Loescher liegt
#   C  ELZA ROOM1091: Spielstand neben dem Loescher -> liegt von Anfang an; Aktion -> msg 29
#   D  SPIND ROOM1000 Leon: Aktion am Spind -> "Nothing unusual.", keine Aufnahme
#   E  ROOM1050 Leon ohne Aufnahme (Bild): Loescher haengt an der Wand und wird gezeichnet
#   F  Spielstand + CONTINUE in ROOM1090 (Feuer aus, Loescher abgelegt) -> liegt und wird gezeichnet
#   G  dasselbe mit dem Loescher im Inventar -> liegt NICHT (kein Doppel)
#   H  Elza Tuer-Weg ROOM1051 -> ROOM1091 -> liegt; in ROOM1051 haengt keiner
#
# Bilder je Lauf per RE15_FRAMEDUMP (RE15_WINDOW_SCALE=1) im Arbeitsordner; geprueft wird das LOG.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r37_feuerloescher_karte> -DWORKDIR=<dir>
#               -P test_r37_feuerloescher.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_feuerloescher: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r37_feuerloescher: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r37_feuerloescher: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# EIGENER EXE-NAME neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake): ein
# paralleler Bau beendet sonst per `taskkill /IM re15_pc.exe` jede re15_pc.exe der Maschine.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37i_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r37i_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

set(_fehler "")
macro(pruef _name _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(_p LESS 0)
        list(APPEND _fehler "${_name}: fehlt '${_muster}'")
        message(STATUS "FAIL ${_name}: fehlt '${_muster}'")
    else()
        message(STATUS "ok   ${_name}: '${_muster}'")
    endif()
endmacro()
macro(pruef_nicht _name _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(NOT _p LESS 0)
        list(APPEND _fehler "${_name}: unerwartet '${_muster}'")
        message(STATUS "FAIL ${_name}: unerwartet '${_muster}'")
    else()
        message(STATUS "ok   ${_name}: nicht da '${_muster}'")
    endif()
endmacro()

# Ein Lauf: Karte schreiben, exe starten, debug.log lesen.
function(fl_lauf _name _raum _kartenargs _skript _ende _extra _out)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${_kartenargs}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r37_feuerloescher[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 400
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
        "RE15_INPUT_SCRIPT=${_skript}"
        RE15_MSG_LOG=1
        "RE15_EXIT_AT=${_ende}"
        ${_extra}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r37_feuerloescher[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(FIND "${_dbg}" "CONTINUE: resumed in room ${_raum}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r37_feuerloescher[${_name}]: Spielstand in ROOM${_raum} nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r37_feuerloescher[${_name}]: Endbild nicht erreicht - Lauf vorher abgerissen "
                            "(exit=${_rv}, ${WORKDIR})")
    endif()
    message(STATUS "r37_feuerloescher[${_name}]: exit=${_rv}, ${WORKDIR}")
    set(${_out} "${_dbg}" PARENT_SCOPE)
endfunction()

# --- A: Ende-zu-Ende ---------------------------------------------------------------------------
# Skript-Takte (30 Hz ab Spielbild 60): A@30 Aufnahme, A@138 Ja; Sprung F420; A@381 Feuer (msg 7),
# A@444 weiter (msg 8), A@537 Ja (Loeschen), danach A je 2 s (msg 9 zu, Wegwerf-Abfrage Ja auf "Yes").
fl_lauf(a 1050 "1050;17200;-22500;1024;cut:6"
    "W1,A0.1,W3.5,A0.1,W8,A0.1,W2,A0.1,W3,A0.1,W2,A0.1,W2,A0.1,W2,A0.1,W2,A0.1,W2,A0.1,W20"
    1400 "RE15_DEBUG_JUMP=1090@420;RE15_PLAYER_POS=300,-300,0,1;RE15_FRAMEDUMP=300-1400/50:fd_" _a)
pruef(A _a "[feuerloescher] ROOM1050: Loescher an der Wand obj 5 bei (17200,-1150,-23775)")
pruef(A _a "oid=0x05 pos=(17200,-1150,-23775)")
pruef(A _a "[feuerloescher] Aufnahme Item 0x31 (9,0x86) -> Set(3,0x85)=1")
pruef(A _a "[feuerloescher] ROOM1090: kein Loescher (Feuer brennt noch, (3,0x81)=0)")
pruef(A _a "id=8 \"Will you use the Fire Extinguisher?\"")
pruef(A _a "[feuerloescher] ROOM1090: kein Loescher (Leon traegt ihn noch (Item 0x31 im Inventar))")
pruef(A _a "id=9 \"You've used the Fire Extinguisher.\"")
pruef(A _a "[feuerloescher] Loescher weggeworfen -> liegt in ROOM1090")
pruef(A _a "[feuerloescher] ROOM1090: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300) rot_y 3250, Zone Slot 6")
pruef(A _a "oid=0x05 pos=(2500,-1800,-1300) rot=(0,3250,0)")

# --- B: Tuer-Weg 1050 -> 1090 mit abgelegtem Loescher ----------------------------------------------
# Tuer: 9 Bilder Halten (aot_common.c door_hold, FUN_8002bd44) -> A0.5 = 15 Bilder.
fl_lauf(b 1050 "1050;19850;-22300;1024;cut:6;flag:9:0x86;flag:3:0x85;flag:3:0x81"
    "W1,A0.5,W12" 520 "" _b)
pruef(B _b "[feuerloescher] ROOM1050: kein Loescher (schon genommen, (9,0x86)=1)")
pruef(B _b "[feuerloescher] ROOM1090: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300)")

# --- C: Elza ROOM1091 ------------------------------------------------------------------------------
# Die vier Zombies aus ROOM1091 main00 (Sce_em_set pc[7] = 0xF8..0xFB) als erledigt (Stage-1-Status-Zone 7,
# re15_em_status_zone; Spawn-Sperre FUN_800420a0 @0x80042120-38) — sonst packt Zombie 3 (850,-2350) Elza
# vor dem Druck (gemessen: Bild 30 Griff, Lage (1163,-2043) statt vor dem Loescher).
fl_lauf(c 1091 "1091;1880;-1300;0;y:-1800;cut:6;elza;flag:7:0xf8;flag:7:0xf9;flag:7:0xfa;flag:7:0xfb"
    "W1.5,A0.1,W4" 300 "RE15_FRAMEDUMP=80-80/50:fd_" _c)
pruef(C _c "[feuerloescher] ROOM1091: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300)")
pruef(C _c "id=29 \"It's an empty fire extinguisher.\"")

# --- D: Spind ROOM1000 -----------------------------------------------------------------------------
fl_lauf(d 1000 "1000;20900;-1100;3072"
    "W1,A0.1,W4" 260 "" _d)
pruef(D _d "[feuerloescher] ROOM1000: Spind Slot 3 (Item 0x31 @0x00C24) -> Raumtext msg 0")
pruef(D _d "room=1000 id=0 \"Nothing unusual.\"")
pruef_nicht(D _d "Aufnahme Item 0x31")

# --- E: Bild ROOM1050 ------------------------------------------------------------------------------
fl_lauf(e 1050 "1050;18900;-20800;0;cut:6" "W3" 140 "RE15_FRAMEDUMP=120-120/50:fd_" _e)
pruef(E _e "oid=0x05 pos=(17200,-1150,-23775)")

# --- F/G: Spielstand + CONTINUE direkt in ROOM1090 (Leon, Feuer aus, Rettung durch) -------------------
fl_lauf(f 1090 "1090;0;250;0;y:-1800;cut:5;flag:9:0x86;flag:3:0x85;flag:3:0x81;flag:3:0xbb;flag:3:0x6e"
    "W2" 100 "" _f)
pruef(F _f "[feuerloescher] ROOM1090: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300)")
pruef(F _f "oid=0x05 pos=(2500,-1800,-1300)")
fl_lauf(g 1090 "1090;0;250;0;y:-1800;cut:5;flag:9:0x86;flag:3:0x85;flag:3:0x81;flag:3:0xbb;flag:3:0x6e;give:0x31:1"
    "W2" 100 "" _g)
pruef(G _g "[feuerloescher] ROOM1090: kein Loescher (Leon traegt ihn noch (Item 0x31 im Inventar))")
pruef_nicht(G _g "oid=0x05")

# --- H: Elza Tuer-Weg ROOM1051 -> ROOM1091 ---------------------------------------------------------
fl_lauf(h 1051 "1051;19850;-22300;1024;cut:6;elza" "W1,A0.5,W12" 520 "" _h)
pruef(H _h "[feuerloescher] ROOM1091: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300)")
pruef_nicht(H _h "[feuerloescher] ROOM1051")

file(REMOVE "${_exe_kopie}")
if(_fehler)
    string(REPLACE ";" "\n  " _f "${_fehler}")
    message(FATAL_ERROR "r37_feuerloescher: ${_f}")
endif()
message(STATUS "r37_feuerloescher: alle Laeufe gruen")
