# EINBAU R9 - rechte Hand "Variante 1" (Krallen-Runde R9) in K5g3 und ins Spiel

Stand 2026-11-15, Zweig r41/form. Nutzerentscheid 15.11.2026 "Variante1" am Blatt
`reAi_v2_entscheidung_2026-11-06/31_riesenform_rechte_hand/rechte_hand_variante1_variante2.png`.
Klarstellung des Orchestrators (bindend): Die Nutzernachrichten "vorher WAR er ja sauber connected" / "Siehe birkin_fehler" betreffen den
Schwanz der zweibeinigen Birkin, nicht diese Hand. Das leere obere Armfeld (Armzone 0,565) hat der Nutzer auf dem Blatt gesehen und trotzdem
Variante 1 gewaehlt. Die Probe "K5c-Arm + V1-Krallen" (fffe6abb) ist NICHT verwendet. -> gebaut, keine Umdeutung.

S = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/kr4/r9_einbau`

Dies ist Port-/Modellinhalt (RE2-Retail-Form im Nachbau, kein Original-Verhalten): es gibt keine `@0x`-Adresse. Alle Grenzen stammen aus den
Regeln der Auftraege (E8/K12 Boden 25, N52 K5c + 25, K4, E6) und sind unten mit dem Messwert belegt.

## 1 Zwei Restfehler von R9 behoben (Schritt 1)

### 1.1 Messung vorher (eigener Scanner `S/scan2.py`, Ganzzahl-Pose wie das Spiel, alle 771 Keys, N52 in BEIDE Richtungen)
- R9-Abgabe (2f807af5): Boden Clip 9 Key 332 y 351, 333 232, 328 146, 334 83, 327 69 (Zahlen-Pruefer bestaetigt, identisch).
  Rueckrichtung (fremd -> Arm, die das offene Kiefernetz braucht): 13 Faelle, groesster Key 546 Kiefer 23 in Hand 18 Tiefe 173 (K5c 55).
- Gemessen auf den K5g3-Netzen (Spalt B4 an 10/16/21/22) statt K5e_f1: zusaetzlich Clip 9 Key 312 Kralle 19 in Brust 10 (+157),
  Clip 27 Keys 704/729 Hand 18 in Kopf 22 (+39/+72, nur durch die groessere Spiegelhand), Clip 10 440-447 linke Hand 13 in Hand 18 (+10..+28),
  Clip 12 551-553 Kopf 22 in Hand 18.
- Bezug: max(K5c, K5g3 ohne R9) + 25 im selben Key. Gegen K5c ALLEIN hat schon K5g3 Runde 3 (abgenommen) 2242 Faelle, alle mit einem
  B4-Netz (10/16/21/22; gemessen: 0 Faelle ohne B4-Netz) - der abgenommene Spalt, nicht die Hand.

### 1.2 Weg
- `korr2.py`: EIN glattes Fenster (Basis sin(pi s) sin(j pi s)) je Clip. Clip 9 blieb rot (Strafe 10036, 312/319/330 bis +26,5).
- `keyweise.py`: Korrektur je Key (Drehvektor 17/18) + Glaette ueber die ZWEITE Differenz (Wert und Steigung 0 an den Fensterenden).
  Klein gestartet: 312 (+5,1) und 320 (+6,2) blieben; gemessen (`beule.py`, `gemeinsam.py`, Zufallsproben): 312 ist nur mit 5-15 Grad loesbar,
  und jede solche Beule drueckt 313/314 in den Rumpf (+33..+61). Loesung: Start mit der R9-Korrektur fuer 309-321 (grosse gemeinsame Drehung wie
  R9 selbst), weich (Kosinus 320 -> 326) uebergeblendet in die kleine Loesung fuer 323-334 -> keyweise. Ergebnis `S/w/f9.json`.
- Clip 12 (Bild-2-Rampe, Basis = R9-Rampe): zwei Loesungen gerechnet; gewaehlt `korr2` (`S/w/c12.json`), weil die kleinere (`b12`, bis 5,8 Grad)
  in der Messbank neue Maler-Streifen Unterarm 17 / Kiefer 23 bringt (H3a 17<->23 1637 > Grenze 1342; Clip-12-Ansichten: K5c 265, R9 V1 allein 606,
  b12 841, c12 223 px). Mit c12 ist H3a 17<->23 wieder unter der Grenze.
- Clip 10 (440-448): `S/w/c10.json`; Clip 27 (700-708, 726-733): `S/w/k27a.json`, `S/w/k27b.json`.

### 1.3 Ergebnis (nur Knochen 17/18; Krallen und Oberarm 16 unveraendert; Key 412 = 542 = Pose h1 unveraendert)

| Fenster | Keys (Clip/Bild) | groesste Drehung 17 / 18 | groesster Weg Hand+Krallen | N52 / Boden danach |
|---|---|---|---|---|
| Clip 9 | 309-334 (Bild 5-30) | 10,7 / 7,0 Grad (Key 319 = Bild 15) | 392 Einheiten (319); ab Key 325 <= 31 | alle <= Grenze - 4,3; Boden tiefst 12 unter y 0 (328; K5c 7) |
| Clip 10 | 440-448 (Bild 80-88) | 2,9 / 2,9 (444) | 79 | <= Grenze - 3,0 |
| Clip 27 | 700-708, 726-733 | 0,5 / 0,8 (729) | 21 | <= Grenze - 7,6 |
| Clip 12 (Bild-2-Keys) | 543-555 (Bild 1-13) | siehe unten | 271 (551) | <= Grenze - 4,3 |

Bild-2-Keys, die sichtbar anders werden (Clip 12 Ausblendung; Grad = Drehung gegen die R9-Rampe):
543 1,4/0,7 · 544 4,4/2,3 · 545 6,6/4,9 · 546 6,0/7,6 · 547 3,5/9,7 · 548 6,1/10,7 · 549 10,9/11,2 · **550 14,2/12,2 · 551 15,3/13,6 ·
552 14,4/13,6** · 553 11,2/10,8 · 554 6,3/6,1 · 555 1,8/1,8 (17/18). Keys 398-425 und 542/556 unveraendert.

Pruefung der neuen Posen ueber ALLE 771 Keys (eingebaut, `kf_k5g3_n52.py` = scan2, Riegel R12/R13): **N52 beidseitig 0 Faelle**, **Boden 0 Keys ueber
max(25, Bezug)**; tiefster Punkt in Keys, in denen der Bezug nicht selbst einsinkt: y 20 (Key 111, Hand 18; K5c dort 7). Clip 12 Bild 49-55
(Keys 591-597) sinkt die Form selbst ins Wasser (K5c bis y 2873) - dort gilt der Bezug.
In Bild 2 (Key 542) liegt die Hand ueber dem Boden auf: tiefster Punkt Kralle 29 39 Einheiten ueber y 0, Hand 18 60 (K5c: Kralle 19 119).

Abgabe Schritt 1 (in S):
- `krallen_posen.json` sha256 **286b2920416a66a6191862a1ec24065323c234c3c0c9651b4f6279287e4023fc** (43 Bild-2-Keys, 52 "n52"-Keys, 13 Keys "bild2_korrektur")
- `handnetz18.json` sha256 **b4e771b87b0642b0f412b15fc8372843e245ce43b9a3fb7435a37c9435dfa714** (unveraendert R9 N33)

## 2 Einbau in K5g3 (Schritt 2)

Generator (`generator/`):
- `kf_k5g3_geo.py`: Stufen posen -> krallen -> **hand18** -> spalt -> uvfein. Posen-Datei mit Abschnitt "n52" (nur Keys ausserhalb der
  Bild-2-Keys, nur 17/18, 'alt' == K5c). Stufe hand18: Netz 18 = `k5g3_handnetz18.json`, vom Generator gegen spiegelhand(Netz 13 K5e_f1)
  nachgerechnet (Abbruch bei Abweichung). Vorgabe-Posen `generator/krallen_posen.json` (= Abgabe Schritt 1).
- `kf_k5g3.py`: Textur weiter auf den K5e_f1-Flaechen; nach dem Spalt Netz 18 = spiegelhand(BEMALTES Netz 13) -> UV/CLUT/Seite von 13 samt
  Kopie-Versatz (R9 N33). Basis-Pruefung per sha256 von ruhe/K5e_f1 (das Spiel-Asset ist jetzt K5g3).
- Neu `kf_k5g3_n52.py` (N52 beidseitig + Boden), `kf_k5g3_tafel_r9.py` (Tafeln 12/13), `kf_k5g3_kampfvideo.sh`; `kf_k5g3_spiel.sh`/`kf_k5g3_zlauf.sh`
  nehmen eine exe ueber `RE15_PC_EXE`.

Ergebnis: geo/BAHNSTEIG_FORM.EMD **f833a905...**, Asset BAHNSTEIG_FORM.EMD **88c49ce1...**, TIM **c00823b3...** (unveraendert gegen Runde 3),
FORM.EMD 290b59e5... Zweiter Generatorlauf byte-gleich (`kf_k5g3_geo.py pruef`: JA; `kf_k5g3.py pruef`: JA; Riegel TB).

Riegel:
- Geometrie (`geo/riegel_k5g3.txt`): **14 von 15 gruen, 19 von 19 Gegenproben rot**. Neu: R11 Netz 18 == handnetz18 == Spiegelhand
  (Gegenproben K5e_f1-Netz 18; Datei mit einer Ecke +1 -> Generator bricht ab), R12 N52 beidseitig alle 771 Keys (Gegenprobe R9-Abgabe: rot),
  R13 Boden alle 771 Keys (Gegenprobe R9-Abgabe: rot, Key 332 y 351). Angepasst, weil die Posen-Datei sie bewusst aendert: R6 (emr1 = K5e_f1 +
  Posen-Datei), R7 (Pool == K5c + keys + n52), R8/R8b (Netz 18 als gewollt fremdes Netz in H6b), R9 (Urteil und Spiellaeufe muessen zur
  Geometrie gehoeren). ROT bleibt R8 wie in Runde 3 (H3a nur an den K5c-Krallen: 19<->23 527, 20<->22 483, 19<->22 157; R8b gruen).
  Messbank voll neu (3995 Ansichten, urteil4): H1a-H2, H3b, H4 7290 (B4 7318), H7 Bild 2 0,68212 (B4 0,65316, Runde 3 0,61248), Bild 1 0,72301.
- Textur (`riegel_k5g3_textur.txt`): **48 von 48 gruen, 42 von 42 Gegenproben rot**. Neu H18 (Netz 18 des Assets = spiegelhand(Netz 13 des
  Assets), 140 Flaechen, UV/CLUT/Seite == 13; Gegenprobe Runde 3); G5 jetzt "Spiel-Asset == diese Variante" (Gegenprobe K5e_f1).
  Z1-Z9 neu im Spiel gemessen (--neu_spiel), A3 Augen gruen.
- Dreiecke (R9): Messbank max 7290 + 773 + 86 + 1000 = 9149 <= 14336; Spiel max 7748, verworfen 0 (geo/messung/spiel_dreiecke.json).

## 3 Ins Spiel (Schritt 3)

- `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM` = K5g3 + R9 (EMD 88c49ce1..., TIM c00823b3...). Wie beim Einbau 62bd0001b nur diese
  zwei Dateien (FORM.EMD/.TIM lagen nie in shared_assets).
- K5e_f1 gesichert: `varianten/K5/einbau_k5e_f1/` (EMD f277916d, TIM 31a35e66, K5E_F1.md); kf_k5g3.py liest die Basis ueber ruhe/K5e_f1 per sha.
- `tests/integration/test_r41_form_dreiecke.cmake`: Vorgabe jetzt das Spiel-Asset (vorher K5g3/geo).
- Spiellaeufe (eigene exe build_r9e, eigene PIDs, kein AUTOSHOT/SOFTWARE_RENDER): 5090 Kampf (gdigrab-Video) hoechststand 7436, verworfen 0,
  Magazin -> 48, Form-HP bis 0, Schnitt 11; 5080 Clip 11 7606; 5090 Schnitt 11 Clip 12 7748; 5080 von vorn 7212; alle "Bahnsteig-Form: 32 Meshes,
  32 Knochen, 28 Clips", verworfen 0. Kein Durchstossen (R12, alle 771 Keys), Hand nicht im Boden (R13).
- Pins: `tests/unit/form_j_posen.h` neu aus `stufe3/gen/s3_testkopf.py` (Referenz K5g3; MD1 462 572 Byte 0x565363c6, TIM 138 784 Byte 0xf428fe01;
  Bild-2-Winkel aus Key 542 des Assets - nur 17/18/19/20/29/31 anders als vorher).
- ctest (local_build.sh test, RE15_BUILD_DIR=re15_port/build_r9e, CTEST_PARALLEL_LEVEL=6): **1408 von 1411 gruen**, 3 rot, alle NICHT vom Einbau:
  `unit_r39_titelversion_cmake` ("git fehlt": local_build setzt einen PATH ohne git; derselbe Test direkt mit git im PATH: 8/8 gruen),
  `integration_r41_restpunkte_original` / `_elza` (ROOM2090 Spinnen-Filter; mit dem alten Asset K5e_f1 gegengeprueft: ebenfalls rot).
  Formtests gruen: unit_birkin_bahnsteig (mit neuen Pins), unit_n2_griff_start, unit_birkin_szenen, integration_r41_form_dreiecke
  (kampf 7436 / s11 7748 / v5080 7212, verworfen 0, 96 Schuesse, Form-HP bis 0, Gegenprobe Grenze 2000 rot), integration_r41_birkin_pistole.

## 4 Tafeln (Schritt 4, roh) - `tafeln_r9/`

01-09 wie K5g3 Runde 3 (04 mit Augen/Kopf von vorn x4; Spalte "K5e_f1 (bis R9 im Spiel)" | K5g3 + R9), dazu
12_rechte_hand.png (Bild 2 Krallen vorn x2 und Spielkamera 5090/5080: Runde 3 | R9), 13_bewegung.png (Filmstreifen Clip 9/10/12/27: Runde 3 | R9),
kampf_5090.mp4 (45 s, 640 x 480, gdigrab des eigenen Fensters, Spiel-Asset).

## 5 Offen / zu entscheiden

1. Clip 12 Bild 6-11 (Keys 548-553): Unterarm/Hand bis 15,3 / 13,6 Grad gegen die gewaehlte R9-Rampe gedreht (Hand bis 271 Einheiten
   verschoben) - noetig fuer N52 (Kiefer in Hand Key 546, Kopf in Hand 551-553) UND gegen die Maler-Streifen Unterarm/Kiefer. Die kleinere
   Variante (bis 5,8 Grad) liegt als `S/w/b12.json` bereit (N52 gruen, aber H3a 17<->23 rot).
2. Clip 9 Bild 7-19 (Keys 311-323): Drehung bis 10,7 Grad (Weg bis 392) - wie R9; kleiner loest die Spiegelhand an 312/320 nicht (gemessen).
3. Armzone Bild 2 oben weiter leer (Entscheid des Nutzers, Variante 1); H3a an den K5c-Krallen wie Runde 3.

## Commits (r41/form)

- 184fa6393 feat(form-k5g3): R9-Einbau - Generator, Posen-Datei, K5g3-Stand, Riegel, Messbank
- 1ee2cdca4 feat(form): Spiel-Asset = K5g3 + Variante 1, K5e_f1 gesichert, Test-Pins
- 7e0a47430 doc(form-k5g3): Tafeln 01-09/12/13, Kampfvideo, Fensterbilder
- (dieser Bericht)
