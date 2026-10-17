# =============================================================================
# BLUTIGE KLEIDUNG AUF DEM HUNDEWEG ROOM1190 (Runde 36, Spur B "kleidung", Nachbesserung 1).
#
# Dossier analysis/befunde_runde36/B_kleidung.md "Nachbesserung 1"; Mangel M1 der Abnahme 0
# (B_abnahme_0.md §5.1): auf dem dokumentierten Weg des Nutzers (Zwingerhunde ROOM1190, RE2-
# Standardgehirn) ging Leon ueber Caution und Danger in den Tod, ohne dass je ein Decal entstand —
# der RE2-Biss kostet 20 HP (`addiu a0,zero,20` @0x80104EB8), bekam aber nur EINEN RE1.5-Stempel.
# Massstab des Originals: RE1.5-Biss = 10 HP (`addiu a0,v0,-10` @0x8010f2c8) + EIN Stempel +0x32
# (@0x8010f360/64) -> je 10 HP ein Stempel -> der RE2-Biss stempelt zweimal (Log "dir=a+b").
#
#   Lauf 1  neues Spiel (Titel-Haken), Debug-Menue-Sprung ROOM1190 @240, Hunde-Sub 13 @450, Wecken
#           @500 (Weg der Abnahme), Leon steht ohne Eingabe: Bisse F582/F585/F587 (hp 80/60/40);
#           Panel 4 erreicht beim 2. Biss 0x78 (@0x80037f14) -> Stufe 1 bei hp 60 = VOR Danger
#           (hp < 20, FUN_8004ed6c). Save-Haken @600 -> das SPIEL schreibt den Spielstand.
#           ⛔ Runde 38 Spur B: seit der RE2-Hund die Freigabe-Sperrzeit des Sprung-Ausgangs ehrt (+0x1d6 = 0x78
#           @0x801115e8-ec, Abzug @0x8010dd5c-70; analysis/befunde_runde38/B_szenenschutz.md) beissen die
#           Hunde erst 120 Bilder nach der Landung: gemessen F664/F674/F685 (hp 80/60/40), dann F764 (20) —
#           der Save-Haken steht deshalb bei 700 (gleicher Stand wie vorher bei 600: drei Bisse, hp 40).
#           Die Hunde kommen damit aus einer anderen Richtung: Biss 2 "dir=1+0" -> Panel 5 (Tabelle
#           05 07 04 06 @0x80121014, dir 0 -> 0x05) = Westen-VORDERSEITE statt Panel 4 (Rueckseite). Laeufe
#           2/3 drehen Leon deshalb mit R4 zur Kamera (Brust sichtbar) statt mit L4 weg; gemessen Bild 42:
#           50 Pixel in (168,90)-(174,102), Figurenkasten jetzt x150..200 / y70..150, Schwelle 25 (halber Wert).
#   Lauf 2  CONTINUE aus genau diesem Spielstand, Leon dreht sich (L ab Bild 30) -> Bild 42 (Ruecken
#           zur Kamera; Panel 4 liegt auf der Westen-Rueckseite).
#   Lauf 3  derselbe Spielstand mit genullter Wund-Tabelle (Werkzeug `nullen`) -> Bild 42.
#
# Mass (gemessen 2026-10-10): Lauf 1 Log "[kleidung] F585 hundebiss typ=0x20 dir=3+2 hp=60 gen=2
# panels=...,1/0,..." + "[wound] sync: applied=1"; Bild 42: 27 Pixel in (196,96)-(207,109), 0
# ausserhalb der Figur. Schwelle = halber Messwert (13), Figurenkasten x170..240 / y80..170.
#
# SDL_ASSERT=always_ignore: SDL-eigener Assertion-Handler (kein Spielverhalten) — in einer RDP-
# Sitzung blieb jede exe an WIN_AddDisplay (SDL_windowsmodes.c:380) stehen (B_abnahme_0.md §5).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<test_r36_kleidung> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r36_kleidung_hundeweg: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r36_kleidung_hundeweg: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r36_kleidung_hundeweg: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/hund_${_lauf_id}")

# Eigener exe-Name (Muster test_r34n_b_cursor.cmake: local_build.sh raeumt re15_pc.exe-Prozesse ab).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r36b_hund_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r36b_hund_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

# ---- Lauf 1: der Hundeweg -------------------------------------------------------------------
set(WORKDIR "${_basis}_bisse")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rv 240
    SDL_ASSERT=always_ignore
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_KLEIDUNG_LOG=1
    RE15_DEBUG_JUMP=1190@240 RE15_DEBUG_SUB=13@450 RE15_DEBUG_DOGWAKE=500
    RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
    RE15_SAVE_TEST_AGAIN=700 RE15_SAVE_TEST_EXIT_AFTER=1   # Runde 38 Spur B: 600 -> 700 (Sperrzeit 0x78)
    RE15_EXIT_AT=1000
    "${_exe}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r36_kleidung_hundeweg[bisse]: kein debug.log (exit=${_rv})")
endif()
file(STRINGS "${WORKDIR}/debug.log" _bisse REGEX "^\\[kleidung\\] F[0-9]+ hundebiss")
list(LENGTH _bisse _n_bisse)
message(STATUS "r36_kleidung_hundeweg[bisse] exit=${_rv} Bisse=${_n_bisse}")
foreach(_z IN LISTS _bisse)
    message(STATUS "  ${_z}")
endforeach()
if(_n_bisse LESS 2)
    message(FATAL_ERROR "r36_kleidung_hundeweg[bisse]: weniger als zwei RE2-Bisse vor dem Save (${_n_bisse}) — Hunde-Weg geaendert?")
endif()
# Massstab: jeder 20-HP-Biss traegt ZWEI RE1.5-Stempel (je 10 HP einer, @0x8010f2c8 / @0x8010f364).
set(_decal_hp "")
foreach(_z IN LISTS _bisse)
    if(NOT _z MATCHES " dir=[0-3]\\+[0-3] ")
        message(FATAL_ERROR "r36_kleidung_hundeweg[bisse]: Biss ohne genau zwei Stempel (Massstab 0x32 je 10 HP): ${_z}")
    endif()
    # erstes Panel mit Stufe >= 1 ("L/A"-Paare nach panels=, L = Level @0x80037f2c)
    if(_decal_hp STREQUAL "" AND _z MATCHES " hp=(-?[0-9]+) gen=[0-9]+ panels=(.*)$")
        set(_hp "${CMAKE_MATCH_1}")
        set(_panels ",${CMAKE_MATCH_2}")
        if(_panels MATCHES ",[12]/")
            set(_decal_hp "${_hp}")
        endif()
    endif()
endforeach()
if(_decal_hp STREQUAL "")
    message(FATAL_ERROR "r36_kleidung_hundeweg[bisse]: kein Biss vor dem Save hebt ein Panel ueber 0x78 (gen bleibt 1) — der Mangel M1")
endif()
# Danger = hp < 20 (FUN_8004ed6c: >= 0x50 Fine, >= 0x14 Caution, sonst Danger)
if(_decal_hp LESS 20)
    message(FATAL_ERROR "r36_kleidung_hundeweg[bisse]: das erste Decal kommt erst bei hp ${_decal_hp} (Danger), nicht davor")
endif()
file(STRINGS "${WORKDIR}/debug.log" _sync REGEX "^\\[wound\\] sync: applied=1 ok=1")
if(NOT _sync)
    message(FATAL_ERROR "r36_kleidung_hundeweg[bisse]: Decal-Stufe gesetzt, aber nie auf die Textur geblittet ([wound] sync fehlt)")
endif()
file(STRINGS "${WORKDIR}/debug.log" _gespeichert REGEX "^\\[save\\] saved \\(room 1190\\)")
if(NOT _gespeichert)
    message(FATAL_ERROR "r36_kleidung_hundeweg[bisse]: der Save-Haken hat keinen Spielstand geschrieben (Leon tot vor Bild 700?)")
endif()
message(STATUS "r36_kleidung_hundeweg[bisse]: erstes Decal bei hp ${_decal_hp} (vor Danger)")
set(_karte_wunden "${WORKDIR}/re15_card.mcr")

# ---- Referenz: derselbe Spielstand, Wunden genullt ------------------------------------------
set(_karte_null "${_basis}_null.mcr")
file(COPY_FILE "${_karte_wunden}" "${_karte_null}")
execute_process(COMMAND "${RE15_KARTE_TOOL}" nullen "${_karte_null}" 0
                TIMEOUT 60 RESULT_VARIABLE _rvn OUTPUT_VARIABLE _on)
message(STATUS "r36_kleidung_hundeweg[nullen] exit=${_rvn} ${_on}")
if(_rvn EQUAL 3)
    message(FATAL_ERROR "r36_kleidung_hundeweg: der vom Spiel geschriebene Spielstand traegt KEINE Wunde")
elseif(NOT _rvn EQUAL 0)
    message(FATAL_ERROR "r36_kleidung_hundeweg: nullen fehlgeschlagen (exit=${_rvn})")
endif()

# ---- Laeufe 2/3: CONTINUE, Drehung, Framedump Bild 42 ------------------------------------------
function(hund_bild _name _karte _out_bild)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(COPY_FILE "${_karte}" "${WORKDIR}/re15_card.mcr")
    re15_start_spiel(_rv 200
        SDL_ASSERT=always_ignore
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=30 "RE15_INPUT_SCRIPT=R4,W5"   # Runde 38 Spur B: L4 -> R4 (Panel 5 vorn)
        "RE15_FRAMEDUMP=42:bild.ppm"
        "RE15_EXIT_AT=50#1190"
        "${_exe}")
    if(NOT EXISTS "${WORKDIR}/bild.ppm")
        message(FATAL_ERROR "r36_kleidung_hundeweg[${_name}]: Framedump fehlt (exit=${_rv})")
    endif()
    message(STATUS "r36_kleidung_hundeweg[${_name}] exit=${_rv}")
    set(${_out_bild} "${WORKDIR}/bild.ppm" PARENT_SCOPE)
endfunction()

hund_bild(nachher "${_karte_wunden}" _nachher)
hund_bild(vorher  "${_karte_null}"   _vorher)

execute_process(COMMAND "${RE15_KARTE_TOOL}" bild "${_vorher}" "${_nachher}" 25 150 70 200 150
                TIMEOUT 60 RESULT_VARIABLE _rvb OUTPUT_VARIABLE _ob)
message(STATUS "r36_kleidung_hundeweg[bild] ${_ob}")
if(NOT _rvb EQUAL 0)
    message(FATAL_ERROR "r36_kleidung_hundeweg: Bildvergleich vorher/nachher fehlgeschlagen (exit=${_rvb})\n${_ob}")
endif()

file(REMOVE "${_exe}")
message(STATUS "r36_kleidung_hundeweg: OK — RE2-Hundebisse stempeln im RE1.5-Massstab (2 x 0x32 je 20 HP), Decal bei hp ${_decal_hp} vor Danger, der Spielstand traegt es, CONTINUE zeigt es auf der Westen-Rueckseite")
