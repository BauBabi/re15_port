# Echte Farben der Bahnsteig-Form

Stand 2026-10-31. Sicherheit: sicher / wahrscheinlich / offen. Koordinaten: Originalbild 640x480.
Farbwerte `e`: Spielfarbe auf dem Bildschirm, gammakodiert 0..255. `V`: Rohwert im Video. `T`: Texturfarbe.

## 1. Ergebnis

Die Figur ist nicht violett. Sie ist fast schwarz-rotbraun mit hellgrauen Baendern.

| Material | Video Bild 1 | Video Bild 2 | Spiel Bild 1 | Spiel Bild 2 | **Textur (Fehler)** | Art | Sicherheit |
|---|---|---|---|---|---|---|---|
| Dunkle Grundhaut | (31,23,42) | (46,8,17) | (18,11,13) | (20,4,5) | **(44,18,27)** (+-22,19,22) | Oberflaeche | dunkel: sicher; Farbton: wahrscheinlich |
| Rotbraune Mitteltoene | (84,47,87) | (76,19,45) | (44,26,30) | (40,22,26) | **(103,63,84)** (+-21,17,17) | Oberflaeche | wahrscheinlich |
| Rosa Stelle A01 | (203,144,183) | nicht erkennbar | (88,59,56) | nicht erkennbar | **(181,137,150)** (+-25,20,22) | Oberflaeche wahrscheinlich, Lichtanteil offen | Farbton: wahrscheinlich; Helligkeit: offen |
| Helle Baender | (152,153,189) | (97,57,84) | (69,65,62) | (48,50,48) | **(126,145,156)** (+-23,28,32) | Oberflaeche, in Bild 1 mehr Licht | wahrscheinlich |
| Feine helle Linien | (108,116,162) | (78,42,70) | (57,55,56) | (44,42,44) | **(114,123,143)** (+-30,28,35) | Oberflaeche | wahrscheinlich |
| Untere Streifen | nicht erkennbar | (201,116,150) | nicht erkennbar | (87,72,70) | **hoechstens (225,209,229)** | Oberflaeche plus Feuerschein | Obergrenze |
| Weisse Kerne A03, B13 | (201,204,219) | (206,173,186) | mindestens (74,71,69) | mindestens (91,96,87) | keine Aussage | geclippt | offen |

Farbmuster: `lesehilfen/farbe_palette.png`. Maschinenlesbar mit allen Merkmalen: `farbe_palette.json`.

Drei Saetze fuer den Modellbau:

- **Violett, Lavendel und Blau gehoeren dem Video.** Neutrales Grau e=60 wird als Flaeche V=(116,134,186) in Bild 1 und (117,84,119) in Bild 2 (sicher).
- **Weiss im Video heisst nicht weiss im Spiel.** Die rote Lampe (161,12,8) steht im Video als (208,198,199) (sicher).
- **Texturwerte gelten fuers Licht beider Raeume:** Schirmfarbe = Textur mal Licht, Bild 2 (0.385,0.342,0.307), Bild 1 (0.487,0.432,0.374).

Alles bis etwa (70,40,45) waere im Video fast ebenso schwarz wie die Grundhaut. Die Oberkante ist belegt, die Unterkante nicht.

## 2. Quellen

- Standbilder: `Version_80%/Biohazard 1.5 Final Build.mp4` ist eine Diashow aus 174 Dias zu je 24 Videobildern. Bild 1 = Dia 143 (mittlere Differenz 0.001), Bild 2 = Dia 145 (0.22).
- **Die uebrigen Dias dienten nur der Farbmessung, nie der Form.** Sie stammen von derselben Abfilmung und zeigen Raeume, deren Hintergrund bekannt ist.
- Hintergruende: 1119 Schnitte, `tests/unit/probe_bg_dump.c` mit den Engine-Dekodern. SIFT, RANSAC, dann ECC: 52 Dias registriert (ECC 0.53-0.93), 38 davon mit ECC >= 0.76 gingen in die Anpassung. Bild 2 auf ROOM5090 Cut 11: ECC 0.675, Kanten deckungsgleich (sicher).
- Bild 1 hat keinen Schnitt: Suche ueber alle 1119 Hintergruende mit der Geometrie der Nachbardias, bester NCC 0.495 ohne Ausreisser, ROOM508 nicht unter den ersten 20. Das bestaetigt Spur R. Ein starker Beweis ist es nicht, die Suche war grob (wahrscheinlich).
- Modelle: Frau Typ 0x4D (`CDEMD0.EMS`), Spawn @0x122A im RDT (600,0,-25300), yaw 0. Leon `PLD/PL00.MD1/TIM`.

Die Frau landet in Cut 11 (RID @0x1C0: fov 26684, (7626,-3256,-25360) -> (-1504,-986,-21970)) auf Video x 138-195, y 169-355. Im Video steht sie bei x ~140-205, y ~165-355. Das belegt Geometrie und Figurengroesse von Bild 2 unabhaengig (sicher).

## 3. Uebertragung Spielbild -> Video

`re15_port/tools/form_video.py`, `video_filter(rgb, bild=1|2)`. Keine Zahl davon gehoert in die Engine: Das ist die Abfilmung, nicht das Spiel.

| Stufe | Modell | Messung | Sicherheit |
|---|---|---|---|
| Geometrie | Homographie Spiel -> Video | Bild 2 gemessen, schraeg gefilmter Schirm. Bild 1: Median aus 35 Dias, Ecken +-10..20 px | Bild 2 sicher, Bild 1 Naeherung |
| Licht | L = g^2.2 | gamma-Fits 1.8 gegen 2.2: 2.2 besser, frei 2.03-2.36 | wahrscheinlich |
| Optik | Gauss sigma 2 px im Licht | Unschaerfe im Licht schlaegt Unschaerfe im Signal, kappa(8 px) geht auf 0. Kante 10-90 Median 4.5-5.1 px | wahrscheinlich |
| Streulicht | 0.133/0.098/0.105 * G(32 px) + 0.138/0.096/0.082 * G(128 px) | Tuer in Bild 2 ist im Spiel exakt schwarz: vorhergesagt L (0.0092,0.00084,0.00016), gemessen (0.0086,0.00098,0.00011) | sicher |
| Kennlinie | Rampe je Kanal: Schwarz e (22.4,17.7,9.0), Weiss e (89.4,78.4,62.8), Video-Weiss V=227, weiche Enden k=8.9 | 15 dunkle Dias teilen eine Kurve, Belichtung +-5..10 %. Hold-out: LO und HI bewegen sich um +-0.5 | sicher |
| Kanalbalance | e-Faktor je Bild. Bild 2 (0.951,0.665,0.620), Bild 1 (0.935,0.887,0.908) | Bild 2 aus den Waenden, Bootstrap +-(0.02,0.03,0.04). Bild 1 siehe unten | Bild 2 sicher, Bild 1 wahrscheinlich |
| Ueberlauf | Kanal ueber Weiss laeuft in die anderen, 2.5/3.0/3.5 V je e-Stufe | R: V_G +23 bei e=95, +57 bei e=115. B: +84 bei e=85. Rechnung gegen gemessen: Lampe (227,170,175) gegen (208,198,199), Kiste (227,227,227) gegen (221,221,221), Deckenlampe (227,186,161) gegen (200,195,192). Richtig in der Art, Blau rund 25 zu klein | wahrscheinlich |
| Chroma | sigma 6 x 3 px | kleiner Effekt, Kostenminimum bei (6,3) | wahrscheinlich |
| Zeilen, Korn | Bild 2 Periode 5.2 px, Amp 3.4; Bild 1 Periode 2 px, Amp 0.7; fester Saat-Wert | helle Flaechen, Spektrum | wahrscheinlich |

Weggefallen, weil gemessen wertlos:

- Freie 3x3-Farbmatrix: 2.3 % Kostengewinn, aber am unabhaengigen Pruefstein Frau schlechter (MedAE 46.2 gegen 39.9).
- Farbregler des Fernsehers (Saettigung, Tint): s=1.14, 0.4 % Gewinn.
- Nachschaerfen: senkt SSIM bei sigma 2 auf allen sieben Prueflingen.

**Warum dunkle Szenen eine feste Kurve haben (wahrscheinlich):** Bis mittlerem Linearlicht L ~ 0.03 bleiben die Rampen gleich, darueber wandern sie nach oben. Das ist eine Belichtungsautomatik am Verstaerkungsanschlag. Beide Bilder liegen im dunklen Bereich.

**Kanalbalance Bild 1:** Ohne Hintergrund gibt es keine direkte Messung. Mittel aus den fuenf aehnlichsten Szenen (0.89,0.83,0.83) und Leons Gesicht unter Ambient (0.98,0.94,0.985), Fehler +-0.08. Das Verhaeltnis der Kanaele ist enger bestimmt als die Gesamthelligkeit: Nachbarn 1.06:1:1.0, Leon 1.04:1:1.05. Leave-one-out ueber die dunklen Dias: 12 % Fehler je Kanal.

### Pruefung

`python re15_port/tools/form_video.py pruefung`, Bild `farbe_filter_pruefung.png` (selbst angesehen: Decke, Rahmen, Kisten, Boden stimmen; im Differenzbild bleiben Frau, Figur, Feuer und Kanten).

| Pruefling | MAE roh | MAE Filter | MedAE | SSIM roh -> Filter |
|---|---|---|---|---|
| Bild 2, ROOM5090 Cut 11, ohne Figur/Frau/Feuer/Logo, 167750 px | 43.3 | 18.5 | 20 -> 10 | 0.495 -> 0.692 |
| Hold-out Dia 144, ROOM5090 Cut 1 | 61.5 | 18.4 | 47 -> 10 | 0.406 -> 0.725 |
| Hold-out Dia 153, ROOM6000 Cut 4 | 50.9 | 20.9 | 29 -> 12 | 0.494 -> 0.727 |
| Hold-out Dia 68, ROOM3010 Cut 8 | 48.9 | 18.2 | 28 -> 9 | 0.363 -> 0.740 |
| Hold-out Dia 60, ROOM3050 Cut 0 | 38.1 | 15.0 | 17 -> 6 | 0.399 -> 0.759 |

Mittelfarbe Bild 2: Video (118.6,45.3,42.9), Filter (120.8,42.3,37.7), Port roh (56.2,24.3,10.1).
Hold-out heisst: Kurve ohne dieses Dia angepasst, Balance von Bild 1, nichts am Dia nachgestellt.
Hin und zurueck (Umkehr, dann Filter): MAE 6.6 Bild 1, 10.4 Bild 2.

Die Filter-MAE von 15-21 ist die Grenze dieses Modells. Bild 2 ist zudem kein Blindtest: Dia 145 lieferte die Kanalbalance und floss mit 1 von 16 Dias in die Kurvenform ein, die Hold-outs oben sind die ehrlichere Zahl.

Bedienung: `video_filter(rgb, bild=2)` legt ein Spielbild in die Bildlage des Videos. Fuer Renderings, die schon in Video-Koordinaten stehen: `entzerren=False`. Fuer Messungen: `rauschen=False`. Gleicher Eingang ergibt gleiches Bild (geprueft). Bild 1 hat keine gemessene Geometrie.

## 4. Genauigkeit der Rueckrechnung

Gemessen an rund 620000 Stichproben je Kanal aus 17 dunklen Dias mit bekanntem Spielwert, Abweichung e je Pixel:

| Band e | Balance bekannt (wie Bild 2), Bias, p16..p84 | Balance geschaetzt (wie Bild 1), Bias, p16..p84 |
|---|---|---|
| 15-25 | -3 / -3 / -1, Breite etwa +-9 | 0 / 0 / +1, Breite etwa +-11 |
| 25-35 | -1 / 0 / +1, +-9 | +3 / +4 / +5, +-11 |
| 35-50 | -1 / 0 / +1, +-10 | +4 / +5 / +5, +-11 |
| 50-70 | -2 / -3 / -6, +-11 | +5 / +4 / -1, +-13 |
| 70-100 | -8 / -11 / -25 | +2 / -3 / -20 |

Ueber e ~ 70 wird Blau zur Untergrenze. Die Fehler der Palette kombinieren diese Werte, Balance, Streulicht +-15 % und die Streuung der Merkmale per Monte-Carlo (500 Zuege).

**Gegenprobe an bekannten Stoffen (sicher fuer Mittelwerte, schwach am Schwarzende):**

| Stoff | Video | zurueckgerechnet | Soll | Urteil |
|---|---|---|---|---|
| Boden Bild 1 | (121,165,182) | (53,65,55) | Port (62,60,53) | Der blaue Boden ist grau |
| Wand Bild 1 | (113,58,45) | (53,32,15) | Port (35,29,15) | Rot 1.5fach, andere Kamera |
| Zug Bild 1 | (198,75,56) | (83,37,19) | Port (67,24,5) | heller, andere Kamera, nur grob vergleichbar |
| Mantel Bild 2 | (178-198,108-129,121-155) | (77,72,65) | Textur (201,209,213) | fast neutral, Weiss-Bezug |
| Leon Gesicht | (178,101,112) | (75,47,37) | Textur (222,139,106) | F 0.34/0.34/0.35, neutral |
| Leon Weste | (31,21,49) | (1,10,17) | (11,17,23) | Schwarzende zu dunkel, Bias -8 |
| Leon Hose | (31,48,88) | (7,26,29) | (15,24,34) | R -8, G,B stimmen |
| Frau Hose | (129,34,50) | (54,28,25) | (33,25,20) | G,B stimmen, **R +21** Einstreuung |

Daraus zwei Korrekturen in der Palette: Am Schwarzende +8/+4/+5, in Bild 2 ist der Rotanteil dunkler Stoffe bis +21 unsicher.

Vorwaertsprobe der Palette (flache Flaeche, Spielfarbe -> V gegen gemessen): Rosa (196,132,176) gegen (203,144,183), Baender Bild 1 (143,152,192) gegen (152,153,189), Rotbraun Bild 1 (66,30,77) gegen (84,47,87), Baender Bild 2 (81,60,88) gegen (97,57,84), Streifen (195,114,146) gegen (201,116,150). A03 trifft nicht ((166,175,209) gegen (201,204,219)): Sein Wert ist nur Untergrenze.

## 5. Licht

Lichtmischung im Spiel, `re15_light.h`: Schirm = Textur mal (Ambient + Summe Farbe*max(0,N*L)*Abfall) / 256. Das ist rein diffus, **Glanzlichter gibt es in dieser Engine nicht**. Ein Glanz muesste in die Textur gemalt sein.

| Raum | Disc | Im Video gemessen |
|---|---|---|
| ROOM5090, RDT @0x55c | alle 16 Schnitte Ambient (83,78,78), alle Lichtfarben 0. Faktor 0.324/0.305/0.305. Kein Licht-Opcode im SCD | Mantel: Faktor (0.385,0.342,0.307), links, rechts und oben gleich hell, Laengsprofil +-7. Kein Seitenlicht messbar |
| ROOM5080, RDT @0x208 | Ambient (74,73,73), L0 (238,238,238) Reichweite 4800, L1 (218,218,148) Reichweite 3500. Punktlichter | Leon: Faktor 0.34 (neutral, Gesicht) bis (0.70,0.63,0.49) an hellsten Stellen. Blau faellt ab wie bei L1. Figur im Mittel (0.487,0.432,0.374) |

Der gemessene Faktor liegt in beiden Raeumen 10-20 % ueber Ambient/256. Das liegt im Fehler der Balance, kann aber auch echt sein (offen). Fuer die Textur zaehlt der gemessene Wert, er ist an bekannten Modellen im selben Bild geeicht.

Die Lichtfassung der Beta-Disc selbst ist nicht belegt, belegt ist nur: Die Frau passt zum flachen Licht, Leon zu L0/L1.

## 6. Licht oder Oberflaeche

| Erscheinung | Urteil | Begruendung | Sicherheit |
|---|---|---|---|
| Violett, Lavendel, Blau | Video | Rampen sind gegeneinander versetzt, Grau wird Lavendel/Violett. Weisser Mantel ist im Video genauso lila | sicher |
| Blauer Boden, blauer Schimmer | Video | Boden zurueckgerechnet grau. Schwarz ist Streulicht, in Bild 1 B am staerksten | sicher |
| Tuerkis unten in Bild 1 | Boden blutet ein | R/G 0.68, nur am Boden, fehlt in Bild 2. Deckt sich mit `lesung_fremd.md` | wahrscheinlich |
| Helle Baender | Oberflaeche | Bild 2 hat flaches Licht und zeigt sie trotzdem. R/G 1.0 gegen 1.7-1.8 bei Rotbraun: neutrales oder gelbes Licht aendert R/G nicht | wahrscheinlich |
| Mehr Helligkeit in Bild 1 | Licht | Baender 1.3-1.4fach, Rotbraun 1.1-1.2fach gegen Bild 2, im Fehler vertraeglich, Faktor passt zu Leon | wahrscheinlich |
| Dunkle Grundhaut | Oberflaeche | dunkler als Leons Weste im selben Licht | wahrscheinlich |
| Rosa Stelle | Oberflaeche, Lichtanteil offen | Waere sie Rotbraun im Licht, braeuchte sie F (0.84,0.88,0.65), Leon erreicht (0.70,0.63,0.49). Reicht fuer Oberflaeche, schliesst Licht aber nicht aus | offen bis wahrscheinlich |
| Weisse Kerne | keine Aussage | Clip plus Ueberlauf: Auch Rot oder Orange werden hier weiss | sicher, dass nichts ablesbar ist |

⛔ **Abweichung zu `lesung_zeichnung.json`:** Dort sind viele helle Stellen als Glanzlicht eingestuft ("die meisten duerften Licht sein"). Das ist als Gestalt-Beschreibung lesbar, als Mechanismus nicht: Die Engine hat kein Glanzlicht, und die Stellen haben einen anderen Farbton als die Haut. In Bild 1 kann diffuses Licht auf Flaechen die Helligkeit um rund Faktor 2 aendern (Leon), den Farbton nicht.

Die Bandfarbe ist leicht kuehl, weil der Mantel als Bezug im Video waermer liest als seine Textur. Neutrales Hellgrau (140,145,150) liegt im Fehler.

## 7. Offen und nicht erkennbar

- Grundhaut unter e ~ 10: nicht aufloesbar. Texturen von fast Schwarz bis (70,40,45) sehen im Video gleich aus.
- Rosa Stelle in Bild 2: nicht erkennbar. Ihre Helligkeit haengt am Licht in Bild 1, die Kamera dort ist unbekannt.
- Farbton der Grundhaut: In Bild 2 ist Rot durch Einstreuung unsicher, in Bild 1 liegt alles im Rauschen. Das Rotbraun der Grundhaut ist abgeleitet, nicht gemessen.
- Untere Streifen: Feuer ist im Video geclippt, sein Streulicht also zu klein abgezogen. Echte Farbe dunkler als angegeben, wie viel: offen.
- Weisse Kerne A03, B13: nur Mindesthelligkeit.
- Kanalbalance Bild 1: +-8 % in e, keine direkte Messung.
- Rosa-weisser Gegenstand (A29, A30): nicht als Haut gewertet, in der JSON als `rosa_band_unten_A29` gefuehrt.

## 8. Dateien

| Datei | Inhalt |
|---|---|
| `re15_port/tools/form_video.py` | `video_filter`, `video_umkehr`; Befehle `pruefung`, `umkehr` |
| `analysis/befunde_runde41/form/farbe_palette.json` | Materialien, Licht, Fremdfarben, 37 Merkmale |
| `analysis/befunde_runde41/form/farbe_bild1_echt.png`, `farbe_bild2_echt.png` | Rueckrechnung in echter Helligkeit. Geclippte Flaechen (Feuer, Kisten, Lampen) sind Untergrenzen ohne Farbaussage, ihr Grau ist kein Messwert |
| `analysis/befunde_runde41/form/farbe_filter_pruefung.png` | Video, Port roh, Port gefiltert, Differenz |
| `analysis/befunde_runde41/form/lesehilfen/farbe_*` | helle Fassungen, Figur x3 mit Gitter, Farbmuster, Port-Hintergruende, Dia 144 samt Homographie |

Die Messskripte liegen im Sitzungs-Scratchpad und wurden nicht uebernommen. Reproduzierbar im Baum sind Filter, Umkehr und Pruefung, die Anpassung der Kurve nicht.
