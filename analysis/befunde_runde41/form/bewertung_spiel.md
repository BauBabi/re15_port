# Bewertung Spielmodell (Linse SPIELMODELL)

Stand 2026-10-31. Unabhaengiger Pruefer, an keinem Kandidaten mitgearbeitet. Frage: Taugt das als Bossmodell eines PS1-Spiels von 1997, ohne Ruecksicht auf die Screenshots?
Punkte sind Urteil am Bild, keine Messung. Die Messungen stuetzen das Urteil und stehen in Abschnitt 4. Belege und Skripte: `bewertung_spiel/`.

## 1. Urteil

| Kandidat | Punkte | Kurz |
|---|---|---|
| B | **6** | Einziges Tier der drei, starke Gestalt. Rumpf reisst beim Biegen, drei Glieder schweben, Textur ist Flecktarn |
| A | **5,5** | Mechanisch das beste Modell, biegt ohne Riss, steht auf allen vieren. Aussehen: Plueschbaer aus Ballons |
| C | **5** | Beste Textur und einzig brauchbarer Atlas. Koerper ist ein Ei auf Stelzen mit Fischflosse, Rumpf reisst wie bei B |

Keiner ist abnahmefaehig. Eichmass ist G4 aus RE2 im selben Renderer (`bewertung_spiel/nah_alle.png`, unterste Reihe), nur fuer das Handwerk, keine Formvorlage. Es laege bei rund 9.
Jeder Kandidat loest genau einen Teil der Aufgabe. Die Staerken schliessen sich nicht aus: Gestalt B, Rumpfbau A, Atlas und Malerei C.

## 2. Teilnoten (Urteil)

| Kriterium | A | B | C | Tragender Befund |
|---|---|---|---|---|
| Klarer Koerperbau | 5 | 8 | 4,5 | B: Schulter, Bauch, Stand. A: Kugel mit Stumpen. C: Kuppel mit Saum auf duennen Beinen |
| Lesbare Glieder | 4 | 8 | 5 | B: Oberarm, Unterarm, Faust, Fuss mit Zehen. A: Glieder verschmelzen von vorn |
| Gemalte Textur statt Matsch | 6 | 3 | 7 | C: Risse und Adern erkennbar gemalt. B: Flecken mit Pfeffer |
| Keine Risse in Ruhe | 6 | 10 | 8 | B: 0 Durchblick. A: Bauchschlitz, Keil am Kopf |
| Nichts schwebt | 9,5 | 3 | 7 | B: drei Glieder 131-188 ueber dem Boden, im Spiel sichtbar |
| Kein Noppen- oder Wurstlook | 3 | 7 | 4 | A: Ballonkette. C: Wurstglieder, Spindel. B nur Faeuste kartoffelig |
| Wird sich bewegen lassen | 8 | 4 | 4,5 | A haelt alles. B und C: Rumpf nur bis rund 6 Grad je Stueck |

Die Endnote ist kein Mittelwert (Mittel 5,9 / 6,1 / 5,7). Maengel, die nur ein Neubau behebt (Bauform A, Gestalt C), wiegen schwerer als Maengel, die sich nachbessern lassen (Rumpf, Stand und Textur B).

## 3. Am Bild

### B, 6 Punkte
Stark: `nah_alle.png`, `klein_alle.png`, `spiel_tafel_nah.png`. Auch auf 100 Bildpunkten liest man Schulter, Arm, Faust, Bein. Einzige Figur mit einer Geste. Glieder sind geschlossen, Normalen stimmen, das Licht formt den Koerper.
Schwach:
- **Rumpfriss, im echten Spiel gesehen.** `spiel_scheitel_biss.png` oben Mitte: heller Spalt am Scheitel. `spiel_scheitel_seitenhieb.png` oben Mitte: links unten sieht man durch den Rumpf auf die Leiter dahinter. Ursache: ein Netz, in drei offene Schalen geschnitten.
- **Schwebt.** `spiel_tafel_nah.png` Zeile 1 und 2: rechte Faust haengt ueber ihrem Schatten, unter dem Bauch haengt ein heller Fingerstummel in der Luft. Das sind rund 8-17 Bildpunkte im Fenster 640 x 480.
- **Textur.** `aus_B_vorn.png`: weiche Tarnflecken mit dunklem Pfeffer, Marken als hartkantige Scherben. Der Atlas `tex_B.png` ist Konfetti, von Hand nicht nachmalbar. In Spielgroesse bleibt nahezu Schwarz mit Splittern.
- Arme haengen am falschen Rumpfstueck: Eltern 9, Kontakt aber mit 10 (Abstand zu 9: 14 und 116, zu 10: 0,8 und 1,1). Knickt der Rumpf, bleiben die Arme stehen. Gelenke 9 und 10 liegen auf demselben Punkt, der dreiteilige Rumpf hat nur zwei Biegestellen.
- Im Biegeblatt `biege_B.png` (Hieb) waechst der gehobene Arm als duenner Stiel oben aus der Kuppel, das Schultergelenk liegt tief im Rumpf.
- Kein Blickpunkt: Die Naht liest sich in Spielgroesse nicht als Maul. Unter dem Bauch sitzt ein spitzer Kiel.

### A, 5,5 Punkte
Stark: `biege_A.png`. Jede Pose bleibt ein zusammenhaengender Koerper, 22 von 22 Gelenken ohne Abriss, Durchblick aus Spielhoehe in allen Posen auf G4-Niveau. Alle vier Glieder stehen auf dem Boden. Die Haut laeuft als durchgehender Strich ueber den Koerper und liest sich in `spiel_tafel_nah.png` als dichte Fasermasse. In ROOM5090 wirkt die Masse bedrohlich.
Schwach:
- **Ballonlook.** `aus_A_hinten.png`, `rundum_hell.png`: Kugel plus Kapseln, von vorn ein Teddy. Die Bauart Rundkegel plus Metaballs erzeugt das, Malerei kaschiert es nur. Kein Fuss, keine Hand, keine Kralle.
- Hals, Kopf und Kiefer kosten 280 Flaechen fuer einen Ball ohne Gesicht. Lesbar bleibt ein rosa Horn.
- Normalen am staerksten verbogen (Hoehenanteil 0,33, G4 0,93). Die Form bleibt in wechselndem Licht flach.
- Marken gezackt, am Scheitel ein Kammmuster (`aus_A_hinten.png` oben), Atlas `tex_A.png` ist Konfetti mit 205 Inseln.
- Bauchschlitz: nur von unten, dort aber deutlich (Ruhe 1215, Seitenhieb 5015, G4 hoechstens 558). In `loch_A_seitenhieb.png` rund 300 x 300 Einheiten. Dazu ein Keil zwischen Kopf und Rumpf (`lochruhe_A_40.png`).

### C, 5 Punkte
Stark: `tex_C.png`, `spiel_scheitel_C.png`. Einziger Atlas, der wie ein PS1-Atlas aussieht: 25 Inseln, zusammenhaengend, links und rechts geteilt (FR und FL 238 von 238 Flaechen gleich, nachgemessen). Risse, Adern und weiche Marken sind als Malerei erkennbar, der lokale Kontrast liegt am naechsten an G4. Aus Gier 3172 ist C im Spiel die lesbarste Figur.
Schwach:
- **Ei auf Stelzen.** `aus_C_vorn.png`: glatte Kuppel mit Saum, duenne Beine. Die Beine sind Bretter: Mitte Glied 309 x 525 (B: 643 x 586), von der Seite rund 1,7-mal duenner als von vorn. Das Modell ist auf eine Ansicht gebaut.
- **Spindel.** Bein NR endet in einer Fischflosse (`spiel_tafel_nah.png` Zeile 1 rechts). Sie ist das Ende des Schlagglieds, also das, womit die Figur zuschlaegt.
- Kopf: Kugel auf Stiel mit rosa Becher, wirkt angesteckt. Rosa Rissplatte auf dem Scheitel wirkt wie Schorf, von hinten fremd.
- Rumpf reisst offline wie bei B (`loch_C_biss.png`, Sichel am Scheitel). **Im Spiel aus dieser Kamera nur als kleine Stufe am rechten Kuppelrand gesehen, kein offener Spalt** (`spiel_scheitel_C.png`). Die Gefahr ist gemessen, die Sichtbarkeit im Spiel fuer C nicht belegt.
- Pose 2: Fuss FR schwebt 189, von C selbst benannt und nachgemessen.
- Normalen verbogen (0,55). Bein FL ist eine Spiegelkopie, die Gelenkpunkte von fl_bein und fl_fuss liegen ausserhalb ihres Netzes (Windungszahl 0,00 und 0,01). Bis 60 Grad entsteht trotzdem kein Abriss.

## 4. Messwerte

Durchblick = Pixel, die nur eine Rueckseite traegt. Typ 0x30 zeichnet Rueckseiten nicht (`werkzeug.md` Abschnitt 3), dort sieht man also den Raum. Oeffnung 3 x 3, 12 Azimute. Massstab rund 0,175 px je Einheit, G4 0,129: G4-Werte fuer gleiche Flaeche mal 1,8 lesen.

| | A | B | C | G4 |
|---|---|---|---|---|
| Flaechen / Knochen | 1862 / 23 | 1890 / 21 | 1842 / 21 | 1427 / 27 |
| UV-Inseln (Einzelflaechen) | 205 (42) | 162 (28) | 25 (3) | 145 (6) |
| Offene Kanten | 139, fast alle Teile | 204, nur Rumpf | 187, nur Rumpf | 19 |
| Normalen Hoehenanteil, 1 = geometrisch | 0,33 | 1,00 | 0,55 | 0,93 |
| Abweichung Normalen, Median | 21,3 Grad | 0,1 Grad | 23,3 Grad | 11,8 Grad |
| Flaechengroesse, Streuung | 1,76 | 0,68 | 0,61 | 1,07 |
| Texel je 1000 Einheiten | 30,8 | 34,2 | 47,0 | 38,1 |
| Y im Bild, Median | 13 | 12 | 12 | 31 |
| Anteil Y ueber 60 | 5,2 % | 6,8 % | 7,1 % | 21,6 % |
| Lokaler Kontrast | 7,6 | 7,0 | 11,1 | 13,7 |
| Gelenke ohne Abriss bis 60 Grad | 22/22 | 19/20 (fr_ober ab 20 Grad) | 19/20 (fr_huefte bei 60 Grad) | |

Durchblick aus Spielhoehe (Blick 0, 20, 40, 65 Grad, 48 Blicke). G4 in 27 echten Animationsbildern: Median 1148, hoechstens 2326.

| Pose | Rumpfknick je Stueck | A | B | C |
|---|---|---|---|---|
| Ruhe | 0 | 1959 | 0 | 1215 |
| Drohen | 5-6 | 990 | 1357 | 6267 |
| Gehen | 5 | 1828 | 2356 | 3948 |
| Hieb | 7-8 | 1023 | 3456 | 7710 |
| Biss | 9-11 | 1794 | **21333** | **21499** |
| Seitenhieb | 8-16 | 2004 | **38683** | **22800** |

Rumpf allein, vorderes Stueck gegen Mitte, hoechster Wert ueber drei Achsen und beide Richtungen, 32 Blicke (Blick -35 bis 65 Grad):

| Knick | A | B | C |
|---|---|---|---|
| 6 Grad | 1926 | 3606 | 5992 |
| 12 Grad | 3026 | 16889 | 14613 |
| 20 Grad | 3395 | 45847 | 35975 |
| 30 Grad | 2480 | 70155 | 107028 |

Hoehe ueber Boden je Glied (FR, FL, NR, NL), Soll 0:

| Clip | A | B | C |
|---|---|---|---|
| 0 Pose 1 | 0, 2, 10, 13 | 0, 104, 122, 131 | 0, 0, 20, 0 |
| 1 Pose 2 | 7, 8, 5, 0 | 127, 469 (gesetzt), 0, 82 | 189, 0, 12, 60 |
| 2 Ruhe | 0, 2, 10, 13 | 0, 139, 188, 131 | 0, 0, 20, 0 |

## 5. Das Beste je Kandidat

**A**
- Rumpf aus einem eigenen, fast geschlossenen Volumen je Knochen (offene Kanten 8, 0, 9) statt einer zerschnittenen Schale (`kandidat_a/entwurf.py`, `kandidat_a/volumen.py`). Bis 30 Grad bleibt der Durchblick bei 1900-3400, bei B und C steigt er auf 70155 und 107028. Das ist der wichtigste Befund dieser Pruefung.
- Gelenkplan `kandidat_a/entwurf.py`: Schlagglieder und Hals an Knochen 10, Kette Hals, Kopf, Kiefer, 22 von 22 Gelenken ohne Abriss.
- Bodendisziplin: alle Glieder in allen Clips hoechstens 13 ueber dem Boden.
- Hautstrich als Malmotiv, er traegt noch in Spielgroesse. Nur das Motiv, nicht den Atlas.

**B**
- Koerperbau als Grundlage: Schulter, Arm, Faust, Fuss mit Ferse und Zehen (`kandidat_b/b_teile.py`, Glieder-Netze).
- 18 Glieder ganz geschlossen, 0 offene Kanten.
- Geometrische Normalen, wie G4. Stauchung nicht uebernehmen, ohne sie in anderen Raeumen zu pruefen.

**C**
- Atlas `kandidat_c/atlas.py`: wenige grosse Inseln, Seiten teilen sich Karten. Nur so kann man die Textur von Hand nacharbeiten.
- Malerei `kandidat_c/maler.py`: Rissnetz, Adern, Marken als weiche Wuelste statt Scherben.
- `tim_genau` (CLUT direkt, 246 Farben): Weisskerne bleiben erhalten, laut Dossier schluckt der Medianschnitt sie.
- Kleine Hoerner an den Fuessen, einziger Silhouetten-Akzent aller drei.
- Reproduzierbarer Bau laut Dossier (`lauf4.py ende --neu` bytegleich), hier nicht nachgeprueft.

**Vorschlag:** Gestalt B, Rumpf nach Bauweise A, Arme an Knochen 10, Stand auf den Boden ziehen, Atlas und Malerei nach C. Das Grundproblem bleibt fuer alle: Figur rund 2,5-mal dunkler als G4, kaum Akzente, kein Blickpunkt. Die Dunkelheit folgt aus `farbe.md` und ist kein Fehler der Kandidaten, kostet aber in Spielgroesse die Binnenform (`klein_alle.png`). Akzente muessen deshalb ueber die Silhouette kommen.

## 6. Hinweise an die Kandidaten-Dossiers

- `kandidat_a/spiel_pose1_room5080_schnitt1.png` ist kein Spielbild: Bildmitte Maximum 0. Das Dossier fuehrt es als Abgabe. Das Modell selbst laeuft: `bewertung_spiel/spiel_A_r5080_s1_pose1.png` (Lage und Gier wie `kandidat_a/lage.json`).
- B: Beide `spiel_*.png` sind aelter als `FORM.EMD` (21:25 und 21:27 gegen 21:28), B beschreibt zudem den Software-Treiber. Die Reihe hier nutzt die abgegebenen Dateien und `direct3d`.
- Rundumblaetter taeuschen bei C: Im Spiel aus Gier 3172 wirkt C deutlich besser als im Blatt. Das Spielbild ist hier die strengere und zugleich gerechtere Probe.

## 7. Grenzen

- Keine echte Animation. Fuenf von mir gesetzte Posen mit gleichen Winkeln je Kandidat, angelehnt an die Tick-Clips, nicht daraus abgeleitet. Bei gleichem Winkel sieht jede Bauart anders aus, die Bodenlage der Biegeposen ist nicht bewertet.
- Spielreihe: 15 Bilder, gdigrab, `direct3d`, ROOM5080 Schnitt 1 und ROOM5090 Schnitt 11. In ROOM5090 verdeckt Feuer die Beine. Andere Kameras sind nicht geprueft, Malerreihenfolge nur in diesen Bildern betrachtet.
- Durchblick misst nur Durchsicht, keine Sortierfehler und keine haesslichen Durchdringungen.
- G4 hat mehrere CLUT-Zeilen, `tex_G4.png` zeigt nur die erste und ist in Teilen falschfarben. Die Tonwerte oben stammen aus dem Render, nicht aus dem Blatt.
- Nicht geprueft: PSX-Ziel, Laufzeit, Tick-Treffer der Teile 10, 13, 18, Kollision, Schatten.

## 8. Dateien (`bewertung_spiel/`)

| Datei | Inhalt |
|---|---|
| `nah_alle.png`, `klein_alle.png` | A, B, C, G4 nah und in Spielgroesse |
| `aus_A_hinten.png`, `aus_B_vorn.png`, `aus_C_vorn.png` | Ausschnitte der Fehlstellen |
| `biege_A.png`, `biege_B.png`, `biege_C.png` | Fuenf Posen, drei Sichten |
| `loch_*.png`, `lochruhe_*.png` | Lochkarten, rot = Durchblick |
| `spiel_tafel_ruhe.png`, `spiel_tafel_nah.png`, `spiel_scheitel_*.png`, `spiel_*.png` | Spielreihe |
| `tex_A.png`, `tex_B.png`, `tex_C.png`, `tex_G4.png` | Atlanten x3, aufgehellt |
| `netz.json`, `gelenk.json`, `biege.json` | Zahlen |
| `g_*.py`, `g_spielreihe.sh`, `spiel_reihe.log` | Skripte, Lauflog. `g_zw.py` erzeugt die Biege-EMD neu |

Kein Commit, keine Datei ausserhalb von `form/bewertung_spiel*` geaendert. Eigene Laufordner unter `re15_port/build/form_spiel/` sind geloescht, fremde Prozesse unberuehrt.
