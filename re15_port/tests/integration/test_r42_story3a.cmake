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
# Drei Stellen, die der Wegplan-Bot NICHT spielen kann, sind vorgesetzt - jede mit GENAU dem Zustand, den das Spiel dort liefert:
#   - 3050 Rechner K8 (NB2 N2): Marvin geht an den Rechner (Slot 10, Rechteck -25900,-16900,1000,2000 @0x01FB4, Band 0) und BENUTZT
#     die Red Master Keycard (Wegplan BENUTZE 0x46 = Inventar-USE ohne Menue: dieselbe Stellen-Pruefung wie die Aktionstaste,
#     re15_keyitems_r41_use, dann sub10 @0x02578 echt: Cut 15, Bank 5 Bits 0..5). Vorgesetzt ist NUR das Kran-Steuerspiel (sub01
#     @0x0219E.. Sce_key_ck 01/04/02/08 -> sub11..14 Speed_set obj 3, Treffer Member_cmp(15 == 0x0b) @0x0220E + Sce_key_ck 0x40 -> Evt_exec
#     sub15 @0x0221C): Wegplan EREIGNIS 15 5 4 startet sub15, sobald sub10 das Spiel eroeffnet hat ((5,4) @0x025A8). sub15 laeuft echt:
#     msg 5 "You've used the Red Master Keycard." @0x02608 + Wegwerf-Frage (gen/discard_sites.inc 3050/5/0x46), Set(3,0xB3) @0x0260E,
#     Set(3,0xB0) @0x02612, msg 6, Slot 10 -> msg 7. Standardfall (Kette) = Ja (vorbelegt, weggeworfen): die Karte ist verbraucht
#     (Konzept 6b). Behalten-Fall = speichern Lauf 2 (TIPPE .. 2 = Nein): Marvin traegt die benutzte Karte bis zur Uebergabe.
#   - 3040 zwei kleine Affen 0x29 (main00 @0x01310/@0x01324, Kill-Ids 0x71/0x72): Bot-Laeufe d10a/d11a/d12a - 1 Treffer aus 66 Schuss,
#     Marvin stirbt. Vorgesetzt: Kill-Bits 7:113/7:114 (Em-Status-Tor FUN_800420a0 @0x80042120-38).
#   - 3010 sieben Zombies nach (4,0x0A) @0x01052 (main00 @0x02122..@0x021EA, Kill-Ids 0x40..0x46): Bot-Laeufe d13/d15 (Pistole) - Leon
#     stirbt an der Ankunft (HP 100 -> 60 -> 0), d14 (SPAS-12) - drei Zombies tot, dann laedt die SPAS nicht nach (mg 0). Vorgesetzt wie
#     bisher: Kill-Bits 7:64..7:70. Die drei Hunde in 3060 kaempft Leon echt (JAGE, F-04).
# NB2 N5: Leon kommt VERLETZT an (RE15_START_ZUSTAND, vorgesetzt wie die Treffer aus den Abschnitten davor): Kette HP 70; speichern HP 70
# und vergiftet (Bit 0x2 + Takt-Zaehler 30 wie RE2 Op 32 @0x80020138/40). Nach dem Rueckwechsel hat Leon genau den Wert vom Tausch
# (RE2 FUN_80049e48 @0x8004a058-@0x8004a07c), nicht HP 100 aus dem Spieler-Lader (@0x80031710).
# NB2 N6: "Untertitel ohne Taste" wird an der EINGABE DER SPIELSCHLEIFE gemessen (state.log pad=, g_engine.pad_current), nicht am
# Wegplan-Log: in jedem Bild mit offenem Untertitel des Wiedersehens (Id 12..27 ausser den Systemtexten 16/18/20) ist kein Quadrat
# (0x8000) gedrueckt, und jeder dieser Untertitel schliesst. Dass die Szene dabei bis Bit 124 durchlaeuft, ist der eigentliche Beleg.
#
# TEIL kette     3000 -> 3010 -> 3020 (Fensterszene, Zeilen 0..2 Neutext + 7/8, Ausloesezone per LAUF) -> Marvin (Tausch: Browning x15
#                + 50 H. Gun Bullets, Platz 0, Waffe 3) -> Marvins Weg (s.o.) -> 3060 (Hunde, Zeile 11, 3 Schuesse, Abblende,
#                Rueckwechsel) -> Leon 3020 ("Gunshots...?", Leons Seite zurueck) -> 3010 -> 3060 (Leon kaempft gegen die Hunde) ->
#                Wiedersehen (12..27: 15 -> Green Keycard 16 -> 21 direkt danach (NB2 N3), Plan 17/18, 19/20 fuer den Super Redhawk;
#                die Red Master Keycard ist an K8 verbraucht; Untertitel OHNE Taste, Systemtexte mit Taste: TIPPE ... 1) -> 3010 ->
#                Laderampe -> Kartentuer (erst jetzt offen, L-10) -> 30E0.
# TEIL speichern Lauf 1 bis Marvin in 30C0 (direkt nach dem Tausch; Leon verletzt + vergiftet), Speichern als Marvin
#                (RE15_SAVE_TEST_AGAIN, Karte im Arbeitsordner); Lauf 2 laedt (RE15_CONTINUE_TEST) und spielt Marvins ganzen Weg bis 3060,
#                an K8 BEHAELT Marvin die benutzte Red Master Keycard (Wegwerf-Frage Nein) -> Rueckwechsel: Leon hat alles zurueck,
#                dieselben HP, Gift und Takt-Zaehler wie beim Tausch (aus dem Spielstand).
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
           "RE15_SET_FLAG=9:90,9:111,9:112,9:113,9:118,9:119,3:245,3:206,3:151,9:121,9:120,7:64,7:65,7:66,7:67,7:68,7:69,7:70,7:113,7:114"
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
# Strang fabrikplan (20.11.2026, K-16): auf dem ersten Gang zur Osttuer nimmt Marvin den Wandplan (Plakatmitte x -7600 an der Nordwand
# z 1100, SCA-Zelle 1 @0x007F8; Zone Slot 20 x -8100..-7100 z 300..1300): Blick 3072 (+z), Aktion, Frage per Quadrat (Ja vorbelegt)
# bis Marvins Bit (9,139) steht, dann die Meldung "You've taken the factory map." schliessen.
set(_weg_marvin "GEHE -8750 -6800 250
GEHE -7550 -1250 250
GEHE -7600 450 150
BLICK 3072
WARTE 10
AKTION
TIPPE 9 139 300 1
TIPPE 9 255 90 1
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
GEHE -24350 -15900 100
BLICK 2048
WARTE 10
BENUTZE 70
EREIGNIS 15 5 4 300
TIPPE 3 176 900 K8WAHL
TIPPE 9 255 120 1
FLAG 3 179
WARTE 30
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

macro(pruefe_wp _text _muster)
    string(FIND "${_wp}" "${_muster}" _p)
    if(_p LESS 0)
        fail("${_text} (fehlt im Wegplan-Log: '${_muster}')")
    else()
        ok("${_text}")
    endif()
endmacro()
# NB2 N5: HP/Gift/Takt beim Tausch (Zeile "Leon: ... HP h Gift g Takt t; Marvin: ...") == nach dem Rueckwechsel (Zeile "Leon: ... HP h
# Gift g Takt t" ohne "; Marvin"), und HP < 100 (verletzt, nicht geheilt).
macro(pruefe_hp_gleich)
    set(_hin "")
    set(_zur "")
    if(_dbg MATCHES "Leon: Ausruest-Platz 0x[0-9a-f]+ Waffe [0-9]+ Vor-Platz 0x[0-9a-f]+ HP ([0-9]+) Gift ([01]) Takt ([0-9]+); Marvin")
        set(_hin "${CMAKE_MATCH_1}/${CMAKE_MATCH_2}/${CMAKE_MATCH_3}")
    endif()
    # die Rueckwechsel-Zeile endet nach "Takt t" (die Tausch-Zeile geht mit "; Marvin" weiter)
    if(_dbg MATCHES "Leon: Ausruest-Platz 0x[0-9a-f]+ Waffe [0-9]+ Vor-Platz 0x[0-9a-f]+ HP ([0-9]+) Gift ([01]) Takt ([0-9]+)\n")
        set(_zur "${CMAKE_MATCH_1}/${CMAKE_MATCH_2}/${CMAKE_MATCH_3}")
    endif()
    string(REGEX MATCH "^[0-9]+" _hp_hin "${_hin}")
    if(NOT _hin STREQUAL "" AND _hin STREQUAL _zur AND _hp_hin LESS 100)
        ok("N5: Leon HP/Gift/Takt beim Tausch ${_hin} = nach dem Rueckwechsel ${_zur} (nicht geheilt, RE2 @0x8004a058-@0x8004a07c)")
    else()
        fail("N5: Leon HP/Gift/Takt beim Tausch '${_hin}' / nach dem Rueckwechsel '${_zur}' (Soll gleich, HP < 100)")
    endif()
endmacro()
# Folge der [msg]-Ids in ROOM3060 ab der ersten Zeile 12 (Wiedersehen) bis 27
function(folge_3060 _aus)
    file(STRINGS "${WORKDIR}/debug.log" _z REGEX "^\\[msg\\] room=3060 id=[0-9]+ ")
    set(_l "")
    set(_an 0)
    foreach(_e IN LISTS _z)
        if(_e MATCHES "id=([0-9]+) ")
            set(_id "${CMAKE_MATCH_1}")
            if(_id EQUAL 12)
                set(_an 1)
            endif()
            if(_an)
                list(APPEND _l "${_id}")
            endif()
            if(_an AND _id EQUAL 27)
                break()
            endif()
        endif()
    endforeach()
    string(REPLACE ";" "," _l "${_l}")
    set(${_aus} "${_l}" PARENT_SCOPE)
endfunction()
# NB2 N6: state.log-Block des Wiedersehens (der Raumblock, in dem id=27 offen ist; Bildzaehler faellt beim Raumwechsel): in jedem Bild mit
# offenem Untertitel (msg a=1, Id 12..27 ohne 16/18/20) kein Quadrat im pad der Spielschleife; jeder Untertitel schliesst (a=0 oder
# andere Id danach).
macro(untertitel_ohne_taste)
    file(STRINGS "${WORKDIR}/state.log" _st REGEX "^F[0-9]+ pad=")
    set(_blk "")
    set(_prev -1)
    set(_treffer "")
    foreach(_zz IN LISTS _st)
        if(_zz MATCHES "^F([0-9]+) pad=([0-9a-f]+) .* msg\\(a=([01]) fsm=[0-9]+ id=([0-9]+) ")
            if(CMAKE_MATCH_1 LESS _prev)
                if(_blk MATCHES ":27")
                    set(_treffer "${_blk}")
                endif()
                set(_blk "")
            endif()
            set(_prev "${CMAKE_MATCH_1}")
            list(APPEND _blk "${CMAKE_MATCH_2}:${CMAKE_MATCH_3}:${CMAKE_MATCH_4}")
        endif()
    endforeach()
    if(_treffer STREQUAL "" AND _blk MATCHES ":27")
        set(_treffer "${_blk}")
    endif()
    set(_n_bilder 0)
    set(_n_taste 0)
    set(_offen "")
    set(_geschlossen "")
    set(_vorher "")
    foreach(_e IN LISTS _treffer)
        string(REPLACE ":" ";" _f "${_e}")
        list(GET _f 0 _pad)
        list(GET _f 1 _a)
        list(GET _f 2 _id)
        set(_ut 0)
        if(_a EQUAL 1 AND _id GREATER_EQUAL 12 AND _id LESS_EQUAL 27 AND NOT _id EQUAL 16 AND NOT _id EQUAL 18 AND NOT _id EQUAL 20)
            set(_ut 1)
        endif()
        if(_ut)
            math(EXPR _n_bilder "${_n_bilder} + 1")
            math(EXPR _q "0x${_pad} & 0x8000")
            if(NOT _q EQUAL 0)
                math(EXPR _n_taste "${_n_taste} + 1")
            endif()
            list(APPEND _offen "${_id}")
        endif()
        if(NOT _vorher STREQUAL "" AND NOT _vorher STREQUAL "${_a}:${_id}")
            string(REPLACE ":" ";" _v "${_vorher}")
            list(GET _v 1 _vid)
            list(APPEND _geschlossen "${_vid}")
        endif()
        set(_vorher "${_a}:${_id}")
    endforeach()
    list(REMOVE_DUPLICATES _offen)
    list(REMOVE_DUPLICATES _geschlossen)
    set(_nicht_zu "")
    foreach(_id IN LISTS _offen)
        list(FIND _geschlossen "${_id}" _i)
        if(_i LESS 0)
            list(APPEND _nicht_zu "${_id}")
        endif()
    endforeach()
    list(LENGTH _offen _n_ut)
    if(_n_bilder GREATER 0 AND _n_taste EQUAL 0 AND _n_ut GREATER_EQUAL 13 AND _nicht_zu STREQUAL "")
        ok("N6: ${_n_ut} Untertitel des Wiedersehens, ${_n_bilder} Bilder offen, 0 mit Quadrat (state.log pad=), alle geschlossen")
    else()
        fail("N6: Untertitel ${_n_ut} (Soll 13), Bilder ${_n_bilder}, davon mit Quadrat ${_n_taste}, nicht geschlossen '${_nicht_zu}'")
    endif()
endmacro()

function(_fertig)
    file(REMOVE "${_exe_kopie}")
    list(LENGTH _fehler _nf)
    if(_nf GREATER 0)
        message(FATAL_ERROR "r42_story3a[${TEIL}]: ${_nf} Fehler (${WORKDIR})")
    endif()
    message(STATUS "r42_story3a[${TEIL}]: alle Pruefungen ok (${WORKDIR})")
endfunction()

if(TEIL STREQUAL "kette")
    string(REPLACE "K8WAHL" "1" _weg_marvin_k "${_weg_marvin}")   # K8: Wegwerf-Frage Ja (vorbelegt) = Standardfall
    file(WRITE "${WORKDIR}/plan.txt" "${_weg_vor_30c0}${_weg_marvin_k}${_weg_tuer_3040}${_weg_leon}")
    re15_start_spiel(_rv 2950 ${_basis} RE15_START_ZUSTAND=70 "RE15_WEGPLAN=${WORKDIR}/plan.txt" RE15_STATE_LOG=state.log
                     "RE15_EXIT_AT=400#30E0" "${_exe_kopie}")
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
    # NB2 N2: K8 in 3050 - Karte benutzt (echter USE-Weg), Kran-Ergebnis = sub15, Wegwerf-Frage Ja
    pruefe_wp("N2: K8 - Red Master Keycard benutzt (Stelle vor dem Spieler, sub10)" "BENUTZE: Item 0x46 (Platz 2) -> Stelle gefunden, Inventar schliesst, Ereignis gestartet")
    pruefe_wp("N2: K8 - Kran-Steuerspiel eroeffnet, sub15 gestartet" "EREIGNIS: (5,4) = 1 nach")
    pruefe("N2: K8 - msg 5 You've used the Red Master Keycard." "[msgtext] room=3050 id=5 \"You've used the Red Master Keycard.\"")
    pruefe_wp("N2: K8 - Wegwerf-Frage Ja (Standardfall, weggeworfen)" "Wegwerf-Frage Item 0x46 beantwortet -> WEGGEWORFEN (Gegenstand nicht mehr im Inventar)")
    pruefe_wp("N2: K8 - (3,176) gesetzt (sub15 @0x02612)" "TIPPE: (3,176) = 1")
    pruefe_wp("N2: K8 - (3,179) gesetzt (sub15 @0x0260E)" "FLAG (3,179) = 1")
    pruefe("N2: K8 - msg 6 The system has been successfully restored." "room=3050 id=6 ")
    if(_wp MATCHES "room=30E0 spieler=\\(7025,2700\\)")
        ok("B2: Marvin betritt 30E0 im Ost-Ring (7025,2700) - die Rueckseite der Kartentuer liegt im West-Ring")
    else()
        fail("B2: Marvins Ankunft in 30E0 (7025,2700) fehlt")
    endif()
    pruefe("T-01: Leons Seite gesichert" "Tausch Leon -> Marvin, Leons Seite gesichert: 03 x15 15 x200")
    pruefe("T-01: Marvin Platz 0, Waffe 3 (Browning x15, H. Gun Bullets x50)" "Marvin: Platz 0 Waffe 3 (Browning x15, H. Gun Bullets x50)")
    # NB2 N5: Leon verletzt vor dem Tausch, nach dem Rueckwechsel derselbe Wert (nicht geheilt)
    pruefe_re("N5: Leon kommt verletzt an (vorgesetzt)" "RE15_START_ZUSTAND F[0-9]+ Raum [0-9A-F]+: HP := 70, Gift 0")
    pruefe_hp_gleich()
    pruefe("T-01: Raumlader meldet Waffe 3 fuer Marvin (kein Messer)" "[sherry-r41] Spielermodell 0 -> 13, Waffe (aca5d) 3,")
    pruefe("3060 Marvin: Zeile 11, 3 Schuesse, Abblende" "ROOM3060: Schuss 3/3")
    pruefe("3060 Marvin: Schwarz bis zum Rueckwechsel" "Schwarz gehalten bis zum Raumwechsel - Rueckwechsel auf Leon")
    pruefe("T-02: Marvins Seite mit allen Funden gemerkt (Super Redhawk, Green Keycard; Red Master Keycard an K8 verbraucht)"
           "Marvins Seite (Uebergabe offen): 03 x15 15 x50 07 x6 36 x1")
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
    string(FIND "${_dbg}" "Marvin uebergibt Item 0x46" _p)
    if(_p LESS 0)
        ok("N2: keine Red Master Keycard in der Uebergabe (an K8 verbraucht und weggeworfen)")
    else()
        fail("N2: Red Master Keycard in der Uebergabe, obwohl an K8 weggeworfen")
    endif()
    pruefe("msg 20 mit Namen" "You've received the Super Redhawk.")
    # NB2 N3: Reihenfolge der Zeilen im Wiedersehen (msg-Log), 21 "A keycard..." direkt nach 16 "You've received the Green Keycard."
    folge_3060(_folge)
    if(_folge MATCHES "12,13,14,15,16,21,17,18,19,20,22,23,24,25,26,27")
        ok("N3: Zeilenfolge 3060 ${_folge} - 21 direkt nach 16")
    else()
        fail("N3: Zeilenfolge 3060 '${_folge}' (Soll ...12,13,14,15,16,21,17,18,19,20,22,23,24,25,26,27)")
    endif()
    # NB2 N6: Untertitel ohne Taste - gemessen an der Eingabe der Spielschleife (state.log pad=), nicht am Wegplan-Log
    untertitel_ohne_taste()
    string(FIND "${_wp}" "TIPPE: Systemtext msg 16" _p)
    if(_p LESS 0)
        fail("B8: Systemtext 16 nicht per Taste bestaetigt")
    else()
        ok("B8: Systemtexte per Taste bestaetigt")
    endif()
    pruefe("K-16: Marvin nimmt den Fabrikplan in 30C0 (nur sein Bit 139)" "ROOM30C0 Ereignis 28 -> Port-Programm Karte Blatt 7 (Marvin nimmt den Plan")
    pruefe("K-16: Meldung You've taken the factory map. in 30C0" "room=30c0 id=31 ")
    pruefe("K-16: Wiedersehen mit Plan (Zeilen 17/18)" "Fabrikplan von Marvin (Zeilen 17/18)")
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
    re15_start_spiel(_rv 900 ${_basis} RE15_START_ZUSTAND=70:1 "RE15_WEGPLAN=${WORKDIR}/plan1.txt" RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
                     "RE15_SAVE_TEST_AGAIN=150#30C0" "RE15_EXIT_AT=320#30C0" "${_exe_kopie}")
    file(READ "${WORKDIR}/debug.log" _dbg)
    file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug_1.log")
    pruefe("Lauf 1: Tausch Leon -> Marvin" "Tausch Leon -> Marvin, Leons Seite gesichert: 03 x15 15 x200")
    pruefe("Lauf 1: gespeichert als Marvin in 30C0" "[save] saved (room 30c0)")
    pruefe_re("N5: Leon kommt verletzt und vergiftet an (vorgesetzt)" "RE15_START_ZUSTAND F[0-9]+ Raum [0-9A-F]+: HP := 70, Gift 1")
    set(_hin "")
    if(_dbg MATCHES "Leon: Ausruest-Platz 0x[0-9a-f]+ Waffe [0-9]+ Vor-Platz 0x[0-9a-f]+ HP ([0-9]+) Gift ([01]) Takt ([0-9]+); Marvin")
        set(_hin "${CMAKE_MATCH_1}/${CMAKE_MATCH_2}/${CMAKE_MATCH_3}")
        set(_gift_hin "${CMAKE_MATCH_2}")
        set(_hp_hin "${CMAKE_MATCH_1}")
    endif()
    if(NOT _hin STREQUAL "" AND _gift_hin EQUAL 1 AND _hp_hin LESS 70)
        ok("N5: Lauf 1 Tausch: Leon HP/Gift/Takt ${_hin} (vergiftet, das Gift hat seit dem Start gezehrt)")
    else()
        fail("N5: Lauf 1 Tausch: Leon HP/Gift/Takt '${_hin}' (Soll Gift 1, HP < 70)")
    endif()
    # Lauf 2: laden und Marvins ganzen Weg bis 3060 spielen (Rueckwechsel nach dem Laden).
    string(REPLACE "K8WAHL" "2" _weg_marvin_b "${_weg_marvin}")   # K8: Wegwerf-Frage NEIN = Behalten-Fall (NB2 N2)
    file(WRITE "${WORKDIR}/plan2.txt" "RAUM 30C0\nWARTE 30\n${_weg_marvin_b}${_weg_tuer_3040}ENDE\n")
    re15_start_spiel(_rv2 2200 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
                     RE15_CARD_SLOT=0 "RE15_WEGPLAN=${WORKDIR}/plan2.txt" RE15_WEGPLAN_LOG=wegplan2.txt "RE15_EXIT_AT=400#3020"
                     "${_exe_kopie}")
    file(READ "${WORKDIR}/debug.log" _dbg)
    pruefe("Lauf 2: Spielstand laedt Marvin (work_vars[0x10] = 0x0D)" "Spielstand: Marvin spielt (Leons Seite gesichert)")
    pruefe("Lauf 2: Neuladen ohne Figurwechsel-Schwanz" "Neuladen als Marvin im laufenden Tausch")
    set(_wp "")
    if(EXISTS "${WORKDIR}/wegplan2.txt")
        file(READ "${WORKDIR}/wegplan2.txt" _wp)
    endif()
    pruefe_wp("N2: Lauf 2 K8 - Red Master Keycard benutzt" "BENUTZE: Item 0x46 (Platz 2) -> Stelle gefunden, Inventar schliesst, Ereignis gestartet")
    pruefe_wp("N2: Lauf 2 K8 - Wegwerf-Frage NEIN (Behalten-Fall)" "Wegwerf-Frage Item 0x46 beantwortet -> BEHALTEN (Gegenstand noch im Inventar)")
    pruefe_wp("N2: Lauf 2 K8 - (3,176) gesetzt (sub15 @0x02612)" "TIPPE: (3,176) = 1")
    pruefe("Lauf 2: Marvin traegt nach seinem Weg alle Funde, auch die behaltene Red Master Keycard"
           "Marvins Seite (Uebergabe offen): 03 x15 15 x50 46 x1 07 x6 36 x1")
    set(_zur "")
    if(_dbg MATCHES "Leon: Ausruest-Platz 0x[0-9a-f]+ Waffe [0-9]+ Vor-Platz 0x[0-9a-f]+ HP ([0-9]+) Gift ([01]) Takt ([0-9]+)\n")
        set(_zur "${CMAKE_MATCH_1}/${CMAKE_MATCH_2}/${CMAKE_MATCH_3}")
    endif()
    if(NOT _hin STREQUAL "" AND _hin STREQUAL _zur)
        ok("N5: Lauf 2 Rueckwechsel nach dem Laden: Leon HP/Gift/Takt ${_zur} = beim Tausch in Lauf 1 ${_hin} (nicht geheilt, Gift bleibt)")
    else()
        fail("N5: Lauf 2 Rueckwechsel: Leon HP/Gift/Takt '${_zur}' / beim Tausch in Lauf 1 '${_hin}' (Soll gleich)")
    endif()
    pruefe("Lauf 2: Leon hat nach dem Rueckwechsel alles zurueck" "Leons Seite zurueck: 03 x15 15 x200")
    pruefe("Lauf 2: Leons Waffe zurueck" "Leon: Ausruest-Platz 0x00 Waffe 3")
    pruefe("Lauf 2: L-08 nach dem Laden" "ROOM3020: Leon hoert die Schuesse -> msg 9")
    pruefe("Lauf 2: 3020 erreicht" "EXIT_AT: Bild 400 in Raum 3020")
    _fertig()
else()
    fail("unbekannter TEIL ${TEIL}")
    _fertig()
endif()
