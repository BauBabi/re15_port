# Messblatt Zonen: Nutzer-Zerlegung auf das Modell übertragen (Runde 41, Riesenform)

Stand 2026-11-09. Nur Messung. Kein Asset, keine Variante und kein Code wurde geändert.
Modell: Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` (K5e_f1, md5 `6e00f105…`, 32 Teile, 28 Clips).
Posen: Bild 1 = Clip 0 Bild 11 (5080 Schnitt 1), Bild 2 = Clip 12 Bild 0 (5090 Schnitt 11).
Kennungs-Render: `v4f.szene_id`. Die Gegenprobe gegen `Kandidat.rendern` ergibt in beiden Bildern 0 Pixel Unterschied.
Seitenangaben gelten aus Sicht des Wesens.

## Ergebnis

1. **Ausrichtung:** Beide Grundlagen sind die Vorlage, um den Faktor 2 vergrößert. Sie sind weder gespiegelt noch gedreht.
   - Bild 1: Vorlage = 0,50095 · Grundlage + (172,15 / 181,21).
   - Bild 2: Vorlage = 0,50049 · Grundlage + (193,45 / 150,53).
   - Rest-Abweichung in Vorlagen-Pixeln (Median / 90 %): Bild 1 1,6 / 2,7, Bild 2 2,6 / 7,9.
   - Alle 11 beschrifteten Bilder enthalten ihre Grundlage pixelgleich. Gemessen wurde nach Alpha-Verrechnung, Annotation ausgenommen. Einzige Ausnahme sind 627 verstreute Pixel in `linkes_bein_hinten.png`.
2. **48 Zonen** (37 Nutzer-Zonen und 11 Abschnitte) mit ihren Flächenlisten stehen in `zonen.json`. Die großen Zonen decken sich gut mit dem Modell (Anteil der Zone, den das Modell bedeckt):

   | Zone | Deckung | Zugehörige Modell-Teile |
   |---|---|---|
   | Schwanz | 0,98 | Meshes 1–5 |
   | Zipfel | 0,99 | Mesh 6 |
   | Oberkörper hinten | 0,99 | Meshes 0, 7, 10 |
   | Bein hinten l | 0,92 | Meshes 8, 24 |
   | Bein hinten r | 0,95 | Meshes 9, 26 |
   | Vorderbein r (Bild 1) | 0,97 | Meshes 16–18 |
   | Oberkörper vorn | 0,96 | Meshes 10, 21 |
   | Gesicht | 0,97 | Meshes 22, 23 |
   | Vorderbein l | 0,96 | Meshes 11–15 |

3. **Abweichungen zwischen Nutzer-Zerlegung und Modell:**
   - **Kopf von hinten (Bild 1):** An der Stelle des Nutzer-Umrisses liegt im Modell ausschließlich Mesh 16 (Oberarm r, 667 px, Deckung 0,90).
     - Der Modellkopf 22 ist in Bild 1 nicht sichtbar (0 px).
     - Vom Kopf ist nur der Unterkiefer 23 zu sehen, mit 125 px bei x 365–377, y 294–314. Der Nutzer-Kopf liegt dagegen bei x 401–432, y 257–295.
     - Auge und Mund des Nutzers treffen im Modell nichts (0 px bzw. 5 px).
   - **Gesicht vorn (Bild 2):** Der Modellkopf 22 reicht bis y 196, das Nutzer-Gesicht beginnt erst bei y 265.
     - 4951 px von Kopf 22 liegen deshalb im Nutzer-Umriss „Oberkörper“, 4799 px davon im „großen fleischfarbenen Teil“.
     - Nach unten reicht das Nutzer-Maul bis y 407, der Modellkopf nur bis y 374.
   - **Fuß hinten l (Bild 1):** Das Modell bedeckt nur 0,26 der Nutzer-Zone.
     - Die beiden Krallenmarken des Nutzers liegen auf Schwanz-Mesh 5, nicht auf Fuß 25.
     - Die drei Zehenspitzen von Fuß 25 zeigen nach vorn und sind in Bild 1 verdeckt.
   - **Bein hinten l, Abschnitt 1 (oberster):** Hier liegen im Modell Hüfte 0 (185 px), Schwanz 3 (126 px) und Oberschenkel 8 (177 px). Das Bein des Modells beginnt also tiefer als im Nutzerbild.
4. **Krallen (Abschnitt 4):**
   - **Vorne links „exakt 4“:** Das Asset zeigt dort nur 2 Krallen (15 und, kaum, 14) und Kralle 30 mit 2 px.
     - Die vierte Nutzer-Kralle (Spitze bei 473/396) trifft in K5c die eingezogene Kralle **28**.
     - **28 müsste also zurück.**
   - **Vorne rechts:** Im Nutzerbild sind 3 Spitzen markiert.
     - Spitze 1 (203/457) trifft in K5c die eingezogene Kralle **19** (417 px in der Nutzer-Krallenzone).
     - Spitze 2 (273/406) trifft Kralle 31.
     - Spitze 3 (276/442) liegt außerhalb der Modellsilhouette.
     - **19 müsste zurück.** Die Krallen 20 und 29 liegen in K5c außerhalb des Nutzer-Umrisses (20: 11 px, 29: 0 px). Für sie zeigen die Bilder keine Rückkehr.
   - **Hinten „wahrscheinlich 3“:** Beide Modellfüße (25 und 27) haben je **3 Zehenspitzen**. Das ergibt die Silhouettenspitzen unter 70° von oben und von unten.

## 1 Ausrichtung

Verfahren:
- Grobsuche über den Maßstab 1,70–2,60 in Schritten von 0,01. Verglichen wird die normierte Kreuzkorrelation lokal normierter Grauwerte, mit der Alpha-Maske der Grundlage.
- Feinanpassung von Ähnlichkeit und freier Affinität mit Powell.
- Kontrolle über das Wasserzeichen. Die Schrift „D 1.5 / ES.COM“ ist scharf und deshalb ein unabhängiger Anker.

| | Bild 1 (`Rückseite.png`) | Bild 2 (`birkin_form3_02.png`) |
|---|---|---|
| Faktor Grundlage je Vorlagenpixel | 1,9962 | 1,9981 |
| Versatz t (Vorlage-px) | 172,15 / 181,21 | 193,45 / 150,53 |
| Freie Affinität: Faktor x / y, Drehung | 1,9945 / 1,9967, 0,067° | 1,9967 / 1,9991, 0,040° |
| Affin minus Ähnlichkeit an den Ecken | ≤ 0,38 px | ≤ 0,28 px |
| NCC normal / gespiegelt | 0,486 / 0,058 | 0,360 / 0,059 |
| Wasserzeichen: Faktor, t | 2,000, 172,40 / 181,46 (NCC 0,72) | 1,985, 193,20 / 148,28 (NCC 0,53) |
| Restversatz in Kacheln (64 px, Phasenkorrelation), Median / p90 / max | 1,58 / 2,71 / 5,95 (55 Kacheln) | 2,58 / 7,88 / 10,61 (66 Kacheln) |

Die Kacheln in Bild 2 sind kontrastarm (dunkle Figur auf rotem Grund). Der Nutzer hat die Bilder nachbearbeitet, deshalb ist die Phasenkorrelation dort unruhig.
Das Wasserzeichen liegt bei y ≈ 440–460. Dort weicht die Gesamtabbildung um weniger als 0,6 px von der Wasserzeichen-Anpassung ab.

**Beschriftete Bilder:** Die Grundlage sitzt in jedem beschrifteten Bild an einem waagerechten Versatz (dy = 0):

| Bild | Grundlage | Versatz dx |
|---|---|---|
| Rücken, Kopf_von_hinten, rechtes_bein_hinten, Bein_Vorne_rechts | Rückseite | 0 |
| linkes_bein_hinten | Rückseite | 223 |
| Schwanz_hinten | Rückseite | 425 |
| linker_rechter_Fuß_hinten | Rückseite | 375 |
| Vorne_Oberkörper, Vorderes_Linkes_Bein | birkin_form3_02 | 0 |
| vorne_rechte_Hand_und_Kralle | birkin_form3_02 | 282 |
| Gesicht vorne | birkin_form3_02 | 401 |

So wurde die Pixelgleichheit des unbeschrifteten Teils geprüft:
- Roh weichen außerhalb der Annotation (dilatiert um 2 px) 1775–3495 px ab. Das sind ausschließlich halbtransparente Randpixel: Alpha zwischen 1 und 254, die RGB-Werte vorfaktoriert gleich.
- Nach der Alpha-Verrechnung bleiben 0 px abweichend, außer in `linkes_bein_hinten.png` mit 627 verstreuten Einzelpixeln (keine Gruppe größer als 20 px).
- Damit gilt für jedes beschriftete Bild dieselbe Abbildung wie für seine Grundlage (`zonen.json` → `meta.beschriftete_bilder`).

## 2 Umrisse

- **Annotationspixel:**
  - Ein Pixel zählt als Annotation, wenn er von der Grundlage abweicht und sich als Mischung aus Grundlage und Paint-Palettenfarbe ergibt (Anteil ≥ 0,45).
  - Palette: rot 237/28/36, grün 34/177/76, grau 127, blau 63/72/204, lila 163/73/164, orange 255/127/39, dunkelrot 136/0/21, hellblau 0/162/232, braun 185/122/87.
- **Zuordnung von Text zu Linie:** über die Farbe und die Linienenden. Gemessen werden die Endpunkte des Skeletts, also die Pixelbelegung (`skripte/zonen_bau.endpunkt`).
- **Bau der Zonen** (`skripte/zonen_spec.py` gibt je Zone Saaten, Wandfarben und Box an):

  | Bauart | Vorgehen |
  |---|---|
  | `flut` | Flutfüllung innerhalb der Umrisslinien. Die Linien sind um 5 px verdickt; Kopf und Oberkörper hinten brauchen 7 px, weil Fremdfarben die roten Linien überdecken. Danach wird die halbe Linienbreite zurückgegeben und auf die Figur zugeschnitten. |
  | `huelle` / `huelle_n` | Konvexe Hülle kleiner geschlossener Umrisse. |
  | `loecher` | Geschlossene Umrisse, gefüllt. |
  | `punkte` | Kreis mit r = 5–8 px (beschriftet) um das gemessene Ende der Hinweislinie. |

- **Prüfungen:**
  - Keine Flut berührt ihre Box. Ausnahme ist der Zipfel: Er ist am unteren Bildrand offen, weil der Nutzer-Ausschnitt dort endet.
  - Sichtprüfung: `tafeln/01_zonen_auf_nutzerbildern.png`.
- **Gemessene Linienenden (beschriftete Koordinaten):**

  | Bild | Hinweislinie | Ende(n) |
  |---|---|---|
  | Rücken | fleischige Wunde | 297/125, links der grauen Trennlinie |
  | Kopf_von_hinten | Auge | 509/200 |
  | Kopf_von_hinten | Mund? | 496/207 |
  | Schwanz_hinten | rotes Fleisch (4 Enden) | 642/458 (im grünen Dreieck), 632/496, 624/530, 618/575 |
  | Schwanz_hinten | Zipfel | trifft die linke blaue Zipfelkante |
  | Schwanz_hinten | Schwanz Musterung | 632/512 |
  | linkes_bein_hinten | weiß | 352/209 |
  | linkes_bein_hinten | bräunlich | 340/244 |
  | rechtes_bein_hinten | weiß | 315/219 |
  | rechtes_bein_hinten | bräunlich | 303/242, dazu ein eigener Strich 280/238–332/226 |
  | Bein_Vorne_rechts | weiß | 455/204 |
  | Gesicht vorne | Augen | 441/245, 568/248 |
  | Gesicht vorne | Unterkiefer | 652/420 auf dem lila Strich 685/361–611/474 |
  | Hand vorn r | vorderer rechter Arm (braun) | 346/426 |
  | Hand vorn r | Hand (lila) | 329/480 |
  | Hand vorn r | Krallen | 412/584 |
  | Vorderes linkes Bein | Beinabschnitt | 424/284 |
  | Vorderes linkes Bein | Fußabschnitt | 439/398 |
  | Fuß hinten | Hinweislinie | 452/467 |

- **Seiten:**
  - In Bild 1 liegen „links“ beim Nutzer und die Modellteile „l“ (8/24/25) auf der linken Bildseite, „rechts“ (9/26/27, 16–18) auf der rechten.
  - In Bild 2 liegt das „vordere rechte“ Bein des Nutzers links im Bild, beim Modellteil 18 (Hand r). Das „vordere linke“ liegt rechts im Bild, bei 11–13.
  - Nutzer und Modell stimmen bei allen Seiten überein.

## 3 Zonen → Flächen

**Zählweise:**
- Je Zone werden die Modellpixel der Pose in der Zone gezählt, aufgeteilt nach Fläche (Mesh, Dreieck 0 / Viereck 1, Index).
- **Anteil** = Pixel der Fläche in der Zone ÷ sichtbare Pixel der Fläche im ganzen Bild.
- `flaechen_kern` = Flächen mit Anteil ≥ 0,5.

**Ganzteil:** Diese Ergänzung gilt nur für Zonen, die ein ganzes Teil benennen. Sie bezieht ein Mesh vollständig ein, also auch seine Rückseite und alle unsichtbaren Flächen, wenn beides zutrifft:
- ≥ 50 % seiner sichtbaren Pixel liegen in der Zone,
- und es stellt ≥ 3 % der Modellpixel der Zone.

`namensteile` ordnet zusätzlich den Teilnamen des Nutzers dem Teile-Bogen zu, etwa „rechtes Bein und Fuß“ → 16–20, 29, 31. Diese Zuordnung ist nicht gemessen. Sie ist für die Teile gedacht, die das Bild nicht zeigt.

**Spalten der Tabelle:**
- Lab = Median im rohen Video, nicht geeicht, nur zur Orientierung.
- Deckung = Anteil der Zone, den das Modell bedeckt.
- Fl. = Zahl der sichtbaren Flächen in der Zone.
- Rücks. = zusätzliche Flächen aus der Ganzteil-Ergänzung.

| Zone | B | Material (Nutzerwort) | Deckung | Haupt-Meshes in der Zone (px, Anteil des Meshes) | Fl. | Ganzteil | Rücks. | Lab Video |
|---|---|---|---|---|---|---|---|---|
| oberkoerper_hinten | 1 | Haut „Oberkörper“ | 0,99 | 0 Hüfte 3820 (64 %), 7 Rücken 1418 (88 %), 16 Oberarm r 608, 10 Brust 383 | 72 | 0, 7, 10 | 646 | 14/9/-12 |
| wunde_hinten | 1 | Fleisch „fleischige Wunde/Haut“ | 1,00 | 0 Hüfte 602 | 4 | – | – | 45/32/-16 |
| musterung_rechts_hinten | 1 | Musterung | 1,00 | 0 Hüfte 222 | 7 | – | – | 57/17/-28 |
| musterung_links_hinten | 1 | Musterung | 0,38 | 2/0/3 je < 10 px (Silhouettenrand) | 4 | – | – | 42/25/5 |
| kopf_hinten | 1 | Kopf | 0,90 | **16 Oberarm r 667** | 14 | – | – | 12/12/-5 |
| auge_hinten | 1 | Auge (wahrscheinlich) | 0,00 | – | 0 | – | – | 24/12/4 |
| mund_hinten | 1 | Maul („Mund?“) | 0,14 | 16: 5 | 1 | – | – | 19/11/-1 |
| schwanz | 1 | Haut „Schwanz der in den Rücken übergeht“ | 0,98 | 3: 2522, 4: 2316, 2: 2055, 5: 2033, 1: 925, 0: 830 | 173 | 1–5 | 547 | 9/9/-12 |
| schwanz_muster | 1 | Musterung „Muster / Maserung“ | 1,00 | 5: 646, 3: 206, 4: 176 | 27 | – | – | 18/10/-20 |
| zipfel | 1 | eigener Schwanzabschnitt „Zipfel“ | 0,99 | **6 Spitze 2809 (80 %)**, 5: 144 | 24 | 6 | 63 | 48/8/-1 |
| zipfel_musterung | 1 | Musterung (orange) | 1,00 | 6: 1068 | 11 | – | – | 64/7/-1 |
| rotes_fleisch_zipfel | 1 | Fleisch rot „wie oben am Becken“ | 1,00 | 6: 134, 5: 64 | 10 | – | – | 48/23/-12 |
| bein_hinten_links | 1 | Haut, Abschnitte | 0,92 | 8: 1658 (96 %), 24: 1013 (84 %), 0: 185, 3: 132, 25: 52 | 87 | 8, 24 | 178 | 16/10/-16 |
| …_abschnitt1–4 | 1 | Abschnitte (lila Trennlinien) | 0,87–0,96 | A1: 0/8/3; A2: 8; A3: 8 + 24; A4: 24 + 25 | | | | |
| weisse_musterung_bein_hinten_links | 1 | Musterung weiß | 1,00 | 0: 33, 8: 15 | 5 | – | – | 30/8/-15 |
| braeunliche_musterung_bein_hinten_links | 1 | Musterung bräunlich | 1,00 | 8: 29 | 3 | – | – | 38/16/4 |
| bein_hinten_rechts | 1 | Haut, Abschnitte | 0,95 | 9: 2339 (96 %), 26: 1727 (92 %), 0: 958, 27: 55 | 102 | 9, 26 | 173 | 18/10/-21 |
| …_abschnitt1–3 | 1 | Abschnitte | 0,80–0,97 | A1: 9 + 0; A2: 26; A3: 26 + 27 | | | | |
| weisse_musterung_bein_hinten_rechts | 1 | Musterung weiß | 1,00 | 0 Hüfte 49 | 2 | – | – | 84/2/-9 |
| braeunliche_musterung_bein_hinten_rechts | 1 | Musterung bräunlich | 1,00 | 9: 151, 0: 60 | 12 | – | – | 58/6/-20 |
| fuss_hinten_links | 1 | Fuß | **0,26** | 25: 300, 5: 33 | 17 | – | – | 15/8/-17 |
| fusskrallen_hinten_links | 1 | Kralle | **0,22** | 5: 22 | 5 | – | – | 35/2/-22 |
| vorderbein_rechts | 1 | Haut „rechtes Bein und Fuß“ | 0,97 | 17: 1567 (94 %), 18: 945 (78 %), 16: 769 | 80 | 17, 18 | 187 | 15/6/-14 |
| …_abschnitt1–4 | 1 | Abschnitte („wahrscheinliche“) | 0,93–0,99 | A1: 16 + 17; A2: 17; A3: 18 + 17; A4: 18 | | | | |
| weisse_musterung_vorderbein_rechts | 1 | Musterung weiß | 1,00 | 16 Oberarm r 242 | 7 | – | – | 43/4/-18 |
| fuss_kralle_vorderbein_rechts | 1 | Kralle „Fuß und Kralle“ | 0,66 | 18 Hand r 97 (in Bild 1 keine Kralle sichtbar) | 4 | – | – | 46/-1/-5 |
| oberkoerper_vorn | 2 | Haut | 0,96 | 10: 7697 (89 %), 22 Kopf 4951, 21: 3650 (95 %), 16: 610 | 123 | 10, 21 | 292 | 7/20/1 |
| fleisch_vorn | 2 | Fleisch „großer fleischfarbener Teil“ | 0,99 | 10: 5791, **22 Kopf 4799**, 21: 3622 | 102 | – | – | 8/20/1 |
| gesicht_vorn | 2 | Kopf | 0,97 | 22: 10220 (70 %), 23: 1812 (93 %) | 197 | 22, 23 | 375 | 9/24/5 |
| maul_vorn | 2 | Maul „geöffnet“ | 0,93 | 23: 1450 (74 %), 22: 527, 18: 125 | 73 | – | – | 47/46/22 |
| unterkiefer_vorn | 2 | Unterkiefer (Teil) | 0,44 | 23: 87, 24: 46 | 6 | – | – | 27/43/24 |
| augen_vorn | 2 | Auge | 1,00 | 22: 37, 0: 7 | 9 | – | – | 10/21/0 |
| arm_hand_krallen_vorn_rechts | 2 | Haut „Arm mit Krallen“ | 0,87 | 18: 3436, 31: 239 | 31 | 18, 31 | 134 | 18/29/13 |
| arm_vorn_rechts | 2 | Haut | 0,80 | 18: 252, 22: 48 | 9 | – | – | 33/44/21 |
| hand_vorn_rechts | 2 | Haut „Hand mit Krallen“ | 1,00 | 18: 2707, 31: 42 | 17 | – | – | 16/30/14 |
| krallen_vorn_rechts | 2 | Kralle | **0,59** | 18: 478, 31: 197 | 15 | – | – | 31/20/5 |
| vorderbein_links | 2 | Haut „mit Krallen“ | 0,96 | 11: 2417, 13: 1461, 12: 903, 10: 626, 15: 433, 14: 333 | 99 | 11–15 | 364 | 23/29/14 |
| beinabschnitt_vorn_links | 2 | Haut | 0,97 | 11: 2417, 12: 876, 10: 626, 21: 288 | 57 | – | – | 18/27/12 |
| fussabschnitt_vorn_links | 2 | Haut | 0,95 | 13: 1442, 15: 433, 14: 333 | 48 | – | – | 35/38/18 |
| krallen_vorn_links | 2 | Kralle „Exakt 4“ | **0,64** | 15: 209, 13: 77, 14: 5, 30: 1 | 19 | – | – | 44/28/9 |

Die vollständigen Listen (Fläche, px, Anteil), die Polygone in Vorlagenkoordinaten und die Rückseiten-Flächen stehen in `zonen.json`.

## 4 Krallen

Grundlage: `krallen.json`, `tafeln/krallen_*.png`. K5c ist `stufe3/basis_k5c`, also die Geometrie vor dem Einziehen, in derselben Lage und Pose.

| Fuß | Nutzer | Asset K5e_f1 sichtbar (px) | Zuordnung der Nutzer-Krallen | Folgerung |
|---|---|---|---|---|
| vorne links (Bild 2) | „Exakt 4 Krallen“, 4 geschlossene Dreiecke; Spitzen in der Vorlage 402/390, 440/419, 470/416, 473/396 | 14: 357, 15: 699, 30: 2, 28: 0 | 1: Kralle 30 (2 px, fast verdeckt). 2: Hand 13 (79 px) + 14 (5 px). 3: Kralle 15 (221 px). 4: Asset nichts, **K5c Kralle 28** (8 px, Dreieck 65 px). | 28 zurück. Die Spitzen von 30 und 14 treffen die Nutzer-Dreiecke 1 und 2 kaum (Lage, nicht Zahl). |
| vorne rechts (Bild 2) | 3 freie Linienenden = Spitzen: 203/457, 273,5/406, 276,5/442; dazu 2 geschlossene Dreiecke | 31: 419, 19/20/29: 0 | Spitze 1: **K5c Kralle 19** (9 px im 7×7-Fenster; linkes Dreieck 62 px; Krallenzone 417 px). Spitze 2: Kralle 31 (39 px). Spitze 3: kein Modell. | 19 zurück. 20 und 29 liegen in K5c außerhalb des Nutzer-Arms (11 bzw. 0 px). |
| hinten links (Bild 1) | „Kaum sichtbar, aber wahrscheinlich 3“, 2 rote Krallenmarken (55 und 96 Vorlage-px) | – | Beide Marken auf Schwanz 5 (4 bzw. 23 px), nicht auf Fuß 25 | Fuß 25 hat 3 Zehenspitzen, Fuß 27 ebenfalls (Rundum-Silhouette von oben und unten, `tafeln/krallen_fuesse_rundum.png`). Die Zahl stimmt. In der Pose von Bild 1 zeigen sie nach vorn und sind verdeckt. |

## 5 Unsicher

**Vom Nutzer selbst als unsicher markiert:**
- „wahrscheinlich etwas Auge“
- „Mund?“
- „wahrscheinliche Bein Abschnitte“ (Vorderbein r)
- „Kaum sichtbar, aber wahrscheinlich 3 Krallen. Rechts wäre es sicher genauso“

**Von den Bildern nicht gezeigt:**
- Bild 1: das linke Vorderbein (11–15).
- Bild 2: die Hinterbeine fast ganz (8/24/25 nur Ränder), der Schwanz (1–6: 0 px) und der Rücken. Die Rückseite von Kopf und Brust fehlt ebenfalls.
- Der rechte Hinterfuß 27 ist in keinem Nutzerbild beschriftet.
- Für alle diese Teile ist nur die `namensteile`-Zuordnung angegeben, keine gemessene Fläche.

**Zonen ohne geschlossenen Umriss (Bauart steht in `zonen.json`):**
- Weiße und bräunliche Musterung der Beine: Kreis r = 8 px (beschriftet) bzw. 4 Vorlage-px um das Linienende.
- Rotes Fleisch im Zipfel: 4 Punkte.
- Augen vorn: 2 Punkte.
- Unterkiefer: das Band ±9 px um den lila Strich, innerhalb von Rot und Blau.

Die Fläche dieser Zonen ist eine Mess-Setzung, keine Nutzerlinie.

**Offene oder geschlossene Umrisse:**
- „linkes Bein“ hinten: Der Nutzer hat nur die Außenkante gezogen. Die Innenseite wurde mit einer Sehne geschlossen, und zwar entlang des dunklen senkrechten Balkens bei x ≈ 370 (beschriftet): 364/134 → 370/200 → 372/330 → 379/358. Gesetzt, nicht vom Nutzer.
- „linker Fuß“ hinten: konvexe Hülle der drei roten Marken (Kante + 2 Krallenmarken). Gesetzt.
- Zipfel: Er endet am unteren Rand des Nutzer-Ausschnitts (Wasserzeichen). Was darunter liegt, zeigt das Bild nicht.

**Ausrichtung:** Bild 2 hat den größeren Restversatz (p90 7,9 px), weil die Figur kontrastarm ist und der Nutzer das Bild nachbearbeitet hat. Kleine Zonen in Bild 2 (Augen 44 px, Unterkiefer 303 px) können deshalb um einige Pixel versetzt sein.

**Pose:**
- Die Krallenspitzen in K5c sind in derselben Pose gemessen. Ob 28 und 19 nach dem Zurückholen an derselben Stelle stehen, hängt davon ab, dass ihre Geometrie wie in K5c ist.
- Die Abweichungen am Kopf (Bild 1: kein Modellkopf sichtbar; Bild 2: Modellkopf 69 px höher) sind Abweichungen in Form oder Pose, keine Abweichungen in der Zerlegung. Hier wurde nur gemessen, nichts verändert.

## Dateien

| Datei | Inhalt |
|---|---|
| `zonen.json` | Meta (Asset-md5, Posen, Abbildung, Pixelgleichheit) und 48 Zonen. Je Zone: Nutzertext, Material, Polygone, Flächen mit px und Anteil, Kern, Ganzteil mit Rückseiten-Flächen, Namensteile, Lab im Video. Dazu die Krallen. |
| `tafeln/<zone>.png` | Je Zone: Nutzer-Umriss über der Vorlage, Modellflächen der Zone eingefärbt (gelb ≥ 50 %, orange < 50 %) in derselben Pose, beides übereinander. |
| `tafeln/00_uebersicht_bild{1,2}.png` | Alle Umrisse über Vorlage und Mesh-Kennung. |
| `tafeln/01_zonen_auf_nutzerbildern.png` | Die gebauten Zonen auf den Nutzerbildern (Sichtprüfung). |
| `tafeln/krallen_*.png` | Vorlage / Asset / K5c mit den Krallen-Meshes; Rundum der Füße 25 und 27. |
| `skripte/` | Alle Messskripte und Zwischenstände (`abbildung.json`, `versatz.json`, `gleichheit.json`, `krallen.json`, `asset_form.json`). |

Ablauf der Skripte (sie liefen im Scratch-Verzeichnis, `HIER`):
1. `versatz.py`
2. `ausrichten.py`, `ecc.py`, `wz.py`
3. `anno.py`, `komp2.py`
4. `zonen_lauf.py`
5. `render_id.py`
6. `zonen_flaechen.py`
7. `krallen.py`
8. `zonen_ausgabe.py <ziel>`, `krallen_tafel.py <ziel>`

Python lief ohne `-I`.
