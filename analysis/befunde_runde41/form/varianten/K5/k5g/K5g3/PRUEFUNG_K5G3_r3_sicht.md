# Prüfung K5g3, Runde 3: Sicht-Prüfer

Stand 2026-11-14. Geprüft wurde die Abgabe der Runde 3 unter `k5g/K5g3/`. Die Hashes habe ich nachgerechnet, sie stimmen mit FARBE.md K5g3R3.0 überein:

| Datei | sha256 |
|---|---|
| BAHNSTEIG_FORM.EMD | `1981f97e…` |
| BAHNSTEIG_FORM.TIM | `c00823b3…` |
| FORM.EMD | `26e310fe…` |

Das Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.*` ist weiter K5e_f1 (EMD `f277916d…`, TIM `31a35e66…`).

**Ergebnis: BESTANDEN.** Keine Antwort fällt in die dritte Klasse („Abweichung, die der Nutzer ablehnen würde“). Zwei Punkte sind „wie gewünscht“, vier sind „Abweichung klein“.

Die drei Änderungen der Runde 3 sind im Bild genau dort zu sehen, wo sie hingehören, und sonst nirgends:
- braune Musterung mit Struktur (G6);
- Hals-Unterseite ohne Buckel-Fleisch (G7);
- keine fast weißen Hautpunkte mehr (G8).

## Was ich angesehen habe

**Tafeln des Bauers:**
- Gelesen habe ich 01 bis 09, dazu 11 (UV der Spalt-Kinder, aus der Geometrie-Runde) und das Messbild `messung_r3/braun_z5080_messbild.png`.
- Tafel 09 habe ich in fünf Teilen gelesen.
- Ausschnitte habe ich 3- bis 8-fach vergrößert: Zonen 16, 19, 24 und 29, Kopf-Unterseite aus Tafel 05, Rücken/F1 aus Tafel 01, Hände aus Tafel 02.

**Vorlagen:**
- Nutzerbilder `C:/workspace/git/reAi_v2/Version_80%/4_Beiner_Zerlegt/`:
  - gelesen: `rechtes_bein_hinten.png`, `linkes_bein_hinten.png` und `Gesicht vorne.png`;
  - die übrigen kenne ich aus Tafel 09, die sie neben das Modell stellt.
- Die abgenommene Übersicht `C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/zonen_nutzer/v2/tafeln/00_uebersicht.png`.

**Vergleich Runde 2 gegen Runde 3, pixelgenau:**
- Grundlage sind die Tafeln 01, 02, 05, 06 und 09 aus Commit f48d37df1, verglichen mit den heutigen Tafeln.
- Geändert sind nur Pixel in den K5g3-Spalten:

| Tafel | geänderte Pixel |
|---|---|
| 01 | 14091 |
| 02 | 17716 |
| 05 | 9827 |
| 06 | 473 |
| 09 | 16717 |

### Eigene Spielläufe

Rahmenbedingungen für jeden Lauf:
- eigene exe-Kopie;
- gdigrab nur auf das Fenster der eigenen PID;
- kein AUTOSHOT und kein SOFTWARE_RENDER;
- Variante über RE15_RE2_ASSET_ROOT;
- beendet wurde nur die eigene PID.

Skript: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/sicht5/lauf.sh`. Es ruft `generator/kf_k5g3_fenster.sh` und `kf_k5g3_spiel.sh` auf, die Gegner kommen aus `sicht4/gegner.sh`. Protokoll `…/sicht5/lauf.log`. Alle Pfade unten liegen unter `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/sicht5/`.

| Lauf | Ablage |
|---|---|
| 5080 Bild-1-Lage, 5080 von vorn, 5090 Schnitt 11 Bild-2-Pose, 5090 Griff (Clip 27 Bild 20) | `fenster/` |
| 5080 Seite (Gier 1400): K5g3 und K5e | `seite_k5g3/`, `seite_k5e/` |
| 5080 von hinten (Gier 3800) | `hinten_k5g3/` |
| 5090 Schnitt 11 mit Schüssen | `schuss_k5g3_5090/` (3 Bilder, Bild 1 ist der Türübergang) |
| Zombie 1140 (liegend), Gorilla 11C0 (im Lastwagen) | `gegner_1140/`, `gegner_11C0/` |

**Abgleich mit dem Bauer:**
- `fenster/k5g3_pose1_r5080.png` und `fenster/k5g3_vorn_r5080.png` sind **pixelgleich** (0 Pixel verschieden) mit den Bildern des Bauers `k5g/K5g3/spiel/k5g_pose1_r5080.png` und `k5g_vorn_r5080.png`.
- Die 5090-Bilder unterscheiden sich nur im animierten Feuer, etwa 41000 Pixel.

**Runde 2 gegen Runde 3 im Spielfenster (5080):**
- Bild-1-Lage: 237 Pixel geändert, verstreute Punkte an Becken und Beinen (`diff_pose1_x3.png`).
- Von vorn: 1142 Pixel geändert, und zwar:
  - helle Punkte auf dem Buckel;
  - eine Fläche an der rechten Halsseite, die vorher heller und röter war und jetzt Haut ist (`diff_vorn_x3.png`).

**Dreiecks-Liste in allen eigenen Läufen:**
- „[tri-liste] ENDE … verworfen=0 bilder_mit_verlust=0 grenze=16384“;
- Höchststand 7318 im 5090-Lauf mit Schüssen, 5936, 5306 und 3053 in den übrigen.

### Eigene Kacheln (roh, nearest)

Alle liegen unter `…/sicht5/`:

| Kachel | Inhalt |
|---|---|
| `kachel_spiel_r2_r3.png` | K5e, K5g3 Runde 2, K5g3 Runde 3 in vier Spielposen |
| `kachel_seite_hinten.png` | Seite und hinten, K5e, R2, R3 |
| `kachel_schuss_5090.png` | Lauf mit Schüssen |
| `kachel_gegner_spiellicht.png`, `kachel_gegner_nah.png` | Zombie, Gorilla, K5g3 5080, K5g3 5090 |
| `griff_hals_vergleich_x4.png` | Hals-Unterseite im Griff: K5e, R2, R3 |
| `vorn_maul_k5g3_x4.png` | Maul von vorn |
| `kopf_vorn_r2_r3_x5.png` | Augen |
| `braun_x5.png` | Messbild, Braun im Spiel |
| `braun16_zoom.png`, `braun_tafel_z16_r2_r3_x3.png`, `braun_tafel_z19_r2_r3_x3.png` | Braun unter weißem Licht, R2, R3 und Änderung |
| `diff05_hals_x3.png`, `diff05_kinn_x3.png` | Tafel 05, R2, R3 und Änderung |
| `b1_ruecken_k5e_k5g3_x3.png` | Rücken und F1 |
| `b2_haende_k5g3_x3.png` | Hände in Bild 2 |
| `z16_/z19_/z29_k5g3_r3_x3.png`, `z29_k5e_k5g3_x3.png`, `h05_unten_k5g3_x3.png` | Zonen-Ausschnitte |

Messbilder tragen die Kopfzeile „Messbild – kein Ergebnis“.

## Antworten

### (1) Wirkt K5g3 im Spiel wie die anderen Gegner? Abweichung klein

**Im Spielfenster** (`kachel_spiel_r2_r3.png`, `kachel_seite_hinten.png`, `kachel_gegner_nah.png`):
- K5g3 ist dunkel rotbraun bis mauve und hat sichtbare Hautstruktur.
- Nicht blass, nicht plastisch, nicht violett-pastell.
- Neben dem liegenden Zombie (dunkel graublau) und dem Gorilla (braun, Wunden rötlich) fällt er in Helligkeit und Struktur nicht aus der Reihe.
- K5e wirkt daneben in jeder Pose wie eine glatte grau-violette Kunststoffmasse.

**Kein Bunt:** Im Spiellicht trägt der ganze Körper einen Hautton. Es gibt keine farbfremden Inseln. Das Bild „Hautprobleme, alles bunt“ von K5c/K5d entsteht nicht.

**Runde 3 gegen Runde 2 im Spiel:** praktisch gleich. Der Gesamteindruck hat sich nicht verändert, nur 237 bzw. 1142 Pixel an den erwarteten Stellen. Die Zielband-Werte des Bauers (Z1–Z9) bleiben unverändert.

**Rest, unverändert seit Runde 2:**
- Unter weißem Licht (Tafeln 05, 06, 07) ist die Haut kupfern-lachsfarben mit rosa Adern.
- G4 ist unter demselben Licht fast schwarz mit roten Adern.
- Das ist der offene Konflikt O3 gegen V2. Im Spiel ist es kein Ablehnungsgrund. Bei einer Weißlicht-Tafel für den Nutzer wäre „zu kupfern“ als Rückmeldung möglich.
- Die runden Kniepolster der Hinterbeine sind in der Bild-1-Lage etwas glatter und heller als der Rest. Das war bei K5e und in Runde 2 genauso.

### (2) Sitzen die Flecken wie in den Nutzerbildern? Abweichung klein

**Lage und Form** (Tafel 09 mit türkisen Umrandungen, verglichen mit der Übersicht 00):
- Jede Zone sitzt an ihrer Stelle:
  - Wunde 2;
  - Musterung 3;
  - Schwanz 9/11;
  - rotes Fleisch 12 am Zipfel;
  - weiße Musterung 14/18 an Becken und Gelenken;
  - weiße Musterung 24 an der rechten Schulter;
  - braune Musterung 16/19 an den Hinterbeinen, wie in `rechtes_bein_hinten.png` und `linkes_bein_hinten.png` knapp unter der weißen Musterung;
  - Buckel-Fleisch 27;
  - Augen 29.
- Die Ränder folgen der Texelmaske. Es gibt keine Quader, keine Klötze und keine Weichzeichnung.

**Nirgends sonst:**
- Der Pixelvergleich Runde 2 gegen Runde 3 zeigt in Tafel 09 und im Spiel nur Änderungen an den Zonen 16/19, an der Hals-Unterseite und an hellen Einzelpunkten.
- Es ist kein neues Fleck-Material an fremden Stellen aufgetaucht.

**Braune Musterung, der rote Punkt der Runde 2, ist behoben:**
- **Weißlicht** (`braun16_zoom.png`):
  - Runde 2 war ein glatter oliv-grauer Schmierfleck.
  - Runde 3 zeigt hellere und dunklere Texel in der Form der Original-Struktur.
  - Gemessen an den geänderten Pixeln der Tafel 09:

| Zone | Pixel | L*-Spanne P10–P90 R2 | L*-Spanne P10–P90 R3 | Stufen R2 | Stufen R3 | Farbton |
|---|---|---|---|---|---|---|
| 16 | 470 | 18,8–82,4 | 9,2–77,2 | 29 | 39 | 69/70 Grad |
| 19 | 455 | 17,4–63,2 | 13,2–57,6 | 19 | 20 | – |

  - In Zone 16 fiel der untere Rand der Spanne von 18,8 auf 9,2. Der Fleck hat also jetzt dunkle Tiefen, statt einheitlich mittelhell zu sein.
- **Im Spiel** (Messbild des Bauers und mein `braun_x5.png`):
  - Runde 2: eine flache dunkle Olivfläche.
  - Runde 3: dieselbe Stelle mit Hell-Dunkel-Struktur.
  - Im Spiellicht ist sie sehr dunkel und fällt kaum auf. Sie wirkt nicht aufgemalt.
- **Kleiner Rest:**
  - Unter weißem Licht ist das Braun eher oliv-ocker als braun (C* 13,2 bei Farbton 70 Grad), auf der kupfernen Haut.
  - In Zone 16 sitzen weiter einzelne grau-türkise Pixel am Kniegelenk wie in Runde 2.
  - Beides ist klein und im Spiel nicht zu sehen.

**Erkennbarkeit, unverändert:**
- Die Musterungen 3, 9, 11, 12 und 16/19 heben sich auf der geäderten Haut wenig ab.
- Deutlich zu sehen sind F1 (lila-rosa), das Buckel-Fleisch 27 und die grau-bläuliche weiße Musterung 24 an der Schulter.

### (3) Hals-Unterseite, Lippe, Unterkiefer: wie gewünscht

**Keine roten Streifen:**
- Im echten Spielfenster im Griff (5090, Clip 27 Bild 20, `griff_hals_vergleich_x4.png`) zeigt K5e die roten Bänder deutlich. Runde 2 und Runde 3 zeigen dort keine.
- Runde 3 ist an dieser Stelle etwas dunkler und gleichmäßiger, weil die rosa Fleisch-Strähnen der Runde 2 weg sind.
- Oben im Bild läuft eine dünne rote Linie entlang der Zahnreihe. Das ist der Maulspalt, also Zone 30, Maul innen.

**Weißlicht von unten** (Tafel 05, 90/−45, `diff05_hals_x3.png` und `h05_unten_k5g3_x3.png`):
- Die Fläche an der rechten Halsseite trug in Runde 2 rötliche Fleisch-Strähnen. Jetzt ist sie normale Haut ohne Kante zur Umgebung.
- Dasselbe gilt am Kinn in der Ansicht 0/−60 (`diff05_kinn_x3.png`). G7 ist im Bild bestätigt.

**Farbe:** Lippe, Unterkiefer außen und Hals-Unterseite sind dunkel graubraun, weder schwarz noch rot. Im Spiel sind sie dunkel mauve-braun wie die übrige Haut.

**Maul innen:** rot, nur im Maulspalt bzw. im offenen Maul (`vorn_maul_k5g3_x4.png`). Das passt zu „geöffnetes Maul“ in `Gesicht vorne.png`.

**Kleinigkeiten, unverändert seit Runde 2:**
- ein kleiner orange-brauner Fleck im Maulwinkel, etwa 12 × 10 Pixel im Spielfenster;
- in der Weißlicht-Frontansicht (Tafel 09, Zone-29-Kachel) ein rosaroter Fleck unter der unteren Zahnreihe, RGB etwa (151, 56, 75). Er ist pixelgleich mit Runde 2 (0 von 1250 Pixeln verschieden), und K5e hat an derselben Stelle seinen schwarzen Maul-Streifen. Es ist also Maul innen, das durch den Lippenspalt sichtbar ist (Riegel V4.1: 0,00 % röter auf Lippe/Unterkiefer);
- helle waagrechte Ader-Strähnen an Kinn und Kehle, nur unter weißem Licht;
- feine Restzacken des B4-Stands, vom Nutzer angenommen.

### (4) Rechte Hand: wie gewünscht

**Bild 2** (`b2_haende_k5g3_x3.png`) und Spielfenster von vorn:
- Die rechte Hand trägt Krallen in Krallenform.
- Die Farbe ist dunkel braun-mauve wie die Hand, mit derselben feinen Sprenkelung.
- Kein Rot, kein Reinweiß (Riegel V5: 0 Zugriffe 0x7FFF, 0 % röter, dE 3,6).

**Seit Runde 2 unverändert:** In Tafel 09 und den Hand-Ausschnitten ist kein Unterschied zu sehen.

Wie in Runde 2 zählt man je nach Blickwinkel mehr als vier Spitzen, weil Hand-Klingen und Krallen dieselbe Farbe haben. Die Lage der Hand entscheidet der Nutzer gesondert (G2/G11), sie wird hier nicht bewertet.

### (5) Neue Fehler? Abweichung klein

**Mosaik, Naht, falsche Zeile, Löcher:** keine neuen gefunden.
- Geprüft in allen eigenen Spielbildern (5080: vier Lagen, 5090: Bild-2-Pose, Griff und Lauf mit Schüssen) und unter weißem Licht.
- Die geänderten Stellen (Hals-Unterseite, Zone 16/19) fügen sich ohne Kante ein.
- Die Silhouette ist geschlossen. Im Spiel wurde nichts verworfen (verworfen=0).

**Ring um F1:** keiner (`b1_ruecken_k5e_k5g3_x3.png`). F1 ist lila-rosa wie in Runde 2.

**Fein geteilte Flächen 10/16/21/22:** unverändert gegen Runde 2, die Geometrie ist gleich. Es bleiben einzelne kurze Strähnen an Kehle und Unterkiefer, nur nah unter weißem Licht (Tafel 11, Rest 140–191 px je Ansicht).

**Fast weiße Hautpunkte (G8):**
- Im Spielfenster sehe ich keine hellen Einzelpunkte mehr.
- Der Vergleich von vorn zeigt die verschwundenen hellen Punkte auf dem Buckel als einzelne geänderte Pixel.
- Unter weißem Licht bleiben die hellsten Adern rosa-weiß (Deckelwort 0x631F, L* 84,5). Sie wirken wie helle Adern, nicht wie weiße Löcher.

**Augen, unverändert seit Runde 2** (`kopf_vorn_r2_r3_x5.png`):
- zwei kleine graue Punkte an den Nutzerstellen;
- erkennbar, aber unauffälliger als die hell-rosa Punkte im Video.

### (6) Klarer Schritt nach vorn gegenüber K5e und K5c? Abweichung klein

**Gegenüber K5e („komplett ausgeblasst“):** ja, deutlich.
- In jeder Spielpose ist K5g3 dunkel, warm und strukturiert. K5e ist grau-violett und flach.
- Die roten Hals-Streifen von K5e sind weg.

**Gegenüber K5c („bunt“):** ja.
- Ein Hautton, keine roten Krallen, keine farbfremden Flecken.
- Lippe und Unterkiefer sind graubraun statt rot.

**Gegenüber Runde 2:**
- Das Braun ist kein flacher Farbklecks mehr.
- An der Hals-Unterseite ist kein Buckel-Rot mehr.
- Die weißen Glanzpunkte sind weg.
- Der Gesamteindruck ist unverändert.

**Klein bleibt:**
- der kupferne Weißlicht-Eindruck;
- wenig sichtbare Flecken auf der geäderten Haut;
- graue Augen;
- Braun eher oliv.

## Zusammenfassung

| Punkt | Bewertung |
|---|---|
| (1) Spielwirkung | Abweichung klein: im Spiel wie die Original-Gegner; unter weißem Licht kupfern (O3/V2 offen) |
| (2) Flecken | Abweichung klein: Lage und Form wie Übersicht 00, nirgends sonst; Braun jetzt mit Struktur, aber eher oliv; Flecken auf der geäderten Haut wenig auffällig |
| (3) Hals, Lippe, Unterkiefer | wie gewünscht: keine roten Streifen, Buckel-Fleisch an der Hals-Unterseite weg, dunkel graubraun |
| (4) Rechte Hand | wie gewünscht (Farbe/Form; Lage gesondert) |
| (5) Neue Fehler | Abweichung klein: nichts Neues; Reste aus Runde 2 (Maulwinkel, Augen grau, Kehlen-Strähnen) |
| (6) Schritt nach vorn | Abweichung klein: klar besser als K5e und K5c; Weißlicht-Eindruck offen |

**Bestanden**, weil keine Antwort in die dritte Klasse fällt.

Hinweise an den Orchestrator, ohne Wertvorschlag:
- Weißlicht kupfern (O3 gegen V2): Das ist der einzige Punkt, an dem ein Nutzer-Einwand bei Weißlicht-Tafeln denkbar wäre.
- Augenfarbe grau gegen hell-rosa im Video.
- Braun eher oliv-ocker (Farbton 70 Grad, C* 13).
