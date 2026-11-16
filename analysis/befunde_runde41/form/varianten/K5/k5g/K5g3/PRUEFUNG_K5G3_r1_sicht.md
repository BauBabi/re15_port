# Prüfung K5g3, Runde 1: Sicht-Prüfer

Stand: 2026-11-14. Geprüft wurde die Abgabe unter `k5g/K5g3/`, also EMD `37407afd…` und TIM `2f7b2444…`, wie in FARBE.md Abschnitt K5g3.0 beschrieben. Das Spiel-Asset bleibt K5e_f1.

**Ergebnis: NICHT BESTANDEN.** Es gibt eine Abweichung, die der Nutzer ablehnen würde: Die Augen sind verschwunden. Der Rest ist ein deutlicher Schritt nach vorn.

## Was ich angesehen habe

- **Tafeln des Bauers:** 01 bis 10, alle mit Read, einzelne Ausschnitte bis zu 8-fach vergrößert.
- **Vorlagen:** die Nutzerbilder `Version_80%/4_Beiner_Zerlegt/*.png` (darunter Gesicht vorne und Rücken) und die abgenommene Übersicht `zonen_nutzer/v2/tafeln/00_uebersicht.png`.
- **Eigener Spiellauf:** mit `generator/kf_k5g3_fenster.sh` und `kf_k5g3_spiel.sh`. Jeder Lauf nutzt eine eigene exe-Kopie. gdigrab nimmt nur das Fenster der eigenen PID auf, ohne AUTOSHOT und ohne SOFTWARE_RENDER, die Variante kommt über RE15_RE2_ASSET_ROOT.
  - Aufgenommen: 5080 Bild-1-Lage, 5080 von vorn, 5080 Seite (Clip 2, Gier 1400) jeweils für K5e und K5g3, dazu 5090 Schnitt 11 in Bild-2-Pose und im Griff.
  - Die Bilder liegen unter `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/sicht3/fenster/`.
  - 5080 von vorn und Bild-1-Lage sind pixelgleich mit den Fensterbildern des Bauers (0 Pixel verschieden). Die 5090-Bilder weichen nur im animierten Feuer ab.
  - Der freie Kampflauf in 5090 zeigte die Form nicht im Bild (andere Kamera) und wird nicht verwendet.
- **Eigene Kacheln:** alle unter `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/sicht3/`.
  - `fenster_kopf_vorn.png`: Kopf im echten Spielfenster 5080 von vorn, K5e | K5g3, 3,5-fach.
  - `augen_bild2.png`: Augenpixel in Bild 2, 8-fach, grün markiert.
  - `09_augen.png`: Ausschnitt aus Tafel 09 „Augen (29)“, K5e | K5g3.
  - `fleckort_bild1.png`, `fleckort_bild2.png`, `fleck_b1_zoom.png`: wo das Fleck-Material im Bild liegt. Markiert sind Pixel, deren Zeile eine Fleck-Zeile ist und deren Farbe von der Basiszeile abweicht. Werkzeug: `werk/fleckort.py`.
  - `haende_rechts.png`, `haende_links.png`: Hände nah unter weißem Licht, K5c | K5e | K5g3.
  - `krallen_netze.png`: Netze je Farbe, K5c | K5g3.
  - `05_cmp_0.png`, `05_cmp_1.png`, `05_k5g3_r*.png`: Kopf und Hals von unten nah.
  - `griff_k5e_k5g3.png`: Hals-Unterseite im Spielfenster.
  - `seite_k5e_k5g3.png`: 5080 Seite.
  - `wunde_crop.png`: Leuchtstelle F1.
  - `b1_k5e_k5g3.png`, `b2_k5g3_zoom.png`, `b2_kopf_k5e_k5g3.png`: Ausschnitte aus Bild 1 und Bild 2.
- **Gegner im Spiellicht:** Lab-Werte aus den Framedumps der Tafel 08 (`werk/ton.py`), Ausschnitt um die Figur über das Normal-/Schlüssel-Paar:

| Figur | L* Median | C* Median | Farbton (Mittelvektor) |
|---|---|---|---|
| Zombie | 7,6 | 3,9 | −38,9° |
| Gorilla | 4,4 | 3,6 | 49,2° |
| EM30 | 13,5 | 5,3 | 21,3° |
| Tyrant | 2,0 | 3,0 | 45,8° |
| K5c | 2,3 | 1,4 | −25,5° |
| K5e_f1 | 8,5 | 4,3 | −26,7° |
| **K5g3** | **6,1** | **4,3** | **19,9°** |

## Antworten

### (1) Wirkt K5g3 im Spiel wie die anderen Gegner? Abweichung klein

- **Spielfenster 5080 und 5090:** Die Haut ist dunkel rotbraun bis mauve und hat sichtbare Hautstruktur. Sie wirkt nicht blass, nicht plastisch und nicht bunt, und aufgemalte Flecken sieht man nicht.
- **Messung im Spiellicht:**
  - Der Farbton liegt bei 19,9 Grad, also warm wie EM30 mit 21,3 Grad. K5e lag bei −26,7 Grad im Violetten.
  - L* 6,1 und C* 4,3 liegen im Bereich der Original-Gegner.
  - Gegen K5e (Seite 5080, `seite_k5e_k5g3.png`) wirkt K5g3 nicht mehr wie aus Plastik.
- **Rest unter weißem Licht** (Tafeln 05, 06, 07):
  - Die ganze Haut wirkt rostrot bis kupfern, mit kräftigen rosa Adern. Rücken, Beine, Kopf und Schwanz sehen gleich aus.
  - Das RE2-Original G4 ist dagegen fast schwarz mit roten Adern.
  - Die Albedo der Haut hat einen C*-Median von 11,98. Der belastbare Wert aus der Eichung ist C* 2–6. Damit ist V2 („warm-dunkelgrau, Buntheit gering“) unter weißem Licht nicht erfüllt.
  - Im Spiellicht sieht man das kaum. Im 5080-Fenster bleibt ein rosa-mauve marmorierter Eindruck.
  - Das ist der Konflikt zwischen dem Zielband aus O3 und V2, den der Bauer schon unter K5g3.5 Punkt 1 benannt hat. Ihn muss der Orchestrator entscheiden; es ist kein Bildfehler.

### (2) Sitzen die Flecken wie in den Nutzerbildern? Abweichung klein

- **Lage:** Das Fleck-Material liegt nur auf den Netzen seiner Zonen, nirgends sonst.

| Bild | Netz | Fleck-Pixel |
|---|---|---|
| Bild 1 | 0 | 212 + 152 |
| Bild 1 | 2–6 (Schwanz) | 2111 |
| Bild 1 | 8/9 | 153 |
| Bild 1 | 16/17 | 259 |
| Bild 2 | 10 | 3838 |
| Bild 2 | 21 | 3540 |
| Bild 2 | 22 | 16 + 5 |

  - Lage und Umriss passen zur Übersicht 00: Schwanz-Maserung 9/11, Wunde 2, beige Punkte 14/18 an den Gelenken, Buckel-Fleisch 27 als großer Bogen über Buckel und Hals.
  - Klötze gibt es keine, die Ränder folgen der Texelmaske.
- **Erkennbarkeit:** Die Flecken heben sich kaum ab.
  - Das Buckel-Fleisch ist in Bild 2 (Szenenlicht) heller und bunter als die Haut daneben: L* 8,7 gegen 5,7 und C* 10,0 gegen 5,4 bei gleichem Farbton. Ohne Videofilter wirkt der Buckel trotzdem fast wie die übrige Haut. Mit Videofilter ist er heller und röter als die Vorlage (Tafel 03).
  - Die „weiße Musterung“ 24 an der rechten Schulter liegt in Bild 1 im Schatten und ist nicht weiß zu sehen.
  - Das rote Fleisch 12 an der Schwanzspitze liegt in Bild 1 unten außerhalb des Bildes. In der Nahkachel von Tafel 09 ist es nicht als rote Stelle zu erkennen.
- **Mangel der Tafel 09:** Die Nahkacheln „Flecken nah“ zeigen den ganzen Teil ohne Markierung. Ob ein Fleck richtig sitzt, kann man daraus nicht ablesen. Meine Kacheln `fleckort_*.png` ersetzen sie.

### (3) Hals-Unterseite, Lippe, Unterkiefer: wie gewünscht

- Die roten Hals-Streifen sind weg: im Spielfenster im Griff (Clip 27 Bild 20, `griff_k5e_k5g3.png`) und unter weißem Licht von unten (Tafel 05, Ansicht 90/−45). K5e und K5c haben dort deutlich rote Bänder.
- Lippe, Unterkiefer und Hals-Unterseite sind dunkelbraun-grau, weder schwarz noch rot.
- Das Maul innen ist rot (Zone 30), die Zähne sind hell.
- Kleinigkeiten:
  - Auf der Kehle sieht man unter weißem Licht kurze, helle, waagrechte Strähnen, also gestreckte Texel.
  - Eine senkrechte Naht läuft in der Mitte des Halses.
  - In der Maulecke bei offenem Maul sitzt ein kleiner orange-brauner Fleck (Tafel 05, Clip 1 60/10).
  - Diese drei Punkte fallen nur nah unter weißem Licht auf.

### (4) Rechte Hand: wie gewünscht

- **Form:** Die rechte Hand hat 4 Krallen. Im Netzfarben-Bild (`krallen_netze.png`) liegen die Netze 19, 20, 29 und 31 so wie in K5c. Die Pixelzahlen in Clip 2 sind für alle Hand- und Krallen-Netze beider Hände in K5c und K5g3 gleich. Beispiel rechts, Ansicht 90/10: 19 = 638, 20 = 1039, 29 = 671, 31 = 181.
- **Farbe:** Sie entspricht der Hand. Kein Rot, kein Reinweiß.
- Kleinigkeiten:
  - Die Krallen tragen feine rosa Sprenkel, sie glitzern leicht.
  - Weil Hand und Krallen jetzt gleich gefärbt sind, verschmelzen die eigenen Klingenspitzen der Hand (Netz 18 bzw. 13) mit den Krallen. Je nach Blickwinkel sieht man dadurch 6 bis 7 Spitzen statt 4 (`haende_*.png`, Ansicht 0/−80).
- Die Lage der Hand in Bild 2 entscheidet der Nutzer gesondert und wird hier nicht bewertet.

### (5) Neue Fehler: Abweichung, die der Nutzer ablehnen würde (Augen)

**Augen verschwunden.**

- **K5e:** Zwei klar erkennbare Augen, dunkle Höhle mit rotem Rand. Zu sehen in Bild 2, im 5080-Fenster von vorn und unter weißem Licht von vorn. Tafel 10 zeigt sie auch auf der K5g3-Geometrie mit K5e-Textur, der Verlust kommt also aus der Textur.
- **K5g3 im echten Spielfenster 5080 von vorn** (`fenster_kopf_vorn.png`):
  - Kein Auge zu sehen.
  - An der rechten Augenhöhle sitzt ein runder schwarzer Fleck, der wie ein Loch wirkt.
  - Er liegt auf Netz 22, Flächen 234–241, Zeile haut1, Einträge 213, 204 und 255. Das sind die als „dunkelster Eintrag“ angehobenen Schwarz-Einträge.
  - Unter weißem Licht von vorn sind 57 Pixel auf Netz 22 nahezu schwarz (RGB-Max ≤ 16), bei K5e sind es 6 Pixel.
  - An der linken Augenhöhle ist nichts.
- **Bild 2 (Szenenlicht):** Das Augen-Material (Zeile fleck_tausch0_4) trifft genau 5 Pixel. Es ist ein 1×4-Pixel-Strich in Braun, RGB maximal (86, 53, 46), gegen einen Kopf-Median von 27 (`augen_bild2.png`).
  - Das zweite Auge ist in Bild 2 nicht sichtbar.
  - Im Video sind es zwei helle, weißlich-rosa Punkte (Nutzerbild „Gesicht vorne.png“, grüne Striche „Augen“).
- **Tafel 09 „Augen (29)“, weißes Licht von vorn:** K5e zeigt zwei Augen, K5g3 keines (`09_augen.png`).
- **Warum der Riegel A1 das nicht fängt:** Er prüft nur, ob das 9×9-Maximum mindestens doppelt so hell ist wie die Kopfhaut daneben (25,8 gegen 5,7). Das erfüllt schon ein 4-Pixel-Strich. Ob ein Mensch dort ein Auge erkennt, prüft er nicht. Dazu kommt: Die Quellen-Einträge der Augen haben nur L* 16–38 (bau.json textur.auge), helle Punkte entstehen daraus nicht.
- **Warum der Nutzer ablehnen würde:** Er hat am 2026-11-09 gesagt: „Die Augen abzulehnen ist natürlich schon etwas gravierend....“ K5g3 zeigt im Spiel ein Gesicht ohne Augen und mit einem einseitigen schwarzen Loch. Das ist ein Rückschritt gegen K5e.

**Weitere neue Fehler, alle Abweichung klein:**

- **Gestreckte Farbstreifen an Kopf und Unterkiefer:** Sie sind unter weißem Licht deutlich (Tafel 05, Clip 1 0/−60, `05_cmp_1.png`) und im Spielfenster schwach zu sehen (Oberkopf links in `fenster_kopf_vorn.png`). Die Ursache sind die UV-entarteten Dreiecke von Netz 22 aus dem Spalt (GEOMETRIE.md 7). Mit der kontrastreicheren K5g3-Textur fallen sie stärker auf als mit K5e.
- **Leuchtstelle F1:** Sie wirkt auf der kupfernen Haut lila-grau, mit grauen Blöcken (`wunde_crop.png`, Bild 1). Einen schwarzen Ring gibt es nicht.
- **Kein Befund:**
  - Kein Mosaik über ganze Flächen.
  - Keine falschen Zeilen; jede Fläche liest eine gültige Zeile (Riegel T5).
  - Keine Löcher in der Silhouette.
  - Ein grauer Block am Bauch im Griff (`griff_k5e_k5g3.png`) ist klein.

### (6) Klarer Schritt nach vorn gegenüber K5e und K5c? Abweichung klein

- **Haut:** ja.
  - Gegenüber K5e ist K5g3 nicht mehr ausgeblasst und nicht mehr violett.
  - Gegenüber K5c ist es nicht bunt und hat keine roten Krallen.
  - Die roten Hals-Streifen sind weg.
- **Gesicht:** Ohne Augen und mit dem schwarzen Fleck an der rechten Augenhöhle ist das Gesicht ein Rückschritt.
- **Weißes Licht:** Der rostrote, rosa geäderte Gesamteindruck (siehe 1) könnte dem Nutzer als „zu rot“ auffallen.
- In der Summe ist K5g3 kein klarer Schritt nach vorn, solange die Augen fehlen.

## Zusammenfassung

| Punkt | Bewertung |
|---|---|
| (1) Spielwirkung | Abweichung klein |
| (2) Flecken | Abweichung klein |
| (3) Hals, Lippe, Unterkiefer | wie gewünscht |
| (4) Rechte Hand | wie gewünscht |
| (5) Neue Fehler | **Abweichung, die der Nutzer ablehnen würde** (Augen weg, schwarzes Loch rechts) |
| (6) Schritt nach vorn | Abweichung klein (Haut ja, Gesicht nein) |

Damit ist K5g3 **nicht bestanden**.

**Hinweis an Bauer und Orchestrator, ohne Wertvorschlag:**

- Die zwei Augenpunkte müssen im Spiel als Augen erkennbar sein. Bisher prüft das kein Riegel; nötig ist eine Sichtprüfung im Fenster.
- Die angehobenen Schwarz-Einträge an der früheren Augenhöhle dürfen kein Loch bilden.
- Der Konflikt O3 gegen V2, also die rostrote Haut unter weißem Licht, ist zu entscheiden.
