# Runde 30 — Thema F: Karte, drei Nutzer-Marken + Verlust beim Laden

Stand: master 437905cb (v0.8.15). Phase: ERMITTLUNG, kein Bau. Nichts an `engine/`,
`platform/`, `include/`, `shared_assets/` veraendert; keine git-Schreiboperation.
`git diff --stat 8d83a025 HEAD -- re15_port/` nennt nur Sonden und Werkzeuge — die
Messungen der beiden Vorgaenger (Stand 8d83a025) gelten fuer HEAD unveraendert.

Dieses Dossier fuehrt ZWEI abgebrochene Vorgaenger-Staende und die Fortsetzung zusammen.
Der Entwurf des ersten Vorgaengers liegt unveraendert unter
`build/r30_karten-marken/f3/karten-marken_entwurf_vorgaenger.md`.

Alle Abzuege stammen aus dem echten Renderpfad der gebauten `re15_pc.exe`
(`re15_port/build_r30_karten-marken/platform/pc/`), CPU-Raster `s_fb5` ueber
`RE15_INV_FB_SHOT` bzw. `RE15_MAP_SHOT_SWEEP` — nicht `RE15_AUTOSHOT`, nicht
`RE15_SOFTWARE_RENDER`, kein gdigrab.

---

## 0. Kurzfassung

| # | Befund des Nutzers | nachgestellt | Ursache (belegt) |
|---|---|---|---|
| a | ROOF: "die Wand unten blau" | 0 von 76800 Punkten Abweichung zu seinem Abzug | Tabellenzeile `s_map_walls[2]` (`re15_map_zones.h:614`) liegt auf der GEMALTEN Suedwand (35/35 Texel Index 4) und wird als deckendes FILL in der festen Farbe `re2_ton` = (16,64,176) gezeichnet (`re15_inv_screen.c:2164`, `:522-527`) |
| b | 2F: "unten eine Tuer, die es nicht gibt" | 0 von 76800 | Marke 55 (`re15_map_zones.h:470`) gehoert zur Tuer ROOM1090 -> ROOM1100 (`ROOM1090.RDT` @0x0213A), traegt aber die Rueckfall-Nummer zid 0 = ROOM1000 Zone 0 und rect 255; das Gatter `re15_map_zones.c:821-823` laesst `UNMAPPED` durch |
| c | 1F: "ROOM 1000 ist blau" | 13 von 76800 (nur der pulsierende Spielermarker) | die Kachel IST ROOM1000 Zone 0 (Ostraum), ein Schema-Kasten des Ports `s_map_synth[0]` = (207,89) 15x33; Fuellung deckend (`abe = 0`, `re15_inv_screen.c:2621`) in `re2_ton` = (16,64,176) (`:2630`) |
| d | "Teile der Karte nach dem Laden verloren" | an seiner Karte gemessen: 4 von 20 Orten ohne Zeichnung | `s_visited_floor[16]` (Etagen-Bits) steht nicht im Spielstand; `visited[32]` (Zonen-Bits) geht verlustfrei durch (102 von 102 Orten) |

Die im Auftrag genannte Spur "die Kachel traegt Index 12/13/14, der Port hat nur Eintrag 1
zurueckgestellt" ist **widerlegt** (§2.4): an beiden blauen Stellen traegt die Kartenkunst
Index 4 bzw. Index 0, die Indizes 12/13/14 kommen in KEINEM Raum-Rechteck der 13 Blaetter
vor, und RE2s Wert 0xD902 steht in RE1.5s ganzer TEX.TIM-CLUT 0-mal. Das Blau kommt nicht
aus einer Palette, sondern aus einer festen Zahl im Code.

---

## 1. Symptom / Auftrag (woertlich, AUFTRAG.md Abschnitt F mit Nachtrag)

> 3 Marker gesetzt: Roof ist irgendwie die Wand unten blau... 2F ist jetzt unten eine Tuer
> eingezeichnet auf der Karte die es nicht gibt.... und ROOM 1000 ist irgendwie jetzt blau
> eingezeichnet.... Ausserdem glaube ich, das wenn das spiel Gespeichert und dann geladen
> wird, Teile der Karte die ich bereits freigeschaltet habe verloren gegangen sind....

Beweisstuecke des Nutzers: `analysis/befunde_runde30/nutzer_marken/` (README.md, drei PNG
960x720, `re15_card_nutzer_2026-09-27.mcr`, `befund_auszug_2026-09-27.log`).

| Abzug | Blatt | gemessen (Werkzeug `kartenabzug_tools/r30_abzug_messen.py`, Ausgabe `build/r30_karten-marken-abzug/nutzer_messung.txt`) |
|---|---|---|
| `befund_1070_F233_marke1.png` | 5 ROOF | 315 Punkte (16,64,176), 960er x 444..548 y 465..467 = 320er x 148..182 y 155 (35 Punkte); Wandgrau (176,176,176) 1422 Punkte |
| `befund_1070_F259_marke1.png` | 3 2F | 7 gelbe Marken (224,168,40); eine davon 960er x 564..566 y 534..548 = 320er (188, 178..182), ringsum nur Panel |
| `befund_1070_F310_marke2.png` | 2 1F | 3627 Punkte (16,64,176), 960er x 624..662 y 270..362 = 320er x 208..220 y 90..120 (403 Punkte); aktueller Raum (48,8,48) |

Alle 76800 3x3-Felder jedes Abzugs sind einfarbig — die Abzuege sind eine reine
Verdreifachung des 320x240-Bildspeichers, der Punktvergleich ist also exakt.

**Alle drei Marken stammen aus einem GELADENEN Stand.** Beleg im Log des Nutzers
(`befund_auszug_2026-09-27.log`): neuer Log-Kopf Zeile 43, erste Datenzeile
`F30 R1070 C2 hp=85 pos=(15392,0,6583) rot=480` — das ist bis auf die Stelle der Block 3
der Karte (§2.5: Raum 1070, cut 2, pos (15392,0,6583), rot 480, hp 85). Die Sitzung beginnt
IN ROOM1070, ohne Weg dorthin. Marke 1 (Zeile 75, F233) liegt in dieser Sitzung; die Marken
F259/F310 (Zeilen 200/225) liegen nach einem weiteren Neustart (Log-Kopf Zeile 166), wieder
`pos=(15392,0,6583) rot=480 hp=85`. Dazwischen (Log-Kopf Zeile 129) wurde einmal Block 2
geladen (`hp=40 pos=(15392,0,6276) rot=4169`).

---

## 2. Messung im Port

### 2.1 Nachstellen mit dem Stand des Nutzers

Lauf (erster Vorgaenger, Ausgaben `build/r30_karten-marken/n1_slot2_{1F,2F,ROOF}.*`):
eigene exe `re15_port/build_r30_karten-marken/platform/pc/re15_pc.exe`, die Karte des
Nutzers als `re15_card.mcr` daneben, frischer Prozess,

```
RE15_NOAUDIO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=2
RE15_INV_OPEN_AT="60#1070" RE15_INPUT_SCRIPT="W40,U0.1[,W2,U0.1,W2,U0.1],W60"
RE15_INV_FB_SHOT=<datei.bmp> RE15_INV_FB_SHOT_AT=<n>
```

Das ist der ECHTE Weg Titel -> LOAD GAME -> Slot: `RE15_CONTINUE_TEST` setzt in der
Titelschleife den Cursor auf 1 und drueckt Kreuz (`platform/pc/main.c:2965-2968`), der
Zweig `cursor == 1` ruft `pc_run_memcard_screen(0, NULL, &resume_room)` (`main.c:3057-3061`),
dort bestaetigt `RE15_CARD_AUTO` den per `RE15_CARD_SLOT` gewaehlten Platz
(`main.c:1557-1571`); eingespielt wird mit `re15_savedata_restore` (`main.c:3991`).
Die Tasten im Titel/Kartenschirm kommen also aus den Testhaken, das Blaettern der
Kartenseiten aus `RE15_INPUT_SCRIPT`. Beleg im Lauf: `n1_slot2_2F.debug.log:38`
`[save] CONTINUE: resumed in room 1070 (hp=85)`.
Die benutzte Karte `build/r30_karten-marken/nutzerkarte_kopie_0927.mcr` ist bytegleich zu
`nutzer_marken/re15_card_nutzer_2026-09-27.mcr` (`cmp`).

Punktvergleich (`kartenabzug_tools/r30_abzug_vergleich.py`, in der Fortsetzung neu gefahren):

| Blatt | eigener Abzug | Abzug des Nutzers | abweichende Punkte (320x240) |
|---|---|---|---|
| ROOF | `n1_slot2_ROOF.bmp` | `befund_1070_F233_marke1.png` | **0 von 76800** |
| 2F | `n1_slot2_2F.bmp` | `befund_1070_F259_marke1.png` | **0 von 76800** |
| 1F | `n1_slot2_1F.bmp` | `befund_1070_F310_marke2.png` | **13 von 76800**, alle in x 177..181 / y 125..129, eigen (176,168,0) gegen Nutzer (88,88,0) = der pulsierende Spielermarker |

Der Stand des Nutzers ist damit punktgenau nachgestellt. VERWORFEN sind die Abzuege
`build/r30_karten-marken-abzug/lauf_2F.bmp` und `lauf_ROOF.bmp` des zweiten Vorgaengers: sie
zeigen das falsche Blatt (6739 bzw. 6822 Punkte Abweichung); sein `lauf_1F.bmp` stimmt
(13 Punkte).

### 2.2 Welche Zeichen-Operation traegt das Blau (Sonde, Abschnitt I)

Sonde `re15_port/tests/unit/probe_r30_karten-marken_rundlauf.c`, Ausgabe
`build/r30_karten-marken/f3/rundlauf_ergebnis.txt`. Sie baut je Blatt die Op-Liste des
echten Zeichners (`re15_inv_screen_build`) und sucht FILL-Ops mit rgb (16,64,176).

Stand des Nutzers (Karte geladen, Platz 2):

```
Blatt  2 Op 118: FILL (207,89) 15x33 abe 0 rgb (16,64,176) -> SCHEMA-ZELLE
Blatt  5 Op 102: FILL (148,155) 35x1 abe 0 rgb (16,64,176) -> LINIE (Innenwand)
SUMME Nutzer-Karte geladen: 2 blaue FILL-Ops
```

Alles begangen, alle 13 Blaetter:

```
Blatt  2: FILL (207,89) 15x33 / (207,123) 15x16 / (207,139) 15x15      SCHEMA-ZELLE x3
Blatt  3: FILL (188,122) 1x23 / (189,133) 24x1                          LINIE x2
Blatt  5: FILL (148,155) 35x1                                           LINIE x1
Blatt  8: FILL (127,78) 5x1 / (134,78) 3x1                              LINIE x2
Blatt  9: FILL (213,107) 9x1 / (177,116) 45x1 / (213,135) 9x1 /
               (195,119) 1x8 / (213,143) 9x1                            LINIE x5
SUMME alles begangen: 13 blaue FILL-Ops, alle abe 0
```

Das sind genau die 3 Zeilen von `s_map_synth_cells` und die 10 Zeilen von `s_map_walls`.
Gegenprobe am BILD (`kartenabzug_tools/r30_blau_zensus.py` ueber `sweep1150_00..12.bmp`,
Ausgabe `f3/zensus_sweep1150.txt`): 914 Punkte (16,64,176) auf den Blaettern 2 (754),
3 (37), 5 (35), 8 (8), 9 (80), auf allen anderen 0. 754+37+35+8+80 = 914; die Differenz
zu den 1130 Punkten Op-Flaeche sind die Schema-Raender und Tuermarken, die darueber liegen.

### 2.3 Messwerte an den drei Stellen

| Befund | Blatt | Messstelle | gemessen |
|---|---|---|---|
| a | 5 | Zeile y=155, x=148..182 | 35 von 35 Punkten (16,64,176); Zeile 154 = Raumkoerper (16,56,40); Zeile 156 = Panel (0,16,88) |
| a | 5 | Nordwand y=101 | (176,176,176) — so sieht die gemalte Wand aus |
| b | 3 | x=188, y=178..182 | 5 Punkte (224,168,40), ringsum nur Panel (0,16,88)/(0,16,120) |
| c | 2 | Kasten (207,89) 15x33 | 403 Punkte (16,64,176) + 92 Randpunkte (176,176,176) |
| c | 2 | alle gemalten Kachelkoerper des Blatts | (16,56,40) und (16,56,56) |

Spieler IN ROOM1000 (`sweep1000_02.bmp`): Schema-Kasten deckend (104,8,8); ein aktueller
Raum mit gemalter Kachel misst (48,8,48)/(48,8,64). Die Schema-Zeichnung weicht also in
BEIDEN Zustaenden von der Kachel ab.

### 2.4 Spur "Index 12/13/14" — WIDERLEGT

Werkzeuge `kartenabzug_tools/r30_clut_zeilen.py` und `r30_texel_an_marke.py`, Ausgaben
`f3/clut_zeilen.txt`, `f3/texel_an_marke.txt`.

**Texel der Kartenkunst an den Marken** (`DATA/MAPxx.PIX`, headerlos 256x256 4bpp, unteres
Nibble = linker Texel; Rechtecktabellen aus `info/Re1.5/PSX.EXE` @0x80076840 + 8*Seite):

| Stelle | Rechteck | Texel | Index |
|---|---|---|---|
| ROOF y=155 x=148..182 | rect 1 @0x800764C8 (148,101) 48x56 uv(0,32) | `MAP06.PIX` Zeile v=86 ab @Datei 0x2B00 | 35 x Index **4** (Wandlinie) |
| ROOF y=154 | dasselbe | v=85 ab @0x2A80 | 33 x Index 1, 2 x Index 4 |
| ROOF y=156 | dasselbe | v=87 ab @0x2B80 | 35 x Index 0 |
| 1F x 208..220 y 90..120 | rect 0 @0x8007636C und rect 5 @0x800763A8 | `MAP03.PIX` ab @0x1A8E bzw. @0x2FC6 | 124 + 13 Texel, alle Index **0** (nichts gemalt) |
| 2F (188, 178..182) | Ersatzzeile `s_map_rectfix[2]` (135,155) 56x32 | `MAP04.PIX` | Index 0 |

**Indizes 12/13/14 in den Raum-Rechtecken aller 13 Blaetter: 0 / 0 / 0.** Sie kommen nur
ausserhalb vor (je Blatt 72 / 16 / 75 Texel, Blatt 6: 201 / 44 / 230) = die Legendenleiste.

**Die Palettenzeilen:**

| Zeile | Eintrag 1 | Eintrag 4 | Eintrag 12 | Eintrag 13 | Eintrag 14 |
|---|---|---|---|---|---|
| RE1.5 `TEX.TIM` Zeile 21 (@Datei 0x0554) | @0x0556 `0x81A4` (32,104,0) STP 1 | @0x055C `0x5AD6` (176,176,176) | @0x056C `0x55A8` (64,104,168) | @0x056E `0x3505` (40,64,104) | @0x0570 `0x1476` (176,24,40) |
| RE2 `ST0.TIM` k=8 (@0x10934) | @0x10936 `0x0000` | @0x1093C `0x4631` (136,136,136) | @0x1094C `0x0000` | @0x1094E `0x0000` | @0x10950 `0x0000` |
| RE2 k=11 (@0x10994) | @0x10996 `0xD902` (16,64,176) STP 1 | @0x1099C `0x4631` | @0x109AC `0xD902` | @0x109AE `0xD902` | @0x109B0 `0xD902` |
| RE2 k=12 (@0x109B4) | @0x109B6 `0x842D` (104,8,8) STP 1 | @0x109BC `0x4631` | @0x109CC `0x842D` | @0x109CE `0x842D` | @0x109D0 `0x842D` |

* RE2s drei Zeilen unterscheiden sich in GENAU {1,12,13,14} (bestaetigt); 12/13/14 tragen
  dort denselben Wert wie Eintrag 1.
* RE1.5s Eintraege 12/13/14 sind Legendenfarben (Hellblau, Dunkelblau, Rot), kein Zustand.
  `0xD902` kommt in der ganzen TEX.TIM-CLUT (32x24 Eintraege) **0-mal** vor.
* Die Zustandszeilen des Ports (`platform/pc/src/inv_render_pc.c:171-182`,
  `karten_cluts_bauen`) sind Kopien von RE1.5s Zeile 21, in denen NUR Eintrag 1 getauscht
  wird (`RE15_KARTE_BESUCHT 0x81A4`, `RE15_KARTE_AKTUELL 0x842D`, `RE15_KARTE_UNBESUCHT
  0x0000`). Ein Kachel-Blit kann deshalb gar kein (16,64,176) erzeugen.
* Halbtransparent ueber dem Panel ergaebe 0xD902 ausserdem (8,40,128)/(8,40,144), nicht das
  gemessene reine (16,64,176) — der gemessene Wert ist DECKEND gezeichnet.

Ergebnis: Das Blau stammt aus `re2_ton` (`re15_inv_screen.c:522-527`), einer festen
RGB-Zahl, die beim Zurueckstellen der Kacheln auf RE1.5-Gruen (Commit 9a75fccb, geaendert
wurde nur `include/re15_inv_screen.h:176`) stehen blieb.

### 2.5 Die Karte des Nutzers

Werkzeug `kartenabzug_tools/r30_karte_lesen.py`, Ausgabe
`build/r30_karten-marken-abzug/karte_nutzer.txt`; Bytes in der Fortsetzung mit `xxd`
nachgelesen (@0x06100 `52 45 31 35 08 00 00 00`, @0x06464 `41 55 14 15 50 c5 04 50`,
@0x06484 `2d 11 00 00`).

| Block (Platz) | Datei-Offset | Version | Raum | Besucht-Bits | Pruefsumme |
|---|---|---|---|---|---|
| 1 (0) | 0x02100 | 8 | ROOM1150 | 4 | 0x0A8A OK |
| 2 (1) | 0x04100 | 8 | ROOM1070 | 13 | 0x0A45 OK |
| 3 (2) | 0x06100 | 8 | ROOM1070, pos (15392,0,6583) rot 480 hp 85 | **20** | 0x112D OK |
| 5 (4) | 0x0A100 | 8 | ROOM1150 | 7 | 0x0DD7 OK |

`visited[32]` liegt bei Struktur +0x364 (= Datei 0x06464 in Block 3), die Pruefsumme bei
+0x384. Schluessel der Bits: `zone_bit` (`re15_map_zones.c:106-127`) = 2 x Platz des
Basisraums in `re15_room_ids[]` + (Zone >= 1), Zusatzbits 240/241.

Block 3, die 20 Bits: ROOM1000/z0, 1030, 1040, 1050, 1060, 1070, 1090, 10A0, 10C0, 10D0,
10E0, 1120, 1130, 1140, 1150, 1170/z0, 1170/z1, 1190, 11E0, 11F0.

**Was die Karte nach dem Laden zeigt** (Sonde Abschnitt G: Karte laden -> `restore` im
Null-Zustand -> erstes Zonen-Update in ROOM1070):

* `visited[]` nach dem Laden bytegleich zum Block: ja, 20 Bits.
* Etagen-Bits nach dem Laden: **0 von 50** Zeilen.
* 16 der 20 Orte werden gezeichnet. **4 Orte haben nach dem Laden KEINE Zeichnung mehr**,
  obwohl ihr Besucht-Bit gesetzt ist:

| Ort | Bit | Zeilen des Ortes (Blatt/rect) | Zustand nach dem Laden | fehlendes Etagen-Bit |
|---|---|---|---|---|
| ROOM1060 (Treppenhaus) | 12 | HAUPT 2/10, GAST 3/1, GAST 4/1 | unbesucht x3 | Zeilen 0 (Band 0), 2 (Band 4), 4 (Band 8) |
| ROOM1090 | 18 | HAUPT 2/5, GAST 3/7 | unbesucht x2 | Zeilen 12 (Band 1), 14 (Band 6) |
| ROOM10A0 | 20 | HAUPT 2/6, GAST 1/9 | unbesucht x2 | Zeilen 18 (Band 8), 16 (Band 1) |
| ROOM1170/z1 | 47 | HAUPT 5/0, GAST 4/3 | unbesucht x2 | Zeilen 26 (Band 4), 24 (Band 0) |

Gezeichnete Rechtecke je Blatt nach dem Laden: Blatt 0: r2; Blatt 1: r0; Blatt 2: r0, r2,
r3 (aktuell), r4 + Schema ROOM1000/z0; Blatt 3: r0, r3, r8; Blatt 4: r2, r4, r5, r6;
Blatt 5: r1.

Im ROOF-Abzug des Nutzers ist die Folge zu SEHEN: die Treppenmarke (drei weisse Striche bei
(156,97)) steht frei ueber dem Landeplatz, weil das kleine Rechteck rect 0 (ROOM1170/z1)
fehlt. Der Nutzer hat das nicht eigens gemeldet; es gehoert zu Befund d.

### 2.6 Rundlauf je Ort (Sonde Abschnitt F)

Jede der 102 Haupt-Zonenzeilen (Leon) EINZELN: Null-Zustand -> den Ort auf jedem seiner
Baender begehen -> Spieler in den neutralen Speicherraum ROOM1150 (fuer ROOM1150 selbst:
ROOM1130) -> `re15_savedata_capture` -> `re15_memcard_save` -> Null-Zustand ->
`re15_memcard_load` -> `re15_savedata_restore` -> Vergleich.

```
SUMME: 102 Orte, 0 ohne treffbaren Punkt, 91 verlustfrei, 11 mit Abweichung
       Orte mit veraendertem ZONEN-Bit: 0 | verlorene Etagen-Bits 25, Rechtecke 25, Marken 16
```

| Ort | Bit | verlorene Rechtecke (Blatt/rect) | verlorene Marken |
|---|---|---|---|
| ROOM1060/z0 | 12 | 2/10, 3/1, 4/1 | 41, 42, 43, 46, 47, 48, 56, 57 |
| ROOM1080/z0 | 16 | 2/9, 3/4, 4/0 | — |
| ROOM10A0/z0 | 20 | 1/9, 2/6 | 38, 39, 40 |
| ROOM1090/z0 | 18 | 2/5, 3/7 | — |
| ROOM10F0/z0 | 30 | 2/8, 3/9 | 54 |
| ROOM1170/z1 | 47 | 4/3, 5/0 | 59, 60, 63, 66 |
| ROOM11A0/z0 | 52 | 0/1, 6/2 | — |
| ROOM3080/z0 | 128 | 7/8 | — |
| ROOM4020/z0 | 148 | 8/2, 9/13, 10/3 | — |
| ROOM4070/z0 | 158 | 8/7, 9/1, 11/3 | — |
| ROOM50D0/z0 | 202 | 10/1, 11/0 | — |

Das sind genau die 11 Orte, die eine Etagenzeile in `s_map_floors` fuehren. Kein einziges
ZONEN-Bit geht verloren oder kommt hinzu.

### 2.7 Rundlauf ueber die ganze Stage, Gegenrichtung, Alt-Staende (Sonde des ersten Vorgaengers, Abschnitt D)

Ausgabe `build/r30_karten-marken/probe_ergebnis.txt`.

* **D1** 39 Orte der Stage 1 begangen, gespeichert in ROOM1150, im Null-Zustand geladen:
  Zonen-Bits 0 Unterschiede; Etagen-Bits 16 verloren; Rechtecke verloren 15, Marken 13.
* **D2** derselbe alte Stand in LAUFENDER Sitzung geladen: 16 Etagen-Bits zu viel —
  16 Rechtecke erscheinen, die der geladene Stand nie besucht hat.
* **D3** Alt-Stand v6 mit den Bits 15, 16, 18 (alte Bedeutung): 3 Bits importiert, Soll 0.
* Im echten Weg (exe, Speicherkarte; `s1_10C0` speichern / `l1_10C0` laden): Blatt 2F vor
  gegen nach = **2964 abweichende Punkte**, Bbox (110,67)-(223,156)
  (`vergleich_2F_vor_nach_laden.png`).

### 2.8 Marken-Zensus ueber alle Blaetter

Zwei unabhaengige Verfahren, gleiches Ergebnis.

**Am Bild** (`r30_blau_zensus.py`: gelbe Marke, deren 8er-Saum nur Panelfarbe traegt), alles
begangen: 8 frei schwebende gelbe Marken — Blatt 1: 3, Blatt 3: 1, Blatt 6: 3, Blatt 7: 1.
An den drei Abzuegen des Nutzers: genau 1 (2F, (188,178..182)).

**An den Tabellen** (Sonde Abschnitt H: Balken der Marke + 1 Punkt Saum beruehrt weder ein
bemaltes Texel eines GEZEICHNETEN Rechtecks noch eine gezeichnete Schema-Zelle; "gezeichnet"
wie im Zeichner `re15_inv_screen.c:2317-2341`), alles begangen: **10 von 186**.

| Marke | Header-Zeile | Blatt | rect | Lage | kind | zid | Klasse |
|---|---|---|---|---|---|---|---|
| 20 | 402 | 1 | 255 | (131,77) | 0 | 0 | A fremde zid |
| 23 | 405 | 1 | 255 | (139,77) | 0 | 0 | A |
| 24 | 406 | 1 | 255 | (144,80) | 1 | 0 | A |
| **55** | **470** | **3** | **255** | **(188,180)** | 3 | 0 | **A — der Befund des Nutzers** |
| 91 | 506 | 6 | 255 | (166,104) | 2 | 0 | A |
| 92 | 507 | 6 | 255 | (185,110) | 2 | 0 | A |
| 72 | 487 | 6 | 0 | (129,153) | 1 | 39 | C Lage neben der Kunst |
| 75 | 490 | 6 | 0 | (139,153) | 5 | 46 | C (Treppe, weiss — im Bild-Zensus unsichtbar) |
| 112 | 527 | 7 | 9 | (224,175) | 1 | 60 | C |
| 155 | 570 | 9 | 7 | (190,91) | 4 | 78 | C (Treppe) |

Dazu Marke 21 und 22 (Header 403/404, Blatt 1, zid 0): dieselbe Klasse A, sie beruehren
zufaellig gemalte Flaeche und zaehlen deshalb nicht als "schwebend".

Im STAND DES NUTZERS (Sonde G3) schweben 9 von 41 sichtbaren Marken: 20, 23, 24, 55, 91, 92
(Klasse A — sichtbar, weil ROOM1000/z0 besucht ist; 91/92 stehen damit auf Blatt 6,
einem Blatt der Stage 2) und 18, 19 (Header 400/401, Blatt 1, rect 9), 67 (Header 482, Blatt 5, rect 0) = Klasse B: ihr
Rechteck fehlt nach dem Laden (§2.5), das Gatter prueft ausserhalb der Blaetter 2..4 aber
nur das Zonen-Bit.

### 2.9 Zensus und Sonde des ersten Vorgaengers (Abschnitte A-C, E)

```
Zonenzeilen 232 (davon Gast-Zeilen 28), Orte (Haupt, Leon) 102
Besucht-Bits: 102 verschiedene belegt, hoechstes Bit 241, Feld fasst 256, Kollisionen 0
Etagenzeilen (s_map_floors): 50 -> 50 Bits, im Spielstand: 0 Bytes
sizeof(re15_savedata_t) = 904, offsetof(visited) = 868, offsetof(checksum) = 900
Marken 186, Innenwaende 10
Marken mit fremder zid: 9 von 186 (20..24, 55, 58, 91, 92)
VERSUCH nur ROOM1000/z0 betreten -> sichtbar: Marken 20,21,22,23,24,55,91,92
GEGENPROBE alles ausser ROOM1000/z0 betreten -> sichtbare zid-0-Marken: 0
```

Innenwaende gegen die Kachel (Texel-Index unter jeder Linie):

| Wand | Blatt/rect | Linie | Punkte | Index 0 | Index 1 (Koerper) | Index 4 (Wand) |
|---|---|---|---|---|---|---|
| 0 | 3/5 | (188,122)-(188,144) | 23 | 0 | 21 | 2 |
| 1 | 3/5 | (189,133)-(212,133) | 24 | 0 | 23 | 1 |
| **2** | **5/1** | **(148,155)-(182,155)** | **35** | **0** | **0** | **35** |
| 3 | 8/2 | (127,78)-(131,78) | 5 | 0 | 4 | 1 |
| 4 | 8/2 | (134,78)-(136,78) | 3 | 0 | 3 | 0 |
| 5 | 9/7 | (213,107)-(221,107) | 9 | 0 | 7 | 2 |
| 6 | 9/7 | (177,116)-(221,116) | 45 | 36 | 7 | 2 |
| 7 | 9/7 | (213,135)-(221,135) | 9 | 0 | 7 | 2 |
| 8 | 9/7 | (195,119)-(195,126) | 8 | 0 | 7 | 1 |
| 9 | 9/7 | (213,143)-(221,143) | 9 | 0 | 7 | 2 |

Nur Wand 2 liegt vollstaendig auf einer schon gemalten Wand.


---

## 3. Original-Mechanismus

Stichprobe der Fortsetzung: jede hier zitierte Adresse / jeder Datei-Offset wurde neu
nachgeschlagen (`re15_disasm.py`, `re2_disasm.py`, `xxd`, eigene Werkzeuge). Abweichung zu
den Angaben der Vorgaenger: keine.

### 3.1 RE1.5: der Kartenzeichner kennt KEINEN Zustand und EINE Palette

`info/Re1.5/PSX.EXE`:

```
80046fd8  addiu sp,sp,-24
80046fdc  ori   a0,zero,0x100
80046fe4  jal   0x8006b3d8            ; GetClut
80046fe8  ori   a1,zero,0x1f5         ; (256,501) = TEX.TIM CLUT-Zeile 21
8006b3d8  sll   v0,a1,6               ; Sprungziel selbst gelesen:
8006b3dc  sra   a0,a0,4               ;   clut = (y<<6) | ((x>>4)&0x3f)
8006b3e0  andi  a0,a0,0x3f
8006b3e4  or    v0,v0,a0
8006b3e8  jr    ra
...
80047314  ori   v0,v0,0x2             ; SPRT-Code |2 = halbtransparent
8004735c  sh    t4,0(v1)              ; CLUT in JEDES Rechteck-Sprite
80047380  ori   v0,v0,0x2             ; (zweiter Puffer)
800473cc  sh    t4,0(v1)
800473d8  sltu  v0,v0,a3
800473dc  bne   v0,zero,0x800472fc    ; EINZIGE Verzweigung der Schleife = Zaehler
```

Aussage: RE1.5 zeichnet jedes Rechteck der Seite immer, halbtransparent, in EINER Palette
(Zeile 21). Tuermarken, Innenwaende, Schema-Kaesten und der Besucht-Zustand sind
Port-Ergaenzungen ohne RE1.5-Vorbild. Massgeblich ist deshalb fuer die FARBE RE1.5s
Palette, fuer den ZUSTANDS-Mechanismus RE2 (Festlegung Runde 27/28,
`include/re15_inv_screen.h:157-176`).

RE1.5s Zeile 21, vollstaendig (`re15_port/shared_assets/PSX/DATA/TEX.TIM`, bytegleich zu
`info/Re1.5/PSX/DATA/TEX.TIM`; TIM-Kopf: CLUT-Block Laenge 0x60C, x=256 y=480 w=32 h=24,
Daten ab @0x14; Zeile 21 ab @0x0554):

| Eintrag | @Datei | Wert | RGB | STP | Bedeutung auf den Blaettern |
|---|---|---|---|---|---|
| 0 | 0x0554 | 0x0000 | (0,0,0) | 0 | durchsichtig |
| 1 | 0x0556 | 0x81A4 | (32,104,0) | 1 | RAUMKOERPER, halbtransparent |
| 2 | 0x0558 | 0x00E1 | (8,56,0) | 0 | |
| 3 | 0x055A | 0x0081 | (8,32,0) | 0 | |
| 4 | 0x055C | 0x5AD6 | (176,176,176) | 0 | WANDLINIE, deckend |
| 5..9 | 0x055E.. | 0x56B5 0x4610 0x358C 0x2508 0x1484 | Graustufen | 0 | Kantenglaettung / Schrift |
| 10 | 0x0568 | 0x0DB8 | (192,104,24) | 0 | Legende |
| 11 | 0x056A | 0x02B8 | (192,168,0) | 0 | Legende |
| 12 | 0x056C | 0x55A8 | (64,104,168) | 0 | Legende (Kompass/Blau) |
| 13 | 0x056E | 0x3505 | (40,64,104) | 0 | Legende |
| 14 | 0x0570 | 0x1476 | (176,24,40) | 0 | Legende (Rot) |
| 15 | 0x0572 | 0x146D | (104,24,40) | 0 | Legende |

Rechtecktabelle Blatt 5 (ROOF): Paar @0x80076840 + 8*5 -> count 2, Liste @0x800764BC;
rect 0 @0x800764BC = (140,80) 48x24 uv(192,16); rect 1 @0x800764C8 = (148,101) 48x56
uv(0,32). Kachelzeile Schirm y=155 = `MAP06.PIX` v=86:
`444444444444444444444444444444444440000000000000` — Index 4 auf x=148..182.

Massstabszeilen @0x800768B0 + 8*Slot (vom ersten Vorgaenger gelesen): ROOM1000
@0x800768B0, ROOM1090 @0x800768F8, ROOM10F0 @0x80076928, ROOM1100 @0x80076930 tragen den
Stub `00 00 00 00 01 00 01 00`; ROOM1170 @0x80076968 = (100,206,2280,2268).

### 3.2 RE2: Zustand je Kachel ueber die CLUT-ZEILE, Wandlinie zustandsfrei

`info/re2leon/PSX.EXE`:

```
8006e614  addiu s5,zero,501          ; Grundzeile = BESUCHT
8006e648  addiu s5,s5,1              ; AKTUELLER RAUM = 502
8006e660  lbu   a1,-21955(at)        ; Karten-Flagbit
8006e668  addiu a0,a0,18724          ; 0x800D4924 = Bank 33 (Karte vorhanden)
8006e66c  jal   0x80077360           ; Bit-Test
8006e674  beq   v0,zero,0x8006e730   ; ohne Karte
8006e67c  lbu   a1,12(s1)            ; Kachel-Record +0x0C = Besucht-Bitnummer
8006e688  jal   0x80077360
8006e68c  addiu a0,a3,-24            ; 0x800D4924-24 = 0x800D490C = Bank 9 (besucht)
8006e690  bne   v0,zero,0x8006e74c
8006e71c  addiu s5,zero,498          ; Karte + unbesucht
8006e744  beq   v0,zero,0x8006e768   ; unbesucht ohne Karte -> gar nicht zeichnen
8006e74c  addiu a0,zero,256
8006e750  jal   0x8008f828           ; GetClut(256,s5)
8006e754  addu  a1,s5,zero
```

Die drei Zeilen aus `info/re2leon/COMMON/DATA/ST0.TIM` (zweites TIM @0x10820, CLUT-Kopf
x=256 y=480 w=16 h=21, Daten ab @0x10834) stehen in der Tabelle §2.4. Aussage: die
WANDLINIE (Eintrag 4) ist in allen drei Zustaenden bitgleich `0x4631`; den Zustand tragen
die Eintraege 1, 12, 13, 14 — und RE2s Kunst benutzt 12/13/14 als Raumflaeche, RE1.5s
Kunst nicht (§2.4: 0 Texel in den Rechtecken).

Die Zuordnung "CLUT-Y 498/501/502 = Zeilenindex 8/11/12" (Verschiebung um 490) stammt aus
Runde 27 (`inv_render_pc.c:139-142`: Cursor `addiu v0,zero,2587` @0x80068588,
`addiu v0,v0,480` @0x80076B08) und wurde hier NICHT neu gemessen; gestuetzt wird sie
dadurch, dass genau diese drei Zeilen sich in genau {1,12,13,14} unterscheiden.

### 3.3 RE2: Besucht-Bit wird im Raumlader gesetzt, und ALLES liegt im Spielstand

Setzer (`re2_disasm.py dis 0x80069384`):

```
80069384  lui   s0,0x800d
80069388  addiu s0,s0,18462          ; s0 = 0x800D481E (Raumnummer)
8006938c  lh    a1,0(s0)
80069390  addiu a0,s0,238            ; 0x800D481E + 0xEE = 0x800D490C = Bank 9
80069394  jal   0x8007730c           ; Bit SETZEN
80069398  addu  a1,v1,a1             ; Bit = Stage-Basis + Raum
800693b0  addiu a0,s0,234            ; 0x800D4908 = Bank 35
800693b4  jal   0x8007730c
```

Bank-Zeiger, in der Fortsetzung aus der Tabelle @0x800A78C8 gelesen: Bank 9 @0x800A78EC
(Datei 0x980EC) = 0x800D490C; Bank 32 @0x800A7948 = 0x800D4920; Bank 33 @0x800A794C =
0x800D4924; Bank 35 @0x800A7954 = 0x800D4908.

Laden (`info/re2leon/COMMON/BIN/MEM_CARD.BIN`, Werkzeug
`analysis/befunde_runde30/r30_karten_re2_memcard_dis.py`; Bytes @Datei 0x13D0 mit `xxd`
nachgelesen: `0d 80 10 3c  a4 44 10 26  21 20 00 02  21 28 20 02  de 41 00 0c  98 07 06 24`):

```
801c0de8  lui   s0,0x800d            ; Datei 0x13D0
801c0dec  addiu s0,s0,17572          ; s0 = 0x800D44A4
801c0df0  addu  a0,s0,zero           ; Ziel   = 0x800D44A4
801c0df4  addu  a1,s1,zero           ; Quelle = Kartenpuffer
801c0df8  jal   0x80010778
801c0dfc  addiu a2,zero,1944         ; 0x798 Bytes
80010778  addu a2,a1,a2 / lw t0,0(a1) .. lw t3,12(a1) / sw t0,0(a0) / addiu a1,a1,16 / ...
```

Block = [0x800D44A4, 0x800D4C3C). Alle vier Baenke, die der Kartenzeichner liest
(0x800D4908, 0x800D490C, 0x800D4920, 0x800D4924), liegen darin.
Aussage: in RE2 wird JEDES Bit, das der Kartenzeichner liest, mit dem Spielstand
zurueckgeholt. RE2 kennt dabei KEINE Etagen-Bits — die sind eine Port-Ergaenzung
(Nutzer-Befunde 2026-09-01 und 2026-09-12). Uebertragbar ist der GRUNDSATZ, nicht das Feld.

### 3.4 Der Tuer-Datensatz hinter der schwebenden Marke

`re15_port/shared_assets/PSX/STAGE1/ROOM1090.RDT`, mainScd @0x2118, Datensatz @0x0213A
(`xxd` nachgelesen):

```
3b 01 02 31 06 00 10 e1 40 ac 68 10 7c 15 5c ae 00 00 a0 d7 00 04 00 10 05 00 00 00 00 00 00 00
```

| Feld | Bytes | Wert |
|---|---|---|
| Opcode | `3b` | Door_aot_set |
| Slot | `01` | Tuer #1 |
| sce / sat | `02` `31` | |
| Band | `06` | 6 = obere Ebene (Etagenzeile `{0x1090, 0, 6, 3, 7}`, `re15_map_zones.h:661`) |
| Trigger | `10 e1` `40 ac` `68 10` `7c 15` | x -7920, z -21440, w 4200, d 5500 -> Mitte (-5820,-18690) |
| Ankunft | `5c ae` `00 00` `a0 d7` | (-20900, 0, -10336) |
| Yaw | `00 04` | 1024 |
| Ziel | `00` `10` `05` | Stage 1, Raumindex 0x10 = ROOM1100, cut 5 |

Herkunft der Marke im Generator (schreibgeschuetzt gefahren,
`analysis/befunde_runde30/r30_karten_gen_diag.py`, Ausgabe
`build/r30_karten-marken/gen/marken_herkunft.json`):
`{"room":"1090","zi":0,"idx":1,"pg":3,"r":255,"mx":188,"my":180,"zid":0,"zid_fehlt":true,
"d":{"slot":1,"lx":-5820,"lz":-18690,"band":6,"dest":"1100"}}`.

---

## 4. Ursache

### 4a. ROOF — "Wand unten blau": zwei Defekte uebereinander

1. **Die Tabellenzeile ist keine Innenwand.** `re15_map_zones.h:614`
   `{ 5, 1, 148, 155, 182, 155, 23 }` liegt mit 35 von 35 Punkten auf Kachel-Index 4 — der
   vom Kuenstler GEMALTEN Suedwand von rect 1; die Zeile darunter ist Index 0 (nichts).
   Der Tabellenkopf verlangt "nur wo BEIDSEITS Raum liegt".
   Entstehung, aus dem Generator-Code abgeleitet (der Einzelfall ist NICHT Schritt fuer
   Schritt nachgerechnet): der Generator nimmt eine Wandzelle, sobald sie zwei
   Selbst-Tuer-Spawns trennt (`tools/gen_map_zones.py:4608-4626`); die im Kommentar
   `:4503-4506` beschriebene Aussenwand-Regel wird danach nicht mehr geprueft
   (`_aussen = False` `:4627`, die Abfrage `:4642` kann nie greifen). ROOM1170 fuehrt drei
   Selbst-Tueren (`ROOM1170.RDT` @0x01206 Ankunft (-11710,-7200,-26500); @0x012CC und
   @0x0135A Ankunft (2300,-7200,14365)), also zwei Spawns, zwischen denen Wandzellen
   liegen. Eine Neuerzeugung bringt die Zeile unveraendert wieder
   (`build/r30_karten-marken-abzug/gen_kopie/re15_map_zones_REGEN.h:449`, dort mit der
   Generator-Nummer zid 21; Lauf-Meldung `regen.log:384` "10 Innenwand-Linien ... 5688
   Wandzellen geprueft: 10 innen, 5523 aussen").
2. **Die Farbe ist eine feste Zahl, und zwar RE2s.** `re15_inv_screen.c:2164` faerbt
   Innenwaende mit `re2_ton(zustand)`; `re2_ton` (`:522-527`) liefert fuer BESUCHT
   (16,64,176). Eingefuehrt mit 72d6f7ba (2026-09-27 01:48); 37 Minuten spaeter stellte
   9a75fccb die KACHELN auf RE1.5s Gruen zurueck — geaendert wurde nur
   `include/re15_inv_screen.h:176`, `re2_ton` blieb stehen. Der Riegel
   `unit_karte_besitz` prueft die Defines gegen die Dateien, aber nicht, ob der Zeichner
   sie benutzt.

Die Op (Blatt 5 Op 102, FILL (148,155) 35x1, abe 0) liegt ueber der gemalten grauen Wand
und ueberdeckt sie.

### 4b. 2F — "Tuer, die es nicht gibt": Marke 55 haengt an einem fremden Besucht-Bit

`re15_map_zones.h:470` `{ 3, 255, 188, 180, 3, 0, 255, 0 }`.

* **Wem sie gehoert:** Tuer #1 von ROOM1090, obere Ebene, nach ROOM1100 (§3.4).
* **Warum zid 0:** `gen_map_zones.py:3561` `'zid': zid_of.get((b, zi), 0)`. ROOM1090 kennt
  der Generator nicht (die Zeilen `re15_map_zones.h:134` und `:332` stehen seit Runde 10
  von Hand im Header), also Rueckfall 0. 0 ist aber eine GUELTIGE Nummer: ROOM1000 Zone 0
  (`re15_map_zones.h:25`, Blatt 2).
* **Warum sichtbar:** `re15_map_zones.c:821-823`
  `if (zid_besucht(m->zid) && re15_map_rect_state(page, rect) != RE15_MAP_RECT_UNVISITED)`.
  `zid_besucht(0)` = Bit 0 = ROOM1000/z0 — in Block 3 der Nutzer-Karte GESETZT
  (`visited[0] = 0x41`, @Datei 0x06464). Fuer (Blatt 3, rect 255) gibt es keine Zonenzeile,
  `re15_map_rect_state` (`:365-441`) liefert `UNMAPPED`, und das Gatter sperrt nur
  `UNVISITED`. Der Kommentar `:809-812` begruendet das mit "UNMAPPED-Rechtecke werden IMMER
  gemalt" — der Zeichner tut das Gegenteil: `re15_inv_screen.c:2341`
  `if (rs == RE15_MAP_RECT_UNMAPPED && !re15_map_stock_mode()) continue;`.
  Gemessen (erster Vorgaenger): nur ROOM1000/z0 betreten -> Marke 55 sichtbar; alles
  AUSSER ROOM1000/z0 betreten -> unsichtbar.
* **Warum "im Nichts":** (188,180) liegt im Kasten der Ersatzzeile `s_map_rectfix[2]`
  (135,155) 56x32, deren Kachel dort Index 0 traegt; alle anderen Rechtecke des Blatts
  liegen ausserhalb. Die Lage stammt aus einem Grundriss-Kasten des Loesers, den es im
  Header nicht mehr gibt. Antwort auf die Frage des Auftrags: NICHT anderes Blatt, NICHT
  Projektion auf ein vorhandenes Rechteck, sondern eine Marke OHNE Traeger (rect 255, kein
  Schema) mit dem Besucht-Bit eines fremden Raums.
* **Warum kein Riegel anschlug:** `test_map_marke_haengt_an.c:127` und `:155` ueberspringen
  `r == 255`.

Dieselbe Klasse (A), vom Nutzer noch nicht gemeldet, aber in SEINEM Stand sichtbar:

| Marke | Header | Blatt | Lage | gehoert wirklich | haengt an |
|---|---|---|---|---|---|
| 20..24 | 402-406 | 1 | (131,77) (134,133) (138,133) (139,77) (144,80) | ROOM1230 (`ROOM1230.RDT` @0x00AEE/0x00B34/0x00B54/0x00B74/0x00BC4) | ROOM1000/z0 |
| 91, 92 | 506-507 | 6 | (166,104) (185,110) | ROOM2020 (`ROOM2020.RDT` @0x00866/0x0082A) | ROOM1000/z0 |
| 58 | 473 | 4 | (148,137) rect 1 | ROOM1160 -> ROOM1180 (`ROOM1160.RDT` @0x007C6) | ROOM1090 (zid 22 in Runde 10 neu vergeben) |
| 36, 37 | 446-447 | 2 | (180,64) (222,75) rect 5 | ROOM10B0 (zid 11; zwei Tueren `ROOM10B0.RDT` @0x016D2 / @0x016F2) | eigene zid 11, aber rect 5 gehoert seit Runde 10 ROOM1090 |

(Die Tuer-Datensaetze dieser Tabelle wurden in der Fortsetzung mit
`kartenabzug_tools/r30_tuer_datensatz.py` nachgeschlagen — jeder genannte Offset traegt
einen Datensatz `3b ..`; Ausgabe `build/r30_karten-marken/f3/tueren_klasse_a.txt`.)

### 4c. 1F — "ROOM 1000 ist blau": Schema-Fuellung in RE2-Blau, deckend

* **Welcher Raum:** die Kachel IST ROOM1000, Zone 0 ("Ostraum"). `re15_map_zones.h:25`
  `{ 0x1000, 14350, -16850, 24800, 1650, 2, 255, 0, 0, ..., 1, 0 }` = Blatt 2, rect 255,
  Schema 1 -> `s_map_synth[0]` = `{ 207, 89, 15, 33, ... }` (`re15_map_zones.h:724`).
  Kasten 15x33 ab (207,89); ohne den 1 Punkt breiten Rand = x 208..220, y 90..120 =
  13x31 = 403 Punkte — genau die Messung am Abzug des Nutzers. Bit 0 ist in seinem Stand
  gesetzt. Die beiden WC-Kaesten (Zonen 1 und 2) fehlen im Abzug, weil ihre Bits
  (1 und 240) nicht gesetzt sind.
* **Kunst gibt es dort keine:** alle 137 Texel der beiden ueberdeckenden Rechtecke sind
  Index 0 (§2.4). RE1.5 zeichnet ROOM1000 nicht; der Kasten ist eine Port-Ergaenzung
  (`analysis/befunde_runde8_2026-09-13/karte-1000-1050.md`).
* **Warum blau und deckend:** `re15_inv_screen.c:2621` reiht die Fuellung mit `abe = 0`
  ein, `:2630` holt die Farbe aus `re2_ton`. Der Kommentar `:510-521` behauptet "blendet
  ebenfalls halb" — der Code tut es nicht.

| Zustand | Schema-Kasten (gemessen) | gemalte Kachel (gemessen) |
|---|---|---|
| besucht | (16,64,176) deckend | (16,56,40) / (16,56,56) |
| aktuell | (104,8,8) deckend | (48,8,48) / (48,8,64) |

Nachgerechnet mit dem Rasterer (`inv_render_pc.c:948-953`, FILL mit `abe`: je Kanal
`(d5 + f5) >> 1` in 5 Bit; Kachel-Blit `blend_ch` `:888-892`): Palettenwert
halbtransparent ueber den beiden Panelfarben ergibt exakt die gemessenen Kachelwerte.

| Panel | 0x81A4 (32,104,0) | 0x842D (104,8,8) | zum Vergleich 0xD902 (16,64,176) |
|---|---|---|---|
| (0,16,88) | (16,56,40) | (48,8,48) | (8,40,128) |
| (0,16,120) | (16,56,56) | (48,8,64) | (8,40,144) |

### 4d. Verlust beim Laden: die Etagen-Bits stehen nicht im Spielstand

* `re15_map_zones.c:38` `s_visited[32]` und `:46` `s_visited_floor[16]` sind ZWEI Felder.
* `:53-54` Export/Import kopieren nur `s_visited`. `re15_savedata_t` fuehrt nur
  `visited[32]` (`include/re15_savedata.h:131`, offsetof 868); Export
  `re15_savedata.c:171`, Import `:258-264`.
* `re15_map_rect_state` (`re15_map_zones.c:432-435`) verlangt fuer jedes Rechteck mit
  Etagenzeile das Etagen-Bit: Haupt-Zeile `(!etage_hier || etage_besucht)`, Gast-Zeile nur
  `etage_besucht`.
* Frischer Prozess -> `s_visited_floor` = 0 -> alle 11 Orte mit Etagenzeile fallen auf
  "unbesucht", bis man sie wieder betritt (§2.6); ihre Marken fallen auf den Blaettern 2..4
  mit (Gatter `:821`), auf den uebrigen Blaettern bleiben sie stehen und schweben (§2.8
  Klasse B).
* Die Zonen-Bits selbst sind NICHT betroffen (102 von 102 Orten, 0 Kollisionen,
  Schluessel = Raumliste). Kartenbesitz Flag(3,115) liegt in `flags[]` und geht mit.

Gesetzt werden die Etagen-Bits in `re15_map_visited_mark_at` (`re15_map_zones.c:236-269`):
das Bit der Zeile des AKTUELLEN Bands, und — wenn alle Zeilen des Ortes dasselbe Band
tragen (Fahrstuhl ROOM1080) — alle Zeilen zugleich. Der Index ist der ZEILENINDEX in
`s_map_floors` (50 Zeilen, Leon und Elza abwechselnd).

Zwei Nebenbefunde im selben Weg (erster Vorgaenger, §2.7):

* **D2, Leck in Gegenrichtung.** Der LOAD-GAME-Zweig (`platform/pc/main.c:3057-3071`)
  ruft `re15_gameflow_new_game` nicht, also auch kein `re15_map_visited_reset`
  (`re15_gameflow.c:71-72`); `re15_map_visited_import` ueberschreibt nur `s_visited`.
  In laufender Sitzung bleiben die Etagen-Bits des vorigen Laufs stehen.
* **D3, Alt-Staende.** `re15_savedata.c:258` verwirft Besucht-Bits bei `in->version < 8`.
  `re15_savedata_validate` (`:87-123`) stempelt v2..v6 vorher auf `RE15_SAVE_VERSION` und
  kopiert `old.visited` (`:120`). Nur v7 wird wirklich verworfen.

---

## 5. Umsetzungsplan fuer den Bau-Agenten

Reihenfolge nach Abhaengigkeit. Jede Konstante mit Beleg. Zeilennummern = Stand 437905cb.

⛔ **`re15_map_zones.h` NICHT neu erzeugen.** Der Kopf sagt "GENERIERT", der Header ist
aber seit Runde 10 von Hand weitergefuehrt: ein Generatorlauf auf eine Kopie weicht in
1029 Zeilen ab (`build/r30_karten-marken-abzug/gen_kopie/re15_map_zones_REGEN.h`, md5
934c2be5 gegen f0721877). Korrigiert wird der HEADER; der Generator bekommt dieselben
Korrekturen, damit ein spaeterer Lauf die Fehler nicht zurueckbringt.

### Schritt 1 — Farben der Port-Ergaenzungen an die Palette binden (Befund a, c)

`re15_port/include/re15_inv_screen.h`, neben `RE15_KARTE_TEX_OFF_BESUCHT` (`:186`):

```
#define RE15_KARTE_WAND          ((uint16_t)0x5AD6u)  /* TEX.TIM @0x055C = b0b0b0, STP 0 */
#define RE15_KARTE_TEX_OFF_WAND  0x055C
```

`re15_port/engine/src/re15_inv_screen.c`:

| Stelle | heute | Soll | Beleg |
|---|---|---|---|
| `re2_ton` BESUCHT `:525` | feste Zahl (16,64,176) | aus `RE15_KARTE_BESUCHT` dekodiert = (32,104,0) | `TEX.TIM` @Datei 0x0556 = `0x81A4`; Zeile 21 = `GetClut(0x100,0x1f5)` @0x80046fdc-fe8 |
| `re2_ton` AKTUELL `:524` | feste Zahl (104,8,8) | aus `RE15_KARTE_AKTUELL` dekodiert = (104,8,8), Wert unveraendert | `ST0.TIM` @0x109B6 = `0x842D`; `addiu s5,s5,1` @0x8006E648 |
| Schema-Fuellung `:2621` | `abe = 0` | `abe = 1` | STP-Bit (Bit 15) in `0x81A4` und `0x842D` gesetzt; RE1.5 zeichnet die Kacheln halbtransparent: `ori v0,v0,0x2` @0x80047314 / @0x80047380 |
| Innenwand-Farbe `:2164` (und toter Zweig `:2214`) | `re2_ton(zustand)` | `re2_ton_kante` = (176,176,176), zustandsfrei, `abe` bleibt 0 | `TEX.TIM` @0x055C = `0x5AD6`, STP 0; RE2 Eintrag 4 in allen drei Zustandszeilen bitgleich `0x4631` @0x1093C / @0x1099C / @0x109BC |
| `re2_ton_kante` `:530-543` | feste Zahl 0xb0 | aus `RE15_KARTE_WAND` dekodiert | wie oben |
| Kommentar `:510-521` | "blendet ebenfalls halb", Werte 1040b0 | berichtigen | — |

Dekodieren: `r = (w & 31) << 3; g = ((w >> 5) & 31) << 3; b = ((w >> 10) & 31) << 3`
(dieselbe Rechnung wie `test_karte_besitz.c:259-262`). Die Zahlen nicht wiederholen.

Nachgerechnet: FILL mit `abe = 1` rechnet je Kanal `(d5 + f5) >> 1` (`inv_render_pc.c:948-953`)
und liefert ueber beiden Panelfarben exakt die Kachelwerte (Tabelle §4c). Unter allen drei
Schema-Kaesten traegt die Kunst nur Index 0 (Ostraum 165+30 Texel, Nord-WC 80, Sued-WC 75 —
gemessen), die Fuellung mischt sich also nur mit dem Panel.

Zur Innenwand-Farbe: der Nutzer-Befund vom 2026-09-07 ("FALSCHE FARBE", Kommentar
`re15_inv_screen.c:1957-1966`) verlangte "die Umrandungsfarbe des Rechtecks, nicht Grau" —
damals wurden die Kacheln MODULIERT, ihre Umrandung war mitgetoent. Seit der CLUT-Zeile
(cc400b1e) ist die Umrandung jeder Kachel in jedem Zustand (176,176,176) (gemessen an allen
drei Abzuegen des Nutzers: 1422 / 4635 / 7551 Punkte). Dieselbe Regel ergibt heute Grau.

### Schritt 2 — Dach-Wandzeile entfernen (Befund a)

* `re15_port/engine/src/re15_map_zones.h:614` `{  5,  1,  148,  155,  182,  155,  23 },`
  streichen. Beleg: 35 von 35 Punkten auf Kachel-Index 4 (`MAP06.PIX` v=86 ab @Datei
  0x2B00, rect 1 @0x800764C8), Zeile darunter Index 0. Nach Schritt 1 waere die Linie
  punktgleich zur gemalten Wand; sie bleibt ein Tabellenfehler.
* `re15_port/tools/gen_map_zones.py:4627-4643`: die Aussenwand-Regel wieder wirksam
  machen — eine Linie verwerfen, wenn ihre Punkte zu 100 % auf Kachel-Index 4 liegen
  (dann malt die Kachel sie schon). Kein Schwellwert: 100 % trifft von den zehn Zeilen nur
  diese eine (Tabelle §2.9; die naechste hat 2 von 9).
* `s_map_teile` (`re15_map_zones.h:631-640`) fuehrt 8 Zeilen, alle fuer (Blatt 3, rect 5)
  und (Blatt 9, rect 7); keine fuer Blatt 5. Die Streichung hat dort keine Folge.

### Schritt 3 — Marken ohne Traeger (Befund b)

**3a — Tabellenzeilen** (`re15_map_zones.h`):

| Zeile | Marke | Inhalt | Massnahme | Beleg |
|---|---|---|---|---|
| 470 | 55 | `{ 3, 255, 188, 180, 3, 0, 255, 0 }` | streichen, neu setzen (3b) | Tuer `ROOM1090.RDT` @0x0213A, `zid_fehlt` |
| 402-406 | 20-24 | Blatt 1, rect 255, zid 0 | streichen | ROOM1230 (`ROOM1230.RDT` @0x00AEE ff.) hat keine Zonenzeile |
| 506, 507 | 91, 92 | Blatt 6, rect 255, zid 0 | streichen | ROOM2020 (`ROOM2020.RDT` @0x00866 / @0x0082A) hat keine Zonenzeile |
| 473 | 58 | `{ 4, 1, 148, 137, 0, 22, 255, 0 }` | streichen | ROOM1160 (`ROOM1160.RDT` @0x007C6) hat keine Zonenzeile; zid 22 = ROOM1090 |
| 446, 447 | 36, 37 | Blatt 2, rect 5, zid 11 | streichen | zid 11 = ROOM10B0 (rect 255, kein Schema); rect 5 gehoert ROOM1090 (`re15_map_zones.h:134`) |

Eine Marke ohne eigene Zonenzeile hat kein Besucht-Bit und keinen Traeger; sie kann nur an
einem fremden haengen.

**3b — Marke 55 neu:** `{ 3, 7, 199, 114, 2, 22, 16, 1 }`.

| Feld | Wert | Beleg |
|---|---|---|
| Blatt 3, rect 7 | Gast-Zeile ROOM1090 obere Ebene | `re15_map_zones.h:332`; Etagenzeile `{0x1090, 0, 6, 3, 7}` `:661`; Band 6 aus dem Tuer-Datensatz (Byte 4 = `06`) |
| zid 22 / zid2 16 | ROOM1090 / ROOM1100 | `re15_map_zones.h:134` / `:144`; Ziel-Byte `10` im Datensatz |
| y = 114 | gemeinsame Wand: Suedwand rect 7 (67+48-1) = Nordwand rect 6 (`s_map_rectfix[6]` y=114) | `MAP04.PIX`: Index 4 in rect 7 UND rect 6 zugleich auf x = 197..221 (Sonde J) |
| kind 2 | waagerechte Wand (Sued) | die Wand liegt in y-Richtung zwischen beiden Raeumen |
| x = 199 | Projektion der Trigger-Mitte (-5820,-18690) durch `re15_map_zone_marker` auf rect 7 | Sonde E/J; Gegenprobe von der ANDEREN Seite: Ankunftspunkt (-20900,-10336) in ROOM1100 -> (203,120), also 4 Punkte daneben und 6 Punkte hinter der Wand (der Spieler wird im Raum abgesetzt). Beide x liegen in der Wandspanne 197..221 |
| auf_partner 1 | Punkt liegt auf gemalter Flaeche von rect 6 (Index 4) | Sonde J: `r6=4 r7=4` |

Stuetze fuer die Lage von rect 7 (zweites Tuerpaar, unabhaengige Daten
`ROOM10F0.RDT` @0x00F52): Trigger in ROOM10F0 -> (192,82) auf rect 9; Ankunft in ROOM1090
oben (-13600,-9000,1300) -> (185,83) auf rect 7. Abstand 7 / 1 Punkte.

⛔ Das x ist eine BBOX-STRECKUNG (ROOM1090 @0x800768F8 und ROOM1100 @0x80076930 tragen den
Massstabs-Stub) — dasselbe Verfahren wie bei jeder anderen Marke eines Stub-Raums, aber
keine Original-Zeile. Siehe §7.

**3c — Gatter des Zeichners** (`re15_map_zones.c:818-835`, `re15_map_mark_get`):

| heute | Soll | Beleg |
|---|---|---|
| `:818` Rechteck-Gatter nur auf Blatt 2..4 ("nur dort existiert das Etagen-System") | auf ALLEN Blaettern | `s_map_floors` fuehrt Zeilen fuer die Blaetter 0, 1, 5, 6, 7, 8, 9, 10, 11 (§2.6); im Stand des Nutzers schweben deshalb 18, 19 (Blatt 1) und 67 (Blatt 5) |
| `:822` `re15_map_rect_state(...) != RE15_MAP_RECT_UNVISITED` | Traeger muss GEZEICHNET sein: Zustand BESUCHT oder AKTUELL, oder UNBESUCHT bei `re15_map_owned_page(page)` | so entscheidet der Zeichner selbst: `re15_inv_screen.c:2317-2318` und `:2341` (`UNMAPPED` -> `continue`) |
| rect 255 geht durch | rect 255 nur, wenn die Zone der Marke auf diesem Blatt ein Schema fuehrt (`synth != 0`) und besucht ist | `re15_inv_screen.c:2554` zeichnet das Schema nur dann |

`re15_port/tools/gen_map_zones.py:3561`: Rueckfall `zid_of.get((b, zi), 0)` durch
Verwerfen der Marke ersetzen.

### Schritt 4 — Etagen-Bits in den Spielstand (Befund d)

Grundsatz aus RE2 (§3.3): alles, was der Kartenzeichner liest, liegt im gespeicherten
Block [0x800D44A4, +0x798) (@0x801C0DE8-FC). Die Etagen-Bits selbst sind Port-Ergaenzung.

1. `re15_map_zones.c`: das Etagen-Bit nicht mehr ueber den ZEILENINDEX von `s_map_floors`
   schluesseln (verschiebt sich bei jeder Tabellenaenderung — derselbe Fehler, der
   2026-09-07 bei den Zonen-Bits behoben wurde, Kommentar `:56-75`), sondern ueber eine
   explizite Nur-Anhaengen-Tabelle `{room_basis, zone, band, page}`. Inhalt = die 25
   geraden Zeilen aus `re15_map_zones.h:646-697`, in dieser Reihenfolge:
   1060/0/0/2, 1060/0/4/3, 1060/0/8/4, 1080/0/0/2, 1080/0/0/3, 1080/0/0/4, 1090/0/1/2,
   1090/0/6/3, 10A0/0/1/1, 10A0/0/8/2, 10F0/0/0/3, 10F0/0/1/2, 1170/1/0/4, 1170/1/4/5,
   11A0/0/0/6, 11A0/0/3/0, 3080/0/0/7, 4020/0/0/8, 4020/0/0/9, 4020/0/0/10, 4070/0/0/11,
   4070/0/8/9, 4070/0/12/8, 50D0/0/0/11, 50D0/0/3/10.
   (In der Fortsetzung Zeile fuer Zeile gegen die Tabelle verglichen: stimmt. Rechtecke
   in derselben Reihenfolge: 10, 1, 1, 9, 4, 0, 5, 7, 9, 6, 9, 8, 3, 0, 2, 1, 8, 2, 13, 3,
   3, 1, 7, 0, 1.) Leon- und Elza-Zeile teilen ein Bit, wie bei den Zonen-Bits.
2. Neue Schnittstelle `re15_map_visited_floor_export/import(uint8_t[16])`.
   `re15_map_visited_import` loescht `s_visited_floor` IMMER zuerst (behebt D2).
3. `include/re15_savedata.h`: `RE15_SAVE_VERSION 9`, Feld `uint8_t visited_floor[16]`
   direkt vor `checksum` (sizeof 904 -> 920; `re15_memcard.c:83/105` kopieren
   `sizeof(*sd)` ab Block+0x100, Block 0x2000 — passt).
4. `re15_savedata.c`:
   * capture (`:171`): beide Felder exportieren.
   * validate (`:87-123`): v7/v8 tragen ihr Pruefwort bei Offset 900 ueber [0,900) —
     pruefen, `visited_floor` nullen, bei v7 zusaetzlich `visited` nullen, auf v9 heben.
     Muster: der vorhandene v5->v6-Zweig (`:49-56`).
   * v2..v6-Hebung (`:120`): `old.visited` NICHT uebernehmen (behebt D3).
   * restore (`:258-264`): beide Felder importieren; die Versionsabfrage entfaellt, weil
     validate die Bedeutung schon bereinigt hat.
5. Alt-Staende v8 (auch die vier Bloecke der Nutzer-Karte): ihre Etagen-Bits sind NICHT
   rekonstruierbar. Exakt ableitbar sind nur Orte, deren Zeilen alle dasselbe Band tragen
   (ROOM1080, ROOM4020, ROOM3080 — dort setzt `re15_map_visited_mark_at` `:253-268` alle
   Zeilen zusammen mit dem Zonen-Bit): fuer sie beim Heben Etagen-Bit := Zonen-Bit.
   Fuer die acht mehrbaendigen Orte siehe §6 Punkt 1 (Entscheidung).

### Schritt 5 — Riegel

| Riegel | prueft | Wert heute |
|---|---|---|
| `unit_r30_karte_nutzerstand` (NEU). Fixture: Kopie von `analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr` unter `re15_port/tests/`; Platz 2 laden, `restore`, Op-Liste je Blatt mit `re15_inv_screen_build` (Muster `test_map_synth.c:141-146`, Vorlage Sonde Abschnitt I) | (1) KEINE Op mit rgb (16,64,176) auf irgendeinem der 13 Blaetter; (2) Blatt 2: Op FILL (207,89) 15x33 traegt rgb (32,104,0) und `abe == 1`; (3) Blatt 5: keine FILL-Op auf y=155; (4) Blatt 3: keine Marken-Op bei (188,180); (5) keine sichtbare Marke ohne gezeichneten Traeger (Vorlage `marke_getragen` der Sonde) | 2 blaue Ops; 9 Marken ohne Traeger |
| derselbe Riegel im Zustand "alles begangen" | (1) und (5) ueber alle 13 Blaetter | 13 blaue Ops; 10 Marken ohne Traeger |
| `unit_map_marke_zid` (NEU) | jede Marke: ihre zid hat eine Zonenzeile auf dem Blatt der Marke; bei rect != 255 gehoert das Rechteck der eigenen oder (auf_partner) der Partner-Zone | 9 + 3 Verstoesse |
| `unit_map_innenwand` erweitern | keine Wand liegt zu 100 % auf Kachel-Index 4 | 1 Verstoss (Wand 2) |
| `unit_karte_besitz` erweitern | `RE15_KARTE_WAND` gegen `TEX.TIM` @0x055C; Schema-FILL und Innenwand-FILL der Op-Liste tragen die dekodierten Werte | Defines stimmen, Zeichner benutzt sie nicht |
| `unit_map_speichern_laden` (NEU) | Sonde F als Test: je Ort einzeln setzen -> capture -> Karte -> Null-Zustand -> load -> restore: 0 verlorene und 0 gewonnene Zonen-Bits, Etagen-Bits, Rechtecke, Marken ueber alle 102 Orte; dazu D2 (Laden in laufender Sitzung: 0 gewonnen) und D3 (v6-Stand: 0 Bits importiert) | 25 / 25 / 16; 16; 3 |
| `test_map_marke_haengt_an.c:127,155` | `r == 255` nicht ueberspringen, sondern gegen die Schema-Kaesten pruefen | blind |

Zu (5) "alles begangen": die vier Marken der Klasse C (72, 75, 112, 155) schweben aus einem
ANDEREN Grund (eigenes Rechteck gezeichnet, Lage neben der Kunst) und sind NICHT Teil
dieses Plans. Der Riegel fuehrt sie als benannte Ausnahmeliste mit genau diesen vier
Nummern — nicht als Zahl "<= 4" —, damit eine fuenfte auffaellt.

### Schritt 6 — Abnahme am Bild (gegen die drei Abzuege des Nutzers)

Lauf wie §2.1 mit der UNVERAENDERTEN Karte des Nutzers, Vergleich mit
`kartenabzug_tools/r30_abzug_vergleich.py` und `r30_blau_zensus.py`. Soll-Werte gerechnet
aus seinen eigenen Abzuegen (`kartenabzug_tools/r30_soll_nach_korrektur.py`, Ausgabe
`f3/soll_nach_korrektur.txt`; das Panel ist auf allen drei Blaettern punktgleich:
49693 / 49135 / 47603 gemeinsame Panelpunkte, 0 ungleich):

| Blatt | heute gegen Nutzer-Abzug | SOLL gegen Nutzer-Abzug | die abweichenden Punkte |
|---|---|---|---|
| ROOF | 0 | 35 (+15 mit Schritt 3c) | y=155 x=148..182: (16,64,176) -> (176,176,176); mit 3c zusaetzlich die Treppenmarke x in {154,156,158}, y 95..99 (240,240,216) -> Panel, weil rect 0 in einem v8-Stand nicht gezeichnet ist |
| 2F | 0 | 5 | (188,178..182): (224,168,40) -> (0,16,88) x4, (0,16,120) x1 |
| 1F | 13 | 403 + hoechstens 13 | x 208..220 y 90..120: (16,64,176) -> (16,56,40) 353 Punkte, (16,56,56) 50 Punkte; dazu der pulsierende Spielermarker x 177..181 y 125..129 |

Dazu: `r30_blau_zensus.py` ueber die drei neuen Abzuege und ueber einen neuen
13-Blatt-Durchlauf (`RE15_MAP_SHOT_SWEEP`, alles besucht) = 0 Punkte (16,64,176);
frei schwebende gelbe Marken im Stand des Nutzers = 0.

Speichern/Laden im echten Weg (Laeufe `s1_10C0` / `l1_10C0` aus §2.7): Blatt 2F vor dem
Speichern gegen nach dem Laden = 0 abweichende Punkte im Kartenfeld (heute 2964).

---

## 6. Risiken / offene Fragen

1. **ENTSCHEIDUNG: Alt-Staende v8 und die acht mehrbaendigen Orte.** Welche Etage begangen
   war, steht in keinem v8-Block. Zwei Wege:
   (a) die Orte bleiben verborgen, bis sie wieder betreten werden — nichts Erfundenes, aber
   die vier vorhandenen Staende des Nutzers zeigen ROOM1060, ROOM1090, ROOM10A0 und
   ROOM1170/z1 weiterhin NICHT, bis er dort war;
   (b) beim Heben gilt die HAUPT-Zeile eines besuchten Ortes als begangen (ROOM1060 -> 1F,
   ROOM1090 -> 1F, ROOM10A0 -> 1F, ROOM1170/z1 -> ROOF) — der Nutzer sieht seine Raeume
   wieder, aber der Port behauptet eine Etage, die er nicht kennt.
   Die Befunde vom 2026-09-01 und 2026-09-12 betrafen GAST-Zeilen; (b) zeigt keine
   Gast-Zeile. Beides ist kein Original-Verhalten. Vorgabe des Plans, falls keine
   Entscheidung faellt: (a).
2. **Pins mit Bezug auf die gestrichenen Marken.** `unit_map_durchgang`,
   `unit_map_tuerachse`, `unit_map_marke_auf_kunst`, `unit_map_mark_band`, `unit_map_1090`
   zaehlen oder verorten Marken; nach Schritt 3 nachmessen, nicht die Schranken lockern.
3. **Pin `unit_map_re2_system` gegen Schritt 3c.** Er haelt fest, dass Marken auf den
   Blaettern ab 5 "direkt mit dem Besuch" erscheinen und `UNMAPPED`-Rechtecke eine Marke
   tragen. Der Zeichner malt `UNMAPPED` ausserhalb des Stock-Modus NICHT
   (`re15_inv_screen.c:2341`); der Pin und der Zeichner widersprechen sich also heute
   schon. Beim Bau messen, welche Marken 3c zusaetzlich sperrt (Sonde H vor/nach), bevor
   der Pin angefasst wird.
4. **Zeichenreihenfolge der halbtransparenten Schema-Fuellung.** Die Op-Liste wird von
   hinten gerastert; die Fuellung mischt sich mit allem, was DANACH in der Liste steht.
   Gemessen ist nur, dass die Kunst unter den drei Kaesten leer ist (Index 0). Rand und
   Tuermarken des Kastens muessen VOR der Fuellung in der Liste bleiben.
5. **`integration_map_raum_live` und `unit_map_synth`** rastern die Op-Liste nach und
   erkennen Rot an (104,8,8); mit `abe = 1` misst ein aktueller Schema-Raum
   (48,8,48)/(48,8,64). Nachmessen.
6. **Wand 6 (Blatt 9 rect 7)** liegt mit 36 von 45 Punkten auf Index 0 (unbemalt) und wird
   trotzdem durchgezogen (im Abzug 45 Punkte bei y=116). Nicht gemeldet, nicht Teil des
   Plans; `RE15_INV_OP_FILLMASK` statt `FILL` wuerde jede Innenwand auf die bemalte
   Flaeche begrenzen.
7. **Klasse C: Marken 72, 75 (Blatt 6), 112 (Blatt 7), 155 (Blatt 9)** schweben neben der
   Kunst ihres eigenen Rechtecks. Ursache nicht untersucht (der Generator meldet fuer
   (129,153) selbst "22 px ROOM2080 Tuer #0 ... -> (107,153)", `regen.log`).
8. **27 Basisraeume ohne Zonenzeile** (1160 1230 1240 1260 1270 2020 20A0 20B0 20C0 20D0
   20E0 20F0 30F0 4060 40C0 40D0 40E0 40F0 5070 5130 5150 5160 5170 6040 6050 6060 6070):
   in ihnen hat die Karte weder Marker noch aktuellen Raum. ROOM1160, 1230, 2020 sind die
   Raeume, deren Marken in Schritt 3a gestrichen werden.
9. **ROOM10F0 Etagenzeile Band 1 -> Blatt 2 rect 8.** `ROOM10F0.RDT` @0x00F52: Ankunft
   y = -9000 -> die Tuer fuehrt auf die OBERE Ebene von ROOM1090 (Blatt 3), nicht auf 1F.
   Die Regel "Band der Tuer -> Seite des Zielraums" traegt nicht, wenn der Zielraum zwei
   Blaetter hat. Nicht weiter untersucht.
10. **Parallele Arbeit im selben Baum.** Thema B (Karte nach der Irons-Cutscene,
    `probe_r30_karte-3010.c`) beruehrt ebenfalls den Kartenbesitz; beim Bau
    `re15_map_owned_page` nur LESEN.

---

## 7. Ausdruecklich NICHT belegt

* **x = 199 der neu gesetzten Marke 55.** Bbox-Streckung des Ports, von zwei Seiten auf
  4 Punkte uebereinstimmend (199 / 203), aber keine Original-Zeile und keine gemalte
  Tuernische in rect 7 / rect 6.
* **Dass rect 7 die obere Ebene von ROOM1090 ist.** Uebernommen aus Runde 10; hier nur
  gestuetzt (Tuerpaar ROOM10F0, 7 / 1 Punkte Abstand), nicht bewiesen.
* **Die Entstehung der Dach-Wandzeile im Einzelnen.** Aus dem Generator-Code abgeleitet,
  der Lauf fuer ROOM1170 nicht Schritt fuer Schritt nachgerechnet.
* **Die Zuordnung CLUT-Y 498/501/502 -> ST0.TIM-Zeile 8/11/12.** Aus Runde 27 uebernommen.
* **RE2s SPEICHER-Richtung.** Gefunden ist die Lade-Kopie @0x801C0DE8-FC; die Stelle, die
  den Block 0x800D44A4 auf die Karte schreibt, ist nicht lokalisiert.
* **Ladeadresse 0x801BFA18 von MEM_CARD.BIN.** Aus der Trefferquote der jal-Ziele bestimmt
  (19 von 31), nicht aus dem Lader der EXE gelesen.
* **Welche Etagen der Nutzer vor seinem Speichern begangen hatte.** Block 3 zeigt nur die
  Zonen-Bits.
* **Die Schema-Lage von ROOM1000** ((207,89) 15x33 usw.). Uebernommen aus
  `analysis/befunde_runde8_2026-09-13/karte-1000-1050.md`, hier nicht neu gemessen.

---

## 8. Artefakte

Werkzeuge (`analysis/befunde_runde30/`):

| Datei | Zweck |
|---|---|
| `r30_karten_run.sh` | ein Messlauf der eigenen exe (erster Vorgaenger) |
| `r30_karten_gen_diag.py` | Generator schreibgeschuetzt fahren, Herkunft je Marke |
| `r30_karten_re2_memcard_dis.py` | Ausschnitt aus RE2s MEM_CARD.BIN disassemblieren |
| `kartenabzug_tools/r30_abzug_messen.py` | Farben/Komponenten in den Nutzer-Abzuegen |
| `kartenabzug_tools/r30_abzug_vergleich.py` | Punktvergleich eigener Abzug gegen Nutzer-Abzug |
| `kartenabzug_tools/r30_karte_lesen.py` | Speicherkarte lesen, Besucht-Bits dekodieren |
| `kartenabzug_tools/r30_rects.py` | Rechtecktabellen aus PSX.EXE |
| `kartenabzug_tools/r30_tuer_datensatz.py` | Tuer-Datensaetze mit Datei-Offset |
| `kartenabzug_tools/r30_clut_zeilen.py` | NEU: Karten-CLUT-Zeilen beider Spiele je Eintrag |
| `kartenabzug_tools/r30_texel_an_marke.py` | NEU: Texel-Index an den drei Marken, Zensus 12/13/14 |
| `kartenabzug_tools/r30_blau_zensus.py` | NEU: (16,64,176) und frei schwebende Marken je Abzug |
| `kartenabzug_tools/r30_soll_nach_korrektur.py` | NEU: Soll-Werte der Abnahme |
| `kartenabzug_tools/r30_abzug_lauf.sh` | Lauf des zweiten Vorgaengers (Blattwahl unzuverlaessig, s. §2.1) |

Sonden (`re15_port/tests/unit/`): `probe_r30_karten-marken.c` (Abschnitte A-E),
`probe_r30_karten-marken_rundlauf.c` (NEU, Abschnitte F-J),
`probes/r30_karten-marken.cmake` (beide Ziele, kein `add_test`).

Ausgaben: `build/r30_karten-marken/` (Laeufe und Abzuege des ersten Vorgaengers,
`probe_ergebnis.txt`), `build/r30_karten-marken/f3/` (Fortsetzung: `rundlauf_ergebnis.txt`,
`clut_zeilen.txt`, `texel_an_marke.txt`, `zensus_nutzer.txt`, `zensus_sweep1150.txt`,
`soll_nach_korrektur.txt`, `tueren_nachbarn.txt`, Entwurf des Vorgaengers),
`build/r30_karten-marken-abzug/` (zweiter Vorgaenger: `karte_nutzer.txt`,
`nutzer_messung.txt`, `rects_2_5.txt`, `tuer_1090_1100.txt`, `gen_kopie/`).
