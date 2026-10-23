# =============================================================================
# FLUT ROOM20B0/20B1 MIT DER ECHTEN EXE (Runde 41, Spur A "wasser20b0").
#
# Dossier analysis/befunde_runde41/A_wasser20b0.md, Belege include/re15_wasser20b0_r41.h.
# Debug-Sprung nach 20B0 (Leon) bzw. 20B1 (Elza, RE15_PSELECT_AUTO) bei Flag (3,0xEB)=0: sub00 spawnt die
# 35 EM024 und startet sub02 sofort (Original-Messung ts_10..flut_a.sav: Szene laeuft nach dem JUMP).
# Geprueft wird, was nur die exe zeigt:
#   - RE15_FX_LOG: Partikel des Effekts 0x0f (id=15) werden GEZEICHNET (Zeilen "id=15 sub=0|7 ..."),
#   - debug.log: sub05 feuert (AUTO-Satz 7 unter der Pad-Sperre, FUN_800436a8 @0x8001ce1c): "Cut_chg(7)",
#     Aot_reset, und die Szene endet (Spieler wieder mo=1 nach dem Sleep 60 / Plc_ret),
#   - die 35 EM024 werden nicht gezeichnet (kein "ZEICHNE Typ 0x24" in enemy_dbg.log, Zeichenmaske @0x8010efe4).
# Bilder: RE15_FRAMEDUMP (RE15_WINDOW_SCALE=1) fuer das Dossier.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_wasser20b0.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_wasser20b0: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_wasser20b0: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41a_flut_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41a_flut_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(flut_lauf _name _raum _extra)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/debug.log" "${WORKDIR}/fx.log" "${WORKDIR}/enemy_dbg.log")
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        "RE15_DEBUG_JUMP=${_raum}@240"
        RE15_FX_LOG=fx.log RE15_ENEMY_DBG=1
        "RE15_FRAMEDUMP=260-900/40:f"
        "RE15_EXIT_AT=900#${_raum}"
        ${_extra}
        "${RE15_PC_EXE}")
    message(STATUS "r41_wasser20b0[${_name}]: exit=${_rv}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r41_wasser20b0[${_name}]: kein debug.log (exit=${_rv})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(REGEX MATCHALL "spawn-diag\\] Sce_em_set type=0x24" _spawns "${_dbg}")
    list(LENGTH _spawns _n_spawn)
    if(NOT EXISTS "${WORKDIR}/fx.log")
        message(FATAL_ERROR "r41_wasser20b0[${_name}]: kein fx.log")
    endif()
    file(STRINGS "${WORKDIR}/fx.log" _fx REGEX "^id=15 sub=[07] ")
    list(LENGTH _fx _n_fx)
    string(REGEX MATCHALL "Cut_chg\\(7\\)" _cut7 "${_dbg}")
    list(LENGTH _cut7 _n_cut7)
    string(REGEX MATCHALL "Cut_chg\\(6\\)" _cut6 "${_dbg}")
    list(LENGTH _cut6 _n_cut6)
    # "frei" = ein [walk]-Bild mit mo=1 NACH dem Cut_chg(6) von sub05 (Plc_ret, Pad-Sperre (2,7)=0)
    set(_n_frei 0)
    string(FIND "${_dbg}" "Cut_chg(6)" _pos6)
    if(NOT _pos6 EQUAL -1)
        string(SUBSTRING "${_dbg}" ${_pos6} -1 _nach6)
        string(REGEX MATCHALL "\\[walk\\] F[0-9]+ cut=[0-9]+ mo=1 " _frei "${_nach6}")
        list(LENGTH _frei _n_frei)
    endif()
    set(_zeichnet 0)
    if(EXISTS "${WORKDIR}/enemy_dbg.log")
        file(READ "${WORKDIR}/enemy_dbg.log" _edbg)
        string(FIND "${_edbg}" "ZEICHNE Typ 0x24" _pos)
        if(NOT _pos EQUAL -1)
            set(_zeichnet 1)
        endif()
    endif()
    message(STATUS "r41_wasser20b0[${_name}]: EM024-Spawns=${_n_spawn} fx-Zeilen id=15: ${_n_fx} Cut7=${_n_cut7} Cut6=${_n_cut6} frei(mo=1)=${_n_frei} zeichnet0x24=${_zeichnet}")
    if(NOT _n_fx GREATER 20)
        message(FATAL_ERROR "r41_wasser20b0[${_name}]: Partikel 0x0f werden nicht gezeichnet (fx.log id=15: ${_n_fx} Zeilen) - der Wasserschwall fehlt")
    endif()
    if(NOT _n_cut7 GREATER 0 OR NOT _n_cut6 GREATER 0)
        message(FATAL_ERROR "r41_wasser20b0[${_name}]: sub05 (Aot 7, Cut 7 -> ... -> Cut 6) lief nicht: Cut7=${_n_cut7} Cut6=${_n_cut6} - die Szene haengt")
    endif()
    if(NOT _n_frei GREATER 0)
        message(FATAL_ERROR "r41_wasser20b0[${_name}]: Spieler wird nach der Szene nicht frei (kein [walk] mo=1 nach Cut_chg(6))")
    endif()
    if(_zeichnet)
        message(FATAL_ERROR "r41_wasser20b0[${_name}]: EM024-Mesh wird gezeichnet (enemy_dbg 'ZEICHNE Typ 0x24') - Zeichenmaske @0x8010efe4 fehlt")
    endif()
    if(NOT _n_spawn EQUAL 35)
        message(FATAL_ERROR "r41_wasser20b0[${_name}]: ${_n_spawn} EM024-Spawns statt 35")
    endif()
endfunction()

flut_lauf(leon 20b0 "RE15_SPAWN_DIAG=1")
flut_lauf(elza 20b1 "RE15_SPAWN_DIAG=1;RE15_PSELECT_AUTO=1;RE15_PSELECT_AUTO_SWITCH=1")

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_wasser20b0: OK (Leon 20B0 + Elza 20B1)")
