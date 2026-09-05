# =============================================================================
# SAVE-ZAEHLER-PIN (Nutzer-Report 2026-08-17: "Beim Laden eines Spielstandes und
# Speichern auf einen NEUEN Slot schreibt er wieder die gleiche Nummer, statt
# hochzuzaehlen.")
#
# BEFUND: das gemeldete Verhalten WAR der ORIGINAL-Mechanismus (kein Port-Fehler).
# ENTSCHEIDUNG des Nutzers 2026-08-17: der Port weicht hier BEWUSST ab und zaehlt
# monoton hoch — auch der erste Save nach einem Load steigt um 1. Umgesetzt an EINER
# Stelle (main.c, LOAD-Restore: geladener Stand + 1); alles andere bleibt byte-true.
# Dieser Pin prueft das NEUE Sollverhalten; die Original-Adressen stehen hier weiter
# vollstaendig, damit die Abweichung spaeter nicht fuer einen Bug gehalten wird.
#
# ORIGINAL — der Save-Zaehler ist LIVE-SPIELZUSTAND (DAT_800b0fbd), nicht
# Karten-/Slot-Zustand:
#
#   [1] FRISCHES SPIEL = 0
#       PSX.EXE t_addr/t_size 0x80010000/0xaf000 (Header @0x18) -> 0x800b0fbd liegt
#       bei Datei-Offset 0xa17bd, Byte dort = 00 (xxd-verifiziert). Kein weiterer
#       Initialisierer: Ghidra-Xref-Liste (ghidra1_V2.txt:492892) kennt fuer
#       DAT_800b0fbd GENAU 4 Referenzen (s.u.), kein Overlay referenziert die
#       Adresse (grep ueber RE_15_Quellcode_Overlays: leer).
#
#   [2] SAVE schreibt den Zaehler PRE-INKREMENT
#       FUN_80025c00 case 8:
#         @0x800261b4  jal 0x80026f48    ; GSB-Kopf auffrischen — schreibt NUR
#                                        ; 0x800b0fbe/fc0/fc2/fc4 (@0x80026f6c-84),
#                                        ; NICHT den Zaehler
#         @0x800261f0  jal 0x80026e54    ; Kartentitel: Ziffern aus DAT_800b0fbd
#                                        ; (@0x80026eac lbu, @0x80026ecc lbu)
#         @0x80026224  jal 0x800271a8    ; KARTEN-WRITE, Quelle a0 = 0x800b0dbc
#                                        ; (@0x800261ec addiu s0,t0,-512 mit
#                                        ; t0=0x800b0fbc), 0x800 Bytes = LIVE-RAM.
#                                        ; Der Zaehler liegt bei Block+0x201.
#                                        ; FUN_800271a8 ist SYNCHRON (open/write/
#                                        ; close + Verify-Read) — kein Async-Fenster.
#
#   [3] INKREMENT erst NACH erfolgreichem Write
#         @0x80026230  beq v0,zero,0x80026480   ; v0==0 = Erfolg
#         @0x8002648c  lbu v0,0x800b0fbd
#         @0x80026494  addiu v0,v0,0x1
#         @0x8002649c  sb  v0,0x800b0fbd
#
#   [4] LOAD stellt den Zaehler WHOLESALE aus dem Save wieder her
#       FUN_80025c00 case 9: @0x80026290-a0 memcpy(&DAT_800b0dbc, buf, 0x1430) —
#       0x800b0dbc+0x201 = 0x800b0fbd. Der Load-Pfad springt an [3] VORBEI
#       (@0x800262a4 j 0x800264a0), inkrementiert also NICHT.
#
#   [5] ANZEIGE = der rohe Zaehler (kein +1)
#       Kartentitel @0x80026ec8 (Zehner, dst+0x25) / @0x80026f00 (Einer, dst+0x27);
#       In-Game-Slot-Zeile FUN_80026658 @0x80026834 `lbu t0,1(s0)` + divu 10
#       @0x8002683c-64 — s0 zeigt auf die GSB-Kopie, +1 = DAT_800b0fbd.
#
#   => ORIGINAL: Laden(N) -> Speichern schreibt N ERNEUT (der Load hat den Zaehler auf
#      den im Save gespeicherten Pre-Inkrement-Stand zurueckgesetzt); erst der DARAUF
#      folgende Save schreibt N+1. Der Ziel-Slot geht in die Nummer NIE ein.
#   => PORT (Entscheid 2026-08-17): Laden(N) -> Speichern schreibt N+1. Der Ziel-Slot
#      geht weiterhin NICHT ein; alles andere (Start bei 0, Store PRE-Inkrement,
#      Post-Inkrement nach Erfolg, Ziffernbau) bleibt byte-true.
#
# SOLL-SEQUENZ (gemessen), drei exe-Laeufe auf EINER Karte:
#   Lauf 1  NEUES SPIEL, zwei Saves   -> Slot 0 = 0, Slot 1 = 1
#   Lauf 2  CONTINUE aus Slot 1 (=1)  -> Slot 2 = 2, Slot 3 = 3
#   Lauf 3  CONTINUE aus Slot 3 (=3)  -> Slot 4 = 4      (mehrfache Lade-Zyklen)
#
# Er DISKRIMINIERT (gemessene Gegenproben, siehe analysis/save_counter.md §5):
#   * Abweichung zurueckgebaut (`+ 0` statt `+ 1`)      -> Lauf 2 '1;2' statt '2;3' ROT
#   * fehlendes Post-Inkrement (main.c s_save_counter)  -> Lauf 1 '0;0' statt '0;1' ROT
#   * fehlender LOAD-Restore (s_save_counter bleibt 0)  -> Lauf 2 '0;1' statt '2;3' ROT
#   * Post-Inkrement-Store (`capture(scount+1)`)        -> Slot 0 = 1 statt 0        ROT
#   * Seed aus dem Karten-Maximum (der alte, erfundene re15_memcard_max_save_count-Pfad)
#
# Harness (reine Testhaken in main.c, kein Spielverhalten):
#   RE15_CARD_AUTO + RE15_CARD_SLOT="a,b,c"  Auto-Drive + Ziel-Slot je Screen
#   RE15_SAVE_TEST / RE15_SAVE_TEST_AGAIN=<f>  zwei Saves in EINER Sitzung
#   RE15_TITLE_SHOT (NEW GAME) bzw. RE15_CONTINUE_TEST (LOAD GAME)
#   RE15_SAVE_TEST_EXIT_AFTER=<n>              definiertes Prozess-Ende nach n Saves
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_save_counter_pin.cmake
# =============================================================================

if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "save_counter_pin: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "save_counter_pin: WORKDIR fehlt")
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
file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")

# --- Lauf 1: NEUES SPIEL -> Save in Slot 0, dann Save in den NEUEN Slot 1 ------
re15_spiel_lauf(rv1 "save_counter_pin Lauf 1"
    RE15_NO_INTRO=1
    RE15_NOAUDIO=1
    RE15_TITLE_SHOT=t.bmp        # Title-Auto-Advance -> NEW GAME (Zaehler := 0)
    RE15_SAVE_TEST=1             # Save #1 beim Boot
    RE15_SAVE_TEST_AGAIN=20      # Save #2 bei Frame 20
    RE15_CARD_AUTO=1
    RE15_CARD_SLOT=0,1           # Save #1 -> Slot 0, Save #2 -> NEUER Slot 1
    RE15_SAVE_TEST_EXIT_AFTER=2  # Prozessende direkt nach Save #2 (deterministisch)
    "${RE15_PC_EXE}")
if(NOT rv1 EQUAL 0)
    message(FATAL_ERROR "save_counter_pin: Lauf 1 exit=${rv1} (erwartet 0)")
endif()
if(NOT EXISTS "${WORKDIR}/re15_card.mcr")
    message(FATAL_ERROR "save_counter_pin: Lauf 1 hat keine Karte geschrieben")
endif()

# Kanal A: die Prozess-eigene Protokollzeile "[save] saved (room ....) slot n=<N> -> next=<M>"
# (main.c) — n = die GESCHRIEBENE Nummer. Unabhaengig von den Karten-Bytes.
#
# ACHTUNG: debug.log enthaelt auch BINAER-Bytes; `file(STRINGS)` verschluckt dann stumm
# alles danach (gemessen: die "CONTINUE"-Zeile wurde gefunden, die beiden spaeteren
# "[save] saved"-Zeilen NICHT, obwohl sie in der Datei stehen). Darum wird das Log
# binaer-sicher als HEX gelesen und im Hex-Strom gesucht.
#   "slot n=" = 736c6f74206e3d, danach ASCII-Ziffern 3X.
function(_log_hex out)
    file(READ "${WORKDIR}/debug.log" lh HEX)
    set(${out} "${lh}" PARENT_SCOPE)
endfunction()
function(_saved_numbers out)
    _log_hex(lh)
    string(REGEX MATCHALL "736c6f74206e3d(3[0-9])+" hits "${lh}")
    set(nums "")
    foreach(h IN LISTS hits)
        string(SUBSTRING "${h}" 14 -1 digits)      # 7 Bytes Praefix "slot n="
        set(v "")
        string(LENGTH "${digits}" dl)
        math(EXPR last "${dl} - 2")
        foreach(i RANGE 0 ${last} 2)
            string(SUBSTRING "${digits}" ${i} 2 pair)
            string(SUBSTRING "${pair}" 1 1 d)      # "3X" -> X
            string(APPEND v "${d}")
        endforeach()
        list(APPEND nums "${v}")
    endforeach()
    set(${out} "${nums}" PARENT_SCOPE)
endfunction()

_saved_numbers(run1_nums)
if(NOT run1_nums STREQUAL "0;1")
    message(FATAL_ERROR "save_counter_pin: Lauf 1 (NEUES SPIEL, zwei Saves) schrieb die Nummern "
                        "'${run1_nums}', erwartet '0;1'. Original: frisches Spiel = 0 (PSX.EXE "
                        "Datei-Offset 0xa17bd = 00), Store PRE-Inkrement (Write @0x80026224), "
                        "Inkrement nach Erfolg @0x8002648c-9c.")
endif()

# --- Lauf 2: CONTINUE aus Slot 1 -> Save in NEUEN Slot 2, dann in NEUEN Slot 3 -
file(REMOVE "${WORKDIR}/debug.log")
re15_spiel_lauf(rv2 "save_counter_pin Lauf 2"
    RE15_NO_INTRO=1
    RE15_NOAUDIO=1
    RE15_CONTINUE_TEST=1         # Title-Auto-Advance -> LOAD GAME
    RE15_SAVE_TEST=1
    RE15_SAVE_TEST_AGAIN=20
    RE15_CARD_AUTO=1
    RE15_CARD_SLOT=1,2,3         # laden aus Slot 1, speichern in 2 und 3
    RE15_SAVE_TEST_EXIT_AFTER=2  # Prozessende direkt nach Save #2 (deterministisch)
    "${RE15_PC_EXE}")
if(NOT rv2 EQUAL 0)
    message(FATAL_ERROR "save_counter_pin: Lauf 2 exit=${rv2} (erwartet 0)")
endif()
_log_hex(log2hex)                                  # "CONTINUE: resumed" = 434f4e54494e55453a20726573756d6564
string(FIND "${log2hex}" "434f4e54494e55453a20726573756d6564" resumed)
if(resumed EQUAL -1)
    message(FATAL_ERROR "save_counter_pin: Lauf 2 hat den Spielstand nicht geladen "
                        "(kein '[save] CONTINUE: resumed' im debug.log)")
endif()

# Kanal A fuer Lauf 2 — GENAU DIE ABWEICHUNG (Nutzer-Entscheid 2026-08-17): nach dem
# Laden von Slot 1 (Nummer 1) traegt der naechste Save 2, nicht nochmal 1.
_saved_numbers(run2_nums)
if(NOT run2_nums STREQUAL "2;3")
    message(FATAL_ERROR "save_counter_pin: Lauf 2 (CONTINUE aus Slot 1 = Nummer 1, zwei Saves) "
                        "schrieb die Nummern '${run2_nums}', erwartet '2;3'. '1;2' ist das "
                        "ORIGINAL-Verhalten (LOAD restauriert den Stand wholesale @0x80026290-a0 "
                        "und springt am Post-Inkrement vorbei @0x800262a4) — der Port weicht hier "
                        "BEWUSST ab (main.c, Nutzer-Entscheid 2026-08-17: geladener Stand + 1). "
                        "'0;1' hiesse: der LOAD restauriert gar nichts; '3;4' hiesse: die Nummer "
                        "kommt aus der Karte — dort wird sie NIE gelesen (Write-Quelle ist "
                        "Live-RAM 0x800b0dbc, @0x800261ec/@0x80026224).")
endif()

# --- Lauf 3: CONTINUE aus Slot 3 (Nummer 3) -> Save in NEUEN Slot 4 ------------
#     Pinnt die Kette Laden->Speichern->Laden->Speichern: die Zaehlung laeuft ueber
#     MEHRERE Lade-Vorgaenge durchgehend weiter (3 -> 4), bleibt also nicht stehen.
file(REMOVE "${WORKDIR}/debug.log")
re15_spiel_lauf(rv3 "save_counter_pin Lauf 3"
    RE15_NO_INTRO=1
    RE15_NOAUDIO=1
    RE15_CONTINUE_TEST=1
    RE15_SAVE_TEST=1
    RE15_CARD_AUTO=1
    RE15_CARD_SLOT=3,4           # laden aus Slot 3, speichern in NEUEN Slot 4
    RE15_SAVE_TEST_EXIT_AFTER=1
    "${RE15_PC_EXE}")
if(NOT rv3 EQUAL 0)
    message(FATAL_ERROR "save_counter_pin: Lauf 3 exit=${rv3} (erwartet 0)")
endif()
_saved_numbers(run3_nums)
if(NOT run3_nums STREQUAL "4")
    message(FATAL_ERROR "save_counter_pin: Lauf 3 (CONTINUE aus Slot 3 = Nummer 3, ein Save) "
                        "schrieb die Nummern '${run3_nums}', erwartet '4' — die Zaehlung muss "
                        "ueber MEHRERE Laden->Speichern-Zyklen durchgehend hochlaufen.")
endif()

# --- Karten-Bytes lesen -------------------------------------------------------
# .mcr-Layout (re15_memcard.c): Slot s -> Verzeichnis-Frame (s+1)*0x80 (Zustand
# 0x51 = belegt) und Datenblock (s+1)*0x2000. Im Block: BIOS-Titel ab +0x04 (die
# Zaehler-Ziffern des Originals liegen bei Titel+0x25/+0x27 -> Block +0x29/+0x2b,
# @0x80026ec8/@0x80026f00), Save-Blob ab +0x100 mit save_count (u16 LE) bei
# Blob+26 -> Block +0x11a.
# Erneut lesen, bis alle vier Slots belegt sind: nach dem Prozess-Ende war der eben
# ueberschriebene 128-KB-Kartenblob auf dieser Maschine gelegentlich noch mit dem
# Stand von Lauf 1 sichtbar (Windows-Dateisicht-Verzoegerung, kein Port-Verhalten).
# Kanal A oben hat die Nummern zu diesem Zeitpunkt bereits geprueft.
set(card "")
foreach(attempt RANGE 20)
    file(READ "${WORKDIR}/re15_card.mcr" card HEX)
    set(all_used TRUE)
    foreach(s 0 1 2 3 4)
        math(EXPR p "((${s}+1)*128)*2")
        string(SUBSTRING "${card}" ${p} 2 b)
        if(NOT b STREQUAL "51")
            set(all_used FALSE)
        endif()
    endforeach()
    if(all_used)
        break()
    endif()
    execute_process(COMMAND "${CMAKE_COMMAND}" -E sleep 0.5)
endforeach()

function(_slot_used slot out)
    math(EXPR p "((${slot}+1)*128)*2")
    string(SUBSTRING "${card}" ${p} 2 b)
    set(${out} "${b}" PARENT_SCOPE)
endfunction()
function(_slot_count slot out)
    math(EXPR p "((${slot}+1)*8192 + 282)*2")
    string(SUBSTRING "${card}" ${p} 4 w)          # u16 little-endian
    string(SUBSTRING "${w}" 0 2 lo)
    string(SUBSTRING "${w}" 2 2 hi)
    if(NOT hi STREQUAL "00")
        message(FATAL_ERROR "save_counter_pin: Slot ${slot} save_count high byte = ${hi} (erwartet 00)")
    endif()
    math(EXPR v "0x${lo}")
    set(${out} "${v}" PARENT_SCOPE)
endfunction()
function(_slot_digits slot out)                    # "<zehner> <einer>" als Dezimalziffern
    math(EXPR pt "((${slot}+1)*8192 + 41)*2")
    math(EXPR po "((${slot}+1)*8192 + 43)*2")
    string(SUBSTRING "${card}" ${pt} 2 t)
    string(SUBSTRING "${card}" ${po} 2 o)
    # Basis = Low-Byte der Vollbreiten-'0' (SJIS 82 4F); die Ziffer wird daraufaddiert.
    math(EXPR td "0x${t} - 79")
    math(EXPR od "0x${o} - 79")
    set(${out} "${td} ${od}" PARENT_SCOPE)
endfunction()

foreach(s 0 1 2 3 4)
    _slot_used(${s} u)
    if(NOT u STREQUAL "51")
        message(FATAL_ERROR "save_counter_pin: Slot ${s} ist nicht belegt (Dir-Zustand 0x${u}) — "
                            "die Auto-Drive-Sequenz hat nicht in alle fuenf Slots gespeichert")
    endif()
endforeach()

_slot_count(0 c0)
_slot_count(1 c1)
_slot_count(2 c2)
_slot_count(3 c3)
_slot_count(4 c4)

# [1] NEUES SPIEL beginnt bei 0 (EXE-Image-Byte @Datei 0xa17bd = 00) und der Save
#     schreibt PRE-Inkrement (@0x800261c4-d8 / @0x80026eac-f0 laufen VOR dem Write).
if(NOT c0 EQUAL 0)
    message(FATAL_ERROR "save_counter_pin: erster Save eines NEUEN Spiels traegt Nummer ${c0}, "
                        "erwartet 0. Original: frisches Spiel = 0 (PSX.EXE Datei-Offset 0xa17bd = 00), "
                        "und der Save speichert den Zaehler PRE-Inkrement (Write @0x80026224, "
                        "Inkrement erst @0x8002648c-9c).")
endif()
# [3] Zweiter Save DERSELBEN Sitzung zaehlt hoch (Post-Inkrement @0x80026488-9c).
if(NOT c1 EQUAL 1)
    message(FATAL_ERROR "save_counter_pin: zweiter Save derselben Sitzung traegt Nummer ${c1}, "
                        "erwartet 1. Original inkrementiert DAT_800b0fbd nach jedem ERFOLGREICHEN "
                        "Write: @0x8002648c lbu / @0x80026494 addiu 1 / @0x8002649c sb.")
endif()
# [4] ABWEICHUNG: nach LOAD von Slot 1 (Nummer 1) traegt der naechste Save 2, nicht 1.
if(NOT c2 EQUAL 2)
    message(FATAL_ERROR "save_counter_pin: erster Save NACH dem Laden von Slot 1 traegt Nummer ${c2}, "
                        "erwartet 2 (geladener Stand 1 + 1, Nutzer-Entscheid 2026-08-17). 1 ist das "
                        "ORIGINAL-Verhalten (LOAD restauriert wholesale @0x80026290-a0, Post-Inkrement "
                        "uebersprungen @0x800262a4); 0 hiesse: der LOAD restauriert gar nichts.")
endif()
# und danach laeuft die Zaehlung weiter (Post-Inkrement @0x8002649c, unveraendert byte-true).
if(NOT c3 EQUAL 3)
    message(FATAL_ERROR "save_counter_pin: zweiter Save nach dem Laden traegt Nummer ${c3}, erwartet 3 "
                        "(Post-Inkrement @0x8002649c).")
endif()
# [5] ueber MEHRERE Lade-Zyklen laeuft die Zaehlung durch: Slot 3 (Nummer 3) geladen -> 4.
if(NOT c4 EQUAL 4)
    message(FATAL_ERROR "save_counter_pin: Save nach dem ZWEITEN Laden (Slot 3 = Nummer 3) traegt "
                        "Nummer ${c4}, erwartet 4 — die Zaehlung muss ueber Laden->Speichern->Laden->"
                        "Speichern durchgehend hochlaufen.")
endif()

# [6] Die Kartentitel-Ziffern folgen dem geschriebenen Wert (Slot 4 -> "0 4"). Der
#     Ziffern-Bau selbst bleibt byte-true (kein +1 in re15_mc_title.c).
_slot_digits(4 d4)
if(NOT d4 STREQUAL "0 4")
    message(FATAL_ERROR "save_counter_pin: Kartentitel-Ziffern von Slot 4 = '${d4}', erwartet '0 4' "
                        "(Zehner @0x80026ec8 dst+0x25, Einer @0x80026f00 dst+0x27, Basis = "
                        "Vollbreiten-'0' 0x824f).")
endif()

message(STATUS "save_counter_pin OK: Slots 0..4 tragen ${c0}/${c1}/${c2}/${c3}/${c4} "
               "(neues Spiel=0, Sitzung zaehlt hoch, jeder LOAD setzt auf geladener Stand + 1 fort "
               "— PORT-ABWEICHUNG Nutzer-Entscheid 2026-08-17)")
