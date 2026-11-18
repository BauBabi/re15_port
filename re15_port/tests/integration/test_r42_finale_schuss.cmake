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
#   Einschlag U+119, Wackeln aus U+134 (Bild 16 @0x80103d24), Kill = Einschlag + 83 (Xa 0x03 1026 Sektoren = 6,84 s ECHTZEIT: Bit 0x20
#   @0x80103d64-74, VSync 5 @0x80103b00-08 = 5 Austastungen je Bild, 410,4 Austastungen -> 83. Bild; Pruefbericht s1 r1 R1), Phase 9 +1.
#   Schnitte (U3 Runde 3, include/re15_finale_r42.h): Ausloeser 12, W1 13, W2 14, W3 13. Bildlage (Runde 3, Pruefbericht s1 r2 R1) mit der
#   Kamera des Schnitts: "Game over" Form ausser Bild und Spieler im Bild (RE2 Schnitt 11); W3 Effekt in jedem Bild im Bild und naeher
#   an der Kamera als die Form (RE2 1181 vor 4112); W1/W2 Rakete in mindestens der Haelfte der Flugbilder im Bild. Einschlag: Raumtonbank
#   R702 Satz 0x0F (Se_on 0x020F0001 @0x80103b54-5c) im Einschlagbild, Bank geladen. Zerplatzen (U5): Bild 16 loescht die Brocken und setzt
#   sechs 0x040E2000 (Marken der Tabelle @0x8010476c). VSync 5 gilt genau vom Einschlag bis Phase 9 (84 Bilder im debug.log).
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
              "Ruetteln:118" "Einschlag:119" "Se_on 0x020F0001. Raumbank R702 Satz 0x0F .PC-Haken 1.:119" "Wackeln aus:134" "Bild 16: Brocken weg, 6 x 0x040E2000:134" "Kill:202" "Phase 9:203")
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
# [^\n] statt .: der CMake-Punkt passt auch auf den Zeilenumbruch (Runde 3: "Ausloeser: .*Schnitt 12" fand "W1 ... Schnitt 12" der Runde 2)
foreach(_paar "Ausloeser: [^\n]*Schnitt 12" "W1: Spieler 10000 vor der Form, Schnitt 13" "W2: Spieler 5000 vor der Form, Schnitt 14"
              "W3: Spieler 3000 vor der Form, Schnitt 13" "Kill: [^\n]*Xa-Fenster 415 Austastungen")
    if(NOT _log MATCHES "${_paar}")
        string(APPEND _f "\n '${_paar}' fehlt (U3-Schnitte / R1-Echtzeit)")
    endif()
endforeach()
# Bildlage (Runde 3): "Game over" - Form ausser Bild (u ausserhalb 0..319 oder hinter dem Auge), Spieler im Bild.
if(_log MATCHES "Bildlage Game over: Form u (-?[0-9]+) Tiefe (-?[0-9]+)")
    set(_u "${CMAKE_MATCH_1}")
    if(_u GREATER_EQUAL 0 AND _u LESS 320)
        string(APPEND _f "\n Game over: Formmitte im Bild (u ${_u}), RE2 Schnitt 11 hat die Form ausser Bild")
    endif()
else()
    string(APPEND _f "\n Bildlage Game over (Form) fehlt")
endif()
if(_log MATCHES "Bildlage Game over: Spieler u (-?[0-9]+) Tiefe")
    if(CMAKE_MATCH_1 LESS 0 OR CMAKE_MATCH_1 GREATER_EQUAL 320)
        string(APPEND _f "\n Game over: Spieler ausser Bild (u ${CMAKE_MATCH_1})")
    endif()
else()
    string(APPEND _f "\n Bildlage Game over (Spieler) fehlt")
endif()
# W1/W2/W3: je Flugbild "Bildlage Rakete W<n> u <u> Tiefe <t>" und direkt danach "Bildlage Form W<n> u <u> Tiefe <t>".
string(REGEX MATCHALL "Bildlage (Rakete|Form) W[123] u -?[0-9]+ Tiefe -?[0-9]+" _bl "${_log}")
set(_w3 0)
set(_w3_vorn 0)
set(_w12 0)
set(_w12_im 0)
set(_ru "")
foreach(_z IN LISTS _bl)
    string(REGEX MATCH "Bildlage (Rakete|Form) W([123]) u (-?[0-9]+) Tiefe (-?[0-9]+)" _x "${_z}")
    if(CMAKE_MATCH_1 STREQUAL "Rakete")
        set(_ru "${CMAKE_MATCH_3}")
        set(_rt "${CMAKE_MATCH_4}")
        set(_rw "${CMAKE_MATCH_2}")
    elseif(NOT _ru STREQUAL "" AND CMAKE_MATCH_2 STREQUAL _rw)
        set(_ft "${CMAKE_MATCH_4}")
        set(_im 0)
        if(_ru GREATER_EQUAL 0 AND _ru LESS 320 AND _rt GREATER 0)
            set(_im 1)
        endif()
        if(_rw EQUAL 3)
            math(EXPR _w3 "${_w3} + 1")
            if(_im AND _rt LESS _ft)
                math(EXPR _w3_vorn "${_w3_vorn} + 1")
            endif()
        else()
            math(EXPR _w12 "${_w12} + 1")
            if(_im)
                math(EXPR _w12_im "${_w12_im} + 1")
            endif()
        endif()
        set(_ru "")
    endif()
endforeach()
# W3 dauert 10 Bilder (`addiu v1,zero,0xa` @0x80103a80), der Effekt entsteht im Bild t = 1 (`addiu v0,zero,1 / bne` @0x80103a48-4c);
# gemessen wird ab dem Folgebild bis zum Phasenwechsel -> 8 Bilder.
if(_w3 LESS 8 OR NOT _w3_vorn EQUAL _w3)
    string(APPEND _f "\n W3: Effekt ${_w3_vorn} von ${_w3} Bildern im Bild und vor der Form (erwartet alle, mindestens 8)")
endif()
math(EXPR _halb "${_w12} / 2")
if(_w12 LESS 10 OR _w12_im LESS _halb)
    string(APPEND _f "\n W1/W2: Rakete nur ${_w12_im} von ${_w12} Flugbildern im Bild")
endif()
message(STATUS "r42_finale_schuss: Bildlage W3 ${_w3_vorn}/${_w3} vorn, W1/W2 ${_w12_im}/${_w12} im Bild")
file(READ "${WORKDIR}/debug.log" _dbg)
if(NOT _dbg MATCHES "\\[finale-se\\] Raumbank R702 \\(RE2 ROOM7020 snd0\\) geladen")
    string(APPEND _f "\n Raumtonbank R702 nicht geladen (Einschlag stumm)")
endif()
if(NOT _dbg MATCHES "\\[finale-se\\] Se_on Raumbank R702 Satz 0x0F")
    string(APPEND _f "\n Einschlag-Ton Raumbank R702 Satz 0x0F nicht gespielt")
endif()
if(_dbg MATCHES "\\[gator-re2\\] VSync 5 [^\n]* ab F([0-9]+)(.*)$")
    set(_v5 "${CMAKE_MATCH_1}")
    set(_nach "${CMAKE_MATCH_2}")
    if(_nach MATCHES "\\[gator-re2\\] VSync 2 [^\n]* ab F([0-9]+)")
        math(EXPR _dv "${CMAKE_MATCH_1} - ${_v5}")
        if(NOT _dv EQUAL 84)
            string(APPEND _f "\n Zeitlupe ${_dv} Bilder statt 84 (83 bis zum Kill + Phase 9)")
        endif()
    else()
        string(APPEND _f "\n VSync 2 nach der Zeitlupe fehlt")
    endif()
else()
    string(APPEND _f "\n VSync 5 (Einschlag) fehlt im debug.log")
endif()
if(_dbg MATCHES "\\[g5a\\]")
    string(APPEND _f "\n [g5a]-Zeile im Lauf: der G5-Auftritt lief")
endif()
if(NOT _dbg MATCHES "PC loaded room6040")
    string(APPEND _f "\n Abspann-Raum 6040 nicht geladen")
endif()
if(NOT _dbg MATCHES "Set\\(0,1,0\\): INGAME aus -> Titel")
    string(APPEND _f "\n Diashow-Ende Set(0,1,0) nicht gesehen")
endif()
# S8 (Plan): die Diashow ROOM6040 dauert die Summe ihrer Sleeps, 3952 Bilder (@0x00534-0x00586, Dossier C 1.2); der Bildzaehler
# beginnt mit dem Raum neu, Set(0,1,0) @0x00590 faellt deshalb im Bild 3952 + Vorlauf des Raumstarts (gemessen 5: F3957).
if(_dbg MATCHES "\\[finale\\] F([0-9]+) ROOM6040 Set\\(0,1,0\\)")
    math(EXPR _dia "${CMAKE_MATCH_1} - 3952")
    if(_dia LESS 0 OR _dia GREATER 8)
        string(APPEND _f "\n Diashow-Ende im Bild ${CMAKE_MATCH_1}, erwartet 3952 + 0..8 (Summe der Sleeps @0x00534-0x00586)")
    endif()
else()   # Runde 3 (Pruefbericht s1 r2, gelb): ohne die Zeile ist die Dauer nicht geprueft -> rot
    string(APPEND _f "\n Messzeile '[finale] F<n> ROOM6040 Set(0,1,0)' fehlt - Diashow-Dauer 3952 nicht pruefbar")
endif()
# S8: das neue Spiel nach dem Titel traegt weder das Finale-Bit 129 noch den Raketenwerfer (main.c memset g_inv/g_game).
if(NOT _dbg MATCHES "\\[flow\\] boot #2: Raum [0-9A-F]+, Bit 129 0, Raketenwerfer im Inventar 0")
    string(APPEND _f "\n neues Spiel nach dem Titel nicht leer (Bit 129 oder Raketenwerfer uebrig)")
endif()
if(NOT _dbg MATCHES "BOOT_EXIT_AT: boot #2")
    string(APPEND _f "\n Titel nicht erreicht (kein zweiter Spiel-Boot, exit=${_rv})")
endif()
message(STATUS "r42_finale_schuss: T=${_T} U=${_U} exit=${_rv}")
if(_f)
    message(FATAL_ERROR "r42_finale_schuss:${_f}\n--- finale.log ---\n${_log}")
endif()
