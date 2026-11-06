# K5c Farbe, Runde 3 von 3 (Bauer, 2026-11-06)

Auftrag: Orchestrator, Spur Form. Die Riesenform K5c ("eher c") neu texturieren nach `MESSBLATT_FARBE_V1.md` und jeden Befund der Pruefung
Runde 2 beheben (`FARBE_PRUEFUNG_r2_sicht.md`, `FARBE_PRUEFUNG_r2_zahlen.md`). Runde 2 steht in `FARBE_r2.md`, ihre Variante in `farbe/K5c_farbe_r2/`,
Runde 1 in `FARBE_r1.md` / `farbe/K5c_farbe_r1/`.
Geometrie, Clips, UV, Dreiecke unveraendert: `farbe/K5c_farbe/FORM.EMD` = `farbe/K5C_FARBE.EMD` = `K5c/FORM.EMD` = Scratchpad `formfarbe/emd/K5C_FARBE.EMD`
(sha256 a71f8d08...). Neu ist nur `FORM.TIM` (sha256 7c719e12..., Texelindizes und 4 CLUT-Zeilen, gleiches Atlas-Layout). Spiel-Asset
`re15_port/shared_assets/RE2/BAHNSTEIG_FORM.*` und Pins unveraendert (`git diff 19a9b2b17..HEAD -- re15_port` leer).
Kein RE-Wert: neues Modell (PORT-WAHL, NUTZER_ENTSCHEIDE E5/E19/E20). Seiten: (W) = aus Sicht des Wesens. Lab = CIE-Lab D65 (OpenCV float).

## 0. Ergebnis

**Riegel 28/33 GRUEN, Exit 1.** ROT: K1, K2 (Bild-1-Farbkraft), K4 (Bild-1-Rotklasse, knapp), B2 (groesste Rotflaeche 3/4 vorn, knapp), B9 (Rot im
Umrissband, neuer Riegel). Gegenproben sauber: K5c heute 6/33 (gruen nur Halteriegel A10-A12 und B6/B7/B9), Runde 1 alle B1-B5 ROT, Runde 2 alle
B6-B9 ROT. **Nach Messblatt 7.5 gehen die Tafeln nicht als Abnahmevorlage an den Nutzer, solange Riegel ROT sind** (Entscheid Orchestrator, Abschnitt 7).

| | K5c heute | Runde 2 | **Runde 3** |
|---|---|---|---|
| Riegel | 6/33 | 24/29 (+ B6-B9: 0/4) | **28/33** |
| Malweise | Rueckprojektion + Grau | 46 Felder aus G4-Hautausschnitten | **Farbe aus der Lage am Koerper (3D), keine Kacheln** |
| Wiederholung B6 (<= 0,28 %) | 0,24 | 2,82 | **0,04** |
| Rot erosionsfest B1 (>= 32,1 %) | 21,5 | 28,0 | **35,9** |
| Groesste Rotflaeche B2 3/4 vorn / Seite (>= 3,2 %) | 1,19 / 0,61 | 3,65 / 2,45 | **2,97 / 5,96** |
| Rot im Umrissband B9 (<= 11,3 %) | 10,2 | 23,3 | **17,8** |
| Rot-Farbton-Streuung B7 (>= 5,8 Grad) | 19,9 | 4,5 | **11,3** |
| Zahnflecken Bild 2 B8 (13,6-33,9 %) | 41,1 | 0,2 | **14,1** |
| Neutraler Render C* N1 (>= 7,4 / 9,4 / 7,8) | 4,6 / 5,0 / 4,0 | 10,2 / 8,8 / 6,9 | **10,3 / 9,7 / 10,1** |
| Bild-1-Kette C* / ab-Streuung (K1 >= 7,9 / K2 >= 8,4) | 6,3 / 6,3 | 6,5 / 8,0 | **6,1 / 6,2 ROT** |
| Kiefer Zahn/Rot/Dunkel A8 | 77/0/0 | 6/96/39 | **6,9 / 25,3 / 39,6** |

## 1. Ursache der Ablehnung in Zahlen

Nutzer 2026-11-06 zu `rundum_bild1_K5c.png`: "passt von den Farben und von der Struktur ueberhaupt nicht ... nicht zum Rest des Spiels ... wie ein Pokemon".
- K5c heute (Messblatt 0/5): Koerper C* 4,9 (Familie 9,3-19,8); Fleischrot 0,5 % Magenta -10 Grad (Familie 11,5-38 % bei 25-48 Grad); sigma_ab je Teil
  4,0 (7,4-10,7); Kiefer 77 % Creme, 0 % Rot, 0 % dunkel, HF 0,461; Augen (207,95,161) C* 54; 0,79 % silberweisse Koerpertexel (Familie <= 0,19 %);
  Tafeln mit Gamma 0,5/0,6 nur auf K5c.
- Runde 1/2 behoben das, erzeugten aber (Pruefung r2): Rot als Netz und Saum (B1 28,0 %, Seite 2,45 %, Rot im Umrissband 23,3 % gegen EM30 11,3 %),
  gekachelte G4-Foto-Fragmente (Wiederholung 2,82 % gegen EM30 0,28 %), Rot ein Ton (4,5 Grad gegen G5 5,8 / EM30 13,2), Zaehne in Bild 2 unsichtbar
  (Hell-Klasse 0,2 % gegen 22,6 %), Konstanten ohne Quelle, Eichung nicht konvergiert.

## 2. Vorgehen (`generator/kf_mal3.py`; Runde 1/2 `kf_mal.py`/`kf_mal2.py` bleiben fuer den Nachbau)

**Grundsatz:** nicht mehr je Atlasfeld malen, sondern auf der Koerperoberflaeche. Jeder genutzte Texel bekommt Lage und Normale in der Ruhepose
(Clip 2, `kf_geo.py`), alle Texel bilden einen Graph (8 naechste 3D-Nachbarn + 4er-Nachbarn im Atlas). Glaetten, Ergaenzen und Formen laufen auf
diesem Graph, also ueber Feld- und Gliedgrenzen hinweg (E8 "aus einem Guss", E9, E12 "keine UV-Naehte"). Nichts wird gekachelt (B6 0,04 %).

1. **Koernung** = K5c-eigene Struktur (Textur H2, E16 "sehr viel besser"; HF 0,105 liegt in der Familie, Messblatt 1): L* minus Grobbild
   (sigma 2,5 Texel). Glanzkeime (L* > 85, C* < 10) und Gaumenstreifen (Zahnklasse im Kopffeld) samt Hof (L* > 58) vorher harmonisch entfernt
   (3236 Texel). Keine G4-Hautausschnitte mehr.
2. **Farbkarte aus den Screenshots** (E11): Original Bild 1/2 als T (`p_zonen.rueck`, Bild 2 ohne Einstreuung a* 12,8 / b* 3,4) auf die sichtbaren
   Texel (Bild 1 9357, Bild 2 13048 Texel); verdeckte aus dem Spiegelpunkt z -> -z (+14748 Texel), Rest harmonisch, Glaettung 2,5 Texel.
   Haut-L* hoechstens 58 (hellstes Original-Band), Blau-Klasse (b* < -6 und C* > 8) auf C* 8 begrenzt (Farbton bleibt).
3. **Hautfarbe in wenigen Toenen** (E8 "wenige Farben je Teil", E10 "Violett, Rotbraun, Blaugrau"): k-Mittel k = 2 der Nicht-Rot-Hautpixel beider
   Bilder -> warme Seite (5,8/8,2) und kuehle Seite (2,0/-6,6); je Seite Rampe Helligkeit -> Farbe (Mittel je L*-Stufe, Werte in `bau.json` 'hautseiten').
   Je Texel Mehrheit warm/kuehl (Schwelle 0,5, Kante 2,5 Texel): 45 % der Haut warm. Statt einer Mittelfarbe, deren Buntheit sich aufhebt.
4. **Form = gemalte Anatomie** (Sicht r2 P2 "kein Feld zeigt Anatomie"; E8 "Lichtauffassung"): Hohlraum-Mass je Texel (mittlerer Sinus der
   Nachbarhoehe ueber der Tangentialebene, k-NN 64 vorzeichenbehaftet + 256 nur Falten). Falten/Beugen dunkel, Woelbungen hell; Staerke so, dass die
   Koerper-L*-Streuung die von RE2-G4 erreicht (14,9, Messblatt 1): K_FORM 10,27. Dunkle Falten (L* < 20) der warmen Seite in der G4-Dunkelrotstufe
   (18,9/25,6/12,2) = "dunkelrote Tiefen" (E10), 9,6 % der Haut.
5. **Fleisch als grosse Flaechen** (Sicht r2 P1/4.1): Screenshot-Mehrheit der Rotklasse (E14, 3744 Texel: Z1 Ruecken, Klauen, Schwanzspitze) und die
   **nie gesehene Rumpf-Unterseite** (Meshes 0/7/10/21, Normale nach unten, 10129 Texel) - nicht mehr an den Gliedunterseiten (Saeume). Zusammen
   20,7 % der Koerpertexel. Fleischfarbe = Rampe ueber die Familien-Rotstufen nach Helligkeit (G4 18,9/25,6/12,2 -> G4 32,8/43,7/25,8 ->
   EM30 36,5/37,4/20,5 -> Familienwunde 51,5/26,5/18,0 -> G4 64,3/24,7/16,3), wo sichtbar plus die Screenshot-Abweichung (Z1 rosa).
   Helligkeit: Screenshot-Fleischmittel 33,3 bzw. am Bauch die satten Familienstufen (34,65) + Form + Koernung auf +-1 sigma begrenzt (keine Loecher).
   Ergebnis: Rot-Farbton-Streuung 11,3 Grad (keine harte Drehung mehr), Fleisch im Render als zusammenhaengende Bauch-/Flankenflaeche.
6. **Kiefer**: sichtbares Band (1161 Texel) = Screenshot Bild 2 samt Zahnflecken (B8 14,1 %); Maulinneres RE2-G4-Maul (10,44,60,40), dessen
   Zahnfleisch nach eigenem Rot (a*/21,5 = G4-Maul-p90) auf die Fleischrampe gezogen; Zaehne und dunkles Inneres bleiben. A8 6,9/25,3/39,6 %
   (Familie: Zahn 2-19, Rot 21-36, Dunkel 24-64). Krallen RE2-G4 (231,200,20,50), Zahnfeld Bild-2-Zahnflecken, Augen E24 dunkel (wie Runde 2).
7. **Eichung gegen die Kette** (Zonenversatz L*/a*/b* je Bildzone, nur auf Haut; Hautbuntheit g): voller Schritt (kein Daempfungsfaktor),
   g <= G_AB 1,48 (kf_mal.py:50), Saettigungsregel (bewegt eine Aenderung > 1 die Kette um < 10 %, bleibt der Anteil gesperrt), Abbruch bei
   Schritt < 1 dE. Verlauf `generator/kf_eich3.json` (15 Runden ueber 4 Codestaende e2-e5; die letzten zwei mit dem Endcode: Schritt 1,10 ->
   **0,71**, Fixpunkt). Endversaetze (L*, a*, b*): Schwanz (-16,5/5,0/-12,7), Ruecken (-4,0/8,0/-19,2), Hinterbein l (W) (-8,9/18,6/-31,6),
   Hinterbein r (W) (-17,0/14,9/-51,4, b* gesperrt: Blau-Grenze), Vorderglied r (W) (-6,9/-5,6/-5,9), Oberkiefer (-0,2/7,0/-7,5),
   Kopf/Rumpf (1,7/2,6/-9,2), Unterkiefer (43,3/26,0/19,2), Klauen 0. Zonen-dE der Riegel-Zonen: K3 max 4,9, K6-K8 <= 1,6.
   Nachbau `python kf_mal3.py bauen <aus>` aus kf_eich3.json ist **byte-gleich** (cmp FORM.TIM).

### Konstanten (Quelle je Zeile im Code, Liste `bau.json` 'konstanten')

Belegt: SIG_GROB 2,5 (kf_mal.py:40-42), GLANZ/HOF/GAUMEN (A5, Messblatt 2/3), EINSTREU_A2 12,8 / B2 3,4 (Messblatt 2, p_zonen.json), Blau-Klasse
-6 / C* 8 (Sicht r1 P2, B3), Fenster -125..80 Grad (H_BLAU Runde 1, Bild-1-Hinterbein -85 Grad), L_MAX_HAUT 58 (Messblatt 2), L_MAX_FLEISCH 70,6
(EM30-Haut p95), RAMPE_FLEISCH (Messblatt-1-Paletten), G4_DUNKELROT, FLEISCH_BAUCH_L 34,65 (G4/EM30), L_STREU_ZIEL 14,9 (G4), Zahnfleisch-Gewicht
21,5 (G4-Maul a* p90, gemessen), G_AB 1,48, ZAHN/AUGE wie Runde 2.
PORT-WAHL mit Nutzerentscheid: Mehrheitsschwelle 0,5 (E14) fuer Fleisch und Hautseite; k = 2 Hautseiten (E8 "wenige Farben je Teil", E10).
Verfahrensparameter (Weg, nicht Ziel; im Code benannt): K_GRAPH 8, K_NAH/K_FERN 64/256, SPIEGEL_ABSTAND 2 Texelabstaende, Falten-Uebergang
1 Standardabweichung, L*-Stufen der Rampen, Saat 7, Abbruch 1 dE, Saettigung 10 %.
Entfallen gegenueber Runde 2: alle Kachelversaetze (`+2`, `+1` bei Haut), `kf_mal.ausschnitt` fuer die Haut, SIG_AB_FEIN, DAEMPF 0,8,
FLEISCH_ANTEIL/UMRING. Weiter benutzt (nur Kiefer/Krallen): `kf_mal.ausschnitt` (Lueckenfuellung sigma 2 / 4 Durchlaeufe) und die Kopienzahl
`+1` - Befund Zahlen-Pruefer r2 5.1/5.3 dort **nicht behoben** (Offen O6).

### Laufprotokoll (Scratchpad `formfarbe/r3/`)

| Lauf | Aenderung | Befund |
|---|---|---|
| v0 | 3D-Malen, Screenshot-Karte direkt | 18/29; Fleisch zu dunkel (B1 19,8), Schwanz-L* steigt zur Spitze (A11 4,4) |
| e1 | globaler Buntheitsfaktor ohne Grenze | g 3,9 -> Magenta/Orange/Tuerkis (verworfen) |
| v1-v2 | zwei Hautseiten, Rampen; Blau auf C* 8 statt b* -6 | Bild 1 allein: warme Seite grau -> beide Bilder |
| e2 | Eichung, g <= 1,48 | K1 6,6-6,9 (Grenze), Bauch-Sattel zu gross (80 % des verdeckten Rumpfs) |
| v3-v5 | Bauch ny > 0, dunkelrote Falten, Versatz nur auf Haut | A4 12 Grad -> Falten nur warme Seite; Kiefer-Rot 12 % -> Zahnfleisch |
| v6-v9 | Bauch-L* (51,5 zu blass, 34,65 satt), Fleisch-Koernung +-1 sigma | B1 22 -> 25, B6 1,62 -> 0,13 |
| v10 | Bauch = nie gesehene statt nicht gespiegelte Texel | B1 35,5, B2 Seite 6,3 |
| e4-e5 | Endeichung, Saettigung gesperrt | Fixpunkt, 28/33 |
| v11 | Screenshot-Farbwechsel zusaetzlich | K1/K2 unveraendert, B8 ROT -> verworfen |

## 3. Herkunft der Atlasfelder (`farbe/K5c_farbe/bau.json` 'herkunft')

| Feld (Atlas) | Herkunft |
|---|---|
| Koerper, Seiten 0-2 (x < 384, 46 Felder aus `J_dicker/bau.json`) und Kopf (384,0,128,104) | K5c-Koernung (H2) + Screenshot-Farbkarte Bild 1/2 (sichtbar / Spiegelpunkt / harmonisch) -> Hautseite warm/kuehl mit Rampe aus den Original-Hautpixeln; Form aus dem Hohlraum-Mass; dunkle Falten G4-Dunkelrot; Fleisch (Screenshot-Mehrheit + Rumpf-Unterseite der Meshes 0/7/10/21) in der Familien-Rotrampe; Zonenversatz aus der Eichung |
| Kiefer (384,106,128,72), Mesh 23 | Band: Screenshot Bild 2 (Zahnflecken mit) + Zonenversatz Unterkiefer; Maulinneres RE2-G4-Maul (10,44,60,40), 90 Grad gedreht, 5 Kopien, Zahnfleisch auf Fleischrampe |
| Zaehne (384,180,24,14) | gemalt: Bild-2-Zahnflecken (79,6/1,4/9,5), L* zum Grund x 0,56 |
| Krallen (410,180,100,40) | RE2-G4-Krallen (231,200,20,50), 8 Kopien |
| Augen (384,196,24,10) + 218 Texel Augenumfeld | gemalt: E24 (13,4/15,8/6,9) |

Mesh-Zuordnung (Ruhepose, x vorn, y unten): 0 Huefte, 1-6 Schwanz, 7 Wirbel, 10 Brust, 21 Hals, 22 Kopf, 23 Unterkiefer, 8/9/24-27 Hinterbeine,
11-20/28-31 Arme/Haende/Klauen.

## 4. Riegel Soll / Ist

`python generator/riegel_farbe.py farbe/K5c_farbe --gegenprobe K5c --gegenprobe-r1 farbe/K5c_farbe_r1 --gegenprobe-r2 farbe/K5c_farbe_r2 --json farbe/riegel_K5c_farbe.json`
(Ausgabe `farbe/riegel_K5c_farbe.txt`, EXIT 1). Neu in Runde 3: B6-B9 zu den Befunden r2; Grenzen = Familienwerte mit demselben Code
(`riegel_farbe.py --familie`: EM30 / G5 / G4 = B6 0,28 / 0,19 / 0,08 %, B7 13,2 / 5,8 / 9,4 Grad, B9 11,3 / 7,3 / 7,7 %).

| # | Soll | Runde 3 | K5c heute | Runde 2 |
|---|---|---|---|---|
| A1 Koerper C* | 9,5-21 | 13,9 GRUEN | 4,9 | 11,8 |
| A2 sigma_ab je Teil | >= 7,0 | 7,6 GRUEN | 4,0 | 13,1 |
| A3 Fleischrot | 8-40 % | 20,3 GRUEN | 0,5 | 22,4 |
| A4 Rot-Farbton | 15-50 Grad | 22 GRUEN | -10 | 26 |
| A5 Neutralweiss | <= 0,20 % | 0,00 GRUEN | 0,79 | 0,00 |
| A6 HF_fft Koerper/Kopf/Kiefer/Kralle | 0,04-0,26 | 0,090/0,167/0,073/0,065 GRUEN | Kiefer 0,461 | 0,091/0,153/0,047/0,064 |
| A7 HF_rest | <= 0,19 | max 0,082 GRUEN | 0,436 | 0,065 |
| A8 Kiefer Zahn/Rot/Dunkel | 2-20 / >= 20 / >= 20 % | 6,9/25,3/39,6 GRUEN | 77,1/0/0 | 6,1/95,7/39,2 |
| A9 Auge L*/C* | <= 30 | 13,1/18,0 GRUEN | 56,1/53,7 | 13,1/18,0 |
| A10 Texeldichte | 0,5-2,7 | 0,64-2,33 GRUEN | gleich | gleich |
| A11 Schwanzsprung | <= 4,0 | 0,1 GRUEN | 3,4 | 1,4 |
| A12 Farbkorn | <= 5,7 | 2,48 GRUEN | 2,63 | 4,54 |
| **K1 Bild 1 C*** | 7,9-11,1 | **6,1 ROT** | 6,3 | 6,5 |
| **K2 Bild 1 ab-Streuung** | >= 8,36 | **6,2 ROT** | 6,3 | 8,0 |
| K3 Bild 1 dE je Zone | <= 6 | max 4,9 (Hinterbein r (W)) GRUEN | 8,3 | 5,4 |
| **K4 Bild 1 Rotklasse** | 3,1-7,8 % | **2,8 ROT** | 2,5 | 6,2 |
| K5 Bild 1 Hell Ruecken / geclippt | <= 14,9 / <= 0,8 % | 11,7 / 0,3 GRUEN | 21,9/1,0 | 4,7/0,4 |
| K6 Bild 2 Figur dE / C* | <= 6 / >= 21,8 | 1,0 / 24,7 GRUEN | 9,9/16,7 | 1,5/23,8 |
| K7 Bild 2 Unterkiefer dE / a* | <= 6 / >= 21,2 | 1,6 / 23,3 GRUEN | 12,4/13,7 | 0,0/24,9 |
| K8 Bild 2 Oberkiefer dE | <= 6 | 0,3 GRUEN | 12,0 | 1,4 |
| K9 Bild 2 Klauen a* r/l | >= 18,1 / 16,7 | 25,7 / 28,8 GRUEN | 13,8/22,8 | 26,1/28,3 |
| K10 Bild 2 Augen L* max | <= 30 | 14,0 / 15,4 GRUEN | 51,5/36,2 | 14,3/15,9 |
| N1 neutral C* | >= 7,5 / 9,4 / 7,9 | 10,3 / 9,7 / 10,1 GRUEN | 4,6/5,0/4,0 | 10,2/8,8/6,9 |
| N2 neutral L* > 85 | <= 0,10 % | 0/0/0 GRUEN | 0,83/1,30/0,11 | 0/0/0 |
| B1 Rot erosionsfest | >= 32,1 % | 35,9 GRUEN | 21,5 | 28,0 |
| **B2 groesste Rotflaeche 3/4 vorn / Seite** | >= 3,2 % | **2,97** / 5,96 **ROT** | 1,19/0,61 | 3,65/2,45 |
| B3 Blau-Klasse | <= 0,10 % | 0,03/0,02/0,00 GRUEN | 3,9/5,0/2,4 | 0 |
| B4 Glanzrest p90 | <= 58 | 44,3 GRUEN | 100 | 47,8 |
| B5 Kopffeld Zahnklasse | <= 2,0 % | 0,0 GRUEN | 15,1 | 1,0 |
| B6 Kachelwiederholung (neu) | <= 0,28 % | 0,04 GRUEN | 0,24 | 2,82 |
| B7 Rot-Farbton-Streuung (neu) | >= 5,8 Grad | 11,3 GRUEN | 19,9 | 4,5 |
| B8 Bild-2-Zahnflecken Hell-Klasse (neu) | 13,6-33,9 % (0,6-1,5 x 22,6) | 14,1 GRUEN | 41,1 | 0,2 |
| **B9 Rot im Umrissband (neu)** | <= 11,3 % | **17,8 ROT** | 10,2 | 23,3 |

**Warum K1/K2 rot bleiben (gemessen):** Die Bild-1-Farbkraft des Originals (C* 9,25) ist zu grossen Teilen Blau. Mit der Blau-Grenze dieser Runde
(b* < -6 nur bis C* 8, statt Runde 2 b* auf -6) traegt das Original selbst je Bildpunkt nur C* 7,53; selbst eine bildpunktgenaue Quantisierung des
Originals auf die zwei Hautseiten erreicht nur C* 8,28 / ab-Streuung 7,59 (Messung im Lauf, Original T Bild 1, k-Mittel k = 2-8 ueber die Hautpixel beider Bilder). Nach Projektion, Glaettung und Kette bleiben
6,1 / 6,2. Mit 3-8 Toenen statt 2 steigt die Quantisierungsgrenze nicht (7,67-8,10). Runde 2 erreichte K2 8,0 nur mit G4-Foto-Korn (P2) - das ist
der Zielkonflikt K1/K2 gegen B3 (Sicht r1 P2 "blaue Beine") und P2 (Foto-Fragmente). Gegen die gelockerte Grenze aus O1 (Runde 2: 0,85 x 7,99 = 6,79)
ist K1 ebenfalls ROT.
**K4/B2 knapp:** Z1 (Rueckenfleck) erscheint in der Bild-1-Kette zu dunkel (Rotklasse 2,8 % gegen >= 3,1); 3/4 vorn verdeckt der nahe Arm den Bauch
(2,97 gegen 3,2). **B9:** im Umrissband liegen Schwanzspitze (Bild 1: rosa-weisses Ende), Krallen (G4) und die Bauchkante; EM30 11,3 %.

## 5. Tafeln (`farbe/`, `generator/kf_tafeln.py`, KF_NAME 'K5c neu (Farbe Runde 3)'; Messblatt 7: roh, kein Gamma auf keiner Kachel)

| Tafel | Inhalt |
|---|---|
| `01_szene_bild1.png` | Bild 1 Original - K4 heute - K5c neu; Zeile 1 Disc-Licht ROOM5080 Cut 2 x k (1,06/0,95/0,82), beide gleich; Zeile 2 durch form_video. Kopfzeile: Zeile 1 heller als das Spielfenster 07 (Sicht r2 3) |
| `02_szene_bild2.png` | dasselbe fuer ROOM5090 Cut 11 (k 1,20/1,12/1,00) |
| `03_familie.png` | EM30_HANGAR - RE2-G5 - RE2-G4 - K5c neu - K4 heute, gleicher Renderer, Licht 0,55 + 0,45 N.L, Umkugel 0,46, 3 Winkel, L*/C*/L*>85/px je Kachel |
| `04_atlas.png` | EM30 - K5c neu - K5c heute, 2 Bildpunkte je Texel, je 8 Palettenfelder mit Lab und Anteil |
| `05_rundum_neu.png` | K5c neu Pose Bild 1 (Clip 0) und Ruhe (Clip 2), 6 Winkel, Licht wie 03, EM30 als Vergleichskachel |
| `06_kopf_nah.png` | Bild-2-Ausschnitt x 200-360, y 290-420 3-fach (Original, T, neu Kette, heute Kette); Kopf+Kiefer neu / heute / RE2-G4-Kopf |
| `07_spiel.png` | echtes Spiel (eigene exe-Kopie, `generator/kf_spiel.sh --mit-heute` -> `re15_port/tools/form_spiel.sh`, gdigrab, kein AUTOSHOT/SOFTWARE_RENDER): Original - K4 heute - K5c neu, 5 Szenen (5080 Nachstellung, Lage Bild 1, Ruhe; 5090 Pose Bild 2, Ruhe). Einzelbilder `farbe/spiel/`, Lauf-Log `farbe/spiel_lauf.log` (10/10 rc=0, alle gdigrab Versuch 1). Runde-2-Bilder in `farbe/spiel_r2/` |

Gesehen (eigene Durchsicht, keine Pruefung): im neutralen Licht ein dunkles Tier mit blaugrauen Beinen, rotbraunem Ruecken/Kopf und einer
zusammenhaengenden roten Bauch-/Flankenflaeche (03, Seite) - naeher an G4 als Runde 2; im Spiel 5080 bleibt die Figur fast schwarz (Hangar-Licht),
rot sind dort Flanke und Schwanzspitze; in 5090 Cut 11 rotbraun im Feuerschein.

## 6. Tests

`bash re15_port/tools/local_build.sh build`: LOCAL-BUILD-OK. `ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3 --timeout 600`: **11/11 gruen**,
nichts wiederholt, 95,8 s. Die Tests pruefen den unveraenderten Bestand (Spiel-Asset), der Kandidat laeuft nur in 07.

## 7. Offen

| # | Punkt | Stand |
|---|---|---|
| O1 | **K1/K2 (Bild-1-Farbkraft) gegen Blau-Grenze B3 und P2**: mit der Blau-Grenze ist die Farbkraft des Originals selbst nur 7,5 (bildpunktgenau 8,3 bei zwei Toenen); erreichbar waere K1 nur mit blauer Beinhaut (Runde 1) oder Foto-Korn (Runde 2). Entscheid: K1/K2 auf das Original mit Blau-Grenze beziehen, oder Blau zulassen | Orchestrator/Nutzer |
| O2 | K4 2,8 % (Grenze 3,1) und B2 3/4 vorn 2,97 % (3,2): Z1 in der Bild-1-Kette zu dunkel, Bauch 3/4 vorn vom Arm verdeckt | knapp, naechster Bau |
| O3 | B9 17,8 % (EM30 11,3): Schwanzspitze, G4-Krallen und Bauchkante liegen im Umrissband. Neuer Riegel dieser Runde, eigene Grenze aus der Familie | naechster Bau / Pruefer |
| O4 | Blaugraue Beine (kuehle Hautseite, C* <= 8): Riegel B3 gruen, aber im neutralen Licht sichtbar blaugrau; Sicht r1 P2 hatte "stahlblaue Beine" als Ablehnungsgrund (dort C* > 8) | Sicht-Pruefer / Nutzer |
| O5 | 32143 Texel werden durch die Blau-Grenze gekappt; Hinterbein-b*-Versatz wirkt nicht (gesperrt, -51,4) | Folge von O1 |
| O6 | Kiefer/Krallen weiter aus `kf_mal.ausschnitt` (sigma 2, 4 Durchlaeufe) und Kopienzahl `+1` (Zahlen-Pruefer r2 5.1/5.3) | nicht behoben |
| O7 | Kopffeld: zwei helle senkrechte Baender (projizierte Bild-2-Hellstellen, L* <= 58) und kleine Rotsprenkel aus der Bild-2-Projektion | Sicht-Pruefer |
| O8 | Eichung lief ueber 4 Codestaende (15 Runden); Endstand ist Fixpunkt des Endcodes (2 Runden, Schritt 0,71), Start nicht neu von null | zweiter Pruefer |
| O9 | Herkunftstext 'hautseiten' in bau.json sagt "k-Mittel Bild 1", gerechnet wird ueber Bild 1 und 2 (Code `hautseiten()`) | Textfehler |
