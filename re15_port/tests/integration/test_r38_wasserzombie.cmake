# =============================================================================
# WASSERZOMBIES ZIEHEN SICH AM RAND HOCH — MIT DER ECHTEN EXE (Runde 38 Spur U).
#
# Dossier analysis/befunde_runde38/U_wasserzombie.md, Code engine/src/wasserzombie_r38.c. Die Unit-Riegel
# (unit_r38_wasserzombie) pruefen die Mechanik Bild fuer Bild; dieser Riegel prueft, was nur die exe zeigt: den
# LADE-Weg (CONTINUE in den Kanalraum, Boot-Weg ueber platform/pc/main.c), die Bank-Meldung aus dem Hybrid-Lader
# (ohne sie zieht der RE2-KI-Zombie NICHT hoch) und die Standard-KI des Nutzers (kein RE15_AI_FLAVOR = RE2).
#
# Laeufe (Spielstand aus probe_r38_wasserzombie_karte, CONTINUE, RE15_WASSERZOMBIE_LOG + RE15_GEGNER_Y_LOG):
#   a  ROOM2020 Leon, Standard-KI (RE2): Wasserzombie Slot 4 -> re2-eintritt, klettern, boden-1800 (+0x1ba),
#      oben auf (x,-1800,2222) Band 1 +0x1ba -1800, re2-uebergabe
#   b  ROOM2020 Leon, RE15_AI_FLAVOR=re15: drehen, klettern, boden-1800, oben (x,-1800,2222) Band 1, kein re2-
#   c  ROOM2021 Elza, Standard-KI: wie a (Elzas Raumvariante, dieselben Spawns)
#   d  ROOM2000 Leon, Standard-KI, ueber den RAUMLADER (Debug-Sprung = derselbe scd_room_reenter wie eine Tuer):
#      ein 0x84-Wasserzombie oben auf dem Oststeg (-20928,-1800,z) Band 1. NICHT per CONTINUE: der Boot-Weg
#      (platform/pc/main.c, s_preintro) haelt sub00 zurueck, sobald main00 ein Ereignis startet — ROOM2000 main00
#      @0x0177c `04 ff 18 07` (Evt_exec sub07) — und sub00 traegt die Spawns (@0x017cc/@0x017e0). Vorbestehend,
#      im Dossier unter OFFEN.
#   e  Nachbesserung 1: ROOM2000 Leon, Standard-KI, ueber die TUER 2070 -> 2000 (Wegplan): der greifende Zombie schreibt
#      dem kletternden im Griff-Ende 0x901 NACH dessen eigenem Tick -> wz.log "zwischenbild" (RE1.5-Bank, Clip 22) und
#      der Abbruch im eigenen Tick mit Bild + 1 (Uhr auf der RE1.5-Bank).
# Fuer jeden Lauf: kein Gegner jemals unter dem Wasserboden (y > 0) und keiner auf Band 2 (der Vorbefund: y = +1800,
# danach Band 2).
#
# RE15_SOFTWARE_RENDER=1 dient nur der Robustheit des Testhakens; geprueft werden die Logs. Die Bild-Abnahme steht
# im Dossier (Framedumps RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r38_wasserzombie_karte> -DWORKDIR=<dir>
#               -P test_r38_wasserzombie.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.16)   # Policies NEW (CMP0012/CMP0054/CMP0007) auch unter dem Container-CMake 3.28 im Skriptmodus wie auf dem Host (Memory reai-v2-linux-test-portabilitaet; Linux-Release v0.8.26)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r38_wasserzombie: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r38_wasserzombie: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r38_wasserzombie: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name (Muster test_r34n_b_cursor.cmake): local_build.sh eines parallelen Agenten beendet sonst jede
# re15_pc.exe. Die Kopie liegt neben der exe (Asset-Wurzel = exe-Verzeichnis).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r38u_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r38u_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(wz_lauf _name _raum _karte_extra _ki _ende _out_wz _out_gy)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log" "${WORKDIR}/wz.log" "${WORKDIR}/gy.log")
    set(_ki_env "")
    if(NOT "${_ki}" STREQUAL "")
        set(_ki_env "RE15_AI_FLAVOR=${_ki}")
    endif()
    if("${_karte_extra}" STREQUAL "sprung")
        # Raumlader per Debug-Sprung (re15_room_request_change -> scd_room_reenter, wie eine Tuer);
        # Spieler auf dem Oststeg wie im Kartenwerkzeug (Band 1).
        re15_start_spiel(_rv 300
            RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
            RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
            "RE15_DEBUG_JUMP=${_raum}@250" "RE15_PLAYER_POS=-20300,-11600,2048,1"
            RE15_WASSERZOMBIE_LOG=wz.log RE15_GEGNER_Y_LOG=gy.log
            "RE15_EXIT_AT=${_ende}#${_raum}"
            ${_ki_env}
            "${RE15_PC_EXE}")
        set(_marke "(ROOM${_raum})")
    elseif("${_karte_extra}" STREQUAL "tuer")
        # Nachbesserung 1: ECHTER WEG der Abnahme 0 (M1/M5) - Debug-Sprung nach ROOM2070, Wegplan (RE15_WEGPLAN,
        # engine/src/dachkiste_r36.c) durch die Tuer nach ROOM2000 und auf den Oststeg vor die Wasserzombies.
        file(WRITE "${WORKDIR}/plan.txt" "RAUM 2070\nWARTE 20\nBLICK 2048\nWARTE 10\nAKTION\nRAUM 2000\nWARTE 10\n"
                                         "GEHE -20600 -10600 200\nBLICK 2048\nWARTE 1500\nENDE\n")
        re15_start_spiel(_rv 300
            RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
            RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
            "RE15_DEBUG_JUMP=2070@250" "RE15_PLAYER_POS=-18500,-6400,2048,0"
            "RE15_WEGPLAN=${WORKDIR}/plan.txt"
            RE15_WASSERZOMBIE_LOG=wz.log RE15_GEGNER_Y_LOG=gy.log
            "RE15_EXIT_AT=${_ende}#${_raum}"
            ${_ki_env}
            "${RE15_PC_EXE}")
        set(_marke "PC loaded room${_raum}.rdt")
    else()
        execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}" ${_karte_extra}
                        WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                        RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
        if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
            message(FATAL_ERROR "r38_wasserzombie[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
        endif()
        re15_start_spiel(_rv 300
            RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
            RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
            RE15_WASSERZOMBIE_LOG=wz.log RE15_GEGNER_Y_LOG=gy.log
            "RE15_EXIT_AT=${_ende}#${_raum}"
            ${_ki_env}
            "${RE15_PC_EXE}")
        set(_marke "CONTINUE: resumed in room ${_raum}")
    endif()
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r38_wasserzombie[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(FIND "${_dbg}" "${_marke}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r38_wasserzombie[${_name}]: ROOM${_raum} nicht erreicht ('${_marke}' fehlt, exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r38_wasserzombie[${_name}]: Endbild nicht erreicht - Lauf vorher abgerissen "
                            "(exit=${_rv}, ${WORKDIR})")
    endif()
    set(_wz "")
    if(EXISTS "${WORKDIR}/wz.log")
        file(READ "${WORKDIR}/wz.log" _wz)
    endif()
    set(_gy "")
    if(EXISTS "${WORKDIR}/gy.log")
        file(READ "${WORKDIR}/gy.log" _gy)
    endif()
    set(${_out_wz} "${_wz}" PARENT_SCOPE)
    set(${_out_gy} "${_gy}" PARENT_SCOPE)
endfunction()

function(wz_zahl _text _muster _out)
    string(REGEX MATCHALL "${_muster}" _m "${_text}")
    list(LENGTH _m _n)
    set(${_out} ${_n} PARENT_SCOPE)
endfunction()

# Gemeinsame Gegner-Invariante: niemand unter dem Wasserboden (y > 0), niemand auf Band 2+.
function(wz_invariante _name _gy)
    string(REGEX MATCHALL "t=16 st=[0-9/]+ g=[0-9a-f]+ mo=[0-9]+ af=[0-9]+ @\\([-0-9]+,[1-9][0-9]*,[-0-9]+\\)" _unter "${_gy}")
    list(LENGTH _unter _nu)
    if(NOT _nu EQUAL 0)
        list(GET _unter 0 _erst)
        message(FATAL_ERROR "r38_wasserzombie[${_name}]: ${_nu} Bilder mit einem Zombie UNTER dem Wasserboden (y > 0), "
                            "z.B. '${_erst}' - der Vorbefund (fehlender Vorschub/+0x1ba)")
    endif()
    string(REGEX MATCHALL "t=16 [^]]* b[2-9] f1ba" _hoch "${_gy}")
    list(LENGTH _hoch _nh)
    if(NOT _nh EQUAL 0)
        list(GET _hoch 0 _erst)
        message(FATAL_ERROR "r38_wasserzombie[${_name}]: ${_nh} Bilder mit einem Zombie auf Band 2+ - '${_erst}'")
    endif()
endfunction()

# --- a: ROOM2020 Leon, Standard-KI (RE2) -----------------------------------------------------------------
wz_lauf(a 2020 "" "" 420 _wz _gy)
wz_zahl("${_wz}" "slot=[0-9]+ t=16 re2-eintritt st=1/10/0" _n1)
wz_zahl("${_wz}" "slot=[0-9]+ t=16 klettern st=1/10/3 mo=22 af=0 @\\([-0-9]+,0,1082\\) b0 f1ba=0 rot=3072" _n2)
wz_zahl("${_wz}" "slot=[0-9]+ t=16 boden-1800 st=1/10/3 mo=22 af=70 @\\([-0-9]+,0,1082\\) b0 f1ba=-1800" _n3)
wz_zahl("${_wz}" "slot=[0-9]+ t=16 oben st=1/0/0 mo=22 af=158 @\\([-0-9]+,-1800,2222\\) b1 f1ba=-1800 rot=3072" _n4)
wz_zahl("${_wz}" "slot=[0-9]+ t=16 re2-uebergabe st=1/0/" _n5)
if(NOT (_n1 GREATER 0 AND _n2 GREATER 0 AND _n3 GREATER 0 AND _n4 GREATER 0 AND _n5 GREATER 0))
    message(FATAL_ERROR "r38_wasserzombie[a]: RE2-KI ROOM2020 - Hochziehen unvollstaendig "
        "(eintritt ${_n1}, klettern ${_n2}, boden ${_n3}, oben ${_n4}, uebergabe ${_n5}) - Bank-Meldung main.c?\n${_wz}")
endif()
wz_invariante(a "${_gy}")

# --- b: ROOM2020 Leon, RE1.5-KI ---------------------------------------------------------------------------
wz_lauf(b 2020 "" re15 420 _wz _gy)
wz_zahl("${_wz}" "slot=[0-9]+ t=16 drehen st=1/10/1 mo=1" _n1)
wz_zahl("${_wz}" "slot=[0-9]+ t=16 boden-1800 st=1/10/3 mo=22 af=70 @\\([-0-9]+,0,1082\\) b0 f1ba=-1800" _n3)
wz_zahl("${_wz}" "slot=[0-9]+ t=16 oben st=1/0/0 mo=22 af=158 @\\([-0-9]+,-1800,2222\\) b1 f1ba=-1800 rot=3072" _n4)
wz_zahl("${_wz}" "re2-" _nr)
if(NOT (_n1 GREATER 0 AND _n3 GREATER 0 AND _n4 GREATER 0 AND _nr EQUAL 0))
    message(FATAL_ERROR "r38_wasserzombie[b]: RE1.5-KI ROOM2020 - (drehen ${_n1}, boden ${_n3}, oben ${_n4}, re2 ${_nr})\n${_wz}")
endif()
wz_invariante(b "${_gy}")

# --- c: ROOM2021 Elza, Standard-KI -------------------------------------------------------------------------
wz_lauf(c 2021 "elza" "" 420 _wz _gy)
wz_zahl("${_wz}" "R2021 slot=[0-9]+ t=16 re2-eintritt" _n1)
wz_zahl("${_wz}" "R2021 slot=[0-9]+ t=16 oben st=1/0/0 mo=22 af=158 @\\([-0-9]+,-1800,2222\\) b1 f1ba=-1800" _n4)
if(NOT (_n1 GREATER 0 AND _n4 GREATER 0))
    message(FATAL_ERROR "r38_wasserzombie[c]: Elza ROOM2021 - (eintritt ${_n1}, oben ${_n4})\n${_wz}")
endif()
wz_invariante(c "${_gy}")

# --- d: ROOM2000 Leon, Standard-KI (Deskriptor 0x84) -------------------------------------------------------
wz_lauf(d 2000 "sprung" "" 600 _wz _gy)
wz_zahl("${_wz}" "R2000 slot=[0-9]+ t=16 oben st=1/0/0 mo=22 af=158 @\\(-20928,-1800,[-0-9]+\\) b1 f1ba=-1800 rot=0" _n4)
if(NOT _n4 GREATER 0)
    message(FATAL_ERROR "r38_wasserzombie[d]: ROOM2000 - kein 0x84-Zombie oben auf dem Oststeg\n${_wz}")
endif()
wz_invariante(d "${_gy}")

# --- e: Tuer 2070 -> 2000 (Wegplan), Leon, Standard-KI - fremder Schreiber NACH dem eigenen Tick (Nachbesserung 1) --
# Gemessen (Abnahme 0 M5, Dossier 8.1): der greifende Zombie schreibt im Griff-Ende P8 das Wort 0x901 in den
# kletternden (RE2-Partner-Domino `sw v0,4(s0)` @0x80102E98), nachdem dessen Tick in diesem Bild schon gelaufen ist.
# Im Original bleibt die Pose bis zum eigenen Tick die des eigenen letzten anim_set (Teileblock +0x188 @0x8001f40c).
# Pflicht: wz.log "zwischenbild" (Bank/Uhr fragen im Zwischenbild die RE1.5-Bank, Clip 22, fremdes Wort 1/9) und der
# folgende "abbruch" desselben Slots mit Bild + 1 (Advancer auf Clip 0x16 = 159 Bilder; vorher a4809c35: RE2-Clip 22 =
# 10 Bilder -> af 49 -> 9, Bild F400 bis auf ein Bein leer).
wz_lauf(e 2000 "tuer" "" 406 _wz _gy)
string(REGEX MATCH "slot=([0-9]+) t=16 zwischenbild st=1/9/[0-9]+ mo=22 af=([0-9]+)" _zw "${_wz}")
if(NOT _zw)
    message(FATAL_ERROR "r38_wasserzombie[e]: kein Zwischenbild mit RE1.5-Bank nach fremdem 0x901 (Szenario der Abnahme 0 M5) "
                        "- Bank haengt am Zustandswort statt an der Marke?\n${_wz}")
endif()
set(_zslot ${CMAKE_MATCH_1})
set(_zaf ${CMAKE_MATCH_2})
math(EXPR _zaf1 "${_zaf} + 1")
string(REGEX MATCH "slot=${_zslot} t=16 abbruch st=1/9/0 mo=22 af=([0-9]+)" _ab "${_wz}")
if(NOT _ab OR NOT CMAKE_MATCH_1 EQUAL _zaf1)
    message(FATAL_ERROR "r38_wasserzombie[e]: Abbruch nach dem Zwischenbild '${_zw}' mit '${_ab}' - erwartet af=${_zaf1} "
                        "(Uhr auf der RE1.5-Bank bis zum eigenen Tick)\n${_wz}")
endif()
wz_zahl("${_wz}" "R2000 slot=[0-9]+ t=16 oben st=1/0/0 mo=22 af=158 @\\(-20928,-1800,[-0-9]+\\) b1 f1ba=-1800 rot=0" _n4)
if(NOT _n4 GREATER 0)
    message(FATAL_ERROR "r38_wasserzombie[e]: ROOM2000 (Tuerweg) - kein Zombie oben auf dem Oststeg\n${_wz}")
endif()
wz_invariante(e "${_gy}")

file(REMOVE "${_exe_kopie}")
message(STATUS "r38_wasserzombie: a/b/c/d/e gruen (Wasserzombies ziehen sich hoch, RE2- und RE1.5-KI, Leon und Elza, "
               "Zwischenbild nach fremdem Schreiber)")
