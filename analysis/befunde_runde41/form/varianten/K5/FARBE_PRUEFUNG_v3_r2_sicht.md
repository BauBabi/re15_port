# Sicht-Pruefung Farbe V3 Runde 2 (K5c, G4-Uebertragung), 2026-11-07

Pruefer: Sicht (Auge des Nutzers). Gegenstand ist `farbe/K5c_farbe_v3/`, Stand Commit 8a6dd0fe3/684d3c7f0 auf r41/form:
- = `farbe/K5C_FARBE_V3.EMD` (sha256 32b48eee...) und `.TIM` (sha256 2bddb092...), bytegleich nachgeprueft.
- Die Tafeln 01-08 sind 13:43-13:45 entstanden, die Variante 13:37. Die Tafeln zeigen also diesen Stand.

Riegel des Baus: `farbe/riegel_K5c_farbe_v3.txt` (36/52 GRUEN). Vergleich mit Runde 1: `farbe/v3r1/`, `FARBE_PRUEFUNG_v3_r1_sicht.md`.

Eigene Ausschnitte und Messungen (nicht im Repo) liegen unter
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3/sicht_v3r2/`:
- `b2_z1_unten_orig.png`, `b2_z1_unten_k4.png`, `b2_z1_unten_v3.png`, `b2_z2_klaue.png`
- `b1_f1_roh_k4_v3.png`, `fam_r0.png` bis `fam_r2.png`, `b08_schulter.png`
- `klaue_v3.png`, `kopf_v3.png` (beide mit `r2/blick.py` des Bauers, Licht 0,35 + 0,65 N.L)
- `kopf_r1_r2.png`, `spiel5090_k4_v3.png`

Seiten: (W) = aus Sicht des Wesens, (B) = Betrachter.

## Ergebnis

**Nicht bestanden.** Ein Punkt wuerde vom Nutzer wieder abgelehnt, und zwar unveraendert aus Runde 1: **die Vorderklaue im Vordergrund von Bild 2.**
- Sie ist weiterhin ein Mosaik aus grossen beigen, weissen und braunen Dreiecken.
- Durch den Videofilter wird daraus ein rosa-weisses Schachbrett.
- Das Original zeigt an dieser Stelle weiche weisse Flecken und eine dunkelrote Klaue.

Was Runde 2 sichtbar besser macht als Runde 1:
- **Kopf.** Die schwarze Kuppel mit den verschmierten, hartkantigen Rotflecken ist weg. Jetzt liegen gemalte rote Aderlinien gleichmaessig auf schwarzer Haut (`kopf_r1_r2.png`).
- **Schulter und Brust.** Lachsfarbene Polster und flache rote Keile sind weg. Es sind Aderlinien auf Schwarz (`b08_schulter.png`).
- **Koerper insgesamt.** Er ist schwarz mit dunkelblutroten Adern, also "verbrannt". In `03` passt er von Machart und Massstab zur Familie.
- **Rotton.** Unter weissem Licht liegt der Farbton der Rotpixel bei 25-27 Grad (`03`, eigene Messung). Das ist G4 (29-31) und EM30 (26-27) nahe, weder rostrot noch magenta.

Drei Punkte aus Runde 1 sind **nicht** behoben. Sie bleiben in der Klasse "klein":
- Die F1-Stelle ist zu gross und gestreift.
- Das Maulband ist im Szenenbild zu rot.
- An den Flaechengrenzen sitzen noch rosa Strichel.

## Befunde je Punkt

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| G1: Koerper schwarz-rot verbrannt (Rumpf, Beine, Schwanz) | Schwarze Haut mit roten Adern als Linien, wie G4 in `03` | **Bild:** In `03` sind alle drei Winkel schwarz mit dunkelroten Aderlinien (`fam_r0.png`). Kein Fleck- oder Flickwerk. **Eigene Messung je Kachel** (Rotklasse C\* >= 20, a\* > 15), V3 gegen G4: Anteil 17,2/23,7 %, 13,1/15,8 % und 12,1/22,4 %. G4 enthaelt dabei Auge und Fleisch. Farbton 25-27 gegen 29-31 Grad. Rot-L\* 19-20 gegen 29-34, das Rot ist also dunkler und leuchtet nicht. **Helligkeit:** Figur-L\* 10,4-12,3, G4 17,0-21,6, K4 17,5-19,9. Die Figur ist gewollt schwarz, aber dunkler als die ganze Familie (N1 ROT) | wie Spielfamilie/Vorlage (die Dunkelheit folgt aus G1 "schwarz rot") |
| G1: Szenen Bild 1, Spiel 5080 | Dunkle Figur im blauen Licht, rote Adern lesbar | `01` Z.2: dunkelviolett mit Adern. Den Beinen fehlen die blaeulichen Glanzstellen des Originals (K3 Hinterbein r dE 10,5 ROT). `07` und `spiel_v3/spiel_pose1_nachstellung_r5080.png`: Im Spielfenster ist die Figur eine fast schwarze Silhouette, die Adern sind kaum zu sehen. K4 zeigt mehr Oberflaeche | Abweichung klein |
| Binnenzeichnung Schulter/Brust nah (`08` Z.2) | Zusammenhaengende Adern wie G4 | **Gegenueber Runde 1:** Aderlinien statt Polster und Keile. **Gegenueber G4:** dichter und in mehr Richtungen. Rotklasse im Ausschnitt 20,2 % gegen G4 14,8 % (die G4-Kachel enthaelt einen Teil des Auges). Riegel B17 Render 0,534; Familie 0,616-0,680, V1 r3 0,561. **Eindruck:** wie Lava-Krakelee; bei Spielgroesse (`03`, `05`) liest es sich als Adernetz. Noch sichtbar sind 2-3 waagerechte rosa Strichel an Flaechengrenzen (`b08_schulter.png` oben Mitte). Das ist der Befund aus Runde 1 | Abweichung klein. B17 ist als einziger Riegel ROT; nach Messblatt V2 Zeile 198 entscheidet dann die Sicht: Linien, keine Flecken |
| Flanke und Ruecken nah (`08` Z.1/Z.3) | wie G4 | Die Flanke ist fast ungezeichnet dunkel (Rotklasse 1,4 % gegen G4 3,3 %). Der Ruecken ist dunkel mit feinen Magenta-Strichen und einer rosa Pflaster-Stelle (F1) | Abweichung klein |
| **Vorderklaue im Vordergrund von Bild 2** | G4-Klauen-Machart. Im Szenenbild wie das Original: weiche weisse Flecken oben, lange dunkelrote Klaue | **Bild** `02` Z.1 (`b2_z1_unten_v3.png`): Die grosse Klaue vorn links (B) ist ein **Mosaik aus grossen beige-weiss-braunen Dreiecken**, wie in Runde 1. Daneben ist die Hand eine flache mauvefarbene Flaeche mit Laengsschlieren. **Ursache:** gedehnte Abtastung ("laengs einmal"; D1 Klauen Anteil > 2,0 bei 16,9 %; P2 Klauen +9,1/+8,7). **Gefiltert** Z.2 (`b2_z2_klaue.png`): rosa-weisses Schachbrett. **Messung** in Kachel x 60-240, y 330-480, Z.2: Original L\* > 75 bei 15,6 % (weiche weisse Flecken), V3 0,0 %. a\* 41,4 gegen 26,3. Weder Helligkeit noch Form der Original-Stelle werden erreicht, und das Muster faellt im Vordergrund auf | **Abweichung, die der Nutzer wieder ablehnen wuerde.** Der Befund aus Runde 1 ist unveraendert. Der Bauer meldet in V3R2.1 "Krallen: dunkelroter Ansatz, eine helle Spitze". Das stimmt in der Kleinansicht `05`, nicht im Vordergrund von Bild 2 |
| Haende im neutralen Licht | Beide Haende G4-dunkelrot | `klaue_v3.png` az 45: Hand A Lab (18,6 / 9,8 / -0,7), Farbton 0 Grad, mauve-lila. Hand B (19,7 / 12,7 / 7,1), 21 Grad, rot. In `03` 3/4 vorn ist die vordere Pfote sichtbar lila. Ursache sind die Bild-1-Zeilen (Magenta) am Vorderglied | Abweichung klein |
| **Kopf** (Machart) | G4-Machart, gemalte Struktur in G4-Dichte, kein Klecks | **Bild** `06` Z.3-5 und `kopf_v3.png`: gleichmaessiges rotes Adernetz auf schwarzer Haut, PSX-weich, ohne Glanz (L\*>85 0,00 %). **Gegenueber Runde 1** (`kopf_r1_r2.png`): Die hartkantigen Schmierflecken sind weg. **Kachelung von F18** (Dossier V3R2.5 Punkt 4): In der Groesse von `06` nicht zu erkennen. In der Seitenansicht `kopf_v3.png` az 0 erscheint eine Reihe aehnlicher senkrechter S-Adern in etwa 4 Zellen, also leicht periodisch. Das Netz ist dichter und gleichfoermiger als die G4-Haut (B12 ROT) | Abweichung klein |
| Maul im neutralen Licht (Gefahr "Pokemon") | Kiefer nicht cremefarben, kein aufgemalter Rahmen | Das Band ist eine lachsrosa Lippe unter der Zahnreihe. Der Rahmen um die ganze Schnauze aus Runde 1 ist weg. In der Seitenansicht (`06` Z.4, `kopf_v3.png` az 0/45) ragt vorn ein rosa Zipfel hervor, wie eine herausgestreckte Zunge. Das wirkt comichaft, ist im Bild aber klein | Abweichung klein |
| G2 Bild 2: Feuerfarbe am Maul an den hellen Stellen | Band an der Original-Stelle, hell rosarot mit hellen Punkten. Rundlauf (49,9 / 28,2 / 16,1) | **Ort:** In `02` und `06` Z.1 stimmen Ort und Richtung, mit Zahnreihe. **Farbe:** roter und gesaettigter als im Original, K15 (47,5 / 39,2 / 20,9), dE 12,2. Das Band wirkt dadurch dunkelrot statt hell-rosarot, die hellen Punkte des Originals fehlen fast ganz. **Spielfenster 5090** (`spiel5090_k4_v3.png`): mattes rosa Band mit weissen Zaehnen, ein Leuchten liest man nicht. Das Texelziel F2t (49,5 / 37,3 / 20,8) ist erreicht, die Kette nicht. Zum Entscheid V3R2.5 Punkt 1: Fuer das Auge zaehlt K15. In der Kette muesste das Band weniger rot sein | Abweichung klein |
| G2 Bild 1: Leuchtstelle | Ort und Form wie Original F1: ein schraeger Streifen, 646 px | **Messung** an der Nahkachel `01` Z.3 (Regel L\* >= 30, a\* >= 20, 8-zusammenhaengend; Flaeche durch 16 wegen 4-fach): Original 646 px (bestaetigt). **V3 941 px = 1,46-fach**; Runde 1 977 px (1,51-fach), Ziel aus Runde 1 rund 2/3. K4 915 px. Schwerpunkt 1,7 / 1,5 px (Original) daneben. Farbe K14 dE 3,9. **Form:** weiterhin ein gelappter Fleck mit drei dunklen Streifen statt eines Streifens. Die drei Streifen sind im Atlas `04` schon angelegt (Seite 3, oben). **Roh** (`b1_f1_roh_k4_v3.png`): ein flaches, staubrosa Pflaster mit harter, ausgefranster Kante, ohne Verlauf. K4 hat dort eine weiche Raute. **Spielfenster 5080:** eine matte mauve Stelle | Abweichung klein. Ort und Farbe stimmen, Flaeche und Form nicht; der Befund aus Runde 1 ist nicht umgesetzt |
| G2 "Gesicht leuchtet" in Bild 1 | Nutzer: Das Gesicht vorne leuchtet | Die Stelle liegt nach Messblatt V3 Zeile 79 auf dem Becken (Knochen 0), der Kopf ist in der Pose verdeckt. Unter neutralem Licht (`05` Seite) ist sie ein rosa Fleck auf der Hueftoberseite, keine Feuerfarbe. Die gemessene Texelfarbe (48,7 / 21,8 / -3,2) ist staubrosa. Orange und Gelb fallen in der Gegenprobe ROT aus (dE 74,6 / 87,3) | Abweichung klein. Die Rueckfrage an den Nutzer bleibt offen (Messblatt V3 7.1) |
| Augen dunkel (E24) | dunkel | `06`: keine Augen sichtbar, A9 L\* 7,3 | wie Spielfamilie/Vorlage |
| Glanz | keiner | Keine Glanzflecken. N2 0,02-0,30 % sind Krallenspitzen und Zaehne (G4 0,09-0,21 %, K4 0,85-1,33 %) | wie Spielfamilie/Vorlage |
| Posterisierung, Atlas | keine Stufen wie in V2 | `04`: G4-Texel in G4-Machart. P1 22,5 % dL0, 324 Farben (GRUEN). Keine Foto-Fragmente. Die Herkunftskarte ist schluessig | wie Spielfamilie/Vorlage |
| Schwanz aus einem Guss (E9) | ohne Teilstuecke | `05` Seite: ein Zug, A11 3,1. K12 Schwanz 8,7 % (Grenze 8,9) | wie Spielfamilie/Vorlage |
| Gesamteindruck: nicht "Pokemon", nicht "billig einfarbig", nicht "hoeher aufgeloest" | passt zur Familie | `03`: ein schwarzer Vierbeiner mit dunkelrotem Adernetz in PSX-Dichte. Nicht einfarbig, nicht zu fein aufgeloest, kein Comic-Farbschema. Comichaft wirken nur der rosa Zungenzipfel (klein) und die Klaue im Vordergrund von Bild 2 (siehe oben) | wie Spielfamilie/Vorlage |
| Fairness der Tafeln | Gleiches Licht und gleiche Behandlung; Szenenkacheln neben dem Screenshot | **Fair:** In `01`/`02` stehen K4 und V3 unter gleichem Disc-Licht x k, in Z.2 mit gleichem Filter, das Original daneben. `03`/`05`/`06`/`08`: gleicher Renderer, 0,55 + 0,45 N.L, roh. Nichts ist einseitig aufgehellt. **Hinweise:** `07` ist mit "echtes Fenster (gdigrab)" beschriftet. Laut `spiel_lauf_v3.log` wurde aber mit "PrintWindow (Versuch 1)" aufgenommen, fuer K4 und V3 gleich. In `03` hat V3 15 000-17 000 px, G4 35 000-39 000 px. Das verbirgt eher Blockigkeit; `08` und `06` gleichen es aus. Der Kandidat laeuft mit 3 Clips, K4 mit 28 | fair (die Beschriftung von `07` ist zu berichtigen) |

## Was der naechste Lauf aendern muss (nur Befund, keine Konstanten)

- **Vorderklaue Bild 2:**
  - Die Krallenflaechen im Kamera-Vordergrund duerfen nicht als grosse Dreiecksmosaike aus der F21-Spitze erscheinen.
  - Massstab ist die Original-Nahstelle: weiche, helle Flecken oben und eine dunkelrote lange Klaue.
  - An `K5c_farbe_v3/bau.json` ist zu pruefen, welche Flaechen der grossen Klaue (Knochen der Klauen l/r) die hellen F21-Texel tragen und wie stark sie gedehnt sind (P2 +9, D1 > 2,0 bei 16,9 %).
  - Der Bauer nennt in V3R2.5 Punkt 3 schon einen Weg: die Krallen laengs zweiteilig.
- **F1:**
  - Die Flaeche weiter verkleinern (941 statt 646 px).
  - Einen schraegen Streifen statt drei.
  - Die Kante weicher, wie die K4-Raute oder das Original.
- **Band:** In der Kette weniger Rot (a\* 39 gegen 28 im Rundlauf). Fuer das Auge zaehlt K15, nicht F2t.
- **Rosa Strichel** an den Flaechengrenzen von Schulter und Brust: die Herkunft klaeren (Randtexel einer Sonderzeile?).
