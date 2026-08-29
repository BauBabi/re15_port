# RE2-Retail-Kartensystem — byte-true RE (FINAL)

Quellen: `info/re2leon/PSX.EXE` (SLUS-00748, PS-X-EXE t_addr=0x80010000), `ghidra_re2_Leon.txt`,
`RE2_Quellcode_V2/FUN_*.c`, `COMMON/DATA/MAPS.PIX`. Alle Adressen mit
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` gegen das Binary verifiziert.
Unbelegtes ist explizit als HYPOTHESE/LÜCKE markiert.

## 0. Architektur in einem Absatz

Die RE2-Karte ist **keine Vektor-Zeichnung**. Pro „Area" (20 Stück, z.B. City, RPD 1F,
RPD 2F, …) existiert eine vorgemalte 4bpp-Texturseite 256×256 px (`MAPS.PIX`, 20 Blöcke
à 32768 B). Jeder Raum ist ein rechteckiger Ausschnitt daraus und wird als **ein
semi-transparentes SPRT-Prim** gezeichnet. Der Zustand (aktuell=rot / besucht=grün /
unbesucht-mit-Karte=dunkel / unbesucht-ohne-Karte=gar nicht) wird **ausschließlich über
die CLUT-Auswahl** des Sprites geschaltet (`GetClut(0x100, zeile)`), nie über Prim-RGB.
Besucht-Bits sind Skript-Flag-Bänke im EXE-RAM. Türen sind als gelbe Pixel in die
Raum-Grafik eingemalt; Treppen/Übergänge sind zusätzlich 14 eigenständige, blinkende
8×8-Marker-Sprites.

---

## 1. Code-Landkarte (alles im EXE, kein Overlay)

| Adresse | Rolle |
|---|---|
| `FUN_8006d650` @0x8006d650 | Map-Screen-Statemaschine; State-Byte 0x800d5bf2, Jumptable @**0x80011c98** = {0x8006d6a8, 0x8006d7c4, 0x8006d820, 0x8006d87c, 0x8006da24} |
| State 0 @0x8006d6a8 | Area bestimmen (`FUN_8006e7f0`), CD-Load „MAP FILE" (String @**0x80011c88**) → RAM 0x801a0000, `LoadImage(RECT{448,256,64,256})` @0x8006d774-0x8006d798, `FUN_8006dea0` (Prim-Init) |
| State 1/2 @0x8006d7c4/0x8006d820 | Rein-/Raus-Zoom, 13 Schritte (`sltiu v0,v0,0xd` @0x8006d7d0), Prim-Verschiebung ±22/±19/±21 (@0x8006d7f4-0x8006d81c) |
| State 3 @0x8006d87c | Interaktiv; **Blink-Zähler** 0x800d5c18: +2/Frame bis >0x50 (`sltiu v0,v0,0x51` @0x8006d8b8) → Phase 0x800d5c19=1, dann −2 bis <10 (`sltiu v0,v0,0xa` @0x8006d894) → Phase 0 |
| State 4 @0x8006da24 | Area-Wechsel: erneuter Load + LoadImage (@0x8006dad4-0x8006daf4) |
| `FUN_8006dea0` @0x8006dea0 | Prim-Init aller Map-Sprites (Puffer 0x8019d000) |
| `FUN_8006e120` @0x8006e120 | **Raum-Zeichner** (Map-Tab; Spielerposition = Marker) |
| `FUN_8006f1c4` @0x8006f1c4 | Raum-Zeichner „CHECK/Karten-Item"-Variante (Raum-Cursor; Cursor-Raum blinkt; Blink-Flip-Sound `FUN_8005ba28(0x22b0000)` @0x8006f234-0x8006f238 beim Zähler-Umschlag `sltiu @0x8006f22c`) |
| `FUN_8006db44` @0x8006db44 | Prim-Init Tür-/Treppen-Marker (Puffer 0x8019c000) |
| `FUN_8006dcc0` @0x8006dcc0 | Zeichner Tür-/Treppen-Marker (blinkend) |
| `FUN_8006e7f0` @0x8006e7f0 | (stage 0..6, raum) → Area 0..0x13; Sonderfälle über Flag-Bits (z.B. `FUN_80077360(&DAT_800d4854, 0xbe/0x63/0x62/0x89)`, B-Szenario `DAT_800cfbd8 & 0x40000000`) |
| `FUN_8006eae8` @0x8006eae8 | (area, raum-Nr) → Index in die Area-Raumtabelle (kleine switch-Remaps, z.B. Area 0: raum ≥0x18 → idx−0x14; Area 3: −8/−9, 0x1b→0xe) |
| `FUN_8006d550` @0x8006d550 | Floor hoch/runter zyklisch über Liste 0x800d4b68 (s. §5.3) |
| `FUN_8006931c` @0x8006931c | **Besucht-Setzer** (§4.1) |
| `FUN_80069eec` @0x80069eec | VRAM-Sicherung der Map-Texturseite: `StoreImage/LoadImage(RECT{0x1c0,0x100,0x40,0x100})` ↔ RAM 0x801b5000 (die Seite (448,256) wird im Gameplay anderweitig genutzt) |
| `FUN_80076a40` @0x80076a40 | Generischer TIM-Uploader mit **CLUT-Zeilen-Stapler**: `crect->y = 0x1e0 + Zähler(0x800cfbf0 Byte1)`, `prect->x = Zähler(Byte0)*0x40 (−0x400 wenn >0xf, y=256)`; Zähler läuft nach jedem Upload weiter |
| Bit-Primitiven | **SET `FUN_8007730c` @0x8007730c, CLEAR `FUN_80077334` @0x80077334, TEST `FUN_80077360` @0x80077360**; MSB-first: `word[n>>5] op (0x80000000 >> (n&31))` — Instruktionen: `srl a2,a1,5 / sll a2,a2,2 / addu / andi a1,0x1f / lui v0,0x8000 / srlv / or|nor+and / sw` |

Texturseite: DR_MODE-Prims @0x800d6bc0/0x800d6bcc, `SetDrawMode(..,tpage=0x17,..)`
@0x8006db98-Ende von FUN_8006db44. tpage 0x17 ⇒ Farbtiefe (0x17>>7)&3=0 = **4bpp**,
VRAM-x (0x17&0xf)*64 = **448**, VRAM-y ((0x17>>4)&1)*256 = **256**.

---

## 2. KARTENDATEN

### 2.1 Pixel-Daten

`COMMON/DATA/MAPS.PIX` = 20 Blöcke à **32768 B** = 64 VRAM-Wörter × 256 Zeilen (4bpp ⇒
256×256 px). Der Block der Area wird ROH (ohne TIM-Header) nach VRAM (448,256) geladen
(LoadImage-RECT-Aufbau `{448,256,64,256}` @0x8006d77c-0x8006d798, identisch @0x8006dadc,
0x8006f0d0, 0x8006f7f4). CD-Subdatei-Tabelle @**0x800a9414** (8 B/Eintrag): +0 u32 Länge
(alle 20 = 32768), +4 u16 Sektor-Offset (0x00,0x10,0x20,… — 16 Sektoren Abstand);
Basis-LBA zur Laufzeit in 0x80098e00/0x80098e02; Datei-Anforderung als Id 171,
Debug-Name „MAP FILE" @0x80011c88 (Aufbau @0x8006d6c0-0x8006d770).

Vorgemalt in der Seite: Raumflächen (Grün-Indizes), Umrisse (Grau-Indizes), **gelbe
Türpunkte** an den Raumkanten, Areaname (Schriftzug oben links, 96×32 @u=0,v=0) und
Kompass/Maßstab (32×48 @u=96,v=0) — die beiden letzten aus Byte-Quads @**0x800a9b8c**
(`u,v,w,h` = {0,0,96,32} und {96,0,32,48}, Init @0x8006dea0, CLUT `GetClut(0x100,0x1f1)`).
Ab v=0xf0 liegt die 8×8-Icon-Zeile für Treppen-Marker (gemessen: nur MAPS.PIX-Blöcke
2-5 und 13-17 haben dort Pixel — genau die Areas, für die Marker existieren, §5).

### 2.2 Area-Tabelle @**0x800aaa38** — 20 Einträge à 8 B: `{u32 roomTablePtr; u8 roomCount; u8 mapItemBit; u16 cdFileNo}`

| Area | RoomTable | Räume | mapItemBit | CD-File |
|---|---|---|---|---|
| 0 | 0x800aa1a8 | 8 | 0 | 0 |
| 1 | 0x800aa228 | 4 | 1 | 1 |
| 2 | 0x800aa388 | 18 | 2 | 4 |
| 3 | 0x800aa268 | 15 | 3 | 2 |
| 4 | 0x800aa358 | 3 | 4 | 3 |
| 5 | 0x800aa4a8 | 11 | 5 | 5 |
| 6 | 0x800aa558 | 11 | 6 | 6 |
| 7 | 0x800aa608 | 6 | 8 | 7 |
| 8 | 0x800aa668 | 13 | 7 | 8 |
| 9 | 0x800aa738 | 4 | 9 | 9 |
| 10 | 0x800aa778 | 3 | 10 | 10 |
| 11 | 0x800aa7a8 | 3 | 11 | 11 |
| 12 | 0x800aa7d8 | 2 | 12 | 12 |
| 13 | 0x800aa7f8 | 6 | 13 | 13 |
| 14 | 0x800aa858 | 1 | 14 | 14 |
| 15 | 0x800aa868 | 1 | 15 | 15 |
| 16 | 0x800aa878 | 13 | 16 | 16 |
| 17 | 0x800aa948 | 7 | 17 | 17 |
| 18 | 0x800aa9b8 | 6 | 18 | 18 |
| 19 | 0x800aaa18 | 2 | 19 | 19 |

Zugriffe (belegt): Ptr `(&PTR_DAT_800aaa38)[area*2]` (FUN_8006e120 @0x8006e120-Kopf),
Count `(&DAT_800aaa3c)[area*8]`, mapItemBit `(&DAT_800aaa3d)[area*8]`,
CD-File `lhu @0x800aaa3e+area*8` (@0x8006d6d4-0x8006d6dc).

### 2.3 Raum-Eintrag — 0x10 Bytes

Belegt durch Init `FUN_8006dea0` (u/v/w/h → SPRT +0xc/+0xd/+0x10/+0x12) und
Draw `FUN_8006e120` (x/y/Flags):

```
+0x00 u8  u        Textur-U des Raum-Stücks       +0x08 s16 mx  Marker-Basis-X
+0x01 u8  v        Textur-V                       +0x0a s16 my  Marker-Basis-Y
+0x02 u8  w        Breite px                      +0x0c u8  visBit  → Bit in 0x800d490c (BESUCHT)
+0x03 u8  h        Höhe px                        +0x0d u8  altBit  → Bit in 0x800d4920 (Map-Event)
+0x04 s16 x        Screen-X (+Pan 0x800d5c48)     +0x0e u8  hideLeon   bit0=A-, bit1=B-Szenario ausblenden
+0x06 s16 y        Screen-Y (+Pan 0x800d5c4a)     +0x0f u8  hideClaire (Spieler: DAT_800cfc00&1; B-Szenario: DAT_800cfbd8&0x40000000)
```

**Dekodierte Beispiel-Räume (Stage 1)** — Area 0 „CITY AREA" @0x800aa1a8:

```
[0] uv=(  0, 48) wh=(109,86) xy=(105, 27) marker=( 86, 99) visBit= 0   ; ROOM100 (Straßenzug)
[1] uv=(168,128) wh=( 25,33) xy=( 81, 79) marker=( 71,112) visBit= 1   ; ROOM101
[2] uv=(144, 16) wh=( 38,80) xy=( 81,112) marker=( 75,216) visBit= 2   ; ROOM102
[3] uv=(  0,144) wh=(125,41) xy=(169,164) marker=(165,201) visBit= 3   ; ROOM103
[4] uv=(152,168) wh=( 43,73) xy=( 40,119) marker=( 33,189) visBit=24   ; ROOM118 (City-Rückweg)
...[5..7] visBit=25..27 = ROOM119..11B
```

Area 2 (RPD 1F) @0x800aa388, erste 3: `[0] uv=(0,112) wh=(92,90) xy=(127,92) visBit=30`,
`[1] uv=(24,80) wh=(32,25) xy=(99,139) visBit=31`, `[2] uv=(128,16) wh=(52,38) xy=(79,115) visBit=32`
— visBit = kumulativer Raum-Index (StageBasis + Raum-Nr, §4.1: Stage2-Basis=0x1e=30 ✓).

Einträge mit Sonder-Flags (vollständiger Scan): altBit≠0 nur bei
area2/r14 (altBit 8), area3/r2 (1), area3/r14 (2), area5/r8 (13), area6/r8 (4),
area8/r1 (10), area16/r5,6 (5,6), area17/r4 (7); Szenario-Hides u.a. area5/r0,2
(03/03 = immer versteckt für beide?? — 03 = A+B, d.h. dieser Spieler sieht den Raum nie),
area7/r0,1 (03/00 = nur Claire sieht ihn), area7/r2,3 (00/03 = nur Leon).

### 2.4 Prim-Aufbau (FUN_8006dea0, Puffer 0x8019d000)

Doppelt gepuffert: Frame-B-Prim = +0x14, Schrittweite 0x28 pro logischem Sprite;
OT = `0x800cc1fc + fb*0x20`. Reihenfolge:

1. **Spieler-Marker**: SPRT 12×12, Code **0x66** (SPRT|semi-trans, `sb 0x66 → prim+7`),
   RGB 0x80, CLUT `GetClut(0x100,0x1ec)`. Im Draw: u = `((yaw+0x100)>>9 & 7)*12`
   (8 Richtungsframes à 12 px; yaw=DAT_800cfc6e; `sb v0,12(s4)` @0x8006e28c),
   RGB = Blinkzähler+0x28 (@0x8006e2a0-0x8006e2ac ⇒ 0x28..0x78 pulsierend),
   Position = Raum-marker-Basis + Spieler-Weltposition:
   `mapX = (DAT_800cfc30+28000)/0x1c2`, `mapY = −((DAT_800cfc38+28000)/0x1c2)`
   (0x1c2 = 450 Welteinheiten/Pixel; FUN_8006e120 Z.37-38, Instr. @0x8006e264-0x8006e2e8).
   Gezeichnet nur wenn angezeigte Area == Spieler-Area.
2. 2× **Floor-Tab-Pfeile** 14×12 (u=0xe bzw. 0, v=0xc), CLUT 0x1ec. Draw-Bedingung
   (@0x8006e368-0x8006e3b8): Nachbar-Area aus `DAT_800a9b04[area]` (abwärts) bzw.
   `DAT_800a9aec[area]` (aufwärts) — Werte sind AREA-NUMMERN (z.B. Area2: auf=3(RPD2F),
   ab=5(B1); Area3: auf=4, ab=2) — Pfeil erscheint nur wenn Nachbar-Area ≥2 und deren
   **Area-besucht-Bit** gesetzt (`FUN_80077360(&DAT_800d4908, nachbar)` @0x8006e3a0-0x8006e3a8);
   RGB blinkt 0x80 ↔ 0x40 mit Phase 0x800d5c19 (@0x8006e334-0x8006e35c).
3. 2× Fix-Sprites (Areaname-Schriftzug 96×32, Kompass 32×48; Quads @0x800a9b8c), CLUT 0x1f1.
4. **1 SPRT pro Raum** (u/v/w/h aus Eintrag), Code 0x66, RGB 0x80, CLUT initial 0x1f2.

---

## 3. ZUSTANDSFARBEN

### 3.1 Auswahl-Algorithmus (FUN_8006e120; Instruktions-Belege 0x8006e4c4-0x8006e72c)

```
clutY = 0x1f5                                    ; li s5,0x1f5 @0x8006e5d0 / 0x8006e614
if TEST(0x800d4920, e.altBit): clutY = 0x1fa     ; li s5,0x1fa @0x8006e620 (u.a.)
if (angezeigteArea == SpielerArea && idx == SpielerRaumIdx):
    clutY += 1                                   ; → 0x1f6 bzw. 0x1fb
if TEST(0x800d4924, area.mapItemBit):            ; Karte der Area im Besitz
    if !TEST(0x800d490c, e.visBit):              ; unbesucht
        if SzenarioHide(e[0xe|0xf]): SKIP        ; Raum existiert für diesen Spieler/Szenario nicht
        clutY = 0x1f2                            ; (Pfad @0x8006f5d8 in der CHECK-Variante identisch)
        if TEST(0x800d4920, e.altBit): clutY = 0x1f7   ; li s5,0x1f7 @0x8006e72c
    DRAW
else:
    if TEST(0x800d490c, e.visBit): DRAW          ; besucht → zeichnen
    else: SKIP                                   ; unbesucht ohne Karte → KEIN Prim
DRAW: sprt.clut = GetClut(0x100, clutY); AddPrim
```

**Matrix (CLUT-Zeile bei VRAM x=0x100):**

| Zustand | CLUT-Zeile |
|---|---|
| besucht | **0x1f5** |
| besucht + Map-Event-Bit (altBit) | **0x1fa** |
| **AKTUELLER Raum** | Basis **+1** → **0x1f6** / **0x1fb** |
| unbesucht, Karte im Besitz | **0x1f2** |
| unbesucht, Karte im Besitz, altBit | **0x1f7** |
| unbesucht, keine Karte | **nicht gezeichnet** („schwarz" = Screen-Hintergrund) |

Antworten auf die Detailfragen:
- **Prim-Typ**: Flächen-SPRT (Code 0x66, semi-transparent) — KEINE LINE_F2/POLY_F4-Zeichnung;
  Umrisse und Türpunkte sind Pixel der Textur. (Die einzigen `setLineF2/G2`-Kommentare im
  Decompilat sind Fehldeutungen von Byte-Stores 0x40/0x50 in RGB/UV-Felder.)
- **Blinkt der aktuelle Raum?** Im Map-Tab (FUN_8006e120): nein — er steht konstant auf
  Basis+1; es pulsiert der Spieler-Positionspfeil (RGB 0x28..0x78). In der CHECK-Variante
  (FUN_8006f1c4 @0x8006f490-0x8006f4dc): ja — Cursor-Raum wechselt mit Phase 0x800d5c19
  zwischen Basis+1 (0x1f6/0x1fb) und 0x1f2/0x1f7.
- **RGB-Bytes im Prim** sind immer 0x80,0x80,0x80 (Raum-Sprites); Farbe kommt zu 100 %
  aus der CLUT-Zeile.

### 3.2 Sonderfall Area 2 (RPD 1F), Raum-Eintrag 0xe (@0x8006e4b0-0x8006e5c8 bzw. 0x8006f39c-0x8006f4dc)

Der Raum tauscht per Map-Event-Bits (Bank 33, Bits 8/9/0xb/0xc) sein Texturstück
(`v = 0x50 ↔ 0x98`, u aus Eintrag[0xe].u @Tabellen-Offset 0xe0) und nutzt CLUT-Zeilen
0x1fd/0x1fe (bzw. 0x1fa/0x1f5, in der CHECK-Variante 0x1f2/0x1fd/0x1f5) — der
umbaubare Treppen-/Durchgangs-Raum, dessen Karten-Darstellung sich mit dem
Spielfortschritt ändert.

### 3.3 Paletten-Inhalte

Basis-16er-Palette der Kartengrafik (RGB555; identisch in allen 20 extrahierten
`COMMON/DATA/MAPS/MAPS_0xx.TIM` @Datei-Offset 0x16):

```
 0 0x0000 transparent    4 0x5ad6 grau 22        8 0x9c85 blaugrau     c 0xb002 blau
 1 0x8120 grün dkl (G9)  5 0x677b hellgrau       9 0x9084 dunkel       d 0xb005 blau
 2 0x81a4 grün (R4 G13)  6 0x4eb5 grau 21        a 0x0000              e 0x0853 ROT (R19 G2 B2)
 3 0x12dc GELB (Türen)   7 0x294a grau dkl       b 0x12dc gelb         f 0x0850 rot (R16)
```

**Upload-Mechanismus der Zustands-Zeilen** (verifiziert): Der generische Uploader
`FUN_80076a40` überschreibt die TIM-Header-CLUT-Position: `crect->y = 0x1e0 +
Zeilenzähler` (Byte 1 von 0x800cfbf0), CLUT-x bleibt aus dem TIM. Beim Boot lädt
0x8002b8ac-0x8002b8d8 „TEX TIM" (Datei 225) mit Zähler 0x1c → dessen 19 CLUT-Zeilen
(Header: x=0x100!) belegen **0x1e0..0x1f2** — deckt 0x1ec (Marker), 0x1f1 (Kompass),
0x1f2 (Unbesucht-Basis) ab. Beim Öffnen des Karten-Screens/CHECK lädt
0x80071bc8-0x80071c64 die Karten-Datei Id **170** (Länge 0x3000, Basis-LBA
@0x80098df8 + area*6 Sektoren, Name @0x80011d5c) mit Zähler 0x817 → deren CLUT-Zeilen
landen ab **0x1e8** aufwärts (weitere Pfade mit 0x917/0xa17 @0x80071d6c/0x800729c8/0x80072a74;
Karten-Item-CHECK aus dem Inventar @0x8006c774 ebenfalls 0x917).

⛔ **DATEN-LÜCKE (ehrlich markiert):** Die Roh-RGB555-Werte der Zustands-Zeilen
0x1f5/0x1f6/0x1f7/0x1fa/0x1fb/0x1fd/0x1fe stecken in CD-Datei 170 (bzw. dem von ihr
gestapelten CLUT-Block) — diese Datei liegt im lokalen Extrakt-Baum **nicht** vor
(`MAPS.PIX` = nur Pixel, Zeilen 240-255 der Blöcke unter x<64 sind leer bis auf die
Icon-Grafik; kein TIM im gesamten `info/re2leon`-Baum trägt CLUT-Zeilen ≥0x1f3).
Byte-genau belegt sind: die Zeilen-ZUORDNUNG je Zustand (§3.1), der Upload-Weg, die
Basis-Palette. HYPOTHESE für die Optik (Nutzer-/Community-Beobachtung, NICHT
byte-verifiziert): Zeile des aktuellen Raums füllt die Rauminnen-Indizes rot (wie
Basis-Indizes e/f), Besucht-Zeile grün (wie 1/2), Unbesucht-Zeile stark abgedunkelt.
**Beschaffungsweg:** RE2-Leon-Disc-Image → Datei @LBA(0x80098df8)+area*6, 6 Sektoren,
TIM parsen; alternativ DuckStation-VRAM-Dump mit offenem Map-Screen, Zeilen
(x=256..271, y=0x1f2..0x1fe) auslesen.

---

## 4. BESUCHT-VERWALTUNG

### 4.1 Setzen — beim Raum-Laden

`FUN_80049e48` (Raum-Setup: RDT laden @0x8004a1b8-Umfeld, Pointer-Fixups, Subsystem-Inits)
ruft als LETZTEN Schritt `FUN_8006931c` @0x8006931c:

```
bitIndex = stageBase[DAT_800d481c] + DAT_800d481e        ; Stage 0..6, Raum-Nr
stageBase = {0, 0x1e, 0x3a, 0x48, 0x59, 0x63, 0x7b}      ; switch @FUN_8006931c
FUN_8007730c(&DAT_800d490c, bitIndex)                    ; RAUM-besucht-Bit
FUN_8007730c(&DAT_800d4908, FUN_8006e7f0(stage, raum))   ; AREA-besucht-Bit
```

Der `visBit` jedes Karten-Raum-Eintrags (§2.3) ist genau dieser kumulative Index.

### 4.2 Die Flag-Bänke

Alle Karten-Arrays sind Einträge der SCD-Flag-Bank-Tabelle @**0x800a78c8**
(32-bit-Pointer je Bank; Nutzer: Skript-Opcode-Handler @0x800519dc „Ck" und
@0x80054384/0x800543e8 „Set/Reset"):

| Bank | RAM-Adresse | Inhalt |
|---|---|---|
| 9 | **0x800d490c** | Raum besucht (kumulativer Index; 0x14 Bytes = 160 Bits) |
| 32 | **0x800d4a34** | Tür-/Treppen-Marker „erledigt" (Marker verschwindet) |
| 33 | **0x800d4920** | Map-Event-Bits (altBit der Räume, Raum-Umbau Area2/0xe) |
| 34 | **0x800d4924** | **Karten-Item im Besitz** (Bit = mapItemBit der Area) |
| 36 | **0x800d4908** | Area besucht |

Bank 34 hat **keinen direkten Code-Schreiber im EXE** (einziger Daten-Xref = Bank-Tabelle
@0x800a794c) ⇒ Karten-Besitz wird ausschließlich per Skript-„Set"-Opcode beim
Aufheben/Benutzen des Karten-Items gesetzt. Ebenso Bänke 32/33.

„Besucht" vs. „Karte bekannt" unterscheidet der Zeichner exakt wie §3.1: Karte zeigt
ALLE (nicht szenario-versteckten) Räume der Area in der Unbesucht-Palette; ohne Karte
existieren nur besuchte Räume auf dem Screen.

### 4.3 Save-Serialisierung

Die Karten-Bänke liegen im zusammenhängenden Spielzustands-Block (0x800d48xx-0x800d4bxx,
direkt neben Stage/Raum @0x800d481c/0x800d481e). **Der Memcard-Kopiercode wurde lokal
nicht nachgewiesen** — er liegt nicht im EXE-Decompilat (keine weiteren Daten-Xrefs auf
die Bankbasen), sondern mutmaßlich im `COMMON/BIN/MEM_CARD.BIN`-Overlay (lädt
@0x80100000; hier nicht decompiliert) → OFFEN, mit klarem Suchpfad. (Verhalten im
Retail-Spiel — Besuchtstand übersteht Save/Load — ist Community-Konsens, hier nicht
byte-belegt.)

---

## 5. TÜREN & TREPPEN

### 5.1 Türen: Teil der Kartengrafik

Türpunkte sind gelbe Pixel (Basis-Palette Index 3/0xb = 0x12dc) **innerhalb des
Raum-Texturstücks** — sie werden mit dem Raum-Sprite gezeichnet und wechseln ihre
Farbe automatisch mit dessen Zustands-CLUT. Es gibt KEINE separate Tür-Datenliste
für die Kartenanzeige.

### 5.2 Treppen/Übergänge: 14 blinkende 8×8-Marker

Tabelle @**0x800a9b1c**, 14 Einträge à 8 B:
`{u8 doneBit(Bank32); u8 area; u8 typ(bits0-1)|szenario(bits6-7); u8 iconSet; u16 x; u16 y}`

```
[ 0] bit  4 area 2  typ 3 set1 (244,135)   [ 7] bit 14 area  5 typ 2 set1 (247,137)
[ 1] bit 19 area 2  typ 2 set1 (244, 91)   [ 8] bit 14 area  5 typ 2 set1 (249,137)
[ 2] bit 12 area 2  typ 2 set0 (211,122)   [ 9] bit 22 area 13 typ 1 set0 (176,123)
[ 3] bit 13 area 2  typ 1 set0 (201, 82)   [10] bit 22 area 13 typ 1 set0 (176,125)
[ 4] bit  8 area 2  typ 1 set0 (101,108)   [11] bit 24 area 16 typ 2+0x40 set0 (148,53)
[ 5] bit  5 area 2  typ 0 set0 ( 55,140)   [12] bit 21 area 16 typ 0 set1 (203,172)
[ 6] bit  9 area 3  typ 0+0x80 set0 (64,159) [13] bit 23 area 17 typ 0 set0 ( 71, 60)
```

- Prim-Init `FUN_8006db44` @0x8006db44: SPRT Code **0x64** (nicht transparent), 8×8,
  RGB 0x80, **v=0xf8**, `u = (typ&3)*8` bzw. `+0x20` wenn iconSet==0
  (@0x8006db60-0x8006db9c), CLUT `GetClut(0x100,0x1fc)` (`li a1,0x1fc` @0x8006dbfc).
  Icon-Pixel liegen in der Map-Seite selbst (v≥0xf0).
- Zeichner `FUN_8006dcc0` @0x8006dcc0 — Marker erscheint nur wenn ALLE gelten:
  1. Karte der Area im Besitz: `FUN_80077360(&DAT_800d4924, DAT_800d5c0a)` — ⚠️ hier
     wird die AREA-NUMMER als Bit genutzt, nicht `mapItemBit` (weicht für die
     vertauschten Areas 7/8 von §3.1 ab — gemessene Retail-Eigenheit);
  2. Marker-Area == angezeigte Area;
  3. done-Bit NICHT gesetzt: `FUN_80077360(&DAT_800d4a34, e[0]) == 0` — das Skript
     setzt Bank-32-Bits beim Benutzen des Übergangs, DANN VERSCHWINDET DER MARKER;
  4. Szenario-Filter: bits 0x40/0x80 von e[+2] gegen `DAT_800cfbd8 & 0x40000000/0x80000000`.
  RGB blinkt **0x50 ↔ 0x80** mit Phase 0x800d5c19 (@0x8006dcc0-Kopf).
  Global abschaltbar über `DAT_800cfbd8 & 0x2000000` (Aufruf-Gate am Ende von
  FUN_8006e120 @0x8006e768-Umfeld).

### 5.3 Floor-Wechsel

- Nachbar-Area-Tabellen: aufwärts `DAT_800a9aec[area]`, abwärts `DAT_800a9b04[area]`
  (Area-Nummern; <2 = kein Nachbar). Werte:
  `9aec = [0,0,3,4,0,2,0,0,7,0,0,0,0,0,13,14,15,16,0,0]`,
  `9b04 = [0,0,5,2,3,0,0,8,0,0,0,0,0,14,15,16,17,0,0,0]`.
  Tab-Pfeil nur sichtbar, wenn Nachbar-Area besucht (Bank 36).
- Erworbene/gesehene Karten-Areas werden zusätzlich in die Liste **0x800d4b68**
  eingetragen (24 Slots, −1 = leer; Append `FUN_800692dc` @0x800692dc, Rückgabe
  Slot ⇒ `slot&7` = Floor-Index 0x800d5c02, `slot>>3` = Gruppe 0x800d5c01 —
  @0x80071d08-0x80071d1c); `FUN_8006d550` zykelt den Floor-Index durch die
  Nicht-−1-Slots der Gruppe.

---

## 6. Portierungs-Zusammenfassung (was der Port 1:1 nachbauen muss)

1. Pro Area eine 256×256-4bpp-Seite (Pixel liegen vor: `MAPS.PIX` Block N),
   Raum-Rechtecke + Marker-/Icon-UVs aus den Tabellen §2.2/§2.3/§5.2 (alle Werte im
   EXE, dumpbar).
2. Zeichnen: 1 semi-transparentes Textur-Rechteck pro Raum; Zustand = Palettenwahl
   nach §3.1-Matrix; unbesucht ohne Karte = weglassen.
3. Besucht-Bits: kumulativer Raumindex (StageBasen §4.1), setzen am Raum-Lade-Ende;
   Area-Bits separat; Karten-Besitz/Events als Skript-Flags (Bänke 32/33/34).
4. Spieler-Pfeil: 8 Richtungsframes, `pos/450 + 28000/450`-Projektion, RGB-Pulsieren.
5. Treppen-Marker: 14 Einträge, blinken 0x50↔0x80, verschwinden über Bank-32-Bit.
6. Farb-Paletten der Zustandszeilen: Basis-Palette liegt vor; die 7 Zustands-Zeilen
   müssen noch von der echten Disc (Datei 170) oder aus einem VRAM-Dump gezogen
   werden (§3.3-Lücke) — bis dahin sind konkrete RGBs Hypothese.

## 7. Offene Punkte

1. **RGB555-Inhalte der CLUT-Zeilen 0x1f5..0x1fe** — braucht RE2-Disc-Image oder
   Emulator-VRAM-Dump (Suchpfad in §3.3 präzise beschrieben).
2. **Save-memcpy** der Flag-Region — im MEM_CARD.BIN-Overlay zu suchen (§4.3).
3. Feinheiten der CHECK-Screen-Cursorliste (DAT_800a9ba0 + DAT_800d69f2) — für das
   Nutzer-Ziel (In-Game-Map) nicht erforderlich.
