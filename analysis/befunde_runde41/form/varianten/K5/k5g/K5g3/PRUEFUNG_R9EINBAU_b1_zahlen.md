# Prüfung R9-Einbau „Variante 1“ in K5g3, Runde 1 – Zahlen-Prüfer (b1)

Unabhängiger Zahlen-Prüfer, 2026-11-15. Nicht committet.
Geprüft: r41/form, Commits 184fa6393, 1ee2cdca4, 7e0a47430, 9b143bf41. Spiel-Asset EMD 88c49ce1…, TIM c00823b3…
Eigener Code und Ausgaben in
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/kr4/pruef_r9einbau_b1_zahlen/`:
- `elib.py`: eigener Leser für EMD, EDD, EMR und MD1, dazu eine eigene Q12-Pose (Nachbau RotMatrix, Rx·Ry·Rz) und eine Gleitkomma-Pose. Keine Projekt-Bibliothek.
- `struktur.py` → `struktur.out`: Posen, Netze und Textur.
- `scan.py` → `s_*.json` und `bewerte.py` → `bewerte.out`: Boden und N52.
  - N52 nutzt eine eigene Strahl-Parität mit 5 eigenen Richtungen (Mehrheit 3) und eine eigene Lot-Tiefe nach Ericson.
  - Gemessen über alle 771 Keys, in beiden Richtungen, für NEU, K5c, R3 (1981f97e) und R9 (2f807af5).
- `robust.py` → `robust.out`: Nachmessung jedes auffälligen Paares, Median über 11 zufällige Richtungssätze.
- `krallen_links.out`: Krallenregel mit der linken Gegenpaarung.
- `rampe.py` → `rampe.out`: Glätte der Rampen.
- `lauf5090.sh` → `lauf_kampf/`: eigener Kampflauf in 5090.
- `gen_*_pruef.log`, `riegel_geo.log`, `riegel_tex.log`, `build.log`.

Bewertet wird der Einbau. Dass die Pfote schräg unter dem Kiefer sitzt, die Armzone oben leer bleibt und rechts etwas übersteht, hat der Nutzer so gewählt. Das ist kein Fehler.

## Ergebnis: BESTANDEN – kein roter Punkt

Die Hand ist nicht im Boden, N52 ist in beiden Richtungen grün, und Posen, Netz 18, Textur und Asset stimmen. Die Bild-2-Schlüsselbilder 542 und 411/412 sowie 398–425 sind bitgleich Variante 1. Einzige echte Abweichung von der gewählten Variante 1 ist die Ausblendung in Clip 12 (Bild 1–13). Sie ist begründet und bei den Bewegungen unten aufgeführt. Rot ist sie nicht.

## 1 Boden: GRÜN

Gemessen ist das größte y der Teile 17, 18, 19, 20, 29 und 31 je Key (Q12-Pose, y > 0 liegt unter dem Boden).
- Kein Key liegt über max(25, K5c im selben Key). Über 25 liegen nur die Keys 591–597. Das ist Clip 12, Bild 49–55, wo die Form selbst einsinkt, und dort ist der Wert bitgleich K5c (bis 2873).
- In den 92 geänderten Keys liegt der tiefste Punkt bei y 12 (Key 408, K5c −140 / Key 328, K5c 7). Alle übrigen geänderten Keys liegen bei y ≤ −7.
- Über alle Keys, in denen der Bezug nicht selbst einsinkt, ist der tiefste Wert y 20 (Key 111, Hand 18; K5c 7).
- Nachgemessen in Clip 9:
  - Key 332: 17/18/Krallen bei −122/−47/≤ −54 (K5c −58)
  - Key 333: ≤ −49
- Gegenprobe R9-Abgabe ist rot, die Werte sind identisch mit dem Bauer: Key 332 y 351, Key 333 y 232, Key 328 y 146, Key 334 y 83, Key 327 y 69.

## 2 N52 in beiden Richtungen, alle 771 Keys: GRÜN

| Richtung | Fälle NEU (K5c / R3) | über K5c+25 | über max(K5c, R3)+25, erste Messung | nach Median-Nachmessung |
|---|---|---|---|---|
| Arm → fremd | 4546 (4306 / 4616) | 1186 | 41 | 39, alle Kralle 20 ↔ Kralle 29 |
| fremd → Arm | 2813 (2660 / 2864) | 1023 | **0** | 0 |

- **„Über K5c+25“:** Alle diese Fälle hängen an einem B4-Netz (10/16/21/22), außer den Krallenpaaren 20/29. Ohne B4-Netz gibt es in der Rückrichtung 0 Fälle. Das ist der abgenommene Spalt, nicht die Hand.
- **Kiefer in Hand, Key 546 (23 → 18):** NEU 70,0 bei K5c 55,3 (Grenze 80,3). Grün. Die Gegenprobe R9 hat 173,4 und ist rot. Die Werte des Bauers (55 / 173) sind bestätigt.
- **Messung instabil an Netz 21 (Hals, B4):** Zwei Fälle der ersten Messung (Key 552/555, 17 → 21) hängen vom Richtungssatz ab.
  - R3 Key 555 gibt je nach Satz 135,9 oder 196,0.
  - Der Median über 11 Sätze liegt unter der Grenze (z. B. 552: NEU 167 gegen R3 187).
  - Der Bauer misst für NEU dieselben Werte (197,75 / 167,42), für R3 aber 196,0 / 187,2.
- **Restliche 39 Fälle:** Es sind die Krallen 20 ↔ 29 in den Bild-2-Keys 402–424 und 542–546. Sie sind identisch mit Variante 1, denn die Posen von 18 und der Krallen gegen 18 sind dort unverändert. Die Gegenprobe R9 zeigt dieselben 39 Fälle.
  - Mit der Krallenregel des Auftrags (1,3167 × linke Gegenpaarung 15/28 + 25, selbst gemessen) liegen 0 Fälle darüber.
  - Größtes Paar: Key 412 29 → 20 108,9 bei Grenze 125,1.
- **Restfehler aus R9, jetzt gemessen:**
  - Key 312 19 → 10: kein Fall (K5c 90)
  - Key 704/729 18 → 22: unter der Grenze
  - Key 444 13 → 18: NEU 176 gegen K5c 185

## 3 Posen: GRÜN

- Zwischen NEU und K5c ist nur die Sektion emr1 verschieden. Alle anderen Sektionen 0–6 sind bitgleich K5c und R3. Wurzel, Bindversätze, Eltern und Clips sind gleich.
- Geändert sind nur die Knochen 17 und 18. Knochen 17 ändert sich in 91 Keys: 309–334, 398–425, 440–448, 542–556, 701–707 und 727–732. Knochen 18 ändert sich zusätzlich in Key 726, also in 92 Keys. Die Krallen 19, 20, 29 und 31 ändern sich nur in den 43 Bild-2-Keys.
- **Oberarm 16 == K5c in allen 771 Keys.**
- Posen-Datei 286b2920 (43 Keys „keys“ + 52 Keys „n52“): Jedes „neu“ ist == Asset und jedes „alt“ == K5c (mod 4096). **0 geänderte Tripel stehen außerhalb der Datei.** „bild2_korrektur“ „neu“ ist == Asset, „r9“ ist == R9-EMD.
- **Bild-2-Keys gegen Variante 1 (R9-EMD 2f807af5):** Bitgleich (alle 32 Knochen) sind 398–425, 542 und 556. Das sind auch alle Keys, die das Entscheidungsblatt zeigt: Clip 12 Bild 0 = Key 542 und Clip 10 Bild 44 = Key 411.

Anders sind nur Clip 12 Bild 1–13 (Weltdrehung gegen R9; Krallen drehen mit 18; 16 = 0):

| Key | 543 | 544 | 545 | 546 | 547 | 548 | 549 | 550 | 551 | 552 | 553 | 554 | 555 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 17 Grad | 1,4 | 4,4 | 6,6 | 6,0 | 3,5 | 6,1 | 10,9 | 14,1 | **15,3** | 14,3 | 11,1 | 6,3 | 1,8 |
| 18 Grad | 2,0 | 6,6 | 11,0 | **12,2** | 9,9 | 6,8 | 6,8 | 8,8 | 9,9 | 8,7 | 6,0 | 3,0 | 0,8 |
| Handgelenk-Weg | 26 | 85 | 129 | 116 | 45 | 92 | 201 | 273 | **295** | 269 | 202 | 111 | 31 |

Außerhalb der Bild-2-Keys gilt gegen K5c (Grad 17/18, lokal):
- Clip 9: 309–334
- Clip 10: 440–448
- Clip 27: 701–707 und 726–732

Die Größenordnungen sind dieselben wie im Bericht (Clip 9 bis rund 10,7 Grad, Clip 10 bis 2,9, Clip 27 unter 1).

## 4 Netz 18: GRÜN

- Netz 18 ist bitgleich `k5g3_handnetz18.json` (b4e771b8). Geprüft sind 124 Ecken, 124 Normalen, 44 Dreiecke und 96 Vierecke, jeweils mit Ecken- und Normalen-Indizes.
- Eigene Spiegelhand: rint(1,3167 · diag(1,1,−1) · Netz 13), Normalen S·n, Umlauf 0,2,1 / 0,2,1,3.
  - Sie ist == Netz 18 für Netz 13 aus K5e_f1, aus dem Asset und aus R9.
  - Gemessener Faktor (Median): 1,3166.
- UV, CLUT und Seite von 18 sind == Netz 13 des Assets in der umgedrehten Reihenfolge (140 Flächen).

## 5 Übrige Netze, Textur, Riegel, Generator: GRÜN, R8 rot wie in Runde 3

- NEU gegen R3: alle 31 übrigen Netze sind bitgleich (Ecken, Normalen, Flächen, UV-Bytes). TIM ist == R3 (c00823b3). Die MD1-Länge steigt nur durch Netz 18 (461 996 → 462 572).
- Generator zweimal selbst gelaufen:
  - `kf_k5g3_geo.py pruef`: JA, f833a905. Byte-Probe KAND und UV-Tabelle: JA.
  - `kf_k5g3.py pruef`, mit eigenem K5G3_SCR: JA. In beiden Läufen ist BAHNSTEIG_FORM.EMD 88c49ce1 und TIM c00823b3.
- Riegel selbst gelaufen:
  - Geometrie: **14 von 15 grün, 19 von 19 Gegenproben rot.**
  - Textur: **48 von 48 grün, 42 von 42 Gegenproben rot.**
  - Die Protokolle sind nach dem Lauf bitgleich zur committeten Fassung. `git status` zeigt keine geänderte Datei.
- **R8 ist rot wie in Runde 3.** Es sind dieselben drei H3a-Paare an den K5c-Krallen. 19↔22 steigt von 104 auf 157, 20↔22 von 424 auf 483, 19↔23 sinkt von 535 auf 527. R8b ist grün. H7 Bild 2 war rot und ist jetzt grün (0,61248 → 0,68212).
- Einschränkung: Die Messbank (3995 Ansichten) habe ich nicht neu gerastert. R8, R8b und R9 lesen die committeten Urteile (`geo/messung/urteil_*.json`), und R12/R13 die N52-Caches des Bauers. Ersetzt habe ich diese durch meine eigenen N52-, Boden- und Spielmessungen.

## 6 Spiel-Asset, Sicherung, ctest

- `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` ist == K5g3/BAHNSTEIG_FORM.EMD == Generator-Ausgabe (88c49ce1). TIM ist c00823b3.
- K5e_f1 ist gesichert in `varianten/K5/einbau_k5e_f1/`: EMD f277916d und TIM 31a35e66, == Spiel-Asset in 968001097.
  - Hinweis: EMD und TIM sind nur lokal vorhanden, weil `*.EMD` und `*.TIM` per .gitignore ausgeschlossen sind (wie bei einbau_k5c).
  - In git stehen sie in 62bd0001b und 968001097. `kf_k5g3.py` braucht die Datei.
- **ctest selbst gelaufen:** `RE15_BUILD_DIR=re15_port/build_b1z bash re15_port/tools/local_build.sh` mit CTEST_PARALLEL_LEVEL 4, Dauer 5,4 h. Ergebnis **1408 von 1411 grün**. Es sind dieselben 3 Roten wie beim Bauer, und keiner hängt an der Form:
  - `unit_r39_titelversion_cmake`: „git fehlt“, weil der PATH von local_build kein git enthält.
  - `integration_r41_restpunkte_original` und `_elza`: „Spinnen-Filter laeuft unter dem Schalter“, ROOM2090.
- Alle Formtests sind grün: unit_r41_form_emd, unit_birkin_formen, unit_birkin_bahnsteig (mit den neuen Pins), unit_n2_griff_start, unit_birkin_szenen, integration_r41_form_wagenweg, integration_r41_birkin_pistole und integration_r41_form_dreiecke.
- integration_r41_form_dreiecke im Einzelnen:
  - kampf 7436 / 0 verworfen / 96 Schuss / Form-HP 0
  - s11 7748 / 0
  - v5080 7212 / 0
  - Gegenprobe mit Grenze 2000 ist rot (6 549 303 verworfen)
- Der Log liegt als `local_build_ctest.log` im Prüferordner. Den Bauordner build_b1z (5,9 GB) habe ich danach gelöscht.

## 7 Dreiecke und eigener Lauf 5090: GRÜN

- Obergrenze ohne Rückseiten-Wegfall: NEU hat 15 273 Dreiecke (Viereck = 2), R3 15 249. Netz 18 bringt also nur +24. Gemessen wird gezeichnet, nicht diese Obergrenze.
- Eigener Kampflauf in 5090 mit eigener exe aus build_b1z, eigener PID, Spiel-Asset ohne Umleitung, ohne SOFTWARE_RENDER und ohne AUTOSHOT:
  - „Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips“
  - `[tri-liste] ENDE hoechststand=7436 (F322) verworfen=0 bilder_mit_verlust=0 grenze=16384`, kein UEBERLAUF
  - Magazin 144 → 48 (96 Schuss), Form-HP bis 0, Schnitt 11 in 501 Zeilen
- 7436 liegt um 8948 unter der Grenze 16384. Mit Spielerin, Waffe und Effekten nach der Rechnung des Riegels sind es 9295 ≤ 14336.

## 8 Rampen: GRÜN (keine Einzelbild-Sprünge)

Gemessen sind die lokalen Grad je Bild und die zweite Differenz von 17 und 18 sowie der Weltweg der Hand mit Krallen.
- **Clip 9:**
  - NEU folgt K5c und R9 Bild für Bild. Höchstwerte: 32,2 Grad je Bild, zweite Differenz 35,3, Weg 1142.
  - K5c hat dieselben Werte (32,2 / 35,5 / 1148). Die Spitzen bei Bild 27/28 (Key 331/332) sind Original-Bewegung.
  - Im geänderten Fenster 309–327 liegen die Abweichungen gegen K5c bei höchstens 2,4 Grad je Bild.
- **Clip 12, Ausblendung:** glockenförmig, ohne Einzelbild-Sprung.
  - Höchstwerte: 17 bis 16,7 Grad je Bild (R9 12,4, K5c 5,5), zweite Differenz bis 8,5 (R9 4,9, K5c 2,2), Weg je Bild bis 865 (R9 717, K5c 694), zweite Differenz des Wegs 391 (R9 284).
  - Bei Bild 12/13 (Key 554/555) gibt es einen kleinen zweiten Buckel: 4,7 → 6,9 → 8,1 → 5,3 Grad je Bild.
- **Clip 10:**
  - 398–425 ist bitgleich R9. Höchstwerte: 12,4 Grad je Bild, zweite Differenz 6,0.
  - 440–448 liegt bei höchstens 1,8 Grad je Bild.
- **Clip 27:** NEU ist praktisch == K5c (unter 0,3 Grad je Bild Unterschied).

## Abweichungen, die der Nutzer ablehnen würde (ROT)
Keine.

## Nur im Riegel oder im Vergleich sichtbar
1. Clip 12, Bild 1–13 (Keys 543–555) weicht von Variante 1 ab: Unterarm 17 bis 15,3 Grad (551), Hand 18 bis 12,2 Grad (546), Handgelenk bis 295 Einheiten.
   - Die Ausblendung wird damit schneller als in R9: 16,7 statt 12,4 Grad je Bild, Weg 865 statt 717.
   - Nötig ist das für N52 (Kiefer in Hand 546) und gegen die Maler-Streifen 17/23. Bild 2 selbst ist unverändert.
   - Sichtbar ist es nur in der Bewegung nach Bild 2. Der Bauer meldet es als offen (Punkt 1). Die kleinere Variante b12 liegt bereit.
2. Clip 9, Bild 5–30 ist gegen K5c um bis zu rund 10,7 Grad gedreht. Es folgt der Linie von R9. Nötig ist das für die größere Spiegelhand.
3. R8 bleibt rot wie in Runde 3. Es sind dieselben drei H3a-Paare an den K5c-Krallen: 19↔22 +53 px, 20↔22 +59 px, 19↔23 −8 px.
4. Krallen 20 ↔ 29 stecken in den Bild-2-Keys bis 108,9 ineinander (K5c 34,6). Das ist Variante 1 selbst und liegt unter der Regel „1,3167 × linke Gegenpaarung + 25“.
5. Die N52-Parität ist am offenen B4-Netz 21 vom Richtungssatz abhängig, um bis zu 60 Einheiten. Erst der Median über mehrere Sätze ist eindeutig.
6. Die K5e_f1-Sicherung liegt nur lokal (wegen .gitignore), aus git ist sie wiederherstellbar.
