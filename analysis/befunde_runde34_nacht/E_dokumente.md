# Spur E — Vier neue Dokumente (Welt-Prop, Aufnahme, FILE-Leser) in ROOM1050/1000/1020/1010

Stufe: ERMITTLUNG + BAUPLAN (noch kein Port-Code). Arbeitsbaum `.claude/worktrees/r34n_dokumente`,
Zweig `r34n/dokumente`, aufgesetzt auf cf0e68ba (master + Auftrag/Vertrag der Nacht).
Vorbild in allen Punkten: Irons Diary (Runde 30, `analysis/befunde_runde30/irons-diary-welt.md`,
`irons-diary-dokument.md`, `nachtrag-diary-en.md`).

Status: IN ARBEIT — Abschnitte werden fortlaufend gefuellt und committet.

Werkzeuge dieser Spur (alle neu, alle im Baum):

| Werkzeug | Zweck |
|---|---|
| `re15_port/tools/r34n_e/texte_aus_auftrag.py` | zieht die vier Texte WOERTLICH aus `AUFTRAG.md` -> `E_texte/dok*.txt` |
| `re15_port/tools/r34n_e/doc_satz_brief.py` | Satz FILE26..29 auf `re2_doc_satz.py` (Atlas/Glyphen/TIM unveraendert) + drei gemessene Erweiterungen (3.2) |
| `re15_port/tools/r34n_e/kontaktbogen_satz.py` | Kontaktbogen der gesetzten Seiten im RE2-Schirmlayout + Weltmodell daneben |
| `re15_port/tools/r34n_e/marken.py` | Cut + rote Marke der Nutzerbilder (Differenz gegen jeden Hintergrund, exakte Markenfarbe) |
| `re15_port/tools/r34n_e/geom.py` | Kamera-Geometrie aus der Engine-Sonde (echte Inverse), Triangulation |
| `re15_port/tests/unit/probe_r34n_e_dokumente.c` + `probes/r34n_e_dokumente.cmake` | Engine-Sonde (kamera, projekt, strahl, sicht, aots, zonen, zonenliste, sca, stand, abdeckung, druck, licht) — reine Messung, kein add_test |

Messausgaben (unversioniert): `build/r34n_e/` im Arbeitsbaum. Belegbilder: `E_belege/`.

## 0 Kurzfassung

(folgt am Ende)

## 1 Nutzerwortlaut + Lesart

### 1.1 Wortlaut

`AUFTRAG.md` Zeilen 20–58 (die vier Dokument-Abschnitte). Die Texte sind NICHT abgetippt, sondern
per `texte_aus_auftrag.py` aus dem Zitat gezogen (Zitatpraefix `>` + ein Leerzeichen und die
Aufzaehlungs-Tabs entfernt, Beschriftungen `Überschrift: ` / `Text: ` entfernt, `>`-Leerzeile =
Leerzeile). Bericht `E_texte/texte_bericht.txt`:

| Dok | Titel (Nutzer) | Text (AUFTRAG.md) | Zeichen | Zeilen (leer) | Woerter | md5 | Zeichen ausserhalb ASCII |
|---|---|---|---|---|---|---|---|
| 1 | Police Officer's Final Diary Entry | Z. 23–27 | 499 | 5 (0) | 97 | 9c8bbf64… | keine |
| 2 | Elliot's Diary | Z. 30–38 | 648 | 9 (4) | 115 | 9d796a1b… | keine |
| 3 | Marvin's Notes | Z. 42–48 | 300 | 7 (1) | 54 | 5d63de08… | keine |
| 4 | Armory Notice | Z. 56–58 | 312 | 3 (0) | 50 | e503898e… | keine |

Dateien: `E_texte/dok1_police_officer.txt`, `dok2_elliot.txt`, `dok3_marvin.txt`, `dok4_armory.txt`
(+ `*_titel.txt`). Apostrophe sind ASCII `'`, Auslassungspunkte drei ASCII-Punkte, keine
typografischen Zeichen. Ortsangaben des Nutzers: Dok 1 „auf der sitzenden Leiche in ROOM 1050,
vor dem Rolltor", Dok 2 „ROOM 1000 auf der Bank (Siehe elliot.bmp) für die Position etwa",
Dok 3 „Room 1020 … an der Stelle von Marvins Desk - markiert in marvin.bmp", Dok 4 „Room 1010 …
Ort in etwa markiert in interrogation.bmp".

### 1.2 Lesart (Mehrdeutigkeiten aufgeloest)

1. **Titelseite in Versalien, Listenname gemischt** (Runde-30-Regel): alle 25 RE2-Titelseiten
   sind Versalien (437 von 437 Titel-Glyphen), RE2s Listennamen gemischt. Titelseite
   „POLICE OFFICER'S FINAL DIARY ENTRY" / „ELLIOT'S DIARY" / „MARVIN'S NOTES" / „ARMORY NOTICE";
   Listenname und Meldung „The <Name> has been filed." mit dem Nutzerwortlaut in gemischter
   Schreibung.
2. **Jede Zeile des Nutzertexts beginnt eine Satzzeile, Leerzeilen bleiben** (Satzregel des Irons
   Diary, `irons-diary-dokument.md` §6.3). Dok 1 und Dok 4 haben keine Leerzeilen — der Nutzer
   hat dort Zeilen ohne Absatzabstand geschrieben, so wird gesetzt.
3. **Papier und Modell** (Nutzer: Dok 1 „Weltmodell wahrscheinlich", Dok 3 „entspräche
   wahrscheinlich am ehesten") — geprueft in 3.1 an RE2s EIGENER Zuordnung und am Kontaktbogen:
   Dok 1, 2, 3 sind RE2-Originalpaare, Dok 4 ist ein stimmiges Paar aus Geschwistern. Keine
   Aenderung an der Nutzerwahl noetig.
4. **„Modell das gleiche wie bei Irons Diary"** (Dok 2) = `mesh03_cf9f316d` (RE2 ROOM10E0,
   Secretary's diary B), Papier wie Irons Diary = FILE08-Illustration (VERTRAG 1.4).
5. **„Marvin Branagh"** ist eine Unterschrift. RE2 setzt einzeilige Unterschriften
   rechtsbuendig (3.2) — der Satz folgt dem; Wortlaut und Zeilenfolge bleiben unveraendert.
6. **„auf der sitzenden Leiche … vor dem Rolltor"**: die Leiche ist gemalt (kein Modell), sie
   sitzt an der Ostwand des Flurs direkt noerdlich des Rolltor-Schalters (2.4). „Auf" = auf
   dem Schoss/Oberschenkel.

## 2 Ist-Zustand im Port (gemessen)

Bau des Arbeitsbaums (cf0e68ba + Sonde), `local_build.sh configure/build` exit 0. Alle Zahlen
unten aus der Engine-Sonde `probe_r34n_e_dokumente` (echter Raumstart `scd_room_reenter` samt
aller Port-Installer) bzw. aus den ausgelieferten RDTs (`shared_assets/PSX/STAGE1`).

### 2.1 Dokument-System heute

* `re15_files.c`: die Dokument-Tabelle fuehrt genau EIN Dokument (Nr 0 = Irons Diary, Item 0x48,
  Bildsatz 25, max_page 15, H 144). `re15_files_doc_from_item` liefert fuer 0x49..0x4C −1
  (`doc >= DOC_COUNT`) — eine Zone mit diesen Ids taete heute NICHTS (weder Leser noch
  Item-Modal, `aot_item_dokument` → `re15_menu_request_doc` → `re15_files_doc(doc) == NULL`).
* `shared_assets/RE2/FILES/`: FILE00..FILE25, kein FILE26..29.
* Speicherstand v9 traegt `files[24]` (Dokument-Nummern roh, `re15_files_export/import`) und die
  Zone-9-Bits in `flags` — Dokument-Nummern 1..4 und Bits 57..60 brauchen KEIN neues Format
  (Pruefung in 5.6).

### 2.2 Raeume, Varianten, Slots, Props (nach dem echten Raumstart)

| Raum | nCut | nOmodel = Props | aktive AOT-Slots < 48 | erster freier | Nachrichten im RDT |
|---|---|---|---|---|---|
| ROOM1000 / 1001 | 9 | 2 (obj 0,1 Items im Duschbereich, y 0) | 0..9 | 10 | 1 (msg 0 „Nothing unusual.") |
| ROOM1010 / 1011 | 9 | 3 (obj 0,1,2 Items, y −1600) | 0..7 / 0..8 | 8 / 9 | 3 / 24 |
| ROOM1020 / 1021 | 13 | 7 / 6 (4 Deckenventilatoren, 2 Items y −1500, Leiche Roy) | 0..12 / 0..13 | 13 / 14 | 6 / 15 |
| ROOM1050 / 1051 | 10 | 2 (obj 0 Rolltor, obj 1) | 0..10 / 0..12 | 11 / 13 | 9 / 6 |

Zone-9-Bits 57..60 sind nach dem Raumstart in allen acht Raeumen 0.
Messdateien `build/r34n_e/aots_<raum>.txt`, `scd_<raum>.txt`, `msg_<raum>.txt`.

### 2.3 Die Nutzerbilder: Cut und Marke

`marken.py` (Differenz gegen JEDEN Hintergrund des Raums, Engine-Dekoder `probe_bg_dump`; Marke =
abweichende Pixel mit exakt (237,28,36)):

| Bild | Raum | Cut (Abweichung / naechstbester) | Marke | Mitte (Pixelmitten) |
|---|---|---|---|---|
| elliot.bmp | 1000 | **0** (4,22 / 77,54) | x 182..203 y 162..182, 462 px voll | (193,0 ; 172,5) |
| marvin.bmp | 1020 | **6** (4,49 / 107,50) | x 151..161 y 101..109, 99 px voll | (156,5 ; 105,5) |
| interrogation.bmp | 1010 | **0** (3,80 / 76,64) | x 208..231 y 166..187, 528 px voll | (220,0 ; 177,0) |

Fuer ROOM1050 gibt es kein Bild; die sitzende Leiche ist GEMALT (Hintergrund Cut 3 und Cut 9,
kein Modell). Die Nutzerbilder sind der reine Hintergrund (Abweichung 3,8..4,5) — 3D-Props sieht
man darin nicht (wichtig fuer ROOM1010, 2.7).

### 2.4 ROOM1050: wo die Leiche sitzt

* Kollision: SCA-Zelle 12 (und ihre vier Quadranten-Kopien) **x[15700..17200] z[−7850..−5950]**
  an der Ostwand (SCA 1 x[17200..18100]), direkt noerdlich des Rolltor-Schalters (Slot 7
  x[16800..17600] z[−8950..−8150]); Rolltor-Linie SCA 19 z[−10600..−10200].
* Kameras: im Spiel sieht man sie nur in **Cut 3** (RVD-Zone 8 „Cut 3", gilt fuer den ganzen Flur
  z −12500..−1300); **Cut 9** (flag 1, Nahaufnahme) hat nur eine Kopfzone ausserhalb des Flurs
  (19600..22100, −6900..−4900) und wird nur per `Cut_chg 9` gezeigt — in ROOM1051 sub03 @0x00DB4.
* Schoss/Oberschenkel trianguliert (`geom.trianguliere`, Engine-Matrix): Hautfleck der Hand auf
  dem Schenkel Cut 9 (157,67;167,83, 48 px) mit Cut 3 (136,06;142,39, 18 px) →
  **(16474, −347, −6592)**, Strahlabstaende 6,0 / 6,0, Rueckprojektion 0,8 / 0,2 px;
  ±1 px in Cut 3 verschiebt um ±45 (x) / ±13 (y).
* Vorhandener Untersuchen-Satz: **nur ROOM1051 (Elza)**: main00 @0x00C0E
  `2c 0b 03 31 00 00 92 3b ae e3 e8 03 e8 03 ff 00 18 03 00 00` = Slot 11, sce 3, Rechteck
  (15250,−7250,1000,1000) → sub03 @0x00DA4: `Plc_motion`, `Cut_chg 9`, `Message_on 5` („He's got a
  big bite on his neck … Sorry, I'll just borrow this..."), `Aot_on 12` (Item_aot_set Slot 12 @0x00C22
  = Id 0x04 x15, Bit 165, Null-Rechteck), `Cut_chg 3`; sub01 @0x00CB2 (jedes Bild): Bit (9,165)
  gesetzt → `Aot_reset 11`. In ROOM1050 (Leon) ist msg 5 toter Text, Slot 11/12 leer.

### 2.5 ROOM1000: Bank

* SCA-Zelle 7 x[18800..20650] z[−12450..−2750] = beide Mittelbaenke (Cut 0: die rechte Bank im
  Bild ist Welt x 18800..~19725).
* Nur Cut 0 sieht die Marke (Sonde `projekt`: Cut 1 y 1061, Cut 2 hinter der Kamera …).
* Hoehe der Sitzflaeche: 3.5.

### 2.6 ROOM1020: Marvins Schreibtisch

* SCA-Zelle 2 x[−10893..−9197] z[−17864..−15064] (floor 2 wie der Irons-Tisch ROOM1150 SCA 1).
* Die Marke liegt mitten auf der Platte (Rueckprojektion 82 Einheiten neben der Zellenmitte).
* Der Tisch ist in Cut 3, 5, 6, 8 zu sehen; Cut 6 ist der des Nutzerbilds.
* **Untersuchen-Satz des Tischs**: ROOM1020 main00 @0x01F0E `2c 0a 01 31 00 00 a4 d4 4c b9 98 08 e4 0c
  03 00 ff ff 00 00` = Slot 10, Nachricht 3 „It's Lieutenant Branagh's desk.", Rechteck
  x[−11100..−8900] z[−18100..−14800] — deckt den GANZEN Tisch; ROOM1021 dieselbe Zone als Slot 11,
  Nachricht 12 (@0x01F6C).

### 2.7 ROOM1010: Verhoertisch — das Original-Item unter der Marke

* SCA-Zelle 5 x[−50..2450] z[1550..5900] = der Tisch (Projektion der Zellenkanten auf y −1600
  deckt sich mit der gemalten Platte, Bild `E_belege/r1010_tisch_zelle_items.png`: gruen = Zelle auf
  y −1600, gelb = Spray-Dose, orange = ihre Zone Slot 2, blau = Munitions-Zone Slot 3).
* **Unter der roten Marke steht im Spiel das Original-Item First Aid Spray**: ROOM1010.RDT sub00
  @0x00996 `50 02 09 31 00 00 a2 fe 50 14 e8 03 e8 03 22 00 01 00 8c 00 00 00` (Slot 2, Id 0x22,
  Bit 140, Prop 0) + Obj_model_set @0x00930 obj 0 bei **(200, −1600, 5500)** rot_y 3084;
  ROOM1011: Id 0x39 (@0x00954) an derselben Stelle. Modell obj 0: bbox x −79..79, **y −489..0**,
  z −90..90 (Sonde `modelle`) — eine 489 hohe Dose. Ihr Fuss projiziert in Cut 0 auf
  (227,9 ; 177,7), 8 px neben der Markenmitte. Der Nutzer hat sie im reinen Hintergrundbild nicht
  sehen koennen.
* Zweites Item auf dem Tisch: obj 1 (Id 0x15 x15) bei (1800, −1600, 5750), Slot 3.

### 2.8 Licht in den vier Raeumen (RDT @0x2C, 40 Byte je Cut)

ROOM1000, ROOM1010, ROOM1050: ALLE Cuts tragen denselben Standard-Satz
`02 00 01 00 00…00 53 4e 4e 00…` (scale 2, Lichtfarben 0, ambient (83,78,78)) — derselbe wie der
Item-Box-Schirm ROOM1150 Cut 8. Vertexfarbe damit fuer jede Normale (41,39,39) = Texel × 0,32 —
fuer JEDES Objekt dieser Raeume (Sonde `licht`), also auch Spieler, Gegner, Items. ROOM1020 hat echte
Lichtsaetze (ambient 63,68,68 + Lichter), Cut 6 an der Tischstelle: Normale −Y (50,52,52) = ×0,39.
Folge (Kontrollabzug 4): die Dokument-Modelle werden in 1000/1010/1050 sehr dunkel (5.4, Risiko 7.1).

## 3 Original-/RE2-Mechanismus (Adressen, Bytes, Instruktionen)

### 3.1 Papier und Weltmodell: RE2s eigene Zuordnung und der Kontaktbogen

RE2 verbindet Item-Id, Dokument und Bildsatz fest: Dokument-Nr = Id − 0x68
(`sltiu v0,a3,0x68` @0x80071bbc, `addiu a0,a3,-104` @0x80071d04, `info/re2leon/PSX.EXE`), Bildsatz
FILEnn = Dokument nn (Seitenlader liest den ersten Slot aus @0x800A9AD0[doc], `lbu a0,-25904(at)`
@0x8006d480). Die Weltmodelle stehen im RE2-Zensus `extracted_re2_dokumente/weltmodelle/_index.txt`
(269 Item-AOTs, 22 Dokument-Platzierungen, 12 verschiedene Meshes):

| Modell (Nutzer) | RE2-Platzierung | Id → Dokument → Bildsatz | Nutzer-Papier | Ergebnis |
|---|---|---|---|---|
| mesh00_0541704e | room1150 sub00 @0x015A2/@0x0169C, Slot 7 | 104 = 0x68 → Dok 0 „CHRIS's diary" → FILE00 | FILE00 | **RE2-Originalpaar** |
| mesh03_cf9f316d (= Irons Diary) | room10E0 sub00 @0x01D26 | 112 = 0x70 → Dok 8 „Secretary's diary B" → FILE08 | FILE08 (wie Irons Diary) | **RE2-Originalpaar** |
| mesh01_ae2d0a30 | room2020 sub00 @0x01D3E | 106 = 0x6A → Dok 2 „Memo to LEON" → FILE02 | FILE02 | **RE2-Originalpaar** |
| mesh04_cab7b32d | room60A0 sub18 @0x01470 | 114 = 0x72 → Dok 10 „User registration" → FILE10 | FILE06 | Geschwisterpaar, s. u. |

Dok 4 im Einzelnen (gemessen, nicht angenommen):
* `FILE06_title_paper.TIM` ist **byte-gleich** `FILE02_title_paper.TIM` (md5 beider
  `4b2d7f1c4110e224839f998a8300f886`) — die Illustration von „Mail to the chief" IST das gefaltete
  Blatt von „Memo to LEON".
* `mesh04` und `mesh01` haben **dieselbe Geometrie**: beide 404 B, 8 Dreiecke, bbox
  x −144..142 / y −98..0 / z −225..211; `cmp -l` zaehlt 32 abweichende Bytes, alle im UV-Block
  (Datei-Offset 310…402, u/v je +0x40) — gleiches Blatt, anderer Texturausschnitt.
* Das RE2-Dokument 6 hat selbst **kein** Weltmodell (room3010 @0x01B90/@0x01CC0: md1 = 255).
  FILE10 (das eigene Papier von mesh04) ist ein zerknuellter handschriftlicher Zettel; mesh04s
  Textur zeigt dagegen ein getipptes, gefaltetes Blatt (Render `mesh04_cab7b32d_a.png`) — sie passt
  also zu FILE06/FILE02, nicht zu FILE10.
* Folge: FILE06 + mesh04 ist kein RE2-Paar, aber ein stimmiges (Papier = FILE02-Blatt, Modell =
  mesh01-Geometrie mit getipptem Blatt). Die Nutzerwahl bleibt.

Hinweis ohne Handlungsbedarf: die FILE00-Illustration traegt auf dem Buchschild die Aufschrift
„Daily Report / Chris Redfield" (4x-Ausschnitt geprueft). Der Nutzer hat FILE00 ausdruecklich
gewaehlt; der Leser zeigt sie unter dem Text wie beim Irons Diary die FILE08-Illustration.

Kontaktbogen (aus den GESCHRIEBENEN TIM gelesen, RE2-Schirmlayout: Grund schwarz, Illustration
(100,60), Textseite (25,30); links das Weltmodell): `E_belege/kontaktbogen_satz.png`.

### 3.2 Satz: was RE2 vorgibt (gemessen an RE2s Seiten)

Grundlage bleibt `re2_doc_satz.py` (Atlas 85 Zeichen, 698/1048 Originalzeilen pixelgenau, Font-
Datei md5 `8cbe2c52…`, neu erzeugt in `build/r34n_e/atlas/`). Das Irons Diary brauchte nur die
Tagebuch-Regel „je Datum eine Seite"; die vier neuen Texte haben kein Datum. Drei Dinge wurden
dafuer an RE2s eigenen Seiten gemessen (`doc_satz_brief.py`, Bericht je Dokument
`build/r34n_e/satz/FILEnn/FILEnn_satz.txt`):

1. **Satzspiegel je Vorlage** (Modus der Stiftlage (Kern-x − Linkslage) ueber die Zeilenanfaenge
   mit gesicherten Kleinbuchstaben; Grenze = groesstes Kern-x der Vorlage):

   | Vorlage | H | Zeilen/Seite | Rand (Verteilung) | Grenze x |
   |---|---|---|---|---|
   | FILE00 (Dok 1) | 144 | 9 | 9 (9:17, 8:4, 10:3) | 253 |
   | FILE08 (Dok 2) | 144 | 9 | 10 (10:10, 12:4) | 249 |
   | FILE02 (Dok 3) | 176 | 11 | 10 (10:5) | 252 |
   | FILE06 (Dok 4) | 176 | 11 | 4 (4:48) | 253 |

2. **Titel ueber zwei Zeilen.** Breiteste einzeilige RE2-Titelzeile: FILE23 „HINT FILES FOR THE
   ROOKIE MODE" x 4..250 = 247 px. Mehrzeilige RE2-Titel (Atlas-Bericht, Titelbaender):
   FILE22 „INVESTIGATIVE REPORT ON" / „P-EPSILON GAS" Baender y 76/92, FILE20 y 68/84(/100),
   FILE01 y 52/68/84 — **Zeilenabstand 16** in allen drei, **jede Zeile fuer sich mittig**
   (Mitten 128,5/126,5; 127,5/126,0; 126,0/128,0/125,5). Blocklage: FILE20 und FILE22 um H/2 = 88
   (2 von 3; FILE01 um 72). Einzeilige H-144-Titel stehen alle auf Oberkante 64 = Mitte 72 = H/2.
   „POLICE OFFICER'S FINAL DIARY ENTRY" ist einzeilig 281 px > 247 → zwei Zeilen; Umbruch am
   Wortende mit der schmalsten breitesten Zeile: „POLICE OFFICER'S" (x 62..192) / „FINAL DIARY
   ENTRY" (x 57..197), Oberkanten 56/72 (Block um H/2 = 72). Die drei anderen Titel passen in eine
   Zeile (114 / 115 / 110 px) und stehen auf der Rasterzeile ihrer Vorlage (Oberkante 64 / 64 / 80).
3. **Einzeilige Unterschrift rechtsbuendig.** RE2 „William Birkin" (FILE05/FILE06 S03/S06/S09):
   Kern-x1 250, 249, 250, 250, 249, 250 → Modus **250**; ebenso „End of report." FILE14 x1 249.
   Mehrzeilige Unterschriftsbloecke stehen dagegen links buendig auf einer Einrueckung (FILE01 66/67,
   FILE02 99/100, FILE18 74). „Marvin Branagh" ist einzeilig → Kern-x1 250 (Stift 134).

### 3.3 Glyphenabdeckung und Seitenzahl (gemessen am Satz)

`zeichen_pruefung.txt` je Dokument: **0 fehlende Glyphen, 0 konstruierte Glyphen** in allen vier
Texten und Titeln (wie beim englischen Irons Diary). Nicht gesicherte Metrik, deren Vorschub im
Text WIRKT: nur „4" in „4312" und „5" in „5632" — beide Vorschub 9, Linkslage 0 bzw. 1; das ist
der Vorschub ALLER acht gesicherten Ziffern (0,1,2,3,6,7,8,9: je 9, n = 19/19/19/5/7/4/7/6),
die Ziffern sind im Original gleichbreit gesetzt. Sonst nur `!` und `Y` (je 0 Wirkung, am Zeilen-
bzw. Wortende).

| Dok | Bildsatz | Vorlage | H | Textseiten | **max_page** | Leserseiten | Seitenaufteilung |
|---|---|---|---|---|---|---|---|
| 1 | FILE26 | 00 | 144 | 3 | **3** | 4 | p01 9 Z., p02 9 Z., p03 2 Z. („good care of yourself and" / „Mom...") |
| 2 | FILE27 | 08 | 144 | 4 | **4** | 5 | p01 9, p02 9, p03 9, p04 3 |
| 3 | FILE28 | 02 | 176 | 2 | **2** | 3 | p01 11 (bis „this code with anyone else."), p02 2 („Thanks!" / Unterschrift) |
| 4 | FILE29 | 06 | 176 | 2 | **2** | 3 | p01 11 („… with the code:" / „5632" / „We hope these reserves make"), p02 2 |

Die Leerzeile vor „Thanks!" (Dok 3) faellt auf den Seitenkopf von p02 und entfaellt dort (Regel
„keine Leerzeile am Kopf einer Folgeseite", §6.3). In Dok 4 bricht der Code auf eine eigene Zeile
um (bei Rand 4 endet „access the room with the code:" bei Kern-x 249, mit „ 5632" bei 293 > 253).

### 3.4 Aufheben: Aktions-Scan, Vorrang und die Original-Regel "Gegenstand vor Nachricht"

Der Aktions-Scan feuert je Druck genau EINEN Satz — den ersten in Slot-Reihenfolge, dessen
Geometrie trifft (FUN_80042bac; Port `aot_common.c` `re15_aot_scan` mit `action_fired`, Test je
Satz: Mitte bei sat&0x40, sonst Punkt 620 voraus bei sat&0x20, `ori v0,zero,0x26c` @0x80042bd0).
Ein Dokument ist eine Item-Zone (sce 9); der Port zweigt fuer Id >= 0x48 in den Leser ab
(`aot_item_dokument`, RE2-Vorbild `sltiu v0,a3,0x68` @0x80071bbc).

**Zensus aller 240 RDTs** (`zensus_item_nachricht.py`, 1579 Bloecke, davon 18 desynchron abgebrochen —
deren Inhalt fehlt, die Zahlen sind Untergrenzen): 27 Paare Item-Rechteck × Nachrichten-Rechteck mit
Ueberschneidung. Sortiert nach Ueberdeckung des Item-Rechtecks:

| Ueberdeckung | Paare | Item im kleineren Slot (gewinnt) |
|---|---|---|
| >= 81 % (ROOM10E0/E1 ×4 je 100 %, ROOM1110/1111 ×4 81/100 %, ROOM30A0/A1 ×2 100 %) | 10 | **10 von 10** |
| <= 12,5 % (Randberuehrung: 10F0/F1, 11A0/A1, 11B0/B1, 4010/4011, 40B0/B1) | 17 | 2 von 17 |

Beispiel ROOM10E0 @0x00C4E `Item_aot_set` Slot 1 Rechteck (−650,−7350,2600,1000) und @0x00CCC
`Aot_set` sce 1 Slot 8 mit DEMSELBEN Rechteck. **Regel des Originals: liegt ein Gegenstand im
Untersuchen-Rechteck, steht er im kleineren Slot und gewinnt.**

Gemessen an den ENDGUELTIGEN Zonen (Sonde `abdeckung`, Raster 50, 64 Blickrichtungen; `druck` = ein
echter Druck durch `re15_aot_scan`, Item 0x48 als Stellvertreter, weil die Dokument-Tabelle 0x49..0x4C
noch nicht kennt — der Leser-Weg ist derselbe; `rot` 0 = Blick +x/Ost, 1024 = Blick −z/Sued).
Alles aus EINEM Lauf `re15_port/tools/r34n_e/endwerte.sh` → `E_belege/endwerte.txt`:

| Raum | Zone (Ecke, Groesse), Slot | Standorte | eigene Treffer | abgefangen | echter Druck |
|---|---|---|---|---|---|
| 1050 | (15250,−7250,1000,1000), 15 | 709 | 8239 | 0 | Stand (14900,−6750) rot 0 → **Leser** |
| 1051 | dieselbe, 15 | 0 | 0 | **8239 von Slot 11** (Leichen-Satz) | → **event 3** (Leichen-Satz) |
| 1000 / 1001 | (18726,−12223,1000,1000), 10 | 134 | 1000 | 0 | Stand (18250,−11723) rot 0 → **Leser** |
| 1020 / 1021 | (−11100,−16928,2200,1000), 14 | 0 | 0 | **5384 von Slot 10 / 11** (Tisch-Nachricht) | → **msg 3** „It's Lieutenant Branagh's desk." |
| 1020 / 1021, Tisch-Nachricht nach Slot 15 | dieselbe, 14 | 611 | 5384 | 0 | Stand (−11500,−16428) rot 0 → **Leser** (1020 und 1021) |
| 1010 / 1011 | (−50,5100,1000,1000), 9 | 201 | 786 | 2202 (Slot 2 Spray) | Stand (700,6400) rot 1024 → **Leser**; Stand (300,6400) rot 1024 → **Spray** (Item-Modal) |

Folgen: ROOM1020 braucht die Vorrang-Regel des Originals (5.3), sonst ist Marvins Notiz
UNERREICHBAR. ROOM1010: waehrend das Spray liegt, bekommt der Spieler dort, wo sich beide Zonen
decken, zuerst das Spray (Item vor Item, kleinerer Slot — Original-Praxis ROOM5010, Runde 30); das
Blatt hat einen eigenen Bereich (786 Treffer), nach dem Spray gehoeren ihm alle. ROOM1051: der
Leichen-Satz (Waffe) kommt zuerst; nach dem Nehmen der Waffe setzt sub01 @0x00CB2
`06 00 10 00 | 21 09 a5 01 | 46 0b 00 00 00 00 00 00 00 00` (Ck(9,165) → Aot_reset(11, sce 0,
sat 0x00)) Slot 11 auf TRAEGE (sat 0 = keine Ausloesebits), dann greift das Tagebuch.

**Zensus aller Slot- und obj-Zugriffe** (`slot_zensus.py`, JEDER Block main + subs, nicht nur der
Raumstart; 0x2C/0x3B/0x50/0x46/0x47 slot = pc[1], 0x2D obj = pc[1]; 0 abgebrochene Bloecke,
`E_belege/slot_zensus.txt`): die Originalskripte beruehren in ROOM1000/1001 Slots 0..9, obj 0..1;
ROOM1010 0..7 / ROOM1011 0..8, obj 0..2; ROOM1020 0..12 / ROOM1021 0..13, obj 0..6; ROOM1050 0..10 /
ROOM1051 0..12, obj 0..1. Slot 10 in ROOM1020 und Slot 11 in ROOM1021 (die Tisch-Nachricht) werden
NUR von ihrem eigenen Aot_set @0x01F0E / @0x01F6C beruehrt — kein Aot_reset/Aot_on darauf, das
Verschieben bricht also keinen Skriptverweis. Kein anderer Aktions-Satz in ROOM1020/1021 schneidet
das Tisch-Rechteck x[−11100..−8900] z[−18100..−14800] (Slots 11/12 bzw. 12/13 liegen bei
x −14500..−12000 / −7300..−6300).

### 3.5 Oberflaechenhoehen — je zwei Verfahren

| Raum | Flaeche | Verfahren 1 | Verfahren 2 | gewaehlt |
|---|---|---|---|---|
| 1000 | Bank (SCA 7) | Kantenlage gegen die SCA-Kanten (`kantenhoehe.py`): Cut 0 Suedende z −12450 → **−385** (0,48 px), Cut 2 Nordende z −2750 → **−400** (0,45 px), Cut 0 Ostkante x 20650 → −325 (0,60 px, andere Kantendefinition) | Deckflaeche (`deckflaeche.py`, Jaccard Holz R−B>=30 gegen vorhergesagte Platte): Cut 0 → **−345** (0,834), Cut 2 → **−385** (0,839) | **−385** (Median der fuenf; Spanne −325..−400). Cut 1 verworfen: kein Gipfel (Jaccard 0,836 gegen 0,833) bzw. Fernkante hinter der Wandbank nicht trennbar |
| 1020 | Schreibtisch (SCA 2) | Kantenlage Westkante x −10893: Cut 6 → **−1405** (0,21 px), Cut 3 → −1450 (0,73 px) | Deckflaeche (hell >= 60): Cut 6 → **−1415** (0,620), Cut 3 → −1455 (0,413) | **−1410** (Cut 6 = Nutzerbild, beide Verfahren); Cut 3 liegt 45 hoeher (0,5 px dort) |
| 1010 | Verhoertisch (SCA 5) | ORIGINAL-DATEN: beide Tisch-Items stehen bei y **−1600** (RDT @0x00930 `… c8 00 c0 f9 7c 15 …` = (200,−1600,5500); @0x00952 (1800,−1600,5750)), Modell-Fuss y 0 (bbox y −489..0 / −72..0) | Deckflaeche Cut 0 → −1545 (0,816; zweiter Gipfel −1410 0,809 — flach) | **−1600** (Original); Kantenlage verworfen (13,5 px Restabstand: die Zellkante ist nicht die gemalte Kante) |
| 1050 | Schoss der Leiche | Triangulation Cut 9 × Cut 3 (Hand auf dem Schenkel) **−347** (Strahlabstand 6) | — (keine zweite gemeinsame Flaeche; zweiter Beleg: Rueckprojektion 0,8/0,2 px in beiden Cuts) | Buch-Unterseite auf −347 → Buchmitte **−360** |

Zum Massstab (Plausibilitaet, kein Beleg): Irons-Tisch −1520 (Runde 30), Marvins Tisch −1410,
Verhoertisch −1600, Tisch-Items ROOM1020 −1500 — Tische liegen in RE1.5 bei 1400..1600; die Bank
bei −385 ist ein Viertel davon.

### 3.6 Verdeckung durch Masken

Sonde `huelle` (8 Ecken des gedrehten Modell-Quaders, jede Maske ueber der Bildhuelle mit dem
Urteil fuer die naechste und fernste Ecke; Regel `re15_pri_mask_occludes` = @0x8002565c otz>>4 gegen
die Maskentiefe):

| Dok | Cut | Masken | Ergebnis |
|---|---|---|---|
ENDWERTE (`endwerte.sh`, Quader um die Modell-bbox, gedreht wie das Prop):

| Dok | Cut | Masken | Ergebnis |
|---|---|---|---|
| 1 (1050) | 3 | 20 nachgezeichnete (`MASKS/ROOM1050.MSK`, md5 `36039eae…`) | keine ueber dem Buch → **sichtbar** |
| 1 | 5 | 83 nachgezeichnete, Maske 64 Tiefe 93 | verdeckt (Fernblick durch die Rolltor-Linie, 4 px breit — der gemalte Vordergrund liegt davor; keine Klemme) |
| 1 | 9 | keine | sichtbar (Nahaufnahme, 55 px breit) |
| 1 (1051) | alle | keine (`ROOM1051.MSK` gibt es nicht) | sichtbar |
| 2 (1000) | 0 | 105 nachgezeichnete (`ROOM1000.MSK`, md5 `0f6b030b…`) | keine ueber der Buchhuelle (vz 5402..5830) → **sichtbar**, keine Klemme |
| 3 (1020 und 1021) | 6 | Original @pri 0x12CC, Maske 32 Tiefe 156 | Blatt vz 9624..9993 < 10058 → sichtbar (Rand 65) |
| 3 | 3 | Original @pri 0xD28, Maske 35 Tiefe 258 (x 127..166 y 91..114 = der Tisch) | Blatt vz 17020..17395 > 16592 → **VERDECKT** (wie der Irons-Tisch Cut 2) → Klemme 258 |
| 3 | 5 / 8 | Original, Tiefe 324 / keine ueber der Huelle | sichtbar |
| 4 (1010) | 0 | 100 nachgezeichnete (`ROOM1010.MSK`, md5 `25542699…`), Masken 14/18/21/25 Tiefe 40/42/44/46 (Tischplatte als Treppe) | naechste Ecke frei, **fernste verdeckt** → Klemme 40 |
| 4 | 1 | Maske 46 Tiefe 53 (auch die naechste Ecke), 48..56 Tiefe 54..58 | Klemme 53 |
| 4 | 6 | Masken 14/20 Tiefe 60/64 | Klemme 60 |
| 4 | 8 | Masken 2/6/11 Tiefe 51/54/57 | Klemme 51 |
| 4 | 7 | keine | sichtbar |
| 4 (1011) | alle | keine (`MASKS/ROOM1011.MSK` gibt es nicht; der Lader sucht je Raum-Id) | sichtbar, keine Klemme |

Klemme = kleinste Tiefe unter den Masken, die die Huelle verdecken; Sortierschluessel hoechstens
`re15_pri_mask_camera_z(Tiefe) − 1` (Irons-Vorbild `re15_irons_tisch_sort_max`). Die ROOM1010-Masken
sind NACHGEZEICHNET: das Original fuehrt dort eine NULL-Sektion und verdeckt im Spiel GAR NICHTS —
die Klemme stellt fuer das Blatt genau das Original her. Das Original-Spray daneben (Huelle vz
2452..2762) wird von denselben Masken 14/17/18 an seiner fernsten Ecke ebenfalls beschnitten; im
Nullbild der echten exe (3.8) ist das nicht zu sehen (Zylinder, die Ecke liegt hinter dem Koerper).

Das Original kennt keinen Tiefen-Versatz je Objekt (FUN_8002c18c reicht nur Mesh/Farbe/ABE weiter,
Runde 30 3.2): ein Objekt auf einer Platte, deren Maske die Platte selbst ist, waere auch im
Original verdeckt. Deshalb (wie Irons-Tisch) die TIEFEN-KLEMME als Port-Zusatz, Wert = Tiefe der
verdeckenden Maske (5.4).

## 4 Soll-Verhalten (Zeitlinie)

(folgt)

## 5 Bauplan

(folgt)

### 5.x Konstanten-Tabelle

| Konstante | Wert | Beleg @0x…/Datei-Offset bzw. NUTZER-VORGABE/PORT-WAHL + Grund |
|---|---|---|

## 6 Abnahmeplan

(folgt)

## 7 Risiken, Softlocks, Wechselwirkungen mit anderen Spuren

(folgt)

## 8 Offene Punkte

(folgt)
