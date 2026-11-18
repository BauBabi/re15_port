cmake_minimum_required(VERSION 3.16)   # cmake -P: Policies wie die uebrigen Integrationsskripte (Linux-Container CMake 3.28)
# =============================================================================
# FINALE 5090/5091: DIE WURFSZENE. Runde 42 Strang finale, Stufe 3 (Plan S5, K-13; include/re15_finale_wurf_r42.h).
#
# NUTZERWUNSCH 07.11.2026: "... Das jemand Leon den Raketenwerfer zuwirft ...". NUTZERENTSCHEID 17.11.2026: "Ok, passt für mich. Aber
# ein Spruch wäre schon gut beim zuwerfen, so etwas wie: Leon, send him to hell, oder Leon, let's finish this oder sowas."
#
# Ein Lauf je Figur (TEIL leon = ROOM5090, elza = ROOM5091): Debug-Sprung nach Wagen B, durch die Tuer nach Wagen C bis zum
# Kampfbeginn (Weg wie integration_r42_finale_schuss), dann OHNE Waffe/Messhaken warten. Pruefhaken RE15_FINALE_UHR=60 kuerzt nur die
# Uhr (Spielwert 1800 prueft unit_r42_finale), RE15_FINALE_WURF_HIN=1 stellt Leon nach der Szene neben den Werfer; das Aufheben laeuft
# ueber den echten Item-Weg (Taste, Ja). Geprueft aus finale.log (Takte = Spieltakt des Finale-Moduls, W = Szenenstart) gegen RE2
# ROOM7020 sub12/sub13:
#   John erscheint W+1 (Bild 0) an der Ankunft (24800,-23550) (Tuer Slot 2 @0x0108E), Zuruf Id 3 W+2 (Bild 1, @0x0188C),
#   Loslassen W+52 = Bild 51 (Sleep 1 + 30 + 20 @0x0187C/@0x01892/@0x018AE) im Clip-Bild 19, Landung mit Se_on Raumbank 0x0E im selben
#   Takt (@0x019DE-E2), Zuruf 71 Bilder (Xa 0 = 354 Sektoren), Reaktion 17 Takte nach dem Spaeteren von Zuruf-Ende und Landung
#   (Wsleep + Do/Edwhile + Sleep 15 @0x018C2-0x018D0), Ende 88 Takte danach (Sleep 1 @0x018FE + Xa-1-Fenster 87 @0x01942),
#   Aufnahme-AOT Slot 6 Gegenstand 0x12 Anzahl 2 Taken-Bit 137, nach dem Aufheben Zellen 0/1 = 0x12 mit Anzahl 2.
#   debug.log: Schnitte 14 (W+1) / 15 (Reaktion) / zurueck auf die gemerkte Kamera (Ende), John aus JOHN_49_WURF.EMD, der Werfer in
#   Hand, Flug und am Boden im Schnitt 14 mit Flaechen gezeichnet.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -DTEIL=leon|elza -P test_r42_finale_wurf.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r42_finale_wurf: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r42_finale_wurf: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL leon)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe_kopie "${_exe_dir}/re15_pc_r42wurf_${TEIL}_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_raum 5090)
set(_figur_env "")
if(TEIL STREQUAL "elza")
    set(_raum 5091)
    set(_figur_env RE15_TITLE_SHOT_CHAR=1)
endif()
re15_start_spiel(_rv 500
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_figur_env}
    RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=500,-9175,2048,0"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=A0.1,W1,U8.5,W7.3,A0.1,W3.5,A0.1,W4"
    RE15_FINALE_LOG=1 RE15_FINALE_UHR=60 RE15_FINALE_WURF_HIN=1 "RE15_EXIT_AT=1000"
    "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")

set(_f "")
if(NOT EXISTS "${WORKDIR}/finale.log" OR NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r42_finale_wurf_${TEIL}: finale.log/debug.log fehlt (exit=${_rv})")
endif()
file(READ "${WORKDIR}/finale.log" _log)
file(READ "${WORKDIR}/debug.log" _dbg)
function(takt_von _muster _var)
    if(_log MATCHES "\\[finale\\] T([0-9]+) F[0-9]+ ${_muster}")
        set(${_var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
    else()
        set(${_var} "" PARENT_SCOPE)
    endif()
endfunction()
takt_von("Wurf: Szene startet" _W)
if(NOT _W)
    message(FATAL_ERROR "r42_finale_wurf_${TEIL}: keine Wurfszene im finale.log (exit=${_rv})\n${_log}")
endif()
foreach(_paar "Wurf: John erscheint [(]Aktor [0-9]+[)] an [(]24800,-23550[)]:1" "Wurf: Zuruf an [(]Id 3[)]:2"
              "Wurf: Loslassen im Clip-Bild 19 [(]Takt W[+]52[)]:52")
    string(REGEX MATCH "^(.*):([0-9]+)$" _x "${_paar}")
    set(_m "${CMAKE_MATCH_1}")
    set(_soll "${CMAKE_MATCH_2}")
    takt_von("${_m}" _t)
    if(NOT _t)
        string(APPEND _f "\n '${_m}' fehlt")
    else()
        math(EXPR _ist "${_t} - ${_W}")
        if(NOT _ist EQUAL _soll)
            string(APPEND _f "\n '${_m}' bei W+${_ist} statt W+${_soll}")
        endif()
    endif()
endforeach()
takt_von("Wurf: Landung nach [0-9]+ Flugbildern" _L)
takt_von("Se_on Raumbank R702 Satz 0x0E [(]PC-Haken 1[)]" _SE)
takt_von("Wurf: Zuruf aus nach 71 Bildern" _Z)
takt_von("Wurf: Reaktion" _R)
takt_von("Wurf: Ende" _E)
if(NOT _L OR NOT _SE OR NOT _Z OR NOT _R OR NOT _E)
    string(APPEND _f "\n Landung '${_L}' / Se_on 0x0E '${_SE}' / Zuruf 71 '${_Z}' / Reaktion '${_R}' / Ende '${_E}' fehlt")
else()
    if(NOT _SE EQUAL _L)
        string(APPEND _f "\n Landeton T${_SE} nicht im Landetakt T${_L} (@0x019E2)")
    endif()
    set(_spaet ${_Z})
    if(_L GREATER _Z)
        set(_spaet ${_L})
    endif()
    math(EXPR _dr "${_R} - ${_spaet}")
    math(EXPR _de "${_E} - ${_R}")
    if(NOT _dr EQUAL 17)
        string(APPEND _f "\n Reaktion ${_dr} Takte nach Zuruf-Ende/Landung statt 17 (Wsleep + Do/Edwhile + Sleep 15)")
    endif()
    if(NOT _de EQUAL 88)
        string(APPEND _f "\n Ende ${_de} Takte nach der Reaktion statt 88 (Sleep 1 + 87)")
    endif()
endif()
foreach(_m "Aufnahme-AOT Slot 6: Typ 2" "Aufnahme-AOT: Gegenstand 0x12 Anzahl 2 Taken-Bit 137" "aufgehoben: Zellen 0/1 = 0x12"
           "aufgehoben: Anzahl 2/2")
    if(NOT _log MATCHES "${_m}")
        string(APPEND _f "\n '${_m}' fehlt (Aufnahme)")
    endif()
endforeach()
# debug.log: Schnitte, Modell, Werfer gezeichnet
math(EXPR _w1 "${_W} + 1")
if(NOT _dbg MATCHES "\\[scd F${_w1}\\] Cut_chg\\(14\\)")
    string(APPEND _f "\n Cut_chg(14) nicht im Bild 0 (F${_w1})")
endif()
if(_R AND NOT _dbg MATCHES "\\[scd F${_R}\\] Cut_chg\\(15\\)")
    string(APPEND _f "\n Cut_chg(15) nicht im Reaktionsbild F${_R}")
endif()
if(_log MATCHES "Kamera vorher ([0-9]+)")
    set(_vorher ${CMAKE_MATCH_1})
    if(_E AND NOT _dbg MATCHES "\\[scd F${_E}\\] Cut_chg\\(${_vorher}\\)")
        string(APPEND _f "\n Rueckschnitt auf ${_vorher} nicht im Endbild F${_E}")
    endif()
endif()
if(NOT _dbg MATCHES "EM49: Ersatzmodell RE2/JOHN_49_WURF.EMD \\(Raum ${_raum}\\)")
    string(APPEND _f "\n John nicht aus JOHN_49_WURF.EMD")
endif()
foreach(_z 1 2 3)
    if(NOT _dbg MATCHES "Werfer gezeichnet: Zustand ${_z} Lage [^\n]* Cut 14 Flaechen [1-9]")
        string(APPEND _f "\n Werfer im Zustand ${_z} nicht im Schnitt 14 gezeichnet")
    endif()
endforeach()
if(_log MATCHES "Ausloeser: Abschuss")
    string(APPEND _f "\n Abschuss ausgeloest (ohne Werfer in der Hand darf das nicht sein)")
endif()
if(_f)
    message(FATAL_ERROR "r42_finale_wurf_${TEIL}: FEHLER${_f}\n--- finale.log ---\n${_log}")
endif()
message(STATUS "r42_finale_wurf_${TEIL}: OK (W ${_W}, Landung T${_L}, Reaktion T${_R}, Ende T${_E})")
