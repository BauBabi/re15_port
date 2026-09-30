# Spur A — Rolltor ROOM1050: Sicherung einsetzen mit Nahansicht, Tor erst danach

Stufe: ERMITTLUNG + BAUPLAN (noch kein Port-Code). Arbeitsbaum `.claude/worktrees/r34n_rolltor`, Zweig `r34n/rolltor`.

Status: IN ARBEIT — Abschnitte 1 und 3 (Teil) stehen, Rest folgt.

## 0 Kurzfassung

(folgt)

## 1 Nutzerwortlaut + Lesart

Woertlich (AUFTRAG.md, erster Punkt):

> Ich möchte das das Tor in ROOM 1050 nicht mehr einfach so geöffnet werden kann, sondern die CUT und
> der Text mit der Sicherung dort aktiviert werden soll. Also das Tor soll erst geöffnet werden können,
> wenn die Sicherung eingesetzt werden. Für die Sicherung und Nachricht soll es eine Nahansicht geben.

Lesart, Satz fuer Satz:

1. **"nicht mehr einfach so geoeffnet … sondern die CUT und der Text mit der Sicherung dort aktiviert"** —
   "die CUT" = die im Auslieferungsstand unerreichbare Nahansicht des Sicherungskastens (Cut 7 leer /
   Cut 8 eingesetzt, Kameratabelle @0x140/@0x160, §3.1), "der Text mit der Sicherung" = ROOM1050 msg 2
   "I need a fuse to run the shutter." (@0x0ED2, nie aufgerufen, §3.1). "statt einfach so geoeffnet" =
   an der Stelle, an der heute das Tor aufgeht (sub02 nach dem Ja), kommen Nahansicht + Text.
2. **"erst geoeffnet werden koennen, wenn die Sicherung eingesetzt"** — Zustand "eingesetzt" ist
   persistent (Bank-9-Bit 63, VERTRAG §1.1). Erst danach verhaelt sich der Schalter wie ausgeliefert.
3. **"Fuer die Sicherung und Nachricht soll es eine Nahansicht geben"** — ZWEI Nahansichten: (a) zur
   Nachricht (ohne Sicherung: Cut 7 + msg 2), (b) zum Einsetzen (Cut 7 -> Cut 8).

Mehrdeutigkeiten, aufgeloest:

* **Reihenfolge Schalterfrage vs. Nahansicht.** Gewaehlt: die ausgelieferte Schalterfrage bleibt ZUERST
  ("It's a shutter switch. / Will you push it?", msg 0), das Sperren sitzt HINTER dem Ja. Gruende:
  (i) der Nutzer ersetzt woertlich das "geoeffnet werden", nicht das Fragen; (ii) msg 2 sagt "to RUN the
  shutter" = Antwort auf einen Bedienversuch; (iii) RE1.5 hat genau dieses Muster VOLLSTAENDIG im
  Schwester-Raetsel ROOM2060: Generator-Frage msg 0 -> Ja -> `Ck(3,144,0)` -> msg 1 "I need to insert the
  missing fuse before I can operate this." (sub12 @0x015BA..@0x015D6, §3.3). Beleg statt Geschmack.
* **Wo die Sicherung eingesetzt wird.** Kasten und Schalter sind DIESELBE Stelle (§3.2): AOT 7 steht
  direkt vor dem Wandschrank, den Cut 3 klein und Cut 7/8 gross zeigen. Also kein zweiter Ort, keine
  zweite Zone — die Einsetz-Abfrage folgt in derselben Nahansicht auf msg 2, wie in ROOM2060 sub18
  (msg 3 Zustandssatz -> msg 4 "Will you use the Fuse?").
* **"Nachricht" = msg 2**, nicht eine neue Zeile. Die Einsetz-Texte sind ROOM2060 msg 4 / msg 5 byte-genau
  (RE1.5-Originalsaetze desselben Raetsels), keine Neuformulierung.

(Abschnitt vorlaeufig — wird nach Messung und Abnahmeplan geschaerft.)

## 2 Ist-Zustand im Port (gemessen)

(folgt)

## 3 Original-/RE2-Mechanismus (Adressen, Bytes, Instruktionen)

### 3.1 ROOM1050 — was ausgeliefert ist (selbst nachgemessen)

SCD-Dump `scd_dump_room.py` (Werte unten Datei-Offsets ROOM1050.RDT):

| Was | Offset | Bytes | Bedeutung |
|---|---|---|---|
| Schalter-Zone | sub00 @0x0C22 | `2c 07 03 31 00 00 a0 41 0a dd 20 03 20 03 ff 00 18 02 00 00` | Aot_set slot 7 sce 3 sat 0x31, Rechteck x 16800..17600 z -8950..-8150, Nutzlast -> sub02 |
| nur solange zu | sub00 @0x0C1E | `21 03 79 00` | Ck(3,121,0) — Tor noch nicht offen |
| Frage | sub02 @0x0CAC | `2b 00 80 ff` | Message_on 0 "It's a shutter switch. / Will you push it?" |
| Ja? | sub02 @0x0CB6 | `21 0c 1f 00` | Ck(12,31,0) — Ja (No-Bit 0) |
| Tor offen | sub02 @0x0CBA | `22 03 79 01` | Set(3,121,1) |
| Schalter tot | sub02 @0x0CBE | `46 07 00 …` | Aot_reset slot 7 sce 0 |
| Fahrt-Blick | sub02 @0x0CD0 | `29 03` | Cut_chg 3 |
| Fahrgeraeusche | sub02 @0x0CD2 / @0x0CE2 / @0x0D3C | `36 02 0c …` / `36 02 0a …` / `36 02 0b …` | Se_on Bank 2 0x0C, 0x0A, 0x0B |
| Rueckgabe | sub02 @0x0D7C / @0x0D82 | `3c 01` / `42` | Cut_auto 1, Plc_ret |
| Nachricht | msg-Block @0xE44, msg 2 @0x0ED2 | — | "I need a fuse to run the shutter." — kein `2b 02` im SCD |
| Nahansicht | Kameratabelle @0x60, Cut 7 @0x140 / Cut 8 @0x160 | erste 28 Byte gleich, pri 0x518 / 0x51C | Kamera (16171,-2733,-8185) -> (18240,-1974,-8706) |

ROOM1051 (Elza-Variante): Schalter-Zone sub00 @0x0C4C, sub02 @0x0CC8..0x0DA4 bytegleich zu ROOM1050
sub02 (nur um +0x1C verschoben), msg 2 @0x0E68 derselbe Satz, Kameratabelle identisch (alle 10 Cuts,
selbst verglichen). ABER: ROOM1051 main00 belegt Slot 11 (`2c 0b 03 31 …` @0x0C0E, Leiche -> sub03) und
Slot 12 (`50 0c 09 31 …` @0x0C22, geparkte SIG P228) — die im VERTRAG fuer Spur A vorgesehenen Slots
11/12 sind in ROOM1051 ORIGINAL belegt (§7).

### 3.2 Kasten und Schalter sind derselbe Ort

(in Arbeit — Hintergrundbilder Cut 3 / Cut 7 / Cut 8, RVD-Zuordnung)

### 3.3 RE1.5-Vorbild: das vollstaendige Sicherungs-Raetsel ROOM2060 (STAGE2)

(in Arbeit)

### 3.4 Gegenstand aus dem Inventar — RE1.5 kann es nicht, RE2 kann es

(in Arbeit)

## 4 Soll-Verhalten (Zeitlinie)

(folgt)

## 5 Bauplan

(folgt)

### 5.1 Konstanten-Tabelle

| Konstante | Wert | Beleg (@0x… / Datei-Offset) bzw. NUTZER-VORGABE / PORT-WAHL + Grund |
|---|---|---|

## 6 Abnahmeplan

(folgt)

## 7 Risiken, Softlocks, Wechselwirkungen

(folgt)

## 8 Offene Punkte

(folgt)
