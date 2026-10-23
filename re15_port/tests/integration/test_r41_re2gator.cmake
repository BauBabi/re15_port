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
# Nachbesserung 1 (Abnahme 0, M5): KEIN file(GLOB)+REMOVE fremder re15_pc_r41b_*.exe mehr - zwei Instanzen
# dieses Riegels auf einem Bauordner toeteten sich gegenseitig; nur die eigene Kopie wird am Ende entfernt.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
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
    "RE15_EXIT_AT=700#2090"
    "${_exe_kopie}")
# Nachbesserung 1 (Abnahme 0, M1/M2): der natuerliche Ablauf aus der Tuernische (-8200,-25550) ist jetzt
# (gemessen 2026-10-23, Lauf A nb1): F6-130 Clip 6 (RE2-Intro), F131-280 Clip 3 (+ yaw 1024), F281-340 Clip 0
# (Biss-Sperre 110, Wende), F341 Zonenwechsel 2 -> 0 = RE2-Dash Clip 2 (F341-490) + Clip 3 (F491-640) mit
# 6 x 9 Putz um Leon, F641 Clip 0. Leon (Statue) stirbt dabei durch die SPINNEN der RDT (Spinne 2 sitzt bei
# (-7882,-25018), 650 von der Nische; Gift-Ticks ab F57, hp < 0 bei F628) - das Paddeln (Clip 1) zum Biss
# kommt im exe-Lauf deshalb nicht mehr vor (Spur F/G entfernt die Spinnen; Mechanik-Pin: unit_r41_re2gator_clips).
# 700 Bilder liegen VOR dem Game-Over-Titel (ein spaeteres RE15_EXIT_AT #2090 griffe nie, Memory
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
set(_dash_ende 0)
set(_intro 0)
set(_intro_ende 0)
set(_putz_runden 0)
set(_putz_gesamt 0)
set(_putz_im_becken 1)
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
    if(_l MATCHES "^RE2 DASH ende")
        set(_dash_ende 1)
    endif()
    if(_l MATCHES "^RE2 INTRO start")
        set(_intro 1)
    endif()
    if(_l MATCHES "^RE2 INTRO ende")
        set(_intro_ende 1)
    endif()
    # Nachbesserung 1 (M2): je Durchlauf eine Zeile; Lage um Leon muss IM Becken liegen (x -8900..7200,
    # z -27000..-5400) - vorher spawnte der Putz um (0,0,0) ausserhalb.
    if(_l MATCHES "^RE2 PUTZ runde ([0-9]+)/6 spawns=([0-9]+) gesamt=([0-9]+) um=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\)")
        math(EXPR _putz_runden "${_putz_runden} + 1")
        set(_putz_gesamt ${CMAKE_MATCH_3})
        if(${CMAKE_MATCH_4} LESS -8900 OR ${CMAKE_MATCH_4} GREATER 7200 OR ${CMAKE_MATCH_6} LESS -27000 OR ${CMAKE_MATCH_6} GREATER -5400)
            set(_putz_im_becken 0)
        endif()
    endif()
endforeach()
if(NOT _gesetzt)
    message(FATAL_ERROR "r41_re2gator: RE15_GATOR_KANISTER ohne 'RE2 KANISTER gesetzt' in gator_boss.log")
endif()
if(NOT _intro OR NOT _intro_ende)
    message(FATAL_ERROR "r41_re2gator: RE2-Intro (Clip 6 -> 3, Sub 0 @0x8010086c-948) nicht gezuendet/beendet (start ${_intro}, ende ${_intro_ende})")
endif()
if(NOT _dash OR NOT _dash_ende)
    message(FATAL_ERROR "r41_re2gator: RE2-Dash (Zonenwechsel -> Clip 2/3 @0x80100dac-e28) nicht gezuendet/beendet (start ${_dash}, ende ${_dash_ende}) - Abnahme-0-Mangel M1")
endif()
if(NOT _putz_runden EQUAL 6 OR NOT _putz_gesamt EQUAL 54)
    message(FATAL_ERROR "r41_re2gator: Putz ${_putz_runden} Durchlaeufe / ${_putz_gesamt} Spawns statt 6 / 54 (ROOM40A0 sub13/sub14) - Abnahme-0-Mangel M2")
endif()
if(NOT _putz_im_becken)
    message(FATAL_ERROR "r41_re2gator: Putz-Lage ausserhalb des Beckens (putz_x/y/z nicht um Leon)")
endif()

# Zustandslog: Gator-Slot mit der RE2-Clip-Folge 6 (Intro) / 2 + 3 (Dash) im Spiel, HP ueber dem Sockel
file(STRINGS "${_dir}/state.log" _zl)
set(_mo6 0)
set(_mo2 0)
set(_mo3 0)
set(_gator 0)
set(_hp_min 99999)
foreach(_l IN LISTS _zl)
    if(_l MATCHES "\\[([0-9]+) t=23 st=[0-9]+ ss1=[0-9]+ ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=([0-9]+) af=[0-9]+ [^]]*\\] hp=(-?[0-9]+)")
        set(_gator 1)
        if("${CMAKE_MATCH_2}" STREQUAL "6")
            math(EXPR _mo6 "${_mo6} + 1")
        elseif("${CMAKE_MATCH_2}" STREQUAL "2")
            math(EXPR _mo2 "${_mo2} + 1")
        elseif("${CMAKE_MATCH_2}" STREQUAL "3")
            math(EXPR _mo3 "${_mo3} + 1")
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
# Clip 6 = 125 Bilder (EDD), Clip 2 = 150, Clip 3 = 2 x 150 (Intro + Dash); Toleranz fuer den Bildzaehler-Start
if(_mo6 LESS 120 OR _mo2 LESS 145 OR _mo3 LESS 290)
    message(FATAL_ERROR "r41_re2gator: Clip-Bilder Clip 6 ${_mo6} / Clip 2 ${_mo2} / Clip 3 ${_mo3} (erwartet 125 / 150 / 300)")
endif()
if(_hp_min LESS 25000)
    message(FATAL_ERROR "r41_re2gator: HP unter dem RE2-Sockel (${_hp_min} < 25000) ohne Beschuss")
endif()
message(STATUS "r41_re2gator: ok — Kanister gesetzt, RE2-Zeilen ${_re2} (INTRO ${_intro}/${_intro_ende}, DASH ${_dash}/${_dash_ende}, Putz ${_putz_runden} x 9 = ${_putz_gesamt}), Clip 6/2/3 Bilder ${_mo6}/${_mo2}/${_mo3}, HP min ${_hp_min}")
