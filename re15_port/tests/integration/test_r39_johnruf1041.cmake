cmake_minimum_required(VERSION 3.16)
# =============================================================================
# RUNDE 39 SPUR P "johnruf1041" MIT DER ECHTEN EXE. Dossier analysis/befunde_runde39/P_johnruf1041.md, Konstanten
# include/re15_johnruf_r39.h. Die Unit-Riegel unit_r39_johnruf1041_* pruefen die Mechanik am Engine-Code; dieser Riegel
# prueft den Nutzerweg mit Elza (Spielstand + CONTINUE, danach nur echte Tueren per RE15_WEGPLAN):
#   TEIL a  ROOM1021 nach der John-Szene -> Tuer -> ROOM1041: zwei LEISE Hilferufe (Xa_vol 50), Elza dreht sich zur Treppe,
#           msg 40 mit main40.wav (Xa_vol 120), Karte mit ROOM1141 (Abzug des Hinweis-Schirms RE15_INV_FB_SHOT), danach
#           Bit 97 + MAIN01; Gang -> Tuer -> ROOM1061: MAIN01 laeuft durch. Pegel je Zeile im Mitschnitt (Werkzeug pegel).
#   TEIL b  Spielstand ROOM1121 MITTEN in der Panikphase (Bit 97) -> MAIN01 nach dem Laden; Tuer -> ROOM1131: Ruf help02
#           LAUTER (Xa_vol 100) einmal; Tuer -> ROOM1141 (Spur J unveraendert: MAIN16, Rufe Standard 120) -> Kampfende per
#           Messhaken RE15_R38F_GORILLAS_TOT (Kill-Flags, keine Schusszahl) -> "John gerettet": Bit 97 = 0 im Bild des
#           Original-Stopps sub01 @0x00C06; Dachtuer -> ROOM1171: Tabellen-Musik MAIN32, kein MAIN01 mehr.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_WERKZEUG=<test_r39_johnruf1041> -DRE15_KARTE_ELZA=<probe_r36_elza_karte>
#               -DRE15_SYNCHRO=<synchro-Ordner> -DWORKDIR=<dir> -DTEIL=a|b -P test_r39_johnruf1041.cmake
# =============================================================================
foreach(_v RE15_PC_EXE RE15_WERKZEUG RE15_KARTE_ELZA RE15_SYNCHRO WORKDIR TEIL)
    if(NOT ${_v})
        message(FATAL_ERROR "r39_johnruf1041: ${_v} fehlt")
    endif()
endforeach()
foreach(_v RE15_PC_EXE RE15_WERKZEUG RE15_KARTE_ELZA)
    if(NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r39_johnruf1041: ${_v} existiert nicht: '${${_v}}'")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Keine RE15_*-Variablen des Aufrufers erben (VERTRAG §2; Muster test_r39_stimmen.cmake).
execute_process(COMMAND "${CMAKE_COMMAND}" -E environment OUTPUT_VARIABLE _envdump)
string(REGEX MATCHALL "(^|\n)RE15_[A-Za-z0-9_]*=" _geerbt "${_envdump}")
foreach(_g ${_geerbt})
    string(REGEX REPLACE "^\n?(RE15_[A-Za-z0-9_]*)=$" "\\1" _n "${_g}")
    unset(ENV{${_n}})
endforeach()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_lauf "${WORKDIR}/lauf_${TEIL}_${_lauf_id}")
if(AUSWERTEN)
    set(_lauf "${AUSWERTEN}")
endif()

# Eigene exe-Kopie mit Praefix JE TEST (und Teil) neben der exe (Asset-Wurzel = exe-Verzeichnis).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe_kopie "${_exe_dir}/re15_pc_r39p_${TEIL}_${_lauf_id}${_exe_ext}")

set(_fehler "")
macro(pruef _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(_p LESS 0)
        list(APPEND _fehler "fehlt '${_muster}'")
        message(STATUS "FAIL fehlt '${_muster}'")
    else()
        message(STATUS "ok   '${_muster}'")
    endif()
endmacro()
macro(pruef_nicht _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(NOT _p LESS 0)
        list(APPEND _fehler "unerwartet '${_muster}'")
        message(STATUS "FAIL unerwartet '${_muster}'")
    else()
        message(STATUS "ok   nicht da '${_muster}'")
    endif()
endmacro()
macro(pruef_wahr _bed _text)
    if(${_bed})
        message(STATUS "ok   ${_text}")
    else()
        list(APPEND _fehler "${_text}")
        message(STATUS "FAIL ${_text}")
    endif()
endmacro()
# Position eines Musters im Text (-1 = fehlt)
macro(pos _text _muster _erg)
    string(FIND "${${_text}}" "${_muster}" ${_erg})
endmacro()
# Pegel einer Sprachzeile: "[voice] Start ROOM<raum> Id <id> CD_VOL <c> capTick <n>" -> Werkzeug pegel
macro(pegel _text _raum _id _wav _xa)
    string(REGEX MATCH "\\[voice\\] Start ROOM${_raum} Id ${_id} CD_VOL ([0-9]+) capTick ([0-9]+)" _m "${${_text}}")
    if(NOT _m)
        list(APPEND _fehler "keine Startzeile ROOM${_raum} Id ${_id}")
        message(STATUS "FAIL keine Startzeile ROOM${_raum} Id ${_id}")
    else()
        set(_takt "${CMAKE_MATCH_2}")
        execute_process(COMMAND "${RE15_WERKZEUG}" pegel "${_lauf}/cap.raw" "${RE15_SYNCHRO}/${_wav}" ${_takt} ${_xa}
                        RESULT_VARIABLE _rv OUTPUT_VARIABLE _out)
        string(STRIP "${_out}" _out)
        message(STATUS "     ${_out}")
        if(NOT _rv EQUAL 0)
            list(APPEND _fehler "Pegel ROOM${_raum} Id ${_id} (${_wav}) != Xa_vol ${_xa}/120")
            message(STATUS "FAIL Pegel ROOM${_raum} Id ${_id}")
        else()
            message(STATUS "ok   Pegel ROOM${_raum} Id ${_id} ${_wav} = Xa_vol ${_xa}/120 (+-10 %)")
        endif()
    endif()
endmacro()

if(TEIL STREQUAL "a")
    # Spielstand ROOM1021 nach der John-Szene (Flags wie integration_r39_dining1141_d: (3,0x5F) (4,0xF0) (4,0xF1) (4,0xEC)
    # (3,0x78); dazu (4,5)/(4,4) = Rolladen 1041 offen, ROOM1041 sub00 @0x01564 / sub01 @0x015A0..@0x015B0), vor Tuer slot 2.
    set(_karte "1021;-25500;-8200;2048;3:0x5F;4:0xF0;4:0xF1;4:0xEC;3:0x78;4:5;4:4;9:64")   # 9:64 Herzschluessel (Nachtrag 17)
    set(_plan "RAUM 1021
WARTE 30
GEHE -26700 -8200 80
BLICK 2048
AKTION
RAUM 1041
WARTE 30
GEGNERWEG
RAUM 1061
WARTE 120
ENDE
")
    # Integration Runde 39: nach dem Kartenschluss (Bit 97) faehrt Spur Q ihre Montage (1041 -> 1061 -> 1031 -> 1041); der
    # Lauf endet in deren 1061-Schnitt. Die Tuer 1041 -> 1031 als Text pruefen unit tuer1041 und integration ..._tuer_t1041.
    set(_ende "RE15_EXIT_AT=120#1061")
    set(_extra "RE15_INV_FB_SHOT=karte.bmp" "RE15_INV_FB_SHOT_AT=60" "RE15_FRAMEDUMP=0-600/30:fd_")
elseif(TEIL STREQUAL "b")
    # Spielstand ROOM1121 vor der Tuer nach 1131 mitten in der Panikphase (Bit 97), Dining Room Key im Inventar (give:0x43).
    set(_karte "1121;-7400;-2900;2048;3:0x5F;4:0xF0;4:0xF1;4:0xEC;3:0x78;9:97;give:0x43")
    set(_plan "RAUM 1121
WARTE 30
GEHE -7900 -2900 80
BLICK 2048
AKTION
RAUM 1131
WARTE 150
GEHE -1300 -13900 100
BLICK 0
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
BLICK 0
AKTION
RAUM 1141
WARTE 420
GEHE -7600 -12000 150
GEHE -8150 -10250 80
BLICK 2048
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 200
GEHE -7350 -8500 150
GEHE -7350 -7600 150
GEHE -7250 -1000 80
BLICK 3072
AKTION
WARTE 40
AKTION
WARTE 40
AKTION
WARTE 60
GEHE -7900 -15000 150
GEHE -7900 -17750 80
BLICK 2048
AKTION
WARTE 90
AKTION
WARTE 30
GEHE -5500 -19000 150
WARTE 700
WARTE 900
GEHE -6000 -12000 150
GEHE -7350 -7600 150
GEHE -7250 -1000 80
BLICK 3072
AKTION
RAUM 1171
WARTE 300
ENDE
")
    set(_ende "RE15_EXIT_AT=300#1171")
    set(_extra "RE15_R38F_GORILLAS_TOT=600" "RE15_R38F_GORILLAS_STARR=1")
else()
    message(FATAL_ERROR "r39_johnruf1041: TEIL '${TEIL}' unbekannt (a|b)")
endif()

if(NOT AUSWERTEN)
    file(MAKE_DIRECTORY "${_lauf}")
    file(WRITE "${_lauf}/wegplan.txt" "${_plan}")
    string(REPLACE ";" ";" _kargs "${_karte}")
    execute_process(COMMAND "${RE15_KARTE_ELZA}" "${_lauf}/re15_card.mcr" ${_kargs}
                    RESULT_VARIABLE _rvk OUTPUT_QUIET ERROR_QUIET)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${_lauf}/re15_card.mcr")
        message(FATAL_ERROR "r39_johnruf1041[${TEIL}]: Spielstand nicht geschrieben (${_rvk})")
    endif()
    file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
    set(WORKDIR "${_lauf}")
    re15_start_spiel(_rv 3000
        RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
        RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_BGM_CTL_DEBUG=1 RE15_BGM_FADE_DEBUG=1 RE15_STATE_LOG=state.txt
        RE15_AUDIO_CAP_SYNC=cap.raw "RE15_WEGPLAN=${_lauf}/wegplan.txt" RE15_WEGPLAN_LOG=wegplan_log.txt
        "${_ende}" ${_extra}
        "${_exe_kopie}")
    file(REMOVE "${_exe_kopie}")
    message(STATUS "r39_johnruf1041[${TEIL}]: exit=${_rv}, ${_lauf}")
endif()

if(NOT EXISTS "${_lauf}/debug.log")
    message(FATAL_ERROR "r39_johnruf1041[${TEIL}]: kein debug.log (${_lauf})")
endif()
file(READ "${_lauf}/debug.log" _d)
string(FIND "${_d}" "EXIT_AT: Bild" _p)
if(_p LESS 0)
    message(FATAL_ERROR "r39_johnruf1041[${TEIL}]: Endbild nicht erreicht - Lauf vorher abgerissen (${_lauf})")
endif()
pruef(_d "CONTINUE: resumed in room")

if(TEIL STREQUAL "a")
    pos(_d "loaded room1041.rdt" _a)
    pos(_d "loaded room1061.rdt" _b)
    set(_ok_ab FALSE)
    if(_a GREATER -1 AND _b GREATER _a)
        set(_ok_ab TRUE)
    endif()
    pruef_wahr(_ok_ab "Weg 1021 -> 1041 (echte Tuer) -> 1061 (Montage der Spur Q nach Bit 97)")
    if(NOT _ok_ab)
        message(FATAL_ERROR "r39_johnruf1041[a]: ${_fehler}")
    endif()
    math(EXPR _len "${_b} - ${_a}")
    string(SUBSTRING "${_d}" ${_a} ${_len} _r)
    string(SUBSTRING "${_d}" ${_b} -1 _r61)
    pruef(_r "ROOM1041: John verschwunden ((4,0xF0)=1), Bit 97=0 -> RUF1041 gestartet")
    pos(_r "ROOM1041 Ruf help03 (Sprach-Id 55, Xa_vol 50 -> CD_VOL 6826)" _p1)
    pos(_r "[msgtext] room=1041 id=40 \"Elza: Oh, no, this is John! He is in trouble, I have to help him!\"" _p3)
    pos(_r "[voice] clip loaded: main40.wav" _p3v)
    pos(_r "ROOM1041 Kartenhinweis angefordert (Eintrag" _p4)
    pos(_r "[hint] F" _p5)
    pos(_r "ROOM1041 Karte zu: Faden laeuft weiter" _p6)
    pos(_r "Panik-Musikfenster auf in ROOM1041 (Bit 97 = 1)" _p7)
    pos(_r "[bgm] stage=0 room=04 entry=FF01 -> MAIN01" _p8)
    set(_folge FALSE)
    if(_p1 GREATER -1 AND _p3 GREATER _p1 AND _p3v GREATER _p1 AND _p4 GREATER _p3 AND _p5 GREATER _p4 AND
       _p6 GREATER _p5 AND _p7 GREATER _p6 AND _p8 GREATER _p6)
        set(_folge TRUE)
    endif()
    message(STATUS "     Folge: help03 ${_p1} msg40 ${_p3} main40 ${_p3v} Karte ${_p4} Schirm ${_p5} zu ${_p6} Fenster ${_p7} FF01 ${_p8}")
    pruef_wahr(_folge "Folge leise Rufe -> msg 40 + main40.wav -> Karte -> Schirm zu -> Bit 97 + MAIN01")
    pruef(_r "Kartenhinweis angefordert (Eintrag 5: ROOM1041 -> ROOM1141)")
    pruef(_r "[voice] clip loaded: help03.wav")
    pruef_nicht(_r "ROOM1041 Ruf help02")                   # Nachtrag 17 Punkt 2: nur der erste Ruf
    pruef(_r "[rbj] Animationsblock von ROOM1031 geliehen")  # Nachtrag 17 Punkt 3: Elzas Gesten
    pruef(_r "[voice] Start ROOM1041 Id 40 CD_VOL 16383")
    pruef(_r61 "[bgm] stage=0 room=06 entry=FF01 -> MAIN01(flag 0) SUB--(flags 1/1)  [unveraendert, laeuft durch]")
    # Integration P -> Q: nach Kartenschluss + Bit 97 sieht Spur Q die Flanke und startet ihre Montage (elzazombies_r39.c)
    pos(_r "[elzazombies-r39] Flanke (9,97) 0->1 in ROOM1041" _p9)
    pos(_r "[elzazombies-r39] Montage startet in ROOM1041" _p10)
    set(_montage FALSE)
    if(_p9 GREATER _p6 AND _p6 GREATER -1 AND _p10 GREATER _p9)
        set(_montage TRUE)
    endif()
    pruef_wahr(_montage "nach Kartenschluss + Bit 97: Flanke und Montage der Spur Q (Positionen ${_p9}/${_p10})")
    pruef(_r "ROOM1041: Panikphase (Bit 97) -> Tuer Slot 1 -> Text msg 41")
    pruef_nicht(_d "loaded room1031.rdt")
    if(EXISTS "${_lauf}/karte.bmp")
        file(SIZE "${_lauf}/karte.bmp" _ks)
    else()
        set(_ks 0)
    endif()
    set(_kb FALSE)
    if(_ks GREATER 100000)
        set(_kb TRUE)
    endif()
    pruef_wahr(_kb "Abzug des Hinweis-Schirms karte.bmp (${_ks} B)")
    # Elza dreht sich zur Treppentuer: Blick im STATE_LOG bei msg 40 (Gier 924 = atan2 zur Tuer slot 2 Mitte, +-32)
    file(STRINGS "${_lauf}/state.txt" _st REGEX "msg\\(a=1 fsm=[0-9]+ id=40 ")
    set(_rot -1)
    foreach(_z IN LISTS _st)
        if(_z MATCHES "PL\\(-?[0-9]+,-?[0-9]+,rot=(-?[0-9]+)")
            set(_rot ${CMAKE_MATCH_1})
            break()
        endif()
    endforeach()
    set(_rok FALSE)
    if(_rot GREATER 891 AND _rot LESS 957)
        set(_rok TRUE)
    endif()
    pruef_wahr(_rok "Elza blickt bei msg 40 zur Treppe (Gier ${_rot}, Soll 924 +-32)")
    pegel(_r 1041 55 "STAGE1/room1141/help03.wav" 50)
    pegel(_r 1041 40 "STAGE1/room1041/main40.wav" 120)
else()
    pos(_d "loaded room1131.rdt" _a)
    pos(_d "loaded room1141.rdt" _b)
    pos(_d "loaded room1171.rdt" _c)
    set(_ok_ab FALSE)
    if(_a GREATER -1 AND _b GREATER _a AND _c GREATER _b)
        set(_ok_ab TRUE)
    endif()
    pruef_wahr(_ok_ab "Weg 1121 -> 1131 -> 1141 -> 1171 ueber echte Tueren")
    if(NOT _ok_ab)
        message(FATAL_ERROR "r39_johnruf1041[b]: ${_fehler}")
    endif()
    string(SUBSTRING "${_d}" 0 ${_a} _r21)
    math(EXPR _l1 "${_b} - ${_a}")
    string(SUBSTRING "${_d}" ${_a} ${_l1} _r31)
    math(EXPR _l2 "${_c} - ${_b}")
    string(SUBSTRING "${_d}" ${_b} ${_l2} _r41)
    string(SUBSTRING "${_d}" ${_c} -1 _r71)
    # Laden mitten in der Phase: Raummusik nach dem Laden MAIN01 (der Boot-BGM-Aufruf lief vor dem Restore)
    pruef(_r21 "Panik-Musikfenster auf in ROOM1121 (Bit 97 = 1)")
    pruef(_r21 "[bgm] stage=0 room=12 entry=FF01 -> MAIN01")
    # 1131: Ruf lauter, einmal; Musik laeuft durch
    pruef(_r31 "ROOM1131 in der Panikphase -> RUF1131 gestartet")
    pruef(_r31 "ROOM1131 Ruf help02 (Sprach-Id 54, Xa_vol 100 -> CD_VOL 13652)")
    pruef(_r31 "[bgm] stage=0 room=13 entry=FF01 -> MAIN01(flag 0) SUB--(flags 1/1)  [unveraendert, laeuft durch]")
    string(REGEX MATCHALL "ROOM1131 Ruf help" _rufe "${_r31}")
    list(LENGTH _rufe _nr)
    set(_eins FALSE)
    if(_nr EQUAL 1)
        set(_eins TRUE)
    endif()
    pruef_wahr(_eins "genau ein Ruf in 1131 (${_nr})")
    # 1141: eigene Musik (Spur J unveraendert), Rufe mit Standardlautstaerke
    pruef(_r41 "[bgm] stage=0 room=14 entry=FF56 -> MAIN16")
    # Nachtrag 17 Punkt 4: in 1141 laeuft MAIN01 weiter, das MAIN Play beim Betreten ist gesperrt; erst nach dem Entriegeln
    # der Dachtuer die Tabellen-Musik MAIN16 (Flag 1: still bis Sce_bgm_control op 1 beim Gorilla-Einbruch)
    pos(_r41 "[bgm] stage=0 room=14 entry=FF01 -> MAIN01" _m01)
    pos(_r41 "Sce_bgm_control slot=0 op=1 in der Panikphase NICHT angewandt" _msp)
    pos(_r41 "Dachtuer entriegelt" _mdt)
    pos(_r41 "Raummusik-Anstoss in ROOM1141: Soll Tabelle" _man)
    pos(_r41 "[bgm] stage=0 room=14 entry=FF56 -> MAIN16(flag 1)" _m16)
    set(_m_ok FALSE)
    if(_m01 GREATER -1 AND _msp GREATER _m01 AND _mdt GREATER _msp AND _man GREATER _mdt AND _m16 GREATER _man)
        set(_m_ok TRUE)
    endif()
    pruef_wahr(_m_ok "1141: MAIN01 weiter (${_m01}), MAIN Play gesperrt (${_msp}), Dachtuer (${_mdt}) -> Tabelle (${_man}) -> MAIN16 flag 1 (${_m16})")
    pruef(_r41 "[voice] Start ROOM1141 Id 55 CD_VOL 16383")
    # Rettung: im Bild des Original-Stopps sub01 @0x00C06 (Sce_bgm_control slot=0 op=2 nach dem Kampf)
    pos(_r41 "John gerettet ((4,0xF0)=0, (4,0xF2)=0): Bit 97 = 0, Panikmusik aus" _pr)
    pos(_r41 "Panik-Musikfenster zu in ROOM1141" _pz)
    set(_gerettet FALSE)
    if(_pr GREATER -1 AND _pz GREATER _pr)
        set(_gerettet TRUE)
    endif()
    pruef_wahr(_gerettet "John gerettet -> Bit 97 = 0, Fenster zu (Position ${_pr}/${_pz})")
    set(_nach_kampf FALSE)
    if(_pr GREATER -1)
        string(SUBSTRING "${_r41}" 0 ${_pr} _vor)
        string(FIND "${_vor}" "Sce_bgm_control slot=0 op=2" _lstop REVERSE)
        if(_lstop GREATER -1)
            string(SUBSTRING "${_vor}" ${_lstop} -1 _zw)
            string(FIND "${_zw}" "[msg] room=1141" _mz)
            if(_mz LESS 0)
                set(_nach_kampf TRUE)
            endif()
        endif()
    endif()
    pruef_wahr(_nach_kampf "Bit 97 faellt direkt hinter dem MAIN16-Stopp des Kampfendes (sub01 @0x00C06)")
    # 1171: normale Musik, kein MAIN01
    pruef(_r71 "[bgm] stage=0 room=17 entry=5572 -> MAIN32")
    pruef_nicht(_r71 "entry=FF01")
    pruef_nicht(_r71 "[johnruf]")
    pegel(_r31 1131 54 "STAGE1/room1141/help02.wav" 100)
    pegel(_r41 1141 55 "STAGE1/room1141/help03.wav" 120)
endif()

list(LENGTH _fehler _nf)
if(_nf GREATER 0)
    message(FATAL_ERROR "r39_johnruf1041[${TEIL}]: ${_nf} Fehler: ${_fehler} (${_lauf})")
endif()
message(STATUS "r39_johnruf1041[${TEIL}]: alles gruen (${_lauf})")
