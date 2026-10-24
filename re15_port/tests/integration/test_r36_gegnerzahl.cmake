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
# Gemessen wird die LETZTE state.log-Zeile im Zielraum: welche Aktoren des Typs aktiv sind.
#
# Laeufe:
#   a  ROOM1030 -> Tuer Slot 2 @0x01CAA -> ROOM1040; 1030 leer (7:0-19), 1040 Records 0..14 tot (7:20-34):
#      soll Aktoren {16..20} = Records 15..19 (vorher: keiner)
#   b  ROOM1040 -> Tuer Slot 1 @0x010B8 -> ROOM1030; 1040 leer (7:20-39), 1030 Records 0..14 tot (7:0-14):
#      soll Aktoren {16..20}
#   c  Kontrolle wie a, in 1040 leben alle: soll {1..5} (Limit 5 @0x011F0)
#   d  wie b, in 1030 Records 0..16 tot: soll {18,19,20} (die drei uebrigen; Limit 6 @0x01DE2 nicht erreicht)
#   e  ROOM20B0 ueber den Debug-Sprung (RE15_DEBUG_JUMP, gemessen wird die SCHATTENLISTE, keine Story): 20 FX-Emitter
#      (Typ 0x24, Slots 0..19 von 35 @0x1aae..) = Aktor 1..20, und JEDER Aktor hat seinen Schatten — die
#      Schattenliste (render_pc.c SHADOW_QUAD_MAX) war fest 16 = alte RE15_ACTOR_MAX und verwarf ab dem 17.
#      (Messhaken RE15_SCHATTEN_LOG: gemessen mit 16 "gezeichnet 16, verworfen 5", mit 21 "gezeichnet 21, verworfen 0").
# Standplaetze = die Tuer-Ziele der Gegenrichtung: 1040 Slot 1 @0x010B8 -> ROOM1030 (-26214,-3861) Blick 0;
# 1030 Slot 2 @0x01CAA -> ROOM1040 (-20083,-3559) Blick 2048. Vor die Tuer gedreht: 1030 Blick 2048 (-x,
# Rechteck x -27500..-26300), 1040 Blick 0 (+x, Rechteck x -20400..-18900). ROOM20B0: re15_room_spawns.h
# (-12635,-2905).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_gegnerzahl_karte> -DWORKDIR=<dir>
#               -P test_r36_gegnerzahl.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.16)   # Policies NEW (CMP0012: while(TRUE)/if(TRUE)) auch unter dem Container-CMake 3.28 im Skriptmodus; ohne sie lief die Schleife im Linux-Release-Bau v0.8.24 nie (exit=)
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

# Ein Lauf: Karte schreiben, exe starten (CONTINUE, Eingabeskript ab Bild 60), aktive Aktoren des Typs <_typ> (hex,
# wie im state.log "t=16") der letzten state.log-Zeile als Liste der Slots zurueckgeben; debug.log in <_out>_DBG.
# Raum-Kennungen klein geschrieben (die exe protokolliert %04x).
function(gz_lauf _name _raum _x _z _blick _bits _skript _ziel _ende _typ _env _out)
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
            RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
            RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
            RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
            "RE15_INPUT_SCRIPT=${_skript}"
            RE15_STATE_LOG=state.log RE15_SPAWN_DIAG=1
            "RE15_EXIT_AT=${_ende}#${_ziel}"
            ${_env}
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
    if(NOT "${_raum}" STREQUAL "${_ziel}")   # Tuerweg: der Zielraum wird per Tuer geladen
        string(FIND "${_dbg}" "[room] PC loaded room${_ziel}.rdt" _p)
        if(_p LESS 0)
            message(FATAL_ERROR "r36_gegnerzahl[${_name}]: ROOM${_ziel} nie geladen - Tuer nicht genommen? (exit=${_rv}, ${WORKDIR})")
        endif()
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild ${_ende} in Raum ${_ziel}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r36_gegnerzahl[${_name}]: Endbild in ROOM${_ziel} nicht erreicht (exit=${_rv}, ${WORKDIR})")
    endif()
    # Eckige Klammern VOR dem Zerlegen ersetzen: eine "[" haelt in CMake-Listen das ";" zusammen (gemessen:
    # die Trefferliste kam als EIN Element "[16 t=16 ;[17 t=16 ;..." zurueck).
    file(READ "${WORKDIR}/state.log" _st)
    string(REPLACE "[" "<" _st "${_st}")
    string(REPLACE "]" ">" _st "${_st}")
    string(REGEX MATCHALL "F[0-9]+ [^\n]*" _zl "${_st}")
    list(GET _zl -1 _letzte)
    string(REGEX MATCHALL "<[0-9]+ t=${_typ} " _treffer "${_letzte}")
    set(_slots "")
    foreach(_t IN LISTS _treffer)
        string(REGEX REPLACE "^<([0-9]+) t=${_typ} $" "\\1" _s "${_t}")
        list(APPEND _slots "${_s}")
    endforeach()
    set(${_out} "${_slots}" PARENT_SCOPE)
    string(REPLACE "[" "<" _dbg "${_dbg}")
    string(REPLACE "]" ">" _dbg "${_dbg}")
    set(${_out}_DBG "${_dbg}" PARENT_SCOPE)
endfunction()

# Sprunglauf (Debug-Menue wie ein Quadrat-Druck, main.c RE15_DEBUG_JUMP): fuer Render-Eigenschaften. Gemessen am
# CONTINUE-Weg mit einer Karte in ROOM20B0: die Kamera blieb in Cut 0 und es wurde KEIN Schatten gezeichnet (300 Bilder,
# "[schatten] gezeichnet 0") - die Emitter stehen dort nicht im Bild; der Sprung zeigt sie (Cut 12..14).
function(gz_sprung _name _raum _ende _typ _env _out)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/debug.log" "${WORKDIR}/state.log")
    set(_versuch 0)
    while(TRUE)
        re15_start_spiel(_rv 300
            RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
            RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
            RE15_STATE_LOG=state.log RE15_SPAWN_DIAG=1
            "RE15_DEBUG_JUMP=${_raum}@250" "RE15_EXIT_AT=${_ende}#${_raum}"
            ${_env}
            "${RE15_PC_EXE}")
        set(_dbg "")
        if(EXISTS "${WORKDIR}/debug.log")
            file(READ "${WORKDIR}/debug.log" _dbg)
        endif()
        string(FIND "${_dbg}" "EXIT_AT: Bild ${_ende} in Raum ${_raum}" _pe)
        if(_pe LESS 0 AND _versuch EQUAL 0)
            message(STATUS "r36_gegnerzahl[${_name}]: Lauf ohne Endbild (exit=${_rv}) -> EIN Wiederholungsversuch")
            file(REMOVE "${WORKDIR}/debug.log" "${WORKDIR}/state.log")
            set(_versuch 1)
            continue()
        endif()
        break()
    endwhile()
    if(_pe LESS 0)
        message(FATAL_ERROR "r36_gegnerzahl[${_name}]: Endbild in ROOM${_raum} nicht erreicht (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/state.log" _st)
    string(REPLACE "[" "<" _st "${_st}")
    string(REPLACE "]" ">" _st "${_st}")
    string(REGEX MATCHALL "F[0-9]+ [^\n]*" _zl "${_st}")
    list(GET _zl -1 _letzte)
    string(REGEX MATCHALL "<[0-9]+ t=${_typ} " _treffer "${_letzte}")
    set(_slots "")
    foreach(_t IN LISTS _treffer)
        string(REGEX REPLACE "^<([0-9]+) t=${_typ} $" "\\1" _s "${_t}")
        list(APPEND _slots "${_s}")
    endforeach()
    set(${_out} "${_slots}" PARENT_SCOPE)
    string(REPLACE "[" "<" _dbg "${_dbg}")
    string(REPLACE "]" ">" _dbg "${_dbg}")
    set(${_out}_DBG "${_dbg}" PARENT_SCOPE)
endfunction()

function(gz_soll _name _ist _soll)
    if(NOT "${_ist}" STREQUAL "${_soll}")
        message(FATAL_ERROR "r36_gegnerzahl[${_name}]: aktive Aktoren = {${_ist}}, soll {${_soll}}")
    endif()
    message(STATUS "r36_gegnerzahl[${_name}]: ok - Aktoren {${_ist}}")
endfunction()

set(_tuer "W1,A0.2,W200")
gz_lauf(a 1030 -26214 -3861 2048 "7:0-19;7:20-34" "${_tuer}" 1040 90 16 "RE15_SOFTWARE_RENDER=1" _a)
gz_soll(a "${_a}" "16;17;18;19;20")
gz_lauf(b 1040 -20083 -3559 0 "7:20-39;7:0-14" "${_tuer}" 1030 90 16 "RE15_SOFTWARE_RENDER=1" _b)
gz_soll(b "${_b}" "16;17;18;19;20")
gz_lauf(c 1030 -26214 -3861 2048 "7:0-19" "${_tuer}" 1040 90 16 "RE15_SOFTWARE_RENDER=1" _c)
gz_soll(c "${_c}" "1;2;3;4;5")
gz_lauf(d 1040 -20083 -3559 0 "7:20-39;7:0-16" "${_tuer}" 1030 90 16 "RE15_SOFTWARE_RENDER=1" _d)
gz_soll(d "${_d}" "18;19;20")

# e: ROOM20B0 — alle 35 Emitter (Runde 41 Spur A: RE15_ACTOR_MAX 36, Original fuehrt 35, Savestate flut_a.sav) und
# die Schattenliste ohne Verwurf. Die EM024-Emitter werfen KEINEN Schatten (ihr Overlay-Code 0x8010ee9c..0x80110a00
# ruft FUN_8001b064 nie; A_wasser20b0.md 1.8), also "gezeichnet 1" (Spieler) bei Kapazitaet 36.
# Beschleunigter Renderer (ohne RE15_SOFTWARE_RENDER), Sprung @250, Ende Bild 300 im Raum.
gz_sprung(e 20b0 300 24 "RE15_SCHATTEN_LOG=1" _e)
gz_soll(e "${_e}" "1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20;21;22;23;24;25;26;27;28;29;30;31;32;33;34;35")
string(FIND "${_e_DBG}" "<schatten> gezeichnet 1, verworfen 0 (Kapazitaet 36)" _p)
if(_p LESS 0)
    message(FATAL_ERROR "r36_gegnerzahl[e]: kein Bild mit genau 1 Schatten (Spieler; Emitter schattenlos) bei Kapazitaet 36 - Schattenliste?")
endif()
string(REGEX MATCH "<schatten> gezeichnet [0-9]+, verworfen [1-9][0-9]*" _verw "${_e_DBG}")
if(_verw)
    message(FATAL_ERROR "r36_gegnerzahl[e]: Schatten verworfen: '${_verw}'")
endif()
message(STATUS "r36_gegnerzahl[e]: ok - 35 Emitter, 1 Schatten (Spieler), keiner verworfen")
message(STATUS "r36_gegnerzahl: OK - 1040 Records 15..19 = Aktor 16..20 (a), 1030 dito (b), Kontrolle 1..5 (c), 1030 17 tot -> 18..20 (d), 20B0 35 Emitter / 1 Schatten (e)")
