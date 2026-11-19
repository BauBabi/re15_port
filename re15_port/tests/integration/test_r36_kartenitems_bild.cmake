# integration_r36_kartenitems_bild — Runde 36 Spur N "Lageplaene zum Aufsammeln", an der ECHTEN exe.
#
# Teil 1 (je Raum, Lade-Weg CONTINUE): zwei Laeufe mit derselben Karte, einmal OHNE und einmal MIT
#   gesetztem Bank-9-Bit (3F 87, 2F 88, 1F 89, B2 77, B1 78 - Nachtrag kartenbits, Dossier 9: B2/B1 lagen auf
#   90/91 = Original-Taken-Bits ROOM2090/2091 @0x00A12 Item 0x3F und ROOM2040/2041 @0x01364 Item 0x32). Geprueft:
#     a) der Boot-/CONTINUE-Haken hat die Karte angelegt ("[kartenitems] ROOMxxxx: Karte ..."), mit Bit nicht;
#     b) im Framedump des Nutzer-Cuts (RE15_FORCE_CUT, RE15_WINDOW_SCALE=1 -> 320x240) unterscheiden sich
#        die Laeufe NUR im Fenster um die Nutzer-Marke (Marken-Huelle +-3 px): dort mindestens MIN Pixel,
#        ausserhalb 0 — die Karte haengt an der Marke;
#     c) danach Statusschirm + L1 (Kartenschirm des eigenen Blatts, RE15_INV_FB_SHOT): MIT Bit zeigt das Blatt
#        mindestens MIN_KARTE Pixel mehr — die unbesuchten Raeume der Etage (RE2 CLUT-Zeile 498).
#     d) (Nachbesserung 1, Licht) JEDER Kartenpixel ist EXAKT eine Farbe der 256er-Palette der RE2-TIM
#        (probe_r36_kartenitems_licht): die Karte traegt das Flach-Attribut 0x0010 (Pool+0x0c) und wird wie
#        im Original ohne Raumlicht gezeichnet (FUN_8002c18c @0x8002c2f4 -> FUN_80022f0c @0x8002c320,
#        Farbe obj+0x70 = 0x808080). Mit NCCT (1140: 41/128) oder einer Tint-Konstante laegen die Pixel
#        ausserhalb der Palette. ROOM1030 zusaetzlich in Cut 0 (Raumlicht an der Karte (89,89,94) statt
#        (54,54,59) in Cut 3, Abnahme 0 Abschnitt 4): dieselbe Palette, dieselbe mittlere Helligkeit.
# Teil 2 (ROOM1140, echter Weg): Spieler vor der Tafel, RE15_INPUT_SCRIPT: Aktion -> Frage -> Ja ->
#   Meldung -> Statusschirm -> L1 Karte. Geprueft: Ereignis 28 lief ("-> Port-Programm"), und der
#   Kartenschirm (RE15_INV_FB_SHOT) zeigt gegenueber dem Lauf ohne Aufnahme mindestens MIN_KARTE
#   zusaetzliche Pixel (die unbesuchten 3F-Raeume, RE2 CLUT-Zeile 498 @0x8006E71C).
# Dossier: analysis/befunde_runde36/N_kartenitems.md (Abschnitt 5).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r35_inhalt_karte>
#               -DRE15_BILD_TOOL=<probe_r34n_e_bild> -DRE15_BMP_TOOL=<probe_r36_kartenitems_bmp>
#               -DRE15_LICHT_TOOL=<probe_r36_kartenitems_licht> -DWORKDIR=<dir> -P test_r36_kartenitems_bild.cmake

foreach(_v RE15_PC_EXE RE15_KARTE_TOOL RE15_BILD_TOOL RE15_BMP_TOOL RE15_LICHT_TOOL)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "kartenitems_bild: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "kartenitems_bild: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

function(hexof _text _out)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(${_out} "${_h}" PARENT_SCOPE)
endfunction()

# _name _raum _x _z _rot _cut _bitarg _script _shot_at -> WORKDIR mit debug.log, raum.ppm, karte.bmp
function(lauf _name _raum _x _z _rot _cut _bitarg _script _shot_at _framedump)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}" ${_x} ${_z} ${_rot} ${_bitarg}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "kartenitems_bild[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    set(_env RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=1
             RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_FORCE_CUT=${_cut}
             "RE15_FRAMEDUMP=${_framedump}:raum.ppm")
    if(_script)
        list(APPEND _env RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=30 "RE15_INPUT_SCRIPT=${_script}"
                         RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=${_shot_at} "RE15_EXIT_AT=700#${_raum}")
    else()
        math(EXPR _ende "${_framedump} + 1")
        list(APPEND _env "RE15_EXIT_AT=${_ende}#${_raum}")
    endif()
    re15_start_spiel(_rv 240 ${_env} "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "kartenitems_bild[${_name}]: kein debug.log (exit=${_rv})")
    endif()
    file(READ "${WORKDIR}/debug.log" _lh HEX)
    hexof("CONTINUE: resumed in room" _hc)
    string(FIND "${_lh}" "${_hc}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "kartenitems_bild[${_name}]: Spielstand nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    if(NOT EXISTS "${WORKDIR}/raum.ppm")
        message(FATAL_ERROR "kartenitems_bild[${_name}]: kein Framedump (exit=${_rv}, ${WORKDIR})")
    endif()
    set(LAUF_DIR "${WORKDIR}" PARENT_SCOPE)
    set(LAUF_LOG "${_lh}" PARENT_SCOPE)
endfunction()

# Runde 42 Strang lageplaene: nur noch Revierplan (ROOM1030, Bits 89+88+87) und Kellerplan (ROOM1200, Bits 77+78);
# die Faelle 3f (1140), 2f (10D0), b1 (11D0) entfallen mit ihren Wandplaenen. Spalte bit = ALLE Bits des Plans
# ("+"-getrennt), denn der Plan haengt, solange eines fehlt (kartenitems_r36.c re15_kartenitems_r36_genommen).
# name | raum | spieler x,z,rot (verdeckt die Karte nicht) | cut | bits | fenster x0 y0 x1 y1 | min
# Fenster = Marken-Huelle (analysis/befunde_runde36/marken/map*.bmp) +-3 px. Gemessen 2026-10-11
# (Dossier 5.2, Lade-Weg F100): 1140 73, 10D0 219, 1030 81, 1200 177, 11D0 169 Pixel, ausserhalb je 0.
# Schwelle = rund die Haelfte.
# NACHARBEIT (Strang lageplaene_nach): der Revierplan haengt jetzt an der Ostwand neben der Ausgangstuer (Kamera 2 = Tuer-
# kamera beider Figuren, s = 2) und setzt zusaetzlich das Dach-Bit 42. Gemessen 2026-11-18 (Lade-Weg F100, RE15_FORCE_CUT):
# Cut 2 1263 Pixel, Huelle x117..159 y41..72; Cut 1 319 Pixel, Huelle x276..308 y76..97 (vom Fernseher am Pfeiler teilweise
# verdeckt); ausserhalb je 0. Fenster = Huelle +-3, Schwelle = rund die Haelfte. Der Vergleichs-Cut fuer die Helligkeit ist
# jetzt Cut 1 (vorher Cut 0; Cut 0 zeigt die Ostwand nicht).
# STRANG LAGEPLAENE_NACH2 (Augenhoehe links der Tuer, Mitte (2398,-2658,-10616), s = 1.5): gemessen 2026-11-18 (Lade-Weg F100,
# RE15_FORCE_CUT, Fenster 0..319 x 0..239): Cut 2 1139 Pixel, Huelle x18..64 y78..103, 1139 von 1139 Palettenfarben; Cut 1
# 179 Pixel, Huelle x233..247 y101..112 (15 x 12 = 180: ganz sichtbar, der Fernseher am Pfeiler steht weiter rechts);
# ausserhalb je 0. Fenster = Huelle +-3, Schwelle = rund die Haelfte.
# STRANG KELLERPLAN (Nordwand westlich der Eingangstuer, Mitte (-23550,-2658,-26771), s = 2 x 0.6896): gemessen 2026-11-19
# (Lade-Weg F100, RE15_FORCE_CUT 0 = Ankunftskamera, Leon bei (-20000,-26000) an der Tuer): 876 Pixel, Huelle x168..201 y66..91
# (bisher Cut 2: 177 Pixel, x57..73 y79..92); ausserhalb 0. Fenster = Huelle +-3, Schwelle = die Haelfte.
set(_faelle
    "1f|1030|-18000|-12000|0|2|89+88+87+42|15|75|67|106|569|452"
    "b2|1200|-20000|-26000|0|0|77+78|165|63|204|94|438|381"
    "1f0|1030|-18000|-12000|0|1|89+88+87+42|230|98|250|115|89|452")
# Sechster Fall (Nachbesserung 1): ROOM1030 im Cut 0 — die Karte an der Rueckwand zwischen den Aushaengen,
# gemessen 2026-10-11 (neue exe, Lade-Weg F100): 48 Pixel, Huelle x91..98 y107..112, Mittel (105,107,96),
# 48 von 48 Palettenfarben. Fenster = Huelle +-3, Schwelle = die Haelfte.
# Letzte Spalte MIN_KARTE: Kartenschirm mit/ohne Plan-Bit, gemessen 2026-10-11 (Dossier 4.4): 3F 537,
# 2F 623, 1F 905, B2 763, B1 866 Pixel, ausserhalb des Kartenfelds je 0. Schwelle = die Haelfte.
set(_karte_skript "W3,S0.3,W2,Q0.3,W4")

set(_fehler "")
foreach(_f ${_faelle})
    string(REPLACE "|" ";" _t "${_f}")
    list(GET _t 0 _n)
    list(GET _t 1 _raum)
    list(GET _t 2 _x)
    list(GET _t 3 _z)
    list(GET _t 4 _rot)
    list(GET _t 5 _cut)
    list(GET _t 6 _bit)
    list(GET _t 7 _x0)
    list(GET _t 8 _y0)
    list(GET _t 9 _x1)
    list(GET _t 10 _y1)
    list(GET _t 11 _min)
    list(GET _t 12 _min_karte)
    hexof("[kartenitems] ROOM${_raum}: Karte" _hk)
    string(REPLACE "+" ";bit:" _bitliste "bit:${_bit}")
    # Nach dem Raumbild (F100) Statusschirm + L1: Kartenschirm des eigenen Blatts (inv-Bild 140).
    lauf(${_n}_mit ${_raum} ${_x} ${_z} ${_rot} ${_cut} "" "${_karte_skript}" 140 100)
    set(_dir_mit "${LAUF_DIR}")
    string(FIND "${LAUF_LOG}" "${_hk}" _p_mit)
    lauf(${_n}_ohne ${_raum} ${_x} ${_z} ${_rot} ${_cut} "${_bitliste}" "${_karte_skript}" 140 100)
    set(_dir_ohne "${LAUF_DIR}")
    string(FIND "${LAUF_LOG}" "${_hk}" _p_ohne)
    if(_p_mit LESS 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: keine Zeile '[kartenitems] ROOM${_raum}: Karte' ohne Bit\n")
    endif()
    if(NOT _p_ohne LESS 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: Karte angelegt trotz Bit (9,${_bit})\n")
    endif()
    execute_process(COMMAND "${RE15_BILD_TOOL}" "${_dir_mit}/raum.ppm" "${_dir_ohne}/raum.ppm"
                            ${_x0} ${_y0} ${_x1} ${_y1} ${_min}
                    TIMEOUT 60 RESULT_VARIABLE _rvb OUTPUT_VARIABLE _outb)
    string(STRIP "${_outb}" _outb)
    message(STATUS "kartenitems_bild ${_n} ROOM${_raum} Cut ${_cut}: ${_outb}")
    if(NOT _rvb EQUAL 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: Karte nicht (genug) an der Marke gezeichnet: ${_outb}\n")
    endif()
    if(NOT _outb MATCHES "AUSSEN: 0$")
        string(APPEND _fehler "${_n} ROOM${_raum}: Unterschied ausserhalb des Marken-Fensters: ${_outb}\n")
    endif()
    # d) Farbe der Karte = Palettenfarben der RE2-TIM (Flach-Weg, kein Raumlicht). Gemessen 2026-10-11 (neue
    #    exe, Messkopie): 1F Cut 3 88 von 88, 1F Cut 0 48 von 48 Palettenpixel (uebrige Raeume: Dossier 8.5); Mittel
    #    1F Cut 3 (101,102,93) Summe 297, Cut 0 (105,107,96) Summe 310 (dasselbe Blatt, anderes
    #    Raumlicht) — gegen die alte exe: 1F (37,57,77) 0/81, B2 (68,70,56) 1/177, B1 (61,64,59) 0/169.
    execute_process(COMMAND "${RE15_LICHT_TOOL}" "${_dir_mit}/raum.ppm" "${_dir_ohne}/raum.ppm"
                            ${_x0} ${_y0} ${_x1} ${_y1} ${_raum} ${_min}
                    TIMEOUT 60 RESULT_VARIABLE _rvl OUTPUT_VARIABLE _outl)
    string(STRIP "${_outl}" _outl)
    message(STATUS "kartenitems_bild ${_n} ROOM${_raum} Cut ${_cut} Licht: ${_outl}")
    if(NOT _rvl EQUAL 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: Kartenpixel sind keine Palettenfarben (Raumlicht/Tint statt Flach-Weg): ${_outl}\n")
    endif()
    if(_outl MATCHES "Summe ([0-9]+)")
        set(_summe_${_n} "${CMAKE_MATCH_1}")
    endif()
    # Kartenschirm je Etage: MIT Plan-Bit zusaetzlich alle unbesuchten Raeume des Blatts (RE2 @0x8006E71C)
    if(NOT EXISTS "${_dir_mit}/karte.bmp" OR NOT EXISTS "${_dir_ohne}/karte.bmp")
        string(APPEND _fehler "${_n} ROOM${_raum}: kein Kartenschirm-Abzug\n")
    else()
        execute_process(COMMAND "${RE15_BMP_TOOL}" "${_dir_ohne}/karte.bmp" "${_dir_mit}/karte.bmp"
                                20 50 300 215 ${_min_karte}
                        TIMEOUT 60 RESULT_VARIABLE _rvm OUTPUT_VARIABLE _outm)
        string(STRIP "${_outm}" _outm)
        message(STATUS "kartenitems_bild ${_n} Kartenschirm mit/ohne Plan: ${_outm}")
        if(NOT _rvm EQUAL 0)
            string(APPEND _fehler "${_n} ROOM${_raum}: Plan-Bit zeichnet die Etage nicht: ${_outm}\n")
        endif()
    endif()
endforeach()

# ROOM1030 zwei Cuts (bis zur Nacharbeit Cut 3 gegen Cut 0, Raumlicht (54,54,59) gegen (89,89,94), Faktor 1,65 — Abnahme 0
# Abschnitt 4; seit der Nacharbeit Cut 2 gegen Cut 1, Werte unten): ohne Raumlicht ist die Karte in beiden Cuts gleich hell.
# Nacharbeit gemessen 2026-11-18: Cut 2 Summe 296 (1263 Pixel, Mittel 99/103/93), Cut 1 Summe 282 (319 Pixel), Abstand 14,
# je 100 % Palettenfarben. Bis zur Nacharbeit: Summe 297 (Cut 3, 88 Pixel)
# und 310 (Cut 0, 48 Pixel); Abstand 13. Zulaessig: 1/10 der Summe (29) fuer die Texel-Abtastung bei
# verschiedener Bildgroesse — ein NCCT-Unterschied von 1,65 waere ein Abstand von rund 120.
if(DEFINED _summe_1f AND DEFINED _summe_1f0)
    math(EXPR _abstand "${_summe_1f} - ${_summe_1f0}")
    if(_abstand LESS 0)
        math(EXPR _abstand "0 - ${_abstand}")
    endif()
    math(EXPR _zul "${_summe_1f} / 10")
    message(STATUS "kartenitems_bild 1F Cut 2 Summe ${_summe_1f} gegen Cut 1 Summe ${_summe_1f0}: Abstand ${_abstand} (zulaessig ${_zul})")
    if(_abstand GREATER _zul)
        string(APPEND _fehler "1F: Karte in Cut 2 und Cut 1 verschieden hell (${_summe_1f} gegen ${_summe_1f0}) — Raumlicht statt Flach-Weg\n")
    endif()
else()
    string(APPEND _fehler "1F: Helligkeit in Cut 3/Cut 0 nicht gemessen\n")
endif()

# ---- Teil 2: echter Weg in ROOM1030 (Runde 42: Revierplan; vorher ROOM1140) -------------------------
# Nacharbeit: Spieler (1700,-14000) im Streifen zwischen Bank und Tuer, Blick Ostwand (rot 0; Zone x 1598..2598
# z -14800..-13800 = Slot 9, die Notfall-Tafel zog nach 39). Strang lageplaene_nach2: Spieler (1500,-11300) vor dem Plan
# links der Tuer, Blick Ostwand (rot 0; Zone Slot 21 x 1598..2598 z -12010..-11010; unit_r42_lageplaene4_druck).
# Skript (Spielbilder ab 30): A Aktion | W3 Frage tippt | A Ja | W3 Meldung | A weg | W2 | S Status | W2 | Q Karte | W4
set(_auf  "A0.1,W3,A0.1,W3,A0.1,W2,S0.3,W2,Q0.3,W4")
set(_kein "W3,W3,W3,W2,S0.3,W2,Q0.3,W4")
lauf(weg_auf  1030 1500 -11300 0 2 "" "${_auf}"  140 270)
set(_dir_auf "${LAUF_DIR}")
hexof("[kartenitems] ROOM1030 Ereignis 28 -> Port-Programm" _hp)
string(FIND "${LAUF_LOG}" "${_hp}" _p)
if(_p LESS 0)
    string(APPEND _fehler "weg: Aktionsdruck vor dem Plan startete das Karten-Programm nicht (Ereignis 28)\n")
endif()
lauf(weg_kein 1030 1500 -11300 0 2 "" "${_kein}" 140 270)
set(_dir_kein "${LAUF_DIR}")
if(NOT EXISTS "${_dir_auf}/karte.bmp" OR NOT EXISTS "${_dir_kein}/karte.bmp")
    string(APPEND _fehler "weg: kein Kartenschirm-Abzug (RE15_INV_FB_SHOT)\n")
else()
    # Kartenfeld des Schirms (20..300, 50..215); Runde 36 (3F): 537 Pixel. Runde 42 (1F nach der Aufnahme des
    # Revierplans): Schwelle = die Haelfte der 1F-Zahl aus Teil 1 (905 -> 452).
    execute_process(COMMAND "${RE15_BMP_TOOL}" "${_dir_auf}/karte.bmp" "${_dir_kein}/karte.bmp" 20 50 300 215 452
                    TIMEOUT 60 RESULT_VARIABLE _rvm OUTPUT_VARIABLE _outm)
    string(STRIP "${_outm}" _outm)
    message(STATUS "kartenitems_bild weg: Kartenschirm 1F mit/ohne Aufnahme: ${_outm}")
    if(NOT _rvm EQUAL 0)
        string(APPEND _fehler "weg: Kartenschirm zeigt nach der Aufnahme keine zusaetzlichen Raeume: ${_outm}\n")
    endif()
endif()

if(_fehler)
    message(FATAL_ERROR "kartenitems_bild: das SICHTBARE Ergebnis stimmt nicht.\n${_fehler}")
endif()
message(STATUS "kartenitems_bild: Revier- und Kellerplan an der Marke gezeichnet (Palettenfarben, kein Raumlicht), Aufnahme am echten Weg schaltet 1F frei")
