cmake_minimum_required(VERSION 3.16)
# =============================================================================
# Runde 38 Spur A "cursor" MIT DER ECHTEN EXE (analysis/befunde_runde38/A_cursor.md).
#
# Nutzer (AUFTRAG.md Runde 38): "Der Cursor, der zum Beispiel in ROOM 1150 oder ROOM 11F0 verwendet wird, hat immer
# noch so einen komischen Schatten. Den hat er im Original nicht..."
# Ursache: die rueckseitige Unterseite (y=+923) des Cursor-Kastens wurde gezeichnet und schien durch die durchsichtige
# Oberseite. Original: NCLIP-Verwurf MAC0 < 0 (FUN_800254a0 @0x800255a8, FUN_800256b0 @0x800257c0).
#
#   A  ROOM11F0 Generator-Raetsel: Debug-Sprung, Kamera Cut 10 (Raetselkamera, sub16), Raetsel scharf wie sub16
#      @0x015C2..0x015F2 (13x Set(5,b,1)); Bild 300. MUSS: kein rueckseitiges Cursor-Dreieck (Platz 4 = Prop 0),
#      Rahmen-Inneres ohne Schatten-Kopie, Klassenkarte G/g/Y/y gleich dem DuckStation-Bild
#      (analysis/grundwahrheit/psx_room11f0_cut10.png).
#   B  ROOM1150 Hebetisch-Cursor am LADE-Weg (Spielstand probe_r30_granate_karte, CONTINUE, Aktion am Tisch):
#      Bild 150 (Cursor seit F101 aktiv). MUSS: Cursor-Sitzung aktiv (cursor.log), kein rueckseitiges Dreieck auf
#      Platz 28 (RE15_HC_TIM_SLOT), Rahmen-Inneres ohne Schatten-Kopie.
#   C  ROOM10D0 Ziffernfeld (Runde 37 Spur E, Weg D): sub17 per RE15_SUBSTART, Nachricht zu; Bild 260. MUSS: kein
#      rueckseitiges Cursor-Dreieck (Platz 5 = Prop 1), Rahmen-Inneres ohne Schatten-Kopie.
#   D  ELZA (RE15_PSELECT_AUTO + _SWITCH) in ROOM11F1, sonst wie A; Bild 500. MUSS wie A + "[pl] Spieler-Familie PL04".
# Bildpruefung + RE15_TRILOG-Auswertung: probe_r38_cursor bild.
#
# GEGENPROBE (Dossier §4/§5): dieselben drei Laeufe mit der exe vor dem Fix (Stand 1bcbd0ff) liefern 4 bzw. 4 bzw. 4
# rueckseitige Muster-Dreiecke, 32..33 reingruene Bildpunkte im Rahmen und 21 Abweichungen gegen das Original.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PROBE=<probe_r38_cursor> -DRE15_KARTE_TOOL=<probe_r30_granate_karte>
#               -DWORKDIR=<dir> -P test_r38_cursor.cmake
# =============================================================================
foreach(_v RE15_PC_EXE RE15_PROBE RE15_KARTE_TOOL)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r38_cursor: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "r38_cursor: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; local_build.sh beendet nur die exe des eigenen
# Bauverzeichnisses, Muster test_r34n_b_cursor.cmake / test_r37_codepanel.cmake).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r38a_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe_kopie "${_exe_dir}/re15_pc_r38a_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

set(_fenster "140,100,180,138")   # Cursor-Fenster (Startlage, Bildmitte) fuer RE15_TRILOG

function(a_lauf _name _bild)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/f.ppm" "${WORKDIR}/trilog.txt" "${WORKDIR}/debug.log")
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        "RE15_TRILOG=trilog.txt@${_fenster}" "RE15_FRAMEDUMP=${_bild}:f.ppm"
        ${ARGN}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/f.ppm" OR NOT EXISTS "${WORKDIR}/trilog.txt")
        message(FATAL_ERROR "r38_cursor[${_name}]: Framedump/Trilog fehlt (exit=${_rv}, ${WORKDIR})")
    endif()
endfunction()

function(a_bild _name _slot)
    execute_process(COMMAND "${RE15_PROBE}" bild "${_basis}_${_name}/f.ppm" "${_basis}_${_name}/trilog.txt" ${_slot} ${ARGN}
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 120)
    message(STATUS "r38_cursor[${_name}]:\n${_out}")
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "r38_cursor[${_name}]: Bildpruefung ROT (exit=${_rv})\n${_out}${_err}")
    endif()
endfunction()

# --- A: ROOM11F0 Cut 10 gegen das Original ----------------------------------------------------------------
a_lauf(a 300 RE15_TITLE_SHOT=title.bmp RE15_DEBUG_JUMP=11F0@30 RE15_FORCE_CUT=10
       "RE15_SET_FLAG_AT=5:0,5:1,5:2,5:3,5:4,5:5,5:6,5:7,5:8,5:9,5:10,5:11,5:12@240" "RE15_EXIT_AT=302")
a_bild(a 4 psx11f0)

# --- B: ROOM1150 Hebetisch-Cursor, Lade-Weg ---------------------------------------------------------------
set(_wd_b "${_basis}_b")
file(MAKE_DIRECTORY "${_wd_b}")
file(REMOVE "${_wd_b}/re15_card.mcr" "${_wd_b}/cursor.log")
execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" 1150
                WORKING_DIRECTORY "${_wd_b}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
if(NOT _rvk EQUAL 0 OR NOT EXISTS "${_wd_b}/re15_card.mcr")
    message(FATAL_ERROR "r38_cursor[b]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
endif()
a_lauf(b 150 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
       RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=W1,A0.2,W4"
       RE15_HEBETISCH_CURSOR_LOG=cursor.log "RE15_EXIT_AT=152#1150")
file(READ "${_wd_b}/debug.log" _dbg_b)
string(FIND "${_dbg_b}" "CONTINUE: resumed in room 1150" _p)
if(_p LESS 0)
    message(FATAL_ERROR "r38_cursor[b]: Spielstand in ROOM1150 nicht geladen (${_wd_b})")
endif()
set(_cur "")
if(EXISTS "${_wd_b}/cursor.log")
    file(READ "${_wd_b}/cursor.log" _cur)
endif()
string(REGEX MATCH "F1[0-4][0-9] raum=1150 (aktiv|takt) cursor=2" _aktiv "${_cur}")
if(NOT _aktiv)
    message(FATAL_ERROR "r38_cursor[b]: Hebetisch-Cursor bis Bild 149 nicht aktiv (cursor.log)\n${_cur}")
endif()
a_bild(b 28)

# --- C: ROOM10D0 Ziffernfeld ------------------------------------------------------------------------------
a_lauf(c 260 RE15_TITLE_SHOT=title.bmp RE15_DEBUG_JUMP=10D0@120 "RE15_SUBSTART=17@150#10D0" "RE15_PAD_AT=210:A"
       "RE15_EXIT_AT=262")
a_bild(c 5)

# --- D: ELZA, ROOM11F1 (Spielerwahl schaltet auf Elza; Debug-Sprung 11F0 laedt fuer Elza ROOM11F1, Raumlader
#        @0x800397e4/@0x800397ec; 11F1.RDT ist bytegleich 11F0.RDT) — dieselben MUSS-Zeilen wie A --------------------
a_lauf(d 500 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
       RE15_DEBUG_JUMP=11F0@250 RE15_FORCE_CUT=10
       "RE15_SET_FLAG_AT=5:0,5:1,5:2,5:3,5:4,5:5,5:6,5:7,5:8,5:9,5:10,5:11,5:12@420" "RE15_EXIT_AT=502")
file(READ "${_basis}_d/debug.log" _dbg_d)
string(FIND "${_dbg_d}" "[pl] Spieler-Familie PL04" _p1)
string(FIND "${_dbg_d}" "in Raum 11f1 erreicht" _p2)
if(_p1 LESS 0 OR _p2 LESS 0)
    message(FATAL_ERROR "r38_cursor[d]: nicht Elza (PL04) in ROOM11F1 (${_basis}_d)")
endif()
a_bild(d 4 psx11f0)

message(STATUS "r38_cursor: A ROOM11F0, B ROOM1150, C ROOM10D0, D Elza ROOM11F1 - kein rueckseitiges Cursor-Dreieck, kein Schatten im Rahmen, 11F0/11F1 gleich dem Original")
