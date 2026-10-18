cmake_minimum_required(VERSION 3.16)
# =============================================================================
# GIFT ZIEHT ENERGIE WIE RE2, EKG ZEIGT ES (Runde 39, Spur F "zustand", Nutzerpunkt 2).
#
# Nutzer (AUFTRAG.md Z. 28): "Der "Poison" Zustand ist zwar jetzt da, aber er zieht keine Energie ab Stueck fuer
# Stueck, so wie im Originalen Resident Evil 2. Zumindest zeigt er es vom EKG her nicht so an."
# Dossier analysis/befunde_runde39/F_zustand.md (2.1-2.4). Beta -> Retail: RE1.5 zog -2 HP alle 121 Bilder nur im
# Kommando-1-Prolog (@0x80031e44-ecc); Zielform RE2 PSX.EXE Leon FUN_8003bfac @0x8003c12c-180: Zaehler 0x800cfe12
# 30 -> 0, bei 0 HP -1 und Zaehler := 30 (Periode 31 Bilder), `slti v0,v0,0x2` = Untergrenze HP 1; EKG-Farbe
# POISON aus FUN_800754ac mit Tabelle @0x800ab178: ff 20 ff / Schritt 08 01 08 je Linie (magenta).
# Die Mechanik misst unit_r39_zustand B/C; dieser Riegel misst die ECHTE exe ueber die Messschiene RE15_R39_GIFT_AT /
# RE15_R39_LOG (env, kein Spielverhalten) und den Framedump des Statusschirms:
#   Lauf G  Leon ROOM1130, HP 100, Gift im Bild 300 (Bit 0x2 + Zaehler 30 wie Op 32 @0x80020138-40), START im Bild
#           1500 (Statusschirm), Framedumps 1540..1600, Ende Bild 1700.
#           hp-Auswerter: jeder Takt -1 HP, erster Takt Bild 330 (Zaehler 30 .. 0), alle Abstaende 31, mindestens 30
#           Takte, letztes protokolliertes Spielbild 1499 (im Statusschirm steht der Spielerschritt, game_step_common.c
#           re15_menu_gameplay_frozen -> return vor dem Spieler-Dispatcher).
#           ekg-Auswerter (Kasten x26..103 / y118..140 = die 32 LineF2 @0x80048c34-94 im EKG-Fenster): in den
#           Bildern 1540/1560/1580 magenta > 0 und gruen = gelb = rot = 0 (vorher: gruen wie FINE, 0 magenta).
#   Lauf U  dasselbe mit HP 5, Ende Bild 900: genau 4 Takte (5 -> 1 in Bild 330/361/392/423), HP-Minimum 1,
#           danach kein Abzug mehr (`slti v0,v0,0x2 / bne` @0x8003c174-78).
#
# Parallel-/Linux-fest: eigene exe-Kopie mit Praefix r39fg, eigener Laufordner je Lauf, keine ererbten Umgebungswerte.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r39_zustand> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r39_zustand_gift: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
    message(FATAL_ERROR "r39_zustand_gift: RE15_TOOL fehlt: '${RE15_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r39_zustand_gift: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r39fg_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r39fg_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

set(_fehler "")

function(r39fg_lauf _name _hp _ende _extra)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    execute_process(COMMAND "${RE15_TOOL}" karte re15_card.mcr 1130 0 -1024 0 -3336 856 "${_hp}" "-" "-" 2
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r39_zustand_gift[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 400
        SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_R39_GIFT_AT=300 RE15_R39_LOG=gift.log
        ${_extra}
        "RE15_EXIT_AT=${_ende}#1130"
        "${_exe}")
    if(NOT EXISTS "${WORKDIR}/gift.log")
        message(FATAL_ERROR "r39_zustand_gift[${_name}]: exe rv=${_rv}, gift.log fehlt in ${WORKDIR}")
    endif()
    message(STATUS "r39_zustand_gift[${_name}]: rv=${_rv}, ${WORKDIR}")
endfunction()

# hp-Auswerter: "hp: N Takte, Summe S, Abzug min a max b, erster Takt Ff, Abstaende 31: p von q, HP-Minimum m,
#                Giftbilder g, letztes Bild Fl"
function(r39fg_hp _name _var)
    execute_process(COMMAND "${RE15_TOOL}" hp "${_basis}_${_name}/gift.log" TIMEOUT 60
                    RESULT_VARIABLE _rvh OUTPUT_VARIABLE _outh)
    # Hoechstens 9 Gruppen (CMake fuellt nur CMAKE_MATCH_0..9): Summe und Giftbilder werden nicht gefangen.
    if(NOT _outh MATCHES "hp: ([0-9]+) Takte, Summe [0-9]+, Abzug min (-?[0-9]+) max (-?[0-9]+), erster Takt F(-?[0-9]+), Abstaende 31: ([0-9]+) von ([0-9]+), HP-Minimum ([0-9]+), Giftbilder [0-9]+, letztes Bild F([0-9]+)")
        message(FATAL_ERROR "r39_zustand_gift[${_name}]: hp-Auswerter ohne Summenzeile (rv=${_rvh})\n${_outh}")
    endif()
    set(${_var}_takte  "${CMAKE_MATCH_1}" PARENT_SCOPE)
    set(${_var}_min    "${CMAKE_MATCH_2}" PARENT_SCOPE)
    set(${_var}_max    "${CMAKE_MATCH_3}" PARENT_SCOPE)
    set(${_var}_erster "${CMAKE_MATCH_4}" PARENT_SCOPE)
    set(${_var}_p31    "${CMAKE_MATCH_5}" PARENT_SCOPE)
    set(${_var}_pn     "${CMAKE_MATCH_6}" PARENT_SCOPE)
    set(${_var}_hpmin  "${CMAKE_MATCH_7}" PARENT_SCOPE)
    set(${_var}_letzt  "${CMAKE_MATCH_8}" PARENT_SCOPE)
    string(REGEX MATCH "hp: [^\n]*" _zeile "${_outh}")
    message(STATUS "r39_zustand_gift[${_name}]: ${_zeile}")
endfunction()

# ---- Lauf G: HP 100, Statusschirm ab Bild 1500 ----
r39fg_lauf(g 100 1700 "RE15_PAD_AT=1500:S;RE15_FRAMEDUMP=1540-1600/20:fd_")
r39fg_hp(g G)
if(NOT G_min EQUAL 1 OR NOT G_max EQUAL 1)
    set(_fehler "${_fehler}\n  G: Abzug je Takt ${G_min}..${G_max}, erwartet 1 (RE2 @0x8003c17c-80; RE1.5-Form war 2)")
endif()
if(NOT G_erster EQUAL 330)
    set(_fehler "${_fehler}\n  G: erster Takt Bild ${G_erster}, erwartet 330 (Zaehler 30 im Bild 300 -> 0 im Bild 330, @0x8003c144-64)")
endif()
if(G_takte LESS 30 OR NOT G_p31 EQUAL G_pn)
    set(_fehler "${_fehler}\n  G: ${G_takte} Takte, Abstaende 31: ${G_p31} von ${G_pn} (erwartet >= 30 Takte, alle 31; RE1.5-Form war 121)")
endif()
if(NOT G_letzt EQUAL 1499)
    set(_fehler "${_fehler}\n  G: letztes protokolliertes Spielbild ${G_letzt}, erwartet 1499 (im Statusschirm kein Spielerschritt)")
endif()
foreach(_b 1540 1560 1580)
    set(_ppm "${_basis}_g/fd_00${_b}.ppm")
    if(NOT EXISTS "${_ppm}")
        set(_fehler "${_fehler}\n  G: Framedump ${_ppm} fehlt")
        continue()
    endif()
    execute_process(COMMAND "${RE15_TOOL}" ekg "${_ppm}" 26 118 103 140 TIMEOUT 60 OUTPUT_VARIABLE _oute)
    if(NOT _oute MATCHES "hell=([0-9]+) gruen=([0-9]+) gelb=([0-9]+) rot=([0-9]+) magenta=([0-9]+)")
        set(_fehler "${_fehler}\n  G: ekg-Auswerter ohne Zensus fuer Bild ${_b}: ${_oute}")
        continue()
    endif()
    string(STRIP "${_oute}" _oute)
    message(STATUS "r39_zustand_gift[g] Bild ${_b}: ${_oute}")
    if(CMAKE_MATCH_5 EQUAL 0 OR NOT CMAKE_MATCH_2 EQUAL 0 OR NOT CMAKE_MATCH_3 EQUAL 0 OR NOT CMAKE_MATCH_4 EQUAL 0)
        set(_fehler "${_fehler}\n  G Bild ${_b}: EKG-Spur nicht magenta (gruen=${CMAKE_MATCH_2} gelb=${CMAKE_MATCH_3} rot=${CMAKE_MATCH_4} magenta=${CMAKE_MATCH_5}; RE2 POISON @0x800ab178)")
    endif()
endforeach()

# ---- Lauf U: HP 5 -> Untergrenze 1 ----
r39fg_lauf(u 5 900 "")
r39fg_hp(u U)
if(NOT U_takte EQUAL 4 OR NOT U_hpmin EQUAL 1 OR NOT U_erster EQUAL 330 OR NOT U_min EQUAL 1 OR NOT U_max EQUAL 1)
    set(_fehler "${_fehler}\n  U: ${U_takte} Takte (erwartet 4: 5 -> 1), HP-Minimum ${U_hpmin} (erwartet 1, `slti v0,v0,0x2` @0x8003c174), erster Takt ${U_erster}, Abzug ${U_min}..${U_max}")
endif()

file(REMOVE "${_exe}")
if(NOT _fehler STREQUAL "")
    message(FATAL_ERROR "r39_zustand_gift: FEHLER${_fehler}")
endif()
message(STATUS "r39_zustand_gift: GRUEN (-1 HP alle 31 Bilder ab Bild 330, Untergrenze 1, EKG magenta im Statusschirm)")
