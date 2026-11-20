# Sichtprüfung K5g2, Runde 1 (Riesenform, Auge des Nutzers)

Stand: 2026-11-09, etwa 22:15 bis 22:45. Nicht committet.

**Ergebnis: NICHT BESTANDEN.** Vier Punkte würde der Nutzer ablehnen:
- die Augen fehlen,
- die rechte Hand ist fast weiß,
- zwei gespiegelte Krallen stehen als Stachel aus der Figur heraus,
- in der Vorderansicht steckt der Kopf noch streifig im Buckel.

Die Richtung stimmt aber: warm-dunkelgrau statt violett-blass, und die roten Hals-Streifen sind weg.

## Was geprüft wurde

- **Geprüfter Stand:** Entwicklungsbau mit f 2,5. Ich habe ihn um 22:15 eingefroren, weil der Bauer ihn während der Halbierungssuche nach f überschreibt.
  - Kopie: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/sicht/T_k5g2_kopie_2205
  - Prüfsummen laut bau.json: FORM.EMD a5d30d9d..., BAHNSTEIG_FORM.EMD 4f0e3e15..., TIM 7d5bed31...
- **f wird noch kleiner.** Die Suche lag zuletzt bei f 1,2 (Bild-2-Video 16,9, Vorlage 15,37). Alles wird also dunkler.
  - Die Helligkeit wirkt nur multiplikativ. Darum bleiben die Verhältnisse, die ich unten bemängle, gleich.
  - Das gilt für: Hand gegen Haut, Augen gegen Umgebung, Rücken gegen Beine.
  - Geometrie und Streifen hängen nicht an f.
- **Eigene Spielläufe:**
  - Werkzeug: re15_port/tools/form_spiel.sh mit eigener exe-Kopie und eigener PID. gdigrab nur auf das eigene Fenster, kein AUTOSHOT, kein SOFTWARE_RENDER. Die Variante kommt über RE15_RE2_ASSET_ROOT.
  - Gegenlauf mit dem Spiel-Asset K5e_f1, gleiche Posen.
  - Alle 14 Läufe rc=0, Verfahren gdigrab.
  - Skript: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/sicht/lauf.sh
  - Bilder: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/sicht/spiel/{k5e,k5g2}_{pose1_r5080, vorn_c12_r5080, vorn_c0_r5080, vorn_c1_r5080, pose2_r5090_s11, c1_r5090_s11, griff_r5090_s11}.png
- **Spiel-Asset unberührt:** shared_assets/RE2/BAHNSTEIG_FORM.EMD f277916d..., TIM 31a35e66... (= K5e_f1). Kein Commit, kein push oder merge. Nichts in Version_80%/, pri/ oder synchro/ geändert.
- **Gesehen habe ich:**
  - die Nutzerbilder Version_80%/4_Beiner_Zerlegt (Rücken, Vorne_Oberkörper, Gesicht vorne, vorne_rechte_Hand_und_Kralle),
  - die abgenommene Übersicht zonen_nutzer/v2/tafeln/00_uebersicht.png,
  - k5f/08_gegner_spiellicht.png,
  - die Szene des Bauers C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/s2_szene.png.
- **Tafeln des Bauers:** C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/k5g/K5g2/tafeln/ war zum Prüfzeitpunkt leer, darum sind keine Bauer-Tafeln geprüft.

Alle eigenen Kacheln (roh) liegen unter C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/sicht/:

| Kachel | Inhalt |
|---|---|
| v_5080.png | Spiel 5080: K5e links, K5g2 rechts; oben Rückansicht, unten Vorderansicht |
| v_5080_vorn_zoom.png, v_5080_kopf_zoom.png | Spiel 5080 von vorn, vergrößert |
| v_5080_pose1_zoom.png | Spiel 5080 Rückansicht, vergrößert |
| v_5090.png, v_5090_zoom.png, v_5090_kopf_zoom.png | Spiel 5090 (Feuer), Bild-2-Lage |
| v_griff.png, v_griff_zoom.png | Spiel 5090, Griff-Pose (Hals-Unterseite) |
| gegner_vergleich.png | andere Gegner im Spiellicht (k5f/08) neben K5g2 im Spiel |
| z_b2.png, z_b1.png | Bild 2 / Bild 1, K5e gegen K5g2 (Szene des Bauers, vergrößert) |
| z_b2_augen.png | Augenbereich: Video, K5e, K5g2 |
| hand_meshkarte_b2.png | rechte Hand Bild 2: Farbe und Teilekarte (gelb Hand 18, grün 19, türkis 20, blau 29, magenta 31) |
| keil_auf_bild2.png | Bild 2: Kopf über Buckel (magenta) / Buckel über Kopf (türkis), wo Malreihenfolge und Tiefe widersprechen |
| keilz_vergleich.png | dasselbe als Teilekarte, K5e und K5g2 |
| fleck_b1.png, fleck_b2.png | Bild 1/2 ohne Licht; grün = Pixel, die Fleck-Material tragen |
| rund_ganz.png, rund_kopf.png | Rundum K5e gegen K5g2 (weißes Licht) |
| krallen_rund.png | linke gegen rechte Hand, Ruhepose |
| f1_zoom.png | Leuchtstelle F1 am Becken |

Messskripte: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/sicht/py/
- bild.py, auge.py, hand.py, rotkopf.py: Bild-2-Kennungspuffer
- keilz.py, keilz_pk.py: eigene Keil-Zählung, Tiefenpuffer gegen Malreihenfolge, nur Gruppen {22,23}/{10,21}
- mesh_hell.py: Helligkeit je Teil, weißes Licht
- zeilen.py: CLUT-Zeilen je Fläche
- f1.py, krallen.py, fleck.py

## Antworten auf die sieben Fragen

### (1) Wirkt K5g2 im Spiel wie die anderen Gegner? Abweichung klein

**Was gut ist:**
- Nicht mehr violett. Spiel 5080, Rückansicht, Figurpixel aus der Differenz zum K5e-Lauf:
  - Farbton-Median +20 Grad (K5e -29 Grad),
  - Buntheit C\* 1,3 (K5e 4,0),
  - L\* Median 6,7, P90 20,3 (K5e 8,2 / 15,7).
- Nicht blass und nicht pastell. Neben Zombie, Gorilla, EM30 und Tyrant liegt K5g2 im selben dunklen Bereich wie der Zombie (gegner_vergleich.png).
- Die G4-Adern sind als hellere Linien zu sehen. Das macht die Haut lebendig statt plastisch.

**Was stört (Einzelheiten in 6):**
- Die weiße rechte Hand.
- Ein fast schwarzer, völlig unbunter Sattel auf Hüfte und Rücken (Teile 0 und 7). Er liest die Zeile haut0 und sticht im Rundum hart von den grauen Beinen ab.
  - Weißes Licht, Rundum (mesh_hell.py): Teil 0 L\* 11,3 / C\* 0,1, Teil 7 L\* 9,9 / C\* 0,1.
  - Teile 1-6, 10-12, 22 (haut1): L\* 24-31 / C\* 3,5.
  - K5e: Teil 0/7 L\* 21,8 / 22,0.
  - 35-41 % der Sattelpixel liegen unter L\* 8.
  - Im Spiel von hinten wirkt das wie Schatten (v_5080_pose1_zoom.png). Im Rundum (rund_ganz.png) ist es ein klar anderer Stoff.
  - Für "je Material EINE Transformation" (V1) muss der Zahlen-Prüfer das ansehen.
- Die Adern an Beinen und Schwanz sind im weißen Licht sehr hell (P90 L\* 51-58 gegen K5e 32-42). Im Spiellicht ist das gemäßigt.

### (2) Sitzen die Flecken wie in den Nutzerbildern? Abweichung, die der Nutzer ablehnen würde (Augen)

**Augen (Zone 29) fehlen.** Der Nutzer sieht zwei helle Punkte: L\* 30 gegen 7 in der Umgebung, Vorlage (217,274) und (280,275).

Bild 2 ohne Videofilter (auge.py, Fenster 9x9):

| Stelle | K5g2 | K5e |
|---|---|---|
| Auge links im Bild, L\* max / Median | 19,7 / 8,9 | 15,0 / 9,4 |
| Auge rechts im Bild, L\* max / Median | 8,9 / 2,9 | 9,4 / 6,6 |

Die Augenflächen 22|0|82, 22|1|112, 22|1|113, 22|1|152 haben in K5g2 einen L\*-Median von 2,3 / 7,5 / 2,9 / 12,9, sind also dunkel.

- In fleck_b2.png ist das Augen-Material grün markiert und sitzt an der richtigen Stelle, ist aber schwarz.
- Im Spiel (v_5080_kopf_zoom.png, v_5090_kopf_zoom.png) gibt es keine Lichtpunkte, nur dunkle Höhlen. K5e hatte wenigstens rote Glanzpunkte.
- Der Nutzer: "Die Augen abzulehnen ist natürlich schon etwas gravierend".
- Vermutlich sind die G4-Augeneinträge selbst dunkel. Für "helle Nutzerpunkte" trägt das nicht.

**Die übrigen Flecken: Abweichung klein.**
- Fleck-Material sitzt in Bild 1 an den Stellen der Übersicht (fleck_b1.png): Musterung Schwanz Mitte/unten (Zonen 9/11), Musterung neben der Wunde (3/4), weiße Stellen links (14/15) und rechts (18/24), braune Punkte (16/19).
- Keine Klötze. Die Ränder folgen den Texeln.
- Rot erscheint nur auf Brust 10 und Hals 21 (Fleck-Zeilen). Kopf 22: 0 rote Pixel außerhalb des Mauls (rotkopf.py).
- **"Musterung" fällt nicht auf**, weil die ganze Haut jetzt G4-Adern trägt. Weiße und braune Musterung sind nur schwach zu sehen. Gegen die Absicht des Nutzers ist das kein Fehler.
- **Buckel (Zone 27):** fleischrot mit hellen, rosa Adern, Grenze links gerade wie die grüne Nutzerlinie. Darin liegen dunkelgraue Inseln (L\* 5,8-9,6, C\* < 5) aus Zone-27-Flächen, deren Texel außerhalb der Maske liegen: 462 px bei (269-316, 193-228), 318 px bei (345-377, 202-224), 147 px bei (301-322, 170-189). Im Spiel als graue Flecken im roten Buckel zu sehen.
- **Wunde am Becken (Zone 2):** rosa-violett statt fleischrot (f1_zoom.png, v_5080_pose1_zoom.png). Wie in K5e.
- **Rotes Fleisch am Zipfel (Zone 12):** nur schwach rosa zu sehen (fleck_b1.png unten links), im Rundum kaum.

### (3) Sind die Streifen weg? Teilweise; der Rest in der Vorderansicht ist eine Abweichung, die der Nutzer ablehnen würde

**Rote Hals-Streifen: weg, wie gewünscht.**
- Spiel 5090, Griff-Pose (v_griff_zoom.png): K5e hat rote Bänder, K5g2 ist dunkelgrau.
- Rundum von unten (rund_kopf.png, az 90 el -60): dasselbe.
- Die 10 Flächen der Zone 45 lesen die Zeile tausch0 (zeilen.py).

**Kopf gegen Buckel in der Vorderansicht: nicht 0.** Eigene Zählung mit Bild-2-Kamera und -Lage. Gezählt wird, wo die Malreihenfolge (mittlere Tiefe) und ein perspektivisch richtiger Tiefenpuffer zwischen {Kopf 22, Unterkiefer 23} und {Brust 10, Hals 21} widersprechen.

| Stand | Bild-2-Pose (FORM Clip 1 Bild 0) | davon Kopf über Buckel | davon Buckel über Kopf |
|---|---|---|---|
| K5e | 1177 px | | |
| K5g2 | 487 px | 420 px | 67 px |

Mit linearer Tiefe: K5g2 512, K5e 1206.

- **Lage** (keil_auf_bild2.png): ein grauer Kopfstreifen am rechten Kopfrand im roten Buckel und einer oben in der Mitte (bbox x 232-348, y 224-278). Das ist genau das, was der Nutzer als "grüne Streifen in den großen Buckel" beanstandet hat.
- **Im Spiel:**
  - 5090 Schnitt 11, Clip 12 Bild 0 = Bild-2-Pose: v_5090_kopf_zoom.png.
  - 5080 von vorn: v_5080_kopf_zoom.png.
  - Beide zeigen die verzahnte Grenze und rote Stücke links am Kopf.
- **Spiel-Clip 12** (BAHNSTEIG_FORM): Bild 0 hat 512 px, Bild 1 694, Bild 2 594, Bild 3 301, über alle 56 Bilder 5397 (K5e 9245).
- **Ausnahme des Bauers:** Er hat Clip 10 und 12 ausdrücklich ausgenommen (k5g2_par.json ausnahme_clips). Clip 12 Bild 0 ist aber die Bild-2-Pose im Spiel. Gerade dort hat der Nutzer die Streifen gesehen.
- Die Halbierung ist ein Fortschritt (-59 %), das Ziel "0 Keil-Pixel" ist in dieser Haltung aber nicht erreicht.

### (4) Lippe, Unterkiefer, Hals-Unterseite: wie gewünscht

- **Weder schwarz noch rot.**
  - Unterkiefer 23 im weißen Licht: L\* Median 22,1, P10 11,2.
  - Hals-Unterseite und Kopf-Unterseite im Spiel (v_griff_zoom.png) dunkelgrau.
- **Kein Fremdzugriff auf Mundfleisch.**
  - Die Zeilen maul0/maul1 tragen genau 23 + 4 = 27 Flächen, alle aus Zone 30, keine fremde.
  - In Bild 2 zeigt nur Teil 23 Rot, 551 px, alle in Zone-30-Flächen.
  - Pixel, die einen schwarzen deckenden Eintrag lesen: Bild 2 15 px, Bild 1 72 px (bild.py). Sichtbar ist das nicht.
- **Klein am Rand:** Der vordere Mundboden (23|1|53-55) erscheint als bräunlich-oranger Block unter den Zähnen (z_b2.png, v_5080_vorn_zoom.png). Im Video ist das offene Maul hell rot-weiß.

### (5) Rechte Hand 4 Krallen wie links, Farbe wie die Hand? Abweichung, die der Nutzer ablehnen würde

**Anzahl und Spiegelung: richtig.**
- In Bild 2 sind alle vier rechten Krallen 19, 20, 29, 31 zu sehen (hand_meshkarte_b2.png).
- In der Ruhepose sind links (14, 15, 28, 30) und rechts (19, 20, 29, 31) Spiegelbilder (krallen_rund.png).
- Die Krallenfarbe ist grau, ohne Rot, und liest keinen reinweißen Eintrag.

**Aber:**
- **Kralle 29 und 20 liegen ganz außerhalb der Figur des Videos.** Gemessen mit der Hintergrundmaske aus zonen_nutzer/v2/krallen_kandidat/krallen_masken_bild2.npz:

  | Kralle | Pixel | davon im Hintergrund | Spitze bei |
  |---|---|---|---|
  | 29 | 562 | 562 (100 %) | (168,363) |
  | 20 | 238 | 238 (100 %) | (180,425) |

  - 29 steht als Stachel nach links oben aus der Hand heraus, wo der Nutzer nichts sieht.
  - Das ganze Modell ragt dadurch mit 5204 statt 4653 px über die Video-Figur hinaus.
  - 19 liegt in der Figur (89 px unter dem Schriftzug). 31 sitzt in Form 1 (152 von 164 px in der Maske asset_31).
  - Die linke Nutzerform (Form 3, Vorlage ~(207,440-451)) deckt Kralle 19 nicht. Spitze 19 liegt bei (218,458).
- **"Farbe wie die Hand" läuft ins Leere, weil die Hand fast weiß ist.** Siehe (6).
  - Krallen in Bild 2: L\* Median 11-19.
  - Hand 18: L\* Median 31,4, P90 36,1.
  - Die Krallen sind also dunkler als die Hand. Beide sind viel heller als im Video, wo die Hand dunkelrot/schwarz ist.

### (6) Neue Fehler? Abweichung, die der Nutzer ablehnen würde (weiße Hand)

**Rechte Hand 18 und linke Hand 13 fast weiß.** Das größte Ding in Bild 2 ist ein hellgraues bis weißes Blatt im Vordergrund mit Fischgrät-Streifen (z_b2.png, v_5080_vorn_zoom.png, v_5080_vorn_c0c1.png). Die Zahlen:

| Messung | K5g2 | K5e |
|---|---|---|
| Hand 18, Bild 2 mit Licht, L\* Median | 31,4 | 6,1 |
| Hand 13, Bild 2 mit Licht, L\* Median | 25,1 | 6,1 |
| Hand 18, weißes Licht, P90 L\* | 87,5 | 33,6 |
| Hand 13, weißes Licht, P90 L\* | 81,2 | 31,2 |
| Bild-2-Pixel aus reinweißen Einträgen (alle Kanäle >= 248) | 1132 | 0 |

- Von den 1132 Pixeln entfallen 850 auf Hand 18 und 247 auf Hand 13, alle aus Zeile haut1.
- Die TIM enthält 847 reinweiße deckende Einträge, davon 122 in haut1 und 139 in haut0.
- Nach "wo kommt denn da jetzt plötzlich dieses ... her" (Nutzer zu K5c) und "plastisch blass" (zu K5e) würde der Nutzer das sofort beanstanden.
- Mit kleinerem f wird die Hand dunkler, bleibt aber im selben Verhältnis (rund 5-fach) heller als die Haut daneben.

**Klein:**
- Schwarzer Sattel Hüfte/Rücken, siehe (1).
- Graue Inseln im Buckel, siehe (2).
- Am Hinterkopf links oben ist ein Band paralleler grauer Streifen jetzt deutlich sichtbar (v_5080_kopf_zoom.png, v_5090_kopf_zoom.png). In K5e war es durch die dunkle Haut fast unsichtbar. Es ist Texturstruktur, kein Loch.

**Ohne Befund:**
- **F1:** kein schwarzer Ring. Der Rand (6 px um das Rosa) liegt bei L\* Median 14,9, die Haut daneben bei 11,7. Rosa ist erhalten (2826 Pixel mit C\* > 15, K5e 2787).
- **Löcher:** keine Löcher ins Freie gesehen.
- **Kopf:** Umriss von vorn und von der Seite wie K5e (rund_kopf.png). Kein verbeulter Kopf sichtbar, die zurückgenommenen Ecken liegen im Inneren.
- **Mosaik:** keins außer der Fischgrät-Textur der Hand.

### (7) Klarer Schritt nach vorn gegenüber K5e und K5c? Nein, noch nicht

**Gegenüber K5e ("komplett ausgeblasst"):**
- Die Haut ist nicht mehr violett-blass, sondern warm-dunkelgrau mit Adern.
- Der Buckel ist fleischrot.
- Die roten Hals-Streifen sind weg.
- Die Kopf/Buckel-Streifen sind halbiert.

**Gegenüber K5c ("bunt"):**
- Die Haut ist fast unbunt.
- Die Krallen haben kein Rot.

**Was der Nutzer trotzdem zuerst sehen würde:**
- die weiße Hand im Vordergrund,
- die fehlenden Augen,
- den Stachel links oben an der Hand,
- die verbliebenen Kopfstreifen im Buckel.

Drei davon hat er schon ausdrücklich beanstandet: Augen, Streifen, "wo kommt das ... her". So ist es für ihn kein klarer Schritt.

## Befunde für den Bauer (Runde 2)

1. **Augen:** Zone 29 hell machen.
   - Ziel: lokaler Hellpunkt wie im Video, L\* deutlich über der Umgebung.
   - Die G4-Augeneinträge sind dunkel. Material und Quelle prüfen.
2. **Hände 13/18:**
   - Prüfen, warum haut1-Einträge unter den Hand-Texeln reinweiß werden (Knie 75 bei f 2,5).
   - Die Hand muss im Band der übrigen Haut liegen, ohne reinweiße Texel. Erst dann ist "Krallen wie die Hand" sinnvoll.
3. **Krallen 29/20:**
   - liegen in Bild 2 zu 100 % außerhalb der Video-Figur (800 px),
   - 29 erscheint als Stachel.
   - Lage nach der Spiegelung gegen die Figur messen und zeigen. Gegebenenfalls ist die Spiegelebene im Handraum nicht die Körpermitte.
4. **Keile in der Bild-2-Pose:**
   - Clip 12 Bild 0 (= FORM Clip 1 Bild 0) hat noch 487 px.
   - Die Ausnahme für Clip 10/12 trifft genau die Haltung, die der Nutzer beanstandet hat.
   - Mindestens Bild 0-3 von Clip 12 brauchen eine Lösung.
5. **Schwarzer Sattel (Teile 0/7, Zeile haut0):** C\* 0,1 und L\* 10 gegen haut1 L\* 29. Eine Transformation je Material prüfen.
6. **Klein:** graue Inseln im Buckel, bräunlicher Mundboden-Block, Wunde rosa-violett.
