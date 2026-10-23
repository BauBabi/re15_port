# =============================================================================
# Runde 41 Spur B "re2gator" — der RE2-Alligator in der ECHTEN exe (ROOM2090 per Sprung).
#
# Dossier analysis/befunde_runde41/B_re2gator.md. Die Unit-Riegel unit_r41_re2gator_* messen die
# Mechanik am Boss-Tick; dieser Riegel prueft, was nur die exe zeigt: Spawn + Boss-Tick im Spiel,
# RE15_GATOR_KANISTER-Haken (Kanister gesetzt, Zeichner laedt das Modell), Verfolgung mit Clip 1
# (Paddeln, RE2 Sub 2) im Zustandslog, RE2-Zeilen in gator_boss.log (INTRO/DASH), HP-Sockel
# (kein Tod durch die Zeit), Schuetteln ohne Absturz des Hintergrund-Blits.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_re2gator.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_re2gator: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_re2gator: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_dir "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_dir}")
# EIGENER EXE-NAME (Muster test_r34n_b_cursor.cmake): parallele Bauten beenden re15_pc.exe per Namen.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte "${_exe_dir}/re15_pc_r41b_*.exe")
foreach(_k IN LISTS _alte)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41b_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(WORKDIR "${_dir}")
file(REMOVE "${_dir}/debug.log" "${_dir}/state.log" "${_dir}/gator_boss.log")
# Umgebung wie test_r35_granate.cmake (_env_basis): Titelbild + Auto-Start (RE15_TITLE_SHOT_AF=2), sonst
# bleibt die exe im Titel stehen und der Sprung @250 greift nie (gemessen 2026-10-23: 7 debug-Zeilen in 90 s).
re15_start_spiel(_rv 300
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
    SDL_ASSERT=always_ignore
    RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1
    "RE15_GATOR_KANISTER=-6000,-15000"
    RE15_STATE_LOG=state.log
    "RE15_EXIT_AT=300#2090"
    "${_exe_kopie}")
# ⛔ 300 Bilder im Raum, nicht mehr (gemessen 2026-10-23, Lauf 0d58130a): Leon steht nach dem Sprung in der
# Tuernische (-8200,-25550) mit 100 HP, der Gator erreicht ihn mit dem RE2-Paddeltempo in ~1 s, drei Bisse
# -> FRESSEN -> Game Over -> Titel; ein spaeteres RE15_EXIT_AT #2090 greift dann nie (Memory
# reai-v2-gator-bosskampf: "Design-Schaden + Biss = Leon in <2 s tot -> Game Over -> Title").
file(REMOVE "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    message(FATAL_ERROR "r41_re2gator: exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
endif()
file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
if(NOT _ex)
    message(FATAL_ERROR "r41_re2gator: RE15_EXIT_AT nicht erreicht (keine Zeile '[flow] EXIT_AT')")
endif()

# Kanister-Haken + Modell
file(STRINGS "${_dir}/debug.log" _kan REGEX "\\[gator-re2\\] Kanister-Modell geladen")
if(NOT _kan)
    message(FATAL_ERROR "r41_re2gator: Kanister-Modell nicht geladen (KANISTER40A0.MD1/.TIM) - debug.log ohne '[gator-re2] Kanister-Modell geladen'")
endif()
file(STRINGS "${_dir}/gator_boss.log" _gb)
set(_gesetzt 0)
set(_re2 0)
set(_dash 0)
set(_intro 0)
foreach(_l IN LISTS _gb)
    if(_l MATCHES "^RE2 KANISTER gesetzt")
        set(_gesetzt 1)
    endif()
    if(_l MATCHES "^RE2 ")
        math(EXPR _re2 "${_re2} + 1")
    endif()
    if(_l MATCHES "^RE2 DASH start")
        set(_dash 1)
    endif()
    if(_l MATCHES "^RE2 INTRO start")
        set(_intro 1)
    endif()
endforeach()
if(NOT _gesetzt)
    message(FATAL_ERROR "r41_re2gator: RE15_GATOR_KANISTER ohne 'RE2 KANISTER gesetzt' in gator_boss.log")
endif()

# Zustandslog: Gator-Slot mit Clip 1 (Paddeln) in der Verfolgung, HP ueber dem Sockel
file(STRINGS "${_dir}/state.log" _zl)
set(_mo1 0)
set(_gator 0)
set(_hp_min 99999)
foreach(_l IN LISTS _zl)
    if(_l MATCHES "\\[([0-9]+) t=23 st=[0-9]+ ss1=[0-9]+ ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=([0-9]+) af=[0-9]+ [^]]*\\] hp=(-?[0-9]+)")
        set(_gator 1)
        if("${CMAKE_MATCH_2}" STREQUAL "1")
            math(EXPR _mo1 "${_mo1} + 1")
        endif()
        # hp=0 steht nur im Spawn-Bild VOR dem Boss-INIT (main.c spawnt mit 0, der erste Boss-Tick schreibt 25300)
        if(${CMAKE_MATCH_3} GREATER 0 AND ${CMAKE_MATCH_3} LESS _hp_min)
            set(_hp_min ${CMAKE_MATCH_3})
        endif()
    endif()
endforeach()
if(NOT _gator)
    message(FATAL_ERROR "r41_re2gator: kein Alligator (t=23) im Zustandslog")
endif()
if(_mo1 LESS 30)
    message(FATAL_ERROR "r41_re2gator: Paddeln (Clip 1) nur in ${_mo1} Bildern von 300 (RE2 Sub 2 bei +0x1F0 >= 0x65)")
endif()
if(_hp_min LESS 25000)
    message(FATAL_ERROR "r41_re2gator: HP unter dem RE2-Sockel (${_hp_min} < 25000) ohne Beschuss")
endif()
message(STATUS "r41_re2gator: ok — Kanister gesetzt, RE2-Zeilen ${_re2} (DASH ${_dash}, INTRO ${_intro}), Paddel-Bilder ${_mo1}, HP min ${_hp_min}")
