# =============================================================================
# Runde 42 "story" (v0.8.31), Stufe STAGE3a (Marvin) - DURCHLAEUFE in der echten exe (RE15_WEGPLAN: Gehen, Blick, Aktionstaste,
# Zielen/Feuern wie der Spieler; Raumwechsel ueber die Tueren des Spiels). Konstanten/Belege include/re15_story3a_r42.h, Code
# engine/src/story3a_r42.c. PORT-STORY nach dem Konzept (5 T-01/T-02, 3.2 L5-L7, L-05..L-10; F-04, F-05, F-06).
#
# Zustand = Ende der Stufe STAGE1 (integration_r42_story1_*: 9:90 9:111 9:112 9:113 9:118 9:119 3:245 3:206 3:151 9:121 + 9:120 Szene an
# der Leiter gesehen). Start in 3000 (RE15_GOTO_ROOM, die Leichenszene sub03 laeuft wie beim Einstieg ueber die Leiter), Leon mit der
# Pistole (RE15_EQUIP=3 - das Ausruesten im Menue kann der Wegplan nicht) und 200 H. Gun Bullets (RE15_GIVE=21:200): nach Marvins
# 3060-Szene setzt sub03 (4,0x0A) @0x01052, ab dann laufen in 3010 sieben Zombies (main00 @0x02122..@0x021EA, Kill-Ids pc[7]
# 0x40..0x46, nur in 3010/3011). Ihr Kampf ist nicht Teil der Stufe und toetete Leon in den Laeufen k5/k6 (zwei Zombies auf der
# Rampe Band 1) - ihre Kill-Bits 7:64..7:70 stehen deshalb von Anfang an (Em-Status-Tor FUN_800420a0 @0x80042120-38 verwirft den
# Satz). Die Hunde in 3060 kaempft Leon echt (JAGE, F-04); RAEUME bleibt als Schutz am Weg.
#
# TEIL kette     3000 -> 3010 -> 3020 (Fensterszene mit Zeilen 7/8, Ausloesezone per LAUF) -> Marvin in 30C0 (Tausch: Browning x15 +
#                50 H. Gun Bullets, Platz 0, Waffe 3) -> [Sprung 30C0 -> 3040: der Weg 30C0 -> 3030 -> 30E0 -> 3050 -> 3040 fuehrt ueber
#                zwei Treppen/Ebenen und ist nicht Teil der Stufe; der Tausch bleibt im Speicher] -> Keycard (Taken-Bit 85), Schloss
#                (4:196), orange Tuer -> 3060 (Hunde, Zeile 11, 3 Schuesse, Abblende, Rueckwechsel) -> Leon 3020 ("Gunshots...?", Leons
#                Seite zurueck) -> 3010 -> 3060 (Leon kaempft gegen die Hunde, F-04) -> Wiedersehen (12..27, Keycard + Fabrikplan) ->
#                3010 -> Laderampe -> Kartentuer (erst jetzt offen, L-10: msg 1 "You've used the Green Keycard.") -> 30E0.
# TEIL speichern Lauf 1 wie kette bis zur Keycard in 3040, Speichern als Marvin (RE15_SAVE_TEST_AGAIN, Karte im Arbeitsordner);
#                Lauf 2 laedt (RE15_CONTINUE_TEST): Marvin mit Keycard, orange Tuer -> 3060 -> Rueckwechsel: Leon hat alles zurueck.
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
           "RE15_SET_FLAG=9:90,9:111,9:112,9:113,9:118,9:119,3:245,3:206,3:151,9:121,9:120,7:64,7:65,7:66,7:67,7:68,7:69,7:70" RE15_GOTO_ROOM=3000
           "RE15_DEBUG_JUMP=3040@120#30C0" "RE15_PLAYER_POS=-8900,-26300,1024,0" RE15_WEGPLAN_LOG=wegplan.txt)

# Weg Leon 3000 -> 3020 -> (Szene) -> Marvin 3040 -> Keycard -> Schloss -> 3060. Wegpunkte aus der SCA (Planer scratchpad
# story/s3a/weg.py, Band 0, Spielerradius 450) und im Lauf nachgemessen.
set(_weg_bis_3040 "RAUM 3000
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
WARTE 200
RAUM 3040
WARTE 30
GEHE -12900 -24700 250
TIPPE 9 85 400
WARTE 30
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
TIPPE 9 124 5000
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
    file(WRITE "${WORKDIR}/plan.txt" "${_weg_bis_3040}${_weg_tuer_3040}${_weg_leon}")
    re15_start_spiel(_rv 2700 ${_basis} "RE15_WEGPLAN=${WORKDIR}/plan.txt" RE15_STATE_LOG=state.log "RE15_EXIT_AT=100#30E0" "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        fail("kein debug.log (exit=${_rv})")
        _fertig()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    pruefe("3020: Fensterszene = sub02 mit Zeilen 7/8 vor dem Wechsel @0x0D7A" "ROOM3020: Fensterszene = sub02 mit Zeilen 7/8")
    foreach(_id 0 1 2 7 8)
        pruefe("3020: Zeile ${_id} (0..2 Glaettung, 7/8 neu)" "room=3020 id=${_id} ")
    endforeach()
    pruefe("T-01: Leons Seite gesichert" "Tausch Leon -> Marvin, Leons Seite gesichert: 03 x15 15 x200")
    pruefe("T-01: Marvin Platz 0, Waffe 3 (Browning x15, H. Gun Bullets x50)" "Marvin: Platz 0 Waffe 3 (Browning x15, H. Gun Bullets x50)")
    pruefe("T-01: Raumlader meldet Waffe 3 fuer Marvin (kein Messer)" "[sherry-r41] Spielermodell 0 -> 13, Waffe (aca5d) 3,")
    pruefe("3060 Marvin: Zeile 11, 3 Schuesse, Abblende" "ROOM3060: Schuss 3/3")
    pruefe("3060 Marvin: Schwarz bis zum Rueckwechsel" "Schwarz gehalten bis zum Raumwechsel - Rueckwechsel auf Leon")
    pruefe("T-02: Marvins Seite mit der Keycard gemerkt" "Marvins Seite (Uebergabe offen): 03 x15 15 x50 36 x1")
    pruefe("T-01: Leon hat alles zurueck (auch die Waffe)" "Leon: Ausruest-Platz 0x00 Waffe 3")
    pruefe("T-01: Raumlader meldet Waffe 3 fuer Leon" "[sherry-r41] Spielermodell 13 -> 0, Waffe (aca5d) 3,")
    pruefe("L-08: 3020 Leon hoert die Schuesse (msg 9)" "ROOM3020: Leon hoert die Schuesse -> msg 9")
    pruefe("L-08: msg 9 gezeigt" "room=3020 id=9 ")
    pruefe("F-04: Leon toetet die Hunde, dann das Wiedersehen" "ROOM3060: Hunde tot (Zone 7 Bits 0x50..0x52) -> Wiedersehen")
    foreach(_id 12 13 14 15 16 17 18 21 22 23 24 25 26 27)
        pruefe("L5-L7: 3060 Zeile ${_id}" "room=3060 id=${_id} ")
    endforeach()
    pruefe("L6/F-05: Keycard uebergeben" "Marvin uebergibt Item 0x36 x1 -> Leons Inventar")
    pruefe("L6/F-05: Fabrikplan uebergeben (Bit 35)" "Fabrikplan uebergeben -> Bit 35 = 1 (Kartenblatt 7)")
    pruefe("L5 Punkt 6: Marvin geht durch die orange Tuer" "geht durch die orange Tuer")
    pruefe("Bit 124" "Wiedersehen fertig nach")
    pruefe("L-10: Kartentuer 3010 benutzt (msg 1 You've used the Green Keycard.)" "room=3010 id=1 ")
    pruefe("L-10: 30E0 erreicht" "PC loaded room30e0.rdt")
    pruefe("Lauf bis 30E0" "EXIT_AT: Bild 100 in Raum 30e0")
    _fertig()
elseif(TEIL STREQUAL "speichern")
    file(WRITE "${WORKDIR}/plan1.txt" "${_weg_bis_3040}WARTE 400\nENDE\n")
    re15_start_spiel(_rv 1800 ${_basis} "RE15_WEGPLAN=${WORKDIR}/plan1.txt" RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
                     "RE15_SAVE_TEST_AGAIN=520#3040" "RE15_EXIT_AT=800#3040" "${_exe_kopie}")
    file(READ "${WORKDIR}/debug.log" _dbg)
    file(RENAME "${WORKDIR}/debug.log" "${WORKDIR}/debug_1.log")
    pruefe("Lauf 1: Tausch Leon -> Marvin" "Tausch Leon -> Marvin, Leons Seite gesichert: 03 x15 15 x200")
    pruefe("Lauf 1: gespeichert als Marvin in 3040" "[save] saved (room 3040)")
    file(WRITE "${WORKDIR}/plan2.txt" "RAUM 3040\nWARTE 30\n${_weg_tuer_3040}ENDE\n")
    re15_start_spiel(_rv2 1200 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
                     RE15_CARD_SLOT=0 "RE15_WEGPLAN=${WORKDIR}/plan2.txt" RE15_WEGPLAN_LOG=wegplan2.txt "RE15_EXIT_AT=400#3020"
                     "${_exe_kopie}")
    file(READ "${WORKDIR}/debug.log" _dbg)
    pruefe("Lauf 2: Spielstand laedt Marvin (work_vars[0x10] = 0x0D)" "Spielstand: Marvin spielt (Leons Seite gesichert)")
    pruefe("Lauf 2: Neuladen ohne Figurwechsel-Schwanz" "Neuladen als Marvin im laufenden Tausch")
    pruefe("Lauf 2: Marvin traegt die Keycard" "Marvins Seite (Uebergabe offen): 03 x15 15 x50 36 x1")
    pruefe("Lauf 2: Leon hat nach dem Rueckwechsel alles zurueck" "Leons Seite zurueck: 03 x15 15 x200")
    pruefe("Lauf 2: Leons Waffe zurueck" "Leon: Ausruest-Platz 0x00 Waffe 3")
    pruefe("Lauf 2: Keycard in Marvins Seite nach dem Laden (Taken-Bit 85 vor dem Speichern)" "36 x1")
    pruefe("Lauf 2: 3020 erreicht" "EXIT_AT: Bild 400 in Raum 3020")
    _fertig()
else()
    fail("unbekannter TEIL ${TEIL}")
    _fertig()
endif()
