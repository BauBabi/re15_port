# 08 Wie RE2 die Tuerobjekte zeichnet - und wie der Port es nachbildet

Stand 2026-09-28. Nur gelesen und gerechnet: nichts am Port geaendert, nichts gebaut, kein git ausser Lesen.
Binaerdatei: `info/re2leon/PSX.EXE` (RE2 Retail, Leon). Jede Adresse unten habe ich selbst disassembliert,
auch die Sprungziele (`RotMatrix 0x8008e1f4`, Blickmatrix `0x80076cb0`, `InitGeom`-Teil `0x8008d29c`,
`ClearOTagR 0x800908a8`, `DrawOTag 0x800909a0`, `Door_model_set 0x80014ba4`, Mesh binden `0x80014b40`).

Werkzeuge (alle neu, unter `build/tor_1170/re_zeichnen/`):

| Skript | Zweck |
|---|---|
| `gte_dis.py` | MIPS-Disassembler MIT GTE-Dekodierung (mtc2/mfc2/ctc2/cfc2/lwc2/swc2 + Kommandos mit sf/lm/mx/v/cv). `re2_disasm.py` gibt diese Worte nur als `.word ...(op12)` aus |
| `gte_scan.py` | alle Schreiber eines GTE-Steuerregisters (`ctc2 <nr>`), lui+Offset-Zugriffe auf eine Adresse |
| `gte_scanrange.py` | lui+Offset-Zugriffe auf einen Adressbereich |
| `probe_farbe.py` | Eckfarbe RE2 (GTE-genau: RotMatrix, MulMatrix0, NCCT) gegen das Port-Rezept (Abschrift von `light_common.c`) |
| `probe_tabelle.py` | Sinustabelle RE2 gegen Port-Tabelle, RE2-RotMatrix gegen `pc_prop_rot_q12` |

GTE-Formeln: `psx-spx .../docs/geometrytransformationenginegte.md:514-530` (NCCT), GPU-Befehlsbits
`.../graphicsprocessingunitgpu.md:140-150`, Modulation `.../graphicsprocessingunitgpu.md:1438-1450`.

---

## Ergebnis in Kuerze

1. **Matrizen.** Je Objekt: `R = RotMatrix(rot)` = Rx*Ry*Rz mit m[0][2] = +sin y (`0x8008e1f4`, selbst gelesen);
   Welt/Blick-Matrix `W = P * R` mit P = Eltern-W (Flag 0x10) oder Kamera `0x800dcba8`; Lage `T = P*pos + P.t`
   (16 Bit, IR-gesaettigt). Die GTE bekommt fuers Zeichnen RT = W, TR = T.
2. **Licht.** Lichtmatrix der GTE = `L * W_VORBILD` mit L @0x8009a470, Farbmatrix @0x8009a490 (neunmal 1600),
   Hintergrundfarbe BK = 68<<4 (Flag 0x1000: 136<<4), Grundfarbe RGBC = 0x808080. Das Licht haengt am
   **Blickraum** (W enthaelt die Kamera). **Skeptiker-Befund bestaetigt:** `obj+84` wird gelesen, bevor es neu
   geschrieben wird - das Licht rechnet mit der Matrix des vorigen Bildes, im ersten Bild eines Objekts mit
   einer Nullmatrix (Door_init nullt die Objekte).
3. **Dreieck.** RTPT -> NCLIP (gezeichnet bei MAC0 >= 0) -> NCCT (sf=1, lm=1) mit den drei **Eckennormalen**
   -> Farben und Bildpunkte ins POLY_GT3 (Code 0x34, Flag 0x4000: 0x36) -> AVSZ3 (ZSF3 = 341) -> verworfen bei
   otz < 64 -> Ordnungstabelle nach Flag 0xc0.
4. **Probe.** Normale (4096,0,0), DOOR2E-Fluegel (BK 68): Drehung 0 -> RE2 **(73,73,73)**, Port-Rezept (73,73,73);
   Drehung y = 570 -> RE2 **(72,72,72)**, Port-Rezept (72,72,72). **Gleich.** Vollvergleich ueber alle 946
   DOOR2E-Normalen x 573 Drehungen x 2 BK = 1 084 116 Ecken: 0 Abweichungen.
5. **Port.** `re15_light_shade_vertex` ist ein bitgleicher NCCT-Nachbau, wenn man den Kontext **von Hand** fuellt
   (L = Tuer-L, C = 1600, ambient = 68) und mit `re15_light_ctx_rotate_for_bone(.., W_vorbild, ..)` dreht. Der
   Raum-Pfad der Prop-Schleife (`re15_light_setup_actor` mit Raumlicht, Drehung ohne Kamera) ist fuer die Tuer
   falsch. Zusaetzlich fehlen im Port: NCLIP, die otz-Regel, die OT-Reihenfolge, eine RE2-genaue RotMatrix
   fuer negative Winkel, Farben ueber 0x80.

---

## 1. `FUN_80014234` - Objekte: Matrizen und Licht

Schleife ueber 10 Objekte: `s0 = [0x800d4dd8]` (@0x80014284/88), `s1 = s0+20` (@0x80014290),
Schritt 332 (@0x80014660 `addiu s1,s1,332`, @0x8001466c `addiu s0,s0,332`), Ende @0x80014664 `slti v0,s2,10`.
Objekt aus, wenn `[obj+0] == 0` (@0x80014294 `lw v0,0(s0)`, @0x8001429c `beq v0,zero,0x8001465c`) - dann wird
auch `obj+84` NICHT fortgeschrieben.

### 1.1 Ablauf je Objekt (Reihenfolge = Reihenfolge im Code)

| # | Schritt | Beleg (selbst gelesen) |
|---|---|---|
| a | `RotMatrix(obj+116 -> obj+36)` | @0x800142a0 `addiu a0,s0,116`, @0x800142a4 `jal 0x8008e1f4`, @0x800142a8 `addiu a1,s0,36` (Delay-Slot) |
| b | BK = 136<<4, wenn `flags & 0x1000`, sonst 68<<4 | @0x800142ac `lhu v0,304(s1)` (= obj+324), @0x800142b4 `andi v0,v0,0x1000`; 136: @0x800142bc..c4 `addiu a3/t0/t1,zero,136`, @0x800142c8..d0 `sll t4..t6,..,4`, @0x800142d4..dc `ctc2 t4,RBK / t5,GBK / t6,BBK`; 68: @0x800142e8..f0 `addiu ..,zero,68`, @0x80014300..08 dieselben `ctc2` |
| c | Farbmatrix LCM <- `0x8009a490` | @0x8001430c/10 `lui a3,0x800a` / `addiu a3,a3,-23408`, @0x8001431c..38 `ctc2 t4,LR1LR2 ... ctc2 t6,LB3` |
| d | **RT <- Lichtmatrix `0x8009a470`** (voruebergehend) | @0x8001433c/40 `addiu t0,t0,-23440`, @0x8001434c..68 `ctc2 t4,RT11RT12 ... ctc2 t6,RT33` |
| e | **`sp+16 = L * obj+84`** (MulMatrix0, je Spalte `MVMVA sf=1 mx=RT v=IR cv=none`, Wort `4a49e012`) | @0x8001436c `addiu a1,s0,84`; Spalte 0 @0x80014370..78 `lhu t4,0(a1)/6(a1)/12(a1)`, @0x8001437c..84 `mtc2 ..,IR1..IR3`, @0x80014390 MVMVA, @0x800143a4..ac `sh ..,0/6/12(sp+16)`; Spalte 1 ab @0x800143b0 (`s0+86`), Spalte 2 ab @0x800143f4 (`s0+88`), letzte Ablage @0x80014434 |
| f | **Lichtmatrix LLM <- sp+16** | @0x80014438..5c `ctc2 t4,L11L12 ... ctc2 t6,L33` |
| g | Flag 0x100: Farbwert pulsiert | @0x80014468 `andi v0,v0,0x100`; Zaehler obj+328 += 8, & 0x1ff (@0x80014474..84); Dreieck 0..255 (@0x80014488..a4); `obj+124 = v | v<<8 | v<<16` (@0x800144b0..c0 `sw v1,104(s1)`) |
| h | RT <- Elternmatrix P (`[obj+128]`) | @0x800144c4 `lw a0,108(s1)`, @0x800144cc..f0 `ctc2 ..,RT11RT12 ... RT33` |
| i | **`obj+84 = P * obj+36`** (MulMatrix0, dasselbe MVMVA) | @0x800144f4 `addiu v1,s0,36`, @0x80014518 MVMVA, @0x80014528..30 `sh t4..t6,0/6/12(a1)` (a1 = obj+84 seit @0x8001436c); Spalten 1/2 @0x80014558/@0x8001459c, letzte Ablage @0x800145b8 |
| j | TR <- P.t | @0x800145bc..d0 `lw t4..t6,20/24/28(a0)`, `ctc2 ..,TRX/TRY/TRZ` |
| k | **`obj+104 = P * pos + P.t`** (MVMVA sf=1 mx=RT v=V0 **cv=TR**, Wort `4a480012`) | pos = untere 16 Bit von obj+56/+60/+64: @0x800145d8 `lhu t5,4(v0)`, @0x800145dc `lhu t4,0(v0)`, @0x800145e8 `mtc2 t4,VXY0`, @0x800145ec `lwc2 VZ0,8(v0)`, @0x800145f8 MVMVA; Ablage aus IR (lm=0, saettigt auf +-32767): @0x80014600..08 `swc2 IR1/IR2/IR3,0/4/8(a1+20)` |
| l | **GTE fuers Zeichnen: RT <- obj+84, TR <- obj+104** | @0x8001460c..30 `ctc2 ..,RT11RT12 ... RT33`, @0x80014634..48 `ctc2 ..,TRX/TRY/TRZ` |
| m | Dreiecke zeichnen | @0x8001464c `addiu a0,s0,16`, @0x80014650 `lw a2,0(s1)` (Mesh-Kopf obj+20), @0x80014654 `jal 0x8001468c`, @0x80014658 `addu a1,s0,zero` |

Eltern-Zeiger `obj+128` setzt `Door_model_set`: @0x80014c64 `andi v0,v1,0x10`, @0x80014c6c `andi v0,v1,0xf`,
@0x80014c78 `lw v0,0(v0)` aus `0x800d4dd8[n]`, @0x80014c80 `addiu v0,v0,84` (Eltern+0x54); ohne Flag 0x10
@0x80014c84/88 `0x800dcba8`; Ablage @0x80014c8c `sw v0,128(a0)`. Grundfarbe setzt Mesh binden:
@0x80014b4c `lui v0,0x80`, @0x80014b58 `ori v0,v0,0x8080`, @0x80014b5c `sw v0,124(a0)` -> **`obj+124 = 0x00808080`**.

Folgen:
- **W enthaelt die Kamera** (Wurzel: P = Kamera). Also `LLM = L * C * R`: die Lichtrichtungen stehen im
  Blickraum fest. Bei fester Tuerkamera heisst das: fest in der Tuerwelt, als Zeilen von `L*C`.
- **Kinder** rechnen die Geometrie mit dem W des Elternobjekts aus DEMSELBEN Bild, wenn der Elternteil eine
  kleinere Objektnummer hat (er wurde in dieser Schleife schon fortgeschrieben). Mit groesserer Nummer: Bild davor.
  DOOR2E: Eltern 0, Kinder 1 und 2 - also dasselbe Bild.

### 1.2 Kamera der Tuerszene (Eltern der Wurzel)

Door_init @0x80013c28/2c `a1 = 0x8001082c`, kopiert nach sp+16.. (@0x80013c30..6c); Bytes `@0x80010830` =
`10 27 00 00` + 20 Nullbytes. @0x80013e38 `addiu a0,sp,20` = Auge (10000,0,0), @0x80013e40 `addiu a1,sp,32` = Ziel (0,0,0),
@0x80013e3c `jal 0x80076cb0`. In `0x80076cb0`: @0x80076cb8 `addu s5,a0,zero` (Auge), @0x80076cc0/c4 Ziel der Matrix
`fp = 0x800dcba8`, Startwert = Einheitsmatrix `0x8009db44` (h16: 4096 0 0 0 4096 0 0 0 4096) @0x80076cf0..d2c;
dx = Ziel - Auge @0x80076d3c; Neigung aus dy (= 0) @0x80076d88..e54; Gier @0x80076e5c..f1c:
`m00 = m22 = (dz<<12)/dxz`, `m02 = -(dx<<12)/dxz`, `m20 = (dx<<12)/dxz` (@0x80076f08/0c/14/1c), `jal 0x8008d934`;
Translation @0x80076f20..4c (`a2 = fp+20`, Vektor = -Auge). Mit dx = -10000:

`C = [[0,0,4096],[0,4096,0],[-4096,0,0]]`, `C.t = (0,0,10000)` (wie 03/K05). H = 290
(@0x80013e30 `jal 0x8008de24`, @0x80013e34 `addiu a0,zero,290`; `0x8008de24` = `ctc2 a0,H`).

### 1.3 Skeptiker-Befund "Licht mit der Matrix des vorigen Bildes" - BESTAETIGT

- Gelesen wird obj+84 in Schritt e: @0x8001436c `addiu a1,s0,84`, @0x80014370/74/78, @0x800143b4..bc (`s0+86`),
  @0x800143f8..0x80014400 (`s0+88`).
- Geschrieben wird obj+84 erst in Schritt i: @0x80014528..30, @0x8001456c..74, @0x800145b0..b8.
- `Door_model_set` (@0x80014ba4..0x80014ccc, selbst gelesen) schreibt obj+0/+8/+16/+56/+60/+64/+116..120/
  +128/+270/+324/+326, Mesh binden (@0x80014b40..9c) +20/+28/+124, dessen Paket-Vorbereitung `0x8002cbc4`
  ueber a0 = obj+16 nur +24 (@0x8002cbdc `sw s0,8(a0)`). Keiner schreibt +84. Die Skriptbefehle
  (Member_set usw.) habe ich NICHT auf +84 geprueft.
- **Erstes Bild eines Objekts:** Door_init nullt die Objekte (`0x80013cd4..0x80013d10`: Zeiger ab Objekt 0,
  `@0x80013cf8 sh zero,0(v1)`, `@0x80013d00 addiu v1,v1,2`, Ende bei Objekt 9 + 326). Der Scheduler setzt
  das Modell im selben Bild vor dem Zeichnen (03 Abschnitt 3.3), also ist `LLM = L * 0 = 0` und die Farbe
  kommt nur aus BK: `probe_farbe.py` -> **(34,34,34)** statt (73,73,73). Dieses Bild liegt unter der
  subtraktiven Blende 56 (03 Abschnitt 3.4).
- Waehrend der Drehung laeuft das Licht ein Bild nach. DOOR2E bei y = 570 (letzter Schritt +2, 04 Abschnitt 4.2):
  Licht aus 568 -> ebenfalls (72,72,72) fuer die Probe-Normale.
- Ein ausgeschaltetes Objekt (+0 == 0) behaelt seine alte Matrix; beim Wiedereinschalten rechnet das Licht
  im ersten Bild mit dem Stand vom letzten gezeichneten Bild.

### 1.4 RotMatrix `0x8008e1f4` (selbst gelesen, alle Elemente)

Tabelle: Wort @0x800adeac + (|a| & 0xfff)*4, untere 16 Bit = sin, obere = cos (Bytes @0x800adeac: `00 00 00 10 06 00 00 10`).
Negativer Winkel: `sin = -tab_sin[(-a)&0xfff]`, `cos = tab_cos[(-a)&0xfff]` (@0x8008e1fc..34 fuer x,
@0x8008e260..98 fuer y, @0x8008e2e0..324 fuer z). Elemente (alle Produkte `multu`, `sra 12`):

| Element | Formel | Ablage |
|---|---|---|
| m00 | (cz*cy)>>12 | @0x8008e368 `sh t6,0(a1)` |
| m01 | (-(sz*cy))>>12 | @0x8008e388 `sh t7,2(a1)` |
| m02 | sy | @0x8008e2c8 `sh t6,4(a1)` |
| m10 | ((sz*cx)>>12) - ((t8*sx)>>12), t8 = (cz*-sy)>>12 | @0x8008e3d0 `sh t7,6(a1)` |
| m11 | ((cz*cx)>>12) + ((t8'*sx)>>12), t8' = (sz*-sy)>>12 | @0x8008e448 `sh t7,8(a1)` |
| m12 | (-(cy*sx))>>12 | @0x8008e2dc `sh t6,10(a1)` |
| m20 | ((sz*sx)>>12) + ((t8*cx)>>12) | @0x8008e400 `sh t6,12(a1)` |
| m21 | ((cz*sx)>>12) - ((t8'*cx)>>12) | @0x8008e474 `sh t6,14(a1)` |
| m22 | (cy*cx)>>12 | @0x8008e2f0 bzw. @0x8008e330 `sh t6,16(a1)` |

Das ist Rx*Ry*Rz. Abschrift: `probe_farbe.py:rotmatrix`.

---

## 2. `FUN_8001468c` - je Dreieck

Aufruf: a0 = obj+16 (-> a3), a1 = obj (-> s7), a2 = Mesh-Kopf. Mesh-Kopf: @0x80014720 `lw s5,0(a2)` Vertices,
@0x80014724 `lw t1,8(a2)` Normalen, @0x80014710 `lw fp,16(a2)` Dreiecksliste, @0x80014718 `lw t0,20(a2)` Anzahl.
Dreieckssatz 12 B = n0,v0,n1,v1,n2,v2 (i16): Vertices @0x8001475c `lh v0,2(fp)`, @0x80014760 `lh v1,6(fp)`,
@0x80014770 `lh v0,10(fp)`; Normalen @0x800148b8 `lh v0,0(fp)`, @0x800148a4 `lh v0,-6(s2)` (= fp+4),
@0x800148a8 `lh v1,-2(s2)` (= fp+8), s2 = fp+10 (@0x80014788). Eintrag je 8 B (`sll 3`).

| Schritt | Beleg |
|---|---|
| Pakete vorbereitet beim Binden (`0x8002cbc4`, a2 = 0 @0x80014b88): Laenge 9 (@0x8002cc7c), Code 0x34 + Farbe 0x808080 (@0x8002cc64..84), rgb1/rgb2 = 0x808080 (@0x8002cc8c, @0x8002cc9c), uv0/clut und uv2 aus dem MD1 (@0x8002cc90, @0x8002ccb0), **uv1/tpage-Wort ODER `([obj+16] & 0xc) << 19`** (@0x8002cc44..58, @0x8002cca0 `or v0,v0,t7`) = tpage-Bits 5-6 = Halbdurchsichtigkeits-Art (psx-spx GPU:381); zweite Kopie +40 (@0x8002ccb4..ec). obj+16 = Satzfeld +8 (@0x80014c0c `lh v0,8(a1)`, @0x80014c14 `sw v0,16(a0)`) | selbst gelesen |
| Paketplatz: `s0 = [obj+24] + Puffer*40`, je Dreieck +80 (zwei POLY_GT3 fuer Doppelpuffer) | @0x8001471c `lw a0,8(a3)`, @0x80014714 `lbu v0,0(a1)` (a1 = 0x800ce5e0), @0x80014728..38 `*5, *8`, @0x80014910 / @0x80014af8 `addiu s0,s0,80` |
| **GPU-Code in RGBC.CODE**: `0x34 | ((flags>>13)&2)` -> 0x34, mit Flag 0x4000 0x36 | @0x80014734 `lhu v0,324(s7)`, @0x8001473c `srl v0,v0,13`, @0x80014740 `andi v0,v0,0x2`, @0x80014744 `ori v0,v0,0x34`, @0x80014748 `sb v0,111(a3)` (= obj+127) |
| **RGBC <- obj+124** (= 0x808080 + Code) | @0x8001474c `addiu v0,a3,108`, @0x80014750 `lwc2 RGBC,0(v0)` |
| Flag 0x2000: Vertex-z *= `[0x800d47f4]` >> 8, **in den Meshdaten selbst**, je Dreieck fuer alle drei Ecken | @0x80014784 `addiu t3,a1,25108`, @0x80014794 `andi v0,v0,0x2000`, @0x800147a0..f0 (`mult`, `srl 8`, `sh ..,4(s3/s1/s6)`) |
| **RTPT** (`cop2 0x0280030`, sf=1) mit den drei Vertices | @0x800147f4..808 `lwc2 VXY0..VZ2`, @0x80014814 |
| Flag 0x20: drei Vertices nach Arbeitsbereich +560/+568/+576 kopieren (fuer die Unterteilung) | @0x80014820 `andi v0,v0,0x20`, @0x80014834..8a0 (`lwl/lwr`, `swl/swr`) |
| Schleifenzaehler | @0x80014828 `addiu t0,t0,-1` (Delay-Slot, laeuft immer) |
| **NCLIP** (`cop2 0x1400006`), MAC0 nach sp+48; **gezeichnet bei MAC0 >= 0** | @0x800148d0, @0x800148ec `swc2 MAC0,0(v0)`, @0x800148f8 `bgez v0,0x80014938`; sonst @0x80014900..34 Zaehler +1, s0 += 80, naechstes Dreieck |
| **NCCT** (`cop2 0x118043f`, sf=1 lm=1) mit den drei **Normalen** n0/n1/n2 | @0x80014938..4c `lwc2 VXY0,0(v1) ... VZ2,4(s1)` (v1/s3/s1 = Normalenzeiger @0x800148b0..c4), @0x80014958 |
| Ergebnis ins POLY_GT3: xy0/1/2 -> +8/+20/+32, rgb0/1/2 -> +4/+16/+28 | @0x80014974..7c `swc2 SXY0/1/2,8/20/32(s0)`, @0x80014980..88 `swc2 RGB0/1/2,4/16/28(s0)` |
| **AVSZ3** (`cop2 0x158002d`), OTZ nach sp+52 | @0x80014994, @0x8001499c `swc2 OTZ,0(v0)` |
| **Verwerfen bei otz < 64** | @0x800149a8 `srl v0,a0,6`, @0x800149ac `beq v0,zero,0x80014ae8` |
| Ordnungstabelle (Abschnitt 2.2) | @0x800149bc..0x80014a24 |
| Flag 0x20: `jal 0x8008ebf4` (Unterteilung), neuer Paketzeiger nach Arbeitsbereich+8 | @0x80014a30 `andi v0,v0,0x20`, @0x80014a90, @0x80014aa4 `sw v0,8(v1)` |
| sonst addPrim: `tag = (*ot & 0xffffff) | 0x09000000`, `*ot = s0 & 0xffffff` | @0x80014ac0..e4 (`lui t4,0x900`, `and v0,s0,t2`, t2 = 0x00ffffff @0x80014754/58) |
| Dreieckszaehler Arbeitsbereich+556 (+1 fuer JEDES Dreieck, auch verworfene) | @0x80014af4..b00, Nullsetzen je Bild @0x80014280 `sh zero,556(v0)` |

ZSF3 hat in der ganzen EXE genau einen Schreiber: @0x8008d29c `addiu t0,zero,341`, @0x8008d2a0 `ctc2 t0,ZSF3`
(`gte_scan.py re2 ctc2 29`). Also `otz = 341*(SZ1+SZ2+SZ3) >> 12` (Mittelwert); otz < 64 heisst mittlere Tiefe < ~64.

Der GPU-Befehl 0x34 = `001 1 0 1 0 0`: Polygon, Gouraud, 3 Ecken, texturiert, **deckend, Modulation**
(psx-spx GPU:144-149). 0x36 = dasselbe halbdurchsichtig. Das CODE-Byte kommt ueber die NCCT-Farb-FIFO
(`Color FIFO = [MAC1/16,MAC2/16,MAC3/16,CODE]`, psx-spx GTE:529) direkt in das Befehlsbyte von rgb0.

### 2.1 Unterteilung (Flag 0x20) - nur die Argumente gelesen

@0x80014a3c..94: a0/a1/a2 = Arbeitsbereich+560/+568/+576 (die drei Vertexkopien), a3 = s0+12 (uv0), Stapel
16 = s0+24 (uv1), 20 = s0+36 (uv2), **24/28/32 = alle drei s0+4** (rgb0, @0x80014a48/58/5c/60), 36 = Paketzeiger,
40 = OT-Platz, 44 = Arbeitsbereich+12. Wenn `0x8008ebf4` die PsyQ-`DivideGT3` ist (Name aus 04, von mir nicht
geprueft), bekommen unterteilte Dreiecke an allen Ecken die Farbe der Ecke 0. DOOR2E nutzt 0x20 nicht.

### 2.2 Sortierung

| flags & 0xc0 | Tabelle | Platz | Beleg |
|---|---|---|---|
| 0x80 | `[0x800ce22c]` | (otz>>7) + 511 | @0x800149c4 `beq v1,v0,0x80014a04` (v0 = 128), @0x80014a04..14 `lw s4,-7636(s4)`, `sll 2`, `addiu v0,v0,2044` |
| 0xc0 | `[0x800ce22c]` | otz>>7 | @0x800149e8..fc `lw s4,-7636(s4)`, `sll v0,v0,2` |
| 0x40 | `[0x800ce2b0]` | otz>>12 | @0x800149d8 `beq v1,v0,0x80014a18` (v0 = 64), @0x80014a18..20 `lw s4,-7504(s4)` |
| 0x00 | **s4 unveraendert** (Platz des vorigen Dreiecks bzw. Wert beim Eintritt) | @0x800149e0 `j 0x80014a28`; s4 wird in `0x8001468c` vor der Schleife nicht gesetzt |

Tabellen und Reihenfolge im Bildwechsel `0x8002b968` (derselbe Funktionsrumpf wie `VSync` @0x8002b998):
- `[0x800ce22c]` = Puffer + 68 + Puffer-Nr.<<12 (@0x8002af68..74), geleert mit `ClearOTagR(ot, 1024)`
  (@0x8002af98..a0; `0x800908a8` gibt den Text `"ClearOTagR(%08x,%d)..."` @0x800125fc aus);
  `[0x800ce2b0]` mit 16 Eintraegen (@0x8002afa4..ac).
- Gezeichnet: zuerst `DrawOTag([0x800ce2b0]+60)` (@0x8002bd3c..44), dann `DrawOTag([0x800ce22c]+4092)`
  (@0x8002bd48..50), dann `DrawOTag([s2+64]+28)` (@0x8002bd54..5c). `0x800909a0` = Text `"DrawOTag(%08x)..."` @0x80012614.

Folgen (ClearOTagR + DrawOTag vom letzten Eintrag): hoher Platz = zuerst gezeichnet = hinten. Innerhalb
eines Platzes kommt das **zuletzt eingehaengte** Dreieck zuerst (addPrim haengt vorn ein), also liegt das
FRUEHER eingehaengte oben. 0x40-Objekte liegen unter allem, 0xc0-Objekte (Platz 0..511) ueber den
0x80-Objekten (Platz 511..1022). Ein Platz fasst 128 Tiefeneinheiten (otz>>7).

Flag 0x400 schaltet die Tabelle zu einem Bild um: wenn `Bildzaehler (Arbeitsbereich+558) == obj+270`
(@0x800146d4..ec), bei 0xc0 Bit 0x80 loeschen (@0x80014700 `andi v0,a0,0xff7f`), sonst setzen
(@0x800146fc `ori v0,a0,0x80`), Ablage @0x80014704.

DOOR2E (Flags aus 04 Abschnitt 4.2): Fluegel 0x0a80 (Tabelle 0x80, BK 68, keine Pulsierung),
Riegel Obj 1 0x00d0 (0xc0, Eltern 0), Riegel Obj 2 0x0050 (0x40, Eltern 0).

---

## 3. Lichtdaten (Bytes aus `info/re2leon/PSX.EXE`)

```
8009a470: 90 01 20 03 0c fe f8 f8 18 fc 74 f5 ac 0d 2c 1a b0 04 00 00   (L, 3x3 i16 + Fuellung)
8009a490: 40 06 40 06 40 06 40 06 40 06 40 06 40 06 40 06 40 06 00 00   (LCM, 3x3 i16)
```

- L (Zeile = Licht): (400,800,-500), (-1800,-1000,-2700), (3500,6700,1200).
- LCM: alle neun 1600. Jedes Licht wirkt weiss mit 1600/4096 auf jeden Kanal.
- BK: 68<<4 = 1088 (0x44), mit Flag 0x1000 136<<4 = 2176 (0x88). Andere BK-Schreiber in der EXE: nur
  `SetBackColor 0x8008dde4` (`sll 4`, `ctc2 RBK/GBK/BBK`), nicht im Tuercode (`gte_scan.py re2 ctc2 13`).
- Lichtmatrix im Tuerraum bei Drehung 0 (`L*C`): (500,800,400), (2700,-1000,-1800), (-1200,6700,3500).

---

## 4. Port

### 4.1 Was vorhanden ist

- `re15_light_shade_vertex` (`engine/src/light_common.c:244-305`): Stufe 1 `IR_dot[i] = lm1((L[i]*N)>>12)`,
  Stufe 2 `IR_c = lm1(((ambient<<16) + sum C[i][ch]*IR_dot[i]) >> 12)`, Stufe 3 `clamp((((128*IR_c)<<4)>>12)>>4)`.
  Das ist die psx-spx-NCCT-Formel mit `BK = ambient<<4`, `LCM[ch][i] = C[i][ch]`, `RGBC = (128,128,128)`
  (`RE15_FACE_RGB_CODE 128`, `include/re15_light.h:78`). Das CODE ist fest.
- `re15_light_ctx_rotate_for_bone` (`light_common.c:218-233`): `out.L[i] = bone_rot^T * L[i]` mit einem `>>12`
  = Zeile i von `L * bone_rot` = RE2-Schritt e (MVMVA sf=1), ohne IR-Saettigung (bei Tuerwerten |.| < 12000 egal).
- `re15_camera_compose_view_bone` (`engine/src/camera_common.c:125-167`): `out_rot = (view.rot*bone_rot)>>12`,
  `out_trans = ((view.rot*trans)>>12) + view.trans` = RE2-Schritte i und k (ohne 16-Bit-Kappung von pos und T).
- Prop-Schleife `platform/pc/main.c:9508-9764`: Licht aus dem **Raum-Cut** (`re15_light_setup_actor` mit
  Raumlichtern, `:9599-9603`), gedreht mit der **Weltdrehung ohne Kamera** (`prot_q12`), Farbe je Ecke aus
  den Eckennormalen (`:9653-9662`), RTPT-Nachbau mit `re15_gte_divide` (`:9632-9645`), **Nahschnitt je Ecke
  bei vz < 64** (`:9637`), Sortierschluessel = mittleres vz (`:9647-9648`), **kein NCLIP**, Vierecke werden
  zu zwei Dreiecken.
- `re15_render_textured_tri_lit` (`platform/pc/src/render_pc.c:2365-2443`): Eckfarbe ueber
  `psx_prim_to_sdl_vert` (`:2356-2359`) = `min(255, (v*255+64)/128)`; Sortierung in `end_frame`
  (`render_pc.c:836-849`): Einfuegesortierung **absteigend nach z, stabil** (gleiche z in Abgabereihenfolge).
- Drehmatrix der Props `pc_prop_rot_q12` (`main.c:565-589`): Rx*Ry*Rz, aber andere Rundung und die
  Tabelle wird fuer negative Winkel mit `a & 0xfff` gelesen.

### 4.2 Rezept: ein Tuerobjekt wie RE2 zeichnen

Je Bild, Objekte 0..9 in Nummernfolge, nur `on != 0`:

1. `R = RotMatrix_RE2(rx,ry,rz)` - **neue** Funktion, Abschrift von `0x8008e1f4` (Tabelle 1.4, Tabelle
   `re15_trig_lut` ist wortgleich, siehe 4.4). `pc_prop_rot_q12` NICHT verwenden.
2. Licht (VOR dem Fortschreiben von W): Welt-Kontext `w` von Hand fuellen:
   `w.L[i][k] = L_tuer[i][k]` (@0x8009a470), `w.C[i][ch] = 1600` (@0x8009a490, `LCM[ch][i]`, NICHT `<<4`),
   `w.ambient[ch] = (flags & 0x1000) ? 136 : 68`, `w.active_lights = 3`; dann
   `re15_light_ctx_rotate_for_bone(&w, W_vorbild[obj], &ctx)`. `W_vorbild` = W des Objekts aus dem letzten
   gezeichneten Bild, nach dem Tuer-Start 0-Matrix. Flag 0x100 geht so nicht (siehe 4.5).
3. Matrix: `P` = W/T des Elternobjekts (Flag 0x10, Nummer = flags & 0xf) oder Tuerkamera
   `{rot = [[0,0,4096],[0,4096,0],[-4096,0,0]], trans = (0,0,10000)}`;
   `re15_camera_compose_view_bone(&P, R, (int16)pos, W, T)`; T auf +-32767 kappen; danach `W_vorbild[obj] = W`.
4. Je Dreieck in Listenreihenfolge (Vertices v0,v1,v2, Normalen n0,n1,n2):
   - Ecke: `vx = ((W*v)>>12) + T`, IR1/IR2 auf +-0x7fff, `sz = clamp(vz, 0, 0xffff)`,
     `n = re15_gte_divide(290, sz)` (H = 290 @0x80013e34), `sx = 160 + ((IR1*n)>>16)`, `sy = 120 + ((IR2*n)>>16)`
     (Bildmitte geerbt, 03 Abschnitt 3.2). **Kein** Nahschnitt je Ecke.
   - NCLIP: `mac0 = x0*y1 + x1*y2 + x2*y0 - x0*y2 - x1*y0 - x2*y1`; bei `mac0 < 0` weiter.
   - Farbe: `re15_light_shade_vertex(&ctx, n0/n1/n2 ...)`.
   - `otz = (341*(sz0+sz1+sz2)) >> 12`; bei `otz < 64` weiter.
   - Platz: 0x80 -> `(otz>>7)+511`, 0xc0 -> `otz>>7`, 0x40 -> `1024 + (otz>>12)` (eigene Tabelle, vor allen);
     Schluessel fuer den Port `z = Platz*4096 + lfd` mit `lfd` = laufende Nummer der Abgabe im Bild
     (spaeter abgegeben = groesseres z = frueher gezeichnet, wie addPrim). Hoechstwert 1039*4096+4095 < 2^24,
     also als `float` exakt.
   - `re15_render_textured_tri_lit(sx,sy,u,v ..., clut, z, Farben)`; Flag 0x4000 halbdurchsichtig, Art =
     tpage-Bits 5-6 = MD1-Seitenwort ODER `(Satzfeld+8 & 0xc) >> 2` (Abschnitt 2, `0x8002cbc4`).
5. Hintergrund schwarz, kein Raumbild, keine Raum-Masken (Door_init @0x80013e44..4c `jal 0x8002bda8` (2,0), 03).

### 4.3 Probe (Aufgabe 4)

`python build/tor_1170/re_zeichnen/probe_farbe.py`:

| Fall | W = C*R | LLM (= L*W) | Normale (4096,0,0) RE2 | Port-Rezept |
|---|---|---|---|---|
| Drehung 0 | [[0,0,4096],[0,4096,0],[-4096,0,0]] | [[500,800,400],[2700,-1000,-1800],[-1200,6700,3500]] | (73,73,73) | (73,73,73) |
| Drehung y = 570 (sin 3142, cos 2623) | [[-3142,0,2623],[0,4096,0],[-2623,0,-3142]] | [[13,800,639],[3109,-1000,918],[-3454,6700,1320]] | (72,72,72) | (72,72,72) |
| erstes Bild (obj+84 = 0) | - | 0 | (34,34,34) | Rezept mit Nullmatrix: gleich |
| Flag 0x1000, Drehung 0 | wie oben | wie oben | (107,107,107) | (107,107,107) |

Handrechnung Drehung 0: IR = lm1(500, 2700, -1200) = (500, 2700, 0); Stufe 2 `(1088*4096 + 1600*3200) >> 12`
= 1088 + 1250 = 2338; Stufe 3 `((128*2338)<<4 >> 12) / 16` = 1169/16 = **73**. Drehung 570: IR = (13, 3109, 0),
`(4456448 + 1600*3122) >> 12` = 2307, `1153/16` = **72**.

Vollvergleich (gleiches Skript): 946 Normalen (DOOR2E, beide Meshes) x Drehungen 0..570, 2048, 2618 x BK 68/136
= 1 084 116 Ecken, **0 Abweichungen**. Das Rezept ist fuer den Farbteil bitgleich.

### 4.4 Tabelle und Drehmatrix

`python build/tor_1170/re_zeichnen/probe_tabelle.py`:
- RE2-Tabelle @0x800adeac und RE1.5-Tabelle @0x800794c4 (= `re15_trig_lut`): 4096/4096 Worte gleich.
- Negativer Winkel: RE2 liest `-tab[(-a)&0xfff]`, der Port `tab[a&0xfff]`; die Tabelle ist nicht genau
  ungerade (`tab[4095]` = `0x10000000`, sin 0), daher bei **4092 von 4095** negativen Winkeln verschieden.
- Nur y-Drehung: RE2-RotMatrix gegen `pc_prop_rot_q12` in -4095..4095 verschieden bei 4092 Winkeln (alle negativ);
  0..4095 gleich. Beliebige (x,y,z) nur positiv: 19969/20000 verschieden, max. 2 Einheiten (Rundungsfolge);
  mit negativen Winkeln bis 15 Einheiten. DOOR2E-Riegel dreht x negativ (0 .. -365, 04 Abschnitt 4.2).

### 4.5 Luecken des Ports gegen RE2 (fuer die Tuer)

1. Kein NCLIP in der Prop-Schleife (RE2 @0x800148f8 zeichnet nur MAC0 >= 0).
2. Nahschnitt je Ecke `vz < 64` (`main.c:9637`) statt `otz < 64` (@0x800149a8); bei der Kamerafahrt (DOOR2E
   Skript 7: x + 7140, also Tiefe bis ~360) nicht gleichwertig.
3. Sortierung: mittleres vz statt OT-Platz; Gleichstand in Abgabereihenfolge statt umgekehrt; keine
   getrennte 0x40-Tabelle.
4. `pc_prop_rot_q12` statt RotMatrix (4.4).
5. Licht aus dem Raum-Cut, gedreht ohne Kamera, ohne Bildverzug (1.3).
6. CODE fest 128 (`light_common.c:298`): Flag 0x100 (Grundfarbe v,v,v) laesst sich nicht abbilden -
   braucht einen Parameter. DOOR2E nutzt 0x100 nicht.
7. Farben > 128 werden gekappt (4.6).
8. Flag 0x2000 (z-Stauchung in den Meshdaten) und 0x20 (Unterteilung) fehlen; DOOR2E nutzt beide nicht.

### 4.6 Aufgabe 5: Textur x Eckfarbe

- PSX (Befehl 0x34, Bit 24 = 0 -> Modulation, psx-spx GPU:149): `Kanal = Texel * Eckfarbe / 128`
  (psx-spx GPU:1444), 0x80 neutral, darueber heller (bis knapp 2x).
- Port: `psx_prim_to_sdl_vert(v) = min(255, (v*255+64)/128)` (`render_pc.c:2356-2359`), SDL moduliert mit
  `/255`. Fuer v <= 128 dasselbe bis auf Rundung (v = 73: SDL 145/255 = 0,5686 gegen 73/128 = 0,5703);
  fuer v > 128 bleibt es bei Texel x 1,0.
- Messung (`probe_farbe.py`-Funktionen, BK 68): DOOR2E-Fluegel ueber die Drehung 0..570 hat 15 492 von
  540 166 Eckwerten > 128 (Variante 1, 2048..2618: 35 401), hoechster Wert **137**. Ueber die ganze
  Einheitskugel: BK 68 max. 137, BK 136 max. 171. Der Port zeichnet diese Ecken bis 7 % (137) bzw. 25 % (171)
  zu dunkel.
- Nicht nachgebildet und nicht gelesen: 5-Bit-Texel (15-Bit-VRAM) und Dithering der GPU (Offen 3).

---

## 5. Offen

1. `flags & 0xc0 == 0`: s4 unbelegt (@0x800149e0). Welcher Platz dann benutzt wird, haengt vom Registerstand
   beim Eintritt ab; nicht verfolgt.
2. `0x8008ebf4` als `DivideGT3` nicht geprueft; ob die Farbe ecke-0-flach wird, haengt daran (2.1).
3. Dithering-Bit der Zeichenumgebung waehrend der Tuer und die 5-Bit-Rundung der GPU-Modulation: nicht gelesen.
4. Halbdurchsichtigkeit: die Art ist gelesen (Abschnitt 2), wie der Port sie je Dreieck setzt
   (`s_tri_blend` in `render_pc.c`), habe ich nicht geprueft.
5. Wer `[0x800d47f4]` (Faktor fuer Flag 0x2000) schreibt: kein lui+Offset-Schreiber gefunden (`gte_scan.py re2
   luiaddr 800d47f4` leer).
6. Bildmitte (160,120) ist geerbt (03), in der Tuer selbst nicht gesetzt; dynamisch nicht gemessen.
7. Dynamische Gegenprobe (Savestate waehrend einer RE2-Tuersequenz) gibt es nicht; alles hier ist statisch
   gelesen und nachgerechnet.

## 6. Messbefehle

```
python build/tor_1170/re_zeichnen/gte_dis.py re2 dis 0x80014234 140      # Objekte (in Teilen gelesen)
python build/tor_1170/re_zeichnen/gte_dis.py re2 dis 0x8001468c 300      # Dreiecke
python build/tor_1170/re_zeichnen/gte_dis.py re2 dis 0x8008e1f4 160      # RotMatrix
python build/tor_1170/re_zeichnen/gte_dis.py re2 bytes 0x8009a470 64     # L, LCM
python build/tor_1170/re_zeichnen/gte_scan.py re2 ctc2 29                # ZSF3-Schreiber
python build/tor_1170/re_zeichnen/gte_scan.py re2 ctc2 13                # RBK-Schreiber
python build/tor_1170/re_zeichnen/probe_farbe.py                         # Probe + Vollvergleich
python build/tor_1170/re_zeichnen/probe_tabelle.py                       # Tabelle, RotMatrix
```
