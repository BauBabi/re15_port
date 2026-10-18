cmake_minimum_required(VERSION 3.16)
# =============================================================================
# SCHLUESSEL AN DER PINNWAND ROOM10F0/10F1 "KLEINER/SUBTILER" MIT DER ECHTEN EXE (Runde 39, Spur L, Punkt 2).
#
# Dossier analysis/befunde_runde39/L_optik.md (Punkt 2). Nutzer (AUFTRAG.md): "Der Schluessel im Raum 10F0 und 10F1
# sollte schon kleiner/subtiler sein, wie er dran haengt, nicht so riesig." FORM (RE2 = Vorbild): die gezeichnete Huelle
# im Nutzer-Cut (Cut 5) ist so breit, wie RE2 dasselbe Modell an seinen Platzierungen im Mittel zeigt (Median L_nat*H/vz:
# Parking Lot Key 4,07 px, Prison Key 4,26 px; gen/itemorte_r38_lage.inc RE15_R39L_*). Runde 38: 12 Spalten (die ganze
# Nutzer-Marke x 133..143).
#
# Laeufe (Bild 150, Pinnwand-Fenster der Runde-38-Abnahme x 131..145 y 75..87, bei Skala 3 x 393..437 y 225..263):
#   leon_s1  / leon_s3   ROOM10F0 Cut 5, Sprung (RE15_DEBUG_JUMP, Flags (9,71)+(3,94) wie Runde 38), mit / ohne (Bit (9,82))
#   elza_s1  / elza_s3   ROOM10F1 Cut 5 ALS ELZA (RE15_PSELECT_AUTO + _SWITCH, debug.log "[pl] Spieler-Familie PL04"),
#                        mit / ohne (Bit (9,83))
#   Mass: probe_r38_itemorte_kontrast (Pixel mit |dR|+|dG|+|dB| > 30 im Fenster, Huelle, AUSSEN) ->
#     KLEINER:  Huellenbreite <= 6 px (Skala 1) bzw. <= 18 px (Skala 3)   [vorher 12 / 36]
#     SICHTBAR: Pixel >= Mindestzahl (gemessen 2026-10-18, s. unten), AUSSEN == 0 (nichts anderes aendert sich)
#   RUNDE 39 SPUR M (NUTZER-VORGABE Nachtrag 5 "maximal halb so gross", analysis/befunde_runde39/M_schluessel2.md): seitdem
#     Skala 3 Huellenbreite <= 7 (Leon) / 8 (Elza) = halbe Spur-L-Messung 14 / 16; Mindestpixel ~60 % der neuen Messung
#     (Leon 9, Elza 16 Pixel). Skala 1: das GTE-Viereck hat nur noch 2 x 1 / 3 x 2 Bildpunkte (ganzzahlige Ecken,
#     FUN_80022f0c @0x80022fd8), die Bilder mit/ohne Schluessel sind dort bitgleich — gemessen 0 Pixel; geprueft wird dort
#     nur noch, dass sich ausserhalb des Fensters nichts aendert (Mindestzahl 0). Breite/Hoehe beider Stufen misst
#     integration_r39_schluessel2.
#   aufnahme_leon / aufnahme_elza  Druck vor der Pinnwand (1300,-2900) Blick -z: "[schluessel] ... genommen" im debug.log
#                        (Aufhebe-Zone Slot 8 unveraendert).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KONTRAST_TOOL=<probe_r38_itemorte_kontrast> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
foreach(_v RE15_PC_EXE RE15_KONTRAST_TOOL)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r39_optik_schluessel: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "r39_optik_schluessel: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; Praefix nur DIESES Tests). Endung wie die Quelle.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r39l_schl_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r39l_schl_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(_basis_wd "${WORKDIR}")
set(_fehler "")

# Leon-Laeufe: RE15_TITLE_SHOT_AF=2 schaltet den Titel weiter (wie test_r37_pri1100_marke.cmake); Elza-Laeufe steuern den
# Titel ueber RE15_PSELECT_AUTO (dort KEIN RE15_TITLE_SHOT, sonst bleibt es Leon - gemessen 2026-10-18).
set(_titel "RE15_TITLE_SHOT=title.bmp;RE15_TITLE_SHOT_AF=2")
set(_leon "${_titel};RE15_DEBUG_JUMP=10F0@250;RE15_PLAYER_POS=0,4000,1024;RE15_FORCE_CUT=5;RE15_SET_FLAG=9:71,3:94;RE15_EXIT_AT=150#10F0")
set(_leon_ohne "${_titel};RE15_DEBUG_JUMP=10F0@250;RE15_PLAYER_POS=0,4000,1024;RE15_FORCE_CUT=5;RE15_SET_FLAG=9:71,3:94,9:82;RE15_EXIT_AT=150#10F0")
set(_elza "RE15_TITLE_CONFIRM_MS=1500;RE15_PSELECT_AUTO=1;RE15_PSELECT_AUTO_SWITCH=1;RE15_GOTO_ROOM=10F1;RE15_PLAYER_POS=0,4000,1024;RE15_FORCE_CUT=5;RE15_EXIT_AT=150#10F1")
set(_elza_ohne "${_elza};RE15_SET_FLAG=9:83")

# Ein Lauf: Framedump Bild 150 nach f.ppm, debug.log merken.
function(lauf _name _skala _env)
    set(WORKDIR "${_basis_wd}/lauf_${_lauf_id}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/f.ppm" "${WORKDIR}/debug.log")
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy "RE15_WINDOW_SCALE=${_skala}"
        "RE15_FRAMEDUMP=150:f.ppm"
        ${_env} ${ARGN}
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/f.ppm" OR NOT EXISTS "${WORKDIR}/debug.log")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r39_optik_schluessel[${_name}]: kein Framedump/debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    set(_ppm_${_name} "${WORKDIR}/f.ppm" PARENT_SCOPE)
    file(READ "${WORKDIR}/debug.log" _dbg)
    set(_dbg_${_name} "${_dbg}" PARENT_SCOPE)
endfunction()

macro(pruef _name _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(_p LESS 0)
        list(APPEND _fehler "${_name}: fehlt '${_muster}'")
        message(STATUS "FAIL ${_name}: fehlt '${_muster}'")
    else()
        message(STATUS "ok   ${_name}: '${_muster}'")
    endif()
endmacro()

# probe_r38_itemorte_kontrast <mit> <ohne> x0 y0 x1 y1 -> "FENSTER x..: N Pixel, SUMDL S, Huelle x%d..%d y%d..%d | AUSSEN: n"
function(mass _name _mit _ohne _x0 _y0 _x1 _y1 _min_px _max_breite)
    execute_process(COMMAND "${RE15_KONTRAST_TOOL}" "${_mit}" "${_ohne}" ${_x0} ${_y0} ${_x1} ${_y1}
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _o TIMEOUT 60)
    string(STRIP "${_o}" _o)
    message(STATUS "r39_optik_schluessel[${_name}]: ${_o}")
    # (Runde 39 M: bei 0 Pixel schreibt das Werkzeug die leere Huelle "x320..-1" -> Vorzeichen zulassen, Breite dann 0)
    if(NOT _o MATCHES "FENSTER [^:]*: ([0-9]+) Pixel, SUMDL ([0-9]+), Huelle x(-?[0-9]+)\\.\\.(-?[0-9]+) y(-?[0-9]+)\\.\\.(-?[0-9]+) \\| AUSSEN: ([0-9]+)")
        set(_fehler "${_fehler};${_name}: Werkzeug-Ausgabe unlesbar '${_o}'" PARENT_SCOPE)
        return()
    endif()
    set(_px "${CMAKE_MATCH_1}")
    if(_px EQUAL 0)
        set(_breite 0)
    else()
        math(EXPR _breite "${CMAKE_MATCH_4} - ${CMAKE_MATCH_3} + 1")
    endif()
    set(_aussen "${CMAKE_MATCH_7}")
    if(_px LESS _min_px)
        set(_fehler "${_fehler};${_name}: nur ${_px} Pixel (Soll >= ${_min_px}) - nicht sichtbar" PARENT_SCOPE)
    endif()
    if(_breite GREATER _max_breite)
        set(_fehler "${_fehler};${_name}: Huelle ${_breite} px breit (Soll <= ${_max_breite}) - nicht kleiner" PARENT_SCOPE)
    endif()
    if(NOT _aussen EQUAL 0)
        set(_fehler "${_fehler};${_name}: ${_aussen} Pixel ausserhalb des Fensters" PARENT_SCOPE)
    endif()
endfunction()

lauf(leon_s1_mit  1 "${_leon}")
lauf(leon_s1_ohne 1 "${_leon_ohne}")
lauf(leon_s3_mit  3 "${_leon}")
lauf(leon_s3_ohne 3 "${_leon_ohne}")
lauf(elza_s1_mit  1 "${_elza}")
lauf(elza_s1_ohne 1 "${_elza_ohne}")
lauf(elza_s3_mit  3 "${_elza}")
lauf(elza_s3_ohne 3 "${_elza_ohne}")
pruef(L1 _dbg_leon_s1_mit "[schluessel] ROOM10F0: PARKING LOT KEY (Item 0x45, Bit (9,82)) Prop obj 4 bei (1358,-2495,-3400) rot (3072,0,2048), Zone Slot 8")
pruef(E1 _dbg_elza_s1_mit "[pl] Spieler-Familie PL04")
pruef(E1 _dbg_elza_s1_mit "[schluessel] ROOM10F1: PRISON KEY (Item 0x42, Bit (9,83)) Prop obj 4 bei (1358,-2495,-3400) rot (1024,0,0), Zone Slot 8")
pruef(E3 _dbg_elza_s3_mit "[pl] Spieler-Familie PL04")

# Spur L: Mindest-Pixel = ~60 % der Messung 2026-10-18 (Leon 7 / 69 Pixel, Elza 5 / 63 bei Skala 1 / 3; Runde 38: 55 / 491
# bzw. 27 / 217); Huelle <= RE2-Quartil aufgerundet + 1 (gemessen 5 / 14 bzw. 4 / 16).
# Runde 39 Spur M (NUTZER-VORGABE "maximal halb so gross", M_schluessel2.md "Messung nachher"): Skala 3 Leon 9 Pixel, Huelle
# 4 x 3, Elza 16 Pixel, 8 x 5 -> Mindest-Pixel 5 / 9 (~60 %), Breite <= 7 / 8 (halbe Spur-L-Messung 14 / 16); Skala 1 0 / 0
# Pixel (GTE-Viereck 2 x 1 / 3 x 2) -> Mindestzahl 0, Breite <= 6 bleibt (leere Huelle = 0).
mass(leon_s1 "${_ppm_leon_s1_mit}" "${_ppm_leon_s1_ohne}" 131 75 145 87 0 6)
mass(leon_s3 "${_ppm_leon_s3_mit}" "${_ppm_leon_s3_ohne}" 393 225 437 263 5 7)
mass(elza_s1 "${_ppm_elza_s1_mit}" "${_ppm_elza_s1_ohne}" 131 75 145 87 0 6)
mass(elza_s3 "${_ppm_elza_s3_mit}" "${_ppm_elza_s3_ohne}" 393 225 437 263 9 8)

# Aufnahme unveraendert: vor der Pinnwand (1300,-2900) Blick -z (rot 1024), Aktion -> Modal -> Ja (Skript wie Runde 38 K1/K2).
# Skript-Zeitachse = Spielbilder (RE15_INPUT_SCRIPT_BASIS=spiel), Start nach dem Sprung (Bild 250) bzw. nach GOTO_ROOM.
lauf(aufnahme_leon 1 "${_titel};RE15_DEBUG_JUMP=10F0@250;RE15_PLAYER_POS=1300,-2900,1024;RE15_FORCE_CUT=5;RE15_SET_FLAG=9:71,3:94;RE15_INPUT_SCRIPT_BASIS=spiel;RE15_INPUT_SCRIPT_START=300;RE15_INPUT_SCRIPT=W1,A0.1,W3.5,A0.1,W4;RE15_EXIT_AT=600#10F0")
pruef(A1 _dbg_aufnahme_leon "[schluessel] PARKING LOT KEY genommen: Item 0x45, Bit (9,82)")
# Elza: RE15_GOTO_ROOM wendet RE15_PLAYER_POS nicht an (Spawn an der Tuer) -> Sprung ueber RE15_DEBUG_JUMP nach der Auswahl
# (bleibt Elza, "[pl] Spieler-Familie PL04"; gemessen 2026-10-18: "[parity] JUMP-Spawn ersetzt durch RE15_PLAYER_POS").
lauf(aufnahme_elza 1 "RE15_TITLE_CONFIRM_MS=1500;RE15_PSELECT_AUTO=1;RE15_PSELECT_AUTO_SWITCH=1;RE15_DEBUG_JUMP=10F1@250;RE15_PLAYER_POS=1300,-2900,1024;RE15_FORCE_CUT=5;RE15_INPUT_SCRIPT_BASIS=spiel;RE15_INPUT_SCRIPT_START=300;RE15_INPUT_SCRIPT=W1,A0.1,W3.5,A0.1,W4;RE15_EXIT_AT=600#10F1")
pruef(A2 _dbg_aufnahme_elza "[pl] Spieler-Familie PL04")
pruef(A2 _dbg_aufnahme_elza "[schluessel] PRISON KEY genommen: Item 0x42, Bit (9,83)")
file(REMOVE "${_exe_kopie}")

if(_fehler)
    string(REPLACE ";" "\n  " _f "${_fehler}")
    message(FATAL_ERROR "r39_optik_schluessel: FEHLER\n  ${_f}")
endif()
message(STATUS "r39_optik_schluessel: OK")
