# Messblatt Zonen v2: erkannte Grundlage zur Abnahme (Runde 41, Riesenform)

Stand 2026-11-09, Korrektor Runde 1 von 2. Nur Messung: kein Asset, keine Textur, keine Farbwahl. Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` (md5 6e00f105..., K5e_f1) unverändert. Grundlage v1: `../MESSBLATT_ZONEN.md` (Commit e53cef212), Prüfung `../PRUEFUNG_ZONEN.md` (Commit d62ae97fa).

## Ergebnis

- **62 Zonen** aus allen 11 beschrifteten Nutzerbildern (jede Beschriftung, auch der zweite grüne Strich), dazu die Abschnitte, je Kralle eine Zone und 5 Kopfteile nach Modellbedeutung (Z4). Sicherheit: sicher 30, unsicher 25, verdeckt 7.
- **Augen (Z2):** Beide Nutzerpunkte gemessen (hell neben dem Linienende). Rechtes Auge (Wesen): Punkt 5.1 px neben dem Linienende, heutige G4-Augenfläche 4.6 Video-px daneben und überlappend. Linkes Auge: Punkt 2.2 px neben dem Linienende, heutige Augenfläche 7.7 Video-px daneben (nach innen/oben), keine Überlappung. Vorschlag: Flächen unter den Punkten = Auge (22|0|82, 22|1|112, 22|1|113).
- **Krallen:** vorn links 28 zurück (K5c-Form; Spitze 8,3 px neben Schleife 4). Vorn rechts 19 mit Faktor x0.4898 (Rest 16.0 px); 20 und 29 durch Drehen **nicht** versteckbar (Frage). Hinten unverändert; Vorschlag Krallen = 3 Zehenspitzen.
- **Kopf (Bild 2):** Modell minus Nutzerumriss oben -69.6, unten -6.0, links +4.5, rechts -2.5 Video-px. Video-Oberkante nicht messbar (keine Helligkeitskante). Augenhöhe passt (Modell 4.1 px höher), Augenabstand Modell 54.6 gegen Video 62.8.

## Verfahren

- Ausrichtung wie v1 (Faktor 2, keine Drehung/Spiegelung; `abbildung.json`). Kennungs-Render **hochaufgelöst in Grundlage-Koordinaten** (Kameramatrix verkettet mit Vorlage -> Grundlage; Gegenprobe Silhouette IoU 0,9956 Bild 1, 0,978 Bild 2 - Rest = Modellteile links außerhalb des Grundlage-Rahmens). Pixelzahlen im Vorlage-Maßstab (Grundlage-px x s²).
- Umrisse: Bauarten flut/huelle/loecher/band unverändert aus v1 (`skripte/g_bau.py` = Kopie von `zonen_bau.py`).
- **Stellen neben Linienenden** (Memory reai-v2-beschriftungslinien-messen): Linienende per Skelett; dann `fleck` = Zusammenhangskomponente einer Klasse (weiß L* >= 70 und C* <= 30; Auge L* >= Median(21 px) + 12) nahe dem Ende, oder `wachsen` = Farbgebiet ab einer Saat 4-6 px hinter dem Ende in Linienrichtung (Delta E <= 14-20). Keine Kreise mehr.
- **Zuordnung:** Teile-Zonen nur Flächen der benannten Teile, Fläche >= 50 % in der Zone oder >= 15 % der Modellpixel der Zone. Stellen: Kern der Stelle (1 px abgetragen), je Stelle Flächen mit >= 10 % des Kerns; Material nur an der Stelle, nicht auf der ganzen Fläche. Kopf Bild 2 (Z4): nach Modellbedeutung (`ruhe/messung/material.json`: Auge, Maulinneres, Zahn, Lippensaum; Unterkiefer außen = Teil 23 ohne Maulinneres/Zahn; Hals-Unterseite = Teil 21 mit Normalen nach unten >= 0,5). Krallen: je Nutzerschleife das Krallenteil mit der nächsten Spitze (K5c, Pose Bild 2).
- **IoU** = zugeordnete Flächen (sichtbare Pixel) gegen die eigene Umrandung. Bei Stellen ist sie klein, weil eine Fläche größer ist als der Fleck.
- Sicherheit: sicher = Umriss/Stelle gemessen und Modell trifft das benannte Teil; unsicher = Nutzer selbst unsicher, Annahme, oder Modell weicht ab; verdeckt = an der Stelle im Modell nichts Passendes sichtbar.

## Zonen (je Zone eine Zeile)

| Nr | Zone | Bild | Nutzerwort | Material laut Nutzerwort | Modellteile (Flächen) | Zone px | IoU | Sicherheit | Grund | Änderung ggü. v1 |
|---|---|---|---|---|---|---|---|---|---|---|
| 1 | oberkoerper_hinten | 1 | "Oberkörper" | Haut (Teil benannt, kein Farbwort) | 0 Hüfte (19), 7 Rücken/Wirbel (17), 10 Brust/Buckel (9) | 6501 | 0.76 | sicher | Umriss deckt das Modell zu 99 %; Rumpfteile Hüfte, Rücken, Brust | – |
| 2 | wunde_hinten | 1 | "fleischige Wunde/Haut" | fleischige Wunde/Haut | 0 Hüfte (3) | 603 | 0.38 | sicher | Umriss liegt ganz auf der Hüfte; die Wunde bedeckt nur einen Teil der 3 Flächen (Stelle, nicht ganze Fläche) | – |
| 3 | musterung_rechts_hinten | 1 | "Oberkörper Musterung rechts" | Musterung | 0 Hüfte (1) | 229 | 0.26 | sicher | Umriss liegt ganz auf der Hüfte; nur Teil einer Fläche | – |
| 4 | musterung_links_hinten | 1 | "Oberkörper Musterung links" | Musterung | 0 Hüfte (1), 2 Schwanz 2 (2), 3 Schwanz 3 (1) | 54 | 0.16 | unsicher | liegt am Rand der Figur; das Modell deckt nur 36 % der Stelle | – |
| 5 | kopf_hinten | 1 | "Kopf" | Haut (Teil benannt, kein Farbwort) | 22 Kopf (Oberkiefer) (400), 23 Unterkiefer (126); sichtbar 6 | 743 | 0.00 | verdeckt | an dieser Stelle zeigt das Modell den rechten Oberarm (666 px); der Modellkopf ist in Bild 1 nicht zu sehen | v1 ordnete die Flächen von Oberarm 16 zu; v2: nichts nach Bildlage, Kopf nur nach Bedeutung (Teile 22/23) |
| 6 | auge_hinten | 1 | "wahrscheinlich etwas Auge" | Auge | – | 21 | 0.00 | verdeckt | Nutzer: "wahrscheinlich"; Modellkopf an dieser Stelle verdeckt (0 px) | – |
| 7 | mund_hinten | 1 | "Mund?" | Maul innen | – | 35 | 0.00 | verdeckt | Nutzer: "Mund?"; Modellkopf an dieser Stelle verdeckt (5 px Oberarm) | v1 nannte 5 px Oberarm 16; v2 ordnet nichts zu |
| 8 | schwanz | 1 | "Schwanz der in den Rücken übergeht" | Haut (Teil benannt, kein Farbwort) | 0 Hüfte (10), 1 Schwanz 1 (30), 2 Schwanz 2 (25), 3 Schwanz 3 (32), 4 Schwanz 4 (32), 5 Schwanz 5 (23) | 11196 | 0.87 | sicher | Umriss deckt das Modell zu 98 %; Schwanzglieder 1-5 | – |
| 9 | schwanz_muster | 1 | "Schwanz Muster / Maserung" | Musterung/Maserung | 3 Schwanz 3 (4), 4 Schwanz 4 (4), 5 Schwanz 5 (3) | 1039 | 0.50 | sicher | 4 grüne Umrisse auf den Schwanzgliedern 3-5 | – |
| 10 | zipfel | 1 | "Zipfel - separater Schwanzabschnitt am Schluss" | Haut (Teil benannt, kein Farbwort) | 6 Schwanzspitze (Zipfel) (12) | 3032 | 0.83 | sicher | Umriss deckt die Schwanzspitze; unten endet das Nutzerbild am Schriftzug | – |
| 11 | zipfel_musterung | 1 | "Schwanz Musterung" | Musterung | 5 Schwanz 5 (1), 6 Schwanzspitze (Zipfel) (3) | 1094 | 0.68 | sicher | 2 orange Umrisse auf der Schwanzspitze | – |
| 12 | rotes_fleisch_zipfel | 1 | "verschieden farbiges rotes Fleisch wie oben am Becken" | rotes Fleisch | 5 Schwanz 5 (1), 6 Schwanzspitze (Zipfel) (6) | 226 | 0.14 | unsicher | 4 Linienenden; je Ende die rötliche Stelle in Linienrichtung (Farbgebiet), nicht vom Nutzer umrandet | v1: Kreise auf den Linienenden; v2: rötliches Farbgebiet neben jedem Ende |
| 13 | bein_hinten_links | 1 | "linkes Bein" | Haut (Teil benannt, kein Farbwort) | 8 Oberschenkel hinten links (36), 24 Unterschenkel hinten links (23), 25 Fuß hinten links (4) | 3313 | 0.77 | sicher | Umriss deckt das Modell zu 92 %; Innenkante vom Nutzer nicht gezogen (Sehne gesetzt) | – |
| 13.1 | bein_hinten_links_abschnitt1 | 1 | "linkes Bein, Bein Abschnitte. unten direkt verbunden mit den fuß (Abschnitt 1 von oben)" | Gliederung (kein Material) | 8 Oberschenkel hinten links (8) | 556 | 0.29 | unsicher | Modellbein beginnt tiefer: im obersten Abschnitt liegen auch Hüfte (183 px) und Schwanz (123 px) | – |
| 13.2 | bein_hinten_links_abschnitt2 | 1 | "linkes Bein, Bein Abschnitte. unten direkt verbunden mit den fuß (Abschnitt 2 von oben)" | Gliederung (kein Material) | 8 Oberschenkel hinten links (8), 24 Unterschenkel hinten links (1) | 783 | 0.83 | sicher | Abschnitt zwischen lila Trennlinien; Teile des benannten Beins zugeordnet | – |
| 13.3 | bein_hinten_links_abschnitt3 | 1 | "linkes Bein, Bein Abschnitte. unten direkt verbunden mit den fuß (Abschnitt 3 von oben)" | Gliederung (kein Material) | 8 Oberschenkel hinten links (18), 24 Unterschenkel hinten links (9) | 1051 | 0.74 | sicher | Abschnitt zwischen lila Trennlinien; Teile des benannten Beins zugeordnet | – |
| 13.4 | bein_hinten_links_abschnitt4 | 1 | "linkes Bein, Bein Abschnitte. unten direkt verbunden mit den fuß (Abschnitt 4 von oben)" | Gliederung (kein Material) | 24 Unterschenkel hinten links (12), 25 Fuß hinten links (4) | 802 | 0.69 | sicher | Abschnitt zwischen lila Trennlinien; Teile des benannten Beins zugeordnet | – |
| 14 | weisse_musterung_bein_hinten_links | 1 | "weiße Musterung" | weiße Musterung | 8 Oberschenkel hinten links (1) | 25 | 0.22 | sicher | weißer Fleck 5 px rechts neben dem Linienende (L* 76 gegen 47 daneben), Kern auf Oberschenkel 8 | Z1: v1-Kreis lag auf dunkler Hüfte; jetzt der weiße Fleck auf dem Oberschenkel |
| 15 | weisse_musterung_bein_hinten_links_strich2 | 1 | "weiße Musterung (zweiter grüner Strich, ohne eigenen Text)" | weiße Musterung | 8 Oberschenkel hinten links (1) | 22 | 0.19 | unsicher | zweiter grüner Strich ohne eigenen Text; an seinem Ende (11,7 px) liegt derselbe weiße Fleck wie Nr. 14 | Z5: neu; zweiter Zeiger auf denselben Fleck |
| 16 | braeunliche_musterung_bein_hinten_links | 1 | "bräunliche Musterung" | bräunliche Musterung | 8 Oberschenkel hinten links (2) | 76 | 0.16 | sicher | rötlich-braunes Farbgebiet direkt rechts vom Linienende (a* 27 gegen 19 daneben), Oberschenkel 8 | v1: Kreis auf dem Linienende; v2: Farbgebiet neben dem Ende |
| 17 | bein_hinten_rechts | 1 | "rechtes Bein hinten" | Haut (Teil benannt, kein Farbwort) | 9 Oberschenkel hinten rechts (31), 26 Unterschenkel hinten rechts (36), 27 Fuß hinten rechts (2) | 5437 | 0.73 | sicher | Umriss deckt das Modell zu 95 % | – |
| 17.1 | bein_hinten_rechts_abschnitt1 | 1 | "rechtes Bein, Beinabschnitte. Unten direkt verbunden mit fuß. (Abschnitt 1 von oben)" | Gliederung (kein Material) | 9 Oberschenkel hinten rechts (23), 26 Unterschenkel hinten rechts (2) | 3312 | 0.66 | unsicher | im obersten Abschnitt liegt im Modell auch die Hüfte (938 px) | – |
| 17.2 | bein_hinten_rechts_abschnitt2 | 1 | "rechtes Bein, Beinabschnitte. Unten direkt verbunden mit fuß. (Abschnitt 2 von oben)" | Gliederung (kein Material) | 9 Oberschenkel hinten rechts (7), 26 Unterschenkel hinten rechts (19) | 1213 | 0.80 | sicher | Abschnitt zwischen lila Trennlinien; Teile des benannten Beins zugeordnet | – |
| 17.3 | bein_hinten_rechts_abschnitt3 | 1 | "rechtes Bein, Beinabschnitte. Unten direkt verbunden mit fuß. (Abschnitt 3 von oben)" | Gliederung (kein Material) | 26 Unterschenkel hinten rechts (13), 27 Fuß hinten rechts (2) | 741 | 0.67 | sicher | Abschnitt zwischen lila Trennlinien; Teile des benannten Beins zugeordnet | – |
| 18 | weisse_musterung_bein_hinten_rechts | 1 | "weiße Musterung" | weiße Musterung | 0 Hüfte (2), 9 Oberschenkel hinten rechts (1) | 107 | 0.23 | sicher | weißer Fleck am Linienende (L* 90), Kern auf Hüfte 0 und Oberschenkel 9 | Z3: ganzer Fleck statt Kreis; Oberschenkel 9 (Fläche 9|1|19) dazu |
| 19 | braeunliche_musterung_bein_hinten_rechts | 1 | "bräunliche Musterung" | bräunliche Musterung | 9 Oberschenkel hinten rechts (1) | 51 | 0.21 | unsicher | im Grundbild nicht braun messbar (L* 66, a* -4, b* -8, also graublau); gemeint vermutlich die helle Fläche links vom Linienende unter dem zweiten dunkelroten Strich | v1: Band um den Strich; v2: Farbgebiet neben dem Linienende |
| 20 | fuss_hinten_links | 1 | "linker Fuß mit Teil der Fußkralle vorne. Kaum sichtbar, aber wahrscheinlich 3 Krallen. Rechts wäre es sicher genauso" | Haut (Teil benannt, kein Farbwort) | 25 Fuß hinten links (16) | 1284 | 0.23 | unsicher | Modellfuß deckt nur 26 % der Nutzer-Hülle; die Krallenmarken liegen neben dem Modellfuß | – |
| 21 | fusskrallen_hinten_links | 1 | "linker Fuß mit Teil der Fußkralle vorne. Kaum sichtbar, aber wahrscheinlich 3 Krallen. Rechts wäre es sicher genauso" | Kralle | 25 Fuß hinten links (30); sichtbar 4 | 15 | 1.00 | unsicher | Nutzermarken liegen über Hintergrund rechts neben dem Modellfuß (0 px auf Fuß 25); Vorschlag nach Bedeutung: die 3 Zehenspitzen von Fuß 25 | v1: Marken auf Schwanz 5; v2: Zehenspitzen von Fuß 25 nach Bedeutung |
| 22 | fusskrallen_hinten_rechts | 1 | "Rechts wäre es sicher genauso (aus demselben Text)" | Kralle | 27 Fuß hinten rechts (30); sichtbar 2 | 1 | 1.00 | verdeckt | nur Textangabe "Rechts wäre es sicher genauso"; Fuß 27 in keinem Bild beschriftet | neu |
| 23 | vorderbein_rechts | 1 | "rechtes Bein und Fuß" | Haut (Teil benannt, kein Farbwort) | 16 Oberarm vorn rechts (13), 17 Unterarm vorn rechts (32), 18 Hand vorn rechts (25) | 3418 | 0.85 | sicher | Umriss deckt das Modell zu 97 % | – |
| 23.1 | vorderbein_rechts_abschnitt1 | 1 | "wahrscheinliche Bein Abschnitte (Abschnitt 1 von oben)" | Gliederung (kein Material) | 16 Oberarm vorn rechts (13), 17 Unterarm vorn rechts (9) | 1413 | 0.77 | unsicher | Nutzer: "wahrscheinliche Bein Abschnitte"; Abschnitt zwischen lila Trennlinien | – |
| 23.2 | vorderbein_rechts_abschnitt2 | 1 | "wahrscheinliche Bein Abschnitte (Abschnitt 2 von oben)" | Gliederung (kein Material) | 17 Unterarm vorn rechts (6) | 614 | 0.60 | unsicher | Nutzer: "wahrscheinliche Bein Abschnitte"; Abschnitt zwischen lila Trennlinien | – |
| 23.3 | vorderbein_rechts_abschnitt3 | 1 | "wahrscheinliche Bein Abschnitte (Abschnitt 3 von oben)" | Gliederung (kein Material) | 17 Unterarm vorn rechts (10), 18 Hand vorn rechts (7) | 625 | 0.58 | unsicher | Nutzer: "wahrscheinliche Bein Abschnitte"; Abschnitt zwischen lila Trennlinien | – |
| 23.4 | vorderbein_rechts_abschnitt4 | 1 | "wahrscheinliche Bein Abschnitte (Abschnitt 4 von oben)" | Gliederung (kein Material) | 17 Unterarm vorn rechts (3), 18 Hand vorn rechts (17) | 605 | 0.77 | unsicher | Nutzer: "wahrscheinliche Bein Abschnitte"; Abschnitt zwischen lila Trennlinien | – |
| 24 | weisse_musterung_vorderbein_rechts | 1 | "weiße Musterung" | weiße Musterung | 16 Oberarm vorn rechts (2) | 250 | 0.62 | sicher | grüner Umriss auf Oberarm 16 | – |
| 25 | fuss_kralle_vorderbein_rechts | 1 | "Fuß und Kralle" | Kralle | 18 Hand vorn rechts (4) | 149 | 0.23 | unsicher | in Bild 1 zeigt das Modell dort nur die Hand (95 px), keine Kralle | – |
| 26 | oberkoerper_vorn | 2 | "Oberkörper" | Haut (Teil benannt, kein Farbwort) | 10 Brust/Buckel (44), 21 Hals (20) | 18443 | 0.59 | unsicher | der Modellkopf reicht 70 px in diesen Umriss (4966 px); ohne Kopf gezählt | Z4: Kopfflächen herausgenommen |
| 27 | fleisch_vorn | 2 | "großer fleischfarbener Teil" | fleischfarben | 10 Brust/Buckel (30), 21 Hals (20) | 14762 | 0.60 | unsicher | der Modellkopf deckt 4812 px dieses Umrisses; zugeordnet nur Brust 10 und Hals 21, dazu 7 Flächen hinter dem Modellkopf | Z4: ohne Kopf 22 und Unterkiefer 23 |
| 28 | gesicht_vorn | 2 | "Vorderseite Gesicht mit Maul nach unten rechts blickend" | Haut (Teil benannt, kein Farbwort) | 22 Kopf (Oberkiefer) (400), 23 Unterkiefer (126); sichtbar 163 | 13161 | 0.68 | unsicher | Modellkopf reicht 70 px höher als Ihr Umriss; Zuordnung nach Bedeutung: ganzer Kopf 22 und Unterkiefer 23 | Z4: nach Bedeutung statt nach Bildlage |
| 29 | augen_vorn | 2 | "Augen" | Auge | 22 Kopf (Oberkiefer) (3) | 19 | 0.14 | sicher | zwei helle Punkte 5,1 bzw. 2,2 px neben den Linienenden (L* 30 gegen 5-7); Flächen genau unter den Punkten | Z2: gemessene Punkte statt Kreise; Hüfte 0 entfällt |
| 30 | maul_vorn | 2 | "geöffnetes Maul" | Maul innen | 22 Kopf (Oberkiefer) (102), 23 Unterkiefer (47); sichtbar 28 | 2316 | 0.16 | unsicher | Ihr Maul-Umriss zeigt im Modell vor allem die Außenseite des Unterkiefers (1445 px); Maulinneres und Zähne nach Bedeutung | Z4: nach Bedeutung (Maulinneres + Zähne) |
| 31 | unterkiefer_vorn | 2 | "Teil des Unterkiefers" | Unterkiefer (kein Farbwort) | 23 Unterkiefer (79); sichtbar 29 | 304 | 0.05 | unsicher | Band am lila Strich; dort zeigt das Modell Unterkiefer (89 px) und Unterschenkel (48 px); nach Bedeutung: Unterkiefer außen | Z4: nach Bedeutung |
| 32 | arm_hand_krallen_vorn_rechts | 2 | "vorderer rechter Arm mit Krallen" | Haut (Teil benannt, kein Farbwort) | 18 Hand vorn rechts (14), 31 Kralle vorn rechts 31 (6) | 4254 | 0.74 | sicher | Umriss deckt das Modell zu 87 % | – |
| 33 | arm_vorn_rechts | 2 | "vorderer rechter Arm" | Haut (Teil benannt, kein Farbwort) | 18 Hand vorn rechts (3) | 359 | 0.16 | sicher | kleiner Abschnitt oben am Arm; Hand 18 | – |
| 34 | hand_vorn_rechts | 2 | "vordere rechte Hand mit Krallen" | Haut (Teil benannt, kein Farbwort) | 18 Hand vorn rechts (8) | 2747 | 0.73 | sicher | Umriss deckt Hand 18 zu 100 % | – |
| 35 | krallen_vorn_rechts | 2 | "Krallen" | Kralle | 31 Kralle vorn rechts 31 (6) | 1147 | 0.15 | unsicher | 2 Krallenformen gezeichnet (Kralle 31 und 19), dazwischen der Schriftzug; im Asset ist 19 eingezogen | v1: eine Zone; v2: Form 1, Form 3 und Fläche dazwischen getrennt |
| 35.1 | krallen_vorn_rechts_form1 | 2 | "Krallen (Form 1, rechts im Bild)" | Kralle | 31 Kralle vorn rechts 31 (30) | 291 | – | sicher | Form deckt Kralle 31 (Asset sichtbar); unteres Ende 17.5 px neben der Spitze von 31 | neu (eigene Zone) |
| 35.2 | krallen_vorn_rechts_form3 | 2 | "Krallen (Form 3, links im Bild)" | Kralle | 19 Kralle vorn rechts 19 (30) | 61 | – | unsicher | Form liegt auf Kralle 19 in K5c-Form; deren Spitze liegt 42 px tiefer als Ihr Strichende; im Asset ist 19 eingezogen | neu (eigene Zone) |
| 35.3 | krallen_vorn_rechts_dazwischen | 2 | "Krallen (Fläche zwischen den Formen, mit Schriftzug)" | Kralle | 20 Kralle vorn rechts 20 (30), 29 Kralle vorn rechts 29 (30) | 803 | – | verdeckt | Annahme nach Ihrem Wort "wahrscheinlich auch 4 ... wegen der Schrift": Krallen 20 und 29 liegen hier verdeckt | neu (eigene Zone) |
| 36 | vorderbein_links | 2 | "Vorderes linkes Bein mit Krallen" | Haut (Teil benannt, kein Farbwort) | 11 Oberarm vorn links (15), 12 Unterarm vorn links (22), 13 Hand vorn links (21), 14 Kralle vorn links 14 (8), 15 Kralle vorn links 15 (6) | 6736 | 0.74 | sicher | Umriss deckt das Modell zu 96 % | – |
| 37 | beinabschnitt_vorn_links | 2 | "Beinabschnitt" | Haut (Teil benannt, kein Farbwort) | 11 Oberarm vorn links (15), 12 Unterarm vorn links (20) | 4359 | 0.68 | sicher | Oberarm 11 und Unterarm 12 | – |
| 38 | fussabschnitt_vorn_links | 2 | "Fußabschnitt" | Haut (Teil benannt, kein Farbwort) | 13 Hand vorn links (17), 14 Kralle vorn links 14 (8), 15 Kralle vorn links 15 (6) | 2377 | 0.84 | sicher | Hand 13 mit Krallen | – |
| 39 | krallen_vorn_links | 2 | "Exakt 4 Krallen" | Kralle | 13 Hand vorn links (4), 15 Kralle vorn links 15 (6), 30 Kralle vorn links 30 (2) | 460 | 0.49 | sicher | 4 geschlossene Schleifen = "Exakt 4" | v2: je Schleife eine eigene Zone |
| 39.1 | kralle_vorn_links_schleife1 | 2 | "Exakt 4 Krallen (Schleife 1 von links im Bild)" | Kralle | 30 Kralle vorn links 30 (30) | 32 | – | sicher | Spitze 6,8 px neben Kralle 30 | neu (eigene Zone) |
| 39.2 | kralle_vorn_links_schleife2 | 2 | "Exakt 4 Krallen (Schleife 2 von links im Bild)" | Kralle | 14 Kralle vorn links 14 (30) | 118 | – | unsicher | Spitze 2,2 px neben der Handspitze 13 und 6,4 px neben Kralle 14; Vorschlag Kralle 14 (die linke Hand hat genau 4 Krallenteile: 14, 15, 28, 30) | neu (eigene Zone) |
| 39.3 | kralle_vorn_links_schleife3 | 2 | "Exakt 4 Krallen (Schleife 3 von links im Bild)" | Kralle | 15 Kralle vorn links 15 (42) | 250 | – | sicher | Spitze 3,0 px neben Kralle 15 | neu (eigene Zone) |
| 39.4 | kralle_vorn_links_schleife4 | 2 | "Exakt 4 Krallen (Schleife 4 von links im Bild)" | Kralle | 28 Kralle vorn links 28 (30) | 60 | – | verdeckt | Spitze 8,3 px neben Kralle 28 in K5c-Form; im heutigen Asset ist 28 eingezogen (0 px) | neu (eigene Zone) |
| 40 | maulinneres | 2 | "Maulinneres (im "geöffneten Maul")" | Maul innen | 22 Kopf (Oberkiefer) (24), 23 Unterkiefer (11); sichtbar 2 | 23 | 1.00 | unsicher | in dieser Pose fast unsichtbar (23 px); keine Fläche liegt in Ihrem Maul-Umriss | neu (Z4, Bedeutung) |
| 41 | zaehne | 2 | "Zähne (im "geöffneten Maul"; das Wort Zahn steht nicht im Bild)" | Zahn | 22 Kopf (Oberkiefer) (78), 23 Unterkiefer (36); sichtbar 26 | 453 | 1.00 | unsicher | 391 von 453 px liegen in Ihrem Maul-Umriss | neu (Z4, Bedeutung) |
| 42 | lippe | 2 | "Oberlippe (Rand des "geöffneten Mauls", Kopf 22)" | Lippe (kein Farbwort) | 22 Kopf (Oberkiefer) (78); sichtbar 25 | 2596 | 1.00 | unsicher | Oberlippe des Kopfes 22; nur 297 px in Ihrem Maul-Umriss | neu (Z4, Bedeutung) |
| 43 | unterkiefer_aussen | 2 | "Unterkiefer außen mit Unterlippe ("Teil des Unterkiefers")" | Unterkiefer (kein Farbwort) | 23 Unterkiefer (79); sichtbar 29 | 1729 | 1.00 | unsicher | Unterkiefer 23 ohne Maulinneres und Zähne; nur 89 px in Ihrem Unterkiefer-Band | neu (Z4, Bedeutung) |
| 44 | hals_unterseite | 2 | "Hals-Unterseite (kein Nutzerwort)" | Haut (nicht belegt) | 21 Hals (29); sichtbar 4 | 524 | 1.00 | verdeckt | kein Nutzerwort; Flächen von Hals 21 mit Normalen nach unten; bleibt Haut (nicht belegt) | neu (Z4, Bedeutung) |

## Änderungen gegenüber v1 (mit Grund)

| Punkt | v1 | v2 | Grund / Beleg |
|---|---|---|---|
| Z1 weiße Musterung Hinterbein l | Kreis r 8 auf dem Linienende 352/209, Hüfte 0 (33 px) | weißer Fleck 5,0 px rechts neben dem Ende (L* 76 gegen 47), Kern auf Oberschenkel 8 (Fläche 8|1|20) | Prüfung IoU 0,06; Fleck selbst gemessen |
| Z2 Augen | 2 Kreise r 5, 93 % Haut, Hüfte 0 dabei | 2 gemessene Punkte (5,1 / 2,2 px neben den Enden), Flächen 22|0|82, 22|1|112, 22|1|113 | Nutzer: Augen-Markierung gilt |
| Z3 weiße Musterung Hinterbein r | Kreis, 49 px, nur Hüfte | ganzer Fleck 107 px, Hüfte 0 (0|1|135, 0|1|136) + Oberschenkel 9 (9|1|19) | Prüfung: Kreis deckte 39 %; Kern-Regel: nur 9|1|19 trägt Kernpixel, die zwei weiteren 9er-Flächen nur 2 Randpixel |
| Z4 Kopf Bild 2 | Flächen nach Bildlage (fleisch_vorn mit 4799 px Kopf 22) | fleisch/Oberkörper ohne 22/23 (+7 Flächen hinter dem Modellkopf), Gesicht/Maul/Unterkiefer nach Modellbedeutung, 5 Modellteile (Nr 40-44) | Modellkopf 70 px zu hoch |
| Z5 zweiter grüner Strich | nicht verwendet | Zone 15: zeigt auf denselben weißen Fleck (11,7 px) | Frage an den Nutzer |
| Z6 Kopf hinten / Auge / Mund | Oberarm 16 als Kopf | verdeckt, Kopf nur nach Bedeutung (22/23) | Modellkopf in Bild 1 nicht sichtbar |
| Z6 rotes Fleisch Zipfel | 4 Kreise | 4 rötliche Farbgebiete in Linienrichtung | Stelle neben dem Ende |
| Z6 bräunliche Musterung l/r | Kreis bzw. Band um den Strich | Farbgebiet neben dem Ende (l: a* 27; r: graublau, unsicher) | Stelle neben dem Ende |
| Z6 Fußkrallen hinten | Marken auf Schwanz 5 | 3 Zehenspitzen von Fuß 25 (und 27 nach "Rechts wäre es sicher genauso") | Marken liegen über Hintergrund (0 px auf Fuß 25) |
| Z6 Krallen vorn | je Hand eine Zone | je Nutzerschleife/-form eine Zone mit Krallenteil | Prüfung: rechts 2 Formen, nicht 3 Spitzen |
| Z6 Abschnitte, Schwanz, Wunde, Musterung, Hand-r, Ganzteil, Namensteile | ungeprüft | selbst geprüft; Ganzteil = benannte Teile ganz (Rest "Haut (nicht belegt)"); Abschnitt 1 der Hinterbeine unsicher (Hüfte im Umriss) | Tafeln 01-11 |

## Augen (Z2)

| Auge (Wesen) | Linienende (beschriftet) | Punkt Grundlage | Punkt Video | Abstand zum Ende | Flächen unter dem Punkt (px, heute Auge?) | heutige Augenfläche: Abstand Mitte (Video-px), Überlappung |
|---|---|---|---|---|---|---|
| Auge rechts (vom Wesen aus; links im Bild) | [441, 245] | [47.8, 247.3] | [217.4, 274.3] | 5.1 | 22|0|82 13 ja, 22|1|152 7 nein, 22|0|81 5 ja, 22|0|83 1 ja | 4.6, 19 px |
| Auge links (vom Wesen aus; rechts im Bild) | [568, 248] | [173.4, 248.9] | [280.2, 275.1] | 2.2 | 22|1|112 26 nein, 22|1|113 23 nein | 7.7, 0 px |

Heutige G4-Augen: 64 Flächen, 20 in Bild 2 sichtbar (143 Video-px). Vorschlag (Kern-Regel): 22|0|82, 22|1|112, 22|1|113; davon heute schon Auge: 22|0|82.

## Krallen-Kandidat (nur `krallen_kandidat/`, nicht im Asset)

- `krallen_kandidat/BAHNSTEIG_FORM.EMD` = Spiel-Asset mit ersetzten Ecken/Normalen der Teile [19, 20, 28, 29] (954 Bytes verschieden, Länge gleich); TIM unverändert kopiert.
- 28: K5c-Ecken. 19: K5c-Ecken, gestreckt entlang Wurzel->Spitze mit Faktor 0.4898; Spitze K5c [203.3, 499.1] -> [218.1, 462.5], Nutzerstrichende [202.96, 457.33], Rest 16.0 px.
- 20: nur um die Wurzel gedreht (111.2 Grad, Achse [-0.224, -0.034, 0.974]), Länge 751.3 -> 751.2. K5c: 1734 px sichtbar, 1686 über Hintergrund. Vorschlag: 135 sichtbar, 0 im Schriftzug, 135 über Hintergrund, 2 von 21 Ecken in der Hand. Suche: 6000 Drehungen, keine mit 0 px außerhalb des Schriftzugs; ohne Durchdringung bestenfalls 558 px. **Nicht gelungen.**
- 29: nur um die Wurzel gedreht (146.9 Grad, Achse [-0.268, -0.197, 0.943]), Länge 961.3 -> 961.3. K5c: 1782 px sichtbar, 1782 über Hintergrund. Vorschlag: 12 sichtbar, 0 im Schriftzug, 12 über Hintergrund, 1 von 20 Ecken in der Hand. Suche: 6000 Drehungen, keine mit 0 px außerhalb des Schriftzugs; ohne Durchdringung bestenfalls 95 px. **Nicht gelungen.**
- Gegenprobe "nur kein Pixel über Hintergrund" (Krallen dürften über der Figur liegen): gleiches Ergebnis (alle Pixel außerhalb des Schriftzugs lagen über Hintergrund).
- Bild 1 mit dem Vorschlag: 20 330 px / 29 683 px über Hintergrund (K5c: 0 / 10).
- Schriftzug-Fläche: helle Glyphen (L* >= 75, C* <= 30) im Textblock x 0-261, y 434-479 der Vorlage, 3 px verdickt, 9 px geschlossen (9192 px). Hintergrund = außerhalb der Nutzer-Freistellung und außerhalb des Schriftzugs.

| Stand | 14 | 15 | 28 | 30 | 19 | 20 | 29 | 31 |
|---|---|---|---|---|---|---|---|---|
| asset (sichtbar / über Hintergrund) | 326 / 17 | 699 / 25 | 0 / 0 | 2 / 0 | 0 / 0 | 0 / 0 | 0 / 0 | 419 / 126 |
| k5c (sichtbar / über Hintergrund) | 326 / 17 | 699 / 25 | 312 / 12 | 2 / 0 | 1521 / 161 | 1734 / 1686 | 1782 / 1782 | 419 / 126 |
| vorschlag (sichtbar / über Hintergrund) | 326 / 17 | 699 / 25 | 312 / 12 | 2 / 0 | 838 / 28 | 135 / 135 | 12 / 12 | 419 / 126 |

Spitzen (Video-px, selbst gemessen): Schleife 1 -> 30 (30 6.8, 13 46.7); Schleife 2 -> 13 (13 2.2, 14 6.4); Schleife 3 -> 15 (15 3.0, 28 24.2); Schleife 4 -> 28 (28 8.3, 15 21.4); Form 1 unteres Ende -> 31 17.5 px; Form 3 unteres Ende -> 19 (K5c) 48.4 px.

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
3. Maul (Bild 2): An der Stelle Ihres "geöffneten Mauls" zeigt das Modell vor allem die Außenseite des Unterkiefers; das Maulinnere ist in dieser Pose fast unsichtbar (23 px). Ich ordne "geöffnetes Maul" dem Maulinneren und den Zähnen zu. Richtig?
4. Krallen vorn rechts: Ihre Annahme "wahrscheinlich auch 4" lässt sich mit bloßem Drehen von Kralle 20 und 29 nicht erfüllen: In 6000 geprüften Drehungen liegt keine ganz hinter dem Schriftzug; die beste lässt noch 135 bzw. 12 px über dem Hintergrund stehen und steckt mit 2 bzw. 1 Ecken in der Hand, und in Bild 1 stünden sie dann sichtbar ab (330 bzw. 683 px). Sollen 20 und 29 eingezogen bleiben (wie heute) oder kürzer zurückkommen?
5. Kralle 19 (Ihre Form 3): Mit einem einzigen Längenfaktor (x0.49) liegt die Spitze noch 16 px neben Ihrem Strichende. Endet die Kralle an Ihrem Strichende, oder läuft sie hinter der Schrift weiter (dann bliebe die K5c-Länge)?
6. Vierte linke Kralle (Schleife 2): Kralle 14 (mein Vorschlag, weil die linke Hand genau 4 Krallenteile hat) oder die Handspitze 13?
7. Krallenmarken hinten links: Ihre zwei roten Marken liegen rechts neben dem Modellfuß über Hintergrund. Sind die Krallen die drei Zehenspitzen des Fußes (mein Vorschlag), oder ragen sie weiter heraus?
8. Zweiter grüner Strich (linkes Hinterbein, ohne Text): Er zeigt auf denselben weißen Fleck wie "weiße Musterung". Richtig, oder meinten Sie eine weitere Stelle?
9. Bräunliche Musterung am rechten Hinterbein: Im Bild ist dort nichts Braunes messbar (graublau). Meinten Sie die helle Fläche direkt unter dem weißen Fleck, entlang Ihres zweiten dunkelroten Strichs?
10. Alles, was in keinem der zwei Bilder zu sehen ist (Rückseiten, Unterseiten, Hals-Unterseite, rechter Hinterfuß): bleibt das Haut ohne eigene Farbe?

## Dateien

| Datei | Inhalt |
|---|---|
| `zonen_v2.json` | meta, 62 Zonen (Nutzerwort, Stelle, Material, Flächenliste, Ganzteil, Sicherheit, Änderung), augen, kopf, krallen.spitzen |
| `krallen_kandidat/` | BAHNSTEIG_FORM.EMD/.TIM, form.json, kandidat.json (Parameter, Ecken, Zählungen), krallen_masken_bild2.npz |
| `tafeln/` | 00_uebersicht, 01-11 je Nutzerbild, 20_rundum, 21_krallen, 22_kopf_augen, LIESMICH.txt (Kopie im Entscheidungsordner 21_riesenform_grundlage_zonen) |
| `skripte/` | g_basis, g_bau, g_fleck, g_spec, g_zonen, g_augen_kopf, g_krallen, g_krallen_spitzen, g_meta, g_ausgabe, g_tafel_basis, g_tafeln, g_tafeln2, g_messblatt |

Ablauf (Arbeitsordner mit asset/, k5c/, anno_*.npz, versatz.json, abbildung.json aus dem v1-Lauf): g_zonen -> g_augen_kopf -> g_krallen -> g_krallen_spitzen -> g_ausgabe -> g_tafeln -> g_tafeln2 -> g_messblatt. Python ohne -I.
