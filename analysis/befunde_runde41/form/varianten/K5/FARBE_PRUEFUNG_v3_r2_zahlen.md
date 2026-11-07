# Zahlen-Pruefung Farbe Versuch 3, Runde 2 (K5c V3, G4-Uebertragung), 2026-11-07

Gegenstand: `varianten/K5/farbe/K5c_farbe_v3/` (FORM.EMD sha256 32b48eee..., FORM.TIM sha256 2bddb092...). Beide sind bytegleich mit
`farbe/K5C_FARBE_V3.EMD/.TIM` und mit `formfarbe3/emd/` (nachgerechnet). Geprueft sind die Commits 17db6f4c9, a406dc133, 8a6dd0fe3 und 684d3c7f0 auf r41/form.

Eigener Code (eigener EMD/MD1/TIM-Leser, unabhaengig von FE, kf_g4 und riegel_farbe_v3; Abtastung: 861 baryzentrische Punkte je Dreieck, floor):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3/zpruef2/`
- `basis.py`, `g0.py`
- `h1s1d1.py` -> `h1s1d1.json`
- `wiederh.py`, `feld.py`, `feld_w.py`, `p1h2.py`
- `a8_wortlaut.py`, `zone2.py`, `saaten.py`
- Gegenprobe `v_bandflach/` mit `riegel_bandflach.txt`
- Nachlauf des Riegels `riegel_nach.txt`/`.json`
- `ctest.log`

Ketten und Render (K-Zeilen, B-Zeilen, P2, N1/N2) habe ich nicht neu implementiert. Ich habe sie nachlaufen lassen und mit Abwandlungen gegengeprueft
(andere Saat, flaches Band, Feld nach Wortlaut).

## Ergebnis

**Nicht bestanden.**
- Der Riegel des Bauers ist ehrlich gerechnet. Mein Nachlauf ist zeilengleich (36/52, Exit 1, Gegenproben 13/13 wie verlangt).
- 16 Zeilen sind ROT: A1, P2, B10, B11, B12, B17, N1, N2, K3, K7, K1v2, K2v2, B2, B9, B9b, K15.
- Mit eigenem Code bestaetigt habe ich: G0, Seiten 0-2 und CLUT 0-2, H1, H1b, S1, H2, P1, F1t, F2t, Zaehne. Die Tests sind gruen, Spiel-Asset und Pins unveraendert.
- Zwei GRUEN-Zeilen halten nur, weil der Riegel in dieser Runde umdefiniert wurde:
  - **A8:** Nach dem Wortlaut des Messblatts ist die Zeile ROT. Die Umdefinition steht nicht im Dossier.
  - **D1:** Nach der Messblatt-Formel ist die Zeile ROT. Die Umdefinition ist offen ausgewiesen.
- Eine GRUEN-Zeile ist nicht stabil: **K12 Schwanz** ist mit Saat 7 ROT.
- **K15** ist ROT, und das liegt nicht am Bau. Der Fehler sitzt zwischen Messblatt 3.2 und der Kette (Gegenprobe unten). Das muss der Messblatt-Pruefer entscheiden.

## (1) Riegel-Zeilen nachgemessen

| Zeile | Bauer | eigene Messung | Bewertung |
|---|---|---|---|
| G0 | gleich | EMD-Abschnitte 0-6 bytegleich (EDD/EMR; Knochen und Clips damit gleich). MD1: 32 Meshes, 1270 Dreiecke und 2558 Vierecke. tv, tn, tf, qv, qn und qf sind in jedem Mesh gleich. 35 856 Bytes sind verschieden, **0 davon ausserhalb der UV-Bloecke**. Laenge und Verzeichnis sind gleich | **GRUEN bestaetigt** |
| TIM | Seiten 0-2 = G4 | Seiten 0, 1 und 2 haben gegen die G4-TIM @0x65e800 je 0 Pixel Unterschied. CLUT 9x256, Zeilen 0-2 bytegleich. Seite 3 tragen nur 31 Flaechen: 6 F1-Flaechen und 25 Bandflaechen | bestaetigt |
| H1 | 100,0 in allen 7 Teilen | Index gleich G4 und Texel in der G4-Inselmaske derselben Basiszeile. Rumpf, Schwanz, Hals und Arme 100,00; Beine und Klauen 99,99; Kopf 99,98 | **GRUEN bestaetigt** |
| H1b | 1,07 % | 1,01 % | GRUEN |
| S1 | 0,00 | 0,00-0,02 % (auch mit der Inselmaske aller Zeilen) | **GRUEN bestaetigt**. Befund A aus Runde 1 ist behoben: Hals 0 % statt 4,69 % |
| S1b (Info) | Schwanz 1,35 / Beine 1,57 | UV-Flaeche < 0,5 Texel^2: Schwanz 1,35, Beine 1,57, Kopf 0,07, sonst 0 % | bestaetigt |
| H2 | 0 Vertauschungen | Zeilen 3-8 haben bei dL >= 2 je 0 Vertauschungen; Zeilen 0-2 sind bytegleich | **GRUEN bestaetigt**. Hinweis: In Zeile 7 (Band) stehen 42 Eintragspaare, die im G4 mindestens 2 L\* auseinanderliegen, jetzt weniger als 0,5 L\* auseinander (Gamma 0,60, `bau.json`). Die Rangfolge haelt, die Stufen sind aber zusammengeschoben |
| P1 | 22,5 / 26,5 / 324 | 22,5 / 26,5 / 324; ohne die Sonderzeilen 21,0 / 27,3 / 243; G4 17,7 / 25,6 / 423 | **GRUEN bestaetigt** |
| D1 | GRUEN: abgebildete G4-Flaeche 1,05-1,25 und Median ortsgenau 1,12-1,41 | Ortsgenau je Dreieck sqrt((A3/AUV) / G4-Dichte am Texel), flaechengewichtet. Median: Rumpf 1,07, Schwanz 1,13, Hals 0,96, Beine 0,98, Arme 1,13, Klauen 1,01, Kopf 1,22. **p90 am Kopf 3,45; 24,8 % der Kopfflaeche > 2,0.** Messblatt-Formel (Riegel-Infozeile): Rumpf 2,64, Schwanz 2,33, **Hals 2,81, Kopf 2,78** | **nach Wortlaut ROT**, nach der Absicht (Texeldichte) GRUEN. Siehe Befund D |
| F1t | dE 0,34 (178 Fleck-Texel) | Fleck-Texel (Art 2): Lab (48,66 / 21,83 / -3,22), **dE 0,34**. Alle 620 gelesenen Texel: dE 34,1. Die 442 Haut-Texel sind eine echte G4-Kopie: Index gleich G4-F11 an der Quelle 442/442, Quelle in der G4-Maske 442/442, Farbe in Zeile 6 gleich der G4-Farbe (dE 0,00) | GRUEN. Der Feld-Begriff der Runde 1 ist sauber umgesetzt: Die Haut ist G4-Herkunft und zaehlt in H1 |
| F2t | dE 0,22 (554) | Eindeutige Texel: 561, dE 0,16. **3D-flaechengewichtet: Lab (50,49 / 41,92 / 22,67), dE 4,84.** Die grossen Bandflaechen tragen die bunteren Eintraege | GRUEN, mit Hinweis |
| Zaehne | 77,7 / 5,3 | 77,68 / 5,28; 3D-gewichtet 76,4 / 5,0 | GRUEN |
| A8 | 5,3 / 39,3 / 64,3 GRUEN | **Feld nach Messblatt 5 ("Unterkiefer aussen + Band"): Zahn 0,9 % < 2 %, ROT** (`a8_wortlaut.py`). Der Bauer hat in Runde 2 die Zaehne von Mesh 23 ins Kieferfeld genommen (`riegel_farbe_v3.py` `atlas_v3`, Kommentar "Runde 2"). Dadurch wird der Wert 5,3. Runde 1 hatte ohne Zaehne 2,5 % (GRUEN) | **ROT nach Wortlaut.** Die Umdefinition steht nicht im Dossier V3R2.1 und nicht in V3R2.2. Siehe Befund B |
| P2 | ROT: Brust +10,2, Klauen +9,1/+8,7 | Nachlauf zeilengleich. Die Quellen in `P2_TEILE` stimmen jetzt mit `kf_g4.ZUORDNUNG` ueberein: Kopf, Brust und Hals gegen 16; Rumpf gegen 0/9 | ROT bestaetigt |
| A1-A12 ohne A8, B10-B17, N1/N2, K1v2-K15, B2, B9, B9b | wie gemeldet | Nachlauf zeilengleich. K14 dE und K15 dE nachgerechnet: 3,93 bzw. 12,24 | wie gemeldet |
| K12 | GRUEN, Schwanz 8,7 (Grenze 8,9) | **Saat 15: 8,7; Saat 12345: 8,9; Saat 7: 9,0, also ROT** (`saaten.py`). K11, K13 und K14 sind in allen drei Saaten GRUEN; K15 ist in allen drei ROT | GRUEN nur mit der Messblatt-Saat, ohne Rundlauf-Reserve. Siehe Befund C |

### Befund A: K15 ROT ist kein Baufehler, sondern ein Widerspruch zwischen Messblatt 3.2 und der Kette

Gegenprobe `v_bandflach`: Ich habe alle 256 Eintraege von Zeile 7 einfarbig auf ZIEL_BAND gesetzt. Das 15-Bit-Wort 0x2977 ergibt (189,90,82); F2t dE 3,11.
- Ergebnis: **K15 Lab (45,2 / 37,7 / 23,0), dE 12,7, ROT. K7 dE 15,8, ROT.**
- Bei der V3 sind es K15 dE 12,2 und K7 dE 15,3.
- Auch eine Bandfarbe exakt auf dem Messblatt-Ziel verfehlt den K15-Rundlauf also um 12,7.
- Die Vorwaerts-Gegenprobe in Messblatt 3.2 meldet fuer diesen Texel aber einen Rest von dE 0,86.

Damit ist die Aussage des Bauers in V3R2.3 belegt: K15 ist innerhalb der F2t-Toleranz nicht gruen zu bekommen. Der Messblatt-Pruefer muss klaeren, welche Zeile gilt.
Ein Verdacht fuer die Ursache: Die Kette liefert fuer diesen Texel a\* 37,7 gegen den Rundlauf 28,2. Das Licht an den Bandpixeln der Kette ist also ein anderes als in der Vorwaerts-Gegenprobe.

### Befund B: A8 nach Wortlaut ROT

- Messblatt 5, Zeile A8, Feld = "Unterkiefer aussen + Band"; Messblatt 1.3: "Unterkiefer aussen samt Band".
- Mit diesem Feld ist Zahn 0,9 % (Soll 2-20 %).
- Der Bauer hat die Zaehne von Mesh 23 dazugenommen und beruft sich auf den Satz "Bild 2 zeigt dort 66 % Rot und 12 % Zahn". Das ist vertretbar, aber es ist eine Umdefinition einer Riegel-Zeile in eigener Sache.
- Sie muss im Dossier stehen und vom Messblatt-Pruefer bestaetigt werden.

### Befund C: K12 Schwanz haengt an der Saat

- Der Schwanz steht mit Saat 15 / 12345 / 7 bei 8,7 / 8,9 / 9,0 %; die Grenze ist 8,9.
- Das Messblatt rechnet mit Saat 15, der Wert ist also formal GRUEN. Gegen die Rauschstreuung des Videofilters gibt es aber keine Reserve.

### Befund D: Wiederholung (Kachelung) und D1

- **Ortsgenau** ist die Texeldichte wie bei der Quelle (Median 0,96-1,22).
- **Messblatt-Formel:** 2,33-2,81 an Rumpf, Schwanz, Hals und Kopf.
- **Ursache** ist die Kachelung, also dieselben G4-Texel mehrfach.
  - Gemessen als Summe der UV-Flaechen / eindeutige Texel: Brust (Mesh 10) 13,7, Hals 14,6, Kopf 22 8,0, Becken 4,8, Wirbel 5,9, Schwanzglied 1 2,8.
  - Zum Vergleich G4 selbst: im Ganzen 2,31, Quellmesh 16 allein 0,93, Becken+Lende 1,3.
  - Die Brust/Hals-Haut ist damit eine rund 14-fache Wiederholung des Hautfelds F18 (G4-Oberarm r, ohne Insel 72).
- **Das Messblatt kennt keine Kachelung.** D1 ist als Formel ohne Wiederholung geschrieben.
  - Der Bauer hat die Gate-Formel ersetzt, offen ausgewiesen.
  - Ob eine 8- bis 15-fache Wiederholung zulaessig ist, entscheidet der Messblatt-Pruefer; ob sie sichtbar ist, der Sicht-Pruefer (Dossier V3R2.5 Punkt 4).
- **E9 am Schwanz** (Messblatt 1.3, "das Muster wiederholt sich nicht je Glied"), gemessen im Rahmen von `kf_g4.Rahmen` (s_T 2,513):
  - Die erste Spiegelkante liegt bei T 0,398. Glied 1 endet bei T 0,401, die Kante liegt also am Gliedende.
  - Das widerspricht dem Docstring von `projektion` ("die Kante liegt nicht an den Gliedgrenzen").
  - Die zweite Spiegelkante (T 0,796) liegt in Glied 5 und am Beginn von Glied 6 (T 0,772). Glied 5 und Glied 6 teilen sich 75 % ihrer Texel (Jaccard 0,75; die uebrigen Paare 0,01-0,54).
  - A11 (L\*-Sprung) ist mit 3,1 GRUEN. Ob man die Spiegelung sieht, beurteilt die Sicht.

### Befund E: Brust/Hals aus F18 (Abweichung von Messblatt 2.3), Zahlen nachgemessen

Bild-2-Kette, Zone Kopf/Rumpf, mit beiden Klassendefinitionen gegen das Original nachgerechnet (`zone2.py`):

| Zone Kopf/Rumpf | Rot (K11-Klasse) | Rot (Klasse Messblatt 4) | dunkel |
|---|---|---|---|
| Original | 14,9 % | 11,0 % | 73,1 % |
| V3 Runde 2 | 8,9 % | 8,2 % | 90,4 % |
| V3 Runde 1 | 45,0 % | 41,3 % | 52,2 % |

- Der Bauer vergleicht im Dossier seine 8,9 % (K11-Klasse) mit den 11,0 % des Originals (Klasse Messblatt 4). Das sind zwei verschiedene Klassen; gleich gegen gleich sind es 8,9 gegen 14,9 bzw. 8,2 gegen 11,0.
- Rot liegt jetzt deutlich naeher am Original als in Runde 1.
- Dunkel schiesst um 17 Punkte ueber, in Runde 1 lag es 21 Punkte darunter.
- Unterkiefer: V3 94,4 % Rot (K11-Klasse) gegen 77,1 % im Original.
- **Bewertung:** Die Abweichung ist durch die Kette gedeckt (K6 dE 4,8, K8 3,3 und K13 sind GRUEN). Die Zahl im Dossier ist aber klassenunrein, und die Zone ist jetzt zu dunkel. Bestaetigen muss das der Messblatt-Pruefer.

## (2) Geometrie, Spiel-Asset, Pins

- G0 ist mit eigenem Code bestaetigt (siehe oben).
- `git diff 02aed15f7..HEAD -- re15_port` ist leer. Ausserhalb von `analysis/` ist nichts geaendert.
- `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` hat sha256 16eb6ea8... und wurde zuletzt in 67408cfcb (2026-11-03, K4) geaendert.
- **Bestaetigt.**

## (3) Tests

- `ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3` auf dem vorhandenen Bau: **11/11 bestanden**, 95,8 s (`zpruef2/ctest.log`).
- Die Tests pruefen K4, nicht V3.
- Im Spiellauf-Log stehen 10x rc=0, und V3 laedt als "32 Meshes, 32 Knochen, 3 Clips -> Slot 12". Bestaetigt.

## (4) Gegenproben

- Der Nachlauf mit `--gegenprobe-v2 farbe/K5c_farbe_v2` ist zeilengleich. Alle 13 fallen wie verlangt aus: V2r2 H1/P1/K11/K14/K15 ROT, G4 P1 und B10-B17 8/8 GRUEN, (b2)-Probe A K11/K12/K13/P2 ROT, Orange und Gelb ROT.
  Der Befund (4) der Runde 1 ist damit erledigt.
- Eigene Gegenproben:
  - G4 P1 17,7 / 25,6 / 423, GRUEN.
  - Flaches Band, siehe Befund A.
  - Saaten, siehe Befund C.
- Schwach: Die Orange/Gelb-Probe faerbt die ganzen Zeilen 6/7 ein. Damit trifft sie auch die Haut-Eintraege und ist trivial ROT; eine Probe nur mit den Fleck-Eintraegen waere schaerfer.

## (5) Konstanten (Diff 4b5800a76..HEAD, generator/)

Mit Quelle:
- _BARY_N 40 (Verfahren des Pruefers).
- ROT_C0/C1/H0/H1 (Messblatt V3 K11 bzw. V1 A3).
- B1_C_JE_SEITE: Seite 1 = 23,6 / 41,14 = 0,574, nachgerechnet aus Messblatt V3 4 "1 Schwanz" und G4-Rot; Seiten 0 und 2 = 1,0 als G4-Buntheit.
- KAPPE_NA 0,8 (Leser A).
- BAND_MEHRHEIT 0,5 (Definition "Mehrheit").
- n_th/s_T gemessen, Werte in `bau.json`.
- VERSCHIEBE_N, SCHRUMPF, ZIEL_BAND_BAU, FLECK_WEITE und BAND_SCHWELLE sind entfallen.

Schwach oder ohne Quelle:
- **WEICH_RAND 1**, "nur die hellere Haelfte des Fleischs": Als PORT-WAHL gibt der Bauer den E10-Wortlaut an ("offene Wunden ... fleischig"). Der Wortlaut nennt aber weder einen Texelring noch die Haelfte. Die Bindung ist schwach.
- `cv2.MORPH_CLOSE` mit 3x3 auf dem F1-Kern hat keine Quelle (neu in Runde 2). Der Kommentar zu WEICH_RAND steht doppelt (Zeilen 1018-1020).
- `bau.json` Klauen l/r: `n_theta` 4/5, `s_T` 4,0 und `umfang_k5c_quelle` sind Summen ueber die 4 Ziel-Meshes (Art 'einzeln'). Einzeln gilt s_T = 1 (laengs_einmal). Die Angabe fuehrt in die Irre.
- Riegel-Aenderungen in eigener Sache:
  - D1-Gate (offen ausgewiesen),
  - A8-Feld (nicht ausgewiesen, Befund B),
  - H1/S1 ueber die Herkunftskarte des Baus (sauber; selbst geprueft: Quelle und Index stimmen).

## (6) Tafeln

- 01 und 03 (verkleinert) gesichtet, 06 dazu.
- K4 und V3 bekommen dasselbe Disc-Licht und denselben Filter, Saat 15.
- Die Familienkachel ist roh, mit Licht 0,55 + 0,45 N.L fuer alle fuenf Modelle und Umkugel 0,46. Die halbe Pixelzahl der Vierbeiner ist vermerkt.
- In 06 steht neu die Quellspalte G4 16 (F18).
- Nichts ist einseitig aufgehellt. **Fair.**
- Zur Sicht: Die F1-Stelle ist in 01 Z.1/Z.2 ein scharf begrenzter, gestufter rosa Fleck. Die Kanten sind deutlich haerter als im Original.

## Baum

- Sauber, ausser diesem Bericht. Er ist nicht committet.
- Kein Upstream, kein Merge-Commit in 02aed15f7..HEAD, kein Push.
- Nebenwirkung meiner Pruefung: Ein Bildaufruf mit MSYS-Pfad schrieb drei verkleinerte Tafeln nach `C:/c/Users/...`. Ich habe sie geloescht.
  Dabei hat `find -delete` leere Verzeichnisse unter `C:/c/` entfernt; dort liegt ein fremder HF-Cache (whisper). Dateien wurden nicht geloescht.
