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

(in Arbeit)

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
