# integration_r36_schluessel — Runde 36 Spur Q "schluessel", ECHTE exe am LADE-Weg (Spielstand + CONTINUE).
# Dossier analysis/befunde_runde36/Q_schluessel.md (Abschnitt 4/5), Konstanten include/re15_schluessel_r36.h.
#
#   zu      Leon ROOM11B0 (Strom an), OHNE Parking Lot Key vor der Parkplatz-Tuer, ein Quadrat-Druck:
#           "[schluessel] ... verschlossen", KEIN "DOOR FIRE", Bild 150 noch in ROOM11B0
#   auf     dasselbe MIT Parking Lot Key im Inventar: "benutzt" (Bild 45), Text schliessen (140), die
#           Wegwerf-Abfrage "You don't need this key any more. Discard it?" (RE2 @0x80051670/@0x80051834,
#           getippt bis ~Bild 235) mit Ja beantworten (245), dann die Tuer halten (270..282) -> "DOOR FIRE
#           slot=1" und ROOM11C0 erreicht (RE2-Tuer-Handler: erst aufschliessen, der naechste Druck geht hindurch)
#   leon    Leon ROOM10F0 vor der Funkkonsole, Druck -> Aufnahme-Modal -> Ja: "PARKING LOT KEY genommen"
#   elza    Elza ROOM10F1, gleiche Stelle: "PRISON KEY genommen" und "(4,0xEC) = 1" (Zelle ROOM1211)
#   check   Statusschirm CHECK beider Schluessel (Harness RE15_INV_CHECK_SHOT): "CHECK-Foto Item 0x45/0x42:
#           crect (0,489) prect (832,256) 56x72 -> Foto und CLUT geladen" (eingesetzter RE2-Block)
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_schluessel_karte> -DWORKDIR=<dir>
#               -P test_r36_schluessel.cmake

if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r36_schluessel: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r36_schluessel: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r36_schluessel: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

function(hexof _text _out)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(${_out} "${_h}" PARENT_SCOPE)
endfunction()

# Lauf: _name, Kartenargumente (Liste), zusaetzliche env-Paare (Liste) -> debug.log als HEX in _out
function(lauf _name _karte _envs _out)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${_karte}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r36_schluessel[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1
        RE15_NOAUDIO=1
        SDL_AUDIODRIVER=dummy
        RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1
        RE15_CARD_AUTO=1
        RE15_CARD_SLOT=0
        ${_envs}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r36_schluessel[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _lh HEX)
    set(${_out} "${_lh}" PARENT_SCOPE)
endfunction()

function(muss _name _lh _text)
    hexof("${_text}" _h)
    string(FIND "${_lh}" "${_h}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r36_schluessel[${_name}]: Zeile fehlt: '${_text}' (${_basis}_${_name}/debug.log)")
    endif()
endfunction()
function(darf_nicht _name _lh _text)
    hexof("${_text}" _h)
    string(FIND "${_lh}" "${_h}" _p)
    if(NOT _p LESS 0)
        message(FATAL_ERROR "r36_schluessel[${_name}]: Zeile darf nicht stehen: '${_text}' (${_basis}_${_name}/debug.log)")
    endif()
endfunction()

# --- befreiung (Nachbesserung 1, Mangel M1; eigener ctest-Eintrag mit -DNUR=befreiung) ---
# Elza in ROOM1211 vor dem Ausloeser der Befreiungsszene (Slot 6, sub00 @0x01E22), Prison Key genommen
# ((4,0xEC), (9,83)) und als EINZIGER Gegenstand im Inventar. U F40..F100 -> Szene sub02 laeuft; msg 9
# "Elza: John. Wait!" (@0x0226A) auf F1771, zu F1870 -> im selben Bild die Wegwerf-Abfrage fuer 0x42
# (RE2 @0x800517f4/@0x80051834), Text fertig F1957; Viereck F1980 = Ja -> weg; Szenenende F1983
# (Set(2,7)=0 @0x0229A). Gemessen in Dossier N1.4 (Laeufe N_Z3_mess / N_Z3_ja).
if(NUR STREQUAL "befreiung")
    set(_pad "")
    foreach(_f RANGE 40 100)
        string(APPEND _pad "${_f}:U,")
    endforeach()
    lauf(befreiung "1211;-19355;-17300;1024;elza;flag:3:0x8B;flag:4:0x24;flag:4:0xEC;flag:9:83;leer;give:0x42:1"
         "RE15_PAD_AT=${_pad}1980:A;RE15_EXIT_AT=1995#1211;RE15_MSG_LOG=1;RE15_DISCARD_LOG=discard.log" _lh)
    muss(befreiung "${_lh}" "CONTINUE: resumed in room 1211")
    muss(befreiung "${_lh}" "[msg] room=1211 id=9")
    muss(befreiung "${_lh}" "EXIT_AT: Bild 1995 in Raum 1211")
    set(_dl "${_basis}_befreiung/discard.log")
    if(NOT EXISTS "${_dl}")
        message(FATAL_ERROR "r36_schluessel[befreiung]: kein discard.log (${_dl})")
    endif()
    file(READ "${_dl}" _d)
    string(FIND "${_d}" "frage=8 gegenstand=0x42" _p_frage)
    if(_p_frage LESS 0)
        message(FATAL_ERROR "r36_schluessel[befreiung]: keine Wegwerf-Abfrage fuer 0x42 (${_dl})")
    endif()
    string(REGEX MATCH "msg_aktiv=1[^\n]*frage=8" _frueh "${_d}")
    if(_frueh)
        message(FATAL_ERROR "r36_schluessel[befreiung]: Abfrage, waehrend eine Nachricht steht (${_dl})")
    endif()
    string(REGEX MATCH "F[0-9]+ raum=1211 msg_aktiv=0[^\n]*frage=8 gegenstand=0x42" _bild "${_d}")
    string(REGEX MATCH "^F[0-9]+" _bild "${_bild}")
    string(FIND "${_d}" "weg=1" _p_weg)
    string(REGEX MATCH "F1995 raum=1211[^\n]*weg=1[^\n]*pausepad=0" _ende "${_d}")
    if(_p_weg LESS 0 OR NOT _ende)
        message(FATAL_ERROR "r36_schluessel[befreiung]: Ja hat den Prison Key nicht weggeworfen oder die Szene "
                            "ist nicht zu Ende (weg=1 / pausepad=0 in F1995 fehlt, ${_dl})")
    endif()
    message(STATUS "r36_schluessel[befreiung] OK: Abfrage fuer 0x42 nach msg 9 (${_bild}), Ja -> weggeworfen, "
                   "Szene zu Ende")
    return()
endif()

# Vor der Parkplatz-Tuer (Rechteck x -26730..-24230, z -29940..-26440; begehbar z > -26720), Blick -z
# (yaw 1024): 620 voraus = (-25480,-26620) im Rechteck. Strom (4,0xF3), Sherry-Szene erledigt (3,0x82)
# (sub01 @0x011BA startet sie sonst bei (3,0x82)==0).
set(_tuer 11B0 -25480 -26000 1024 flag:4:0xF3 flag:3:0x82)

# --- zu ---
lauf(zu "${_tuer}" "RE15_PAD_AT=45:A;RE15_EXIT_AT=150#11b0" _lh)
muss(zu "${_lh}" "CONTINUE: resumed in room 11b0")
muss(zu "${_lh}" "[schluessel] ROOM11B0 Slot 1 (Tuer -> ROOM11C0) gesperrt")
muss(zu "${_lh}" "[schluessel] ROOM11B0: Parkplatz-Tuer verschlossen")
darf_nicht(zu "${_lh}" "DOOR FIRE slot=1")
muss(zu "${_lh}" "EXIT_AT: Bild 150 in Raum 11b0")
message(STATUS "r36_schluessel[zu] OK: ohne Schluessel Text msg 28 + Verschlossen-Ton, kein Raumwechsel")

# --- auf ---
set(_halten "")
foreach(_f RANGE 270 282)
    string(APPEND _halten ",${_f}:A")
endforeach()
lauf(auf "${_tuer};give:0x45:1" "RE15_PAD_AT=45:A,140:A,245:A${_halten};RE15_EXIT_AT=5#11c0" _lh)
muss(auf "${_lh}" "[schluessel] ROOM11B0: Parking Lot Key benutzt -> Tuer offen, Flag (9,93) = 1")
muss(auf "${_lh}" "DOOR FIRE slot=1")
muss(auf "${_lh}" "EXIT_AT: Bild 5 in Raum 11c0")
message(STATUS "r36_schluessel[auf] OK: mit Schluessel Text msg 29 + Aufschliessen + Wegwerf-Abfrage, naechster Druck -> ROOM11C0")

# --- leon / elza: Aufnahme an der Funkkonsole (Stand (8350,2050) Blick +z) ---
set(_pad "RE15_PAD_AT=45:A,120:A,160:A,200:A")
lauf(leon "10F0;8350;2050;3072;flag:9:71;flag:3:94" "${_pad};RE15_EXIT_AT=240#10f0" _lh)
muss(leon "${_lh}" "[schluessel] ROOM10F0: PARKING LOT KEY (Item 0x45, Bit (9,82))")
muss(leon "${_lh}" "[schluessel] PARKING LOT KEY genommen: Item 0x45, Bit (9,82)")
darf_nicht(leon "${_lh}" "Flag (4,0xEC) = 1")
message(STATUS "r36_schluessel[leon] OK: Parking Lot Key in ROOM10F0 aufgenommen")

lauf(elza "10F1;8350;2050;3072;elza" "${_pad};RE15_EXIT_AT=240#10f1" _lh)
muss(elza "${_lh}" "[schluessel] ROOM10F1: PRISON KEY (Item 0x42, Bit (9,83))")
muss(elza "${_lh}" "[schluessel] PRISON KEY genommen: Item 0x42, Bit (9,83)")
muss(elza "${_lh}" "Prison Key genommen -> Flag (4,0xEC) = 1")
message(STATUS "r36_schluessel[elza] OK: Prison Key in ROOM10F1 aufgenommen, Zellen-Flag (4,0xEC) gesetzt")

# --- check: Statusschirm CHECK, Schluessel auf Platz 0 ---
lauf(check45 "10F0;5200;-2500;1024;flag:9:71;flag:3:94;flag:9:82;leer;give:0x45:1;give:0x42:1"
     "RE15_INV_SHOT=chk.bmp;RE15_INV_CHECK_SHOT=1" _lh)
muss(check45 "${_lh}" "CHECK-Foto Item 0x45: crect (0,489) prect (832,256) 56x72 -> Foto und CLUT geladen")
lauf(check42 "10F1;5200;-2500;1024;elza;flag:9:83;leer;give:0x42:1;give:0x45:1"
     "RE15_INV_SHOT=chk.bmp;RE15_INV_CHECK_SHOT=1" _lh)
muss(check42 "${_lh}" "CHECK-Foto Item 0x42: crect (0,489) prect (832,256) 56x72 -> Foto und CLUT geladen")
message(STATUS "r36_schluessel[check] OK: CHECK-Foto beider Schluessel geladen (RE2-Block, RE1.5-Rechtecke)")
