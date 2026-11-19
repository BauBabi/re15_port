# =============================================================================
# Runde 41 Integration A2, Gegenpruefung Welle 2 Befund 3: ROOM5090 mit den AN-Bits WIE IM ECHTEN WEG - (3,89) Tor (ROOM6000
# sub07 @0x011B4 "22 03 59 01") und (9,133) Zugstart 6030 gesehen (RE15_AN_ZUG_BIT, annette_r41.c) - durchgehend:
#   Annette-Szene vor sub04 (RE2 ROOM6141 sub02, re15_annette_r41_evt_exec statt sub01 @0x01296 Evt_exec sub04)
#   -> Riesenform-Kampf (BAHNSTEIG_FORM K5c, HP 1000 = RE2-G4-Ctor "addiu v0,zero,1000" @0x80100678)
#   -> Rueckstoss-Szene (st 2, Clip 13, HP 1) -> Sturz aus dem Wagen (Clip 12) -> G5-Auftritt (Typ 0x36, RE2 room7040 sub06)
#   Annette bleibt danach als Leiche liegen (RE2-Liegeclip 0x18 = Clip 24 Bild 0, hp -1 = Begleiter-Regel NB5).
# Weg und Waffe wie integration_r41_g5_tod (Debug-Sprung Wagen B, Tuer 2, Rakete RE15_GIVE 18:200), das Feuer beginnt erst
# nach dem Kampfstart: W16.7 deckt die AN-Szene (gemessen Lauf b3_leon: Szenenstart F280, Kampfstart F888 = 608 Bilder).
# Gemessen (Lauf b3_leon, Integrationsbaum nach Integration A2, Renderer direct3d): Raketen F949 699, F980 398, F1012 97,
# F1044 -204 (wie g5_tod ohne AN: 699/398/97/-204), Rueckstoss F1045 (Clip 13, HP 1), Clip 8 F1107, Sturz Clip 12 F1135,
# G5-Vorlauf F1192, Armierung F1247, Szenenende F2409; Spieler-HP 100 bis F2409; exit 0, kein Absturz.
# KEIN RE15_SOFTWARE_RENDER: mit dem K5c-Asset stuerzt der Software-Renderer in SDL_BlitTriangle_Slow ab
# (analysis/befunde_runde41/form/integration_a2/pistole_absturz_k5c.txt, offen bei Spur FORM). Linux-Lauf steht aus.
#
#
# RUNDE 42 STRANG FINALE, Stufe 1 (Plan S4/S6/S7, NUTZERWUNSCH 07.11.2026 "die Form Birkin G5 aus dem Finale wieder entfernst ... ein
# finale wie beim Golem am Ende von Resident Evil 2"): nach der AN-Szene traegt die Form die Tyrant-HP 20000 + 200 (@0x80101be0) und
# stirbt nur durch den Abschuss mit dem Raketenwerfer (Ausloeser @0x80101904-0x80101a00, Sequenz finale_r42.c), danach Abspann 6040.
# Umgestellt (Name bleibt): statt Tod -> Rueckstoss -> Sturz -> G5 jetzt Kampfstart -> Abschuss -> Kill -> ROOM6040; kein [g5auftritt];
# Spieler-HP > 0 bis zum Kill; Annette liegt am Ende als Leiche wie bisher.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_annette_g5.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_annette_g5: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_annette_g5: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41ang5_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41ang5_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

re15_start_spiel(_rv 600
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=5090@250 "RE15_PLAYER_POS=500,-9175,2048,0"
    RE15_SET_FLAG=3:89,9:133
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=A0.1,W1,U8.5,W16.7,MA13,W80"
    RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log RE15_GIVE=18:200 RE15_EQUIP=18 RE15_FINALE_LOG=1
    "RE15_EXIT_AT=2600"
    "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")

set(_f "")
if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.log")
    message(FATAL_ERROR "r41_annette_g5: kein Protokoll (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
if(NOT _dbg MATCHES "EXIT_AT: Bild")
    string(APPEND _f "\n Endbild 2600 nicht erreicht (exit=${_rv}) - Absturz oder Haenger")
endif()
if(NOT _dbg MATCHES "Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips")
    string(APPEND _f "\n Riesenform nicht geladen (BAHNSTEIG_FORM.EMD)")
endif()
# 1 AN-Szene in RE2-Reihenfolge, Kampfstart sub04 zuletzt
set(_pos 0)
set(_bild_kampf -1)
foreach(_schritt "Szenenstart statt sub04" "Sturz:" "gelandet, Drohen:" "Hieb \\(RE2" "Treffer:" "Schnitt weg, liegt, Schrei"
                 "Kampfstart \\(sub04\\), Form frei")
    string(REGEX MATCH "\\[annette\\] F([0-9]+) ${_schritt}" _m "${_dbg}")
    if(NOT _m)
        string(APPEND _f "\n AN-Schritt '${_schritt}' fehlt")
        continue()
    endif()
    set(_bild_kampf "${CMAKE_MATCH_1}")
    string(FIND "${_dbg}" "${_m}" _p)
    if(_p LESS _pos)
        string(APPEND _f "\n AN-Schritt '${_schritt}' in falscher Reihenfolge")
    endif()
    set(_pos ${_p})
endforeach()
# 2 Finale statt G5 (finale.log)
set(_abschuss -1)
set(_kill -1)
if(EXISTS "${WORKDIR}/finale.log")
    file(READ "${WORKDIR}/finale.log" _fin)
    if(_fin MATCHES "\\[finale\\] T[0-9]+ F([0-9]+) Ausloeser")
        set(_abschuss "${CMAKE_MATCH_1}")
    endif()
    if(_fin MATCHES "\\[finale\\] T[0-9]+ F([0-9]+) Kill")
        set(_kill "${CMAKE_MATCH_1}")
    endif()
endif()
if(_abschuss LESS 0 OR NOT _abschuss GREATER _bild_kampf)
    string(APPEND _f "\n kein Abschuss nach dem Kampfstart (Abschuss F${_abschuss}, Kampfstart F${_bild_kampf})")
endif()
if(_kill LESS 0 OR NOT _kill GREATER _abschuss)
    string(APPEND _f "\n kein Kill nach dem Abschuss (Kill F${_kill})")
endif()
if(_dbg MATCHES "\\[g5auftritt\\]")
    string(APPEND _f "\n G5-Auftritt lief (Plan S4: entfallen)")
endif()
if(NOT _dbg MATCHES "PC loaded room6040")
    string(APPEND _f "\n Abspann-Raum 6040 nach dem Kill nicht geladen")
endif()
# 3 Zustaende aus state.log (bis zum Kill = Ende des Finales in 5090)
file(STRINGS "${WORKDIR}/state.log" _zeilen REGEX "^F[0-9]+ ")
set(_hp_vor_kampf -1)
set(_pl_min 100)
set(_frau "")
set(_rueck -1)
foreach(_z IN LISTS _zeilen)
    string(REGEX MATCH "^F([0-9]+) " _m "${_z}")
    set(_b "${CMAKE_MATCH_1}")
    if(_kill GREATER 0 AND _b GREATER _kill)
        break()
    endif()
    if(_z MATCHES "PL\\(-?[0-9]+,-?[0-9]+,rot=-?[0-9]+,hp=(-?[0-9]+)\\)")
        if(CMAKE_MATCH_1 LESS _pl_min)
            set(_pl_min "${CMAKE_MATCH_1}")
        endif()
    endif()
    if(_z MATCHES "(\\[[0-9]+ t=4d [^]]*\\] hp=-?[0-9]+)")
        set(_frau "${CMAKE_MATCH_1}")
    endif()
    if(NOT _z MATCHES "t=30 st=([0-9]+) ss1=[0-9]+ ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=([0-9]+) [^]]*\\] hp=(-?[0-9]+)")
        continue()
    endif()
    set(_st "${CMAKE_MATCH_1}")
    set(_mo "${CMAKE_MATCH_2}")
    set(_hp "${CMAKE_MATCH_3}")
    if(_bild_kampf GREATER 0 AND _b LESS _bild_kampf)
        set(_hp_vor_kampf "${_hp}")
    endif()
    if(_rueck LESS 0 AND _mo EQUAL 13 AND _hp EQUAL 1)
        set(_rueck "${_b}")
    endif()
endforeach()
if(NOT _hp_vor_kampf EQUAL 20200)
    string(APPEND _f "\n Riesenform vor dem Kampfstart mit HP ${_hp_vor_kampf}, erwartet 20200 (Tyrant @0x80101be0)")
endif()
if(_rueck GREATER 0)
    string(APPEND _f "\n Rueckstoss-Szene (Clip 13, HP 1) F${_rueck} lief (Plan S4: entfallen)")
endif()
if(NOT _pl_min GREATER 0)
    string(APPEND _f "\n Spieler-HP ${_pl_min} bis zum Kill")
endif()
# Strang finale Stufe 6: die Leiche liegt, wo der Hieb sie traf (Ankunft an S, in diesem Lauf (3773,-22529)), Gier 1695
if(NOT _frau MATCHES "mo=24 af=0 " OR NOT _frau MATCHES "hp=-1$" OR NOT _frau MATCHES "@\\(3773,-22529,r1695\\)")
    string(APPEND _f "\n Annette am Ende nicht als Leiche (Clip 24 Bild 0, (3773,-22529) Gier 1695, hp -1): ${_frau}")
endif()
message(STATUS "r41_annette_g5 (Finale): Kampfstart F${_bild_kampf}, Form ${_hp_vor_kampf}, Abschuss F${_abschuss}, Kill F${_kill}, "
               "Spieler-HP min ${_pl_min}, Leiche ${_frau}, exit ${_rv}")
if(_f)
    message(FATAL_ERROR "r41_annette_g5: FEHLER${_f}")
endif()
message(STATUS "r41_annette_g5: alle Pruefungen gruen")
