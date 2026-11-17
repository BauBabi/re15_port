cmake_minimum_required(VERSION 3.16)   # cmake -P: ohne diese Zeile gelten im Linux-Container (CMake 3.28) die alten Policies (IN_LIST, if(TRUE)) - Muster der aelteren Integrationsskripte, Memory reai-v2-linux-test-portabilitaet
# =============================================================================
# LABOR-BIRKIN TRIFFT NACH DEM HAKEN WIEDER (Integration Runde 41, analysis/befunde_runde41/INT_labor_birkin.md).
#
# Befund: Seit 555556172 (Angriffsbox FUN_8001a5e0) traf der Birkin im Labor (50E0 Leon / 50F1 Elza) nach dem ersten
# Treffer (Haken) den stehenden Spieler nie wieder: Schleife ss1 4 -> 1 -> 4 bei Abstand ~2480, Spieler-hp blieb 90.
# Ursache: re15_bk_kontakt posierte die Klaue am Keyframe N = +0x95 nach anim_set; das Original liest die GEZEICHNETE
# Welt-Matrix Teil+0x40/+0x54..+0x5c (@0x8001a5ec / @0x8001a604 / @0x8001a610-638), und anim_set posiert +0x95 VOR dem
# Hochzaehlen (@0x8001f35c / @0x8001f36c, +1 erst @0x8001f618-61c) -> Box = KF N-2. Die Klaue ist bei KF 0x21 am
# Spieler; das Bissfenster ist +0x95 in [0x23..0x2a] (`dd ff 42 24` addiu -35 / `08 00 42 2c` sltiu 8 @0x801183e0-e4
# STAGE5). Im Original trifft der Biss also beim ersten Fensterbild +0x95 = 0x23: aca58 = 0x205 (0x23 < 0x26 @0x8011843c),
# Se(7), hp -10 (`f6 ff 42 24` @0x8011848c).
#
# Echter Weg (BERICHT/GEGENPRUEFUNG labor_check): Debug-Sprung nur fuer den Raum, Spieler an der Tuer-Ankunft aus 50D0
# (50E0: 0x0121A -> (-25450,-14700); 50F1: 0x011FA -> (-24550,-23650)), Route durch das Ausloesefeld AOT Slot 1
# (sub02 -> sub04 Member_set(0x0C,0x13)), danach steht der Spieler und schiesst mit Neu-Zielen (R1 los je Abschnitt).
# Waffe Super Redhawk (Id 7, Birkin-Schaden 50 = s_wpn_dmg_birkin[7] @0x8006e0d0-Zeile 0x30): mit der Pistole (15) und
# dem dauernd angegriffenen Spieler stirbt der Birkin in 50F1 erst nach > 9000 Bildern (gemessen f_e6, hp 150 bei F9000).
#
# Pruefungen je Lauf (state.log / debug.log):
#   1  (3,61) 0 -> 1: das Ereignis lief (echter Weg, nicht der Parkplatz)
#   2  erster Spieler-Treffer (Haken); danach mindestens ein weiterer Treffer (der Befund)
#   3  die ERSTE Biss-Episode danach (ss1 = 4, Clip 4) trifft den Spieler bei +0x95 = 0x23 (af = 35) mit hp -10 -
#      genau das Verhalten des Originals (s. o.). Alter Stand: die Episode laeuft ohne Treffer durch (ROT).
#      Seit der Klauen-Sonde (Kralle ACT[3], `jal 0x8001c2dc` @0x80117834 / STAGE5 @0x80118048) zieht Birkin auf dieser
#      Route die Klaue aus der Gangwand, bevor sie trifft; ist der erste Treffer der Biss selbst, zaehlt er als Haken und Biss.
#      Gemessen (r41/birkin_kralle): Leon erster Treffer F1000 ss1 4 af 35 -10, Elza F941 ss1 4 af 35 -10.
#      Pflicht nur noch im Leon-Lauf (50E0): NUTZERENTSCHEID 17.11.2026 "Nein" - ein Kontakt der Klinge beim Ausholen (Clip 3
#      af < 0x20, RE15_BKK_SCHLAG_AB) loest keinen Rueckzug mehr aus. Im Elza-Lauf (50F1) zog Birkin bis 32b4575e6 bei F1276
#      (af 23, Ausholen) zurueck und biss danach (Treffer F1346 af 35); jetzt laeuft der Hieb durch und trifft (F1293), eine
#      Biss-Episode kommt bis zum Tod nicht mehr. Kommt eine, gilt auch dort af 35 / -10.
#   4  Birkin stirbt (st = 3, hp <= 0)
#   5  (3,73) und (3,84) 0 -> 1 (sub01 ueber 5:28 aus der Todesmaschine)
# Gemessen mit dem Fix (Integrationsbaum, labor_check/angriff f_e5): Elza Haken F873, Biss F1014 af 35 hp 90 -> 80,
# Birkin tot F1885, Bits F2006. Gegenprobe a3051c902 (ohne Fix): Haken, danach kein Treffer mehr.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_labor_birkin_angriff.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_labor_birkin_angriff: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_labor_birkin_angriff: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41labor_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41labor_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

if(NOT DEFINED _exit_bild)
    set(_exit_bild 3000)
endif()
if(NOT DEFINED _laeufe)
    set(_laeufe leon elza)
endif()

set(_f "")
function(ein_lauf _name _raum _pos _route _extra _biss_pflicht)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    # Route durchs Ausloesefeld (GEGENPRUEFUNG labor_check 3.1), dann stehen, zielen, feuern, R1 kurz los (neu zielen).
    set(_skript "${_route},W2")
    foreach(_i RANGE 1 120)
        string(APPEND _skript ",M0.4,MA0.12,W0.1")
    endforeach()
    re15_start_spiel(_rv 400
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=${_pos}" RE15_ENEMY_DBG=1 RE15_SPAWN_DIAG=1
        RE15_STATE_LOG=state.log "RE15_FLAG_WATCH=3:61,3:73,3:84,5:28"
        "RE15_GIVE=7:255" RE15_EQUIP=7
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=300 "RE15_INPUT_SCRIPT=${_skript}"
        "RE15_EXIT_AT=${_exit_bild}#${_raum}"
        "${_exe_kopie}")
    set(_g "")
    if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.log")
        set(_f "${_f}\n ${_name}: kein debug.log/state.log (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    # 1 Ereignis
    if(NOT _dbg MATCHES "\\[flagwatch\\] F[0-9]+ ROOM${_raum} \\(3,61\\) 0 -> 1")
        string(APPEND _g "\n ${_name}: (3,61) nie gesetzt - Ausloesefeld nicht betreten")
    endif()
    # 2-4 aus state.log (Slot 1 = Birkin)
    file(STRINGS "${WORKDIR}/state.log" _zeilen REGEX "^F[0-9]+ .*PL\\(")
    set(_prev_hp "")
    set(_haken "")
    set(_biss_start "")
    set(_biss_ende "")
    set(_biss_treffer "")
    set(_tot "")
    set(_nach 0)
    set(_nach_liste "")
    foreach(_z IN LISTS _zeilen)
        if(NOT _z MATCHES "^F([0-9]+) .*PL\\([-0-9]+,[-0-9]+,rot=[-0-9]+,hp=([-0-9]+)\\)")
            continue()
        endif()
        set(_bild ${CMAKE_MATCH_1})
        set(_hp ${CMAKE_MATCH_2})
        set(_st "")
        set(_ss1 "")
        set(_af "")
        set(_bhp "")
        if(_z MATCHES "\\[1 t=30 st=([0-9]+) ss1=([0-9]+) ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=([0-9]+) af=([0-9]+)[^]]*\\] hp=(-?[0-9]+)")
            set(_st ${CMAKE_MATCH_1})
            set(_ss1 ${CMAKE_MATCH_2})
            set(_af ${CMAKE_MATCH_4})
            set(_bhp ${CMAKE_MATCH_5})
        endif()
        if(NOT _prev_hp STREQUAL "" AND _hp LESS _prev_hp AND NOT _haken STREQUAL "")
            math(EXPR _nach "${_nach} + 1")   # Spieler-Treffer NACH dem Haken (Befund: vorher keiner mehr)
            string(APPEND _nach_liste " F${_bild}/ss${_ss1}/af${_af}")
        endif()
        if(NOT _prev_hp STREQUAL "" AND _hp LESS _prev_hp)
            if(_haken STREQUAL "")
                set(_haken "${_bild}")
                # Kralle ACT[3] mit Klauen-Sonde (@0x80117834): auf dieser Route steckt die Klaue im Fenster in Wandzelle #17 / #3
                # (50E0) bzw. #12 / #2 (50F1) und Birkin zieht sie heraus (Clip 0xa), bevor sie Leon/Elza erreicht. Der erste
                # Treffer ist dann schon der Biss selbst (ss1 4): er zaehlt als Haken UND als die gesuchte Biss-Episode.
                if(_st STREQUAL "1" AND _ss1 STREQUAL "4")
                    math(EXPR _d "${_prev_hp} - ${_hp}")
                    set(_biss_start "${_bild}")
                    set(_biss_treffer "F${_bild} af=${_af} -${_d}")
                endif()
            elseif(NOT _biss_start STREQUAL "" AND _biss_ende STREQUAL "" AND _biss_treffer STREQUAL "")
                math(EXPR _d "${_prev_hp} - ${_hp}")
                set(_biss_treffer "F${_bild} af=${_af} -${_d}")
            endif()
        endif()
        if(NOT _haken STREQUAL "" AND _biss_ende STREQUAL "")
            if(_st STREQUAL "1" AND _ss1 STREQUAL "4" AND _biss_start STREQUAL "")
                set(_biss_start "${_bild}")
            elseif(NOT _biss_start STREQUAL "" AND NOT _ss1 STREQUAL "4")
                set(_biss_ende "${_bild}")
            endif()
        endif()
        if(_st STREQUAL "3" AND _tot STREQUAL "" AND NOT _bhp STREQUAL "" AND _bhp LESS_EQUAL 0)
            set(_tot "F${_bild} hp=${_bhp}")
        endif()
        set(_prev_hp ${_hp})
    endforeach()
    if(_haken STREQUAL "")
        string(APPEND _g "\n ${_name}: kein Spieler-Treffer (Haken)")
    elseif(_nach EQUAL 0)
        string(APPEND _g "\n ${_name}: nach dem Haken F${_haken} trifft Birkin den stehenden Spieler nie wieder (Befund 555556172)")
    elseif(_biss_start STREQUAL "" AND _biss_pflicht)
        string(APPEND _g "\n ${_name}: nach dem Haken F${_haken} keine Biss-Episode (ss1 4)")
    elseif(_biss_start STREQUAL "")
        # Elza (50F1), NUTZERENTSCHEID 17.11.2026 "Nein" (Kontakt beim Ausholen loest keinen Rueckzug aus, RE15_BKK_SCHLAG_AB): bis
        # 32b4575e6 zog Birkin bei F1276 (Clip 3 af 23, Klinge beim Ausholen in der Wand) zurueck und biss danach (F1331, Treffer
        # F1346 af 35). Jetzt laeuft dieser Hieb durch und trifft (F1293); es folgen Hiebe und Wuerfe, keine Biss-Episode bis zum
        # Tod. Gefordert bleibt der Befund (Treffer nach dem Haken); die Biss-Box (KF N-2, af 35) prueft der Leon-Lauf.
    elseif(_biss_treffer STREQUAL "")
        string(APPEND _g "\n ${_name}: Biss F${_biss_start}..F${_biss_ende} nach dem Haken F${_haken} verfehlt den stehenden Spieler (Original: Treffer bei +0x95 0x23, Box = KF N-2)")
    elseif(NOT _biss_treffer MATCHES "af=35 -10$")
        string(APPEND _g "\n ${_name}: Biss-Treffer ${_biss_treffer}, erwartet af=35 (+0x95 0x23 @0x801183e0) mit -10 (@0x8011848c)")
    endif()
    if(_tot STREQUAL "")
        string(APPEND _g "\n ${_name}: Birkin stirbt nicht bis F${_exit_bild}")
    endif()
    foreach(_bit 73 84)
        if(NOT _dbg MATCHES "\\[flagwatch\\] F[0-9]+ ROOM${_raum} \\(3,${_bit}\\) 0 -> 1")
            string(APPEND _g "\n ${_name}: (3,${_bit}) nach dem Tod nicht gesetzt")
        endif()
    endforeach()
    message(STATUS "r41_labor_birkin_angriff ${_name} (${_raum}): exit=${_rv} Haken F${_haken}, Biss ab F${_biss_start}: ${_biss_treffer}, Treffer nach dem Haken ${_nach}:${_nach_liste}, Tod ${_tot}${_g}")
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

if("leon" IN_LIST _laeufe)
    ein_lauf(leon 50E0 "-25450,-14700,0,0" "L0.34,XU1.4,R0.34,XU1.8,R0.34,XU3" "" ON)
endif()
if("elza" IN_LIST _laeufe)
    ein_lauf(elza 50F1 "-24550,-23650,0,0" "R0.34,XU1.0,L0.34,XU2.0,L0.34,XU4" "RE15_TITLE_SHOT_CHAR=1" OFF)
endif()

file(REMOVE "${_exe_kopie}")
if(_f)
    message(FATAL_ERROR "r41_labor_birkin_angriff:${_f}")
endif()
message(STATUS "r41_labor_birkin_angriff: OK (${_laeufe}, Laufverzeichnis ${_wurzel})")
