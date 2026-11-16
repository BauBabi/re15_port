# =============================================================================
# RUNDE 41 OPTIONEN MIT DER ECHTEN EXE: "AI MIXED" raus, "ULTIMATE 1.5" rein (include/re15_ultimate15.h).
#
# TEIL=menue
#   (1) AI-Schirm (RE15_CONFIG_SHOT, Tab 8) mit RE15_CONFIG_AI=2 (der alte MIXED-Index):
#       - debug.log "[optionen] KI-Stufen 2 (RE1.5 RE2) gewaehlt 1" -> der Index 2 landet bei RE2;
#       - das Rechteck der frueheren dritten Zeile (x 116..203, y 89..106) ist pixelgleich mit dem TOP-Schirm
#         (Tab 0) -> dort ist keine Kachel mehr. Auf master steht dort die MIXED-Kachel (rot).
#   (2) ULTIMATE-Zeile aus re15_options.cfg: Tab 9 (TOP, Cursor auf ULTIMATE 1.5) mit "ultimate=on" ->
#       "[optionen] ... ULTIMATE 1.5 ON", ohne die Zeile -> "OFF"; die Zeile (x 120..255, y 225..238) unterscheidet
#       sich zwischen Tab 9 (Kasten blau) und Tab 0.
#   (3) Echte Tastenkette im Options-Schirm (pc_run_config): Start TOP Cursor 4 (RE15_CONFIG_TAB=9), Kreis/Quadrat
#       schaltet AN und schreibt re15_options.cfg sofort ("ultimate=on"), LEFT LEFT auf EXIT, Kreis/Quadrat beendet.
# TEIL=bilder
#   (4) Inventar mit der Startausruestung (Messer 0x01, Browning 0x03 — beide mit Disc-Icon): Raster-Abzug
#       RE15_INV_SHOT + RE15_INV_GRID_SHOT einmal mit RE15_ULTIMATE15=0, einmal =1 -> Bilder verschieden, das AN-Log
#       meldet die eingesetzten Disc-Teile.
#   (5) Welt: Sprung nach ROOM1020 (RE15_DEBUG_JUMP), bei AN "[ultimate15] Raum 1020 obj 4/5: Item 0x15 ->
#       Disc-Weltmodell", bei AUS keine solche Zeile.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r41_optionen> -DWORKDIR=<dir> -DTEIL=menue|bilder
#               -P test_r41_optionen.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "optionen_r41: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
    message(FATAL_ERROR "optionen_r41: RE15_TOOL fehlt: '${RE15_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "optionen_r41: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
set(_exe "${_exe_dir}/re15_pc_r41opt_${TEIL}_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")
set(_fehler "")

function(log_lesen _dir _out)
    if(EXISTS "${_dir}/debug.log")
        file(READ "${_dir}/debug.log" _t)
    else()
        set(_t "")
    endif()
    set(${_out} "${_t}" PARENT_SCOPE)
endfunction()

function(bmp_pruefen _art _a _b _x0 _y0 _x1 _y1 _out)
    execute_process(COMMAND "${RE15_TOOL}" "bmp_${_art}" "${_a}" "${_b}" ${_x0} ${_y0} ${_x1} ${_y1}
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _o)
    string(STRIP "${_o}" _o)
    message(STATUS "optionen_r41[bmp_${_art}]: ${_o} (rv=${_rv})")
    set(${_out} "${_rv}" PARENT_SCOPE)
endfunction()

if(TEIL STREQUAL "menue")
    # ---- (1) AI-Schirm ohne dritte Zeile ------------------------------------------------------------------------
    set(WORKDIR "${_basis}_shots")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(WRITE "${WORKDIR}/opt.cfg" "volume=50\nsound=stereo\n")
    re15_start_spiel(_rv 120 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 "RE15_OPTIONS_FILE=${WORKDIR}/opt.cfg"
        RE15_CONFIG_TEST=1 RE15_CONFIG_SHOT=top.bmp RE15_CONFIG_TAB=0 "${_exe}")
    log_lesen("${WORKDIR}" _log_top)
    file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug_top.log")
    re15_start_spiel(_rv 120 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 "RE15_OPTIONS_FILE=${WORKDIR}/opt.cfg"
        RE15_CONFIG_TEST=1 RE15_CONFIG_SHOT=ai.bmp RE15_CONFIG_TAB=8 RE15_CONFIG_AI=2 "${_exe}")
    log_lesen("${WORKDIR}" _log_ai)
    file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug_ai.log")
    string(FIND "${_log_ai}" "[optionen] KI-Stufen 2 (RE1.5 RE2) gewaehlt 1," _p)
    if(_p LESS 0)
        set(_fehler "${_fehler}\n[1] AI-Schirm: keine Zeile 'KI-Stufen 2 ... gewaehlt 1' (MIXED-Index 2 muss RE2 werden)")
    endif()
    if(NOT EXISTS "${WORKDIR}/top.bmp" OR NOT EXISTS "${WORKDIR}/ai.bmp")
        set(_fehler "${_fehler}\n[1] Abzug fehlt (top.bmp/ai.bmp)")
    else()
        bmp_pruefen(gleich "${WORKDIR}/top.bmp" "${WORKDIR}/ai.bmp" 116 89 203 106 _g)
        if(NOT _g EQUAL 0)
            set(_fehler "${_fehler}\n[1] AI-Schirm zeigt im Feld der frueheren MIXED-Zeile (116..203, 89..106) noch etwas")
        endif()
        bmp_pruefen(anders "${WORKDIR}/top.bmp" "${WORKDIR}/ai.bmp" 116 51 203 87 _a)
        if(NOT _a EQUAL 0)
            set(_fehler "${_fehler}\n[1] AI-Schirm ohne die zwei Kacheln RE1.5/RE2 (116..203, 51..87 gleich dem TOP-Schirm)")
        endif()
    endif()

    # ---- (2) ULTIMATE-Zeile aus der Options-Datei ------------------------------------------------------------------
    file(WRITE "${WORKDIR}/opt_an.cfg" "volume=50\nsound=stereo\nultimate=on\n")
    re15_start_spiel(_rv 120 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 "RE15_OPTIONS_FILE=${WORKDIR}/opt_an.cfg"
        RE15_CONFIG_TEST=1 RE15_CONFIG_SHOT=ult_an.bmp RE15_CONFIG_TAB=9 "${_exe}")
    log_lesen("${WORKDIR}" _log_an)
    file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug_an.log")
    string(FIND "${_log_an}" "ULTIMATE 1.5 ON, Schirm 9 Cursor 4" _p_an)
    string(FIND "${_log_top}" "ULTIMATE 1.5 OFF, Schirm 0 Cursor 0" _p_aus)
    message(STATUS "optionen_r41[2]: ON-Zeile ${_p_an}, OFF-Zeile ${_p_aus}")
    if(_p_an LESS 0 OR _p_aus LESS 0)
        set(_fehler "${_fehler}\n[2] ULTIMATE aus re15_options.cfg nicht gelesen (ON-Zeile ${_p_an}, OFF-Zeile ${_p_aus})")
    endif()
    if(EXISTS "${WORKDIR}/ult_an.bmp" AND EXISTS "${WORKDIR}/top.bmp")
        bmp_pruefen(anders "${WORKDIR}/top.bmp" "${WORKDIR}/ult_an.bmp" 120 225 255 238 _a)
        if(NOT _a EQUAL 0)
            set(_fehler "${_fehler}\n[2] ULTIMATE-Zeile (120..255, 225..238) zwischen OFF/Tab 0 und ON/Tab 9 gleich")
        endif()
    else()
        set(_fehler "${_fehler}\n[2] Abzug ult_an.bmp fehlt")
    endif()

    # ---- (3) Tastenkette: Kreis/Quadrat auf ULTIMATE schaltet AN und speichert -----------------------------------
    set(WORKDIR "${_basis}_tasten")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(WRITE "${WORKDIR}/opt.cfg" "volume=50\nsound=stereo\n")
    re15_start_spiel(_rv 180 RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1
        "RE15_OPTIONS_FILE=${WORKDIR}/opt.cfg" RE15_CONFIG_TEST=1 RE15_CONFIG_TAB=9
        "RE15_INPUT_SCRIPT=W5,A0.1,W0.5,L0.1,W0.5,L0.1,W0.5,A0.1,W3" RE15_INPUT_SCRIPT_START=0
        "${_exe}")
    log_lesen("${WORKDIR}" _log_t)
    file(READ "${WORKDIR}/opt.cfg" _cfg)
    string(FIND "${_log_t}" "[optionen] ULTIMATE 1.5 AN" _p)
    string(FIND "${_cfg}" "ultimate=on" _q)
    message(STATUS "optionen_r41[3]: exit=${_rv}, Log ${_p}, Datei '${_cfg}'")
    if(_p LESS 0 OR _q LESS 0 OR NOT _rv EQUAL 0)
        set(_fehler "${_fehler}\n[3] Kreis auf ULTIMATE 1.5 schrieb nicht ultimate=on (exit=${_rv}, Datei '${_cfg}')")
    endif()
elseif(TEIL STREQUAL "bilder")
    # ---- (4) Inventar AUS / AN ---------------------------------------------------------------------------------------
    foreach(_u 0 1)
        set(WORKDIR "${_basis}_inv${_u}")
        file(MAKE_DIRECTORY "${WORKDIR}")
        re15_start_spiel(_rv 240 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_SOFTWARE_RENDER=1
            RE15_ULTIMATE15=${_u} RE15_TITLE_SHOT=title.bmp RE15_INV_SHOT=inv.bmp RE15_INV_GRID_SHOT=1 "${_exe}")
        log_lesen("${WORKDIR}" _log_inv${_u})
        message(STATUS "optionen_r41[4]: RE15_ULTIMATE15=${_u} exit=${_rv}")
    endforeach()
    string(FIND "${_log_inv1}" "[ultimate15] ITEMALL-Icons: 61 Disc-Teile eingesetzt" _p)
    if(_p LESS 0)
        set(_fehler "${_fehler}\n[4] AN-Lauf ohne '[ultimate15] ITEMALL-Icons: 61 Disc-Teile eingesetzt'")
    endif()
    if(EXISTS "${_basis}_inv0/inv.bmp" AND EXISTS "${_basis}_inv1/inv.bmp")
        bmp_pruefen(anders "${_basis}_inv0/inv.bmp" "${_basis}_inv1/inv.bmp" 0 0 319 239 _a)
        if(NOT _a EQUAL 0)
            set(_fehler "${_fehler}\n[4] Inventar-Raster AUS und AN pixelgleich (Messer/Browning muessten Disc-Icons zeigen)")
        endif()
    else()
        set(_fehler "${_fehler}\n[4] Inventar-Abzug fehlt")
    endif()
    # ---- (5) Welt ROOM1020 -------------------------------------------------------------------------------------------
    foreach(_u 0 1)
        set(WORKDIR "${_basis}_welt${_u}")
        file(MAKE_DIRECTORY "${WORKDIR}")
        re15_start_spiel(_rv 240 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_SOFTWARE_RENDER=1
            RE15_ULTIMATE15=${_u} RE15_TITLE_SHOT=title.bmp RE15_DEBUG_JUMP=1020@240 "RE15_EXIT_AT=300#1020" "${_exe}")
        log_lesen("${WORKDIR}" _log_w${_u})
        message(STATUS "optionen_r41[5]: RE15_ULTIMATE15=${_u} exit=${_rv}")
    endforeach()
    string(FIND "${_log_w1}" "[ultimate15] Raum 1020 obj 4: Item 0x15 -> Disc-Weltmodell" _p4)
    string(FIND "${_log_w1}" "[ultimate15] Raum 1020 obj 5: Item 0x15 -> Disc-Weltmodell" _p5)
    string(FIND "${_log_w0}" "-> Disc-Weltmodell" _p0)
    string(FIND "${_log_w0}" "[room] PC loaded room1020.rdt" _r0)
    message(STATUS "optionen_r41[5]: AN obj4 ${_p4} obj5 ${_p5}, AUS Tausch ${_p0}, AUS Raum ${_r0}")
    if(_p4 LESS 0 OR _p5 LESS 0)
        set(_fehler "${_fehler}\n[5] AN: ROOM1020 obj 4/5 ohne Disc-Weltmodell (${_p4}/${_p5})")
    endif()
    if(NOT _p0 LESS 0 OR _r0 LESS 0)
        set(_fehler "${_fehler}\n[5] AUS: Tausch-Zeile vorhanden oder Raum nicht geladen (${_p0}/${_r0})")
    endif()
else()
    message(FATAL_ERROR "optionen_r41: TEIL unbekannt: '${TEIL}'")
endif()

file(REMOVE "${_exe}")
if(NOT _fehler STREQUAL "")
    message(FATAL_ERROR "optionen_r41 (${TEIL}) FEHLER:${_fehler}")
endif()
message(STATUS "optionen_r41 (${TEIL}): OK")
