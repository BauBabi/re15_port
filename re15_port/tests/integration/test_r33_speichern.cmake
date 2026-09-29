# =============================================================================
# SPEICHERN NUR MIT MEMORY CARD (Runde 33, Thema S; Nutzer: "eine Message ... wie in
# Resident Evil 2, wenn man speichern moechte, aber kein Farbband besitzt. Nur statt
# Farbband eben Memory Card. Speichern soll nur moeglich sein, wenn man eine Memory Card
# besitzt.")  Dossier: analysis/befunde_runde33/speichern_memory_card.md
#
# RE2-VORBILD (info/re2leon/PSX.EXE): AOT-Typ 9 -> Handler 0x80051AB0; die Fortsetzung
# 0x80051B04 sucht das Farbband (Item 0x1E) mit FUN_800696CC NUR im Inventar
# (@0x80051b34/@0x80051b38). Ohne Farbband Meldung (0x100,0) @0x8009EFCC, kein Speichern
# (@0x80051b68-b98). Mit Farbband Meldung (0x100,1) @0x8009F01F + Ja/Nein; JA ->
# Speicherbildschirm (@0x80051bc4-dc), NEIN -> zurueck (@0x80051be8-f4). Der Port ersetzt
# das Farbband durch die Memory Card (Item 0x21) und verbraucht sie NICHT (PORT-WAHL,
# Nutzerwunsch; RE2 verbraucht in MEM_CARD.BIN @0x801C118C-0x801C11E0).
#
# GEMESSEN VOR DEM BAU (echte exe, Spielstand des Nutzers ROOM1150, VIERECK am Telefon):
#     [msg] room=1150 id=1 SAVEPOINT (menu, message suppressed)
#     [save] saved (room 1150) slot n=1 -> next=2; card=none
# d.h. der Port speicherte OHNE Karte, ohne jede Meldung.
#
# DER RIEGEL, drei Laeufe mit der ECHTEN exe, Spielstand vom Kartenwerkzeug
# probe_r33_speichern_karte (ROOM1150, Leon vor dem Telefon), echtes Untersuchen per
# VIERECK (RE15_PAD_AT, Bildnummern im Raum):
#   A  OHNE Karte: 150 A (untersuchen), 200 A (Seite), 330 A (schliessen).
#      MUSS: "card_slot=-1 -> HINWEIS"; DARF NICHT: "FRAGE", "[save] saved".
#   B  MIT Karte, JA: 150 A, 200 A, 290 A (Seiten), 370 A (Ja ist vorbelegt).
#      MUSS: "FRAGE", "Antwort: JA", "[save] saved", "card=kept (slot 0 qty 1)" UND im
#      geschriebenen Kartenblock (Platz 1) Inventarplatz 0 = 21 01 (Karte mitgespeichert).
#   C  MIT Karte, NEIN: wie B, dazu 360 R (Cursor auf NEIN), 380 A.
#      MUSS: "FRAGE", "Antwort: NEIN"; DARF NICHT: "[save] saved".
#
# Er DISKRIMINIERT: der Vorher-Stand (Sofort-Menue) scheitert in A ("[save] saved" ohne
# Karte) und in B/C (kein "FRAGE"); ein Verbrauch wie in RE2 scheitert in B
# ("card=kept (slot 0 qty 1)" fehlt, Block traegt 00 00).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r33_speichern_karte>
#               -DWORKDIR=<dir> -P test_r33_speichern.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r33_speichern: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r33_speichern: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r33_speichern: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Eigenes Arbeitsverzeichnis je Aufruf (Rest eines vorigen Aufrufs haelt sonst debug.log).
string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

function(_hex _text _out)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(${_out} "${_h}" PARENT_SCOPE)
endfunction()
_hex("CONTINUE: resumed"          _h_continue)
_hex("EXIT_AT: Bild"              _h_exit)
_hex("card_slot=-1 -> HINWEIS"    _h_hinweis)
_hex("-> FRAGE"                   _h_frage)
_hex("Antwort: JA"                _h_ja)
_hex("Antwort: NEIN"              _h_nein)
_hex("[save] saved"               _h_saved)
_hex("card=kept (slot 0 qty 1)"   _h_kept)

# Ein Lauf: Karte schreiben (mit/ohne Memory Card), exe fahren, debug.log als HEX liefern
# (debug.log enthaelt Binaer-Bytes; file(STRINGS) verschluckt dahinter stumm alles).
function(r33_lauf _name _karte _pad _out_hex)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${_karte}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r33_speichern[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 180
        RE15_NO_INTRO=1
        RE15_NOAUDIO=1
        RE15_CONTINUE_TEST=1
        RE15_CARD_AUTO=1
        RE15_CARD_SLOT=0,1           # laden aus Platz 0, speichern in Platz 1
        RE15_MSG_LOG=1
        "RE15_PAD_AT=${_pad}"
        "RE15_EXIT_AT=460#1150"
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r33_speichern[${_name}]: kein debug.log (exit=${_rv})")
    endif()
    file(READ "${WORKDIR}/debug.log" _lh HEX)
    foreach(_m _h_continue _h_exit)
        string(FIND "${_lh}" "${${_m}}" _p)
        if(_p LESS 0)
            message(FATAL_ERROR "r33_speichern[${_name}]: Lauf unvollstaendig (${_m} fehlt, "
                                "exit=${_rv}) — ueber die Speicherstelle sagt er nichts")
        endif()
    endforeach()
    set(${_out_hex} "${_lh}" PARENT_SCOPE)
endfunction()

macro(_muss _log _hex _was)
    string(FIND "${${_log}}" "${${_hex}}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r33_speichern: ${_was}")
    endif()
endmacro()
macro(_darf_nicht _log _hex _was)
    string(FIND "${${_log}}" "${${_hex}}" _p)
    if(NOT _p LESS 0)
        message(FATAL_ERROR "r33_speichern: ${_was}")
    endif()
endmacro()

# --- A: ohne Memory Card ------------------------------------------------------
r33_lauf(a "" "150:A,200:A,330:A" _log_a)
_muss(_log_a _h_hinweis "A: ohne Memory Card kam nicht der Hinweis (RE2 Meldung (0x100,0) @0x8009EFCC)")
_darf_nicht(_log_a _h_frage "A: ohne Memory Card wurde gefragt")
_darf_nicht(_log_a _h_saved "A: OHNE Memory Card wurde gespeichert (RE2 @0x80051b68-b98: kein Speichern)")
message(STATUS "r33_speichern A: ohne Karte Hinweis, kein Speichern")

# --- B: mit Memory Card, JA ---------------------------------------------------
r33_lauf(b "karte" "150:A,200:A,290:A,370:A" _log_b)
_muss(_log_b _h_frage "B: mit Memory Card kam keine Frage (RE2 Meldung (0x100,1) @0x8009F01F)")
_muss(_log_b _h_ja    "B: die Antwort JA kam nicht an")
_muss(_log_b _h_saved "B: nach JA wurde nicht gespeichert (RE2 @0x80051bc4-dc)")
_muss(_log_b _h_kept  "B: die Memory Card ist nach dem Speichern nicht mehr unveraendert da (PORT-WAHL: kein Verbrauch)")
# Kartenblock Platz 1 = Block 2: Spielstand @0x4000+0x100, Inventar @+40 -> Datei 0x4128.
file(READ "${_basis}_b/re15_card.mcr" _inv0 OFFSET 16680 LIMIT 2 HEX)   # 16680 = 0x4128
if(NOT _inv0 STREQUAL "2101")
    message(FATAL_ERROR "r33_speichern: B: der geschriebene Spielstand traegt in Inventarplatz 0 "
                        "'${_inv0}', Soll '2101' (Memory Card x1 mitgespeichert, nicht verbraucht)")
endif()
message(STATUS "r33_speichern B: mit Karte + JA gespeichert, Karte bleibt (Log + Kartenblock 21 01)")

# --- C: mit Memory Card, NEIN -------------------------------------------------
r33_lauf(c "karte" "150:A,200:A,290:A,360:R,380:A" _log_c)
_muss(_log_c _h_frage "C: mit Memory Card kam keine Frage")
_muss(_log_c _h_nein  "C: die Antwort NEIN kam nicht an")
_darf_nicht(_log_c _h_saved "C: nach NEIN wurde gespeichert (RE2 @0x80051be8-f4: zurueck)")
message(STATUS "r33_speichern C: mit Karte + NEIN kein Speichern")
