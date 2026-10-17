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

Keine Regression im Sinne eines Bisects: die Raum-Prop-Schleife hatte seit der Uebernahme der Engine nie einen
Rueckseiten-Verwurf (`git log -S IS_BACKFACE` / `-G backface` auf platform/pc/main.c: nur aa346af1 (Figuren) und
370dfd5e (Waffe in der Hand), nie die Props). Der Schatten war also seit jeher da; die zwei frueheren Meldungen
(2026-09-26/27) wurden der Textur zugeschrieben (§0).

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
Flaechensatzes (Dreieck v0,v1,v2: `lh` Satz+2/+6/+10 @0x800254f4/@0x800254f8/@0x80025508 -> RTPT @0x8002554c; Viereck
ebenfalls v0,v1,v2: `lh` Satz+2/+6/+10 @0x8002570c/@0x80025710/@0x80025720 -> lwc2 @0x80025744..58 -> RTPT @0x80025764,
die vierte Ecke folgt per RTPS). Verworfen wird STRENG kleiner 0; = 0 wird gezeichnet.
Zusatz (nicht geaendert, nur festgehalten): OTZ>>6 == 0 -> nicht in die OT (@0x80025654/58 `sra v0,v1,6` /
`beq v0,zero`).

Ausgeschlossene Hypothesen (je mit Beleg):
* **Objekt-Schatten (Figuren-Schattenroutine):** die Objekt-Schleife FUN_8002c18c ruft NUR `jal 0x80068098`
  (RotMatrix) @0x8002c218, 0x80022da0 @0x8002c22c/@0x8002c294, 0x80053fc0 @0x8002c254, 0x80068068 @0x8002c264,
  0x80066ee0 @0x8002c278, 0x80067f28 @0x8002c280, 0x80066960 @0x8002c29c, 0x80068328 @0x8002c2a4, 0x8003e64c
  @0x8002c2c8, 0x80014368 @0x8002c2dc, 0x80022f0c @0x8002c320, 0x80023098 @0x8002c350, 0x800254a0 @0x8002c380,
  0x800256b0 @0x8002c3b0 — keinen Schattenzeichner (FUN_8001b064/FUN_8001af5c). Der Port zeichnet fuer Props
  ebenfalls keinen Schatten-Quad (re15_render_shadow_quad nur Spieler/NPC). -> kein Schatten-Pass.
* **Halbtransparenz:** Cursor-Attribut pc[8..9] = 0 (obj+0x0C, LAB_80040914 @0x800409a8/@0x800409b0), CLUT ohne
  STP-Bit (Eintraege 0..5 = 0000/0160/00E8/03E1/035B/47FE), Port-Blend 0. -> nein.
* **Unterlage/fremdes Objekt:** die zusaetzlichen Bildpunkte stammen aus dem Textur-Platz des Cursors selbst (Platz 4 =
  Prop 0, RE15_TRILOG §1.2). -> nein.
* **Doppelzeichnung des Musters:** JA — Oberseite UND Unterseite tragen das Muster; das Original zeichnet nur die
  vorderseitige (NCLIP >= 0).

### 2.2 Der Port

* `platform/pc/main.c` Raum-Prop-Schleife (Obj_model_set = FUN_8002c18c): Dreiecke und Vierecke werden projiziert
  und OHNE jede Rueckseiten-Pruefung eingereiht (Stand 1bcbd0ff; die Figuren-Zeichner derselben Datei pruefen
  `IS_BACKFACE`, die Props nicht).
* `platform/pc/src/hebetisch_cursor_pc.c` (ROOM1150/1151): dasselbe, "genau so wie die Raum-Prop-Schleife".
* Gegenstueck: der PSX-Port des Ports zeichnet Objekte mit `backface_cull = 1` (platform/psx/main.c:719/756) und
  verwirft `nclip_result < 0` (platform/psx/src/mesh_psx.c:489-493) — nur der PC-Zweig hatte die Pruefung nicht.

### 2.3 Das Original im Bild: keine Kopie der Unterseite

`analysis/grundwahrheit/psx_room11f0_cut10.png` ist ein DuckStation-Bild des Originals (Runde 27). Selbst nachgeprueft:
der Spielstand `psx_room11f0_cut10.sav` laeuft auf der Auslieferungs-EXE (@0x80026e4c = `08 00 e0 03`), Objekt 0 =
Cursor (+0 = 0x0101, Typ 4, Lage (-19554,0,22684)), und BEIDE 320x240-Puffer im VRAM des Spielstands sind
stichprobengleich mit dem PNG (0 von 19200 Stichproben abweichend). Im Original liegt im Inneren des Eckwinkel-Rahmens
KEIN Cursor-Gruen (einziger reingruener Punkt (151,110) = (8,16,8) = Hintergrund-Rasterung). Der Spielstand
`analysis/befunde_runde36/G2_belege/room11f0_orig.sav` belegt zusaetzlich, dass das Original die Cursor-Textur
unveraendert nutzt (CLUT bei VRAM (0,490) = Datei-CLUT, MD1 im RAM gepatcht auf CLUT 0x7A80 / Seite 0x85, 8 bpp,
ABR 0): der Unterschied liegt NICHT in Textur oder Farbe, sondern darin, WELCHE Flaechen gezeichnet werden.

### 2.4 Alle Cursor-Raeume haben dieselbe Bauart (Zensus)

`A_belege/cursor_raeume_zensus.py` (Byte-Muster `2d <obj> 04` + Box (0,-900,0,900,900,900); Modell = Eintrag obj_id der
Prop-Tabelle @RDT+0x30): 23 Raum-Varianten — 1080/1081, 4020/4021, 30E0, 10D0/10D1, 11E0/11E1, 1230/1231, 11F0/11F1,
1100/1101, 2060/2061, 2040/2041, 5050/5051, 3050/3051 (ROOM30E1 hat keinen Cursor-Record). JEDER Cursor: 29
Muster-Vierecke auf der Oberseite y=-900 + 4 Muster-Dreiecke auf der Unterseite y=+923, Unterseite gegensinnig
gewunden. Die Raeume 11E0/11E1 und 2060/2061 tragen je zwei Records desselben Modells, 3050/3051 daneben ein
anderes Objekt (obj 1, voll texturierter Kasten). ROOM1150 hat keinen eigenen Cursor (Port-Bau, Runde 34 Nacht:
11F0-Modell 1:1). -> Der Fehler traf alle Cursor-Raeume; die Behebung in der Prop-Schleife trifft alle.

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

### 3b. Behebung der Nebenwirkung: Feuerloescher-Modell wie das Original gewickelt (Befund §4.5)

* `tools/r37_feuerloescher/feuerloescher_modell.py` `md1_bauen()`: neue Funktion `wicklung()` (w wie §4.5); jede
  Flaeche mit w > 0 bekommt **v1 <-> v2** getauscht (samt Normale und UV). Beim Dreieck kehrt das den Umlauf um; beim
  Viereck in Z-Ordnung (v0,v1,v2,v3) bleiben beide Teilungs-Diagonalen (PSX v1-v2, Port v0-v3) und alle vier Ecken
  dieselben — Form, Textur und Licht unveraendert, nur der Umlauf dreht. Kopf des .inc nennt die Regel.
  Herleitung der Regel: die Original-Konvention w < 0 (9364 : 41, §4.5) + der Verwurf @0x800255a8/@0x800257c0.
* `engine/src/gen/feuerloescher_r37.inc` mit dem Werkzeug NEU ERZEUGT (vorher geprueft: das unveraenderte Werkzeug
  erzeugt die eingecheckte Datei inhaltsgleich, `git diff` leer; die Vorschaubilder unter
  analysis/befunde_runde37/I_belege/ bleiben bytegleich). Unterschied alt -> neu, Feld fuer Feld nachgeprueft:
  TIM, Text msg 29, alle 235 Punkte und Normalen gleich; 10 Dreiecke + 26 Vierecke gleich, **112 Vierecke mit
  v1 <-> v2 (und UV1 <-> UV2)**, sonst nichts (stehend und liegend). Danach w < 0 bei 148 von 148 Flaechen.
* Keine neue Konstante; Lage, Masse, Textur des Loeschers unberuehrt (Runde-37-Spur-I-Riegel unit_r37_feuerloescher_*
  pruefen Kopf, Huelle, Normalen nach aussen und Z-Ordnung weiter).
* **Ueberschneidung fuer die Integration:** Spur K (r38_itemorte) haengt den Loescher nach ROOM1030 (Lage in
  feuerloescher_r37.c/.h). Aendert K das Modell nicht, gibt es keinen Konflikt; erzeugt K `gen/feuerloescher_r37.inc`
  neu, dann mit DIESEM Werkzeugstand (sonst kommt die gegensinnige Wicklung zurueck — unit_r38_cursor_wicklung schlaegt
  dann an).

### 3c. Behebung der Nebenwirkung auf flache Port-Modelle: Parking Lot Key gewendet (Befund §4.6)

* `include/re15_schluessel_r36.h`: `RE15_R36Q_PARK_ROT_X 2048`, `RE15_R36Q_PRISON_ROT_X 0` mit Herleitung im Kopf:
  Verwurf MAC0 < 0 (RE1.5 FUN_800254a0 @0x800255a8 / FUN_800256b0 @0x800257c0, RE2 FUN_8002d0e8 @0x8002d1ac) -> ein
  flaches Modell zeigt nur die Seite, von der seine Umlauf-Normale WEG zeigt; P. Room Key (RE2 ROOM6150 @0x004FBC,
  Umlauf-Normale -Y) legt RE2 selbst GEWENDET (ROOM6150 MAIN sub00 @0x02C98 Obj_model_set obj 8, Dir z 0x600), den
  Cabin Key (+Y) UNGEWENDET (ROOM1050 MAIN @0x01274, Dir 0,0,0). 2048 = 180 Grad.
* `engine/src/schluessel_r36.c` `schluessel_anlegen()`: eine Zeile `rot_x` je Modell (Parking Lot Key 2048, Prison
  Key 0).
* Achse x statt z (RE2 wendet um z): Rx(180) = Ry(180) * Rz(180) — beide zeigen dieselbe (Vorder-)Seite, sie
  unterscheiden sich nur um eine halbe Drehung in der Ebene. Um z lag der Schluesselkopf am anderen Ende (gemessen,
  nicht committeter Zwischenstand mit rot_z 2048, Bild k10f0b_neu_s1/s3: Kopf rechts statt links); um x bleibt er an dem Ende, an das Runde 36 ihn gelegt
  hat (PORT-WAHL der Lage, rot_y 256 = 22,5 Grad zur Konsolenkante, unveraendert).
* **Merge mit Spur K (r38/itemorte):** K ersetzt in `schluessel_anlegen()` und `re15_schluessel_r36.h` genau diese
  Lage (Pinnwand, `re15_itemorte_r38_schluessel_lage`, Konstanten RE15_R36Q_X/Y/Z/ROT_Y entfernt). Beim Konflikt
  **K's Seite nehmen** und meine zwei Konstanten + die `rot_x`-Zeile verwerfen: K's Lage zeigt die Vorderseite (§4.6
  gerechnet), und integration_r38_cursor E/F prueft das danach an der exe (Cut 5 bzw. 6 sind K's Pinnwand-Cuts).
  NICHT beide Seiten behalten — sonst setzt die spaetere Zeile die Lage der anderen Spur zurueck.

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

RE15_TRILOG (Fenster 140,100..180,138, alle Bilder des Laufs, Platz 4): ALT 8456 eingereihte Cursor-Dreiecke mit
NCLIP < 0, NEU 0. Reingruene Bildpunkte (R,B <= 12, G >= 15, G-R/G-B >= 12) im Rahmen-Inneren: ALT 33, NEU 0
(Original 0).

### 4.2 ROOM1150 Hebetisch-Cursor (Lade-Weg: Spielstand probe_r30_granate_karte, CONTINUE, Aktion am Tisch)

`A_belege/lauf_1150.sh`, Bild 150 (Cursor seit F101 aktiv, cursor.log), Skala 1. Bild
`A_belege/nachher_1150_orig11f0_alt_neu.png` (Original-Cursor 11F0 | 1150 alt | 1150 neu): die innere Kopie ist weg.
Reingruen im Rahmen: ALT 33, NEU 0. Klassen G/g gegen den Original-Cursor 11F0: NEU deckungsgleich (die 4 Rest-
"Abweichungen" (144..145,104..105) sind brauner Hintergrund des Raums, in alt und neu gleich). Trilog Platz 28
(RE15_HC_TIM_SLOT): ALT 1456 Dreiecke mit NCLIP < 0, NEU 0. Nutzersicht Skala 3 (Fenster des Nutzers):
`A_belege/nutzersicht_s3_1150_alt_neu.png` — alt je Eckwinkel ein dickes dunkles inneres L und eine versetzte dunkle
Kreuz-Kopie, neu nur noch der feine Saum der Textur.

### 4.3 ROOM10D0 Ziffernfeld (weiterer Panel-Raum; Weg D aus Runde 37 Spur E: sub17 per RE15_SUBSTART)

Bild 260, Skala 1. `A_belege/nachher_10d0_orig11f0_alt_neu.png`: alt mit innerer Kopie, neu ohne. Geaendert ALT->NEU:
50 Bildpunkte, ALLE im Cursor-Fenster x 150..170, y 110..130. Reingruen im Rahmen: ALT 32, NEU 0. Trilog Platz 5
(Prop 1): ALT 2997 Dreiecke mit NCLIP < 0, NEU 0.

### 4.3b Elza: ROOM11F1 (Spielerwahl auf Elza, Debug-Sprung 11F0 -> fuer Elza ROOM11F1) und ROOM1151

`A_belege/lauf_elza_11f1.sh` (RE15_PSELECT_AUTO=1 + _SWITCH=1, debug.log "[pl] Spieler-Familie PL04 (character=4,
Elza-Bit=1)", "EXIT_AT: Bild 502 in Raum 11f1"), Cut 10, Bild 500: NEU 0 rueckseitige Cursor-Dreiecke, 0 reingruene
Punkte im Rahmen, Klassenkarte 0 Abweichungen gegen das Original; ALT 14056 / 33 / 21. Raumvariante ROOM1151
(Hebetisch, Lade-Weg mit RAUM=1151; das Kartenwerkzeug schreibt Spielerfamilie PL00, die Cursor-Zeichnung ist vom
Spieler unabhaengig): NEU Platz 28 0 rueckseitige Dreiecke, 0 reingruene Punkte.

### 4.4 Nebenwirkungen (der Verwurf gilt fuer ALLE Raum-Props, wie im Original)

* **Scan aller 120 Leon-Raeume** (`A_belege/raum_scan.py`, Debug-Sprung, Bild 200, Skala 1, exe alt gegen neu;
  `A_belege/raum_scan_120.txt`): 112 Raeume pixelgleich, 8 mit 1..41 geaenderten Punkten (5010 41, 2040 19, 3080 8,
  10F0 5, 20B0 5, 4010 5, 4000 3, 2090 1). Wiederholungslauf: alt/alt und neu/neu je 0 Punkte Unterschied -> die
  Aenderungen sind deterministisch und stammen vom Verwurf. Bild `A_belege/nebenwirkung_scan_5010_2040_3080.png`
  (alt | neu | Markierung): 5010 ein dunkelroter Streifen (Rueckseite) ueber einem Geraet faellt weg, darunter die
  beleuchtete Kante und die gruene Lampe; 2040 ein 4x4-Block an Leons Hand; 3080 Einzelpunkte entlang einer Kante.
  **Nachtrag 2026-10-17:** die 5 Punkte in 10F0 (96..99, 169..170) waren KEINE Kleinigkeit, sondern der ganze
  Parking Lot Key (Port-Prop, flach, Rueckseite oben) — er verschwand. Befund und Behebung §4.6/§3c.
* **Elza-Raeume (Fortsetzung 2026-10-16 abends).** Statischer Vergleich zuerst (`A_belege/elza_props_vergleich.py`):
  der Verwurf wirkt nur auf Raum-Objekte; in 73 Raeumen sind Modelle (Prop-Tabelle RDT+0x30) und Obj_model_set-Saetze
  von ROOMxxx1 und ROOMxxx0 gleich (Wirkung = Leon-Scan), 30 unterscheiden sich, 17 haben keine Elza-Variante. Scan
  mit Elza (`raum_scan.py --elza`: RE15_PSELECT_AUTO + _SWITCH, Debug-Sprung @250 -> der Raumlader nimmt ROOMxxx1,
  Bild 400; exe ALT 1bcbd0ff gegen NEU = Verwurf + neu gewickelter Loescher) ueber 75 Raeume = 1000..3030 (60, davon
  12 der "verschiedenen") + die uebrigen 15 "verschiedenen" (3060 3070 3080 3090 30E0 4000 4010 4030 5000 5010 5020
  5030 50B0 6000 6030); `A_belege/raum_scan_elza_75.txt`: **68 pixelgleich**, 7 mit 4..41 Punkten (5010 41, 2040 38,
  4010 32, 20B0 6, 2090 4, 3080 4, 4000 4) — dieselben Raeume wie im Leon-Scan. (Der Lauf "3080" endet laut debug.log
  in ROOM4001 — Raumwechsel nach dem Sprung —, daher die gleichen 4 Punkte wie "4000".) Kein Elza-Raum mit einer
  groesseren Aenderung.
* **ROOM11F0 Cut 10:** 85 Punkte an den Kippschaltern (Platz 28/29 hatten je 2 Rueckseiten-Dreiecke); mittlere
  Abweichung der Schalterzeile 1 zum Original 12,90 -> 12,61 (naeher am Original).
* **ROOM1150 Cut 4 (Hebetisch-Sicht):** 747 Punkte. `A_belege/nebenwirkung_1150_cut4_alt_neu.png`: alt lag ein
  dunkelbraunes Band (Rueckseiten von Prop 0 = Plattform mit den Papierstapel-Hochhaeusern, im Band-Fenster 54
  Dreiecke mit NCLIP < 0, RE15_TRILOG) quer ueber Hochhaeusern und Hintergrund; neu sind dort Hochhaeuser/Hintergrund
  zu sehen. Prop 0 ist ein Original-Objekt (ROOM1150 Obj_model_set), das Original zeichnet es durch dieselben
  Zeichner FUN_800254a0/FUN_800256b0 mit demselben Verwurf (§2.1).
* **Hinweis fuer die Integration mit Spur W (ROOM20A0, Rolladen "direkt vor der Kamera", Nachtrag 1):** die Rolllaeden
  sind Raum-Objekte (obj 4/5, Typ 3, ROOM20A0.RDT Obj_model_set @0x01998/@0x019BA), die sub01 je Kamera-Schnitt
  (Cmp 0x0A = Cut) zwischen ihrer Lage und y = -16200 umsetzt (@0x01B22/@0x01B2A, @0x01B7E/@0x01B86, @0x01BDA/@0x01BE2).
  Der Verwurf aendert ihr Bild: Debug-Sprung 20A0, Cut 2 erzwungen, Bild 200, alt gegen neu 170 Punkte, alle am
  Rolladen oben links (x 7..135, y 0..54; was dort wegfaellt, sind dessen Rueckseiten-Flaechen); Cut 4/10/12
  0/1/5 Punkte. Spur W misst auf der Basis OHNE Verwurf — ihre Rolladen-Messung ist in der Integration mit diesem
  Stand zu wiederholen (Messbilder nicht versioniert: re15_port/build/r38_a/w20a0_*).
* Port-eigene Props (dokumente_r34, feuerloescher_r37, granate_1150, inhalt_r35, irons_tisch_1150, kartenitems_r36,
  schluessel_r36, sicherung_1150) und alle Panel-/Hebetisch-Riegel: §5 (Bild-Riegel ihrer Spuren laufen mit).
  **Fortsetzung 2026-10-16 abends: das reichte NICHT** — der Feuerloescher-Riegel prueft nur das LOG
  (test_r37_feuerloescher.cmake Kopf: "geprueft wird das LOG"), ein falsch gewickeltes Modell faellt dort nicht auf.
  Deshalb §4.5.

### 4.5 Nebenwirkung auf PORT-GEBAUTE Modelle: Wicklung (Fortsetzung, 2026-10-16 abends)

Der Verwurf zeichnet eine Flaeche nur, wenn ihre Ecken v0,v1,v2 auf dem Bildschirm im Original-Umlaufsinn liegen.
Das stimmt fuer "zeigt zur Kamera" nur, wenn das Modell so gewickelt ist wie die Original-Modelle. Mass je Flaeche
(Modellraum, kameraunabhaengig): w = sign(((v1-v0) x (v2-v0)) . (n0+n1+n2[+n3])) mit den gespeicherten
(Licht-)Normalen. Werkzeug `A_belege/wicklung_props.py`:

| Modelle | w < 0 | w > 0 | w = 0 |
|---|---|---|---|
| ALLE Original-Props aller RDTs (je Modell einmal, 119 Modelle; 107 rein w<0, 10 gemischt, 2 leer) | 9364 | 41 | 72 |
| Port: Dokumente 1-4, Irons Diary/Keycard, Granate, Schrot, Lageplan-Karten 0-4, Schluessel Park/Prison, Sicherung, Hebetisch-Cursor | alle | 0 | 0 |
| **Port: Feuerloescher `k_fl_md1_stehend` / `k_fl_md1_liegend` (gen/feuerloescher_r37.inc, Runde 37 NEU GEBAUT)** | 36 | **112** | 0 |

-> Konvention des Originals: w < 0 (99,6 % aller Flaechen). Der Feuerloescher ist das EINZIGE Port-Modell mit
umgekehrter Wicklung: die 90 Band-Vierecke (Boden-Fase, Mantel, Schulter, Hals) und die 16+4 Schlauch-Vierecke sind
gegensinnig, die 10 Boden-Dreiecke und die 24 Kasten-Vierecke (Ventil, Griff, Hebel) richtig; von den 4
Schlauch-Kappen je eine richtig, eine falsch. Ohne Verwurf war das unsichtbar; MIT Verwurf zeichnet der Port vom
Mantel nur die abgewandte INNENSEITE.

Messung (Lade-Weg wie Lauf E/F von test_r37_feuerloescher.cmake, `A_belege/lauf_loescher.sh`; exe ALT = 1bcbd0ff
ohne Verwurf, exe VORWICKLUNG = 87aeed73 mit Verwurf und altem Modell): ROOM1050 Cut 6 (Loescher an der Wand)
Skala 1: 13 Punkte geaendert (x 221..225, y 111..119); ROOM1090 Cut 5 (liegend): 11 Punkte (x 155..165). Skala 3
(Nutzerfenster) ROOM1050: ALT voller roter Zylinder mit weissem Etikett; VORWICKLUNG schmaler, Etikett dunkel und
schmal, unten ein schwarzer Rand = die Innenseite der Rueckwand. -> echte Nebenwirkung des Verwurfs, Behebung §3b.

**Messung nachher** (dieselben Laeufe, exe NEU = Verwurf + neu gewickeltes Modell):

| Lauf | NEU gegen ALT (ohne Verwurf) | NEU gegen VORWICKLUNG |
|---|---|---|
| ROOM1050 Cut 6, Skala 1 | **0 Punkte** | 13 Punkte (x 221..225, y 111..119) |
| ROOM1050 Cut 6, Skala 3 (Nutzerfenster) | **0 Punkte** | 120 Punkte (x 664..680, y 317..360) |
| ROOM1090 Cut 5, Skala 1 | 2 Punkte bei (155..157, 75..76) = ein Original-Prop weiter oben (Verwurf wie §4.4), am Loescher 0 | 9 Punkte (x 160..165, y 131..133) |

Bild `A_belege/nebenwirkung_loescher_alt_vorw_neu.png` (oben ROOM1050 Skala 3, unten ROOM1090 Skala 1 x6; je
ALT | VORWICKLUNG | NEU): NEU ist am Loescher pixelgleich mit dem Stand vor dem Verwurf, VORWICKLUNG zeigt die
Innenseite. Riegel: unit_r38_cursor_wicklung (§5).

### 4.6 Nebenwirkung auf FLACHE Port-Modelle: der Parkplatz-Schluessel ROOM10F0 verschwand (Fortsetzung 2026-10-17)

**Messung.** Der 120-Raum-Scan (§4.4) meldete ROOM10F0 mit 5 Punkten bei (96..99, 169..170). Ausschnitt ALT/NEU: in ALT
liegt dort ein kleiner dunkler Gegenstand mit rotem Punkt auf der Konsole, in NEU fehlt er. RDT-Kopf ROOM10F0
`00 0c 04` = nCut 12, nOmodel 4 -> obj 4 ist kein Original-Prop, sondern der Port-Schluessel der Runde 36 Spur Q
(RE15_R36Q_OBJ 4, TIM-Platz RE15_TIM_SLOT_PROP(4) = 8). RE15_TRILOG (exe ALT 1bcbd0ff, Debug-Sprung 10F0, Bild 200,
Fenster 85,155..115,185): Platz 8 = 4 Dreiecke, ALLE mit NCLIP < 0; NEU: Platz 8 fehlt. Je Cut (`A_belege/
port_modelle_sicht.py`, RE15_FORCE_CUT 0..11): Cut 0, 2..10 je 4 von 4 Dreiecken NCLIP < 0 (WEG), Cut 1 2x < 0 + 2x = 0
(Flaeche 0). -> Mit dem Verwurf ist der Parking Lot Key in KEINEM Cut sichtbar (Runde 36 Q hatte ihn in Cut 0 mit
9 Bildpunkten abgenommen, `analysis/befunde_runde36/Q_schluessel.md` §3.2). Echte Regression dieses Stands.

**Warum §4.5 das nicht sah.** Die Wicklungs-Pruefung nimmt die gespeicherte Normale als "aussen". Ein FLACHES Modell hat
aber nur eine Seite; welche die Kamera sieht, bestimmt allein die LAGE. Die Normale der flachen RE2-Items ist bei allen
gleich (0,4096,0), ihr Umlauf aber verschieden (Zensus `scratchpad` flach_zensus, Port-Modelle in gen/*.inc):

| Modell | Flaechen | Umlauf-Normale g = (v1-v0)x(v2-v0) | Lage im Port |
|---|---|---|---|
| Parking Lot Key `re15_r36q_park_md1` (RE2 ROOM6150 @0x004FBC) | 4 Dreiecke, flach | **-Y** | flach auf der Konsole, nur rot_y 256 (ungewendet) -> Rueckseite oben (s.u.) |
| Prison Key `re15_r36q_prison_md1` (RE2 Cabin Key ROOM1050 @0x001558) | 1 Viereck, flach | +Y | flach |
| Irons Keycard `re15_irons_karte_md1` | 2, flach | +Y | Schreibtisch |
| Lageplan-Karten `k_r36k_md1_0..4` | je 4, flach | +Y | an Waenden (rot_z 1024 / 3072) |
| Dokument 3/4 | 8, fast flach | ~+Y | - |

**Regel (RE-Beleg).** Gezeichnet wird bei MAC0 >= 0 (§2.1), d.h. wenn g im Bild VON der Kamera weg zeigt (Blickraum:
Dreieck (0,0),(1,0),(0,1) bei y nach unten hat MAC0 = +1 und g = +z = in den Schirm). Fuer ein flaches Modell auf einer
Platte, von oben gesehen (PSX: y nach unten), muss g also nach +Y zeigen. Der Parking Lot Key hat g = -Y und liegt
ungewendet (nur rot_y) -> Rueckseite oben -> verworfen.
RE2 Retail verwirft genauso (info/re2leon/PSX.EXE, selbst disassembliert, re2_disasm.py): Objekt-Schleife FUN_800363f0
(Pool 0x800d0324, Schritt 0x1F8) -> FUN_8002d0e8 (Attribut & 0x10) RTPT `4a280030` @0x8002d170, NCLIP `4b400006`
@0x8002d18c, `swc2 $24` @0x8002d1a0, `lw v0,0(sp)` @0x8002d1a4, `bgez v0,0x8002d1bc` @0x8002d1ac, sonst `j 0x8002d148`
+ `addiu t0,t0,80` @0x8002d1b4/b8 (naechste Flaeche); FUN_80027bec (Attribut & 0x10 == 0) NCLIP @0x80027ca8,
`bgtz v0,0x80027ce4` @0x80027cc8. Und RE2 legt jedes flache Item so, dass seine NCLIP-Vorderseite zur Kamera zeigt
(re15_port/tools/r34n_c/re2_scd_dump.py):

| RE2-Raum, Satz | Modell | g im Modell | Dir (x, y, z) |
|---|---|---|---|
| ROOM6150 MAIN sub00 @0x02C98 `2d 08 .. 10 00 ba f1 b0 f8 b6 bc c0 ff 00 07 00 06` | obj 8 = P. Room Key (= unser Parking Lot Key) | -Y | (-64, 0x700, **0x600**) = um z GEWENDET (135 Grad) |
| ROOM2020 MAIN sub00 @0x01D90 `2d 02 .. 10 00 ea a2 24 fa 6e a6 00 00 00 00 00 00` | obj 2 = Herz-Schluessel (Spur F) | +Y | (0, 0, 0) = flach |
| ROOM1050 MAIN @0x01274 `2d 00 .. 10 00 e4 cb 88 fa be c4 00 00 00 00 00 00` | obj 0 = Cabin Key (= unser Prison Key) | +Y | (0, 0, 0) = flach |

-> Die Port-Lage des Parking Lot Key (Runde 36 Q, ungewendet) zeigte die RUECKSEITE (Textur gespiegelt gegen RE2) — ohne
Verwurf unbemerkt, mit Verwurf unsichtbar. Behebung §3c.

**Laufende Spuren (fuer die Integration, gerechnet mit der Port-Matrix pc_prop_rot_q12 = Ry*Rx*Rz, main.c):**
* Spur K (r38/itemorte) haengt beide Schluessel an die Pinnwand ROOM10F0/10F1 (gen/itemorte_r38_lage.inc, "Sichtseite
  Modell +1y"): Modelle k_r38k_park_md1 / k_r38k_prison_md1 g = -Y; PARK rot (3072, 0, 2048), PRISON rot (1024, 0, 0) ->
  Spalte 1 der Matrix = (0, 0, 1) -> g_Welt = (0, 0, -1); die Pinnwand liegt an der Wand z = -3400, die Kamera davor
  (z > -3400) -> g zeigt von der Kamera weg -> VORDERSEITE, mit dem Verwurf sichtbar. K's gen/feuerloescher_r37.inc ist die
  ALTE Fassung (K aendert nur .c/.h) -> beim Merge gilt die neu gewickelte Datei dieses Zweigs (§3b), kein Konflikt.
  K ersetzt in schluessel_r36.c genau die Lage-Zeilen, die §3c aendert -> beim Merge **K's Seite nehmen** (s. §3c).
* Spur F (r38/dining1141): Herz-Schluessel re15_r38f_key_md1 (RE2 ROOM2020 @0x003474) g = +Y, Lage ROOM1011 obj 6
  (-2000, 0, 7450) rot 0 = flach auf dem Boden wie RE2 @0x01D90 -> g_Welt = +Y, Kamera oben -> VORDERSEITE, sichtbar.
  Achtung: die Normalen-Pruefung (unit_r38_cursor_wicklung Teil 2) wuerde dieses Modell faelschlich anmahnen (w > 0) —
  deshalb gilt sie seit §3c nur noch fuer RAEUMLICHE Modelle; flache prueft die Lage (§5).

**Messung nachher (§3c, exe NEU = Verwurf + gewendeter Schluessel):**
* `port_modelle_sicht.py --exe neu 10F0 10F0:elza` (RE15_TRILOG je Cut): ROOM10F0 Platz 8 in ALLEN 12 Cuts gezeichnet,
  0 Dreiecke mit NCLIP < 0 (Cut 1/11 je 2 mit Flaeche 0); ROOM10F1 (Elza, Prison Key) in allen Cuts gezeichnet
  (Cut 11: 1 von 2 Dreiecken des einen Vierecks < 0 — das Viereck wird wie im Original nur ueber v0,v1,v2 entschieden).
* Bild ROOM10F0 Cut 0 (Debug-Sprung, Bild 200) ALT gegen NEU: Skala 1 4 geaenderte Punkte in x 97..102, y 169..171,
  6 dunkle Schluesselpunkte in beiden; Skala 3 (Nutzerfenster) 55 Punkte in x 289..308, y 507..515.
  `A_belege/schluessel10f0_alt_neu_s1_s3.png` (oben Skala 1 x12, unten Skala 3 x4; je links ALT, rechts NEU): derselbe
  flache Schluessel an derselben Stelle, Kopf links, Klinge rechts; NEU zeigt die Vorderseite (Textur wie RE2), ALT
  zeigte die Rueckseite (gespiegelt).
* GEGENPROBE des Riegels (probe sicht, 10F0 Cut 0 + 5): exe ALT 1bcbd0ff -> je 4 Dreiecke, alle MAC0 < 0 -> ROT;
  exe VORWICKLUNG 87aeed73 (Verwurf, ungewendet) -> Platz 8 leer -> ROT; exe NEU -> gruen (§5).

## 5. Tests

Neu (Registrierung NUR `tests/unit/probes/r38_cursor.cmake`; Sonde `tests/unit/probe_r38_cursor.c`, exe-Haken
`tests/integration/test_r38_cursor.cmake`):

| Test | misst | Gegenprobe alter Stand |
|---|---|---|
| unit_r38_cursor_nclip | Praedikat: MAC0 = psx-spx NCLIP woertlich; -100 verworfen, +100 und 0 (kollinear) gezeichnet; Saettigung kippt (0,0)(3000,10)(1500,6) von +3000 auf -4092; Zaehler | - (Mechanik-Einheit) |
| unit_r38_cursor_kasten | ROOM11F0-Cursor unter Cut 10 an 25 Lagen (+-4000 um den Start): 29 Muster-Vierecke oben gezeichnet, 4 Muster-Dreiecke unten verworfen | im Test: ohne Verwurf ueberdeckten an allen 25 Lagen 4 Muster-Dreiecke den Cursor, an der Startlage ganz im Oberseiten-Umriss |
| unit_r38_cursor_raeume | 23 Cursor-Raeume: Record am Zensus-Offset, Modell aus der Prop-Tabelle, 29+4 Muster-Flaechen, Unterseite gegensinnig gewunden (gleichsinnig mit 11F0 oben) | - (Zensus) |
| integration_r38_cursor | echte exe, 4 Laeufe: A 11F0 Cut 10, B 1150 Hebetisch (Lade-Weg), C 10D0 Ziffernfeld, D Elza 11F1. Je: RE15_TRILOG des Cursor-Platzes ohne NCLIP < 0, Rahmen-Inneres ohne reingruene Punkte; A/D zusaetzlich Klassenkarte = Original | exe 1bcbd0ff mit demselben Skript: A ROT (8456 Rueckseiten-Dreiecke, 33 Punkte, 21 Abweichungen); Einzelpruefung B 1456/33, C 2997/32, D 14056/33/21 |
| unit_r38_cursor_wicklung (Stand 2026-10-17) | wie unten, aber Teil (2)/(3) nur fuer die 11 RAEUMLICHEN Port-Modelle; die 8 FLACHEN (Parking/Prison Key, Keycard = Memory Card 1150, 5 Lageplan-Karten; Kriterium: alle Umlauf-Normalen parallel, \|cos\| > 0,999) werden erkannt und an die Lage-Pruefung verwiesen (ihre gespeicherte Normale entscheidet die Sichtseite nicht, §4.6) | - |
| probe_r38_cursor sicht + integration_r38_cursor E/F (2026-10-17) | E: ROOM10F0 Cut 0 + 5, F: Elza ROOM10F1 Cut 0 + 6, RE15_TRILOG ganzer Schirm: Platz 8 (Schluessel) zeigt in mindestens einem Cut seine Vorderseite (MAC0 > 0). Zwei Cuts, damit der Riegel auch die Pinnwand-Lage der Spur K deckt | exe ALT 1bcbd0ff: Cut 0/5 je 4 Dreiecke MAC0 < 0 -> ROT; exe 87aeed73 (Verwurf, ungewendet): Platz 8 leer -> ROT |
| unit_r38_cursor_wicklung (Fortsetzung 2026-10-16) | (1) Original-Konvention ueber alle Props aller RDTs (206 RDTs, 570 Props, w<0 30037 : w>0 151 : w=0 72) >= 99 % w<0; (2) alle 19 eingebackenen Port-Modelle (Cursor, Sicherung, Granate, Irons Diary/Keycard, 2 Loescher, 5 Dokumente, Memory Card/Schrot, 5 Lageplan-Karten, 2 Schluessel) ohne Flaeche w>0; (3) Mechanik an 60 Blicken (12 Gier- x 5 Nickwinkel, Projektion re15_hebetisch_cursor_projiziere = main.c, Verwurf re15_nclip_r38_verwerfen): keine Aussenseite verworfen, keine abgewandte Flaeche gezeichnet (ausgelassen nur streifende Blicke, |cos| <= sin(Normalen-Glaettung + 5 Grad), und Splitter mit \|MAC0\| <= 2 x Umfang, deren Vorzeichen die Ganzzahl-Rundung kippen kann) | im Test: v1 <-> v2 an jedem Modell -> je Hunderte falsch verworfene/gezeichnete Flaechen (Loescher stehend 3006/3510); Altstand des Loeschers (git 79006fef): 112 von 148 Flaechen w>0 (wicklung_props.py) |

Gezielter Lauf (keine volle Suite), Regex = eigene Tests + Testgruppen aller beruehrten Module (Raum-Prop-Schleife
-> alle Bild-Riegel mit Raum-Objekten und die Raeume, deren Bild sich im Scan aenderte; Hebetisch; Panels/Cursor;
Port-eigene Props; Feuerloescher-Modell):
`ctest --test-dir re15_port/build -R "r38_cursor|r34n_b|hebetisch|11f0|cursor|panel|codepanel|irons_tisch|sicherung|dokumente|schrift1150|kartenitems|feuerloescher|schluessel|kiste|tor_hell|cut1150|szene1150|pri_schatten|pri1100|granate|dachkiste|fahrstuhl|ziel1190|cut10f0|elza_irons|pri_masken|generator|zeiger|r36_elza|r36_kleidung|r36_klettern|r22_10f0|r36_szenenleon|r36_gegnerzahl|r30_tuer_r4000|r36_tueren|r35_cut11c0|r30_cut_blitz|r34_plattform|room1150|irons_neck_1150|r35_inhalt" --timeout 900 --output-on-failure`
(Stand 2026-10-17: 197 Tests = 148 ohne + 49 mit der exe; `r35_inhalt` neu, weil Memory Card/Schrot Port-Props sind.
Gefahren in zwei Teilen am Endstand cd41f6b2 + Dossier, exe/Sonden gebaut nach 3f11372f/cd41f6b2:)

* Teil 1, `-R "<obiger Ausdruck>" -E "^integration_" -j4 --timeout 600`:
  **`100% tests passed, 0 tests failed out of 148`** (Total Test time (real) = 14.90 sec)

## 6. OFFEN

* Kein eigener DuckStation-Abzug fuer ROOM1150 Cut 4 (Hebetisch-Sicht, Nebenwirkung §4.4) und ROOM10D0. Begruendung:
  derselbe Original-Zeichner (FUN_8002c18c -> FUN_800254a0/FUN_800256b0) mit derselben Verzweigung zeichnet alle
  Objekte; dass er die Unterseite eines Cursors verwirft, zeigt das Original-Bild 11F0 (§2.3). ROOM1150 hat im
  Original keinen Cursor (Port-Bau Runde 34).
* Nicht Teil dieses Punkts, nur festgehalten: der PC-Port rastert 3D in Fensteraufloesung (render_pc.c
  SDL_RenderSetLogicalSize + SDL_RenderGeometry), das Original in 320x240. Der in die Cursor-Textur GEBACKENE
  Schlagschatten (+5u/+3v, Original-Kunst) erscheint deshalb bei Skala 3/4 als feiner dunkler Saum rechts/unten jedes
  Elements statt als einzelner Bildpunkt. Bei Skala 1 ist der Cursor klassengleich mit dem Original (§4.1).

## 7. Fuer den Nutzer

* Behoben: der "komische Schatten" war die UNTERSEITE des Cursor-Kastens (dasselbe Muster, 1828 Einheiten tiefer),
  die der PC-Port durch die durchsichtige Oberseite mitzeichnete. Das Original verwirft wie jede PlayStation-Engine
  Rueckseiten (GTE NCLIP). Gilt fuer alle 23 Cursor-Raeume des Originals (Generator 11F0, Ziffernfelder
  10D0/11E0/1230, Aufzug 1080/4020, 1100, 2040, 2060, 3050, 30E0, 5050 und Elzas Varianten) und fuer den
  Hebetisch-Cursor in Irons' Buero 1150/1151 (Port-Bau mit dem 11F0-Modell).
* Sichtbare Nebenwirkung, wie im Original: auch alle anderen Raum-Objekte zeichnen ihre Rueckseiten nicht mehr. In
  120 Leon-Raeumen aenderten sich nur Einzelpunkte (hoechstens 41); in Irons' Buero (Hebetisch-Sicht) verschwindet ein
  dunkles Band hinter den Papierstapel-Hochhaeusern.
* Der in Runde 37 neu gebaute Feuerloescher war als einziges Port-Modell "falsch herum" gewickelt und waere mit dem
  Verwurf von innen gezeichnet worden; sein Modell ist neu erzeugt (nur die Eckenreihenfolge, Form/Textur gleich) und
  sieht wieder genau so aus wie vorher (§4.5).
* Der Parkplatz-Schluessel (Parking Lot Key) im Funkraum ROOM10F0 lag seit Runde 36 mit der RUECKSEITE nach oben —
  ein flaches RE2-Modell hat nur eine Vorderseite, und RE2 selbst legt genau dieses Modell gewendet hin. Mit dem
  Verwurf waere er unsichtbar gewesen; er liegt jetzt gewendet an derselben Stelle (Kopf links wie bisher) und zeigt
  seine Vorderseite (§3c/§4.6). Elzas Prison Key in ROOM10F1 lag schon richtig herum.
* Ebenfalls wie im Original: eine Lageplan-Karte oder ein Dokument an der Wand ist aus einer Kamera HINTER der Wand
  nicht mehr zu sehen (ROOM11D0 Cut 7: die B1-Karte schien vorher durch die Wand; ROOM1020 Cut 5 Dokument 3 hochkant,
  14 Punkte; ROOM1010 Cut 2 ohne sichtbare Aenderung). In allen anderen Cuts sind sie unveraendert sichtbar (§4.6).
* Keine neuen Assets, keine Sprachdateien, keine Bedienaenderung, keine Entscheidungsfrage. Neue Engine-Datei
  `engine/src/nclip_r38.c` (Android: release/build_android.sh verwirft den GLOB-Cache bereits); geaendert
  `engine/src/gen/feuerloescher_r37.inc` (mit tools/r37_feuerloescher/feuerloescher_modell.py erzeugt),
  `engine/src/schluessel_r36.c` + `include/re15_schluessel_r36.h` (Wendung des Parking Lot Key).

## 8. Fuer die Integration

* **Regel fuer JEDES Port-Modell, das durch die Raum-Prop-Schleife (main.c pc_load_room_prop_set) oder den
  Hebetisch-Cursor geht:** sichtbar ist nur, was mit NCLIP MAC0 >= 0 auf v0,v1,v2 auf dem Schirm liegt (§2.1).
  RAEUMLICHE Modelle muessen wie das Original gewickelt sein (Umlauf-Normale gegen die Licht-Normale, w < 0;
  unit_r38_cursor_wicklung); FLACHE Modelle muessen so LIEGEN, dass ihre Vorderseite (die Seite, von der die
  Umlauf-Normale (v1-v0)x(v2-v0) weg zeigt) zur Kamera zeigt — wie RE2 (ROOM6150 @0x02C98 gewendet, ROOM2020
  @0x01D90 / ROOM1050 @0x01274 ungewendet). Pruefen an der exe: `analysis/befunde_runde38/A_belege/
  port_modelle_sicht.py <aus> --exe neu <RAUM[:elza]>` (je Cut, Platz mit "KEIN MAC0>0" = unsichtbar).
* **Spur K (r38/itemorte):** `schluessel_r36.c` / `re15_schluessel_r36.h` — Konflikt an der Lage des Schluessels:
  **K's Seite nehmen** (K's Pinnwand-Lage ersetzt die Konsolen-Lage samt RE15_R36Q_X/Y/Z/ROT_Y; meine Konstanten
  RE15_R36Q_PARK_ROT_X/PRISON_ROT_X und die `rot_x`-Zeile entfallen). K's Lage zeigt die Vorderseite (gerechnet §4.6:
  g_Welt = (0,0,-1) vor der Wand z = -3400); integration_r38_cursor E/F (10F0 Cut 0+5, 10F1 Cut 0+6) prueft das danach
  an der exe. `gen/feuerloescher_r37.inc`: K aendert es nicht -> die neu gewickelte Fassung dieses Zweigs gilt; erzeugt
  K es spaeter neu, dann mit dem Werkzeugstand dieses Zweigs (§3b).
* **Spur F (r38/dining1141):** Herz-Schluessel (re15_r38f_key_md1, flach, Umlauf-Normale +Y) liegt ungewendet auf dem
  Boden von ROOM1011 wie in RE2 ROOM2020 @0x01D90 -> Vorderseite oben, mit dem Verwurf sichtbar (§4.6). Nicht in die
  Normalen-Pruefung von unit_r38_cursor_wicklung aufnehmen (flach); falls gewuenscht, dort `port_dazu(...,
  re15_dining_r38_md1_bytes(0x1011, &n))` — der Teil erkennt es als flach.
* Die uebrigen laufenden Spuren fuegen keine Weltmodelle hinzu (git diff je Zweig auf main.c/gen/*.inc, 2026-10-17).
