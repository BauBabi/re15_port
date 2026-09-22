# Wegwerf-Abfrage: welche der 16 Benutzungsstellen sind Tore mit Code-Panel?

Untersuchung A, 2026-09-22. **Dieser Lauf hat KEINEN Code geändert** — `item_discard_common.c`
und `tests/unit/r21_discard_wegwerfen.c` sind unberührt. Ergebnis ist eine umsetzungsreife
Bedingung je Stelle plus die Messwerkzeuge.

## Nutzer-Auftrag (wörtlich)

> "Nein, ich will die Abfrage schon haben. Das Problem das ich Bei dir sah, war das du die
> discard Abfrage auch bei Toren mit Rätsel panels machst, also wo man einen Code eingeben
> muss. Da ist das natürlich erst dann korrekt, wenn man den zugriffscode den man braucht
> einmalig richtig eingegeben hat. erst dann darf die Abfrage kommen - you don't need this
> item anymore...."

## Antwort in einem Satz

**6 der 16 Stellen sind Tore mit echter Code-Eingabe** — die sechs Kartenleser
ROOM10D0/10D1/11E0/11E1/1230/1231. Bei allen sechs fällt die Nachricht
`"You've used the <Karte>."` **bevor** eine Ziffer eingegeben ist; die Abfrage ist dort
heute also nachweislich zu früh. Die anderen 10 Stellen sind **keine** Code-Tore: bei
ihnen ist der Erfolg zum Zeitpunkt der Nachricht bereits eingetreten, die Abfrage ist dort
schon korrekt.

## Sollzahl und Abdeckung

| Messung | Zahl |
|---|---|
| Benutzungsstellen laut `discard_sites.inc` (Commit `47766cb4`) | **16** |
| davon im ausgelieferten SCD mit ausführendem `Message_on` gefunden | **16 / 16** |
| RDTs mit Header durchgelaufen | **206** von 240 Dateien (34 Stummel < 0x48 B) |
| Desync-Stopps im Opcode-Walk | **0** |
| Räume mit echter Ziffern-Eingabe (ganzes Spiel) | **6** |
| Räume, die überhaupt Tasten lesen (`Sce_key_ck` 0x51 / `Sce_espr_control` 0x52) | **29** |
| `Sce_espr_control` (0x52) im ganzen Spiel | **0** — nur 0x51 kommt vor |

Die Sollzahl 6 ist nicht aus den 16 Stellen geraten, sondern über **drei unabhängige
Zensus-Läufe über alle 206 RDTs** gefallen (`codepanel_muster.py`, Log `muster_zensus.log`):

1. **Text**: `"Enter the first number."` — 6 Nachrichten in 6 Räumen:
   ROOM10D0, ROOM10D1, ROOM11E0, ROOM11E1, ROOM1230, ROOM1231.
   Identisch: `"Wrong code, try again."` (6) und `"insert four digits"` (6).
2. **Byte-Muster** über die **rohen Dateibytes** (nicht über den Opcode-Walk, damit kein
   Desync eine Stelle verschluckt):
   `21 05 0d 01 21 05 0e 01 21 05 0f 01 21 05 10 01`
   = `Ck(5,13,1) Ck(5,14,1) Ck(5,15,1) Ck(5,16,1)` = "alle vier Ziffern richtig".
   **6 rohe Treffer, alle 6 danach im Opcode-Walk als Opcode-Anfang bestätigt.**
3. **Eingabe**: 29 Räume lesen Tasten. Von den 16 Stellen liegen 10 in solchen Räumen,
   4 Stellen (ROOM1090, 3010, 3011, 4000) liegen in Räumen **ohne einen einzigen** 0x51/0x52
   — die können konstruktiv keine Eingabe verlangen.

Alle drei Messungen liefern dieselben sechs Räume. Und diese sechs sind genau die sechs
Kartenleser, die der Nutzer ausdrücklich genannt hat.

## Die Erkennungsregel — Mechanik, nicht Name

| Klasse | Signatur im SCD | Bedeutung |
|---|---|---|
| **CODE** | `Sce_key_ck(1, 0x0001/0x0002/0x0004/0x0008)` (Ziffern-Auswahl) + `Member_cmp(member=15)` je Zifferntaste + `Sce_key_ck(1,0x0040)` (SQUARE = Bestätigen) + die Vier-Ziffern-Schranke `Ck(5,13..16,1)` + Texte `"Enter the … number."`/`"Wrong code, try again."` | Es gibt einen **eingebbaren Zahlencode**. |
| **CURSOR** | `Sce_key_ck` für die vier Richtungen + **genau ein** `Member_cmp(member=15 == <Zelle>)` + `Sce_key_ck(1,0x0040)` → `Evt_exec` — aber **keine** Vier-Ziffern-Schranke, kein "Enter the … number." | **Zielen und Bestätigen**, kein Code. Der Gebrauch selbst ist die richtige Handlung. |
| **EINFACH** | kein 0x51/0x52 im ganzen Raum | reines Schlüsselschloss. |

`Member_cmp member=15` ist die Cursor-Zelle (`member_0b`, Clear-Wert 0 = "über keiner
Zelle", `@0x80043788`); Maske `0x0040` = virtuelles Bit 6 = SQUARE
(`re15_pad_virtual_word`, Preset-Tabelle `@0x80073dbc`; Prädikat `LAB_80042920`,
im Port `op_sce_key_ck`, `scd_vm.c:4664`).

## Das Code-Panel der sechs Kartenleser, vollständig disassembliert

Beispiel ROOM10D0 (`STAGE1/ROOM10D0.RDT`, 286320 B). Alle Offsets sind
**Datei-Byte-Offsets** in der RDT.

**1. main00 wählt das Raum-Layout am Schloss-Bit** — `@0x01022`:

```
0x01022  06 00 4e 01      Ifel_ck   (überspringe 334 B -> 0x1174)
0x01026  21 03 32 00      Ck(3,50,0)          ; Schloss noch NICHT offen
0x0102A  2c 00 01 31 …    Aot_set  slot 0     ; Kartenleser-Nachricht
0x0103E  2c 01 03 31 …    Aot_set  slot 1     ; Kartenleser-Aktion -> sub20
0x01052  3b 02 00 31 …    Door_aot_set slot 2 ; Tür, sce=0 = installiert aber INERT
0x01072  2c 03..0d 05 44  Aot_set  slot 3..13 ; die 11 Tastenfelder des Zahlenschlosses
0x0114E  2d 01 04 …       Obj_model_set       ; das Panel-Modell
0x01170  07 00 26 00      Else_ck  -> 0x1196
0x01174  3b 00 02 31 …    Door_aot_set slot 0 ; Schloss OFFEN: normale Tür, sce=2
```

**2. Der Kartenleser (sub20) — hier fällt die "used the"-Nachricht** — `0x019AE..0x019DE`:

```
0x019AE  2b 07 ff ff      Message_on 7   ; "A card reader. You have to use the Blue Keycard
                                         ;  and insert four digits to unlock the door.
                                         ;  Will you operate the card reader?"   (Ja/Nein, ctrl 0x03)
0x019B4  06 00 24 00      Ifel_ck  -> 0x19DC
0x019B8  21 0c 1f 00      Ck(12,31,0)    ; Antwort JA
0x019BC  06 00 12 00      Ifel_ck  -> 0x19D2
0x019C0  21 09 34 01      Ck(9,52,1)     ; Blaue Keycard GENOMMEN? (taken-Bit)
0x019C4  2b 09 ff ff      Message_on 9   ; "You've used the Blue Keycard."   <<< HEUTE die Abfrage
0x019CA  04 ff 18 11      Evt_exec sub17 ; JETZT ERST beginnt die Code-Eingabe
0x019CE  07 00 0c 00      Else_ck  -> 0x19DA
0x019D2  2b 08 ff ff      Message_on 8   ; "You have not the Blue Keycard to operate it."
0x019DC  01 00            Evt_end
```

**3. Die Code-Eingabe beginnt (sub17)** — `0x018D6`:

```
0x018D6  29 0b            Cut_chg 0x0B        ; Kamera auf das Tastenfeld
0x018D8  46 01 00 …       Aot_reset slot 1
0x018E2  2b 00 ff ff      Message_on 0        ; "Enter the first number."
0x018EC  22 05 00 01      Set(5,0,1)          ; Zifferntasten scharf
0x018F0  22 05 01 01      Set(5,1,1)          ; Cursor scharf
0x018F4  22 05 02 01      Set(5,2,1)
```

**4. Die Ziffern-Maschine (sub01)** — 15× `Sce_key_ck`, 11× `Member_cmp`:
4 Richtungstasten (`0x0001/0x0004/0x0002/0x0008`) → sub02..05 bewegen den Cursor;
11 Zellen `Member_cmp(15 == 3..13)` + SQUARE `0x0040` → sub06..16 = die 11 Tasten.
sub18 (`0x018FA`) schiebt die Eingabe weiter: "Enter the second/third/fourth number."
bzw. `Message_on 4` = `"Wrong code, try again."` `@0x01950`.

**5. Die Erfolgs-Schranke (sub01, Ende)** — `@0x0150E`. **Das ist die Bedingung, die der
Nutzer verlangt:**

```
0x0150E  06 00 1e 00      Ifel_ck   (Block 30 B)
0x01512  21 03 32 00      Ck(3,50,0)     ; Schloss noch NICHT offen
0x01516  21 05 0d 01      Ck(5,13,1)     ; Ziffer 1 richtig
0x0151A  21 05 0e 01      Ck(5,14,1)     ; Ziffer 2 richtig
0x0151E  21 05 0f 01      Ck(5,15,1)     ; Ziffer 3 richtig
0x01522  21 05 10 01      Ck(5,16,1)     ; Ziffer 4 richtig
0x01526  04 ff 18 13      Evt_exec sub19 ; Erfolgs-Ereignis
0x0152A  22 03 32 01      Set(3,50,1)    ; <<< ERFOLGS-BIT: "Code einmal richtig eingegeben"
```

**6. Das Erfolgs-Ereignis (sub19)** — `0x01960..0x019AE`: räumt die Zifferntasten ab
(`Set(5,0..2,0)`), `Aot_reset` der Leser-Slots 0/1, armiert Slot 2 als echte Tür
(`46 02 02 31 …` `@0x01988`), und gibt `Message_on 5` = `"You've opened the lock."`
`@0x0199E`.

### Damit sind zwei Dinge gemessen, nicht vermutet

1. **Zum Zeitpunkt von `Message_on 9` ist das Erfolgs-Bit beweisbar 0.** sub20 ist nur
   erreichbar, solange main00 die Leser-AOTs installiert, und das tut main00 nur bei
   `Ck(3,50,0)` (`@0x01026`). Gesetzt wird das Bit erst `@0x0152A`, nach der
   Vier-Ziffern-Schranke.
2. **Nach dem Erfolg ist sub20 nie wieder erreichbar.** sub19 setzt die Leser-Slots zurück
   (`@0x01974`/`@0x0197E`) und retypt Slot 2 zur Tür (`@0x01988`); beim Wiedereintritt nimmt
   main00 den Else-Zweig (`@0x01174`). Eine Bedingung "Bit gesetzt **und** `Message_on 9`"
   wäre also **nie** wahr und würde die Abfrage dauerhaft töten.

**Folgerung für die Umsetzung:** die Abfrage dieser sechs Stellen darf nicht an der
"used the"-Nachricht hängen, sondern an der **Erfolgs-Nachricht des Panels**,
`Message_on 5` = `"You've opened the lock."` — zusätzlich abgesichert durch das
Erfolgs-Bit als Prädikat (fail-closed: ist das Bit 0, kommt keine Abfrage).

`Evt_exec` **spawnt** einen Thread, läuft nicht inline (`op_evt_exec`, `scd_vm.c:1060-1064`,
`t->pc += 4` *vor* dem Spawn, byte-true `@0x8003f2b8`). sub01 führt also `Set(3,50,1)`
`@0x0152A` sofort aus, während sub19 erst 10 Opcodes später bei `Message_on 5` ankommt —
das Bit steht zum Zeitpunkt der Erfolgs-Nachricht.

### Die sechs Panels, byte-identisch (`schranke.log`)

| Raum | Datei | Muster @ | Schloss-Ck | Erfolgs-`Set` @ | Erfolgs-Bit | `Message_on 5` @ | "used the" @ |
|---|---|---|---|---|---|---|---|
| ROOM10D0 | STAGE1, 286320 B | 0x01516 | `21 03 32 00` @0x01512 | **0x0152A** | **Bank 3 / Bit 50** | sub19 @0x0199E | sub20 @0x019C4 |
| ROOM10D1 | STAGE1, 240364 B | 0x01500 | `21 03 32 00` @0x014FC | **0x01514** | **Bank 3 / Bit 50** | sub19 @0x01988 | sub20 @0x019AE |
| ROOM11E0 | STAGE1, 209684 B | 0x01A76 | `21 03 8b 00` @0x01A72 | **0x01A8A** | **Bank 3 / Bit 139** | sub19 @0x01F86 | sub20 @0x01FAC |
| ROOM11E1 | STAGE1, 209684 B | 0x01A76 | `21 03 8b 00` @0x01A72 | **0x01A8A** | **Bank 3 / Bit 139** | sub19 @0x01F86 | sub20 @0x01FAC |
| ROOM1230 | STAGE1, 166268 B | 0x00FDE | `21 03 89 00` @0x00FDA | **0x00FF2** | **Bank 3 / Bit 137** | sub19 @0x01466 | sub20 @0x0148C |
| ROOM1231 | STAGE1, 166268 B | 0x00FDE | `21 03 89 00` @0x00FDA | **0x00FF2** | **Bank 3 / Bit 137** | sub19 @0x01466 | sub20 @0x0148C |

main00-Layout-Schranke am selben Bit: ROOM10D0 `@0x01026`, ROOM10D1 `@0x01026`,
ROOM11E0 `@0x01576`, ROOM11E1 `@0x01576`, ROOM1230 `@0x00B98`, ROOM1231 `@0x00B98`.

Besitz-(taken-)Bit im Leser-Zweig, deckungsgleich mit dem `Item_aot_set`-`tk_bit`
(`item_zensus.log`): 0x38 Blue Keycard = Bit 52, 0x39 Yellow Keycard = Bit 138,
0x37 Red Keycard = Bit 136.

## Fall (a) oder Fall (b): trägt der Gegenstand den Code, oder verlangt das Panel einen zusätzlichen?

Der Auftrag verlangt diese Trennung ausdrücklich. Sie fällt an den Raumtexten, die jeweils
im Nachrichtenblock der RDT stehen:

* **Fall (a) — Gegenstand ist der Schlüssel, Code kommt ZUSÄTZLICH: die sechs Kartenleser.**
  msg 7 wörtlich: *"A card reader. You have to use the `<Karte>` **and insert four digits**
  to unlock the door. Will you operate the card reader?"* Zwei getrennte Anforderungen;
  die Karte allein öffnet nichts. ROOM1230 msg 10 nennt den Code sogar im Klartext:
  *"He is holding a slip. The numbers 5632" are printed on the slip."*
* **Fall (b) — der Gegenstand IST der Code-/Autorisierungsträger.**
  * ROOM1100/1101 msg 1: *"It's a control panel. **A type of voice device is required** to
    open the lock."* Der "Minidisc Player w/ Disc" bringt die Stimme mit; es gibt keinen
    zweiten Code. Direkt nach `Message_on 4` folgt in derselben Routine `Message_on 2` =
    *"You've opened the lock."* (`@0x00CBA`).
  * ROOM3050/3051 msg 3: *"In case the machinery stops working, it will be necessary to use
    the Red Master Keycard to restore the system."* Die Karte ist die ganze Autorisierung.
* **Keiner von beiden** — kein Code im Spiel: ROOM11E0/11E1 Zange (Werkzeug an einem
  Kabel), ROOM1090 Feuerlöscher, ROOM3010/3011 und ROOM4000 (*"An ID card is required to
  unlock it."* — reines Schlüsselschloss).

## Tabelle: alle 16 Stellen

`Message_on`-Offset = Datei-Byte-Offset des `2b <msg> ff ff`-Records in der RDT.
"Panel" = verlangt das Tor eine **Code-Eingabe**? "Erfolg vor Nachricht?" = steht das
persistente Erfolgs-Bit schon, wenn die "used the"-Nachricht aufgeht?

| # | Raum | msg | Gegenstand | Sub / `Message_on` @ | Klasse | Panel (Code)? | Erfolgs-Bit | Erfolg vor Nachricht? | Bedingung für die Abfrage |
|---|---|---|---|---|---|---|---|---|---|
| 1 | ROOM1090 | 9 | 0x31 Fire Extinguisher | sub03 @0x02502 | EINFACH | **nein** | Bank 3 / Bit 129, `Set` @0x0271E (sub06, direkt nach dem JA) | **ja** (eine Subroutine früher) | unverändert — an `msg 9` |
| 2 | ROOM10D0 | 9 | 0x38 Blue Keycard | sub20 @0x019C4 | **CODE** | **JA, 4 Ziffern** | Bank 3 / Bit 50, `Set` @0x0152A | **NEIN** | **umhängen** auf `msg 5` @0x0199E, Prädikat `flag(3,50)==1` |
| 3 | ROOM10D1 | 9 | 0x38 Blue Keycard | sub20 @0x019AE | **CODE** | **JA, 4 Ziffern** | Bank 3 / Bit 50, `Set` @0x01514 | **NEIN** | **umhängen** auf `msg 5` @0x01988, Prädikat `flag(3,50)==1` |
| 4 | ROOM1100 | 4 | 0x44 Minidisc Player w/ Disc | sub02 @0x00C90 | CURSOR | nein (Fall b) | Bank 4 / Bit 232, `Set` @0x00C68 | **ja** (0x28 B davor) | unverändert — an `msg 4` |
| 5 | ROOM1101 | 4 | 0x44 Minidisc Player w/ Disc | sub02 @0x00C90 | CURSOR | nein (Fall b) | Bank 4 / Bit 232, `Set` @0x00C68 | **ja** | unverändert — an `msg 4` |
| 6 | ROOM11E0 | 9 | 0x39 Yellow Keycard | sub20 @0x01FAC | **CODE** | **JA, 4 Ziffern** | Bank 3 / Bit 139, `Set` @0x01A8A | **NEIN** | **umhängen** auf `msg 5` @0x01F86, Prädikat `flag(3,139)==1` |
| 7 | ROOM11E0 | 12 | 0x30 Pliers | sub21 @0x01FEE | CURSOR | nein | Bank 4 / Bit 36, `Set` @0x01FC6 | **ja** (0x28 B davor) | unverändert — an `msg 12` |
| 8 | ROOM11E1 | 9 | 0x39 Yellow Keycard | sub20 @0x01FAC | **CODE** | **JA, 4 Ziffern** | Bank 3 / Bit 139, `Set` @0x01A8A | **NEIN** | **umhängen** auf `msg 5` @0x01F86, Prädikat `flag(3,139)==1` |
| 9 | ROOM11E1 | 12 | 0x30 Pliers | sub21 @0x01FEE | CURSOR | nein | Bank 4 / Bit 36, `Set` @0x01FC6 | **ja** | unverändert — an `msg 12` |
| 10 | ROOM1230 | 9 | 0x37 Red Keycard | sub20 @0x0148C | **CODE** | **JA, 4 Ziffern** | Bank 3 / Bit 137, `Set` @0x00FF2 | **NEIN** | **umhängen** auf `msg 5` @0x01466, Prädikat `flag(3,137)==1` |
| 11 | ROOM1231 | 9 | 0x37 Red Keycard | sub20 @0x0148C | **CODE** | **JA, 4 Ziffern** | Bank 3 / Bit 137, `Set` @0x00FF2 | **NEIN** | **umhängen** auf `msg 5` @0x01466, Prädikat `flag(3,137)==1` |
| 12 | ROOM3010 | 1 | 0x36 Green Keycard | sub02 @0x02306 | EINFACH | **nein** | Bank 3 / Bit 60, `Set` @0x02302 | **ja** (4 B davor) | unverändert — an `msg 1` |
| 13 | ROOM3011 | 1 | 0x36 Green Keycard | sub02 @0x0233A | EINFACH | **nein** | Bank 3 / Bit 60, `Set` @0x02336 | **ja** (4 B davor) | unverändert — an `msg 1` |
| 14 | ROOM3050 | 5 | 0x46 Red Master Keycard | sub15 @0x02608 | CURSOR | nein (Fall b) | Bank 3 / Bit 179 + Bit 176, `Set` @0x0260E/@0x02612 | **nein**, aber 2 Opcodes danach, unbedingt, in derselben Routine | unverändert — an `msg 5` |
| 15 | ROOM3051 | 5 | 0x46 Red Master Keycard | sub15 @0x02608 | CURSOR | nein (Fall b) | Bank 3 / Bit 179 + Bit 176, `Set` @0x0260E/@0x02612 | **nein**, s.o. | unverändert — an `msg 5` |
| 16 | ROOM4000 | 2 | 0x47 Blue Master Keycard | sub02 @0x0144A | EINFACH | **nein** | Bank 3 / Bit 32, `Set` @0x01450 | **nein**, aber 1 Opcode danach, unbedingt, im selben Ifel_ck-Block | unverändert — an `msg 2` |

**Summe: 6 Stellen brauchen die neue Bedingung, 10 nicht.**

### Warum die 10 anderen Stellen richtig sind — je Stelle belegt

* **ROOM1090** (`raum_1090_voll.log`): Der Raum enthält **keinen einzigen** `Sce_key_ck`.
  Das einzige `Cmp` `@0x02400` ist `Cmp(bank 0, var 10, ==, 13)` und gatet die
  Intro-Sequenz sub02, nicht den Feuerlöscher. Der Ja/Nein-Dialog steht in sub06:
  `Message_on 7` `@0x02702`, `Message_on 8` = *"Will you use the Fire Extinguisher?"*
  `@0x02708`, `Ck(12,31,0)` `@0x02712`, dann `Set(3,129,1)` `@0x0271E` = das persistente
  Erfolgs-Bit und `Set(3,132,1)` `@0x02722` = die Einmal-Zündung, + `Aot_on 3`.
  sub00 prüft `Ck(3,132,1)` `@0x022A6` und startet daraufhin `Evt_exec sub03` `@0x022E0`;
  sub03 löscht das Feuer und gibt `Message_on 9` `@0x02502`. Bit 129 steht also schon
  eine Subroutine früher, und sub00 gatet den ganzen Vorher-Zustand daran
  (`Ck(3,129,0)` `@0x021C8`, `@0x021EE`, `@0x022EA`, `@0x02332`).
* **ROOM1100/1101** (`raum_1100_voll.log`): Cursor-Panel, **keine** Ziffern.
  sub07 `@0x00D50`: msg 1 (Kontrollpanel) + msg 3 *"Will you use the Minidisc Player w/
  Disc?"* + `Ck(12,31,0)` `@0x00D60` → `Set(5,0..4,6)` scharf. sub01: vier Richtungstasten
  → sub03..06; **zwei** Bestätigungen: `Member_cmp(15==4)` `@0x00C14` + SQUARE `@0x00C1E`
  → sub08 (Gerät einsetzen), dann `Member_cmp(15==5)` `@0x00C3C` + SQUARE `@0x00C46`
  → sub02 = die Benutzung. sub02 setzt `Set(4,232,1)` **als ersten Opcode** `@0x00C68`,
  40 Byte **vor** `Message_on 4`. Der Erfolg ist also schon da.
* **ROOM11E0/11E1 Zange** (`raum_11E0_voll.log`): sub26 `@0x020AC` fragt
  (msg 10 + msg 11 *"Will you use the Pliers?"* + `Ck(12,31,0)` `@0x020BC`), dann
  `Set(5,21,1)`/`Set(5,22,1)`/`Set(5,2,1)`. sub01 `@0x01AF0`: `Ck(5,22,1)` +
  `Member_cmp(15==7)` `@0x01B00` + SQUARE `@0x01B0A` → sub21. sub21 setzt `Set(4,36,1)`
  **als ersten Opcode** `@0x01FC6`, 40 Byte **vor** `Message_on 12`.
  Zange und Kartenleser sind **unabhängig**: die Zangen-Schranke sitzt in sub00
  `@0x017CC` (`Ck(4,36,0)`), die Leser-Schranke in main00 `@0x01576` (`Ck(3,139,0)`).
* **ROOM3010/3011** (`raum_3010_voll.log`): **kein** `Sce_key_ck` im Raum. msg 0 lautet
  *"It's locked. An ID card is required to open it."* — kein Code. sub02 setzt
  `Set(3,60,1)` `@0x02302` (3011: `@0x02336`) **unmittelbar vor** der Nachricht.
  Ausgelöst wird sub02 von einem Aktions-AOT mit `sce=3`: `Aot_set` `@0x020D2` =
  `2c 06 03 31 01 00 c0 95 70 ae 84 03 48 08 ff 00 18 02 00 00`; Nutzlast `u16@+16` =
  `0x0218`, `>>8` = **sub 02** (`aot_common.c:246-251`, `event_id = p1 >> 8`, byte-true
  `@0x80043100`). Die zwei `Cmp` im Raum sind Kamera-Vergleiche
  (`Cmp(0,11,==,6)` main00 `@0x02162`, `Cmp(0,1,==,7)` sub01 `@0x022D8`).
* **ROOM3050/3051** (`raum_3050_voll.log`): Cursor-Panel, **keine** Ziffern.
  sub10 `@0x02578` fragt (msg 3 + msg 4 + `Ck(12,31,0)` `@0x02588`) → `Set(5,0..5,1)`.
  sub01: vier Richtungstasten → sub11..14; **eine** Bestätigung `Ck(5,4,1)` +
  `Member_cmp(15==11)` `@0x0220E` + SQUARE `@0x02218` → sub15. `Set(3,179,1)`/`Set(3,176,1)`
  folgen 2 Opcodes nach `Message_on 5`, unbedingt in derselben Routine — keine Verzweigung
  dazwischen. Die vier `Cmp` sind Kamera-Vergleiche (`Cmp(0,10,==,0/2/8/3)`).
* **ROOM4000** (`raum_4000_voll.log`): **kein** `Sce_key_ck` im Raum. msg 0:
  *"Door is under emergency lockdown. An ID card is required to unlock it."* — kein Code.
  sub02: `Ck(3,32,0)` `@0x0142A` → msg 0, msg 1 (*"Will you use the Blue Master
  Keycard?"*), `Ck(12,31,0)` `@0x01446`, `Message_on 2` `@0x0144A`, `Set(3,32,1)`
  `@0x01450` — ein Opcode danach, unbedingt im selben `Ifel_ck`-Block. Die sechs `Cmp`
  sind Kamera-Vergleiche.

## Umsetzungsreife Bedingung (kein Code in diesem Lauf geändert)

Für die **sechs** Code-Panel-Stellen ist die Stellen-Tabelle von
`(Raum, msg_used, Gegenstand)` auf `(Raum, msg_erfolg, Gegenstand, Erfolgs-Flag)`
umzustellen:

| Raum | heute | künftig Auslöser | künftig Prädikat |
|---|---|---|---|
| ROOM10D0 | `msg 9` | `msg 5` @0x0199E | `flag(3, 50) == 1` |
| ROOM10D1 | `msg 9` | `msg 5` @0x01988 | `flag(3, 50) == 1` |
| ROOM11E0 | `msg 9` | `msg 5` @0x01F86 | `flag(3, 139) == 1` |
| ROOM11E1 | `msg 9` | `msg 5` @0x01F86 | `flag(3, 139) == 1` |
| ROOM1230 | `msg 9` | `msg 5` @0x01466 | `flag(3, 137) == 1` |
| ROOM1231 | `msg 9` | `msg 5` @0x01466 | `flag(3, 137) == 1` |

Die anderen zehn Zeilen bleiben, wie sie sind.

Das Prädikat ist **fail-closed**: ist das Bit 0, kommt keine Abfrage. Es ist außerdem
nicht redundant, sondern die Absicherung gegen eine Neuzuordnung der Nachrichten-Ids:
`"You've opened the lock."` ist **kein** eindeutiger Schlüssel — der Text steht in
**22 Räumen** (ROOM10D0/10D1, 1100/1101, 1140/1141, 11E0/11E1, 1230/1231, 2020/2021,
2070/2071, 20A0/20A1, 3040/3041, 30C0/30C1, 6010/6011). Kollisionsfrei ist nur das
Paar (Raum, msg) **plus** das Flag; in ROOM1100 trägt derselbe Text sogar die
**msg 2** und liegt 0x2A Byte nach der Wegwerf-Stelle `msg 4`.

### Wonach im Testlauf zu greifen ist (Riegel gegen die Rückkehr des Fehlers)

1. **Sollzahl-Riegel**: genau **6** der 16 Stellen tragen ein Erfolgs-Flag; wer die
   Tabelle ändert, muss diese 6 wieder treffen.
2. **Reihenfolge-Riegel je Code-Panel**: die Abfrage darf bei `msg 9` **nicht** erscheinen,
   auch nicht, wenn die Karte im Inventar liegt und der Ja/Nein-Dialog mit JA beantwortet
   wurde. Messpunkt: `flag(3,<bit>)` ist in diesem Moment 0.
3. **Erreichbarkeits-Riegel**: nach vier richtigen Ziffern muss die Abfrage genau
   **einmal** kommen. Fällt sie aus, ist entweder das Flag-Bit falsch oder der Auslöser
   noch `msg 9` (dann ist er unerreichbar — siehe oben).
4. **Nicht-Regressions-Riegel**: für die 10 übrigen Stellen bleibt das Verhalten
   unverändert.

## Beifunde (nicht Teil des Auftrags, aber belegt)

* **RE1.5 hat einen eigenen, nie geöffneten "brauche ich nicht mehr"-Text.**
  `"I don't need to use this anymore."` steht im Nachrichtenblock von ROOM1100/1101 als
  **msg 5** und von ROOM3050/3051 als **msg 7** — und **kein** `Message_on` im jeweiligen
  SCD öffnet ihn (ROOM1100: nur msgs 1, 2, 3, 4 werden ausgegeben). Toter Text, genau in
  den Räumen mit einem Fall-(b)-Gegenstand.
* `Sce_espr_control` (0x52) kommt in den **206** ausgelieferten RDTs **0 mal** vor. Für die
  Panel-Erkennung reicht 0x51.
* Die 34 Stummel-RDTs (4 Byte, kein Header) sind: ROOM1270/1271, 20C0/20C1, 20D0/20D1,
  20E0/20E1, 20F0/20F1, 30F0/30F1, 4060/4061, 40C0/40C1, 40D0/40D1, 40E0/40E1, 40F0/40F1,
  5150/5151, 5160/5161, 5170/5171, 6050/6051, 6060/6061, 6070/6071 — keine davon ist eine
  Benutzungsstelle.
* Die 29 Räume mit Tastenlesung enthalten neben den hier untersuchten noch weitere
  Rätsel-Panels (ROOM1080, 11B0/11B1, 11F0/11F1, 1240/1241, 2040/2041, 2050/2051,
  2060/2061, 30E0, 4020/4021, 5050/5051). Keiner davon ist eine Wegwerf-Stelle —
  ROOM2060/2061 fiel im Generator schon an Bedingung B aus (Fuse 0x40 wird nie platziert).

## Noch offen

* **Wer setzt `Ck(3,59)` in ROOM3010?** Der Raum wählt an `Ck(3,59,0)` `@0x020AE` zwischen
  "Nachricht: verschlossen" und "Aktion: Karte benutzen", prüft dabei aber **nicht** das
  taken-Bit der Green Keycard (Bit 85; das gatet ROOM3040/3041). Für die Panel-Frage
  irrelevant, für die Besitzprüfung des Parallel-Laufs möglicherweise nicht.
* Die endgültige Formulierung des Prädikats im Port (Flag-Bank-API vs. eigenes Feld in der
  Stellen-Tabelle) ist Umsetzungssache und bewusst hier nicht entschieden — dieser Lauf
  durfte `item_discard_common.c` nicht anfassen.

## Messwerkzeuge und Rohprotokolle

Alle unter `analysis/befunde_2026-09-22/discard-codepanel/`:

| Datei | Zweck |
|---|---|
| `codepanel_zensus.py` | klassifiziert je Stelle: findet den ausführenden `Message_on`, dumpt die Subroutine mit Datei-Offsets, listet die gatenden Prädikate und den Raum-Zensus (0x51/0x52/0x3E/0x23/0x5E). `--room`, `--full`. |
| `codepanel_muster.py` | die drei Zensus-Läufe über alle 206 RDTs (Text / rohes Byte-Muster / Eingabe-Opcodes) mit ausgewiesener Abdeckung. |
| `codepanel_schranke.py` | druckt die Erfolgs-Schranke der sechs Panels als rohe Datei-Bytes. |
| `discard_zensus.py`, `discard_nutzstellen.py` | aus Commit `47766cb4` übernommene Decoder (Opcode-Längen = `scd_vm.c` `s_opcode_sizes`, Glyphentabelle = `msg_common.c`). |
| `alle16_stellen.log` | alle 16 Stellen, 16/16 gefunden. |
| `muster_zensus.log` | die drei Zensus-Läufe. |
| `schranke.log` | die sechs Erfolgs-Schranken. |
| `item_zensus.log` | `Item_aot_set`-Zensus: 164 Records, `tk_bit` je Gegenstand, `Keep_Item_ck` 0×. |
| `raum_<ID>_voll.log` | vollständiger Opcode-Dump + Nachrichtenblock je Basisraum (1090, 10D0, 1100, 11E0, 1230, 3010, 3050, 4000). |
