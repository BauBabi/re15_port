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
* **ROOM11F0 Cut 10:** 85 Punkte an den Kippschaltern (Platz 28/29 hatten je 2 Rueckseiten-Dreiecke); mittlere
  Abweichung der Schalterzeile 1 zum Original 12,90 -> 12,61 (naeher am Original).
* **ROOM1150 Cut 4 (Hebetisch-Sicht):** 747 Punkte. `A_belege/nebenwirkung_1150_cut4_alt_neu.png`: alt lag ein
  dunkelbraunes Band (Rueckseiten von Prop 0 = Plattform mit den Papierstapel-Hochhaeusern, im Band-Fenster 54
  Dreiecke mit NCLIP < 0, RE15_TRILOG) quer ueber Hochhaeusern und Hintergrund; neu sind dort Hochhaeuser/Hintergrund
  zu sehen. Prop 0 ist ein Original-Objekt (ROOM1150 Obj_model_set), das Original zeichnet es durch dieselben
  Zeichner FUN_800254a0/FUN_800256b0 mit demselben Verwurf (§2.1).
* Port-eigene Props (dokumente_r34, feuerloescher_r37, granate_1150, inhalt_r35, irons_tisch_1150, kartenitems_r36,
  schluessel_r36, sicherung_1150) und alle Panel-/Hebetisch-Riegel: §5 (Bild-Riegel ihrer Spuren laufen mit).

## 5. Tests

Neu (Registrierung NUR `tests/unit/probes/r38_cursor.cmake`; Sonde `tests/unit/probe_r38_cursor.c`, exe-Haken
`tests/integration/test_r38_cursor.cmake`):

| Test | misst | Gegenprobe alter Stand |
|---|---|---|
| unit_r38_cursor_nclip | Praedikat: MAC0 = psx-spx NCLIP woertlich; -100 verworfen, +100 und 0 (kollinear) gezeichnet; Saettigung kippt (0,0)(3000,10)(1500,6) von +3000 auf -4092; Zaehler | - (Mechanik-Einheit) |
| unit_r38_cursor_kasten | ROOM11F0-Cursor unter Cut 10 an 25 Lagen (+-4000 um den Start): 29 Muster-Vierecke oben gezeichnet, 4 Muster-Dreiecke unten verworfen | im Test: ohne Verwurf ueberdeckten an allen 25 Lagen 4 Muster-Dreiecke den Cursor, an der Startlage ganz im Oberseiten-Umriss |
| unit_r38_cursor_raeume | 23 Cursor-Raeume: Record am Zensus-Offset, Modell aus der Prop-Tabelle, 29+4 Muster-Flaechen, Unterseite gegensinnig gewunden (gleichsinnig mit 11F0 oben) | - (Zensus) |
| integration_r38_cursor | echte exe, 4 Laeufe: A 11F0 Cut 10, B 1150 Hebetisch (Lade-Weg), C 10D0 Ziffernfeld, D Elza 11F1. Je: RE15_TRILOG des Cursor-Platzes ohne NCLIP < 0, Rahmen-Inneres ohne reingruene Punkte; A/D zusaetzlich Klassenkarte = Original | exe 1bcbd0ff mit demselben Skript: A ROT (8456 Rueckseiten-Dreiecke, 33 Punkte, 21 Abweichungen); Einzelpruefung B 1456/33, C 2997/32, D 14056/33/21 |

Gezielter Lauf (keine volle Suite), Regex = eigene Tests + Testgruppen aller beruehrten Module (Raum-Prop-Schleife
-> alle Bild-Riegel mit Raum-Objekten; Hebetisch; Panels/Cursor; Port-eigene Props):
`ctest --test-dir re15_port/build -R "r38_cursor|r34n_b|hebetisch|11f0|cursor|panel|codepanel|irons_tisch|sicherung|dokumente|schrift1150|kartenitems|feuerloescher|schluessel|kiste|tor_hell|cut1150|szene1150|pri_schatten|pri1100|granate|dachkiste|fahrstuhl|ziel1190|cut10f0|elza_irons|pri_masken|generator|zeiger" --timeout 900 --output-on-failure`

(Ergebnis: siehe unten, wird nach dem Lauf eingetragen)

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
* Keine neuen Assets, keine Sprachdateien, keine Bedienaenderung, keine Entscheidungsfrage. Neue Engine-Datei
  `engine/src/nclip_r38.c` (Android: release/build_android.sh verwirft den GLOB-Cache bereits).
