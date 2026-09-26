# ROOM11F0 Boiler-Room-Rätsel — Cursor, Schatten, 90-Grad-Drehung (RE1.5-Seite + Port)

**Urteil:** Das Rätsel läuft im Port mechanisch byte-true; der „verzerrte/versetzte Schatten"
ist der in die Cursor-Textur GEBACKENE Schlagschatten (+5 u / +3 v, ROOM11F0.RDT @0x018DAC),
und er verzerrt, weil der PC-Zeichner jedes Quad an der falschen Diagonale teilt
(`main.c:9513-9527` v0–v3 statt der PSX-Diagonale v1–v2); die 90°-Drehung ist an beiden
Bildern vermessen (RE1.5 = RE2 um 90° gegen den Uhrzeigersinn), und die „80" hat in RE1.5
**kein** Gegenstück — ROOM11F0 besitzt keinerlei Zeiger-Objekt.

## 1. Was der Nutzer meldet / was ich gemessen habe

Nutzer-Zitat: *"Das Rätsel vom Boiler Room 11F0 ist das Äquivalent dazu [RE2 ROOM2130/213].
… ich möchte in ROOM 11F0 so einen roten Cursor mit der Bewegungslogik — wie in Resident
Evil 2 im Room 2130/213 für das Schalter Rätsel. Der muss an der gleichen Stelle sein wie
bei Resident Evil 2, nur dass bei uns die Anzeige eben um 90 Grad gedreht ist. Nach der
Eingabe der korrekten Position muss der Cursor auf 80 stehen. Ausserdem … hat der Cursor
immer so einen verzerrten/versetzten Schatten."*

**Was ich selbst gemacht habe** (alles nachvollziehbar, kein Bau):

1. `ROOM11F0.RDT` und `ROOM11F1.RDT` byte-weise verglichen → **identisch** (0 Unterschiede
   bei 152 588 Byte).
2. Das komplette SCD des Raums opcode-exakt gelaufen
   (`python re15_port/tools/scd_dump_room.py re15_port/shared_assets/PSX/STAGE1/ROOM11F0.RDT`,
   main00 + 20 Subs, 694 Zeilen) und jede zitierte Stelle an den Rohbytes nachgelesen (§2).
3. Die Prop-Zeigertabelle @0x0030 → 0x0240 aufgelöst, das Cursor-MD1 @0x001928 und das
   Cursor-TIM @0x018DAC selbst geparst, die Texel gezählt und die UV-Achsen per
   Kleinste-Quadrate-Fit auf die Vertex-Koordinaten bestimmt (§2.6, §3.3).
4. Die vorhandene `re15_port/build/platform/pc/re15_pc.exe` (Stand 2026-09-26 12:08) **nur
   ausgeführt**, nicht gebaut: Sprung nach ROOM11F0, Kamera auf Cut 8/9/10/11/12/13/14
   gepinnt, je ein `RE15_FRAMEDUMP`- und ein `RE15_FBDUMP`-Bild (§3.2, §3.6).
5. Die gerenderten Cursor-Pixel gegen die aus Textur + gemessenem Maßstab vorhergesagte
   Lage gestellt (§3.3).
6. Das RE2-Gegenstück `info/re2leon/COMMON/BSS/ROOM213/ROOM21306.bmp` selbst geöffnet und
   vermessen (§2.7) — unabhängig vom Parallel-Agenten.

**Reproduktion der Laufzeit-Bilder** (aus einem beliebigen Arbeitsverzeichnis, relative
Zielnamen; die exe ist GUI-Subsystem und hat KEIN stderr):

```
RE15_NO_INTRO=1 RE15_TITLE_SHOT=1 RE15_DEBUG_JUMP="11F0@30" RE15_FORCE_CUT=10 \
RE15_SET_FLAG_AT="5:0,5:1,5:2,5:3,5:4,5:5,5:6,5:7,5:8,5:9,5:10,5:11,5:12@240" \
RE15_FRAMEDUMP="300:q1.ppm"  <pfad>/re15_pc.exe      # voller Frame (3D + 2D)

RE15_NO_INTRO=1 RE15_TITLE_SHOT=1 RE15_DEBUG_JUMP="11F0@30" RE15_FORCE_CUT=10 \
RE15_FBDUMP="300:fb10.ppm"   <pfad>/re15_pc.exe      # nur die 2D-Ebene
```
Der Prozess beendet sich nicht von selbst — mit `timeout 40 …` abbrechen, die PPMs sind
dann schon geschrieben. `RE15_SUBSTART="16@200#11F0"` startet zwar sub16, bleibt aber an
dessen zwei `Message_on` @0x15A2/@0x15A8 stehen (gemessen: Kamera blieb auf Cut 0) —
deshalb der Umweg über `RE15_FORCE_CUT`.

## 2. Das Original — mit Adressen und Bytes

Alle Datei-Offsets beziehen sich auf `re15_port/shared_assets/PSX/STAGE1/ROOM11F0.RDT`
(152 588 Bytes). **`ROOM11F1.RDT` ist byte-identisch** — selbst nachgerechnet:
`len 152588 == 152588, diffs 0`. Es gibt also keine Zwillings-Variante mit anderem
Verhalten; 11F1 ist nur der zweite Spieler-Slot.

RDT-Kopf: `nSprite=0 nCut=15 nOmodel=12 nItem=0 nDoor=0 nRoom_at=0`,
`main_scd=0x0CC4`, `sub_scd=0x0D34`, Prop-Zeigertabelle `@0x0030 -> 0x0240`.

### 2.1 Das Rätsel-Brett: 11 AOT-Zellen + 10 Schalter + 1 Cursor (sub00)

| Datei-Offset | Bytes | Bedeutung |
|---|---|---|
| 0x00D78 | `2c 02 05 44 00 00 5c 95 32 64 02 08 02 08 00 00 00 00 00 00` | Aot_set slot 2, **sce=5**, rect x=-27300 z=25650 w=2050 h=2050 |
| 0x00D8C | `2c 03 05 44 … 9a 5b …` | slot 3, x=-27300 z=23450 |
| 0x00DA0 | `2c 04 05 44 … 02 53 …` | slot 4, x=-27300 z=21250 |
| 0x00DB4 | `2c 05 05 44 … 38 4a …` | slot 5, x=-27300 z=19000 |
| 0x00DC8 | `2c 06 05 44 … a0 41 …` | slot 6, x=-27300 z=16800 |
| 0x00DDC | `2c 07 05 44 … 0c b3 32 64 …` | slot 7, x=-19700 z=25650 |
| 0x00DF0 | `2c 08 05 44 …` | slot 8, x=-19700 z=23450 |
| 0x00E04 | `2c 09 05 44 …` | slot 9, x=-19700 z=21250 |
| 0x00E18 | `2c 0a 05 44 …` | slot 10, x=-19700 z=19000 |
| 0x00E2C | `2c 0b 05 44 …` | slot 11, x=-19700 z=16800 |
| 0x00E40 | `2c 0c 05 44 00 00 3c c4 86 3d 02 08 02 08 00 00 00 00 00 00` | slot 12, x=-15300 z=15750 — **die Ausstiegszelle** |

Das Rätselbrett liegt also **flach in der XZ-Ebene**: 2 Spalten (x=-27300 / -19700)
× 5 Zeilen (z=25650 / 23450 / 21250 / 19000 / 16800), Zellenraster 2050 × 2050,
Spaltenabstand 7600, Zeilenabstand 2200. Es liegt räumlich abseits des begehbaren
Raums (Spielerbereich um x≈-2200…+11000) — der klassische Capcom-Trick: eine eigene
„Bühne" in Weltkoordinaten, die nur eine eigene Kamera (Cut 0x0A) sieht.

Die 10 Schalter-Props (Obj_model_set, alle Typ 0, Band 2, `rot=(0,0,512)`):

| Datei-Offset | obj | Position |
|---|---|---|
| 0x00E76 | 0x02 | (-25975, 10, 26000) |
| 0x00E98 | 0x03 | (-25975, 10, 24000) |
| 0x00EBA | 0x04 | (-25975, 10, 22000) |
| 0x00EDC | 0x05 | (-25975, 10, 19800) |
| 0x00EFE | 0x06 | (-25975, 10, 17800) |
| 0x00F20 | 0x07 | (-18775, 10, 26000) |
| 0x00F42 | 0x08 | (-18775, 10, 24000) |
| 0x00F64 | 0x09 | (-18775, 10, 22000) |
| 0x00F86 | 0x0A | (-18775, 10, 19800) |
| 0x00FA8 | 0x0B | (-18775, 10, 17800) |

Der Cursor:

| Datei-Offset | Bytes | Bedeutung |
|---|---|---|
| 0x00E54 | `2d 00 04 00 00 00 00 01 00 00 9e b3 00 00 9c 58 00 00 00 00 00 00 00 00 7c fc 00 00 84 03 84 03 84 03` | Obj_model_set **obj=0x00, type=4**, pos=(-19554, 0, 22684), rot=(0,0,0), Box c=(0,-900,0) h=(900,900,900) |

`type==4` ⇒ Renderabsenkung `y-900` (@0x8002c23c/@0x8002c24c, im Port
`re15_prop_render_y`, `re15_port/include/re15_aot.h:471-474`).
Startposition (-19554, 22684) liegt in Zelle **slot 9** (x∈[-19700,-17650],
z∈[21250,23300]).

### 2.2 Die Bewegungslogik — sub01 (Treiber) + sub02..05 (D-Pad)

sub01 läuft jedes Bild, solange Bank 5 Bit 0 gesetzt ist (Rätsel aktiv):

| Datei-Offset | Bytes | Bedeutung |
|---|---|---|
| 0x01098 | `51 01 01 00` | Sce_key_ck Maske 0x01 → |
| 0x0109C | `04 ff 18 02` | Evt_exec(0x18, sub02) |
| 0x010B0 | `51 01 04 00` | Maske 0x04 → sub03 |
| 0x010C8 | `51 01 02 00` | Maske 0x02 → sub04 |
| 0x010E0 | `51 01 08 00` | Maske 0x08 → sub05 |

Die vier Bewegungs-Subs (je 12 Byte):

| Datei-Offset | Bytes | Bedeutung |
|---|---|---|
| 0x012F2 | `2e 03 00 00` | sub02: Work_set(kind=3, idx=0) = der Cursor-Prop |
| 0x012F6 | `2f 02 c8 00` | **Speed_set(Achse 2, +200)** |
| 0x012FA | `30` | Add_speed |
| 0x01302 | `2f 02 38 ff` | sub03: **Speed_set(Achse 2, −200)** |
| 0x0130E | `2f 00 c8 00` | sub04: **Speed_set(Achse 0, +200)** |
| 0x0131A | `2f 00 38 ff` | sub05: **Speed_set(Achse 0, −200)** |

Also: ±200 Einheiten pro Bild auf der Z-Achse (Achse 2) bzw. X-Achse (Achse 0),
solange die Taste gehalten wird. **Kein Rastersprung, kein Snapping** — der Cursor
gleitet frei über das Brett; die „Zelle" ergibt sich nur daraus, in welchem AOT-Rechteck
er gerade steht.

### 2.3 Der Zell-Stempel und die Bestätigung

sub01 fragt pro Zelle das Paar (Bank-5-Bit, Prop-Member 0x0F) ab, Beispiel Zelle 2:

| Datei-Offset | Bytes | Bedeutung |
|---|---|---|
| 0x010F0 | `21 05 01 01` | Ck(Bank 5, Bit 1) == 1 — Zelle 2 freigeschaltet |
| 0x010F4 | `2e 03 00` | Work_set(3,0) — Cursor-Prop |
| 0x010FC | `3e 00 0f 00 02 00` | **Member_cmp(member 0x0F, == 2)** — Cursor steht über AOT-Slot 2 |
| 0x01106 | `51 01 40 00` | Sce_key_ck Maske **0x40** (Aktionstaste) |
| 0x0110A | `04 ff 18 06` | Evt_exec(0x18, sub06) |

Die vollständige Zuordnung (selbst aus den Bytes gelesen):

| Bank-5-Bit | Member_cmp @ | Wert | Sub | Schalter-Prop | Zustands-Bit |
|---|---|---|---|---|---|
| 1 | 0x010FC | 2 | sub06 @0x1322 | obj 0x02 | Bank 5 Bit 13 |
| 2 | 0x01124 | 3 | sub07 @0x1362 | obj 0x03 | Bit 14 |
| 3 | 0x0114C | 4 | sub08 @0x13A2 | obj 0x04 | Bit 15 |
| 4 | 0x01174 | 5 | sub09 @0x13E2 | obj 0x05 | Bit 16 |
| 5 | 0x0119C | 6 | sub10 @0x1422 | obj 0x06 | Bit 17 |
| 6 | 0x011C4 | 7 | sub11 @0x1462 | obj 0x07 | Bit 18 |
| 7 | 0x011EC | 8 | sub12 @0x14A2 | obj 0x08 | Bit 19 |
| 8 | 0x01214 | 9 | sub13 @0x14E2 | obj 0x09 | Bit 20 |
| 9 | 0x0123C | 10 | sub14 @0x1522 | obj 0x0A | Bit 21 |
| 10 | 0x01264 | 11 | sub15 @0x1562 | obj 0x0B | Bit 22 |
| 11 | 0x0128C | 12 | **sub17** @0x15FA | — (Ausstieg) | — |

(Die Kette ist an genau einer Stelle NICHT fortlaufend: Bit 11 / Zelle 12 springt zu
sub**17**, nicht sub16. sub16 ist der EINSTIEG, siehe unten.)

Ein Schalter-Umlegen, Beispiel sub06:

| Datei-Offset | Bytes | Bedeutung |
|---|---|---|
| 0x01322 | `22 05 01 00` | Set(5,1,0) — Zelle sperren (Re-Entry-Schutz) |
| 0x01326 | `2e 03 02` | Work_set(3, **2**) = Schalter-Prop obj 0x02 |
| 0x0132E | `21 05 0d 00` | Ck(5, 13) == 0 → Zweig „einschalten" |
| 0x01332 | `2f 05 40 00` | **Speed_set(Achse 5, +0x40 = +64)** |
| 0x01336 | `0d 00 04 00 10 00` | For 0x10 = **16 Durchläufe** |
| 0x0133C | `30` / `02` | Add_speed + Evt_next (1 Bild je Durchlauf) |
| 0x01340 | `22 05 0d 01` | Set(5,13,1) |
| 0x01348 | `2f 05 c0 ff` | Gegenzweig: Speed_set(Achse 5, **−64**) |
| 0x0135C | `22 05 01 01` | Set(5,1,1) — Zelle wieder freigeben |

16 × 64 = **1024 = 90°** (4096 = 360°). Jeder Schalter kippt also um genau 90° um
Achse 5, Startwinkel `rot_z = 512` (aus dem Obj_model_set-Record, s. o.).

### 2.4 Einstieg (sub16) und Ausstieg (sub17)

| Datei-Offset | Bytes | Bedeutung |
|---|---|---|
| 0x00D64 | `2c 01 03 31 00 00 68 f7 16 c2 20 03 78 05 ff 00 18 10 00 00` | Aot_set slot 1, **sce=3**, rect (-2200,-15850,800,1400), data `ff 00 **18 10**` = Evt_exec(Typ 0x18, **sub 0x10 = 16**) — das ist die Untersuchen-Zone am Panel |
| 0x015A2 | `2b 00 ff ff` / `2b 01 ff ff` | sub16: Message_on 0, Message_on 1 |
| 0x015B2 | `21 0c 1f 00` | Ck(Bank 12, Bit 31) == 0 |
| 0x015B6 | `46 01 00 00 00 00 00 00 00 00` | Aot_reset slot 1 (Untersuchen-Zone aus) |
| 0x015C0 | `29 0a` | **Cut_chg 0x0A** — Kamera auf die Rätselbühne |
| 0x015C2..0x015F2 | 13 × `22 05 xx 01` | Set(5, 0..12, 1) — Rätsel + alle 11 Zellen scharf |
| 0x012A8 | `21 05 0c 01` | sub01: Ck(5,12)==1 → |
| 0x012AC | `22 02 00 01` | Set(2,0,1) = RE15_PAUSE_PLAYER |
| 0x012B0 | `22 02 02 01` | Set(2,2,1) = RE15_PAUSE_AI |
| 0x015FA | `09 0a 01 00` | sub17: Sleep 1 |
| 0x01602 | `32 00 9e b3 00 00 9c 58` | **Pos_set Cursor = (-19554, 0, 22684)** — Reset auf die Startzelle |
| 0x0160E ff. | 10 × `33 00 00 00 00 00 00 02` | Dir_set aller Schalter zurück auf (0,0,**512**) |
| 0x01682 | `46 01 03 31 ff 00 18 10 00 00` | Aot_reset slot 1 → Untersuchen-Zone wieder an |
| 0x0168C..0x016E4 | 23 × `22 05 xx 00` | Bank 5 Bits 0..22 alle auf 0 |
| 0x016E8/0x016EC | `22 02 00 00` / `22 02 02 00` | Pause wieder aus |
| 0x016F0/0x016F2 | `2a` / `3c 01` | Cut_old + Cut_auto 1 |

### 2.5 DIE LÖSUNG — sub01 @0x012B6..0x012EA

| Datei-Offset | Bytes | Bedeutung |
|---|---|---|
| 0x012B6 | `06 00 36 00` | Ifel_ck, Länge 0x36 |
| 0x012BA | `21 04 ee 00` | Ck(4, 238) == 0 — global „noch nicht gelöst" |
| 0x012BE | `21 05 0d 01` | Bit 13 (Schalter 1) == **1** |
| 0x012C2 | `21 05 0e 00` | Bit 14 (Schalter 2) == **0** |
| 0x012C6 | `21 05 0f 01` | Bit 15 (Schalter 3) == **1** |
| 0x012CA | `21 05 10 00` | Bit 16 (Schalter 4) == **0** |
| 0x012CE | `21 05 11 01` | Bit 17 (Schalter 5) == **1** |
| 0x012D2 | `21 05 12 00` | Bit 18 (Schalter 6) == **0** |
| 0x012D6 | `21 05 13 01` | Bit 19 (Schalter 7) == **1** |
| 0x012DA | `21 05 14 00` | Bit 20 (Schalter 8) == **0** |
| 0x012DE | `21 05 15 01` | Bit 21 (Schalter 9) == **1** |
| 0x012E2 | `21 05 16 00` | Bit 22 (Schalter 10) == **0** |
| 0x012E6 | `04 ff 18 12` | Evt_exec(0x18, **sub18**) |
| 0x012EA | `22 04 ee 01` | Set(4, 238, 1) |

**Die Lösung ist also: obj 0x02/0x04/0x06/0x08/0x0A (Spalte x=-25975, Zeilen 1/3/5 bzw.
Spalte x=-18775, Zeilen 1/3 …) — exakt das Muster AN-AUS-AN-AUS-AN-AUS-AN-AUS-AN-AUS
über die Reihenfolge obj 0x02..0x0B.**

sub18 (Erfolg) @0x016F6: `22 04 f3 01` = Set(4, 243, 1) (das globale „Reservestrom an"),
alle Bank-5-Bits löschen, `29 08` Cut_chg 8, Message_on 2 @0x01742, `29 0d` Cut_chg 0x0D,
`56 00 02 07 00 00` Member_calc2 (das Flackern) @0x0174A, vier Sce_bgmtbl_set-Wechsel,
`29 0e` Cut_chg 0x0E, `3c 01` Cut_auto.

### 2.6 Das Cursor-MODELL (die Quelle des „Schattens")

Prop-Zeigertabelle @0x0240: `prop0 tim=0x018DAC md1=0x001928`
(props 2..11 teilen sich alle `tim=0x0231EC md1=0x003140`).

MD1 @0x001928: `len=65, unk=0, nobj=2 → 1 Mesh`.
Mesh-Kopf @0x001934 (14 × u32): tri `v@0x38 n=132, nrm@0x458 n=114, f@0x7E8 n=16,
uv@0xEC8`; quad `v@0x38 n=132, nrm@0x458 n=114, f@0x8A8 n=98, uv@0xF88`
(Offsets relativ zu 0x001928+12).

Selbst nachgerechnete Geometrie (132 Vertices):
* **49 Quads in der Ebene y = −900** — ein 7×7-Raster, 1800 × 1800 Einheiten.
  PSX-Y ist nach OBEN negativ ⇒ das ist die **Oberseite**.
* **49 Quads in der Ebene z = +900** — ebenfalls 7×7, 1800 × 1800 ⇒ die **Vorderseite**.
* 16 Dreiecke auf x=−900 (4), x=+900 (4), z=−900 (4), y=+923 (4) — Kantenkappen.

UV-Bereiche (selbst gelesen, alle CLUT 0x7800, Page 0x80):
* Ebene y=−900 → **v = 0…82**
* Ebene z=+900 → **v = 84…168**

TIM @0x018DAC: `magic=0x10, flags=0x09` (8bpp + CLUT), CLUT 256×1 @VRAM(0,480),
Pixel 64 Words × 256 = **128 × 256, 8bpp**.

Texel-Zensus, den ich selbst gefahren habe:
* v = 0…82 (Oberseite): **2645 von 10624 Texeln ≠ 0**
* v = 84…168 (Vorderseite): **0 von 10880 Texeln ≠ 0** — komplett transparent
* v = 169…255: **0 von 11136** — leer

**Also zeichnet nur die Oberseite etwas.** Und was sie zeichnet:
vier **hellgrüne Eck-Winkel** plus ein **gelbes Kreuz** in der Mitte — und **jedes dieser
Elemente hat im TEXEL-BILD selbst eine dunkle, nach rechts-unten versetzte Kopie**
(dunkelgrün hinter grün, dunkelbraun hinter gelb). Der „Schatten" des Cursors ist ein
**in die Textur gebackener Schlagschatten**, kein Renderpass.
(PNG-Beleg: `scratchpad/11f0_top.png`, erzeugt aus genau diesen Bytes.)

### 2.7 Die 90-Grad-Drehung — an Bildschirmkoordinaten belegt, nicht am Gefühl

Ich habe beide Anzeigen selbst vermessen, jeweils im 320 × 240-Bild.

**RE1.5, ROOM11F0, Cut 10** (gemessen an der 2D-Ebene `fb10.ppm`, s. §3.2):

| Element | Bildschirm |
|---|---|
| Hebel-Props, linke Spalte (Welt x = −25975, obj 0x02…0x06) | x 64…84 |
| Hebel-Props, rechte Spalte (Welt x = −18775, obj 0x07…0x0B) | x 155…169 |
| Zeile 1 (Welt z = 26000) | y 70…78 |
| Zeile 2 (z = 24000) | y 96…102 |
| Zeile 3 (z = 22000) | y 121…126 |
| Zeile 4 (z = 19800) | y 147…155 |
| Zeile 5 (z = 17800) | y 171…180 |
| Skala (senkrecht), Teilstrich-Spalte | x 275…281 |
| 11 Teilstriche (Helligkeits-Minima, selbst gemessen) | y = 54, 66, 78.5, 91, 103, 115.5, 127.5, 140, 152, 164.5, 177 |
| rote „80"-Ziffer | x 264…269, y 79…83 |
| rotes Band (80→100) | x = 281, y 56…75 |

⇒ **Eichung RE1.5: Wert v → Bildschirm-y = 177 − v × 1.23** (100 oben bei y = 54, 0 unten bei
y = 177). Das rote Band liegt exakt zwischen dem 80er- (78.5) und dem 100er-Teilstrich (54).

**RE2, ROOM213, Hintergrundbild 06** — `info/re2leon/COMMON/BSS/ROOM213/ROOM21306.bmp`,
320 × 240, bereits im Repo extrahiert, von mir selbst geöffnet und vermessen:

| Element | Bildschirm |
|---|---|
| Skala (waagerecht), Ziffern `0 10 20 … 70` schwarz, **`80` rot**, `90 100` schwarz | y ≈ 52…63 |
| rote „80"-Ziffer | x 230…247, y 52…63 |
| rotes Band (80→100) | y 76…81, x 239…293 |
| 5 obere Anzeige-Lampen | 5 Spalten |
| Beschriftung über dem Hebel | `36` (5×) |
| 5 Kipphebel | 5 Spalten |
| Beschriftung unter dem Hebel | `14` (5×) |

⇒ **Eichung RE2: Wert v → Bildschirm-x = 23 + v × 2.70** (0 links, 100 rechts).

**Die Drehung, belegt:** dasselbe Bedienfeld — gelb-schwarzes Warnband als Rahmen, vier
Schrauben in den Ecken, eine Skala 0…100 mit ROT gedruckter 80 und einem roten 80→100-Band,
dazu ein 5 × 2-Schalterfeld. In RE2 liegt die Skala **waagerecht oben** (Wert wächst nach
RECHTS), das Schalterfeld darunter als **5 Spalten × 2 Zeilen**. In RE1.5 liegt dieselbe
Skala **senkrecht rechts** (Wert wächst nach OBEN), das Schalterfeld als **2 Spalten ×
5 Zeilen**. Die RE1.5-Ziffern sind im Hintergrundbild um 90° gedreht gesetzt und lesen sich
von unten nach oben. Das ist **die RE2-Anzeige um 90° GEGEN den Uhrzeigersinn gedreht**
(RE2-links → RE1.5-unten).

Die Bewegungslogik ist dabei in RE1.5 NICHT mitgedreht, sondern korrekt auf die gedrehte
Anzeige gelegt — selbst nachgerechnet über die virtuelle Pad-Tabelle
(`re15_port/engine/src/pad_common.c:27-34`, vtbl[0]=UP, [1]=RIGHT, [2]=DOWN, [3]=LEFT):

| Sce_key_ck-Maske @Datei | Taste | Sub | Speed_set | Welt | Bildschirm |
|---|---|---|---|---|---|
| 0x01 @0x1098 | UP | sub02 @0x12F6 | Achse 2, +200 | +Z | nach oben |
| 0x04 @0x10B0 | DOWN | sub03 @0x1302 | Achse 2, −200 | −Z | nach unten |
| 0x02 @0x10C8 | RIGHT | sub04 @0x130E | Achse 0, +200 | +X | nach rechts |
| 0x08 @0x10E0 | LEFT | sub05 @0x131A | Achse 0, −200 | −X | nach links |

(Bildschirmrichtung aus dem UV-Fit in §3.3: u wächst mit +X, v wächst mit −Z, und v zeigt
im Bild nach unten.)

## 3. Was der Port tut — mit datei.c:zeile

### 3.1 Das Rätsel LÄUFT bereits — die Stellen

| Stelle | Was sie tut |
|---|---|
| `re15_port/engine/src/scd_vm.c:3103-3157` `op_speed_set` | Achse 0/1/2 → `t->vel[0..2]` (X/Y/Z), Achse 5 → `t->vel[5]` (rot_z). Deckt ROOM11F0 sub02..05 (Achse 0/2, ±200) und sub06..15 (Achse 5, ±64) |
| `re15_port/engine/src/scd_vm.c:3158-3200` `op_add_speed` | integriert `t->vel[]` in `g_scd.props[pi].x/y/z` und `rot_x/y/z` — der Cursor-Prop wandert damit tatsächlich |
| `re15_port/engine/src/scd_vm.c:4714-4776` `op_sce_key_ck` | Pad-Maske gegen `g_scd_pad_held` (virtuelles Wort), plus der Bestätigungs-Laut |
| `re15_port/engine/src/aot_common.c:414-455` `re15_object_notch_update` | stempelt pro Bild `props[p].member_0b` = der AOT-Slot, über dem der Prop steht (Clear @0x80043788, Stempel @0x80042f5c) |
| `re15_port/engine/src/scd_vm.c:3379-3406` `prop_member_get/set` | `case 15: → props[].member_0b` — das ist das `Member_cmp(0x0F, ==N)` aus sub01 |
| `re15_port/engine/src/aot_common.c:467-485` | der Bewegungs-Klick (CORE-SE 4) am Zellenwechsel |
| `re15_port/engine/src/game_state.c:154-186` `re15_game_flag_set` | Set(2,0)/Set(2,2) aus sub01 @0x12AC/@0x12B0 → `g_re15_pauseflags` (Spieler-/AI-Freeze) |
| `re15_port/platform/pc/main.c:9294-9528` | die Prop-Zeichenschleife: lädt alle `nOmodel` Props, projiziert, zeichnet Tris und Quads |
| `re15_port/include/re15_aot.h:471-474` `re15_prop_render_y` | `type==4 → y-900` (@0x8002c23c/@0x8002c24c) — gilt genau für den Cursor |

### 3.2 Laufzeit-Messung (eigener Lauf, ohne Bau)

Ausgeführt wurde die **vorhandene** `re15_port/build/platform/pc/re15_pc.exe` (Stand 2026-09-26
12:08). Zwei Läufe:

```
RE15_NO_INTRO=1 RE15_TITLE_SHOT=1 RE15_DEBUG_JUMP="11F0@30" RE15_FORCE_CUT=10 \
RE15_SET_FLAG_AT="5:0,…,5:12@240" RE15_FRAMEDUMP="300:q1.ppm"        -> q1.ppm  (voller Frame)
RE15_NO_INTRO=1 RE15_TITLE_SHOT=1 RE15_DEBUG_JUMP="11F0@30" RE15_FORCE_CUT=10 \
RE15_FBDUMP="300:fb10.ppm"                                            -> fb10.ppm (nur 2D-Ebene)
```
(Die exe ist GUI-Subsystem, stderr ist tot — Beleg dafür in Memory `reai-v2-re2-trace-datei`;
gemessen wird deshalb über die PPM-Dumps und `befund.log`.)

Ergebnis: Cut 10 zeigt das Rätselbrett vollständig — 2 × 5 Hebel-Props, der grüne Cursor,
der EXIT-Knopf und rechts die **senkrechte Skala 0…100 mit rot gedruckter „80"**.
Der Unterschied „voller Frame minus 2D-Ebene" grenzt die 3D-Anteile auf den Bildkasten
**x 64…172, y 70…180** ein — das sind die 10 Hebel plus der Cursor. Die Skala und der
rote Balken darin liegen in der **2D-Ebene** (also im BSS-Hintergrund), nicht im 3D-Teil.

Der Cursor steht (Startzelle) bei Bildschirm **x 148…172, y 107…130** (25 × 24 Pixel bei
320 × 240).

### 3.3 Der Cursor-„Schatten" — was ich gemessen habe

Der Schatten ist **in der Textur gebacken** (§2.6): Palette-Index 1 = (0,88,0) hinter
Index 3 = (8,248,0), und Index 2 = (64,56,0) hinter Index 4/5. Ich habe den Versatz
selbst über eine Verschiebungs-Suche bestimmt: **(+5 u, +3 v)** für Klammern *und*
Kreuz (Überlappung 824 von 991 bzw. 311 von 395 Texeln).

Die UV-Achsen der Oberseite habe ich aus den 49 Quad-Records per Kleinste-Quadrate-Fit
auf die Vertex-Koordinaten bestimmt:
```
u =  0.066863 * x + 0.000000 * z + 64.181      (u wächst mit +X)
v = -0.000000 * x - 0.045017 * z + 40.183      (v wächst mit −Z)
```
⇒ der gebackene Schatten entspricht Weltversatz **(+74.8 in X, −66.6 in Z)**, auf dem
Bildschirm also **nach rechts-unten** — und genau dorthin zeichnet ihn der Port. Die
RICHTUNG stimmt.

Der Maßstab: 121 Textur-Spalten liegen auf 24 Bildschirm-Pixeln (0.198 px/Texel),
78 Textur-Zeilen auf 23 Pixeln (0.295 px/Texel). Der 5×3-Texel-Schatten schrumpft damit
auf **rund 1 Pixel** — bei der Fenster-Skalierung des Nutzers wird daraus ein deutlich
sichtbarer Doppelrand.

**Der eigentliche Messbefund** (Pixelvergleich `q1.ppm` gegen die aus der Textur
vorhergesagte Lage; Klassifikation strikt gegen die 6 Palettenfarben):

| Element | vorhergesagt aus Textur + Maßstab | gerendert |
|---|---|---|
| Kreuz waagerecht (hell) | x 154.1…165.2, y 117.9…118.5 | x 154…165, y 118 ✅ |
| Kreuz waagerecht (Schatten) | y 118.8…119.7 | y 119 ✅ |
| Kreuz senkrecht (hell) | **x 159.5…160.3, y 112.9…123.5** (11 px hoch) | **nur y 117…119** (3 px) |
| Kreuz senkrecht (Schatten) | x 160.5…161.3 | **x 159…160** (1 px zu weit links) |

Das heisst: **der senkrechte Arm des Kreuzes wird auf 8 von 11 Zeilen in der
SCHATTENFARBE (64,56,0) statt in Gelb gezeichnet, und der Schatten sitzt einen Pixel
links von seiner Sollposition.** Auf dem Schirm sieht das Kreuz dadurch aus wie ein
gelber Querbalken mit einem dunklen, versetzten Senkrechtstrich — genau die Beschreibung
des Nutzers.

### 3.4 DIE URSACHE: der Port teilt jedes Quad an der FALSCHEN Diagonale

`re15_port/platform/pc/main.c:9514-9525` (Raum-Props) zerlegt ein MD1-Quad so:

```c
re15_render_textured_tri_lit(ax[0],ay[0],u0,v0,  ax[1],ay[1],u1,v1,  ax[3],ay[3],u3,v3, …);
re15_render_textured_tri_lit(ax[0],ay[0],u0,v0,  ax[3],ay[3],u3,v3,  ax[2],ay[2],u2,v2, …);
```
also die Diagonale **v0–v3**.

Die PSX-GPU macht es anders. Beleg, selbst nachgelesen —
`info/Resident_Evil_und_Playstation_Information/psx-spx.github.io-master/docs/graphicsprocessingunitgpu.md`,
Zeilen 209–212:

> „Quads are internally processed as two triangles, the first consisting of vertices 1,2,3,
> and the second of vertices 2,3,4. This is an important detail, as splitting the quad into
> triangles affects the way colours are interpolated."

Null-basiert: **(v0,v1,v2) und (v1,v2,v3)** — Diagonale **v1–v2**.

Dass RE1.5 hier wirklich EIN Vierpunkt-Primitiv an die GPU gibt (und nicht selbst zwei
Dreiecke baut), habe ich selbst disassembliert — der Quad-Zeichner `FUN_800256b0`:

```
  800256b0: addiu sp,sp,-24
  800256bc: sll   a3,a3,1
  800256c0: ori   a3,a3,0x3c      ; Primitiv-Code 0x3C = POLY_GT4
  800256ec: sb    a3,3(a2)        ; Code ins Primitiv schreiben
```
(Der Dreiecks-Zwilling `FUN_800254a0` setzt analog 0x34 = POLY_GT3.)
Die Zerlegung macht also die GPU, nach der oben zitierten Regel.

**Der PSX-Zweig des Ports macht es richtig:** `re15_port/platform/psx/src/mesh_psx.c:681-706`
(„CANONICAL GTE HARDWARE NCCT quad = ONE POLY_GT4 (matches FUN_800256b0)") emittiert ein
echtes `POLY_GT4`. **Nur der PC-Zweig teilt selbst — und an der falschen Diagonale.**

Betroffen sind im PC-Zweig ALLE fünf Quad-Stellen, nicht nur der Cursor (selbst
nachgezählt über `grep -n re15_render_textured_tri_lit re15_port/platform/pc/main.c`):
`main.c:1916/1918`, `7996/8002`, `8205/8211`, `9265/9271` (Gegner/NPC), `9514/9520`
(Raum-Props). Alle fünf Aufrufpaare verwenden die Reihenfolge
(v0,v1,v3) + (v0,v3,v2).

Für ein Quad, dessen Bildschirm-Viereck unter Perspektive KEIN Parallelogramm ist (und
das ist es hier nicht, die Kamera schaut schräg auf das Brett), liefern die beiden
Zerlegungen **unterschiedliche** Textur-Abbildungen im Inneren — sie stimmen nur auf der
gemeinsamen Diagonale überein. Das ist die „Verzerrung", und bei einem 1-Pixel-breiten
Merkmal wie dem senkrechten Kreuzarm entscheidet genau diese Differenz darüber, ob der
helle oder der dunkle Texel getroffen wird.

Die Textur-Filterung scheidet als Ursache aus: der Port setzt `SDL_HINT_RENDER_SCALE_QUALITY`
nirgends (`grep` über `platform/pc/src/render_pc.c` + `main.c` → nur `SDL_HINT_ORIENTATIONS`
:510 und `SDL_HINT_ANDROID_TRAP_BACK_BUTTON` :513), SDL2 bleibt damit auf `nearest` — wie die
PSX-GPU.

### 3.5 Der Endzustand nach richtiger Eingabe — und wo die „80" fehlt

Im Port läuft nach dem Lösungstest (§2.5) sub18 @0x16F6. Der Port führt davon aus:
`Set(4,243,1)`, Bank 5 löschen, Pause aus, `Cut_chg 8` @0x173A, `Message_on 2` @0x1742,
`Cut_chg 0x0D` @0x1748, das Flacker-`Member_calc2` @0x174A, vier `Sce_bgmtbl_set` mit
Sleeps, `Cut_chg 0x0E` @0x1770, `Aot_reset slot 1` @0x1776, `Cut_chg 8` @0x1780,
`Cut_auto 1` @0x1782, dann vier `Work_set(2,n)/Member_set(0x0C, 137/138)` @0x178C…0x17B0.

**Es gibt in ROOM11F0 KEIN Objekt, das einen Zeiger auf der Skala bewegt.** Beleg:
* RDT-Kopf: `nSprite = 0` (keine ESP-Sprites), `nOmodel = 12`.
* Die 12 Props sind vollständig zugeordnet: obj 0x00 = Cursor (§2.1), obj 0x01 = das Prop
  im begehbaren Raum @Datei 0x00D10 (11040, −16200, −15084), obj 0x02…0x0B = die 10 Hebel.
* Laufzeit-Gegenprobe: der Bildkasten aller 3D-Anteile bei Cut 10 ist **x 64…172**
  (§3.2), die Skala liegt bei **x 275…281** — dort zeichnet der Port kein einziges
  3D-Primitiv. Skala, Teilstriche, rote „80" und das rote 80→100-Band stecken vollständig
  im BSS-Hintergrund (`fb10.ppm` zeigt sie ohne jeden 3D-Anteil).

Die „80", die der Nutzer will, ist damit **in RE1.5 nicht vorhanden** — weder als Prop
noch als Sprite noch als Skript-Zustand. Sie muss aus dem RE2-Mechanismus kommen.

**Schnittstelle zum RE2-Befund** (der Parallel-Agent liefert das RE2-Innenleben; was ich
von meiner Seite fest zusagen kann):
1. Zielkoordinate im RE1.5-Bild: **y = 177 − v × 1.23**, Teilstrich-Spalte x ≈ 275…281,
   bei 320 × 240, Cut 10 (§2.7). Für v = 80 also **y ≈ 78.5**.
2. Zustandsquelle für v: die zehn Bank-5-Bits 13…22 (§2.3). Welche Zahl RE2 daraus bildet
   (die Beschriftungen `36`/`14` unter den RE2-Hebeln sind der offensichtliche Kandidat),
   muss der RE2-Befund liefern — ich rechne sie NICHT herbei.
3. Der Lösungszustand von RE1.5 ist eindeutig `1010101010` über obj 0x02…0x0B (§2.5); ein
   Zeiger-Modell muss genau für dieses Muster auf 80 landen, und sub18 wird an genau
   dieser Stelle (@0x12E6) ausgelöst.
4. Die 90°-Drehung ist in §2.7 an beiden Bildern vermessen: RE1.5 = RE2 um 90° gegen den
   Uhrzeigersinn. Ein aus RE2 übernommener Zeiger muss entsprechend auf die y-Achse gelegt
   werden, mit Wachstumsrichtung NACH OBEN.

### 3.6 Nebenbefund: Cut 13 und Cut 14 zeigen im Port Fremdbilder

sub18 schaltet nach dem Lösen auf `Cut_chg 0x0D` (@0x1748) und `Cut_chg 0x0E` (@0x1770).
Gemessen (`RE15_FORCE_CUT=13` bzw. `14`, `RE15_FBDUMP` bei Frame 400, 55 s Laufzeit —
derselbe Lauf lieferte für Cut 9 ein korrektes Bild): Cut 13 und Cut 14 zeigen **kein
ROOM11F0-Bild**, sondern eine Kachelung aus sechs fremden Korridor-Ansichten. Die beiden
Dumps unterscheiden sich voneinander (75 696 abweichende Pixel) und von Cut 8 — es ist
also kein hängengebliebener Puffer, sondern ein tatsächlich dekodierter, aber falscher
Bereich. `ROOM11F.BSS` = 983 040 Byte. Ob das ein Port-Defekt oder der Auslieferungsstand
ist, ist **nicht belegt** (§6).

## 4. Der Unterschied, in einem Satz

Das Rätsel selbst läuft im Port byte-true (Zellen, Cursor-Fahrt ±200/Bild, Schalter-Kippung
16 × 64 = 90°, Lösungsmuster 1010101010, Ausstieg über Zelle 12) — falsch sind nur die
**Quad-Diagonale des Zeichners** (`main.c:9513-9527`, PSX = v1–v2, Port = v0–v3), die den
in die Cursor-Textur gebackenen Schlagschatten (+5u/+3v, §2.6) verzerrt und den senkrechten
Kreuzarm in die Schattenfarbe kippt, und die vom Nutzer gewünschte **rote RE2-Cursor-Optik
samt Zeiger-Endstand „80"**, für die es in RE1.5 (noch) keinen belegten Mechanismus gibt.

## 5. Umsetzungsplan (konkrete Dateien/Zeilen, jede Konstante mit @0x)

### 5.1 Der Schatten — EINE Änderung, belegt

**Datei:** `re15_port/platform/pc/main.c`, Prop-Quads **Zeile 9514–9525**.
**Änderung:** Quad-Zerlegung von der Diagonale v0–v3 auf die PSX-Diagonale v1–v2 umstellen:

```c
/* PSX-GPU: Quad = Dreieck (v1,v2,v3) + Dreieck (v2,v3,v4), 1-basiert
 * = (v0,v1,v2) + (v1,v2,v3). Beleg:
 * info/.../psx-spx.github.io-master/docs/graphicsprocessingunitgpu.md:209-212 */
re15_render_textured_tri_lit(ax[0],ay[0],u0,v0, ax[1],ay[1],u1,v1, ax[2],ay[2],u2,v2, …);
re15_render_textured_tri_lit(ax[1],ay[1],u1,v1, ax[2],ay[2],u2,v2, ax[3],ay[3],u3,v3, …);
```

**Dieselbe Änderung an den vier weiteren Quad-Stellen des PC-Zweigs** (alle mit derselben
falschen Diagonale, selbst nachgezählt): `main.c:1916/1918`, `7996/8002`, `8205/8211`,
`9265/9271`. Der PSX-Zweig braucht nichts (`platform/psx/src/mesh_psx.c:681-706` schickt
schon ein echtes `POLY_GT4`).

**Verifikation** (ohne zu raten): denselben Messlauf wie §3.2 wiederholen und die Tabelle
aus §3.3 erneut aufstellen. Erwartung aus der Textur + gemessenem Maßstab
(0.198 px/Texel in u, 0.295 px/Texel in v): senkrechter Kreuzarm **hell** auf
**y 113…123** bei x = 160, sein Schatten bei **x = 161**. Heute: hell nur y 117…119,
Schatten bei x 159…160.

**Nicht ändern:** die Textur. Der Schlagschatten (+5 u, +3 v, Palette-Index 1 hinter 3 und
2 hinter 4/5, §2.6 / §3.3) ist Original-Kunst und bleibt.

### 5.2 Roter RE2-Cursor statt des grünen RE1.5-Cursors

Der heutige Cursor ist das Prop-Modell obj 0x00 (MD1 @Datei 0x001928, TIM @Datei 0x018DAC),
grün/gelb. Der Nutzer will die RE2-Optik. Zwei Wege, beide ohne Raten:

* **Weg A (kleinster Eingriff, empfohlen):** die CLUT des Cursor-TIMs beim Upload
  umsetzen. Nur vier Einträge sind betroffen (§2.6-Zensus):
  Index 3 = 0x03E1 (8,248,0) → RE2-Rot, Index 1 = 0x0160 (0,88,0) → dunkles Rot,
  Index 4 = 0x035B (216,208,0) und Index 5 = 0x47FE (240,248,136) bleiben oder werden
  mitgezogen. Stelle: `re15_port/platform/pc/main.c:1305-1307`
  (`re15_render_pc_upload_tim_slot(&tt, RE15_TIM_SLOT_PROP(op))`) bzw. der Slot-Upload in
  `platform/pc/src/render_pc.c:2072`. ⛔ Die konkreten RE2-Rotwerte muss der RE2-Befund
  liefern (aus RE2s eigenem Cursor-TIM); ich setze hier KEINE erfundene Farbe ein.
* **Weg B:** das RE2-Cursormodell/-Sprite übernehmen. Erfordert die RE2-Quelle und ist
  kein 1:1-Port mehr; nur wenn der Nutzer das ausdrücklich so will.

**Nicht anzufassen:** die Bewegungslogik. Sie ist in RE1.5 bereits genau die von RE2
geforderte („an der gleichen Stelle wie bei RE2, nur 90° gedreht"): ±200 pro Bild
@0x12F6/0x1302/0x130E/0x131A, Zellenraster 2050 × 2050 @Datei 0x00D78…0x00E40,
Bestätigung mit Maske 0x0040 @0x1106 ff., Ausstieg über Zelle 12 → sub17 @0x15FA.
Und sie ist bildschirmrichtig auf die gedrehte Anzeige gelegt (Tabelle in §2.7).

### 5.3 Der Zeiger auf „80"

Neues Element, in RE1.5 nicht vorhanden (§3.5). Umsetzung erst, wenn der RE2-Befund die
Wertbildung liefert. Dann:

* Zeichenstelle: eine 2D-Marke in der Text-/Overlay-Ebene des PC-Renderers
  (`re15_port/platform/pc/src/render_pc.c`, Schritt nach dem 3D-Pass — Reihenfolge siehe
  Skill `re15-pc-render-order`), gegated auf `g_re15_active_cut == 10 && g_current_room_id
  == 0x11F0`, damit sie in keinem anderen Bild auftaucht.
* Lage: **x ≈ 275…281, y = 177 − v × 1.23** (gemessen, §2.7), bei 320 × 240.
* Wert v: aus Bank 5 Bits 13…22 (§2.3) nach der RE2-Regel.
* Endzustand: beim Lösungsmuster `1010101010` (@0x12BE…0x12E2) muss v = 80 ⇒ y ≈ 78.5
  herauskommen — dieselbe Zeile, auf der die rote „80" steht (y 79…83 gemessen).

### 5.4 Reihenfolge

1. §5.1 (Quad-Diagonale) — eigenständig, belegt, sofort umsetzbar.
2. §5.2 Weg A, sobald die RE2-Cursorfarben vorliegen.
3. §5.3, sobald die RE2-Wertbildung vorliegt.
4. §3.6 (Cut 13/14) getrennt weiter-RE'en.

## 6. Offen / nicht belegt

1. **Ist die „Verzerrung" wirklich NUR die Quad-Diagonale?** Belegt ist: (a) die PSX teilt
   Quads an v1–v2 (psx-spx-Zitat), der Port an v0–v3 (`main.c:9513-9527`); (b) der
   senkrechte Kreuzarm landet heute auf 8 von 11 Zeilen in der Schattenfarbe und der
   Schatten sitzt 1 px zu weit links (§3.3). Dass (a) genau (b) erzeugt, ist **nicht
   gemessen** — dafür bräuchte es den Lauf nach der Änderung (in dieser Runde verboten:
   kein Bau) oder einen PSX-Gegenlauf. Bis dahin steht (a) als belegter byte-true-Defekt
   und (b) als belegter Messwert nebeneinander, nicht als bewiesene Kausalkette.
2. **PSX-Gegenmessung fehlt ganz.** Es gibt in dieser Runde keinen DuckStation-Lauf von
   ROOM11F0 Cut 10. Der nächste Weg: `re15-room-capture` (Debug-Menü → Stage 1, Raum 11F0),
   Savestate, Framebuffer-Dump, und dieselbe Pixeltabelle wie §3.3 gegenrechnen.
3. **Cut 13 / Cut 14 (§3.6).** Nicht geklärt, ob die Fremdbilder ein Port-Defekt sind oder
   ob `ROOM11F.BSS` nur 13 echte Cuts trägt. Nächster Weg: den BSS-Blockindex des Ports
   gegen den Cut-Index halten (`engine/src/` BSS-Leser) und `ROOM11F.BSS` roh zählen.
4. **Die RE2-Wertbildung („36" / „14" unter den RE2-Hebeln).** Ich habe die Beschriftungen
   im Bild gelesen, aber die Rechenregel NICHT disassembliert — das ist der Teil des
   Parallel-Agenten. 5 Hebel mit 36 bzw. 14 ergeben additiv 70/92/114/136/158/180, also
   **nicht** 80; die Regel ist damit sicher nicht „Summe der fünf Werte". Ich fülle diese
   Lücke nicht mit einer Zahl.
5. **Die RE2-Rotwerte für den Cursor.** Nicht gemessen (RE2-Cursor-TIM nicht geöffnet) —
   gehört zum RE2-Befund.
6. **`Sce_key_ck`-Maskenbelegung in RE1.5 selbst.** Die Zuordnung Maske→Taste habe ich über
   die Port-Tabelle `pad_common.c:27-34` samt deren Belegen (@0x80073dbc) genommen, nicht
   durch eigenes Disassemblieren von FUN_80030444. Die vier Masken 0x01/0x02/0x04/0x08 sind
   aber in sich konsistent (zwei Achsenpaare mit ±200), deshalb ist die Richtung robust.
7. **Warum in §3.3 einige Cursor-Texel mit ~24 % Helligkeit erscheinen** (z. B. (2,60,0)
   statt (8,248,0)): unentschieden, weil die beiden Grün-Paletteneinträge 0x03E1 und 0x0160
   im RGB nahezu kollinear sind und sich aus einem Pixel allein nicht trennen lassen. Das
   ist deshalb KEIN Befund, sondern eine Messgrenze. Nächster Weg: eine der beiden Farben
   im Upload temporär verstimmen und denselben Frame erneut dumpen — braucht einen Bau.
