# Sicht-Pruefung Farbe V3 Runde 4 (K5c, weniger Rot, Feuerrillen entfernt), 2026-11-07 Abend

Pruefer: Sicht (Auge des Nutzers). Gegenstand: `farbe/K5c_farbe_v3/` = `farbe/K5C_FARBE_V3.EMD` (sha256 07d679a57254b56e...) / `.TIM` (512828067ea7f048...),
bytegleich nachgeprueft gegen `bau_v3r4.log` (sha256-Zeile) und `spiel_lauf_v3.log`; Commits b6ed73526 / bf3b8a765 / 604f460d2 auf r41/form.
Tafeln 01-09 (19:42-19:52) zeigen diesen Stand (Bau 19:41). Runde 3 zum Vergleich: `farbe/v3r3/` (d19fe901 / abd0ba1b).
Riegel: `farbe/riegel_rot_K5c_farbe_v3.txt` (RV 45/45), `farbe/riegel_K5c_farbe_v3.txt` (76/93). Vorrunde: `FARBE_PRUEFUNG_v3_r3_sicht.md`.

Massgebender Nutzersatz (2026-11-07 ~17:10): "entfernst die uebertriebenen roten feuerrillen wieder, und stellst es wieder nach vorlage um?"
Nutzerurteil zu Runde 3: "zu viel rot im Vergleich zur Vorlage, aber immerhin bedrohlich".

Eigene Ausschnitte und Messungen (nicht im Repo):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe4/sicht_v3r4/`
- `a_krallen_z1_r3_r4.png`, `a_krallen_z2_r3_r4.png`, `a_krallen_orig.png`, `f_spiel5090_klaue_r3_r4.png`, `dbg_kasten.png` (Krallen)
- `b_familie_seite/vorn/hinten_r3_r4.png`, `e_rundum_seite/vorn_r3_r4.png` (weisses Licht)
- `c_schulter_r3_r4.png`, `c_flanke_r3_r4.png` (Binnenzeichnung nah)
- `d_spiel5080_lage_r3_r4.png`, `d_spiel5080_ruhe_r3_r4.png`, `d_spiel5090_pose_r3_r4.png` (Spielfenster)
- `mess_sicht_v3r4.txt` (alle Zahlen unten; Lab D65, OpenCV float, Farbtonklassen violett h -90..-15, mauve/rot -15..15, braun 15..80, neutral C* <= 6)

Seiten: (W) = aus Sicht des Wesens, (B) = Betrachter.

## Ergebnis

**Nicht bestanden, ein Punkt der dritten Klasse: die Vorderkrallen sind gesprenkelt.** Alles, was der Nutzersatz verlangt, ist erfuellt:
- **Feuerrillen weg.** In keiner Tafel (01-09) gibt es noch helle rote Linien. Adern sind nur noch dunkle Struktur (`08`, `05`, `03`).
- **Rot nach der Vorlage.** `09`: Bild 1 Schwanz 23,4 % wie die Vorlage 23,5 % (Runde 3 43,5 %), verstreute kleine Flecken statt des roten Blocks; Figur-Farbton -22,4 Grad
  (Vorlage -15,8, Runde 3 -0,2); Bild 2 Ruecken -8,9 Grad (Vorlage -5,3, Runde 3 30,6), Hals 26,4 % (Vorlage 45,1, Runde 3 87,1). Mit dem Auge: In `09` Bild 1 liest sich
  Runde 4 neben der Vorlage als dieselbe dunkle Figur mit rosa-mauve Tupfen; in Bild 2 ist der Koerper dunkel karmin ohne Linienzeichnung, wie das Video.
- **Bedrohlich.** Figur-L\* 7,9-9,4 unter weissem Licht (Runde 3 9,9-11,6), Glanz 0,00 %, keine Comic-Konturen, Maul mit Zaehnen, dunkle Augen.

Was ich zu widerlegen versucht habe:
1. **Krallen (Bild 2, Vordergrund).** Runde 3: dunkelrote Klaue mit khakifarbener Spitze, ein durchgehender Laengsverlauf je Kralle (`a_krallen_z1_r3_r4.png` links).
   Runde 4: schwarze Klaue mit verstreuten hellen khaki Tupfen ueber die ganze Laenge, wie ein geschecktes Fell (`a_krallen_z1_r3_r4.png` rechts, `dbg_kasten.png`).
   Gemessen (Pixel heller als das Umfeld um > 10 L\*, Zusammenhangskomponenten): linke Vorderklaue Runde 3 426 px / 53 Komponenten, Runde 4 **1099 px / 83 Komponenten**;
   rechte Hand 136 / 26 gegen **664 / 58**. Das Original hat an der Klaue 161 px / 25 Komponenten (0,3 %), also eine glatte Klaue.
   Im **Spielfenster 5090** (`f_spiel5090_klaue_r3_r4.png`, gdigrab, ohne Filter) ist die Vorderklaue grau-schwarz gescheckt mit khaki Punkten (736 px / 47 Komponenten
   gegen 503 / 41, Feuer in beiden Kaesten gleich). Gefiltert (`02` Z.2) verschwinden die Tupfen weitgehend, der Nutzer sieht aber das Spielfenster.
   Ursache nach Dossier V3R4.3: Krallen liegen in der Gruppe Vorderbeine; die dunkelrote Wurzelzone (G4-Adern) ist gestaucht und dunkel, die hellen Spitzen (L\* >= 52,4)
   bleiben G4 (A7 0,349 gegen 0,096, P2 Klauen +39,8 / +50,4 ROT). Die Sicht-Pruefung Runde 2 hat die Vorderklaue schon einmal als Ablehnungsgrund gefuehrt (Mosaik);
   Runde 3 hatte das behoben. Runde 4 macht daraus ein anderes, aber ebenso sichtbares Muster. **Dritte Klasse.** Das Dossier (V3R4.5 Punkt 2) nennt den Weg: eigene
   CLUT-Zeile fuer die Krallen mit der Runde-3-Farbe; das aendert nur RV Bild 2 Vorderbeine und ist eng begrenzt.
2. **Violetter Rumpf neben braunen Vorderbeinen (weisses Licht und Spielfenster 5080).** Unter weissem Licht (`03`, `05`) ist Runde 4 zweifarbig: Rumpf, Flanke und Schwanz
   violett (Farbtonklasse violett 31-38 % der Figurpixel, h -34..-36 Grad, L\* 6-7; Runde 3 1,0-1,5 %), Vorderbeine, Brust und Hals braunrot (11-21 %, h 35-36 Grad).
   Im **Spielfenster 5080 Schnitt 1** (`d_spiel5080_lage_r3_r4.png`, echtes Fenster) dasselbe: violett 28,7 % (h -39,3, L\* 4,5), braun 33,9 % (h 35,8, L\* 7,6), neutral 37,3 %;
   Runde 3 dort 61,7 % neutral schwarz, 17,4 % mauve/rot, 2,1 % violett. Der braune Vorderteil ist nicht neu (Runde 3 18,9-21,5 %, G4-Fleisch), neu ist das Violett statt Schwarz.
   Quelle: `generator/v3_daten/r4_par.json` Rumpf Flanke h -42,78 / C 48,6, Vorderbeine h +39,85 / a 1,44 (Koordinatensuche gegen RV1-RV4). Der Bauer schreibt selbst
   (V3R4.4): "Die Flanke wirkt unter weissem Licht violetter als 'mauve-rosa'".
   Dagegen spricht fuer "klein": Die Vorlage Bild 1 ist selbst blau-violett getoent, die Bild-1-Kette trifft (Figur -22,4 im Fenster -30,6..-1,0, RV2 GRUEN), und bei L\* 4-8
   ist beides fast schwarz; im Spielfenster bei 1x liest es sich als sehr dunkles Aubergine mit dunkelbraunem Vorderteil, nicht als Lila-Tier. Unter dem Bahnsteig-Licht
   (`01` Z.1/Z.2) und im Feuer (`02`) ist der Unterschied nicht zu sehen. **Abweichung klein, aber das groesste Restrisiko nach den Krallen**: Der Nutzer hat "schwarz rot"
   gesagt; wenn er die Form in 5080 Schnitt 1 als violett liest, lehnt er das ab. Zahlen liegen vor, er kann es im Spiel selbst sehen.
3. **Ist es noch bedrohlich oder ausgewaschen?** Nicht ausgewaschen: L\* 7,9-9,4, C\* 9,6-10,4 (EM30 10,0-12,5). Der Schrecken kommt aus Dunkelheit und Form, die glimmenden
   Risse fehlen (vom Nutzer so verlangt). In `02` Z.2 steht die Figur als schwarzer Block mit karminrotem Schimmer vor dem Feuer, wie das Video. Trug nicht.
4. **"Pokemon" / Comic?** Keine Konturlinien, kein Glanz, keine Flaechenfarben. Der rosa Zungenzipfel (`06` Z.4) und der Maulrand sind wie Runde 3. Trug nicht.
5. **Fairness der Tafeln.** `09`: Vorlage = Video roh, Runde 3 und 4 = gleiches Spiellicht + Videofilter Saat 15, gleicher Ausschnitt, Messregion eingezeichnet; das ist die
   Messkette des Nachtrags, nicht einseitig. `01`/`02`/`03`/`05`/`06`/`08` wie Runde 3 (gleiches Licht, roh). `07` gdigrab fuer K4 und V3. Fair. Hinweis: `01`/`02` Z.1 sind
   laut Kopfzeile heller als das Spielfenster; die Krallen-Tupfen sieht man trotzdem auch in `07`.

## Befunde je Punkt

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| Nutzersatz: Feuerrillen entfernt (`03`, `05`, `08`, `02`) | Keine hellen roten Adern; Adern hoechstens dunkel, nie heller/roter als die Haut | In allen Tafeln weg. `08` Schulter/Flanke/Ruecken: Adern als dunkle Linien in der Haut. `02` Z.1/Z.2: kein roter Linienzug mehr am Kopf und Ruecken (Runde 3 deutlich). RV6 alle 8 Regionen dL\* -2,3..-7,7 | wie Vorlage |
| Rotmenge und Verteilung Bild 1 (`09` Z.1-6, `01`) | Verstreute kleine Flecken, Schwanz 23,5 %, groesste Rotflaeche 2,3 % | Schwanz 23,4 %, kleine rosa-mauve Tupfen statt des Blocks (Runde 3 43,5 %); groesste Rotflaeche 2,6 %. Mit dem Auge: Runde 4 neben der Vorlage dieselbe dunkle Figur mit Tupfen; die Tupfen sind einen Hauch pinker und punktfoermiger als das weiche Mauve des Videos | wie Vorlage (Tupfenform: Abweichung klein) |
| Farbton Bild 1 (`09`) | mauve-rosa, -15,8 Grad (Fenster -30,6..-1,0) | -22,4 Grad, Flanke -21,4 (Vorlage -19,5), Hinterbeine -30,1 (Vorlage -30,4). Runde 3 -0,2 / +5,8 (orange-rot) | wie Vorlage |
| Rot Bild 2 (`09` Z.7-12, `02`) | Ruecken 53,6 %, Hals 45,1 %, Farbton -5,3 .. 22,8 Grad; Koerper dunkel karmin ohne Linien | Ruecken 58,5 % / -8,9 Grad, Hals 26,4 % / 18,6, Figur 53,9 % / 24,6 (Runde 3 70,2 / 29,7). `02` Z.2: schwarzer Koerper mit karminrotem Schimmer, Maulband lesbar. Etwas flacher als das Video, dem am Rand das Feuer rote Saeume gibt | wie Vorlage |
| Vorderkrallen Bild 2 und Spiel 5090 (`02` Z.1, `07` Z.4, `a_krallen_*`, `f_spiel5090_*`) | Glatte dunkle Klaue mit heller Spitze, wie Original (0,3 % Tupfen) und wie Runde 3 | Gescheckt: schwarze Klaue mit khaki Tupfen ueber die ganze Laenge. Linke Klaue 1099 px / 83 Komponenten (Runde 3 426 / 53), rechte Hand 664 / 58 (136 / 26). Im Spielfenster 5090 grau-schwarz gescheckt (736 / 47 gegen 503 / 41). Gefiltert kaum sichtbar, roh und im Spiel deutlich. Runde 2 wurde fuer eine Klauen-Stoerung an derselben Stelle abgelehnt; das hier ist ein neues Muster derselben Art | **Abweichung, die der Nutzer wieder ablehnen wuerde** |
| Grundton unter weissem Licht und 5080 Schnitt 1 (`03`, `05`, `07` Z.2/3, `b_*`, `d_spiel5080_*`) | schwarz-rot verbrannt, eine Haut aus einem Guss | Rumpf/Flanke/Schwanz violett (31-38 %, h -34..-39 Grad, L\* 4,5-7), Vorderbeine/Brust/Hals braunrot (11-34 %, h 35-36 Grad), Rest schwarz. Runde 3: 62-76 % schwarz, braun 14-21 %, violett 1-2 %. Bei L\* 4-8 fast schwarz, in `01`/`02`-Licht nicht sichtbar; in 5080 Schnitt 1 als sehr dunkles Aubergine mit braunem Vorderteil zu sehen | Abweichung klein (groesstes Restrisiko nach den Krallen) |
| Binnenzeichnung Schulter nah (`08` Z.2, `c_schulter_r3_r4.png`) | Zusammenhaengende Adern wie G4, keine Fremdflaechen | Adern dunkel in violetter Haut. Der waagerechte Streifen unter dem Schultergrat ist noch da, jetzt als brauner, laengsgestreifter Fleck statt rosa (Vorderbein-Zeile auf einer Mesh-16-Flaeche). In Spielgroesse (`03` Seite) ein brauner Fleck an der Schulter | Abweichung klein |
| Flanke nah (`08` Z.1, `c_flanke_r3_r4.png`) | wie G4 | Grosse violette Flaeche, oben links graue und braune Fragmente (Hinterbein- und Vorderbein-Zeilen angrenzender Flaechen). Nah als Flickwerk lesbar, in Spielgroesse nicht | Abweichung klein |
| Leuchtstelle F1 Bild 1 (`01` Z.1-3, Nahkachel, `06` Z.2) | wie Runde 3 (Nutzerfrage offen), ohne roten Saum | Ort, Form und Farbe wie Runde 3; der grellrote Saum von Runde 3 ist weg (Baufehler A 11 -> 0 Texel), roh jetzt ein rosa Fleck mit dunkler Haut ringsum | wie Vorlage |
| Maulband Bild 2 (`02` Nahkachel, `06` Z.1) | wie Runde 3 | Unveraendert: Band mit Zaehnen am richtigen Ort, gefiltert matter als Original und K4 (Runde-3-Befund, L\* p95 49 gegen 59) | Abweichung klein (wie Runde 3) |
| Kopf (`06` Z.3-5) | G4-Machart, keine Kleckse, dunkle Augen | Dunkelviolett-braune Haut mit dunkler Fleckung, keine Risse; Kiefer dunkel, Lippe lachsrosa, Zaehne weiss; Augen dunkel. L\* 12,3-14,9 (Runde 3 ~13) | wie Spielfamilie |
| Familie (`03`) | Passt neben EM30/G5/G4 | Dunkler als alle (L\* 7,9-9,4 gegen G4 17-22), Buntheit wie EM30. Farbton violett statt Fleischrot (A4 5,0 Grad ROT); die Vorlage verlangt mauve, nicht G4-Rot (Nutzer "nach vorlage"). Machart (Texelgroesse, Adern als Struktur) wie G4 | Abweichung klein |
| Gesamteindruck Spielfenster (`07`) | dunkle, bedrohliche Figur, kein Comic, kein Flickwerk | 5080: fast schwarze Figur, violetter Schimmer, F1 als blasse Stelle. 5090: schwarzer Koerper im Feuer, Band mit Zaehnen, gescheckte Klaue vorn (siehe oben) | wie Vorlage, ausser Klaue |
| Fairness der Tafeln | gleiches Licht, gleiche Kette, Original daneben | `09` neu und fair (Kette des Nachtrags, eingezeichnete Messregion, Werte in der Unterschrift). Uebrige wie Runde 3. Hashes der Variante in Bau-Log, Spiellauf-Log und Dateien gleich | fair |

## Was der Bauer noch tun muesste (nur Befund, keine Konstanten)

- **Krallen:** eigene CLUT-Zeile fuer die Krallenflaechen mit der Runde-3-Krallenfarbe (Dossier V3R4.5 Punkt 2), danach `02` Z.1 und `07` 5090 neu und die Tupfen-Zahl gegen Runde 3 pruefen (Ziel: nicht mehr Komponenten als Runde 3, 53 / 26).
- **Violett:** keine Pflicht aus dem Nutzersatz, aber dem Nutzer mit `d_spiel5080_lage_r3_r4.png` oder `07` Z.2 zeigen und fragen, ob das dunkle Aubergine in 5080 Schnitt 1 fuer ihn "schwarz rot" ist; falls nein, Flanken-Farbton naeher an die Bild-1-Figur (-15,8) statt -42,8 und Buntheit kleiner.
- Schulterfleck und Flanken-Fragmente wie Runde 3: Zeilenzuordnung der Mesh-16-Flaeche pruefen.
- Rueckfrage Messblatt V3 7.1 (Becken oder Gesicht) bleibt offen.
