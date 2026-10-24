# =============================================================================
# LAUTSTAERKE-OPTION MIT DER ECHTEN EXE (Runde 41, Spur D "kleinkram_a").
#
# Dossier analysis/befunde_runde41/D_kleinkram_a.md §1, Belege include/re15_lautstaerke_r41.h.
# Zwei deterministische Ton-Mitschnitte (RE15_AUDIO_CAP_SYNC, 1470 Stereo-Frames je Spielbild, dieselbe
# Mischkette wie der SDL-Rueckruf) desselben Laufs — Leon ROOM2030 mit Schuss (Waffe 8) und Giftkugel-Treffer
# der RE2-Spinne — einmal mit re15_options.cfg `volume=100`, einmal `volume=50` (ueber RE15_OPTIONS_FILE):
#   RMS(50) / RMS(100) = 0,5 (Endstufe Q15 0x4000, audio_pc.c audio_callback) bei gleicher Sample-Zahl.
# Dazu der Options-Schirm (RE15_CONFIG_SHOT, Tab 4 = SOUND) mit `volume=50`: die VOLUME-Kachel (y 221, freier Streifen) ist
# pixelgleich mit der ersten (y 51) in den textfreien Randspalten (dieselbe CONFIG.TIM-Kachel uv 0,232) und
# rechts in der Kachel stehen Ziffern (helle Pixel im Zahlenfeld x 178..201), links "VOLUME".
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r41_kleinkram_a> -DWORKDIR=<dir>
#               -P test_r41_kleinkram_a_lautstaerke.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_kleinkram_a_lautstaerke: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
    message(FATAL_ERROR "r41_kleinkram_a_lautstaerke: RE15_TOOL fehlt: '${RE15_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_kleinkram_a_lautstaerke: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte "${_exe_dir}/re15_pc_r41d_laut_*${_exe_ext}")
foreach(_k IN LISTS _alte)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r41d_laut_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

set(_fehler "")

# Ein Mitschnitt: Optionsdatei schreiben, 330 Spielbilder aufnehmen (CAP_FRAMES beendet die exe).
function(laut_mitschnitt _name _prozent)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(WRITE "${WORKDIR}/opt.cfg" "volume=${_prozent}\nsound=stereo\n")
    re15_start_spiel(_rv 400
        SDL_ASSERT=always_ignore RE15_WINDOW_SCALE=1 RE15_NO_INTRO=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        "RE15_OPTIONS_FILE=${WORKDIR}/opt.cfg"
        RE15_AUDIO_CAP_SYNC=cap.raw RE15_AUDIO_CAP_FRAMES=330
        RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=6550,2000,3072,3
        RE15_GIVE=8:30 RE15_EQUIP=8
        "RE15_INPUT_SCRIPT=M1,MA0.2,W5" RE15_INPUT_SCRIPT_START=20 RE15_INPUT_SCRIPT_BASIS=spiel
        "RE15_EXIT_AT=400#2030"
        "${_exe}")
    if(NOT EXISTS "${WORKDIR}/cap.raw")
        set(_fehler "${_fehler}\n[${_name}] kein cap.raw (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    # (Die Zeile "[lautstaerke] ... gelesen" faellt VOR der stderr-Umleitung nach debug.log und steht nur auf der
    #  Konsole; dass die Datei wirkt, beweist das RMS-Verhaeltnis unten.)
    string(FIND "${_dbg}" "CAP_SYNC: 330 Frames geschrieben" _p)
    if(_p LESS 0)
        set(_fehler "${_fehler}\n[${_name}] Mitschnitt nicht vollstaendig (exit=${_rv})" PARENT_SCOPE)
    endif()
endfunction()

laut_mitschnitt(v100 100)
laut_mitschnitt(v50 50)
if(EXISTS "${_basis}_v100/cap.raw" AND EXISTS "${_basis}_v50/cap.raw")
    # RMS(50)/RMS(100) in Promille: soll 500 +- 5 (Integer-Shift >> 15, gleiche Sample-Zahl).
    execute_process(COMMAND "${RE15_TOOL}" rms_vergleich "${_basis}_v100/cap.raw" "${_basis}_v50/cap.raw" 500 5
                    WORKING_DIRECTORY "${_basis}_v100" TIMEOUT 300 RESULT_VARIABLE _rp OUTPUT_VARIABLE _op)
    message(STATUS "r41_kleinkram_a_lautstaerke: ${_op}")
    if(NOT _rp EQUAL 0)
        set(_fehler "${_fehler}\n[rms] ${_op}")
    endif()
    # Mindestpegel: ein Mitschnitt ohne Ton waere trivial halbiert.
    execute_process(COMMAND "${RE15_TOOL}" rms "${_basis}_v100/cap.raw"
                    WORKING_DIRECTORY "${_basis}_v100" TIMEOUT 300 RESULT_VARIABLE _rr OUTPUT_VARIABLE _or)
    string(REGEX MATCH "rms=([0-9]+)" _m "${_or}")
    if(NOT _rr EQUAL 0 OR NOT _m OR CMAKE_MATCH_1 LESS 200)
        set(_fehler "${_fehler}\n[rms] 100-%-Mitschnitt ohne Ton: ${_or}")
    endif()
endif()

# --- Options-Schirm: VOLUME-Zeile ----------------------------------------------------------------
set(WORKDIR "${_basis}_shot")
file(MAKE_DIRECTORY "${WORKDIR}")
file(WRITE "${WORKDIR}/opt.cfg" "volume=50\nsound=stereo\n")
re15_start_spiel(_rv 120
    SDL_ASSERT=always_ignore RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
    "RE15_OPTIONS_FILE=${WORKDIR}/opt.cfg"
    RE15_CONFIG_TEST=1 RE15_CONFIG_SHOT=sound.bmp RE15_CONFIG_TAB=4
    "${_exe}")
if(NOT EXISTS "${WORKDIR}/sound.bmp")
    set(_fehler "${_fehler}\n[shot] kein sound.bmp (exit=${_rv})")
else()
    # BMP 24 bpp 320x240 (bottom-up): Pixel (x, y) liegt bei off + ((239 - y) * 960 + x * 3).
    file(READ "${WORKDIR}/sound.bmp" _bmp HEX)
    string(SUBSTRING "${_bmp}" 20 8 _off_hex)              # Byte 10..13 = Pixel-Offset (LE)
    string(REGEX REPLACE "(..)(..)(..)(..)" "\\4\\3\\2\\1" _off_be "${_off_hex}")
    math(EXPR _off "0x${_off_be}")
    function(bmp_pixel _x _y _out)
        math(EXPR _i "(${_off} + (239 - ${_y}) * 960 + ${_x} * 3) * 2")
        string(SUBSTRING "${_bmp}" ${_i} 6 _px)
        set(${_out} "${_px}" PARENT_SCOPE)
    endfunction()
    # (a) VOLUME-Kachel (y 221..238, freier Streifen unter dem Kasten) == erste Kachel (y 51..68) in den textfreien
    #     Spalten x 116/117 und 202/203, Zeilen 2..15 (die abgerundeten Ecken der Kachel sind durchsichtig und
    #     zeigen den Hintergrund, der an y 51 (Controller-Bild) und y 221 (Streifen) verschieden ist — gemessen:
    #     genau die 8 Eckpixel wichen ab).
    set(_gleich 0)
    set(_anders 0)
    foreach(_dy RANGE 2 15)
        foreach(_x 116 117 202 203)
            math(EXPR _y1 "51 + ${_dy}")
            math(EXPR _y3 "221 + ${_dy}")
            bmp_pixel(${_x} ${_y1} _p1)
            bmp_pixel(${_x} ${_y3} _p3)
            if(_p1 STREQUAL _p3)
                math(EXPR _gleich "${_gleich} + 1")
            else()
                math(EXPR _anders "${_anders} + 1")
            endif()
        endforeach()
    endforeach()
    message(STATUS "r41_kleinkram_a_lautstaerke[shot]: Kachel 3 vs Kachel 1 Randspalten gleich=${_gleich} anders=${_anders}")
    if(NOT _anders EQUAL 0)
        set(_fehler "${_fehler}\n[shot] dritte Kachel nicht pixelgleich mit der ersten (anders=${_anders})")
    endif()
    # (b) Ziffern "50" rechtsbuendig bis x 201: helle Pixel (> 0xC0 in allen Kanaelen) im Feld x 186..201, y 223..236,
    #     und KEINE hellen Pixel im Feld x 172..184 (dort stuende "100" bei 100 %).
    set(_hell 0)
    set(_hell_links 0)
    foreach(_y RANGE 223 236)
        foreach(_x RANGE 186 201)
            bmp_pixel(${_x} ${_y} _p)
            string(SUBSTRING "${_p}" 0 2 _b)
            string(SUBSTRING "${_p}" 2 2 _g)
            string(SUBSTRING "${_p}" 4 2 _r)
            if("0x${_r}" GREATER 192 AND "0x${_g}" GREATER 192 AND "0x${_b}" GREATER 192)
                math(EXPR _hell "${_hell} + 1")
            endif()
        endforeach()
        foreach(_x RANGE 172 184)
            bmp_pixel(${_x} ${_y} _p)
            string(SUBSTRING "${_p}" 0 2 _b)
            string(SUBSTRING "${_p}" 2 2 _g)
            string(SUBSTRING "${_p}" 4 2 _r)
            if("0x${_r}" GREATER 192 AND "0x${_g}" GREATER 192 AND "0x${_b}" GREATER 192)
                math(EXPR _hell_links "${_hell_links} + 1")
            endif()
        endforeach()
    endforeach()
    message(STATUS "r41_kleinkram_a_lautstaerke[shot]: helle Pixel Zahlenfeld 186..201 = ${_hell}, 172..184 = ${_hell_links}")
    if(_hell LESS 10 OR _hell_links GREATER 0)
        set(_fehler "${_fehler}\n[shot] Ziffern '50' nicht rechtsbuendig im Zahlenfeld (hell=${_hell}, links=${_hell_links})")
    endif()
endif()

# --- Echte Tastenkette im Options-Schirm (pc_run_config, Start im SOUND-Tab): LEFT LEFT = 80 %, Kreis/Quadrat
#     speichert (@0x8002f2c0-Analog -> Datei), RIGHT auf EXIT, Kreis/Quadrat verlaesst den Schirm (exit 0).
#     Eingabeskript auf der Tick-Achse des Eingabetreibers (laeuft auch im Titel, input_pc.c), Vorspann uebersprungen.
set(WORKDIR "${_basis}_tasten")
file(MAKE_DIRECTORY "${WORKDIR}")
file(WRITE "${WORKDIR}/opt.cfg" "volume=100\nsound=stereo\n")
re15_start_spiel(_rv 180
    SDL_ASSERT=always_ignore RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1
    "RE15_OPTIONS_FILE=${WORKDIR}/opt.cfg"
    RE15_CONFIG_TEST=1 RE15_CONFIG_TAB=4
    "RE15_INPUT_SCRIPT=W5,L0.1,W0.5,L0.1,W0.5,A0.1,W0.5,R0.1,W0.5,A0.1,W3" RE15_INPUT_SCRIPT_START=0
    "${_exe}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    set(_fehler "${_fehler}\n[tasten] kein debug.log (exit=${_rv})")
else()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(FIND "${_dbg}" "[lautstaerke] ${WORKDIR}/opt.cfg geschrieben: 80 %, Stereo" _p)
    file(READ "${WORKDIR}/opt.cfg" _cfg)
    string(FIND "${_cfg}" "volume=80" _q)
    message(STATUS "r41_kleinkram_a_lautstaerke[tasten]: exit=${_rv}, geschrieben-Zeile=${_p}, Datei '${_cfg}'")
    if(_p LESS 0 OR _q LESS 0 OR NOT _rv EQUAL 0)
        set(_fehler "${_fehler}\n[tasten] LEFT LEFT + Bestaetigen schrieb nicht volume=80 (exit=${_rv}, Datei '${_cfg}')")
    endif()
endif()

file(REMOVE "${_exe}")
if(NOT _fehler STREQUAL "")
    message(FATAL_ERROR "r41_kleinkram_a_lautstaerke FEHLER:${_fehler}")
endif()
message(STATUS "r41_kleinkram_a_lautstaerke: OK (RMS halbiert, Optionsdatei gelesen/geschrieben, VOLUME-Zeile im Schirm, Tastenkette)")
