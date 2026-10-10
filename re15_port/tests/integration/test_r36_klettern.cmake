# =============================================================================
# KLETTERN AUF DEN VERSCHIEBBAREN TRITT MIT DER ECHTEN EXE (Runde 36, Spur A — BLOCKER):
# "aus irgendeinem Grund kann ich nicht mehr auf die verschiebbaren Leitern/Tritte klettern - z.B.
#  ROOM 10F0. Das macht einen Fortschritt im Spiel unmoeglich".
#
# Dossier analysis/befunde_runde36/A_klettern.md. Ursache: die Port-Sonde FUN_8002d2c0 verlangte
# (flags & 0x101) == 0x101; das Original @0x8002d358-35c (`andi v0,v0,0x101` / `beq v0,zero`)
# ueberspringt nur bei 0. Der Tritt in ROOM10F0/10F1 (obj 0, Typ 4, sub00 @0x010F4/@0x0111A,
# pc[6..7] = 00 00 -> Flags 0x0001) fiel damit durch. Die Unit-Riegel unit_r36_klettern_10f0_* messen
# die Sonde in der VM; dieser Riegel misst den ECHTEN WEG mit der exe: Tritt aus der Startlage
# (-6050,0,4950) verschieben (Substate 8, FUN_8002bd44-Objektpass), hochklettern (Substate 9,
# KIND 0x81 @0x8002d614/18, Band 0 -> 1 @0x80038200-08), oben ankommen (Standobjekt @0x80031d20)
# und von dort die Tuer nach ROOM1090/1091 (Door_aot_set Slot 1 @0x00F52, floor 1) nehmen.
#
# Laeufe (RE15_CLIMB_TRACE schreibt je Spielbild eine Zeile; Tasten als Eingabeskript auf der
# Spielbild-Zeitachse):
#   A  LEON, ROOM10F0 per Sprung (Szene (9,71) schon gesehen), Leon westlich des Tritts: Tritt nach +X
#      schieben, aussen herum, Tritt nach -Z schieben, Aktionstaste -> sub=9 kind=0x81 -> oben
#      (y=-1800, band=1, stand=Tritt) -> Tuer -> ROOM1090.
#   B  ELZA, ROOM10F1 ueber RE15_GOTO_ROOM (Spawn = Tuer-Eintritt (8400,0,-350) aus ROOM10D1), Weg durch
#      den Raum (Ostflur, Durchgang z~9200, Mittelstreifen, Westteil), dann wie A -> Tuer -> ROOM1091.
#   C  LEON, ERSTES Betreten von ROOM10F0 ((9,71)=0): die Runde-35-Szene (Ada/Leon/Marvin) laeuft
#      zu Ende, danach vom Szenen-Endpunkt (4792,0,11354) aus wie B -> Tuer -> ROOM1090.
#      (Belegt, dass die Szene keinen Zustand hinterlaesst, der die Sonde sperrt.)
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r36_klettern.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r36_klettern: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r36_klettern: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name (Muster test_r34n_b_cursor.cmake / test_r35_cut10f0.cmake).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r36a_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r36a_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

set(_fehler "")
macro(befund _text)
    message(STATUS "r36_klettern: FEHLER ${_text}")
    set(_fehler "${_fehler}\n  ${_text}")
endmacro()

# Der gemeinsame Schluss aller Laeufe (ab dem Punkt "Leon/Elza steht westlich des Tritts, Blick +X"):
#   Tritt nach +X schieben (U2.6), aussen herum (L, U, R, U, R), Tritt nach -Z schieben (U10.5),
#   Aktionstaste (A) = Klettern, oben nach -Z gehen (U0.6), Aktionstaste = Tuer.
set(_schluss "U2.6,W1,L0.37,W0.3,U0.5,W0.3,R0.37,W0.3,U0.55,W0.3,R0.37,W0.5,U10.5,W1,A0.1,W2.5,U0.6,W0.3,A0.1,W3")
# Der Weg von der Tuer (8400,0,-350, Gierung 2048) bzw. vom Szenen-Endpunkt bis westlich des Tritts.
set(_weg_tuer "W3,XU2,W0.4,R0.37,W0.4,XU1.6,W0.4,L0.37,W0.4,XU0.9,W0.4,L0.37,W0.4,XU1.3,W0.4,R0.37,W0.4,XU1.35,W0.4,R0.37,W0.4,XU0.6,W0.4,R0.37,W0.5")
set(_weg_szene "L0.13,W0.3,XU0.37,W0.4,R0.37,W0.4,XU0.77,W0.4,L0.37,W0.4,XU1.4,W0.4,R0.37,W0.4,XU1.35,W0.4,R0.37,W0.4,XU0.6,W0.4,R0.37,W0.5")

# lauf(<name> <raum-hex> <zielraum-hex> <exit-bild> <timeout> <extra-env...>)
function(klettern_lauf _name _raum _ziel _exit _timeout)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/debug.log" "${WORKDIR}/climb.txt")
    re15_start_spiel(_rv ${_timeout}
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        RE15_WINDOW_SCALE=1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_CLIMB_TRACE=climb.txt
        "RE15_EXIT_AT=${_exit}" ${ARGN}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/climb.txt")
        befund("[${_name}] exe exit=${_rv}, kein climb.txt in ${WORKDIR}")
        set(_fehler "${_fehler}" PARENT_SCOPE)
        return()
    endif()
    file(STRINGS "${WORKDIR}/climb.txt" _zeilen)
    file(STRINGS "${WORKDIR}/debug.log" _log)
    string(TOUPPER "${_raum}" _RAUM)
    string(TOUPPER "${_ziel}" _ZIEL)

    # 1. Der Tritt wurde VERSCHOBEN: zuletzt im Zielraum-Vorraum steht er in der Schiebezone
    #    (Aot_set Slot 10 @0x010E0: x -5800..-3750, z -1800..250), also x > -5800 und z < 250.
    set(_t4_start 0)
    set(_t4_ende 0)
    set(_klettern 0)
    set(_oben 0)
    set(_push 0)
    foreach(_z IN LISTS _zeilen)
        if(_z MATCHES "room=${_RAUM} .* t4pos=\\(-6050,4950\\)")
            set(_t4_start 1)
        endif()
        if(_z MATCHES "room=${_RAUM} .* push=5 ")
            set(_push 1)
        endif()
        if(_z MATCHES "room=${_RAUM} .* sub=9 kind=0x81 ")
            set(_klettern 1)
        endif()
        if(_z MATCHES "room=${_RAUM} .* pos=\\(-?[0-9]+,-1800,-?[0-9]+\\) rot=[0-9]+ band=1 sub=0 .* stand=3 .* t4=3 t4pos=\\((-[0-9]+),(-[0-9]+)\\)")
            set(_oben 1)
            if(${CMAKE_MATCH_1} GREATER -5800 AND ${CMAKE_MATCH_2} LESS 250)
                set(_t4_ende 1)
            endif()
        endif()
    endforeach()
    if(NOT _t4_start)
        befund("[${_name}] Tritt nicht in der Startlage (-6050,4950) gesehen (ROOM${_RAUM} sub00 @0x010F4)")
    endif()
    if(NOT _push)
        befund("[${_name}] kein Schieben (Substate 8 Phase 5 = Clip 0x12) im Log")
    endif()
    if(NOT _klettern)
        befund("[${_name}] KEIN Klettern: keine Zeile 'sub=9 kind=0x81' (FUN_8002d474 @0x8002d604/@0x8002d618)")
    endif()
    if(NOT _oben)
        befund("[${_name}] nicht OBEN angekommen (y=-1800, band=1, Standobjekt = Tritt, @0x80038200-08/@0x80031d20)")
    elseif(NOT _t4_ende)
        befund("[${_name}] oben, aber der Tritt steht nicht in der Schiebezone (x > -5800, z < 250)")
    endif()
    # 2. Die Tuer nach ROOM<ziel> wurde von oben genommen (floor-1-Zone, Slot 1 @0x00F52).
    set(_ziel_ok 0)
    foreach(_l IN LISTS _log)
        if(_l MATCHES "EXIT_AT: Bild [0-9]+ in Raum ${_ziel} erreicht")
            set(_ziel_ok 1)
        endif()
    endforeach()
    if(NOT _ziel_ok)
        befund("[${_name}] Tuer von oben NICHT genommen: kein 'EXIT_AT ... in Raum ${_ziel}' (exe exit=${_rv})")
    endif()
    message(STATUS "r36_klettern: [${_name}] start=${_t4_start} schieben=${_push} klettern=${_klettern} oben=${_oben} zone=${_t4_ende} ziel=${_ziel_ok}")
    set(_fehler "${_fehler}" PARENT_SCOPE)
endfunction()

# A — Leon, Sprung nach ROOM10F0, Szene schon gesehen ((9,71)=1), Leon westlich des Tritts, Blick +X.
klettern_lauf(A_leon_10f0 10f0 1090 1400 240
    RE15_DEBUG_JUMP=10f0@250 "RE15_SET_FLAG=9:71" "RE15_PLAYER_POS=-7125,4950,0"
    RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=W10,${_schluss}")

# B — Elza, ROOM10F1 ueber RE15_GOTO_ROOM (Spawn = Tuer-Eintritt), echter Weg durch den Raum.
klettern_lauf(B_elza_10f1 10f1 1091 1320 240
    RE15_TITLE_SHOT_CHAR=1 RE15_GOTO_ROOM=10F1 "RE15_SET_FLAG=9:71"
    RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_weg_tuer},${_schluss}")
# Zusatz fuer B: wirklich ELZA im Spiel (Spieler-Familie PL04, Muster test_r35_werfer_elza.cmake), nicht
# ein stiller Rueckfall auf Leon im Elza-Raum.
if(EXISTS "${_basis}_B_elza_10f1/debug.log")
    file(STRINGS "${_basis}_B_elza_10f1/debug.log" _logb REGEX "\\[pl\\] Spieler-Familie PL04")
    if("${_logb}" STREQUAL "")
        befund("[B_elza_10f1] kein '[pl] Spieler-Familie PL04' im debug.log — Elza nicht im Spiel")
    endif()
endif()

# C — Leon, ERSTES Betreten (Szene laeuft, Ende bei Bild ~2994, Kartenhinweis bis ~3250), Weg ab 3400.
klettern_lauf(C_leon_szene 10f0 1090 4550 480
    RE15_DEBUG_JUMP=10f0@250
    RE15_INPUT_SCRIPT_START=3400 "RE15_INPUT_SCRIPT=${_weg_szene},${_schluss}")
# Zusatz fuer C: die Szene ist wirklich gelaufen.
file(STRINGS "${_basis}_C_leon_szene/debug.log" _logc)
set(_szene_start 0)
set(_szene_ende 0)
foreach(_l IN LISTS _logc)
    if(_l MATCHES "\\[cut10f0\\] ROOM10F0: Szene gestartet")
        set(_szene_start 1)
    endif()
    if(_l MATCHES "\\[cut10f0\\] Szene zu Ende")
        set(_szene_ende 1)
    endif()
endforeach()
if(NOT _szene_start OR NOT _szene_ende)
    befund("[C_leon_szene] Runde-35-Szene nicht gelaufen/beendet (start=${_szene_start} ende=${_szene_ende})")
endif()

file(REMOVE "${_exe_kopie}")
if(NOT "${_fehler}" STREQUAL "")
    message(FATAL_ERROR "r36_klettern: Riegel verletzt:${_fehler}")
endif()
message(STATUS "r36_klettern: OK — Tritt verschoben, hochgeklettert, oben angekommen, Tuer genommen (Leon 10F0 x2, Elza 10F1)")
