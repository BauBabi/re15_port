# =============================================================================
# Runde 42 "story" (v0.8.31), Stufe STAGE3a (Marvin) - DURCHLAEUFE in der echten exe (RE15_WEGPLAN: Gehen, Blick, Aktionstaste,
# Zielen/Feuern wie der Spieler; Raumwechsel ueber die Tueren des Spiels). Konstanten/Belege include/re15_story3a_r42.h, Code
# engine/src/story3a_r42.c. PORT-STORY nach dem Konzept (5 T-01/T-02, 3.2 L5-L7, L-05..L-10; F-04, F-05, F-06).
#
# Zustand = Ende der Stufe STAGE1 (integration_r42_story1_*: 9:90 9:111 9:112 9:113 9:118 9:119 3:245 3:206 3:151 9:121 + 9:120 Szene an
# der Leiter gesehen). Start in 3000 (RE15_GOTO_ROOM, die Leichenszene sub03 laeuft wie beim Einstieg ueber die Leiter), Leon mit der
# Pistole (RE15_EQUIP=3 - das Ausruesten im Menue kann der Wegplan nicht) und 200 H. Gun Bullets (RE15_GIVE=21:200).
#
# NB1 B3 (Pruefer pruef_s3a_r1): KEIN Raumsprung mehr. Marvin geht seinen ganzen Weg im Spiel: 30C0 -> 3090 -> 30A0 (Red Master Keycard,
# Taken-Bit 180 @0x00742) -> 3090 -> 30C0 (Treppe Slot 3/4 auf Band 4) -> 3030 -> 30E0 (OST-Ring, Ankunft (7025,2700)) -> 3050 (Kiste
# schieben, Fall, Lift sub04 (3,82), Kiste in die Kerbe (3,93), Klettern, Super Redhawk Bit 92 @0x01EC6, Absprung) -> 3040 (Treppe
# Slot 2/3, Knopf sub03 (3,0xB1) = Kisten weg, Gang Band 1, Treppe Slot 4/5 hinab, Green Keycard Bit 85 @0x014B2, Schloss (4,0xC4)
# sub02) -> orange Tuer -> 3060. Wegpunkte aus der SCA (Planer scratchpad story/s3a/weg.py) und in Einzellaeufen nachgemessen
# (scratchpad story/nb1/d2..d16).
# Drei Stellen, die der Wegplan-Bot NICHT spielen kann, sind als Ergebnis vorgesetzt (RE15_SET_FLAG), alles andere ist echt:
#   - 3050 Rechner K8 (Kistenregel @0x01F90): nach USE der Red Master Keycard (keyitems_r41.c Stelle 0x3050/0x46) folgt ein
#     Kran-Steuerspiel (sub01 @0x0219E.. Sce_key_ck 01/04/02/08 -> sub11..14 Speed_set obj 3, Treffer Member_cmp 0x0f == 0x0b @0x0220E
#     -> sub15 @0x0260E/@0x02612 Set(3,0xB3)/(3,0xB0)). Vorgesetzt: (3,0xB3) + (3,0xB0) = "System wiederhergestellt"; die Karte
#     bleibt dadurch bei Marvin und geht in die Uebergabe (msg 20).
#   - 3040 zwei kleine Affen 0x29 (main00 @0x01310/@0x01324, Kill-Ids 0x71/0x72): Bot-Laeufe d10a/d11a/d12a - 1 Treffer aus 66 Schuss,
#     Marvin stirbt. Vorgesetzt: Kill-Bits 7:113/7:114 (Em-Status-Tor FUN_800420a0 @0x80042120-38).
#   - 3010 sieben Zombies nach (4,0x0A) @0x01052 (main00 @0x02122..@0x021EA, Kill-Ids 0x40..0x46): Bot-Laeufe d13/d15 (Pistole) - Leon
#     stirbt an der Ankunft (HP 100 -> 60 -> 0), d14 (SPAS-12) - drei Zombies tot, dann laedt die SPAS nicht nach (mg 0). Vorgesetzt wie
#     bisher: Kill-Bits 7:64..7:70. Die drei Hunde in 3060 kaempft Leon echt (JAGE, F-04).
#
# TEIL kette     3000 -> 3010 -> 3020 (Fensterszene, Zeilen 0..2 Neutext + 7/8, Ausloesezone per LAUF) -> Marvin (Tausch: Browning x15
#                + 50 H. Gun Bullets, Platz 0, Waffe 3) -> Marvins Weg (s.o.) -> 3060 (Hunde, Zeile 11, 3 Schuesse, Abblende,
#                Rueckwechsel) -> Leon 3020 ("Gunshots...?", Leons Seite zurueck) -> 3010 -> 3060 (Leon kaempft gegen die Hunde) ->
#                Wiedersehen (12..27 mit 19/20 je Fund: Red Master Keycard, Super Redhawk; Untertitel OHNE Taste, Systemtexte mit
#                Taste: TIPPE ... 1) -> 3010 -> Laderampe -> Kartentuer (erst jetzt offen, L-10) -> 30E0.
# TEIL speichern Lauf 1 bis Marvin in 30C0 (direkt nach dem Tausch), Speichern als Marvin (RE15_SAVE_TEST_AGAIN, Karte im Arbeitsordner);
#                Lauf 2 laedt (RE15_CONTINUE_TEST) und spielt Marvins ganzen Weg bis 3060 -> Rueckwechsel: Leon hat alles zurueck.
# Kartentuer: msg 1 "You've used the Green Keycard." und die Wegwerf-Frage (gen/discard_sites.inc 3010 msg 1 -> 0x36) schliesst wie im
# Spiel erst ein Tastendruck - TIPPE 9 255 = Quadrat je 30 Bilder fuer 200 Bilder (Bank 9 Bit 255 ist nirgends belegt, Zensus).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -DTEIL=<teil> -P test_r42_story3a.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r42_story3a: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r42_story3a: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL kette)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}_${TEIL}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r42s3a_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r42s3a_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_fehler "")
macro(fail _t)
    list(APPEND _fehler "${_t}")
    message(STATUS "FAIL r42_story3a[${TEIL}]: ${_t}")
endmacro()
macro(ok _t)
    message(STATUS "ok   r42_story3a[${TEIL}]: ${_t}")
endmacro()
macro(pruefe _text _muster)
    string(FIND "${_dbg}" "${_muster}" _p)
    if(_p LESS 0)
        fail("${_text} (fehlt: '${_muster}')")
    else()
        ok("${_text}")
    endif()
endmacro()
macro(pruefe_re _text _regex)
    if(_dbg MATCHES "${_regex}")
        ok("${_text}")
    else()
        fail("${_text} (fehlt: /${_regex}/)")
    endif()
endmacro()

set(_basis RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_EQUIP=3 "RE15_GIVE=21:200"
           "RE15_SET_FLAG=9:90,9:111,9:112,9:113,9:118,9:119,3:245,3:206,3:151,9:121,9:120,7:64,7:65,7:66,7:67,7:68,7:69,7:70,7:113,7:114,3:176,3:179"
           RE15_GOTO_ROOM=3000 RE15_WEGPLAN_LOG=wegplan.txt)

# Weg Leon 3000 -> 3020 -> (Szene) -> Marvin in 30C0. Wegpunkte aus der SCA (Planer scratchpad story/s3a/weg.py, Band 0,
# Spielerradius 450) und im Lauf nachgemessen.
set(_weg_vor_30c0 "RAUM 3000
WARTE 30
GEHE -5555 -17520 300
BLICK 0
AKTION
RAUM 3010
WARTE 30
GEHE -22790 10744 250
GEHE -19040 -14756 250
GEHE -8990 -20006 250
GEHE -7340 -18356 250
GEHE -6500 -18300 250
BLICK 0
AKTION
RAUM 3020
WARTE 30
GEHE -19500 -19000 300
BLICK 3656
LAUF 2 7 90
RAUM 30C0
WARTE 60
")
# Marvins ganzer Weg 30C0 -> 3090 -> 30A0 -> 3090 -> 30C0 -> 3030 -> 30E0 -> 3050 -> 3040 (NB1 B3, Kopf oben).
set(_weg_marvin "GEHE -8750 -6800 250
GEHE -7550 -1250 250
GEHE -6350 -350 150
BLICK 0
WARTE 10
AKTION
RAUM 3090
WARTE 30
GEHE -25832 -5996 250
GEHE -25832 -11546 250
GEHE -20732 -13046 250
GEHE -13232 -9746 150
BLICK 3072
WARTE 10
AKTION
RAUM 30A0
WARTE 30
GEHE -19543 13613 250
GEHE -19393 15713 250
GEHE -19693 15863 250
GEHE -26700 17500 80
BLICK 3072
WARTE 10
TIPPE 9 180 400
WARTE 30
GEHE -19543 15863 250
GEHE -19393 13763 250
GEHE -24643 10913 250
GEHE -25543 11813 150
BLICK 2048
WARTE 10
AKTION
RAUM 3090
WARTE 30
GEHE -20582 -13046 250
GEHE -25832 -11546 250
GEHE -25832 -5996 250
GEHE -26000 -1046 150
BLICK 2048
WARTE 10
AKTION
RAUM 30C0
WARTE 30
GEHE -19550 -3200 250
GEHE -21650 -1100 250
GEHE -21700 300 150
BLICK 0
WARTE 10
AKTION
WARTE 400
GEHE -11800 300 250
BLICK 3072
WARTE 10
AKTION
RAUM 3030
WARTE 30
GEHE -25652 -17348 250
BLICK 3072
WARTE 10
AKTION
RAUM 30E0
WARTE 30
GEHE 4300 4550 100
BLICK 2048
WARTE 10
AKTION
OBJ 1
RAUM 3050
WARTE 30
GEHE -6450 1523
GEHE -6450 -20750
GEHE -11880 -20500
GEHE -16650 -20300 120
SCHIEB 2048 X <= -24040 1500
WARTE 40
GEHE -22720 -22540
GEHE -24058 -22150 100
SCHIEB 3072 Z >= -17000 900
WARTE 200
GEHE -16650 -20300
GEHE -11880 -20500
GEHE -6450 -20750
GEHE -6450 -8150
GEHE -4400 -8150 120
GEHE -1900 -8150 150
BLICK 0
WARTE 20
AKTION
WARTE 400
FLAG 3 82
GEHE -4400 -8150 120
GEHE -8000 -8150
GEHE -8000 -14200
GEHE -20000 -14200
GEHE -24058 -14250 100
SCHIEB 1024 Z <= -22200 1500
WARTE 60
FLAG 3 93
GEHE -24300 -18500 150
GEHE -24390 -20875 100
BLICK 1024
WARTE 20
AKTION
WARTE 150
GEHE -24058 -22900 100
BLICK 1024
TIPPE 9 92 400
WARTE 60
GEHE -23400 -22200 100
BLICK 0
WARTE 20
AKTION
WARTE 150
GEHE -22000 -18000 250
GEHE -24000 -15000 250
GEHE -24000 1400 100
BLICK 3072
WARTE 10
AKTION
RAUM 3040
WARTE 30
GEHE -24300 -13150 200
BLICK 0
WARTE 10
AKTION
WARTE 200
GEHE -20850 -13250 60
BLICK 1024
WARTE 10
TIPPE 3 177 400
WARTE 200
GEHE -19275 -12525 200
GEHE -18825 -8025 200
GEHE -18800 -2200 200
GEHE -9000 -2200 200
GEHE -9000 -3500 200
GEHE -9000 -5700 200
GEHE -12500 -5700 200
GEHE -13400 -7500 200
GEHE -13400 -12500 200
GEHE -10880 -13575 50
BLICK 0
WARTE 10
AKTION
WARTE 200
GEHE -8100 -14200 250
GEHE -8250 -19150 250
GEHE -9150 -22000 250
GEHE -12900 -24700 250
GEHE -12850 -24300 100
BLICK 1024
TIPPE 9 85 400
WARTE 30
GEHE -8850 -26350 250
BLICK 1024
TIPPE 4 196 300
")
set(_weg_tuer_3040 "GEHE -8850 -26350 250
BLICK 1024
TIPPE 4 196 300
TIPPE 3 160 900
RAUM 3020
WARTE 300
")
set(_weg_leon "GEHE -12650 -14718 250
GEHE -23900 -22968 250
GEHE -26150 -22518 250
GEHE -26750 -22500 650
BLICK 2048
AKTION
RAUM 3010
WARTE 30
RAEUME 6000 2500
GEHE -18890 -14906 250
RAEUME 6000 2500
GEHE -22040 -8606 250
RAEUME 6000 2500
GEHE -19190 9244 250
RAEUME 6000 2500
GEHE -19040 9394 250
RAEUME 6000 2500
GEHE -7340 9844 250
RAEUME 6000 2500
GEHE -6563 9810 650
RAEUME 6000 2500
BLICK 0
AKTION
RAUM 3060
WARTE 30
JAGE 7 80 2000 99999 14000
JAGE 7 81 1500 99999 14000
JAGE 7 82 1500 99999 14000
JAGE 7 80 3000 9000 9000
JAGE 7 81 3000 9000 9000
JAGE 7 82 3000 9000 9000
TIPPE 9 124 5000 1
WARTE 60
GEHE -24506 -24532 250
GEHE -26606 -22432 250
GEHE -26900 -22400 650
BLICK 2048
AKTION
RAUM 3010
WARTE 30
RAEUME 6000 2500
GEHE -19040 9394 250
RAEUME 6000 2500
GEHE -21890 3844 250
RAEUME 6000 2500
GEHE -22040 -16706 250
RAEUME 6000 2500
GEHE -21500 -16900 100
RAEUME 6000 2500
BLICK 1024
AKTION
WARTE 90
GEHE -25976 -19836 250
RAEUME 6000 2500
GEHE -26750 -19800 800
RAEUME 6000 2500
BLICK 2048
TIPPE 3 60 600
TIPPE 9 255 200
RAUM 30E0
WARTE 30
ENDE
")

function(_fertig)
    file(REMOVE "${_exe_kopie}")
    list(LENGTH _fehler _nf)
    if(_nf GREATER 0)
        message(FATAL_ERROR "r42_story3a[${TEIL}]: ${_nf} Fehler (${WORKDIR})")
    endif()
    message(STATUS "r42_story3a[${TEIL}]: alle Pruefungen ok (${WORKDIR})")
endfunction()

if(TEIL STREQUAL "kette")
    file(WRITE "${WORKDIR}/plan.txt" "${_weg_vor_30c0}${_weg_marvin}${_weg_tuer_3040}${_weg_leon}")
    re15_start_spiel(_rv 2950 ${_basis} "RE15_WEGPLAN=${WORKDIR}/plan.txt" RE15_STATE_LOG=state.log "RE15_EXIT_AT=400#30E0" "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        fail("kein debug.log (exit=${_rv})")
        _fertig()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    set(_wp "")
    if(EXISTS "${WORKDIR}/wegplan.txt")
        file(READ "${WORKDIR}/wegplan.txt" _wp)
    endif()
    pruefe("3020: Fensterszene = sub02 mit Zeilen 7/8 vor dem Wechsel @0x0D7A" "ROOM3020: Fensterszene = sub02 mit Zeilen 7/8")
    foreach(_id 7 8)
        pruefe("3020: Zeile ${_id} (neu)" "room=3020 id=${_id} ")
    endforeach()
    # NB1 B1: der WORTLAUT der Glaettung (re15_story3a_r42_texte nach re15_msg_load_room_block), nicht nur die Id
    pruefe("B1: 3020 msg 0 Neutext" "[msgtext] room=3020 id=0 \"Leon: Damn it! It won't budge.\"")
    pruefe("B1: 3020 msg 1 Neutext" "[msgtext] room=3020 id=1 \"Marvin: There's an open window up there. We could get in that way.\"")
    pruefe("B1: 3020 msg 2 Neutext" "[msgtext] room=3020 id=2 \"Marvin: Leon, give me a hand!\"")
    # NB1 B3: Marvins Weg im Spiel (Wegplan-Log)
    foreach(_r 3090 30A0 3030 30E0 3050 3040)
        string(FIND "${_wp}" "RAUM ${_r} geladen" _p)
        if(_p LESS 0)
            fail("B3: Marvin erreicht ${_r} nicht")
        else()
            ok("B3: Marvin in ${_r}")
        endif()
    endforeach()
    string(FIND "${_wp}" "TIPPE: (9,180) = 1" _p)
    if(_p LESS 0)
        fail("B3: Red Master Keycard 30A0 (Bit 180 @0x00742) nicht genommen")
    else()
        ok("B3: Red Master Keycard genommen (30A0)")
    endif()
    string(FIND "${_wp}" "TIPPE: (9,92) = 1" _p)
    if(_p LESS 0)
        fail("B3: Super Redhawk 3050 (Bit 92 @0x01EC6) nicht genommen")
    else()
        ok("B3: Super Redhawk genommen (3050, Kiste/Lift/Kerbe/Klettern)")
    endif()
    foreach(_f "(3,82) = 1" "(3,93) = 1" "(3,177) = 1" "(9,85) = 1" "(4,196) = 1")
        string(FIND "${_wp}" "${_f}" _p)
        if(_p LESS 0)
            fail("B3: ${_f} fehlt (Lift 3050 / Kerbe 3050 / Knopf 3040 / Keycard 3040 / Schloss 3040)")
        else()
            ok("B3: ${_f}")
        endif()
    endforeach()
    if(_wp MATCHES "room=30E0 spieler=\\(7025,2700\\)")
        ok("B2: Marvin betritt 30E0 im Ost-Ring (7025,2700) - die Rueckseite der Kartentuer liegt im West-Ring")
    else()
        fail("B2: Marvins Ankunft in 30E0 (7025,2700) fehlt")
    endif()
    pruefe("T-01: Leons Seite gesichert" "Tausch Leon -> Marvin, Leons Seite gesichert: 03 x15 15 x200")
    pruefe("T-01: Marvin Platz 0, Waffe 3 (Browning x15, H. Gun Bullets x50)" "Marvin: Platz 0 Waffe 3 (Browning x15, H. Gun Bullets x50)")
    pruefe("T-01: Raumlader meldet Waffe 3 fuer Marvin (kein Messer)" "[sherry-r41] Spielermodell 0 -> 13, Waffe (aca5d) 3,")
    pruefe("3060 Marvin: Zeile 11, 3 Schuesse, Abblende" "ROOM3060: Schuss 3/3")
    pruefe("3060 Marvin: Schwarz bis zum Rueckwechsel" "Schwarz gehalten bis zum Raumwechsel - Rueckwechsel auf Leon")
    pruefe("T-02: Marvins Seite mit allen Funden gemerkt (Red Master Keycard, Super Redhawk, Green Keycard)"
           "Marvins Seite (Uebergabe offen): 03 x15 15 x50 46 x1 07 x6 36 x1")
    pruefe("T-01: Leon hat alles zurueck (auch die Waffe)" "Leon: Ausruest-Platz 0x00 Waffe 3")
    pruefe("T-01: Raumlader meldet Waffe 3 fuer Leon" "[sherry-r41] Spielermodell 13 -> 0, Waffe (aca5d) 3,")
    pruefe("L-08: 3020 Leon hoert die Schuesse (msg 9)" "ROOM3020: Leon hoert die Schuesse -> msg 9")
    pruefe("L-08: msg 9 gezeigt" "room=3020 id=9 ")
    pruefe("F-04: Leon toetet die Hunde, dann das Wiedersehen" "ROOM3060: Hunde tot (Zone 7 Bits 0x50..0x52) -> Wiedersehen")
    foreach(_id 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27)
        pruefe("L5-L7: 3060 Zeile ${_id}" "room=3060 id=${_id} ")
    endforeach()
    pruefe("L6/F-05: Keycard uebergeben" "Marvin uebergibt Item 0x36 x1 -> Leons Inventar")
    pruefe("L6/F-06: Super Redhawk uebergeben (msg 19/20)" "Marvin uebergibt Item 0x07 x6 -> Leons Inventar")
    pruefe("L6/F-06: Red Master Keycard uebergeben (msg 20)" "Marvin uebergibt Item 0x46 x1 -> Leons Inventar")
    pruefe("msg 20 mit Namen" "You've received the Super Redhawk.")
    # NB1 B8 Punkt 4: im Wiedersehen drueckt der Plan NUR bei Systemtexten (TIPPE ... 1); die Untertitel 12..27 laufen ohne Taste aus
    if(_wp MATCHES "TIPPE: Systemtext msg (1[2-579]|2[1-7]) ")
        fail("B8: Taste bei einem Untertitel des Wiedersehens")
    else()
        ok("B8: Untertitel 12..27 ohne Taste ausgelaufen (Druecke nur bei Systemtexten)")
    endif()
    string(FIND "${_wp}" "TIPPE: Systemtext msg 16" _p)
    if(_p LESS 0)
        fail("B8: Systemtext 16 nicht per Taste bestaetigt")
    else()
        ok("B8: Systemtexte per Taste bestaetigt")
    endif()
    pruefe("L6/F-05: Fabrikplan uebergeben (Bit 35)" "Fabrikplan uebergeben -> Bit 35 = 1 (Kartenblatt 7)")
    pruefe("L5 Punkt 6: Marvin geht durch die orange Tuer" "geht durch die orange Tuer")
    pruefe("Bit 124" "Wiedersehen fertig nach")
    pruefe("L-10: Kartentuer 3010 benutzt (msg 1 You've used the Green Keycard.)" "room=3010 id=1 ")
    pruefe("L-10: 30E0 erreicht" "PC loaded room30e0.rdt")
    pruefe("Lauf bis 30E0 (Leon, nach der Kartentuer)" "EXIT_AT: Bild 400 in Raum 30e0")
    _fertig()
elseif(TEIL STREQUAL "speichern")
    # Lauf 1: bis Marvin in 30C0 (direkt nach dem Tausch), dort speichern.
    file(WRITE "${WORKDIR}/plan1.txt" "${_weg_vor_30c0}WARTE 400\nENDE\n")
    re15_start_spiel(_rv 900 ${_basis} "RE15_WEGPLAN=${WORKDIR}/plan1.txt" RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
                     "RE15_SAVE_TEST_AGAIN=150#30C0" "RE15_EXIT_AT=320#30C0" "${_exe_kopie}")
    file(READ "${WORKDIR}/debug.log" _dbg)
    file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug_1.log")
    pruefe("Lauf 1: Tausch Leon -> Marvin" "Tausch Leon -> Marvin, Leons Seite gesichert: 03 x15 15 x200")
    pruefe("Lauf 1: gespeichert als Marvin in 30C0" "[save] saved (room 30c0)")
    # Lauf 2: laden und Marvins ganzen Weg bis 3060 spielen (Rueckwechsel nach dem Laden).
    file(WRITE "${WORKDIR}/plan2.txt" "RAUM 30C0\nWARTE 30\n${_weg_marvin}${_weg_tuer_3040}ENDE\n")
    re15_start_spiel(_rv2 2200 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
                     RE15_CARD_SLOT=0 "RE15_WEGPLAN=${WORKDIR}/plan2.txt" RE15_WEGPLAN_LOG=wegplan2.txt "RE15_EXIT_AT=400#3020"
                     "${_exe_kopie}")
    file(READ "${WORKDIR}/debug.log" _dbg)
    pruefe("Lauf 2: Spielstand laedt Marvin (work_vars[0x10] = 0x0D)" "Spielstand: Marvin spielt (Leons Seite gesichert)")
    pruefe("Lauf 2: Neuladen ohne Figurwechsel-Schwanz" "Neuladen als Marvin im laufenden Tausch")
    pruefe("Lauf 2: Marvin traegt nach seinem Weg alle Funde" "Marvins Seite (Uebergabe offen): 03 x15 15 x50 46 x1 07 x6 36 x1")
    pruefe("Lauf 2: Leon hat nach dem Rueckwechsel alles zurueck" "Leons Seite zurueck: 03 x15 15 x200")
    pruefe("Lauf 2: Leons Waffe zurueck" "Leon: Ausruest-Platz 0x00 Waffe 3")
    pruefe("Lauf 2: L-08 nach dem Laden" "ROOM3020: Leon hoert die Schuesse -> msg 9")
    pruefe("Lauf 2: 3020 erreicht" "EXIT_AT: Bild 400 in Raum 3020")
    _fertig()
else()
    fail("unbekannter TEIL ${TEIL}")
    _fertig()
endif()
