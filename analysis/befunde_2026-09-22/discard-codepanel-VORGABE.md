# Wegwerf-Abfrage bei Code-Panels — Vorgabe

Alle `@0x…` = Datei-Byte-Offset in `re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT`.
In diesem Lauf an den Bytes nachgemessen (52 Offsets, alle OK).

## 1. Die 16 Stellen

| # | Raum / Sub | Gegenstand | Panel | Bedingung vor der Vormerkung |
|---|---|---|---|---|
| 1 | 1090 sub03 `msg 9` @0x02502 | Extinguisher 0x31 | nein | **unverändert** |
| 2 | **10D0 sub20** `msg 9` @0x019C4 | Blue Card 0x38 | **4 Ziffern** | **Auslöser → `msg 5` @0x0199E, Gate `flag(3,50)==1`** (`Set` @0x0152A) |
| 3 | **10D1 sub20** `msg 9` @0x019AE | Blue Card 0x38 | **4 Ziffern** | **→ `msg 5` @0x01988, `flag(3,50)==1`** (`Set` @0x01514) |
| 4 | 1100 sub02 `msg 4` @0x00C90 | Minidisc 0x44 | Cursor | **unverändert** (`Set(4,232)` @0x00C68 liegt davor) |
| 5 | 1101 sub02 `msg 4` @0x00C90 | Minidisc 0x44 | Cursor | **unverändert** |
| 6 | 11E0 sub20 `msg 9` @0x01FAC | Yellow Card 0x39 | 4 Ziffern | ⛔ **streichen/tot** — `flag(3,139)` steht ab Spielstart (ROOM1240 @0x0055A) |
| 7 | 11E0 sub21 `msg 12` @0x01FEE | Pliers 0x30 | Cursor | **unverändert** (`Set(4,36)` @0x01FC6 davor) |
| 8 | 11E1 sub20 `msg 9` @0x01FAC | Yellow Card 0x39 | 4 Ziffern | ⛔ **streichen/tot** |
| 9 | 11E1 sub21 `msg 12` @0x01FEE | Pliers 0x30 | Cursor | **unverändert** |
| 10 | **1230 sub20** `msg 9` @0x0148C | Red Card 0x37 | **4 Ziffern** | **→ `msg 5` @0x01466, `flag(3,137)==1`** (`Set` @0x00FF2) |
| 11 | **1231 sub20** `msg 9` @0x0148C | Red Card 0x37 | **4 Ziffern** | **→ `msg 5` @0x01466, `flag(3,137)==1`** (`Set` @0x00FF2) |
| 12 | 3010 sub02 `msg 1` @0x02306 | Green Card 0x36 | nein | **unverändert** (`Set` @0x02302) |
| 13 | 3011 sub02 `msg 1` @0x0233A | Green Card 0x36 | nein | **unverändert** (`Set` @0x02336) |
| 14 | 3050 sub15 `msg 5` @0x02608 | Red Master 0x46 | Cursor | **unverändert** |
| 15 | 3051 sub15 `msg 5` @0x02608 | Red Master 0x46 | Cursor | **unverändert** |
| 16 | 4000 sub02 `msg 2` @0x0144A | Blue Master 0x47 | nein | **unverändert** (`Set` @0x01450) |

4 ändern, 2 tot, 10 unverändert.

## 2. Der Satz für den Code

Die Stellen-Tabelle bekommt zwei Bytes `{gate_zone, gate_bit}` (0/0 = kein Gate).
Beim Tabellen-Treffer in `re15_discard_notice_message(room,msg,…)`:
`if (s->gate_zone && !re15_game_flag_get(s->gate_zone, s->gate_bit)) return 0;`
— **vor** der Besitzprüfung. Begründung: das Gate entscheidet, ob (Raum,msg) überhaupt
eine Wegwerf-Stelle **ist**; die Besitzprüfung bleibt unverändert an der RE2-Kettenposition
(@0x80051628 / @0x80051634 vor @0x8005164C). Fail-closed: kein Flag → keine Abfrage.
Kein `if (room == 0x10D0)` — Gate im Generator ableiten: Panel = Roh-Muster
`21 05 0d 01 21 05 0e 01 21 05 0f 01 21 05 10 01` (genau 6 Räume); Gate-Bit = das
`Ck` unmittelbar davor; Auslöser-msg = erstes `Message_on` im Sub des folgenden `Evt_exec`.
`Evt_exec` startet einen Thread und gibt nicht ab (`op_evt_exec`, byte-true @0x8003f2b8) —
das `Set` läuft vor `Message_on 5`.

## 3. Riegel

Je Panel-Raum zwei Fälle, plus Kollisions- und Nicht-Regressions-Riegel:
1. `flag(3,bit)=0`, `msg 9` mit Karte im Inventar und JA → **keine** Abfrage.
2. `flag(3,bit)=1`, `msg 5` → **genau eine** Abfrage; danach nimmt main00 (@0x01026 /
   @0x00B98) den Else-Zweig, `msg 5` ist unerreichbar → kein Zähler nötig.
3. **Kollisions-Riegel im Generator** (fing Bit 139): genau **ein** `Set(zone,bit,1)`,
   **null** löschende und null indizierte (0x59) Schreiber im Zensus über alle RDT,
   sonst Zeile verwerfen.
4. Gegenprobe: die 10 unveränderten Stellen feuern weiter wie bisher.

## 4. Widersprüche

* **6 vs. 4 Gate-Stellen** → **4** trägt. Roh-Byte-Zensus über 240 RDT: `22 03 8b 01`
  = **3** Treffer (11E0/11E1 @0x01A8A **+ ROOM1240 @0x0055A**, Neuspiel-Startraum).
  `22 03 32 01`/`22 03 89 01` je 2, Löschmuster je **0**.
* **„1100 hat ein Zahlenschloss"** → falsch, Cursor-Panel; das Muster liegt in 10D0,
  10D1, 11E0, 11E1, 1230, 1231 — sonst nirgends.
* **Codes 3201/9402** → Formel widerlegt (liefert für 1230 4521 statt gedruckt 5632).
  Bonus, nicht tragend: streichen.
* **RE2-Panel-Zensus** → mit RE1.5-Offsets/-Längen gemessen (0x51 ist in RE2 BGM,
  nicht Sce_key_ck): Zahlen streichen, Aufschließ-Routine @0x80051808–@0x80051824 bleibt.

## 5. Offen

1. **2 Stellen** (11E0/11E1 `msg 9`) im Prüfstand nicht erreichbar — Nutzer-Entscheidung.
2. **0 Laufzeit-Nachweise**; Headless stirbt unter RDP (`WIN_AddDisplay`).
3. **1 Falle:** `flag(3,129)` wird in ROOM10B1 @0x01954 gelöscht — nie als Marker nehmen.
4. **10 weitere Rätsel-Panels** ungeprüft (keine Wegwerf-Stellen).
