# Vordergrund-Masken (PRI) STAGE1 — Audit der Pipeline gegen die Freistellungen des Nutzers

Datum 2026-09-19. Sonde `re15_port/tests/unit/probe_r16_pri_masken_audit.c` (registriert in
`re15_port/tests/unit/probes/r16_pri-masken-audit.cmake`, Build `re15_port/build_r16_pri`).
Messdateien, Bilder, Skripte und die aus Git zurueckgeholten Nutzer-Originale liegen in
`analysis/befunde_2026-09-19/pri-masken-audit/`.

## 0. Kurzfassung

1. **Die Masken sind NICHT die PNGs des Nutzers.** Bei 40 der 84 STAGE1-Cuts (alle seit 2026-09-13 eingebundenen Raeume 1000/1010/1050/10A0/1160/1180/11B0/11D0/11F0/1220/1230) ist die gelieferte Deckung bitgenau `PNG(alpha>110)` **um 4 Zeilen nach oben und 1 px rundum aufgeweitet** (`raum.py:56` STD `oben=4, grow=1`, Halo-Beweis 0 px Abweichung in 38/40 Cuts, Ausnahmen 1000 C3 und 1180 C5): 5-18 % Hintergrundpixel werden mit Objekttiefe UEBER die Figur gemalt. Dazu 4 Cuts, die zu 82-100 % aus MEINEN Quellen bestehen (Polygone 1000 C0/C2, 11F0 C0; Quader 1100 C1/C2, 10D0 C7 1794 px, 10F0 C4/C5 9937/5709 px "Tiefschwarz-Etiketten"), obwohl der Nutzer fuer STAGE1 SAEMTLICHE gewuenschten Freistellungen geliefert hat (118/118 referenziert).
2. **Drei Nutzer-PNGs wurden in Git veraendert, zwei ersetzt/zerteilt:** `10D0/06.png` (Nutzer 30x46, 245 px) wurde durch meine 65x120-Fassung ersetzt, waehrend die eingemessene Lage `x=34,y=172` stehen blieb -> die "Stuhlkante" liegt heute zu 2/3 ausserhalb des Bildes und deckt **Boden** links unten mit Tiefe 109..117 (= "Boden ueber Leons Bein", ROOM10D0). `10D0/01.png` (1021 px) wurde in 01_01/01_02 zerteilt, `10D0/07_01.png` (+337/-168 px), `1130/03.png` (38x192 -> 75x225, meine Fassung), `10E0/07_01.png` (-5/+1 px) veraendert. Originale: `git show 61607499:pri/10D0/{01,06,07_01}.png`, `45a81425:pri/1130/03.png`, `61607499:pri/10E0/07_01.png` (kopiert nach `…/nutzer_original_*.png`, Lage neu gemessen: 06.png passt zu 100 % genau an x=34,y=172).
3. **Tiefe:** Alle neuen Raeume laufen ohne Tiefenschluessel in die Spaltenregel mit `DEPTH_FACTOR 0.90` (`geom.py:43`, `:1214`): die Maske behauptet 10 % NAEHER zu sein als ihre eigene Bodenlinie -> wer 0..10 % VOR dem Objekt steht, wird voll verdeckt ("komplett ueberdeckend": z.B. 1000 C5 198 von 290 Vorne-Standplaetzen, 1010 C6 120/210, 1050 C6 101/251, 11B0 C3 186/845). Mit Faktor 1.00 faellt das auf 155/81/48/68 — der Rest ist die **Kachel-Median-Tiefe** (`anwenden.py`, `_st = np.median`) ueber 8..32 px breite Quadrate, waehrend die Bodenlinie diagonal laeuft: ein Rechteck hat EINE Tiefe, die halbe Kachel liegt falsch -> Schachbrett = "halb transparent" (1010 C6 Tisch: Kacheln 58..91 nebeneinander; 1050 C6: 198..301 neben 134). Objekte ohne Bodentreffer (Sehstrahl ueber dem Horizont, ROOM1000 C3: 3 von 4 Objekten, 719 von 1076 px) bekommen GAR KEINE Maske (`anwenden.py` verwirft Kacheln mit Tiefe 0) = "funktioniert gar nicht".
4. **Original:** eine Maske ist ein opakes SPRT (Code 0x64, kein ABE) mit Tiefe = OT-Index (`@0x80039650-60`), Zeichen entscheidet allein `depth < otz>>4` (`@0x8002565c`, ZSF3=341 `@0x80066c70`) -> PC-Schwelle `depth*64` (`re15_pri.h`). Die Kuenstler setzen im Median 28 Rechtecke (16x16) mit **19 verschiedenen Tiefen je Cut** (Spanne median 126) — gestufte Tiefen entlang des Objekts, keine Quadrate mit Median. Farbschluessel der GPU ist der WERT 0x0000, nicht der Index (psx-spx): 12556 Texel in 1000 C7 waeren auf PSX Loecher (`atlas.py` quantisiert Dunkles auf 0x0000; `bg_pc.c pri_publish_tim` schluesselt Index 0 -> PC zeigt sie).
5. **Phase 2:** (a) Nutzer-PNGs aus Git zurueckholen, alle Nicht-PNG-Objekte in STAGE1 entfernen, `oben=0/grow=0` fuer PNG-Objekte erzwingen, Treue-Riegel "Deckung == PNG(>110) bitgenau" in `raum.py` UND als ctest ueber die Engine-Leser; (b) Tiefe ohne Faktor, je Pixel aus Geometrie (Sehstrahl gegen die SCA-Zelle des Objekts, sonst Bodenkontakt je Spalte mit Huellen-Pruefung und Spalten-Erbe), Rechtecke ADAPTIV so zerlegt, dass jedes Rechteck hoechstens eine Tiefenstufe traegt (Streifen entlang des Gradienten statt Quadrate), Ueberlauf ueber 105 LAUT statt Verwerfen; (c) Abnahme je Cut mit der Standplatz-Schiene der Sonde (VOR der Bodenlinie nie voll verdeckt, DAHINTER >= 95 %, Bild je Cut) und Kalibrierung des Tiefenmodells an den 114 STAGE1-Original-Cuts (3583 Kuenstler-Rechtecke) BEVOR ein Nutzer-Cut gebaut wird; (d) CLUT-Schwarz auf 0x8000 fuer PSX.

## 1. Reproduktion/Messung

### 1.1 Sonde und Leser-Verifikation

`probe_r16_pri_masken_audit.exe` laedt jede STAGE1-RDT ueber `re15_rdt_parse`, baut die Sicht mit
`re15_camera_build_view`, parst die Original-Sektion (`re15_pri_parse_section`) und — nur wenn die
NULL ist — die R15M-Sektion (`re15_pri_msk_section_offset`), laedt den Atlas `MASKS/ROOM####_PRI##.TIM`
ueber `re15_tim_parse` und rastert die Deckung 320x240 genau wie `bg_pc.c pri_publish_tim`
(Index 0 = durchsichtig) + `render_pc.c` (`SDL_RenderCopy atlas[src]->screen[dst]`, nur `draw_count`
Rechtecke). Bodenpunkte kommen aus `re15_collision_on_floor` mit gesetztem Band
(`re15_collision_set_band`, Raster 200 ueber die SCA-Huelle). 78 RDTs, 850 Cuts, 2,1 s.

Python-Leser (`tools/maske/maskenbild.py masken/lies_tim`) gegen die Engine-Deckung: **84 Cuts,
0 Abweichungen** (Deckung UND naechste Tiefe je Pixel bitgleich) — `messung_A_…txt` Teil A.
Alle weiteren Zahlen rechnen deshalb auf den Engine-Werten (Rechtecke, Kamera, Bodenpunkte aus dem Dump).

### 1.2 Lage der PNGs (`messung_F_lage_der_pngs.txt`)

Alle 108 STAGE1-PNG-Objekte an der eingetragenen (x, y, massstab) gegen den Hintergrund
(`build/bg_ppm`, Metrik von `maske_aus_png.platziere`): 104 zu 97,7-100 %. Auffaellig:

| Objekt | eingetragen | Treffer | Suche |
|---|---|---|---|
| ROOM10D0 C6 `10D0/06_01.png` (65x120) | x=34 y=172 s=1 | **56,3 %**, nur 600 von 1797 px im Bild | passt zu 100 % bei **x=0 y=103** |
| ROOM1000 C1 `01_01/01_02/01_03` | s=4 | 92,6 / 87,8 / 97,0 % | s=4 ist die beste Skala (s=1: 74,9 %) |
| ROOM1000 C3 `03_01` | s=4 | 89,5 % | s=4 beste Skala |
| ROOM11F0 C1 `01.png` | s=4 | 85,2 % | s=4 beste Skala; die Maske (32x38 px) beruehrt **0** begehbare Standplaetze |

Erklaerung zu 10D0 C6 in 1.5: `x=34,y=172` ist die korrekte Lage des NUTZER-Originals `06.png`
(30x46 px, Treffer 100,0 %) — die Datei wurde unter dem Eintrag ausgetauscht.

### 1.3 Pixelmenge je Cut (`messung_A_…txt` Teil B, Auszug; MaskePx = Engine-Deckung, Loch = PNG-Pixel ohne Maske, Ueber = Maskenpixel ohne PNG)

| Raum | Cut | Rects | MaskePx | PNGPx | Loch | Ueber | eigene Quelle | Tiefe | Schluessel |
|---|---|---|---|---|---|---|---|---|---|
| 1000 | 0 | 102 | 15703 | 0 | 0 | 15703 | polygon (meins) | 55 | Spaltenregel |
| 1000 | 1 | 45 | 1973 | 1374 | 0 | 599 | – | 65..**531** | Spaltenregel, s=4 |
| 1000 | 2 | 104 | 31566 | 0 | 0 | 31566 | polygon (meins) | 63..74 | Spaltenregel |
| 1000 | 3 | 17 | 521 | 1076 | **719** | 164 | – | 216..**496** | Spaltenregel, s=4 |
| 1000 | 4..8 | 92..104 | 14948..28166 | 14020..26881 | 0 | 928..1752 | – | 50..499 | Spaltenregel |
| 1010 | 0/1/6/8 | 70..91 | 9470..15895 | 8505..13430 | 0 | 965..**2465** | – | 52..124 | Spaltenregel |
| 1050 | 1/3/5/6 | 9..101 | 385..36060 | 237..34653 | 0 | 148..1407 | – | 55..**301** | Spaltenregel |
| 1060 | 0/1/2/4/7 | 55..101 | = PNG | = PNG | 0 | 0 | – | 58..246 | ebene+spalten |
| 10A0 | 0/1/2/5/7/8 | 72..104 | 3664..18625 | 3180..17794 | 0 | 484..1008 | – | 56..138 | ebene (Spaltenregel) |
| 10C0 | 2/3/4 | 62..103 | = PNG | = PNG | 0 | 0 | – | 62..142 | aufrecht/spalten/kollision |
| 10D0 | 1 | 45 | 1021 | 1021 | 0 | 0 | (Nutzer-01.png zerteilt) | 107..213 | flach + spalten |
| 10D0 | 6 | 89 | 2030 | 2030 | 0 | 0 | 06_01 = MEINE Fassung, falsch platziert; 06_02 meins | 58..115 | aufrecht |
| 10D0 | 7 | 105 | 5435 | 3641 | 0 | **1794** | quader (meins) | 53..87 | aufrecht/spalten/quader |
| 10D0 | 8/9 | 6/105 | = PNG | = PNG | 0 | 0 | – | 61..255 | spalten/aufrecht |
| 10E0 | 0..9 | 22..102 | = PNG | = PNG | 0 | 0 | (07_01 -5/+1 px veraendert) | 38..167 | diverse |
| 10F0 | 4 | 105 | 12068 | 2131 | 0 | **9937** | quader+Tiefschwarz (meins) | 26..174 | szene/quader |
| 10F0 | 5 | 105 | 9285 | 3576 | 0 | **5709** | quader+Tiefschwarz (meins) | 44..185 | szene/quader |
| 10F0 | 0/1/2/3/6 | 27..101 | = PNG | = PNG | 0 | 0 | – | 59..116 | spalten |
| 1100 | 1/2 | 105/105 | 28160/25211 | 0 | 0 | alles | quader (meins), kein Nutzer-PNG | 45..163 | quader |
| 1110/1120/1130/1140 | alle | – | = PNG | = PNG | 0 | 0 | (1130/03 = MEINE 75x225-Fassung) | – | diverse |
| 1160/1180/11B0/11D0 | – | 59..88 | | | 0 | 157..1523 | – | 42..220 | Spaltenregel |
| 11F0 | 0 | 104 | 12626 | 0 | 0 | 12626 | polygon (meins) | 44..80 | Spaltenregel |
| 11F0 | 1/6/7 | 26..104 | | | 0 | 302..2940 | – | 44..67 | Spaltenregel |
| 1220 | 0/2/4/6/8 | 74..93 | | | 0 | 636..902 | – | 54..91 | Spaltenregel |
| 1230 | 2/3/5/6 | 67..84 | | | 0 | 277..759 | – | 42..143 | Spaltenregel |

Summen: **0 Loecher** ausser ROOM1000 C3 (719 px = die Objekte 03_01/03_02/03_03 komplett);
**Ueberschuss in 45 Cuts** (alle Cuts mit Spaltenregel/ebene ohne `oben/grow`-Eintrag);
**eigene Pixel** (Nicht-PNG-Quellen) in 1000 C0/C2, 10D0 C7, 10F0 C4/C5, 1100 C1/C2, 11F0 C0.
PNG-Alpha: die Schwelle 110 schneidet je PNG 13..634 px Saum (0<alpha<=110) weg (Anti-Aliasing-
Rand, 1-2 px); die Massstab-4-Objekte verlieren beim BOX-Verkleinern zusaetzlich duenne Teile
(1000/01_01: 14061 px Alpha im PNG -> 877 im Bild).

### 1.4 Ursachen-Beweise (`messung_D_…txt`, `messung_E_…txt`)

**(a) Halo.** Fuer jeden PNG-Cut wurde `PNG(alpha>110)` mit den in `auswahl.json` wirksamen
`oben/grow` (Default `4/1` aus `raum.py:56`, explizit `0/0` bei 1060/10C0..1140) aufgeweitet und
gegen die Engine-Deckung verglichen: **`Halo!=Maske` = 0 px in 38 von 40 Default-Cuts** (Ausnahmen:
1000 C3 = 1020 px verworfene Objekte, 1180 C5 = 121 px Kachelverlust). Die gelieferten Masken SIND das
aufgeweitete PNG. Bild: `ROOM1010_C6_halo.png` (gelber Saum ueber dem Tisch).

**(b) Faktor 0.90 + Kachel-Median.** Fuer alle reinen Spaltenregel-Cuts wurde das Tiefenmodell
(Bodenkontakt je Spalte auf `ebene`, Faktor 0.90, Median je Kachel ueber die Engine-Rechtecke)
nachgerechnet: **Treue |Kachel-Modell| <= 2: 100 % in 36 von 43 Cuts** (1000 C1 44 %, C3 80 % wegen
s=4-Rundung, 1000 C4/C7 93-95 %, 1140 C0 98 %, 10A0 C0 99,5 %). Standplatz-Audit (Koerper 1500
hoch, +-450 breit, Koerper-vz je Bildzeile, Standlinie des Objekts = unterste PNG-Zeile je Spalte
auf der Objekt-Ebene):

| Raum | Cut | VOR-Plaetze | voll verdeckt 0.90 | teil 0.90 | voll 1.00 | teil 1.00 | HINTER-Plaetze | nicht voll 0.90 |
|---|---|---|---|---|---|---|---|---|
| 1000 | 5 | 290 | **198** | 84 | 155 | 88 | 673 | 5 |
| 1000 | 8 | 297 | **180** | 97 | 146 | 82 | 625 | 41 |
| 1000 | 7 | 264 | 97 | 81 | 47 | 68 | 775 | 73 |
| 1010 | 0 | 276 | 91 | 89 | 31 | 88 | 848 | 31 |
| 1010 | 6 | 210 | **120** | 81 | 81 | 68 | 1031 | 39 |
| 1010 | 8 | 197 | 88 | 104 | 52 | 76 | 1014 | 32 |
| 1050 | 5 | 215 | 67 | 85 | **1** | 38 | 2597 | 21 |
| 1050 | 6 | 251 | **101** | 95 | 48 | 104 | 406 | 8 |
| 10A0 | 0 | 236 | 55 | 133 | 21 | 86 | 1897 | 24 |
| 10A0 | 5 | 700 | 97 | 223 | 37 | 79 | 1537 | 27 |
| 11B0 | 3 | 845 | **186** | 295 | 68 | 277 | 1287 | 66 |
| 1180 | 6 | 87 | 62 | 25 | 20 | 34 | 553 | 0 |
| 1230 | 6 | 97 | 61 | 36 | 26 | 43 | 978 | 0 |
| 1000 | 1 | 226 | 39 | 23 | 9 | 10 | 178 | **117** |

Lesart: "VOR voll verdeckt" = der Spieler steht laut dem Modell der Pipeline selbst VOR dem
Objekt und ist trotzdem zu >=95 % hinter der Maske = "komplett ueberdeckend". Das Verhaeltnis
Standlinie/Fuss dieser Plaetze liegt im Median bei 1,004..1,05 (Spalte in `messung_E`), also
genau im Fenster 0,90..1,00 des Faktors; wo es darueber liegt (1000 C5 1,141, C8 1,256) traegt der
Kachel-Median (Bodenlinie diagonal, Kachel 20 px). "HINTER nicht voll" = hinter dem Objekt und
trotzdem sichtbar = "funktioniert nicht/halb transparent": 1000 C1 117 von 178 (Tiefen bis 531).

**(c) Loecher = Sehstrahl ueber dem Horizont.** ROOM1000 C3: `03_01/03_02/03_03` — 21/10/12 Spalten,
**0 Bodentreffer** (die Bodenebene y=0 wird nur in den Bildzeilen 136..239 getroffen, die Objekte
enden bei y<136); `03_04` 14 von 15 Spalten, vz 16658..194855 -> Tiefe 234..2740 (geklemmt 1023).
`anwenden.py bau_objektweise` verwirft Kacheln ohne Tiefe (`if len(win)==0: continue`) -> 3 Objekte
ohne jede Maske. Bild `ROOM1000_C3_loecher.png` (rot = PNG ohne Maske).

**(d) Kachel-Staffelung = "halb transparent".** `ROOM1050_C6_tiefenstaffel.png`: Kaffeeautomat
Kacheln 301/208/241/216/198 links neben 134/135 (die schraege Unterkante wird je Spalte als
Bodenkontakt gelesen; s. `analysis/befunde_2026-09-14/maske_1050_offen.md` §3).
`ROOM1010_C6_halo.png`: Tisch mit Beinen — Beinspalten bekommen den Bodenkontakt (58..61), die
Plattenspalten dazwischen die Unterkante der Platte (64..91) — nebeneinanderliegende Kacheln
58/91/65/60/64/68/69/70/61: Leon wird zwischen den Beinen verdeckt und daneben nicht.

**(e) Eigene Pixel.** `ROOM10D0_C7_quader_eigene.png`: der Quader "Holztisch mit Klappstuhl"
(`auswahl.json`, `minus 58,0,320,240`) deckt links unten 1794 px, die in KEINEM Nutzer-PNG liegen —
Boden/Tischbereich mit Tiefe 53..62. `ROOM10F0_C4_eigene_dunkel.png`: 9937 px "Tiefschwarz-
Etiketten" (Schreibtische, Bodenkanten) mit Stuhltiefen 26..174 neben 2131 px Nutzer-Lasso.
`ROOM10D0_C6_lage.png`: die falsch platzierte Stuhlkante liegt als Block ueber den Bodenfliesen
links unten (Tiefe 109..117).

### 1.5 Git-Historie der Nutzer-PNGs

`git log --raw -- pri/`: 110 von 113 STAGE1-PNGs sind blobgleich mit ihrer ersten Fassung. Veraendert
bzw. ersetzt (alles MEINE Commits):

| Datei (heute) | Nutzer-Original | was passierte |
|---|---|---|
| `pri/STAGE1/10D0/06_01.png` 65x120, 1797 px | `61607499:pri/10D0/06.png` 30x46, **245 px** | `055374ab` ueberschrieben (7237ef95), `91284cac` nach 06_01 umbenannt; Lage x=34,y=172 blieb (Original passt dort zu 100 %) |
| `pri/STAGE1/10D0/06_02.png` "Pflanze links" | – (meins, `91284cac`) | kein Nutzer-PNG |
| `pri/STAGE1/10D0/01_01.png` + `01_02.png` (331+690 px) | `61607499:pri/10D0/01.png` 54x61, 1021 px | `91284cac` geloescht und in zwei Objekte (flach/spalten) zerteilt |
| `pri/STAGE1/10D0/07_01.png` 2677 px | `61607499:pri/10D0/07_01.png` 2508 px | `a56cff68` +337/-168 px |
| `pri/STAGE1/1130/03.png` 75x225, 10289 px | `45a81425:pri/1130/03.png` 38x192, 3633 px | `9e403d60/566b0e22/61607499` "verbreitert" (meine Fassung) |
| `pri/STAGE1/10E0/07_01.png` 14035 px | `61607499:pri/10E0/07_01.png` 14039 px | `91284cac` -5/+1 px |

`pri/1140/01.png` -> `00_01.png` ist nur eine Umbenennung (gleicher Blob). Die Originale liegen unter
`analysis/befunde_2026-09-19/pri-masken-audit/nutzer_original_*.png`; Lage gemessen: 10D0/06 x=34 y=172
(100 %), 10D0/01 x=124 y=87 (100 %), 10D0/07_01 x=0 y=123 (100 %), 1130/03 x=79 y=0 (99,9 %),
10E0/07_01 x=110 y=97 (100 %).

### 1.6 Atlas/Kapazitaet (`messung_D_…txt`, Spalten rects/kante/atlasPx)

Kein Cut ueberschreitet 105 Rechtecke oder 65536 Atlaspunkte (Maximum 105 in 10D0 C7/C9, 10F0 C4/C5,
1100 C1/C2; 58922 px in 10E0 C7). Der Preis ist die Kachelkante: bei grossen Objekten waehlt `_waehle`
20-32 px (1000 C2/C5/C7/C8, 1050 C5, 1180 x4, 1230 x4, 11F0 C6/C7 = 20; 10F0 C4/C5, 1100 = 24; 10E0 C7 =
32) — ein Rechteck mit EINER Tiefe ueber 20-32 Spalten einer diagonalen Bodenlinie (s. 1.4 b/d).
Kein Cut lief in den "Abbruch"-Pfad; die Loecher in 1000 C3 stammen aus (c), nicht aus der Kapazitaet.

## 2. Original-Mechanismus

- **Sektion/Parse (FUN_800392d4):** NULL-Sektion = erstes u32 `FF FF FF FF`; `srcX/srcY` u8
  (`@0x80039408 / @0x80039418`), Bildposition = Gruppenanker + Masken-Byte (`@0x8003940c / @0x8003941c`),
  Tiefe unveraendert kopiert (`@0x800393f0 lhu v0,-6(s2)` / `@0x800393f8 sh v0,0(t1)`), Groessenfeld
  High-Nibble 0 = rechteckig (+u16 w, u16 h), sonst Quadrat `(size>>12)*8`; Arena je RDT-Kopf
  Byte[7] (`@0x8003928c lbu v0,7(a0)`, Kopf += hdr7*68 `@0x800392bc`), spielweit max 105; gezeichnet
  wird der deklarierte Count & 0xFF (`@0x80039358 sb t2,0(a0)`).
- **Zeichner (FUN_80039590):** SPRT Code **0x64 = opak, kein ABE**, TPage 0x95 und CLUT 0x7800
  (`ori v0,zero,0x7800 @0x80039498` = getClut(0,480)) hartkodiert; **OT-Index = depth x1**
  (`@0x80039650 lh a0,2(s3)`, `@0x80039658 sll a0,a0,2`, `@0x8003965c addu a0,a0,s6`,
  `@0x80039660 jal 0x8006b538`); OT-B `0x800AA6D8 + frameflip*0x1000`, 1024 Woerter, hoeherer Index =
  weiter weg (ClearOTagR `@0x80020c6c`, DrawOTag &ot[1023] `@0x800215d0`).
- **Figuren:** OT-Index = `otz>>4` (`@0x8002565c sra v1,v1,4`, identisch in allen 8 Zeichnern),
  Near-Gate `otz<64` (`@0x80025654`), `otz = AVSZ3 = ZSF3*(SZ1+SZ2+SZ3)>>12`, ZSF3 = 341
  (`@0x80066c70/74 li t0,0x155; ctc2 t0,$29`) -> Maske verdeckt gdw. `depth < (1023*vz)>>16`
  -> PC-Schwelle `vz >= depth*64` (`re15_pri.h re15_pri_mask_camera_z`). Eine Maske hat GENAU EINE
  Tiefe; es gibt keine Halbtransparenz und keinen Tiefen-Vergleich je Pixel.
- **Texel-Farbschluessel (GPU):** psx-spx `docs/graphicsprocessingunitgpu.md` Z.1128-1131 und
  1157-1160: `Color 0000h = Fully-transparent`, `0001h..7FFFh = Non-transparent`; Z.1167-1176: fuer
  opake Kommandos ist `8000h = Non-Transparent Black`. Der Schluessel haengt am aufgeloesten WERT,
  nicht am Palettenindex.
- **Atlas-Herkunft:** SLD-Trailer je (Raum, Cut): `L = u16 STAGE<n>.BIN[tab + raum*0x20 + cut*2]`
  (`@0x80021d4c/50/5c`), `present = u32 chunk[L-4]` (`@0x80021d6c/74`), `sld_off = u32 chunk[L-8]`
  (`@0x80021da4`), Dekompressor `FUN_800c47e8` (BIN/DEBUG.BIN +0x47E8); Port `sld_common.c`.
- **Was die Kuenstler bauen (STAGE1, aus dem Dump gemessen):** 114 Original-Cuts mit Masken, 3583
  Rechtecke; je Cut median 28 (max 79); Rechteck median 16x16 (p90 40x56); **verschiedene Tiefen
  je Cut median 19 (max 55), Spanne max-min median 126**; 50,6 % der Rechtecke haben eine Kante
  >= 32. Ein Gegenstand wird also aus vielen Rechtecken mit GESTUFTEN Tiefen aufgebaut, nicht aus
  Quadraten mit einem Median (vgl. Memory reai-v2-re2-pri-vorbild: ~5 Stufen je Objekt in RE2).

## 3. Port-Ist

- `re15_port/tools/maske/raum.py:56-57` `STD = {"segments":220, "oben":4, "grow":1, …}`;
  `:411-418` wendet `oben/grow` auf JEDES Objekt an, auch auf `png`-Objekte, wenn der Eintrag die
  Schluessel nicht traegt. Die ab 2026-09-13 eingebundenen Raeume tragen sie nicht (Tabelle 1.3).
  Warum falsch: der Nutzer hat "alles Nicht-Transparente soll ueberdecken" verlangt — und NUR das;
  die Aufweitung malt 5-18 % Hintergrundpixel mit Objekttiefe ueber die Figur.
- `re15_port/tools/maske/geom.py:43` `DEPTH_FACTOR = 0.90`; `:1207-1214` Spaltenregel
  `dep = int(z*DEPTH_FACTOR/64)`; `:381` dito in `depth_map`. Der Faktor ist an KUENSTLER-Silhouetten
  kalibriert (Kopf von geom.py), nicht an Nutzer-PNGs, deren Unterkante der echte Bodenkontakt ist;
  fuer `aufrecht`/`kollision`/`quader` wurde er schon abgeschafft (`geom.py:968-981`, `:1154-1173`),
  fuer die Spaltenregel nicht. Wirkung: 1.4(b).
- `re15_port/tools/maske/anwenden.py bau_objektweise`: Kachelung `geom.gitter_mit_kante` mit
  KANTEN 8..64, Tiefe je Kachel `np.median(win)` (`_st`), Kacheln ohne Tiefe werden verworfen
  (`if len(win)==0: continue`) -> Loecher 1.4(c), Staffelung 1.4(d).
- `re15_port/tools/maske/geom.py:1184-1206/1207-1242`: Bodenkontakt je Spalte OHNE Pruefung, ob die
  unterste Zeile den Boden ueberhaupt trifft (Zeile ueber dem Horizont -> keine Tiefe) oder ob der
  Weltpunkt im Raum liegt (die Huellen-Pruefung existiert nur im `aufrecht:"spalten"`-Zweig
  `:1097-1119`); Tiefen bis 1023 (1000 C1 531, C3 496).
- `analysis/esp_masken_2026-09-03/auswahl.json` `ROOM10D0/"6"`: `{"name":"Stuhlkante","png":
  "pri/STAGE1/10D0/06_01.png","x":34,"y":172,"aufrecht":true}` — Lage des Nutzer-Originals, Datei
  ist meine 65x120-Fassung (1.5). `ROOM10D0/"7"` Quader `[x,z,w,d,h]` mit `minus 58,0,320,240`;
  `ROOM10F0/"4"/"5"` acht bzw. vier Quader `nur_kunst` + Tiefschwarz-Etiketten (`raum.py:289-322`);
  `ROOM1000/"0"/"2"`, `ROOM11F0/"0"` Polygone; `ROOM1100` nur Quader — alle ohne Nutzer-PNG.
- `re15_port/tools/maske/atlas.py` CLUT: `q = (pal+4)>>3`, `clut[1..] = r|g<<5|b<<10` — Schwarz wird
  0x0000 (auf PSX durchsichtig, psx-spx). Gemessen `clut0000`: 1000 C7 12556, C8 13468, 1180 C2 12264,
  10A0 C5 8939, 1000 C6 6384 Texel.
- Laufzeit PC: `platform/pc/src/bg_pc.c` `pri_publish_tim` Index 0 -> Alpha 0, sonst Alpha 255;
  `render_pc.c:1842-1861` Atlas-Textur RGBA8888 `SDL_BLENDMODE_BLEND`, keine Alpha-Modulation, keine
  Skalierungs-Hint (nearest); `:830-943` Masken nach `depth*64` absteigend in die Dreiecksfolge
  eingemischt (`mask_due`), Rest obenauf; `main.c:4696-4772` Liste nur bei (Raum,Cut)-Wechsel,
  `n = pri.draw_count`. **Es gibt keinen Pfad, der eine Maske halbtransparent (ABE) oder mit anderem
  Farbschluessel zeichnet** — "halb transparent" ist ausschliesslich die Kachel-Staffelung (1.4 b/d).
  Einzige Laufzeit-Divergenz zum Original: Index-0- statt Wert-0x0000-Schluessel (nur auf PSX sichtbar).
- Varianten: `RE15_NEWGAME_ROOM 0x1240` (`re15_gameflow.c:16`), Tueren tragen das Varianten-Nibble
  weiter (`aot_common.c:493-498`); MSK gibt es fuer STAGE1 nur fuer gerade Raum-IDs (+1221). Im
  normalen Durchlauf bleibt die ID gerade — kein Befund, aber eine Luecke fuer Szenario B.

## 4. Fix-Plan (Phase 2 — ausschliesslich aus den Nutzer-PNGs, jede Maske geprueft)

Reihenfolge, Werkzeugkette, Riegel. Keine Engine-Aenderung noetig ausser (7).

1. **Nutzer-Originale zurueckholen** (Datei = Blob des Nutzers):
   `git show 61607499:pri/10D0/06.png > pri/STAGE1/10D0/06.png` (06_01/06_02 loeschen, Eintrag
   `"png":"pri/STAGE1/10D0/06.png","x":34,"y":172`), `61607499:pri/10D0/01.png` (01_01/01_02 loeschen,
   EIN Objekt x=124 y=87), `61607499:pri/10D0/07_01.png` (x=0 y=123), `45a81425:pri/1130/03.png`
   (x=79 y=0), `61607499:pri/10E0/07_01.png` (x=110 y=97). Riegel: `git hash-object` jeder
   `pri/STAGE1/**.png` == erste Git-Fassung (Skript `pri-masken-audit/…` Abschnitt 1.5) als ctest.
2. **Nur Nutzer-Quellen in STAGE1:** alle `polygon/quader/kaesten/zauberstab`-Objekte aus
   `auswahl.json` fuer ROOM1000 C0/C2, ROOM1100 C1/C2, ROOM10D0 C7 (Quader), ROOM10F0 C4/C5 (Quader +
   Tiefschwarz), ROOM11F0 C0 entfernen; die MSK-Sektionen dieser Cuts fallen weg (1100.MSK ganz).
   `raum.py`: fuer STAGE1 abbrechen, wenn ein Objekt keine `png`-Quelle hat.
3. **Silhouette = PNG, punktgenau:** `raum.py` Default fuer png-Objekte `oben=0, grow=0, fuellen=aus`
   (STD nur noch fuer Altquellen); Alpha-Schwelle bleibt 110 (Anti-Alias-Saum 1-2 px, `1.3`), es sei
   denn der Nutzer will `>0`; Massstab-4-PNGs (1000 C1/C3, 11F0 C1) beim Nutzer als 1x-Freistellung
   nachfragen (BOX-Verkleinerung verliert duenne Teile, Treffer nur 85-93 %). Riegel A (Bau):
   `TREUE` in `raum.py:606-630` wird zum Abbruch (`fehlt==0 and zuviel==0`), sonst keine Datei.
   Riegel B (ctest, Engine-Leser): `raum.py` schreibt je Cut die Ziel-Silhouette als
   `MASKS/ROOM####_PRI##.PBM`; neuer Test `test_pri_silhouette` rastert wie die Sonde
   (`re15_pri_parse_section` + `re15_tim_parse`, Index!=0, `draw_count`) und verlangt Deckung == PBM
   bitgenau fuer JEDEN Cut mit PBM.
4. **Tiefe ohne Faktor, aus Geometrie, je Pixel:** `DEPTH_FACTOR` fuer PNG-Objekte auf 1.00
   (`geom.py:1214`, `:1202`), Beleg: 1.4(b) — ueber die 42 Spaltenregel-Cuts (6912 Standplaetze VOR
   der Standlinie) faellt "voll verdeckt" von 1789 auf 787 allein durch den Faktor; die Kollisions-/Quader-Zweige rechnen seit 2026-09-07 mit 1.00
   (`geom.py:968-981`). Tiefenquelle je Pixel in dieser Reihenfolge: (i) Sehstrahl gegen die
   SCA-Sperrzelle, die das Objekt IST (Zuordnung ueber Silhouetten-Deckung wie `raum.py:249-287`,
   Wandzellen als Saeule, Kreis/Moebel als Quader; Beleg Original: Median 10,5 Einheiten zu Capcoms
   Tiefen ueber 36 Winkel, `geom.py:714-718`); (ii) sonst Bodenkontakt je Spalte MIT Huellen-Pruefung
   (Weltpunkt in der SCA-Huelle, `geom.py:1097-1112`) und Spalten-Erbe fuer Spalten ohne Treffer
   (`:1117-1119`) statt Verwerfen — das schliesst die 1000-C3-Loecher; (iii) je Bildzeile das
   Senkrechten-Profil `vz_der_senkrechten` fuer alles, was ueber seinem Standpunkt aufragt.
   Multi-Band-Raeume (10A0, 1060, 10C0, 10F0, 1120): Ebene = Band der Zelle bzw. `ebene`, Standplaetze
   je Band (Sonde liefert sie).
5. **Rechtecke tragen EINE Tiefenstufe:** Zerlegung nicht in Quadrate mit Median, sondern adaptiv
   (Streifen entlang des Tiefengradienten: Spalten-Streifen fuer diagonale Bodenlinien, Zeilen-Streifen
   fuer Senkrechten-Profile), Teilung so lange, bis `max-min` der Tiefe innerhalb der opaken Pixel eines
   Rechtecks <= 1 Stufe (64 Einheiten) ist; Tiefe des Rechtecks = sein MAXIMUM (fernster Punkt: ein
   Rechteck darf nur verdecken, wenn es GANZ vor der Figur liegt — dieselbe Regel wie fuer die
   Kollisionstiefe, `anwenden.py` Kommentar "Maximum", Nutzer-Marken F468/F508). Ueberschreitet ein Cut
   105 Rechtecke oder 65536 Atlaspunkte: LAUT abbrechen und das Objekt beim Nutzer melden — nie
   vergroebern (heute `_waehle` -> 20-32-px-Kacheln, 1.6). Massstab: die Kuenstler kommen mit median
   28 Rechtecken und 19 Tiefenstufen je Cut aus (Abschnitt 2).
6. **Abnahme je Cut, bevor die Datei geschrieben wird** (Werkzeug: die Sonde + `pri_audit.py`/`audit3.py`
   aus `pri-masken-audit/`, in `raum.py` eingebaut): (a) Deckung == PNG; (b) Standplatz-Schiene: kein
   begehbarer Standplatz VOR der Objekt-Standlinie (Zelle bzw. PNG-Bodenlinie) zu >=95 % verdeckt
   (`VORverd == 0`), Standplaetze DAHINTER zu >=95 % verdeckt (`HINTfrei` -> 0, Restliste ausgeben);
   (c) Bild je Cut wie `ROOM1010_C6_halo.png` (PNG/Maske/Kacheltiefen) fuer den Nutzer; (d) VORHER die
   Tiefenregel an den 114 STAGE1-Original-Cuts kalibrieren: Silhouette aus dem SLD-Atlas, Regel (4)+(5)
   anwenden, je Kuenstler-Rechteck Tiefe vergleichen — Ziel Medianfehler ~0 und beide Richtungen
   ausgeglichen (heute mit 0.90: +2 / 35,5 % zu nah / 41,8 % zu fern laut `geom.py` Kopf; die
   Quadertiefe schafft -1,0 / 21,7 / 21,6 %). Erst wenn diese Zahl steht, werden Nutzer-Cuts gebaut.
7. **PSX-Farbschluessel** (`atlas.py`): CLUT-Eintraege mit Wert 0x0000 auf 0x8000 setzen (opakes Schwarz
   fuer das opake SPRT 0x64, psx-spx Z.1167-1176); PC-Seite unveraendert (`pri_publish_tim` Index-0).
   Test `unit_sld_atlas`/`integration_pri_masken` um "kein Eintrag 0x0000 ausser Index 0" erweitern.
8. **Pins/ctests:** `test_pri_silhouette` (3), Hash-Riegel (1), `test_pri_kopfschnitt` um die
   Standplatz-Zahlen VOR/HINTER je Cut erweitern (Bodenlinie aus dem PBM + `ebene`-Sidecar), Schranke
   `VORverd == 0` fuer alle STAGE1-Cuts mit PBM; `integration_pri_masken` Schranke "dateien >= 32"
   auf die reale Menge nach (2) anpassen (heute 54 Dateien).

Risiken: (4i) braucht je Objekt eine Zelle — Objekte ohne Zelle (Fahnen, Kamerastativ, Wandbilder)
laufen in (4ii)/(4iii) und brauchen `fuss`/`ebene` wie heute; (5) kann fuer breite, flache Objekte
(Tischplatten, 10D0 C1 `flach`) mehr als 105 Rechtecke verlangen -> laut melden; die 1060-Gelaender
(Tiefe von der OBERKANTE, `ebene` -15400 usw.) sind ein eigenes Modell und in 1.4 nicht bewertet.

## 5. Offen / nicht belegt

- Die Bildnummern des Nutzers (10C0 F423, 10D0 F1078/F3727/F4801, 1050 F724/F209/F676, 1000 F709…,
  10A0 F116…, 11F0 F158/F626) sind keinem Cut zuordenbar; die Fehlerklassen kommen aus der Messung
  aller Cuts, nicht aus seinen Bildern. 11F0 C1 beruehrt 0 Standplaetze (Objekt 32x38 px) — falls
  F158/F626 dort liegen, ist die Ursache nicht gemessen.
- Das VOR/HINTER-Kriterium nimmt die unterste PNG-Zeile je Spalte als Standlinie. Fuer 10C0 C3
  (Holzbank, Kollisionstiefe: 317 von 426 Vorne-Plaetzen voll verdeckt) und 1130 C3 (139/174) kann die
  Zelle die wahre Standlinie sein — nicht entschieden; fuer 1060 (Oberkanten-Modell) nicht anwendbar.
- Ob der Nutzer die frueher abgenommenen Nicht-PNG-Objekte (10F0 C4/C5 Stuehle, 2026-09-09 "Ok,
  einwandfrei") wirklich entfernt haben will, folgt aus seiner heutigen Aussage ("nichts weiter …
  als diese zu verwenden"), ist aber nicht rueckgefragt.
- Alpha-Schwelle 110 gegen ">0" (13..634 px Saum je PNG): Nutzer-Entscheidung.
- PSX: Wert-0x0000-Loecher sind aus psx-spx abgeleitet, nicht auf Hardware/DuckStation mit unserem
  Atlas gemessen (PSX-Target baut nicht).
- Der Tiefenvergleich der Engine (Kamera-z je Dreieck) ist in der Sonde durch ein Koerpermodell
  (Senkrechte 1500, +-450) ersetzt; die Engine-Schiene deckt nur den Spieler ab
  (`reai-v2-runde15`, 14,2 %). Zahlen sind Standplatz-Zaehlungen, keine Bild-Diffs.
- Nicht gemessen: ob `massstab 4` fuer 1000 C1/C3 und 11F0 C1 wirklich der Wunsch des Nutzers war
  (Treffer 85-93 %) oder die PNGs als 1x gemeint sind — nachfragen.
