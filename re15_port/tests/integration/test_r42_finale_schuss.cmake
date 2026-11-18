cmake_minimum_required(VERSION 3.16)   # cmake -P: Policies wie die uebrigen Integrationsskripte (Linux-Container CMake 3.28)
# =============================================================================
# FINALE LEON 5090: ABSCHUSS BIS TITEL. Runde 42 Strang finale, Stufe 1 (include/re15_finale_r42.h).
#
# NUTZERWUNSCH 07.11.2026: "... die Form Birkin G5 aus dem Finale wieder entfernst ... ein finale wie beim Golem am Ende von Resident
# Evil 2 ... mit mehreren Wiederholungen ... wie die Rakete angeflogen kommen und den Gegner zersprengt." NUTZERWUNSCH 17.11.2026:
# Schlusszeile "Game over." (K-11).
#
# Ein Lauf: Debug-Sprung 5090 (wie integration_r41_g5_tod), Pruefhaken RE15_FINALE_WERFER=1 (2 Raketen, ausgeruestet - die Wurfszene
# folgt in Stufe 3), Anlegen + Feuer. Geprueft aus finale.log (Takte T = Spieltakt des Finale-Moduls) gegen die RE2-Zahlen:
#   Ausloeser T, Freigabe U = T + 15 + 33 + 1 (sub15 @0x01A30 Sleep 15, Lesefenster 162 Sektoren = 33 Bilder, Programm ab T+1)
#   echte Rakete weg U+7 (@0x80103750-68), W1 U+37 (Sleep 32 + 5 @0x01A60/64), Rakete U+38, Einfrieren U+52 (t 15 @0x80103848),
#   W2 U+77 (40 Bilder @0x801038a4), Rakete U+78, Einfrieren U+84 (t 7), W3 U+109 (32 @0x801039a8), Ruetteln U+118 (10 @0x80103a80),
#   Einschlag U+119, Wackeln aus U+134 (Bild 16 @0x80103d24), Kill = Einschlag + 206 (Xa 0x03 1026 Sektoren = 6,84 s), Phase 9 +1.
#   Danach: Abspann-Raum 6040 und Titel (zweiter Spiel-Boot, RE15_BOOT_EXIT_AT=2), kein [g5a]/G5-Auftritt im ganzen Lauf.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r42_finale_schuss.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r42_finale_schuss: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r42_finale_schuss: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r42finale_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r42finale_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

re15_start_spiel(_rv 800
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=5090@250 "RE15_PLAYER_POS=500,-9175,2048,0"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=A0.1,W1,U8.5,MA6,W60"
    RE15_FINALE_WERFER=1 RE15_FINALE_LOG=1 RE15_BOOT_EXIT_AT=2 "RE15_EXIT_AT=7000"
    "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")

set(_f "")
if(NOT EXISTS "${WORKDIR}/finale.log")
    message(FATAL_ERROR "r42_finale_schuss: finale.log fehlt (exit=${_rv}) - kein Finale-Modul oder kein Kampfbeginn")
endif()
file(READ "${WORKDIR}/finale.log" _log)
function(takt_von _muster _var)
    if(_log MATCHES "\\[finale\\] T([0-9]+) F[0-9]+ ${_muster}")
        set(${_var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
    else()
        set(${_var} "" PARENT_SCOPE)
    endif()
endfunction()
takt_von("Ausloeser" _T)
takt_von("Freigabe" _U)
if(NOT _T OR NOT _U)
    message(FATAL_ERROR "r42_finale_schuss: kein Ausloeser/keine Freigabe im finale.log (exit=${_rv})\n${_log}")
endif()
math(EXPR _d "${_U} - ${_T}")
if(NOT _d EQUAL 49)
    string(APPEND _f "\n U - T = ${_d} statt 49 (Sleep 15 + Lesefenster 33 + Programmstart T+1)")
endif()
foreach(_paar "echte Rakete geloescht:7" "W1:.*vor der Form:37" "Wiederholung 020d1000 Besitzer 0x80:38" "W1 eingefroren:52"
              "W2:.*vor der Form:77" "Wiederholung 020d1000 Besitzer 0x81:78" "W2 eingefroren:84" "W3:.*vor der Form:109"
              "Ruetteln:118" "Einschlag:119" "Wackeln aus:134" "Kill:325" "Phase 9:326")
    string(REGEX MATCH "^(.*):([0-9]+)$" _x "${_paar}")
    set(_m "${CMAKE_MATCH_1}")
    set(_soll "${CMAKE_MATCH_2}")
    takt_von("${_m}" _t)
    if(NOT _t)
        string(APPEND _f "\n '${_m}' fehlt")
    else()
        math(EXPR _ist "${_t} - ${_U}")
        if(NOT _ist EQUAL _soll)
            string(APPEND _f "\n '${_m}' bei U+${_ist} statt U+${_soll}")
        endif()
    endif()
endforeach()
if(NOT _log MATCHES "echte Rakete geloescht \\([1-9][0-9]* Plaetze\\)")
    string(APPEND _f "\n der echte Schuss im Freigabebild U hat keine Rakete (2,0x0D) erzeugt (RE2: Rakete ab Clip-Bild 1, @0x8004559c)")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
if(_dbg MATCHES "\\[g5a\\]")
    string(APPEND _f "\n [g5a]-Zeile im Lauf: der G5-Auftritt lief")
endif()
if(NOT _dbg MATCHES "PC loaded room6040")
    string(APPEND _f "\n Abspann-Raum 6040 nicht geladen")
endif()
if(NOT _dbg MATCHES "Set\\(0,1,0\\): INGAME aus -> Titel")
    string(APPEND _f "\n Diashow-Ende Set(0,1,0) nicht gesehen")
endif()
if(NOT _dbg MATCHES "BOOT_EXIT_AT: boot #2")
    string(APPEND _f "\n Titel nicht erreicht (kein zweiter Spiel-Boot, exit=${_rv})")
endif()
message(STATUS "r42_finale_schuss: T=${_T} U=${_U} exit=${_rv}")
if(_f)
    message(FATAL_ERROR "r42_finale_schuss:${_f}\n--- finale.log ---\n${_log}")
endif()
