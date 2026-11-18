# =============================================================================
# Runde 42 "story" (v0.8.31), Stufe STAGE6 (Zug 6030 Leon / 6031 Elza) - DURCHLAEUFE in der echten exe (RE15_WEGPLAN: Gehen, Blick,
# Aktionstaste wie der Spieler; Raumwechsel ueber die Tueren des Spiels, Treppe im Wagen ueber ihre Aktion). Konstanten/Belege
# include/re15_story6_r42.h, Code engine/src/story6_r42.c. PORT-STORY nach dem Konzept (3.2 L21, 4.2 E6/E9, 5 T-05).
#
# Weg je Szenario: Bahnsteig an der Zugtuer -> Wagen (Szene L21 bzw. E6) -> Treppe -> Fuehrerstand (Annette bzw. Sherry: "not enough
# power", Original) -> zurueck auf den Bahnsteig -> Torschalter -> Wagen (vor dem Zugstart: Tuer nach hinten zu) -> Treppe ->
# Fuehrerstand (Zugstart: Leon mit Annette, Elza E9 mit Sherry/John) -> Wagen -> Tuer nach hinten -> 5090/5091.
# Mit Ton (SDL_AUDIODRIVER=dummy): vorhandene Aufnahmen spielen (auch unter Neutext, AUFNAHMELISTE "zuerst"), fehlende -> Untertitel
# mit der Textdauer ("keine Aufnahme").
# Vorgesetzt (der Wegplan-Bot kann es nicht spielen):
#   - Ankunft am Bahnsteig: RE15_DEBUG_JUMP 6000/6001 + RE15_PLAYER_POS=1693,-9900,1024,2 = Ziel der Wagentuer zum Bahnsteig
#     (ROOM6030/6031 main00 @0x00C72: (1693,-3600,-10054), Band 2). RE15_GOTO_ROOM setzte Band 0 -> die Zugtuer (Band 2) nahm die
#     Aktion nicht (gemessen, Lauf ea1).
#   - Strom: (3,48)/(3,49) = Bahnsteig-Kampf 5080 (ROOM5080 sub01 @0x00792/@0x00796, nach dem Tod der Riesenform (5,0x1c)); Leser
#     ist nur der Torschalter (ROOM6000/6001 main00 @0x00DFE Ck(3,49)). Den Kampf gewinnt der Bot nicht (Stufe STAGE3b Lauf d2).
#   - Leon: Annettes Begruessung auf dem Bahnsteig gesehen ((3,100) ROOM6000 sub06 @0x0106A, (3,239) sub02 @0x00F8E) - sie ist
#     vor dem Betreten des Zuges.
#   - Torschalter: RE15_FIRE_AOT (6000 Slot 5 / 6001 Slot 4, main00 @0x00E66) = derselbe Pfad wie das Hineinlaufen (re15_aot_fire_slot);
#     die Bahnsteigtreppe zum Schalter geht der Bot nicht (gemessen Lauf e1: haengt bei (18332,-8482)); Antwort "Ja" ueber TIPPE.
#     Form wie Integration A2 (analysis/befunde_runde41/INT_belege/a2_zug6030/kal6000.txt).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -DTEIL=<leon|elza> -P test_r42_story6.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r42_story6: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r42_story6: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL elza)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}_${TEIL}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r42s6_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r42s6_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_fehler "")
macro(fail _t)
    list(APPEND _fehler "${_t}")
    message(STATUS "FAIL r42_story6[${TEIL}]: ${_t}")
endmacro()
macro(ok _t)
    message(STATUS "ok   r42_story6[${TEIL}]: ${_t}")
endmacro()
macro(pruefe _text _muster)
    string(FIND "${_dbg}" "${_muster}" _p)
    if(_p LESS 0)
        fail("${_text} (fehlt: '${_muster}')")
    else()
        ok("${_text}")
    endif()
endmacro()
macro(pruefe_nicht _text _muster)
    string(FIND "${_dbg}" "${_muster}" _p)
    if(_p LESS 0)
        ok("${_text}")
    else()
        fail("${_text} (gefunden: '${_muster}')")
    endif()
endmacro()
macro(pruefe_re _text _regex)
    if(_dbg MATCHES "${_regex}")
        ok("${_text}")
    else()
        fail("${_text} (fehlt: /${_regex}/)")
    endif()
endmacro()
function(_fertig)
    file(REMOVE "${_exe_kopie}")
    list(LENGTH _fehler _nf)
    if(_nf GREATER 0)
        message(FATAL_ERROR "r42_story6[${TEIL}]: ${_nf} Fehler (${WORKDIR})")
    endif()
    message(STATUS "r42_story6[${TEIL}]: alle Pruefungen ok (${WORKDIR})")
endfunction()

# Wege im Zug (SCA ROOM6030/6031 @0x708: Gang z -24471..-22290 zwischen Block 2 und 3, Treppe = Aot Slot 4 main00 @0x00CB2 mit
# Aktion, Tuer Slot 1 @0x00C52 in den Fuehrerstand, Slot 0 @0x00C32 zurueck, Slot 2 @0x00C72 zum Bahnsteig, Slot 3 @0x00C92 nach hinten)
set(_zur_treppe "GEHE -21000 -23400 250
GEHE -15000 -23400 250
GEHE -13300 -23160 250
BLICK 0
AKTION
WARTE 200
GEHE -10300 -23300 250
GEHE -9500 -23500 250
BLICK 0
AKTION
")
set(_vom_fuehrerstand "GEHE -3873 -23487 150
BLICK 2048
AKTION
WARTE 200
GEHE -10500 -23300 250
BLICK 2048
AKTION
WARTE 200
GEHE -15000 -23400 250
")
set(_zum_bahnsteig "GEHE -17700 -22900 250
GEHE -17950 -20450 150
BLICK 3072
AKTION
")
set(_nach_hinten "GEHE -26500 -23300 250
BLICK 2048
AKTION
")
set(_basis RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 SDL_AUDIODRIVER=dummy
           RE15_STIMME_LOG=1 RE15_SE_DEBUG=1 RE15_WEGPLAN_LOG=wegplan.txt RE15_STATE_LOG=state.log "RE15_PLAYER_POS=1693,-9900,1024,2")

if(TEIL STREQUAL "elza")
    file(WRITE "${WORKDIR}/plan.txt" "RAUM 6001
WARTE 30
GEHE 1700 -10100 150
BLICK 1024
AKTION
RAUM 6031
WARTE 40
GEHE -21000 -21000 250
GEHE -24300 -21200 250
WARTE 1500
${_zur_treppe}WARTE 1700
${_vom_fuehrerstand}${_zum_bahnsteig}RAUM 6001
WARTE 330
TIPPE 3 89 900
WARTE 400
BLICK 1024
AKTION
RAUM 6031
WARTE 60
GEHE -17700 -23300 250
${_zur_treppe}WARTE 1300
${_vom_fuehrerstand}${_nach_hinten}RAUM 5091
WARTE 200
ENDE
")
    re15_start_spiel(_rv 2400 ${_basis} RE15_PSELECT_AUTO_SWITCH=1 RE15_DEBUG_JUMP=6001@250 "RE15_SET_FLAG=3:48,3:49"
        "RE15_FIRE_AOT=4@300#6001" "RE15_WEGPLAN=${WORKDIR}/plan.txt" "RE15_EXIT_AT=200#5091" "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        fail("kein debug.log (exit=${_rv})")
        _fertig()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    # E6 Wagen-Szene
    # 270 B seit U-03 (2f8960754): Disc ROOM6041 sub06 @0x0070E..@0x00758 (Elzas Blick zu Marvin) vor dem Gespraech,
    # vorher 192 B. Laenge = k_s6_6031_wagen in gen/story6_r42_szenen.inc.
    pruefe("E6: Wagen-Programm" "ROOM6031 Evt_exec sub02 -> Programm (270 B): Wagen-Szene E6")
    pruefe_re("E6: Zeilen 0, 1, 2, 12, 13 in dieser Folge"
        "msgtext] room=6031 id=0 .*msgtext] room=6031 id=1 .*msgtext] room=6031 id=2 .*msgtext] room=6031 id=12 .*msgtext] room=6031 id=13 ")
    pruefe("E6: msg 0 bleibt" "[msgtext] room=6031 id=0 \"Elza: Sherry! Is anyone else here?\"")
    pruefe("E6: msg 1" "[msgtext] room=6031 id=1 \"Sherry: A policeman brought Marvin down here. Then he went back into the fire.\"")
    pruefe("E6: msg 2" "[msgtext] room=6031 id=2 \"Elza: Back? Why?\"")
    pruefe("E6: msg 12" "[msgtext] room=6031 id=12 \"Sherry: He said he had to find someone. He told us not to wait for him.\"")
    pruefe("E6: msg 13" "[msgtext] room=6031 id=13 \"Elza: ...Then we won't. If that core goes, no one gets out. Let's get this train moving.\"")
    pruefe_nicht("E6: kein alter Satz 'fighting off monsters'" "fighting off monsters")
    pruefe_nicht("E6: kein alter Satz 'I'll do it!'" "Elza: I'll do it!")
    # Stimmen: msg 1/2 alte Aufnahme unter Neutext (NEUAUF, AUFNAHMELISTE "zuerst"), 12/13 ohne Aufnahme -> Textdauer
    pruefe("Ton: msg 1 spielt die vorhandene Aufnahme" "[voice] Start ROOM6031 Id 1 ")
    pruefe("Ton: msg 2 spielt die vorhandene Aufnahme" "[voice] Start ROOM6031 Id 2 ")
    foreach(_m 12 13 14 15 16 17)
        pruefe("Ton: msg ${_m} ohne Aufnahme -> Untertitel mit Textdauer" "ROOM6031 msg ${_m}: keine Aufnahme")
    endforeach()
    # Fuehrerstand vor dem Tor: Original sub08
    pruefe("Fuehrerstand vor dem Tor: Original sub08 (msg 10)" "[msgtext] room=6031 id=10 ")
    # Tor
    pruefe("Torschalter 6001 Slot 4" "[fire-aot] slot=4 at F300 (Raum 6001)")
    pruefe("Torschalter-Frage" "[msgtext] room=6001 id=0 ")
    # Wagen vor dem Zugstart
    pruefe("Wagen vor dem Zugstart: Tuer nach hinten zu" "ROOM6031 Evt_exec sub05 -> Programm (44 B): Wagen vor dem Zugstart")
    # E9 Zugstart
    pruefe("E9: Zugstart-Programm" "ROOM6031 Evt_exec sub05 -> Programm (422 B): Zugstart E9")
    pruefe_re("E9: Zug-an-Ton, msg 14, Krachen, msg 15, 16, 17 in dieser Folge"
        "Zug-an-Ton [(]ANNETTE_SE Satz 3[)].*msgtext] room=6031 id=14 .*Krachen aus dem hinteren Wagen.*msgtext] room=6031 id=15 .*msgtext] room=6031 id=16 .*msgtext] room=6031 id=17 ")
    pruefe("E9: msg 14" "[msgtext] room=6031 id=14 \"Sherry: It's moving! Elza, we did it!\"")
    pruefe("E9: msg 15" "[msgtext] room=6031 id=15 \"John: What the hell was that? It came from the back car!\"")
    pruefe("E9: msg 16" "[msgtext] room=6031 id=16 \"Sherry: It's that monster... it followed us!\"")
    pruefe("E9: msg 17" "[msgtext] room=6031 id=17 \"Elza: John, stay with Sherry and Marvin. Don't open that door for anyone but me.\"")
    pruefe_re("Toene: ANNETTE_SE Satz 3 (Zug an) vor Satz 0 (Krachen)" "[[]annette] F[0-9]+ Ton 3 .*[[]annette] F[0-9]+ Ton 0 ")
    # danach wie heute: nach hinten in 5091
    pruefe("5091 erreicht" "EXIT_AT: Bild 200 in Raum 5091")
    _fertig()
elseif(TEIL STREQUAL "leon")
    file(WRITE "${WORKDIR}/plan.txt" "RAUM 6000
WARTE 30
GEHE 1700 -10100 150
BLICK 1024
AKTION
RAUM 6030
WARTE 40
GEHE -19100 -20900 200
WARTE 2700
${_zur_treppe}WARTE 2200
${_vom_fuehrerstand}${_zum_bahnsteig}RAUM 6000
WARTE 330
TIPPE 3 89 900
WARTE 400
BLICK 1024
AKTION
RAUM 6030
WARTE 60
GEHE -17700 -23300 250
${_zur_treppe}WARTE 1900
${_vom_fuehrerstand}${_nach_hinten}RAUM 5090
WARTE 200
ENDE
")
    re15_start_spiel(_rv 2400 ${_basis} RE15_DEBUG_JUMP=6000@250 "RE15_SET_FLAG=3:48,3:49,3:100,3:239"
        "RE15_FIRE_AOT=5@300#6000" "RE15_WEGPLAN=${WORKDIR}/plan.txt" "RE15_EXIT_AT=200#5090" "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        fail("kein debug.log (exit=${_rv})")
        _fertig()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    # L21 Wagen-Szene
    pruefe("L21: Wagen-Programm" "[story80] ROOM6030 Evt_exec sub02 -> Programm")
    pruefe_re("L21: Zeilen 0, 1, 21, 2, 3, 22, 4, 5, 6, 7, 23, 24, 8 in dieser Folge"
        "room=6030 id=0 .*room=6030 id=1 .*room=6030 id=21 .*room=6030 id=2 .*room=6030 id=3 .*room=6030 id=22 .*room=6030 id=4 .*room=6030 id=5 .*room=6030 id=6 .*room=6030 id=7 .*room=6030 id=23 .*room=6030 id=24 .*room=6030 id=8 ")
    pruefe("L21: msg 21" "[msgtext] room=6030 id=21 \"John: Name's John. This is Sherry. We came down here with a girl named Elza.\"")
    pruefe("L21: msg 2 (Leon)" "[msgtext] room=6030 id=2 \"Leon: Where is she now?\"")
    pruefe("L21: msg 3" "[msgtext] room=6030 id=3 \"John: We got split up in the fire. One second she was right behind us...\"")
    pruefe("L21: msg 22 (Sherry)" "[msgtext] room=6030 id=22 \"Sherry: Elza's still out there...!\"")
    pruefe("L21: msg 4 (zwei Seiten)" "[msgtext] room=6030 id=4 \"Leon: If she's as tough as you say, she'll find another way out.Leon: But we can't wait. When that reactor goes, nobody gets out.\"")
    pruefe("L21: msg 5" "[msgtext] room=6030 id=5 \"John: Then I'm staying with the kid. And I'll keep an eye on your friend here.\"")
    pruefe_nicht("L21: kein 'fighting the monsters'" "fighting the monsters")
    pruefe_nicht("L21: kein 'catch up'" "catch up")
    # Stimmen: 21/2/3/22/4/5 = NEUAUF: alte Aufnahmen spielen unter dem neuen Text (AUFNAHMELISTE "zuerst")
    foreach(_m 21 2 3 22 4 5)
        pruefe("Ton: msg ${_m} spielt die vorhandene Aufnahme" "[voice] Start ROOM6030 Id ${_m} ")
    endforeach()
    # Fuehrerstand vor dem Tor (Spur ST80) und Zugstart (Spur AN) unveraendert
    pruefe("Fuehrerstand: Programm sub08 (ST80)" "[story80] ROOM6030 Evt_exec sub08 -> Programm")
    pruefe("Torschalter 6000 Slot 5" "[fire-aot] slot=5 at F300 (Raum 6000)")
    pruefe("Wagen vor dem Zugstart (Spur AN)" "[annette] ROOM6030 Wagen (Cut 6): Zugstart nicht gelaufen")
    pruefe("Zugstart Leon (Spur AN)" "ROOM6030 Zugstart-Szene statt sub05")
    pruefe_re("Zugstart Leon: msg 27..30" "room=6030 id=27 .*room=6030 id=28 .*room=6030 id=29 .*room=6030 id=30 ")
    pruefe_nicht("keine Elza-Programme in 6030" "ROOM6031 Evt_exec")
    pruefe("5090 erreicht" "EXIT_AT: Bild 200 in Raum 5090")
    _fertig()
else()
    fail("unbekannter TEIL ${TEIL}")
    _fertig()
endif()
