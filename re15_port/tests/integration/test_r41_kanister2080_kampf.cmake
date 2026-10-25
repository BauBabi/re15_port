# =============================================================================
# Runde 41 Spur G "kanister2080", Nachbesserung 1 (Abnahme 0 M2 / Nachtrag 23, Orchestrator-Entscheide FG-E3/FG-E4) - die GANZE
# Folge im ECHTEN Ablauf an der exe:
#   Kampf in ROOM2090/2091 -> Treffer bis zur RE2-Schwelle (Messhaken RE15_K2080_GATOR_HP=24990: EIN echter Pistolentreffer statt
#   43, die Schwelle HP < 25000 @0x80101ebc-dc bleibt) -> Kanister-Szene in 2080/2081 (Marvin/John: Hinknien Loco c11 statt
#   Klettern, Zeile 42, Loslassen) -> Fall in 2090/2091 Cut 12 (FG-E4) -> Kanister liegt an der Aufschlagstelle -> DERSELBE
#   Alligator kaempft weiter (FG-E3: Aktor zurueck in seinen Slot, kein Untertauchen/Auftauchen/Intro) -> er nimmt den Kanister
#   auf (Spur B: Fenster gator_re2_r41.c Stufe 0, "KANISTER Stufe 1" / "angehaengt") -> Schuss -> R1-Tod ("R1 ende (Leiche)").
# OHNE RE15_GB_TEST und OHNE RE15_GATOR_KANISTER (beide ausdruecklich geloescht), Leon UND Elza. Leon beginnt im Westkanal bei
# (-6000,-17500) und schiesst (Zielen/Feuern alle ~4 s), wie ein Spieler, der den Kampf fortsetzt. Gemessen (Wurf-Gier 3072,
# Dossier 8.9): Ausloeser F902 bei (-5790,-6245), Aufschlag (-7515,-12304); nach der Rueckkehr Aufnahme F104, R1 F570, Leiche
# F606. EXIT_AT 1500 Bilder nach der Rueckkehr.
# Nachbesserung 2 (Abnahme 1, N3): VIER Laeufe mit ZWEI weiteren Startlagen - Leon (0,-7000) (Kampf im Nordosten, Abnahme-Lauf
# v2/v4) und Elza (-8000,-16000) (Start West, Abnahme-Lauf v3/v5: vorher keine Aufnahme in 4000 Bildern). Je Lauf: Wahl der
# Wurfrichtung ("Wurfrichtung" = der weitere Landepunkt vom Alligator), "ANSCHWIMMEN an" (RE2-Zielmodus, Header
# RE15_K2080_ANSCHWIMM_AB) und die ganze Folge bis zum R1-Tod in 1500 Bildern nach der Rueckkehr.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_kanister2080_kampf.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_kanister2080_kampf: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_kanister2080_kampf: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
# EIGENER EXE-NAME (Muster test_r34n_b_cursor.cmake): nur die eigene Kopie wird geloescht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe_kopie "${_exe_dir}/re15_pc_r41gk_${_lauf_id}.exe")
if(NOT WIN32)
    set(_exe_kopie "${_exe_dir}/re15_pc_r41gk_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

function(r41gk_fehler _text)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_kanister2080_kampf: ${_text}")
endfunction()

# Testhaken, die den echten Ablauf verdecken wuerden, NICHT vom Aufrufer erben (Abnahme 0 M2/M4)
unset(ENV{RE15_GB_TEST})
unset(ENV{RE15_GATOR_KANISTER})
unset(ENV{RE15_KEIN_GATOR})
unset(ENV{RE15_K2080_WURF_GIER})

set(_basisenv RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore)
set(_leon RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
set(_elza RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
# Zielen + Feuern (R1 / R1+Quadrat) nach 6 s alle ~4 s, 18 Bloecke
set(_schiess "W6.0")
foreach(_i RANGE 1 18)
    string(APPEND _schiess ",M0.6,MA0.1,M0.3,MA0.1,W3.0")
endforeach()

foreach(_lauf "leon|-6000,-17500,3072|kampf" "elza|-6000,-17500,3072|kampf" "leon|0,-7000,2048|nordost" "elza|-8000,-16000,1024|west")
    string(REPLACE "|" ";" _lt "${_lauf}")
    list(GET _lt 0 _wer)
    list(GET _lt 1 _start)
    list(GET _lt 2 _lname)
    set(WORKDIR "${_basis}/${_lname}_${_wer}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    if(_wer STREQUAL "leon")
        set(_wahl ${_leon})
        set(_r80 2080)
        set(_r90 2090)
        set(_npc "Marvin 0x40")
    else()
        set(_wahl ${_elza})
        set(_r80 2081)
        set(_r90 2091)
        set(_npc "John 0x49")
    endif()
    re15_start_spiel(_rv 900 ${_basisenv} ${_wahl}
        RE15_DEBUG_JUMP=2090@250 "RE15_SET_FLAG=9:110,3:245" "RE15_GIVE=3:60" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100
        RE15_K2080_GATOR_HP=24990 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_schiess}"
        RE15_STATE_LOG=state.log "RE15_PLAYER_POS=${_start}" "RE15_EXIT_AT=1500#${_r90}" "${_exe_kopie}")
    if(NOT _rv EQUAL 0)
        r41gk_fehler("Lauf ${_lname}_${_wer} exe exit=${_rv}")
    endif()
    # ---- Szene (2080/2081) und Rueckweg ----
    foreach(_muster
            "FG-E3: Alligator gesichert Slot [0-9]+ "
            "AUSLOESER geschwaecht .*-> ROOM${_r80}"
            "ROOM${_r80} SZENE \\(${_npc}, 8 lebende Zombies"
            "Kanister aufgehoben \\(Loco c11 Bild 24, Hinknien ROOM11B0 sub06 @0x01650\\)"
            "Wurf: Start .* -> Landung \\(-?[0-9]+,-?[0-9]+,-?[0-9]+\\) nach [0-9]+ Bildern \\(Wasser 2090 y -1620\\)"
            "FG-E3: ROOM${_r90} derselbe Alligator in Slot [0-9]+ "
            "ROOM${_r90} RUECKKEHR: .*Zone-7-Tot-Bits 236\\.\\.243 gesetzt: 8 von 8"
            "Aufschlag im Wasser bei "
            "ROOM${_r90} Kanister im Becken \\(-?[0-9]+,-1200,-?[0-9]+\\) = Aufschlagstelle"
            "Wurfrichtung (3072|1024) \\((Nord|Sued)becken, Alligator beim Ausloesen"
            "ANSCHWIMMEN an: Gator Slot [0-9]+ "
            "HALS zum Kanister: Part 5 "
            "HALS zurueck \\(Akku -> 0\\)")
        file(STRINGS "${WORKDIR}/debug.log" _z REGEX "\\[k2080\\] F[0-9]+ ${_muster}")
        if(NOT _z)
            r41gk_fehler("Lauf ${_lname}_${_wer}: keine Zeile '${_muster}'")
        endif()
    endforeach()
    # Nachtrag 24b: Hinknien/Aufstehen (Plc_motion(1,11) zweimal), kein Klettern (1,2) und kein RUN-Gait als "Tragen" (2,0)
    file(STRINGS "${WORKDIR}/debug.log" _kn REGEX "\\[scd\\] Plc_motion\\(entity=1, motion=11, ")
    list(LENGTH _kn _n_kn)
    file(STRINGS "${WORKDIR}/debug.log" _kl REGEX "\\[scd\\] Plc_motion\\(entity=1, motion=2, |\\[scd\\] Plc_motion\\(entity=2, motion=0, ")
    if(NOT _n_kn EQUAL 2 OR _kl)
        r41gk_fehler("Lauf ${_lname}_${_wer}: ${_n_kn} x Plc_motion(1,11) statt 2 bzw. Kletter-/Lauf-Clip im Szenen-Programm (${_kl})")
    endif()
    # FG-E4: der Fall laeuft in Cut 12
    file(STRINGS "${WORKDIR}/state.log" _c12 REGEX " cam=12 ")
    if(NOT _c12)
        r41gk_fehler("Lauf ${_lname}_${_wer}: Cut 12 nie im Bild (FG-E4)")
    endif()
    # ---- weiterer Kampf nach der Rueckkehr (gator_boss.log, Reihenfolge ab "KANISTER gesetzt") ----
    file(STRINGS "${WORKDIR}/gator_boss.log" _gb REGEX
         "GATOR VERSCHWUNDEN|RE2 AUFTAUCHEN|INTRO ende|KANISTER gesetzt|KANISTER Stufe 1 |KANISTER angehaengt|R1 start|R1 ende \\(Leiche\\)")
    set(_stand 0)
    set(_aufnahme "")
    foreach(_l IN LISTS _gb)
        if(_stand GREATER 0 AND _l MATCHES "GATOR VERSCHWUNDEN|RE2 AUFTAUCHEN|INTRO ende")
            r41gk_fehler("Lauf ${_lname}_${_wer}: nach der Szene ein neuer Alligator/Intro ('${_l}') - FG-E3 verlangt denselben")
        endif()
        if(_stand EQUAL 0 AND _l MATCHES "KANISTER gesetzt")
            set(_stand 1)
        elseif(_stand EQUAL 1 AND _l MATCHES "KANISTER Stufe 1 ")
            set(_stand 2)
            set(_aufnahme "${_l}")
        elseif(_stand EQUAL 2 AND _l MATCHES "KANISTER angehaengt")
            set(_stand 3)
        elseif(_stand EQUAL 3 AND _l MATCHES "R1 start")
            set(_stand 4)
        elseif(_stand EQUAL 4 AND _l MATCHES "R1 ende \\(Leiche\\)")
            set(_stand 5)
        endif()
    endforeach()
    set(_schritte "Kanister im Becken (KANISTER gesetzt)" "Aufnahme (KANISTER Stufe 1)" "angehaengt" "R1 start" "R1 ende (Leiche)")
    if(NOT _stand EQUAL 5)
        list(GET _schritte ${_stand} _fehlt)
        r41gk_fehler("Lauf ${_lname}_${_wer}: Folge nach der Szene bricht ab vor '${_fehlt}' (Stand ${_stand} von 5)")
    endif()
    message(STATUS "r41_kanister2080_kampf ${_lname}_${_wer} (Start ${_start}): Szene (${_npc}, Hinknien) -> Fall Cut 12 -> derselbe Alligator -> ${_aufnahme} -> R1-Tod")
endforeach()

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_kanister2080_kampf: Leon und Elza, drei Startlagen gruen")
