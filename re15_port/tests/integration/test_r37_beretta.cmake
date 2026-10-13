# Runde 37 Spur G — Integration mit der ECHTEN exe: der 3-Schuss-Feuerstoss der Beretta M93R (5) / Glock 18 (6) trifft
# JE KUGEL (Nutzer-Nachtrag 2026-10-12). Dossier: analysis/befunde_runde37/G_beretta.md; Belege include/re15_beretta_r37.h.
# Registrierung: tests/unit/probes/r37_beretta.cmake.
#
# Aufbau wie die Messung (G_beretta_mess/run_mess.sh): ROOM1140 per RE15_DEBUG_JUMP=1140@250, Leon (200,-18200) Blick -z,
# Ziel Platz 5 (Typ 0x11, (200,-19600)), RE15_GIVE=<w>:15 / RE15_EQUIP=<w>, EIN Abzug "M2.0,MA0.1,M3.0,W2" (Square 3 Bilder;
# Elza "M3.0,..." — sie dreht erst auf den Zombie ein).
# Ausgewertet wird state.log nach dem Raumsprung (Bildzaehler faellt zurueck): das Eintrittsbild E (Munition faellt), die
# Bilder mit HP-Abzug irgendeines Gegners im Fenster [E, E+10] und der Munitionsabzug in E.
#   leon_re2_w5    RE2-KI, M93R:   drei HP-Abzuege in E, E+3, E+6 (Kugel-Takt @0x80048910-20 / ESP-Kette 0/2/2), 15 -> 12
#   leon_re2_w6    RE2-KI, Glock:  dito (Handler 0x800338A8 @0x80074118)
#   leon_re15_w5   RE1.5-KI, M93R: dito — der RE1.5-Riegel +0x93 (`ori 0x1` @0x800124F0) haelt Kugel 2/3 nicht mehr ab
#   leon_re2_w3    Gegenprobe Browning: EIN Abzug in E, keiner in E+3/E+6, 15 -> 14 (Handler 0x800337BC unveraendert)
#   elza_re2_w5    Elza (RE15_PSELECT_AUTO, ROOM11E1, Platz 1 Typ 0x10 vor ihr), M93R: drei HP-Abzuege, 15 -> 12
# Nachbesserung 1 (Abnahme 0 M1-M3) — Gegner mit RE1.5-Gehirn unter der STANDARD-KI (kein RE15_AI_FLAVOR = RE2-Flavor):
#   t2b_std_w5     ROOM5040 Platz 1 Typ 0x2b, Leon (-14493,-13185) Blick -z: drei Abzuege E/E+3/E+6 je -10
#                  (= Spalte 3 @0x8006EFA4; Stoss 30 statt vorher 1 x 10), Munition -3
#   roach_std_w5   ROOM3040 Platz 2 Typ 0x29, Leon (-13500,-10300) Blick -z: drei Abzuege je -14 (Spalte 3 @0x8006EEF4)
#   Vor der Nachbesserung lieferten beide Laeufe EINEN Abzug (F61 -10 / -14, Dossier G_beretta.md 8.1) — rot.
# Die exe laeuft als Messkopie re15_pc_r37g_haken_<id> neben der Original-exe (Muster test_r34n_b_cursor.cmake).
# Ein Lauf ohne Ergebnis (exit=1/-1 = von aussen beendet; debug.log < 10 Zeilen) wird EINMAL wiederholt.

cmake_minimum_required(VERSION 3.16)
set(_tag "r37_beretta")
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "${_tag}: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "${_tag}: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_wurzel}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)          # ".exe" unter Windows, leer unter Linux
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37g_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r37g_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(r37_fehler _lauf _text)
    message(FATAL_ERROR "${_tag} [${_lauf}]: ${_text}")
endfunction()

# _start: "leon" (Titelbild-Weg) oder "elza" (Spielerwahl -> Elza, Raumvariante 11x1)
function(r37_exe _lauf _dir _start _jump _exit _pos _waffe _flavor _input)
    file(MAKE_DIRECTORY "${_dir}")
    set(WORKDIR "${_dir}")
    if(_start STREQUAL "elza")
        set(_st RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
    else()
        set(_st RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
    endif()
    # "std" = Standard-KI: KEIN RE15_AI_FLAVOR (Default RE2, enemy_ai_re2_zombie.c) — so spielt der Nutzer
    if(NOT _flavor STREQUAL "std")
        list(APPEND _st "RE15_AI_FLAVOR=${_flavor}")
    else()
        unset(ENV{RE15_AI_FLAVOR})   # auch eine geerbte Vorgabe der Testumgebung faellt weg
    endif()
    set(_env RE15_NO_INTRO=1 RE15_NOAUDIO=1 ${_st}
             RE15_WINDOW_SCALE=1 "RE15_DEBUG_JUMP=${_jump}@250" "RE15_PLAYER_POS=${_pos}"
             "RE15_GIVE=${_waffe}:15" "RE15_EQUIP=${_waffe}"
             RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_input}"
             RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log "RE15_EXIT_AT=${_exit}")
    re15_start_spiel(_rv 240 ${_env} "${RE15_PC_EXE}")
    if(NOT _rv EQUAL 0)
        set(_z "")
        if(EXISTS "${_dir}/debug.log")
            file(STRINGS "${_dir}/debug.log" _z)
        endif()
        list(LENGTH _z _n)
        if(_n LESS 10 OR "${_rv}" STREQUAL "1" OR "${_rv}" STREQUAL "-1")
            message(STATUS "${_tag} [${_lauf}]: Lauf ohne Ergebnis (exit=${_rv}, debug.log ${_n} Zeilen) -> EIN Wiederholungsversuch")
            file(REMOVE "${_dir}/debug.log" "${_dir}/state.log" "${_dir}/wf.log")
            re15_start_spiel(_rv 240 ${_env} "${RE15_PC_EXE}")
        endif()
    endif()
    if(NOT _rv EQUAL 0)
        r37_fehler("${_lauf}" "re15_pc.exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
    endif()
    foreach(_d debug.log state.log)
        if(NOT EXISTS "${_dir}/${_d}")
            r37_fehler("${_lauf}" "kein ${_d} in ${_dir}")
        endif()
    endforeach()
    file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
    if(NOT _ex)
        r37_fehler("${_lauf}" "RE15_EXIT_AT nicht erreicht (keine Zeile '[flow] EXIT_AT') — Haenger?")
    endif()
endfunction()

# state.log NACH dem Raumsprung: Eintrittsbild E (erster Munitionsabzug), Abzug dort, HP-Abzuege im Fenster [E, E+10]
# als Liste "<Bild-E>:<Platz>:<dHP>".
function(r37_auswerten _datei _e _mgab _treffer)
    file(STRINGS "${_datei}" _zl)
    set(_vorher -1)
    set(_nach 0)
    set(_mg "")
    set(_E -1)
    set(_ab 0)
    set(_liste "")
    foreach(_l IN LISTS _zl)
        if(NOT _l MATCHES "^F([0-9]+) .* mg=(-?[0-9]+)")
            continue()
        endif()
        set(_f "${CMAKE_MATCH_1}")
        set(_m "${CMAKE_MATCH_2}")
        if(NOT _nach AND _vorher GREATER _f)
            set(_nach 1)
        endif()
        set(_vorher ${_f})
        if(NOT _nach)
            continue()
        endif()
        if(NOT _mg STREQUAL "" AND _E LESS 0 AND _m LESS _mg)
            set(_E ${_f})
            math(EXPR _ab "${_mg} - ${_m}")
        endif()
        set(_mg ${_m})
        string(REGEX MATCHALL "\\[[0-9]+ t=[0-9a-f]+ st=[0-9]+ [^]]*\\] hp=-?[0-9]+" _gs "${_l}")
        foreach(_g IN LISTS _gs)
            if(_g MATCHES "^\\[([0-9]+) .*\\] hp=(-?[0-9]+)$")
                set(_s "${CMAKE_MATCH_1}")
                set(_hp "${CMAKE_MATCH_2}")
                if(DEFINED _hp_${_s} AND _E GREATER -1)
                    math(EXPR _rel "${_f} - ${_E}")
                    if(_hp LESS _hp_${_s} AND _rel GREATER -1 AND _rel LESS 11)
                        math(EXPR _d "${_hp} - ${_hp_${_s}}")
                        list(APPEND _liste "${_rel}:${_s}:${_d}")
                    endif()
                endif()
                set(_hp_${_s} ${_hp})
            endif()
        endforeach()
    endforeach()
    set(${_e} ${_E} PARENT_SCOPE)
    set(${_mgab} ${_ab} PARENT_SCOPE)
    set(${_treffer} "${_liste}" PARENT_SCOPE)
endfunction()

# Feuerstoss: drei Abzuege in E, E+3, E+6, Munition -3
function(r37_feuerstoss_pruefen _lauf _dir)
    r37_auswerten("${_dir}/state.log" _E _ab _tr)
    if(_E LESS 0)
        r37_fehler("${_lauf}" "kein Munitionsabzug im state.log — der Abzug kam nicht")
    endif()
    list(LENGTH _tr _n)
    set(_bilder "")
    foreach(_t IN LISTS _tr)
        string(REGEX REPLACE ":.*$" "" _b "${_t}")
        list(APPEND _bilder ${_b})
    endforeach()
    if(NOT _ab EQUAL 3)
        r37_fehler("${_lauf}" "Munition im Eintrittsbild F${_E}: -${_ab}, erwartet -3 (@0x80033974/7c/84)")
    endif()
    if(NOT _n EQUAL 3 OR NOT "${_bilder}" STREQUAL "0;3;6")
        r37_fehler("${_lauf}" "HP-Abzuege ab F${_E}: '${_tr}' (Bild-E:Platz:dHP), erwartet DREI in E, E+3, E+6")
    endif()
    message(STATUS "${_tag} [${_lauf}]: OK — F${_E} Munition -3, Treffer ${_tr}")
endfunction()

# ---------------------------------------------------------------------------------------------------
set(_d "${_wurzel}/leon_re2_w5")
r37_exe("leon_re2_w5" "${_d}" leon 1140 "260#1140" "200,-18200,1024" 5 re2 "M2.0,MA0.1,M3.0,W2")
r37_feuerstoss_pruefen("leon_re2_w5" "${_d}")

set(_d "${_wurzel}/leon_re2_w6")
r37_exe("leon_re2_w6" "${_d}" leon 1140 "260#1140" "200,-18200,1024" 6 re2 "M2.0,MA0.1,M3.0,W2")
r37_feuerstoss_pruefen("leon_re2_w6" "${_d}")

set(_d "${_wurzel}/leon_re15_w5")
r37_exe("leon_re15_w5" "${_d}" leon 1140 "260#1140" "200,-18200,1024" 5 re15 "M2.0,MA0.1,M3.0,W2")
r37_feuerstoss_pruefen("leon_re15_w5" "${_d}")

# Gegenprobe: Browning HP (Einzelschuss) — EIN Abzug, Munition -1
set(_d "${_wurzel}/leon_re2_w3")
r37_exe("leon_re2_w3" "${_d}" leon 1140 "260#1140" "200,-18200,1024" 3 re2 "M2.0,MA0.1,M3.0,W2")
r37_auswerten("${_d}/state.log" _E _ab _tr)
list(LENGTH _tr _n)
if(_E LESS 0 OR NOT _ab EQUAL 1 OR NOT _n EQUAL 1 OR NOT "${_tr}" MATCHES "^0:")
    r37_fehler("leon_re2_w3" "Browning: E=F${_E} Munition -${_ab} Treffer '${_tr}', erwartet EIN Abzug in E, -1")
endif()
message(STATUS "${_tag} [leon_re2_w3]: OK — Gegenprobe Browning F${_E} Munition -1, Treffer ${_tr}")

# Elza: Spielerwahl schaltet auf Elza, der Debug-Sprung 11E0 landet in ihrer Variante ROOM11E1 (Zombies (-11000,-14000)..)
set(_d "${_wurzel}/elza_re2_w5")
r37_exe("elza_re2_w5" "${_d}" elza 11E0 "220#11E1" "-10500,-11000,1024" 5 re2 "M3.0,MA0.1,M3.0,W2")
file(STRINGS "${_d}/debug.log" _pl REGEX "\\[pl\\] Spieler-Familie PL04")
file(STRINGS "${_d}/debug.log" _rm REGEX "loaded room11e1\\.rdt")
if(NOT _pl OR NOT _rm)
    r37_fehler("elza_re2_w5" "Elza nicht in ROOM11E1 (PL04: '${_pl}', Raum: '${_rm}')")
endif()
r37_feuerstoss_pruefen("elza_re2_w5" "${_d}")

# Nachbesserung 1: Schaden je Kugel — jeder der drei Abzuege genau _dhp (Spalte 3 der RE1.5-Zeile)
function(r37_schaden_pruefen _lauf _dir _dhp)
    r37_auswerten("${_dir}/state.log" _E _ab _tr)
    foreach(_t IN LISTS _tr)
        if(NOT _t MATCHES ":${_dhp}$")
            r37_fehler("${_lauf}" "Abzug '${_t}' in '${_tr}', erwartet je ${_dhp} (Spalte 3)")
        endif()
    endforeach()
    message(STATUS "${_tag} [${_lauf}]: OK — Schaden je Kugel ${_dhp}")
endfunction()

set(_d "${_wurzel}/t2b_std_w5")
r37_exe("t2b_std_w5" "${_d}" leon 5040 "200#5040" "-14493,-13185,1024" 5 std "M2.0,MA0.1,M3.0,W2")
r37_feuerstoss_pruefen("t2b_std_w5" "${_d}")
r37_schaden_pruefen("t2b_std_w5" "${_d}" -10)

set(_d "${_wurzel}/roach_std_w5")
r37_exe("roach_std_w5" "${_d}" leon 3040 "200#3040" "-13500,-10300,1024" 5 std "M2.0,MA0.1,M3.0,W2")
r37_feuerstoss_pruefen("roach_std_w5" "${_d}")
r37_schaden_pruefen("roach_std_w5" "${_d}" -14)

file(REMOVE "${_exe_kopie}")
message(STATUS "${_tag}: alle Laeufe OK")
