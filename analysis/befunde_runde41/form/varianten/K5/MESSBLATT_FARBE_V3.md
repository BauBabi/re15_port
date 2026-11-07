# Messblatt Farbe V3: G4-Uebertragung und Feuerfarben (Riesenform K5c, Pruefer, 2026-11-07)

Gegenstand: Farbe Versuch 3 der Riesenform K5c (`varianten/K5/K5c/FORM.EMD`, sha256 a71f8d08...). Geometrie, Knochen und Clips von K5c bleiben; UV, CLUT
und TPAGE der Flaechen duerfen sich aendern. Grundlage: Leser A (`FARBE_V3_LESER_A.md`, G4-Atlas und Teile-Zuordnung), Leser B (`FARBE_V3_LESER_B.md`,
Feuerstellen und Schwarz-Rot), eigene Nachmessung des Pruefers. Kein RE-Wert: K5c ist ein neues Modell (PORT-WAHL "eher c", Nutzer 2026-11-06).
Jede Zahl unten ist gemessen; Konstanten tragen ihre Quelle (Messung, Messblatt-Zeile, Datei) oder sind als PORT-WAHL des Pruefers markiert.
Seiten: (W) = aus Sicht des Wesens, (B) = Betrachter. Lab = CIE-Lab D65, OpenCV float (RGB/255), wie V1/V2.

Eigener Code und Rohzahlen: `farbe_lesung/V3P_skripte/` (Kopie des Scratchpads
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3/pruef/`):
- `szene.py`: K5c in Kamera Bild 1/2 mit Szenenlicht (`Kandidat`-Licht "mess"), **Licht je Pixel aus einem Weiss-Texel-Render**, Knochen-ID je Pixel durch
  Weglassen je Knochen (32 Renders je Bild; unabhaengig vom Rasterer von Leser B; ohne ID 93 / 1019 von 36 383 / 51 271 Figurpixeln).
- `orte.py` -> `orte.json`: Lage, Groesse, Knochen der Leuchtstellen.
- `gegenprobe.py` -> `gegenprobe.json`, `gegenprobe.log`: **Vorwaerts-Gegenprobe**. Texel T an der Stelle x Licht je Pixel, eingesetzt in das rueckgerechnete
  Spielbild e des Originals, dann `video_filter` und Vergleich mit dem Original-Video im Kern der Stelle. Damit entfallen die Annahme eines einheitlichen
  Figurlichts (Leser A und B) und ein freies Licht (Leser B 3). Rundlauf ohne Ersatz: dE 2,86 (F1), 3,03 (Band).
- `stich.py` -> `stich.json`: drei Stichproben der Teile-Zuordnung mit eigenem Verfahren (Schwerpunkt-affin, siehe 2.2).
- `schwarzrot.py` -> `schwarzrot.json`: Klassenanteile je Region, mit Figurlicht und mit Licht je Pixel; G4-Atlas mit eigener UV-Rasterung.
- `kette_b2.py` -> `kette_b2.json`: Ketten-Vorhersage fuer Leser As (b2)-Machbarkeitsprobe (`formfarbe3/A/voll2_uv.pkl`), gleicher Weg wie `riegel_farbe.kette_v2`.
- `rundlauf_ton.py`, `ziel_kette.py` -> Zielwerte der neuen Ketten-Riegel im Messraum von `p_zonen.rueck`, Original und Rundlauf mit 3 Saaten.
- `gegen_v2.py` -> `gegen_v2.json`: Gegenprobe Versuch 2 Runde 2 an den neuen Riegeln.

Belegbilder (`farbe_lesung/`, nichts aufgehellt, Knochenkarten als Falschfarbe beschriftet):
`V3P_1_stichproben.png`, `V3P_2_bild1_F1_x4.png`, `V3P_3_bild2_maul_x3.png`, `V3P_4_ketten_vorhersage_b2.png`.

## 0. Ergebnis in Kuerze

1. **G1 (schwarz-rot verbrannt) passt messbar zum G4.** Die dunkle Klasse (T L\* < 20) macht in Bild 1 63,6-65,3 % der Figur aus, mit Mittel T (33-37, 28-32, 32-37).
   Im G4-Atlas sind es 60,0 % mit Mittel T (32, 26, 28). Die G4-Haut ist also die Bahnsteig-Haut. In Bild 2 ist die Haut roetlicher, T (64-65, 23, 26).
2. **Das G4-Rot hat den falschen Farbton fuer Bild 1.** Das Rot im G4-Atlas liegt bei 30,3 Grad. Das Bahnsteig-Rot liegt in der Kette bei -9,5 Grad (Figur)
   und -11,8 Grad (Ruecken), im Rundlauf bei -6,0 bzw. -9,3 Grad. Leser As (b2)-Probe aus reinen G4-Texeln landet in der Bild-1-Kette bei +24,8 Grad
   und 21,1 % Rot am Schwanz (Original 5,9 %). Das ist der Fehler von Versuch 2 (+38,1 Grad) noch einmal. Darum gibt es neu K11 und K12.
3. **Die Leuchtstelle in Bild 1 liegt auf dem Becken, nicht auf dem Gesicht.** Das bestaetigt Leser B mit eigenem Verfahren.
   - Stelle F1: 646 px, bbox (301,222)-(330,259), Schwerpunkt (317,3 / 241,2), 45 x 20 px, Achse 119 Grad. Deckung mit Leser B: IoU 0,85.
   - In der Bild-1-Pose liegen 646 von 646 px auf Knochen 0. Kopf 22 und Kiefer 23 ueberdecken die Stelle selbst ohne Verdeckung mit 0 px;
     ihr naechster Punkt liegt 21,6 px vom Schwerpunkt. Sichtbar sind von Hals 21 und Kopf 22 0 px, vom Kiefer 23 125 px.
4. **Farbe F1 bei festem K5c-Licht.** Das Licht je Pixel an der Stelle ist (0,490, 0,431, 0,373) im Median, also gleich dem Figurlicht.
   - Ziel-Texel T (149, 103, 122), Lab (48,9 / 21,4 / -3,1), Farbton -8,3 Grad: ein **gedecktes Rosa-Mauve**. Vorwaerts-Rest dE 0,2.
   - Leser As Ziel (133,81,104) hat den Farbton richtig, ist aber zu dunkel: Rest dE 11,8.
   - Leser Bs gemeinsamer Texel (174,95,96) braucht ein freies Licht nahe Ambient. Mit dem Licht, das die K5c-Geometrie dort wirklich bekommt, verfehlt er F1 um dE 15,7.
5. **Feuerfarbe am Maul (Bild 2) ist das Fleischband des Unterkiefers.** Leser B ist bestaetigt, Leser A widerlegt.
   - Unterkiefer im e-Raum: rot 66,4 %, hell neutral (Zaehne) 12,2 %, gelb 10,3 %.
   - Band: T (184, 90, 85), Lab (49,6 / 37,5 / 20,8), Farbton 29,0 Grad, Rest dE 0,86. Es liegt auf Knochen 23 (1284 px), 22 (386 px) und 18 (192 px).
   - Leser As Creme (234,208,189) ist das hellste Zehntel: 29 % Zaehne, 20 % Band, der Rest Saum und Uebergang. Am Band verfehlt es um dE 31,5.
6. **Gesaettigte Feuertexel sind in beiden Bildern widerlegt.** Vorwaerts-Rest dE an F1 / am Band:
   - Orange (255,140,40): 58,3 / 23,5. Gelb (255,200,60): 65,6 / 38,9.
   - G4-F8-Feueranteil: 37,0 / 10,7. G4-F16-Feueranteil: 25,0 / 6,8.
   - "Leuchten" heisst gemessen: helle Texel mit L\* 49 auf einer Haut mit L\* 13-15. Gelb-Orange gehoert nicht dazu.
7. **Ein Texel fuer beide Stellen geht mit K5c nicht.** Die gemeinsame Anpassung verfehlt F1 um dE 11,7 und das Band um dE 5,7.
   Die Kreuzproben verfehlen um dE 21-22. F1 und Band brauchen also zwei Felder: Rosa-Mauve -8 Grad am Becken, Lachsrot 29 Grad am Kiefer.
8. **Die Teile-Zuordnung traegt an Rumpf und Gliedern, am Kopf nicht.** Das zeigen die drei Stichproben mit eigenem Verfahren.
   - Lineare Vergroesserung: Oberschenkel 9 <- G4 1 1,20, Unterarm 12 <- G4 12 1,22, Kopf+Kiefer 22/23 <- G4 26 2,67.
   - Posterisierung im Render (dL = 0, gleicher Weltmassstab), K5c gegen G4-Quelle: Oberschenkel 70,0 % / 71,8 %, Unterarm 76,7 % / 76,0 %, Kopf 56,3 % / 46,3 %.
   - Der Kopf aus G4-F1 wird hell knochenfarben. In der Bild-2-Kette faellt die dunkle Klasse der Figur dadurch auf 17,2 % (Original 54,6 %),
     an Kopf/Rumpf steigt Rot auf 74,0 % (Original 14,9 %). Zu sehen in `V3P_4`.

## 1. Gestaltung nach dem Nutzer-Wortlaut 2026-11-07

Wortlaut: "Ich koennte mir vorstellen, das er so schwarz rot ist in seiner 4 beider Form, weil er verbrannt ist, wenn der generator explodiert. Man sieht auch in der
4 beider Ansicht von hinten das das Gesicht vorne irgendwie in flammenfarben leuchtet. Bei der vorderansicht ja auch vielleicht hat er mit der Mutatione die feuer
Farbe beim Mund nach der explosion angenommen."

### 1.1 Die Gestaltung

**G1, Koerper schwarz-rot verbrannt:**
- Die Hautflaechen kommen aus den handgemalten G4-Texeln (CDEMD0.EMS Typ 0x34, TIM @0x65e800). Die 8-bit-Indizes sind bytegleich.
- Die CLUT ist bytegleich, ausgenommen die Bild-1-Anpassung in 1.3 und die Sonderfelder.
- Messgrundlage: Die G4-Dunkelklasse trifft Bild 1 (0. Punkt 1). Rot in Adern ist G4-Machart (A: Gesamt-Lab 20,9 / 12,2 / 6,0, Rotanteil 28,6 %;
  eigene Rasterung: dunkel 60,0 %, rot 26,1 %, Rot-Farbton 30,3 Grad).

**G2, Feuerfarben:** Gesetzt wird an den Stellen, die die Bilder zeigen, mit der Farbe, die die Vorwaerts-Gegenprobe ergibt (Abschnitt 3).
- (a) Bild 2, Maul: Fleischband am Unterkiefer, Lachsrot 29 Grad, darauf helle Zaehne.
- (b) Bild 1, die Stelle, die der Nutzer als leuchtendes Gesicht liest: Rosa-Mauve -8 Grad auf den Beckenflaechen von Knochen 0.

### 1.2 Entscheide des Pruefers zwischen Wortlaut, Messung und frueheren Entscheiden

| Frage | Entscheid | Begruendung (Messung) |
|---|---|---|
| "Gesicht vorne ... leuchtet" in Bild 1: Gesicht oder Becken? | **Becken (Knochen 0), Flaechen tf 107/108/119/120/95/96 (= qf 19/31/20/32/7/8, Leser B 1.3)** | Die K5c-Geometrie ist fest (Auftrag). In der Bild-1-Pose sind 646/646 px Knochen 0; Kopf 22/23 beruehren F1 auch unverdeckt nicht (0 px, Abstand 21,6 px). Am Gesicht gemalt waere die Stelle in Bild 1 unsichtbar, und Bild 1 verloere seine Leuchtstelle (516 der 869 Rotklassen-Pixel am Ruecken liegen in F1, `ziel_kette.json`, `rundlauf_ton.json`). Was der Nutzer erzaehlt, bleibt gewahrt: Das Leuchten des Gesichts zeigt Bild 2 am Maul (1.1 a). Offen 7.1 |
| G2 "Flammenfarbe" gegen gemessenes Rosa (Bild 1) und Lachs (Bild 2) | **Messung gilt** (E11 "Farben aus dem Screenshot uebernehmen", E14 "keine Farben, die der Screenshot nicht zeigt") | Gelb, Orange und G4-Feuer F8/F16 verfehlen beide Bilder um dE 6,8-65,6 (0. Punkt 6). Am Maul ergibt die Messung trotzdem eine Flammenfolge: rot 66 %, gelber Saum 10 %, weiss 12 % |
| E24 "Augenstellen dunkel" gegen G2 "Gesicht leuchtet" | **E24 bleibt, kein Widerspruch** | Bild 2, Oberkiefer: 98,2 % mit e-L\* < 10, e-Lab (4,0 / 7,6 / 2,7). Augen e-L\* 8,5 und 12,4 (Knochen 22). Das Band beginnt bei y 323, die Zaehne bei y 318, also 43-48 px unter der Augenlinie y 275 |
| "Kiefer nicht cremefarben" (V1/V2) gegen Leser As Bild-2-Ziel Creme | **Kiefer lachsrot, Creme nur an den Zaehnen** | As Auswahl (hellstes Zehntel nach Video-L\*) trifft zu 29 % Zaehne und zu 20 % Band; am Band verfehlt sie um dE 31,5 |
| Kopfquelle G4-F1 (Leser A 4.2) | **nicht F1 allein**: Schaedel, Schnauze oben und Kiefer aussen aus dunklen G4-Hautfeldern | F1 ist hell (Lab 30,3 / 9,1 / 6,4, oben beige) und 2,67-fach vergroessert (Render dL = 0 +10 Punkte). Mit F1 sinkt die dunkle Klasse in der Bild-2-Kette auf 17,2 % (Original 54,6 %, Riegel K13) |

### 1.3 Was aus V1/V2 gilt und was V3 bewusst ueberholt

- **Gilt unveraendert:**
  - V1: A1-A5, A7, A9 (E24), A11 (E9), A12, K1-K10, N1, N2.
  - V2: K1v2, K2v2, K4b, B2 (float), B9/B9b, B10-B17 (Binnenzeichnung in der Familienspanne), Abschnitt 4 (Konstanten nur mit Quelle).
  - Kein rueckprojizierter Glanz (A5, N2). PSX-Machart: 8 bpp, wenige Farben je Teil, keine Foto-Fragmente.
  - "Aus einem Guss" (E9, A11): Schwanz mit durchgehendem T ueber die Glieder 1-6, das Muster wiederholt sich nicht je Glied.
- **Bewusst ueberholt:**
  - V2 3.2, "Bild-1-Rot Farbton 34 Grad": falsch, nach Pruefung v2 r2 und hier nachgemessen. Ersetzt durch K11.
  - V2 Abschnitt 5 (Mal-Verfahren kf_mal*): Versuch 3 malt die Haut nicht, er uebertraegt die G4-Texel.
    Die Malkonstanten aus V2 4 (K_GRAPH, SPIEGEL_ABSTAND, KORN_*, L_STUFEN, Zahnfleisch-Gewicht) entfallen. An ihre Stelle treten H1, H2, P1, P2, D1 und S1.
  - V1 A6 (HF_fft je Feld, Grenze 0,26) gilt nur noch fuer die Sonderfelder. Fuer die G4-Felder ist der Massstab G4 selbst, gemessen mit den Riegeln aus V2.
  - V1 A8 (Kiefer: Zahn 2-20 %, Rot >= 20 %, Dunkel >= 20 %) bekommt das Kieferfeld neu: Unterkiefer aussen samt Band.
    Bild 2 zeigt dort 66 % Rot und 12 % Zahn. Die 20 % Dunkel kommen aus der Kieferflanke, die in Bild 2 verdeckt ist.
  - Atlas-Zonen der V1-Riegel (`k_form.py:11`, Koerper = x < 384): Die V3-Anordnung ist neu. Die Felder sind im Riegel-Skript nachzutragen und im Dossier zu nennen (wie V1 6 verlangt).

## 2. Teile-Zuordnung K5c <- G4 mit Verfahren je Teil

### 2.1 Grundlage

- K5c ist nicht aus G4 abgeleitet (Leser A 2, nachgemessen): G4 hat 27 Knochen und 27 Meshes, K5c 32 Knochen. K5c ist ein Ringbau mit eigenem Gerippe.
  Es gibt keine Eckpunkt-Entsprechung. Verfahren (a), die UV-Uebernahme je Eckpunkt, geht nirgends.
- Die Rollen der Teile entsprechen sich, die lokalen Achsen sind gleich: x vorn, y unten, z links (W). Rechts (W) ist in beiden Modellen -Z.
- Verfahren (b1), die Neuabtastung in die vorhandene K5c-UV, posterisiert. Leser A misst mit `stufen3.py` 54,2 % dL = 0 am Koerper (G4 17,2 %). **Verboten** (P1).
- **Verfahren (b2), UV-Projektion:**
  - Die G4-Seiten 0-2 kommen bytegleich in die K5c-TIM, samt CLUT 0-2.
  - Jede K5c-Flaeche bekommt G4-UV, -CLUT und -TPAGE; alle Ecken liegen in einer G4-Insel.
  - Geometrie, Normalen, Indizes, Knochen und Clips bleiben (G0).

### 2.2 Eigene Stichproben (`stich.py`, `stich.json`, Bild `V3P_1_stichproben.png`)

**Verfahren** (anders als bei Leser A, der je Ecke eine Insel sucht):
- Zylinder-Parameter je Teil: theta um die Hauptachse, t entlang der Achse von der Wurzel weg.
- Jede K5c-Flaeche nimmt die affine Abbildung des G4-Dreiecks, das ihren Schwerpunkt in (theta, t) enthaelt, und wendet sie auf alle ihre Ecken an.
- Liegt der Schwerpunkt in keinem G4-Dreieck, gilt das naechste.

**Darstellung:** gleicher Weltmassstab fuer G4-Teil und K5c-Teil, Licht 0,35 + 0,65 max(0, N.L), roh. dL = 0 ist der Anteil benachbarter Pixel mit gleichem gerundetem L\*.

| Probe | K5c <- G4 | Flaeche K5c / G4 (Einheiten^2) | lin. Vergr. (eigen / Leser A) | Nahtflaechen > 20 Texel (eigen / A) | Render dL = 0, Mittel 3 Ansichten K5c / G4 | Render Lab K5c / G4 (Seite) | Geometrie unveraendert |
|---|---|---|---|---|---|---|---|
| Hinterbein r (W) Oberschenkel | 9 <- 1 | 3 460 521 / 2 398 264 | 1,20 / 1,10 | 9,0 % / 4,9 % | 70,0 / 71,8 | (6,9 / 1,9 / 0,7) / (8,4 / 2,5 / 1,0) | ja |
| Vorderbein l (W) Unterarm | 12 <- 12 | 3 095 438 / 2 070 866 | 1,22 / 0,85 | 15,8 % / 2,5 % | 76,7 / 76,0 | (5,3 / 1,6 / 0,2) / (5,9 / 2,1 / 0,3) | ja |
| Kopf + Kiefer | 22+23 <- 26 | 15 783 153 / 2 222 212 | **2,67** / 3,09 | 4,4 % / 4,4 % | **56,3 / 46,3** | (20,6 / 10,3 / 6,3) / (18,7 / 5,7 / 4,1) | ja |

**Lesart:**
- An Bein und Arm uebertraegt (b2) die G4-Machart (sichtbar in `V3P_1`):
  - schwarze Haut, rote Adern als zusammenhaengende Linien;
  - Lab-Abstand zur Quelle unter 2;
  - Posterisierung im Render wie bei der Quelle (Differenz -1,8 bzw. +0,7 Punkte).
- Unterschiede in der Vergroesserung gegenueber Leser A:
  - Sie kommen aus der Definition. Hier: Wurzel aus dem Verhaeltnis der ganzen Mesh-Flaechen. Leser A: Feldtexel je Zielflaeche, mit ausgeschlossenen Feldern.
  - In beiden Messungen ist der Kopf der einzige Ausreisser (> 2).
- Die Nahtflaechen haengen am Verfahren:
  - Mein einfacheres Schwerpunkt-Verfahren hat an den Gliedern mehr Nahtflaechen als Leser As Inselsuche.
  - Der Bau muss Nahtflaechen loesen. Massstab ist S1 (keine fremden Texel), nicht der Prozentwert.
- Am Kopf zeigt die Probe den Befund von Leser A: beige Knochenschnauze, grobe Bloecke (dL = 0 +10 Punkte). Daraus folgen D1 und K13.

### 2.3 Zuordnung je Teil (verbindlich fuer den Bau)

Grundlage ist Leser A Abschnitt 3. Die Spalte "Pruefer" nennt Bestaetigung oder Aenderung.

| K5c-Teil (Mesh) | G4-Quelle (Mesh / Feld) | Verfahren | Pruefer |
|---|---|---|---|
| Rumpf: Huefte, Wirbel, Brust (0, 7, 10) | Becken 0 (F11), Lende 9 (F10), Brust 10 (F4/F5/F6) ohne F3 (Rippen, hell), F7, F8, F9, F16 | b2, Kette mit durchgehendem T | bestaetigt; **dazu das Sonderfeld F1-Stelle auf den Flaechen von 1.2** (Abschnitt 3.1) |
| Schwanz (1-6) | Vorderbein l 11+12 (F17+F19) | b2, Kette mit durchgehendem T (E9) | bestaetigt mit Auflage K12. In der (b2)-Probe ist der Schwanz in der Bild-1-Kette zu 21,1 % rot (Original 5,9 %). Erlaubt sind dunklere Quellbereiche (F10/F11 ohne Adern) oder Adern-CLUT nach H2 |
| Hals (21) | Brustflanken F4/F5 | b2 | **geaendert** gegen A (Brust-T 0,70-1,0 mit 65 % Nahtflaechen). Das ist As eigene Alternative (A 7.3). In Bild 1 und 2 ist der Hals verdeckt (Bild 1: 0 px) |
| Hinterbein l (W) 8 / 24 / 25 | 5 ohne Auge F16 / 6 / 7 | b2 | bestaetigt |
| Hinterbein r (W) 9 / 26 / 27 | 1 / 2 / 3 | b2 | bestaetigt (Stichprobe 9 <- 1) |
| Zehen an 25/27 | Hinterkrallen 4/8 (F15) | b2, eigene Felder je Fuss (das gemeinsame Zehenfeld wird geteilt) | bestaetigt |
| Vorderbein l (W) 11 / 12 / 13 | 11 / 12 / 13 (F17 / F19 / F20) | b2 | bestaetigt (Stichprobe 12 <- 12) |
| Vorderbein r (W) 16 / 17 / 18 | 15+16 ohne Auge / 17 / 18 (= Texel von 13) | b2 | bestaetigt; Oberarm 16 mit 41 % Nahtflaechen (A): S1 |
| Klauen l 14/15/28/30, r 19/20/29/31 | 14 bzw. 19 (F21) | b2 je Klaue, neue Felder statt dem gemeinsamen Feld "kralle" | bestaetigt |
| Kopf 22 aussen (Schaedel, Schnauze oben, Flanke) | dunkle Hautfelder F18 (Oberarm r: schwarz mit Laengsadern) oder F4/F5; Zahnflanke aus dem dunklen Teil von F1 | b2 je Bereich, Vergroesserung <= 2,0 (D1) | **geaendert**: nicht F1 allein. Bild 2 Oberkiefer: T (69, 23, 23), Lab (15 / 22 / 11), 87-88 % dunkel |
| Kiefer 23 aussen, Unterseite | Unterseite und Gaumen: F2 (Maulinneres); aussen: wie Kopf | b2 | Trennung nach Normalen (A 4.2, in b1 schon gebaut) |
| Kieferband und Kieferkante 22 (Bild-2-Band) | **Sonderfeld Maul-Feuer** (3.2): Indizes aus G4-F7 (strahlenfoermiges Fleisch, 175 Farben, handgemalt), eigene CLUT-Zeile | eigenes Feld | neu (G2) |
| Zaehne (Seite 3, 384,180,24,14) | F21/F15-Spitzen (hell) | eigenes Feld | bestaetigt |
| Augen (22, Insel 44) | dunkel (E24) | eigenes Feld | bestaetigt |
| nicht uebertragen | kleine Arme F22-F24, Augen F16 auf der Haut, Rippen F3 | - | bestaetigt |

## 3. Feuerfarben: Gesicht vorn (Bild 1) und Maul (Bild 2)

### 3.1 Bild 1, Stelle F1 (Belegbild `V3P_2_bild1_F1_x4.png`)

**Abgrenzung** (PORT-WAHL des Pruefers, unabhaengig von Leser Bs e-Regel):
- Regel: Video-Lab L\* >= 30 und a\* >= 20, 8-zusammenhaengend mit dem a\*-Maximum der Auftragsbox (x 300-345, y 225-265); Startpunkt (311,250).
- Flaeche je Schwelle: L>=25/a>=20 807 px | L>=30/a>=15 856 | **L>=30/a>=20 646** | L>=30/a>=25 530 | L>=30/a>=30 373.
- Ab L\* >= 35 laeuft die Flaeche in den Hintergrund aus (292 720 px).

**Form:** 646 px, bbox (301,222)-(330,259), Schwerpunkt (317,3 / 241,2), Laenge x Breite (4 sigma) 45,2 x 19,7 px, Achse 119 Grad.
Leser B: 708 px, Schnitt 622 px, IoU 0,85.

**Ort auf dem Koerper:**
- Knochen 0 mit 646/646 px. Flaechen nach Leser B 1.3: tf 107/108/119/120/95/96.
- Knochenraum laut Leser B (-753..-355, -564..-103, -597..-343), 567 x 243 Einheiten. Das ist die Beckenoberseite hinten.
- Kopf 22+23 unverdeckt: bbox (336,240)-(403,314), 0 px auf F1.
- Hals 21 unverdeckt: 79 px auf F1; in der Pose verdeckt (0 px sichtbar).

**Farbe:**

| Stufe | Wert |
|---|---|
| Video V | (150,6 / 90,8 / 136,2), Lab (46,6 / 31,5 / -14,8) |
| Schirm e | (68,6 / 43,1 / 43,4), Lab (20,6 / 11,9 / 4,6) |
| Licht je Pixel K5c (Median, p10 / p90) | (0,490 / 0,431 / 0,373); (0,39 / 0,35 / 0,30) / (0,67 / 0,59 / 0,51) |
| T = e / Licht (Median) | (140 / 95 / 116), Lab (45,7 / 21,6 / -4,4) |
| **T aus der Vorwaerts-Gegenprobe** | **(149, 103, 122), Lab (48,9 / 21,4 / -3,1), Farbton -8,3 Grad, Rest dE 0,2** |
| Empfindlichkeit, Licht wie Leons Gesicht (Ambient) | (228 / 144 / 140), Lab (68,1 / 31,5 / 15,3), +25,9 Grad |
| Empfindlichkeit, Licht wie Leons hellste Stelle | (110 / 78 / 100), Lab (37,2 / 17,5 / -7,3), -22,6 Grad |

- Die Licht-Spanne nach Leon kaeme nur bei einer anderen Geometrie zum Tragen. Bei fester K5c-Geometrie ist das Licht am Ort gemessen.
  Die Empfindlichkeit zeigt: Der Farbton ist nur durch die Geometrie festgelegt. Das ist ein weiterer Grund, F1 auf das Becken zu legen und nicht frei zu waehlen.
- **Zielfeld F1:** Feldmittel T (149,103,122) +- dE 6 (Toleranz wie V1 K3 je Zone). Zur Lage: die Flaechen von Knochen 0 aus 1.2.
  Die Groesse im Bild ist 646 px, die Kette prueft sie (K14).
- **Machart:** G4-Fleischindizes (F7 oder F16 ohne Iris) mit eigener CLUT-Zeile, die das Feldmittel auf das Ziel legt. Die L\*-Rangfolge der Eintraege bleibt (H2).
  Keine flache Fuellung (P1 gilt im Feld). Ein dunkler Rand zur Haut ist G4-Machart und erlaubt.

### 3.2 Bild 2, Maul (Belegbild `V3P_3_bild2_maul_x3.png`)

**Unterkiefer** (Nutzer-Teil `maske_unterkiefer.png`, ohne Logo):
- 3355 px, bbox (243,318)-(343,408), Schwerpunkt (293,6 / 364,0), 126 x 38 px, Achse 141 Grad.
- Auf K5c: 3097 px; Knochen 23 1896 px, 22 807 px, 18 235 px, 24 114 px.
- Klassen im e-Raum (PORT-WAHL des Pruefers, andere Grenzen als Leser B):

| Klasse | Grenze | Anteil |
|---|---|---|
| rot | a\* >= 10, Farbton -10..60 Grad | 66,4 % |
| hell neutral | C\* < 10, L\* >= 20 | 12,2 % |
| gelb | C\* >= 10, Farbton 60..100 Grad | 10,3 % |
| dunkel | | 0,4 % |
| geclippt | | 0,1 % |
| Rest | | 10,6 % |

Leser B mit eigenen Grenzen: 56,5 / 18,2 / 5,5 %.

**Teilflaechen:**

| Teil | px | bbox, Schwerpunkt | Knochen (px) | V | e Lab | T Vorwaerts-Gegenprobe (Rest) | T = e/Licht (Median) |
|---|---|---|---|---|---|---|---|
| **Band** (Feuerfarbe) | 2227 | (243,323)-(343,408), (290,2 / 368,5), 130 x 39 px, 145 Grad | 23: 1284, 22: 386, 18: 192, 24: 96 | (159 / 43 / 55) | (16,9 / 17,2 / 11,2) | **(184, 90, 85), Lab (49,6 / 37,5 / 20,8), 29,0 Grad (dE 0,86)** | (181 / 94 / 87) |
| Zaehne | 408 | (251,318)-(339,407), (305,1 / 345,5) | 22: 231, 23: 155 | (175 / 104 / 131) | (27,8 / 4,1 / 2,9) | unsicher: Rundlauf-Fehler dE 7,4 bei 161 Kernpixeln | (191 / 184 / 203), Lab (75,7 / 6,6 / -9,0) |
| gelber Saum | 345 | im Band | 23 | (197 / 109 / 96) | - | unsicher: Rundlauf dE 5,2 | (214 / 194 / 154), Lab (79,1 / 1,2 / 22,5) |

**Weitere Stellen in Bild 2:**
- **Die hellen Flecken links unten** (die "hellen Flecken am Maul" des Auftrags): 969 px, bbox (164,331)-(225,383), alle geclippt.
  K5c deckt davon 5 px (Knochen 0), Nutzer-Teile 0 px. Keine Farbaussage (farbe.md 1: Weiss im Video ist nicht Weiss im Spiel). Am Koerper ist dafuer kein Ort; Offen 7.3.
- **Gesicht und Augen** sind dunkel: Oberkiefer e-Lab (4,0 / 7,6 / 2,7), 98,2 % mit e-L\* < 10. E24 gilt.

**Zielfelder Maul:**
- Band-Feldmittel T (184,90,85) +- dE 6.
- Zaehne: L\* >= 70, C\* <= 12 (Leser B: (199,191,183), Lab 77,7 / 2,1 / 4,8; Kittel als Weissbezug).
- Gelber Saum erlaubt, nicht verlangt.
- Machart wie 3.1: Indizes aus G4-F7, eigene CLUT-Zeile. Die G4-F7-Mittelfarbe (168,54,53) verfehlt das Band in der Kette um dE 6,5. Darum ist eine eigene CLUT-Zeile noetig.

### 3.3 Gegenprobe: ein Texel fuer beide Bilder (Leser B 3)

| T | Lab | dE F1 | dE Band |
|---|---|---|---|
| Ziel F1 (149,103,122) | (48,9 / 21,4 / -3,1) | 0,2 | 22,3 |
| Ziel Band (184,90,85) | (49,6 / 37,5 / 20,8) | 21,1 | 0,9 |
| gemeinsame Anpassung (187,96,109) | (51,8 / 38,0 / 9,4) | 11,7 | 5,7 |
| Leser B, Licht frei (174,95,96) | (49,4 / 32,2 / 13,8) | 15,7 | 5,6 |
| Leser B, Figurlicht (163,91,96) | (47,0 / 30,1 / 10,3) | 14,2 | 10,0 |
| Leser A Bild 1 (133,81,104) | (40,9 / 25,2 / -3,9) | 11,8 | 24,6 |
| Leser A Bild 2 Creme (234,208,189) | (85,0 / 6,1 / 12,7) | 47,2 | 31,5 |
| G4 F7 Mittel (168,54,53) | (39,9 / 46,5 / 27,0) | 36,3 | 6,5 |
| G4 F16 Feueranteil (203,104,86) | (55,2 / 37,7 / 28,1) | 25,0 | 6,8 |
| G4 F8 Feueranteil (217,141,107) | (65,6 / 25,1 / 30,2) | 37,0 | 10,7 |
| Orange (255,140,40) / Gelb (255,200,60) / Feuerrot (220,60,30) | - | 58,3 / 65,6 / 37,4 | 23,5 / 38,9 / 19,2 |

**Entscheid:**
- Zwei Felder: Rosa-Mauve am Becken (Bild 1), Lachsrot am Kiefer (Bild 2).
- Leser Bs "eine Farbe fuer beide" braucht an F1 ein Licht nahe Ambient. Die K5c-Geometrie gibt dort aber Figurlicht (0,490 / 0,431 / 0,373), gemessen am Weiss-Render.

## 4. Koerper schwarz-rot: Ziele je Region (rueckgerechnet)

Methode: `schwarzrot.py`. T = e / Licht, (a) mit dem Figurlicht aus `farbe_palette.json`, (b) mit dem Licht je Pixel aus K5c.
Klassen wie Leser B 4: dunkel T-L\* < 20; dunkelrot = dunkel und a\* >= 8; rot = a\* >= 15, L\* >= 20, Farbton -30..70 Grad; hell = L\* >= 45, a\* < 10.
In jeder Zelle steht (a) / (b).

| Bild / Region | dunkel % | dunkelrot % | rot % | hell % | T dunkel | Rot Lab, Farbton |
|---|---|---|---|---|---|---|
| 1 Schwanz | 78,7 / 86,1 | 24,7 / 16,1 | 3,9 / 3,9 | 2,4 / 1,6 | (37,29,34) / (29,23,26) | (39,7 / 23,6 / -0,4) -1 Grad / +2 Grad |
| 1 Ruecken/Flanke | 55,9 / 54,2 | 12,3 / 15,3 | 9,7 / 8,1 | 7,9 / 5,5 | (40,33,35) / (44,34,35) | (39,5 / 20,9 / -2,9) -8 Grad / -13 Grad |
| 1 Hinterbein l (W) | 51,0 / 31,9 | 7,2 / 6,1 | 1,3 / 1,0 | 7,4 / 11,7 | (38,34,40) / (39,33,36) | wenig Rot |
| 1 Hinterbein r (W) | 51,0 / 76,4 | 8,8 / 9,3 | 0 / 1,2 | 6,1 / 4,1 | (35,36,47) / (29,29,38) | - |
| 1 Vorderglied r (W) | 62,7 / 46,8 | 3,9 / 3,2 | 0 / 0,2 | 6,2 / 5,9 | (36,36,41) / (34,34,37) | - |
| **1 Figur** | **63,6 / 65,3** | 14,4 / 12,1 | **3,7 / 3,7** | 5,4 / 4,6 | **(37,32,37) / (33,28,32)** | (39,1 / 21,7 / -1,2) **-3 Grad / -6 Grad** |
| 2 Oberkiefer | 88,4 / 88,0 | 87,1 / 86,5 | 9,4 / 9,7 | 0,4 | (69,23,23) | +13 Grad |
| 2 Kopf/Rumpf | 73,1 / 71,6 | 69,2 / 68,2 | 11,0 / 11,7 | 3,1 / 2,8 | (62,22,26) | -0,5 Grad |
| 2 Unterkiefer | 0 | 0 | 70,2 / 66,0 | 18,6 / 22,1 | - | (49,2 / 32,8 / 18,8) 30 Grad |
| 2 Klauen r / l (W) | 21 / 6 | 21 / 6 | 68-75 | 5-11 | (69-80, 26-32, 25-27) | 38-40 Grad (Feuerschein, Leser B V2) |
| **2 Figur** | **54,6 / 54,1** | 52,3 / 52,0 | **30,6 / 30,8** | 5,4 / 5,0 | (64,23,26) | 31 Grad |
| **G4-Atlas, genutzt (48 820 Texel)** | **60,0** | 6,7 | **26,1** | 1,8 | **(32,26,28)**, Lab (10,3 / 3,4 / -0,2) | (38,0 / 35,5 / 20,8) **30,3 Grad** |

**Lesart (Ziele):**
- **Haut:** Die G4-Dunkelklasse trifft Bild 1 in Anteil und Farbe. Die roetlichere Haut in Bild 2 (T (64,23,26)) erklaert Leser B (farbe.md 4) mit Rot-Einstreuung von bis zu +21 e.
  Massstab fuer die Haut ist darum die Kette (K6, K8, K13), nicht ein Texelziel.
- **Rot in Bild 1:** nur 3,7 % der Figur. Es sitzt in F1 und am Schwanzende, mit Farbton -3 bis -13 Grad.
  - Das G4-Aderrot mit 30 Grad darf in Bild-1-sichtbaren Teilen hoechstens so viel Rotklasse erzeugen, dass K11 und K12 halten.
  - Bild-1-sichtbare Teile: Schwanz 1-6, Becken 0, Wirbel 7, Hinterbeine 8/9/24-27, Vorderglied r 16-18.
- **Rot in Bild 2:** 30,6 % der Figur, mit 31 Grad passend zum G4. Es wird getragen vom Kiefer (Sonderfeld) und den Klauen (G4-F20/F21, 24-26 Grad, dazu Feuerschein).

## 5. Riegel-Tabelle

**Messung:**
- Der Bau stellt einen Variantenordner im Format von `form_pruefung.Kandidat` bereit (FORM.EMD, FORM.TIM, form.json).
- Bestehende Riegel: `generator/riegel_farbe.py`, Atlas-Felder auf die V3-Anordnung nachgetragen.
- Neue Riegel G0, H1, H2, P1, P2, D1, S1, K11-K15: im selben Skript.
  - Die Methode steht in der Tabelle.
  - Fuer K11-K15 sind die Zielwerte gemessen (`rundlauf_ton.json`, `ziel_kette.json`).
  - Die Gegenproben laufen mit denselben Funktionen (`gegen_v2.py`, `kette_b2.py`).
- Saat 15; Rundlauf mit Saaten 15 / 12345 / 7.

| # | Mass | Vorgabe | Toleranz | Messmethode Atlas | Messmethode Render / Kette | Quelle | Gegenprobe |
|---|---|---|---|---|---|---|---|
| G0 | Geometrie-Diff | 0 | 0 (bytegleich) | Je Mesh tv, tn, tf, qv, qn, qf gleich K5c (FE.modell); EMR- und EDD-Abschnitt der EMD (emd_dir d[1]:d[3]) cmp gleich; Knochenzahl 32, Clipzahl gleich. Erlaubt verschieden: tuv/quv (u, v, CLUT, TPAGE), TIM | - | Auftrag | K5c gegen sich selbst 0; Probe `stich.py`: geometrie_unveraendert = ja |
| H1 | Herkunft G4-Texel je Teil | >= 95 % je Teil (Rumpf, Schwanz, Hals, Beine, Arme, Klauen, Kopf/Kiefer aussen) | - | UV-gedeckte Texel je Teil (flaechengewichtet): 8-bit-Index bytegleich mit dem G4-Atlas an der kopierten Stelle (Seiten 0-2, Lage im Dossier) und innerhalb der G4-Inselmaske. Sonderfelder (F1-Stelle, Maul-Band, Zaehne, Augen) zaehlen nicht mit | - | G1 | V2r2: 0,0 % der 8x8-Indexbloecke kommen im G4 vor (`gegen_v2.json`, 2048 Bloecke): **ROT** |
| H1b | Anteil der Sonderfelder | <= 10 % der UV-gedeckten 3D-Koerperflaeche | - | 3D-Flaeche der Flaechen mit Sonderfeld-UV / Gesamtflaeche | - | Bildanteile: F1 646 / 32 326 = 2,0 % (Bild 1); Band 2227 + Zaehne 408 + Saum 345 = 6,6 % von 45 038 (Bild 2); Summe 8,6 %, gerundet 10 | - |
| H2 | CLUT | G4-CLUT bytegleich, ausser fuer Sonderfelder und Bild-1-Aderrot (eigene Zeilen) | Umgefaerbte Zeilen: L\*-Rangfolge der genutzten Eintraege unveraendert (0 Vertauschungen bei dL >= 2) | Zeilenvergleich mit G4 CLUT 0-2; je umgefaerbter Zeile die Liste der Eintraege mit dE zum G4-Eintrag im Dossier | - | G1 "nicht prozedural neu gemalt": Die Zeichnung steckt in den Indizes, die Farbe in der CLUT | - |
| P1 | Posterisierung Atlas (Fleisch a\* > 15, Erosion 1; Mass `formfarbe2/v2r2sicht/stufen.py`) | dL = 0 <= 30 %; 5 <= dL < 10 >= 15 %; Farben im Fleisch >= 200 | - | Nachbarpaare der genutzten Fleischtexel | - | Familie dL = 0 17,1-28,9 %, 5-10 21,3-25,6 %, Farben 206-420 (FARBE_PRUEFUNG_v2_r2_sicht.md); (b1) 54,2 % (Leser A) | V2r2: 52,9 % / 1,4 % / 89 Farben: **ROT**; G4: 17,1 / 25,6 / 420: GRUEN |
| P2 | Posterisierung Render je Teil | dL = 0 (K5c-Teil) <= dL = 0 (G4-Quellteil) + 8 Punkte | - | - | `stich.py`: gleicher Weltmassstab, Licht 0,35 + 0,65 max(0, N.L), 3 Ansichten, Mittel | PORT-WAHL Pruefer, zwischen den Stichproben: Glieder -1,8 / +0,7 GRUEN, Kopf aus F1 +10,0 | Kopf aus G4-F1 (Probe): **ROT** |
| D1 | Texeldichte: lineare Vergroesserung je Teil | <= 2,0 | - | sqrt(3D-Flaeche K5c-Teil / 3D-Flaeche der G4-Quellflaechen) | - | Stichproben 1,20 / 1,22 GRUEN, Kopf 2,67 ROT (mit P2 +10); Leser A: alle Teile ausser dem Kopf <= 1,89. V1 A10 (0,5-2,7 x Koerpermittel) gilt weiter | Kopf aus G4-F1: **ROT** |
| S1 | Nahtflaechen: fremde Texel | <= 1 % je Teil; Sichtkontrolle: kein schwarzes Dreieck | - | Je Flaeche das UV-Dreieck rastern; Anteil Texel ausserhalb der G4-Inselmaske bzw. ausserhalb des eigenen Sonderfelds, flaechengewichtet | Tafel 3/4 vorn, Seite, 3/4 hinten | PORT-WAHL Pruefer; Leser A `V3A_4`: schwarze Dreiecke an Schulter und Hals bei 41 / 65 % Nahtflaechen | (b2)-Probe A Hals: ROT (gesehen) |
| B10-B17 | Binnenzeichnung (V2 2) | Familienspanne wie V2 | wie V2 | `farbe_lesung/N2_skripte/n2_riegel.py`, Kopffeld = Atlasfeld von Kopf 22 in der V3-Anordnung | wie V2 | MESSBLATT_FARBE_V2 2 | G4 selbst 8/8 GRUEN (`N2_riegel_familie.json`); V2r2 B12, B17 ROT |
| A1-A5, A7, A9, A11, A12 | wie V1 6 | wie V1 | wie V1 | Felder V3-Anordnung | - | V1 6 | - |
| A8 | Kiefer (Feld = Unterkiefer aussen + Band) | Zahn 2-20 %, Rot >= 20 %, Dunkel >= 20 % | wie V1 | wie V1 | - | V1 6; Bild 2 Unterkiefer rot 66 %, Zahn 12 % | - |
| Glanz | A5 + N2 | <= 0,20 % Atlas; <= 0,10 % Render | - | wie V1 | wie V1 | V1 A5/N2 | - |
| E9 | A11 Schwanz-Gliedsprung | <= 4,0 | Spitze frei | wie V1 | - | V1 A11 | - |
| K1v2, K2v2, K3-K10, K4b, B2, B9, B9b | Bild-1/2-Ketten | wie V1/V2 | wie V1/V2 | - | `riegel_farbe.kette_v2`, `p_riegel.kette_riegel` | V1 6, V2 3.2 | - |
| **K11** | Bild-1-Kette, Farbton der Rotklasse (a\* >= 12, C\* >= 15, L\* >= 20), Figur und Ruecken/Flanke | Figur <= +4,0 Grad; Ruecken <= +0,7 Grad | Rundlauf + 10 Grad | - | `rotton.py` (Pruefung v2 r2): Szenenrender -> video_filter Saat 15 -> p_zonen.rueck -> Farbton des Rotklassen-Mittels | Original -9,5 / -11,8; Rundlauf (3 Saaten) -6,0 / -9,3 (`rundlauf_ton.json`). Toleranz 10 Grad = PORT-WAHL Pruefer, das 2,4-fache der groessten Rundlauf-Verschiebung (+4,1 Grad Schwanz) | V2r2: +38,1 / +23,5 **ROT**; (b2)-Probe A: +24,8 Figur **ROT**; K5c heute -7,9 / -13,0 GRUEN |
| **K12** | Bild-1-Kette, Rotklasse je Zone | <= max(1,5 x Original, Original + 3 Punkte) | - | - | wie K11, je Nutzer-Zone (`p_zonen.regionen(1)`) | Original: Schwanz 5,9, Ruecken 11,9, Hinterbein l 1,9, r 1,1, Vorderglied 0,0 %; Faktor 1,5 wie V1 K4/K5 | (b2)-Probe A Schwanz 21,1 % (Grenze 8,9) **ROT** |
| **K13** | Bild-2-Kette, dunkle Klasse der Figur (T-L\* < 20) | >= 0,85 x Original 54,6 % = 46,4 % | - | - | wie K6, `kette_b2.py`-Masse | Faktor 0,85 wie V1 K1/K2 | (b2)-Probe A 17,2 % **ROT**; K5c heute 48,2 % GRUEN |
| **K14** | Bild-1-Kette, Stelle F1 (Maske `V3P_skripte`: F1_pruef = Regel 3.1) | Lab-Mittel dE <= 6 gegen den Rundlauf (45,0 / 16,6 / -2,0); Rotklassen-Anteil >= 0,85 x 69,2 % = 58,8 % | - | - | `ziel_kette.py` | Rundlauf 3 Saaten (`ziel_kette.json`); dE 6 wie V1 K3 | V2r2: (28,9 / 6,6 / 7,9), dE 21 **ROT**; (b2)-Probe A: (14,6 / 2,1 / 1,9), 0 % **ROT**; K5c heute: dE 6,7 ROT |
| **K15** | Bild-2-Kette, Fleischband (Maske band2_pruef = Regel 3.2) | Lab-Mittel dE <= 6 gegen den Rundlauf (49,9 / 28,2 / 16,1); Rotklasse >= 0,85 x 96,7 % = 82,2 % | - | - | `ziel_kette.py` | Rundlauf 3 Saaten; dE 6 wie V1 K7 | V2r2: (55,8 / 28,9 / 20,2), dE 7,2 **ROT**; (b2)-Probe A dE 14,9 **ROT**; K5c heute dE 12,6 ROT |
| F1t / F2t | Feldmittel der Sonderfelder (Atlas) | F1-Stelle T (149,103,122); Band T (184,90,85); Zaehne L\* >= 70, C\* <= 12 | dE <= 6 (V1 K3) | genutzte Texel des Felds, Lab-Mittel | - | Vorwaerts-Gegenprobe 3.1/3.2 | Orange/Gelb/G4-F8-Feuer: dE 23-66 ROT |

**Gegenproben laut Auftrag:**
- Versuch 2 Runde 2 ist bei Herkunft (H1 0 %), Posterisierung (P1 52,9 %) und Feuerfarbe (K11 +38,1 Grad, K14 dE 21, K15 dE 7,2) rot. Gemessen in `gegen_v2.json`; P1 aus der Sicht-Pruefung v2 r2.
- G4 selbst ist bei der Binnenzeichnung gruen: B10-B17 8/8 (`N2_riegel_familie.json`, V2 2), P1 gruen.

## 6. Darstellungsregel fuer die Tafeln

V1 7 gilt weiter. Fuer V3 kommt dazu:
1. **Nie ein Modell aufhellen**, ohne die Vergleichsmodelle gleich zu behandeln. Rohfassung zuerst; die Behandlung steht in der Beschriftung.
2. **Familienkachel:** EM30_HANGAR | RE2-G5 | RE2-G4 | K5c V3.
   - Gleicher Renderer (`form_buehne.rastern`), gleiches weisses Licht, gleiche Umkugel 0,46, drei Winkel, roh.
   - G4 und K5c V3 nebeneinander, damit man die Uebertragung sieht.
3. **Szenenlicht-Kacheln neben dem Screenshot**, je Bild drei Kacheln gleicher Groesse: Original | Neubau mit Spiellicht | Neubau durch `video_filter`.
   Dazu je eine Nahkachel der Feuerstelle:
   - Bild 1: x 270-410, y 200-300, 4-fach, wie `V3P_2`.
   - Bild 2: x 150-360, y 290-425, 3-fach, wie `V3P_3`.
4. **Stichprobenkachel je Teil:** G4-Quellteil | K5c-Teil mit G4-Texeln, gleicher Weltmassstab (wie `V3P_1`). So sieht man die Vergroesserung.
5. Knochen- und Klassenkarten nur als beschriftete Falschfarbe, nie als Bild des Modells.
6. Bilder erst zum Nutzer, wenn alle Riegel aus 5 gruen sind und ein zweiter Pruefer dieselben Zahlen erhalten hat.

## 7. Offen (nur, was kein Bild beantwortet)

1. **Gesicht oder Becken in Bild 1.** Mit der festen K5c-Geometrie liegt die Stelle, die der Nutzer als leuchtendes Gesicht liest, auf der Beckenoberseite.
   Gemessen sind 646/646 px auf Knochen 0, der Kopf ist verdeckt. Das Messblatt legt das Leuchten auf das Becken, das Gesicht leuchtet am Maul (Bild 2).
   Ob der Nutzer damit einverstanden ist, beantwortet kein Bild. Soll das Gesicht in Bild 1 selbst leuchten, muss sich die Pose oder die Geometrie aendern.
   Das schliesst der Auftrag aus (K5c bleibt). Rueckfrage an den Nutzer mit `V3P_2`.
2. **Gelber Saum am Unterkiefer** (345 px, Rundlauf dE 5,2): gelbliche Zahnbasis oder Feuerschein. Das Bild trennt es nicht; erlaubt, nicht verlangt.
3. **Die hellen, geclippten Flecken links unten in Bild 2** (969 px): Feuer oder Krallenspitzen.
   - K5c hat dort keinen Koerper (5 px) und kein Nutzer-Teil.
   - Geclippt heisst: keine Farbinformation.
   - Sie sind kein Malziel fuer V3.
4. **Rotanteil der dunklen Haut in Bild 2** (T (64,23,26)): Rot-Einstreuung von bis zu +21 e (farbe.md 4). Die Untergrenze der Hautroete bestimmt kein Bild.
   Die Kette (K6, K8, K13) entscheidet, kein Texelziel.
