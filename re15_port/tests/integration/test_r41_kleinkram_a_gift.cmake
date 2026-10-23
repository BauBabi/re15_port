# =============================================================================
# GIFTKUGEL BEI DANGER = TOD, MIT DER ECHTEN EXE (Runde 41, Spur D "kleinkram_a").
#
# Dossier analysis/befunde_runde41/D_kleinkram_a.md §3, Belege include/re15_giftkugel_r41.h.
# Leon ROOM2030 (Spinnen unter dem RE2-Gehirn = Voreinstellung) und Elza ROOM2031: Messschiene
# RE15_R41_HP_AT=5:15 setzt im 5. Spielbild HP 15 (Danger = HP < 20, Schwelle @0x800112b5); der erste
# Giftklumpen (gemessen Bild 19) muss den Spieler toeten:
#   - debug.log: "[giftkugel] F<n> RE2-Treffer (Spinne Slot <s>): HP 15 -> -5 = TOD (Kommando 3)",
#   - state.log: pst=3 (Todeskommando) ab dem Treffer und mindestens 100 Bilder lang (Todesfolge 0x78 @0x8003694c),
#   - gift.log (RE15_R39_LOG): nach dem negativen HP-Stand folgt ein Neuladen (hp=100 im Bild F0).
# Gegenprobe im selben Lauf: ohne den Messhaken (HP 100) nimmt der erste Klumpen 20 HP (RE2 Op 32 @0x800200dc),
# der Spieler lebt (Kommando 2).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_kleinkram_a_gift.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_kleinkram_a_gift: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_kleinkram_a_gift: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte "${_exe_dir}/re15_pc_r41d_gift_*${_exe_ext}")
foreach(_k IN LISTS _alte)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r41d_gift_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

set(_fehler "")

function(gift_lauf _name _raum _hp_at _extra)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_env "")
    if(NOT _hp_at STREQUAL "")
        set(_env "RE15_R41_HP_AT=${_hp_at}")
    endif()
    re15_start_spiel(_rv 400
        SDL_ASSERT=always_ignore RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_DEBUG_JUMP=${_raum}@250 RE15_PLAYER_POS=6550,2000,3072,3
        ${_env}
        RE15_R39_LOG=gift.log RE15_STATE_LOG=state.log
        "RE15_EXIT_AT=700#${_raum}"
        "${_exe}")
    if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/gift.log" OR NOT EXISTS "${WORKDIR}/state.log")
        set(_fehler "${_fehler}\n[${_name}] Logs fehlen (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    file(READ "${WORKDIR}/gift.log" _gift)
    file(READ "${WORKDIR}/state.log" _state)
    string(FIND "${_dbg}" "DEBUG_JUMP" _pj)
    if(_hp_at STREQUAL "")
        # Gegenprobe HP 100: erster Treffer -20, lebt.
        string(REGEX MATCH "\\[giftkugel\\] F([0-9]+) RE2-Treffer \\(Spinne Slot [0-9]+\\): HP 100 -> 80 \\(Kommando 2\\)" _m "${_dbg}")
        if(NOT _m)
            set(_fehler "${_fehler}\n[${_name}] kein RE2-Treffer 100 -> 80 (Kommando 2) im debug.log" PARENT_SCOPE)
        else()
            message(STATUS "r41_kleinkram_a_gift[${_name}]: ${_m}")
        endif()
        string(FIND "${_dbg}" "= TOD" _pt)
        if(NOT _pt LESS 0)
            set(_fehler "${_fehler}\n[${_name}] Tod bei HP 100 (unerwartet)" PARENT_SCOPE)
        endif()
        return()
    endif()
    string(FIND "${_dbg}" "[giftkugel] F5 Messhaken: HP := 15" _ph)
    if(_ph LESS 0)
        set(_fehler "${_fehler}\n[${_name}] Messhaken HP 15 nicht im debug.log" PARENT_SCOPE)
    endif()
    string(REGEX MATCH "\\[giftkugel\\] F([0-9]+) RE2-Treffer \\(Spinne Slot ([0-9]+)\\): HP 15 -> -5 = TOD \\(Kommando 3\\)" _m "${_dbg}")
    if(NOT _m)
        set(_fehler "${_fehler}\n[${_name}] kein toedlicher RE2-Treffer HP 15 -> -5 im debug.log" PARENT_SCOPE)
        return()
    endif()
    set(_bild ${CMAKE_MATCH_1})
    message(STATUS "r41_kleinkram_a_gift[${_name}]: ${_m}")
    # Todeskommando pst=3 ab dem Trefferbild, mindestens 100 Bilder (Todesfolge 0x78 = 120 Bilder).
    set(_n3 0)
    foreach(_d RANGE 0 99)
        math(EXPR _f "${_bild} + ${_d}")
        string(REGEX MATCH "\nF${_f} pad=[0-9a-fA-F]+ PL\\([^)]*hp=-5\\) pst=3 " _s "${_state}")
        if(_s)
            math(EXPR _n3 "${_n3} + 1")
        endif()
    endforeach()
    message(STATUS "r41_kleinkram_a_gift[${_name}]: pst=3 mit hp=-5 in ${_n3} von 100 Bildern ab F${_bild}")
    if(_n3 LESS 100)
        set(_fehler "${_fehler}\n[${_name}] Todeskommando nicht durchgehend (pst=3 hp=-5 in ${_n3}/100 Bildern)" PARENT_SCOPE)
    endif()
    # Neuladen nach der Todesfolge: ein weiterer Bild-0-Eintrag mit hp=100 NACH dem negativen Stand.
    string(FIND "${_gift}" "hp=-5" _pneg)
    if(_pneg LESS 0)
        set(_fehler "${_fehler}\n[${_name}] gift.log ohne hp=-5" PARENT_SCOPE)
    else()
        string(SUBSTRING "${_gift}" ${_pneg} -1 _rest)
        string(FIND "${_rest}" "F0 aktiv=1 hp=100" _pre)
        if(_pre LESS 0)
            set(_fehler "${_fehler}\n[${_name}] kein Neuladen (hp=100 in Bild 0) nach dem Tod" PARENT_SCOPE)
        endif()
    endif()
endfunction()

gift_lauf(leon 2030 "5:15" "")
gift_lauf(leon_hp100 2030 "" "")
gift_lauf(elza 2031 "5:15" "RE15_TITLE_SHOT_CHAR=1")   # Elza (PL04) wie integration_r38_spinnen_echtlauf Lauf E; RE15_PSELECT_AUTO greift nur im echten Titelweg

file(REMOVE "${_exe}")
if(NOT _fehler STREQUAL "")
    message(FATAL_ERROR "r41_kleinkram_a_gift FEHLER:${_fehler}")
endif()
message(STATUS "r41_kleinkram_a_gift: OK (Danger + Giftkugel = Tod, Todesfolge, Neuladen; HP 100 -> 80 lebt)")
