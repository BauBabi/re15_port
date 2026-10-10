# =============================================================================
# ZWANZIG GEGNER-PLAETZE JE RAUM MIT DER ECHTEN EXE (Runde 36, Spur E).
#
# Nutzer (analysis/befunde_runde36/AUFTRAG.md Z.19-20): "Der Port verwaltet hoechstens 15 Gegner je Raum,
# das Original 20. In ROOM1040 und 1030 fehlen deshalb die letzten fuenf Zombies [...] --> Das auch!"
# Dossier analysis/befunde_runde36/E_gegnerzahl.md; Konstante include/re15_actor.h RE15_ACTOR_MAX = 21
# (Gegner-Feld 0x800acc2c, 20 Plaetze: FUN_80037250 `sltiu v0,v0,0x14` @0x800372b4; Sce_em_set ohne
# Grenzpruefung @0x800420f8/@0x80042118).
#
# Der ECHTE Weg: Spielstand (Karte aus probe_r36_gegnerzahl_karte) + CONTINUE vor einer Original-Tuer,
# Aktionstaste -> Tuer -> der gemessene Raum baut sich mit den Tot-Bits aus dem GELADENEN Spielstand auf.
# Gemessen wird die LETZTE state.log-Zeile im Zielraum: welche Aktoren vom Typ 0x16 aktiv sind.
#
# Laeufe:
#   a  ROOM1030 -> Tuer Slot 2 @0x01CAA -> ROOM1040; 1030 leer (7:0-19), 1040 Records 0..14 tot (7:20-34):
#      soll Aktoren {16..20} = Records 15..19 (vorher: keiner)
#   b  ROOM1040 -> Tuer Slot 1 @0x010B8 -> ROOM1030; 1040 leer (7:20-39), 1030 Records 0..14 tot (7:0-14):
#      soll Aktoren {16..20}
#   c  Kontrolle wie a, in 1040 leben alle: soll {1..5} (Limit 5 @0x011F0)
#   d  wie b, in 1030 Records 0..16 tot: soll {18,19,20} (die drei uebrigen; Limit 6 @0x01DE2 nicht erreicht)
# Standplaetze = die Tuer-Ziele der Gegenrichtung: 1040 Slot 1 @0x010B8 -> ROOM1030 (-26214,-3861) Blick 0;
# 1030 Slot 2 @0x01CAA -> ROOM1040 (-20083,-3559) Blick 2048. Vor die Tuer gedreht: 1030 Blick 2048 (-x,
# Rechteck x -27500..-26300), 1040 Blick 0 (+x, Rechteck x -20400..-18900).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_gegnerzahl_karte> -DWORKDIR=<dir>
#               -P test_r36_gegnerzahl.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r36_gegnerzahl: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r36_gegnerzahl: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r36_gegnerzahl: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name (Muster test_r34n_b_cursor.cmake: fremde Bauten beenden keine Messkopie).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r36e_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r36e_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

# Ein Lauf: Karte schreiben, exe starten (CONTINUE, Aktionstaste ab Bild 60), aktive Typ-0x16-Aktoren der letzten
# state.log-Zeile zurueckgeben (Liste der Slots).
function(gz_lauf _name _raum _x _z _blick _bits _ziel _out)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log" "${WORKDIR}/state.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}" "${_x}" "${_z}" "${_blick}" ${_bits}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r36_gegnerzahl[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    set(_versuch 0)
    while(TRUE)
        re15_start_spiel(_rv 300
            RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
            RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
            RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
            "RE15_INPUT_SCRIPT=W1,A0.2,W200"
            RE15_STATE_LOG=state.log RE15_SPAWN_DIAG=1
            "RE15_EXIT_AT=90#${_ziel}"
            "${RE15_PC_EXE}")
        set(_dbg "")
        if(EXISTS "${WORKDIR}/debug.log")
            file(READ "${WORKDIR}/debug.log" _dbg)
        endif()
        string(FIND "${_dbg}" "EXIT_AT: Bild" _pe)
        # Startabbruch ohne Ergebnis (exe nach 0,1 s mit exit 1, s. VERTRAG §1.5): EINMAL wiederholen.
        if(_pe LESS 0 AND _versuch EQUAL 0)
            message(STATUS "r36_gegnerzahl[${_name}]: Lauf ohne Endbild (exit=${_rv}) -> EIN Wiederholungsversuch")
            file(REMOVE "${WORKDIR}/debug.log" "${WORKDIR}/state.log")
            set(_versuch 1)
            continue()
        endif()
        break()
    endwhile()
    string(FIND "${_dbg}" "CONTINUE: resumed in room ${_raum}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r36_gegnerzahl[${_name}]: Spielstand in ROOM${_raum} nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    string(TOLOWER "${_ziel}" _ziel_kl)
    string(FIND "${_dbg}" "[room] PC loaded room${_ziel_kl}.rdt" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r36_gegnerzahl[${_name}]: Tuer nicht genommen - ROOM${_ziel} nie geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild 90 in Raum ${_ziel_kl}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r36_gegnerzahl[${_name}]: Endbild in ROOM${_ziel} nicht erreicht (exit=${_rv}, ${WORKDIR})")
    endif()
    file(STRINGS "${WORKDIR}/state.log" _zl REGEX "^F[0-9]+ ")
    list(GET _zl -1 _letzte)
    string(REGEX MATCHALL "\\[[0-9]+ t=16 " _treffer "${_letzte}")
    set(_slots "")
    foreach(_t IN LISTS _treffer)
        string(REGEX REPLACE "^\\[([0-9]+) t=16 $" "\\1" _s "${_t}")
        list(APPEND _slots "${_s}")
    endforeach()
    set(${_out} "${_slots}" PARENT_SCOPE)
endfunction()

function(gz_soll _name _ist _soll)
    if(NOT "${_ist}" STREQUAL "${_soll}")
        message(FATAL_ERROR "r36_gegnerzahl[${_name}]: aktive Zombies (Typ 0x16) = {${_ist}}, soll {${_soll}}")
    endif()
    message(STATUS "r36_gegnerzahl[${_name}]: ok - Aktoren {${_ist}}")
endfunction()

gz_lauf(a 1030 -26214 -3861 2048 "7:0-19;7:20-34" 1040 _a)
gz_soll(a "${_a}" "16;17;18;19;20")
gz_lauf(b 1040 -20083 -3559 0 "7:20-39;7:0-14" 1030 _b)
gz_soll(b "${_b}" "16;17;18;19;20")
gz_lauf(c 1030 -26214 -3861 2048 "7:0-19" 1040 _c)
gz_soll(c "${_c}" "1;2;3;4;5")
gz_lauf(d 1040 -20083 -3559 0 "7:20-39;7:0-16" 1030 _d)
gz_soll(d "${_d}" "18;19;20")
message(STATUS "r36_gegnerzahl: OK - 1040 Records 15..19 = Aktor 16..20 (a), 1030 dito (b), Kontrolle 1..5 (c), 1030 17 tot -> 18..20 (d)")
