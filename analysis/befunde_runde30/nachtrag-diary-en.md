# Runde 30 — Nachtrag J: Irons Diary auf Englisch (Spur diary-en)

Zweig `r30/n-diary-en`, aufgesetzt auf 80d579a5 (Integrationsstand der Runde).
Auftrag: `AUFTRAG.md` Abschnitt J — der englische Text des Nutzers steht wörtlich in
`analysis/befunde_runde30/irons_diary_en.txt` (2358 Zeichen, md5
`cf02eeb6f02cb65451546873b2bbef10`) und ersetzt den deutschen Text der Seiten FILE25
vollständig. Titelseite bleibt „IRONS DIARY" (Versalien wie alle 25 RE2-Titelseiten),
Listenname „Irons Diary".

Vorgänger-Dossier (Satzregeln, Glyphen-Atlas, Leser): `irons-diary-dokument.md` §6 und §10.

## Kurzfassung

| Größe | deutsch (Abschnitt E) | englisch (Nachtrag J) |
|---|---|---|
| Textseiten | 17 (p01…p17) | **15 (p01…p15)** |
| `max_page` (re15_files.c) | 17 | **15** |
| Leserseiten (Titel + Text) | 18 | **16** |
| Wörter | 427 | **408**, alle wortgleich zurückgelesen |
| Daten am Seitenkopf | 8 | **8** |
| Kernpixel ohne Glyphe | 0 | **0** |
| konstruierte Glyphen im Satz | 7 Zeichen (ä ö ü Ä Ö Ü ß) | **0** |
| Titel/p00/Illustration | — | byte-gleich den bisherigen Dateien |

Framedump im echten Spiel: alle 16 Leserseiten + Ende-Stellung + Leser aus der Liste zeigen
die erwartete englische Seite **pixelgleich** (5 Bit je Kanal, 162 … 9926 sichtbare Pixel je
Seite, 0 Abweichungen).

Kontaktbogen: `irons-diary-en_kontaktbogen.png` · Framedumps: `irons-diary-en_framedump.png`.

## 1. Zeichenprüfung VOR dem Satz

`re2_doc_satz.py satz` prüft jetzt jedes Zeichen von Text und Titel gegen den Atlas, bevor
es setzt (Bericht `build/r30_n_diary_en/zeichen_pruefung.txt`). Der Atlas wurde neu erzeugt
(`re2_doc_satz.py atlas`) und ist **byte-gleich** dem der Runde 30 E
(`re2_doc_font.json`, `cmp` gegen den Hauptbaum): 78 gemessene Zeichen + 7 Konstruktionen.
Nebenbefund: der Kontur-/CLUT-Zensus des Atlas-Berichts las alle `FILE*_page.TIM` und damit
die EIGENE Ausgabe FILE25 mit (210 statt 191 Seiten, 856 666 statt 781 561 Kontur-Sollpixel).
Jetzt nur RE2s Sätze 0…24: der Bericht zeigt wieder die Zahlen aus `irons-diary-dokument.md`
§6.1 (191 Seiten, 781 561, 277, 44); die Schrift-Datei ist davon unberührt (vor und nach der
Änderung byte-gleich).

* 50 verschiedene Zeichen in Text + Titel, **0 ohne Glyphe, 0 Konstruktionen**.
* Die gefragten Zeichen sind alle gemessen und gesichert: Apostroph (77 Belege im Original,
  Linkslage 1 in 77 von 77, Vorschub 6), Komma (108), Punkt (409; Folge „..“ Abstand 4 in
  38 von 38), Ziffern 0 1 2 6 8 9.
* Nicht gesichert (Atlas: < 3 Belege oder < 60 %) sind nur vier Zeichen. Ihr Vorschub wirkt
  im Text an **0** Stellen, weil sie jeweils am Zeilen- bzw. Absatzende stehen:

| Zeichen | Stellen | Vorschub wirkt | Linkslage (Beleg) |
|---|---|---|---|
| `!` | „soon enough!“, „out alive!“ | 0 | 3 (3 von 3) |
| `?` | „my city?“, „going on?“ | 0 | 1 (5 von 5) |
| `7` | „September 27“ | 0 | 1 (2 von 4; wie alle Ziffern) |
| `Y` | Titel „DIARY“ (unverändert gegenüber E) | 0 | 0 (2 von 2) |

* Neu: Zeichen mit KONSTRUIERTER Glyphe brechen den Satz ab, außer mit
  `--konstruktion-erlaubt` (die deutsche Fassung brauchte das).
  Negativ-Kontrollen: Text mit „ß/ä“ ohne Schalter → Abbruch; Text mit `#` → Abbruch
  „Zeichen ohne Glyphe“.

## 2. Satz

Gleiche Regeln wie in Abschnitt E (`irons-diary-dokument.md` §6.3, keine Messung): je Datum
eine neue Seite, Text direkt unter dem Datum, lange Einträge laufen auf Folgeseiten weiter,
keine Leerzeile am Kopf einer Folgeseite, Umbruch nur an Leerzeichen, keine Silbentrennung;
jede Zeile des Nutzertexts beginnt eine neue Satzzeile. Seitenraster FILE08: H 144, 9 Zeilen
je Seite, Zeilenraster 16, Rand 10, Grenze x 249.

Einzige Regeländerung: die Datumszeile wird jetzt auch englisch erkannt
(`September 18, 1998` / `September 19`; vorher nur `18. September 1998`).

| Datum | Seiten | Zeilen |
|---|---|---|
| September 18, 1998 | p01 | 6 |
| September 19 | p02 | 9 |
| September 20 | p03–p04 | 9 + 2 |
| September 21 | p05–p06 | 9 + 1 |
| September 22 | p07–p08 | 9 + 3 |
| September 26 | p09–p10 | 9 + 1 („can.“) |
| September 27 | p11–p12 | 9 + 6 |
| September 28 | p13–p15 | 9 + 9 + 1 |

Größtes Kernpixel-x je Seite 38 … 249 (Grenze 249). Seitenaufteilung mit jeder Zeile:
`build/r30_n_diary_en/FILE25_satz.txt`.

Aufruf (wiederholbar, deterministisch):

```
python re15_port/tools/re2_doc_satz.py atlas --out build/r30_n_diary_en
python re15_port/tools/re2_doc_satz.py satz  --out build/r30_n_diary_en \
   --font build/r30_n_diary_en/re2_doc_font.json \
   --text analysis/befunde_runde30/irons_diary_en.txt --titel "IRONS DIARY" --doc 25 --vorlage 8
cp build/r30_n_diary_en/FILE25_*.TIM re15_port/shared_assets/RE2/FILES/
python analysis/befunde_runde30/r30_diary_satz_pruefung.py --tim re15_port/shared_assets/RE2/FILES
python analysis/befunde_runde30/r30_diary_kontaktbogen.py re15_port/shared_assets/RE2/FILES \
   analysis/befunde_runde30/irons-diary-en_kontaktbogen.png
```

## 3. Dateien und Tabelle

* `shared_assets/RE2/FILES/FILE25_p01…p15_page.TIM` neu (je 18 496 B; Kopf, CLUT und Bildkopf
  byte-gleich `FILE08_p01_page.TIM`), `FILE25_p16/p17_page.TIM` **entfernt** (`git rm`).
* `FILE25_title_page.TIM` = `FILE25_p00_page.TIM` (md5 `481b94ab…`) und
  `FILE25_title_paper.TIM` (33 312 B, byte-gleich FILE08) sind **unverändert**.
* `re15_files.c`: `max_page` 17 → **15**. Port-Wahl, keine Original-Adresse — gemessen am
  Satz (`FILE25_satz.txt` „max_page … = 15“; p15 da, p16 nicht). Gegenstück zu RE2s u16
  @0x800AA144 + doc·4, gelesen `lhu a0,-24252(at)` @0x800727c8. Die Seitenzahl im Leser ist
  max_page + 1 = 16, die Fußzeile „n/16“.

md5 der Textseiten: p01 252870c8, p02 b2a519d8, p03 29f5e7d8, p04 94b53274, p05 7a4da625,
p06 574aa7c7, p07 7e8deceb, p08 d54f08a3, p09 3f7c8946, p10 fa2a0c7b, p11 7cc84342,
p12 5daaa795, p13 81fcc196, p14 a952573d, p15 e77f05e6.

## 4. Riegel mitgezogen (gemessen)

| Riegel | vorher | jetzt |
|---|---|---|
| `probe_r30_irons_diary_dokument` B | max_page 17, Fußzeile „1/18“ | max_page 15, „1/16“ (4 Glyphen) |
| … C | Seitenzahl 18, p17 da / p18 nicht | 16, p15 da / p16 nicht; 0 deckende 0x0000-Texel bei 97 106 sichtbaren |
| … D | 19 Seitendateien (Titel + p00…p17) | 17 (Titel + p00…p15) |
| … E | Seite 17 → p17, Ende 18 → p17 | Seite 15 → p15, Ende 16 → p15 |
| `test_inv_fsm` F6/F10 | file_end 18, letzte Seite 17 | 16 / 15 |
| `test_r30_irons_diary_ablauf` A2 | 18 Seiten | 16 |
| `test_r30_pfeil` A | 38 Stellungen | 34 (Seitenzahl aus `DIARY_LETZTE`, gegen die Tabelle geprüft) |
| `test_r30_pfeil` B Negativ-Kontrolle | 126 Pixel auf 13 Stellungen, p01 8, p02 13, Ende 7 | **79 auf 10**, p01 13, p02 7, Ende 0 |

Die Negativ-Kontrolle (RE1.5-Pfeile an RE1.5-Lage, DEBUG.BIN @0x800c7554-70, gegen die
Textseite) hängt am TEXT der Seiten und wurde zweimal unabhängig gemessen: im C-Riegel und
mit `r30_pfeil_ueberdeckung.py` (liest die Seitenzahl jetzt aus dem Dateibestand) —
p01 13, p02 7, p03 7, p05 3, p07 6, p09 7, p11 14, p12 6, p13 13, p14 3, Summe 79. Ende 0,
weil p15 nur Zeile 0 trägt (y 30…45) und der Pfeil bei y 0x70…0x7f liegt. RE2-Lage: 0.

`test_re2doc_bildebene` liest max_page aus der Tabelle (nur Kommentar geändert).

## 5. Abnahme

### 5.1 Am Artefakt (die KOPIERTEN Dateien)

`r30_diary_satz_pruefung.py --tim re15_port/shared_assets/RE2/FILES`
(Bericht `build/r30_n_diary_en/satz_pruefung_kopierte_dateien.txt`):

```
[1] Nutzertext irons_diary_en.txt (2358 Zeichen) gegen satz_eingabe.txt   ZEICHENGLEICH
[2] Ruecklesen von 15 Textseiten: Kernpixel ohne Glyphe 0, Abstaende ausserhalb der Metrik 0
[3] Wortfolge: Nutzertext 408 Woerter, zurueckgelesen 408 Woerter        WORTGLEICH
[4] Daten im Nutzertext: 8   Seiten mit Datum am Kopf: 8 (p01 p02 p03 p05 p07 p09 p11 p13)
[5] 0 von 15 Textseiten mit abweichendem Kopf; title_page = p00; title_paper = FILE08;
    Titelseite zurueckgelesen: Rasterzeile 4 = 'IRONS DIARY';
    Dateibestand 18 Dateien = Titel + Illustration + p00..p15, kein p16
[6] konstruierte Glyphen zurueckgelesen: 0
[7] kopierte Dateien gegen die Satz-Ausgabe: 18 von 18 byte-gleich
ERGEBNIS: ALLE PRUEFUNGEN BESTANDEN
```

Das Prüfwerkzeug wurde erweitert (Soll = `irons_diary_en.txt`, `--satz`/`--tim` getrennt,
[5] Dateibestand, [6] Konstruktionen, [7] Kopie). Negativ-Kontrollen: eine fremde p17 im
Prüfverzeichnis → [5] „zu viel“, [7] 18 von 19; ein Satz mit „ß/ä“ → [6] meldet 2.

### 5.2 Kontaktbogen

`analysis/befunde_runde30/irons-diary-en_kontaktbogen.png` (2078 × 2872): oben alle 16
Leserseiten auf neutralem Grund (2×), unten im RE2-Schirmlayout (Grund schwarz, Illustration
bei (100,60), Textseite bei (25,30)) — aus den kopierten TIM gelesen.

### 5.3 Framedump im echten Spiel

`analysis/befunde_runde30/r30_diary_en_lauf.sh lauf1`: Speicherkarte des Nutzers Platz 0
(ROOM1150), `RE15_DOC_REQUEST=260` (Aufnahme-Leser), RECHTS alle 90 Bilder bis zur
Ende-Stellung, KREUZ, Meldung bestätigen, Menü → FILE → Zeile 0 öffnen. `RE15_FRAMEDUMP`
(Readback vor `SDL_RenderPresent`, beschleunigter Renderer); kein AUTOSHOT, kein
SOFTWARE_RENDER. Auswertung `r30_diary_en_framedump.py`
(`build/r30_n_diary_en/lauf1/framedump_abnahme.txt`): je Abzug die beste Seite unter allen
englischen UND den 17 alten deutschen Seiten (aus 29391b2b), Maß = Anteil gleicher sichtbarer
Textseiten-Pixel. Die beste deutsche Seite erreicht höchstens 44,4 % (meist die einzeilige
alte p14 mit 669 Pixeln), die erwartete englische jedes Mal 100 %:

| Bild | Leser-Log | erwartet | beste | gleich | beste deutsche |
|---|---|---|---|---|---|
| F310 | page 0/16 | Titel | Titel | 717/717 | p17 170/5741 |
| F400 | page 1/16 | p01 | p01 | 6349/6349 | p14 265/669 |
| F490 | page 2/16 | p02 | p02 | 9358/9358 | p14 256/669 |
| F580…F1570 | page 3…14/16 | p03…p14 | je gleich | je 100 % | ≤ 44,4 % (F1390: p14 297/669) |
| F1660 | page 15/16 | p15 (letzte) | p15 | 1111/1111 | p14 266/669 |
| F1750 | page 16/16 (Ende) | p15 | p15 | 1111/1111 | p14 266/669 |
| F2200 | Leser aus der Liste, page 0/16 | Titel | Titel | 717/717 | p17 170/5741 |

Nicht-schwarz außerhalb Textseite und Illustration in allen Abzügen nur zwei Bänder: y 111…122
(Pfeile x 9…23 / 282…296, Ende-Marke x 280…306) und y 211…222 (Fußzeile „n/16“,
x 141…178). F1840 zeigt die Meldung „The Irons Diary has been filed.“ (Schreibmaschine
läuft), F2110 die FILE-Liste mit „Irons Diary“ auf Zeile 0.
Bild: `analysis/befunde_runde30/irons-diary-en_framedump.png` (Titel, p01, p07, p15, Ende,
Meldung, Liste, Leser aus der Liste; 2×).

### 5.4 Suite

Windows, Bau dieses Zweigs, `ctest --timeout 240` (volle Suite, 550 s): **402 von 403 grün**.
Rot nur `integration_r30_irons_tisch_licht` — einzeln wiederholt ebenfalls rot, mit
„FEHLER: Bildgroessen 320x240 / 320x240, erwartet 960x720“: das Spielfenster liefert in
dieser Sitzung einen 320×240-Readback statt 960×720 (auch die Framedumps dieses Nachtrags
sind 320×240). **Nicht von dieser Spur verursacht:** derselbe Test mit der exe und den
Sonden des Integrationsstands 80d579a5 (Arbeitsbaum r30_integration, eigenes WORKDIR)
scheitert identisch. Die sechs gezielten Riegel dieser Spur (`unit_inv_fsm`,
`unit_re2doc_bildebene`, `unit_r26_inventar`, `unit_r30_irons_diary_dokument`,
`unit_r30_irons_diary_ablauf`, `unit_r30_pfeil`) sind grün.

## 6. Offen / nicht Teil dieser Spur

1. **Kurze Folgeseiten.** p06 („earth is going on?“), p10 („can.“) und p15 („hope you make it
   out alive!“) tragen je eine Zeile, p04 zwei — Folge der 9 Zeilen je Seite und der Regel
   „jedes Datum beginnt eine Seite“, wie in der deutschen Fassung (dort p07/p14 mit einer
   Zeile). RE2 selbst blättert solche Seiten (FILE08 p04 ist ganz leer). Eine andere
   Aufteilung wäre eine neue Satzregel, keine Messung.
2. **Paket:** die entfernten `FILE25_p16/p17` dürfen in einem Paket nicht aus einem alten
   Staging-Ordner nachkommen (folgenlos für das Spiel — max_page 15 fragt sie nie ab —, aber
   Ballast). Frisch paketieren.
3. **Linux-Bau-Dauer (Abschnitt L)** ist eine eigene Spur (`r30/n-linux-bau`), nicht diese.
4. Wie in E: kein Bild vom laufenden RE2; PSX-Ziel ohne Bild-Ebene.

## 7. Artefakte

| Pfad | Inhalt |
|---|---|
| `re15_port/tools/re2_doc_satz.py` | Datum englisch, Zeichenprüfung vor dem Satz, `satz_eingabe.txt`, Zensus nur Sätze 0…24 |
| `analysis/befunde_runde30/r30_diary_satz_pruefung.py` | Abnahme am Artefakt, erweitert ([5]–[7]) |
| `analysis/befunde_runde30/r30_pfeil_ueberdeckung.py` | Seitenzahl aus dem Dateibestand |
| `analysis/befunde_runde30/r30_diary_en_lauf.sh` | Framedump-Lauf im echten Spiel |
| `analysis/befunde_runde30/r30_diary_en_framedump.py` | Auswertung der Framedumps |
| `analysis/befunde_runde30/irons-diary-en_kontaktbogen.png` | Kontaktbogen aller 16 Leserseiten |
| `analysis/befunde_runde30/irons-diary-en_framedump.png` | Framedumps aus dem Spiel |
| `build/r30_n_diary_en/` (nicht versioniert) | Atlas, Satz, Berichte, Lauf `lauf1/` |
