# =============================================================================
# RE-LATCH-PIN (Nutzer-Reports 2026-08-17: "Messer-Anims trotz Handfeuerwaffe nach
# Load" + "Modelle kaputt nach Tod -> NEW GAME").
#
# GEMEINSAME URSACHE der Bug-KLASSE: der Tod fuehrt IN-PROCESS zum Titel zurueck
# (`goto re_title`), der ganze Boot-Block von main() laeuft erneut, ALLE lokalen
# Variablen werden neu initialisiert — aber funktions-lokale `static`-Latches
# ueberleben. Das Original kennt diese Klasse nicht: es leitet bei JEDEM Modul-/
# Raum-Start alles unbedingt neu ab bzw. laedt es neu von CD.
#
# ---------------------------------------------------------------------------
# (A) LAUFZEIT-PIN — Waffen-Anim-Bank (Report "Messer-Anims nach Load")
#
# Defekt: `static int s_wgun` im Equip-Watcher ueberlebte den Reboot, waehrend die
# W-Bank-Locals (wact_skel/wact_anim/wact_ok) auf die W01-Messer-Defaults
# zurueckfielen -> `want_gun == s_wgun` -> der Watcher feuerte NIE mehr -> die
# GERENDERTE Pose kam ab Session 2 aus der MELEE-Bank (Recoil-Clip 7 fc=25 statt 23).
#
# Original: die Waffen-Bank wird bei JEDEM Spieler-Load UNBEDINGT neu abgeleitet —
#   FUN_800314b0  `jal FUN_80036b68`  @0x800316f0
#   FUN_80036b68  `lbu v0,DAT_800aca5c` @0x80036b84 (charid)
#                 `lbu a0,DAT_800aca5d` @0x80036b8c (equipped item)
#                 `lhu v0,DAT_800741e8[char*2]` @0x80036bb4 (CD-Datei-Basis)
#                 `jal 0x80013b60` @0x80036bc0 (CD-Load)
#                 `sw v0,DAT_800acbc8` @0x80036be4 / `sw v0,DAT_800acbc4` @0x80036c04
#   — KEIN "letzter Zustand"-Vergleich. Caller von FUN_800314b0: @0x8001d5a4 und
#   der Raumlader FUN_800396fc @0x80039788 (selbst gescannt, jal-Wort 0x0C00C56C).
#
# Harness: RE15_EQUIP=3 (Browning) + RE15_TITLE_SHOT (Title-Auto-Advance -> NEW GAME)
# + RE15_KILL_AT=60 (Tod -> Titel) + RE15_BOOT_EXIT_AT=3 (Prozess endet beim 3. Boot,
# d.h. Boot 1 und Boot 2 laufen vollstaendig). Erwartung: der Equip-Watcher bindet in
# JEDEM Boot neu, also >= 2 Zeilen
#   [equip] W-bank -> W03 (recoil clip7 fc=23)
# GEGENPROBE gemessen 2026-08-17: mit dem alten `static int s_wgun` liefert derselbe
# Lauf genau EINE solche Zeile (Boot 2 bindet nicht um) -> der Pin diskriminiert.
#
# ---------------------------------------------------------------------------
# (B) QUELL-PIN — Player-Select-TIM-Slots (Report "Modelle kaputt nach Tod -> NEW GAME")
#
# Defekt: pc_run_player_select lud Modelle+TIMs nur EINMAL pro Prozess
# (`static int s_models_loaded`) in die TIM-Slots 20/21; jeder In-Game-Boot
# ueberschreibt 20-23 mit den globalen Effekt-Sheets (Blut->20, Muzzle->21,
# Smoke->22, Shell->23) -> der ZWEITE Select rendert Leon/Elza aus Effekt-Texturen.
#
# Original: der Menue-Confirm ERSETZT die Title-Task durch die Select-Task
# (TITLE.BIN `addiu a0,a0,0x1094` -> a0=0x80101094 @0x80102c98 + `jal 0x80029ba4`
# @0x80102c9c), und deren Scene-Init FUN_80101720 (gerufen @0x801011d8) laedt die
# Modelle pro Entry von CD (`jal 0x80013b60` @0x80101808) UND registriert ihre TIMs
# erneut im VRAM (`jal 0x80022150` @0x80101870, a0=2). Kein prozess-langlebiger Cache.
#
# Der Select-Screen ist headless nicht ansteuerbar (er wartet interaktiv auf den
# Confirm; der RE15_TITLE_SHOT-Auto-Advance ruft `re15_gameflow_new_game(0)` direkt
# und ueberspringt ihn), darum pinnt (B) die QUELLE: der VRAM-Re-Upload muss pro
# Entry laufen, also AUSSERHALB des `if (!s_models_loaded)`-Blocks stehen.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -DRE15_PC_MAIN=<main.c>
#               -P test_relatch_pin.cmake
# =============================================================================

if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "relatch_pin: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "relatch_pin: WORKDIR fehlt")
endif()

# ⛔ EIGENES ARBEITSVERZEICHNIS JE AUFRUF (Befund 2026-09-04).
# Diese Tests starten ein GUI-Binary. Gemessen: das Spiel selbst ist stabil —
# 40 direkte Laeufe und 25 Laeufe unter gdb mit exakt dieser Umgebung: 0 Fehler.
# Unter ctest fiel derselbe Lauf trotzdem etwa jedes sechste Mal mit exit=1 aus,
# und zwar schon beim ERSTEN Spielstart des Aufrufs. Ursache ist ein Rest des
# VORHERIGEN Aufrufs: ein noch nicht ganz beendeter Prozess haelt die Dateien im
# gemeinsamen Arbeitsverzeichnis; CMake meldet dann "resource busy or locked",
# das Spiel kann sein debug.log nicht anlegen und stirbt sofort. Der Ausfall
# hinterlaesst wiederum einen Prozess — ab da fielen auch die Folgelaeufe.
# Ein eigenes Verzeichnis je Aufruf schneidet diese Kette durch, ohne fremde
# Prozesse abzuschiessen (was den laufenden Port des Nutzers treffen koennte)
# und ohne Wiederholungsversuche (die echte Fehler verstecken wuerden).
# ⛔ SELBSTDIAGNOSE + EIN Wiederholungsversuch NUR fuer Startfehler (Befund 2026-09-04).
# Gemessen: das Spiel ist stabil, wenn man es direkt startet — 40 Laeufe mit exakt dieser
# Umgebung und 25 Laeufe unter gdb: 0 Fehler. Unter ctest fiel derselbe Lauf trotzdem etwa
# jedes achte Mal mit exit=1 aus, und das debug.log des Fehlerlaufs hatte dann nur FUENF
# Zeilen: es endete nach "[pad] kein Controller gefunden", also mitten im Hochfahren, bevor
# ueberhaupt ein Bild lief. (Der Log ist ungepuffert — es geht nichts verloren, das ist
# wirklich alles, was der Prozess geschrieben hat.) Ein Spielfehler kann das nicht sein:
# dieselbe Binaerdatei laeuft direkt 40 von 40 Mal durch.
#
# Zwei Konsequenzen, beide bewusst eng gefasst:
#  * Der Fehlerfall meldet ab jetzt den Ruecknahmecode UND die letzten Logzeilen. Ein
#    blosses "exit=1" hat mich Stunden gekostet.
#  * Genau EIN Wiederholungsversuch, und NUR wenn der Log kuerzer als 10 Zeilen ist, das
#    Spiel also nicht ueber das Hochfahren hinauskam. Lief es und lieferte ein falsches
#    ERGEBNIS, wird NICHT wiederholt — sonst wuerde der Riegel echte Regressionen
#    verschlucken. Jede Wiederholung steht laut in der Ausgabe.
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")
macro(re15_spiel_lauf _erg _wozu)
    re15_start_spiel(${_erg} 180 ${ARGN})
    if(NOT ${_erg} EQUAL 0)
        set(_zeilen "")
        if(EXISTS "${WORKDIR}/debug.log")
            file(STRINGS "${WORKDIR}/debug.log" _zeilen)
        endif()
        list(LENGTH _zeilen _n)
        message(STATUS "${_wozu}: exit=${${_erg}}, debug.log hat ${_n} Zeilen")
        foreach(_z ${_zeilen})
            message(STATUS "   | ${_z}")
        endforeach()
        if(_n LESS 10)
            message(STATUS "${_wozu}: das Spiel kam nicht ueber das Hochfahren hinaus "
                           "-> EIN Wiederholungsversuch (Startfehler, kein Ergebnisfehler)")
            file(REMOVE "${WORKDIR}/debug.log")
            re15_start_spiel(${_erg} 180 ${ARGN})
        endif()
    endif()
endmacro()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
file(REMOVE "${WORKDIR}/debug.log")

re15_spiel_lauf(rv "relatch_pin"
    RE15_NO_INTRO=1            # Boot-FMV aus
    RE15_NOAUDIO=1             # kein Audio-Device im CI
    RE15_TITLE_SHOT=t.bmp      # Title-Auto-Advance -> re15_gameflow_new_game(0)
    RE15_EQUIP=3               # BROWNING HP equipped -> Gun-Bank W03 erwartet
    RE15_KILL_AT=60            # Tod bei Frame 60 -> Death-FSM -> Titel
    RE15_BOOT_EXIT_AT=3        # Boot 1 + Boot 2 vollstaendig, dann exit(0)
    "${RE15_PC_EXE}")
if(NOT rv EQUAL 0)
    message(FATAL_ERROR "relatch_pin: re15_pc.exe exit=${rv} (erwartet 0)")
endif()
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "relatch_pin: kein debug.log in ${WORKDIR}")
endif()

# --- (A) Waffen-Bank wird in JEDEM Boot neu gebunden ---
file(STRINGS "${WORKDIR}/debug.log" boot_lines REGEX "BOOT_EXIT_AT: boot #")
if(NOT boot_lines)
    message(FATAL_ERROR "relatch_pin: der Tod->Titel->Boot-Zyklus lief nicht (kein BOOT_EXIT_AT im Log)")
endif()

file(STRINGS "${WORKDIR}/debug.log" bank_lines REGEX "\\[equip\\] W-bank -> ")
list(LENGTH bank_lines n_bank)
if(n_bank LESS 2)
    message(FATAL_ERROR "relatch_pin: nur ${n_bank} W-Bank-Bindung(en) ueber 2 Boots — ein "
                        "prozess-langlebiges Latch verhindert das Re-Binding nach Tod->Titel. "
                        "Original bindet unbedingt neu: FUN_800314b0 jal FUN_80036b68 @0x800316f0, "
                        "Bank-Pointer-Writes @0x80036be4/@0x80036c04. Zeilen: '${bank_lines}'")
endif()
foreach(line IN LISTS bank_lines)
    if(NOT line MATCHES "W03 \\(recoil clip7 fc=23\\)")
        message(FATAL_ERROR "relatch_pin: falsche Bank gebunden (RE15_EQUIP=3 = BROWNING HP -> W03, "
                            "Recoil-Clip 7 fc=23; W01/Messer waere fc=25): '${line}'")
    endif()
endforeach()

# --- (B) Player-Select-TIM-Re-Upload laeuft pro Entry (Quell-Pin) ---
if(RE15_PC_MAIN AND EXISTS "${RE15_PC_MAIN}")
    file(READ "${RE15_PC_MAIN}" main_src)
    string(FIND "${main_src}" "PSELECT TIM RE-UPLOAD" up_pos)
    if(up_pos EQUAL -1)
        message(FATAL_ERROR "relatch_pin: der Player-Select-TIM-Re-Upload pro Entry fehlt in "
                            "platform/pc/main.c (Original: jal 0x80022150 @0x80101870 laeuft bei "
                            "JEDEM Select-Entry; die In-Game-Effekt-Sheets ueberschreiben Slot 20-23).")
    endif()
    string(FIND "${main_src}" "s_models_loaded = 1" latch_pos)
    if(latch_pos EQUAL -1)
        message(STATUS "relatch_pin: kein s_models_loaded-Latch mehr (voller Per-Entry-Reload) — ok")
    elseif(up_pos LESS latch_pos)
        message(FATAL_ERROR "relatch_pin: der TIM-Re-Upload steht VOR/INNERHALB des "
                            "s_models_loaded-Einmal-Latches — er muss pro Select-Entry laufen "
                            "(@0x80101870).")
    endif()
else()
    message(WARNING "relatch_pin: RE15_PC_MAIN nicht gesetzt — Teil (B) uebersprungen")
endif()

message(STATUS "relatch_pin OK: ${n_bank} W-Bank-Bindungen ueber 2 Boots, Select-TIM-Re-Upload pro Entry")
