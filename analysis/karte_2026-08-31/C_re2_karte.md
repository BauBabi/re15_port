# UNTERSUCHUNG C — RE2-Retail: Etagenwechsel, Tueren, Treppen, Spieler-Marke

Quelle: `info/re2leon/PSX.EXE` (SLUS-00748, PS-X-EXE `t_addr=0x80010000`, `t_size=0xf0800`,
Datei-Offset = `addr-0x80010000+0x800`), `ghidra_re2_Leon.txt`, `RE2_Quellcode_V2/FUN_*.c`,
`info/re2leon/COMMON/DATA/MAPS/MAPS_0NN.TIM`.
Alle Disasm-Zeilen in diesem Bericht wurden in DIESEM Lauf mit
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` direkt aus dem Binary gezogen.
Ergaenzt/korrigiert `analysis/nutzer_batch_2026-08-30b/map-re2-system.md`.

---

## 0. Kurzfassung

* Das angezeigte Blatt ist ein **eigenes Byte `DAT_800d5c0a` (Area 0..0x13)**.
  Beim Oeffnen wird es aus `FUN_8006e7f0(Stage, Raum)` gesetzt — also aus **Stage+Raumnummer**,
  mit der **Etagen-Bande `DAT_800cfcfe` nur als Stichentscheid** fuer Raeume, die ueber
  zwei Ebenen reichen.
* Umgeschaltet wird mit **D-Pad HOCH / RUNTER** (gehalten, mit Auto-Repeat), nicht L1/R1.
  Die Ziel-Area kommt aus zwei Nachbar-Tabellen; ist die Nachbar-Etage noch nie betreten,
  passiert nichts und der Pfeil-Reiter ist unsichtbar.
* **Tueren UND Treppen sind in die Karten-Grafik EINGEMALT** (Tuer = 2x4/4x2 Block in
  Palettenindex 3 = Gelb; Treppe = Schraffur aus Index-2-Linien). Es gibt dafuer **keine**
  Datentabelle und **keine** Ableitung aus den Tuer-AOTs.
* Zusaetzlich existieren **14 blinkende 8x8-Marken** (Tabelle @0x800a9b1c) fuer besondere
  Uebergaenge; ihre Pixel sind **6x4- bzw. 4x6-BALKEN** in der Karten-Seite bei **v=248**.
  Sie verschwinden, sobald das Skript ihr Bank-32-Bit setzt.
* Spieler = 12x12-Sprite, 8 Richtungsframes, RGB pulsiert 0x2E..0x7C.

---

## 1. Q1 — WELCHES BLATT wird gezeigt?

### 1.1 Der Etagen-/Blatt-Index

`DAT_800d5c0a` (u8) = **Area-Nummer 0..0x13**, ein eigenes Feld im Map-Screen-Block
(`s0 = 0x800d5bf0`, also `s0+0x1a`).

Alle Schreiber (Xref-Scan `ghidra_re2_Leon.txt`, 40 Refs):

| Adresse | Kontext |
|---|---|
| `sb v0,0x1a(s0)` **@0x8006d6c8** | Map-Screen State 0 (Oeffnen) — Ergebnis von `FUN_8006e7f0` |
| **@0x8006d958 / @0x8006d9bc** | Map-Screen State 3 — D-Pad hoch / runter |
| **@0x8006edec / @0x8006ee50** | Karten-Item-/CHECK-Screen — dieselben zwei Tasten |
| **@0x8006f01c, @0x8006f738** | CHECK-Screen-Init aus der Karten-Liste |

Beim Oeffnen (State 0, Jumptable-Eintrag 0 aus `0x80011c98`):

```
8006d6a8: lui a0,0x800d ; lh a0,18460(a0)     ; a0 = DAT_800d481c = Stage
8006d6b0: lui a1,0x800d ; lh a1,18462(a1)     ; a1 = DAT_800d481e = Raum-Nr
8006d6b8: jal 0x8006e7f0
8006d6c8: sb v0,26(s0)                        ; DAT_800d5c0a = Area
```

=> **Das Blatt wird bei JEDEM Oeffnen auf die Area des Spielers zurueckgesetzt**; das
Blaettern in State 3 gilt nur fuer die Dauer des Screens.

### 1.2 `FUN_8006e7f0` @0x8006e7f0 — (Stage, Raum[, Etage]) -> Area

Primaer entscheidet die **Raumnummer**, per `switch(stage)`; die Etagen-Bande ist nur
Stichentscheid (Decompile `RE2_Quellcode_V2/FUN_8006e7f0.c`, Lesestelle der Bande
`lbu a1,-0x302(a1)` **@0x8006e800**):

```
stage 0: raum 0x11|0x17               -> 4
         raum 0x12 && band==3         -> 4      <-- Etagen-Stichentscheid
         raum <4 ->0 ; <8 ->1 ; <0x16 ->3
         raum 0x16: band==4 ? 3 : 2             <-- Etagen-Stichentscheid
stage 1: raum 0x1b ->3 ; raum <0x11 ->2 ; sonst 5
stage 2: raum <2 ->5 ; raum!=8 ->6 ; raum==8 ->5
stage 3: raum in {0,1,3} && band>3 ->7 ; raum in {2,9,0xb} ->7 ; sonst 8
stage 4: ... (Flag-Bits FUN_80077360(&DAT_800d4854, 0xbe/0x63/0x62), B-Szenario 0x40000000)
stage 5: raum 3 && band<=2 -> 0xe ; ...
stage 6: FUN_80077360(&DAT_800d4854,0x89) ? 0x13 : 0x12
```

### 1.3 Was `DAT_800cfcfe` ist — BELEGT

Einziger Schreiber: `FUN_80026b7c` beim Tuer-Uebergang.

```
80026d1c: lbu v1,11(a1)                  ; Byte +0xB aus dem Tuer-Datensatz
80026d24: sb  v1,-770(at)                ; DAT_800cfcfe = <band>
80026d28..3c: v0 = ((band*8 - band)*32 + band)*8   = band * 1800
```

=> `DAT_800cfcfe` ist die **Etagen-Bande** (Y des Spielers = Bande * 1800), also RE2s
Gegenstueck zu RE1.5s `+0x82`. Sie wird als Stichentscheid in die Blattwahl gefuettert,
nicht als Blattindex selbst.

### 1.4 Welcher Raum im Blatt wird hervorgehoben

`FUN_8006eae8(Area, Raum)` @0x8006eae8 — eine **hart codierte switch-Umrechnung**
Raumnummer -> Index in die Raumtabelle der Area (z.B. Area 3: `raum==0x1b -> 0xe`,
sonst `raum-8` bzw. `raum-9` ab 0x12). Im Zeichner:
`if (angezeigteArea == SpielerArea && idx == FUN_8006eae8(...)) clutY += 1`
(`addiu s5,zero,501` / `498` / `509` @0x8006e5d0 / @0x8006e5dc / @0x8006e600).

=> **RE2 besitzt eine explizit autorisierte Raum<->Rechteck-Zuordnung.** Genau die fehlt
in RE1.5 (siehe `map-port-und-daten.md` §2.5).

---

## 2. Q2 — WIE schaltet der Spieler die Etage um?

### 2.1 Die Eingabe: D-Pad HOCH / RUNTER, gehalten

Map-State 3 @0x8006d87c (interaktiv). Die vollstaendige Eingabeflaeche des Screens sind
**genau drei** Pad-Abfragen (`s1 = 0x800cc1e8`):

```
8006d8d8: lui a0,0x800d ; addiu a0,a0,-1164   ; a0 = 0x800cfb74 (globales Flagwort)
8006d8e0: lw  v0,0(a0)
8006d8e8: bgez v0,0x8006d9e0                  ; Bit31 == 0 -> Etagenwechsel UEBERSPRINGEN
8006d8f0: lbu a1,26(s0)                       ; a1 = Area
8006d8f8: sltiu v0,a1,0x2
8006d8fc: bne  v0,zero,0x8006d9e0             ; Area < 2 -> keine Etagen

8006d904: lhu v0,0x211c(s1)   ; = DAT_800ce304   [96 22 21 1c]
8006d90c: andi v0,v0,0x1000   ; PADLup           [30 42 10 00]
   -> a1 = DAT_800a9aec[area]            (Nachbar OBEN)
   -> if (a1 >= 2 && FUN_80077360(&DAT_800d4908, a1)) DAT_800d5c0a = a1   @0x8006d958

8006d95c: lhu v0,0x211c(s1)
8006d964: andi v0,v0,0x4000   ; PADLdown         [30 42 40 00]
   -> a1 = DAT_800a9b04[area]            (Nachbar UNTEN)
   -> if (a1 >= 2 && FUN_80077360(&DAT_800d4908, a1)) DAT_800d5c0a = a1   @0x8006d9bc

8006d9c8: beq s2,v0,...        ; Area veraendert?
8006d9d0: sb  4,2(s0)          ; -> State 4 (Blatt nachladen)
8006d9d4: lui a0,0x0404 ; jal 0x8005ba28   ; Ton 0x04040000

8006d9e0: lw  v0,0x2128(s1)   ; = DAT_800ce310 (Druckflanke, logisches Wort)
8006d9e8: andi v0,v0,0x2000   ; -> State 2 = Abbrechen, Ton 0x04050000
```

**Kein L1/R1, kein Pan, kein Raum-Cursor.** (`DAT_800d5c48`/`DAT_800d5c4a`, die Pan-Offsets
der Raum-Rechtecke, haben im ganzen EXE nur Schreiber, die sie **auf 0 setzen**:
@0x8006d7b0/@0x8006d7b8, @0x8006f0fc/@0x8006f104, @0x8006f6e4/@0x8006f6ec.)

### 2.2 `DAT_800ce304` ist das GEHALTENE Rohpad — Beleg

Pad-Latch @0x80039340-0x800393ac:

```
a2 = [0x800ce2fc]   ; aktuelles Rohwort (frisch geschrieben in FUN_80038e..)
v1 = [0x800ce300]   ; Vorframe-Kopie (sw a2,0x800ce300 @0x80038eb0)
v1 = (v1 ^ a2) & a2 ; Druckflanke
80039374: sw v1,[0x800ce300]
8003939c: sh v1,[0x800ce306]   ; Flanke, Pad1
800393a4: sh a2,[0x800ce304]   ; GEHALTEN, Pad1     <---
```

Bitlage = PsyQ `PadRead`: `PADLup=0x1000`, `PADLright=0x2000`, `PADLdown=0x4000`,
`PADLleft=0x8000`. Die semantische Gegenprobe stimmt: 0x1000 greift die
**Aufwaerts**-Tabelle, 0x4000 die **Abwaerts**-Tabelle.

### 2.3 Warum "gehalten" nicht jeden Frame durchblaettert: Auto-Repeat via `DAT_800cfb74` Bit31

`bgez v0 -> skip` @0x8006d8e8 heisst: der Wechsel laeuft **nur, wenn Bit 31 von
`DAT_800cfb74` gesetzt ist**. Dieses Bit ist der **Repeat-Tick**:

```
800393b0: and v1,v1,a0            ; a0 = [0x800dfc14] = repeat-faehige Tastenmaske
800393b4: beq v1,zero,0x800393cc  ; keine frische Flanke?
800393c0: lbu v0,[0x800dfc18]     ; ERST-Verzoegerung
        -> j 0x8003941c: sb v0,[0x800a2704] ; Timer laden, dann Bit31 SETZEN
800393cc: lbu v1,[0x800a2704]     ; Timer
800393d8: beq v1,zero,0x80039414  ; Timer 0?
800393e0..ec: if (held & mask) timer--
800393f0..408: DAT_800cfb74 &= 0x7fffffff   ; Bit31 LOESCHEN
80039414: lbu v0,[0x800dfc19]     ; WIEDERHOL-Intervall
80039420: sb  v0,[0x800a2704]
80039430..38: DAT_800cfb74 |= 0x80000000    ; Bit31 SETZEN
```

Konfiguriert wird das mit `FUN_80039464(maske, (intervall<<8)|erstverzoegerung)`:

| Aufrufer | Maske | Erst-Verzoegerung | Wiederholung |
|---|---|---|---|
| `FUN_80039b88` @0x80039b8c-ba4 | `0xF000` (nur D-Pad) | `0x0A` = **10** Frames | `0x02` = **2** Frames |
| Menue-/Inventar-Task `LAB_800689bc` @0x800689ec-f4 | `0xF01C` | `0x0A` = **10** Frames | `0x06` = **6** Frames |

Der Map-Screen laeuft im Inventar-Task: `FUN_80031f6c(1, 0x800689bc)` @0x80025b8c-94; der
Task-Rumpf `LAB_800689bc` benutzt **denselben Block `s2 = 0x800d5bf0`** wie `FUN_8006d650`
(`addiu s2,s2,23536` @0x80068a04) und setzt die Repeat-Konfiguration als **erste** Handlung
(@0x800689ec-f4). Der Tab-Dispatcher `LAB_8006a774` @0x8006a774 waehlt ueber
`DAT_800d5bf1` aus der Zeiger-Tabelle `0x800a9aac`; Index 4 = `0x800a9abc -> 0x8006d650`.
=> **10 Frames Erstverzoegerung, danach alle 6 Frames ein Etagenschritt**.
(Die zweite Konfiguration `0xF000 / 10 / 2` gehoert zu `FUN_80039b88`, aufgerufen aus
`FUN_800398ac` @0x800398d0, deren Aufrufer @0x8003b17c/@0x8003b540 ausserhalb des
Inventar-Tasks liegen — sie ist also nicht die des Map-Screens.)

### 2.4 Die Nachbar-Tabellen (aus dem Binary gedumpt)

```
DAT_800a9aec (HOCH)   = 00 00 03 04 00 02 00 00 07 00 00 00 00 00 0d 0e 0f 10 00 00
DAT_800a9b04 (RUNTER) = 00 00 05 02 03 00 00 08 00 00 00 00 00 0e 0f 10 11 00 00 00
```

Wert < 2 = kein Nachbar. Beispiel RPD: Area 2 (1F) hoch->3 (2F), runter->5 (B1);
Area 3 (2F) hoch->4 (3F), runter->2 (1F).

Gate: `FUN_80077360(&DAT_800d4908, nachbar)` — **Area-besucht-Bank 36**. Eine nie betretene
Etage ist nicht anwaehlbar.

### 2.5 Die sichtbaren Reiter (Pfeile)

Gezeichnet in `FUN_8006e120` @0x8006e2f8-0x8006e3c0:

```
8006e304: sltiu v0,v0,0x2 ; bne -> Area <2: gar keine Pfeile
8006e320: addiu s1,zero,408
8006e324: addiu s1,s1,-194        ; 1. Durchgang y = 214  (RUNTER, Tab 0x800a9b04)
8006e328: addiu v0,zero,152 ; sh v0,2(s0)   ; x = 152
8006e330: sh s1,4(s0)                       ; y
   naechster Durchgang: y = 214-194 = 20    (HOCH,   Tab 0x800a9aec)
8006e334: lbu v0,41(fp)   ; Blinkphase DAT_800d5c19
   phase==0 -> RGB 0x80   (@0x8006e354-5c)
   phase!=0 -> RGB 0x40   (@0x8006e344-50)
Sichtbar nur wenn Nachbar>=2 UND FUN_80077360(&DAT_800d4908,nachbar)  (@0x8006e368-3b8)
```

Geometrie aus `FUN_8006dea0` @0x8006dea0: SPRT code 0x66, **14x12**, `u=14` (Pfeil 0 = RUNTER),
`u=0` (Pfeil 1 = HOCH), `v=12`, CLUT `GetClut(0x100,0x1ec)` — liegt auf der TEX-Seite,
nicht auf der Karten-Seite.

### 2.6 Der zweite Blaetterer (Karten-Item / CHECK-Screen)

Identische Tasten, andere Datenquelle: `FUN_8006d550(dir)` @0x8006d550 zykelt
`DAT_800d5c02` (0..7) durch die 8er-Gruppe `DAT_800d5c01` der Liste `DAT_800d4b68`
(24 Slots, -1 = leer).

```
8006cdec: lhu v0,8476(s0) ; andi 0x1000 -> FUN_8006d550(1)   ; HOCH   = Index -1
8006ce08: lhu v0,8476(s0) ; andi 0x4000 -> FUN_8006d550(0)   ; RUNTER = Index +1
8006ce34: jal 0x8005ba28 (a0=0x04040000)  ; gleicher Ton
```

Dieselbe `bgez DAT_800cfb74`-Repeat-Schranke @0x8006cddc-de4.
Ein zweiter, funktionsgleicher Etagen-Umschalter (Nachbar-Tabellen) liegt zusaetzlich
@0x8006ed70-0x8006ee64 (0x1000/0x4000 gegen 0x800a9aec/0x800a9b04).

---

## 3. Q3 — TUEREN und TREPPEN auf der Karte

### 3.1 GEMESSEN, nicht vermutet: beide sind Teil der Karten-GRAFIK

Karten-Seiten liegen als `info/re2leon/COMMON/DATA/MAPS/MAPS_0NN.TIM` vor
(20 Stueck, je 34336 B = TIM-Kopf + 1548 B CLUT-Block + 32780 B Pixel = **64x256 @4bpp =
256x256 px**). Die Zuordnung ist **NICHT** Datei = Area, sondern ueber das Feld
`cdFileNo` der Area-Tabelle @0x800aaa38: `MAPS_%03d` = `cdFileNo + 1`.
Beispiel: Area 2 (RPD 1F) hat `cdFileNo=4` -> `MAPS_005.TIM`; Area 3 (RPD 2F) -> `MAPS_003.TIM`.

Basis-Palette (Datei-Offset 0x14, in allen 20 identisch), RGB555 -> RGB888:

```
 0 0x0000 transparent      4 0x5ad6 (176,176,176) HELLGRAU = Wandlinie
 1 0x8120 (  0, 72,  0)    5 0x677b (216,216,200)   6 0x4eb5 (168,168,152)
   DUNKELGRUEN = Raumflaeche
 2 0x81a4 ( 32,104,  0)    7 0x294a ( 80, 80, 80)   8 0x9c85 ( 40, 32, 56)
   GRUEN = Innen-Details / Treppenstufen             9 0x9084 ( 32, 32, 32)
 3 0x12dc (224,176, 32)   11 0x12dc (224,176, 32)  12/13 0xb002/0xb005 blau
   GOLDGELB = TUER                                  14/15 0x0853/0x0850 rot
```

**Tuer** (Auszug `MAPS_005.TIM`, Area-2-Raum 3, uv=(64,64) 22x30 — Index-Art):

```
4444444444444444444444      4 = Wandlinie
4111111111111111111114      1 = Raumflaeche
4111111111221111111114      2 = Innen-Detail
3311111111221111111114   <- 3 = TUER: 2 px breit x 4 px hoch, IN der Wand
3311111111221111111114
3311111111221111111114
3311111111221111111114
4444444444444444444444
```

und quer (Raum 9, uv=(192,136) 24x18): `411111333311111111111114` ueber 2 Zeilen —
also **4 px lang x 2 px dick**. => Tuermarke = **2x4 (senkrechte Wand) / 4x2 (waagrechte
Wand), Palettenindex 3**. Sie faerbt sich automatisch mit der Zustands-CLUT des Raum-Sprites
mit; **es gibt keine Tuer-Datentabelle und keinen AOT-Bezug**.

**Treppe** (`MAPS_003.TIM`, Area 3 = RPD 2F, u=56..90 / v=88..112):

```
0044121212121212121111440      <- senkrechte Stufen: abwechselnd Index 1/2
0044121212121212121111440
0044222222222222222222400      <- Podest/Kante
0044111111111111121111400
0044111111111111122222400      <- waagrechte Stufen: Index-2-Linien
0044111111111111121111400
0044111111111111122222400
```

=> **Treppe = Schraffur aus Palettenindex-2-Linien, ebenfalls eingemalt.**

Beweis-Renderings dieser Untersuchung:
`analysis/karte_2026-08-31/MAPS_005_area02_RPD1F.png` (Schriftzug "POLICE STATION 1F")
und `MAPS_003_area03_RPD2F.png` ("POLICE STATION 2F") — je 256x256 mit Basis-Palette,
2x vergroessert. Tuerpunkte gelb, Treppenschraffuren, Titel (u=0,v=0 96x32) und
Kompass/Massstab "0 5 10 15 [m] 1:610" (u=96,v=0 32x48) sind darin sichtbar;
die Quads dazu stehen als Bytes @0x800a9b8c = `{0,0,96,32}` / `{96,0,32,48}`.

Gegenprobe "gibt es Tuer-Prims?": `FUN_8006dea0` legt genau
`1 Spieler + 2 Pfeile + 2 Fixsprites + roomCount Raum-Sprites` an (Puffer 0x8019d000,
Schrittweite 0x28 je logischem Sprite). **Kein einziges Tuer-Prim.**

### 3.2 Die 14 blinkenden Uebergangs-Marken — vollstaendig dekodiert

Tabelle **@0x800a9b1c**, 14 Eintraege a 8 B
`{u8 doneBit(Bank32); u8 area; u8 typ(Bit0-1)|szenario(Bit6-7); u8 iconSet; u16 x; u16 y}`
(Roh: `04 02 03 01 f4 00 87 00 ...`, Ende 0x800a9b8c):

```
[ 0] done= 4 area= 2 typ=3 set=1 (244,135) -> u=24
[ 1] done=19 area= 2 typ=2 set=1 (244, 91) -> u=16
[ 2] done=12 area= 2 typ=2 set=0 (211,122) -> u=48
[ 3] done=13 area= 2 typ=1 set=0 (201, 82) -> u=40
[ 4] done= 8 area= 2 typ=1 set=0 (101,108) -> u=40
[ 5] done= 5 area= 2 typ=0 set=0 ( 55,140) -> u=32
[ 6] done= 9 area= 3 typ=0 szen=0x80 set=0 ( 64,159) -> u=32
[ 7] done=14 area= 5 typ=2 set=1 (247,137) -> u=16
[ 8] done=14 area= 5 typ=2 set=1 (249,137) -> u=16
[ 9] done=22 area=13 typ=1 set=0 (176,123) -> u=40
[10] done=22 area=13 typ=1 set=0 (176,125) -> u=40
[11] done=24 area=16 typ=2 szen=0x40 set=0 (148, 53) -> u=48
[12] done=21 area=16 typ=0 set=1 (203,172) -> u= 0
[13] done=23 area=17 typ=0 set=0 ( 71, 60) -> u=32
```

**Prim-Aufbau `FUN_8006db44` @0x8006db44 (raw, in diesem Lauf disassembliert):**

```
8006db50: ori   s4,s4,0xc000        ; Puffer 0x8019c000
8006db58: addiu s7,zero,100         ; code 0x64  (SPRT, NICHT semi-transparent)
8006db60: addiu s5,zero,128         ; r=g=b=0x80
8006db68: addiu s6,zero,248         ; v = 248 = 0xF8      <-- NICHT 0xF0
8006db74: addiu s2,s2,-25714        ; s2 = 0x800a9b8e, je Runde -8 (-> Eintrag+2 = typ)
8006db7c: addiu s1,zero,112         ; s1 = 0x70, je Runde -8 (erste Runde 0x68 = Eintrag 13)
8006dbbc: lbu v0,-25825(at)         ; iconSet = [0x800a9b1f + s1]
8006dbc4: beq v0,zero,0x8006dbe0
   iconSet != 0 : u = (typ&3)*8            (@0x8006dbcc-dc)
   iconSet == 0 : u = (typ&3)*8 + 32       (@0x8006dbe0-f0)
8006dbd0/e4: sb s6,-1(s0)           ; v = 248
8006dc0c: lhu v0,-25824(at)         ; x = [Eintrag+4]
8006dc20: lhu v1,-25822(at)         ; y = [Eintrag+6]
8006dc24: addiu v0,zero,8 ; sh v0,2(s0) ; sh v0,4(s0)   ; w = h = 8
8006dbf8: addiu a0,zero,256 ; addiu a1,zero,508 ; jal GetClut  ; CLUT (0x100, 0x1fc)
Ende: SetDrawMode(0x800d6bc0/0x800d6bcc, tpage 0x17)  ; 4bpp-Seite bei VRAM (448,256)
```

**Die tatsaechlichen Icon-Pixel** (aus `MAPS_005.TIM`, u=0..63):

```
 v=248 1111110022222200333333004444440011110000222200003333000044440000
 v=249 1111110022222200333333004444440011110000222200003333000044440000
 v=250 1111110022222200333333004444440011110000222200003333000044440000
 v=251 1111110022222200333333004444440011110000222200003333000044440000
 v=252 0000000000000000000000000000000011110000222200003333000044440000
 v=253 0000000000000000000000000000000011110000222200003333000044440000
```

=> **iconSet==1 (u=0/8/16/24): waagrechter Balken 6 px breit x 4 px hoch.**
=> **iconSet==0 (u=32/40/48/56): senkrechter Balken 4 px breit x 6 px hoch.**
`typ` (0..3) waehlt nur die **Farbe**: Palettenindex 1 / 2 / 3 / 4 (dunkelgruen / gruen /
tuergelb / hellgrau). Der Rest der 8x8-Kachel ist Index 0 = transparent.
(Eine ZWEITE, kleinere Icon-Zeile liegt bei **v=240** — 4x2 bzw. 2x4 — wird von
`FUN_8006db44` aber in **keinem** der beiden Zweige benutzt; `v` ist dort hart 248.
Der aeltere Bericht/Port-Kommentar "Icons ab v=0xf0" ist damit korrigiert.)

**Zeichner `FUN_8006dcc0` @0x8006dcc0** — Marke erscheint nur, wenn ALLES gilt:

1. `FUN_80077360(&DAT_800d4924, DAT_800d5c0a)` — Karte der Area im Besitz.
   ACHTUNG: hier wird die **Area-Nummer** als Bit benutzt, waehrend der Raum-Zeichner das
   Feld `mapItemBit` der Area-Tabelle nimmt (`(&DAT_800aaa3d)[area*8]`) — fuer Area 7/8 sind
   diese vertauscht (7->Bit 8, 8->Bit 7, aus dem Binary gedumpt). Retail-Eigenheit.
2. `Marke.area == DAT_800d5c0a`.
3. `FUN_80077360(&DAT_800d4a34, Marke.doneBit) == 0` — Bank 32; **setzt das Skript das Bit,
   verschwindet die Marke.**
4. Szenario-Filter ueber Bits 0x40/0x80 gegen `DAT_800cfbd8 & 0x40000000/0x80000000`.

Blink: `DAT_800d5c19 == 0 ? RGB 0x80 : RGB 0x50`.
Global abschaltbar: Aufruf nur wenn `(DAT_800cfbd8 & 0x2000000) == 0` (@0x8006e768-Umfeld).

**Zeichenreihenfolge (AddPrim-Kette in `FUN_8006e120`):**
`Spieler -> Pfeile -> DR_MODE(0x800d6c20) -> 2 Fixsprites -> Raum-Sprites -> DR_MODE(0x800d6c08)
-> [FUN_8006dcc0:] 14 Marken -> DR_MODE(0x800d6bc0)`.
Da ein OT von hinten gerastert wird (frueher = oben), liegen die **Marken UNTER den
Raum-Rechtecken**; die Raum-Sprites sind aber semi-transparent (code 0x66), die Marken
opak (0x64) — sie scheinen also durch. Der Spieler-Pfeil liegt ganz oben.

---

## 4. Q4 — Spieler-Marke

Init `FUN_8006dea0` @0x8006dea0 (zwei Puffer): SPRT **code 0x66** (semi-transparent),
**w=h=12**, CLUT `GetClut(0x100,0x1ec)` (TEX-Seite, nicht die Karten-Seite).

Per Frame, `FUN_8006e120` @0x8006e1dc-0x8006e2ac — nur wenn
`DAT_800d5c0a == FUN_8006e7f0(Stage,Raum)` (angezeigtes Blatt == Blatt des Spielers):

```
8006e1dc: lui v0,0x91a2 ; ori v0,v0,0xb3c5   ; Magic-Divisor
8006e1e4: lw a0,[0x800cfc30]                 ; Spieler-X
8006e1ec: addiu a0,a0,28000
8006e1f0..e20c: v1 = (hi(a0*M)+a0)>>8 ; v1 -= a0>>31   ==  (X+28000)/450
8006e1f8: lw a1,[0x800cfc38] ; addiu a1,a1,28000       ; Spieler-Z
8006e24c..e25c: v0 = (Z+28000)/450 ; v0 = -v0          ; Y-Achse gespiegelt
8006e240/e274: += DAT_800d5c48 / DAT_800d5c4a          ; (im Spiel immer 0)
8006e220: lh v0,[0x800cfc6e]                 ; Blickrichtung (Yaw)
8006e234: addiu v0,v0,256 ; sra v0,v0,9 ; andi s2,v0,0x7   ; 8 Richtungen
8006e280..8c: sb (s2*3)<<2 -> u              ; u = richtung*12
8006e290: sb zero,13(s4)                     ; v = 0
```

Divisor verifiziert: `0x91a2b3c5` reproduziert exakt `x/450` fuer alle geprueften Werte
(0..60000 in 997er-Schritten + Randwerte, 0 Abweichungen).
**450 Welteinheiten = 1 Kartenpixel.**

Endposition = Projektion **+ Marker-Basis des aktuellen Raum-Eintrags** (`Eintrag+8` = mx,
`Eintrag+10` = my) — jeder Raum bringt also seinen eigenen Welt->Karte-Ursprung mit.

Puls (kein Blinken, sondern Helligkeitsrampe), Zaehler `DAT_800d5c18`, Phase `DAT_800d5c19`,
State 3 @0x8006d87c-0x8006d8d4:

```
Phase 0: if (z < 0x51) bleibt Phase 0 ; sonst Phase = 1 ; z += 2
Phase 1: if (z < 0x0a) Phase = 0      ;                   z -= 2
```

=> z laeuft in 2er-Schritten durch **6 ... 84**; `RGB = z + 0x28` => **0x2E ... 0x7C**
(`cVar = DAT_800d5c18 + 0x28` im Decompile, `addiu`-Kette @0x8006e2a0-ac).

Der **aktuelle Raum blinkt im Map-Tab NICHT** — er steht konstant auf CLUT-Basis+1
(`addiu s5,...` @0x8006e5d0/@0x8006e614 + `+1`). Blinkende Raeume gibt es nur in der
CHECK-Variante `FUN_8006f1c4`.

---

## 5. Q5 — EMPFEHLUNG fuer den Port

Reihenfolge = Aufwand/Nutzen. Alles unten ist RE2-belegt; wo eine Zahl eine Port-Wahl
bleibt, steht es dabei.

### 5.1 Tuermarke: Geometrie + Farbe auf die RE2-Messwerte ziehen (klein, sofort)

`re15_port/engine/src/re15_inv_screen.c:1459-1465` zeichnet heute
`5 px x 2 px, RGB(240,200,64)`.
RE2 gemessen: **4 px x 2 px** (waagrechte Wand) bzw. **2 px x 4 px** (senkrechte Wand),
Farbe = Palettenindex 3 = `0x12dc` = **RGB(224,176,32)**.
Zu bauen: im Marken-Datensatz (`re15_port/engine/src/re15_map_zones.h:214`,
`re15_map_mark_t`) ein Orientierungs-Bit ergaenzen — aus dem Seitenverhaeltnis des
RE1.5-Tuer-Rechtecks (`w > h` -> waagrecht) — und in `re15_inv_screen.c` die beiden
Groessen + die Farbe setzen.

### 5.2 Treppenmarke: das erfundene Leiter-Symbol durch RE2s BALKEN ersetzen (klein)

Heute (`re15_port/engine/src/re15_inv_screen.c:1466-1481`): 7x7-Grauklotz + drei
5x1-Sprossen — frei erfunden.
RE2: **ein einziger deckender Balken**, `6x4` (waagrecht) bzw. `4x6` (senkrecht), eine
Farbe, und er **blinkt** zwischen RGB `0x50` und `0x80` mit der Karten-Blinkphase.
Zu bauen:

* denselben Orientierungs-Bit wie 5.1 nutzen,
* eine Blinkphase im Map-Screen mitfuehren (RE2: Zaehler +/-2, Phasenwechsel bei `>=0x51`
  bzw. `<0x0a` — im Port existiert mit `ecg_glow` bereits ein Puls, aber mit anderer
  Kadenz; sauberer ist ein eigener Zaehler nach RE2s Regel),
* Farbe: RE2 waehlt sie ueber `typ` aus derselben Basispalette (Index 1/2/3/4). Fuer eine
  Treppe im Port ist Index 2 = `0x81a4` = RGB(32,104,0) die naechstliegende Wahl;
  die Zuordnung "RE1.5-Treppe -> typ" ist eine **Port-Wahl**, kein RE2-Fund.
* optional (RE2-treu): Marke verschwinden lassen, sobald der Uebergang benutzt wurde —
  RE2 macht das ueber Bank-32-Bits; der Port hat mit `s_visited` bereits ein passendes
  Bitfeld.

### 5.3 Etagenwechsel per D-Pad HOCH/RUNTER nachbauen (mittel, groesster Nutzen)

Der Port-Map-Runner `map_mode()` (`re15_port/engine/src/menu_common.c:1377-1440`) hat
denselben Aufbau wie RE2 (State 0 Slide-in / 1 interaktiv / 2 Slide-out); RE2s
Etagenlogik ist ein reiner **Zusatz in State 1**:

```
if (repeat_tick && seite_hat_nachbarn) {
    if (held & UP   && nachbar_oben  != KEINER && seite_besucht(nachbar_oben))  seite = nachbar_oben;
    if (held & DOWN && nachbar_unten != KEINER && seite_besucht(nachbar_unten)) seite = nachbar_unten;
    if (seite geaendert) { ton; seite neu laden; }
}
```

Konkret zu bauen:

1. **Nachbar-Tabelle Seite->Seite (hoch/runter)** fuer RE1.5s 14 MAP-Seiten. RE2 hat sie
   als `DAT_800a9aec`/`DAT_800a9b04`; RE1.5 hat **kein** Gegenstueck — das ist neu zu
   autorisieren (die Zonen-Tabelle `re15_map_zones.h` kennt die Seiten bereits, die
   Stockwerks-Reihenfolge ergibt sich aus den PIX-Titeln "B1 / 1F / 2F / ...").
2. **Auto-Repeat** nach RE2: erste Wiederholung nach **10** Frames, danach alle **6**
   (Inventar-Task-Konfiguration `FUN_80039464(0xF01C, 0x060A)` @0x800689ec).
3. **Sichtbarkeitsschranke**: Seite nur anwaehlbar, wenn schon besucht
   (`re15_map_visited` / `s_visited` liefert das bereits).
4. **Zwei Pfeil-Reiter** zeichnen, x=152, y=20 (hoch) / y=214 (runter), 14x12,
   RGB blinkt 0x80<->0x40 — nur sichtbar, wenn die jeweilige Nachbarseite besucht ist.
   RE1.5s TEX-Seite liefert keine Pfeil-Grafik an RE2s uv (0/14, 12); das Symbol ist
   also eine **Port-Wahl** (z.B. ein 14x12-Dreieck aus FILL-Ops).
5. Beim Wechsel den Ton spielen (RE2: `0x04040000`); das RE1.5-Aequivalent im Port ist
   `se4(...)` — welcher Index, ist eine Port-Wahl.

### 5.4 Bestaetigung fuer eine bereits getroffene Port-Entscheidung

`re15_inv_screen.c:161-165` waehlt die Seite inzwischen ueber den **Bereich**, in dem der
Spieler steht, statt nur ueber die Raumnummer (Nutzer-Report: nach dem Treppenlauf blieb
die Karte auf der alten Ebene). RE2 macht genau dasselbe: `FUN_8006e7f0` zieht bei
mehrstoeckigen Raeumen die **Etagen-Bande** `DAT_800cfcfe` hinzu (@0x8006e800). Die
Port-Entscheidung ist damit RE2-gedeckt.

### 5.5 Was NICHT uebernommen werden sollte

* **Pan/Scroll**: RE2 hat keinen — `DAT_800d5c48/4a` sind immer 0. Der Port hat auch
  keinen; so lassen.
* **Kein Raum-Cursor** im In-Game-Map-Tab (nur im Karten-Item/CHECK-Screen).
* **CLUT-Umschaltung** als Faerbe-Mechanismus: RE2 faerbt ueber CLUT-Zeilen
  (0x1f2/0x1f5/0x1f6/0x1f7/0x1fa/0x1fb/0x1fc/0x1fd/0x1fe). Deren RGB-Inhalt liegt in
  RE2-CD-Datei 170 und ist **lokal nicht vorhanden** (die 20 `MAPS_0NN.TIM` tragen nur
  die 16er-Basispalette bei Datei-Offset 0x14; der Rest des 1548-B-CLUT-Blocks ist bis auf
  zwei Eintraege 0). Der Port moduliert stattdessen RGB — das ist bereits so gebaut und
  bleibt die richtige Entscheidung, solange die Zeilen fehlen.

---

## 6. Offene Punkte (ehrlich)

1. **RGB der Zustands-CLUT-Zeilen 0x1f2/0x1f5/0x1f6/0x1f7/0x1fa/0x1fb/0x1fc/0x1fd/0x1fe.**
   Nicht im lokalen Baum. Weg: RE2-Leon-Disc-Image -> Datei @ `LBA[0x80098df8] + area*6`,
   6 Sektoren, TIM parsen; **oder** DuckStation-VRAM-Dump mit offenem Map-Screen,
   Zeilen x=256..271, y=0x1f2..0x1fe auslesen. Bis dahin sind konkrete Zustandsfarben
   Hypothese — die *Logik* (welcher Zustand welche Zeile) ist belegt.
2. **Kleine Icon-Zeile v=240** (4x2 / 2x4) — existiert in jeder Karten-Seite, wird von
   `FUN_8006db44` nicht benutzt. Wer sie benutzt, ist nicht gefunden. Weg: Xref-Suche auf
   `sb`-Stores des Werts 0xf0 in 0x8006c000-0x80072000 bzw. auf weitere
   `GetClut(0x100,0x1fc)`-Aufrufer.
3. **Ton-Ids** `0x04040000` (Etagenwechsel) / `0x04050000` (Abbruch) / `0x04090000`
   (Slide-in fertig) sind belegt, ihre RE1.5-Entsprechung ist nicht ermittelt.
4. **RE1.5-Seiten-Nachbarschaft** (welche MAP-Seite liegt ueber/unter welcher) ist im
   RE1.5-Auslieferungsstand nicht vorhanden und muss autorisiert werden — kein RE-Fund.
5. Die genaue Bedeutung von `typ` 0..3 bei den 14 Marken (Farbe ist belegt, die
   *Semantik* — Treppe / Aufzug / Rolltor / verschlossen — nicht). Weg: die 14
   Bildschirmpositionen gegen die Raum-Rechtecke der jeweiligen Area schneiden und mit
   den bekannten RPD-Uebergaengen abgleichen.
