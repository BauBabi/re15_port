# Prüfung K5g3, Runde 2: Sicht-Prüfer

Stand: 2026-11-14. Geprüft wurde die Abgabe unter `k5g/K5g3/`:

| Datei | sha256 |
|---|---|
| EMD | `482bd63f…` |
| TIM | `ce9a1dc9…` |
| FORM.EMD | `9eab983e…` |

Die Hashes habe ich nachgerechnet, sie stimmen mit FARBE.md K5g3R2.0 überein. Das Spiel-Asset ist weiter K5e_f1 (EMD `f277916d…`, TIM `31a35e66…`).

**Ergebnis: BESTANDEN.** Es gibt keine Abweichung, die der Nutzer ablehnen würde. Zwei Punkte sind „wie gewünscht“, vier sind „Abweichung klein“. Die Augen-Lücke aus Runde 1 ist geschlossen.

## Was ich angesehen habe

### Tafeln und Vorlagen

- **Tafeln des Bauers:** alle Tafeln 01 bis 11 mit Read. Ausschnitte habe ich 2- bis 8-fach vergrößert: Rücken in Bild 1, Augen in Bild 2, rechte Hand in Bild 2, die K5g3-Spalte der Tafel 05 sowie Tafel 09 Zonen 16, 19 und 29.
- **Vorlagen:**
  - die Nutzerbilder `C:/workspace/git/reAi_v2/Version_80%/4_Beiner_Zerlegt/*.png`, gelesen wurden „Rücken“, „Gesicht vorne“, „Schwanz_hinten“ und „Bein_Vorne_rechts“; die übrigen kenne ich aus Tafel 09;
  - die abgenommene Übersicht `C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/zonen_nutzer/v2/tafeln/00_uebersicht.png`.

### Eigene Spielläufe

Rahmenbedingungen für jeden Lauf:
- eigene exe-Kopie;
- gdigrab nur auf das Fenster der eigenen PID;
- kein AUTOSHOT und kein SOFTWARE_RENDER;
- die Variante kommt über RE15_RE2_ASSET_ROOT, K5e läuft über den Rückfall auf shared_assets;
- beendet wurde nur die eigene PID.

Werkzeuge waren `generator/kf_k5g3_fenster.sh` und `kf_k5g3_spiel.sh`, aufgerufen von `lauf_alle.sh`. Alle Pfade unten liegen unter `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/sicht4/`.

| Lauf | K5e | K5g3 | Ablage |
|---|---|---|---|
| 5080 Bild-1-Lage (Clip 0 Bild 11) | ja | ja | `fenster/` |
| 5080 von vorn (Clip 12 Bild 0) | ja | ja | `fenster/` |
| 5090 Schnitt 11, Bild-2-Pose (Clip 12) | ja | ja | `fenster/` |
| 5090 Schnitt 11, Griff (Clip 27 Bild 20, Hals-Unterseite) | ja | ja | `fenster/` |
| 5080 Seite (Clip 2, Gier 1400) | ja | ja | `seite_*/` |
| 5080 von hinten (Clip 2, Gier 3800) | ja | ja | `hinten_*/` |
| 5090 Schnitt 11 mit Schüssen (3 Bilder) | nein | ja | `schuss_k5g3_5090/` |

- `fenster/k5g3_vorn_r5080.png` ist **pixelgleich** mit dem Augen-Lauf des Bauers `k5g2/t3r2/augen_spiel/augen_normal/fenster_1.png` (0 Pixel verschieden). Die Spielmessungen des Bauers sind damit nachvollzogen.
- **Dreiecks-Warteschlange in allen Läufen:** „[tri-liste] … verworfen=0 bilder_mit_verlust=0 grenze=16384“. Der höchste Stand war 7318, im 5090-Lauf mit Schüssen.

**Gegner im Spiellicht:** eigener Lauf mit `gegner.sh`, Ablage `gegner_1140/`, `gegner_11C0/`, `gegner_50A0/`. Er hat Zombie (1140, liegend) und Gorilla (11C0) im echten Fenster aufgenommen. In 50A0 war ohne Schnitt 4 kein Gegner im Bild; für EM30 und Tyrant nehme ich deshalb Tafel 08 des Bauers.

### Eigene Kacheln (roh, nearest)

Alle unter `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/sicht4/`:

- **Spielfenster:**
  - `kachel_spiel_k5e_k5g3.png`: sechs Posen, K5e | K5g3.
  - `kachel_gegner_spiellicht.png`: Zombie, Gorilla, K5g3 5080, K5g3 5090.
  - `kachel_schuss_5090.png`: K5g3 in 5090 mit Schüssen.
  - `seite_k5e_oben_k5g3_unten_x2.png`: Seitenansicht.
  - `vorn_kopf_k5g3_x5.png`: Kopf von vorn.
  - `vorn_haende_k5e_x3.png` und `vorn_haende_k5g3_x3.png`: Hände.
  - `griff_k5g3_x3.png`: Hals-Unterseite im Griff.
- **Ausschnitte aus Bild 2 (Tafel 02):**
  - `augen_b2_v_k5e_k5g3.png`: Augen, Vorlage | K5e | K5g3, 8-fach.
  - `b2_k5g3_kopf.png`: Kopf.
  - `b2_hand_rechts_k5g3.png` und `b2_hand_rechts_k5e.png`: rechte Hand.
- **Ausschnitte aus Bild 1 (Tafel 01):** `b1_k5g3_ruecken.png` und `b1_k5e_ruecken.png`, Rücken.
- **Ausschnitte aus Tafel 05:** `h05_ruhe.png`, `h05_unten.png`, `h05_offen.png`.
- **Ausschnitte aus Tafel 09:** `z09_a.png`, `z09_b.png`, `z16_k5g3.png`, `z29.png`.

## Antworten

### (1) Wirkt K5g3 im Spiel wie die anderen Gegner? Abweichung klein

**Spielfenster 5080 und 5090** (`kachel_spiel_k5e_k5g3.png`, 6 Posen):
- K5g3 ist dunkel rotbraun bis mauve und zeigt deutliche Hautstruktur.
- Nicht blass, nicht plastisch, nicht violett-pastell.
- Gegen K5e ist der Unterschied in jeder Pose sofort sichtbar: K5e ist eine glatte grau-violette Masse, K5g3 hat Struktur.
- In der Seitenansicht (`seite_k5e_oben_k5g3_unten_x2.png`) liest sich K5g3 als lebendige, geäderte Haut. K5e wirkt wie aus Kunststoff.

**Neben den Original-Gegnern** (`kachel_gegner_spiellicht.png`, Tafel 08):
- Der Zombie ist dunkelgrau-blau, der Gorilla braun mit rötlichen Wunden. K5g3 liegt in Helligkeit und Strukturdichte dazwischen und fällt nicht aus der Reihe.
- Messung des Bauers, die ich am pixelgleichen Fenster nachvollzogen habe (`spiel_z.json`, Bild-1-Lage):

| Wert | K5g3 | K5e | Band der Originale |
|---|---|---|---|
| L* | 7,79 | 9,45 | 5,57–18,8 |
| L-cv | 0,708 | 0,409 | – |
| lokaler Kontrast | 0,363 | 0,168 | – |
| C* | 4,84 | – | 4,41–7,59 |
| Farbton | 20 Grad | 343 Grad | – |

**Kein Bunt:** Unter weißem Licht hat die Haut einen einzigen Farbton (Ruhe R3/R4 1,1 Grad), es gibt keine farbfremden Flecken (R2 0,0). Das Bild „Hautprobleme, alles bunt“ von K5c/K5d entsteht nicht.

**Rest, nur unter weißem Licht** (Tafeln 05, 06, 07):
- Die ganze Haut ist lachs- bis kupferfarben mit hellrosa Adern. Gemessene Hautpixel sind z. B. RGB (109, 70, 70): G gleich B, also ein einziger rosa-kupferner Ton.
- Das RE2-Original G4 ist unter demselben Licht fast schwarz mit roten Adern.
- Im Spiellicht ist davon nur ein mauve-rosa Schimmer der Adern übrig.
- Das ist der bekannte Konflikt O3 gegen V2 (FARBE.md K5g3R2.5 Punkt 3): Albedo-C* 11,98 gegen Eichung 2–6.
- Sollte der Nutzer Weißlicht-Tafeln (06/07) zu sehen bekommen, ist „zu kupfern/rosa“ als Rückmeldung möglich. Im Spiel ist es kein Ablehnungsgrund.

### (2) Sitzen die Flecken wie in den Nutzerbildern? Abweichung klein

**Lage** (Tafel 09 mit türkisen Umrandungen, `z09_a.png`, `z09_b.png`):
- Jede Zone sitzt an der Stelle der Übersicht 00:
  - Wunde 2: schmaler Streifen rechts am Becken mit kleinen Inseln;
  - Musterung 3: an der rechten Flanke;
  - Schwanz-Maserung 9/11: zwei längliche Felder auf dem Schwanz, wie die grünen Striche in „Schwanz_hinten“;
  - rotes Fleisch 12: am Schwanzende;
  - weiße Musterung 14/18: Punkte am Becken und an den Gelenken;
  - weiße Musterung 24: an der rechten Schulter, wie in „Bein_Vorne_rechts“;
  - braune Musterung 16/19: an den Beinen;
  - Buckel-Fleisch 27: großer Bogen über Buckel und Halsseiten;
  - Augen 29.
- Die Ränder folgen der Texelmaske. Es gibt keine Quader und keine Klötze, und die Flecken sind nicht weichgemalt.

**Nirgends sonst:** Das kann ich mit dem Auge nur stichprobenhaft prüfen. In den Weißlicht-Ansichten sehe ich kein Fleck-Material außerhalb der Umrandungen. Der Riegel V3.1 zählt 0 fremde Leser und 0 Material außerhalb der Texel.

**Erkennbarkeit, der eigentliche Rest:**
- Die Grundhaut ist selbst kräftig geädert, die Adern sind G4-Struktur mit hellen kupfernen Strähnen.
- Deshalb heben sich die Musterungen 3, 9, 11 und 12 kaum von der Haut ab. Ohne Umrandung findet man sie nicht.
- Im Video ist die Haut fast schwarz, und die Flecken leuchten daraus hervor.
- Deutlich erkennbar sind nur:
  - die Wunde F1 als lila-rosa Streifen;
  - das Buckel-Fleisch 27, heller und röter;
  - die grau-bläuliche weiße Musterung 24 an der Schulter.

Dazu drei Kleinigkeiten:
- **Braune Musterung 16:** unter weißem Licht graubraun bis oliv, z. B. RGB (61, 55, 49) und (72, 60, 48), auf rosa-kupferner Haut (109, 70, 70). Sie wirkt eher olivgrau als braun. Im Spiellicht ist sie nicht zu sehen.
- **Zone 16 nah:** Ein dunkler runder Bereich am Kniegelenk trägt einzelne türkisgraue Pixel (`z16_k5g3.png`). Sie sind klein und im Spiel nicht sichtbar.
- **Wunde F1 auf Kupfer:** Sie hebt sich in Bild 1 als lila Fleck ab (`b1_k5g3_ruecken.png`). Das passt zur Vorlage „Rücken“, wo die fleischige Wunde rosa-violett leuchtet. F1 hat der Nutzer abgenommen (E30).

### (3) Hals-Unterseite, Lippe, Unterkiefer: wie gewünscht

**Keine roten Streifen am Hals:**
- Im echten Spielfenster im Griff (5090, Clip 27 Bild 20) zeigt K5e deutlich rote Bänder am Hals. Bei K5g3 ist dort keines (`kachel_spiel_k5e_k5g3.png` Zeile 4, `griff_k5g3_x3.png`).
- Unter weißem Licht von unten (Tafel 05, 90/−45, `h05_unten.png`) sind Kehle und Hals-Unterseite dunkel graubraun.

**Lippe, Unterkiefer außen, Hals-Unterseite:** dunkelgrau-braun, weder schwarz noch rot. Riegel V4.1 und V4.2 zeigen 0 % röter als die Haut und 0 % schwarz.

**Maul innen:** rot, nur im Maulspalt bzw. im offenen Maul. Das ist Zone 30 mit G4-Mundfleisch, wie entschieden, und passt zu „geöffnetes Maul“ in „Gesicht vorne“.

Kleinigkeiten, nur nah unter weißem Licht zu sehen (wie in Runde 1):
- kurze helle waagrechte Strähnen am Unterkiefer;
- eine senkrechte Naht in der Halsmitte;
- ein orange-brauner Fleck mit schwarzem Kreuzpixel im Maulwinkel bei offenem Maul (`h05_offen.png`);
- im Griff ein kleiner grau-blauer Keil an der Brust und zwei hellgraue Bänder am Bauch (`griff_k5g3_x3.png`).

Der vom Nutzer angenommene B4-Stand mit feinen Restzacken gilt.

### (4) Rechte Hand: wie gewünscht

**Form:**
- In Bild 2 (`b2_hand_rechts_k5g3.png`) und im Fenster von vorn (`vorn_haende_k5g3_x3.png`) trägt die rechte Hand Krallen in Krallenform.
- Die Netze 19, 20, 29 und 31 sind nach dem Riegel TG geometriegleich mit der Geometrie-Stufe aus K5c. Die Pixelzählung der Runde 1 ist unverändert, weil die Geometrie gleich ist.

**Farbe wie die Hand:**
- dunkel braun-mauve mit derselben feinen Sprenkelung;
- kein Rot, kein Reinweiß;
- Riegel V5: 0 Zugriffe auf 0x7FFF, 0 % röter, dE 3,6 zur Hand.

**Rest:**
- Hand-Klingen und Krallen haben dieselbe Farbe. Je nach Blickwinkel zählt man darum 5 bis 7 Spitzen statt 4.
- Die Lage der Hand in Bild 2 entscheidet der Nutzer gesondert (G2), sie wird hier nicht bewertet.
- Die Hand liegt in der Seitenansicht anders als bei K5e. Das kommt von den K5c-Posen und ist so gewollt.

### (5) Neue Fehler? Abweichung klein

**Augen, der Befund aus Runde 1, behoben:**
- **Echtes Spielfenster 5080 von vorn** (`vorn_kopf_k5g3_x5.png`): An beiden Nutzerpunkten sitzen kleine helle Punkte.

| Auge | Größe | Mitte | Median L* | Kopfhaut daneben |
|---|---|---|---|---|
| 1 | 9 px | (259, 236) | 15,6 | 4,7 |
| 2 | 15 px | (332, 240) | 17,4 | 4,1 |

  - Das sind meine eigenen Werte, gemessen am pixelgleichen Fenster.
  - Im 50×50-Fenster um Auge 2 sind nur 1,7 % der Kopfpixel mindestens so hell wie das Auge.
  - Das schwarze Loch an der rechten Augenhöhle ist weg.
- **Bild 2** (`augen_b2_v_k5e_k5g3.png`): Die zwei Punkte liegen genau unter den Nutzerpunkten.
- **Verbleibende Abweichung:**
  - Die Punkte sind grau, praktisch unbunt: Lab-Median (15,6 / 1,9 / 0,7) und (17,4 / 2,5 / 0,9), RGB etwa (45, 40, 40), Maximum (86, 80, 80).
  - Im Video sind sie hell rosa (Lab 30 / 37 / −2).
  - In Bild 2 ist das linke Auge zwischen den hellen Adern der Stirn schwer auszumachen.
  - Als Augen erkennbar sind sie, aber unauffälliger als im Video.
- Größe und Lage stimmen mit den Nutzerpunkten überein (8/12 px gegen 6,5/12,3 px), die Farbe kommt aus den G4-Augeneinträgen (V3). Deshalb werte ich das als **Abweichung klein**.

**Weitere Prüfpunkte:**
- **Mosaik, falsche Zeile, Löcher:** Keine gefunden, weder in den 6 Spielposen noch im 5090-Lauf mit Schüssen (`kachel_schuss_5090.png`) noch unter weißem Licht. Die Silhouette ist geschlossen. Riegel K1 zeigt 0 von 457.031 Zugriffen auf ungeschriebene Texel, T5 0 ungültige Zeilen.
- **Ring um F1:** keiner (Bild 1, `b1_k5g3_ruecken.png`). F1 ist lila-rosa mit grauen Blöcken, wie in Runde 1.
- **Verzerrte Textur auf den fein geteilten Flächen von 10, 16, 21 und 22:**
  - Gegen Runde 1 ist das stark zurückgegangen (Tafel 11, Linien-Dreiecke):

| Ansicht | Runde 1 | Runde 2 |
|---|---|---|
| 0/−60 | 194 px | 140 px |
| 0/10 | 539 px | 180 px |
| 180/20 | 1424 px | 191 px |
| 90/−45 | 1286 px | 163 px |

  - Was bleibt, sind einzelne kurze Strähnen an Kehle und Unterkiefer, nur nah unter weißem Licht zu sehen. Im Spielfenster sind sie nicht auffällig.
- **Dreiecks-Warteschlange:** In allen eigenen Läufen wurde nichts verworfen (höchster Stand 7318 von 16384). Es fehlen also keine Flächen.

### (6) Klarer Schritt nach vorn gegenüber K5e und K5c? Abweichung klein

**Gegenüber K5e („komplett ausgeblasst“):** ja, deutlich.
- In allen sechs Spielposen ist K5g3 dunkel, strukturiert und warm. K5e ist grau-violett, flach und wie aus Kunststoff.
- Die roten Hals-Streifen von K5e sind weg.
- K5e hatte große gemalte dunkle Augen mit rotem Rand, K5g3 hat zwei kleine helle Punkte an den Nutzerstellen.

**Gegenüber K5c („bunt“):** ja.
- Ein Hautton, keine roten Krallen, keine farbfremden Flecken. Lippe und Unterkiefer sind grau.

**Klein bleibt:**
- der kupfern-rosa Gesamteindruck unter weißem Licht (siehe 1);
- die Flecken heben sich auf der geäderten Haut kaum ab (siehe 2);
- die Augen sind grau statt hell rosa (siehe 5).

## Zusammenfassung

| Punkt | Bewertung |
|---|---|
| (1) Spielwirkung | Abweichung klein: im Spiel wie die Original-Gegner; unter weißem Licht kupfern-rosa, Konflikt O3/V2 |
| (2) Flecken | Abweichung klein: Lage und Form wie die Übersicht, nirgends sonst; auf der geäderten Haut wenig auffällig; braun 16 wirkt oliv |
| (3) Hals, Lippe, Unterkiefer | wie gewünscht |
| (4) Rechte Hand | wie gewünscht (4 Krallen, Farbe wie die Hand; Lage gesondert) |
| (5) Neue Fehler | Abweichung klein: Augen da, aber grau und unauffällig; Reste an Maulwinkel und Kehle |
| (6) Schritt nach vorn | Abweichung klein: gegen K5e und K5c klar besser; Weißlicht-Eindruck offen |

**Bestanden**, denn keine Antwort fällt in die dritte Klasse.

**Hinweise an den Orchestrator, ohne Wertvorschlag:**
- O3 gegen V2: die kupferne Haut unter weißem Licht.
- Die Augenfarbe: grau aus den G4-Einträgen gegen hell rosa im Video.
- Wenn die Flecken stärker sichtbar sein sollen: Das ginge nur über die Grundhaut, nicht über die Flecken selbst.
