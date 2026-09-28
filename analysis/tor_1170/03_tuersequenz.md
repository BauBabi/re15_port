# Tor ROOM1170 - Dossier 03: Mechanik der Tuersequenz (RE2 Retail) und was RE1.5 davon hat

Stand 2026-09-28. Nur gelesen und gemessen, nichts am Port geaendert, nichts gebaut.

Werkzeug: `re15_port/tools/tor/tuerskript_dump.py` (neu). Messwerte: `build/tor_1170/tuersequenz.json`.
Binaerdateien: RE2 = `info/re2leon/PSX.EXE` (Leon, Retail), RE1.5 = `info/Re1.5/PSX.EXE` (Auslieferungsstand).
Disassembler: `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` bzw. `re15_disasm.py`.

Herkunftsmarken in diesem Dokument:
- `@0x8...` = Instruktion an dieser Adresse selbst gelesen (Direkt-Disassembly, nicht Decompilat).
- `Datei 0x...` = Byte-Offset in der genannten Datei.
- `[SIM]` = Ergebnis der Skript-Simulation des Werkzeugs. Die Simulation fuehrt die Skriptbytes nach den
  gelesenen Handlern aus; sie ist KEINE Emulator-Messung. Bildrechtecke rechnet sie in Gleitkomma,
  nicht mit der GTE-Festkomma-Division (Abweichung im Subpixelbereich moeglich).

---

## 0. Kernaussagen

1. RE2 faehrt die Tuer als eigene Task mit eigenem Skript-Scheduler (4 Threads, 10 Objekte), fester
   Kamera und festem Licht vor schwarzem Hintergrund. Bewegt wird die TUER, nicht die Kamera.
2. Der Platzierungs-Opcode in den RE2-Archiven ist **0x4D** (22 B). Er traegt weder Geschwindigkeit
   noch Dauer. Die Bewegung entsteht aus `Work_set`/`Speed_set`/`Add_speed`/`Add_aspeed` in Schleifen.
   RE1.5 hat denselben Satz unter **0x4F**, aber mit anderem Elternbit (0x8 statt 0x10) und nur 4 Objekten.
3. Skriptwahl: Skript 0 jedes Archivs ist ein Verteiler `Switch(var 12)`. var 12 = Byte +13 des
   Tuer-Payloads & 0x7f. Das Archiv waehlt Byte +12 des Payloads.
4. RE1.5 besitzt die komplette Maschine (Lader, Init, Scheduler, Matrizen, Renderer, Opcode, Exit) und
   ruft sie bei JEDEM Tuerwechsel auf. Sie laeuft genau 1 Bild, weil das einzige Skript `Evt_end` ist.
   Die fruehere Notiz "kein DO2 geladen" ist damit widerlegt: geladen wird, gezeichnet wird nichts.
5. In den RE1.5-Raumdaten sind Payload +12..+17 in allen 649 `Door_aot_set`-Saetzen 0. Welche Tuer
   welches Modell und welche Richtung bekommt, steht also NICHT in den RE1.5-Daten.
6. Modell: nur Dreiecke, Ursprung des Blatt-Meshes auf der Angelachse, Blick entlang -x
   (Bild-rechts = +z, Bild-unten = +y). Standardblatt 288 x 6602 x 3599 Einheiten steht bei
   Tiefe 8000 als 132,8 x 243,6 Pixel im Bild.

---

## 1. Funktionskarte RE2 Retail

| Rolle | Adresse | Beleg |
|---|---|---|
| Tuer-AOT-Handler (sce = 1) | `LAB_80051514` | Tabelle `0x800a73c4[1]` = `14 15 05 80`; `@0x800516e8 sw s0,0x800ce550` (Payload merken), `@0x800516d4 sb v1,0x800df348` (Latch = 1) |
| AOT-Dispatch, Payload-Zeiger | `0x80051428..44` | `@0x80051444 addiu a0,s0,12` (Rechteck) bzw. `@0x8005141c addiu a0,s0,20` (4P) |
| Raumwechsel mit Tuer | `FUN_80026b7c` | Aufruf `@0x80025a70`; `@0x80026bec jal 0x8002bda8` (a0=2,a1=0: schwarz), `@0x80026bf8 addiu a1,a1,0x3bc4` + `@0x80026bfc jal 0x80031f6c` mit `a0=1` (Task 1 = Door_main), `@0x80026c0c jal 0x80014cd0` (Ton laden) |
| Door_main | `LAB_80013bc4` | Schleife ueber Tabelle `0x8009a7b4` = `0x80013c1c, 0x80013eb4, 0x8001417c, 0` (`@0x80013bf0 jalr v0`) |
| Door_init | `FUN_80013c1c` | siehe Abschnitt 3.2 |
| Laden Teil 2 (Textur+Modell+Skripte) | `FUN_80015064` | `@0x80013c7c jal`, Ziel `0x801a1000` (`@0x80015068/6c`), String "DOOR TEXTURE" `@0x80010858` |
| Laden Teil 1 (Ton) | `FUN_80014cd0` | String "DOOR SOUND" `@0x8001084c`; Ziel `0x801a1000 - Tonteilgroesse` (`@0x80014da0 subu s0,s0,s4`) |
| Door_move (Bildschleife) | `FUN_80013eb4` | siehe 3.3 |
| Scheduler | `FUN_80014058` | Threads 10..13 (`@0x80014068 addiu s2,zero,10`, `@0x80014150 sltiu v0,s2,0xe`), Dispatch `0x800a74c8` (`@0x80014074`) |
| Objektmatrizen + Zeichnen | `FUN_80014234` | 10 Objekte (`@0x80014664 slti v0,s2,10`), Schritt 332 (`@0x80014660`) |
| Dreiecks-Renderer | `FUN_8001468c` | ein einziger Aufruf je Objekt `@0x80014654` |
| Mesh an Objekt binden | `FUN_80014b40` | `@0x80014b60..68` Mesh-Index * 56 |
| Opcode 0x4D Door_model_set | `0x80014ba4` | Tabelle `0x800a74c8[0x4d]` = `0x80014ba4` |
| Door_exit | `0x8001417c` | siehe 3.6 |
| Kamera setzen | `FUN_80076cb0` | auch von der Raumkamera benutzt (`FUN_8002bdf4`) |
| SetGeomScreen | `FUN_8008de24` | `gte_ldH` |

Nachmessen: `python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py table 0x8009a7b4 4`,
`... dis 0x80013bc4 22`, `... dis 0x80026b7c 45`.

---

## 2. Das Archiv (DO2)

### 2.1 RE2: zwei Teile

`DOOR00.DO2` (56784 B):

| Datei-Offset | Inhalt |
|---|---|
| 0x0000 | Tonkopf 16 B = 4 Eintraege je 4 B: `00 00 14 16 | 00 00 24 17 | ff ff ff ff | ff ff ff ff` |
| 0x0010 | VH (3104 B, Magic `pBAV`) |
| 0x0c30 | `10 00 00 00 00 00 00 00` (Versatz des VH) |
| 0x0c38 | VB (16080 B) bis 0x4b08 |
| 0x5000 | **Teil 2**: u32 Versatz MD1 (0x224), u32 Versatz TIM (0xbb0), ab +8 Skript-Versatztabelle (u16) |
| 0x501a | Skript 0 |
| 0x5224 | MD1 (2444 B) |
| 0x5bb0 | TIM (33312 B) |

Die Aufteilung steht in der EXE, Tabelle `0x8009a520`, 12 B je Tuertextur-Typ:
`u16 Tonteil-Groesse, u16 Teil-2-Groesse, u32 Sektorversatz, u8, u8`. Eintrag 0 = `08 4b d0 8d 0a 00 00 00 9a 42`
= 0x4b08, 0x8dd0, Sektor 10 (= 0x5000). Gelesen in `FUN_80015064`: `@0x800150b0 lw t2,4(v1)`,
`@0x800150f4 lhu v1,2(v1)`. Datei-Nummer aus `0x8009a4b0[typ]` (u16; Typ 0 = 234).

Gegenprobe: bei **55 von 55** Archiven gilt `Sektorversatz * 0x800 == Teil-2-Anfang` und
`Teil-2-Anfang + Teil-2-Groesse == Dateigroesse`.

Alle 55 Archive haben genau **2 Toene** (Eintrag 0 und 1 belegt, 2 und 3 = `ff ff ff ff`).

### 2.2 RE1.5: Zeigertabelle

`re15_port/shared_assets/PSX/DOOR/DOOR00.DO2` (57016 B, sha1 `5725afcb...` = `info/Re1.5/PSX/DOOR/DOOR00.DO2`):

| Datei-Offset | Bytes | Bedeutung |
|---|---|---|
| 0x00 | `0c 00 00 00` | Versatz der Zeigertabelle |
| 0x04 | `c8 8b 00 00` | Tonkopf+VH @0x8bc8 (`00 00 13 16 00 00 23 17 ...`) |
| 0x08 | `f8 97 00 00` | VB @0x97f8 |
| 0x0c | `0c 00 00 00 98 09 00 00 9c 09 00 00` | MD1, SCD, TIM - jeweils + 0xC |
| 0x18 | MD1 | byte-gleich mit RE2 `DOOR00.md1` (2444 B) |
| 0x9a4 | `02 00 01 00` | Skript-Tabelle mit 1 Eintrag, Skript 0 @0x9a6 = `01 00` = `Evt_end` |
| 0x9a8 | TIM | byte-gleich mit RE2 `DOOR00.tim` (33312 B) |

Relokation: `@0x800162b4..c8` ([4],[8] += Basis), `@0x800162f4 ori a0,a0,0x100c` (Tabelleneintraege + Basis + 0xC).

Nachmessen: `python re15_port/tools/tor/tuerskript_dump.py container <DO2>`.

---

## 3. Ablauf RE2, Schritt fuer Schritt

### 3.1 Ausloesung und Reihenfolge der beiden Tasks

1. Tuer-AOT feuert -> `LAB_80051514` (prueft Schloss ueber Payload +15/+16: `@0x800515a8 lbu a1,15(s2)`,
   `@0x800515d0 lbu s0,16(s2)`), merkt den Payload-Zeiger.
2. `FUN_80026b7c`: Bild schwarz, Task 1 = Door_main starten, 1 Bild schlafen, dann Ton laden.
   Der Ton-Lader wartet, bis Door_init mit seinem CD-Zugriff fertig ist
   (`@0x80014d3c lw a0,0x800c3a84` / `@0x80014d70 bne`; Door_init setzt das Wort `@0x80013c78` auf 1
   und `@0x80013dc8` auf 0).
3. Waehrend die Sequenz laeuft, setzt `FUN_80026b7c` die neue Spielerposition aus dem Payload und
   laedt den Zielraum; am Ende wartet es auf das Ende der Tuer-Task
   (Bit 0x2000000 in `0x800cfb74`, gesetzt `@0x80013cb4..bc`, geloescht in Door_exit `@0x80014218`).

### 3.2 Door_init `FUN_80013c1c`

| Schritt | Beleg |
|---|---|
| Teil 2 nach `0x801a1000` laden | `@0x80013c7c jal 0x80015064` |
| Arbeitsbereich `0x801bd000` | `@0x80013c84/88`, `@0x80013cac sw v0,0x800c3a80` |
| 10 Objektzeiger `0x800d4dd8..0x800d4dfc`, Objekt 9 @`0x801bddf8`, Schritt -332 | `@0x80013c8c addiu a2,zero,9`, `@0x80013c98/9c`, `@0x80013cd0 addiu a1,a1,-332` |
| Objekte nullen (Objekt 0 bis Objekt 9 + 0x146) | `@0x80013cf8 sh zero,0(v1)` |
| Kopfworte relozieren: [4] -> TIM, [0] -> MD1 | `@0x80013d24..4c` |
| TIM-Ziel: Wort `0x800cfbf0` = 0x1f15 -> Seite 21 (x=320,y=256), CLUT-Zeile 0x1e0+0x1f = 511 | `@0x80013d28 addiu v0,zero,7957`; Rechnung in `FUN_80076a40` |
| TIM hochladen | `@0x80013d50 jal 0x80076a40` |
| MD1 umsetzen: tpage += 21, CLUT-Zeile += 31; **nicht** bei Tuertextur-Typ 40 | `@0x80013d78 lbu v1,12(v0)`, `@0x80013d7c addiu v0,zero,40`, `@0x80013d88 addiu a2,zero,21`, `@0x80013d90 addiu a3,zero,31`, `@0x80013d9c jal 0x80076b60` |
| MD1-Kopf 12 B ueberspringen | `@0x80013de4 addiu v1,v1,12` |
| Bildzaehler = 0 (Arbeitsbereich +0x22e) | `@0x80013de0 sh zero,558(a0)` |
| Threads 10..13 anlegen, Schritt 372 | `@0x80013dec..0c` |
| Skripttabelle = `0x801a1008`, Thread 10 startet **Skript 0** | `@0x80013e10..2c`, `@0x80013e24 addu a1,zero,zero`, `@0x80013e28 jal 0x800530ec` |
| **Projektionsdistanz H = 290** | `@0x80013e34 addiu a0,zero,290`, `@0x80013e30 jal 0x8008de24` |
| **Kamera: Auge (10000,0,0), Ziel (0,0,0)** | Daten `@0x80010830` = `10 27 00 00 | 00 00 00 00 | 00 00 00 00`, `@0x8001083c` = 12 Nullbytes; kopiert `@0x80013c30..6c` nach sp+16..; `@0x80013e38 addiu a0,sp,20`, `@0x80013e40 addiu a1,sp,32`, `@0x80013e3c jal 0x80076cb0` |
| Hintergrund schwarz | `@0x80013e44..4c jal 0x8002bda8` (2,0) |
| Variablen 12..15 | siehe Abschnitt 4 |
| Schliesston-Merker = 0 (Arbeitsbereich +0x248) | `@0x80013e9c sh zero,584(v1)` |

Bildmitte: Door_init ruft `SetGeomOffset` (`0x8008de04`) nicht auf. Die beiden Spiel-Aufrufer setzen
(160,120): `@0x80049cac/b0` und `@0x80068e80/88`.

Sichtmatrix aus `FUN_80076cb0` fuer diese Werte (nachgerechnet mit `tuerskript_dump.camera`):
Zeilen (0,0,1), (0,1,0), (-1,0,0), t = (0,0,10000). Das heisst:

- Bild-x waechst mit Welt **+z**, Bild-y mit Welt **+y** (PSX: y nach unten),
- Tiefe = 10000 - x; zur Kamera hin ist **+x**.

Licht (fest, nicht aus dem Raum): Lichtmatrix `@0x8009a470` = `90 01 20 03 0c fe | f8 f8 18 fc 74 f5 | ac 0d 2c 1a b0 04`
= (400,800,-500), (-1800,-1000,-2700), (3500,6700,1200); Farbmatrix `@0x8009a490` = neunmal 0x0640;
Hintergrundfarbe 0x44 je Kanal (`@0x800142e8 addiu a3,zero,68`), mit Flag 0x1000 0x88 (`@0x800142bc`).

### 3.3 Door_move `FUN_80013eb4` - ein Durchlauf = ein Bild

```
warten, solange 0x800cfb74 & 0x10000                       @0x80013ed0..f8
wenn (Objekt0.flags & 0x8000) == 0: 0x800dfc1a = 0         @0x80013f0c..24
solange Thread 10 aktiv (0x800d86e9):                       @0x80013f2c / @0x8001402c
    wenn Ladebit 0x20000 geloescht: var 13 = 0              @0x80013f54..68
    Scheduler  FUN_80014058                                 @0x80013f6c
    Objekte    FUN_80014234                                 @0x80013f74
    wenn 41 <= Bildzaehler <= 259 und Tuertextur-Typ 50 bzw. 52: Textzeilen   @0x80013f90..0x80014004
    Bildzaehler += 1                                        @0x8001401c / @0x80014024
    Task_sleep(1)                                           @0x80014020
```

Scheduler: je Thread Handler aufrufen, solange Rueckgabe 1; Rueckgabe 2 = Bild zu Ende
(`@0x800140e8`, `@0x800140f0`). Reihenfolge 10, 11, 12, 13 - ein in Bild N von Thread 10 gestarteter
Thread 11 laeuft noch im selben Bild.

**Bildtakt.** Die Bildwechsel-Routine ruft `VSync(0x800dfc1a)` (`@0x8002b994 lbu a0`, `@0x8002b998 jal 0x80085ea0`).
Door_move schreibt 0 (`@0x80013f24`), Door_exit schreibt 2 (`@0x80014214`). Die Sequenz laeuft also mit
`VSync(0)`, das Spiel danach wieder mit `VSync(2)`. Der Test auf Flag 0x8000 liegt VOR dem ersten
Scheduler-Lauf; zu dem Zeitpunkt sind alle Objekte genullt (3.2). Statisch gelesen greift er deshalb nie.
Dynamisch nicht nachgemessen (siehe Offen 1).

### 3.4 Blende

Kanalsatz 0x4c B ab `0x800dfc1c`: +0 Pegel (Bit 15 = fertig), +2 Schritt, +4 Art, +5..7 RGB-Maske.
Takt `FUN_8002c378`: Helligkeit = Pegel >> 7, danach Pegel += Schritt.

- Einblenden: `Sce_fade_set(ch 0, Art 2, Maske 7, Schritt -512)` + `Sce_fade_adjust(ch 0, 7168)`.
  Art 2 = subtraktiv. Verlauf `[SIM]`: 56, 52, ... 4, 0 in den Bildern 0..14 = **15 Bilder**.
  Alle 141 simulierten Sequenzen blenden mit Schritt -512 ein. Im Skripttext: 122-mal -512, 6-mal -1024
  (`DOOR20/21/32/34/36` Skript 0 und `DOOR15` Skript 9); 123 von 154 `Sce_fade_adjust` setzen 7168,
  die uebrigen 31 sind eine Rampe 224..6944 in Schritten von 224.
- Ausblenden: `Sce_fade_set(..., +1024)`: 0, 8, 16, ... -> nach **32 Bildern** Bit 15.
  `[SIM]` 123 Sequenzen mit +1024, 6 mit +2048 (16 Bilder), 12 ohne eigenes Ausblenden
  (`DOOR1F`, `DOOR28`, `DOOR2B`, `DOOR2D` Typ 0/2/4).
- Door_exit wartet auf Bit 15 und stellt dann Pegel 0x7fff (voll schwarz).

`Sce_fade_adjust` (0x74) schiebt den PC um **4**, nicht 5 (`@0x80058004 addiu s0,s0,4`). Mit 5 verrutscht
jedes Tuerskript um ein Byte (`74 00 00 1c | 09 0a 46 00`).

### 3.5 Ton

- `Se_on` (0x36, 12 B): `pc[1]` = VAB-Platz, `s16@2` = Tonnummer, `s16@4` Low-Byte = Positionsquelle
  (0 = Ursprung), `@0x80056530 jal 0x8005ba28`. 128 Vorkommen, davon 125 mit Tonnummer 0.
- Tonnummer n = Eintrag n des 16-B-Tonkopfs am Dateianfang (`FUN_8005ba28`: Kopf + n*4, -1 = leer).
  Eintrag 0 spielt das Skript zu Beginn der Oeffnung, Eintrag 1 spielt Door_exit nach der Sequenz.
  "Oeffnen" und "Schliessen" sind Deutung aus dem Zeitpunkt; abgehoert wurde nichts.
- Der Oeffnungston kommt erst, wenn der Ton geladen ist: Hilfsskript wartet, solange var 13 == 1
  (`Ifel_ck / Cmp(var 13 == 1) / Evt_next / Goto`, in drei Archiven als `While`), genau ein solches
  `Cmp` in jedem der 55 Archive.
- **Schliesston** spielt nicht das Skript, sondern Door_exit: `@0x800141d8 lhu v0,584(v0)`,
  `@0x800141f0 jal 0x8005ba28` mit `a0 = 0x10000` (Ton 1) an der neuen Spielerposition `0x800cfc30`.
  Der Merker wird von jedem `Door_model_set` mit Flag 0x800 gesetzt (`@0x80014c90..a8`);
  100 von 378 Saetzen tragen das Bit.
- Erstes `Se_on` je Sequenz `[SIM]`, ohne Ladewarten: Bild 60 (30x), 70 (45x), 100 (10x), 110 (17x), 120 (12x).

### 3.6 Door_exit `0x8001417c`

```
warten, bis Blende Kanal 0 fertig            @0x80014184 jal 0x8002c350 / @0x80014194 Task_sleep(1)
Blende Kanal 0: Art 2, Maske 7, Pegel 0x7fff @0x800141a4..c4
wenn Schliesston-Merker: Ton 1 spielen       @0x800141d8..f4
0x800dfc1a = 2                               @0x8001420c / @0x80014214
0x800cfb74 &= ~0x2000000                     @0x800141f8..0x80014220
Task beenden                                 @0x8001421c jal 0x80031fe4
```

### 3.7 Zeitachse der Standardtuer (`DOOR00`, Typ 0) `[SIM]`

| Bild | Ereignis |
|---|---|
| 0 | Blatt (Objekt 0, Mesh 0) bei (2000,3790,2048), Knauf (Objekt 1, Mesh 1) als Kind bei (130,-3224,-3372); Einblenden beginnt |
| 0..69 | Tuer steht (`Sleep 70`) |
| 14 | Einblenden fertig |
| 70 | `Se_on` Ton 0; Thread 11 dreht den Knauf: rot-x +50 je Bild, 30 Bilder -> 1500 |
| 120 | Thread 11 (neu gestartet) beginnt das Blatt zu drehen (rot-y) |
| 120..139 | Blatt auf 80 (= 7,0 Grad), dann 18 Bilder Pause |
| 158..260 | Blatt beschleunigt und bremst bis rot-y = **1030** (= 90,5 Grad) |
| 181..299 | Objekt 0 faehrt auf die Kamera zu: x 2000 -> 11450 (Kamera steht bei x = 10000) |
| 270 | Ausblenden beginnt (+1024) |
| 300 | letztes Bild; Thread 10 endet nach **301** Bildern |
| danach | Door_exit (3.6) |

Ueber alle 141 Sequenzen: **165 / 301 / 451** Bilder (kleinste / Median / groesste); 67 Sequenzen haben 301.

Nachmessen: `python re15_port/tools/tor/tuerskript_dump.py sim info/re2leon/COMMON/DOOR/DOOR00.DO2 0`
(drittes Argument = Bilder Ladewarten).

---

## 4. Wie das richtige Skript gewaehlt wird

`Door_aot_set` (0x3B, 32 B; RE2-Handler `0x80054be4`, Satz = pc+2 `@0x80054c30`, Vorschub 32 `@0x80054c40`).
Payload = Satz + 12 = **pc + 14**:

| pc+ | Payload+ | Feld | Leser |
|---|---|---|---|
| 14,16,18 | 0,2,4 | Zielposition x,y,z | `FUN_80026b7c` |
| 20 | 6 | Zielrichtung | " |
| 22,23,24,25 | 8,9,10,11 | Stage, Raum, Cut, Etage | " |
| **26** | **12** | **Tuertextur-Typ = welches Archiv** | `@0x80015088 lbu v1,12(v0)` |
| **27** | **13** | **Tuertyp = welche Variante** (Bit 7 extra) | `@0x80013e5c lbu v0,13(a0)` |
| 28 | 14 | (nicht gelesen in den hier untersuchten Funktionen) | - |
| 29 | 15 | Schluessel-Flag (Bit 7 = verschlossen, & 0x3f = Index) | `@0x800515a8` |
| 30 | 16 | Schluesselart (254/255 Sonderfaelle) | `@0x800515d0` |

Variablen (Tabelle `0x800d47ec`, s16, `@0x80054098`):

| Index | Adresse | Wert | Beleg |
|---|---|---|---|
| 12 | 0x800d4804 | Payload+13 & 0x7f | `@0x80013e6c andi v0,v0,0xff7f`, `@0x80013e74` |
| 13 | 0x800d4806 | 1 = Ton laedt noch, 0 = fertig | `@0x80013e60/68`, `@0x80013f68` |
| 14 | 0x800d4808 | Payload+13 & 0x80 | `@0x80013e84/8c` |
| 15 | 0x800d480a | Payload+12 | `@0x80013e90/98` |

Skript 0 ist der Verteiler, z.B. `DOOR00` Datei 0x501a:
`13 0c 16 00 | 14 00 04 00 00 00 | 18 01 | 1a 00 | 14 00 04 00 01 00 | 18 02 | 1a 00 | 16 00 | 01 00`
= `Switch(var 12)`, `Case 0 -> Gosub 1`, `Case 1 -> Gosub 2`.

- 50 von 55 Archiven verteilen so ueber var 12. 5 Archive (`DOOR20/21/32/34/36`) haben nur 2 Skripte und
  bauen direkt in Skript 0 auf.
- `Case` mit Groesse 0 faellt durch (`DOOR27`: Werte 0,1,2,3 -> alle Skript 1).
- Skripte je Archiv: 2 bis 18, zusammen 498; davon **127** Aufbauskripte (= enthalten `Door_model_set`).
- var 14 wird 8-mal abgefragt (`Cmp(var 14 == 0)`), var 15 in keinem Tuerskript.

**Zensus RE2-Raeume** (495 RDT, 572 `Door_aot_set`/`_4p`, 0 Desyncs): Payload+12 belegt 0..54 (= 55 Archive),
Payload+13 & 0x7f: 0 (253x), 1 (178x), 2 (27x), 3 (22x), 4 (44x), 5 (45x), 7 (1x), 8 (2x); Bit 7 in 14 Saetzen
(nur Typ 1 und 5). Die Werte je Archiv decken sich mit den `Case`-Werten des jeweiligen Skripts 0.

**Richtung** `[SIM]` an `DOOR00`: Typ 0 stellt das Blatt mit rot-y 0 bei z = 2048 auf und dreht auf 1030;
die freie Kante endet bei x = -3599 relativ zur Angel = **von der Kamera weg**. Typ 1 stellt mit rot-y 2048
bei z = -1600 auf und dreht auf 3078; freie Kante bei x = +3599 = **zur Kamera hin**. Typ 0/1 sind also
die beiden Seiten derselben Tuer (druecken / ziehen). 2/3 sind bei Doppeltueren die zweite Haelfte,
4/5 Treppen und Leitern.

Nachmessen: `python re15_port/tools/tor/tuerskript_dump.py dis <DO2> 0`; Zensus-Skript siehe Abschnitt 10.

---

## 5. Der Platzierungs-Opcode feldgenau

### 5.1 RE2: 0x4D, Handler `0x80014ba4`, 22 B

| pc+ | Breite | Ziel im Objekt | Bedeutung | Beleg |
|---|---|---|---|---|
| 1 | u8 | - | Objektnummer 0..9, Zeiger aus `0x800d4dd8[n]` | `@0x80014bb8`, `@0x80014bcc` |
| 2 | u8 | +0x08 (sb) | Kennung (kein Leser gefunden) | `@0x80014bd0/d8` |
| 3 | u8 | +0x10e (sh) | Bildnummer fuer Flag 0x400 | `@0x80014bdc/e4` |
| 4 | u8 | +0x00 (sw) | an/aus; gezeichnet nur wenn != 0 | `@0x80014be8/f0`, `@0x8001429c` |
| 5 | u8 | +0x146 (sh) | **Mesh-Index** | `@0x80014bf4/fc`, `@0x80014cb4` |
| 6 | u16 LE | +0x144 (sh) | **Flags** | `@0x80014c00/08` |
| 8 | s16 LE | +0x10 (sw) | Attribut; Bits 0xC gehen als ABR in die Pakete | `@0x80014c0c/14`; `@0x8002cc44 lw v0,0(a0)`, `@0x8002cc50 andi v0,v0,0xc`, `@0x8002cc58 sll t7,v0,19` |
| 10,12,14 | s16 LE | +0x38,+0x3c,+0x40 (sw) | **Position x,y,z** | `@0x80014c18..38` |
| 16,18,20 | u16 LE | +0x74,+0x76,+0x78 (sh) | **Drehung x,y,z**, 4096 = 360 Grad | `@0x80014c3c..60` |
| - | - | +0x80 (sw) | Elternmatrix-Zeiger | `@0x80014c8c` |
| - | - | - | PC += 22, dann `FUN_80014b40(obj, mesh)` | `@0x80014cac`, `@0x80014cb8` |

Kein Feld fuer Geschwindigkeit, kein Feld fuer Dauer.

Flags:

| Bit | Wirkung | Beleg | Saetze (von 378) |
|---|---|---|---|
| 0x000f | Nummer des Elternobjekts | `@0x80014c6c andi v0,v1,0xf` | - |
| 0x0010 | **Elternmatrix** = zusammengesetzte Matrix des Elternobjekts (Objekt + 0x54); sonst Kameramatrix `0x800dcba8` | `@0x80014c64`, `@0x80014c80 addiu v0,v0,84`, `@0x80014c84/88` | 213 |
| 0x0020 | Dreiecke unterteilen (`DivideGT3`) | `@0x80014820` | 83 |
| 0x00c0 | Ordnungstabelle: 0x80 -> `0x800ce22c`, Platz (otz>>7)+511; 0xc0 -> `0x800ce22c`, Platz otz>>7; 0x40 -> `0x800ce2b0`, Platz otz>>12; Dreieck entfaellt bei otz < 64 | `@0x800149a8..ac`, `@0x800149bc..0x80014a24` | 0x80: 320, 0x40: 197 |
| 0x0100 | pulsierende Farbe | `@0x80014468` | 2 |
| 0x0200 | kein Leser in `FUN_80014234`/`FUN_8001468c` gefunden | - | 148 |
| 0x0400 | im Bild pc[3] Bit 0x80 umschalten | `@0x800146c8..0x80014704` | 71 |
| 0x0800 | Schliesston beim Verlassen | `@0x80014c90..a8` | 100 |
| 0x1000 | hellere Hintergrundfarbe 0x88 | `@0x800142b4` | 1 |
| 0x2000 | Ecken-z * var 4 / 256 (schreibt in die Mesh-Daten) | `@0x80014794..f0`, Faktor `0x800d47f4` | 19 |
| 0x4000 | Paketcode 0x36 statt 0x34 (halbtransparent) | `@0x8001473c..48` | 5 |
| 0x8000 | Bildtakt (greift statisch gelesen nie, 3.3) | `@0x80013f14` | 6 |

Attribut ist in allen 378 Saetzen 0x0010.

Matrix je Objekt (`FUN_80014234`): lokal = `RotMatrix(Drehung)` (`@0x800142a4 jal 0x8008e1f4`) mit
Translation = Position; zusammengesetzt = Eltern * lokal, t = Eltern * Position + Eltern.t
(`@0x800144c4..0x80014608`). Die Position geht als 16-Bit-Vektor in die GTE (`@0x800145d8/dc lhu`).
`RotMatrix` rechnet **M = Rx * Ry * Rz** (Formel aus der Routine `0x8008e1f4`; Sinustabelle `0x800adeac`
= (sin,cos)-Paare, Eintrag 1024 = (4096,0)). 57 Saetze drehen um mehr als eine Achse, die Reihenfolge
ist also nicht egal.

Instanzen: dasselbe Mesh darf mehrfach gesetzt werden. `DOOR10` setzt 10 Objekte, neun davon Mesh 0,
jedes als Kind des vorigen bei (0,0,-650). Objekte je Sequenz `[SIM]`: 1 (17x), 2 (44x), 3 (35x), 4 (21x),
5 (10x), 6 (12x), 10 (2x).

### 5.2 RE1.5: 0x4F, Handler `LAB_80016f20`, 22 B

Tabelle `0x800744a8[0x4f]` = `0x80016f20`.

| pc+ | Ziel | Beleg | Unterschied zu RE2 |
|---|---|---|---|
| 1 | Zeiger aus `0x800b23f4[n]` | `@0x80016f30..48` | nur 4 Objekte zu 0x90 B |
| 2 | +0x08 (sb) | `@0x80016f4c/54` | - |
| 3 | +0x09 (sb) | `@0x80016f58/60` | RE2: +0x10e (sh) |
| 4 | +0x00 (sw) | `@0x80016f64/6c` | - |
| 5 | +0x8e (sb) Mesh | `@0x80016f70/78` | - |
| 6 | +0x8c (sh) Flags | `@0x80016f7c/84` | - |
| 8 | +0x0c (sw) | `@0x80016f88/90` | - |
| 10,12,14 | +0x34,+0x38,+0x3c Position | `@0x80016f94..b4` | - |
| 16,18,20 | +0x68,+0x6a,+0x6c Drehung | `@0x80016fb8..dc` | - |
| Flags | **Elternbit 0x8, Nummer = Flags & 7**, Ziel Objekt + 0x48 | `@0x80016fe4 andi a3,v0,0x8`, `@0x80016ff4 andi v0,v0,0x7`, `@0x80017018 addiu v0,v0,72` | RE2: 0x10 und & 0xf |
| - | PC += 22, `FUN_80017048(n, mesh)` | `@0x80017020`, `@0x8001702c` | - |

Der RE1.5-Renderer kennt die Flags 0xc0 (`@0x80016d88`), 0x20 (`@0x80016e44`), 0x200 (Pulsieren,
`@0x80016a78`) und Attribut & 0x10 (`@0x80016a38`). 0x400, 0x800, 0x1000, 0x2000, 0x4000 kommen nicht vor.

### 5.3 Bewegung

| Opcode | Wirkung | Beleg |
|---|---|---|
| 0x2E `Work_set(5, n)` | Thread arbeitet auf Tuerobjekt n; loescht die 12 Geschwindigkeits-/Beschleunigungswerte | `@0x80055908..1c`, Sprungtabelle `0x800111f0[4]` = `0x800559b0`, `@0x800559bc lw v0,19928(at)` |
| 0x2F `Speed_set(i, v)` | Thread + 0x158 + 2i = v | `@0x80055aac sh a1,344(v1)` |
| 0x30 `Add_speed` | Position += Wert 0..2, Drehung += Wert 3..5 | `@0x80055ab0..0x80055b10` |
| 0x31 `Add_aspeed` | Wert i += Wert i+6 (i = 0..5) | `@0x80055b2c..90` |

Index 0,1,2 = Geschwindigkeit x,y,z; 3,4,5 = Drehgeschwindigkeit x,y,z; 6,7,8 und 9,10,11 = die
zugehoerigen Beschleunigungen. Haeufigste Indizes: 0 (250x), 6 (170x), 10 (157x), 4 (110x), 8 (88x).

RE1.5 hat dieselben vier Handler mit denselben Thread-Offsets, nur mit den Objekt-Offsets +0x34/+0x68
statt +0x38/+0x74 (`0x80040d2c`, `0x80040f14`, `0x80040f40`, `0x80040fd4`; `Work_set` Art 5 ->
`0x800b23f4[n]` `@0x80040df4..e04`).

---

## 6. Alle Opcodes der Tuerskripte

498 Skripte, 11047 Befehle, 38 verschiedene Opcodes. Jede Laenge ist am Handler gelesen (Spalte Beleg
in `tuersequenz.json` -> `histogram`).

| Op | Name | Anzahl | Laenge |
|---|---|---|---|
| 0x02 | Evt_next | 1751 | 1 |
| 0x30 | Add_speed | 1574 | 1 |
| 0x2F | Speed_set | 1039 | 4 |
| 0x00 | Nop (Fuellbyte zur Ausrichtung) | 926 | 1 |
| 0x0D / 0x0E | For / Next | 843 / 843 | 6 / 2 |
| 0x31 | Add_aspeed | 591 | 1 |
| 0x01 | Evt_end | 498 | 2 |
| 0x09 | Sleep (+ 0x0A Sleeping 3 B eingebettet) | 459 | 4 |
| **0x4D** | **Door_model_set** | **378** | 22 |
| 0x18 | Gosub | 347 | 2 |
| 0x2E | Work_set | 294 | 3 |
| 0x53 | Sce_fade_set | 260 | 6 |
| 0x04 | Evt_exec | 248 | 4 |
| 0x74 | Sce_fade_adjust | 154 | **4** |
| 0x14 | Case | 141 | 6 |
| 0x36 | Se_on | 128 | 12 |
| 0x1A | Break | 126 | 2 |
| 0x23 | Cmp | 63 | 6 |
| 0x06 / 0x08 / 0x07 | Ifel_ck / Endif / Else_ck | 60 / 52 / 8 | 4 / 2 / 4 |
| 0x17 | Goto | 52 | 6 |
| 0x13 / 0x16 / 0x15 | Switch / Eswitch / Default | 50 / 50 / 4 | 4 / 2 / 2 |
| 0x26, 0x25, 0x24 | Calc, Copy, Save | 20, 8, 8 | 6, 3, 4 |
| 0x3D, 0x35, 0x34 | Member_copy, Member_set2, Member_set | 16, 16, 8 | 3, 3, 4 |
| 0x8A, 0x8C, 0x8B | Vibration | 10, 7, 5 | 6, 8, 6 |
| 0x1D | Work_copy (schreibt in den Skripttext) | 4 | 4 |
| 0x0F / 0x10 | While / Ewhile | 3 / 3 | 4 / 2 |

`Evt_exec` nennt den Thread ausdruecklich: 11 (141x), 12 (75x), 13 (31x), 14 (1x = "ersten freien nehmen",
`@0x800531ac sltiu v0,a0,0xe`).

Nachmessen: `python re15_port/tools/tor/tuerskript_dump.py stat info/re2leon/COMMON/DOOR`.

---

## 7. Pruefung der Vorarbeit

| Vorarbeit | Befund |
|---|---|
| Entpackte `info/re2leon/COMMON/DOOR/DOORxx/*.c` | **Nicht verwendbar.** Alle 16-Bit-Werte sind big-endian gelesen. `DOOR0001.c`: `Sleep(17920)` - Bytes `09 0a 46 00`, richtig 70. `Obj_model_move(0,0,0,1,0,40970,4096,-12281,...)` - Bytes `a0 0a | 10 00 | d0 07`, richtig Flags 0x0aa0, Attribut 0x0010, x = 2000. `Case 256` ist `Case 1`. |
| `SCDScriptDisassembler.parseDoorObjModelMoveCommand` | Feldaufteilung (5 Bytes, dann 8 LE-Worte, das erste vorzeichenlos) passt zum RE1.5-Handler 0x4F und zum RE2-Handler 0x4D. |
| Kommentar ueber `readDoorShort` (Zeile 3058) | Die "Gegenprobe" ist falsch herum: die Bytes in `DOOR0001.scd` @6 sind `a0 0a`, nicht `0a a0`. LE = 0x0aa0, Bit 0x8 ist NICHT gesetzt. Ausserdem stammt das Beispiel aus einer RE2-Datei; dort ist der Opcode 0x4D und das Elternbit 0x10. |
| Aufgabenstellung "Aufbau-Opcode 0x4F" | Gilt fuer die RE1.5-EXE. In den 55 RE2-Archiven steht ausnahmslos 0x4D. |
| Notiz "RE1.5-Tuerwechsel laedt kein DO2" | Widerlegt, siehe 8. |

---

## 8. RE1.5-Seite: Funktion gegen Funktion

| Rolle | RE2 | RE1.5 | Unterschied |
|---|---|---|---|
| Start aus dem Raumwechsel | `FUN_80026b7c` `@0x80026bfc` | `FUN_8001d600`: `@0x8001d618 bne v0,zero,0x8001d82c` (Payload-Zeiger `0x800ac9a8` != 0), `@0x8001d830 jal 0x80021634` (schwarz), `@0x8001d838 jal 0x800171f4`, `@0x8001d844/48 jal 0x80029a98` mit a0=1, a1=`0x80016188` | gleich; RE1.5 laedt das Archiv VOR dem Taskstart |
| Payload merken | `LAB_80051514` | `LAB_800430bc` `@0x800430c4 sw a0,0x800ac9a8` | RE1.5 ohne Schlossfelder |
| Door_main | `0x80013bc4`, Tabelle `0x8009a7b4` | `0x80016188`, Tabelle `0x80071d30` = `0x800161e0, 0x800164c8, 0x80016664, 0` | gleich |
| Archiv laden | `FUN_80015064`, Datei aus `0x8009a4b0[Payload+12]`, 55 Eintraege | `FUN_800171f4`, Datei aus `0x80071d2c[Payload+12]` (`@0x8001720c lbu v0,12(v0)`), Tabelle = `25 00 00 00` = **ein** Eintrag (Datei 37) | RE1.5: ganze Datei in einem Stueck |
| Ton laden | `FUN_80014cd0` | `FUN_800170e0` (`@0x800162dc`) | beide laden VH+VB |
| Door_init | `FUN_80013c1c` | `FUN_800161e0` | siehe unten |
| Bildschleife | `FUN_80013eb4` | `FUN_800164c8` | RE1.5 ohne Bildzaehler, ohne Ladewarten, ohne Text |
| Scheduler | `FUN_80014058` | `FUN_80016518`, Threads 10..13, Schritt 368, Dispatch `0x800744a8` | RE1.5 schaltet einen Thread zusaetzlich ab, wenn sein naechstes Byte 0x01 ist (`@0x80016614..2c`) |
| Matrizen | `FUN_80014234`, 10 x 0x14c | `FUN_800166c4`, **4 x 0x90** (`@0x800161e4 ori a3,zero,0x3`, `@0x8001622c addiu a1,a1,-144`) | Objektzahl |
| Renderer | `FUN_8001468c` | `FUN_80016b54` | beide zeichnen nur die Dreiecksgruppe |
| Mesh binden | `FUN_80014b40` | `FUN_80017048` (`@0x8001705c..6c` * 56) | RE1.5 legt zusaetzlich Viereck-Pakete an (`@0x800170bc jal 0x80025a98`), zeichnet sie aber nie |
| Platzierung | 0x4D `0x80014ba4` | 0x4F `0x80016f20` | 5.2 |
| Exit | `0x8001417c` | `0x80016664` | RE1.5 ohne Schwarzstellen, ohne Schliesston |
| Bildtakt | `0x800dfc1a` = 0 / 2 | `0x800b5456` = 0 (`@0x80016208`) / 2 (`@0x800166a4`) | gleich |

Door_init im Vergleich:

| Groesse | RE2 | RE1.5 | Beleg RE1.5 |
|---|---|---|---|
| Kamera Auge | (10000,0,0) | **(30000,0,0)** | `@0x80016460 ori v0,zero,0x7530` -> `0x800b2210` |
| Kamera Ziel | (0,0,0) | **(22000,0,0)** | `@0x80016468 ori v0,zero,0x55f0` -> `0x800b221c` |
| H | 290 | **1000** | `@0x80016434 jal 0x80066c40` (InitGeom), darin `@0x80066c88 addiu t0,zero,1000` + `@0x80066c8c ctc2`; kein Aufruf von `0x80066c30` im Tuercode (dessen Aufrufer: `0x80021e6c`, `0x80046124`) |
| Bildmitte | (160,120) vom Spiel | (160,120) | `@0x8001643c/44` |
| Texturziel | 0x1f15 | 0x1f15 | `@0x80016344 ori v0,zero,0x1f15` |
| MD1 umsetzen | tpage +21, CLUT-Zeile +31 | **+0, +0** | `@0x80016370 addu a2,zero,zero`, `@0x8001639c addu a3,zero,zero`, `@0x80016398 jal 0x80022150` |
| Tuertyp-Variable | var 12 (& 0x7f), dazu 13/14/15 | **var 7**, ungefiltert, sonst keine | `@0x800164a8 lbu v0,13(v0)`, `@0x800164b4 sh v0,0x800b0fde`; Tabellenbasis `0x800b0fd0` (`@0x8003fad0`) |
| Blende beim Start | vom Skript | Kanal 0 auf "fertig" | `@0x80016420 jal 0x80021764` |

**Erreichbarkeit.** Die Maschine laeuft bei jedem Tuer-AOT. Thread 10 startet Skript 0 = `01 00`;
`Evt_end` auf Ebene 0 schaltet den Thread ab (`@0x8003f204 sb zero,1(a1)`), die Schleife
`@0x800164c8..0x80016504` endet nach **1** Durchlauf, gezeichnet wird nichts (alle vier Objekte aus).
Der sichtbare Uebergang ist deshalb nur schwarz + Blende.

**Gleiche Rahmung.** Die RE2-Aufstellung von `DOOR00` Typ 0 durch die RE1.5-Kamera gesehen `[SIM]`:
Blatt x 104,3..233,5, y 19,1..256,1. Durch die RE2-Kamera: x 102,8..235,6, y 16,3..260,0.
290/8000 = 0,03625 gegen 1000/28000 = 0,03571 Pixel je Einheit. RE1.5 war also fuer dieselbe
Aufstellung (Tuer bei x = 2000) ausgelegt, nur mit laengerer Brennweite.

**Was RE1.5 gegenueber RE2 fehlt:**
1. Skripte (das Archiv hat 4 Byte Skriptblock).
2. Die Texturverschiebung des MD1: die Textur liegt auf Seite 21, das Modell zeigt weiter auf Seite 0
   (MD1: tpage 0x80, CLUT 0x7800; nach RE2-Umsetzung 0x95 / 0x7fc0).
3. Tuertyp und Tuertextur-Typ in den Raumdaten (alle 649 Saetze: Payload +12..+17 = 0).
4. Schliesston, Schwarzstellen am Ende, Ladewarten, Bildzaehler, Flags 0x400/0x800/0x1000/0x2000/0x4000.
5. Sechs Objektplaetze (4 statt 10).

**Nicht binaervertraeglich.** Ein RE2-Skript laeuft auf der RE1.5-Maschine nicht: anderer Opcode
(0x4D ist in RE1.5 `0x800408a8`, 10 B), anderes Elternbit, andere Variable, 0x74/0x8A..0x8C liegen
jenseits der 95 RE1.5-Opcodes, 0x53 ist in RE1.5 ein anderer Befehl (`0x80040e18`).

---

## 9. Anforderungen an ein neues Modell

### 9.1 Koordinatensystem

Aus 3.2: Blick entlang -x. **Dicke entlang x** (Vorderseite bei +x), **Hoehe entlang -y**
(Unterkante bei y = 0), **Breite entlang z**. Bild-rechts = +z.

### 9.2 Ursprung = Drehachse

Die Drehung wirkt auf die Mesh-Ecken, danach kommt die Position dazu (5.1). Das Mesh dreht sich also
um seinen eigenen Ursprung. Belege an den Vorbildern:

| Mesh | Ausdehnung | Ursprung liegt | Dreht um |
|---|---|---|---|
| Standardblatt (`DOOR00` Mesh 0, MD1 @Datei 0x5224) | x -145..143, y -6600..2, z -3599..0 | auf der Angel, unten, in Blattmitte der Dicke | y (Index 4) |
| Knauf (`DOOR00` Mesh 1) | x 1..306, y -107..107, z -106..106 | auf der Knaufachse an der Blattoberflaeche | x (Index 3) |
| Klappe (`DOOR1E` Mesh 0) | x 0..300, y 0..1200, z 3..1797 | an der Oberkante | z (Index 5 / 11) |

Das Blatt laeuft von der Angel nach **-z**. Ein Tor, das in der Aufstellung "Typ 0" rechts angeschlagen
ist, braucht also z von -Breite bis 0.

### 9.3 Netz

- **Nur Dreiecke.** Beide Renderer lesen ausschliesslich die Dreiecksgruppe des Mesh-Eintrags
  (RE2 `@0x80014650 lw a2,0(s1)`, eine Schleife bis `@0x80014b04`; RE1.5 `@0x80016ba4 lw v1,4(s4)`,
  eine Schleife `@0x80016c00`..`@0x80016ee4`). Alle 55 RE2-Archive: 8560 Dreiecke, 0 Vierecke.
- Mesh-Eintrag 56 B, Dreiecksgruppe @+0x00, MD1-Kopf 12 B (2.2, 3.2).
- **Normalen je Ecke noetig**: beleuchtet wird mit `ncct` ueber drei Normalen (`@0x80014958`).
- **Umlaufsinn**: nach `nclip` (`@0x800148d0`) wird bei negativem Ergebnis verworfen (`@0x800148f8 bgez`).
  Nachgerechnet am Standardblatt im Bild 0: gezeichnet werden genau die zwei Dreiecke der Flaeche
  x = +143 (MAC0 = +32370), die zehn anderen sind negativ. Von aussen gesehen laufen die Ecken im
  Bild im Uhrzeigersinn (y nach unten).
- Koordinaten und Positionen sind 16 Bit.
- Dreieckszahl je Archiv 12..490 (Summe der Meshes); je Sequenz durch Mehrfachsetzen mehr.

### 9.4 Textur

Alle 55 RE2-TIM und das RE1.5-TIM: 8 Bit, **128 x 256 Pixel**, **eine** CLUT 256 x 1 (Kopf `10 00 00 00 09 00 00 00`,
CLUT-Rechteck 0,480,256,1, Bild 64 x 256 Worte). Im MD1 stehen tpage 0x80 und CLUT 0x7800; der Lader
setzt auf 0x95 / 0x7fc0 um (3.2). Ausnahme `DOOR28`: steht schon auf 0x95 / 0x7fc0.

### 9.5 Groesse im Bild

Pixel = H * Einheiten / Tiefe, H = 290, Tiefe = 10000 - x.

| Tiefe | Pixel je Einheit | 240 Zeilen entsprechen | 320 Spalten entsprechen |
|---|---|---|---|
| 8000 (Standard, x = 2000) | 0,03625 | 6621 | 8828 |
| 4000 | 0,0725 | 3310 | 4414 |
| 3400 (`DOOR1E`, x = 6600) | 0,0853 | 2814 | 3752 |

Standardaufstellung: Position x = 2000 in 122 von 165 Wurzel-Saetzen, y = 3800 in 108.
Fuer das Standardblatt (in 36 Archiven Mesh 0): Typ 0 meist (2000,3800,2048) mit Drehung 0,
Typ 1 meist (2000,3800,-1600) mit rot-y 2048.

Bildrechteck des Standardblatts im ersten Bild `[SIM]`: **132,8 x 243,6 Pixel**
(x 102,8..235,6, y 16,7..260,3). Die Bodenlinie y = 3800 liegt bei Bildzeile 257,8, also 17,8 Zeilen
unter dem unteren Bildrand; die Tuer fuellt die Bildhoehe.

Fuer ein niedriges Tor folgt daraus:

- In der Standardaufstellung (Tiefe 8000, Boden y = 3800) liegt die Oberkante eines Tors der Hoehe h
  bei Bildzeile 257,8 - 0,03625 * h. Knaufhoehe des Standardblatts = 3224 Einheiten ueber der
  Unterkante (Knauf-Position y = -3224, 48,8 % der Blatthoehe). Ein Tor dieser Hoehe reicht bis
  Zeile 140,9 und belegt sichtbar die Zeilen 141..240.
- RE2 stellt niedrige Teile naeher: `DOOR1E` (Klappe 1794 breit, 1200 hoch) steht bei x = 6600 und
  nimmt 167,0 x 111,7 Pixel ein (x 73,5..240,5, y 58,6..170,3, Tiefe 3100..3400).
- Obergrenzen, damit das Teil ganz im Bild steht: Hoehe <= 240 * Tiefe / 290, Breite <= 320 * Tiefe / 290
  (Tabelle oben).

Die Wahl der Tiefe fuer das Tor ist eine Gestaltungsentscheidung; belegt sind nur die beiden Vorbilder
und die Formel.

---

## 10. Nachmessen

```bash
T=re15_port/tools/tor/tuerskript_dump.py
python $T container info/re2leon/COMMON/DOOR/DOOR00.DO2
python $T container re15_port/shared_assets/PSX/DOOR/DOOR00.DO2
python $T dis  info/re2leon/COMMON/DOOR/DOOR00.DO2          # alle Skripte feldgenau
python $T stat info/re2leon/COMMON/DOOR                      # Opcode- und Flag-Statistik
python $T sim  info/re2leon/COMMON/DOOR/DOOR00.DO2 0         # Zeitachse Typ 0
python $T json info/re2leon/COMMON/DOOR build/tor_1170/tuersequenz.json re15_port/shared_assets/PSX/DOOR/DOOR00.DO2

R2=.claude/skills/re15-psx-disasm/scripts/re2_disasm.py
R1=.claude/skills/re15-psx-disasm/scripts/re15_disasm.py
python $R2 dis 0x80013c1c 170      # Door_init
python $R2 dis 0x80013eb4 100      # Door_move
python $R2 dis 0x8001417c 46       # Door_exit
python $R2 dis 0x80014ba4 75       # Opcode 0x4D
python $R2 bytes 0x8001082c 32     # Kameradaten
python $R1 dis 0x800161e0 190      # Door_init RE1.5
python $R1 dis 0x80016f20 74       # Opcode 0x4F
python $R1 dis 0x8001d82c 16       # Start aus dem Raumwechsel
```

Zensus der Raumdaten (liest nur): RE2 ueber `analysis/nutzer_batch_2026-08-27/tools/re2_scd_walk.py`
(`all_subs`, `walk`), je `Door_aot_set` Payload = pc+14 (0x3B) bzw. pc+22 (0x68); RE1.5 ueber
`re15_scd_walk.py` (`blocks`, `oplen`) auf `re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT`.
Die beiden Wegwerf-Skripte lagen im Scratchpad und sind nicht im Repo.

---

## 11. Offen

1. **Bildtakt dynamisch.** `VSync(0)` ist gelesen, die tatsaechliche Bildrate der Sequenz ist nicht im
   Emulator gemessen. Ebenso offen, ob Flag 0x8000 wirklich nie greift.
2. **Ladezeit.** Wie viele Bilder das Warten auf den Ton dauert, haengt vom Laufwerk ab. Die Simulation
   rechnet mit 0; liegt die Ladezeit ueber dem ersten `Sleep`, verschiebt sich alles danach.
3. **Flag 0x0200** (148 Saetze): kein Leser gefunden. **Feld pc[2]** (Objekt + 8): kein Leser gefunden.
4. **Tiefengrenzen.** Dreiecke entfallen bei otz < 64 (`@0x800149a8 srl v0,a0,6`). Welcher
   Mittelungsfaktor (ZSF3) in RE2 gilt, also welcher Welt-Tiefe das entspricht, ist nicht gelesen.
   Betrifft die Durchfahrt, nicht die Aufstellung.
5. **Massstab Tuerraum gegen Spielraum.** Die Sequenz hat ihre eigene Kamera, ein Bezug der
   Tuer-Einheiten zu den Raum-Einheiten ist nirgends im Code. Die Knaufhoehe 3224 ist der einzige
   belegte Anhalt fuer "huefthoch".
6. **Zuordnung Tuer -> Modell und Richtung in RE1.5.** Die Raumdaten tragen nichts (alle Felder 0).
   Fuer ROOM1170 Slot 0 (Datei 0x1206) und Slot 6 (Datei 0x135a) muss die Zuordnung ausserhalb der
   Originaldaten entstehen.
7. **Skript fuer das Tor.** Kein RE2-Archiv ist ein huefthohes Schwingtor. Die Schwingbewegung des
   Standardblatts (Skript 5 von `DOOR00`, 1030 Einheiten in 141 Bildern) ist uebertragbar, Aufstellung
   und Durchfahrt muessen neu gesetzt werden.
8. **Textzeilen** fuer Tuertextur-Typ 50 und 52 (Bilder 41..259) sind nur als Aufruf gelesen, der
   Inhalt nicht.
9. **RE1.5-Kopie in `FUN_800171b4`** (`@0x800163d0`): kopiert ab dem Tabelleneintrag statt ab dem
   Skript. Zweck nicht geklaert; bei 4 Byte Skriptblock ohne Wirkung.
10. **Toene nicht abgehoert.** Welche Rolle Ton 0 und Ton 1 klanglich haben, ist nur aus dem Zeitpunkt
    geschlossen. Die Eintraege des Tonkopfs (4 B je Ton) sind nicht feldweise zerlegt.
11. **Simulation gegen Original.** Die Zeitachsen stammen aus dem nachgebauten Scheduler. Eine
    Gegenprobe gegen einen RE2-Savestate oder eine Bildaufnahme steht aus.
