# Pruefbericht Zahlen, Farbe Versuch 3 Runde 4 (K5c, weniger Rot, Feuerrillen entfernt)

Pruefer: Zahlen-Pruefer, 2026-11-07 Abend. Nicht committet.

Gegenstand: `farbe/K5c_farbe_v3/`, also FORM.EMD sha256 07d679a5... und FORM.TIM sha256 51282806....
Beide sind bytegleich mit `farbe/K5C_FARBE_V3.EMD/.TIM`. Stand: Zweig r41/form, HEAD 604f460d2. Geprueft wurden die Commits 32f3d6b9a, b6ed73526, bf3b8a765 und 604f460d2.

Bindend ist der Nutzersatz vom 2026-11-07 ~17:10: "... entfernst die uebertriebenen roten feuerrillen wieder, und stellst es wieder nach vorlage um?"

Eigener Code liegt im Scratchpad `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe4/zahlen_r4/`:
- `gem.py`: Hilfen. Ader-Klasse = Rampe der G4-Rotklasse ROT_C0..C1 15/20, Farbton -40..70 (kf_g4.py:95). Regionen nach Nachtrag 1.2.
- `statisch.py`: EMD/TIM-Diff r3 -> r4, CLUT-Zeilen, Zeilennutzung je Region.
- `texel.py` -> `texel.txt`, `texel.json`: Adern gegen die Haut im 3-Texel-Ring, je Komponente und je Texel. Dazu Rotbloecke und Farbton im Texelraum, jeweils r4, r3 und die G4-Basiszeile auf denselben Texeln.
- `kette.py` -> `kette_r4.json`, `kette_r4_saat123.json`, `kette_r3.json`:
  - RV1-RV4 neu gerechnet, mit scipy-Label statt cv2;
  - zusaetzliche Saaten 1, 2 und 3;
  - Adern gegen die Haut je Pixel ueber ein ID-Raster: Albedo-Render mit Kennungs-CLUT, R = Zeile, G = Index;
  - gemessen in "Texel x Licht" (Szenenrender vor der Kette) und in der "Kette T" (Mittel der 3 Saaten).
- `p1_pruef.py`, `h2_pruef.py`, `f1_pruef.py`, `dunkel.py`, `kopf_bild1.py`: Gegenrechnungen zu P1, H2, Baufehler A, dunklem Grundton und Kopf unter Bild-1-Licht.
- Nachlaeufe:
  - `riegel_nach.txt`: riegel_farbe_v3.py mit --gegenprobe-v2 und --gegenprobe-r3;
  - `rundlauf.txt`: riegel_rot.py --rundlauf;
  - `ctest.log`.

## 0. Ergebnis

**Nicht bestanden.** Der Kern des Nutzersatzes ist erfuellt: Die Feuerrillen sind weg, und Rotmenge, Rotverteilung und Farbton liegen in beiden Szenenketten im Fenster der Vorlage. Drei Gruende sprechen trotzdem gegen eine Abnahme:
1. Das Riegel-Paket hat 17 ROT. Drei davon sind neue, echte Rueckschritte gegen Runde 3: A7 (Krallen), P1 (die Adern selbst sind posterisiert) und K6 (die Haut in Bild 2 ist dunkler als die Vorlage).
2. "Nach Vorlage umstellen" ist bei der Hauthelligkeit in Bild 2 verfehlt, und zwar weiter als in Runde 3. Kein bindender Riegel faengt das.
3. "Adern nirgends heller oder roter als die Haut" gilt im Mittel und in fast allen Komponenten, aber nicht woertlich ueberall (Abschnitt 3).

Kurzfassung der Messungen:

| Pruefpunkt | Ergebnis | Beleg |
|---|---|---|
| RV1-RV4 (Rot-Ziele je Region, beide Szenenlichter) | **45/45 GRUEN**, eigene Rechnung zahlengleich mit dem Bauer; mit **neuen Saaten 1/2/3 ebenfalls 45/45** | `kette_r4.txt`, `kette_r4_saat123.txt` |
| Feuerrillen (Adern heller UND roter als die Haut, > 2) | Texel x Licht: Bild 1 **0,5 %** der Aderpixel (r3 28,8 %), Bild 2 **0,3 %** (r3 22,4 %) | `kette_r4.json` / `kette_r3.json` |
| Adern im Mittel je Region | dL und da\* in **allen** Regionen <= 0 (Texel und Texel x Licht) | Abschnitt 3 |
| Rote Bloecke in den Ketten (RV4) | GRUEN in allen bindenden Regionen; Bild 1 Figur 2,63 % (Vorlage 2,3, Grenze 5,55) | `kette_r4.txt` |
| Geometrie | Diff 0: Ecken, Normalen, Flaechen, Skelett, Frames und Clips sind gleich. 3592 UV-Saetze unterscheiden sich nur im CLUT-Feld. TIM-Indizes bytegleich | `statisch.py` |
| Spiel-Asset, Pins, re15_port | unveraendert. BAHNSTEIG_FORM zuletzt in 67408cfcb (K4). `git diff 657512427..HEAD` beruehrt nur `.../varianten/K5/` | git |
| Form-Tests | **11/11** bestanden, 96,2 s, `-j 3` | `ctest.log` |
| Riegel-Paket riegel_farbe_v3.py | **76/93, Exit 1**; der Nachlauf ist bis auf die Zeile EXIT bytegleich mit `farbe/riegel_K5c_farbe_v3.txt` | `riegel_nach.txt` |
| Gegenproben | Alle fallen aus wie verlangt: Runde 3 RV 21/45 (24 ROT), RV6 an r3 in allen Regionen VERLETZT, Rundlauf 45/45 Exit 0, V2/G4/Orange/Gelb wie Messblatt | `riegel_nach.txt`, `rundlauf.txt`, `kette_r3.txt` |
| Baufehler A (F1 mit rohem G4-Blutrot) | **behoben**: 12 Texel in r3, **0** in r4. Die 8 Rand-Eintraege der Zeile 6 sind jetzt mauve (a\* 7-27, b\* -4..0) statt G4-Rot (a\* 18-59, b\* 6-43) | `f1_pruef.py`, `statisch.py` |
| Tafeln | fair: Tafel 09 zeigt Vorlage, r3 und r4 im gleichen Ausschnitt, mit gleichem Filter und gleicher Saat, ohne Gamma (Code `kf_tafeln_v3.rot_vorlage`). Kosmetik: In der Vorlage-Spalte ist die Unterschrift rechts abgeschnitten | Tafel 09 |

## 1. Rot-Ziele RV1-RV4 (Nachtrag 5), eigene Rechnung

Das Verfahren ist das des Nachtrags 1.4, eigenstaendig nachgebaut:
- Klasse: a\* >= A0; A0 aus mess.json (Bild 1 T 8, Bild 2 T 19).
- "Rot eng": Farbton -30..70.
- Farbton: atan2 der Mittelwerte.
- dL: a\*-Klasse minus Rest.
- Groesste Flaeche: scipy, 8-Nachbarschaft.
- Maske: rot_masken.npz geschnitten mit der Figur der Variante.
- Fenster aus ziele.json.

Alle 45 bindenden Zeilen sind GRUEN. Die Werte stimmen auf 2 Nachkommastellen mit `farbe/riegel_rot_K5c_farbe_v3.txt` ueberein.

**Generalisierung:** Die Parameter wurden mit Saat 15 gegen genau diese Riegel gesucht (r4_par.json, quelle). Deshalb habe ich mit drei Saaten nachgemessen, die in der Suche nicht vorkamen (1/2/3): wieder **45/45**. Die knappsten Zeilen, Wert bei den Saaten 15/12345/7 gegen die Fenstergrenze:
- Bild 2 Hals RV3: -1,86 (-2,09 / -1,71 / -1,77) gegen die Obergrenze -1,53.
- Bild 1 Flanke RV4: 1,83, die Untergrenze 2,31 ist nur Info.
- Bild 2 Ruecken RV1: 58,5 (61,0 / 58,5 / 56,0) gegen die Obergrenze 65,2.
- Bild 1 Figur RV3: +1,93 gegen die Obergrenze 3,03.

Belegzahlen gegen den Bericht des Bauers, Vorlage | r3 | r4:
- Bild 1 Schwanz Rot eng: 23,5 | 43,5 | 23,45.
- Bild 1 Figur, groesste Rotflaeche: 2,3 | 10,6 | 2,63.
- Bild 2 Ruecken, Farbton: -5,3 | 30,6 | -8,86.
- Bild 2 Hals, Rot eng: 45,1 | 87,1 | 26,38.

Alle stimmen.

## 2. Riegel-Paket riegel_farbe_v3.py: 76/93, die 17 ROT eingeordnet

Ein eigener Nachlauf ergibt dieselben Zeilen (Exit 1). Verglichen mit Runde 3 (657512427: `farbe/riegel_K5c_farbe_v3.txt`):

**Neu GRUEN:**

| Riegel | r3 | r4 |
|---|---|---|
| A1 | 7,3 | 11,5 |
| A8 | ROT | GRUEN |
| B11 | ROT | GRUEN |
| N1 | (9,1 / 7,5 / 7,1) | (10,4 / 9,6 / 9,8) |
| B9 | 14,2 | 8,3 |

B2 entfaellt nach Nachtrag 4.1.

**Neu ROT:**

| Riegel | r3 | r4 | Grenze | Einordnung (eigene Messung) |
|---|---|---|---|---|
| **A7 Kralle HF_rest** | 0,096 | **0,349** | <= 0,19, Familie 0,039-0,182 | **Echter Rueckschritt.** Ursache gemessen (`texel.txt`, Zeile 16): Die Krallen-Adern liegen jetzt bei L\* 8-18 im behaltenen Horn (L\* 56-65). Komponenten haben dL -38,6 bis -50,5 gegen den Ring. Im G4 an denselben Texeln war es dL -23,2 (r3-Komponente 233/207: Ader L\* 30,2, Ring 53,5). Die Krallen sind dadurch schwarz gesprenkelt. |
| **P1 Posterisierung** | 20,9 / 27,1 / 281 | **65,8** / 12,0 / 191 | dL0 <= 30, 5-10 >= 15, Farben >= 200 | Die Erklaerung des Bauers ("zaehlt jetzt flaechige Haut") stimmt **nur zum Teil**. Von den P1-Fleischtexeln sind 2085 Haut und 473 Ader. **In den Adern selbst** liegt dL = 0 bei **72,4 %**, an den G4-Basisfarben derselben Texel bei 21,9 %. dL 5-10 liegt bei 2,5 % gegen 27,0 %, es gibt 135 statt 217 Farben (`p1_pruef.py`). Die Abstufung in den Adern ist also weitgehend verloren. Bei L\* 5-13 sind 15-Bit-Stufen etwa 2-4 L\* gross; dunkle Adern koennen darum nur wenige Stufen haben. Teilkonflikt mit "dunklere Adern"; das muss bewusst entschieden werden. |
| **A4 Farbton Fleischrot** | 21,0 | **5,0** | 15-50 Grad (Familie V1) | **Konflikt mit dem Nutzersatz** ("nicht G4-Orange"). Das Familienfenster verlangt Orange. Als Info fuehren. |

**H2** war schon in Runde 3 ROT (FARBE_PRUEFUNG_v3_r3_zahlen.md, Befund A). Die Meldung des Bauers "neu ROT" ist insoweit falsch. Neu ist die Ursache. Eigene Rechnung (`h2_pruef.py`, Vertauschungen der genutzten Eintraege je Klasse, |dL\*| > 0,5):
- Ader-Klasse (w = 1): in allen Zeilen **0** Vertauschungen. Die Ader-Stauchung ist rangtreu.
- Haut (w = 0): 0-3 kleine Vertauschungen (<= 1,3 L\*), ausser in **Zeile 16 Vorderbeine: 42 Vertauschungen, davon 38 mit > 2 L\*, groesste 51,5**. Ursache:
  - Haut mit a = 1,44 wird ueber die behaltenen Horn-Eintraege (L\* >= 52,4) gehoben.
  - Zugleich wird Haut oberhalb G4-L\* 21,7 **unbunt grau**: s(L) = 0 bei beta 1,5. Das betrifft 22 Eintraege mit L\* 30-63 und a\* = b\* = 0, also 4,5 % der Texel der Zeile 16. In Zeile 14 "Hals Bauch" (a 1,6) sind es 13,0 %, in Zeile 4 "Hinterbeine" 4,2 %.
- Rampen-Eintraege (0 < w < 1): Zeilen 4, 12 und 16 haben bis 14,0 / 8,7 / 23,8 L\*.

**Schon in Runde 3 ROT, unveraendert eingeordnet:**

| Riegel | Wert / Stand | Einordnung |
|---|---|---|
| F2t | bewusst (V3R3.3) | wie Runde 3 |
| P2 | Klauen +39,8 / +50,4 | der gleiche Mechanismus wie A7 |
| B10, B12, B13, B14, B16, B17 | - | Binnenzeichnung |
| K3 | max 8,2 | - |
| **K6** | **7,3** (r3 6,8) | siehe unten |
| K7 | 9,9 | Leuchtstelle, Unterkiefer |
| K1v2 / K2v2 | (6,4 / 7,8), (5,8 / 8,0) | duerfen nach Nachtrag 4.1 nicht mit Rot erfuellt werden |

**K6 (Bild 2 Figur dE) ist schlechter geworden: 7,3 statt 6,8.** Eigene Messung (`dunkel.py`, feste RV-Maske, 3 Saaten):

| Bild | Vorlage | Runde 3 | **Runde 4** |
|---|---|---|---|
| 1, T-L\* < 20 / Mittel-L\* | 68,2 % / 19,6 | 62,4 % / 19,1 | **73,3 % / 17,2** |
| 2, T-L\* < 20 / Mittel-L\* | 59,5 % / 22,1 | 71,9 % / 18,0 | **87,3 % / 15,6** |

Der Auftrag lautet "dunklere Adern, Haut nach Vorlage". In Bild 2 liegt die Haut **6,5 L\* unter** der Vorlage, weiter weg als in Runde 3 (4,1).
- K13 (>= 46,4 %) misst nur die Untergrenze und ist deshalb GRUEN (81,1 %).
- RV3 misst nur relativ.
- Die Parameter a = 1,3-1,6 der Bild-2-Gruppen heben den Mittelwert nicht genug.

Der Grund: Ruecken- und Flankenzeilen sind sehr dunkel, Lm 5,1-8,5. Ausserdem fehlen die hellen Adern aus Runde 3.

Bild 1 bleibt im Rahmen "dunkler Grundton" (73,3 % gegen 68,2 %, Messblatt V3 0.1: 63,6-65,3 %).

## 3. Adern gegen die Haut (Zusatzpruefung, eigener Code)

### 3.1 Texelraum (`texel.py`)

Ader-Komponenten (8-Nachbarschaft) je Zeile und Region, verglichen mit dem Haut-Ring von 3 Texeln.

| Region | Komponenten | dL > 0 | da\* > 0 | Mittel dL / da\* | dL max | lokal heller (Texel, > +0,5) | lokal roter |
|---|---|---|---|---|---|---|---|
| Ruecken | 17 | 0 | 6 | -4,97 / -2,81 | -0,59 | 7,8 % | 22,9 % |
| Flanke | 15 | 3 | 6 | -4,87 / -1,90 | +2,6 | 11,0 % | 23,8 % |
| Brust/Bauch | 12 | 2 | 5 | -6,13 / -0,56 | +2,57 | 10,3 % | 26,0 % |
| Vorderbeine | 58 | 5 | 22 | -8,62 / -0,70 | +7,16 | 6,1 % | 33,7 % |
| Hinterbeine | 32 | 11 | 4 | -2,45 / -3,82 | +4,13 | 25,1 % | 28,7 % |
| Hals | 2 | 1 | 0 | -2,49 / -9,53 | +5,47 | 41,7 % (12 Texel) | 0 % |
| Kopf | 9 | 3 | 2 | -3,93 / -3,33 | +12,96 | 13,0 % | 19,0 % |
| Schwanz | 12 | 0 | 2 | -6,37 / -0,10 | -5,41 | 0,4 % | 44,7 % |

Gegenprobe Runde 3: In allen Regionen sind die Mittel positiv, Mittel bis dL +12,3 und da\* +20,5. 87-100 % der Adertexel liegen in verletzenden Komponenten.

G4-Basis auf denselben Texeln: 100 %, da\* +17..+30.

Komponenten mit >= 4 Texeln und dL > +1 **und** da\* > +1, also eine helle rote Linie:
- Es gibt **genau eine**: Zeile 0 (G4 roh), Feld "Augen F10", Texel (104,71), 11 Texel.
- Lab (19,9 / 27,0 / 13,6) gegen den Ring (7,0 / 3,3 / -2,0): dL +13,0, da\* +23,6.
- Das ist unveraendertes G4-Rot im Augenfeld. Die Zeilen 0-2 sind nach H2 bytegleich G4, und RV6 nimmt "Augen F10" aus (RV6_AUS).
- In Bild 2 sind es 5 Pixel (234/273), in Bild 1 0 Pixel.
- In Runde 3 lag es genauso. Das ist keine Feuerrille am Koerper, aber woertlich eine Ausnahme von "nirgends".

Die uebrigen Ausnahmen sind klein:
- **dL > +1:** sechs Komponenten von 5-12 Texeln, **alle mit da\* < 0**. Es sind hellere graue oder entsaettigte Punkte, kein Rot. Die groesste ist Hals Zeile 14 mit dL +5,5 und da\* -11,3.
- **da\* > +1:** Diese Komponenten sind **alle dunkler** als der Ring, dL -1,8 bis -50. Die staerkste ist Ruecken Zeile 13 (234,48): Ader (9,3 / 13,7 / -9,0) gegen Ring (17,2 / 5,5 / -3,8), also ein dunkler, etwas buntrer Mauve-Punkt.

### 3.2 Szenen (`kette.py`, ID-Raster)

Jeder Aderpixel wird gegen das Haut-Mittel im 7x7-Fenster verglichen.

| Raum | Bild | Aderpixel | dL Mittel | da\* Mittel | heller > 2 | roter > 2 | **beides > 2** | Komponenten mit beidem > 2 |
|---|---|---|---|---|---|---|---|---|
| Texel x Licht | 1 | 2859 | -1,94 | -0,52 | 0,8 % | 7,6 % | **0,5 %** (r3 28,8 %) | 0 (r3 50) |
| Texel x Licht | 2 | 9055 | -3,21 | -1,95 | 0,5 % | 4,9 % | **0,3 %** (r3 22,4 %) | 2: Augenfeld, 5 px (r3 10) |
| Kette T | 1 | 2859 | -0,49 | +0,05 | 6,2 % | 3,3 % | 0,5 % (r3 0,3 %) | 0 (r3 2) |
| Kette T | 2 | 9055 | -1,38 | +0,78 | 2,4 % | 14,1 % | 0,4 % (r3 3,6 %) | 1 (r3 9) |

Je Region gilt in Texel x Licht: dL und da\* sind in **allen** Regionen beider Bilder negativ.

In der Kette T gilt:
- Das Mittel von da\* ist in Bild 2 leicht positiv: Figur +0,78, Vorderbeine +1,69, Ruecken +0,80.
- Das ist eine Folge der Chroma-Unschaerfe der Kette, keine Texeleigenschaft. Gegenprobe Runde 3: Texel x Licht da\* +9,07, in der Kette -0,52. Die Kette taugt also fuer "roter" nicht als Mass.
- Die eine Komponente in der Kette T von Bild 2 liegt bei (408,212): 18 px, dL +3,7, da\* +4,4. In Texel x Licht hat sie keine Entsprechung.

**Bewertung:**
- Die Feuerrillen sind messbar entfernt: Das Merkmal "heller und roter" ist von 22-29 % auf 0,3-0,5 % der Aderpixel gefallen.
- "Nirgends roter" gilt woertlich nicht: 4,9-7,6 % der Aderpixel liegen in Texel x Licht lokal mehr als 2 a\* ueber der Haut. Diese Stellen sind alle dunkler als die Haut.
- Dazu kommen das G4-Augenfeld (Zeile 0, 11 Texel) und sechs kleine graue, hellere Punkte.

## 4. Rote Bloecke

- **In den bindenden Ketten** ist RV4 in allen bindenden Regionen GRUEN. Bild 1 Schwanz 6,7 % (r3 22,9 %, Grenze 9,65), Figur 2,63 %.
- **Restrisiko (Info, kein Ziel der Vorlage):** Kopf, Hals und Brust haben ihre Ziele nur aus Bild 2, also unter Feuerlicht (Nachtrag 1.2 und 7.4). Ihre Haut ist **an sich rot** eingefaerbt:
  - Gruppe Kopf: Farbton -9, C 28,9, a 1,3.
  - Texelraum Kopf: 86,8 % "Rot eng", groesster zusammenhaengender Block **37,6 %** der Kopftexel. G4-Basis auf denselben Texeln: 34,6 % / 12,2 %.
- Probe unter **Bild-1-Licht** (Bahnsteig), Figur um 1536 / 2048 / 2560 gedreht, Kette mit Saat 15 (`kopf_bild1.txt`). Rot eng / groesste Flaeche in Prozent des Teils:

| Variante | Kopf | Hals | Brust | Vorderbeine |
|---|---|---|---|---|
| r4 | 65-83 / 60-80 | 61-90 / 61-90 | 34-63 / 29-62 | 38-55 / 19-32 |
| r3 | 60-73 / 29-66 | 75-78 / 73-78 | 50-72 / 50-71 | 54-57 / 18-29 |

  - Zum Vergleich die Vorlage Bild 1, ganze Figur (Rueckansicht): 14,1 / 2,3.
  - Der Farbton ist von Orange (r3 +20..+27 Grad) zu Karmin (r4 -12..+11 Grad) gewandert.
  - Der Kopf ist in r4 unter Bahnsteiglicht so rot wie in Runde 3 oder roeter, und er ist **ein** Block.
  - Ob die Front so sein soll, sagt kein Bild der Vorlage. Der Nutzer hat aber "flammenfarben leuchtendes Gesicht" selbst beschrieben (Auftrag V3).
  - Das gehoert vor die Abnahme vor den Sicht-Pruefer oder den Nutzer, mit einer Frontansicht unter Bahnsteiglicht.

## 5. Farbton "mauve-rosa, nicht G4-Orange" (Texelraum, a\*-Klasse, Info)

| Region | r4 | r3 | G4-Basis gleiche Texel |
|---|---|---|---|
| Schwanz | -24,4 | -3,8 | 23,7 |
| Hinterbeine | -25,7 | 0,2 | 22,0 |
| Flanke | -37,9 | 27,5 | 28,2 |
| Brust | -37,0 | 27,6 | 28,2 |
| Hals | -24,8 | 30,7 | 27,7 |
| Kopf | +1,1 | 25,9 | 27,2 |
| Ruecken | -0,3 | 27,9 | 27,6 |
| **Vorderbeine** | **+32,9** | 17,4 | 25,8 |

- Die Vorderbeine (Zeile 16, h 39,85) sind im Texel **oranger als G4**. Das folgt dem Bild-2-Ziel der Klauen (Vorlage 36,6 Grad, Feuerschein).
- Nachtrag 6 schreibt dazu selbst: "Klauen brauchen eher mehr Orange, der Arm weniger Rot. Eine gemeinsame Umfaerbung trifft beides nicht."
- Nach RV2 ist das zulaessig. Unter Bahnsteiglicht sind die Vorderbeine aber G4-orange-braun.
- Die Gruppe "Rumpf Ruecken" hat h +33 (G4-Orange) bei kleiner Buntheit (C_haut 6,5 / 2,8). Die "Rumpf Flanke" hat h -42,8 bei C 48,6. Sie ist **violett** (Mauve-Klasse < -30 Grad nach Nachtrag 1.3).
- Im Spielbild `farbe/spiel_v3/spiel_ruhe_r5080_s1.png` ist der Rumpf deutlich violett. Der Bauer hat das selbst vermerkt ("violetter als mauve-rosa").
- In der Kette mittelt sich das zu -21,4 Grad und ist GRUEN.

## 6. Konstanten mit Quelle

- `v3_daten/r4_par.json`: 9 Gruppen x 7 Werte.
  - Quelle: Koordinatensuche such5.py gegen RV1-RV4 und die Texelbedingung, dazu ein Handscan Hals.
  - Grenzen mit Nutzer-Wortlaut: q, kappa <= 1; a = 1 fuer Bild 1.
  - Belegt, aber **an die Riegel selbst angepasst**. Die Riegel sind deshalb kein unabhaengiger Nachweis. Die neuen Saaten 1/2/3 (45/45) zeigen nur, dass das Ergebnis nicht an der Saat haengt.
  - Suchgrenzen erreicht: Schwanz beta -1,0 (unten), Rumpf Ruecken und Hals Bauch beta 3,0 (oben), Vorderbein r B1 kappa 1,0 (Nutzerregel).
- `R4_L_HELL 52,4`: gemessen, Luecke 45,3-59,5, Dossier V3R4.2. Belegt.
- `R4_SMAX 3,0`: gemessen ohne Einfluss. Belegt.
- `R4_SMIN 0`: Suchgrenze. Belegt.
- `R4_N_GRENZE 0,5`: Nachtrag 1.2. Belegt.
- **Ohne Quelle:** die Untergrenze der Ader-Helligkeit "Haut-p10" in kf_g4.r4_zeile (`wquant(..., 0.10)`). Weder Messung noch Nutzer-Wortlaut sind zitiert.
- Die tanh-Fuehrung an den sRGB-Rand ist Verfahren, keine Zahl.

## 7. Weitere Feststellungen

- Die CLUT hat jetzt 17 Zeilen (TIM 139 808 statt 135 712 Byte).
  - Das Spiel laedt sie: Spiellauf in `farbe/spiel_lauf_v3.log`, 32 Meshes, 32 Knochen, 3 Clips. Die zehn Bilder liegen in `farbe/spiel_v3/` (bf3b8a765).
  - Die GLES2-Grenze von 4096 ist im Dossier V3R4.5 als offen vermerkt.
- Zeilenbelegung (`statisch.py`):
  - Zeile 0 traegt nur "Augen F10" (64 Flaechen) und "Maulinneres F2" (91).
  - Zeilen 1 und 2 sind unbenutzt.
  - Die Zeilen 3-5 und 9-16 sind die 11 Gruppenzeilen.
- Klasseneinteilung an der Kante: Eintrag 150 der Zeile 16 (G4 L\* 59,6, Farbton 70,2 Grad) wurde dunkel (11,4 / 11,0 / 10,2). Nach meiner Rechnung waere er nach R4_L_HELL behalten worden.
  - Wahrscheinlich liegt der Farbton in float32 knapp unter 70, der Eintrag wurde also als Ader eingestuft.
  - Betroffen sind 2 Texel. Keine Wirkung.

## 8. Empfehlung an den Orchestrator

Der Nutzersatz (Feuerrillen weg, Rot nach Vorlage) ist in Zahlen erfuellt. Das sollte die Vorlage an den Nutzer tragen. Vor einer Abnahme gehoeren dem Nutzer gezeigt oder entschieden:
1. **Krallen (A7, P2):** Die Adern sind jetzt fast schwarz im hellen Horn. Moegliche Abhilfe ist ein Ader-L\*-Boden je Gruppe aus dem Horn-Ring statt Haut-p10.
2. **Hauthelligkeit Bild 2 (K6):** Mittel-L\* 15,6 gegen 22,1 der Vorlage. "Haut nach Vorlage" ist hier verfehlt; die Haut ist dunkler als in Runde 3.
3. **Kopf und Hals unter Bahnsteiglicht:** ein roter Block von 60-90 %, ohne Vorlage.
4. **Riegel, die dem Nutzersatz widersprechen:** A4 (Orange-Fenster) und, teilweise, P1 (Abstufung dunkler Adern). Sie sollten als Info umgestellt werden, damit das Paket nicht dauerhaft ROT zeigt.
