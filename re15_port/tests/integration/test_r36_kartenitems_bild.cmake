# integration_r36_kartenitems_bild — Runde 36 Spur N "Lageplaene zum Aufsammeln", an der ECHTEN exe.
#
# Teil 1 (je Raum, Lade-Weg CONTINUE): zwei Laeufe mit derselben Karte, einmal OHNE und einmal MIT
#   gesetztem Bank-9-Bit (87..91). Geprueft:
#     a) der Boot-/CONTINUE-Haken hat die Karte angelegt ("[kartenitems] ROOMxxxx: Karte ..."), mit Bit nicht;
#     b) im Framedump des Nutzer-Cuts (RE15_FORCE_CUT, RE15_WINDOW_SCALE=1 -> 320x240) unterscheiden sich
#        die Laeufe NUR im Fenster um die Nutzer-Marke (Marken-Huelle +-3 px): dort mindestens MIN Pixel,
#        ausserhalb 0 — die Karte haengt an der Marke;
#     c) danach Statusschirm + L1 (Kartenschirm des eigenen Blatts, RE15_INV_FB_SHOT): MIT Bit zeigt das Blatt
#        mindestens MIN_KARTE Pixel mehr — die unbesuchten Raeume der Etage (RE2 CLUT-Zeile 498).
# Teil 2 (ROOM1140, echter Weg): Spieler vor der Tafel, RE15_INPUT_SCRIPT: Aktion -> Frage -> Ja ->
#   Meldung -> Statusschirm -> L1 Karte. Geprueft: Ereignis 28 lief ("-> Port-Programm"), und der
#   Kartenschirm (RE15_INV_FB_SHOT) zeigt gegenueber dem Lauf ohne Aufnahme mindestens MIN_KARTE
#   zusaetzliche Pixel (die unbesuchten 3F-Raeume, RE2 CLUT-Zeile 498 @0x8006E71C).
# Dossier: analysis/befunde_runde36/N_kartenitems.md (Abschnitt 5).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r35_inhalt_karte>
#               -DRE15_BILD_TOOL=<probe_r34n_e_bild> -DRE15_BMP_TOOL=<probe_r36_kartenitems_bmp>
#               -DWORKDIR=<dir> -P test_r36_kartenitems_bild.cmake

foreach(_v RE15_PC_EXE RE15_KARTE_TOOL RE15_BILD_TOOL RE15_BMP_TOOL)
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

# name | raum | spieler x,z,rot (verdeckt die Karte nicht) | cut | bit | fenster x0 y0 x1 y1 | min
# Fenster = Marken-Huelle (analysis/befunde_runde36/marken/map*.bmp) +-3 px. Gemessen 2026-10-11
# (Dossier 5.2, Lade-Weg F100): 1140 73, 10D0 219, 1030 81, 1200 177, 11D0 169 Pixel, ausserhalb je 0.
# Schwelle = rund die Haelfte.
set(_faelle
    "3f|1140|-8000|-11000|0|5|87|228|69|244|84|36|1"
    "2f|10D0|2000|-6000|0|6|88|267|117|292|138|110|1"
    "1f|1030|-18000|-12000|0|3|89|152|100|167|116|40|1"
    "b2|1200|-20000|-26000|0|2|90|55|77|77|96|88|1"
    "b1|11D0|-3000|-21000|0|0|91|224|58|253|100|84|1")
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
    # Nach dem Raumbild (F100) Statusschirm + L1: Kartenschirm des eigenen Blatts (inv-Bild 140).
    lauf(${_n}_mit ${_raum} ${_x} ${_z} ${_rot} ${_cut} "" "${_karte_skript}" 140 100)
    set(_dir_mit "${LAUF_DIR}")
    string(FIND "${LAUF_LOG}" "${_hk}" _p_mit)
    lauf(${_n}_ohne ${_raum} ${_x} ${_z} ${_rot} ${_cut} "bit:${_bit}" "${_karte_skript}" 140 100)
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

# ---- Teil 2: echter Weg in ROOM1140 -----------------------------------------------------------------
# Spieler (-2050,-9900) Blick +z (rot 3071, Zone x -2551..-1551 z -9510..-8510; unit_r36_kartenitems D).
# Skript (Spielbilder ab 30): A Aktion | W3 Frage tippt | A Ja | W3 Meldung | A weg | W2 | S Status | W2 | Q Karte | W4
set(_auf  "A0.1,W3,A0.1,W3,A0.1,W2,S0.3,W2,Q0.3,W4")
set(_kein "W3,W3,W3,W2,S0.3,W2,Q0.3,W4")
lauf(weg_auf  1140 -2050 -9900 3071 5 "" "${_auf}"  140 270)
set(_dir_auf "${LAUF_DIR}")
hexof("[kartenitems] ROOM1140 Ereignis 28 -> Port-Programm" _hp)
string(FIND "${LAUF_LOG}" "${_hp}" _p)
if(_p LESS 0)
    string(APPEND _fehler "weg: Aktionsdruck vor der Tafel startete das Karten-Programm nicht (Ereignis 28)\n")
endif()
lauf(weg_kein 1140 -2050 -9900 3071 5 "" "${_kein}" 140 270)
set(_dir_kein "${LAUF_DIR}")
if(NOT EXISTS "${_dir_auf}/karte.bmp" OR NOT EXISTS "${_dir_kein}/karte.bmp")
    string(APPEND _fehler "weg: kein Kartenschirm-Abzug (RE15_INV_FB_SHOT)\n")
else()
    # Kartenfeld des Schirms (20..300, 50..215); gemessen 2026-10-11: 537 Pixel (Huelle x120..193
    # y80..158 = die unbesuchten 3F-Raeume als Umriss). Schwelle = die Haelfte.
    execute_process(COMMAND "${RE15_BMP_TOOL}" "${_dir_auf}/karte.bmp" "${_dir_kein}/karte.bmp" 20 50 300 215 268
                    TIMEOUT 60 RESULT_VARIABLE _rvm OUTPUT_VARIABLE _outm)
    string(STRIP "${_outm}" _outm)
    message(STATUS "kartenitems_bild weg: Kartenschirm 3F mit/ohne Aufnahme: ${_outm}")
    if(NOT _rvm EQUAL 0)
        string(APPEND _fehler "weg: Kartenschirm zeigt nach der Aufnahme keine zusaetzlichen Raeume: ${_outm}\n")
    endif()
endif()

if(_fehler)
    message(FATAL_ERROR "kartenitems_bild: das SICHTBARE Ergebnis stimmt nicht.\n${_fehler}")
endif()
message(STATUS "kartenitems_bild: alle fuenf Karten an der Marke gezeichnet, Aufnahme am echten Weg schaltet 3F frei")
