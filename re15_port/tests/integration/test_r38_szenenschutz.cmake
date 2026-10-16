# =============================================================================
# SZENENSCHUTZ an der echten exe (Runde 38 Spur B, Dossier analysis/befunde_runde38/B_szenenschutz.md).
#
# Nutzer (AUFTRAG.md 2026-10-16): "Das Leon schon in Cutscenes angegriffen werden kann, wo er sich noch nicht mal
# bewegen kann ist mist. So wie bei Leon zum Beispiel bei ROOM 1190 bei den Hunden oder 11D0 bei den Hunden. Es sei
# den es ist Teil der Cutscene wie bei Elza Room 1170."
#
#   Lauf 1190  Leon, Standard-KI: Debug-Sprung ROOM1190 @240, Hunde-Sub 13 @450, Hebel-Kombination per
#              RE15_SET_FLAG_AT @520 -> sub01 startet sub10 (Szene: Set(2,7)=1 @0x02788, Hunde grid 0x43 @0x027E6,
#              Plc_ret @0x02844). MUSS: kein HP-Verlust und kein Angriffs-Zustand (Hund st1/3) solange pm=2;
#              der erste Biss frueh(est)ens 30 Bilder nach dem Szenenende (vorher F752 = 13 Bilder danach,
#              jetzt Sperrzeit 0x78 @0x801115e8-ec ab der Landung); danach beissen die Hunde (Angriff normal).
#   Lauf 11D0  Leon, Standard-KI: Debug-Sprung ROOM11D0 @240, Aufnahme-AOT 9 @330 (sub03, Set(2,7)=1 @0x01892),
#              Quadrat bestaetigt das Modal, sub02 gibt die Kaefighunde frei (Member_set(12,0x42) @0x0172C..).
#              MUSS: kein HP-Verlust und kein Hund st1/3 solange pm=2; danach Biss.
#   Lauf 1171  Elza (Spielerwahl), RE1.5-KI: RE15_SET_FLAG=3:194, Debug-Sprung -> ROOM1171, sub07 spawnt John +
#              2 Szenen-Kraehen grid 0x40 (@0x016DC/@0x016F0). MUSS (Gegenprobe "Teil der Cutscene"): die Kraehe
#              greift Elza WAEHREND pm=2 (hp 100 -> 92, gr=1; HOVER-Griff @0x80115254-0x801152c4).
#   Lauf 1171s Elza, Standard-KI: derselbe Weg; MUSS: der Szenenschutz fasst in 1171 keinen Akteur an
#              ("[szenenschutz] ... aus (0 geschuetzte Gegner-Ticks)" bzw. kein geschuetzter Tick im Raum).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r38_szenenschutz: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r38_szenenschutz: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r38b_szene_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r38b_szene_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

# state.log des ZIELRAUMS auswerten (Segment 1 = nach dem Debug-Sprung; der Bildzaehler beginnt je Raum neu).
# Ergebnis in Variablen mit Praefix _p: _p_szene_von/_bis (erstes/letztes Bild mit pm=2 nach dem ersten freien
# Bild), _p_hp_szene (HP-Verlust waehrend pm=2), _p_angriff_szene (Bilder mit Hund st1/3 waehrend pm=2),
# _p_biss (erstes Bild mit HP-Verlust nach der Szene), _p_griff_szene (erstes Bild mit gr=1 waehrend pm=2).
function(r38_auswerten _log _praefix)
    file(STRINGS "${_log}" _zeilen REGEX "^F[0-9]+ ")
    set(_seg 0)
    set(_vorF -1)
    set(_hpvor -9999)
    set(_frei_gesehen 0)
    set(_von -1)
    set(_bis -1)
    set(_hp_szene 0)
    set(_angriff 0)
    set(_biss -1)
    set(_griff -1)
    foreach(_z IN LISTS _zeilen)
        if(NOT _z MATCHES "^F([0-9]+) .*hp=(-?[0-9]+)\\) .* pm=([0-9]+) gr=([0-9]+)")
            continue()
        endif()
        set(_f "${CMAKE_MATCH_1}")
        set(_hp "${CMAKE_MATCH_2}")
        set(_pm "${CMAKE_MATCH_3}")
        set(_gr "${CMAKE_MATCH_4}")
        if(_f LESS _vorF)
            math(EXPR _seg "${_seg} + 1")
            set(_hpvor -9999)
        endif()
        set(_vorF "${_f}")
        if(NOT _seg EQUAL 1)
            continue()
        endif()
        if(_pm EQUAL 0 AND _f GREATER 30)
            set(_frei_gesehen 1)
        endif()
        set(_in_szene 0)
        if(_frei_gesehen AND _pm EQUAL 2 AND NOT _biss GREATER -1)
            set(_in_szene 1)
            if(_von LESS 0)
                set(_von "${_f}")
            endif()
            set(_bis "${_f}")
        endif()
        if(_hpvor GREATER -9999 AND _hp LESS _hpvor)
            if(_in_szene)
                math(EXPR _hp_szene "${_hp_szene} + ${_hpvor} - ${_hp}")
            elseif(_von GREATER -1 AND _biss LESS 0)
                set(_biss "${_f}")
            endif()
        endif()
        if(_in_szene AND _z MATCHES "t=20 st=1 ss1=3 ")
            math(EXPR _angriff "${_angriff} + 1")
        endif()
        if(_in_szene AND _gr EQUAL 1 AND _griff LESS 0)
            set(_griff "${_f}")
        endif()
        set(_hpvor "${_hp}")
    endforeach()
    set(${_praefix}_szene_von "${_von}" PARENT_SCOPE)
    set(${_praefix}_szene_bis "${_bis}" PARENT_SCOPE)
    set(${_praefix}_hp_szene "${_hp_szene}" PARENT_SCOPE)
    set(${_praefix}_angriff_szene "${_angriff}" PARENT_SCOPE)
    set(${_praefix}_biss "${_biss}" PARENT_SCOPE)
    set(${_praefix}_griff_szene "${_griff}" PARENT_SCOPE)
endfunction()

function(r38_lauf _name _timeout)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_env ${ARGN})
    re15_start_spiel(_rv ${_timeout} SDL_ASSERT=always_ignore RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1
                     RE15_STATE_LOG=state.log ${_env} "${_exe}")
    if(NOT EXISTS "${WORKDIR}/state.log" OR NOT EXISTS "${WORKDIR}/debug.log")
        message(STATUS "r38_szenenschutz[${_name}]: Lauf ohne Ergebnis (exit=${_rv}) -> EIN Wiederholungsversuch")
        file(REMOVE "${WORKDIR}/state.log" "${WORKDIR}/debug.log")
        re15_start_spiel(_rv ${_timeout} SDL_ASSERT=always_ignore RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1
                         RE15_STATE_LOG=state.log ${_env} "${_exe}")
    endif()
    if(NOT EXISTS "${WORKDIR}/state.log")
        message(FATAL_ERROR "r38_szenenschutz[${_name}]: kein state.log (exit=${_rv})")
    endif()
    set(_dir "${WORKDIR}" PARENT_SCOPE)
endfunction()

# ---- Lauf 1190 ------------------------------------------------------------------------------------------
r38_lauf(r1190 240 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=1190@240 RE15_DEBUG_SUB=13@450
         "RE15_SET_FLAG_AT=4:243,5:4,5:5@520" "RE15_PLAYER_POS=-4100,-24500,3072" RE15_EXIT_AT=950)
r38_auswerten("${_dir}/state.log" _a)
file(STRINGS "${_dir}/debug.log" _an REGEX "^\\[szenenschutz\\] F[0-9]+ aus \\([1-9][0-9]* geschuetzte")
message(STATUS "r38_szenenschutz[1190] Szene F${_a_szene_von}..F${_a_szene_bis} hp-Verlust=${_a_hp_szene} "
               "Angriffs-Bilder=${_a_angriff_szene} erster Biss F${_a_biss}; ${_an}")
if(_a_szene_von LESS 0)
    message(FATAL_ERROR "r38_szenenschutz[1190]: die Szene (pm=2) lief nicht — Weg geaendert?")
endif()
if(NOT _a_hp_szene EQUAL 0)
    message(FATAL_ERROR "r38_szenenschutz[1190]: Leon verlor in der Szene ${_a_hp_szene} HP")
endif()
if(NOT _a_angriff_szene EQUAL 0)
    message(FATAL_ERROR "r38_szenenschutz[1190]: ${_a_angriff_szene} Bilder Hund im Angriff (st1/3) waehrend der Szene")
endif()
if(NOT _an)
    message(FATAL_ERROR "r38_szenenschutz[1190]: kein geschuetzter Gegner-Tick protokolliert ([szenenschutz] ... aus)")
endif()
if(_a_biss LESS 0)
    message(FATAL_ERROR "r38_szenenschutz[1190]: nach der Szene kein Biss bis F950 — Angriff nicht normal")
endif()
math(EXPR _abstand "${_a_biss} - ${_a_szene_bis}")
if(_abstand LESS 30)
    message(FATAL_ERROR "r38_szenenschutz[1190]: erster Biss ${_abstand} Bilder nach der Szene (vorher 13) — "
                        "Sperrzeit 0x78 @0x801115e8-ec nicht wirksam")
endif()

# ---- Lauf 11D0 ------------------------------------------------------------------------------------------
set(_druck "")
foreach(_b RANGE 380 450 10)   # bestaetigt das Aufnahme-Modal; spaetere Druecke wuerden AOT 9 neu zuenden
    list(APPEND _druck "square@${_b}")
endforeach()
string(REPLACE ";" "," _druck "${_druck}")
r38_lauf(r11d0 240 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=11d0@240 "RE15_FIRE_AOT=9@330#11D0"
         "RE15_PRESS=${_druck}" "RE15_PLAYER_POS=-10600,-13250,1024" RE15_EXIT_AT=800)
r38_auswerten("${_dir}/state.log" _b)
message(STATUS "r38_szenenschutz[11D0] Szene F${_b_szene_von}..F${_b_szene_bis} hp-Verlust=${_b_hp_szene} "
               "Angriffs-Bilder=${_b_angriff_szene} erster Biss F${_b_biss}")
if(_b_szene_von LESS 0)
    message(FATAL_ERROR "r38_szenenschutz[11D0]: die Szene (pm=2) lief nicht — Weg geaendert?")
endif()
if(NOT _b_hp_szene EQUAL 0 OR NOT _b_angriff_szene EQUAL 0)
    message(FATAL_ERROR "r38_szenenschutz[11D0]: Angriff in der Szene (hp ${_b_hp_szene}, ${_b_angriff_szene} Bilder st1/3)")
endif()
if(_b_biss LESS 0)
    message(FATAL_ERROR "r38_szenenschutz[11D0]: nach der Szene kein Biss bis F800 — Angriff nicht normal")
endif()

# ---- Lauf 1171 (Gegenprobe, RE1.5-KI) --------------------------------------------------------------------
r38_lauf(r1171 240 RE15_AI_FLAVOR=re15 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
         "RE15_SET_FLAG=3:194" RE15_DEBUG_JUMP=1170@250 RE15_EXIT_AT=300)
file(STRINGS "${_dir}/debug.log" _elza REGEX "Spieler-Familie PL04")
file(STRINGS "${_dir}/debug.log" _raum REGEX "loaded room1171\\.rdt")
if(NOT _elza OR NOT _raum)
    message(FATAL_ERROR "r38_szenenschutz[1171]: nicht Elza in ROOM1171 (Spielerwahl/Sprung)")
endif()
# Hier gibt es kein "erstes freies Bild" (die Szene beginnt mit dem Raum): Griff direkt aus Segment 1 lesen.
file(STRINGS "${_dir}/state.log" _zeilen REGEX "^F[0-9]+ ")
set(_seg 0)
set(_vorF -1)
set(_griff -1)
set(_griff_hp 0)
foreach(_z IN LISTS _zeilen)
    if(NOT _z MATCHES "^F([0-9]+) .*hp=(-?[0-9]+)\\) .* pm=([0-9]+) gr=([0-9]+)")
        continue()
    endif()
    if(CMAKE_MATCH_1 LESS _vorF)
        math(EXPR _seg "${_seg} + 1")
    endif()
    set(_vorF "${CMAKE_MATCH_1}")
    if(_seg EQUAL 1 AND _griff LESS 0 AND CMAKE_MATCH_3 EQUAL 2 AND CMAKE_MATCH_4 EQUAL 1)
        set(_griff "${CMAKE_MATCH_1}")
        set(_griff_hp "${CMAKE_MATCH_2}")
    endif()
endforeach()
message(STATUS "r38_szenenschutz[1171] Szenen-Kraehe greift bei F${_griff} (hp ${_griff_hp}, pm=2)")
if(_griff LESS 0)
    message(FATAL_ERROR "r38_szenenschutz[1171]: die Szenen-Kraehe griff Elza waehrend der Szene NICHT — "
                        "der Szenenschutz hat den Szenen-Angriff (HOVER @0x80115254-0x801152c4) unterdrueckt")
endif()

# ---- Lauf 1171s (Standard-KI): der Schutz fasst in 1171 nichts an ----------------------------------------
r38_lauf(r1171s 240 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
         "RE15_SET_FLAG=3:194" RE15_DEBUG_JUMP=1170@250 RE15_EXIT_AT=300)
file(STRINGS "${_dir}/debug.log" _abgefangen REGEX "^\\[szenenschutz\\] F[0-9]+ (Treffer abgefangen|WARNUNG)")
file(STRINGS "${_dir}/debug.log" _geschuetzt REGEX "^\\[szenenschutz\\] F[0-9]+ aus \\([1-9][0-9]* geschuetzte")
if(_abgefangen OR _geschuetzt)
    message(FATAL_ERROR "r38_szenenschutz[1171s]: der Szenenschutz griff in ROOM1171 ein: ${_abgefangen} ${_geschuetzt}")
endif()
message(STATUS "r38_szenenschutz[1171s]: kein geschuetzter Akteur in ROOM1171 (John NPC, Kraehen grid 0x40)")

file(REMOVE "${_exe}")
message(STATUS "r38_szenenschutz: OK — 1190/11D0 kein Treffer in der Szene, danach Angriff; 1171 Szenen-Angriff bleibt")
