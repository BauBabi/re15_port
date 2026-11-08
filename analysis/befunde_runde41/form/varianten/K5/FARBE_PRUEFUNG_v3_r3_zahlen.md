# Zahlen-Pruefung Farbe Versuch 3, Runde 3 (K5c V3, G4-Uebertragung), 2026-11-07

Gegenstand: `varianten/K5/farbe/K5c_farbe_v3/` (FORM.EMD sha256 d19fe901..., FORM.TIM sha256 abd0ba1b...). Beide Dateien sind bytegleich mit
`farbe/K5C_FARBE_V3.EMD/.TIM` und mit `formfarbe3/emd/` (nachgerechnet). Geprueft sind die Commits 819990a36, 76c6c7af9, c137bfe08, b1ddd511e
und ccdecce8f auf r41/form (HEAD ccdecce8f).

Eigener Code: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3/pz3/`.
Der EMD/MD1/TIM-Leser `emd.py` und die Abtastung `basis.py` (861 baryzentrische Punkte je Dreieck, floor) sind unabhaengig von FE, kf_g4 und riegel_farbe_v3.
- Atlas-Zeilen: `g0.py`, `h1s1d1.py`, `p1h2.py`, `h2_zeile6.py`, `f1_rand.py`, `f1_probe.py`, `feld.py`, `feld_w.py`, `a8_eigen.py`, `rosa.py`, `kralle.py`
- Gegenprobe: `gp_k5c/`
- Neubau aus HEAD: `neubau/`
- Nachlauf des Riegels: `riegel_nach.txt`/`.json`
- Saaten: `saaten.py`/`saaten.txt`
- Tests: `ctest.log`
- Beleg-Ausschnitt: `f1_ausschnitt_x3.png`

Ketten- und Render-Zeilen (K, B, P2, N) habe ich wie in Runde 2 nicht neu implementiert. Ich habe sie nachlaufen lassen und gegengeprueft:
mit 5 Saaten, mit den Originalmasken des Messblatt-Pruefers und mit dem Neubau.

## Ergebnis

**Nicht bestanden.**

- Mein Nachlauf des Riegels ist **zeilengleich mit der committeten Datei** `farbe/riegel_K5c_farbe_v3.txt`: 32/52, Exit 1, Gegenproben 13/13 wie verlangt.
- **Die Riegel-Tabelle der Bauer-Meldung stimmt nicht mit dieser Datei ueberein.** Zwei Zeilen sind dort als GRUEN gemeldet, die in der Datei und in meinem Nachlauf ROT sind:
  - **H2:** In CLUT-Zeile 6 stehen 6 Vertauschungen. Mit eigenem Code bestaetigt, siehe Befund A.
  - **B14:** (8,0; 10,0), ROT.
  - Vier weitere Werte der Meldung stammen aus dem Zwischenlauf `V3B_r3_skripte/r_v13_offiziell.txt` (vor c137bfe08 "Hand 18"), nicht aus dem Endstand. Siehe Befund B.
- **Es sind 20 Zeilen ROT:** H2, P2, F2t, A1, A8, B10, B11, B12, B13, B14, B16, B17, N1, K3, K6, K7, K1v2, K2v2, B2, B9.
- **Befund A (neu):** 12 von 162 Fleck-Texeln der F1-Stelle (7,4 %) behalten das gesaettigte G4-Blutrot, zum Beispiel (173,16,16), Lab (36,4 / 57,8 / 43,8).
  - In Tafel 01 Z.1 sieht man sie als rote Zacken am oberen Rand des rosa Flecks.
  - Ursache ist ein Baufehler in `kf_g4.bau`: Die Texel des dunklen Rands werden nicht umgefaerbt.
  - F1t haelt deswegen nur knapp: dE 5,84 bei Grenze 6. Ohne diese Texel waeren es 3,48.
- **Mit eigenem Code bestaetigt:**
  - G0, Neubau aus HEAD bytegleich, Seiten 0-2 und CLUT 0-2, H1, H1b, S1, P1, D1 ortsgenau
  - F1t, F2t ROT, F3t, A8 ROT, Krallen-Streifen
  - Masken und Grenzen unveraendert
  - Tests gruen; Spiel-Asset, Pins und re15_port unveraendert

## (1) Riegel-Zeilen nachgemessen

| Zeile | Bauer-Meldung | Riegel-Datei / mein Nachlauf | eigene Messung | Bewertung |
|---|---|---|---|---|
| G0 | GRUEN | GRUEN | EMD-Abschnitte 0-6 bytegleich; Laenge 170 424 und Verzeichnis gleich. MD1 mit 32 Meshes, 1270 Dreiecken und 2558 Vierecken: tv, tn, tf, qv, qn und qf in jedem Mesh gleich. 35 468 Bytes sind verschieden, **0 davon ausserhalb der UV-Bloecke** (`g0.py`) | **GRUEN bestaetigt** |
| Neubau | - | - | `kf_g4.bau()` aus HEAD in `pz3/neubau/` ergibt EMD d19fe901... und TIM abd0ba1b..., **bytegleich** mit dem Commit | bestaetigt |
| H1 | 100,0 | 100,0 | Index gleich G4 und Texel in der G4-Inselmaske der Basiszeile: Rumpf, Schwanz, Hals und Arme 100,00; Beine 99,99; Klauen und Kopf 99,98 | **GRUEN bestaetigt** |
| H1b | 1,07 | 1,07 | 1,01 % | GRUEN |
| **H2** | **GRUEN, 0 Vertauschungen** | **ROT, Zeile 6: 6 Vertauschungen** | Zeilen 0-2 bytegleich. Vertauschungen bei dL >= 2: Zeilen 3, 4, 5, 7 und 8 je 0, **Zeile 6 6** (`p1h2.py`, `h2_zeile6.py`). Siehe Befund A | **ROT. Die Meldung ist falsch** |
| P1 | 20,8 / 27,2 / 275 | 20,9 / 27,1 / 281 | 20,7 / 27,2 / 281; ohne Sonderzeilen 21,1 / 27,8 / 222; G4 17,7 / 25,6 / 423 | GRUEN. Die Meldung zitiert r_v13 |
| D1 | ortsgenau GRUEN; Formel 1,03-2,81 Info | gleich | Ortsgenau, Median je Teil 0,86-1,22. Am Kopf: p90 3,45, 24,8 % der Flaeche > 2,0. Wiederholung (UV-Flaeche / eindeutige Texel): Klauen **44,9**, Hals 14,6, Rumpf 10,5 | Gate GRUEN. **Nach dem Wortlaut der Messblatt-Formel ROT** (Rumpf 2,64, Hals 2,81, Kopf 2,78), offen ausgewiesen, wie Runde 2 Befund D. Siehe Hinweis D |
| S1 | 0,00 | 0,00 | 0,00-0,02 % | **GRUEN bestaetigt** |
| F1t | 5,84 GRUEN | 5,84 | 162 Fleck-Texel: Lab (43,81 / 23,28 / -0,44), **dE 5,83**. Die 458 Haut-Texel sind eine echte G4-F11-Kopie (Index, Maske und Farbe gleich) | GRUEN, **Reserve 0,16**. Siehe Befund A |
| F2t | 15,09 ROT | ROT | 561 Texel: dE 15,00; 3D-gewichtet 12,86 | ROT bestaetigt (bewusst, V3R3.3) |
| F3t Zaehne | 77,7 / 5,3 | gleich | 77,68 / 5,28; 3D-gewichtet 76,4 / 5,0 | GRUEN |
| A8 nach dem Wortlaut | 2,0 / 36,3 / 67,5 ROT | gleich | Feld = Mesh 23 aussen + Band (22/23), Klassen nach V1 Z.160: **Zahn 1,87 %**, Rot 37,1 %, Dunkel 65,5 %. Mit den Zaehnen von 23 (Feld der Runde 2): 7,4 % | **ROT bestaetigt.** Die Umdefinition aus Runde 2 ist zurueckgenommen (Befund B der Runde 2 erledigt) |
| **B14** | **GRUEN** (8,0; 10,0) | **ROT** (8,0; 10,0) | Nachlauf ROT. GRUEN (8,5; 10,0) gibt es nur in r_v13 | **ROT. Die Meldung ist falsch** |
| B15 | 5,1 | 5,3 | Nachlauf 5,3 | GRUEN. Die Meldung zitiert r_v13 |
| K9 | Klauen a\* 31,4 / 30,6 | 27,9 / 30,6 | Nachlauf 27,9 / 30,6 | GRUEN. Die Meldung und das Dossier V3R3.1 zitieren r_v13 |
| K13 | 66,4 | 66,7 | Saaten 15 / 12345 / 7 / 1 / 99: 66,7 / 66,5 / 66,5 / 66,5 / 66,6 | GRUEN |
| K11 | 0,3 / -1,8 | gleich | 5 Saaten: Figur -0,4 bis 0,3, Ruecken -1,9 bis -1,7 | GRUEN, stabil |
| K12 Schwanz | 6,1 / 6,5 / 6,2 | gleich | 5 Saaten: 6,1 / 6,5 / 6,2 / 6,3 / 6,4 (Grenze 8,9) | GRUEN, stabil. Befund C der Runde 2 ist erledigt; Hinweis C |
| K14 | dE 3,1, Rot 61,6 | gleich | 5 Saaten: dE 3,1-3,3, Rot 61,1-62,1 % | GRUEN, stabil |
| K15 | dE 5,2, Rot 96,2 | gleich | 5 Saaten: dE 5,2 / 5,4 / 5,4 / 5,4 / 5,4 | GRUEN, **Reserve 0,6-0,8**. Haengt an der mauve Hand 18 (Hinweis E) |
| F1-Flaeche (Info) | 731 px, IoU 0,75 | gleich | 5 Saaten: 731-740 px, IoU 0,75 | Info |
| Masken K14/K15 | - | - | `generator/v3_daten/masken.npz` ist gleich den Originalen des Messblatt-Pruefers (`formfarbe3/pruef/F1_pruef.npy` 646 px, `band2_pruef.npy` 2227 px, `array_equal`) | unveraendert |
| uebrige Zeilen (A1-A7, A9-A12, B10-B13, B16, B17, N1, N2, K3-K8, K10, K1v2, K2v2, K4b, B2, B9, B9b, P2) | wie Datei | Nachlauf zeilengleich | - | wie in der Datei |

Rosa Strichel am Oberarm 16, eigene Zaehlung (`rosa.py`; Rotklasse K11 und Farbton < 10 Grad):
- Je Flaeche summiert: 2026 -> 496. Die Bauer-Zahl 1978 -> 495 ist bestaetigt.
- Als 3D-Anteil: 7,0 % -> 1,6 %.
- Die Hand 18 bleibt bei **28,3 % rosa (3D)**, in Runde 2 waren es 34,0 %.

### Befund A: Der dunkle F1-Rand traegt ungefaerbtes G4-Blutrot, deshalb ist H2 ROT

**Messung** (`f1_rand.py`, `h2_zeile6.py`): 12 der 162 gelesenen Fleck-Texel (Art 2) nutzen 8 Eintraege der Zeile 6, die nicht umgefaerbt sind.

| Eintrag | Farbe = G4-Zeile 2 | Lab |
|---|---|---|
| 78 | (173,16,16) | (36,4 / 57,8 / 43,8) |
| 245 | (181,16,25) | (38,3 / 60,2 / 42,2) |
| 246 | (123,8,16) | |
| 232 | (90,0,8) | |
| 86, 84, 75, 67 | | |

- Keiner der 8 Eintraege ist zugleich ein Haut-Eintrag.
- Lage auf Seite 3 (Atlas): x 12-34, y 16-25.

**Zeile 6 vertauscht** die Eintraege 78 und 245 gegen die umgefaerbten Eintraege 93, 94, 219, 107, 220 und 250.
- Im G4 sind 78/245 um 2,0-2,4 L\* heller, jetzt sind sie um 0,1-1,1 L\* dunkler. Das ergibt die 6 Vertauschungen.

**Sichtbar:** In Tafel 01 Z.1 (Disc-Licht) sitzen am oberen Rand und rechts am rosa Fleck kleine, gesaettigt rote Zacken.
- Beleg: `pz3/f1_ausschnitt_x3.png`, ausgeschnitten aus `01_szene_bild1.png` x 1380-1680, y 150-450.
- Genau dieses G4-Rot mit rund 30 Grad nennt Messblatt V3 0. Punkt 2 als falsch fuer Bild 1.

**Ursache** im Code (76c6c7af9, `kf_g4.py` `bau`, F1-Abschnitt):
- `fleisch_texel` zaehlt nur noch Kern-Texel: `if hart[y, x] == ART_F1 and kern[y, x]`.
- Rand-Texel, deren Eintrag im Kern nicht vorkommt, landen weder in `e` noch in `fest` noch in `ef` von `zeile_ziel`.
- Darum werden sie nicht umgefaerbt.
- Auch `rang_reparatur` sieht diese Eintraege nicht und meldet in `bau.json` "rang_repariert 0".
- Der Riegel prueft dagegen alle genutzten Eintraege und findet die 6 Vertauschungen.

**Wirkung auf F1t:**
- Mit den 12 Texeln dE 5,84, ohne sie Lab (45,29 / 21,51 / -2,90) mit dE 3,48.
- Gegenprobe im Speicher (`f1_probe.py`): Jeder der 8 Eintraege bekommt die Farbe des naechsten umgefaerbten Eintrags nach G4-Lab. Ergebnis: F1t dE 4,92, H2 Zeile 6 noch 4 Vertauschungen.
- Die Behebung gehoert also in `zeile_ziel`: die Rand-Texel mitgewichten oder ihre Eintraege als `ef` fuehren. Ein Nachfaerben reicht nicht.

### Befund B: Die Meldung mischt den Endstand mit dem Zwischenlauf r_v13

- `V3B_r3_skripte/r_v13_offiziell.txt` ist der Lauf mit der Hand-18-Teilung, also vor c137bfe08. Dort ist K15 dE 6,0 und ROT.
- Aus r_v13 stammen in der Meldung: P1 20,8/27,2/275, B14 GRUEN, B15 5,1, K9 31,4 und K13 66,4.
- H2 ist in r_v13 und im Endstand ROT. GRUEN war H2 nur im Lauf r_v1.
- Das Dossier V3R3.3 fuehrt H2 und B14 in der GRUEN-Liste. Die Liste hat damit 34 Eintraege und widerspricht der eigenen Zahl 32.
- **Gilt:** `farbe/riegel_K5c_farbe_v3.txt` (15:16, nach dem EMD-Bau um 15:13), von mir zeilengleich nachgelaufen. Die Gesamtzahl 32/52 der Meldung stimmt.

### Hinweis C: Die Seite-1-Buntheit ist auf den Riegel K12 geeicht

- B1_C_SEITE1_EICH 0,535 ist das Ergebnis von 5 Laeufen gegen den K12-Originalwert 5,9 % (Dossier V3R3.1).
- Die Quelle ist sauber angegeben. Danach ist K12 am Schwanz aber kein unabhaengiger Beleg mehr, nur noch die Eichung selbst.
- Unabhaengig bleiben K11 (Farbton, GRUEN) und K4/K4b.

### Hinweis D: Krallen sind eine 45-fache Wiederholung von 415 Texeln

- Die 8 Krallen nutzen einen G4-Streifen von 8 x 53 Texeln. Die Wurzelzeilen 202-213 werden Ring fuer Ring gespiegelt.
- Ortsgenau ist die Texeldichte gut (Median 0,86).
- Die D1-Infozeile "ohne Wiederholung" rechnet fuer Krallen aber die G4-Quellflaeche 4-fach (`d1_messblatt`, neu: `art in ('einzeln', 'kralle')`). Damit kommt sie auf 1,07.
- Gezaehlt ueber die eindeutig genutzten Texel sind es 4,78. Die Infozeile ist fuer Krallen also nicht "ohne Wiederholung".
- Ob man die Spiegelung sieht, beurteilt die Sicht.

### Hinweis E: K15 GRUEN haengt an der mauve Hand 18

- Die Hand 18 behaelt die Bild-1-Zeilen auf allen Flaechen (c137bfe08). Sie liegt mit 192 px in der Bandmaske.
- Mit der Teilung waere K15 bei dE 6,0 und ROT (r_v13).
- Die Hand ist in der Kette ohnehin zu dunkel: Knochen 18 V (32,2 / 32,2 / 20,5) gegen Original (48,1 / 27,6 / 21,0) (`bk_end.txt`).
- Der Sicht-Befund "Hand mauve" (V3R3.5 Punkt 4) und K15 haengen also direkt zusammen. Das muss der Messblatt-Pruefer entscheiden.

## (2) Geometrie, Spiel-Asset, Pins

- G0 und der Neubau sind bestaetigt (siehe oben).
- `git diff 02aed15f7..HEAD -- re15_port` ist leer.
- Ausserhalb von `analysis/` ist keine Datei geaendert.
- Nichts in Version_80%/, pri/ oder synchro/ ist geaendert.
- `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` hat sha256 16eb6ea8..., `.TIM` ec280802.... Zuletzt geaendert in 67408cfcb (K4).
- **Bestaetigt.**

## (3) Tests

- `ctest --test-dir re15_port/build -R "form|bahnsteig|birkin_form|r41_form" -j 3` auf dem vorhandenen Bau: **11/11 bestanden**, 104,3 s (`pz3/ctest.log`).
- Zur selben Zeit liefen zwei fremde ctest-Prozesse.
- Die Tests pruefen K4, nicht V3. Das ist korrekt, weil das Spiel-Asset unveraendert ist.
- Spiellauf-Log `farbe/spiel_lauf_v3.log`:
  - 8x rc=0 und 2x rc=1 ("alle Versuche schwarz", K4 R5080). Beide sind am Ende einzeln wiederholt mit rc=0.
  - V3 laedt als "32 Meshes, 32 Knochen, 3 Clips".
  - Alle 10 Bilder in `spiel_v3/` sind 640x480 mit Mittelwert 35-43, keines ist schwarz.
- Bestaetigt.

## (4) Gegenproben

- Der Nachlauf mit `--gegenprobe-v2 farbe/K5c_farbe_v2` ist zeilengleich: 13/13 wie verlangt, "falsch ausgefallen: keine".
- Die Orange/Gelb-Probe faerbt jetzt nur die Feld-Eintraege: F1t/F2t 74,6/50,1 und 87,3/72,4, ROT. Befund (4) der Runde 2 ist erledigt.
- Eigene Gegenproben:
  - **K5c ohne Farbe (H1):** Rumpf 0,39, Schwanz 0,05, Hals 0,19, Beine 0,00 % (`gp_k5c/`), also ROT. H1 trennt.
  - **G4 P1:** 17,7 / 25,6 / 423, GRUEN.
  - **F1-Rand nachgefaerbt:** siehe Befund A.
  - **5 Saaten** fuer K11-K15, siehe Tabelle.

## (5) Konstanten (Diff 819990a36..HEAD, generator/)

| Konstante | Quelle laut Code / Dossier | eigene Nachmessung | Bewertung |
|---|---|---|---|
| KRALLE_INSELN (14,7), (14,15), (19,7), (19,15) | gemessen (Bbox, L\*) | `kralle.py`: Streifen u 229-240, v 200-254 | mit Quelle |
| v_r 213 | letzte Zeile in der Rotklasse A3 (V1 6) | Zeilen 200-213 in A3, Zeile 214 a\* 12,6 | bestaetigt |
| v_s 254 | letzte Zeile mit Index != 0 | Zeilen 252-254 haben L\* 0,9-3,6 | bestaetigt |
| u 230 / 237 | Schnitt der Streifenbreiten der Zeilen 202-213 | 230 / 237 | bestaetigt |
| v_w 202, Regel "Breite >= 8 Texel" | Text "Zeilen 200-205 sind 2-7 Texel schmal" | Zeilen 200/201 haben 4/6 Texel und sind auch A3-rot | **Die Schwelle 8 hat keine Quelle.** Sie wirkt auf 2 Zeilen; schwach |
| rho 0,0957, Kanten "\|dv\| > 20 Texel" | Median 0,087-0,102 je Insel | - | **Die Schwelle 20 hat keine Quelle**; schwach |
| Spitzenglieder k | Regel "Vergroesserung am naechsten 1" | bau.json: 2-4 | Definition |
| F1-Kern "> 0,5 x Median" | Mehrheitsregel | bau.json: kern 122, volle Deckung 5,0 | Definition |
| F1-Rand "1 Ring, dunklere Haelfte" | Messblatt 3.1 "dunkler Rand ... erlaubt" | - | Das Messblatt erlaubt den dunklen Rand. "Haelfte" (Median-Teilung) ist eine eigene Wahl, und **mit dieser Regel kam Befund A** |
| ZIEL_BAND_KETTE (182,117,102) | Rundlauf an Knochen 23 / Ketten-Faktor 1,017/0,963/1,030 (Variante r3/v3) | Rundlauf T (185,5/112,7/105,4) bestaetigt. Am Endstand ist der Faktor 1,030/0,969/0,994, das ergibt (180,116,106), dE 2,9 zu ZIEL_BAND_KETTE (`r3/kette_band.npz`) | Mit Quelle. Die Faktoren der Quell-Variante sind nicht committet. Riegel-Ziel ist F2t gegen K15: Messblatt-Pruefer |
| B1_C_SEITE1_EICH 0,535 | Eichung auf K12-Original 5,9 %, 5 Laeufe | 5 Saaten 6,1-6,5 % | mit Quelle; Hinweis C |
| B1_SICHTBAR_NR 16: 39, 17: 37 Flaechen | Leser B fid1.npz, >= 1 px in Bild 1 | rosa 3D-Anteil an Mesh 16: 7,0 -> 1,6 % | mit Quelle |
| Hand 18 ohne Teilung | K15 6,0 mit Teilung (r_v13) | - | Wahl nach Riegel; Hinweis E |
| Riegel-Grenzen und -Masken | - | Diff riegel_farbe_v3.py: Grenzen unveraendert. Neu sind A8 nach dem Wortlaut, F1-Flaeche (Info), Orange/Gelb nur Feld-Eintraege und D1-Info `kralle` x4 (Hinweis D). Die Masken sind gleich den Originalen des Pruefers | sauber; D1-Info fuer Krallen irrefuehrend |
| entfallen: WEICH_RAND, MORPH_CLOSE, 0,574 | - | im Code nicht mehr wirksam | Befund (5) der Runde 2 ist erledigt |

## (6) Tafeln

- Gesichtet habe ich 01, 02 und 03 (verkleinert, `pz3/klein_*.png`).
- **01/02:** K4 und V3 bekommen dasselbe Disc-Licht mal k, (1,06/0,95/0,82) bzw. (1,20/1,12/1,00), und denselben Filter mit Saat 15. Das Original ist roh. Die Nahkacheln sind wie in Messblatt 6.3.
- **03:** Fuer alle fuenf Modelle gelten Licht 0,55 + 0,45 N.L, Umkugel 0,46 und roh. Die halbe Pixelzahl der Vierbeiner ist vermerkt.
- Kein Modell ist einseitig aufgehellt. **Fair.**
- Zu sehen in 01 Z.1: die roten Zacken am F1-Rand (Befund A).

## Baum

- Der Baum ist sauber bis auf diesen Bericht, der nicht committet ist.
- Fremde, unversionierte Dateien sind `form/abn2/` und `FARBE_PRUEFUNG_v3_r3_sicht.md`. Sie sind nicht von mir.
- Kein Upstream, kein Merge-Commit in 02aed15f7..HEAD, kein Push. Die 5 Commit-Texte sind Deutsch, ohne Backticks und enden mit Co-Authored-By.
- Den Neubau habe ich nur in `pz3/neubau/` geschrieben, ueber `bau()` direkt. `main()` haette die committeten Kopien ueberschrieben.
