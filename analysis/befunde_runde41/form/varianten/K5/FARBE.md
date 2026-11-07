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
