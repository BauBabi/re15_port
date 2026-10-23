# =============================================================================
# NPC-SCHRITTE UND LANDUNG MIT DER ECHTEN EXE (Runde 41, Spur A "wasser20b0", Nachbesserung 1, M5).
#
# Dossier analysis/befunde_runde41/A_wasser20b0.md (Nachbesserung 1), Belege include/re15_wassersprung_r41.h.
#   (a) NPC-Schritte im Spiel: ROOM2000 (Leon) mit Flag (4,244)=1 -> sub00 spawnt Marvin (Typ 0x42, Slot 1) und Ada
#       (Typ 0x40, Slot 2), sub02 fuehrt beide per Plc_dest (Sub-VM-Walk, OWN-Bank Clip 5) ~13000 Einheiten durch den
#       Raum. Das Mess-Log RE15_NPCSCHRITT_LOG (wassersprung_r41.c) muss Schritte BEIDER Slots zeigen (Bildwort-Marken
#       Bild 7/22 bzw. 8/22 der OWN-Bank, Ton = (Boden & 0x7f) - 8 @0x8004575c, Zweig trocken = nur Ton).
#   (b) Landung ueber den ECHTEN Weg: Tuer 20A0 -> 2090 (Leon) bzw. 20A1 -> 2091 (Elza; Satz @0x1754 byte-gleich:
#       Rect (3450,4500) 3000x1000, Ziel (6300,-1800,-16250), rot 0x800, Cut 2, Band 1), Steg, Drehung auf rot 3072,
#       Kante: Kletter-Sub 9/10 -> Landung @0x80038730-d0 -> 8 x FUN_80019700(0x06033000) = fx.log "id=6 sub=3".
#       Eingabeskript = Abnahme 0 Lauf E2 (Tick 270 + Raumbild; LEFT -96/Bild).
#       Beide Laeufe starten ueber RE15_TITLE_SHOT (+_AF=2) mit fester Bildzahl im Vorspann; Elza ueber
#       RE15_TITLE_SHOT_CHAR=1 = re15_gameflow_new_game(1), derselbe Aufruf wie das Player-Select-Ende (main.c).
#       NICHT ueber RE15_TITLE_CONFIRM_MS + RE15_PSELECT_AUTO: der Titel bestaetigt dort nach WANDUHR-Zeit, das
#       Eingabeskript zaehlt aber gerenderte Bilder MIT Vorspann (input_pc.c s_input_ticks) - gemessen 2026-10-24:
#       alle 7 Skript-Tasten (Tick 270..488) fielen in den Vorspann (F0), die Tuer 20A1 feuerte nie (Zeitueberschreitung).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_wasser20b0_npc.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_wasser20b0_npc: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_wasser20b0_npc: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41a_npc_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41a_npc_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

# ---- (a) NPC-Schritte ROOM2000 (Leon): Marvin Slot 1 (0x42) + Ada Slot 2 (0x40) ---------------------------------
set(WORKDIR "${_basis}_npc2000")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rv 420
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_SET_FLAG=4:244 "RE15_DEBUG_JUMP=2000@240" "RE15_PLAYER_POS=4350,-20500,0,1"
    RE15_NPCSCHRITT_LOG=npc.log RE15_SPAWN_DIAG=1
    "RE15_EXIT_AT=1500#2000"
    "${RE15_PC_EXE}")
message(STATUS "r41_wasser20b0_npc[npc2000]: exit=${_rv}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r41_wasser20b0_npc[npc2000]: kein debug.log (exit=${_rv})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
string(FIND "${_dbg}" "PC loaded room2000.rdt" _p)
if(_p EQUAL -1)
    message(FATAL_ERROR "r41_wasser20b0_npc[npc2000]: ROOM2000 nicht geladen")
endif()
file(STRINGS "${WORKDIR}/debug.log" _sp42 REGEX "Sce_em_set type=0x42")
file(STRINGS "${WORKDIR}/debug.log" _sp40 REGEX "Sce_em_set type=0x40")
list(LENGTH _sp42 _n42)
list(LENGTH _sp40 _n40)
if(NOT EXISTS "${WORKDIR}/npc.log")
    message(FATAL_ERROR "r41_wasser20b0_npc[npc2000]: kein npc.log (RE15_NPCSCHRITT_LOG) - kein NPC-Schritt im ganzen Lauf (Spawns 0x42=${_n42} 0x40=${_n40})")
endif()
file(STRINGS "${WORKDIR}/npc.log" _s1 REGEX "^F[0-9]+ slot=1 typ=0x42 clip=5 ")
file(STRINGS "${WORKDIR}/npc.log" _s2 REGEX "^F[0-9]+ slot=2 typ=0x40 clip=5 ")
file(STRINGS "${WORKDIR}/npc.log" _ton REGEX " ton=-?[0-9]+ ")
list(LENGTH _s1 _n1)
list(LENGTH _s2 _n2)
set(_ton_ok 1)
set(_ton_erw "")
foreach(_z IN LISTS _ton)
    # trockener Boden: Ton = (Boden & 0x7f) - 8 (a0 = 0 im Sub 4 @0x80051290) muss >= 0 sein; Zweig trocken ohne Platz
    string(REGEX MATCH "ton=(-?[0-9]+)" _m "${_z}")
    if(CMAKE_MATCH_1 LESS 0)
        set(_ton_ok 0)
    endif()
endforeach()
message(STATUS "r41_wasser20b0_npc[npc2000]: Spawns 0x42=${_n42} 0x40=${_n40}; Schritte Slot 1 (0x42, Clip 5) = ${_n1}, Slot 2 (0x40, Clip 5) = ${_n2}, Toene >= 0: ${_ton_ok}")
if(NOT _n42 EQUAL 1 OR NOT _n40 EQUAL 1)
    message(FATAL_ERROR "r41_wasser20b0_npc[npc2000]: Szene (4,244) spawnt nicht Marvin+Ada (0x42=${_n42}, 0x40=${_n40})")
endif()
if(_n1 LESS 4 OR _n2 LESS 4)
    message(FATAL_ERROR "r41_wasser20b0_npc[npc2000]: zu wenige NPC-Schritte (Slot 1: ${_n1}, Slot 2: ${_n2}; erwartet >= 4 je Slot ueber ~13000 Einheiten Plc_dest)")
endif()
if(NOT _ton_ok)
    message(FATAL_ERROR "r41_wasser20b0_npc[npc2000]: negativer Tonindex im npc.log")
endif()

# ---- (b) Landung ueber die Tuer 20A0 -> 2090 (Leon) und 20A1 -> 2091 (Elza) ----------------------------------------
function(landung_lauf _name _raum_ab _raum_ziel _elza_bit _extra)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        "RE15_DEBUG_JUMP=${_raum_ab}@240" "RE15_PLAYER_POS=4950,5000,2048,2"
        "RE15_INPUT_SCRIPT=W9,A0.2,W4,R0.37,W0.3,U0.7,W0.2,A0.2,W0.3,A0.2,W0.3,A0.2,W0.3,A0.2,W4"
        RE15_INPUT_SCRIPT_START=0 RE15_CLIMB_TRACE=climb.log RE15_FX_LOG=fx.log RE15_KEIN_GATOR=1
        "RE15_EXIT_AT=360#${_raum_ziel}"
        ${_extra}
        "${RE15_PC_EXE}")
    message(STATUS "r41_wasser20b0_npc[${_name}]: exit=${_rv}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r41_wasser20b0_npc[${_name}]: kein debug.log (exit=${_rv})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(FIND "${_dbg}" "PC loaded room${_raum_ziel}.rdt" _pz)
    string(FIND "${_dbg}" "DOOR FIRE slot=0" _pd)
    if(_pz EQUAL -1 OR _pd EQUAL -1)
        message(FATAL_ERROR "r41_wasser20b0_npc[${_name}]: Tuer ${_raum_ab} -> ${_raum_ziel} nicht genommen (DOOR FIRE=${_pd}, geladen=${_pz})")
    endif()
    # Szenario: die Spielerfamilie meldet main.c beim Spielstart (PL00 Leon / PL04 Elza)
    string(FIND "${_dbg}" "Elza-Bit=${_elza_bit})" _pe)
    if(_pe EQUAL -1)
        message(FATAL_ERROR "r41_wasser20b0_npc[${_name}]: falsches Szenario (erwartet Elza-Bit=${_elza_bit} im debug.log)")
    endif()
    set(_n_land 0)
    if(EXISTS "${WORKDIR}/climb.log")
        # Landung = Kletter-Sub 10 Phase 4 (@0x800386d4, der Spritzer-Spawn), danach Phase 5 mit y = 0 im Becken
        file(STRINGS "${WORKDIR}/climb.log" _land REGEX "room=${_raum_ziel} .*sub=10 kind=0x02 ph=4")
        list(LENGTH _land _n_land)
    endif()
    set(_n_fx 0)
    if(EXISTS "${WORKDIR}/fx.log")
        file(STRINGS "${WORKDIR}/fx.log" _fx REGEX "^id=6 sub=3 ")
        list(LENGTH _fx _n_fx)
    endif()
    message(STATUS "r41_wasser20b0_npc[${_name}]: Landungen ${_n_land}, fx-Zeilen id=6 sub=3: ${_n_fx}")
    if(_n_land LESS 1)
        message(FATAL_ERROR "r41_wasser20b0_npc[${_name}]: keine Landung im climb.log (kein Takt sub=10 kind=0x02 ph=4 in Raum ${_raum_ziel}; Kante nicht erreicht?)")
    endif()
    if(_n_fx LESS 8)
        message(FATAL_ERROR "r41_wasser20b0_npc[${_name}]: Landungs-Spritzer fehlen (fx.log id=6 sub=3: ${_n_fx} Zeilen, erwartet >= 8 = 8 Plaetze 0x06033000 @0x8003876c-70)")
    endif()
endfunction()

landung_lauf(landung_leon 20a0 2090 0 "RE15_TITLE_SHOT=title.bmp;RE15_TITLE_SHOT_AF=2")
landung_lauf(landung_elza 20a1 2091 1 "RE15_TITLE_SHOT=title.bmp;RE15_TITLE_SHOT_AF=2;RE15_TITLE_SHOT_CHAR=1")

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_wasser20b0_npc: OK (NPC-Schritte 2000 + Landung Leon 2090 / Elza 2091)")
