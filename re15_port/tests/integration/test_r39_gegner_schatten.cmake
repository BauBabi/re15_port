# =============================================================================
# Runde 39 Spur E "gegner" — der Gorilla-Schatten bleibt beim Sprung auf dem Boden, gemessen an der ECHTEN exe.
#
# Nutzer (AUFTRAG.md Nachtrag 2026-10-18): "Die Gorillas haben beim springen direkt ihren Schatten unter dem beim."
# Dossier analysis/befunde_runde39/E_gegner.md Punkt 4; Code engine/src/gegner_r39.c (re15_r39_schatten_y),
# Haken platform/pc/main.c hinter re15_wasserzombie_schatten_y.
#
# Original: Gorilla-Wurzel FUN_80116db8 (STAGE1.BIN) `lh a1,442(v0)` @0x80116f30 = +0x1ba (Boden-Referenz) ->
# `jal 0x8001b064` @0x80116f34; FUN_8001b064 `local_88.t[1] = param_2` = Quad-Hoehe. Der Port nahm npc->y (Koerper).
#
# Aufbau (Runde 35 Spur J, Lauf A2): ROOM11C0 per Debug-Sprung (RE15_DEBUG_JUMP=11C0@240), Leon am Szenen-Start
# (-22604,14455), keine Eingabe; die Szene laeuft, Gorilla Platz 3 macht einen Fernsprung (Sub 7, Basislauf ab F1352,
# 39 Bilder). Gemessen wird gegner_r39.log (RE15_R39_LOG=1): eine Zeile je gezeichnetem Gorilla-Bild, in dem Koerper-Y
# != Boden-Y ist ("schatten slot=.. koerper_y=.. boden_y=.. quad_y=..").
# Geprueft: (a) im state.log gibt es Bilder mit einem 0x27 in Sub 7 (der Sprung fand statt); (b) mindestens 10
# Schatten-Zeilen mit Koerper-Y != Boden-Y (der Gorilla war in der Luft und wurde gezeichnet); (c) in JEDER dieser
# Zeilen quad_y == boden_y (Quad auf +0x1ba, nicht am Koerper). Vorher (Basisstand c71fa144, derselbe Aufbau):
# Quad-Y = Koerper-Y, Framedumps F1355..F1385 zeigen den Blob unter dem fliegenden Gorilla (Dossier).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r39_gegner_schatten.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.16)
set(_tag "r39_gegner_schatten")
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "${_tag}: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "${_tag}: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_dir "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_dir}")

# Eigene Messkopie neben der exe (Asset-Wurzel = exe-Verzeichnis), Praefix NUR dieses Tests.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r39e_schatten_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r39e_schatten_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# keine Umgebung des Aufrufers erben
foreach(_v RE15_AI_FLAVOR RE15_R39_LOG RE15_FRAMEDUMP RE15_INPUT_SCRIPT RE15_INPUT_SCRIPT_START RE15_PLAYER_POS)
    unset(ENV{${_v}})
endforeach()
set(WORKDIR "${_dir}")
set(_env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
         RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
         RE15_DEBUG_JUMP=11C0@240 "RE15_PLAYER_POS=-22604,14455,0"
         RE15_STATE_LOG=state.log RE15_R39_LOG=1
         "RE15_FRAMEDUMP=1340-1400/10:fd_" "RE15_EXIT_AT=1500#11C0")
re15_start_spiel(_rv 420 ${_env} "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    message(STATUS "${_tag}: Lauf ohne Ergebnis (exit=${_rv}) -> EIN Wiederholungsversuch")
    file(REMOVE "${_dir}/debug.log" "${_dir}/state.log" "${_dir}/gegner_r39.log")
    re15_start_spiel(_rv 420 ${_env} "${_exe_kopie}")
endif()
if(NOT _rv EQUAL 0)
    message(FATAL_ERROR "${_tag}: re15_pc exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
endif()
foreach(_d debug.log state.log gegner_r39.log)
    if(NOT EXISTS "${_dir}/${_d}")
        message(FATAL_ERROR "${_tag}: kein ${_d} in ${_dir}")
    endif()
endforeach()
file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
if(NOT _ex)
    message(FATAL_ERROR "${_tag}: RE15_EXIT_AT nicht erreicht — Haenger?")
endif()

# (a) der Sprung fand statt: ein 0x27 in Sub 7 (Fernsprung) oder Sub 8 (Luft-Finisher)
file(STRINGS "${_dir}/state.log" _sprung REGEX "t=27 st=1 ss1=[78] ")
list(LENGTH _sprung _n_sprung)
if(_n_sprung LESS 10)
    message(FATAL_ERROR "${_tag}: kein Gorilla-Sprung im state.log (Bilder mit 0x27 in Sub 7/8: ${_n_sprung})")
endif()

# (b)/(c) Schatten-Zeilen: koerper_y != boden_y -> quad_y == boden_y
file(STRINGS "${_dir}/gegner_r39.log" _zl REGEX "^schatten ")
set(_n_luft 0)
set(_fehler "")
foreach(_l IN LISTS _zl)
    if(_l MATCHES "koerper_y=(-?[0-9]+) boden_y=(-?[0-9]+) quad_y=(-?[0-9]+)")
        set(_ky "${CMAKE_MATCH_1}")
        set(_by "${CMAKE_MATCH_2}")
        set(_qy "${CMAKE_MATCH_3}")
        if(NOT _ky EQUAL _by)
            math(EXPR _n_luft "${_n_luft} + 1")
            if(NOT _qy EQUAL _by)
                string(APPEND _fehler " [${_l}]")
            endif()
        endif()
    endif()
endforeach()
if(_n_luft LESS 10)
    message(FATAL_ERROR "${_tag}: zu wenige Luft-Bilder mit gezeichnetem Gorilla-Schatten (${_n_luft} < 10): ${_dir}")
endif()
if(_fehler)
    message(FATAL_ERROR "${_tag}: Quad-Y != Boden-Y in Luft-Bildern:${_fehler}")
endif()
message(STATUS "${_tag}: OK — ${_n_sprung} Sprung-Bilder (Sub 7/8), ${_n_luft} Luft-Bilder mit Quad-Y == Boden-Y (+0x1ba @0x80116f30), ${_dir}")
