# =============================================================================
# IRONS-TISCH-BILD-PIN (Runde 30, Thema irons-diary-welt; Nachbesserung nach dem Gegenpruefer).
#
# BEFUND DES GEGENPRUEFERS (Mutationsprobe in einer Kopie): die zwei Klemmzeilen im
# Prop-Zeichner (platform/pc/main.c, Dreiecke "wz_for_sort" und Vierecke "wz_avg") entfernt ->
# Buch und Memory Card in Cut 2 UNSICHTBAR (0 Pixel gegen das Nullbild statt 674), aber
# integration_r30_irons_tisch_laden blieb gruen (rc=0): es sucht nur die Logzeile
# "[prop-render] pi=5/6", und die steht VOR der Klemme. unit_r30_irons_tisch Teil K prueft nur
# den Rueckgabewert von re15_irons_tisch_sort_max und linkt main.c nicht. Kein Riegel hielt
# das SICHTBARE Ergebnis fest.
#
# WARUM ES DIE KLEMME GIBT (Port-Zusatz, include/re15_irons_tisch.h): ueber beiden Ablagen
# liegen in Cut 2 drei Original-Masken der Tiefe 87,
#     ROOM1150.RDT (und ROOM1151.RDT) @0x006E8 / @0x006F4 / @0x00714, je +4 = 57 00,
# Masken und Objekte teilen EINE Ordnungstabelle (Maske `lh a0,2(s3)` @0x80039650,
# `sll a0,a0,2` @0x80039658; Objekt otz >> 4 @0x8002565c), die Props liegen bei Bucket 92.
#
# DIESER RIEGEL faehrt die ECHTE exe viermal am Lade-Weg (CONTINUE, ROOM1150, Cut 2
# erzwungen) und liest je EIN Bild ueber RE15_FRAMEDUMP (Readback des fertig komponierten
# Frames VOR SDL_RenderPresent, beschleunigter Renderer - KEIN RE15_SOFTWARE_RENDER, KEIN
# RE15_AUTOSHOT):
#   P   nichts genommen,                Spieler weit weg      (-20500, -24500) rot 0
#   P0  beides genommen (Bits (9,54)/(9,55) im Spielstand), Spieler weit weg
#   S   nichts genommen,                Spieler vor dem Tisch (-22664, -18450) rot 2048
#   S0  beides genommen,                Spieler vor dem Tisch
# probe_r30_irons_tisch_bild verlangt:
#   - Buch: > 0 Pixel (P != P0) in der ROTEN Marke des Nutzerbilds (x 146..158, y 120..132)
#   - Karte: > 0 Pixel in der BLAUEN Marke (x 135..144, y 121..131)
#   - 0 Prop-Pixel ausserhalb der beiden Marken
#   - Figur vor dem Tisch: Ueberschneidung > 0, darin ueberall S == S0 (Figur obenauf)
#
# ER DISKRIMINIERT (gemessen, Dossier UMSETZUNG 9.6): Klemme entfernt -> 0 Pixel in beiden
# Marken -> rot; Klemme auf 0 (Props vor allem) -> Prop obenauf in der Ueberschneidung -> rot.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r30_irons_tisch_karte>
#               -DRE15_BILD_TOOL=<probe_r30_irons_tisch_bild> -DWORKDIR=<dir>
#               -P test_r30_irons_tisch_bild.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "irons_tisch_bild: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "irons_tisch_bild: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT RE15_BILD_TOOL OR NOT EXISTS "${RE15_BILD_TOOL}")
    message(FATAL_ERROR "irons_tisch_bild: RE15_BILD_TOOL fehlt: '${RE15_BILD_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "irons_tisch_bild: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Eigenes Arbeitsverzeichnis je Aufruf (ein Rest des vorigen Aufrufs haelt sonst
# debug.log gesperrt).
string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

function(hexof _text _out)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(${_out} "${_h}" PARENT_SCOPE)
endfunction()
hexof("CONTINUE: resumed in room 1150" _hex_continue)
hexof("-> bild.ppm" _hex_dump)
hexof("EXIT_AT: Bild" _hex_exit)

# Bild 100 im Raum: der Raum steht (Lade-Weg-Laeufe der Abnahme: F100 = F150 = ... = F300,
# 674 Prop-Pixel in jedem dieser Bilder).
set(_bild 100)
math(EXPR _ende "${_bild} + 1")

function(bild_lauf _name _pos _genommen _out_ppm)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_karg "")
    if(_genommen)
        set(_karg "genommen")
    endif()
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "1150" ${_karg} "pos=${_pos}"
                    WORKING_DIRECTORY "${WORKDIR}"
                    TIMEOUT 60
                    RESULT_VARIABLE _rvk
                    OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "irons_tisch_bild[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 180
        RE15_NO_INTRO=1
        RE15_NOAUDIO=1
        RE15_WINDOW_SCALE=3
        RE15_CONTINUE_TEST=1
        RE15_CARD_AUTO=1
        RE15_CARD_SLOT=0
        RE15_IRONS_LOG=1
        RE15_FORCE_CUT=2
        "RE15_FRAMEDUMP=${_bild}:bild.ppm"
        "RE15_EXIT_AT=${_ende}#1150"
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "irons_tisch_bild[${_name}]: kein debug.log (exit=${_rv})")
    endif()
    file(READ "${WORKDIR}/debug.log" _lh HEX)
    string(FIND "${_lh}" "${_hex_continue}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "irons_tisch_bild[${_name}]: der Lauf hat den Spielstand in "
                            "ROOM1150 nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_lh}" "${_hex_dump}" _p)
    if(_p LESS 0 OR NOT EXISTS "${WORKDIR}/bild.ppm")
        message(FATAL_ERROR "irons_tisch_bild[${_name}]: kein Framedump von Bild ${_bild} "
                            "(exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_lh}" "${_hex_exit}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "irons_tisch_bild[${_name}]: Bild ${_ende} in ROOM1150 nicht "
                            "erreicht (keine EXIT_AT-Zeile, exit=${_rv}, ${WORKDIR})")
    endif()
    set(${_out_ppm} "${WORKDIR}/bild.ppm" PARENT_SCOPE)
endfunction()

bild_lauf(P  "-20500,-24500,0"    FALSE _ppm_p)
bild_lauf(P0 "-20500,-24500,0"    TRUE  _ppm_p0)
bild_lauf(S  "-22664,-18450,2048" FALSE _ppm_s)
bild_lauf(S0 "-22664,-18450,2048" TRUE  _ppm_s0)

execute_process(COMMAND "${RE15_BILD_TOOL}" "${_ppm_p}" "${_ppm_p0}" "${_ppm_s}" "${_ppm_s0}"
                TIMEOUT 60
                RESULT_VARIABLE _rvb
                OUTPUT_VARIABLE _outb)
message(STATUS "irons_tisch_bild: Auswertung\n${_outb}")
if(NOT _rvb EQUAL 0)
    message(FATAL_ERROR
        "irons_tisch_bild: das SICHTBARE Ergebnis stimmt nicht (Auswerter exit=${_rvb}).\n"
        "Buch und Memory Card muessen in Cut 2 an den Marken des Nutzerbilds zu sehen sein, "
        "und eine Figur vor dem Tisch muss ueber ihnen liegen. Ohne die Tiefen-Klemme im "
        "Prop-Zeichner (main.c, re15_irons_tisch_sort_max) verdecken die Tischmasken der "
        "Tiefe 87 (ROOM1150.RDT @0x006E8/@0x006F4/@0x00714) beide.\n${_outb}")
endif()

message(STATUS "irons_tisch_bild: OK - Buch und Karte sind in Cut 2 an den Marken sichtbar, "
               "die Figur vor dem Tisch liegt darueber")
