cmake_minimum_required(VERSION 3.16)
# =============================================================================
# VOLLE HEILUNG = GANZE KLEIDUNG (Runde 39, Spur F "zustand", Nutzerpunkt 1).
#
# Nutzer (AUFTRAG.md Z. 18): "Wenn der Spieler verletzt wurde, hat dann kaputte Klamotten und heilt sich aber
# wieder vollstaendig - zum Beispiel mit Heilspray - dann muss er auch wieder die ganzen Klamotten haben."
# Dossier analysis/befunde_runde39/F_zustand.md (1.1-1.4). NUTZER-VORGABE: das Original loescht die Blut-Decals
# nie (Writer auf 0x800b10ec nur Builder FUN_80037c1c + Akku FUN_80037edc, blood_decals.md); FORM = HP erreichen
# den Wert des Heilsprays (`sh s0`, s0 = 100 @0x8004ae14, + Westen-Bonus) -> Wund-Tabelle nullen + Spielerhaut aus
# der PLD neu laden (= Spieler-Lade-Pfad FUN_800314b0 / FUN_8004ee78 Slot 0x17). Die Mechanik misst
# unit_r39_zustand A; dieser Riegel misst, was nur die exe zeigt: die TEXTUR der Figur nach dem Spray.
#
# Vier Laeufe (ROOM1130 Leon / ROOM1131 Elza, Lage (-1024,0,-3336) rot 856, Cut 2 wie integration_r36_kleidung_bild,
# HP 20, Inventar Platz 0 = Heilspray 0x22), CONTINUE, Spray ueber den ECHTEN Statusschirm (RE15_PAD_AT
# 200 START, 260/300/340 SQUARE = ITEM / Kommandokranz / USE, 500 START; Heilung in Bild 375), Framedumps Bild 150
# (vor dem Spray) und 650 (danach):
#   w  alle 8 Wund-Panels Stufe 2        o  ohne Wunden
# Mass (`test_r39_zustand bild`, Figurenkasten x177..247 / y81..205 wie r36):
#   Bild 150: w vs o  >= 300 Pixel verschieden (die Wunden sind VOR dem Spray sichtbar; gemessen 737 Leon / 462 Elza)
#   Bild 650: w vs o  == 0 Pixel verschieden (nach dem Spray ist die Kleidung ganz; vorher 787 / 603)
#   debug.log: "[r39-zustand] Heilung 0x22: HP 100 >= 100 voll" + "Spielerhaut PL0x.TIM neu geladen".
#
# Parallel-/Linux-fest: eigene exe-Kopie mit Praefix r39fk, eigener Laufordner je Lauf, keine ererbten Umgebungs-
# werte (spiel_lauf.cmake setzt und loescht sie). SDL_ASSERT=always_ignore wie r36.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r39_zustand> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r39_zustand_kleidung: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
    message(FATAL_ERROR "r39_zustand_kleidung: RE15_TOOL fehlt: '${RE15_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r39_zustand_kleidung: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r39fk_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r39fk_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

set(_fehler "")

# Ein Lauf: Spielstand (Werkzeug karte), CONTINUE, Spray per Pad-Skript, Framedumps 150/650.
function(r39fk_lauf _name _raum _chr _wund)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    execute_process(COMMAND "${RE15_TOOL}" karte re15_card.mcr "${_raum}" "${_chr}" -1024 0 -3336 856 20 "${_wund}" "22:1" 2
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r39_zustand_kleidung[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 300
        SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        "RE15_PAD_AT=200:S,260:A,300:A,340:A,500:S"
        "RE15_FRAMEDUMP=150-650/500:fd_"
        "RE15_EXIT_AT=700#${_raum}"
        "${_exe}")
    if(NOT EXISTS "${WORKDIR}/fd_000150.ppm" OR NOT EXISTS "${WORKDIR}/fd_000650.ppm")
        message(FATAL_ERROR "r39_zustand_kleidung[${_name}]: exe rv=${_rv}, Framedumps fehlen in ${WORKDIR}")
    endif()
    message(STATUS "r39_zustand_kleidung[${_name}]: rv=${_rv}, ${WORKDIR}")
endfunction()

# bild <a> <b> <min_diff> <kasten>: rv 0 = Mass erfuellt.
function(r39fk_bild _was _a _b _min)
    execute_process(COMMAND "${RE15_TOOL}" bild "${_a}" "${_b}" "${_min}" 177 81 247 205
                    TIMEOUT 60 RESULT_VARIABLE _rvb OUTPUT_VARIABLE _outb)
    string(STRIP "${_outb}" _outb)
    message(STATUS "r39_zustand_kleidung ${_was}: ${_outb}")
    if(NOT _rvb EQUAL 0)
        set(_fehler "${_fehler}\n  ${_was}: ${_outb}" PARENT_SCOPE)
    endif()
endfunction()

foreach(_fig "leon;1130;0" "elza;1131;4")
    list(GET _fig 0 _n)
    list(GET _fig 1 _raum)
    list(GET _fig 2 _chr)
    r39fk_lauf("${_n}_w" "${_raum}" "${_chr}" "alle:2")
    r39fk_lauf("${_n}_o" "${_raum}" "${_chr}" "-")
    set(_w "${_basis}_${_n}_w")
    set(_o "${_basis}_${_n}_o")
    r39fk_bild("${_n} Bild 150 (Wunden sichtbar, >= 300)" "${_w}/fd_000150.ppm" "${_o}/fd_000150.ppm" 300)
    r39fk_bild("${_n} Bild 650 (nach dem Spray, == 0)"     "${_w}/fd_000650.ppm" "${_o}/fd_000650.ppm" 0)
    file(READ "${_w}/debug.log" _log)
    if(NOT _log MATCHES "\\[r39-zustand\\] Heilung 0x22: HP 100 >= 100 voll")
        set(_fehler "${_fehler}\n  ${_n}: debug.log ohne '[r39-zustand] Heilung 0x22: HP 100 >= 100 voll'")
    endif()
    if(NOT _log MATCHES "\\[r39-zustand\\] Spielerhaut PL0[04]\\.TIM neu geladen")
        set(_fehler "${_fehler}\n  ${_n}: debug.log ohne '[r39-zustand] Spielerhaut PL0x.TIM neu geladen'")
    endif()
endforeach()

file(REMOVE "${_exe}")
if(NOT _fehler STREQUAL "")
    message(FATAL_ERROR "r39_zustand_kleidung: FEHLER${_fehler}")
endif()
message(STATUS "r39_zustand_kleidung: GRUEN (Leon + Elza: Wunden vor dem Spray sichtbar, nach dem Spray 0 Pixel Differenz)")
