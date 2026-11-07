# Kandidat C (Bildnah): Dossier

Stand 2026-10-31, Lauf `lauf/ende`. Deutung: keine Anatomie vorweg. Jedes Teil kommt aus seinem Umriss im Bild, die Tiefe aus Kamera und Massstab, die Toene aus `farbe_bild1_echt.png` und `farbe_bild2_echt.png`.
⛔ Kein RE-Wert: Die Figur ist ein neues Modell nach zwei Videostandbildern. Keine Zahl hier ist Disc-Beleg. Formvorlagen nur `bild1.png`, `bild2.png`.

## 1. Ergebnis

Messung `form_pruefung.py`, `lauf/ende/bericht.txt`. Alle zaehlenden Punkte bestanden, 0 mal NEIN.

| | Bild 1 | Bild 2 |
|---|---|---|
| Umriss IoU auf sicheren Pixeln (Ziel Bild 1 >= 0,90) | 0,997 | 0,981 |
| Weiss gedeckt | 0,999 | 0,982 |
| Schwarz getroffen | 44 px | 26 px |
| Pruefpunkte | 33/33 | 7/7 |
| Boden frei, Verdeckung | 3/3 | 9/9 |
| Zeichnung | 9/9 | 7/7 |
| Verbot | N2 ok | N1 ok |
| MAE, NCC im Weiss | 9,6 / 0,852 | 8,5 / 0,803 |

Rahmen: 1842 Flaechen (423 Dreiecke, 1419 Vierecke), 21 Knochen, 21 Meshes, Textur 384x256 8 bpp, 1 CLUT-Zeile, 3 Clips je 1 Bild, Boden je Pose y=0. `FE.pruefe` 0 Fehler, 1 Warnung (Tick-Clips 3 bis 27 fehlen, die gehoeren zur naechsten Stufe).

**Wie wenig diese Zahlen beweisen.** Sie sind zum grossen Teil selbstbestaetigend:
- Das Netz ist auf dieselbe Zielmaske gezogen, die IoU und Punkte messen. Die Zeichnung ist in genau die Polygone gemalt, die der Zeichnungstest abfragt.
- Die Toene sind im geschlossenen Kreis gegen dasselbe Original geregelt, gegen das NCC und MAE rechnen. Y Video/Render stimmt deshalb an vielen Marken fast genau.
- Punkt `b1.nr_y346_links` und die Flaechen L1, L2 bestehen nur durch sechs von Hand gesetzte Ecken an der Taille (Abschnitt 5).
- Pose 2 ist gegen die Zonen von Bild 2 gesucht, deren Treffer die Tafel dann zaehlt.

Aussagekraft haben die Lage der Marken im jeweils anderen Bild (Abschnitt 4), das Rundumblatt und die Spielbilder. Das Spiel beweist nur Laden, Zeichnen und Stand, keine Aehnlichkeit.

**Urteil am Bild:** Grossform, Glieder und Lage der Zeichnung sind in beiden Bildern wiedererkennbar. 1:1 ist es nicht: Die Textur liest sich anders als das Original (Abschnitt 6), und Pose 2 ist ein starker Umbau.

## 2. Bau

Aktorraum Pose 1: +X = Ende N (vorn), Y unten, +Z = Seite R. Knochen 0 ist die Wurzel.

| Knochen | Name | Eltern | Teil |
|---|---|---|---|
| 0, 9, 10 | rumpf_h, rumpf_m, rumpf_v | -, 0, 9 | ein Netz, bei X -420 und +240 geteilt, Flaechen am Schnitt liegen in beiden Teilen. **10 = Rumpf** |
| 1, 2, 3, 4 | fr_huefte, fr_bein, fr_fuss, fr_zeh | 0, 1, 2, 3 | Schulter S, Pfeiler A1, Fuss A2, Spitze |
| 5, 6, 7, 8 | fl_huefte, fl_bein, fl_fuss, fl_zeh | 0, 5, 6, 7 | Spiegel von FR, teilt dessen Texturkarten |
| 11, 12, 13 | nr_ober, nr_unter, nr_hand | 10, 11, 12 | C1, C3, Rundling C4. **13 = Schlagglied-Ende** |
| 16, 17, 18 | nl_ober, nl_kugel_o, nl_kugel_u | 10, 16, 17 | B1, B2, B3. **18 = Schlagglied-Ende** |
| 19 | nl_unter | 16 | Innenglied B4 |
| 14, 15, 20 | hals, vorsprung, kiefer | 10, 14, 14 | Vorsprung am Ende N |

Vier Beine mit je mindestens drei Abschnitten, Rumpf drei Knochen, Vorsprung beweglich, Eltern vor Kindern. Knochennamen stehen nur im Generator, das EMD kennt keine.
FL und FR teilen ihre Texturkarten. Z5 und Z9 stehen deshalb auch auf Bein FL, frei gesetzt. Beim Malen zaehlt nur FR.

**Rumpf:** Ovalnetz, an beide Umrisse gezogen, Fourier-Begrenzung je Ring, dazu zwei triangulierte Punkte. Verworfen: Huellen-Querschnitte (Grate, Ausreisser) und ein Fit reiner Ovale (IoU 0,81 und 0,78).
**Glieder:** Schlaeuche, Stationen aus den Formblatt-Polygonen und dem Umriss in Bild 1, Tiefe je Station von Hand nach Formblatt 1.4. Danach gemeinsame Anpassung aller Teile an den Gesamtumriss.
**Textur:** Rissnetz im Raum, Falten, Adern, Knoten (`maler.py`), alles in harten Stufen der Palette. Tonflaechen aus der Rueckrechnung von Bild 1 werden geglaettet, in Stufen gesetzt und nur als zusammenhaengende Flaechen uebernommen. Marken sind gemalte Wuelste nach den Sollpolygonen. Bild 1 wird nicht aufprojiziert: Durchlauf 1 zeigte Pixelbrei mit eingebluteter Bodenfarbe (`lauf/d1`).
`tim_genau` setzt die 5-Bit-Farben direkt in die CLUT, 246 Farben. Der Medianschnitt aus `form_emd` schluckte seltene Farben wie die Weisskerne.

## 3. Was belegt ist und was frei gesetzt ist

| Stelle | Stand | Quelle |
|---|---|---|
| Umriss Bild 1, Beine, Rumpf | an Bildkanten, Tiefe geschaetzt | Formblatt 1.4, 7 |
| Rumpflaenge in X | aus Bild 1 und Bild 2, setzt Sicht Flanke voraus | **Annahme** 6B |
| Lage Bild 1 | Formblatt 3.1, Gier 2145 | [s], Ende N vorn ist Annahme 6C |
| Lage Bild 2 | Probe (2625,0,-23625), Gier **1166** statt 1081 | **frei** im Bereich a 60-100. Rumpf-Probe: IoU je Bild 0,966/0,956 bei 1081, 0,971/0,964 bei 1166. Der Unterschied ist schwach, die Weltlage nicht gemessen |
| Vorsprung = Haken H = Traeger von Z1 | Sehstrahlen schneiden sich mit Rest 9-68 bei Gier 1081, 38-97 bei 1160 | nur Vertraeglichkeit, Formblatt 6D, 6F |
| Kiefer | traegt Z3, Z4 | **frei**. Eigenes Glied, weil Bild 2 dort nichts Helles zeigt |
| Kein Kopf, kein Gesicht, keine Augen | bewusst, das Bild verlangt keines | Formblatt Abschnitt 5 |
| Bein FL | Spiegel von FR, X +420, Masse 0,9 | **frei**, Formblatt 6A |
| Seite L, Ende F, Unterseite | Form Spiegel oder Rumpfoval, helle Marken an der Mittelebene Z=60 gespiegelt, auf 0,85 abgedunkelt | **frei** |
| Fuss-Spitzen | kleines Horn an jedem Fuss, Lage (428,363) | Stelle aus Bild, Art frei |
| Rundling C4 | liegender Spindelkoerper, X 500 bis 1400 | Masse wahrscheinlich, Laenge aus Sehstrahlen mit Flaeche Y, Rest 3-165, nur bei Probe-Lage |
| Gegenstand X, Block C5, Masse B5 | nicht gebaut | Formblatt offen |
| Pose 2 | Haken 35 Grad um Y, Kiefer 78 Grad eingezogen, Saeule NL 45 Grad gerollt, Beine NL, FL, FR umgestellt | **alles frei**, aus Suchen gegen die Bild-2-Zonen (`nl_such.py`, `fr_such.py`, `fl_such3.py`) oder von Hand |
| Haut: Risse, Falten, Adern, Knoten | Farben aus der Palette, Muster erfunden | Auftrag. Das Bild zeigt unter 4 px nichts |
| Grundhaut Farbton | rotbraun nach Palette | Farbton offen, farbe.md 7 |
| Rosa Z1 | Grund (190,126,152), Gewinn 1,31, in die Palettengrenzen geklemmt | Helligkeit offen. Filter (166,127,164) gegen Video (179,119,162) |
| Weisskerne Z3, P | Palettenweiss | nur Mindesthelligkeit belegt |

## 4. Marken im jeweils anderen Bild

Eine Marke aus Bild 2 faellt ueber das Modell nach Bild 1 (`quer.py`). Nur diese Probe kommt nicht direkt aus einer Zielflaeche.

| Marke | Bild 1 Lage | Sichtbar | Video Y dort | Lesart |
|---|---|---|---|---|
| Q1 | (334,204) | 0,99 | 77 | trifft die Flecken A35, A36. Die Flaeche wurde dorthin gezogen, also kein unabhaengiger Treffer |
| Q2 | (340,199) | 0,46 | 140 | Rand vor hellem Grund, sagt nichts |
| Q3 | (368,219) | 0,79 | 140 | Rand vor hellem Grund, sagt nichts |
| Q4 | (373,230) | 0,0 | | verdeckt, kein Widerspruch |
| Q5 | (385,259) | 0,98 | 45 | **Widerspruch ungeklaert**: Bild 1 zeigt dort dunkel, Bild 2 eine Linie. Die Quer-Sperre nimmt die Marke weg, wo Bild 1 sie saehe. Q5 steht deshalb nur noch mit Material 0,71, Groesse 0,43 da |
| G | (352,332) | 0,74 | 77 | trifft den Rand von C1 nahe Z6, passt zu A14/G, Rest 39-103 |
| P | (380,287) | 0,19 | 53 | meist verdeckt |

## 5. Eingriffe von Hand

Hier ist das Modell an den Test angepasst worden.

- Soll-Umriss des Fits: Taille C1/C3 und Knick-Band Z7 ergaenzt, (280-288,326-346) ergaenzt, drei Stellen beschnitten (Rundling links, Unterkante L1, Oberkante Kugel). Die Messmaske selbst ist unveraendert.
- Taille: sechs Ecken von `nr_ober` und `nr_unter` per Hand auf Bildlagen gesetzt (`lauf4.py`), dazu `abstossen` und eine Schleife, die Ecken bei Treffern auf L1, L2, SCH schiebt. Grund: Zwischen Sollpunkt (308,346) und Boden L2 (bis x=314 bei y=350) bleiben 3 px Platz.
- `POSE2_FL` nach der Suche von Hand auf -5,5 gesetzt, Rolle der Saeule und Kiefer von Hand.
- Bild-2-Rumpfziel `T2P` ist selbst gezeichnet. Links oben liegt der Render bis 18 px ausserhalb der gelesenen Kante (`b2.gelesen_1..4`: -9,5, -15,9, -18,1, -10,1, zaehlen nicht, Formblatt A2 nennt sie unbelegt), damit Q1 Innenlinie wird wie Q3. Das ist eine Entscheidung und macht die Kuppel dort breiter als die gelesene Linie.
- Gewinn `H` des Hakeninneren misst auf dem Rand-Polygon, nicht im Inneren. Wirkung gering, er ist auf 1,2 begrenzt und in die Palette geklemmt.

Gebogene Normalen: `normalen_licht` staucht den Hoehenanteil der Normalen. L0 in ROOM5080 steht fast senkrecht ueber der Figur, senkrechte Flaechen bekaemen sonst wenig Licht, Oberseiten liefen weiss zu (pruefung.md Abschnitt 6). Die Normalen sind damit nicht mehr geometrisch: Die Form liest sich in Punktlicht flacher. Wirkung in anderen Raeumen ist nicht geprueft.

Verworfen nach Bildurteil: Suche der NR-Gelenke gegen G. Zahlen besser (IoU 2 0,993, G auf der Figur 0,96), Bild schlechter: G zerfiel in Stuecke, die Hand lag ueber dem Band links unten, in Bild 1 standen weisse Flecken auf Bein NR, NCC fiel auf 0,795 und 0,741.

## 6. Was im Bild noch abweicht

Bild 1:
- Streifen ueber Z1 (333-346,192-222): Video Y 82, Filter 93, Filter blaustichiger. Original zwei Flecken, Render ein zusammenhaengender Strich. Sichtbarster Fehler dort.
- Zeichnung: Im Original zerfasert, im Render glatte Bohnen. Der Stapel Z2, Z3, Z4 wirkt wie aufgesetzte Kiesel. Z4 Y 141 gegen 109, Gewinn am Anschlag 1,7.
- Kuppel im Render ein glattes Ei, Saeule und Kugel NL ohne innere Gliederung. Original zerkluefteter.
- Unten: Kugel NL Y 53 gegen 63, Hand C4 Y 54 gegen 66, Render heller, Video blauer und dunkler. Hand C4 fuellt die Pruefzone nur zu 0,83, Umriss dort offen.
- Bodenschatten fehlt in der Buehne, Differenz zeigt ihn als Fleck. Kein Modellfehler.

Bild 2:
- Figur im Mittel Y 15 gegen 21, Video (51,12,24), Filter (43,9,19). Rumpf Mitte Y 13 gegen 17. Render zu dunkel, die Haut kommt aus Bild 1.
- G: nur 0,72 auf der Figur, die Zone (300-345,318-352) ist nur zu 0,52 Figur. Formblatt fuehrt G als sicher. Bleibt offen, Suche verworfen (Abschnitt 5).
- Zug Q im Filter durchgehender und kraeftiger als im Original, dort nur Grat. Q1 zu schwach, Y 58 gegen 46, Gewinn am Anschlag. Q4 Y 96 gegen 75, Gewinn am Anschlag.
- Scheitelsichel Q2 ist Flaeche aus Hautzellen, Original weicher.
- Bein FR im Feuer: Filter Y 38 gegen Video 57, nicht pruefbar. Unter dem Feuerschein ist die Figur zu dunkel, oder das Bein steht dort nicht.
- Fehlendes Feuer in der Buehne treibt die Differenz links und rechts. Kein Modellfehler.

Rundum und Bewegung:
- Rundling liegt als lange Spindel nach vorn und wirkt aus der Seite fremd. Er ist nur von vorn belegt.
- Unterseite traegt helle Flecken aus G und P, Scheitel von oben rosa-braun gefleckt, Bild-2-Zeichnung aus ungesehener Richtung. Rueckseite glatt, dort fehlt jede Bildaussage.
- Biegeprobe (`lauf/biege`, Gelenke bis 34 Grad): keine Loecher gesehen. An der Rumpfteilung entsteht bei 12 Grad eine kleine Stufe, Seite -Z oben. Eine Sichtprobe, keine Lochmessung.
- Pose 2 Boden (y): Bein FL 0, Hand NR -12, Innenglied -60, Kugel NL -115, Unterglied NR -104, **Fuss FR -189**. Fuss FR schwebt. Bild 2 zeigt ihn nicht, fuer Bewegung braucht das eine Antwort.

Spiel:
- Beide Laeufe laden: `Bahnsteig-Form: 21 Meshes, 21 Knochen, 3 Clips`, Fensterbild per PrintWindow, Sitzung getrennt.
- Pose 2, ROOM5090 Schnitt 11: Rumpf, Haken, Zug Q und G da, Spielfeuer deckt die Beine. Rumpf Median (8,2,5). Das Feuer steht anders als im Video, Vergleich `spiel_vergleich_bild2.png`.
- Pose 1, ROOM5080: Bild 1 hat keinen Schnitt. Schnitt 2 blickt weg, Abnahme in Schnitt 1, die Figur steht klein und zeigt nicht die Seite aus Bild 1. Sie wirkt rotvioletter und fleckiger als das Video. Median (16,8,10) passt zu farbe.md (18,11,13), p90 (56,56,53) kommt von den Mittelton-Flecken.

## 7. Durchlaeufe

Mehr als sechs. Im Baum liegen `lauf/d1`, `d9`, `d20`, `d32`, `ende`, `biege`.

| Lauf | Aenderung | Bild 1 Punkte, Flaechen, Zeichnung | Bild 2 Punkte, Flaechen, Zeichnung | NCC 1 / 2 |
|---|---|---|---|---|
| d1 | direkte Projektion | 18/33, 1/3, 1/9 | | |
| d2 | gemalte Textur, Normalen gestaucht | 30/33, 1/3, 9/9 | 7/7, 4/9, 6/7 | 0,805 / 0,527 |
| d9 | exakte CLUT, Marken-Wulst, Gliedposen | 31/33, 1/3, 9/9 | 7/7, 8/9, 7/7 | 0,828 / 0,760 |
| d20 | verdeckte Seite, Kiefer | 33/33, 2/3, 8/9 | 7/7, 9/9, 7/7 | 0,792 / 0,812 |
| d32 | Ovalrumpf, Rissnetz, NL-Suche | 33/33, 3/3, 9/9 | 7/7, 9/9, 7/7 | 0,855 / 0,782 |
| ende | Zug Q, Punktziele, FR-Suche, tieferes Oberglied NR | 33/33, 3/3, 9/9 | 7/7, 9/9, 7/7 | 0,852 / 0,803 |

NCC lief nicht stetig und war kein Abnahmemass, siehe Abschnitt 1.

## 8. Offen fuer den Vergleich mit A und B

- Sichtfrage 6B bleibt offen. Modell und Rumpflaenge X haengen an Sicht Flanke.
- Pose 2 ist ein starker Umbau: Beine FR, NL, Kiefer und Haken stehen weit von Pose 1 weg. Zeigt Bild 2 dieselbe Pose, ist dieser Weg falsch.
- Ohne Hand-Ecken an der Taille fallen L2 und `b1.nr_y346_links`.
- Nicht geprueft: bewegte Clips, Rumpf- und Hiebpunkte im Tick (Teile 10, 13, 18 sind vorhanden, im Lauf nicht ausgeloest), Kollision, Schatten, Masken.

## 9. Dateien und Lauf

Generator: `lauf4.py` (Hauptlauf), `teile.py`, `bau.py`, `maler.py`, `atlas.py`, `netz.py`, `modell.py`, `kern.py`, `schau.py`. Posen: `pose2_nl.json` aus `nl_such.py`, `pose2_fr.json` aus `fr_such.py`, `fl_such3.py` nur Herkunft von `POSE2_FL`.
Messung: `offen.py`, `quer.py`, `farbmess.py`, `kontakt.py`, `biege.py`, `spiel_vergleich.py`.

```bash
cd analysis/befunde_runde41/form/kandidat_c
python lauf4.py ende --neu      # baut alles: FORM.*, lage.json, vergleich_*, rundum, lauf/ende
```

Der Lauf liest die beiden Pose-Dateien, ohne sie entsteht eine andere Pose 2. Geprueft: `--neu` zweimal hintereinander, FORM.EMD und FORM.TIM bytegleich (md5 8f6743ef..., 054ea089...). Die Spielbilder zeigen genau diese Dateien.

Abgabe: `FORM.EMD`, `FORM.TIM`, `lage.json`, `vergleich_bild1.png`, `vergleich_bild2.png`, `rundum.png`, `spiel_pose1_r5080_s1.png`, `spiel_pose2_r5090_s11.png`, `spiel_vergleich_bild2.png`, `textur_ende.png`, `DOSSIER.md`.
Kein Commit. Geschrieben nur in diesem Ordner und in `re15_port/build/form_spiel/` (eigene Laufordner danach geloescht). Kurzzeitig lagen vier Merkdateien in `/tmp`, sie sind entfernt.
