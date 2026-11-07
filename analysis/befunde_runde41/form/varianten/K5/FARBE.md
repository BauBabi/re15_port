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


# Versuch 2, Runde 1 (Bauer, 2026-11-07)

Auftrag: Orchestrator, Spur Form, Versuch 2 (der letzte, Nutzer 2026-11-06). Grundlage `MESSBLATT_FARBE_V1.md` + Nachtrag `MESSBLATT_FARBE_V2.md`
(Binnenzeichnung B10-B17, Zielwerte K1/K2/K4/K4b/B2/B9, Konstanten-Vorschriften Abschnitt 4). Generator `generator/kf_mal4.py` (baut auf
`kf_mal3.py` auf), Eichstand `generator/kf_eich4.json`, Variante `farbe/K5c_farbe_v2/` (+ Kopie `farbe/K5C_FARBE_V2.EMD/.TIM`, Scratchpad
`formfarbe2/emd/K5C_FARBE_V2.*`). Runde 3 liegt jetzt in `farbe/K5c_farbe_r3/`, `farbe/spiel_r3/`, `farbe/riegel_K5c_farbe_r3.*`.
FORM.EMD = `K5c/FORM.EMD` (sha256 a71f8d08..., Geometrie, Clips, UV bytegleich, cmp), neu nur FORM.TIM (sha256 eca20f1d...). Nachbau
`python generator/kf_mal4.py bauen <aus>` byte-gleich (cmp). Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.*` und Pins unveraendert.
Kein RE-Wert: neues Modell (PORT-WAHL Form, E5/E19/E20); fuer Malkonstanten liegt kein Nutzer-Wortlaut vor, jede Konstante traegt eine Messquelle.

## V2.0 Ergebnis

**Riegel 38/45 GRUEN, EXIT 1** (`farbe/riegel_K5c_farbe_v2.txt/.json`; V1 A1-A12, K3-K10, N1/N2, B1-B9 + V2 K1v2, K2v2, K4b, B9b, B6 je Feld, B10-B17).
Alle fuenf roten r3-Riegel (K1, K2, K4, B2-Seite, B9) und alle acht Binnenzeichnungs-Riegel bis auf B12/B17 sind gruen. ROT, alle knapp ausser B6 Kiefer:

| Riegel | Soll | V2 | Abstand |
|---|---|---|---|
| K5 geclippte Bildpunkte Bild 1 (W Figur) | <= 0,75 % | 0,8 % | 0,05 |
| N1 neutraler Render C* Seite | >= 9,35 | 9,3 | 0,05 |
| B2 groesste Rotflaeche 3/4 vorn | >= 3,2 % | 3,11 % (Seite 3,67) | 0,09 |
| B6 Kachelwiederholung Koerper | <= 0,28 % | 0,29 % | 0,01 |
| B6 je Feld Kiefer (V2 4) | <= 0,28 % | 1,83 % | 1,55 |
| B12 Kopf Canny-Dichte | >= 25,2 | 24,5 (Linien 20,1 % gruen) | 0,7 |
| B17 Render-Kohaerenz (Mittel 3 Winkel) | >= 0,60 | 0,599 (Atlas 0,586 gruen) | 0,001 |

Gegenproben (EXIT-relevant, alle wie verlangt ROT): K5c heute (alle Farbriegel), Runde 1 (B1-B5), Runde 2 (B6-B9), **Runde 3 (alle V2-Riegel:
K1v2 6,1, K2v2 6,2, K4b 2,3, B2 2,88, B9 17,8, B10-B17)**; B9b (Clip 0) ist an r3 mit 9,9 % gruen, wie V2 3.2 angibt (Bezugsprobe). Familie mit
`n2_riegel.py --familie` (Pruefer-Lauf, `farbe_lesung/N2_riegel_familie.json`): EM30 8/8, G4 8/8, G5 7/7.
**Unabhaengigkeitsnachweis K_GRAPH / SPIEGEL_ABSTAND (V2 4) NICHT erbracht** (Abschnitt V2.4): bei 4/16 bzw. 1,0/4,0 kippen 10 Riegel.
Nach Messblatt V1 7.5 gehen die Tafeln deshalb nicht als Abnahmevorlage an den Nutzer.

## V2.1 Ursache in Zahlen (r3 -> V2)

| Mass | Familie | r3 | V2 |
|---|---|---|---|
| dunkle Linien Fleisch / Haut (B10/B11) | 9,5-19,3 / 3,9-8,3 % | 3,71 / 2,99 | 8,94 / 6,22 |
| Canny Fleisch / Kopf (B13/B12) | 29,6-30,2 / 29,6-30,1 | 19,05 / 8,85 | 32,85 / 24,5 |
| L*-Streuung Rotflaechen Render / Atlas (B14) | 9,6-11,2 / 10,9-14,5 | 5,4 / 8,6 | 10,1 / 12,1 |
| a*-Streuung Render (B15) | 5,0-8,7 | 3,1 | 6,2 |
| Rotpixel in Flaechen mit >= 2 Stufen, Render (B16) | 60-81 % | 2,1 | 81,0 |
| Kohaerenz Render / Atlas (B17) | 0,601-0,690 / 0,576-0,753 | 0,561 / 0,561 | 0,599 / 0,586 |
| Bild-1-Kette C* / ab-Streuung (K1v2/K2v2) | Rundlauf 7,8 / 8,0 | 6,1 / 6,2 | 6,7 / 8,2 |
| Rotklasse Figur / Ruecken (K4/K4b) | 3,15-7,87 / 7,1-17,9 % | 2,75 / 2,3 | 6,5 / 11,5 |
| Rot im Umrissband Clip 2 / Clip 0 (B9/B9b) | <= 11,3 % | 17,8 / 9,9 | 8,1 / 5,6 |

Gemessene Ursachen, die den Bau bestimmt haben (Laeufe im Scratchpad `formfarbe2/`):
1. **Die r3-Koernung (Wirbel) ist der inkohaerente Teil**: ohne Koernung Kohaerenz Atlas 0,747 / Render 0,635, mit r3-Koernung 0,570 / 0,563
   (Laeufe exp_*). Gezeichnete Linien allein heben B17 nicht. Laengs der Muskelachse gemittelt (Linienfaltung) wird die Koernung zu Straengen:
   Render 0,599-0,616 je nach Lauf. EM30 und G5 sind in Tafel 03 laengs gestreift.
2. **Linien zaehlen nur, wenn sie elongiert bleiben**: dichte Linien verschmelzen zu Netzen und fallen aus B10/B11 heraus (Lauf v1/v2: 4811-9910
   Faltenlinien, Anteil sinkt auf 3-5 %). Deshalb Abstand >= 2 Texel zwischen Linien (Halbbreite der 5x5-Schliessung des Top-Hat) und je1000-Obergrenze.
3. **Auf dunkler Haut gibt es keine dunkle Linie**: L* - Kontrast < 0; der fehlende Kontrast geht als Aufhellung in einen 2-Texel-Hof (Fuge zwischen Wuelsten).
4. **Rot im Umrissband (B9) sass an den Klauen und den Krallen** (Zuordnung durch Graufaerben der Mesh-Gruppen, Laeufe b9_*/c9_*: Arme 17,4 -> 12,4,
   Kopf+Krallen -> 11,0) und am Bauch, der in den Riegelwinkeln (Hoehe 8-20 Grad) nur am unteren Umriss sichtbar ist. Abhilfe: Screenshot-Fleisch
   nur auf gesehenen/gespiegelten Texeln, verdeckte Rumpf-Wunde an der Flanke.
5. **Die r3-Eichversaetze passen weiter**; der Fixpunkt der Zonenmittel senkte die Bild-1-Kette C* 6,7 -> 5,6 (Laeufe e1/e2, `kf_eich4.json`
   'laeufe_verworfen'), ohne g-Grenze stieg g 2,22 -> 3,09 ohne Wirkung (Kappen a* <= 11, C* kuehl <= 8).
6. **Glaettung und Lochfuellung legten 855 sichtbare Texel rot** (Lauf dbg: Huefte/Wirbel neben der verdeckten Rumpf-Wunde, K4b 27 %). E14 jetzt streng.

## V2.2 Verfahren (`generator/kf_mal4.py`)

Schritte 1-3 wie r3 (Koernung H2, Screenshot-Farbkarte, Hautseiten, Form), dann neu:
1. **Muskelachse je Mesh**: PCA der Ruhepose-Lage (Clip 2) der Texel, in die Tangentialebene projiziert und ueber die lokale Jacobi-Matrix
   (dP/dx, dP/dy aus den Atlas-Nachbarn im selben Mesh) nach (du, dv); Doppelwinkel im Mesh geglaettet (sigma SIG_GROB 2,5).
2. **Koernung laengs**: r3-Koernung entlang der Achse gemittelt (13,5 Texel), auf die r3-Streuung zurueckskaliert (Haut, Fleisch x 1, Kopf x c_k).
3. **Fleisch** = Screenshot-Mehrheit nur auf gesehenen/gespiegelten Texeln (1211) + Bild-1-Ruecken-Wunde (1025 Texel, gewachsen ab dem roetesten
   Original-Bildpunkt (104,8) bis 16,5 % der Zonen-Bildpunkte, Kern L* 45-55) + verdeckte Rumpf-Wunde (13 768 Texel, zusammenhaengend gewachsen,
   Prioritaet |ny| = Flanke vor Bauch) bis 23,9 % der Koerpertexel; Kante 2,5 Texel geglaettet, Inseln/Loecher < 20 Texel bereinigt, auf
   sichtbaren Texeln nur Screenshot-Fleisch (E14).
4. **Wundstufen**: geodaetischer Abstand zum Fleischrand auf dem Oberflaechengraph (Texel des Meshs): < 2 G4-Dunkelrot (18,9/25,6/12,2),
   2-4 G4-Fleischrot (32,8/43,7/25,8), Kern je Wunde haelftig EM30-Rot (36,5/37,4/20,5) / Familienwunde (51,5/26,5/18,0) nach der Form;
   Uebergang 0 Texel. Screenshot-Farbabweichung (glatt) wie r3. Farbton je Texel um k x Koernung/sigma gedreht (k 14,1 Grad).
5. **Haut**: Rampen der Seite x g 2,22, ohne Faltenfaerbung; a* <= 11 ausserhalb der Wunden, Blau-Klasse C* <= 8, L* <= 58.
6. **Linien**: Adern im Fleisch (laengs der Achse, quer, wo das Fleischband schmaler ist; G4-Dunkelrot-ab, Kontrast 13), Falten in der Haut
   (Saat in Hohlraeumen, quer zur Achse, Kontrast 10,5, Breite 1,4), Kopf laengs parallel (Breite 2,1, Kontrast 16,2, 1,22 je 1000 = EM30-Kopf);
   Dichte je Art per Halbierung auf das V2-Mass am Float-Atlas.
7. **Kiefer/Krallen**: je EINE G4-Kopie auf das Feld gestreckt (Maul bilinear, Krallen naechster Nachbar), Zahnfleisch a*/19,30; Zaehne, Augen,
   E9-Schwanz, Farbfenster wie r3.

## V2.3 Konstanten (Quelle je Zeile; Liste auch in `farbe/K5c_farbe_v2/bau.json` 'konstanten')

| Konstante | Wert | Quelle |
|---|---|---|
| K_NAH | 755 | pi x (31/2)^2, Linienabstand Fleisch Familien-Median G4 31 (V2 4 Zeile 1) |
| K_FERN je Mesh | 17-2061 | pi x (Gliedhalbbreite/Texelabstand)^2 aus PCA der Ruhepose (V2 4); Werte je Mesh in bau.json 'mesh_geometrie' (z. B. Huefte 24,6 Texel -> 1906, Schwanz 1 10,6 -> 352, Kopf 25,6 -> 2061) |
| L_STUFEN | 0 / 10,5 / 12,1 / 13,4 / 14,7 / 16,3 / 18,3 / 20 / 21,5 / 28,6 / 43,1 / 45 / 100 | Pflicht 20 und 45 (V1 2) + Dezile der Original-T-Helligkeit Bild 1+2, Nicht-Rot (V2 4); je Stufe >= 30 Bildpunkte |
| Zahnfleisch-Gewicht | 19,30 | a* p90 der genutzten G4-Texel (10,44,60,40) (V2 4) |
| Hautseiten-Schwelle | 0,414 | Warm-Anteil der Nicht-Rot-Figurpunkte Bild 1+2 (k-Mittel) = 0,537 -> Quantil der Texel-Warmanteile (V2 4); warm 53,9 % der Haut |
| Faltenuebergang 1 sigma | entfaellt | V2 4: Falten werden Linien |
| Fleisch-Koernung | Faktor 1,0 | V2 4 verlangt Atlas-B14 10,9-14,5: Halbierung auf den Familien-Median 11,2 ergibt 0 (Stufen allein 11,2), damit Render-B14 7,8 < 8,2; Faktor 1 = Hautkoernung (E8 "aus einem Guss"), Atlas 12,1 in der Spanne (Laeufe cf1/1,5/2,5: 12,1/12,8/13,7, B10 7,9/4,7/1,7) |
| Eichregeln | Abbruch 2 x 0,125; Sperre bei Aenderung > 2 x 0,125 und Wirkung < 0,125; g-Ziel Rundlauf 7,85, g <= 2,22 | Kettenstreuung 3 Saaten (15/12345/7) max 0,125 (kf_eich4.json); Fenster 0,95-1,05 entfaellt (V2 4); g-Grenze 1,5 x r3 (V2 3.2) |
| Linienlaenge Fleisch/Haut | Median 13,5 / p90 32 (Messlaenge) | Familienmitte V2 1 (11-16 / 21-43); gezeichnet / 1,155 (Messdefinition 4 sqrt(l1)) |
| Linienbreite / Kontrast Fleisch | 1,65 / 13 | V2 1: 1,5-1,8 / 11-15 |
| Breite / Kontrast Haut | 1,4 / 10,5 | V2 1: 1,3-1,5 / 9-12 |
| Kopflinien | 2,1 / 16,2 / 1,22 je 1000 / Laenge 15,6-42,9 | EM30-Kopf (V2 5.5, N2_messung 'EM30_kopf'); Laengen G5 (laengste Familie, V2 1) |
| Liniendichte-Ziel Fleisch / Haut | B10 12,65 / B11 5,35 % | Familien-Median EM30 (V2 1); Obergrenze je1000 3,45 / 3,50 = Familienmaximum |
| Richtungsabweichung je Linie | sigma 0,58 rad | R = exp(-2 sigma^2) = 0,51 = Familienmaximum Feldkohaerenz (V2 5.2) |
| Linienabstand | 2 Texel | Halbbreite der 5x5-Schliessung des Top-Hat-Masses (V2 1 (a)) |
| Wundrand / Innenrand | 2 / 4 Texel | V2 5.3 (Rand 1-2) = Aussenrand Mass (c); Kern = Erosion 2 Texel Mass (f) |
| Hell-Kern | 50 % je Wunde | G4-Leitwert (V2 1 (f), 5.3) |
| Mindestflaeche Wunde/Loch | 20 Texel | Rotflaeche-Definition V2 1 |
| Fleischanteil Koerper | 23,9 % | RE2-G4 Fleischrot (V1 1, Vorbild) |
| Ruecken-Wunde | 16,5 % der Zonen-Bildpunkte, Kern L* 45-55 | V2 3.2 K4b (15-18 %, L* 45-55) |
| Haut-Buntheit | g 2,22, a* <= 11 | V2 3.2 (1,5 x r3, V8-Kappe) |
| Kopf-Koernung | c_k 2,0 (Halbierung) | Ziel Canny 29,6 (EM30-Kopf, V2 1), Obergrenze HF_fft 0,258 = A6-Grenze 0,26 minus gemessene CLUT-Drift (Lauf v13 0,260 -> 0,261) |
| Koernungslaenge | 13,5 Texel | Linienlaenge Familienmitte (V2 1) |
| Farbton-Drehung | k 14,1 Grad/sigma | Halbierung auf Rot-Farbton-Streuung G4 9,4 (Familien-Median, V1 B7) |
| Saat Linien | 41 (SAAT_MAL) | fest und genannt (V2 4); CLUT-Saat 7 wie r3 |
| K_GRAPH / SPIEGEL_ABSTAND | 8 / 2,0 | Rechenparameter, **Nachweis nicht erbracht** (V2.4) |
| kf_mal.ausschnitt sigma 2 / 4 Durchlaeufe | unveraendert | V2 4: zulaessig bei A6-A8 gruen (gruen) und B6 je Feld <= 0,28 (Krallen 0,0 gruen, **Kiefer 1,83 rot**) |
| Kopienzahl +1 | entfaellt | je EINE Kopie (B6 Feld Krallen 85,4 -> 0,0 %, Kiefer 11,1 -> 1,83 %) |
Textfehler r3 berichtigt: Herkunft 'hautseiten' nennt jetzt "k-Mittel Bild 1 und 2"; das Farbfenster nennt -125..80 Grad (H_MIN) im Herkunftstext.

## V2.4 Unabhaengigkeitsnachweis (V2 4: K_GRAPH 4/16, SPIEGEL_ABSTAND 1,0/4,0)

Laeufe `formfarbe2/ind_*`, Riegel `riegel_ind_*.json`: 39/45, 40/45, 40/45, 42/45 gegen 38/45. Entscheidungen, die kippen (V2 / kg4 / kg16 / sp1 / sp4):
A6 Kopf 0,250 / **0,271** / 0,259 / 0,254 / 0,255; K5 W 0,8 / 1,1 / **0,7** / 0,9 / **0,7**; K1v2 6,7 / **6,6** / 7,0 / 6,8 / 6,7; N1 Seite 9,3 / 9,3 / 9,3 /
**9,4** / 9,2; B2 3/4 vorn 3,11 / **4,85** / **5,88** / **4,41** / **4,49**; B6 0,29 / **0,18** / 0,70 / 0,32 / 0,39; B8 15,9 / 20,1 / **9,9** / 17,2 / 17,1;
B10 8,94 / 10,57 / 8,20 / **7,82** / 9,05; B12 24,5 / **25,2** / **27,0** / **26,0** / **28,5**; B17 0,599 / 0,587 / 0,597 / 0,595 / **0,602**.
Ergebnis: K_GRAPH und SPIEGEL_ABSTAND sind nicht unabhaengig; die knappen Riegel (Abstand < Laufstreuung) kippen in beide Richtungen. Nach V2 4
sind beide Werte damit **nicht zulaessig belegt** (Offen V2-O2).

## V2.5 Eingeeichte Riegel mit zweiter Saat (V2 3.2 letzter Absatz)

Versaetze unveraendert aus r3 (`kf_eich3.json`): Schwanz (-16,5/5,0/-12,7), Ruecken (-4,0/8,0/-19,2), Hinterbein l (W) (-8,9/18,6/-31,6),
Hinterbein r (W) (-17,0/14,9/-51,4), Vorderglied r (W) (-6,9/-5,6/-5,9), Oberkiefer (-0,2/7,0/-7,5), Kopf/Rumpf (1,7/2,6/-9,2),
Unterkiefer (43,3/26,0/19,2), Klauen 0. **Ueber 6 dE: Schwanz 21,4, Ruecken 21,2, beide Hinterbeine 37,7/56,1, Vorderglied 10,7,
Oberkiefer 10,3, Kopf/Rumpf 9,7, Unterkiefer 54,0** - Begruendung: die Versaetze gleichen die Spiellicht-/Videokette (Kanalbalance, Blau-Verlust
an den Beinen, Messblatt V1 2) und die Kappen der Textur aus; b* der Beine ist gesperrt (Blau-Grenze), wirkt also nicht (V1 Offen O5 r3).
Saat 15 / 12345: K3 max 5,1 / 5,1 (Hinterbein r (W)); K6 dE 2,2 / 2,2, C* 23,1 / 23,1; K7 2,6 / 2,6, a* 22,4 / 22,4; K8 4,0 / 3,9;
K1v2 6,7 / 6,8; K2v2 8,2 / 8,2; K4 6,5 / 6,4; K4b 11,5 / 11,4.

## V2.6 Laufprotokoll (Scratchpad `formfarbe2/`, Auszug)

| Lauf | Aenderung | Befund |
|---|---|---|
| v0 | Adern/Falten/Kopflinien, Wundstufen, Ruecken-Wunde, g 2,22 | B10 5,8, B17 0,551; Wundstufen mit Lochmuster |
| v1-v3 | Fleisch bereinigt, Linien auf V2-Mass kalibriert, Hof, kein Kreuzen | Faltenlinien verschmelzen (9910 Linien, 5 %); Kopf "Leopard"-Striche |
| exp_* | Komponenten einzeln aus | Koernung = inkohaerent (B17 0,747 ohne) |
| exp_lic | Koernung laengs 6 / 13,5 | B17 Render 0,586 / 0,599, Haut wie Fell im Atlas |
| v4 | + Fleisch-Koernung 1,44 | B10-B17 8/8 gruen, aber K4 21 %, K4b 22 % |
| v5-v6 | Ruecken-Wunde gesamt statt Kern 16,5 % | K4b 21,9 -> Ursache Glaettung (dbg) |
| v7 | Maul/Krallen eine Kopie, Screenshot-Fleisch nur gesehen | B6 je Feld 0,0, B9 14,6 |
| kopf_* / v9-v12 | Kopf: Linien dicht, G4-Baender, Koernung | Linien allein -> HF 0,26+; Baender zaehlen nicht; Koernung + HF-Grenze traegt B12 |
| ny-* / v14-v16 | Rumpf-Wunde nach G4-Anteil, E14 streng | B9 9,3, K4b 13,8 |
| o_* | Reihenfolge Unterseite / Flanke / Oberseite | Render-B14 7,8 / 10,5 / 10,8 |
| v21 | Farbton-Drehung | B7 4,0 -> 10,6 |
| e1/e2 | Fixpunkt-Eichung | K1 6,7 -> 5,6, verworfen |
| v2final | Endstand | 38/45 |

## V2.7 Tafeln (`farbe/`, `generator/kf_tafeln.py`, KF_NAME 'K5c V2 (Versuch 2)'; Messblatt 7: roh, kein Gamma)

| Tafel | Inhalt |
|---|---|
| `01_szene_bild1.png` / `02_szene_bild2.png` | Original - K4 heute - V2, Zeile 1 Disc-Licht x k (beide gleich), Zeile 2 + Videofilter |
| `03_familie.png` | EM30 - G5 - G4 - V2 - K4 heute, gleicher Renderer/Licht (0,55 + 0,45 N.L), 3 Winkel; V2 L* 18,4/15,9/17,6, C* 10,7/9,3/12,6 |
| `04_atlas.png` | EM30 - V2 - K5c heute mit Paletten; darunter Kanten-/Richtungsbild (n2_mass) RE2-G4 und V2 |
| `05_rundum_neu.png` / `06_kopf_nah.png` | wie r3 |
| `07_spiel.png` | echtes Spiel (eigene exe-Kopie, `kf_spiel.sh --mit-heute`, gdigrab Versuch 1, 10/10 rc=0, `farbe/spiel_lauf.log`, Einzelbilder `farbe/spiel/`) |
| `08_binnenzeichnung.png` (neu) | Flanke / Schulter / Ruecken nah: RE2-G4 - EM30 - r3 - V2, gleiches Licht, gleiche Ausschnittregel (Figurrahmen) |

Gesehen (eigene Durchsicht, keine Pruefung): Fleisch an Flanke und Ruecken jetzt gestuft (dunkler Rand, Rot, rosa Kern) mit dunklen Adern,
Haut laengs gestreift; im Spiel 5080 eine rote Rueckenwunde. Schwaechen: Kopf laengs gestreift, im Atlas haarartig (warme Seite x 2,22 ist
ocker); Wunden im Nahbild pixelig-fleckig, G4-Adern sind klarer (Tafel 08); Ausschnitt "Schulter" trifft bei K5c den Kopf.

## V2.8 Tests

`RE15_BUILD_DIR=.../re15_port/build bash re15_port/tools/local_build.sh build`: LOCAL-BUILD-OK. `ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3
--timeout 600`: **11/11 gruen**, 95,6 s, nichts wiederholt (Tests pruefen das unveraenderte Spiel-Asset).

## V2.9 Offen

| # | Punkt |
|---|---|
| V2-O1 | 7 Riegel ROT (V2.0), sechs davon mit Abstand <= Laufstreuung (V2.4). B6 Kiefer 1,83 % (gestrecktes G4-Maul hat glatte Doppelbloecke) braucht ein gemaltes Maulinneres statt der Kopie |
| V2-O2 | Unabhaengigkeitsnachweis K_GRAPH/SPIEGEL_ABSTAND nicht erbracht (V2.4) |
| V2-O3 | Zielkonflikte gemessen: Laengs-Koernung traegt B17 und kostet B6 (Laenge 9: B6 0,18, B17 0,588); Fleisch-Koernung traegt Render-B14 und kostet B10; Kopf-Canny gegen HF_fft-Grenze |
| V2-O4 | Kopf wirkt im Atlas haarig (Sicht-Pruefung); Farbe der warmen Hautseite x 2,22 ocker |
| V2-O5 | Commit aee1a1b71 enthaelt bereits die Verschiebung r3 -> K5c_farbe_r3 (Nachricht nennt sie in 1984269ca); die Zahlen der Nachricht 1984269ca zu B7/B10/B11/B13-B16 stammen aus Lauf v20, gueltig sind die Werte in V2.1 / riegel_K5c_farbe_v2.txt |
| V2-O6 | Zweiter Pruefer rechnet Rundlauf (7,8/8,0) und K1v2/K2v2 nach (V2 6.1) |


# Versuch 2, Runde 2 (Bauer, 2026-11-07)

Auftrag: Orchestrator, Spur Form, Versuch 2 Runde 2 von 2. Grundlage wie Runde 1 (`MESSBLATT_FARBE_V1.md`, `MESSBLATT_FARBE_V2.md`) und die Pruefberichte
`FARBE_PRUEFUNG_v2_r1_sicht.md` (P2 Laengsstreifung = Fell/Holz, Klasse "Nutzer lehnt ab") und `FARBE_PRUEFUNG_v2_r1_zahlen.md` (38/45; K5, N1, B2, B6 Koerper,
B6 Kiefer, B12, B17 ROT; Unabhaengigkeitsnachweis fehlt; SIG_GROB/Mehrheitsschwelle ohne Tabellenzeile; FLEISCH_KOERNUNG gewaehlt; Textfehler; Tafel 08
ungleiche Anatomie; Commit-Trailer). Generator `generator/kf_mal4.py` (weitergefuehrt), Eichstand `generator/kf_eich4.json`, Variante `farbe/K5c_farbe_v2/`
(+ `farbe/K5C_FARBE_V2.EMD/.TIM`, Scratchpad `formfarbe2/emd/K5C_FARBE_V2.*`). Runde 1 liegt in `farbe/K5c_farbe_v2r1/`, `farbe/riegel_K5c_farbe_v2r1.*`,
`farbe/spiel_v2r1/`, `farbe/spiel_lauf_v2r1.log` (Scratchpad `formfarbe2/emd/v2r1/`).
FORM.EMD = `K5c/FORM.EMD` (sha256 a71f8d08..., cmp gleich), neu nur FORM.TIM (sha256 085bb433...). Nachbau `python generator/kf_mal4.py bauen <aus>` mit
frischem Cache bytegleich (cmp). Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.*`, Pins und `re15_port/` unveraendert.

## V2r2.0 Ergebnis

**Riegel 43/45 GRUEN, EXIT 1** (`farbe/riegel_K5c_farbe_v2.txt/.json`). Gruen geworden gegen Runde 1: K5, N1, B2, B6 Koerper, B6 Kiefer. ROT:

| Riegel | Soll | Runde 2 | Runde 1 | Grund |
|---|---|---|---|---|
| B12 Kopf Canny-Dichte / Linien | >= 25,2 / >= 4,6 % | 4,1 / 0,1 % | 24,5 / 20,1 % | Runde 1 erreichte den Wert nur ueber die Laengsstriche, die der Sicht-Pruefer als Fell ablehnt (P2). Kopflinien und Kopf-Koernung sind entfernt, V2r2.4 |
| B17 Kohaerenz Render (Mittel) / Atlas | >= 0,60 / >= 0,576 | 0,543 / 0,562 | 0,599 / 0,586 | ebenso: die Laengs-Koernung lieferte die kohaerenten Kanten. Ohne sie 0,54-0,57 in allen Laeufen, V2r2.4 |

Messblatt V2 6.3 sagt, B17 sei schwach (Familie/r3 nur 0,03-0,05 auseinander) und bei ROT allein entscheide die Sichtpruefung. B12 steht dort nicht.
Beide ROT sind also Befund, und nach V1 7.5 geht die Tafel damit nicht als Abnahmevorlage an den Nutzer. Was das kostet, steht in V2r2.4: Der Kopf
wuerde mit diesen Riegeln zu Rinde oder Zebra.

Die Gegenproben verhalten sich wie verlangt. K5c heute, Runde 1 (B1-B5), Runde 2 (B6-B9) und Runde 3 (alle V2-Riegel) sind ROT, und an Runde 3 ist
B9b mit 9,9 die Bezugsprobe. Die Familie (Pruefer-Lauf `farbe_lesung/N2_riegel_familie.json`) steht bei EM30 8/8, G4 8/8 und G5 7/7.

Ketten-Riegel mit drei Saaten des Videofilters (`generator/kf_saaten.py`, `farbe/saaten_K5c_farbe_v2.*`, V2 3.2 letzter Absatz) sind je 11/11 gruen. Werte fuer die Saaten 15 / 12345 / 7:

| Riegel | Werte |
|---|---|
| K1v2 | 7,0 / 7,0 / 6,9 |
| K2v2 | 8,4 / 8,4 / 8,4 |
| K3 max | Hinterbein r (W) 4,4 bei allen Saaten |
| K4 | 5,4 / 5,3 / 5,4 |
| K4b | 8,8 / 9,0 / 8,9 |
| K5 geclippt | 0,3 / 0,3 / 0,3 |
| K6 dE | 1,1 bei allen Saaten |
| K7 dE | 1,9 / 1,9 / 2,0 |
| K8 dE | 3,5 / 3,4 / 3,6 |

## V2r2.1 Ursache in Zahlen (Runde 1 -> Runde 2)

| Befund | Ursache (gemessen) | Runde 1 | Runde 2 |
|---|---|---|---|
| P2 Laengsstreifung Haut/Kopf | Laengs-Koernung (r3-Struktur, laengs 13,5 Texel gemittelt) auf Haut und Kopf, Kopflinien laengs 1,22 je 1000, Kopf-Koernung x 2,0 | dunkle duenne Linien im Kopf-Render 14,9 / 18,0 / 15,3 % (elong. 6,6 / 6,6 / 6,2) | 9,7 / 7,5 / 12,0 % (2,2 / 2,3 / 4,0); G4 7,9 / 5,5 / 18,9, K5c heute 10,2 / 9,0 / 11,3 (`farbe/sicht_mass_v2r2.txt`) |
| K5 geclippt | Schwanzspitze vor der Bahnsteiglampe (Bildpunkte x 260-270, y 430-450, Albedo L* 45): Kette Schwanz L* 21,8 gegen Original 17,4 | 0,8 % | 0,3 % (L*-Eichung) |
| K4 (nach Wegfall der Streifen) | ohne die dunklen Laengsstriche besteht die warme Haut (a* <= 11) die Ketten-Rotklasse; Haut grau gesetzt: 0,2 % Rot, Fleisch grau: 6,1 % (Laeufe d_gh/d_gf) | (6,5) | 5,4 % (L*-Eichung) |
| B6 Koerper | glatte Hautverlaeufe werden von der CLUT in gleiche Stufen zerlegt (Doppelgaenger 1,41 RMS, Lauf m4), ohne Korn 9,7 % (x_kh0) | 0,29 % | 0,14 % (Feinkorn) |
| B6 Kiefer | G4-Maul um 90 Grad gedreht, gestreckt 3,2 x 1,2 | 1,83 % | 0,00 % (Lage, 2,1 x 1,8, naechster Nachbar + Feinkorn) |
| N1 Seite | Fleischanteil nach der Bereinigung 22,0 % statt 23,9 % | 9,3 | 9,5 (Fleisch 24,1 %) |
| B2 3/4 vorn | wie N1, dazu konzentrische Wundbaender | 3,11 | 4,90 |
| P1 Fleckenmosaik | Hell-Kern je Texel nach Form und Screenshot, Farbton je Texel nach der Laengs-Koernung gedreht | B16 81,0 | B16 80,7, B14 9,4 / 11,8, B15 6,5 (konzentrisch, Farbton glatt) |

## V2r2.2 Verfahren (Aenderungen gegen V2.2)

1. **Haut, Kopf, Kiefer: Feinkorn statt Laengs-Koernung.**
   - Das Korn ist der Rest der K5c-eigenen Struktur (H2) gegen eine Glaettung entlang der Oberflaeche mit sigma 1 Texel.
   - Es wird auf die Streuung KORN_SD 2,30 L* gebracht. Das ist das RE2-G4-Feinkorn, neu gemessen mit derselben Definition an der Haut der Familie: EM30 3,53 / RE2-G4 2,30 / RE2-G5 3,02 (`generator/kf_korn_mass.py`, `farbe_lesung/N2_korn.json`).
   - Das Ergebnis liegt bei 3,05 und damit in der Familie; r3 lag bei 3,26.
   - Kopflinien und Kopf-Koernung entfallen.
2. **Fleisch.** Die Laengs-Koernung (Fasern) wird nur noch mit c_f aus der Halbierung auf den Familien-Median B14-Atlas 11,2 aufgetragen. Das Ergebnis ist c_f = 0, denn die Stufen allein ergeben 11,2.
3. **Wunden konzentrisch.**
   - Baender nach dem geodaetischen Abstand zum Rand: < 2 Texel G4-Dunkelrot, < 4 Texel G4-Fleischrot, Kern EM30-Rot. Die innere Haelfte des Kerns je Wunde ist Familienwunde (Hell-Kern 50 %, G4-Leitwert).
   - Die Farbton-Drehung folgt der geglaetteten r3-Struktur (SIG_GROB) statt jedem einzelnen Texel; k = 15,3 Grad je sigma, Farbton-Streuung 9,41 bei Ziel 9,4.
4. **Fleischanteil.** Nach der Bereinigung wird nachgewachsen, bis FLEISCH_ANTEIL 23,9 % erreicht ist (hoechstens 4 Durchgaenge). Ergebnis 24,1 %: Rumpf 14 946, Screenshot 1 211, Ruecken-Wunde 1 043 Texel.
5. **Ruecken-Wunde.** RUECKEN_ZIEL steht auf 18 %, der Obergrenze von V2 3.2. Bei 16,5 % lag K4b auf der Untergrenze (7,1 gegen 7,14, Lauf f3), bei 18 % auf 8,8.
6. **Maul.**
   - Eine G4-Kopie in Lage, also nicht gedreht, gestreckt mit dem naechsten Nachbarn, dazu das Feinkorn.
   - Bilinear ergab B6 Kiefer 0,0, aber HF_fft Kiefer 0,030 < 0,04 (Laeufe m1/m4/m5).
   - Mit naechstem Nachbarn plus Korn ergibt sich HF 0,063-0,067 bei allen K_GRAPH und B6 Kiefer 0,0.
7. **Eichung `eichen --nur-l`.**
   - Nur der L*-Anteil der Zonenversaetze wird nachgefuehrt, a*/b* bleiben auf dem r3-Stand.
   - Abbruch nach V2 4, wenn der Schritt kleiner als 2 x Kettenstreuung (0,125) ist. Erreicht in Runde 3 mit 0,10 (`formfarbe2/eichL2.log`); g bleibt 2,22.
   - Der volle Lab-Schritt (Lauf e3, `formfarbe2/kf_eich4_lauf_e3.json`) trieb b* in die Blau-Grenze (Ruecken -21,9 -> -25,5, Hinterbein r -55,5) und senkte K1 7,3 -> 6,3, ohne in 3 Runden abzubrechen.
   - Versaetze L* neu: Schwanz -21,3, Ruecken -0,8, Hinterbein l -7,0, Hinterbein r -22,7, Vorderglied -8,1, Oberkiefer 1,6, Kopf/Rumpf 5,8, Unterkiefer 41,9.
   - Ueber 6 dE bleiben dieselben Zonen wie in V2.5. Sie gleichen die Kette und die Blau-Kappe aus; b* der Beine wirkt nicht (gesperrt durch C* <= 8).
8. **Tafel 08.** Der Ausschnitt ist je Modell auf dieselbe Anatomie gesetzt (`kf_tafeln.ANAT`, abgelesen am Figurrahmen mit Viertelraster): Rumpfseite, Schultergelenk, Rueckenmitte.

## V2r2.3 Konstanten (neu oder geaendert; uebrige wie V2.3)

| Konstante | Wert | Quelle |
|---|---|---|
| KORN_SIG / KORN_SD | 1,0 Texel / 2,30 L* | Messgroesse und RE2-G4-Wert aus `kf_korn_mass.py` (Familie 2,30-3,53); G4 = Vorbild (V1 1) |
| c_f (Fleisch-Koernung) | 0 | Halbierung auf B14-Atlas = Familien-Median 11,2 (V2 4), nicht mehr gewaehlt |
| Kopflinien, Kopf-Koernung, HF_MAX, JE1000_KOPF, KOPF_BREITE/KONTRAST, ZIEL_KOPF_CANNY | entfallen | Sicht-Pruefer v2 r1 P2 |
| SIG_GROB (neu verwendet: Muskelachse, Wundkante, Farbton-Drehung) | 2,5 Texel | `kf_mal.py:40-42`: PORT-WAHL nach E12 (Nutzer: "Screenshot-Farben nur grossflaechig je Bereich, geglaettet entlang der Glieder") und G4-Atlas Feld (130,96,20,62), halbe Breite einer Adergruppe |
| Mehrheitsschwelle Wundkante | 0,5 | Definition: geglaettete Fleisch-Indikatorfunktion >= 1/2 bedeutet mehr Fleisch- als Hautgewicht in der Nachbarschaft (Mehrheitsregel, keine Wahl) |
| HELLKERN_ANTEIL | 0,50 nach Abstand | G4-Leitwert Hell-Kern 50 % (V2 1 (f)) |
| RUECKEN_ZIEL | 0,18 | V2 3.2 "15-18 %" (Obergrenze, Begruendung V2r2.2 Nr. 5) |
| Eichung L* | Abbruch 2 x 0,125 | V2 4 (Kettenstreuung `kf_eich4.json`) |
| ANAT (Tafel 08) | je Modell | Sichtwahl derselben Anatomie (kein Malwert) |

Textfehler im Kopf von `kf_mal4.py` sind berichtigt: KOPF_QUER faellt weg, g ist als "Startwert 1,48 x 1,5 = 2,22 (= Obergrenze)" beschrieben.

## V2r2.4 Was B12 und B17 kosten wuerden (Laeufe, Scratchpad `formfarbe2/`)

| Lauf | Aenderung | B12 Canny | B17 Render / Atlas | gesehen |
|---|---|---|---|---|
| w0 | Runde-1-Stand | 24,5 | 0,599 / 0,586 | Kopf haarig (P2) |
| x_kh0 | ohne Haut-Koernung (Kopf-Koernung laengs bleibt) | 26,2 | 0,572 / 0,584 | B6 9,7 % |
| q1 / r1 | Kopf-Koernung quer (EM30-/G4-Kopf: Kanten quer zur Feldachse 74,8 Grad / 75 % > 60 Grad) | 29,7 | 0,585 | Kopf wie Rinde (`t_r1/06c.png`) |
| p1-p8 | Kopf als gestufte Platten | 19,3-25,9 | 0,574-0,594 / 0,586 | erst bei Stufen 48 L* (c_k 16 x 3) ueber 25,2; Zebra (`t_p5/06c.png`) |
| b1-b4, c1-c4 | Kopf glatt; Haut: Feinkorn 1,5-2,3 / Straenge laengs (gestuft und ungestuft) / Fleischfasern x 1-3 | 3,5-4,5 | 0,541-0,559 / 0,537-0,579 | Kopf glatt dunkel (wie Endstand) |
| t1-t4 | Kopf quer gestreift; Haut: Feinkorn 1,2-2,3 | 28,9-29,8 | 0,560-0,569 / 0,576-0,597 | Kopf wie Rinde |
| a_oa/a_of/a_ok | Adern / Falten / Kopflinien einzeln aus | — | 0,576-0,588 | — |
| g_* | Krallen, Kiefer, Kopf, Fleisch, Haut grau | — | +0,002 / 0,000 / +0,008 / +0,009 / -0,043 | die Haut-Laengsstreifen trugen die Kohaerenz |

Ergebnis:
- B12 >= 25,2 verlangt eine Kante je etwa 4 Texel ueber den ganzen Kopf.
- Das gibt es nur als Laengsstriche (Fell), Querrippen (Rinde) oder Platten mit 48 L* Stufe (Zebra). Alle drei widersprechen Bild 2 (dunkle glatte Masse), E16 und E22.
- B17 >= 0,60 erreichten nur die Laufe mit Laengs-Koernung auf der Haut.

## V2r2.5 Unabhaengigkeitsnachweis (V2 4), Endstand, `farbe/unabh_v2r2/`

| Riegel | Endstand (K_GRAPH 8, SPIEGEL 2,0) | K_GRAPH 4 | K_GRAPH 16 | SPIEGEL 1,0 | SPIEGEL 4,0 |
|---|---|---|---|---|---|
| Summe | 43/45 | 40/45 | 41/45 | **43/45 (keine Entscheidung kippt)** | 41/45 |
| K4 | 5,4 | **3,1** | 6,0 | 5,2 | 4,2 |
| K1v2 | 7,0 | **6,6** | 7,6 | 7,0 | 6,9 |
| K4b | 8,8 | **3,8** | 11,6 | 10,7 | 8,3 |
| N1 Seite | 9,5 | 9,5 | 9,6 | 9,7 | **9,2** |
| B2 Seite | 3,95 | 3,46 | 4,69 | 4,62 | **3,18** |
| B8 | 17,7 | 20,5 | **10,0** | 17,4 | 16,2 |
| B10 | 8,81 | 9,01 | **7,0** | 9,52 | 9,04 |

Ergebnis:
- SPIEGEL_ABSTAND 1,0: bestanden. 4,0: N1/B2 kippen um 0,2 / 0,02.
- K_GRAPH: nicht bestanden.
- Messung `generator/kf_kgraph_mass.py` (`farbe_lesung/N2_kgraph.json`):

| k | deckt Atlas-8-Nachbarschaft | Zusammenhangsteile | Querkanten |
|---|---|---|---|
| 4 | 43 % | 109 | — |
| 8 | 80 % | 31 | 19 % |
| 16 | 95 % | — | 43 % |

  - Bei k = 4 zerfaellt die Ruecken-Wunde (K4b 3,8).
  - Bei k = 16 verbinden 43 % der Kanten Texel ueber Mesh- bzw. Inselgrenzen.
- Die Werte haengen am Graphen selbst. Nach V2 4 ist K_GRAPH damit **nicht zulaessig belegt** (Offen V2r2-O2).

## V2r2.6 Tafeln (`farbe/`, Messblatt 7: roh, kein Gamma, gleiche Behandlung)

| Tafel | Inhalt |
|---|---|
| `01_szene_bild1.png` / `02_szene_bild2.png` | Original - K4 heute - V2 R2, Zeile 1 Disc-Licht x k (beide gleich), Zeile 2 + Videofilter |
| `03_familie.png` | EM30 - G5 - G4 - V2 R2 - K4 heute. V2 R2: L* 19,1 / 16,3 / 18,3, C* 11,0 / 9,5 / 13,4 |
| `04_atlas.png` | EM30 - V2 R2 - K5c heute mit Paletten, Kanten-/Richtungsbild G4 und V2 R2 |
| `05_rundum_neu.png` | wie bisher |
| `06_kopf_nah.png` | wie bisher. Kopf glatt dunkelbraun mit Feinkorn, Augenwuelste, Kieferband hell |
| `07_spiel.png` | `kf_spiel.sh --mit-heute`, eigene exe-Kopie, gdigrab mit freigestelltem Fenster, 10/10 rc=0 (`farbe/spiel_lauf.log`, `farbe/spiel/`) |
| `08_binnenzeichnung.png` | Ausschnitt je Modell auf derselben Anatomie |

Eigene Durchsicht (keine Pruefung):
- Kopf und Ruecken sind nicht mehr gestreift.
- Die Wunden an Flanke und Ruecken sind gestuft (dunkler Rand, Rot, rosa Kern), haben aber weiterhin Polygonkanten und Texelstufen (P13).
- Im Spiel 5080 wirkt die Figur roter als K4 heute. Der Chain-Riegel K4 steht auf 5,4 % bei 5,2 % im Original.

## V2r2.7 Tests

- Bau: `RE15_BUILD_DIR=.../re15_port/build bash re15_port/tools/local_build.sh build` meldet LOCAL-BUILD-OK (no work to do).
- Tests: `ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3 --timeout 600` ist **11/11 gruen** in 95,1 s, nichts wiederholt (`formfarbe2/ctest_r2.log`).
- Die Tests pruefen das unveraenderte Spiel-Asset.

## V2r2.8 Offen

| # | Punkt |
|---|---|
| V2r2-O1 | B12 und B17 ROT, beide im gemessenen Zielkonflikt mit P2 (V2r2.4). Ob B12/B17 oder die Sicht gilt, entscheidet Pruefer/Nutzer; die Tafeln gehen nach V1 7.5 nicht als Abnahmevorlage raus |
| V2r2-O2 | Unabhaengigkeitsnachweis K_GRAPH nicht erbracht (K_GRAPH 4: K4/K1/K4b, 16: B8/B10); SPIEGEL_ABSTAND 1,0 bestanden, 4,0 nicht (N1, B2) |
| V2r2-O3 | Wundkanten folgen weiter Polygon- und Texelstufen (P13), Pfoten hell (P12) |
| V2r2-O4 | Commit-Trailer: gesetzt ist "Claude Opus 5.5" gemaess der Attributionsvorgabe der Sitzung; der Auftragstext verlangt "Claude Fable 5.1" (Formfehler aus Runde 1 bleibt damit bestehen) |


# Versuch 3 (G4-Uebertragung), Runde 1 (Bauer, 2026-11-07)

Grundlage: `MESSBLATT_FARBE_V3.md` (Pruefer). Nutzer 2026-11-07: "schwarz rot ... weil er verbrannt ist", "das Gesicht vorne irgendwie in
flammenfarben leuchtet", "feuer Farbe beim Mund". Kein RE-Wert: K5c ist ein neues Modell (PORT-WAHL "eher c", Nutzer 2026-11-06).
Spiel-Asset `BAHNSTEIG_FORM` und Pins sind unveraendert. K4 bleibt fuer v0.8.30.

## V3.0 Ergebnis

- **Variante:**
  - `farbe/K5c_farbe_v3/` (FORM.EMD, FORM.TIM, form.json, bau.json).
  - Kopien `farbe/K5C_FARBE_V3.EMD` (sha256 6f4d5fb0...) und `farbe/K5C_FARBE_V3.TIM` (sha256 47725110...), dazu Scratchpad `formfarbe3/emd/`.
- **Riegel** `generator/riegel_farbe_v3.py`: **33 von 52 Zeilen GRUEN, Exit 1** (`farbe/riegel_K5c_farbe_v3.txt`, `.json`).
  - Gegenprobe Versuch 2 Runde 2: H1, P1, K11, K14 und K15 sind dort ROT, wie verlangt.
  - G4 selbst ist bei P1 GRUEN (17,3 % / 26,0 % / 422 Farben).
- **Neu und gruen:**
  - G0: Geometrie bytegleich.
  - H1: 99,85-100 % G4-Texel je Teil. H1b: 1,13 %.
  - H2: Zeilen 0-2 bytegleich, 0 Vertauschungen in den Zeilen 3-8.
  - P1: 21,0 % / 26,2 % / 401 Farben. S1: <= 0,15 %.
  - F1t: dE 0,2. Zaehne: L* 76,9, C* 5,2.
  - K11: -5,0 / -7,9 Grad. K14: dE 5,1, Rotklasse 74,5 %.
  - Dazu K4, K4b, K5, K6, K9, K10, A2-A12, B10, B13-B16.
- **Spiel:** 10 von 10 Laeufen mit rc 0 (`farbe/spiel_lauf_v3.log`). Die Form laedt mit 9 CLUT-Zeilen ("32 Meshes, 32 Knochen, 3 Clips -> Slot 12").
- **Tests:** Form-Gruppe `ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3`: 11 von 11 gruen.

## V3.1 Verfahren (`generator/kf_g4.py`)

1. **TIM:** Die Seiten 0-2 sind die G4-Indizes, bytegleich (CDEMD0.EMS Typ 0x34, TIM @0x65e800). Die CLUT-Zeilen 0-2 sind G4, bytegleich.
   Seite 3 traegt die Sonderfelder.
2. **UV je Flaeche (b2, Messblatt 2.2):**
   - Ein Zylinderrahmen je Teil, ueber alle Meshes in Ruhelage (Versatz aus dem Gerippe).
   - Rumpf, Hals und Kopf haben die feste Achse x. Grund: Bei der K5c-Brust kippte die PCA-Achse auf (0,04 / 0,02 / -1,00). Lauf 0 legte
     dadurch die Flanken auf den Ruecken (`formfarbe3/bau/t6.py`).
   - Die G4-Winkelreferenz ist die K5c-Referenz, zurueckgedreht ueber die kleinste Drehung von der G4-Achse auf die K5c-Achse.
   - Der Schwerpunkt der K5c-Flaeche liegt in einem G4-Dreieck (theta, T). Dessen affine Abbildung gilt fuer alle Ecken.
   - Ausgeschlossene Felder (F3, F7, F8, F9, F16) werden in theta und T verdichtet. Die Naht liegt dort, wo die Luecke war.
3. **Einpassen (S1):**
   - Ein UV-Dreieck muss in der Insel liegen. Der 1-Texel-Malrand ist erlaubt (gemessen zu 80 % bemalt), hoechstens 1 % fremde Texel.
   - Reihenfolge: zuerst am Ort mit Groesse 1,0 / 0,85 / 0,7 / 0,55. Dann in voller Groesse auf eines der 300 naechsten Inseltexel verschoben.
     Erst danach wird es kleiner.
   - Vorher wurde auf einen Punkt geschrumpft. Das ergab 139 einfarbige Flaechen am Rumpf, sichtbar als flache Bloecke an Schulter und Hals (Lauf 1).
     Jetzt sind es 10.
4. **Zeilen 3-5, Bild-1-Aderrot:**
   - Rote Eintraege (Rampe C* 10..20, Farbton -40..70 Grad) werden auf -3,2 Grad gedreht, L* bleibt fest.
     Quelle: Messblatt 4, "1 Figur", Rot Lab 39,1/21,7/-1,2.
   - Buntheit Seite 1 (Schwanz, Vorderglied r, Zehen): mal 0,574. Quelle: Messblatt 4, "1 Schwanz", C* 23,6 gegen G4 41,1.
   - Seiten 0 und 2 behalten die G4-Buntheit. Grund aus zwei Laeufen:
     - Lauf 1 (alle Seiten mal 0,53): K1v2 5,7, K2v2 5,7, K4b 5,2 %, alle ROT.
     - Lauf 2 (alle Seiten mal 1,0): K12 Schwanz 17,1 %, ROT.
   - Die Bild-1-Zeilen tragen Rumpf 0/7, Schwanz, Hinterbeine und Vorderbein r 16-18 (Messblatt 4, "Bild-1-sichtbare Teile").
5. **Sonderfelder** (Zeile, Quelle, Ziel, Feldmittel):
   - **F1-Stelle (Zeile 6):**
     - Flaechen: Becken-Vierecke qf 7/8/19/20/31/32 (Messblatt 1.2).
     - Fleck: die Pixel der Maske F1 im Knochenraum (Leser B `fid1.npz`), plus 1 Texel.
     - Fleisch: G4-F16-Strahlen ohne Iris (288,211, 23 x 13). Haut: G4-F11, gespiegelt.
     - Rand: im aeussersten Ring bleibt nur die hellere Haelfte der Fleisch-Texel.
     - Die Haut-Eintraege bleiben. Das Fleisch-L* wird erst oberhalb von L* 28,66 (hellste Haut) gestreckt; so haelt die Rangfolge (H2).
     - Feldmittel Lab 48,6/21,6/-3,0, Ziel 48,8/21,6/-3,0.
   - **Maul-Band (Zeile 7):**
     - 27 Flaechen von 22/23, die in Bild 2 zu >= 30 % im Band liegen.
     - Quelle G4-F7, Loecher gefuellt.
     - Bauziel RGB (183,99,92) = Lab 51,5/33,1/19,2 (siehe V3.3).
   - **Zaehne (Zeile 8):** K5c-Zahnfeld auf G4-F8 (quer gelegt). Feldmittel L* 76,9, C* 5,2.
   - **Augen:** K5c-Augenfeld auf ein dunkles Rechteck in F10 (91,64). A9: L* 7,3.
6. **Kopf aussen:** aus F18, ohne Wundoval (G4 16 Insel 72).
   - Mit F4/F5/F6 (Lauf 2): K13 31,2 % und K6 dE 9,1, beide ROT. Mit F18: 46,2 % und dE 4,7.
   - Das Wundoval lag am rechten Auge (K10 L* 35,1 > 30). Ohne Oval: 11,7.

## V3.2 Konstanten (Quelle je Zeile; Liste auch in `farbe/K5c_farbe_v3/bau.json`, Schluessel konstanten)

| Konstante | Wert | Quelle |
|---|---|---|
| G4_KIND, TIM | 0x34, @0x65e800 | Messblatt 1.1, Leser A 1 |
| CLUT_Y, Zeile je Seite | 480; Seite p -> Zeile p | FE.tim_lesen beider TIM; 1427 von 1427 G4-Flaechen |
| KAPPE_NA | 0,8 | Leser A uebertrag.py |
| T_FEIN | 48 | PORT-WAHL Bauer (4 je Mitte-Abschnitt) |
| SCHRUMPF, VERSCHIEBE_N | 1,0..0; 300 | PORT-WAHL Bauer, Lauf 1 -> 2 (flache Bloecke) |
| B1_ROT_LAB, G4_ROT_LAB | (39,1/21,7/-1,2), (38,0/35,5/20,8) | Messblatt 4 |
| B1_C_JE_SEITE | 1,0 / 0,574 / 1,0 | Messblatt 4 "1 Schwanz"; Laeufe 1 und 2 (V3.1 Punkt 4) |
| Rot-Rampe | C* 10..20, Farbton -40..70 Grad | V1-A3-Klasse als Rampe, PORT-WAHL Bauer |
| ZIEL_F1, ZIEL_BAND, ZIEL_ZAHN | (149,103,122), (184,90,85), (199,191,183) | Messblatt 3.1, 3.2, 5 (F1t/F2t) |
| ZIEL_BAND_BAU | (183,99,92) | PORT-WAHL Bauer nach K15/K7 (V3.3) |
| BAND_SCHWELLE | 0,3 | PORT-WAHL Bauer; Schwelle 0,2/0,3/0,5 ergibt 43/39/34 Flaechen |
| FLECK_WEITE, WEICH_RAND | 1, 1 | K14-Rotklasse 51,9 % -> 74,5 %; Auftrag "weiche Uebergaenge" |

## V3.3 Rote Riegel (`farbe/riegel_K5c_farbe_v3.txt`)

- **D1** (<= 2,0):
  - ROT: Rumpf 2,11, Schwanz 2,33, Kopf/Kiefer 2,77.
  - GRUEN: Beine 1,03, Arme 1,60, Hals 1,64, Klauen 1,07.
  - Wirksame Vergroesserung (genutzte Texel, nur Info): Rumpf 2,73, Schwanz 3,45, Kopf/Kiefer 4,38.
- **P2** (<= G4 + 8 Punkte):
  - ROT: Rumpf +14,6, Hals +10,5, Vorderbein r +12,7, Klauen +12,4/+12,6, Kopf +13,1.
  - GRUEN: Schwanz +2,6, Hinterbeine -3,9/-2,4.
- **F2t:** dE 6,18 > 6. Das Bauziel liegt bei dE 5,1. Der Unterschied kommt aus der Gewichtung: Der Riegel mittelt ueber die Texel einzeln,
  der Bau gewichtet nach der Abtastzahl.
- **K15 und K7:**
  - K15: dE 7,9 (Lab 49,9/35,2/19,7 gegen 49,9/28,2/16,1). K7: dE 10,2 (a* 31,6 gegen 24,9).
  - Mit ZIEL_BAND selbst ist die Kette noch roter (K15 dE 11,8).
  - Am Knochen 23 ergibt die Albedo (181,88,86) in der Kette a* 39,8, im Original a* 33,8.
  - Die 258 Bandpixel ohne K5c (Hintergrund) liegen bei a* 46 gegen 33.
  - Zwischen F2t und K15 bleibt so kein Spielraum (offen).
- **K12:** Schwanz 12,4 % > 8,9 %. Mit dem Schrumpf-Einpassen waren es 7,5 %; das Verschieben zeigt mehr Adern.
- **K13 und K8**, beide knapp: K13 46,2 % gegen 46,4 %, K8 6,1 gegen 6.
- **K3:** Hinterbein r dE 10,5, Hinterbein l 7,5, Vorderglied 7,9, Ruecken 6,6.
  - Es fehlt b*: Hinterbein r im Original Lab 23,2/0,3/-8,3, in der Variante 26,0/0,2/2,2.
  - Die blaeuliche Haut des Originals hat keine G4-Hautzeile. Die Bild-1-Zeilen drehen nur das Aderrot (H2).
- **K1v2, K2v2, A1:** K1v2 6,6 < 6,6 (knapp), K2v2 6,4 < 6,8, A1 8,5 < 9,5. Die dunklen G4-Glieder haben C* 2-7 (F10, F11, F13, F17, F19).
- **N1, N2:**
  - N1: C* 3/4 vorn 6,9 < 9,4.
  - N2: L* > 85 bei 0,12-0,41 %, an den Krallenspitzen G4-F21 (L* p95 97,4) und an den Zaehnen.
- **B-Riegel:** B2 2,5/2,33 < 3,2; B9 15,5 > 11,3; B11 2,42 < 3,3; B12 Canny 19,9 < 25,2; B17 Render 0,588 < 0,60.

GRUEN: G0, H1, H1b, H2, P1, S1, F1t, Zaehne, A2-A12 (V3-Felder), B10, B13-B16, K4, K4b, K5, K6, K9, K10, K11, K14, B9b.

## V3.4 Tafeln (`farbe/`, `generator/kf_tafeln_v3.py`, Messblatt 6; die Tafeln von Versuch 2 liegen in `farbe/v2r2/`)

- `01_szene_bild1.png`, `02_szene_bild2.png`: Original | K4 heute | V3, Spiellicht roh und durch form_video, dazu je eine Nahkachel der Feuerstelle.
- `03_familie.png`: EM30 | G5 | G4 | V3 | K4 heute, roh, gleiches Licht.
- `04_atlas.png`: G4-Atlas | V3 (G4-Zeilen und Sonderfelder) | V3 (Bild-1-Zeilen) | Herkunftskarte in Falschfarbe mit Legende.
- `05_rundum_neu.png`.
- `06_kopf_nah.png`: Bild 2 und Bild 1 nah, dazu der Kopf neben dem G4-Kopf, neutral.
- `07_spiel.png`: eigene exe-Kopie, Bilder in `spiel_v3/`.
- `08_binnenzeichnung.png`: G4 | EM30 | Versuch 2 | V3.

Gesehen (Bauer, keine Abnahme):
- Der Koerper wirkt schwarz mit roten Adern. Die F1-Stelle ist ein rosa-mauve Fleck.
- Am Hals und an der Schulteroberkante bleiben einzelne flache rote bzw. pfirsichfarbene Flaechen.
- Der Kopf ist dunkel. Die Adern dort sind grob, weil der Kopf 2,8-fach vergroessert ist.

## V3.5 Offen

1. **D1/P2 an Rumpf und Kopf:** Die Quellflaechen sind zu klein (Kopf aus F18: 15,8 Mio. gegen etwa 2 Mio. Einheiten^2).
   Moeglicher Weg: Kopf auf F18, F17 und F19 aufteilen (Schaedel, Schnauze, Kiefer je eigenes Feld), Rumpf-Brust zusaetzlich aus F11.
2. **K15 gegen F2t:** Fuer die K5c-Kette widersprechen sich beide um etwa 10 a*. Rueckfrage an den Pruefer, welcher Riegel gilt.
3. **K3 b*** (blaeuliche Haut in Bild 1): Laut H2 duerfen die Bild-1-Zeilen nur das Aderrot aendern. Soll eine Bild-1-Hautzeile erlaubt werden?
4. **N2:** Die G4-Krallenspitzen sind heller als L* 85, und G4 selbst hat sie so. Sollen sie ohne eigene Zeile bleiben?
5. **Messblatt 7.1** (Gesicht oder Becken in Bild 1) bleibt eine Rueckfrage an den Nutzer.


# Versuch 3 (G4-Uebertragung), Runde 2 (Bauer, 2026-11-07)

Grundlage: `MESSBLATT_FARBE_V3.md` und die Pruefberichte der Runde 1 (`FARBE_PRUEFUNG_v3_r1_zahlen.md`, `FARBE_PRUEFUNG_v3_r1_sicht.md`).
Der Stand der Runde 1 liegt in `farbe/K5c_farbe_v3r1/` und `farbe/v3r1/` (Tafeln, Riegel-Ausgabe, Spielbilder).
Spiel-Asset `BAHNSTEIG_FORM` und Pins sind unveraendert. K4 bleibt fuer v0.8.30.

## V3R2.0 Ergebnis

- **Variante:** `farbe/K5c_farbe_v3/` (FORM.EMD, FORM.TIM, form.json, bau.json, herkunft.npz).
  - Kopien: `farbe/K5C_FARBE_V3.EMD` (sha256 32b48eee...) und `farbe/K5C_FARBE_V3.TIM` (sha256 2bddb092...).
  - Dazu Scratchpad `formfarbe3/emd/`, bytegleich.
- **Riegel** `generator/riegel_farbe_v3.py`: **36 von 52 Zeilen GRUEN, Exit 1** (`farbe/riegel_K5c_farbe_v3.txt`, `.json`). Runde 1: 33 von 52.
- **Gegenproben:** Alle 13 fallen so aus, wie das Messblatt verlangt ("falsch ausgefallen: keine").
  - V2r2: H1, P1, K11, K14 und K15 ROT.
  - G4 selbst: P1 GRUEN und B10-B17 8/8 GRUEN.
  - (b2)-Probe A: K11 +24,8 Grad, K12 Schwanz 21,3 %, K13 17,2 % und P2 Kopf aus F1 +11,2 Punkte, alle ROT.
  - Orange und Gelb als Feldfarbe: F1t/F2t dE 74,6/50,1 bzw. 87,3/72,4, beide ROT.
- **Spiel:** 10 von 10 Laeufen mit rc 0 (`farbe/spiel_lauf_v3.log`, Bilder `farbe/spiel_v3/`). Die Form laedt mit "32 Meshes, 32 Knochen, 3 Clips".
- **Tests:** Form-Gruppe `ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3`: 11 von 11 gruen, 96,9 s.

## V3R2.1 Befunde der Runde 1 und was geaendert ist

| Befund (Pruefer) | Aenderung | Beleg Runde 2 |
|---|---|---|
| Zahlen A: Flaechen ohne Texelmittelpunkt fielen aus H1/S1/F1t/F2t; UV als Punkt/Linie (Hals 10,8 %) | Bau und Riegel tasten wie die PSX ab: `kf_g4.texel_psx`, 861 baryzentrische Punkte je Dreieck, floor (Verfahren des Zahlen-Pruefers). Auch die G4-Inselmasken sind so gerastert. Neue Info-Zeile S1b: Flaechen mit Punkt/Linie, die bei G4-Dichte >= 1 Texel^2 decken. | S1 0,00 % in allen Teilen. S1b: Hals 0 %, Kopf 0 %, Schwanz 1,35 %, Beine 1,57 % (Runde 1 laut Pruefer 10,8 / 8,6 / 3,7 / 6,6 %) |
| Zahlen D1: Kopf ortsgenau Median 4,00, Rumpf 1,61 | **Texeldichte wie die Quelle** (`projektion`): theta n_th-mal wiederholt (n_th = round(Umfang K5c / gedeckter Umfang Quelle)), laengs gespiegelt gekachelt (s_T = Laenge K5c / gedeckte Laenge). | D1 (abgebildete G4-Flaeche mit Wiederholung) / Median ortsgenau: Rumpf 1,16/1,20, Schwanz 1,20/1,27, Hals 1,05/1,12, Beine 1,03/1,14, Arme 1,21/1,24, Klauen 1,10/1,31, Kopf 1,25/1,41 |
| Zahlen/Sicht P2 > +8 an Rumpf, Hals, Vorderbein r, Klauen, Kopf; Riegel mass Kopf gegen G4 10 | Wie D1. P2 misst jedes Teil gegen seine tatsaechliche Quelle (Kopf, Brust, Hals gegen G4 16; Rumpf hinten gegen G4 0/9) | Kopf +6,8 (Runde 1 +13,1), Hals +3,9, Vorderbein r +3,4, Rumpf +3,5. **ROT bleiben:** Brust +10,2, Klauen l/r +9,1/+8,7 |
| Sicht: flache rote Keile, Strahlenmuster vorn an der Brust | **Kappen** (Flaechen mit \|N.a\| > 0,8 zur Teilachse, dieselbe Grenze KAPPE_NA wie fuer die Quell-Kappen) bekommen je Ende eine gemeinsame Ebene quer zur Achse in Quelldichte | Brust vorn: dunkle Scheibe statt Keilen (`05_rundum_neu.png`, vorn) |
| Sicht: lachsfarbene Polster an Schultern/Brust | Brust 10 und Hals 21 aus F18 (wie der Kopf). Abweichung von Messblatt 2.3 (Brust <- G4 10 F4/F5/F6, Hals <- F4/F5), begruendet mit der Bild-2-Kette (Messblatt 4: "Massstab fuer die Haut ist die Kette"): Mit F4/F5/F6 war Knochen 10 in Bild 2 zu 76,7 % Rotklasse und zu 19,1 % dunkel. Die Zone Kopf/Rumpf lag bei 38,5 % rot und 58,6 % dunkel, das Original bei 11,0 % und 73,1 %. Oberarm r ohne das rosa Wundoval G4 16 Insel 72 | Zone Kopf/Rumpf jetzt 8,9 % rot, 90,4 % dunkel (`r2/zone_knochen.py`). K13 66,6 % (Runde 1 46,2 %). K6 dE 4,8, K8 dE 3,3 |
| Sicht: Vorderklaue als Tarnmuster | Haende (F20) und Krallen (F21) laengs genau einmal (`laengs_einmal`). Mit Spiegelkachel wiederholten sich die hellen Spitzen als Querbaender | Krallen: dunkelroter Ansatz, eine helle Spitze (`05`, `02`). Die Krallen bleiben im Bild-2-Vordergrund hell, so hell wie F21 im G4. P2 Klauen ROT, siehe oben |
| Sicht: Kopf schwarze Kuppel mit verschmierten Rotflecken; roter Maulkorb | Kopf in G4-Dichte (F18, gekachelt): gemalte Adern statt gedehnter Texel. Band nur noch Flaechen mit Mehrheit (> 50 %) ihrer Bild-2-Pixel im Band: 25 statt 27 Flaechen | `06_kopf_nah.png`: dunkle Haut mit roten Aderlinien. Neue Vergleichsspalte "RE2-G4 Mesh 16 = Quelle F18" |
| Sicht F1: 977 px (1,51-fach), gelappt | Fleck-Kern = Maskenpunkte selbst (FLECK_WEITE entfaellt). Saum = 1 Texelring, darin nur die hellere Haelfte des Fleischs | K14 dE 3,9 (Runde 1 5,1), Rotklasse 82,2 % |
| Zahlen F1t: Feld zu 70 % Haut | **Herkunftskarte** `herkunft.npz`: Die Haut der F1-Flaechen ist eine G4-Kopie aus F11, lueckenlos, Quelle je Texel festgehalten. Das zaehlt in H1 als G4-Texel ("Index bytegleich ... an der kopierten Stelle"). Das Sonderfeld sind die Fleck-Texel (Art 2). F1t mittelt ueber **alle** Fleck-Texel, auch die mit Haut-Eintrag | F1-Flaechen lesen 620 Texel: 442 G4-Haut, 178 Fleisch. F1t ueber die 178: Lab (48,66/21,83/-3,22), dE 0,34 |
| Zahlen F2t: Zaehlweise; ZIEL_BAND_BAU ohne Nutzer-Wortlaut | Band auf ZIEL_BAND (Messblatt 3.2) gebaut. Bau und Riegel zaehlen jedes gelesene Texel einmal | F2t dE 0,22 (554 Texel). **K15 und K7 ROT** (V3R2.3) |
| Zahlen H2: Zeile 7 31 Vertauschungen, Zeile 8 2 | Rang-Reparatur bis zum Stillstand. Die genutzten Eintraege stammen aus der PSX-Abtastung | 0 Vertauschungen in den Zeilen 3-8 |
| Zahlen: Gegenproben nicht als Lauf | `gegenproben_weitere`: G4 B10-B17, (b2)-Probe A (K11/K12/K13/P2 Kopf), Orange/Gelb (F1t/F2t) | siehe V3R2.0 |
| Zahlen: Konstanten ohne Quelle | VERSCHIEBE_N, SCHRUMPF, ZIEL_BAND_BAU, FLECK_WEITE und BAND_SCHWELLE 0,3 entfallen. Die Rot-Rampe kommt aus den Klassen der Messblaetter. Veraltete Kommentarzahlen sind berichtigt | Tabelle V3R2.2 |
| Sicht: Vorderglied r in Bild 1 (Runde 2 neu gemessen) | Durch die Dichte kamen mehr Adern ins Bild. Die Handflammen lagen auch auf dem Handruecken: 9,8 % gegen die Grenze 3,0 %. Mit "laengs einmal" sitzen die Flammen an den Fingern | K12 Vorderglied r 1,8 % |

## V3R2.2 Konstanten (neu oder geaendert in Runde 2)

| Konstante | Wert | Quelle |
|---|---|---|
| Abtastung `_BARY_N` | 40 (861 Punkte je Dreieck) | Verfahren des Zahlen-Pruefers (FARBE_PRUEFUNG_v3_r1_zahlen.md, Kopf) |
| ROT_C0, ROT_C1, ROT_H0/H1 | 15, 20, -40/70 Grad | C0 = K11-Rotklasse "C* >= 15" (Messblatt V3 5), C1 und Farbtonfenster = A3-Klasse (Messblatt V1 6 / Leser A 1). Runde 1: C0 10 als Bauer-Wahl |
| n_th, s_T | je Teil gemessen, Werte in `bau.json` je Teil | Umfang und Laenge K5c / Quelle (Rahmen-Messung) |
| `laengs_einmal` (Haende, Krallen) | s_T = 1 | Sicht-Befund "Tarnmuster"; Messung K12 Vorderglied r 9,8 % -> 2,1 % |
| KAPPE_NA auch fuer K5c-Kappen | 0,8 | Leser A uebertrag.py (dieselbe Grenze wie fuer die Quell-Kappen) |
| BAND_MEHRHEIT | > 0,5 | Mehrheitsregel der Bildpixel (Definition), Maske band2_pruef (Messblatt 3.2) |
| WEICH_RAND | 1 Texelring | PORT-WAHL mit Nutzer-Wortlaut E10 ("die offenen Wunden sehen so richtig fleischig aus ... Bei uns wirkt alles wie billig einfarbig bemalt") |
| B1_C_JE_SEITE | 1,0 / 0,574 / 1,0 | Messblatt V3 2.3 Schwanz (Auflage K12: "Adern-CLUT nach H2") und Messblatt V3 4 "1 Schwanz" Rot C* 23,6 / G4 41,1; Seiten 0/2 mit G4-Buntheit |
| Brust/Hals <- F18 | G4 16 ohne Insel 72 | Bild-2-Kette (Tabelle V3R2.1). Messblatt 2.3 Kopf: "dunkle Hautfelder F18" |

## V3R2.3 Rote Riegel und warum

- **K15 / K7 (Band, Bild 2):** K15 dE 12,2 (a\* 39,2 gegen 28,2), K7 dE 15,3.
  - Gebaut ist auf ZIEL_BAND, F2t dE 0,22. In Runde 1 brachte ein Bauziel mit dE 5,1 zu ZIEL_BAND K15 nur auf 7,9.
  - Innerhalb der F2t-Toleranz erreicht K15 also kein Gruen.
  - Dazu kommen 258 der 2227 Bandpixel, die K5c nicht deckt (Hintergrund, Feuer: a\* 46 gegen 33).
  - Das Messblatt muss entscheiden, welcher der beiden Riegel gilt (wie V3.5 Punkt 2).
- **P2:** Brust +10,2 und Klauen l/r +9,1/+8,7.
  - Brust: Die F18-Inseln sind schmal. Ein Teil der Brustflaechen passt nur verkleinert hinein (`bau.json` Brust "kleiner").
  - Klauen: Durch "laengs einmal" ist die Kralle laengs 3,9- bis 5,1-fach vergroessert.
  - Abwaegung: Querbaender (Sicht-Befund) gegen Posterisierung (P2 +8,7/+9,1). Gewaehlt ist die Form der G4-Kralle (dunkelroter Ansatz, eine helle Spitze).
- **A1 7,8 / N1 / K1v2 6,5 (Grenze 6,63) / K2v2 6,2:** Die Haut ist duenkler und weniger bunt als die V1-Familie.
  - Das folgt aus G1 "schwarz rot" und der Bild-2-Kette. Die Zone Kopf/Rumpf ist jetzt mit 90,4 % dunkler als das Original mit 73,1 %.
  - Rot gegen dunkel ist dort 8,9 gegen 11,0 %.
- **K3** (Bild 1, Hinterbein r dE 10,5):
  - Es fehlt die blaeuliche Haut des Originals (b\* -8,3).
  - Eine Bild-1-Hautzeile laesst H2 nicht zu (wie V3.5 Punkt 3).
- **N2 0,27 / 0,02 / 0,30 %:** Krallenspitzen F21 und Zaehne. G4 selbst liegt bei 0,09-0,21 % (V3.5 Punkt 4).
- **B10 6,9 / B11 2,61 / B12 2,62;17,2 / B17 0,534:**
  - Binnenzeichnung aus F18, F11 und F10 ist duenner liniert als die Familienspanne.
  - G4 selbst ist 8/8 GRUEN (Gegenprobe). Die Auswahl der dunklen Felder senkt die Liniendichte.
- **B2 2,59 / B9 15,8 / B9b 12,1:** Rot sitzt als Aderlinien am Umriss, nicht als grosse Rotflaeche. Das ist G4-Machart, die V2-Grenzen stammen aus der Malzeit.

## V3R2.4 Tafeln (`farbe/`, Messblatt 6; Runde 1 in `farbe/v3r1/`)

- 01 und 02: Original | K4 heute | V3 Runde 2, Spiellicht roh und durch form_video, je mit Nahkachel.
- 03: Familie EM30 | G5 | G4 | V3 | K4, roh, gleiches Licht.
- 04: Atlas G4 | V3 mit Herkunftskarte.
- 05: rundum.
- 06: Kopf nah, mit G4-Kopf 26 und neu der Hautquelle G4 16 (F18).
- 07: Spiel, eigene exe-Kopie, 5080/5090.
- 08: Binnenzeichnung.

Gesehen (Bauer, keine Abnahme):
- Rumpf vorn, Hals und Kopf sind schwarz mit roten Aderlinien, ohne lachsfarbene Flaechen.
- Der Schwanz ist schwarz mit magentafarbenen Adern (Bild-1-Zeilen).
- Die Krallen haben einen dunkelroten Ansatz und eine helle Spitze.
- Das Band ist eine lachsrote Lippe mit Zahnreihe.
- Im Spiel (07) ist die Form in 5080 schwarz, in 5090 schwarz mit rotem Schimmer.

## V3R2.5 Offen

1. K15 gegen F2t (V3R2.3): Entscheid des Pruefers noetig.
2. Brust aus F18 weicht von Messblatt 2.3 ab (begruendet mit der Bild-2-Kette, V3R2.1). Der Pruefer bestaetigt oder verwirft.
3. P2 Brust und Klauen knapp ueber der Grenze. Wege fuer Runde 3:
   - Brust aus F18 plus F19 (breitere Inseln);
   - Krallen laengs zweiteilig: Ansatz gekachelt, Spitze einmal.
4. Die Wiederholung von F18 ueber Kopf, Hals und Brust (Kachelung, G4-Machart) ist in 05/06 nicht als Muster zu erkennen. Das sollte ein Sicht-Pruefer bestaetigen.
5. Messblatt 7.1 (Gesicht oder Becken in Bild 1) bleibt eine Rueckfrage an den Nutzer.


# Versuch 3 (G4-Uebertragung), Runde 3 (Bauer, 2026-11-07)

Grundlage: `MESSBLATT_FARBE_V3.md` und die Pruefberichte der Runde 2 (`FARBE_PRUEFUNG_v3_r2_zahlen.md`, `FARBE_PRUEFUNG_v3_r2_sicht.md`).
Der Stand der Runde 2 liegt in `farbe/K5c_farbe_v3r2/` und `farbe/v3r2/`. Spiel-Asset `BAHNSTEIG_FORM` und Pins sind unveraendert, K4 bleibt fuer v0.8.30.
Messskripte und Zwischenlaeufe dieser Runde: `farbe_lesung/V3B_r3_skripte/` (Kopie aus dem Scratchpad `formfarbe3/r3/`).

## V3R3.0 Ergebnis

- **Variante:** `farbe/K5c_farbe_v3/` (FORM.EMD sha256 d19fe901..., FORM.TIM abd0ba1b...). Kopien `farbe/K5C_FARBE_V3.EMD/.TIM` und
  Scratchpad `formfarbe3/emd/`, bytegleich.
- **Riegel** `generator/riegel_farbe_v3.py`: **32 von 52 Zeilen GRUEN, Exit 1** (`farbe/riegel_K5c_farbe_v3.txt`, `.json`). Runde 2: 36 von 52.
  Die Zahl sinkt, weil zwei Zeilen jetzt nach dem Wortlaut bzw. nach dem Auge gerechnet werden (A8, F2t) und die Krallen anders aussehen (B13, B16).
- **Gegenproben:** alle 13 wie verlangt ("falsch ausgefallen: keine"). Die Orange/Gelb-Probe faerbt jetzt nur die Feld-Eintraege (Zahlen-Pruefer (4)):
  F1t/F2t dE 74,6/50,1 und 87,3/72,4, ROT.
- **Spiel:** 10 von 10 Bildern (`farbe/spiel_lauf_v3.log`, `farbe/spiel_v3/`). Zwei K4-Aufnahmen waren beim ersten Lauf schwarz (rc 1) und sind
  einzeln wiederholt (rc 0). Die Form laedt mit "32 Meshes, 32 Knochen, 3 Clips". Aufnahmeweg: PrintWindow (Tafel 07 nennt ihn jetzt).
- **Tests:** Form-Gruppe `ctest -R "form|bahnsteig|birkin_form|r41_form" -j 3`: 11 von 11 gruen, 95,1 s (`V3B_r3_skripte/ctest.log`).

## V3R3.1 Befunde der Runde 2 und was geaendert ist

| Befund (Pruefer) | Aenderung | Beleg Runde 3 |
|---|---|---|
| Sicht: Vorderklaue Bild 2 als Mosaik aus beige-weiss-braunen Dreiecken, gefiltert Schachbrett | **`kf_g4.krallen_uv`**: je K5c-Kralle EIN G4-Krallenstreifen, durchgehend. Gemessen: alle 8 Krallen-Meshes sind Sechseckringe (Ecke 6r+j, dann Wurzelmitte und Spitze). Die G4-Kralleninseln 14/7, 14/15, 19/7, 19/15 liegen auf einem Streifen u 229-240, v 200-254 (Seite 1); v gross = Spitze (Abstand zum Knochen 282-458 gegen 87-250). Seiten abwechselnd an den Streifenrand (gespiegelt, ohne Naht). Ansatz = Zeilen 202-213 (Zeilenmittel in der Rotklasse A3, Zeile 214 a\* 12,7), Ring fuer Ring gespiegelt; die letzten k Glieder tragen 213-254 einmal, k so, dass die Vergroesserung dort der G4-Dichte am naechsten ist (0,78-1,21) | Bild 2: dunkelrote Krallen mit heller Spitze, ohne Mosaik (`02`, Z.1/Z.2). S1 0,00 %, keine Flaeche verkleinert. K9 Klauen a\* 31,4 / 30,6 (Original 24,1 / 22,2, GRUEN). N2 0,0 % in allen Winkeln (Runde 2: 0,27/0,02/0,30 %, ROT): Der helle rechte Streifen (L\* p90 98-100) wird nicht mehr benutzt |
| Sicht: Original "weiche weisse Flecken oben" | keine | Die hellen, geclippten Flecken liegen nach Messblatt V3 3.2/7.3 auf keinem K5c-Koerper (5 von 969 px). Kein Malziel, offen wie im Messblatt |
| Sicht F1: 941 px (1,46-fach), drei dunkle Streifen, harte Kante | Kern = Texel, die die 646 Maskenpunkte mehrheitlich decken (volle Deckung = Median 5,0 Punkte je Innentexel), eingeschlossene Loecher gefuellt. Rand = 1 Texelring aussen mit der dunkleren Haelfte des Fleischs (Messblatt V3 3.1 "Ein dunkler Rand zur Haut ist G4-Machart und erlaubt"). Fleischquelle: Lage in F16 ohne Iris (Seite 2) mit den wenigsten Texeln, die nicht heller als die hellste Haut (L\* 28,66) sind (17 Texel). Bauziel auf den Kerntexeln | F1-Flaeche (Regel des Sicht-Pruefers) **731 px (1,13-fach)**, IoU 0,75 mit der Maske (Runde 2: 941 px, 0,59). Ein Streifen mit schwacher Innenzeichnung (`01` Z.3). K14 (47,9 / 17,5 / -1,2), dE 3,1, Rotklasse 61,6 %. Zwischenlaeufe: Ring innen 447 px; Ring aussen ohne Kerngewicht K14 dE 9,3 |
| Sicht/Zahlen: Band in der Kette zu rot (K15 dE 12,2), Zahlen-Pruefer Befund A | Das Band wird auf das Ketten-Ziel **ZIEL_BAND_KETTE (182,117,102)** gebaut, Lab (55,7 / 24,2 / 18,8). Messung `band_knochen.py`: An den 1282 Bandpixeln auf Knochen 23 gibt die Kette das Texel fast unveraendert wieder (Ketten-T / Texel 1,017 / 0,963 / 1,030); der Rundlauf des Originals liegt dort bei T (185,5 / 112,7 / 105,4). Das Messblatt-Ziel (184,90,85) ergibt dort (49,2 / 40,2 / 19,5) gegen den Rundlauf (55,2 / 27,9 / 16,4) | **K15 (50,9 / 30,7 / 20,6), dE 5,2, GRUEN**. Knochen 23 jetzt (53,8 / 30,0 / 19,1). K7 dE 15,3 -> 9,6 (ROT). **F2t dE 15,1, ROT** (bewusst, V3R3.3) |
| Sicht: rosa Strichel an Flaechengrenzen von Schulter/Brust | Herkunft gemessen (`strichel.py`): rosa-magenta Texel (Rotklasse, Farbton < 10 Grad) gibt es nur auf Mesh 16 in den Bild-1-Zeilen, 7,1 % im Inselinneren und 3,8 % am Inselrand. Es sind die umgefaerbten G4-Adern, kein Randtexel einer Sonderzeile. Darum tragen Oberarm 16 und Unterarm 17 die Bild-1-Zeilen nur noch auf den Flaechen, die Bild 1 zeigt (39 bzw. 37 Flaechen, Leser B `fid1.npz`) | Rosa Texel auf Mesh 16: 1978 -> 495 (nur noch auf Bild-1-Flaechen). K11/K12 unveraendert gruen. Die Hand 18 behaelt die Bild-1-Zeilen auf allen Flaechen: mit Teilung K15 dE 6,0 (ROT) und K6 7,5 (Lauf `V3B_r3_skripte/r_v13_offiziell.txt`) |
| Zahlen Befund B: A8 nur durch Umdefinition gruen | Riegel nach dem Wortlaut: Kieferfeld = Unterkiefer aussen + Band, ohne Zaehne | **A8 Zahn 2,0 %** (gerundet; ROT, knapp unter 2), Runde 2 nach Wortlaut 0,9 %. Rot 36,3 %, Dunkel 67,5 % |
| Zahlen Befund C: K12 Schwanz ohne Reserve (Saat 7 ROT) | Buntheit Seite 1 geeicht auf das K12-Original 5,9 %: Laeufe 0,35 / 0,45 / 0,515 / 0,535 / 0,574 ergeben 1,3 / 2,8 / 5,3 / 6,1 / 8,7 % (Saat 15). Gewaehlt 0,535 (`B1_C_SEITE1_EICH`) | **K12 Schwanz 6,1 / 6,5 / 6,2 %** mit Saat 15 / 12345 / 7 (Grenze 8,9) |
| Zahlen (5): MORPH_CLOSE ohne Quelle, WEICH_RAND schwach | beide entfallen; ersetzt durch Mehrheitsregel und Messblatt 3.1 (siehe F1) | `bau.json` sonder.F1: kern_texel 122, rand_texel 70, volle_deckung_punkte 5,0 |
| Zahlen (5): bau.json Klauen n_theta/s_T als Summen | Krallen je Mesh unter `je_mesh` (Streifen, Ringe, Spitzenglieder, Vergroesserung); im Zweig 'einzeln' Masse je Mesh statt Summen | `farbe/K5c_farbe_v3/bau.json` |
| Sicht: Tafel 07 mit "gdigrab" beschriftet | `kf_tafeln.spiel` nennt den Weg aus der Zeile "Verfahren:" des Spiellauf-Logs | `07`: "echtes Fenster (PrintWindow)" |
| Zahlen Befund E: Brust/Hals aus F18, klassenunrein verglichen; Zone zu dunkel | Vergleich gleich gegen gleich (Zonen-Lab unten). Versuch Brust <- G4 10 F5/F6 (Messblatt 2.3 ohne F4): K6 dE 6,0 und Knochen 10 L\* 31,9, aber in der Rundum-Ansicht wieder lachsfarbene Scherben an der Schulter (Scratchpad `r3/all_v10.png`, der Sicht-Befund der Runde 1) und K12 Vorderglied 4,3 % > 3,0 (11 Brustflaechen in Bild 1). Verworfen, Brust bleibt F18 | Zone Kopf/Rumpf Bild 2 (`zonen2.py`): V (13,9 / 22,2 / 13,3), Original (18,5 / 18,3 / 3,7). Bleibt zu dunkel, siehe K6 |

## V3R3.2 Konstanten (neu oder geaendert in Runde 3)

| Konstante | Wert | Quelle |
|---|---|---|
| KRALLE_INSELN | (14,7), (14,15), (19,7), (19,15) | G4-Inseln des linken Krallenstreifens, gemessen (Bbox, L\*-Mittel 40-49; rechter Streifen L\* p90 98-100) |
| Krallen v_w / v_r / v_s | 202 / 213 / 254 | v_w: erste Streifenzeile mit >= 8 Texel Breite; v_r: letzte Zeile mit Zeilenmittel in der A3-Rotklasse (MESSBLATT_FARBE_V1 6 A3); v_s: letzte Zeile mit Index != 0 |
| Krallen u | 230 / 237 | Schnitt der Streifenbreiten der Zeilen 202-213 (S1) |
| rho laengs | 0,0957 Texel je Einheit | Median der Kanten Spitze -> Wurzel der G4-Krallen (0,087-0,102 je Insel) |
| Spitzenglieder k | je Kralle 2-4 | Vergroesserung des Spitzenteils am naechsten an 1 (Quelldichte) |
| F1-Kern | Deckung > 0,5 x Median | Mehrheitsregel (Definition, wie BAND_MEHRHEIT) |
| F1-Rand | 1 Texelring aussen, dunklere Haelfte | Messblatt V3 3.1 Machart; 1 Ring = kleinste Einheit |
| ZIEL_BAND_KETTE | (182,117,102) | Ketten-Rueckrechnung an Knochen 23 (V3R3.1) |
| B1_C_SEITE1_EICH | 0,535 | Eichung auf K12-Original 5,9 % (5 Laeufe, V3R3.1) |
| B1_SICHTBAR_NR | 16: 39, 17: 37 Flaechen | Leser B fid1.npz, >= 1 px in Bild 1 |
| entfallen | WEICH_RAND, MORPH_CLOSE 3x3, Seite-1-Buntheit 0,574 | Zahlen-Pruefer (5), Befund C |

## V3R3.3 Rote Riegel und warum

- **F2t (dE 15,1) gegen K15 (dE 5,2):** Entscheid fuer K15 (Sicht-Pruefer V3 r2: "Fuer das Auge zaehlt K15, nicht F2t").
  - Ein Texel, das beide Riegel erfuellt, gibt es nicht: F2t verlangt dE <= 6 um (49,6 / 37,5 / 20,8), die Kette an Knochen 23 (55,7 / 24,2 / 18,8); Abstand 14,8.
  - Vermutete Ursache der Messblatt-Zahl: Die Vorwaerts-Gegenprobe setzte das Texel auf alle K5c-Pixel des Bandkerns. In der Kette tragen es nur die
    Bandflaechen auf Knochen 23; 258 Maskenpixel sind Hintergrund (Kette a\* 41,8, Original 33,4), 192 px Hand 18 und 96 px Bein 24 (`bk_end.txt`).
  - Der Messblatt-Pruefer muss F2t neu fassen oder streichen.
- **A8 (Zahn 2,0 %):** nach dem Wortlaut knapp unter 2 %. Das hellere Band bringt die hellsten F7-Eintraege ueber L\* 60; mehr helle Punkte waeren
  eine neue, unbelegte Stellgroesse.
- **K7 (dE 9,6):** Unterkiefer-Zone V (48,5 / 29,2 / 18,4), Original (56,3 / 24,1 / 13,3); Runde 2 15,3.
- **K6 (dE 6,8):** Figur Bild 2 V (21,0 / 24,6 / 14,5), Original (25,1 / 20,5 / 9,6). Treiber ist die dunkle Zone Kopf/Rumpf (Brust Knochen 10 L\* 14,5
  gegen 21,8). In Runde 2 (4,8) haben die beigen Krallen (Knochen 20/29 L\* 60-67, Original 20/41) das ueberdeckt. Krallen einmal je Streifen (Lauf v1): 6,4.
  Hellere Brust aus F5/F6: 6,0, aber lachsfarbene Scherben (verworfen, V3R3.1).
- **P2:** Brust +10,2 (unveraendert), Klauen l/r +16,3 / +21,0. Der Ansatz ist die gleichmaessig dunkelrote Wurzelzone der G4-Kralle, 1,7-2,9-fach gedehnt.
  Varianten: Streifen einmal je Kralle (v1) +13,5/+15,9 mit beigen Krallen; nur Spitze im letzten Glied (v2) +21,6/+24,4; gebaut (v12) +16,3/+21,0.
  Abwaegung zugunsten des Sicht-Befunds "dunkelrote lange Klaue"; die Krallen sind ohne Mosaik.
- **B13 (24,6 < 25,1), B16 (46,7 %):** kamen mit den gleichmaessig roten Krallenansaetzen (Lauf v1 mit beigen Krallen: 25,1 / 57,3, beide GRUEN).
- **B10 7,0 / B11 2,7 / B12 2,6;17,3 / B17 0,541, A1 7,3, N1, K1v2 6,3 / K2v2 6,0:** wie Runde 2. Die dunklen G4-Felder (F10, F11, F18, F17, F19)
  zeichnen duenner und weniger bunt als die V1/V2-Familienspanne; G1 "schwarz rot" verlangt sie.
- **K3 (Hinterbein r dE 10,5, Ruecken 8,4):** blaeuliche Haut des Originals ohne G4-Hautzeile (V3.5 Punkt 3). Ruecken von 7,3 auf 8,4 mit dem kleineren F1-Fleck.
- **B2 (2,6 / 2,0), B9 14,2:** Rot als Aderlinien am Umriss (G4-Machart), wie Runde 2. B9b jetzt GRUEN (10,8).
- **D1 nach Messblatt-Formel** (Info-Zeile 2,33-2,81 an Rumpf, Schwanz, Hals, Kopf): unveraendert die Kachelung der Runde 2 (Zahlen-Pruefer Befund D).
  Das Gate rechnet ortsgenau (Median 1,06-1,41), offen ausgewiesen. Klauen jetzt 0,95 / 1,06, Anteil > 2,0 bei 0,0 % (Runde 2 16,9 %).

GRUEN: G0, H1, H1b, H2, P1, D1 (ortsgenau), S1, F1t (dE 5,84), F3t, A2-A7, A9-A12, B14, B15, N2, K4, K5, K8-K14, K4b, B9b, K15.

## V3R3.4 Tafeln (`farbe/`, Messblatt 6; Runde 2 in `farbe/v3r2/`)

- 01/02: Original | K4 heute | V3 Runde 3, Disc-Licht x k, roh und mit Videofilter (Saat 15), Nahkacheln der Feuerstellen.
- 03 Familie (EM30 | G5 | G4 | V3 | K4), 04 Atlas mit Herkunftskarte, 05 rundum, 06 Kopf nah, 07 Spiel (PrintWindow), 08 Binnenzeichnung.
- Gesehen (Bauer, keine Abnahme): Vorderklaue in Bild 2 dunkelrot mit heller Spitze, ohne Schachbrett; F1 ein schraeger rosa Streifen; das Band
  heller lachsrosa. Die Hand 18 bleibt mauve (Bild-1-Zeilen).

## V3R3.5 Offen

1. **F2t gegen K15:** Der Messblatt-Pruefer muss F2t neu fassen (V3R3.3). Die Zahlen dafuer stehen in `V3B_r3_skripte/bk_end.txt`.
2. **K6 / Zone Kopf/Rumpf zu dunkel:** Es fehlt eine helle Hautquelle ohne Lachs-Look; F5/F6 bringt die Polster der Runde 1 zurueck.
3. **Krallen-Posterisierung (P2 +16/+21, B13, B16):** Abwaegung Farbe (Sicht) gegen Posterisierung; Pruefer-Entscheid.
4. **Hand 18 mauve** (Sicht-Befund klein) gegen K15: mit G4-Zeilen auf den Bild-2-Flaechen wird K15 ROT.
5. Messblatt 7.1 (Gesicht oder Becken in Bild 1) bleibt eine Rueckfrage an den Nutzer.
