# Runde 38 — Spur A "cursor": Dossier

Baum: `.claude/worktrees/r38_cursor` (Zweig `r38/cursor`, Basis 1bcbd0ff). Belege/Werkzeuge: `A_belege/`,
Messausgaben (nicht versioniert): `re15_port/build/r38_a/<marke>/`.

## Nutzer-Punkt (woertlich, AUFTRAG.md)

> Der Cursor, der zum Beispiel in ROOM 1150 oder ROOM 11F0 verwendet wird, hat immer noch so einen komischen
> Schatten. Den hat er im Original nicht...

## 0. Vorgeschichte ("immer noch")

| Stand | Was gemacht wurde | Warum es nicht hielt |
|---|---|---|
| 2026-08-21 `c43cf0be` | Cursor war SCHWARZ statt durchsichtig: GPU-Farbschluessel (Texel 0 = durchsichtig) | anderer Befund (Flaeche), nicht der Schatten |
| 2026-09-26 Nutzer: "hat der Cursor immer so einen verzerrten/versetzten Schatten" | `analysis/befunde_2026-09-26/raum11f0-raetsel-cursor.md` §2.6/§3.3: Urteil "in die Textur GEBACKENER Schlagschatten (+5u/+3v)" + Abtastphase (`2ee610a0` -0.5, dann `c2ba1631` +0.375 gegen ein echtes PSX-Bild geeicht) | **Fehldiagnose.** Die Untersuchung zaehlte nur die Texel der OBERSEITE und nahm an, nur sie zeichne etwas. Die 4 Dreiecke der UNTERSEITE (y=+923) tragen DASSELBE Muster (u 4..125, v 0..82, je ~1250 Texel != 0) und der PC-Zeichner verwirft keine Rueckseiten (s. §1/§2). Die Phase verschob nur, welche Bahn der Oberseite getroffen wird; die Unterseite blieb -> "immer noch". |
| Runde 34 Nacht Spur B | Hebetisch-Cursor ROOM1150 = 11F0-Cursor 1:1 (`hebetisch_cursor_pc.c`), "genau so wie die Raum-Prop-Schleife in main.c" | hat den Fehler der Prop-Schleife mitkopiert (keine Rueckseiten-Pruefung) |
| Runde 36 G2 | Ausschiebung des Cursors aus den Rahmenzellen | Bewegung, nicht Bild |

## 1. Messung vorher

### 1.1 Port (echte exe dieses Baums, Stand 1bcbd0ff, beschleunigter Renderer, RE15_FRAMEDUMP)

Lauf `A_belege/lauf_11f0.sh` (RE15_DEBUG_JUMP=11F0@30, RE15_FORCE_CUT=10 = Raetselkamera, Raetsel scharf wie
sub16 @0x015C2..0x015F2, Bild 300), Skala 1 und 4 (Nutzer-Fenster: Bildschirm 1920x1080 -> WINDOW_SCALE 3,
render_pc.c:624-630). Gegenbild Original: `analysis/grundwahrheit/psx_room11f0_cut10.png` (DuckStation, nativ;
die DuckStation des Nutzers steht auf `ResolutionScale = 1`, `TextureFilter = Nearest`, settings.ini gelesen).

Bild: `A_belege/vorher_11f0_psx_s1_s4.png` (links Original, Mitte Port Skala 1, rechts Port Skala 4; 12-fach).
Im Port sitzt INNERHALB jedes Eckwinkels eine kleinere, dunkle Kopie (z.B. Skala 1 Spalte x=150/151 Zeilen 110..128
= (0,21,0)/(2,60,0), Zeile 128 x 151..156 = (2,60,0); Original an denselben Stellen (0,0,0)/(32,32,32)).

### 1.2 Wer malt den Schatten? (RE15_TRILOG, Fenster 140,100..180,138, letztes Bild, Platz 4 = Prop 0 = Cursor)

| UV-Bereich | Ebene | NCLIP (x1-x0)(y2-y0)-(y1-y0)(x2-x0) | Anzahl | Tiefe |
|---|---|---|---|---|
| v 0..82 (Muster) | Oberseite y=-900 (49 Vierecke = 98 Dreiecke) | > 0 | 98 | 15687 |
| **v 0..82 (Muster)** | **Unterseite y=+923 (4 Dreiecke D03/D04/D05/D11)** | **< 0** | **4** | **17515** |
| v 83..168 (leer) | Vorderseite z=+900 | 0 (84) / < 0 (18) | 102 | - |
| v > 168 (leer) | Seiten x=+-900, z=-900 | < 0 (6) / 0 (2) | 8 | - |

Flaechenliste des Cursor-MD1 (`A_belege/cursor_md1.py`, ROOM11F0.RDT MD1 @0x001928, TIM @0x018DAC):
Unterseite D03 u 4..125 v 41..82 (1238 Texel != 0), D04 u 65..125 v 0..82 (1219), D05 u 4..125 v 0..41 (1264),
D11 u 4..65 v 0..82 (1262) = das komplette Eckwinkel-/Kreuz-Muster der Oberseite. Alle Seiten- und Vorderflaechen
tasten nur Texel 0 ab (durchsichtig).

**Befund:** Der Port zeichnet die 4 RUECKSEITIGEN Dreiecke der Unterseite (NCLIP < 0). Sie liegen 1828 Einheiten
tiefer (perspektivisch kleiner, zur Bildmitte versetzt), ihre Normale zeigt vom Licht weg (dunkel schattiert) und
sie scheinen durch die zu 92,8 % durchsichtige Oberseite: eine dunkle, kleinere Kopie von Eckwinkeln und Kreuz =
der "komische Schatten".

## 2. RE-Belege

### 2.1 Das Original verwirft Rueckseiten JEDES Objekt-Polygons (NCLIP, MAC0 < 0)

Objekt-Zeichenschleife FUN_8002c18c (Pool 0x800b3f98, Schritt 0x94; `RE_15_Quellcode_V2/FUN_8002c18c.c`):
Typ 4 -> y -= 900; Attribut `& 0x10 == 0` -> FUN_800254a0 (GT3) + FUN_800256b0 (GT4), sonst FUN_80022f0c +
FUN_80023098 (flach); Typ 3 -> FUN_8003e64c (ruft wieder FUN_80022f0c/FUN_80023098). Der Cursor (Typ 4, Attribut
pc[8..9] = 0) laeuft durch FUN_800254a0/FUN_800256b0. Selbst disassembliert (`re15_disasm.py dis`):

| Zeichner | RTPT | NCLIP `4b400006` | MAC0 sichern `e9980000` (swc2 r24) | Verzweigung |
|---|---|---|---|---|
| FUN_800254a0 GT3 | @0x8002554c `4a280030` | @0x80025580 | @0x8002559c, `lw v0,0(sp)` @0x800255a0 | @0x800255a8 `bgez v0,0x800255d0` (nur MAC0 >= 0 zeichnen; sonst a1 += 80, naechstes) |
| FUN_800256b0 GT4 | @0x80025764 `4a280030` (v0,v1,v2) | @0x80025798 | @0x800257b4 | @0x800257c0 `bgez v0,0x800257e8` |
| FUN_80022f0c GT3 flach | | @0x80022fec | @0x80023004 | @0x80023010 `bltz v0,0x80023080` (verwerfen) |
| FUN_80023098 GT4 flach | | @0x80023168 | @0x80023184 | @0x80023190 `bgez v0,0x800231b8` |

NCLIP (psx-spx GTE "NCLIP": MAC0 = SX0*SY1+SX1*SY2+SX2*SY0-SX0*SY2-SX1*SY0-SX2*SY1) auf den ersten drei Ecken des
Flaechensatzes (Dreieck v0,v1,v2; Viereck ebenfalls v0,v1,v2 — RTPT @0x80025764 laedt Satz+2/+6/+10, die vierte
Ecke folgt per RTPS). Verworfen wird STRENG kleiner 0; = 0 wird gezeichnet.
Zusatz (nicht geaendert, nur festgehalten): OTZ>>6 == 0 -> nicht in die OT (@0x80025654/58 `sra v0,v1,6` /
`beq v0,zero`).

### 2.2 Der Port

* `platform/pc/main.c` Raum-Prop-Schleife (Obj_model_set = FUN_8002c18c): Dreiecke und Vierecke werden projiziert
  und OHNE jede Rueckseiten-Pruefung eingereiht (Stand 1bcbd0ff; die Figuren-Zeichner derselben Datei pruefen
  `IS_BACKFACE`, die Props nicht).
* `platform/pc/src/hebetisch_cursor_pc.c` (ROOM1150/1151): dasselbe, "genau so wie die Raum-Prop-Schleife".

## 3. Umsetzung

* NEU `include/re15_nclip_r38.h` + `engine/src/nclip_r38.c`: `re15_nclip_r38_mac0()` = GTE NCLIP woertlich nach
  psx-spx (`geometrytransformationenginegte.md:439`) auf den wie RTPS/RTPT gesaettigten Bildschirmwerten
  (-0x400..+0x3FF, ebd. :418-419); `re15_nclip_r38_verwerfen()` = 1 bei MAC0 < 0 (FUN_800254a0 @0x800255a8 /
  FUN_800256b0 @0x800257c0 `bgez`, FUN_80022f0c @0x80023010 `bltz`, FUN_80023098 @0x80023190 `bgez`), zaehlt mit
  (`g_re15_nclip_r38_verworfen`, Messhaken).
* Haken `platform/pc/main.c` (gemeinsame Datei, 1 Include + 2 Zeilen): Raum-Prop-Schleife (= FUN_8002c18c), Dreieck
  nach der Projektion (v0,v1,v2), Viereck (v0,v1,v2) — jeweils `continue` bei Verwurf, Kommentar mit Adressen.
* `platform/pc/src/hebetisch_cursor_pc.c` (ROOM1150/1151, Modul des Cursors): dieselben zwei Pruefungen + Kopfzeile.
* Keine Konstante ausser den belegten (Saettigung -0x400/+0x3FF psx-spx; Schwelle "< 0" aus den vier Verzweigungen).

## 4. Messung nachher

### 4.1 ROOM11F0 Cut 10 (gleicher Lauf wie §1.1, neue exe)

`A_belege/nachher_11f0_psx_alt_neu_s1_s4.png` (oben: Original | Port alt Skala 1 | Port neu Skala 1; unten: Port alt
Skala 4 | Port neu Skala 4) und `A_belege/nutzersicht_s3_psx_alt_neu.png` (Skala 3 = Nutzerfenster, Original 1x
nativ auf dieselbe Groesse gezogen | alt | neu): die innere dunkle Kopie ist weg.

Pixelmass im Cursor-Fenster x 140..179, y 100..137 (Skala 1, `A_belege/cursor_klassen.py` Klassen G/g/Y/y = Paletten-
eintraege 3/1/4/2 des Cursor-TIMs):

| Bild | Abweichungen der Klassenkarte gegen das Original | gruenstichige Pixel (G > R+12 und G > B+12) in x 144..175, y 104..133 |
|---|---|---|
| Original (DuckStation) | - | 106 |
| Port ALT (1bcbd0ff) | 21 (alle "dunkelgruen statt Hintergrund": Spalte 151 Zeilen 110-114/123-128, Zeile 128 x 152-156/166-170) | 166 |
| Port NEU | **0** | **106** |

Der Rest der Cursor-Darstellung (der in die Textur gebackene 1-Pixel-Schlagschatten rechts/unten, Original-Kunst
+5u/+3v) ist bei Skala 1 klassengleich mit dem Original und bleibt unveraendert.

## 5. Tests

(in Arbeit)

## 6. OFFEN

(in Arbeit)

## 7. Fuer den Nutzer

(in Arbeit)
