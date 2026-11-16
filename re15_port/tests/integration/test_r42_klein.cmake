# =============================================================================
# Runde 42 "story" (v0.8.31), Stufe Kleinkram, T-08 (Ada im Schutzraum 6020 nur, solange sie lebt) - DURCHLAEUFE in der echten exe
# (RE15_WEGPLAN: Gehen, Blick, Aktionstaste wie der Spieler; Raumwechsel ueber die Tuer des Spiels). Konstanten/Belege
# include/re15_story_klein_r42.h, Code engine/src/story_klein_r42.c. PORT-STORY nach dem Konzept (5 T-08 [B22]).
#
# TEIL=szene  ROOM50B0, Leon geht in die Ausloesezone (main00-Satz Slot 3 sub00 @0x004FE): Adas Szene laeuft (sub02), Ada stirbt
#             (msg 13), das Spiel selbst setzt den Merker (3,0x62) @0x005A6 - FLAG (3,98) vorher 0, nachher 1.
# TEIL=lebt   ROOM5130 -> Tuer -> ROOM6020 mit (3,0x62) = 0: Marvin (0x40) UND Ada (0x42) erscheinen (sub00 @0x00B96/@0x00BAA).
# TEIL=tot    ROOM5130 -> Tuer -> ROOM6020 mit (3,0x62) = 1: nur Marvin, Ada verworfen.
#
# Vorgesetzt (der Wegplan-Bot kann es nicht spielen):
#   - (3,76): gesetzt in ROOM4010 @0x00DAC bzw. ROOM5011 @0x0094C, Bedingung des Schutzraum-Satzes (6020 sub00 @0x00B92). Liegt im
#     normalen Spiel lange vor 50B0/6020.
#   - lebt/tot: Ankunft in 5130 = Ziel der Schutzraum-Tuer (ROOM6020 main00 @0x00B46: (-28150,13250) in 5130); von dort geht der Bot
#     selbst zur Tuer 5130 Slot 1 (main00 @0x009DE) und drueckt die Aktionstaste.
#   - tot: (3,98) = 1 ist GENAU der Zustand, den Adas Szene hinterlaesst (einziger Schreiber im Spiel: ROOM50B0 sub02 @0x005A6, Zensus
#     aller RDTs; der Port schreibt das Bit nirgends - TEIL szene zeigt den Schreiber in der echten exe). Dazu das Ergebnis des Kampfs
#     danach: die zwei Gegner Typ 0x29 (Kill-Flags (8,0x80)/(8,0x81), sub00 @0x00526/@0x0053A) und sub04 ((3,99) @0x009EA, oeffnet
#     die Tueren). Den Kampf gewinnt der Bot nicht (gemessen Laeufe tot1/tot2: Leon stirbt, hp -5). Vorher gesetzte Kill-Flags in 50B0
#     selbst sind KEIN normaler Zustand (sub04 laeuft dann schon beim Betreten, Lauf tot4) - deshalb setzt der Lauf in 5130 ein.
#     Kein anderer Leser: 6020 fragt nur (3,76) und jetzt (3,0x62); die 5130-Gegner tragen die Kill-Flags 0xA2/0xA3 (@0x00A1E/@0x00A32).
#
# T-09 BILD (Nachbesserung Kleinkram R1: das oeffnende Anfuehrungszeichen 0x61 erschien als fremdes Zeichen, weil der Inventar-Renderer
# die Schriftseite nur 128 Zeilen hoch hielt; inv_render_pc.c s_font4 jetzt 256 Zeilen):
# TEIL=glyphe_karte  ROOM4010 als Leon, Blue Master Keycard im Inventar, START / ITEM / Karte / CHECK; der reine Software-Framebuffer
#                    (RE15_INV_FB_SHOT) wird mit test_r42_klein glyphe gegen die nachgezeichnete Beschreibung verglichen.
# TEIL=glyphe_uhr    dasselbe mit der Taschenuhr 0x3F (F-19, story2), Seite 2 '"For Sherry. Love, Mom."' (beginnt mit 61 22).
#   Vorgesetzt (der Wegplan-Bot nimmt die Karte nicht vom Pult, Pruefer-Lauf R1): Gegenstand im Inventar per RE15_GIVE = Zustand nach
#   dem Aufheben (Karte: ROOM4010 Item_aot_set Slot 9 @0x00592, Taken-Bit (9,6) gesetzt; Uhr: ROOM2090 @0x00A12, Taken-Bit 90).
#   Zustand der Geschichte vor 4010 wie im Pruefer-Lauf: (4,34), (3,72), (3,76), (3,79). Die CHECK-Darstellung liest keinen davon.
#   Tasten (menu_common.c command_select): A = Viereck = Bestaetigen (virtuell 0x4000), D = Platz 2 im Raster, L = CHECK (25d6 = 1
#   -> 25c2 = 9, @0x8004a558-64).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -DTEIL=<szene|lebt|tot|glyphe_karte|glyphe_uhr> [-DPRUEFER=<test_r42_klein>]
#         -P test_r42_klein.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r42_klein: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r42_klein: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL lebt)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}_${TEIL}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r42kk_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r42kk_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_fehler "")
macro(fail _t)
    list(APPEND _fehler "${_t}")
    message(STATUS "FAIL r42_klein[${TEIL}]: ${_t}")
endmacro()
macro(ok _t)
    message(STATUS "ok   r42_klein[${TEIL}]: ${_t}")
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
        message(FATAL_ERROR "r42_klein[${TEIL}]: ${_nf} Fehler (${WORKDIR})")
    endif()
    message(STATUS "r42_klein[${TEIL}]: alle Pruefungen ok (${WORKDIR})")
endfunction()

set(_basis RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 SDL_AUDIODRIVER=dummy
           RE15_SPAWN_DIAG=1 RE15_WEGPLAN_LOG=wegplan.txt RE15_STATE_LOG=state.log "RE15_WEGPLAN=${WORKDIR}/plan.txt")

if(TEIL STREQUAL "szene")
    file(WRITE "${WORKDIR}/plan.txt" "RAUM 50B0
WARTE 30
FLAG 3 98
GEHE -16000 -12300 300
FLAG 3 98
ENDE
")
    re15_start_spiel(_rv 1500 ${_basis} RE15_DEBUG_JUMP=50b0@250 "RE15_SET_FLAG=3:76" "RE15_EXIT_AT=2700#50b0" "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/wegplan.txt")
        fail("kein debug.log/wegplan.txt (exit=${_rv})")
        _fertig()
        return()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    file(READ "${WORKDIR}/wegplan.txt" _wp)
    pruefe("Ada (0x42) in 50B0 gesetzt (sub00 @0x00512)" "Sce_em_set type=0x42 behavior=0x40 slot=0")
    pruefe("Adas Szene: msg 0" "[msgtext] room=50b0 id=0 ")
    pruefe("Adas Szene: Ada stirbt (msg 13)" "[msgtext] room=50b0 id=13 ")
    pruefe("Ende des Laufs erreicht" "EXIT_AT: Bild 2700 in Raum 50b0")
    set(_dbg "${_wp}")
    pruefe_re("(3,98) vor der Szene 0, danach 1 (das Spiel setzt es: sub02 @0x005A6)" "FLAG [(]3,98[)] = 0.*FLAG [(]3,98[)] = 1")
    _fertig()
    return()
endif()

if(TEIL MATCHES "^glyphe_")
    if(NOT PRUEFER OR NOT EXISTS "${PRUEFER}")
        message(FATAL_ERROR "r42_klein[${TEIL}]: PRUEFER (test_r42_klein) fehlt: '${PRUEFER}'")
    endif()
    if(TEIL STREQUAL "glyphe_karte")
        set(_item 0x47)
        set(_seite 1)
        set(_flags "RE15_SET_FLAG=4:34,3:72,3:76,3:79,9:6")
        set(_give "RE15_GIVE=71:1")
        set(_tasten "RE15_PAD_AT=500:S,620:A,740:D,860:A,980:L,1100:A")
        set(_bild 900)
        set(_ende 1600)
    else()
        set(_item 0x3F)
        set(_seite 2)
        set(_flags "RE15_SET_FLAG=4:34,3:72,3:76,3:79,9:90")
        set(_give "RE15_GIVE=63:1")
        set(_tasten "RE15_PAD_AT=500:S,620:A,740:D,860:A,980:L,1100:A,1500:A")
        set(_bild 1300)
        set(_ende 2000)
    endif()
    re15_start_spiel(_rv 900 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 SDL_AUDIODRIVER=dummy
        RE15_DEBUG_JUMP=4010@250 "${_flags}" "${_give}" "${_tasten}" "RE15_INV_FB_SHOT=${WORKDIR}/check.bmp"
        "RE15_INV_FB_SHOT_AT=${_bild}" "RE15_EXIT_AT=${_ende}#4010" "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        fail("kein debug.log (exit=${_rv})")
        _fertig()
        return()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    pruefe("Gegenstand im Inventar" "[give] item")
    pruefe("Inventarbild geschrieben" "[inv] fb shot (inv frame")
    pruefe("Ende des Laufs erreicht" "EXIT_AT: Bild ${_ende} in Raum 4010")
    execute_process(COMMAND "${PRUEFER}" glyphe "${WORKDIR}/check.bmp" ${_item} ${_seite} TIMEOUT 60
                    RESULT_VARIABLE _rvg OUTPUT_VARIABLE _outg ERROR_VARIABLE _errg)
    message(STATUS "${_outg}")
    if(NOT _rvg EQUAL 0)
        fail("Bildvergleich der Beschreibung ${_item} Seite ${_seite}: rv=${_rvg}")
    else()
        ok("Beschreibung ${_item} Seite ${_seite} im Bild wie nachgezeichnet")
    endif()
    _fertig()
    return()
endif()

if(TEIL STREQUAL "lebt")
    set(_flags "RE15_SET_FLAG=3:76")
elseif(TEIL STREQUAL "tot")
    set(_flags "RE15_SET_FLAG=3:76,3:98,3:99,8:128,8:129")
else()
    message(FATAL_ERROR "r42_klein: TEIL '${TEIL}' unbekannt")
endif()
file(WRITE "${WORKDIR}/plan.txt" "RAUM 5130
WARTE 30
FLAG 3 98
GEHE -28500 13300 150
BLICK 2048
AKTION
RAUM 6020
WARTE 120
FLAG 3 98
ENDE
")
re15_start_spiel(_rv 900 ${_basis} RE15_DEBUG_JUMP=5130@250 "${_flags}" "RE15_PLAYER_POS=-28150,13250,2048,0" "RE15_EXIT_AT=200#6020"
    "${_exe_kopie}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    fail("kein debug.log (exit=${_rv})")
    _fertig()
    return()
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
pruefe("6020 ueber die Tuer erreicht" "EXIT_AT: Bild 200 in Raum 6020")
pruefe("Marvin (0x40) gesetzt (6020 sub00 @0x00B96)" "Sce_em_set type=0x40 behavior=0x40 slot=0")
if(TEIL STREQUAL "lebt")
    pruefe("Ada lebt: Satz @0x00BAA zugelassen" "[story-klein] F")
    pruefe("Ada lebt: Haken meldet 'erscheint'" "ROOM6020: Ada (Typ 0x42, sub00 @0x00BAA) erscheint - lebt")
    pruefe_nicht("Ada lebt: nicht verworfen" "Ada (Typ 0x42, sub00 @0x00BAA) verworfen")
else()
    pruefe("Ada tot: Satz @0x00BAA verworfen" "ROOM6020: Ada (Typ 0x42, sub00 @0x00BAA) verworfen - tot in 50B0")
    pruefe_nicht("Ada tot: nicht zugelassen" "Ada (Typ 0x42, sub00 @0x00BAA) erscheint")
endif()
# Aktor-Zaehlung im Zustandsprotokoll (Typ hex, "t=42" = Ada 0x42, "t=40" = Marvin 0x40): Ada nur im Fall "lebt" als Aktor in 6020
if(EXISTS "${WORKDIR}/state.log")
    file(STRINGS "${WORKDIR}/state.log" _letzte REGEX "t=40 " LIMIT_COUNT 100000)
    list(LENGTH _letzte _n)
    if(_n GREATER 0)
        list(GET _letzte -1 _z)
        string(FIND "${_z}" "t=42 " _ada)
        string(FIND "${_z}" "t=40 " _mar)
        if(_mar LESS 0)
            fail("state.log: Marvin (t=40) fehlt im letzten Bild")
        elseif(TEIL STREQUAL "lebt" AND _ada LESS 0)
            fail("state.log: Ada (t=42) fehlt im letzten Bild")
        elseif(TEIL STREQUAL "tot" AND NOT _ada LESS 0)
            fail("state.log: Ada (t=42) ist da")
        else()
            ok("state.log letztes Bild: Marvin da, Ada ${TEIL}")
        endif()
    else()
        fail("state.log: keine Aktorzeile mit Marvin/Ada")
    endif()
endif()
_fertig()
