# =============================================================================
# WESTEN-LADE-PIN (Nutzer-Befund 2026-09-20: "dann sammelt man die weste ein
# speichert und laedt den Spielstand — ist die weste weg").
#
# GEMESSEN (echte exe, vor dem Fix): der Ladevorgang stellt den Save-Block samt
# Flag(3,0x75) und Westen-Bonus korrekt her — debug.log meldet
#     [save] CONTINUE: resumed in room 1190 (hp=105)
# — aber im GANZEN Log steht KEINE `[pld]`-Zeile. Der Modellwechsel lief nie,
# Leon stand in PL00 (ohne Weste) im Raum.
#
# ORIGINAL: der LOAD kopiert den Save-Block wholesale zurueck
#     80026294  addiu a0,a0,3516      ; a0 = 0x800b0dbc
#     8002629c  jal   0x8004ee38      ; memcpy
#     800262a0  ori   a2,zero,0x1430  ; 0x1430 Bytes
# und die Modell-Variante DAT_800b0ff0 liegt bei Block+0x234, also MITTEN drin.
# Danach laeuft der Raumlader FUN_800396fc und zieht das Modell nach:
#     80039760  lbu  a0,0x0(s0)=>DAT_800aca5c
#     80039768  lh   v1,DAT_800b0ff0
#     8003976c  andi v0,a0,0xf
#     80039770  beq  v0,v1,LAB_80039790     ; unveraendert -> nichts tun
#     80039788  jal  FUN_800314b0           ; sonst Spielermodell NEU LADEN
# FUN_800314b0 @0x800314d4-1c waehlt die Datei aus der Tabelle @0x80073f70
# (16 u16 0x3C..0x4B) -> Datei-Id = 0x3C + Index, Index 1 = PL01.PLD = die
# R.P.D.-Ruestung (main.c, pc_sync_player_model).
#
# DER RIEGEL: Karte mit angelegter Weste schreiben (probe_r17_weste_karte setzt
# Flag(3,0x75)=1 + Raum 0x1190), die exe per CONTINUE-Auto-Drive hineinladen und
# pruefen, dass das debug.log GENAU diese Zeile traegt:
#     [pld] Spielermodell -> PLD/PL01.PLD ...  [R.P.D.-Ruestung an]
#
# Er DISKRIMINIERT: ohne den Nachzieher am Lade-Weg (main.c, CONTINUE-Restore)
# fehlt die Zeile komplett — genau der gemessene Vorher-Stand.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe> -DWORKDIR=<dir>
#               -P test_weste_load_pin.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "weste_load_pin: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "weste_load_pin: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "weste_load_pin: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Eigenes Arbeitsverzeichnis je Aufruf — gleiche Begruendung wie im Save-Zaehler-Pin
# (ein Rest des vorigen Aufrufs haelt sonst debug.log gesperrt).
string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")

# --- Karte mit ANGELEGTER Weste schreiben (Flag(3,0x75)=1, Raum 0x1190) -------
execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "1190"
                WORKING_DIRECTORY "${WORKDIR}"
                TIMEOUT 60
                RESULT_VARIABLE _rvk
                OUTPUT_VARIABLE _outk)
if(NOT _rvk EQUAL 0)
    message(FATAL_ERROR "weste_load_pin: Kartenwerkzeug exit=${_rvk}\n${_outk}")
endif()
if(NOT EXISTS "${WORKDIR}/re15_card.mcr")
    message(FATAL_ERROR "weste_load_pin: keine Karte geschrieben")
endif()

# --- CONTINUE-Lauf: Titel -> LOAD GAME -> Slot 0 -> Raum 0x1190 ---------------
re15_start_spiel(_rv1 180
    RE15_NO_INTRO=1
    RE15_NOAUDIO=1
    RE15_SOFTWARE_RENDER=1
    RE15_CONTINUE_TEST=1
    RE15_CARD_AUTO=1
    RE15_CARD_SLOT=0
    RE15_BOOT_EXIT_AT=1          # Prozessende direkt nach dem Boot-Block (deterministisch)
    "${RE15_PC_EXE}")

if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "weste_load_pin: kein debug.log (exit=${_rv1})")
endif()

# debug.log enthaelt auch Binaer-Bytes -> binaer-sicher als HEX lesen und suchen
# (dieselbe Falle wie im Save-Zaehler-Pin: file(STRINGS) verschluckt stumm alles
# hinter dem ersten Nullbyte).
file(READ "${WORKDIR}/debug.log" _lh HEX)

# "CONTINUE: resumed" = 434f4e54494e55453a20726573756d6564
string(FIND "${_lh}" "434f4e54494e55453a20726573756d6564" _pos_cont)
if(_pos_cont LESS 0)
    message(FATAL_ERROR "weste_load_pin: der Lauf hat gar nicht geladen "
                        "(keine CONTINUE-Zeile im debug.log, exit=${_rv1})")
endif()

# "PLD/PL01.PLD" = 504c442f504c30312e504c44
string(FIND "${_lh}" "504c442f504c30312e504c44" _pos_pld)
if(_pos_pld LESS 0)
    message(FATAL_ERROR
        "weste_load_pin: nach dem Laden wurde PL01.PLD NICHT geladen — Leon steht "
        "ohne R.P.D.-Weste im Raum. Der Modell-Nachzieher am Lade-Weg fehlt "
        "(Original: FUN_800396fc @0x80039760-8c -> FUN_800314b0).")
endif()

message(STATUS "weste_load_pin: OK — CONTINUE geladen und PL01.PLD (R.P.D.-Weste) nachgezogen")
