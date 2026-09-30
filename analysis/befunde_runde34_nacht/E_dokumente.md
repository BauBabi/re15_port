# Spur E — Vier neue Dokumente (Welt-Prop, Aufnahme, FILE-Leser) in ROOM1050/1000/1020/1010

Stufe: ERMITTLUNG + BAUPLAN (noch kein Port-Code). Arbeitsbaum `.claude/worktrees/r34n_dokumente`,
Zweig `r34n/dokumente`, aufgesetzt auf cf0e68ba (master + Auftrag/Vertrag der Nacht).
Vorbild in allen Punkten: Irons Diary (Runde 30, `analysis/befunde_runde30/irons-diary-welt.md`,
`irons-diary-dokument.md`, `nachtrag-diary-en.md`).

Status: ERMITTLUNG + BAUPLAN ABGESCHLOSSEN. Kein Port-Code geschrieben; Werkzeuge, Sonde und
Belege im Baum.

Werkzeuge dieser Spur (alle neu, alle im Baum, `re15_port/tools/r34n_e/`):

| Werkzeug | Zweck |
|---|---|
| `texte_aus_auftrag.py` | zieht die vier Texte WOERTLICH aus `AUFTRAG.md` -> `E_texte/dok*.txt` |
| `doc_satz_brief.py` | Satz FILE26..29 auf `re2_doc_satz.py` (Atlas/Glyphen/TIM unveraendert) + drei gemessene Erweiterungen (3.2) |
| `satz_pruefung.py` | Rueckleseprobe der geschriebenen TIM (Woerter, Titel, p00 == title, Papier bytegleich) |
| `kontaktbogen_satz.py` | Kontaktbogen der gesetzten Seiten im RE2-Schirmlayout + Weltmodell daneben |
| `leser_lauf.sh` | die gesetzten Seiten im Leser der ECHTEN exe (ohne Port-Code, 3.8) |
| `marken.py` | Cut + rote Marke der Nutzerbilder (Differenz gegen jeden Hintergrund, exakte Markenfarbe) |
| `geom.py` | Kamera-Geometrie aus der Engine-Sonde (echte Inverse), Triangulation |
| `kantenhoehe.py`, `deckflaeche.py` (+ verworfen: `flaechenhoehe.py`, `bankhoehe_1000.py`) | Flaechenhoehen, je zwei Verfahren (3.5) |
| `lage_1010.py` | Lage des Blatts neben dem Original-Spray (Dok 4) |
| `kontrollabzug.py` | Modelle mit Engine-Matrix in die Hintergruende (Lage/Groesse/Ausrichtung) |
| `zensus_item_nachricht.py`, `slot_zensus.py`, `id_zensus.py` | Vorrang-Regel des Originals, Slot-/obj-Zugriffe aller Bloecke, Item-Ids 0x48..0x4C |
| `codes.py` | Ziffernfolgen der Kartenleser-Schloesser aus den Skripten (3.7) |
| `namensbreite.py` | Kodierung und Breite der Listennamen/Meldung (3.9) |
| `raum_lauf.sh`, `licht_gegenprobe.py` | Nullbild eines Raums in der echten exe, Licht am Original-Item (3.8) |
| `endwerte.sh` | alle Sonden fuer die Endwerte in einem Lauf (`E_belege/endwerte.txt`) |
| `re15_port/tests/unit/probe_r34n_e_dokumente.c` + `probes/r34n_e_dokumente.cmake` | Engine-Sonde (kamera, projekt, strahl, sicht, huelle, aots, modelle, zonen, zonenliste, sca, stand, abdeckung, druck, licht) — reine Messung, kein add_test |

Messausgaben (unversioniert): `build/r34n_e/` im Arbeitsbaum. Belege: `E_belege/`, Texte: `E_texte/`.

## 0 Kurzfassung

* **Texte**: woertlich aus `AUFTRAG.md` gezogen (md5 je Text, 1.1), gesetzt als FILE26..29 auf RE2s
  Glyphen-Atlas: 0 fehlende / 0 konstruierte Glyphen, max_page 3 / 4 / 2 / 2 (Leserseiten 4 / 5 / 3 / 3),
  Titelseite in Versalien (Dok 1 zweizeilig nach RE2-Regel: Abstand 16, jede Zeile mittig),
  Unterschrift „Marvin Branagh" rechtsbuendig (RE2 „William Birkin" x1 250). Rueckleseprobe PASS und
  **im Leser der echten exe gesehen** (3.8, `leser_echt_bogen.png`).
* **Lage** (Nutzerbilder → Cut + Marke, Rueckprojektion mit der Engine-Matrix, Flaechenhoehe je zwei
  Verfahren): Dok 1 auf dem Schoss der gemalten Leiche ROOM1050 (16474, −360, −6592), Dok 2 Bank
  ROOM1000 (19226, −398, −11723), Dok 3 Marvins Tisch ROOM1020 (−9975, −1410, −16428), Dok 4
  Verhoertisch ROOM1010 (450, −1600, 5600, rot 3840) — NEBEN dem Original-Spray, das im Spiel genau
  unter der Nutzermarke steht (der Nutzer sah im reinen Hintergrund keine Props).
* **Vorrang**: das Original legt einen Gegenstand im Untersuchen-Rechteck in den KLEINEREN Slot
  (Zensus 10 von 10). Ohne Eingriff waere Marvins Notiz unerreichbar (Tisch-Nachricht Slot 10 faengt
  alle 5384 Treffer) → die Tisch-Nachricht zieht fuer die Liegezeit nach Slot 15 (Waechter auf den
  Original-Satz @0x01F0E/@0x01F6C). ROOM1051: der Original-Leichen-Satz (Waffe) fuehrt, danach das
  Tagebuch; ROOM1010: das Spray fuehrt, wo sich die Zonen decken.
* **Verdeckung**: Klemme wie am Irons-Tisch in ROOM1020/1021 Cut 3 (Original-Maske Tiefe 258) und
  ROOM1010 Cut 0/1/6/8 (nachgezeichnete Masken; das Original verdeckt dort nichts).
* **Licht**: byte-true Satz des aktiven Cuts; am Original-Spray in der echten exe gemessen
  (22.7,21.5,21.5) = Modell (22.2,21.1,21.1) — die Dokumente werden so hell wie die Items der Raeume.
* **Codes**: 4312 = Communication-Room-Schloss (ROOM10D0), 5632 = Weapon-Storage-Schloss (ROOM1230) —
  aus den Ziffern-Subs gelesen und durch die Original-Zettel bestaetigt. **Nutzertexte stimmen.**
  Spur F entfernt die Zettel → E und F nur zusammen ausliefern (7.5).
* **Ressourcen** (VERTRAG): Items 0x49..0x4C, FILE26..29, Bits 57..60, Slots 1050:15 · 1000:10 ·
  1020:14 (+15 fuer die Nachricht) · 1010:9, obj 2/2/7/3 — per Zensus aller Skriptbloecke frei;
  keine neuen Nachrichten-IDs, keine Stimmen, kein neues Speicherformat.
* **Bauplan** (5): eine neue Engine-Datei + Kopf + eingebackene RE2-Modelle (md5-geprueft), fuenf
  Haken-Zeilen in gemeinsamen Dateien, Konstanten-Tabelle mit Beleg je Wert (5.2), Riegel 6.1 a..m.
* **Offen** (8): Drehung der Buecher ist optisch (Grundstellung, Nutzer-Abnahme), PSX-Prop-Lader,
  Absprache mit D (Slots 13/14 in 1050), gemeinsame Auslieferung mit F.

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
  (Pruefung in 5.5).

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
Folge: die Dokument-Modelle werden in 1000/1010/1050 so dunkel wie die Original-Items dort (am Spray in der echten exe gemessen, 3.8; Risiko 7.1).

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
die Maskentiefe).

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

Gegenprobe der NULL-Sektionen (Datei-Bytes): ROOM1010.RDT @0x458/0x45C/0x460/0x464/0x470/0x478
(pri-Zeiger der Cuts 0/1/2/3/6/8) = `ff ff ff ff`, ROOM1000.RDT @0x4C8 (Cut 0) = `ff ff ff ff`;
ROOM1020.RDT @0xD28 (Cut 3) = `09 00 2c 00 06 00 00 78 …` = echte Original-Sektion.

Das Original kennt keinen Tiefen-Versatz je Objekt (FUN_8002c18c reicht nur Mesh/Farbe/ABE weiter,
Runde 30 3.2): ein Objekt auf einer Platte, deren Maske die Platte selbst ist, waere auch im
Original verdeckt. Deshalb (wie Irons-Tisch) die TIEFEN-KLEMME als Port-Zusatz, Wert = Tiefe der
verdeckenden Maske (5.2).

### 3.7 Codes 4312 / 5632: Nutzertexte gegen Original und Port

`codes.py` (→ `E_belege/codes.txt`) liest die Schloss-Skripte selbst: sub01 vergleicht den Radwert
Member[15] (`Member_cmp 3e 00 0f 00 v 00`) und startet bei VIERECK die passende Ziffern-Sub; jede
Ziffern-Sub setzt ein Stellen-Bit Bank 5 Bit 13..16. Die geforderte Folge = die Subs in
Bit-Reihenfolge:

| Schloss | Raum (Beleg) | Stelle 1..4: Sub, Bank5-Bit @, Radwert (Member_cmp @) | Original-Zettel |
|---|---|---|---|
| Communication Room | ROOM10D0 msg 6 @0x1F3B „Communication Room" It's electronically locked…; Ziel ROOM10F0 „COMMUNIC. ROOM" (DEBUG.BIN 0x027C0) | sub09 Bit13 @0x0165E, 6 (@0x013DA) · sub08 Bit14 @0x01612, 5 (@0x013B2) · sub06 Bit15 @0x0157A, 3 (@0x01362) · sub07 Bit16 @0x015C2, 4 (@0x0138A); ROOM10D1 dieselben Subs 0x16 frueher | ROOM1110/1111 msg 0 @0x0D68 „It's a police officer, he's dead. He is holding a slip. The numbers 4312 are printed on the slip." |
| Weapon Storage (Armory) | ROOM1230 msg 6 @0x15BF „Weapon Storage" It's electronically locked… | sub10 Bit13 @0x01172, 11 (@0x00ECA) · sub11 Bit14 @0x011BE, 12 (@0x00EF2) · sub08 Bit15 @0x010DA, 9 (@0x00E7A) · sub07 Bit16 @0x0108A, 8 (@0x00E52); ROOM1231 identisch | ROOM1230/1231 msg 10 @0x16F4 „A miserable death... He is holding a slip. The numbers 5632 are printed on the slip." |

Die Umrechnung Radwert → Ziffer steht nicht im Skript (Radstellung des Modells). Die Radwerte
[6,5,3,4] ergeben mit genau EINEM Versatz (v = 2) „4312", [11,12,9,8] mit genau einem (v = 6) „5632"
(alle Versaetze in `codes.txt`): die Zettel des Originals sind exakt die Folgen, die die Schloss-Subs
verlangen. Der Port fuehrt diese Skripte unveraendert aus (`tests/integration/test_keypad.c`,
ROOM1230 „code 5632": sub17 → sub01 → Ziffern-Subs → sub19). **Ergebnis: beide Nutzertexte stimmen —
Dok 3 „4312" oeffnet den Communication Room, Dok 4 „5632" die Weapon Storage (Armory). Kein Text
geaendert.** Kopplung mit Spur F: 7.5.

### 3.8 Am Artefakt geprueft (echte exe, OHNE Port-Code)

1. **Leser** (`leser_lauf.sh`, eigene exe-Kopie + eigenes `shared_assets/` in `build/r34n_e/mess`):
   die gesetzten Seiten liegen dort UNTER DEM NAMEN eines RE2-Dokuments mit gleichem max_page und
   gleicher Seitenhoehe (FILE26 → FILE14, FILE27 → FILE08, FILE28 → FILE03, FILE29 → FILE23; u16
   @0x800AA144 + doc*4), die Ansehhilfe `RE15_DOC=<n>` oeffnet sie im echten Leser. Spielstand
   Nutzerkarte Platz 0, START F400, R1 F520, VIERECK F580/F600, RECHTS ab F700 alle 90 Bilder,
   `RE15_FRAMEDUMP` (Readback vor SDL_RenderPresent, kein AUTOSHOT/SOFTWARE_RENDER). Ergebnis
   `E_belege/leser_echt_bogen.png`: alle vier lesbar, Titel „POLICE OFFICER'S / FINAL DIARY ENTRY"
   zweizeilig mittig, die anderen einzeilig; Zaehler 1/4..4/4, 1/5..5/5, 1/3..3/3, 1/3..3/3
   (= max_page + 1); „Marvin Branagh" rechtsbuendig; Ende-Stellung EXIT, danach die FILE-Liste.
   (Erster Versuch mit START F100 landete auf der KARTE: das R1 kam nicht an, VIERECK bestaetigte
   den Vorgabe-Reiter — deshalb der Zeitplan des Runde-30-Laufs.)
2. **Licht am Original-Item** (`raum_lauf.sh`: CONTINUE, `RE15_DEBUG_JUMP 1010@gp`,
   `RE15_PLAYER_POS`): im Nullbild ROOM1010 Cut 0 steht das Spray unter der Nutzermarke
   (`E_belege/nullbild_1010_cut0_echt.png`, rotes Rechteck = Marke). `licht_gegenprobe.py`: Spray in der
   echten exe Mittel-RGB **(22.7, 21.5, 21.5)**, Abzug mit Faktor (41,39,39)/128 **(22.2, 21.1, 21.1)**,
   ohne Licht (70.7, 70.7, 70.7) — das Lichtmodell der Abzuege stimmt, und die Lage-Kette (Engine-
   Matrix, Prop-Drehung, Projektion) auch (die Maske aus dem Abzug liegt in der exe auf dem Spray).
   **Folge: die Dokumente in 1000/1010/1050 werden genau so hell wie die Original-Items dieser
   Raeume.**
3. **ROOM1000 Cut 0 im Spiel**: im Markenfenster weichen 0 von 462 Pixeln vom Hintergrund ab (nur
   der Spieler, 403 px anderswo) — die Stelle auf der Bank ist frei (`nullbild_1000_cut0_echt.png`).

### 3.9 Listenname und Meldung: Breite (`namensbreite.py` → `E_belege/namensbreite.txt`)

Kodierung belegt: DEBUG.BIN 0x04e04 = `1f 44 4e 45 4f 3a 00 20 45 3d 4e 55 07` = „Chris' Diary"
(ASCII − 0x24, Leerzeichen 0x00, Apostroph 0x3A, Ende 0x07). Breite ueber die Vorschubtabelle
@0x800c4416 (FUN_80028ec4 `x += width[c]` @0x8002910c-24).

| Dok | Listenname | Bytes | Breite | Zeile ab x 44 → Ende | Meldung Zeile 1 „The …" ab x 34 → Ende |
|---|---|---|---|---|---|
| 1 | Police Officer's Final Diary Entry | `2c 4b 48 45 3f 41 00 2b 42 42 45 3f 41 4e 3a 4f 00 22 45 4a 3d 48 00 20 45 3d 4e 55 00 21 4a 50 4e 55 07` | 228 | 271 (**75 ueber die Hervorhebung**) | 257 → 290 |
| 2 | Elliot's Diary | `21 48 48 45 4b 50 3a 4f 00 20 45 3d 4e 55 07` | 88 | 131 | 117 → 150 |
| 3 | Marvin's Notes | `29 3d 4e 52 45 4a 3a 4f 00 2a 4b 50 41 4f 07` | 100 | 143 | 129 → 162 |
| 4 | Armory Notice | `1d 4e 49 4b 4e 55 00 2a 4b 50 45 3f 41 07` | 96 | 139 | 125 → 158 |

Hervorhebungs-Kachel der Zeile: x 0x2b, w 0x9a (@0x800c749c-c0) → x 43..196. Blaue Tafel im
Framedump der echten exe x 10..305. RE1.5s breitester eigener Listenname (Ids 0x48..0x65) 140 px →
keiner ueberragt die Kachel; RE2 hat keine Zeilenliste. Die Meldung: Skript [5] @0x800c506f
`30 44 41 00 05 01 06 00 05 00 08 44 3d 4f …` = „The " + Name, Zeilenumbruch, „has been filed.";
breitester Original-Name der Bank 158 px („Minidisc Player w\ Disc", 0x44), die Dok-1-Zeile (257 px)
endet bei x 290 — innerhalb des Schirms. Entscheidung Dok 1: Nutzerwortlaut bleibt (NUTZER-VORGABE),
die Zeile ragt 75 px ueber die Kachel, bleibt in der Tafel (Risiko 7.6).

## 4 Soll-Verhalten (Zeitlinie)

Gleich fuer alle vier Dokumente; Unterschiede je Raum in Klammern. Alle Schritte ab 2 sind der
fertige Runde-30-Weg (`irons-diary-dokument.md`), unveraendert.

| # | Wann | Was | Beleg |
|---|---|---|---|
| 0 | Raumstart (Tuer UND Boot/CONTINUE), nach dem Init-Lauf von main00 | Bank-9-Bit (57/58/59/60) = 0 → Prop (obj 2/2/7/3) + Aufhebe-Zone (Slot 15/10/14/9). ROOM1020/1021 zusaetzlich, solange Bit 59 = 0: Tisch-Nachricht Slot 10 bzw. 11 → Slot 15 | Irons-Muster `re15_irons_tisch_install`; Umwidmen nach dem Init-Lauf wie `re15_tuer1120_install` (R33) |
| 1 | Spieler am Dokument, VIERECK | Aktions-Scan feuert den ersten treffenden Slot (1050: Slot 15; 1051: erst Slot 11 = Leichen-Satz mit Waffe, nach Bit (9,165) Slot 15; 1000: Slot 10; 1020/1021: Slot 14; 1010/1011: wo sich die Zonen decken erst das Spray Slot 2, sonst Slot 9) | FUN_80042bac, Punkt 620 voraus `ori v0,zero,0x26c` @0x80042bd0 |
| 2 | derselbe Druck | Id >= 0x48 → `re15_menu_request_doc(doc 1..4, Bit, Slot, obj)` statt Item-Modal | RE2 `sltiu v0,a3,0x68` @0x80071bbc, `addiu a0,a3,-104` @0x80071d04 |
| 3 | Aufnahme-Leser | Ueberblendung, Titelseite (Versalien) ueber der Illustration, RECHTS blaettert, Zaehler k/N (N = max_page + 1 = 4/5/3/3), Ende-Stellung EXIT | Leser der Runde 30 (`irons-diary-dokument.md`); die neuen Seiten darin gesehen: hier 3.8 Punkt 1 |
| 4 | Schliessen | Eintrag in die FILE-Liste (erster freier der 24 Plaetze), Meldung „The <Listenname> has been filed." (Schreibmaschine) | FUN_800692dc @0x800692dc-314; Skript [5] @0x800c506f |
| 5 | Meldung bestaetigt | im selben Bild: Zone aus, Bank-9-Bit setzen, Prop weg, Ton Satz 5; Menue schliesst | RE2 @0x80072b40 / @0x80072b8c / @0x80072bb0 / @0x80072bf0 |
| 6 | spaeter | FILE-Reiter: Zeile mit Listenname, VIERECK → Leser des FILE-Schirms (RE2 Zustand 11/13) | `menu_common.c` file_mode |
| 7 | Raum erneut | Bit gesetzt → kein Prop, keine Zone; ROOM1020/1021: Tisch-Nachricht bleibt in ihrem Original-Slot 10/11 | wie Schritt 0 |
| 8 | Speichern/Laden | `files[24]` + Bank-9-Bits im Stand v9 → Liste und „genommen" wie vorher | `re15_savedata.h:145` (files), `:131` (flags) |

Nicht vorgesehen (keine Nutzer-Vorgabe, kein Original): Stimme, neue RDT-Nachrichten, Kamera-
wechsel beim Aufheben. Die VERTRAG-IDs 26/27 (1050) und 20..23 (andere Raeume) bleiben ungenutzt.

## 5 Bauplan

### 5.1 Dateien

NEU (Datei-Hoheit Spur E, VERTRAG 2):

| Datei | Inhalt |
|---|---|
| `re15_port/include/re15_dokumente.h` | alle Konstanten aus 5.2 mit Beleg-Kommentar, API: `re15_dokumente_install(room)`, `re15_dokumente_sort_max_mit(irons, room, cut, obj)`, `re15_dokumente_obj_id(room)`, `re15_dokumente_md1_bytes(obj, room, &n)` / `_tim_bytes` |
| `re15_port/engine/src/dokumente_r34.c` | Installer (Muster `irons_tisch_1150.c` `anlegen()`), Tisch-Nachricht-Umzug mit Satz-Waechter (Muster `tuer1120_1130.c`), Klemmen-Tabelle, Modell-Bytes je Raum |
| `re15_port/engine/src/gen/dokumente_props.inc` | MD1+TIM von mesh00/mesh03/mesh01/mesh04, UNVERAENDERT aus den RE2-RDTs (Tabelle „Modelle" in 5.2), erzeugt von … |
| `re15_port/tools/dokumente_engine_export.py` | … diesem Werkzeug (Muster `irons_tisch_engine_export.py`: jede Quelle gegen ihre md5, sonst Abbruch) |
| `re15_port/shared_assets/RE2/FILES/FILE26..29_*.TIM` | 23 Dateien (FILE26 6, FILE27 7, FILE28 5, FILE29 5), erzeugt von `re15_port/tools/r34n_e/doc_satz_brief.py`; md5 `E_belege/satz_md5.txt`, Rueckleseprobe `satz_pruefung.py` PASS. Reproduzierbar mit `bash re15_port/tools/r34n_e/satz_bauen.sh` (Atlas + die vier Aufrufe + md5-Vergleich; gemessen: 4 von 4 gleich der Liste) — danach die TIM aus `build/r34n_e/satz_neu/FILEnn/` kopieren |
| `re15_port/tests/unit/test_r34n_e_dokumente.c` (+ Eintrag in der Unit-CMake) | Riegel 6.1 |
| `re15_port/tests/integration/test_r34n_e_dokumente_bild.cmake` | Riegel 6.2 (echte exe, Framedumps) |

GEAENDERT (minimale Haken, VERTRAG 2; Zeilen = Stand cf0e68ba):

| Datei:Zeile | Haken |
|---|---|
| `engine/src/re15_files.c:44-50` | 4 Namensfelder (hinter `s_name_irons_diary` @Zeile 44) (Bytes aus 3.9) + 4 Tabellenzeilen `{ RE15_FILES_FIRST_ITEM_ID + n, 25 + n, max_page, H, name }` |
| `engine/src/scd_room_setup.c:423` | hinter `re15_granate_install(...)`: `re15_dokumente_install((uint16_t)g_current_room_id);` (+ `#include "re15_dokumente.h"` bei Zeile 23) — NACH dem Init-Lauf `scd_vm_tick()` @Zeile 408, damit main00 die Tisch-Nachricht schon angelegt hat |
| `platform/pc/main.c:4685` | dieselbe Zeile hinter `re15_granate_install` im Boot/CONTINUE-Weg (+ include bei Zeile 72); Original: EIN Raumlader FUN_800396fc, zwei Aufrufer @0x8001d5ac / @0x8001d988 |
| `platform/pc/main.c:~1418` (Ende `pc_load_room_prop_set`) | ein Block wie der Irons-Block: `oid = re15_dokumente_obj_id(room)`; wenn `oid >= 0 && nprops <= oid`: MD1 parsen, TIM in `RE15_TIM_SLOT_PROP(oid)` |
| `platform/pc/main.c:10313` | Initialisierer: `const int irons_sort_max = re15_dokumente_sort_max_mit(re15_irons_tisch_sort_max(room, cut, oid), room, cut, oid);` — gibt die Dokument-Klemme zurueck, wo eine gilt, sonst den Irons-Wert unveraendert |

KEIN Haken: `aot_common.c` (Dokument-Zweig vorhanden), `menu_common.c` (Leser, Meldung, Abraeumen
vorhanden), `item_prompt_common.c` (Name kommt aus der Tabelle), Speicherstand (5.5), Licht (5.2).
CMake: `engine/src/*.c` per GLOB (neue Datei beim naechsten Configure; Android: 7.9).

### 5.2 Konstanten-Tabelle

Dokument-Tabelle (`re15_files.c`):

| Konstante | Wert | Beleg bzw. NUTZER-VORGABE / PORT-WAHL + Grund |
|---|---|---|
| Item-Id Dok 1..4 | 0x49, 0x4A, 0x4B, 0x4C | VERTRAG 1.4; Dokument = Id − 0x48 (RE2 `addiu a0,a3,-104` @0x80071d04 mit RE1.5-Basis u8 @0x800c7370 = 0x48); kein Original-Skript nutzt 0x48..0x4C (`id_zensus.py`: 416 Item_aot_set- und 52 Aot_reset-Muster in 240 RDTs, 0 Treffer) |
| Bildsatz | 26, 27, 28, 29 | VERTRAG 1.4; PORT-WAHL: die naechsten freien Saetze hinter FILE25 (RE2 fuehrt 0..24, `RE2_FILES_DOC_COUNT`) |
| max_page | 3, 4, 2, 2 | PORT-WAHL gemessen am Satz (3.3; `FILEnn_pNN` vorhanden, `pNN+1` nicht) — Gegenstueck u16 @0x800AA144 + doc*4 |
| Seitenhoehe H | 144, 144, 176, 176 | TIM-Kopf der gesetzten Titelseiten = Vorlage FILE00/FILE08 (144) bzw. FILE02/FILE06 (176) |
| Listenname | 3.9 (Bytes) | Wortlaut NUTZER-VORGABE (AUFTRAG Z. 22/29/41/55), gemischte Schreibung (Runde-30-Regel); Kodierung @0x04e04 DEBUG.BIN |

Welt (`re15_dokumente.h`):

| Konstante | Wert | Beleg bzw. NUTZER-VORGABE / PORT-WAHL + Grund |
|---|---|---|
| Bank-9-Bit | 57, 58, 59, 60 | VERTRAG 1.1 (freier Block 53..84, Zensus `re15_irons_tisch.h`) |
| AOT-Slot | 1050/1051: 15 · 1000/1001: 10 · 1020/1021: 14 · 1010/1011: 9 | VERTRAG 1.2 (1050: 15/16); sonst der erste Slot, der in BEIDEN Varianten frei ist, nach `slot_zensus.py` (JEDER Block, nicht nur Raumstart): ROOM1001 belegt 0..9 → 10; ROOM1011 belegt 0..8 → 9 (ROOM1010 nur 0..7); ROOM1021 belegt 0..13 → 14 (ROOM1020 nur 0..12) — je Raum EIN Wert fuer beide Varianten |
| obj_id | 1050/1051: 2 · 1000/1001: 2 · 1020/1021: 7 · 1010/1011: 3 | VERTRAG 1.5 „erste freie Nummer" nach `slot_zensus.py` (0x2D obj = pc[1]) und Sonde `aots`; TIM-Slot `RE15_TIM_SLOT_PROP` = 6 / 6 / 27 / 7 (`main.c:162`) |
| Dok 1 Ursprung | (16474, −360, −6592) | Triangulation Hand auf dem Schenkel Cut 9 × Cut 3 = (16474, −347, −6592) (2.4), minus 13 = halbe Buchdicke (mesh00 bbox y −13..13, RE2 ROOM1150.RDT @0x029F34) |
| Dok 2 Ursprung | (19226, −398, −11723) | Rueckprojektion der Markenmitte (193.0, 172.5) Cut 0 auf die Bank y −385 → (19226.4, −11723.4) (Sonde `strahl`, Engine-Inverse); −385 = Median der fuenf Messungen (3.5); −13 halbe Buchdicke (mesh03 bbox, ROOM10E0.RDT @0x002320) |
| Dok 3 Ursprung | (−9975, −1410, −16428) | Rueckprojektion der Markenmitte (156.5, 105.5) Cut 6 auf den Tisch y −1410 → (−9975.9, −16434.6) minus Grundriss-Mitte des Blatts (−1, −7) (mesh01 bbox x −144..142, z −225..211, Fuss y 0; ROOM2020.RDT @0x003254); −1410 = Mittel Kantenlage −1405 / Deckflaeche −1415 in Cut 6 |
| Dok 4 Ursprung, Drehung | (450, −1600, 5600), rot_y 3840 | y: beide Original-Items dieses Tischs stehen bei −1600 (ROOM1010.RDT @0x00930/@0x00952); x/z/rot: globales Optimum `lage_1010.py` (PORT-WAHL, Regel im Werkzeugkopf: groesster sichtbarer Blatt-Anteil in der Marke, Dose Radius 120 nicht beruehrt, 30 Rand; Raster 20, Drehung in 256er-Schritten) → `E_belege/lage_1010.txt` Zeile 1: 0,266 / 0,900 |
| Drehung Dok 1..3 | 0 | PORT-WAHL, keine Original-Adresse: Grundstellung des Modells; lange Achse (z) = lange Achse der Ablage (Bank SCA 7 z 9700 lang; Tisch SCA 2 z 2800 > x 1696; Schoss: quer ueber beide Oberschenkel). Offener Punkt 8.1 |
| Dok 1 Rechteck | (15250, −7250, 1000, 1000) | Leichen-Satz derselben Leiche, ROOM1051 main00 @0x00C0E `2c 0b 03 31 00 00 92 3b ae e3 e8 03 e8 03 …` (Irons-Regel: Rechteck vom Original-Satz auf demselben Gegenstand) |
| Dok 2 Rechteck | (18726, −12223, 1000, 1000) | PORT-WAHL: 1000×1000 = haeufigste Groesse der Original-Item_aot_set (69 von 162; naechste 800×800 und 1000×1300 je 10; `groessen_zensus.py` → `E_belege/groessen_zensus.txt`), mittig auf dem Ursprung; ROOM1000s eigene Items liegen hoechstens 50 neben ihrer Rechteckmitte (@0x00C66 obj 0 (−4400,−325) ↔ Rechteck x[−4850..−3850] z[−800..200]) |
| Dok 3 Rechteck | (−11100, −16928, 2200, 1000) | x/w = Tisch-Nachricht DESSELBEN Tischs ROOM1020 @0x01F0E (`a4 d4` = −11100, `98 08` = 2200); z/d = 1000 mittig auf −16428 (wie Dok 2). Ein 1000 breites, mittiges Rechteck erreicht KEIN Standort (Sonde `abdeckung`: 0) — der Tisch ist 1696 breit |
| Dok 4 Rechteck | (−50, 5100, 1000, 1000) | PORT-WAHL 1000×1000 mittig; die Original-Items desselben Tischs haben 1000×1000 (@0x00996 Spray x[−350..650] z[5200..6200] zu (200, 5500); @0x009AC Munition x[1300..2300] z[5300..6300] zu (1800, 5750)) |
| Tisch-Nachricht umziehen | ROOM1020 Slot 10 → 15, ROOM1021 Slot 11 → 15, nur solange Bit 59 = 0 | Original-Regel „Gegenstand im Untersuchen-Rechteck steht im kleineren Slot" (Zensus 10 von 10 bei >= 81 % Deckung, z.B. ROOM10E0 @0x00C4E Item Slot 1 / @0x00CCC Nachricht Slot 8, gleiches Rechteck); Slot 15 in beiden Varianten frei; kein Skript beruehrt 10 bzw. 11 ausser dem Aot_set selbst (3.4) |
| Satz-Waechter | aktiv, Typ MESSAGE, Nachricht 3 (1020) / 12 (1021), Rechteck (−11100, −18100, 2200, 3300), sat 0x31 | ROOM1020.RDT @0x01F0E `2c 0a 01 31 00 00 a4 d4 4c b9 98 08 e4 0c 03 00 ff ff 00 00`; ROOM1021.RDT @0x01F6C (Slot 11, Nachricht 12). Weicht etwas ab → nichts umziehen (Muster `re15_tuer1120_install`) |
| Klemme ROOM1020 + 1021 | Cut 3: Tiefe 258 | Original-Maske 35 der Sektion @0xD28 (x 127..166 y 91..114 = die Tischplatte); Blatt vz 17020..17395 > 16592 — PORT-ZUSATZ wie `RE15_IRONS_KLEMME_*` |
| Klemme ROOM1010 | Cut 0: 40 · Cut 1: 53 · Cut 6: 60 · Cut 8: 51 | NACHGEZEICHNETE Masken `MASKS/ROOM1010.MSK` (md5 `255426998bea71fddf7f24649534cabd`): Cut 0 Maske 14, Cut 1 Maske 46, Cut 6 Maske 14, Cut 8 Maske 2 = kleinste verdeckende Tiefe (3.6); das Original hat dort NULL-Sektionen und verdeckt nichts. ROOM1011: keine Klemme (keine Masken) |
| Klemmen-Schluessel | `re15_pri_mask_camera_z(Tiefe) − 1` | wie `re15_irons_tisch_sort_max`; Schwelle (Tiefe+1)*65536/1023 = otz>>4 @0x8002565c |
| Prop-Felder | Typ 0, Band 1, Eltern −1 (0x00), flags 0x000B, Kollisionsbox 0 | wie Irons: `RE15_IRONS_PROP_FLAGS/BAND` (@0x80040998; Zensus 100 von 121 Item-Weltmodellen) |
| Zonen-sat / floor / Menge | 0x31 / 0 / 1 | wie Irons (`RE15_IRONS_AOT_SAT` 160 von 164, floor 152 von 164; Menge wie RE2 ROOM10E0 @0x01D26 +16) |
| Licht | keiner (Satz des aktiven Cuts) | byte-true FUN_8002c18c; am Spray gemessen (3.8). Keine Port-Wahl wie beim Irons-Tisch: in 1000/1010/1050 tragen ALLE Cuts denselben Satz, es gibt nichts zu waehlen; 1020 Cut 6: echte Lichter (−Y (50,52,52)) |

Modelle (`gen/dokumente_props.inc`, alle UNVERAENDERT aus `info/re2leon/PL0/RDT/`, md5 geprueft):

| Modell | Dok | MD1 | TIM | CLUT-Zeile |
|---|---|---|---|---|
| mesh00_0541704e | 1 | ROOM1150.RDT @0x029F34, 372 B, md5 `0541704ee41d6445b718ee0b1b8abb3b` | @0x04C3F0, 34848 B, md5 `6e6f32cb59ddb4b83b27c921a2dbfd90` | clut 0x7880 → Zeile 482 = 2 von 4 |
| mesh03_cf9f316d | 2 | ROOM10E0.RDT @0x002320, 372 B, md5 `cf9f316dd6ba0ecf599bea18a83c36d1` | @0x015224, 17440 B, md5 `63bd93f2be1cc97066aa09afd3c67e4d` | 0x7800 → 0 von 2 (dieselben Bytes wie das Irons Diary) |
| mesh01_ae2d0a30 | 3 | ROOM2020.RDT @0x003254, 404 B, md5 `ae2d0a30a18ed9cb9f089c21839ca61e` | @0x01A160, 34848 B, md5 `6b864a6f02ffd6c0720be1efa654c3cf` | 0x7800 → 0 von 4 |
| mesh04_cab7b32d | 4 | ROOM60A0.RDT @0x002F08, 404 B, md5 `cab7b32d230a19728221ffb6b1d75104` | @0x03B924, 34848 B, md5 `73fa3595c568578eace3379747390ff6` | 0x7840 → 1 von 4 |

Der PC-Zeichner waehlt die CLUT-Zeile relativ zum TIM-Kopf (`render_pc.c` tri_lit: `clut_idx =
clut_y − s_tim_clut_base_y`, `v_offset = clut_idx * one_clut_h`) — mehrzeilige CLUTs (mesh00 Zeile 2,
mesh04 Zeile 1) gehen damit ohne Umbau; das Irons Diary (Zeile 0) hat das nie gebraucht → Riegel 6.1 (m).

### 5.3 Ablauf `re15_dokumente_install(room)`

```
1050/1051: anlegen(Dok 1)
1000/1001: anlegen(Dok 2)
1020/1021: wenn Bit (9,59) = 0: tisch_nachricht_umziehen(1020 ? 10 : 11 -> 15)   // Waechter 5.2
           anlegen(Dok 3)
1010/1011: anlegen(Dok 4)
anlegen(d):  Bit (9,d.bit) gesetzt -> nichts
             Prop wie irons_tisch_1150.c anlegen() (obj frei, prop_count < RE15_SCD_MAX_PROPS)
             Zone: re15_aot_set_item_tk_prop(slot, x+w/2, z+d/2, w/2, d/2, item, 1, bit, obj)
                   + sce_flags 0x31, band 0 (Slot vorher frei, sonst nichts)
umziehen:    Quelle aktiv + Waechter passt + Ziel frei -> slots[15] = slots[q] (samt item/door/
             flag_params), slots[q] = 0   // genau so gemessen: Sonde abdeckung/druck VON NACH
```

`re15_dokumente_sort_max_mit(irons, room, cut, obj)`: obj = Dokument-obj des Raums UND (room, cut)
in der Klemmen-Tabelle → `re15_pri_mask_camera_z(Tiefe) − 1`, sonst `irons` unveraendert.

### 5.4 Varianten x0/x1

Kameratabellen, Lichtbloecke, die betroffenen SCA-Zellen und die pri-Sektionen sind zwischen den
Varianten gleich (Sonde `kamera`/`licht`/`sca`, 2.2). Unterschiede, die der Bauplan traegt: ROOM1021
Tisch-Nachricht in Slot 11 (Nachricht 12) statt 10 (3); ROOM1051 Leichen-Satz Slot 11/12 (Vorrang,
7.2); ROOM1011/1001/1051 ohne `.MSK` (keine Klemme in 1011).

### 5.5 Speicherstand

`files[24]` (v9) haelt Dokument-Nummern als Byte (0..4 passen), die Bits 57..60 liegen in `flags[]`
(Bank 9). **Kein neues Format.** Alte Staende: Bits 0 → die Dokumente liegen in der Welt.

## 6 Abnahmeplan

### 6.1 Unit-Riegel `test_r34n_e_dokumente` (je Punkt die Gegenprobe, die ihn rot macht)

a. Tabelle: 4 neue Zeilen (Id, Bildsatz, max_page, H, Namensbytes 3.9); H == TIM-Kopf der Titelseite;
   `FILEnn_p<max_page>` vorhanden, `p<max_page+1>` NICHT.
b. Je Raum (8 Varianten) nach `scd_room_reenter`: Prop obj/Ort/Drehung/flags 0x000B/Band 1, Zone
   Slot/Rechteck/sat 0x31/Item/Bit/obj — Werte 5.2.
c. Bit gesetzt → weder Prop noch Zone (je Raum).
d. 1020/1021: Nachricht in Slot 15, Quellslot frei; mit Bit 59 → Nachricht bleibt in 10/11;
   Waechter: veraenderter Satz (andere Nachricht) → kein Umzug.
e. Echter Druck (`re15_aot_scan`) von den Standorten aus 3.4 → `re15_menu_doc_active` mit Dok n;
   1020 ohne Umzug → Nachricht 3 (Gegenprobe); 1010 Stand (300,6400) → Spray.
f. 1051: Druck → Leichen-Satz (event 3); Bit (9,165) setzen, sub01 ticken → Slot 11 traege →
   Druck → Dok 1.
g. Abraeumen: Meldung bestaetigen → Zone aus, Bit gesetzt, Prop weg, FILE-Liste traegt n.
h. Speicher-Rundlauf: export/import → files[] und Bits gleich.
i. Klemmen: `sort_max_mit` liefert je (Raum, Cut) den Wert aus 5.2, sonst den Irons-Wert.
j. Masken-Riegel: fuer jeden Cut, in dem das Dokument zu sehen ist, verdeckt KEINE Maske die
   Huelle mehr, sobald die Klemme gilt (Sonde-`huelle`-Rechnung) — faellt, wenn jemand die
   `.MSK` neu zeichnet (7.8).
k. Kein Original-Skript nutzt 0x49..0x4C (Bytescan wie `id_zensus.py`).
l. Meldungstext: „The <Name>" + „has been filed." aus Skript [5] mit dem Tabellennamen.
m. Modell-Bytes: md5 der eingebetteten MD1/TIM = 5.2; CLUT-Zeile je Modell (2/0/0/1).

### 6.2 Am Artefakt (echte exe, `RE15_FRAMEDUMP`, Bilder ansehen)

1. Je Dokument: CONTINUE → `RE15_DEBUG_JUMP <raum>@gp` → Standort + Blick aus 3.4 per
   `RE15_PLAYER_POS` → VIERECK (`RE15_PAD_AT`) → Framedumps: Welt vorher (Modell an der Marke im
   Nutzer-Cut: 1000 C0, 1020 C6, 1010 C0, 1050 C3 und C9), Leser (Titel + jede Seite), Meldung,
   Welt nachher (Modell weg).
2. FILE-Reiter: Liste mit dem Namen (Dok 1: Zeile ragt ueber die Kachel, 7.6), Leser aus der Liste.
3. Raum verlassen und wieder betreten → Modell bleibt weg; CONTINUE nach Speichern → dito.
4. Klemmen im Bild: 1020 Cut 3 und 1010 Cut 0/1/6/8 Blatt ganz sichtbar; Gegenprobe Klemme aus →
   1020 Cut 3 0 Pixel, 1010 fernes Blattdrittel fehlt.
5. Helligkeit: Mittel-RGB des Dokuments im Framedump gegen den Abzug (Faktor 0,32 bzw. Lichtsatz
   Cut 6) — Toleranz wie `licht_gegenprobe.py` (< 1 je Kanal).
6. ROOM1051 (Elza): erst Leichen-Satz mit Waffe, danach das Tagebuch.
7. Codes: mit Dok 3 in ROOM10D0 4312 eingeben → Communication Room offen; mit Dok 4 in ROOM1230
   5632 → Weapon Storage offen (Schloss-Weg wie test_keypad.c).

## 7 Risiken, Softlocks, Wechselwirkungen mit anderen Spuren

1. **Dunkle Dokumente** in 1000/1010/1050 (Texel × 0,32): byte-true, gemessen gleich hell wie das
   Original-Spray (3.8). Kein anderer Lichtsatz im Raum, also keine Port-Wahl wie beim Irons-Tisch
   moeglich ohne erfundene Zahl. Wirkt es dem Nutzer zu dunkel, ist das eine neue Nutzer-Vorgabe.
2. **ROOM1051 (Elza)**: der Leichen-Satz (Slot 11, Waffe 0x04 x15) fuehrt; kann Elza die Waffe nicht
   tragen, bleibt Slot 11 aktiv und das Tagebuch bis dahin unerreichbar (kein Softlock — das Spiel
   laeuft weiter, die Waffe ist jederzeit nehmbar).
3. **ROOM1020 Umzug**: aendert nur den Vorrang im Dokument-Rechteck; kein anderer Satz schneidet das
   Tisch-Rechteck (3.4). Ohne Umzug waere Marvins Notiz UNERREICHBAR → Softlock fuer den Code 4312,
   sobald Spur F den Zettel in ROOM1110 entfernt (7.5). Riegel 6.1 d/e.
4. **ROOM1010 Spray-Vorrang**: wo sich die Zonen decken, zuerst das Spray (Original-Praxis Item vor
   Item). Das Blatt hat eigene 786 Treffer; nach dem Spray alle.
5. **Kopplung E + F (Softlock-Gefahr)**: Spur F ersetzt die Zettel „4312" (ROOM1110/1111 msg 0
   @0x0D68) und „5632" (ROOM1230/1231 msg 10 @0x16F4) — danach stehen die Codes NUR in Dok 3/Dok 4.
   F ohne E ausgeliefert = beide Tueren ohne Code. **E und F zusammen ausliefern** (oder F wartet).
6. **Langer Listenname Dok 1** (228 px): ragt 75 px ueber die Hervorhebungs-Kachel (x 43..196), bleibt
   in der Tafel (x 10..305); ohne Vorbild in RE1.5 (max 140 px). Wortlaut bleibt (Nutzer-Vorgabe).
7. **Satz FILE26..29** ist aus dem Werkzeug reproduzierbar (md5-Liste); wer `re2_doc_satz.py` aendert,
   aendert die Seiten mit — `satz_pruefung.py` erneut laufen lassen.
8. **Nachgezeichnete Masken aendern sich** (ROOM1010.MSK neu gezeichnet, ROOM1011.MSK kommt dazu):
   Klemmen neu messen (`endwerte.sh`); Riegel 6.1 j schlaegt an.
9. **Android-Bau** friert die GLOB-Liste ein (memory reai-v2-android-glob-cache): `app/.cxx` loeschen /
   neu konfigurieren, sonst fehlt `dokumente_r34.c`.
10. **PSX**: Installer, Zone und Leser sind Engine-Code und liefen dort mit; das WELTMODELL laedt
    aber nur der PC-Prop-Lader (`pc_load_room_prop_set`) — wie beim Irons Diary gaebe es auf PSX eine
    Aufhebe-Zone ohne sichtbares Modell. Offener Punkt 8.3.
11. **Spur D** (1050/1051 Slots 13/14, kleiner als 15): ein AKTIONS-Satz von D, der das Rechteck
    x[15250..16250] z[−7250..−6250] schneidet, faengt das Tagebuch ab. Spur A (Slots 11/12 in 1050)
    liegt am Rolltor-Schalter x[16800..17600] z[−8950..−8150] → keine Ueberschneidung; in ROOM1051 sind
    11/12 Original (Leichen-Satz/Waffe) und NICHT frei.
12. **obj 7 in ROOM1020** = TIM-Slot 27, derselbe wie die Granate in ROOM1150 (je Raum neu geladen,
    kein Konflikt).
13. **Speicherstaende aus spaeteren Baeuen** in aelteren exe: Dokument-Nummern 1..4 ohne
    Tabellenzeile → Zeile zeigt Unterstriche (`re15_files_doc` NULL), kein Absturz.

## 8 Offene Punkte

1. **Drehung** der Buecher (Dok 1, 2) und des Blatts Dok 3 = Grundstellung 0 (PORT-WAHL, 5.2):
   0 und 2048 sind gleichwertig (lange Achse gleich); die Wahl ist rein optisch — Abnahme im Bild
   beim Nutzer (`E_belege/drehung_bogen.png`, `drehung_bogen.py`: 0/1024/2048/3072 je Dokument an
   den Endwerten, ohne Licht, damit man die Ausrichtung sieht).
2. **Liste mit dem langen Namen** ist erst mit Port-Code am Artefakt pruefbar (6.2 Punkt 2).
3. **PSX-Seite** (7.10): Prop-Lader fuer portseitige Props fehlt dort generell (auch Irons).
4. **Absprache mit D** (7.11) ueber die Geometrie der Slots 13/14 in ROOM1050/1051.
5. **Auslieferung E + F gemeinsam** (7.5).

## 9 Umsetzung (Bau-Stufe)

### 9.0 Selbstpruefung des Plans
(offen)

### 9.1 Dok 1 — Police Officer's Final Diary Entry (ROOM1050, Item 0x49, FILE26)
(offen)

### 9.2 Dok 2 — Elliot's Diary (ROOM1000, Item 0x4A, FILE27)
(offen)

### 9.3 Dok 3 — Marvin's Notes (ROOM1020, Item 0x4B, FILE28)
(offen)

### 9.4 Dok 4 — Armory Notice (ROOM1010, Item 0x4C, FILE29)
(offen)

### 9.5 Codes 4312/5632 gegen Schloesser
(offen)

### 9.6 Dateien und Commits
(offen)

### 9.7 Suite
(offen)

### 9.8 Eigene Abnahme (Bilder)
(offen)

### 9.9 Abweichungen vom Plan
(offen)

### 9.10 Offene Punkte
(offen)
