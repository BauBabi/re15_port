cmake_minimum_required(VERSION 3.16)
# =============================================================================
# "I HAVE TO GET THE CHIEF FIRST..." AUCH AN DER TUER ROOM1050 -> ROOM10A0 — ECHTE EXE (Runde 37, Spur F).
#
# Nutzer: "man kann über die Luke bis nach F1 und kann den "Ich muss zuerst irons holen" Blocker bei der Tür
# überspringen. [...] du machst den Text zusätzlich noch in ROOM 1050 bei der Tür, wo wir auch die Ada Cutscene
# triggern". Und (#5) "Bezug zu ada bei elza kann raus".
# Dossier analysis/befunde_runde37/F_sperre1050.md, Konstanten include/re15_sperre1050_r37.h.
#
# Laeufe (Endspiel-Zustand per RE15_SET_FLAG: (9,71) 10F0-Szene, (3,94) erste Irons-Szene, (3,0xBB) Ada gerettet,
# (3,121) Rolltor offen, (3,129) Feuer aus; (9,73)=0):
#   A  LUKE, Ende-zu-Ende in EINEM Lauf: ROOM10F0 Tritt schieben + hochklettern, Lueftungsgitter (Slot 1 @0x00F52) ->
#      ROOM1090 Laufsteg Band 5 -> ABSPRUNG (Zelle @0x01E3C, sub=9 kind=0x0E) -> Kiste -> Hof -> Tuer @0x0211A ->
#      ROOM1050 -> Slot 4: Text msg 26, KEIN ROOM10A0. (Vor dem Bau oeffnete hier die Tuer nach 10A0,
#      F_belege/r7_e2e_vorher.txt.) Durchgehende Bild-Zeitachse (kein RE15_INPUT_SCRIPT_BASIS).
#   E  ENDSPIEL direkt: Sprung an den 1090-Eingang von ROOM1050, derselbe Weg zu Slot 4 -> Text msg 26.
#   B  MITTELSPIEL ((9,71)=0): derselbe Weg -> ROOM10A0 (Original-Tuer bleibt).
#   C  NACH IRONS ((9,73)=1): derselbe Weg -> ROOM10A0.
#   D  ELZA ROOM50B1: an Adas Leiche (Slot 2 sce 3 -> sub02 Message_on 0) -> "It's a woman. She's dead." ...
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r37_sperre1050.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_sperre1050: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r37_sperre1050: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name (Muster test_r34n_b_cursor.cmake): local_build.sh beendet sonst jede re15_pc.exe der Maschine.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37f_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r37f_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

set(_fehler "")
macro(befund _text)
    message(STATUS "r37_sperre1050: FEHLER ${_text}")
    set(_fehler "${_fehler}\n  ${_text}")
endmacro()

set(_endspiel "9:71,3:94,3:187,3:121,3:129")
# ROOM1050 vom 1090-Eingang (19850,-22300) Blick +Z bis vor Slot 4 (16732,-13750) Blick +X, Aktionstaste
# (gemessen r6: Westflur bis x 16568, Nordflur bis z -13705).
set(_weg1050 "L0.37,W0.3,U2.0,W0.3,R0.37,W0.3,U3.9,W0.3,R0.37,W0.3,U0.5,W0.3,A0.1,W5")
# ROOM10F0: Tritt schieben + hochklettern + Gitter (test_r36_klettern.cmake Lauf A), vorne 18,4 s bis Bild ~300.
set(_weg10f0 "W18.4,U2.6,W1,L0.37,W0.3,U0.5,W0.3,R0.37,W0.3,U0.55,W0.3,R0.37,W0.5,U10.5,W1,A0.1,W2.5,U0.6,W0.3,A0.1,W3")
# ROOM1090: vom Gitter-Spawn (-13600,1300) Blick +Z an den Nordrand bei x -9900, Absprung, von der Kiste nach +X,
# Hof, umdrehen, Hof-Tuer.
set(_weg1090 "U1.0,W0.3,R0.37,W0.3,U1.67,W0.3,L0.37,W0.3,U0.6,W0.5,A0.1,W3,R0.37,W0.3,U0.3,W0.3,A0.1,W2,R0.37,W0.3,A0.1,W3")

# lauf(<name> <timeout> <env...>) -> liest debug.log / climb.txt nach _log_<name> / _climb_<name>
function(lauf _name _timeout)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/debug.log" "${WORKDIR}/climb.txt")
    re15_start_spiel(_rv ${_timeout}
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
        RE15_MSG_LOG=1 RE15_CLIMB_TRACE=climb.txt ${ARGN}
        "${RE15_PC_EXE}")
    set(_log "")
    set(_climb "")
    if(EXISTS "${WORKDIR}/debug.log")
        file(STRINGS "${WORKDIR}/debug.log" _log)
    endif()
    if(EXISTS "${WORKDIR}/climb.txt")
        file(STRINGS "${WORKDIR}/climb.txt" _climb REGEX "sub=9 kind=")
    endif()
    set(_log_${_name} "${_log}" PARENT_SCOPE)
    set(_climb_${_name} "${_climb}" PARENT_SCOPE)
    set(_rv_${_name} "${_rv}" PARENT_SCOPE)
endfunction()

# Zaehlt Zeilen in _liste, die _regex treffen.
function(zaehle _liste _regex _out)
    set(_n 0)
    foreach(_z IN LISTS ${_liste})
        if(_z MATCHES "${_regex}")
            math(EXPR _n "${_n} + 1")
        endif()
    endforeach()
    set(${_out} ${_n} PARENT_SCOPE)
endfunction()

# ---- A: LUKE Ende-zu-Ende ------------------------------------------------------------------------------------
lauf(A_luke 420 RE15_DEBUG_JUMP=10f0@250 "RE15_SET_FLAG=${_endspiel}" "RE15_PLAYER_POS=-7125,4950,0"
     RE15_INPUT_SCRIPT_START=0 "RE15_INPUT_SCRIPT=${_weg10f0},${_weg1090},${_weg1050}" "RE15_EXIT_AT=700#1050")
zaehle(_log_A_luke "DOOR FIRE slot=1 .*spawn=\\(-13600,-9000,1300\\)" _a_gitter)
zaehle(_climb_A_luke "room=1090 .* band=5 sub=9 kind=0x0E" _a_absprung)
zaehle(_log_A_luke "DOOR FIRE slot=0 rect=\\(-10080,3060,.*spawn=\\(19850,0,-22300\\)" _a_hof)
zaehle(_log_A_luke "\\[sperre1050\\] ROOM1050 Slot 4 -> Text-Platz msg 26" _a_sperre)
zaehle(_log_A_luke "\\[msgtext\\] room=1050 id=26 \"I have to get the Chief first\\.\\.\\.\"" _a_text)
zaehle(_log_A_luke "PC loaded room10a0" _a_10a0)
zaehle(_log_A_luke "EXIT_AT: Bild [0-9]+ in Raum 1050 erreicht" _a_ende)
message(STATUS "r37_sperre1050: [A_luke] Gitter=${_a_gitter} Absprung=${_a_absprung} Hof-Tuer=${_a_hof} Sperre=${_a_sperre} Text=${_a_text} 10A0=${_a_10a0} Ende1050=${_a_ende} (exit=${_rv_A_luke})")
if(_a_gitter LESS 1)
    befund("[A_luke] Lueftungsgitter ROOM10F0 Slot 1 (@0x00F52) nicht genommen")
endif()
if(_a_absprung LESS 1)
    befund("[A_luke] kein Absprung vom Laufsteg (climb: room=1090 band=5 sub=9 kind=0x0E, Zelle @0x01E3C)")
endif()
if(_a_hof LESS 1)
    befund("[A_luke] Hof-Tuer ROOM1090 @0x0211A -> ROOM1050 nicht genommen")
endif()
if(_a_sperre LESS 1 OR _a_text LESS 1)
    befund("[A_luke] Slot 4 ohne Text-Platz/Text msg 26 (Sperre=${_a_sperre} Text=${_a_text})")
endif()
if(_a_10a0 GREATER 0)
    befund("[A_luke] ROOM10A0 wurde geladen — die Luecke ist offen")
endif()
if(_a_ende LESS 1)
    befund("[A_luke] Lauf endete nicht in ROOM1050")
endif()

# ---- E / B / C: derselbe 1050-Weg vom 1090-Eingang ---------------------------------------------------------------
foreach(_fall E_endspiel B_mittelspiel C_nach_irons)
    if(_fall STREQUAL "E_endspiel")
        set(_fl "${_endspiel}")
        set(_ende "500#1050")
    elseif(_fall STREQUAL "B_mittelspiel")
        set(_fl "3:94,3:187,3:121,3:129")
        set(_ende "150#10a0")
    else()
        set(_fl "${_endspiel},9:73")
        set(_ende "150#10a0")
    endif()
    lauf(${_fall} 300 RE15_DEBUG_JUMP=1050@250 "RE15_SET_FLAG=${_fl}" "RE15_PLAYER_POS=19850,-22300,3072,0"
         RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=W3,${_weg1050}" "RE15_EXIT_AT=${_ende}")
    zaehle(_log_${_fall} "\\[sperre1050\\] ROOM1050 Slot 4 -> Text-Platz msg 26" _s)
    zaehle(_log_${_fall} "\\[msgtext\\] room=1050 id=26 \"I have to get the Chief first\\.\\.\\.\"" _t)
    zaehle(_log_${_fall} "DOOR FIRE slot=4 rect=\\(17200,-13700," _d)
    zaehle(_log_${_fall} "PC loaded room10a0" _r)
    message(STATUS "r37_sperre1050: [${_fall}] Sperre=${_s} Text=${_t} Tuer4=${_d} 10A0=${_r} (exit=${_rv_${_fall}})")
    if(_fall STREQUAL "E_endspiel")
        if(_s LESS 1 OR _t LESS 1 OR _d GREATER 0 OR _r GREATER 0)
            befund("[E_endspiel] erwartet Text msg 26 ohne Tuer: Sperre=${_s} Text=${_t} Tuer4=${_d} 10A0=${_r}")
        endif()
    else()
        if(_s GREATER 0 OR _t GREATER 0 OR _d LESS 1 OR _r LESS 1)
            befund("[${_fall}] erwartet Original-Tuer -> ROOM10A0: Sperre=${_s} Text=${_t} Tuer4=${_d} 10A0=${_r}")
        endif()
    endif()
endforeach()

# ---- D: ELZA ROOM50B1 ----------------------------------------------------------------------------------------
# Standplatz westlich der Leiche (obj 0 @ (-11800,-11600), sub00 @0x004F2), Blick +X: der Vorwaerts-620-Punkt
# (-11880,-11600) liegt im Ereignis-Platz Slot 2 (sub00 @0x004DE, Rechteck (-12400,-12100,1000,1000)) -> sub02
# Message_on 0 @0x0052E.
lauf(D_elza_50b1 200 RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=50b0@250 "RE15_PLAYER_POS=-12500,-11600,0,0"
     RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=W3,A0.1,W5,A0.1,W2,A0.1,W3"
     "RE15_EXIT_AT=500#50b1")
zaehle(_log_D_elza_50b1 "\\[pl\\] Spieler-Familie PL04" _d_elza)
zaehle(_log_D_elza_50b1 "\\[msgtext\\] room=50b1 id=0 \"It's a woman\\. She's dead\\.It looks like one of those gorillas killed her\\.\"" _d_neu)
zaehle(_log_D_elza_50b1 "\\[msgtext\\] room=50b1 .*Ada" _d_ada)
message(STATUS "r37_sperre1050: [D_elza_50b1] Elza=${_d_elza} neuerText=${_d_neu} Ada=${_d_ada} (exit=${_rv_D_elza_50b1})")
if(_d_elza LESS 1)
    befund("[D_elza_50b1] kein '[pl] Spieler-Familie PL04' — Elza nicht im Spiel")
endif()
if(_d_neu LESS 1 OR _d_ada GREATER 0)
    befund("[D_elza_50b1] ROOM50B1 msg 0 nicht der neue Text (neu=${_d_neu}, mit Ada=${_d_ada})")
endif()

file(REMOVE "${_exe_kopie}")
if(NOT _fehler STREQUAL "")
    message(FATAL_ERROR "r37_sperre1050: ${_fehler}")
endif()
message(STATUS "r37_sperre1050: alle Laeufe gruen")
