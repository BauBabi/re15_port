# Runde 35 Spur B, Nachbesserung 2 (Maengel N1/N2 der Abnahme 1) — an der ECHTEN exe.
#
# Dossier analysis/befunde_runde35/B_werfer.md §9. Registrierung: tests/unit/probes/r35_werfer.cmake.
#   rakete10e0  ROOM10E0, Leon (-3000,-3000) Blick 3072, RE15_GIVE=18:4, "M0.6,MA0.3,M1.6,W2" (Rezept Abnahme 1 §2.6).
#               Zelle 21 ist Typ 5 (LAB_8003c734, Verteiler 0x800b2858 @0x8003af44): fest ist d*(x-X)/w < z-Z mit
#               {X -3700, Z -1400, w 2850, d 3700}. Vorher flog die Rakete drei Bilder durch dieses Dreieck und explodierte
#               erst @(-2070,-2728,1897). Gemessen wird: die Explosion 0x01140001 liegt VOR der Hypotenuse (z < 528) und
#               keine RE2FLUG-Lage der Rakete liegt im festen Dreieck.
#   python1140  ROOM1140, Leon (-1676,-18070) Blick 1076, RE15_GIVE=20:6, "M0.6,MA1.5,M1.0,W4" (Rezept Abnahme 1 §2.2):
#               der Python-Treffer auf Platz 2 (Typ 0x10, freie Schusslinie, d 1530) setzt die Kritklasse (+0x93 |= 0x40,
#               @0x800123b4-b8; w20 = PORT-WAHL §3.5) -> HP -1 (@0x800124fc-1c) wie der Redhawk. Vorher hp -850.
# Ein Lauf ohne Ergebnis (exit 1/-1 = von aussen beendet) wird EINMAL wiederholt.

cmake_minimum_required(VERSION 3.16)
set(_tag "r35_werfer_form")
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "${_tag}: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "${_tag}: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_wurzel}")

function(r35f_lauf _name _raum _pos _give _equip _skript _ende)
    set(_dir "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${_dir}")
    set(WORKDIR "${_dir}")
    set(_env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
             RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=${_pos}"
             "RE15_GIVE=${_give}" "RE15_EQUIP=${_equip}"
             RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_skript}"
             RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log RE15_WPN_DBG=1 "RE15_EXIT_AT=${_ende}#${_raum}")
    re15_start_spiel(_rv 240 ${_env} "${RE15_PC_EXE}")
    if("${_rv}" STREQUAL "1" OR "${_rv}" STREQUAL "-1" OR NOT EXISTS "${_dir}/wf.log")
        message(STATUS "${_tag} [${_name}]: Lauf ohne Ergebnis (exit=${_rv}) -> EIN Wiederholungsversuch")
        file(REMOVE "${_dir}/debug.log" "${_dir}/state.log" "${_dir}/wf.log")
        re15_start_spiel(_rv 240 ${_env} "${RE15_PC_EXE}")
    endif()
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "${_tag} [${_name}]: re15_pc.exe exit=${_rv}, ${_dir}")
    endif()
    foreach(_d debug.log wf.log state.log)
        if(NOT EXISTS "${_dir}/${_d}")
            message(FATAL_ERROR "${_tag} [${_name}]: kein ${_d} in ${_dir}")
        endif()
    endforeach()
    file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
    if(NOT _ex)
        message(FATAL_ERROR "${_tag} [${_name}]: RE15_EXIT_AT nicht erreicht — Haenger?")
    endif()
endfunction()

# ---- N1: Rakete gegen die Diagonalzelle ROOM10E0 Zelle 21 ------------------------------------------------------
r35f_lauf(rakete10e0 10E0 "-3000,-3000,3072" "18:4" 18 "M0.6,MA0.3,M1.6,W2" 120)
set(_d "${_wurzel}/rakete10e0")
file(STRINGS "${_d}/wf.log" _ex REGEX "re2fx code=0x01140001 .*@\\(-?[0-9]+,-?[0-9]+,-?[0-9]+\\)")
list(LENGTH _ex _nex)
if(_nex LESS 1)
    message(FATAL_ERROR "${_tag} [rakete10e0]: keine Raketen-Explosion 0x01140001 im wf.log")
endif()
list(GET _ex 0 _e0)
string(REGEX MATCH "@\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\)" _m "${_e0}")
set(_ez "${CMAKE_MATCH_3}")
if(_ez GREATER_EQUAL 528 OR _ez LESS -3000)
    message(FATAL_ERROR "${_tag} [rakete10e0]: Explosion nicht VOR der Hypotenuse der Zelle 21 (z soll -3000..527): '${_e0}'")
endif()
file(STRINGS "${_d}/wf.log" _fl REGEX "RE2FLUG platz=[0-9]+ bank=2 sub=13 .*welt=\\(")
set(_innen 0)
foreach(_z IN LISTS _fl)
    string(REGEX MATCH "welt=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\)" _m "${_z}")
    set(_x "${CMAKE_MATCH_1}")
    set(_zz "${CMAKE_MATCH_3}")
    if(_x GREATER_EQUAL -3700 AND _x LESS -850 AND _zz GREATER_EQUAL -1400 AND _zz LESS 2300)
        math(EXPR _line "3700 * (${_x} + 3700) / 2850")
        math(EXPR _zterm "${_zz} + 1400")
        if(_line LESS _zterm)
            math(EXPR _innen "${_innen} + 1")
            message(STATUS "${_tag} [rakete10e0]: Flug-Lage im festen Dreieck: ${_z}")
        endif()
    endif()
endforeach()
list(LENGTH _fl _nfl)
if(_innen GREATER 0)
    message(FATAL_ERROR "${_tag} [rakete10e0]: ${_innen} von ${_nfl} Raketen-Lagen im festen Teil der Typ-5-Zelle 21")
endif()

# ---- N2: Python in der Kritklasse ----------------------------------------------------------------------------
r35f_lauf(python1140 1140 "-1676,-18070,1076" "20:6" 20 "M0.6,MA1.5,M1.0,W4" 120)
set(_d "${_wurzel}/python1140")
file(STRINGS "${_d}/state.log" _st REGEX "\\[2 t=10 st=3 [^]]*\\] hp=-?[0-9]+")
list(LENGTH _st _nst)
if(_nst LESS 1)
    message(FATAL_ERROR "${_tag} [python1140]: Platz 2 nie im Todeszustand (st=3) — kein Python-Treffer")
endif()
list(GET _st 0 _s0)
string(REGEX MATCH "\\[2 t=10 st=3 [^]]*\\] hp=(-?[0-9]+)" _m "${_s0}")
set(_hp "${CMAKE_MATCH_1}")
if(NOT _hp EQUAL -1)
    message(FATAL_ERROR "${_tag} [python1140]: Python-Treffer ohne Kritklasse — hp ${_hp} statt -1 (Redhawk: -1)")
endif()
message(STATUS "${_tag}: OK — Rakete 10E0 explodiert vor der Hypotenuse (${_e0}), ${_nfl} Flug-Lagen ausserhalb des "
               "festen Dreiecks; Python-Treffer Platz 2 -> hp -1 (Kritklasse)")
