cmake_minimum_required(VERSION 3.16)
# =============================================================================
# SPINNEN-PLATZEN IM ECHTLAUF (Runde 41, Spur S "spinnengore").
#
# Nutzer (2026-11-01): "die spinnen platzen bei magnum Schuss z.b. immer noch nicht..." Dossier
# analysis/befunde_runde41/S_spinnengore.md. Gemessen vorher: RE2-Gehirn besitzt jede 0x25, +0x93 = 0x41 beim
# Redhawk-Tod, Teilschicht aus (ta=0). Nachher: Teile 0/19 platzen, alle Beine fliegen (sh 0x80001, fl 0x7fff8).
#
#   ECHTER WEG: Debug-Sprung nach ROOM2070 (Leon) bzw. ROOM2071 (Elza, RE15_TITLE_SHOT_CHAR=1), Waffe per
#   RE15_GIVE/RE15_EQUIP (die Super Redhawk liegt im Spiel in ROOM3050/3051 @RDT 0x1ec6), Zielen + Feuern im Wechsel
#   (Zielhilfe FUN_8003703c waehlt die naechste Spinne). KI = Standard (kein RE15_AI_FLAVOR).
#   Lauf L7    Leon, Redhawk          -> "platzt"
#   Lauf E7    Elza, Redhawk          -> "platzt"
#   Lauf L8    Leon, Remington        -> "bein"   (RE2 trennt mit Zeile 7 ein Bein ab, FUN_80105BF0)
#   Lauf L7AUS Leon, Redhawk, RE15_KEIN_80PROZENT_GORE=1 -> "aus" (Gegenprobe: ohne die Hebung kein Platzen)
#   Nachbesserung 2 (Lache der Leiche, Messschiene RE15_LACHE_LOG aus main.c = was der Zeichner bekommt):
#   Lauf L7    zusaetzlich pruef_lache "re2": Leiche pool=1 rgb=bf10bf, sichtbar bis 2020 x 2120
#              (FUN_80104DE8: 0x00BF10BF @0x80104e7c-80, 90 x 8 @0x80104eac-dc)
#   Lauf E7R15 Elza, Redhawk, RE15_AI_FLAVOR=re15 -> pruef_lache "r15": rgb=9f80df, 2028 x 2128
#              (CORPSE 0x801153d4: 0x00DF809F @0x8011546c-70, 91 x 8 @0x8011541c / @0x801154a8-b8)
#   Nachbesserung 3 (Abnahme 3):
#   Lauf BT    Leon, Redhawk, Messhaken RE15_BABYTOD_TEST=200 -> pruef_flecken: Fleck toter Babys 200 x 200 bf10bf
#              sichtbar=1 gezeichnet=1 (main.c meldet, ob der Renderer das Viereck eingereiht hat; Mangel 2)
#   Lauf K     Leon ROOM10C0, Kraehe Platz 3 (Leon bei 4300/5000, Kraehe greift an, Redhawk-Treffer im Flug), RE15_FEDER_LOG
#              -> pruef_federn gib: 8..11 RE2-Federn (EMOVL21_S0 @0x80102eb8-bc, FUN_80104678), je gespawnt +-512 um die
#              Kraehe, gezeichnete Feder-Quads (glas1120_pc.c, GLAS1090_0C.TIM; Mangel 1)
#   Lauf KE    Elza ROOM10C1, derselbe Weg
#
# Eigener exe-Name je Lauf (Muster test_r41_spinnenanim.cmake), SDL_ASSERT=always_ignore ueber spiel_lauf.cmake.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r41_spinnengore> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_spinnengore: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
    message(FATAL_ERROR "r41_spinnengore: RE15_TOOL fehlt: '${RE15_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_spinnengore: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41s_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r41s_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

set(_fehler "")
set(_skript "M0.3,MA0.1,M0.8,MA0.1,M0.8,MA0.1,M0.8,MA0.1,M0.8,MA0.1,M0.8,MA0.1,M0.8,MA0.1,M0.8,MA0.1,M1,W10")
# L7 (Lache RE2 "sichtbar"): Vorlauf, bevor Leon hebt. Seit r41/zielhilfe (d12b972fb, Nutzer 2026-11-16: Heben dreht vollstaendig
# zum Ziel, RE2-Schritt 0xd8 @0x800434a0) trifft der erste Schuss die Spinne Platz 2 schon in F14 an ihrem Austritt (-500,-6400);
# dort liegen bei voller Lache (2020 x 2120) Ecken vor der Kamera-Nahgrenze (vz < 64), das Viereck geht nie an den Zeichner.
# Vorher (Heben drehte nur 0xc8 je Bild, erste Schuesse daneben) lief die Spinne bis (3977,-5683) und starb dort in F79 - die
# Lage, an der die Pruefung entstand. Mit dem Vorlauf W1.5 laeuft sie aus dem Austritt heraus (gemessen Release v0.8.30: Tod bei
# (3577,-6273), volle Lache sichtbar); gepruefte Mechanik (Tinte bf10bf, 90 x 8 bis 2020 x 2120, sichtbar) unveraendert.
set(_vorlauf_L7 "W1.5,")

function(r41s_lauf _name _raum _waffe _art _lache)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${ARGN}
        RE15_DEBUG_JUMP=${_raum}@250
        RE15_GIVE=${_waffe}:60 RE15_EQUIP=${_waffe}
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=5 "RE15_INPUT_SCRIPT=${_vorlauf_${_name}}${_skript}"
        RE15_SPINNEN_LOG=sp.log RE15_LACHE_LOG=lache.log
        "RE15_EXIT_AT=400#${_raum}"
        "${_exe}")
    if(NOT EXISTS "${WORKDIR}/sp.log")
        set(_fehler "${_fehler}\n[${_name}] kein sp.log (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    set(_f "")
    if(NOT _art STREQUAL "-")
        execute_process(COMMAND "${RE15_TOOL}" pruef_log "${WORKDIR}/sp.log" ${_art}
                        WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 120 RESULT_VARIABLE _rp OUTPUT_VARIABLE _op)
        message(STATUS "r41_spinnengore[${_name}] exe exit=${_rv}\n${_op}")
        if(NOT _rp EQUAL 0)
            set(_f "${_f}\n[${_name}] pruef_log ${_art} exit=${_rp}\n${_op}")
        endif()
    endif()
    if(NOT _lache STREQUAL "-")
        execute_process(COMMAND "${RE15_TOOL}" pruef_lache "${WORKDIR}/lache.log" ${_lache}
                        WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 120 RESULT_VARIABLE _rl OUTPUT_VARIABLE _ol)
        message(STATUS "r41_spinnengore[${_name}] Lache\n${_ol}")
        if(NOT _rl EQUAL 0)
            set(_f "${_f}\n[${_name}] pruef_lache ${_lache} exit=${_rl}\n${_ol}")
        endif()
    endif()
    if(NOT _f STREQUAL "")
        set(_fehler "${_fehler}${_f}" PARENT_SCOPE)
    endif()
endfunction()

r41s_lauf(L7 2070 7 platzt re2)
r41s_lauf(E7 2071 7 platzt - RE15_TITLE_SHOT_CHAR=1)
r41s_lauf(L8 2070 8 bein -)
r41s_lauf(L7AUS 2070 7 aus - RE15_KEIN_80PROZENT_GORE=1)
r41s_lauf(E7R15 2071 7 - r15 RE15_TITLE_SHOT_CHAR=1 RE15_AI_FLAVOR=re15)

# Nachbesserung 3, Mangel 2: Babyflecken im Echtlauf (Messhaken RE15_BABYTOD_TEST, nur Messlaeufe)
r41s_lauf(BT 2070 7 - - RE15_BABYTOD_TEST=200)
if(EXISTS "${_basis}_BT/lache.log.flecken")
    execute_process(COMMAND "${RE15_TOOL}" pruef_flecken "${_basis}_BT/lache.log.flecken"
                    WORKING_DIRECTORY "${_basis}_BT" TIMEOUT 120 RESULT_VARIABLE _rb OUTPUT_VARIABLE _ob)
    message(STATUS "r41_spinnengore[BT] Flecken\n${_ob}")
    if(NOT _rb EQUAL 0)
        set(_fehler "${_fehler}\n[BT] pruef_flecken exit=${_rb}\n${_ob}")
    endif()
else()
    set(_fehler "${_fehler}\n[BT] keine lache.log.flecken (kein Babyfleck gezeichnet)")
endif()

# Nachbesserung 3, Mangel 1: RE2-Kraehenfedern (Leon ROOM10C0, Elza ROOM10C1)
function(r41s_kraehe _name _raum)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${ARGN}
        RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=4300,5000,581" RE15_FORCE_CUT=1
        RE15_GIVE=7:12 RE15_EQUIP=7
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1
        "RE15_INPUT_SCRIPT=W3,M0.4,MA0.1,M0.9,MA0.1,M0.9,MA0.1,M0.9,MA0.1,M0.9,MA0.1,M0.9,W4"
        RE15_FEDER_LOG=feder.log RE15_STATE_LOG=state.log
        "RE15_EXIT_AT=320#${_raum}"
        "${_exe}")
    execute_process(COMMAND "${RE15_TOOL}" pruef_federn "${WORKDIR}/feder.log" gib
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 120 RESULT_VARIABLE _rk OUTPUT_VARIABLE _ok)
    message(STATUS "r41_spinnengore[${_name}] exe exit=${_rv} Federn\n${_ok}")
    if(NOT _rk EQUAL 0)
        set(_fehler "${_fehler}\n[${_name}] pruef_federn gib exit=${_rk}\n${_ok}" PARENT_SCOPE)
    endif()
endfunction()
r41s_kraehe(K 10C0)
r41s_kraehe(KE 10C1 RE15_TITLE_SHOT_CHAR=1)

file(REMOVE "${_exe}")
if(NOT _fehler STREQUAL "")
    message(FATAL_ERROR "r41_spinnengore: ${_fehler}")
endif()
message(STATUS "r41_spinnengore: L7 (Leon, Lache RE2), E7 (Elza), L8 (Bein), L7AUS (Gegenprobe), E7R15 (Lache RE1.5), BT (Babyflecken) und K/KE (Kraehenfedern) bestanden")
