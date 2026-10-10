# Runde 36 Spur C — Integration mit der ECHTEN exe: Sichtlinie (RE1.5 FUN_8001b9b4 @0x80012168-74) und
# Fresser-Sperre (RE2 FUN_800410CC @0x80041270-9c) fuer den allgemeinen Schusspfad.
#
# Dossier: analysis/befunde_runde36/C_schuss.md §1. Registrierung: tests/unit/probes/r36_schuss.cmake.
# Harness wie test_r35_werfer.cmake: ROOM1140 per RE15_DEBUG_JUMP=1140@250, RE2-KI, Python (20, 6 Schuss),
# Eingabe "M0.6,MA1.5,M1.0,W4", RE15_STATE_LOG, RE15_EXIT_AT=200#1140. Plaetze (state.log): 1 (-800,-20600)
# Liegender, 2 (-1800,-19600), 3 (-1800,-21600), 4 (200,-21600), 5 (200,-19600) = Fresser (ss1 8, 1D3 80).
#   tisch    Leon (200,-10300) Blick -z: der Konferenztisch (SCA-Zelle 6, z -17450..-11350) liegt zwischen ihm
#            und den Plaetzen 4/5 -> KEIN Platz verliert HP (vorher: Platz 5 F19 und Platz 4 F44 auf -1).
#   fresser  Leon am Tuerpunkt (-7600,-17600) Blick +x, alle Fresser > 4000 entfernt -> KEIN Platz verliert HP
#            (vorher: Platz 2 und 5 auf -1).
#   frei     Gegenprobe: Leon (200,-18200) Blick -z, HINTER dem Tisch (Kante -17450), Platz 5 in 1400 -> der
#            Schuss trifft (Platz 5 verliert HP) — der Resolver schiesst noch.
#   gl_tisch / gl_fresser  dieselben Aufstellungen mit dem Granatwerfer (15, Explosiv): KEIN Platz verliert HP.
# Ein Lauf ohne Ergebnis (exit=1 = von aussen beendet; debug.log < 10 Zeilen) wird EINMAL wiederholt.

cmake_minimum_required(VERSION 3.16)
set(_tag "r36_schuss")
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

function(r36_fehler _lauf _text)
    message(FATAL_ERROR "${_tag} [${_lauf}]: ${_text}")
endfunction()

function(r36_exe _lauf _dir _pos _waffe)
    file(MAKE_DIRECTORY "${_dir}")
    set(WORKDIR "${_dir}")
    set(_env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
             RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=1140@250 "RE15_PLAYER_POS=${_pos}"
             RE15_AI_FLAVOR=re2 "RE15_GIVE=${_waffe}:6" "RE15_EQUIP=${_waffe}"
             RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=M0.6,MA1.5,M1.0,W4"
             RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log "RE15_EXIT_AT=200#1140")
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
        r36_fehler("${_lauf}" "re15_pc.exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
    endif()
    foreach(_d debug.log state.log)
        if(NOT EXISTS "${_dir}/${_d}")
            r36_fehler("${_lauf}" "kein ${_d} in ${_dir}")
        endif()
    endforeach()
    file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
    if(NOT _ex)
        r36_fehler("${_lauf}" "RE15_EXIT_AT nicht erreicht (keine Zeile '[flow] EXIT_AT') — Haenger?")
    endif()
endfunction()

# Groesster und kleinster HP-Wert des Platzes _slot nach dem Raumwechsel (Muster r35_gegner).
function(r36_gegner _datei _slot _hp0 _hpmin)
    file(STRINGS "${_datei}" _zl)
    set(_vorher -1)
    set(_nach 0)
    set(_min 100000)
    set(_start -100000)
    foreach(_l IN LISTS _zl)
        if(_l MATCHES "^F([0-9]+) ")
            set(_f "${CMAKE_MATCH_1}")
            if(NOT _nach AND _vorher GREATER _f)
                set(_nach 1)
            endif()
            set(_vorher ${_f})
            if(NOT _nach)
                continue()
            endif()
            if(_l MATCHES "\\[${_slot} t=[0-9a-f]+ st=([0-9]+) ss1=([0-9]+) ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=[0-9]+ af=[0-9]+ stun=-?[0-9]+ d=[0-9]+ @\\(-?[0-9]+,-?[0-9]+,r-?[0-9]+\\)\\] hp=(-?[0-9]+)")
                set(_hp "${CMAKE_MATCH_3}")
                # die ersten Bilder nach dem Sprung tragen hp=0, bevor das Raumskript die Gegner setzt:
                # erst ab dem ersten positiven Wert zaehlen
                if(_hp GREATER _start)
                    set(_start ${_hp})
                endif()
                if(_start GREATER 0 AND _hp LESS _min)
                    set(_min ${_hp})
                endif()
            endif()
        endif()
    endforeach()
    set(${_hp0} ${_start} PARENT_SCOPE)
    set(${_hpmin} ${_min} PARENT_SCOPE)
endfunction()

# Alle fuenf Plaetze muessen ihren Ausgangs-HP behalten (Ausgangs-HP > 0, sonst war die Aufstellung nicht da).
function(r36_keiner_verliert _lauf _datei)
    foreach(_s 1 2 3 4 5)
        r36_gegner("${_datei}" ${_s} _hp0 _hpmin)
        if(_hp0 LESS 1)
            r36_fehler("${_lauf}" "Platz ${_s} ohne Ausgangs-HP (${_hp0}) — Aufstellung fehlt?")
        endif()
        if(_hpmin LESS _hp0)
            r36_fehler("${_lauf}" "Platz ${_s} verliert HP (${_hp0} -> ${_hpmin}) — Schuss kam durch")
        endif()
        message(STATUS "${_tag} [${_lauf}]: Platz ${_s} HP ${_hp0} -> ${_hpmin}")
    endforeach()
endfunction()

# ---------------------------------------------------------------------------------------------------
set(_d "${_wurzel}/tisch")
r36_exe("tisch" "${_d}" "200,-10300,1024" 20)
file(STRINGS "${_d}/wf.log" _schuss REGEX "arms_rec=0|SPAWN id=2")
list(LENGTH _schuss _n_schuss)
if(_n_schuss LESS 1)
    r36_fehler("tisch" "kein Schuss im wf.log (arms_rec=0 / Muendungsblitz) — der Abzug kam nicht")
endif()
r36_keiner_verliert("tisch" "${_d}/state.log")

set(_d "${_wurzel}/fresser")
r36_exe("fresser" "${_d}" "-7600,-17600,0" 20)
r36_keiner_verliert("fresser" "${_d}/state.log")

set(_d "${_wurzel}/frei")
r36_exe("frei" "${_d}" "200,-18200,1024" 20)
r36_gegner("${_d}/state.log" 5 _hp0 _hpmin)
if(_hpmin GREATER_EQUAL _hp0)
    r36_fehler("frei" "Gegenprobe: Platz 5 hinter dem Tisch in 1400 verliert keine HP (${_hp0} -> ${_hpmin}) — der Resolver schiesst nicht mehr")
endif()
message(STATUS "${_tag} [frei]: Platz 5 HP ${_hp0} -> ${_hpmin} (Gegenprobe Treffer)")
# Granatwerfer-Variante (Wortlaut "alle Schusswaffen"): GL Explosiv 15 — die Runde laeuft nicht durch
# FUN_80011f50, sondern als RE2-Projektil (re2_fx.c, Spur B r35: Zellen-Sperre am Tisch, Applier-Gates
# +0x1D3/+0x10E @0x80047138-64 gegen Fresser). Dieselben zwei Aufstellungen, dieselbe Erwartung.
set(_d "${_wurzel}/gl_tisch")
r36_exe("gl_tisch" "${_d}" "200,-10300,1024" 15)
r36_keiner_verliert("gl_tisch" "${_d}/state.log")

set(_d "${_wurzel}/gl_fresser")
r36_exe("gl_fresser" "${_d}" "-7600,-17600,0" 15)
r36_keiner_verliert("gl_fresser" "${_d}/state.log")

message(STATUS "-- ${_tag}: OK — Tisch sperrt (Python, GL), Fresser gesperrt (Python, GL), Gegenprobe trifft")
