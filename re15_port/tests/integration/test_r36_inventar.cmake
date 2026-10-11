# =============================================================================
# Runde 36 Spur I "inventar" — ECHTE exe. Dossier: analysis/befunde_runde36/I_inventar.md
#
# Nutzer (AUFTRAG.md Z.28/29): "Mit Nutzung von Memory Cards beim Speichern soll sich die Anzahl
# reduzieren. Sind Memory Cards oder Handgranaten 0 sollen sie aus dem Player inventory verschwinden"
# / "Bei den Debug Menu ... schickst du, wenn man mit Elza spielt Elza in Leons Raeume. Wenn man mit
# Elza spielt, sind die Raeume mit '1' hinten korrekt, zum Beispiel ROOM 1071."
#
# LAEUFE
#   s2  Spielstand ROOM1150 vor dem Telefon, Memory Card x2 in Platz 0, Browning ausgeruestet in Platz 1.
#       VIERECK am Telefon (Runde-33-Weg), JA -> Speicherschirm -> Platz 1.
#       MUSS: "card=used (slot 0 qty 1)"; geschriebener Block: Inventar 21:1 03:15 15:50 (Karte x1).
#   s1  wie s2 mit der LETZTEN Karte (x1).
#       MUSS: "card=used-last, slot removed (slot 0 qty 0)", Live-Inventar "03:15 15:50", geschriebener
#       Block: Inventar 03:15 15:50 leer (Browning rueckt nach, RE2 @0x801c11d0-e0 / FUN_8004dadc),
#       Ausruest-Platz 0 (zieht mit, @0x8004dbac-c8), Waffe 3.
#   g   letzte Handgranate in ROOM1140 werfen (Rezept integration_r35_granate "wand", RE15_GIVE=9:1),
#       START erst nach dem Wurf.
#       MUSS: Projektil gespawnt (gr.log "SPAWN granate art=2") — der Wurf laeuft trotz Entfernen zu Ende;
#       "[inventar-r36] F<a> Platz 2 Id 09 Menge 0 -> entfernt" mit 19 <= a < 280 (vor dem START);
#       "[inventar-r36] F<b> Waffe 9 (leer, abgelegt) -> 1" mit b > a (erst nach dem Wurf);
#       "[equip] W-bank -> W01" (Messer-Bank).
#   e1  Elza (Spielerwahl umgeschaltet), Debug-Menue-Sprung 115 -> MUSS "(ROOM1151)" + "loaded room1151.rdt".
#   e0  Leon, derselbe Sprung -> MUSS "(ROOM1150)" + "loaded room1150.rdt".
#
# Er DISKRIMINIERT: der Vorher-Stand (78a4a748) scheitert in s2/s1 ("card=kept", Block 21 01 unveraendert),
# in g (kein "[inventar-r36]", Granate 0 bleibt) und in e1 (ROOM1150 statt ROOM1151).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_inventar_karte> -DWORKDIR=<dir>
#               -P test_r36_inventar.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r36_inventar: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r36_inventar: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r36_inventar: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Muster test_r34n_b_cursor.cmake: fremde `taskkill /IM re15_pc.exe`).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r36i_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r36i_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(_fehler _was)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r36_inventar: ${_was}")
endfunction()

# debug.log als HEX (er traegt UTF-8-/Binaerbytes; file(STRINGS) verschluckt dahinter stumm alles —
# Befund test_r33_speichern.cmake). Ein Lauf ohne EXIT_AT-Zeile ist ungueltig.
function(_hex _text _out)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(${_out} "${_h}" PARENT_SCOPE)
endfunction()

function(_lauf _name _out_hex)
    set(WORKDIR "${_basis}_${_name}")
    re15_start_spiel(_rv 240 ${ARGN} "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        _fehler("[${_name}] kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _lh HEX)
    _hex("EXIT_AT: Bild" _he)
    string(FIND "${_lh}" "${_he}" _p)
    if(_p LESS 0)
        _fehler("[${_name}] Endbild nicht erreicht — Lauf abgerissen (exit=${_rv}, ${WORKDIR})")
    endif()
    set(${_out_hex} "${_lh}" PARENT_SCOPE)
endfunction()

macro(_muss _log _text _was)
    _hex("${_text}" _mh)
    string(FIND "${${_log}}" "${_mh}" _p)
    if(_p LESS 0)
        _fehler("${_was} (gesucht: '${_text}')")
    endif()
endmacro()

# Dezimalzahl direkt hinter _text im HEX-Log (Ziffern 0x30..0x39), leer wenn _text fehlt.
function(_zahl_nach _log_hex _text _out)
    _hex("${_text}" _th)
    string(FIND "${_log_hex}" "${_th}" _p)
    set(_z "")
    if(NOT _p LESS 0)
        string(LENGTH "${_th}" _l)
        math(EXPR _i "${_p} + ${_l}")
        foreach(_k RANGE 0 7)
            string(SUBSTRING "${_log_hex}" ${_i} 2 _pr)
            if(NOT _pr MATCHES "^3[0-9]$")
                break()
            endif()
            string(SUBSTRING "${_pr}" 1 1 _d)
            string(APPEND _z "${_d}")
            math(EXPR _i "${_i} + 2")
        endforeach()
    endif()
    set(${_out} "${_z}" PARENT_SCOPE)
endfunction()

# ---- Speichern ---------------------------------------------------------------------------------
function(_speichern _name _menge _out_log)
    set(_wd "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${_wd}")
    file(REMOVE "${_wd}/re15_card.mcr" "${_wd}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_menge}"
                    WORKING_DIRECTORY "${_wd}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${_wd}/re15_card.mcr")
        _fehler("[${_name}] Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    _lauf(${_name} _l
        RE15_NO_INTRO=1 RE15_NOAUDIO=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
        RE15_CARD_SLOT=0,1                    # laden aus Platz 0, speichern in Platz 1
        RE15_MSG_LOG=1
        "RE15_PAD_AT=150:A,200:A,290:A,370:A" # untersuchen, Seiten, JA (Runde-33-Weg)
        "RE15_EXIT_AT=460#1150")
    _muss(_l "CONTINUE: resumed in room 1150" "[${_name}] Spielstand nicht geladen")
    _muss(_l "Antwort: JA -> Speicherbildschirm" "[${_name}] die Speicherfrage kam nicht / kein JA")
    set(${_out_log} "${_l}" PARENT_SCOPE)
endfunction()

# Block Platz 1 = Datei 0x4100 (Block 2 @0x4000 + 0x100): equipped_slot +35 = 0x4123, weapon_id
# +36 = 0x4124, inv +40 = 0x4128 (re15_savedata.h).
_speichern(s2 2 _log_s2)
_muss(_log_s2 "card=used (slot 0 qty 1)" "s2: zwei Memory Cards -> nach dem Speichern nicht 1 (RE2 @0x801c11ac)")
# Platz = 4 Bytes {Id, Menge, Flag, +3} (re15_inv_slot_t, Schritt 4 wie 0x800b10ac).
file(READ "${_basis}_s2/re15_card.mcr" _inv OFFSET 16680 LIMIT 12 HEX)
if(NOT _inv STREQUAL "21010000030f000015320000")
    _fehler("s2: geschriebener Block Inventar '${_inv}', Soll 21010000 030f0000 15320000 (Karte x1 MIT abgezogen, RE2: Abzug vor dem Blockbau)")
endif()
message(STATUS "r36_inventar s2: Memory Card x2 -> x1 (Log + Kartenblock 21:1 03:15 15:50)")

_speichern(s1 1 _log_s1)
_muss(_log_s1 "card=used-last, slot removed (slot 0 qty 0)" "s1: letzte Memory Card nicht verbraucht/entfernt")
_muss(_log_s1 "Memory Card verbraucht: Platz 0 Rest 0 -> Platz entfernt | Inventar 03:15 15:50 00:0"
      "s1: Live-Inventar nach dem Speichern nicht verdichtet (FUN_8004dadc)")
file(READ "${_basis}_s1/re15_card.mcr" _inv OFFSET 16680 LIMIT 12 HEX)
if(NOT _inv STREQUAL "030f00001532000000000000")
    _fehler("s1: geschriebener Block Inventar '${_inv}', Soll 030f0000 15320000 00000000 (Karte weg, Browning rueckt nach)")
endif()
file(READ "${_basis}_s1/re15_card.mcr" _eq OFFSET 16675 LIMIT 2 HEX)
if(NOT _eq STREQUAL "0003")
    _fehler("s1: Block Ausruest-Platz/Waffe '${_eq}', Soll 0003 (Platz 0 = Browning, Waffe 3; @0x8004dbac-c8)")
endif()
message(STATUS "r36_inventar s1: letzte Memory Card -> Platz weg, Browning Platz 0 ausgeruestet (Block 03:15 15:50 leer, Platz 0 / Waffe 3)")

# ---- Handgranate -------------------------------------------------------------------------------
set(_wd "${_basis}_g")
file(MAKE_DIRECTORY "${_wd}")
_lauf(g _log_g
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 SDL_ASSERT=always_ignore
    RE15_WINDOW_SCALE=1 RE15_GRANATE_LOG=gr.log
    RE15_DEBUG_JUMP=1140@250 "RE15_PLAYER_POS=-4311,-19289,0" RE15_AI_FLAVOR=re2
    RE15_GIVE=9:1 RE15_EQUIP=9
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1
    "RE15_INPUT_SCRIPT=M0.6,MA0.2,M2.5,W6,S0.1,W3"
    "RE15_EXIT_AT=400#1140")
_muss(_log_g "[give] item 9 x1" "g: Vorbedingung (eine Handgranate) fehlt")
if(NOT EXISTS "${_wd}/gr.log")
    _fehler("g: kein gr.log — Wurf nicht gelaufen")
endif()
file(READ "${_wd}/gr.log" _gr)
string(FIND "${_gr}" "SPAWN granate art=2" _p)
if(_p LESS 0)
    _fehler("g: die letzte Granate wurde nicht geworfen (kein 'SPAWN granate art=2')")
endif()
_zahl_nach("${_log_g}" "[inventar-r36] F" _fa)
_muss(_log_g "Platz 2 Id 09 Menge 0 -> entfernt" "g: leere Handgranate (Platz 2) wurde nicht entfernt")
_muss(_log_g "Waffe 9 (leer, abgelegt) -> 1 (Messer-Rueckfall 0x80)"
      "g: Waffe nach der leeren Granate nicht auf das Messer (Commit-Regel @0x80046654-88)")
# zweite [inventar-r36]-Zeile = der Waffenwechsel: Bildnummer vor "Waffe 9"
_hex(" Waffe 9 (leer, abgelegt)" _hw)
string(FIND "${_log_g}" "${_hw}" _pw)
string(SUBSTRING "${_log_g}" 0 ${_pw} _vor)
_hex("[inventar-r36] F" _hp)
string(FIND "${_vor}" "${_hp}" _pz REVERSE)
string(SUBSTRING "${_log_g}" ${_pz} -1 _ab)
_zahl_nach("${_ab}" "[inventar-r36] F" _fb)
if("${_fa}" STREQUAL "" OR "${_fb}" STREQUAL "")
    _fehler("g: Bildnummern nicht lesbar (Platz '${_fa}', Waffe '${_fb}')")
endif()
if(_fa LESS 19 OR NOT _fa LESS 280)
    _fehler("g: Granate bei F${_fa} entfernt — Soll nach dem Abzug (F19) und vor dem START (F280)")
endif()
if(NOT _fb GREATER _fa)
    _fehler("g: Waffe wechselte bei F${_fb}, Platz weg bei F${_fa} — der Wechsel muss NACH dem Wurf liegen")
endif()
_muss(_log_g "[equip] W-bank -> W01" "g: Messer-Bank W01 nicht geladen")
message(STATUS "r36_inventar g: letzte Granate geworfen, Platz weg F${_fa}, Messer ab F${_fb}")

# ---- Debug-Menue Elza / Leon -------------------------------------------------------------------
foreach(_fall e1 e0)
    set(_wd "${_basis}_${_fall}")
    file(MAKE_DIRECTORY "${_wd}")
    if(_fall STREQUAL "e1")
        set(_wahl RE15_PSELECT_AUTO_SWITCH=1)
        set(_raum 1151)
    else()
        set(_wahl "")                            # Leon bleibt gewaehlt
        set(_raum 1150)
    endif()
    _lauf(${_fall} _log_e
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_ASSERT=always_ignore RE15_TITLE_CONFIRM_MS=1500
        RE15_PSELECT_AUTO=1 ${_wahl}
        RE15_DEBUG_JUMP=1150@250 "RE15_EXIT_AT=300#${_raum}")
    _muss(_log_e "JUMP -> 115 " "${_fall}: Debug-Menue-Sprung lief nicht")
    _muss(_log_e "(ROOM${_raum})" "${_fall}: Sprungziel nicht ROOM${_raum} (Elza-Bit @0x800397e4/@0x800397ec)")
    string(TOLOWER "loaded room${_raum}.rdt" _geladen)
    _muss(_log_e "${_geladen}" "${_fall}: ROOM${_raum}.RDT nicht geladen")
    message(STATUS "r36_inventar ${_fall}: Debug-Sprung 115 -> ROOM${_raum}")
endforeach()

file(REMOVE "${_exe_kopie}")
message(STATUS "r36_inventar: OK")
