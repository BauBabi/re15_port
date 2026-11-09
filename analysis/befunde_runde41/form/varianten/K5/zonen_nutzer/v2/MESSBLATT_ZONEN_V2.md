# Messblatt Zonen v2: erkannte Grundlage zur Abnahme (Runde 41, Riesenform)

Stand 2026-11-09, Korrektor Runde 2 von 3. Nur Messung: kein Asset, keine Textur, keine Farbwahl. Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` (md5 6e00f105f6ea854b521e23fa1f4b9d5c) und `.TIM` (md5 4668e504198ad2c24a29a3e1ec590448) unverändert. Grundlage v1: `../MESSBLATT_ZONEN.md` (Commit e53cef212), Prüfung v1 `../PRUEFUNG_ZONEN.md` (Commit d62ae97fa), Prüfung Runde 1 `PRUEFUNG_GRUNDLAGE_V2_r1.md`.

## Ergebnis

- **61 Zonen** aus allen 11 beschrifteten Nutzerbildern (jede Beschriftung, auch der zweite grüne Strich), dazu Abschnitte, je Kralle eine Zone und 4 Kopfteile als Modellgliederung ohne Nutzerwort (Nr. 40, 41, 42, 44). Sicherheit: sicher 27, unsicher 27, verdeckt 7.
- **Flecken texelgenau (T1):** 14 Zonen (Wunde, Musterungen, Schwanz-Muster, rotes Fleisch, weiße/bräunliche Musterung, großer fleischfarbener Teil, Augen) sind jetzt Texel-Masken in der Form Ihrer Stelle statt ganzer Flächen; zusammen 3890 Texel. Rückprobe (Texel-Fleck gegen Ihre Stelle im Bild) IoU 0,58 bis 0,89.
- **Geteilte Texel (T2):** Die heutige G4-Textur benutzt dieselben Texel an vielen Stellen. Bei 13 von 14 Flecken nutzen andere Flächen dieselben Texel; größter Fall "großer fleischfarbener Teil": 556 weitere Flächen, der Fleck erschiene in Bild 2 zusätzlich auf 14011 Video-px. Je Zone steht in `zonen_v2.json` `brauchen_eigene_texel` (nur benannt, nichts umgebaut); Tafel 23.
- **Augen (Z2):** Auge = nur die Texel unter den beiden Nutzerpunkten, eine Liste (`zonen_v2.json` Zone 29 `texel` = `augen.nutzerpunkte[*].auge_texel`). Rechts (Wesen) 9 Texel / 6,5 Video-px, links 12 Texel / 12,3 Video-px. Heutige G4-Augen 68 bzw. 75 Video-px, Mitte 4,6 bzw. 7,7 px neben dem Punkt.
- **Krallen:** vorn links 28 mit K5c-Form zurück; vorn rechts 31 bleibt, 19 mit Faktor x0,4898 (Rest 16,0 px), 20 und 29 bleiben eingezogen (Frage). Hinten unverändert. Kandidat: nur Teile [19, 28] anders, 385 Bytes verschieden.
- **Kopf (Bild 2):** Modell minus Nutzerumriss oben -69,6, unten -6,0, links +4,5, rechts -2,5 Video-px; nur gemessen.

## Verfahren

- Ausrichtung wie v1 (Faktor 2, keine Drehung/Spiegelung; `abbildung.json`). Kennungs-Render in Grundlage-Koordinaten (Kameramatrix verkettet mit Vorlage -> Grundlage). Pixelzahlen im Video-Maßstab (Grundlage-px x s², s = 0,5005 bzw. 0,5010).
- Umrisse: Bauarten flut/huelle/loecher/band aus v1 (`skripte/g_bau.py`).
- Stellen neben Linienenden (Memory reai-v2-beschriftungslinien-messen): Linienende per Skelett; `fleck` = Zusammenhangskomponente einer Klasse (weiß L* >= 70 und C* <= 30; Auge L* >= Median(21 px) + 12) am Ende; `wachsen` = Farbgebiet ab einer Saat 4-6 px hinter dem Ende in Linienrichtung (Delta E <= 14-20), Runde 2: je Ende genau eine Komponente (die am saatnächsten Pixel).
- **Texel-Maske (T1, `skripte/g_texel.py`):** Ausschnitt um die Stelle (+12 Grundlage-px), 5 x 5 Abtastpunkte je Grundlage-Pixel (die Pixelmitte ist einer davon), je Abtastpunkt baryzentrisch u, v und CLUT-Zeile (`v4f.rastern_id`). Mehrheitsregel je (Fläche, Texel): das Texel gehört zur Stelle, wenn mindestens die Hälfte der Abtastpunkte, mit denen diese Fläche das Texel zeigt, in der Stelle liegt (angeschnittene Randtexel vergrößern die Stelle nicht). Lückenschluss einmal: nicht getroffenes Texel nur, wenn alle 4 Nachbarn getroffen sind und es im UV-Bereich einer Trägerfläche liegt (insgesamt 1 Texel). Kein Glätten. Teilfilter nur bei Augen (Kopf 22) und "großer fleischfarbener Teil" (Brust 10 / Hals 21, Render ohne Kopf 22 und Unterkiefer 23, Z4). Texel-Format `[Seite, CLUT-Zeile, u, v]`, u = Spalte in der TIM (512 x 256, 8 Bit) einschließlich 128 x Seite.
- **Geteilte Texel (T2):** UV-Bereich jeder Fläche (4 x 4 Proben je Texel, floor wie im Render). Andere Flächen mit einem Fleck-Texel im UV-Bereich; gezählt, wo der Fleck dadurch zusätzlich erscheinen würde: Bild 1 und 2 (Video-px) und 5 Rundum-Ansichten (Pixel der Rundum-Ansicht, 760 x 760).
- Teil-Zonen: Flächen der benannten Teile, Fläche >= 50 % in der Zone oder >= 15 % der Modellpixel der Zone. Kopf Bild 2 (Z4) nach Modellbedeutung; die material.json-Liste "Maulinneres" ist in Runde 2 nach Lage getrennt: Flächenmitte Knochen-y < -200 (14 Flächen von Kopf 22 bei y -307 bis -354, Augen -434 bis -468, Zähne +262 bis +282) = "Augenhöhle", Rest 21 Flächen = Maulinneres.
- **IoU** nur gegen eine Nutzerstelle: Teil-Zonen = sichtbare zugeordnete Flächen gegen Ihren Umriss; Flecken = Rückprobe der Texel-Maske gegen die Stelle; Modellgliederung, Zehen, Krallen-Schleifen/-Formen = "–".

## Zonen (je Zone eine Zeile)

| Nr | Zone | Bild | Nutzerwort | Material laut Nutzerwort | Modellteile / Texel | Stelle px | IoU | Sicherheit | Grund | Änderung |
|---|---|---|---|---|---|---|---|---|---|---|
| 1 | oberkoerper_hinten | 1 | "Oberkörper" | Haut (Teil benannt, kein Farbwort) | 0 Hüfte (19), 7 Rücken/Wirbel (17), 10 Brust/Buckel (9) | 6501 | 0,76 | sicher | Umriss deckt das Modell zu 99 %; Rumpfteile Hüfte, Rücken, Brust | – |
| 2 | wunde_hinten | 1 | "fleischige Wunde/Haut" | fleischige Wunde/Haut | texelgenau: 116 Texel, 603 Video-px auf 0 Hüfte (4 Trägerflächen) | 603 | 0,89 | sicher | Ihr grauer Umriss liegt ganz auf der Hüfte; nur dieser Umriss wird zur Wunde (Texel-Maske), nicht die ganzen Flächen | R2: texelgenau statt 3 ganzer Flächen |
| 3 | musterung_rechts_hinten | 1 | "Oberkörper Musterung rechts" | Musterung | texelgenau: 70 Texel, 229 Video-px auf 0 Hüfte, 9 Oberschenkel hinten rechts (8 Trägerflächen) | 229 | 0,82 | sicher | Ihr grüner Umriss liegt ganz auf der Hüfte; Texel-Maske in Form des Umrisses | R2: texelgenau statt ganzer Fläche |
| 4 | musterung_links_hinten | 1 | "Oberkörper Musterung links" | Musterung | texelgenau: 45 Texel, 20 Video-px auf 0 Hüfte, 2 Schwanz 2, 3 Schwanz 3 (4 Trägerflächen) | 54 | 0,82 | unsicher | liegt am Rand der Figur; das Modell zeigt nur 20 von 54 Video-px der Stelle | R2: texelgenau |
| 5 | kopf_hinten | 1 | "Kopf" | Haut (Teil benannt, kein Farbwort) | 22 Kopf (Oberkiefer) (400), 23 Unterkiefer (126); sichtbar 6 | 743 | 0,00 | verdeckt | an dieser Stelle zeigt das Modell den rechten Oberarm (666 px); der Modellkopf ist in Bild 1 nicht zu sehen | v1 ordnete die Flächen von Oberarm 16 zu; v2: nichts nach Bildlage, Kopf nur nach Bedeutung (Teile 22/23) |
| 6 | auge_hinten | 1 | "wahrscheinlich etwas Auge" | Auge | – | 21 | 0,00 | verdeckt | Nutzer: "wahrscheinlich"; Modellkopf an dieser Stelle verdeckt (0 px) | – |
| 7 | mund_hinten | 1 | "Mund?" | Maul | – | 35 | 0,00 | verdeckt | Nutzer: "Mund?"; Modellkopf an dieser Stelle verdeckt (5 px Oberarm) | v1 nannte 5 px Oberarm 16; v2 ordnet nichts zu |
| 8 | schwanz | 1 | "Schwanz der in den Rücken übergeht" | Haut (Teil benannt, kein Farbwort) | 0 Hüfte (10), 1 Schwanz 1 (30), 2 Schwanz 2 (25), 3 Schwanz 3 (32), 4 Schwanz 4 (32), 5 Schwanz 5 (23) | 11196 | 0,87 | sicher | Umriss deckt das Modell zu 98 %; Schwanzglieder 1-5 | – |
| 9 | schwanz_muster | 1 | "Schwanz Muster / Maserung" | Musterung/Maserung | texelgenau: 408 Texel, 1039 Video-px auf 3 Schwanz 3, 4 Schwanz 4, 5 Schwanz 5, 6 Schwanzspitze (Zipfel) (28 Trägerflächen) | 1039 | 0,72 | sicher | 4 grüne Umrisse auf den Schwanzgliedern 3-5; Texel-Maske in Form der Umrisse | R2: texelgenau statt 11 ganzer Flächen |
| 10 | zipfel | 1 | "Zipfel - separater Schwanzabschnitt am Schluss" | Haut (Teil benannt, kein Farbwort) | 6 Schwanzspitze (Zipfel) (12) | 3032 | 0,83 | sicher | Umriss deckt die Schwanzspitze; unten endet das Nutzerbild am Schriftzug | – |
| 11 | zipfel_musterung | 1 | "Schwanz Musterung" | Musterung | texelgenau: 446 Texel, 1094 Video-px auf 5 Schwanz 5, 6 Schwanzspitze (Zipfel) (12 Trägerflächen) | 1094 | 0,85 | sicher | 2 orange Umrisse auf der Schwanzspitze; Texel-Maske in Form der Umrisse | R2: texelgenau statt 4 ganzer Flächen |
| 12 | rotes_fleisch_zipfel | 1 | "verschieden farbiges rotes Fleisch wie oben am Becken" | rotes Fleisch | texelgenau: 133 Texel, 219 Video-px auf 5 Schwanz 5, 6 Schwanzspitze (Zipfel) (9 Trägerflächen) | 219 | 0,67 | unsicher | nicht umrandet: je Linienende das Farbgebiet in Linienrichtung (4 Stellen). Gemessen: 2 Stellen violett-rot (a* 51/46, b* -43/-40), 1 rot (a* 44, b* -7), 1 nur schwach rötlich (a* 20, b* 0); die erste Stelle ist nur 1 Video-px groß | v1: Kreise; R2: je Ende genau ein Farbgebiet, texelgenau |
| 13 | bein_hinten_links | 1 | "linkes Bein" | Haut (Teil benannt, kein Farbwort) | 8 Oberschenkel hinten links (36), 24 Unterschenkel hinten links (23), 25 Fuß hinten links (4) | 3313 | 0,77 | sicher | Umriss deckt das Modell zu 92 %; Innenkante vom Nutzer nicht gezogen (Sehne gesetzt) | – |
| 13.1 | bein_hinten_links_abschnitt1 | 1 | "linkes Bein, Bein Abschnitte. unten direkt verbunden mit den fuß (Abschnitt 1 von oben)" | Gliederung (kein Material) | 8 Oberschenkel hinten links (8) | 556 | 0,29 | unsicher | Modellbein beginnt tiefer: im obersten Abschnitt liegen auch Hüfte (183 px) und Schwanz (123 px) | – |
| 13.2 | bein_hinten_links_abschnitt2 | 1 | "linkes Bein, Bein Abschnitte. unten direkt verbunden mit den fuß (Abschnitt 2 von oben)" | Gliederung (kein Material) | 8 Oberschenkel hinten links (8), 24 Unterschenkel hinten links (1) | 783 | 0,83 | sicher | Abschnitt zwischen lila Trennlinien; Teile des benannten Beins zugeordnet | – |
| 13.3 | bein_hinten_links_abschnitt3 | 1 | "linkes Bein, Bein Abschnitte. unten direkt verbunden mit den fuß (Abschnitt 3 von oben)" | Gliederung (kein Material) | 8 Oberschenkel hinten links (18), 24 Unterschenkel hinten links (9) | 1051 | 0,74 | sicher | Abschnitt zwischen lila Trennlinien; Teile des benannten Beins zugeordnet | – |
| 13.4 | bein_hinten_links_abschnitt4 | 1 | "linkes Bein, Bein Abschnitte. unten direkt verbunden mit den fuß (Abschnitt 4 von oben)" | Gliederung (kein Material) | 24 Unterschenkel hinten links (12), 25 Fuß hinten links (4) | 802 | 0,69 | sicher | Abschnitt zwischen lila Trennlinien; Teile des benannten Beins zugeordnet | – |
| 14 | weisse_musterung_bein_hinten_links | 1 | "weiße Musterung" | weiße Musterung | texelgenau: 20 Texel, 25 Video-px auf 0 Hüfte, 3 Schwanz 3, 8 Oberschenkel hinten links (4 Trägerflächen) | 25 | 0,73 | sicher | weißer Fleck 5 px rechts neben dem Linienende (L* 76 gegen 47 daneben), vor allem auf Oberschenkel 8; texelgenau | Z1: v1-Kreis lag auf dunkler Hüfte; jetzt der weiße Fleck, texelgenau |
| 15 | weisse_musterung_bein_hinten_links_strich2 | 1 | "weiße Musterung (zweiter grüner Strich, ohne eigenen Text)" | weiße Musterung | texelgenau: 19 Texel, 22 Video-px auf 0 Hüfte, 3 Schwanz 3, 8 Oberschenkel hinten links (4 Trägerflächen) | 22 | 0,78 | unsicher | zweiter grüner Strich ohne eigenen Text; an seinem Ende (11,7 px) liegt derselbe weiße Fleck wie Nr. 14 (Frage) | Z5: neu; zweiter Zeiger auf denselben Fleck |
| 16 | braeunliche_musterung_bein_hinten_links | 1 | "bräunliche Musterung" | bräunliche Musterung | texelgenau: 94 Texel, 76 Video-px auf 8 Oberschenkel hinten links (2 Trägerflächen) | 76 | 0,67 | unsicher | Farbgebiet direkt am Linienende ist dunkel rot-violett (L* 27, a* 26, b* -6 gegen L* 20, a* 17 daneben), kaum braun | v1: Kreis; R2: Farbgebiet neben dem Ende, texelgenau; Sicherheit sicher -> unsicher |
| 17 | bein_hinten_rechts | 1 | "rechtes Bein hinten" | Haut (Teil benannt, kein Farbwort) | 9 Oberschenkel hinten rechts (31), 26 Unterschenkel hinten rechts (36), 27 Fuß hinten rechts (2) | 5437 | 0,73 | sicher | Umriss deckt das Modell zu 95 % | – |
| 17.1 | bein_hinten_rechts_abschnitt1 | 1 | "rechtes Bein, Beinabschnitte. Unten direkt verbunden mit fuß. (Abschnitt 1 von oben)" | Gliederung (kein Material) | 9 Oberschenkel hinten rechts (23), 26 Unterschenkel hinten rechts (2) | 3312 | 0,66 | unsicher | im obersten Abschnitt liegt im Modell auch die Hüfte (938 px) | – |
| 17.2 | bein_hinten_rechts_abschnitt2 | 1 | "rechtes Bein, Beinabschnitte. Unten direkt verbunden mit fuß. (Abschnitt 2 von oben)" | Gliederung (kein Material) | 9 Oberschenkel hinten rechts (7), 26 Unterschenkel hinten rechts (19) | 1213 | 0,80 | sicher | Abschnitt zwischen lila Trennlinien; Teile des benannten Beins zugeordnet | – |
| 17.3 | bein_hinten_rechts_abschnitt3 | 1 | "rechtes Bein, Beinabschnitte. Unten direkt verbunden mit fuß. (Abschnitt 3 von oben)" | Gliederung (kein Material) | 26 Unterschenkel hinten rechts (13), 27 Fuß hinten rechts (2) | 741 | 0,67 | sicher | Abschnitt zwischen lila Trennlinien; Teile des benannten Beins zugeordnet | – |
| 18 | weisse_musterung_bein_hinten_rechts | 1 | "weiße Musterung" | weiße Musterung | texelgenau: 14 Texel, 107 Video-px auf 0 Hüfte, 9 Oberschenkel hinten rechts (3 Trägerflächen) | 107 | 0,67 | sicher | weißer Fleck am Linienende (L* 90); liegt im Modell zu 88 % auf der Hüfte, Rest Oberschenkel 9; texelgenau | Z3: ganzer Fleck statt Kreis; texelgenau |
| 19 | braeunliche_musterung_bein_hinten_rechts | 1 | "bräunliche Musterung" | bräunliche Musterung | texelgenau: 13 Texel, 51 Video-px auf 9 Oberschenkel hinten rechts (4 Trägerflächen) | 51 | 0,58 | unsicher | im Grundbild nicht braun messbar (L* 63,5, a* 7,7, b* -20,7: graublau); gemeint vermutlich die helle Fläche links vom Linienende unter dem zweiten dunkelroten Strich (Frage) | v1: Band um den Strich; R2: Farbgebiet neben dem Ende, texelgenau |
| 20 | fuss_hinten_links | 1 | "linker Fuß mit Teil der Fußkralle vorne. Kaum sichtbar, aber wahrscheinlich 3 Krallen. Rechts wäre es sicher genauso" | Haut (Teil benannt, kein Farbwort) | 25 Fuß hinten links (16) | 1284 | 0,23 | unsicher | Modellfuß deckt nur 26 % der Nutzer-Hülle; die Krallenmarken liegen neben dem Modellfuß | – |
| 21 | fusskrallen_hinten_links | 1 | "linker Fuß mit Teil der Fußkralle vorne. Kaum sichtbar, aber wahrscheinlich 3 Krallen. Rechts wäre es sicher genauso" | Kralle | 25 Fuß hinten links (30); sichtbar 4 | 15 | – | unsicher | Nutzermarken liegen über Hintergrund rechts neben dem Modellfuß (0 px auf Fuß 25); Vorschlag nach Bedeutung: die 3 Zehenspitzen von Fuß 25 | v1: Marken auf Schwanz 5; v2: Zehenspitzen von Fuß 25 nach Bedeutung |
| 22 | fusskrallen_hinten_rechts | 1 | "Rechts wäre es sicher genauso (aus demselben Text)" | Kralle | 27 Fuß hinten rechts (30); sichtbar 2 | 1 | – | verdeckt | nur Textangabe "Rechts wäre es sicher genauso"; Fuß 27 in keinem Bild beschriftet | neu |
| 23 | vorderbein_rechts | 1 | "rechtes Bein und Fuß" | Haut (Teil benannt, kein Farbwort) | 16 Oberarm vorn rechts (13), 17 Unterarm vorn rechts (32), 18 Hand vorn rechts (25) | 3418 | 0,85 | sicher | Umriss deckt das Modell zu 97 % | – |
| 23.1 | vorderbein_rechts_abschnitt1 | 1 | "wahrscheinliche Bein Abschnitte (Abschnitt 1 von oben)" | Gliederung (kein Material) | 16 Oberarm vorn rechts (13), 17 Unterarm vorn rechts (9) | 1413 | 0,77 | unsicher | Nutzer: "wahrscheinliche Bein Abschnitte"; Abschnitt zwischen lila Trennlinien | – |
| 23.2 | vorderbein_rechts_abschnitt2 | 1 | "wahrscheinliche Bein Abschnitte (Abschnitt 2 von oben)" | Gliederung (kein Material) | 17 Unterarm vorn rechts (6) | 614 | 0,60 | unsicher | Nutzer: "wahrscheinliche Bein Abschnitte"; Abschnitt zwischen lila Trennlinien | – |
| 23.3 | vorderbein_rechts_abschnitt3 | 1 | "wahrscheinliche Bein Abschnitte (Abschnitt 3 von oben)" | Gliederung (kein Material) | 17 Unterarm vorn rechts (10), 18 Hand vorn rechts (7) | 625 | 0,58 | unsicher | Nutzer: "wahrscheinliche Bein Abschnitte"; Abschnitt zwischen lila Trennlinien | – |
| 23.4 | vorderbein_rechts_abschnitt4 | 1 | "wahrscheinliche Bein Abschnitte (Abschnitt 4 von oben)" | Gliederung (kein Material) | 17 Unterarm vorn rechts (3), 18 Hand vorn rechts (17) | 605 | 0,77 | unsicher | Nutzer: "wahrscheinliche Bein Abschnitte"; Abschnitt zwischen lila Trennlinien | – |
| 24 | weisse_musterung_vorderbein_rechts | 1 | "weiße Musterung" | weiße Musterung | texelgenau: 290 Texel, 250 Video-px auf 16 Oberarm vorn rechts, 17 Unterarm vorn rechts (7 Trägerflächen) | 250 | 0,80 | sicher | grüner Umriss auf Oberarm 16 | – |
| 25 | fuss_kralle_vorderbein_rechts | 1 | "Fuß und Kralle" | Fuß: Haut (Teil benannt, kein Farbwort); Kralle: im Modell hier nicht sichtbar | 18 Hand vorn rechts (4) | 149 | 0,23 | unsicher | das Modell zeigt hier nur die Hand 18 (95 px) = Fuß; eine Kralle ist im Modell an dieser Stelle nicht zu sehen | R2: Hand nicht mehr als Kralle geführt |
| 26 | oberkoerper_vorn | 2 | "Oberkörper" | Haut (Teil benannt, kein Farbwort) | 10 Brust/Buckel (44), 21 Hals (20) | 18443 | 0,59 | unsicher | der Modellkopf reicht 70 px in diesen Umriss (4966 px); ohne Kopf gezählt | Z4: Kopfflächen herausgenommen |
| 27 | fleisch_vorn | 2 | "großer fleischfarbener Teil" | fleischfarben | texelgenau: 2201 Texel, 13709 Video-px auf 10 Brust/Buckel, 21 Hals (68 Trägerflächen) | 14762 | 0,85 | unsicher | der Modellkopf deckt 4812 px dieses Umrisses; Texel-Maske nur auf Brust 10 und Hals 21, gemessen im Bild ohne Kopf (Z4) | Z4 + R2: ohne Kopf 22/Unterkiefer 23, texelgenau |
| 28 | gesicht_vorn | 2 | "Vorderseite Gesicht mit Maul nach unten rechts blickend" | Haut (Teil benannt, kein Farbwort) | 22 Kopf (Oberkiefer) (400), 23 Unterkiefer (126); sichtbar 163 | 13161 | 0,68 | unsicher | Modellkopf reicht 70 px höher als Ihr Umriss; Zuordnung nach Bedeutung: ganzer Kopf 22 und Unterkiefer 23 | Z4: nach Bedeutung statt nach Bildlage |
| 29 | augen_vorn | 2 | "Augen" | Auge | texelgenau: 21 Texel, 19 Video-px auf 22 Kopf (Oberkiefer) (5 Trägerflächen) | 19 | 0,66 | sicher | zwei helle Punkte 5,1 bzw. 2,2 px neben den Linienenden (L* 30 gegen 5-7); Auge = nur die Texel unter den Punkten | Z2: gemessene Punkte statt Kreise; R2: nur die Texel, nicht ganze Flächen |
| 30 | maul_vorn | 2 | "geöffnetes Maul" | Maul | 22 Kopf (Oberkiefer) (10), 23 Unterkiefer (11); sichtbar 0 | 2316 | 0,00 | unsicher | Ihr Maul-Umriss zeigt im Modell die Außenseite des Unterkiefers (1445 px) und Zähne, weil der Modellkopf anders sitzt; das Maulinnere ist in Bild 2 im Modell nicht zu sehen (0 px). Zuordnung nach Bedeutung: die 21 Flächen des Maulinneren | Z4 + R2: ohne Zähne, ohne die 14 Augenhöhlen-Flächen |
| 31 | unterkiefer_vorn | 2 | "Teil des Unterkiefers" | Unterkiefer (kein Farbwort) | 23 Unterkiefer (79); sichtbar 29 | 304 | 0,05 | unsicher | Band am lila Strich; dort zeigt das Modell Unterkiefer (89 px) und Unterschenkel (48 px); nach Bedeutung: Unterkiefer außen | Z4: nach Bedeutung |
| 32 | arm_hand_krallen_vorn_rechts | 2 | "vorderer rechter Arm mit Krallen" | Haut (Teil benannt, kein Farbwort) | 18 Hand vorn rechts (14), 31 Kralle vorn rechts 31 (6) | 4254 | 0,74 | sicher | Umriss deckt das Modell zu 87 % | – |
| 33 | arm_vorn_rechts | 2 | "vorderer rechter Arm" | Haut (Teil benannt, kein Farbwort) | 18 Hand vorn rechts (3) | 359 | 0,16 | unsicher | an Ihrem Abschnitt zeigt das Modell die Hand 18 (239 px) und den Kopf 22 (45 px), nicht den Arm; Zuordnung deckt die Stelle schlecht (IoU 0,16) | R2: sicher -> unsicher |
| 34 | hand_vorn_rechts | 2 | "vordere rechte Hand mit Krallen" | Haut (Teil benannt, kein Farbwort) | 18 Hand vorn rechts (8) | 2747 | 0,73 | sicher | Umriss deckt Hand 18 zu 100 % | – |
| 35 | krallen_vorn_rechts | 2 | "Krallen" | Kralle | 31 Kralle vorn rechts 31 (6) | 1147 | 0,15 | unsicher | 2 Krallenformen gezeichnet (rechts im Bild auf Kralle 31, links im Bild auf Kralle 19), dazwischen der Schriftzug; im Asset ist 19 eingezogen | v1: eine Zone; v2: die zwei Formen und die Fläche dazwischen getrennt |
| 35.1 | krallen_vorn_rechts_form1 | 2 | "Krallen (gezeichnete Form rechts im Bild)" | Kralle | 31 Kralle vorn rechts 31 (30) | 291 | – | sicher | Form deckt Kralle 31 (im Asset sichtbar); unteres Formende 17.5 px neben der Spitze von 31 | neu (eigene Zone) |
| 35.2 | krallen_vorn_rechts_form3 | 2 | "Krallen (gezeichnete Form links im Bild)" | Kralle | 19 Kralle vorn rechts 19 (30) | 61 | – | unsicher | Form liegt auf Kralle 19 in K5c-Lage; deren Spitze liegt 42 px tiefer als Ihr Strichende; im Asset ist 19 eingezogen | neu (eigene Zone); R2: Name ohne "Form 3" |
| 35.3 | krallen_vorn_rechts_dazwischen | 2 | "Krallen (Fläche zwischen den Formen, mit Schriftzug)" | Kralle | 20 Kralle vorn rechts 20 (30), 29 Kralle vorn rechts 29 (30) | 803 | – | verdeckt | Annahme nach Ihrem Wort "wahrscheinlich auch 4 ... wegen der Schrift": Krallen 20 und 29 lägen hier verdeckt; im Kandidaten bleiben sie eingezogen wie heute (Frage) | neu (eigene Zone); R2: 20/29 nicht mehr gedreht |
| 36 | vorderbein_links | 2 | "Vorderes linkes Bein mit Krallen" | Haut (Teil benannt, kein Farbwort) | 11 Oberarm vorn links (15), 12 Unterarm vorn links (22), 13 Hand vorn links (21), 14 Kralle vorn links 14 (8), 15 Kralle vorn links 15 (6) | 6736 | 0,74 | sicher | Umriss deckt das Modell zu 96 % | – |
| 37 | beinabschnitt_vorn_links | 2 | "Beinabschnitt" | Haut (Teil benannt, kein Farbwort) | 11 Oberarm vorn links (15), 12 Unterarm vorn links (20) | 4359 | 0,68 | sicher | Oberarm 11 und Unterarm 12 | – |
| 38 | fussabschnitt_vorn_links | 2 | "Fußabschnitt" | Haut (Teil benannt, kein Farbwort) | 13 Hand vorn links (17), 14 Kralle vorn links 14 (8), 15 Kralle vorn links 15 (6) | 2377 | 0,84 | sicher | Hand 13 mit Krallen | – |
| 39 | krallen_vorn_links | 2 | "Exakt 4 Krallen" | Kralle | 13 Hand vorn links (4), 15 Kralle vorn links 15 (6), 30 Kralle vorn links 30 (2) | 460 | 0,49 | sicher | 4 geschlossene Schleifen = "Exakt 4" | v2: je Schleife eine eigene Zone |
| 39.1 | kralle_vorn_links_schleife1 | 2 | "Exakt 4 Krallen (Schleife 1 von links im Bild)" | Kralle | 30 Kralle vorn links 30 (30) | 32 | – | unsicher | Spitze 4,7 px neben der Spitze von Kralle 30, aber Kralle 30 ist im Modell nur 2 px sichtbar (1 px in der Schleife) | R2: Spitze am richtigen Ende; sicher -> unsicher |
| 39.2 | kralle_vorn_links_schleife2 | 2 | "Exakt 4 Krallen (Schleife 2 von links im Bild)" | Kralle | 14 Kralle vorn links 14 (30) | 118 | – | unsicher | Spitze 2,2 px neben der Handspitze 13 und 6,4 px neben Kralle 14; Vorschlag Kralle 14 (die linke Hand hat genau 4 Krallenteile: 14, 15, 28, 30) | neu (eigene Zone) |
| 39.3 | kralle_vorn_links_schleife3 | 2 | "Exakt 4 Krallen (Schleife 3 von links im Bild)" | Kralle | 15 Kralle vorn links 15 (42) | 250 | – | sicher | Spitze 3,0 px neben Kralle 15 | neu (eigene Zone) |
| 39.4 | kralle_vorn_links_schleife4 | 2 | "Exakt 4 Krallen (Schleife 4 von links im Bild)" | Kralle | 28 Kralle vorn links 28 (30) | 60 | – | verdeckt | Spitze 8,3 px neben Kralle 28 in K5c-Form; im heutigen Asset ist 28 eingezogen (0 px). Zurückgeholt liegt 28 nur zu 2 % in Ihrer Schleife, der Rest über dem Fußabschnitt (Frage) | neu (eigene Zone) |
| 40 | augenhoehle | 2 | – (kein Wort von Ihnen; Modellgliederung: Flächen rund um die Augen, die dieselbe Texturstelle wie das Maulinnere benutzen (Modellgliederung)) | nicht belegt (kein Wort von Ihnen) | 22 Kopf (Oberkiefer) (14); sichtbar 2 | 23 | – | unsicher | kein Wort von Ihnen. 14 Flächen um die Augen benutzen dieselbe Texturstelle wie das Maulinnere (u 91-105, v 157-170); in Bild 2 sind 2 davon sichtbar (23 px), keine unter Ihren Augenpunkten | R2: war "Maulinneres" (Prüfer: lag an den Augen) |
| 41 | zaehne | 2 | – (kein Wort von Ihnen; Modellgliederung: Zähne (Modellgliederung, im "geöffneten Maul")) | nicht belegt (kein Wort von Ihnen) | 22 Kopf (Oberkiefer) (78), 23 Unterkiefer (36); sichtbar 26 | 453 | – | unsicher | kein Wort von Ihnen; Zähne des Modells, 391 von 453 px liegen in Ihrem Maul-Umriss (Frage: eigenes Material?) | R2: Material nicht mehr "Zahn", nicht belegt |
| 42 | lippe | 2 | – (kein Wort von Ihnen; Modellgliederung: Oberlippe (Modellgliederung, Rand des "geöffneten Mauls")) | nicht belegt (kein Wort von Ihnen) | 22 Kopf (Oberkiefer) (78); sichtbar 25 | 2596 | – | unsicher | kein Wort von Ihnen; Oberlippe des Kopfes 22, nur 297 px in Ihrem Maul-Umriss | R2: Material nicht mehr "Lippe", nicht belegt |
| 44 | hals_unterseite | 2 | – (kein Wort von Ihnen; Modellgliederung: Hals-Unterseite (Modellgliederung)) | Haut (nicht belegt) | 21 Hals (29); sichtbar 4 | 524 | – | verdeckt | kein Wort von Ihnen; Flächen von Hals 21 mit Normalen nach unten; bleibt Haut (nicht belegt) | neu (Z4, Modellgliederung) |

## Flecken texelgenau und geteilte Texel (T1, T2)

| Nr | Zone | Texel (getroffen + Lücke) | Fläche Video-px (Stelle) | Rückprobe IoU | nicht übernommen (Teilfilter) | andere Flächen mit denselben Texeln | zusätzlich sichtbar Bild 1 / Bild 2 (Video-px) | Teile, die eigene Texel bräuchten |
|---|---|---|---|---|---|---|---|---|
| 2 | wunde_hinten | 116 (116 + 0) | 603 (603) | 0,89 | – | 0 | 0 / 0 | – |
| 3 | musterung_rechts_hinten | 70 (70 + 0) | 229 (229) | 0,82 | – | 27 | 31 / 0 | 0 Hüfte (14), 7 Rücken/Wirbel (9), 9 Oberschenkel hinten rechts (3), 8 Oberschenkel hinten links (1) |
| 4 | musterung_links_hinten | 45 (45 + 0) | 20 (54) | 0,82 | – | 63 | 5 / 6 | 2 Schwanz 2 (21), 1 Schwanz 1 (14), 0 Hüfte (12), 3 Schwanz 3 (7), 12 Unterarm vorn links (7), 17 Unterarm vorn rechts (2) |
| 9 | schwanz_muster | 408 (408 + 0) | 1039 (1039) | 0,72 | – | 304 | 1167 / 550 | 5 Schwanz 5 (40), 1 Schwanz 1 (38), 16 Oberarm vorn rechts (35), 11 Oberarm vorn links (33), 12 Unterarm vorn links (32), 4 Schwanz 4 (31), 6 Schwanzspitze (Zipfel) (29), 17 Unterarm vorn rechts (27), 3 Schwanz 3 (25), 2 Schwanz 2 (14) |
| 11 | zipfel_musterung | 446 (446 + 0) | 1094 (1094) | 0,85 | – | 208 | 1180 / 993 | 16 Oberarm vorn rechts (48), 11 Oberarm vorn links (41), 5 Schwanz 5 (33), 4 Schwanz 4 (31), 1 Schwanz 1 (21), 6 Schwanzspitze (Zipfel) (16), 2 Schwanz 2 (6), 3 Schwanz 3 (6), 12 Unterarm vorn links (3), 17 Unterarm vorn rechts (3) |
| 12 | rotes_fleisch_zipfel | 133 (133 + 0) | 219 (219) | 0,67 | – | 166 | 119 / 250 | 16 Oberarm vorn rechts (48), 5 Schwanz 5 (35), 11 Oberarm vorn links (29), 4 Schwanz 4 (22), 1 Schwanz 1 (18), 6 Schwanzspitze (Zipfel) (13), 2 Schwanz 2 (1) |
| 14 | weisse_musterung_bein_hinten_links | 20 (20 + 0) | 25 (25) | 0,73 | – | 61 | 21 / 0 | 9 Oberschenkel hinten rechts (15), 8 Oberschenkel hinten links (14), 0 Hüfte (6), 1 Schwanz 1 (5), 12 Unterarm vorn links (5), 3 Schwanz 3 (4), 2 Schwanz 2 (3), 4 Schwanz 4 (3), 17 Unterarm vorn rechts (3), 7 Rücken/Wirbel (2), 5 Schwanz 5 (1) |
| 15 | weisse_musterung_bein_hinten_links_strich2 | 19 (19 + 0) | 22 (22) | 0,78 | – | 61 | 18 / 0 | 9 Oberschenkel hinten rechts (15), 8 Oberschenkel hinten links (14), 0 Hüfte (6), 1 Schwanz 1 (5), 12 Unterarm vorn links (5), 3 Schwanz 3 (4), 2 Schwanz 2 (3), 4 Schwanz 4 (3), 17 Unterarm vorn rechts (3), 7 Rücken/Wirbel (2), 5 Schwanz 5 (1) |
| 16 | braeunliche_musterung_bein_hinten_links | 94 (94 + 0) | 76 (76) | 0,67 | – | 59 | 168 / 5 | 9 Oberschenkel hinten rechts (35), 8 Oberschenkel hinten links (24) |
| 18 | weisse_musterung_bein_hinten_rechts | 14 (14 + 0) | 107 (107) | 0,67 | – | 35 | 32 / 0 | 0 Hüfte (10), 9 Oberschenkel hinten rechts (10), 8 Oberschenkel hinten links (8), 7 Rücken/Wirbel (7) |
| 19 | braeunliche_musterung_bein_hinten_rechts | 13 (13 + 0) | 51 (51) | 0,58 | – | 27 | 10 / 0 | 9 Oberschenkel hinten rechts (17), 8 Oberschenkel hinten links (10) |
| 24 | weisse_musterung_vorderbein_rechts | 290 (290 + 0) | 250 (250) | 0,80 | – | 247 | 229 / 1179 | 10 Brust/Buckel (88), 21 Hals (51), 22 Kopf (Oberkiefer) (50), 16 Oberarm vorn rechts (35), 23 Unterkiefer (8), 17 Unterarm vorn rechts (5), 3 Schwanz 3 (3), 12 Unterarm vorn links (3), 1 Schwanz 1 (2), 4 Schwanz 4 (2) |
| 27 | fleisch_vorn | 2201 (2200 + 1) | 13709 (14762) | 0,85 | Oberschenkel hinten links 209, Oberarm vorn links 116, Oberarm vorn rechts 512, Unterarm vorn rechts 57, Unterschenkel hinten links 36 | 556 | 1636 / 14011 | 10 Brust/Buckel (196), 22 Kopf (Oberkiefer) (166), 21 Hals (96), 16 Oberarm vorn rechts (63), 23 Unterkiefer (35) |
| 29 | augen_vorn | 21 (21 + 0) | 19 (19) | 0,66 | – | 179 | 63 / 53 | 10 Brust/Buckel (43), 22 Kopf (Oberkiefer) (37), 21 Hals (29), 7 Rücken/Wirbel (26), 16 Oberarm vorn rechts (22), 0 Hüfte (18), 23 Unterkiefer (4) |

## Änderungen gegenüber v1 und Runde 1 (mit Grund)

| Punkt | vorher | jetzt (Runde 2) | Grund / Beleg |
|---|---|---|---|
| Z1 weiße Musterung Hinterbein l | v1 Kreis auf dem Linienende, Hüfte 0 | weißer Fleck 5,0 px rechts neben dem Ende (L* 76 gegen 47), texelgenau (20 Texel) | Prüfung v1 IoU 0,06 |
| Z2 Augen | v1 Kreise; Runde 1: 3 bzw. 6 ganze Flächen, links 8,7-fach zu groß | nur die Texel unter den Punkten, eine Liste (rechts 9, links 12 Texel) | Prüfer r1 Nr. 7; Nutzer: Markierung gilt |
| Z3 weiße Musterung Hinterbein r | v1 Kreis, 39 % | ganzer Fleck (L* 90), texelgenau (14 Texel), Hüfte und Oberschenkel 9 | Prüfung v1 |
| Z4 Kopf Bild 2 | v1 nach Bildlage | fleisch/Oberkörper ohne 22/23; Gesicht/Maul/Unterkiefer nach Bedeutung; Kopfteile ohne Nutzerwort = Modellgliederung, Material "nicht belegt" | Modellkopf 70 px zu hoch; Prüfer r1 Nr. 9 |
| Z5 zweiter grüner Strich | nicht verwendet | Zone 15 auf denselben weißen Fleck (Frage) | Prüfung v1 |
| Flecken allgemein (T1) | ganze Flächen ("komplette Quader") | Texel-Masken in der Form der Stelle, Nahansicht je Fleck | Nutzer 2026-11-09 |
| Geteilte Texel (T2) | nicht geprüft | je Fleck gezählt und benannt (Tafel 23) | Nutzerauftrag T2 |
| Zone 40 | "Maulinneres" (23 px an den Augen) | "Augenhöhle": 14 Flächen um die Augen mit der Maulinneres-Texturstelle, Material nicht belegt | Prüfer r1 Nr. 8: Liste nach Lage getrennt |
| Zonen 41/42 | Material "Zahn" / "Lippe" unter "Ihr Wort" | kein Nutzerwort, Material "nicht belegt"; Zahnflächen nicht mehr in Zone 30 | Prüfer r1 Nr. 9 |
| Zone 43 | "Unterkiefer außen" doppelt zu Zone 31 | entfällt (= Zone 31 "Teil des Unterkiefers") | Doppelung |
| Zone 30 "geöffnetes Maul" | Maulinneres + Zähne, Material "Maul innen" | 21 Flächen Maulinneres, Material "Maul" | Prüfer r1 Nr. 9/14 |
| Zone 7 "Mund?" | Material "Maul innen" | "Maul" | Prüfer r1 Nr. 14 ("innen" war dazuerfunden) |
| Zone 25 "Fuß und Kralle" | Hand als Kralle | Hand = Fuß (Haut), Kralle im Modell nicht sichtbar | Prüfer r1 Nr. 14 |
| Zone 33 | sicher | unsicher (Modell zeigt Hand und Kopf, IoU 0,16) | Prüfer r1 Nr. 16 |
| Zone 16 / 19 bräunlich | 16 sicher; Messblatt-Werte 19 falsch | 16 unsicher (rot-violett); 19 Werte aus den Daten (L* 63,5, a* 7,7, b* -20,7) | Prüfer r1 Nr. 15 |
| Zone 12 rotes Fleisch | 6 Teile bei 4 Enden | je Ende genau ein Farbgebiet; Farben je Stelle im Grund | Prüfer r1 Nr. 15 |
| Krallen vorn links | Schleife 1 Spitze am Ansatz, 39.1 sicher | Spitze = fernster Punkt vom Fußabschnitt ([406.2, 397.3] gegen Runde 1 [401.7, 389.8]); 39.1 unsicher; Frage zu 28 | Prüfer r1 Nr. 11 |
| Krallen-Kandidat | 20/29 gedreht (über Hintergrund, in der Hand) | 20/29 eingezogen wie im Asset; nur 19 und 28 anders (385 Bytes) | Prüfer r1 Nr. 12/13 |
| Krallenformen vorn rechts | "Form 1" / "Form 3" | "Form rechts im Bild" / "Form links im Bild" | Prüfer r1 Nr. 18 |
| IoU-Spalte | bei 21, 22, 40-44 gegen die eigene Zuordnung (1,00) | "–" ohne Nutzerstelle | Prüfer r1 Nr. 17 |

## Augen (Z2)

| Auge (Wesen) | Linienende | Punkt Grundlage / Video | Abstand zum Ende | Punkt Video-px | Auge-Texel | Trägerflächen (Texel, Art) | heutiges G4-Auge: Video-px, Abstand Mitte, Überlappung |
|---|---|---|---|---|---|---|---|
| Auge rechts (vom Wesen aus; links im Bild) | [441, 245] | [47.8, 247.3] / [217.4, 274.3] | 5,1 | 6,5 | 9 | 22/0/82 8 heute G4-Auge, 22/0/83 3 heute G4-Auge, 22/1/152 1 Haut | 68, 4,6 px, 4,8 px |
| Auge links (vom Wesen aus; rechts im Bild) | [568, 248] | [173.4, 248.9] / [280.2, 275.1] | 2,2 | 12,3 | 12 | 22/1/112 7 Haut, 22/1/113 5 Haut | 75, 7,7 px, 0,0 px |

Heutige G4-Augen: 64 Flächen, 20 in Bild 2 sichtbar (143 Video-px). Vorschlag: Auge = die 21 Texel unter den Punkten; die Flächen tragen sie nur. Augenhöhe Modell 270,1 / 271,1 / Video 274,3 / 275,1, Augenabstand Modell 54,6 / Video 62,8 Video-px.

## Krallen-Kandidat (nur `krallen_kandidat/`, nicht im Asset)

- `krallen_kandidat/BAHNSTEIG_FORM.EMD` = Spiel-Asset mit ersetzten Ecken/Normalen der Teile [19, 28] (385 Bytes verschieden, gleiche Länge; mit `cmp -l` nachgezählt); TIM unverändert kopiert.
- 28: K5c-Ecken (2 von 17 Ecken außerhalb der Wurzel stecken in Hand 13, wie in K5c). 19: K5c-Ecken, gestreckt entlang Wurzel -> Spitze mit Faktor 0,4898; Spitze K5c [203.3, 499.1] -> [218.1, 462.5], Nutzerstrichende [202.96, 457.33], Rest 16,0 px; 0 Ecken in Hand 18.
- 20 und 29: eingezogen wie im Asset. Runde 1 hatte 6000 Drehungen um die Wurzel geprüft; keine lag ganz im Schriftzug ohne Pixel über Hintergrund und ohne Durchdringung der Hand (bestes Ergebnis 135 / 12 px über Hintergrund, 2 / 1 Ecken in der Hand, in Bild 1 330 / 683 px über Hintergrund). Frage an den Nutzer.
- Schriftzug-Fläche: helle Glyphen (L* >= 75, C* <= 30) im Textblock x 0-261, y 434-479 der Vorlage, 3 px verdickt, 9 px geschlossen (9192 px).

| Stand (Bild 2: sichtbar / über Hintergrund) | 14 | 15 | 28 | 30 | 19 | 20 | 29 | 31 |
|---|---|---|---|---|---|---|---|---|
| asset | 326 / 17 | 699 / 25 | 0 / 0 | 2 / 0 | 0 / 0 | 0 / 0 | 0 / 0 | 419 / 126 |
| k5c | 326 / 17 | 699 / 25 | 312 / 12 | 2 / 0 | 1521 / 161 | 1734 / 1686 | 1782 / 1782 | 419 / 126 |
| vorschlag | 326 / 17 | 699 / 25 | 312 / 12 | 2 / 0 | 838 / 28 | 0 / 0 | 0 / 0 | 419 / 126 |

Spitzen (Video-px, Regel: fernster Umrisspunkt vom Schwerpunkt des Fußabschnitts): Schleife 1 [406.2, 397.3] -> 30 (30 4,7, 13 38,6); Schleife 2 [440.7, 418.8] -> 13 (13 2,2, 14 6,4); Schleife 3 [470.2, 416.3] -> 15 (15 3,0, 28 24,2); Schleife 4 [472.7, 396.8] -> 28 (28 8,3, 15 21,4); Form rechts im Bild, unteres Ende -> 31 17,5 px; Form links im Bild, unteres Ende -> 19 (K5c) 48,4 px.

## Kopfumriss (Bild 2, nur gemessen)

| Kante | Nutzerumriss | Modell heute | Modell minus Nutzer | Video |
|---|---|---|---|---|
| oben | 265.1 | 195.6 | -69.6 | keine Kante (L*-Profile in `zonen_v2.json` kopf.video_oberkante) |
| unten | 406.8 | 400.8 | -6.0 | Maul-Unterkante |
| links | 207.0 | 211.5 | +4.5 | Silhouette in Augenhöhe 201.5, Modell 214.0 |
| rechts | 350.6 | 348.1 | -2.5 | dunkel auf dunkel, nicht messbar |

Modellkopf über der Nutzer-Oberkante: 3230 Video-px. IoU Modellkopf/Nutzerumriss 0.678.

## Fragen an den Nutzer

1. Kopfhöhe (Bild 2): Der Modellkopf reicht 70 Video-Pixel höher als Ihr Gesichtsumriss (oben y 196 statt 265). Im Video ist die Kopfoberkante nicht als Kante zu sehen (alles gleich dunkel). Gilt Ihre rote Linie oben als Kopfoberkante, d. h. ist das, was das Modell darüber zeigt, Brust ("großer fleischfarbener Teil")?
2. Kopf von hinten (Bild 1): An der Stelle Ihres Kopf-Umrisses zeigt das Modell den rechten Oberarm, der Kopf ist dahinter verdeckt. Soll der Kopf dort sichtbar sein?
3. Augen, Größe (Bild 2): Ihre Augenpunkte sind klein: rechts 6,5, links 12,3 Video-Pixel. Die heutigen Modellaugen sind 68 bzw. 75 Video-Pixel groß und liegen 4,6 bzw. 7,7 Pixel daneben. Mein Vorschlag (beide Augen nach derselben Regel): Auge ist NUR der helle Punkt, das übrige heutige Augenfeld wird Haut. Richtig, oder ist das ganze dunkle Augenfeld um den Punkt das Auge? Sollen beide Augen gleich groß sein (heute ist das linke fast doppelt so groß wie das rechte)?
4. Augenhöhle: 14 Modellflächen rund um die Augen benutzen dieselbe Texturstelle wie das Maulinnere (in Bild 2 sind 2 davon sichtbar, 23 Video-Pixel, direkt neben den Augen). In Runde 1 hatte ich sie fälschlich "Maulinneres" genannt. Sollen sie Haut sein (mein Vorschlag, Zone 40)?
5. Maul (Bild 2): An der Stelle Ihres "geöffneten Mauls" zeigt das Modell die Außenseite des Unterkiefers und Zähne, weil der Modellkopf anders sitzt; das Maulinnere des Modells ist in Bild 2 gar nicht zu sehen. Ich ordne "geöffnetes Maul" den 21 Flächen des Maulinneren zu. Die Zähne (Zone 41) haben kein Wort von Ihnen: eigenes Material, wie das Maul, oder Haut?
6. Krallen vorn rechts: Ihre Annahme "wahrscheinlich auch 4": Kralle 20 und 29 lassen sich durch bloßes Drehen nicht hinter der Schrift verstecken (6000 Drehungen geprüft; die beste stand noch über dem Hintergrund, steckte in der Hand und stünde in Bild 1 sichtbar ab). Im Vorschlag bleiben 20 und 29 eingezogen wie heute. Sollen sie so bleiben, oder kürzer zurückkommen?
7. Kralle 19 (Ihre Form links im Bild): Mit einem einzigen Längenfaktor (x0,49) liegt die Spitze noch 16 Pixel neben Ihrem Strichende. Endet die Kralle an Ihrem Strichende, oder läuft sie hinter der Schrift weiter (dann bliebe die K5c-Länge)?
8. Vorn links, Schleife 2: Kralle 14 (mein Vorschlag, weil die linke Hand genau 4 Krallenteile hat) oder die Handspitze 13? Und Schleife 1: Kralle 30 ist im Modell fast unsichtbar (2 Pixel); ist sie trotzdem die Kralle dieser Schleife?
9. Kralle 28 (vorn links, Schleife 4): Zurückgeholt in ihrer früheren Form liegt Kralle 28 nur zu 2 Prozent in Ihrer Schleife, der größte Teil (312 Video-Pixel) läge über Ihrem "Fußabschnitt", und 2 Ecken stecken in der Hand. Soll 28 so zurückkommen, oder soll ich sie an Ihre Schleife 4 anpassen?
10. Krallenmarken hinten links: Ihre zwei roten Marken liegen rechts neben dem Modellfuß über Hintergrund. Sind die Krallen die drei Zehenspitzen des Fußes (mein Vorschlag), oder ragen sie weiter heraus?
11. Zweiter grüner Strich (linkes Hinterbein, ohne Text): Er zeigt auf denselben weißen Fleck wie "weiße Musterung". Richtig, oder meinten Sie eine weitere Stelle?
12. Bräunliche Musterung: Am rechten Hinterbein ist im Bild nichts Braunes messbar (L* 63,5, a* 7,7, b* -20,7, also graublau); meinten Sie die helle Fläche direkt unter dem weißen Fleck, entlang Ihres zweiten dunkelroten Strichs? Am linken Hinterbein ist die Stelle dunkel rot-violett (L* 27, a* 26, b* -6): ist das die bräunliche Musterung?
13. Rotes Fleisch am Zipfel: Von den 4 Stellen an Ihren Linienenden sind 2 violett-rot, 1 rot und 1 nur schwach rötlich; die erste ist nur 1 Pixel groß. Sind alle vier "rotes Fleisch"?
14. Alles, was in keinem der zwei Bilder zu sehen ist (Rückseiten, Unterseiten, Hals-Unterseite, rechter Hinterfuß): bleibt das Haut ohne eigene Farbe?

## Dateien

| Datei | Inhalt |
|---|---|
| `zonen_v2.json` | meta, 61 Zonen (Nutzerwort, Stelle, Material, Flächen bzw. Texel-Maske, geteilte Texel, Sicherheit, Änderung), augen, kopf, krallen.spitzen |
| `krallen_kandidat/` | BAHNSTEIG_FORM.EMD/.TIM, form.json, kandidat.json, krallen_masken_bild2.npz |
| `tafeln/` | 00_uebersicht, 01-11 je Nutzerbild (mit Nahansichten der Flecken), 20_rundum, 21_krallen, 22_kopf_augen, 23_geteilte_texel, LIESMICH.txt |
| `skripte/` | g_basis, g_bau, g_fleck, g_spec, g_zonen, g_texel, g_augen_kopf, g_krallen, g_krallen_spitzen, g_meta, g_ausgabe, g_tafel_basis, g_tafeln, g_tafeln2, g_messblatt |

Ablauf (Arbeitsordner GRUNDLAGE_ARBEIT mit asset/, k5c/, anno_*.npz, versatz.json, abbildung.json aus dem v1-Lauf): g_zonen -> g_texel -> g_augen_kopf -> g_krallen -> g_krallen_spitzen -> g_ausgabe -> g_tafeln -> g_tafeln2 -> g_messblatt. Python ohne -I.
