# integration_r34n_e_dokumente_bild — Runde 34 Nacht, Spur E "Vier neue Dokumente".
#
# RIEGEL AUF DAS SICHTBARE ERGEBNIS an der ECHTEN exe, am LADE-Weg (CONTINUE direkt in den
# Dokument-Raum): je Raum zwei Laeufe mit derselben Karte, einmal Dokument in der Welt, einmal mit
# gesetztem Genommen-Bit (9, 56 + Nr). Geprueft wird:
#   1. der Lauf hat den Spielstand im Raum geladen (debug.log "CONTINUE: resumed in room XXXX");
#   2. der Boot-/CONTINUE-Haken hat das Prop angelegt ("[dokumente] Boot-Weg: Prop obj_id=…"),
#      mit gesetztem Bit NICHT (platform/pc/main.c; Original EIN Raumlader FUN_800396fc,
#      `jal 0x800396fc` @0x8001d5ac LOAD / @0x8001d988 Tuer);
#   3. im Framedump liegt um die projizierte Dokument-Mitte (unit_r34n_e_dokumente Teil V/M)
#      eine Mindestzahl Pixel, die sich zwischen beiden Laeufen unterscheidet — das Dokument ist
#      also GEZEICHNET: Prop-Lader-Haken (MD1+TIM), Regions-Test, Licht und in 1010 Cut 0 die
#      Tiefen-Klemme (ohne sie verdecken die nachgezeichneten Tischmasken die ferne Blatthaelfte).
# Kamera: RE15_FORCE_CUT = Cut des Nutzerbilds (reiner Mess-Schalter; die Kamera-Logik ist
# nicht Gegenstand dieses Riegels).
# Dossier: analysis/befunde_runde34_nacht/E_dokumente.md (6.2, 9).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r34n_e_karte>
#               -DRE15_BILD_TOOL=<probe_r34n_e_bild> -DWORKDIR=<dir> -P test_r34n_e_dokumente_bild.cmake

if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "dokumente_bild: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "dokumente_bild: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT RE15_BILD_TOOL OR NOT EXISTS "${RE15_BILD_TOOL}")
    message(FATAL_ERROR "dokumente_bild: RE15_BILD_TOOL fehlt: '${RE15_BILD_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "dokumente_bild: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

function(hexof _text _out)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(${_out} "${_h}" PARENT_SCOPE)
endfunction()
hexof("[dokumente] Boot-Weg: Prop" _hex_boot)
hexof("EXIT_AT: Bild" _hex_exit)

set(_bild 100)
math(EXPR _ende "${_bild} + 1")

# _raum _pos _cut _genommen -> _out_ppm, _out_boot (1 = Boot-Weg-Zeile gefunden)
function(bild_lauf _name _raum _pos _cut _genommen _out_ppm _out_boot)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_karg "")
    if(_genommen)
        set(_karg "genommen")
    endif()
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}" ${_karg}
                            "pos=${_pos}" "cut=${_cut}"
                    WORKING_DIRECTORY "${WORKDIR}"
                    TIMEOUT 60
                    RESULT_VARIABLE _rvk
                    OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "dokumente_bild[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 180
        RE15_NO_INTRO=1
        RE15_NOAUDIO=1
        SDL_AUDIODRIVER=dummy
        RE15_WINDOW_SCALE=3
        RE15_CONTINUE_TEST=1
        RE15_CARD_AUTO=1
        RE15_CARD_SLOT=0
        RE15_FORCE_CUT=${_cut}
        "RE15_FRAMEDUMP=${_bild}:bild.ppm"
        "RE15_EXIT_AT=${_ende}#${_raum}"
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "dokumente_bild[${_name}]: kein debug.log (exit=${_rv})")
    endif()
    file(READ "${WORKDIR}/debug.log" _lh HEX)
    hexof("CONTINUE: resumed in room ${_raum}" _hex_continue)
    string(FIND "${_lh}" "${_hex_continue}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "dokumente_bild[${_name}]: der Lauf hat den Spielstand in ROOM${_raum} "
                            "nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    if(NOT EXISTS "${WORKDIR}/bild.ppm")
        message(FATAL_ERROR "dokumente_bild[${_name}]: kein Framedump von Bild ${_bild} (exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_lh}" "${_hex_exit}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "dokumente_bild[${_name}]: Bild ${_ende} in ROOM${_raum} nicht erreicht "
                            "(keine EXIT_AT-Zeile, exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_lh}" "${_hex_boot}" _p)
    if(_p LESS 0)
        set(${_out_boot} 0 PARENT_SCOPE)
    else()
        set(${_out_boot} 1 PARENT_SCOPE)
    endif()
    set(${_out_ppm} "${WORKDIR}/bild.ppm" PARENT_SCOPE)
endfunction()

# Name  Raum  Spielerlage (fern vom Dokument, verdeckt es nicht)  Cut  Fenster (960x720)  Mindestzahl
set(_faelle
    "d1|1050|14900,-6750,0|3|348|387|468|467|MIN_D1"
    "d2|1000|18250,-11723,0|0|519|476|639|556|MIN_D2"
    "d3|1020|-16000,-11000,0|6|409|276|529|356|MIN_D3"
    "d3c3|1020|-16000,-11000,0|3|375|254|495|334|MIN_D3C3"
    "d4|1010|3400,7200,1024|0|570|470|690|550|MIN_D4")
# Mindestzahlen = rund die Haelfte der gemessenen Pixel (Kalibrierlauf 2026-09-30, Dossier 9.8:
# d1 283, d2 783, d3 228, d3c3 51, d4 1603; ausserhalb der Fenster jeweils 0 — die zwei Laeufe
# unterscheiden sich NUR im Dokument). d3c3 ist der Riegel fuer die Tiefen-Klemme im Prop-Zeichner
# (main.c, re15_dokumente_sort_max_mit): ROOM1020 Cut 3 liegt die Original-Tischmaske Tiefe 258
# (@0xD28, Maske 35) ueber dem ganzen Blatt — ohne Klemme 0 Pixel (Mutationsprobe M1, Dossier 9.8).
set(MIN_D1 140)
set(MIN_D2 390)
set(MIN_D3 110)
set(MIN_D3C3 25)
set(MIN_D4 800)

set(_fehler "")
foreach(_f ${_faelle})
    string(REPLACE "|" ";" _t "${_f}")
    list(GET _t 0 _n)
    list(GET _t 1 _raum)
    list(GET _t 2 _pos)
    list(GET _t 3 _cut)
    list(GET _t 4 _x0)
    list(GET _t 5 _y0)
    list(GET _t 6 _x1)
    list(GET _t 7 _y1)
    list(GET _t 8 _minvar)
    set(_min ${${_minvar}})
    bild_lauf(${_n}_mit  ${_raum} "${_pos}" ${_cut} FALSE _ppm_mit  _boot_mit)
    bild_lauf(${_n}_ohne ${_raum} "${_pos}" ${_cut} TRUE  _ppm_ohne _boot_ohne)
    if(NOT _boot_mit EQUAL 1)
        string(APPEND _fehler "${_n} ROOM${_raum}: keine Zeile '[dokumente] Boot-Weg' im Lauf MIT Dokument "
                              "(Boot-/CONTINUE-Haken in main.c fehlt?)\n")
    endif()
    if(NOT _boot_ohne EQUAL 0)
        string(APPEND _fehler "${_n} ROOM${_raum}: Boot-Weg-Zeile trotz Genommen-Bit\n")
    endif()
    execute_process(COMMAND "${RE15_BILD_TOOL}" "${_ppm_mit}" "${_ppm_ohne}" ${_x0} ${_y0} ${_x1} ${_y1} ${_min}
                    TIMEOUT 60
                    RESULT_VARIABLE _rvb
                    OUTPUT_VARIABLE _outb)
    message(STATUS "dokumente_bild ${_n} ROOM${_raum} Cut ${_cut}: ${_outb}")
    if(NOT _rvb EQUAL 0)
        string(APPEND _fehler "${_n} ROOM${_raum} Cut ${_cut}: Dokument nicht (genug) gezeichnet: ${_outb}\n")
    endif()
endforeach()

if(_fehler)
    message(FATAL_ERROR "dokumente_bild: das SICHTBARE Ergebnis stimmt nicht.\n${_fehler}")
endif()
message(STATUS "dokumente_bild: alle vier Dokumente am Lade-Weg angelegt und gezeichnet")
