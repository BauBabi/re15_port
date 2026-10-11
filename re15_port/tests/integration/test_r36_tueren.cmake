# =============================================================================
# Runde 36 Spur G "tueren" MIT DER ECHTEN EXE (analysis/befunde_runde36/G_tueren.md).
#
# Die Unit-Riegel unit_r36_tueren_* messen die Mechanik headless (Spion statt Ton). Dieser Riegel prueft, was nur
# die exe zeigt: den PC-Spieler (platform/pc/src/tuer_r36_pc.c Anmeldung -> audio_pc.c Slot KARTSE.VBS -> Mischer)
# und den Tuersequenz-Laeufer mit den neuen Zeilen.
#   A  ROOM3010 Green Keycard (KARTE): RE15_SET_FLAG=3:59 (Karte da), Sprung, RE15_FIRE_AOT=6 -> sub02 Message_on 1.
#      Erwartet: auf.log drei Toene Bild +0/+15/+30 (Satz 0/1/2), debug.log je ein "[se] Stimme: se=N" mit 16397 Hz
#      (RE2 ROOM2110-Toene: center 83/81/82, shift 57 -> note2pitch2 = 0x5f3).
#   B  ROOM10D0 Code richtig (KARTE_AUF): Bank-5-Bits 13..16 per RE15_SET_FLAG_AT -> sub19 Message_on 5.
#      Erwartet: Piep (Satz 1) und 15 Bilder spaeter Aufschliessen (Satz 2).
#   C  Tuersequenzen ueber den Pruefhaken RE15_TUER_SEITE: S040 (10C0 -> Fahrstuhl) laeuft DOOR27 V0 als RE2-Datei,
#      S164 (2090 -> 20A0) DOOR26 V0, S041 (10C0 -> 10D0) Port-Archiv P0CD; Bogen-Bilder entstehen.
# Ton ohne Geraet: RE15_AUDIO_CAP_SYNC (Mischkette je Spielbild, audio_pc.c).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r36_tueren.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r36_tueren: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r36_tueren: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake)
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r36g_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r36g_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(g_lauf _name _out_auf _out_dbg)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        RE15_AUDIO_CAP_SYNC=cap.raw RE15_AUFSCHLIESS_LOG=auf.log RE15_SE_DEBUG=1
        ${ARGN}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r36_tueren[${_name}]: debug.log fehlt (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    set(_auf "")
    if(EXISTS "${WORKDIR}/auf.log")
        file(READ "${WORKDIR}/auf.log" _auf)
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r36_tueren[${_name}]: Endbild nicht erreicht (exit=${_rv}, ${WORKDIR})")
    endif()
    file(REMOVE "${WORKDIR}/cap.raw")
    set(${_out_auf} "${_auf}" PARENT_SCOPE)
    set(${_out_dbg} "${_dbg}" PARENT_SCOPE)
endfunction()

function(g_muss _name _text _muster)
    string(REGEX MATCH "${_muster}" _m "${_text}")
    if(NOT _m)
        message(FATAL_ERROR "r36_tueren[${_name}]: erwartet /${_muster}/ - fehlt")
    endif()
endfunction()

# --- A: ROOM3010 Green Keycard ------------------------------------------------------------------
g_lauf(a _auf _dbg RE15_SET_FLAG=3:59 RE15_DEBUG_JUMP=3010@120 "RE15_FIRE_AOT=6@100#3010" "RE15_EXIT_AT=200#3010")
g_muss(a "${_auf}" "F([0-9]+) raum=3010 nachricht=1 art=KARTE satz=0\\(KARTE_EIN\\) bild=\\+0")
g_muss(a "${_auf}" "raum=3010 nachricht=1 art=KARTE satz=1\\(PIEP\\) bild=\\+15")
g_muss(a "${_auf}" "raum=3010 nachricht=1 art=KARTE satz=2\\(AUF_E\\) bild=\\+30")
string(REGEX MATCHALL "F([0-9]+) raum=3010" _fs "${_auf}")
list(LENGTH _fs _nf)
if(NOT _nf EQUAL 3)
    message(FATAL_ERROR "r36_tueren[a]: ${_nf} Toene statt 3")
endif()
g_muss(a "${_dbg}" "\\[se\\] Stimme: se=0 layer=0 vag=0 note=65 fine=57 center=83 [^\n]*16397 Hz")
g_muss(a "${_dbg}" "\\[se\\] Stimme: se=1 layer=0 vag=1 note=63 fine=57 center=81 [^\n]*16397 Hz")
g_muss(a "${_dbg}" "\\[se\\] Stimme: se=2 layer=0 vag=2 note=64 fine=57 center=82 [^\n]*16397 Hz")
string(FIND "${_dbg}" "KARTSE.VBS fehlt" _fehlt)
if(NOT _fehlt LESS 0)
    message(FATAL_ERROR "r36_tueren[a]: KARTSE.VBS nicht geladen")
endif()

# --- B: ROOM10D0 Ziffernfeld richtig -------------------------------------------------------------
g_lauf(b _auf _dbg RE15_DEBUG_JUMP=10D0@120 "RE15_SET_FLAG_AT=5:13,5:14,5:15,5:16@200" "RE15_EXIT_AT=300#10D0")
g_muss(b "${_auf}" "raum=10D0 nachricht=5 art=KARTE_AUF satz=1\\(PIEP\\) bild=\\+0")
g_muss(b "${_auf}" "raum=10D0 nachricht=5 art=KARTE_AUF satz=2\\(AUF_E\\) bild=\\+15")

# --- C: Tuersequenzen -----------------------------------------------------------------------------
set(WORKDIR "${_basis}_c")
file(MAKE_DIRECTORY "${WORKDIR}/bogen")
re15_start_spiel(_rv 300
    RE15_WINDOW_SCALE=1 RE15_NOAUDIO=1 RE15_TUER_SCHNELL=1 RE15_TUER_SEITE=S040,S164,S041
    "RE15_TUER_BOGEN=${WORKDIR}/bogen"
    "${RE15_PC_EXE}")
file(READ "${WORKDIR}/debug.log" _dbg)
g_muss(c "${_dbg}" "\\[tuer-seite\\] S040 ROOM10C0 DOOR27 V0 Spender FF\n")
g_muss(c "${_dbg}" "\\[tuer-seite\\] S164 ROOM2090 DOOR26 V0 Spender FF\n")
g_muss(c "${_dbg}" "\\[tuer-seite\\] S041 ROOM10C0 DOOR0C V1 Spender FF Port-Archiv P0CD")
g_muss(c "${_dbg}" "\\[tuer\\] Sequenz Archiv 2 DOOR27 Variante 0 Bit7 0 Tuer 39 Seite S040")
g_muss(c "${_dbg}" "\\[tuer\\] Sequenz Archiv 2 DOOR26 Variante 0 Bit7 0 Tuer 38 Seite S164")
foreach(_s S040 S164 S041)
    if(NOT EXISTS "${WORKDIR}/bogen/${_s}_mitte.ppm")
        message(FATAL_ERROR "r36_tueren[c]: Bogen-Bild ${_s}_mitte.ppm fehlt")
    endif()
endforeach()
file(REMOVE "${_exe_kopie}")
message(STATUS "r36_tueren: A (ROOM3010 Karte), B (ROOM10D0 Code), C (Sequenzen S040/S164/S041) bestanden")
