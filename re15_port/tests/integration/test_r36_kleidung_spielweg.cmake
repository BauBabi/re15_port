# =============================================================================
# BLUTIGE KLEIDUNG AUF DEM ECHTEN SPIELWEG (Runde 36, Spur B "kleidung").
#
# Dossier analysis/befunde_runde36/B_kleidung.md §5.3. Der Nutzer-Befund war: "Leons Kleidung hat
# keine Blutflecken/Risse mehr, wenn er auf Warning/Danger runter kommt" — Ursache: die Stempel
# hingen am RE1.5-Zombie-Code, das RE2-Standardgehirn (seit e11962ec) stempelte nie. Dieser Riegel
# faehrt den WEG des Nutzers mit der echten exe, unter dem Standardgehirn (RE2), ohne Sprung:
#
#   Lauf 1  CONTINUE aus einem Spielstand neben dem Briefing-Zombie (ROOM1140, hp 100), CROSS-Hammern
#           -> drei ueberlebte Griffe (Abwurf @0x80102968 -> Phase 4 @0x8010a4e8 -> Helper
#           0x801201b8[dir]): hp 100 -> 70 -> 50 -> 20 (= Caution/"Warning", naechster Treffer Danger),
#           beim 3. Abwurf erreicht Panel 7 150 >= 0x78 (@0x80037f14) -> Stufe 1; danach Drehung zur
#           Kamera und der Save-Haken (RE15_SAVE_TEST_AGAIN) schreibt den Spielstand — das SPIEL
#           schreibt ihn, mit der Wund-Tabelle aus dem Griff (v5 wounds[8][2]).
#   Lauf 2  CONTINUE aus genau diesem Spielstand -> Framedump Bild 60 ("nachher").
#   Lauf 3  CONTINUE aus demselben Spielstand mit genullter Wund-Tabelle (Werkzeug `nullen`, Rest
#           bytegleich: Position, Blick, HP, Cut, Inventar) -> Framedump Bild 60 ("vorher").
#
# Mass: debug.log von Lauf 1 traegt die [kleidung]-Zeilen (RE15_KLEIDUNG_LOG) mit hp und Panels;
# `nullen` meldet Exit 3, wenn der Spielstand KEINE Wunde traegt; die Pixel-Differenz vorher/nachher
# liegt vollstaendig im Figurenkasten x150..240 / y80..210 (gemessen 2026-10-10: 121 Pixel in
# (176,103)-(190,125) = Weste/Brust; Schwelle = halber Messwert).
#
# SDL_ASSERT=always_ignore (Nachbesserung 1, M2): SDL-eigener Assertion-Handler, kein Spielverhalten —
# in einer RDP-Sitzung blieb jede exe an WIN_AddDisplay (SDL_windowsmodes.c:380) stehen (B_abnahme_0.md §5).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<test_r36_kleidung> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r36_kleidung_spielweg: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r36_kleidung_spielweg: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r36_kleidung_spielweg: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/weg_${_lauf_id}")

# Eigener exe-Name (Muster test_r34n_b_cursor.cmake: local_build.sh raeumt re15_pc.exe-Prozesse ab).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r36b_weg_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r36b_weg_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

# ---- Lauf 1: der Spielweg --------------------------------------------------------------------
set(WORKDIR "${_basis}_griffe")
file(MAKE_DIRECTORY "${WORKDIR}")
execute_process(COMMAND "${RE15_KARTE_TOOL}" karte "re15_card.mcr" 1140 0 -1400 0 -19600 0 100 "-" 1
                WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _ok)
if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
    message(FATAL_ERROR "r36_kleidung_spielweg: Kartenwerkzeug exit=${_rvk}\n${_ok}")
endif()
# CROSS-Hammern (1 Bild Taste, 1 Bild Pause) ab Spielbild 60 fuer 176 Bilder (= drei Griffe, Abwuerfe
# F97/F163/F229), dann 36 Bilder RECHTS (Drehung 48/Bild -> Blick zur Kamera von Cut 1), Save-Haken F300.
set(_skript "")
foreach(_i RANGE 87)
    string(APPEND _skript "X0.034,W0.034,")
endforeach()
string(APPEND _skript "R1.2,W5")
re15_start_spiel(_rv 240
    SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
    RE15_KLEIDUNG_LOG=1
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=${_skript}"
    RE15_SAVE_TEST_AGAIN=300 RE15_SAVE_TEST_EXIT_AFTER=1
    "RE15_EXIT_AT=600#1140"
    "${_exe}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r36_kleidung_spielweg[griffe]: kein debug.log (exit=${_rv})")
endif()
file(STRINGS "${WORKDIR}/debug.log" _kleid REGEX "^\\[kleidung\\] F[0-9]+ abwurf")
list(LENGTH _kleid _n_abwurf)
message(STATUS "r36_kleidung_spielweg[griffe] exit=${_rv} Abwuerfe=${_n_abwurf}")
foreach(_z IN LISTS _kleid)
    message(STATUS "  ${_z}")
endforeach()
if(_n_abwurf LESS 3)
    message(FATAL_ERROR "r36_kleidung_spielweg[griffe]: weniger als drei ueberlebte Griffe (${_n_abwurf}) — Skript/Spawn geaendert?")
endif()
# Der dritte Abwurf muss die Stufe gesetzt haben (gen springt, Panel 7 = 1/0 im Log).
list(GET _kleid 2 _dritter)
if(NOT _dritter MATCHES "gen=2 ")
    message(FATAL_ERROR "r36_kleidung_spielweg[griffe]: der 3. Abwurf stempelt nicht (erwartet gen=2): ${_dritter}")
endif()
file(STRINGS "${WORKDIR}/debug.log" _gespeichert REGEX "^\\[save\\] saved \\(room 1140\\)")
if(NOT _gespeichert)
    message(FATAL_ERROR "r36_kleidung_spielweg[griffe]: der Save-Haken hat keinen Spielstand geschrieben (Leon tot vor Bild 300?)")
endif()
set(_karte_wunden "${WORKDIR}/re15_card.mcr")

# ---- Referenz: derselbe Spielstand, Wunden genullt ------------------------------------------
set(_karte_null "${_basis}_null.mcr")
file(COPY_FILE "${_karte_wunden}" "${_karte_null}")
execute_process(COMMAND "${RE15_KARTE_TOOL}" nullen "${_karte_null}" 0
                TIMEOUT 60 RESULT_VARIABLE _rvn OUTPUT_VARIABLE _on)
message(STATUS "r36_kleidung_spielweg[nullen] exit=${_rvn} ${_on}")
if(_rvn EQUAL 3)
    message(FATAL_ERROR "r36_kleidung_spielweg: der vom Spiel geschriebene Spielstand traegt KEINE Wunde")
elseif(NOT _rvn EQUAL 0)
    message(FATAL_ERROR "r36_kleidung_spielweg: nullen fehlgeschlagen (exit=${_rvn})")
endif()

# ---- Laeufe 2/3: CONTINUE + Framedump Bild 60 --------------------------------------------------
function(weg_bild _name _karte _out_bild)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(COPY_FILE "${_karte}" "${WORKDIR}/re15_card.mcr")
    re15_start_spiel(_rv 200
        SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        "RE15_FRAMEDUMP=60:bild.ppm"
        "RE15_EXIT_AT=70#1140"
        "${_exe}")
    if(NOT EXISTS "${WORKDIR}/bild.ppm")
        message(FATAL_ERROR "r36_kleidung_spielweg[${_name}]: Framedump fehlt (exit=${_rv})")
    endif()
    message(STATUS "r36_kleidung_spielweg[${_name}] exit=${_rv}")
    set(${_out_bild} "${WORKDIR}/bild.ppm" PARENT_SCOPE)
endfunction()

weg_bild(nachher "${_karte_wunden}" _nachher)
weg_bild(vorher  "${_karte_null}"   _vorher)

execute_process(COMMAND "${RE15_KARTE_TOOL}" bild "${_vorher}" "${_nachher}" 60 150 80 240 210
                TIMEOUT 60 RESULT_VARIABLE _rvb OUTPUT_VARIABLE _ob)
message(STATUS "r36_kleidung_spielweg[bild] ${_ob}")
if(NOT _rvb EQUAL 0)
    message(FATAL_ERROR "r36_kleidung_spielweg: Bildvergleich vorher/nachher fehlgeschlagen (exit=${_rvb})\n${_ob}")
endif()

file(REMOVE "${_exe}")
message(STATUS "r36_kleidung_spielweg: OK — drei ueberlebte RE2-Griffe (hp 100 -> 20) stempeln Leons Weste, der Spielstand traegt die Wunde, CONTINUE zeigt sie")
