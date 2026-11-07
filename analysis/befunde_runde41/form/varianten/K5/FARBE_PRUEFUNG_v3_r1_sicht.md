# Sicht-Pruefung Farbe V3 Runde 1 (K5c, G4-Uebertragung), 2026-11-07

Pruefer: Sicht (Auge des Nutzers). Gegenstand: `farbe/K5c_farbe_v3/` (= `farbe/K5C_FARBE_V3.EMD/.TIM`), Stand Commit 4b5800a76 auf r41/form.
Tafeln: `farbe/01_szene_bild1.png` bis `farbe/08_binnenzeichnung.png`, `farbe/spiel_v3/`, Vergleich `farbe/v2r2/`.
Riegel des Baus: `farbe/riegel_K5c_farbe_v3.txt` (33/52 GRUEN).
Eigene Ausschnitte und Messungen (nicht im Repo): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3/sicht_v3r1/`
(`fam_g4_v3.png`, `kopf_nah.png`, `b08_v3.png`, `f1_nah.png`, `b1_v3_roh.png`, `b2_v3_roh.png`, `b2_filt.png`, `r05.png`).
Seiten: (W) = aus Sicht des Wesens, (B) = Betrachter.

## Ergebnis

**Nicht bestanden.** Drei Punkte wuerde der Nutzer wieder ablehnen. Sie betreffen den Kopf und grosse Teile, die in den Szenen vorn liegen:
1. **Kopf** (der Ort, den der Nutzer benannt hat): ein schwarzer Klotz mit verschmierten, hartkantigen Rotflecken oben. Um die Schnauze laeuft
   ein flacher roter Rahmen wie ein Maulkorb. Gemalte Anatomie wie am G4-Kopf fehlt.
2. **Schulter/Oberarm:** wieder gestueckelt. Man sieht gedehnte Texelbloecke, lachsfarbene Rundflecken auf Schultern und Ellbogen
   (wie Polster) und flache rote Keile.
3. **Vorderklaue im Vordergrund von Bild 2:** ein Tarnmuster aus grossen blassen Dreiecken.

Was V3 gegenueber V2 sichtbar besser macht:
- Rumpf, Beine und Schwanz sind schwarz-rot verbrannt, in G4-Machart: schwarze Haut, rote Adern als Linien.
- Die Leuchtstelle in Bild 1 sitzt am richtigen Ort.
- Der Rotton ist nicht mehr rostrot.
- Die Atlas-Posterisierung ist weg.

## Befunde je Punkt

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| G1 Rumpf, Hinterbeine, Schwanz: schwarz-rot verbrannt, G4-Machart | Schwarze Haut mit roten Adern als zusammenhaengende Linien, wie G4 in `03` | `01` Z.1 (Ausschnitt `b1_v3_roh.png`): schwarzbraune Haut, rot-magenta Adern laengs an Beinen und Schwanz, als Linien gezeichnet. `07` 5080 Ruhe: schwarzer Koerper mit roten Adern. H1 99,9-100 %; P1 Atlas 21,0 % / 26,2 % / 401 Farben (G4 17,3 / 26,0 / 422) | wie Spielfamilie/Vorlage |
| G1 Rotton unter neutralem Licht | G4-Blutrot | `03`/`05`: Unter weissem Licht sind die Adern magenta-rosa statt blutrot (Bild-1-Zeilen 3-5, Farbton -8 Grad nach K11). Unter Bahnsteig-Licht (`01`, `07`) liest es sich richtig als Rot. Die Figur ist dunkler und matter als G4: L\* 14,3 / C\* 8,8 gegen 21,6 / 12,6 (`03` Z.1); A1 und N1 ROT | Abweichung klein |
| Flanke und Ruecken nah | zusammenhaengende Adern wie G4 (`08` Spalte 1) | `08` Flanke/Ruecken V3: dunkel, die Adern zerfallen in magentafarbene Splitter und Punkte. B17 Kohaerenz 0,588 gegen G4 0,616; B11 ROT | Abweichung klein |
| **Schulter / Oberarm** | Haut aus einem Guss wie die G4-Schulter (`08` Z.2 Sp.1) | `08` Z.2 Sp.4 (Ausschnitt `b08_v3.png`): ein lachsfarbenes Oval mit gedehnten Texelbloecken, daneben flache rote Keile mit harten Kanten und waagerechte rosa Strichel an den Flaechengrenzen, also Flickwerk aus drei Machart-Fragmenten. `05` vorn (`r05.png`): lachsfarbene Rundflecken auf beiden Schultern und Ellbogen wie aufgesetzte Polster. P2 Render dL=0 ueber der G4-Quelle (Grenze +8): Rumpf +14,6, Vorderbein r (W) +12,7 Punkte. D1 Rumpf 2,11 (wirksam 2,73) ROT | **Abweichung, die der Nutzer wieder ablehnen wuerde** ("fleckig gestueckelt" wie V2) |
| **Kopf** (Machart, Gesicht) | G4-Kopf-Machart (gemalte Rippen und Wuelste, weich); Augen dunkel (E24); dunkel mit Rot (Bild 2 Oberkiefer T (69,23,23)) | `06` Z.3-5 (`kopf_nah.png`): Der Schaedel ist eine schwarze, fast strukturlose Kuppel. Oben sitzen grob verschmierte, hartkantige Rotflecken; das sind vergroesserte Aderntexel, keine gemalte Form. In `05` Seite und 3/4 vorn (`r05.png`) laeuft um die Schnauze ein flacher, leuchtend roter Rahmen wie ein Maulkorb oder Lippenstift. Das wirkt aufgemalt; Gefahr "Pokemon". D1 Kopf 2,77 (wirksam 4,38), P2 Kopf +13,1, B12 ROT | **Abweichung, die der Nutzer wieder ablehnen wuerde** |
| G2 Bild 1: leuchtet die Stelle dort, wo das Original rosa-magenta ist? | Ort und Farbe wie Original F1 (646 px, Schwerpunkt (317,3/241,2)) | `01` Nahkachel (`f1_nah.png`), eigene Messung mit der F1-Regel des Messblatts (L\* >= 30, a\* >= 20, 8-zusammenhaengend): Original 646 px, Schwerpunkt Kachel (190,6/166,4); V3 977 px (**1,51-fach**), Schwerpunkt (196,3/160,6), also 1,4 px (Original) daneben. Farbe K14 dE 5,1 GRUEN. Das Original zeigt **einen** schraegen Streifen, oben hell. V3 zeigt einen breiteren, gelappten Fleck mit drei Streifen. Roh (`b1_v3_roh.png`) ein blasses Mauve-Pflaster mit dunklen Texelpunkten und harter Kante, kein Leuchten | Abweichung klein (Ort stimmt, Flaeche 1,5-fach, Form gelappt) |
| G2 Bild 1: Gesicht oder Becken | Nutzer: "das Gesicht vorne ... in flammenfarben leuchtet" | Laut Messblatt V3 1.2 liegt die Stelle auf dem Becken (646/646 px Knochen 0). Im Szenenbild sieht der Nutzer sie dort, wo er das Gesicht liest. In den anderen Ansichten (`05` 3/4 hinten, Seite) ist sie ein rosa Fleck am Kreuz, nicht am Gesicht | Abweichung klein; die Rueckfrage an den Nutzer bleibt (Messblatt V3 7.1) |
| G2 Bild 2: Feuerfarbe am Maul | Fleischband lachsrot T (184,90,85), Zaehne hell, an der Stelle des Originals | `02`/`06` Z.1: Das Band liegt diagonal an derselben Stelle wie im Original, mit Zahnreihe. Nach dem Videofilter ist es rosaroter und weniger orange als im Original. F2t dE 6,18, K15 dE 7,9, K7 dE 10,2 ROT. Im Spiel (`spiel_v3/spiel_pose2_r5090_s11.png`) sieht man das Maul im Feuerlicht nur als Zahnreihe mit rotem Saum; ein Leuchten liest sich nicht | Abweichung klein |
| **Vorderklaue (Bild 2 Vordergrund)** | G4-Klauen-Machart (dunkelrot mit hellen Spitzen) | `02` Z.1 (`b2_v3_roh.png`): Die grosse Klaue vorn links (B) ist ein Tarnmuster aus grossen beigen, weissen und rotbraunen Dreiecken, nach dem Videofilter weisse Bloecke (`b2_filt.png`). Helligkeit im Bereich (Kachel x 60-340, y 380-500): Original L\* > 60 6,9 %, V3 8,8 %. Der Mittelwert passt, die Form nicht. Wirksame Vergroesserung der Klauen 3,78; P2 Klauen l/r +12,6 / +12,4 | **Abweichung, die der Nutzer wieder ablehnen wuerde** (dasselbe Stueckelbild wie an der Schulter, im Vordergrund der Szene) |
| Glanz | kein rueckprojizierter Glanz | Keine Glanzflecken sichtbar. Die hellen Pixel (N2 0,12-0,41 %) sind Zahn- und Krallenspitzen. Zum Vergleich: G4 0,09-0,21 %, K4 0,85-1,33 % | wie Spielfamilie/Vorlage |
| Posterisierung | keine Stufen wie in V2 (89 Farben, 52,9 % dL=0) | Atlas P1 GRUEN (21,0 %, 401 Farben). Im Render entstehen aber Bloecke durch die Vergroesserung (P2 ROT an Rumpf, Hals, Vorderbein r, Klauen und Kopf) | Abweichung klein am Rumpf; Kopf, Schultern und Klauen sind in den Zeilen oben gewertet |
| Schwanz aus einem Guss (E9) | ohne Teilstuecke | `05` Seite: ein Zug ohne sichtbare Gliedgrenzen, A11 2,8. Die Rotklasse im Bild-1-Schwanz liegt bei 12,4 % gegen die Grenze 8,9 (K12 ROT); in `01` Z.2 sichtbar als rosa Laengsstreifen am Schwanz | Abweichung klein |
| Augen dunkel (E24) | Augen dunkel | `06`: keine Augen sichtbar, A9 L\* 7,3 | wie Spielfamilie/Vorlage |
| Spielbild (`07`, `spiel_v3/`) | echtes Fenster, Kandidat gegen K4 | 5080: schwarz mit roten Adern, der Kopf oben rot. 5090: der Kopf oben aus roten und schwarzen Splittern, die Klaue vorn hell gefleckt, das Maul nur als Zahnreihe erkennbar | bestaetigt die Befunde zu Kopf und Klaue |
| Fairness der Tafeln | gleiches Licht und gleiche Behandlung fuer alle Modelle; Szenenkacheln neben dem Screenshot | `03`/`05`/`08`: gleicher Renderer, Licht 0,55+0,45 N.L, roh, ohne Aufhellung. `01`/`02`: K4 und V3 unter gleichem Disc-Licht x k; Z.2 mit gleichem Filter (Saat 15). Die Nahkacheln zeigen das Original-Video neben dem gefilterten Render. Einschraenkung: In `03` hat K5c rund 15 000 px gegen 35 000 px des G4. Das verbirgt eher Blockigkeit, als dass es V3 benachteiligt; `08` und `06` gleichen es aus. `spiel_v3/` enthaelt Bilder von Kandidat und K4 (Log `spiel_lauf_v3.log`, 10/10 rc=0). Der Kandidat laeuft mit 3 Clips (Variante), K4 mit 28 | fair |

## Was der naechste Lauf aendern muss (nur Befund, keine Konstanten)

- **Kopf:** Die Quelle ist zu stark gedehnt (D1 2,77, wirksam 4,38). Woher der flache rote Rahmen an der Schnauze kommt, ist an
  `K5c_farbe_v3/bau.json` zu pruefen (Flaechen-Zuordnung von Kieferkante und Band), nicht zu raten. Ziel ist eine dunkle Kuppel mit gemalter Struktur in G4-Dichte;
  Rot nur am Band.
- **Schulter/Oberarm, Rumpf, Klauen:** P2 liegt bei Rumpf, Hals, Vorderbein r (W), Klauen und Kopf mehr als 8 Punkte ueber der Quelle.
  Die Quellflaechen sind fuer die K5c-Flaechen zu klein, und auf einem Teil treffen fremde Machart-Fragmente zusammen (lachsfarbenes Oval, rote Keile).
- **F1:** Den Fleck auf rund 2/3 der Flaeche verkleinern und als einen schraegen Streifen fuehren (Nahkachel: Original 646 px, V3 977 px).
