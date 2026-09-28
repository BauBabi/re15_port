# Tor ROOM1170 - Dossier 08: Die Blende der RE2-Tuersequenz und die Blenden-Maschine des Ports

Stand 2026-09-28. Nur gelesen und gerechnet, nichts am Port geaendert, nichts gebaut.

Binaerdateien: RE2 = `info/re2leon/PSX.EXE`, RE1.5 = `info/Re1.5/PSX.EXE`.
Jede `@0x...`-Adresse unten habe ich selbst disassembliert (`re2_disasm.py` / `re15_disasm.py`), auch die
Sprungziele. Alle 313 zitierten Instruktionen prueft `build/tor_1170/re_blende/blende_pruefen.py` erneut
gegen die EXE (Ergebnis `INSTR 313/313`). Die Bildfolgen in Abschnitt 4 rechnet dasselbe Skript mit einer
Nachbildung der gelesenen Instruktionen; das ist Rechnung, keine Emulator-Messung.

Registerbasen, die die Disassembler-Anmerkung falsch aufloest (sie addiert zum letzten `lui`), habe ich
von Hand gerechnet: in `main` ist `s0 = 0x800cc1e8`, also `64(s0) = 0x800cc228`, `8260(s0) = 0x800ce22c`,
`8392(s0) = 0x800ce2b0`, `9208(s0) = 0x800ce5e0` (Pufferindex); `s3 = s0 + 14732 = 0x800cfb74`
(`@0x8002af10 addiu s3,s0,14732`); `s1 = 0x800dfae0`, `316(s1) = 0x800dfc1c` = Kanal 0.

---

## 0. Kurzantwort

1. **Kanal:** 4 Kanaele zu 0x4c B ab `0x800dfc1c`. Je Kanal: Pegel, Schritt, Mischart, RGB-Maske,
   OT-Platz, zwei TILEs, zwei DR_MODEs, Rechteck.
2. **Takt** `0x8002c378`, einmal je Hauptschleifen-Durchlauf, NACH den Tasks und VOR dem Bildwechsel:
   Pegel mit gesetztem Bit 15 = aus. Sonst Helligkeit = Pegel >> 7, TILE-Farbe = Helligkeit & Maske,
   danach Pegel += Schritt (16 Bit, ohne Saettigung). Fertig = Bit 15.
3. **Zeichnen:** ein TILE 320 x 240 ab (0,0), Code 0x62 (halbtransparent), DR_MODE mit tpage = Mischart << 5,
   bei Mischart 2 also B - F (subtraktiv). Kanaele 0..2 liegen in der 8er-Ordnungstabelle, die der
   Bildwechsel als LETZTE zeichnet, also ueber der Tuer.
4. **Bildfolgen:** (a) 56, 52, ..., 4, 0 = 15 Bilder, danach aus. (b) 0, 8, ..., 248 = 32 Bilder, danach aus,
   unabhaengig vom vorherigen Pegel. (c) 255 in jedem Bild, bis ein neuer Befehl kommt.
5. **Port:** Takt, Fertig-Test, Abschalten und Zeichnung entsprechen RE2. **Setzen und Anstossen nicht:**
   RE2 schreibt den Pegel in beiden Funktionen selbst, RE1.5 (und damit der Port) nicht bzw. ignoriert den
   Wert bei Schritt != 0. Eine woertliche Uebersetzung 0x53 -> `re15_fade_config`, 0x74 -> `re15_fade_kick`
   liefert fuer (a) 255, 251, ..., 3 = 64 Bilder statt 15. Mit drei Aufrufen bzw. einem direkten
   Pegelschreiber ist die Bildfolge exakt gleich (Abschnitt 5.3).

---

## 1. Die beiden Opcodes

### 1.1 `Sce_fade_set` 0x53, Handler `0x80057ef0`, 6 B

| Byte | Beleg | Weiter an |
|---|---|---|
| pc[1] Kanal | `@0x80057f18 lbu s0,1(s2)` | a0 low byte |
| pc[2] Mischart | `@0x80057f38 lbu a0,2(s2)`, `@0x80057f40 sll a0,a0,8`, `@0x80057f44 or a0,s0,a0` | a0 = Kanal \| Art<<8 |
| pc[3] RGB-Maske | `@0x80057f3c lbu a2,3(s2)` | a2 |
| s16 pc[4] Schritt | `@0x80057f1c lhu s3,4(s2)`, `@0x80057f4c sra s1,v0,16`, `@0x80057f54 addu a1,s1,zero` (Delay-Slot) | a1 |
| OT-Platz 7 | `@0x80057f14 addiu a3,zero,7` | a3 |

Ablauf:

```
0x8002c1a0(Kanal|Art<<8, Schritt, Maske, 7)          @0x80057f50
0x8002c2b0(Kanal, 0, 0, &{x0,y0,w320,h240})          @0x80057f58 a1=0, @0x80057f5c a2=0, @0x80057f64, a3=sp+16 @0x80057f68
                                                     Rechteck: @0x80057f20/24 w=320, @0x80057f28/34 h=240, @0x80057f2c/30 x=y=0
Kanalzeiger = 0x800dfc1c + Kanal*76                  @0x80057f6c..8c
Schritt == 0  -> nichts                              @0x80057f88 beq s1,zero,0x80057fa8
Schritt  > 0  -> Pegel = 0                           @0x80057f90 bgtz s1,0x80057fa4 ; @0x80057fa4 sh zero,0(v1)
Schritt  < 0  -> Pegel = (u16 Schritt + 0x8000)      @0x80057f94 ori v0,zero,0x8000 ; @0x80057f98 addu v0,s3,v0 ; @0x80057fa0 sh v0,0(v1) (Delay-Slot)
PC += 6                                              @0x80057fa8 addiu v0,s2,6
```

### 1.2 `Sce_fade_adjust` 0x74, Handler `0x80057fd8`, 4 B

```
0x8002c2b0(pc[1], (s16)pc[2], 0, NULL)    @0x80057ff4 lbu a0,1(s0) ; @0x80057ff8 lh a1,2(s0) ; @0x80057ff0 a2=0 ; @0x80058000 a3=0 (Delay-Slot) ; @0x80057ffc jal
PC += 4                                   @0x80058004 addiu s0,s0,4
```

### 1.3 `0x8002c1a0` - Kanal einrichten (a0 = Kanal | Art<<8, a1 = Schritt, a2 = Maske, a3 = OT-Platz)

| Feld | Wert | Beleg |
|---|---|---|
| Kanal | a0 & 0xff, Zeiger `0x800dfc1c + Kanal*76` | `@0x8002c1b4 andi a0,a0,0xff`, `@0x8002c1b8..d8` |
| +2 Schritt | a1 | `@0x8002c1e4 sh s1,2(s0)` |
| +4 Mischart | a0 >> 8 | `@0x8002c1a8 srl a2,a0,8`, `@0x8002c1e8 sb a2,4(s0)` |
| +8 OT-Platz | a3 | `@0x8002c1f0 sb a3,8(s0)` (Delay-Slot, laeuft immer) |
| +5 / +6 / +7 | 0xff wenn Maskenbit 4 / 2 / 1, sonst 0 | `@0x8002c1dc andi v0,t0,0x4` -> `@0x8002c1fc sb v0,5(s0)`; `@0x8002c204 andi 0x2` -> `@0x8002c214 sb 6`; `@0x8002c21c andi 0x1` -> `@0x8002c22c sb 7` |
| +44, +56 DR_MODE je Puffer | `SetDrawMode(p, 1, 0, Art<<5, NULL)` | `@0x8002c234 addiu a0,s0,44`, `@0x8002c238 addiu a1,zero,1`, `@0x8002c24c sll a3,a3,5`, `@0x8002c248 jal 0x800912ac`; zweiter `@0x8002c250 addiu a0,s0,56`, `@0x8002c264` |
| +68 / +72 Rechteck | x,y = 0; w,h = 320,240 | `@0x8002c280 sw zero,68(s0)`; `@0x8002c26c lui v0,0xf0` + `@0x8002c270 ori v0,v0,0x140` + `@0x8002c274 sw v0,72(s0)` |
| **+0 Pegel** | Schritt < 0: Schritt + 0x8000; sonst 0 | `@0x8002c278 sll v0,s1,16`, `@0x8002c27c bgez v0,0x8002c294`, `@0x8002c288 addu v0,s1,v0`, `@0x8002c290 sh v0,0(s0)`, `@0x8002c294 sh zero,0(s0)` |

`0x800912ac` ist SetDrawMode (Ghidra-Name; die Funktion ruft `@0x800912d4 jal 0x800917ac` und speichert
`@0x800912dc sw v0,4(s0)`). `0x800917ac` baut `0xe1000000 | (tpage & 0x9ff)` bzw. `& 0x27ff`
(`@0x800917cc lui v1,0xe100`, `@0x800917f4 andi v0,a2,0x9ff`, `@0x800917d8 andi v0,a2,0x27ff`,
`@0x80091800 or v0,v1,v0`). Beide Masken lassen die Bits 5-6 stehen: GP0(E1h).5-6 = Mischart
(psx-spx `graphicsprocessingunitgpu.md` Zeile 840: `0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4`).

### 1.4 `0x8002c2b0` - Pegel setzen (a0 = Kanal, a1 = Pegel, a2 = Farbe, a3 = Rechteck oder NULL)

| Wirkung | Beleg |
|---|---|
| **Pegel = a1**, immer | `@0x8002c2d4 sh a1,0(a0)` |
| TILE-Farbwort des aktuellen Puffers = a2 \| (Byte +19 << 24) | `@0x8002c2d0 lbu v1,19(a0)`, `@0x8002c2dc lbu v0,0x800ce5e0`, `@0x8002c2e4 or a2,a2,v1`, `@0x8002c2f4 sw a2,16(v0)` |
| Rechteck aus a3, sonst (0,0,320,240) | `@0x8002c2f0 beq a3,zero,0x8002c310`, `@0x8002c300 sw v0,68(a0)`, `@0x8002c318 sw zero,68(a0)`, `@0x8002c320 sw v0,72(a0)` (Delay-Slot von `jr ra`, laeuft in beiden Zweigen) |

Die Farbe hat keine sichtbare Wirkung: der Takt ueberschreibt sie vor dem Einhaengen (Abschnitt 2).

### 1.5 Nebenfunktionen

| Adresse | Wirkung | Beleg |
|---|---|---|
| `0x8002c324` | Pegel = 0xffff (aus) | `@0x8002c338 addiu v1,zero,-1`, `@0x8002c344 sh v1,-996(at)` |
| `0x8002c350` | Rueckgabe = Bit 15 des Pegels | `@0x8002c36c lh v0,-996(at)`, `@0x8002c374 srl v0,v0,31` |

### 1.6 Kanalsatz 0x4c B

| + | Inhalt | Beleg |
|---|---|---|
| 0 | u16 Pegel | 1.3, 1.4 |
| 2 | s16 Schritt | `@0x8002c1e4` |
| 4 | Mischart | `@0x8002c1e8` |
| 5,6,7 | Maske R,G,B (0 oder 0xff) | `@0x8002c1fc/214/22c` |
| 8 | OT-Platz | `@0x8002c1f0` |
| 12 / 28 | TILE Puffer 0 / 1 (16 B: tag, r g b code, x y, w h) | Takt `@0x8002c464 addiu a1,a1,12` (a1 = Puffer*16) |
| 44 / 56 | DR_MODE Puffer 0 / 1 (12 B) | `@0x8002c234`, `@0x8002c250`, Takt `@0x8002c4c4 addiu a1,a1,44` (a1 = Puffer*12) |
| 68 / 72 | Rechteck x,y / w,h | 1.3 |

Grundaufbau beim Spielstart (`main`, je Kanal und Puffer): x = y = 0, w = 320, h = 240
(`@0x8002b788/8c sh zero,336/338(v1)`, `@0x8002b780 sh v0,340(v1)`, `@0x8002b794 sh v0,342(v1)`),
`SetTile` (`@0x8002b790 jal 0x8008fb34`: `@0x8008fb34 addiu v0,zero,3` Laenge, `@0x8008fb3c addiu v0,zero,96`
+ `@0x8008fb44 sb v0,7(a0)` Code 0x60), `SetSemiTrans(p,1)` (`@0x8002b79c jal 0x8008f9cc`, `@0x8002b7a0 a1=1`;
`@0x8008f9dc ori v0,v0,0x2` -> Code 0x62), Schleife ueber 4 Kanaele (`@0x8002b7c8 addiu fp,fp,76`,
`@0x8002b7d4 slti v0,s5,4`).

---

## 2. Takt `0x8002c378` und Fertig-Test

```
fuer 4 Kanaele (s3 = 4..1)                                 @0x8002c398 addiu s3,zero,4 ; @0x8002c4d8 bgtz s3
  v0 = Pegel << 16                                         @0x8002c3a8 lhu ; @0x8002c3b0 sll v0,v0,16
  Bit 15 gesetzt -> Kanal ueberspringen (kein TILE, kein +=) @0x8002c3b4 bltz v0,0x8002c4d0
  h = v0 >> 23 (arithmetisch) = Pegel >> 7, 0..255         @0x8002c3b8 sra a0,v0,23 (Delay-Slot)
  TILE[p].r/g/b = Maske & h                                @0x8002c3c0 lbu v0,-3(s1) ; @0x8002c3cc and ; @0x8002c3d0/3e8/400 sb 16/17/18
  Pegel += Schritt (16 Bit)                                @0x8002c408 lhu v1,-6(s1) ; @0x8002c410 addu ; @0x8002c414 sh
  TILE[p].xy = +68, TILE[p].wh = +72                       @0x8002c41c/428, @0x8002c430/43c
  Kanal 3:   AddPrim(*0x800ce2b0 + Platz*4, TILE), dann DR_MODE    @0x8002c440 slti v0,s3,2 ; @0x8002c450 lw v0,8392(s2) ; @0x8002c468
  Kanal 0-2: AddPrim(*0x800cc228 + Platz*4, TILE), dann DR_MODE    @0x8002c484 lw v0,64(s2) ; @0x8002c49c ; @0x8002c4c8
```

p = Pufferindex `0x800ce5e0`. `0x8008f918` ist AddPrim (`@0x8008f924 lw v1,0(a1)` ... `@0x8008f938 sw v1,0(a1)`).
Weil AddPrim am Kopf einhaengt, steht das DR_MODE vor dem TILE in der Kette.

- **Helligkeit = Pegel >> 7.** Da Bit 15 vorher ausgeschlossen ist, liegt sie in 0..255.
- **Keine Saettigung.** Der Pegel laeuft 16-bittig ueber; das Kippen von Bit 15 ist das Ende.
  Aufwaerts endet ein Kanal beim Ueberschreiten von 0x7fff, abwaerts beim Unterschreiten von 0.
- **Die Farbe gehoert zum Pegel VOR der Addition**, also sieht man im ersten Bild den gesetzten Pegel.
- **Fertig** = Bit 15 (`0x8002c350`, 1.5). Ein Kanal, der fertig ist, wird nicht mehr gezeichnet.
- Der zweite Teil von `0x8002c378` (`@0x8002c4e0..5ec`) betrifft nicht die vier Kanaele, sondern einen
  eigenen Zaehler `0x800dfd55` (+-16 bis 0xf0) mit Prims ab `0x8009dbac` in Platz 4 der 8er-Tabelle
  (`@0x8002c5ac addiu a0,a0,16`, `@0x8002c5b4`). Er wird uebersprungen, solange `0x800cfb74 & 0x4000`
  (`@0x8002c588 andi v0,v0,0x4000`, `@0x8002c58c bne`). Dieses Bit setzt der Uebergang vor der Tuer
  (`@0x80025a6c ori v0,v0,0x4000`, `@0x80025a74 sw`) und loescht es danach (`@0x80025c9c addiu v1,zero,-16385`,
  `@0x80025ca4 sw`).

### 2.1 Wo der Takt im Bild sitzt

Hauptschleife RE2 (Normalzweig):

| Schritt | Beleg |
|---|---|
| drei Ordnungstabellen leeren (ClearOTagR): 8 Eintraege `*0x800cc228`, 1024 Eintraege `*0x800ce22c`, 16 Eintraege `*0x800ce2b0` | `@0x8002af54 sw v0,64(s0)`, `@0x8002af74 sw v0,8260(s0)`, `@0x8002af64 sw v1,8392(s0)`, `@0x8002af34 a1=8`, `@0x8002af90 jal 0x800908a8`, `@0x8002afa0 a1=1024`, `@0x8002afac a1=16` |
| Tasks, darin Door_main | `@0x8002b320 jal 0x80031d8c` |
| Blenden-Takt | `@0x8002b448 jal 0x8002c378` |
| Bildwechsel | `@0x8002b450 jal 0x8002b968` |
| zurueck | `@0x8002b460 j 0x8002af20` |

`0x800908a8` = ClearOTagR, `0x800909a0` = DrawOTag (Debug-Strings `ClearOTagR(%08x,%d)` / `DrawOTag(%08x)` in
den Funktionen, Ghidra-Dump Zeilen 76455/76459). Task_sleep(1) setzt den Zaehler 1 (`@0x80031fb0 sh a0,2(v0)`,
`@0x80031fb4 sh v1,0(v0)`), der Verteiler zieht ihn im naechsten Durchlauf auf 0 und setzt die Task fort
(`@0x80031e28 addiu v0,v0,-1`, `@0x80031e34 bgtz`, `@0x80031e3c sh s2,0(s1)`). Door_move laeuft also einmal
je Durchlauf, und **ein Blendenbefehl im Skript wirkt schon auf das Bild desselben Durchlaufs**.

---

## 3. Wie die Blende gezeichnet wird

| Frage | Antwort | Beleg |
|---|---|---|
| Primitiv | TILE (GP0 0x60) mit Halbtransparenz-Bit = Code 0x62, Laenge 3 | 1.6 |
| Groesse | 320 x 240 ab (0,0) | Sce_fade_set uebergibt das Rechteck (1.1), `adjust`/Door_exit NULL -> Vorgabe (1.4), Takt kopiert es jedes Bild (2) |
| Farbe | je Kanal Helligkeit & Maskenbyte; Maske 7 = R,G,B je 0xff | `@0x8002c1dc..22c`, `@0x8002c3cc..400` |
| Mischart 2 | DR_MODE mit tpage 0x40 -> GP0(E1h) Bits 5-6 = 2 = **B - F** | `@0x8002c24c sll a3,a3,5`, 1.3 |
| Unterboden | psx-spx: subtraktiv klemmt bei 0 (Zeile 1435 f.); RECTs werden nicht gedithert (Zeile 1406), mit Dither aus werden die unteren 3 Bit gestrichen (Zeile 842) | DR_MODE dtd = 0 (`@0x8002c240 addu a2,zero,zero`) |
| OT, Kanal 0-2 | 8er-Tabelle `*0x800cc228` = `0x800cc1e8 + Puffer*32`, Platz = Byte +8 | `@0x8002af44..54`, Takt 2 |
| OT, Kanal 3 | 16er-Tabelle `*0x800ce2b0` | Takt 2 |
| Reihenfolge | Bildwechsel zeichnet 16er (`&ot[15]`), dann Welt 1024 (`&ot[1023]`), dann **8er zuletzt** (`&ot[7]`) | `@0x8002bd40 jal 0x800909a0` a0+60 (`@0x8002bd44`), `@0x8002bd4c` a0+4092 (`@0x8002bd50`), `@0x8002bd58` a0+28 (`@0x8002bd5c`) |
| Innerhalb der 8er | ClearOTagR verkettet rueckwaerts, DrawOTag beginnt bei `ot[7]`: **Platz 7 zuerst, Platz 0 zuletzt** | `@0x8002bd5c addiu a0,a0,28` |

**Ueber allem?** Ueber allem aus der Welt- und der 16er-Tabelle, also ueber allen Tuerobjekten (die liegen
nach 03 §5.1 in `0x800ce22c` bzw. `0x800ce2b0`). In der 8er-Tabelle selbst liegen weitere Nutzer ueber Platz 7:
Platz 1 (`@0x8002b0b8 addiu a0,a0,4`, `@0x8002b0c0 jal 0x8008f918`), Platz 4 (Balken-Zaehler, waehrend der
Tuer gesperrt, 2), Platz 5 (`FUN_80034ba8`, `@0x80034cf0 addiu a0,a0,20`). Door_exit legt die
Schwarzhaltung auf **Platz 1** (Abschnitt 4c), also ueber Platz 4 und 5.

Hintergrund der Tuerszene: `0x8002bda8(2,0)` aus Door_init (`@0x80013e44..4c`) und aus dem Raumwechsel
(`@0x80026be8..f0`) schreibt Modus 2 nach `0x800dfd54` (`@0x8002bdac sb a0,-684(at)`) und Farbe 0 nach
env+140 (`@0x8002bdd0 sw a1,140(v1)`). Der Bildwechsel legt bei Modus 2 ein TILE env+136 in `ot16[15]`
(`@0x8002bd0c lbu v1,628(s1)`, `@0x8002bd14 bne v1,v0`, `@0x8002bd20 jal 0x8008fb34`, `@0x8002bd30 addiu a0,a0,60`,
`@0x8002bd34 jal 0x8008f918`) - schwarz, zuunterst.

---

## 4. Exakte Bildfolgen

Bild 0 = der Hauptschleifen-Durchlauf, in dem das Skript den Befehl ausfuehrt. Angegeben ist die
Helligkeit h des Kanal-0-TILEs in diesem Bild (R = G = B = h, subtraktiv); `-` = Kanal aus.

### (a) `Sce_fade_set(0,2,7,-512)` + `Sce_fade_adjust(0,7168)` im selben Bild

| Schritt | Pegel |
|---|---|
| 0x8002c1a0 (Schritt < 0) | 0x7e00 |
| 0x8002c2b0(0,0,...) aus dem 0x53-Handler | 0 |
| Handler, Schritt < 0 | 0x7e00 |
| 0x74 -> 0x8002c2b0(0,7168,...) | **0x1c00** |

| Bild | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15.. |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| h | 56 | 52 | 48 | 44 | 40 | 36 | 32 | 28 | 24 | 20 | 16 | 12 | 8 | 4 | 0 | - |

Nach Bild 14 ist der Pegel 0xfe00 (Bit 15) = fertig. Gezeichnet 15 Bilder, sichtbar abdunkelnd 14.

### (b) `Sce_fade_set(0,2,7,1024)`

0x8002c1a0 setzt 0 (Schritt >= 0), 0x8002c2b0 setzt 0, der Handler setzt 0 (`@0x80057fa4`). Der Anfangspegel
haengt also **nicht** davon ab, was vorher im Kanal stand (gerechnet ab 0, ab 0xfe00 = Rest von (a) und ab 0x7fff:
jeweils dieselbe Folge).

| Bild | 0 | 1 | 2 | ... | k | ... | 30 | 31 | 32.. |
|---|---|---|---|---|---|---|---|---|---|
| h | 0 | 8 | 16 | ... | 8k | ... | 240 | 248 | - |

Nach Bild 31 ist der Pegel 0x8000 = fertig. Der Kanal zeichnet danach **nichts** mehr; das Bild waere wieder
hell, wenn nicht Door_exit uebernimmt (c).

### (c) Door_exit `0x8001417c`

```
solange 0x8002c350(0) == 0: Task_sleep(1)          @0x80014184 jal ; @0x8001418c bne v0,zero,0x800141a4 ; @0x80014194 jal 0x80031f94
0x8002c1a0(0x200, 0, 7, 1)                         a0=512 @0x80014190 (Delay-Slot) ; @0x800141a4 a1=0 ; @0x800141a8 a2=7 ; @0x800141b0 a3=1 ; @0x800141ac jal
0x8002c2b0(0, 0x7fff, 0xffffff, NULL)              @0x800141b8 a1=32767 ; @0x800141bc/c0 a2=0x00ffffff ; @0x800141c8 a3=0 ; @0x800141c4 jal
```

Kanal 0 hat danach Schritt 0, Pegel 0x7fff, Platz 1: **h = 255 in jedem Bild**, ohne Ende. 255 & 0xff
subtrahiert den vollen Wert, das Bild ist schwarz. Danach schreibt Door_exit den Bildtakt 2 zurueck
(`@0x8001420c addiu v1,zero,2`, `@0x80014214 sb v1,-998(at)` = `0x800dfc1a`) und loescht Bit 0x2000000
(`@0x800141fc ori a1,a1,0xffff` mit `lui a1,0xfdff`).

**Was man danach sieht:** schwarz, bis Kanal 0 neu gesetzt wird. Das tut der Uebergang nach der Rueckkehr aus
`FUN_80026b7c` (`@0x80025a70 jal`, `@0x80025a78 j 0x80025c70`, `@0x80025a7c addiu a0,zero,512` im Delay-Slot):
`0x8002c1a0(0x200, -6144, 7, 1)` (`@0x80025c70 a1=-6144`, `@0x80025c74 a2=7`, `@0x80025cc4 a3=1`,
`@0x80025cc0 jal`) - **ohne** folgendes `0x8002c2b0`. Pegel = -6144 + 0x8000 = 0x6800:
h = 208, 160, 112, 64, 16, dann aus (5 Bilder). Wie viele schwarze Bilder zwischen Door_exit und dieser
Einblendung liegen, haengt am Laden des Zielraums (OFFEN 2).

### (d) Ganze Zeitachse DOOR2E (Typ 0 und Typ 1 gleich)

`python build/tor_1170/re_blende/blende_pruefen.py`, Teil ARCHIV; Befehlszeitpunkte aus dem Skript-Simulator
`re15_port/tools/tor/tuerskript_dump.py` (03), Blendenwerte aus der eigenen Kanal-Nachbildung, beide stimmen
Bild fuer Bild ueberein.

| Bild | h | Ursache |
|---|---|---|
| 0..14 | 56 .. 0 | (a) aus Skript 1 bzw. 2 @0x2c / @0x42 |
| 15..259 | - | Kanal fertig, Tuer unverdunkelt |
| 260..290 | 0 .. 240 | (b) aus Skript 7 bzw. 8 @0x40; Door_move laeuft bis Durchlauf 290 (291 Durchlaeufe) |
| 291 | 248 | Door_move endet (`@0x8001402c lbu v0,0x800d86e9`, `@0x80014034 bne`), Door_main ruft sofort Door_exit (`@0x80013bf0 jalr`), Kanal noch nicht fertig -> Task_sleep; in diesem Bild keine Tuerobjekte, nur der schwarze Hintergrund |
| 292.. | 255 | (c) |

Es gibt also keinen hellen Blitz zwischen Ausblenden und Schwarzhaltung.

### (e) Zensus aller 55 RE2-Archive

| Befehl | Anzahl |
|---|---|
| `Sce_fade_set(0,2,7,+1024)` | 125 |
| `Sce_fade_set(0,2,7,-512)` | 122, **alle** direkt gefolgt von `Sce_fade_adjust(0,7168)` |
| `Sce_fade_set(0,2,7,+2048)` / `(-1024)` / `(+512)` | 6 / 6 / 1 |
| `Sce_fade_adjust` | 154: 123 x 7168; die uebrigen 31 sind die Rampe 6944, 6720, ..., 224 (Abstand 224), alle in `DOOR15`, nach einem `Sce_fade_set(...,-1024)` (je Bild ein `adjust` bei laufendem Schritt) |

Kanal immer 0, Art immer 2, Maske immer 7.

---

## 5. Vergleich mit RE1.5 und dem Port

### 5.1 Funktion gegen Funktion

| Rolle | RE2 | RE1.5 | Port | gleich? |
|---|---|---|---|---|
| Kanalsatz | 4 x 0x4c ab `0x800dfc1c` | 4 x 0x44 ab `0x800b5458` (`@0x800217d0 addiu v1,v1,21592`, `@0x800219d4 addiu s1,s1,68`) | `g_fade_ch[4]` (`include/re15_fade.h:26-40`) | Aufbau gleich bis auf das Rechteck +68/+72 (RE1.5 schreibt es direkt in die TILEs, `@0x80021734..58`) |
| Einrichten | `0x8002c1a0` | `FUN_800217b0` | `re15_fade_config` (`fade_common.c:31`) | **nein**: RE2 setzt zusaetzlich den Pegel (`@0x8002c27c..294`); RE1.5 endet nach dem zweiten SetDrawMode ohne Pegelschreiber (`@0x80021864 jal 0x80069858`, `@0x80021878 jr ra`) |
| Pegel setzen | `0x8002c2b0` | `FUN_800216ec` | `re15_fade_kick` (`fade_common.c:50`) | **nur bei Schritt 0**: RE2 schreibt immer (`@0x8002c2d4`); RE1.5 schreibt den Wert nur bei Schritt 0 (`@0x8002170c beq v0,zero,0x80021724`, `@0x80021728 sh a1,0(v1)`), sonst 0x7fff bzw. 0 (`@0x80021710 slti v0,v0,1`, `@0x80021714 subu`, `@0x80021718 andi v0,v0,0x7fff`, `@0x80021720 sh`) |
| Takt | `0x8002c378` | `FUN_80021880` | `re15_fade_tick` (`fade_common.c:83`), aufgerufen in `re15_render_end_frame` (`render_pc.c:1024`) | **ja**: `@0x800218cc bltz`, `@0x800218d0 sra a0,v0,23`, `@0x800218e4 and`, `@0x80021928 addu`/`@0x8002192c sh`; RE2 kopiert zusaetzlich das Rechteck (ohne Wirkung, immer Vollbild) |
| Fertig | `0x8002c350` | `FUN_8002178c` (`@0x800217a4 lh`, `@0x800217ac srl v0,v0,31`) | `re15_fade_done` | ja |
| Aus | `0x8002c324` | `FUN_80021764` (`@0x80021770 addiu v1,zero,-1`, `@0x80021780 sh`) | `re15_fade_kill` | ja |
| Opcode einrichten | 0x53, 6 B, Platz 7 | 0x56 (`0x800744a8[0x56] = 0x80042a58`), 6 B, Platz 7 (`@0x80042a70 ori a3,zero,0x7`, `@0x80042a88 jal 0x800217b0`, `@0x80042a90 addiu s0,s0,6`) | `op_fade_config` (`scd_vm.c:4729`) | Bytes gleich, Wirkung auf den Pegel nicht |
| Opcode Pegel | 0x74, 4 B | 0x57 (`0x800744a8[0x57] = 0x80042ab4`), 4 B (`@0x80042ad0 lhu a1,2(v0)`, `@0x80042ad4 jal 0x800216ec`, `@0x80042ae4 addiu v0,v0,4`) | `scd_vm.c:1949` | Bytes gleich, Wirkung nur bei Schritt 0 gleich |
| OT | 8er zuletzt, 16er zuerst | 8er `0x800aa698` zuletzt, 16er `0x800ac6d8` zuerst (`@0x800218b0 addiu s4,s2,-9116`, `@0x800218a8 addiu s5,s2,-860` mit s2 = `0x800aca34`; Zeichnen `@0x800215bc` 16er, `@0x800215d0` Welt, `@0x800215e4` 8er) | Kanaele 0-2 nach der 3D-Warteschlange, vor Balken und Text (`render_pc.c:1024-1038`); Platz wird nicht ausgewertet; Kanal 3 nie | fuer die Tuerszene gleichwertig (keine anderen Prims in der 8er) |
| Mischung | B - F in 15 Bit | wie RE2 | `SDL_BLENDOPERATION_REV_SUBTRACT` in 8 Bit (`render_pc.c:1030-1032`) | Formel gleich; Rundung siehe OFFEN 3 |
| Einblendung nach der Tuer | `0x8002c1a0(0x200,-6144,7,1)`: 208,160,112,64,16 | `@0x8001cc00` config(0x200,-6144,7,0) + `@0x8001cc18` kick(0,0,...): 255,207,159,111,63,15 | `re15_room_transition_present` (`room_common.c:148-149`) = RE1.5 | zwei verschiedene Originale |

### 5.2 Folge der woertlichen Uebersetzung

`re15_fade_config(0,2,7,-512,7)` + `re15_fade_kick(0,7168)`: der Wert 7168 wird ignoriert, Pegel = 0x7fff.
h = 255, 251, 247, ..., 7, 3 = **64 Bilder** statt 56 ... 0 in 15. Fuer (b) reicht `re15_fade_config` allein
nicht: der Pegel bleibt 0xfe00 (fertig), es wird **gar nichts** gezeichnet (Skript: `b_port_nur_config` = leer).

### 5.3 Rezept - Port-Aufrufe, die genau die Bildfolgen aus 4 erzeugen

Voraussetzung: der Tuer-Skriptschritt eines Bildes laeuft VOR `re15_render_end_frame()` desselben Bildes
(dort sitzt `re15_fade_tick`, `render_pc.c:1024`), so wie RE2 Tasks vor Takt vor Bildwechsel ausfuehrt (2.1).

**Allgemein, mit direktem Pegelschreiber** (`g_fade_ch` ist in `re15_fade.h:40` exportiert; genau die
RE2-Schreibzugriffe):

```c
/* RE2 0x53 Sce_fade_set  (@0x80057f50..fa4, @0x8002c27c..294) */
re15_fade_config(ch, kind, mask, step, 7);
g_fade_ch[ch].level = (step < 0) ? (uint16_t)(step + 0x8000) : 0;
/* RE2 0x74 Sce_fade_adjust (@0x8002c2d4 sh a1,0(a0)) */
g_fade_ch[ch].level = (uint16_t)value;
```

**Nur mit der vorhandenen Schnittstelle** (config veraendert den Pegel nie, kick uebernimmt den Wert bei Schritt 0):

| Fall | Aufrufe | Folge (gerechnet) |
|---|---|---|
| (a) | `re15_fade_config(0,2,7,0,7); re15_fade_kick(0,7168); re15_fade_config(0,2,7,-512,7);` | 56, 52, ..., 4, 0, aus - gleich RE2 |
| (b) | `re15_fade_config(0,2,7,1024,7); re15_fade_kick(0,0);` | 0, 8, ..., 248, aus - gleich RE2, auch ab dem Rest von (a) |
| (c) | `re15_fade_config(0,2,7,0,1); re15_fade_kick(0,0x7fff);` | 255 dauerhaft - gleich RE2 |
| 0x53 mit Schritt < 0 ohne 0x74 (z.B. `DOOR20`) | `config(ch,k,m,0,7); kick(ch,step+0x8000); config(ch,k,m,step,7);` | wie RE2 |
| 0x74 bei laufendem Schritt s (`DOOR15`) | `config(ch,k,m,0,b); kick(ch,value); config(ch,k,m,s,b);` | wie RE2 |

Anschluss danach: die RE1.5-Einblendung `re15_room_transition_present()` startet bei Pegel 0x7fff (h = 255),
also nahtlos an (c). Die RE2-Einblendung (208 ...) waere eine andere Folge; welche gilt, ist eine
Entscheidung ausserhalb dieses Dossiers (5.1 letzte Zeile).

Zwei Punkte, die die vorhandene Maschine nicht von selbst abdeckt:

1. **Balken.** RE2 sperrt die Balken-Prims waehrend der Tuer (`0x800cfb74 & 0x4000`, Abschnitt 2). Der Port
   zeichnet seine Balken, sobald `g_letterbox_level != 0` (`render_pc.c:1048`); die Tuerszene muss dafuer
   sorgen, dass dort 0 steht, sonst liegen Balken ueber der Blende (im Port kommen sie nach der Blende).
2. **Platz 1 gegen Platz 7** hat im Port keine Wirkung (`ot_bucket` wird nicht ausgewertet). In der Tuerszene
   gibt es keinen anderen Nutzer der 8er-Tabelle, daher ohne sichtbaren Unterschied.

---

## 6. Das Abdunkeln VOR der Tuerszene (gehoert zur RE2-Sequenz, liegt nicht in den Kanal-Befehlen)

Die Tuerszene beginnt nicht aus dem Spielbild heraus, sondern aus Schwarz. RE2 erzeugt das Schwarz mit
**Kanal 0, aber ohne Takt**:

- Uebergangszustand (Sprungtabellen-Eintrag `0x800109c4` = `0x80025970`, `re2_disasm.py table 0x800109bc 4`): Kanal 0 muss fertig sein
  (`@0x80025988 jal 0x8002c350`); Tuerzweig, wenn weder `0x800cfbd8 & 0x8000` noch `0x800cfb74 & 0x40000`
  (`@0x800259b4`, `@0x800259cc`); dann `0x800cfb74 |= 0x10000` (`@0x80025a44 lui a2,0x1`, `@0x80025a48 or`,
  `@0x80025a50 sw`) und `FUN_80026b7c` (`@0x80025a70`).
- Solange Bit 0x10000 steht, nimmt die Hauptschleife statt Takt + Bildwechsel diesen Zweig
  (`@0x8002b33c lw v0,0(s3)`, `@0x8002b340 lui v1,0x1`, `@0x8002b348 beq v0,zero,0x8002b440`):
  - einmalig, wenn Kanal 0 fertig ist (`@0x8002b350`, `@0x8002b358`): DrawSync, PutDispEnv(env[p]+92)
    (`@0x8002b36c jal 0x80090c6c`, `@0x8002b370`; Ghidra-Name PutDispEnv), SetDrawMode(DR_MODE[p], 1, 0, 64)
    = Mischart 2 (`@0x8002b380 addiu a3,zero,64`, `@0x8002b398`), `0x8002c2b0(0, 31, 0x080808, NULL)`
    (`@0x8002b3a4 a1=31`, `@0x8002b3a8/ac a2=0x00080808`, `@0x8002b3b0`) -> Pegel 31, Farbe (8,8,8);
  - jedes Bild: TILE und DR_MODE von Kanal 0 in `ot8[0]` (`@0x8002b3c4 addiu a1,a1,328` = Kanal 0 + 12,
    `@0x8002b3c8`; `@0x8002b3e4 addiu a1,a1,360` = Kanal 0 + 44, `@0x8002b3e8`), Pegel -= 1
    (`@0x8002b3f0 lhu v0,316(s1)`, `@0x8002b3f8 addiu v0,v0,-1`, `@0x8002b3fc sh`), bei < 0 Bit 0x10000 loeschen
    (`@0x8002b404 bgez`, `@0x8002b408 lui v1,0xfffe`, `@0x8002b418 sw v0,0(s3)`), DrawSync, `VSync(0)`
    (`@0x8002b424 jal 0x80085ea0`, `@0x8002b428 a0=0`), DrawOTag nur der 8er-Tabelle (`@0x8002b430`, `@0x8002b434 a0+28`).
- Kein Bildwechsel, kein Neuzeichnen der Welt: jedes Bild zieht (8,8,8) vom stehenden Bild ab. 8 >> 3 = 1
  Stufe von 31 je Kanal und Bild; **Pegel 31..0 = 32 Bilder**, nach 31 Bildern ist jeder Kanal 0.
- Door_move wartet genau darauf (`@0x80013ee0 jal 0x80031f94`, `@0x80013ef4 and v0,v0,s0` mit `s0 = 0x10000`,
  `@0x80013ef8 bne`). Kanal 0 steht danach auf 0xffff (fertig), bis das Skript (a) setzt.

RE1.5 hat denselben Zweig mit anderen Zahlen: `aca38 & 0x10000` (`@0x80020dec..f8`), Pegel 20
(`@0x80020e3c ori v0,zero,0x14`, `@0x80020e4c sh`), Farbe 16 (`@0x80020e44 ori a0,zero,0x10`), Eintrag in
`ot8[0]` (`@0x80020bf8 addiu s4,s0,-9120` = `0x800aa698` mit `s0 = 0x800aca38`, Ghidra-Anmerkung
`@0x80020e18 lbu v0,-0x4(s0)=>DAT_800aca34`; `@0x80020eb0`, `@0x80020ed8`), DrawOTag
(`@0x80020efc`), Pegel -= 1 (`@0x80020f0c`, `@0x80020f18 bgez`), Bit loeschen (`@0x80020f30`). Das sind
21 Bilder zu 2 Stufen, schwarz nach 16. RE1.5 ruft dabei kein SetDrawMode; die Mischart ist die des
letzten `FUN_800217b0` auf Kanal 0. Der Port kennt diesen Zweig nicht (`grep` auf `aca38 & 0x10000` /
`0x800b5458` in `engine/src`, `platform/pc`: nur der Kommentar `room_common.c:89`).

---

## 7. Nachmessen

```bash
python build/tor_1170/re_blende/blende_pruefen.py        # INSTR 313/313, Modelle, Zensus, DOOR2E; Exit 1 bei Abweichung
R2=.claude/skills/re15-psx-disasm/scripts/re2_disasm.py
R1=.claude/skills/re15-psx-disasm/scripts/re15_disasm.py
python $R2 dis 0x80057ef0 58     # Sce_fade_set
python $R2 dis 0x80057fd8 12     # Sce_fade_adjust
python $R2 dis 0x8002c1a0 110    # einrichten, Pegel, aus, fertig, Takt-Anfang
python $R2 dis 0x8002c378 100    # Takt
python $R2 dis 0x8001417c 44     # Door_exit
python $R2 dis 0x8002b308 56     # Abdunkel-Zweig
python $R2 dis 0x8002bd0c 20     # Zeichenreihenfolge
python $R1 dis 0x800216ec 110    # RE1.5 kick/kill/done/config
python $R1 dis 0x80021880 60     # RE1.5 Takt
```

Details: `build/tor_1170/re_blende/blende.json`.

---

## 8. OFFEN

1. **Bildtakt.** Die Folgen zaehlen Hauptschleifen-Durchlaeufe. Die Tuerszene setzt `VSync`-Modus 0
   (`@0x80013f24 sb zero,0x800dfc1a`, nur wenn Objekt-0-Flag 0x8000 fehlt, `@0x80013f18`), der Bildwechsel
   uebergibt ihn (`@0x8002b994 lbu`, `@0x8002b998 jal 0x80085ea0`); das Abdunkeln ruft `VSync(0)` direkt.
   Wie viele Durchlaeufe das je Sekunde sind, ist nicht gemessen (vgl. 03 OFFEN 1). Der Port muss die
   Tuerszene mit derselben Durchlaufrate fahren, sonst stimmt die Dauer bei gleicher Bildfolge nicht.
2. **Schwarze Bilder zwischen Door_exit und Einblendung.** Haengt am Laden des Zielraums in `FUN_80026b7c`;
   nicht gezaehlt.
3. **Rundung der Subtraktion.** Mit Dither aus werden 8-Bit-Farben auf 5 Bit gekuerzt (psx-spx Zeile 842, RECTs
   nie gedithert Zeile 1406). Ob die Mischung mit dem gekuerzten F rechnet (dann wirken h = 0..7 gar nicht und
   h = 52 wie 48), steht in den gelesenen Zeilen nicht. Der Port subtrahiert 8-bittig. Unterschied hoechstens
   7/255 je Kanal und Bild. Messbar an einem RE2-Savestate waehrend (a).
4. **Welche Einblendung nach der Tuer.** RE2 208..16 (5 Bilder, Platz 1) gegen RE1.5 255..15 (6 Bilder,
   Platz 0). Beide gelesen; die Wahl ist keine RE-Frage.
5. **RE1.5-Abdunkeln sichtbar?** Statisch gelesen dunkelt RE1.5 vor jedem Tuerwechsel 21 Bilder lang ab
   (Abschnitt 6). Der Port-Kommentar `platform/pc/main.c:7076` sagt "a PLAIN door CUTS to black ... NO gradual
   fade-OUT". Ob die RE1.5-Anzeige dabei den abgedunkelten Puffer zeigt (DISPENV-Belegung `0x800b5428` nicht
   gelesen), ist nicht geprueft. Naechster Weg: DuckStation-Aufnahme eines RE1.5-Tuerwechsels Bild fuer Bild.
6. **Bedeutung der Bits** `0x800cfbd8 & 0x8000` und `0x800cfb74 & 0x40000`, die statt der Tuer den
   6-Bild-Ausblendzweig `@0x80025a80` waehlen, ist nicht bestimmt.
