# Sicht-Pruefung Farbe V3 Runde 3 (K5c, G4-Uebertragung), 2026-11-07

Pruefer: Sicht (Auge des Nutzers). Gegenstand: `farbe/K5c_farbe_v3/` = `farbe/K5C_FARBE_V3.EMD` (sha256 d19fe901...) / `.TIM` (abd0ba1b...),
bytegleich nachgeprueft; Commit b1ddd511e auf r41/form. Variante 15:13, Tafeln 01-08 15:16-15:25: die Tafeln zeigen diesen Stand.
Riegel: `farbe/riegel_K5c_farbe_v3.txt` (32/52 GRUEN). Vorrunde: `farbe/v3r2/`, `FARBE_PRUEFUNG_v3_r2_sicht.md`.

Eigene Ausschnitte und Messungen (nicht im Repo):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3/sicht_v3r3/`
- `b2_z1_v3_unten.png`, `b2_z1_orig_unten.png`, `b2_z2_unten_drei.png`, `b2_nah_drei.png`, `band_mass.py`
- `b1_z1_f1_drei.png`, `schulter_r2_r3.png`, `ruecken_r2_r3.png`, `spiel5080_k4_v3_r3.png`, `spiel5090_k4_v3_r3.png`

Seiten: (W) = aus Sicht des Wesens, (B) = Betrachter.

## Ergebnis

**Bestanden, mit kleinen Abweichungen.** Keine Abweichung, die der Nutzer nach meinem Urteil wieder ablehnen wuerde.
Der einzige Ablehnungsgrund der Runde 2 ist behoben: die Vorderklaue im Vordergrund von Bild 2.
- Runde 2 zeigte ein Mosaik aus beige-weiss-braunen Dreiecken, gefiltert ein rosa-weisses Schachbrett.
- Runde 3 zeigt dunkelrote Krallen mit khakifarbener Spitze und durchgehendem Laengsverlauf, wie die G4-Krallen in `03` (`b2_z1_v3_unten.png`).
- Gefiltert (`b2_z2_unten_drei.png`) liest sich die Klaue als dunkelrote Klaue, ohne Schachbrett.

Was ich zu widerlegen versucht habe und was nicht trug (Einzelheiten in der Tabelle):
1. **Maul in Bild 2 zu matt?** Im gefilterten Szenenbild ist das V3-Band schwaecher lesbar als das Original und auch als K4.
   - Grobe Messung im Bandstreifen der Nahkachel (`band_mass.py`; Hintergrund nicht sauber getrennt, nur Richtwert): L\* p95 Original 58,6, K4 55,1, V3 49,4.
   - Ort, Richtung, Zahnreihe und Mittelfarbe stimmen (K15 dE 5,2). Im Spielfenster 5090 ist das Band mit Zaehnen lesbar.
   - Darum "klein". Dies ist der Punkt mit dem groessten Restrisiko.
2. **F1 als aufgeklebtes Pflaster?** Roh und im Spielfenster 5080 ist F1 ein staubrosa Fleck mit treppiger, harter Kante. Neu ist ein grellroter Saum oben rechts, Pixel RGB (139,11,14), Lab (28,8 / 49,4 / 36,0).
   - Das Original hat roh keinen solchen Saum, sein buntestes Pixel ist magenta (124,43,103).
   - Gefiltert verschwindet der Saum. Flaeche 731 px (1,13-fach) und ein Streifen statt drei sind eine sichtbare Besserung.
   - Darum "klein".
3. **"Gesicht leuchtet in Feuerfarben"?** Unter neutralem Licht leuchtet nichts gelb-orange.
   - Das Leuchten von Bild 1 liegt nach Messblatt V3 Zeile 79 auf dem Becken, Kopf verdeckt.
   - Orange/Gelb fallen in der Gegenprobe ROT aus (dE 74,6 / 87,3).
   - Was das Original-Bild zeigt (rosa-magenta in Bild 1, lachsrot am Maul in Bild 2), gibt die Variante am richtigen Ort wieder.
   - Ob der Nutzer das Becken als "Gesicht" gelten laesst, beantwortet kein Bild: Rueckfrage Messblatt V3 7.1, unveraendert offen.

## Befunde je Punkt

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| G1 Koerper schwarz-rot verbrannt, Machart (`03`, `05`) | Schwarze Haut mit roten Adern als Linien, G4-Machart, kein Flickwerk, kein Glanz | `03`: in allen drei Winkeln schwarz mit dunkelrotem Adernetz in PSX-Dichte. Aus Familienabstand gelesen kein Flickwerk, keine Posterisierung. Glanz L\*>85 0,00 % (G4 0,09-0,21 %, K4 0,85-1,33 %). Figur-L\* 9,9-11,6 gegen G4 17,0-21,6 und K4 17,5-19,9, also etwa halb so hell wie die Familie (N1 ROT, C\* 7,1-9,1 gegen EM30 10,0-12,5). Die Dunkelheit folgt aus G1 "schwarz rot" | wie Spielfamilie/Vorlage (Helligkeit: Abweichung klein) |
| Binnenzeichnung Schulter/Brust nah (`08` Z.2, `schulter_r2_r3.png`) | Zusammenhaengende Adern wie G4, keine Fremdstriche | Unveraendert gegen Runde 2: dichtes rotes Krakelee, an Dreieckskanten abgebrochen, dichter und vielrichtiger als G4 (B17 0,541 ROT, Familie 0,616-0,680). Der linke senkrechte rosa Strich ist weg. **Der lange waagerechte rosa Strich unter dem Schultergrat ist noch da** (`schulter_r2_r3.png`, rechts Mitte). In Spielgroesse (`03`, `05`) liest es sich als Adernetz | Abweichung klein |
| Flanke und Ruecken nah (`08` Z.1/Z.3, `ruecken_r2_r3.png`) | wie G4 | Flanke fast ungezeichnet dunkel. Am Ruecken ist F1 kleiner (rosa px im Ausschnitt 146 -> 74), Lab (46,3 / 13,1 / -1,9). Dazu kommt jetzt ein roter Saum an einer Seite | Abweichung klein |
| Vorderklaue Vordergrund Bild 2 (Ablehnungsgrund Runde 2) | G4-Krallen-Machart. Im Szenenbild wie das Original: lange dunkelrote Klaue, kein Mosaik | `02` Z.1 (`b2_z1_v3_unten.png`): dunkelroter Ansatz, khakifarbene Spitze mit dunklen Querschlieren, je Kralle ein durchgehender Streifen. Kein Mosaik. `03`: dieselbe Machart wie die G4-Krallen (heller Spitzenkegel). Z.2 gefiltert: dunkelrote Klaue, kein Schachbrett. K9 Klauen a\* 30,6/31,4 gegen Original 22,2/24,1 (GRUEN). Rest: Die Spitzen sind 1,7- bis 2,9-fach gedehnt und wirken leicht gebaendert (P2 Klauen +16,3/+21,0 ROT). Die weichen weissen Flecken des Originals oben liegen nach Messblatt V3 3.2/7.3 auf keinem K5c-Koerper (5 von 969 px). Das ist eine Frage der Pose, nicht der Farbe | Abweichung klein (Ablehnungsgrund behoben) |
| Haende (Hand 18), neutral und Bild 2 | G4-dunkelrot | `02` Z.1 links: weiterhin eine flache mauvefarbene Flaeche mit Laengsschlieren. `03` 3/4 vorn: die Vorderpfoten rosa-lila. Ursache sind die Bild-1-Zeilen auf Hand 18 (Dossier V3R3.5 Punkt 4; mit G4-Zeilen faellt K15 auf ROT) | Abweichung klein |
| Kopf Machart (`06` Z.3-5) | G4-Machart, gemalte Struktur, keine Kleckse | Schwarze Haut mit kraeftigen roten Aderlinien, PSX-weich, Glanz 0,00 %. Die Linien sind breiter und dichter als auf der G4-Hautquelle F18 (`06` rechts; B12 2,62 / 17,26 ROT). Das liest sich als glimmende Brandrisse und passt zu "verbrannt". Keine hartkantigen Kleckse | Abweichung klein |
| Maul neutral (Gefahr "Pokemon") | Kiefer nicht cremefarben, kein aufgemalter Rahmen | Unterkiefer dunkel mit roten Adern, darunter eine lachsrosa Lippe und weisse Zaehne. Keine Creme, kein Rahmen. In der Seitenansicht (`06` Z.4) bleibt der kleine rosa Zungenzipfel vorn | Abweichung klein |
| G2 Bild 2: Feuerfarbe am Maul an den hellen Stellen (`02` Nahkachel, `06` Z.1, `b2_nah_drei.png`) | Band an der Original-Stelle, hell-rosarot mit hellen Punkten | **Ort und Richtung** stimmen, mit Zahnreihe. **Mittelfarbe** K15 (50,9 / 30,7 / 20,6), dE 5,2 (Runde 2: 12,2). In `06` Z.1 (Kette T) ein helles rosa Band mit Zaehnen, nah am Original. **Gefiltert** (`02` Nahkachel) ist das Band matter als Original und K4. Die hellen Punkte fehlen weitgehend: L\* p95 49,4 gegen 58,6 (Original) und 55,1 (K4); Pixel mit L\* >= 60: 307 gegen 2547 und 944 (Richtwert, Streifenmaske mit Hintergrundanteil). **Spielfenster 5090** (`spiel5090_k4_v3_r3.png`): Band mit Zaehnen lesbar, Kopf mit glimmenden Rissen | Abweichung klein (groesstes Restrisiko) |
| G2 Bild 1: Leuchtstelle F1 (`01` Z.1-3, `b1_z1_f1_drei.png`, `spiel5080_k4_v3_r3.png`) | Ort und Form wie Original: ein schraeger rosa-magenta Streifen, weich | **Ort:** stimmt. **Flaeche:** 731 px (1,13-fach, IoU 0,75). Runde 2: 941 px, 0,59. **Gefiltert** (`01` Z.2/Z.3): ein schraeger rosa Streifen mit hellem Kern, dem Original sehr nah. K14 dE 3,1. **Roh und im Spiel:** staubrosa Fleck mit treppiger, harter Kante und grellrotem Saum oben rechts (RGB 139,11,14). Das Original hat roh keinen roten Saum (buntestes Pixel magenta 124,43,103). K4 hat hier eine weiche Raute. Im Spielfenster 5080 ist F1 als blasse lila Stelle mit rotem Rand gut zu sehen | Abweichung klein |
| G2 "Gesicht leuchtet" in Bild 1 | Nutzer: das Gesicht vorne leuchtet | Die Stelle liegt auf dem Becken (Knochen 0, 646/646 px, Messblatt V3 Zeile 79); der Kopf ist in der Pose verdeckt. Feuerfarben im Sinne Gelb-Orange gibt es nirgends. Gemessen ist rosa-magenta, die Gegenprobe Orange/Gelb ist ROT | Abweichung klein; Rueckfrage Messblatt V3 7.1 bleibt offen |
| Augen dunkel (E24) | dunkel | `06`: keine hellen Augen, Oberkiefer dunkel; K10 L\*max 13,0 / 29,9 GRUEN | wie Spielfamilie/Vorlage |
| Szene Bild 1 Koerper (`01`, `07`) | Dunkle Figur im blauen Licht, Adern lesbar | Gefiltert dunkelviolett wie das Original. Den Beinen fehlen die blaeulichen Glanzstellen (K3 Hinterbein r dE 10,5 ROT). Im Spielfenster 5080 eine fast schwarze Figur mit lesbaren roten Adern an Beinen und Schulter | Abweichung klein |
| Szene Bild 2 Koerper (`02`) | Dunkel mit roten Konturen | Kopf und Rumpf dunkel mit roten Adern. Etwas zu dunkel: K6 dE 6,8, Figur L\* 21,0 gegen 25,1 | Abweichung klein |
| Posterisierung, Atlas (`04`) | Keine Stufen wie in V2, keine Foto-Fragmente | G4-Texel in G4-Machart, Herkunftskarte schluessig. P1 GRUEN. Posterisierung nur an den gedehnten Krallenspitzen (P2, siehe oben) | wie Spielfamilie/Vorlage |
| Schwanz aus einem Guss (E9) | ohne Teilstuecke | `05` Seite: ein Zug. K12 Schwanz 6,1 % (Grenze 8,9) | wie Spielfamilie/Vorlage |
| Gesamteindruck: nicht "Pokemon", nicht "billig einfarbig", nicht "hoeher aufgeloest", Wunden fleischig | passt zur Familie | `03`/`05`: ein schwarz-roter, verbrannter Vierbeiner mit Brandriss-Adern in PSX-Dichte. Nicht einfarbig, nicht feiner aufgeloest als G4, kein Comic-Schema. "Fleischige Wunden" im Sinne EM30/G5 hat er nicht; das hat der Nutzer mit "schwarz rot ... verbrannt" so gewollt. Leicht comichaft wirken nur der Zungenzipfel und die rosa-lila Pfoten | wie Spielfamilie/Vorlage |
| Fairness der Tafeln | Gleiches Licht und gleiche Behandlung; Szenenkacheln neben dem Screenshot | `01`/`02`: K4 und V3 unter gleichem Disc-Licht x k, Z.2 mit gleichem Filter (Saat 15), Original daneben. `03`/`05`/`06`/`08`: gleicher Renderer, 0,55 + 0,45 N.L, roh, nichts einseitig aufgehellt. `07`: Zeilenbeschriftung jetzt "PrintWindow", fuer K4 und V3 gleich. Die Kopfzeile von 07 sagt noch "echtes Fenster" (sachlich richtig). Hinweise: In `03` hat V3 15 000-17 300 px, G4 35 000-39 000 px; das verbirgt eher Blockigkeit, `06`/`08` gleichen es aus. Der Kandidat laeuft mit 3 Clips, K4 mit 28 (Spiellauf-Log) | fair |

## Was ein weiterer Lauf noch verbessern koennte (nur Befund, keine Konstanten)

- **Maul Bild 2:** Die hellen Punkte im Band fehlen im gefilterten Szenenbild. Mittelfarbe passt, Spitzenhelligkeit nicht (L\* p95 49 gegen 59, Richtwert).
- **F1:** Den grellroten Saum pruefen. Laut Dossier V3R3.1 soll der Rand die "dunklere Haelfte des Fleischs" sein, gerendert ist er grellrot (139,11,14). Die Kante ist roh treppig, K4 hat dort eine weiche Raute.
- **Waagerechter rosa Strich an der Schulter** (`08` Z.2): noch sichtbar. Wahrscheinlich liegt er auf einer Bild-1-Flaeche von Mesh 16.
- **Hand 18 mauve, Zungenzipfel, Helligkeit der Figur** (L\* rund 10 gegen G4 17-22): wie Runde 2.
- **Rueckfrage an den Nutzer** (Messblatt V3 7.1): Ist ihm das Leuchten am Becken (Bild 1) und am Maul (Bild 2) recht, oder soll das Gesicht selbst leuchten? Letzteres geht nur mit anderer Pose oder Geometrie.
