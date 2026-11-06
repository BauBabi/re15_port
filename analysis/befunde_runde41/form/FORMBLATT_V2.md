# FORMBLATT V2: Riesenform (vierbeinig, Bahnsteig 5080 / Zug 5090)

Pruefer, 2026-11-05. Grundlage: Leser A (`LESER_A.md`) und Leser B (`LESER_B.md`), beide gegengelesen und an den Streitpunkten selbst nachgemessen.
Nur gelesen und gemessen; keine Datei unter Version_80%, pri/, synchro/, re15_port veraendert.
Skripte und Messwerte: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/leser/pruefer/` (`py/*.py`, `*.json`).

**Abnahme-Tafel:** `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/leser/pruefer/abnahme_formblatt_v2.png`
(neu erzeugbar mit `py/tafel_abnahme.py`; Inhalt in Abschnitt 6).

## 0. Bezug, Kennungen, Konventionen

| Punkt | Wert |
|---|---|
| Nutzervorlage | `Version_80%/Vorderseite_Beschreibung.png`, Stand 2026-11-05 11:17:07, sha256 `f352bb85...ef83` (um 11:5x unveraendert nachgeprueft). 8 rote Linien, Farbe exakt (237,28,36). |
| Bildteil der Vorlage | x 0-1279 ist pixelgleich `vlcsnap-2026-11-04-16h27m04s664.png` (KI). Abweichend sind nur 28 329 px, alle exakt Linienrot. |
| Massstab | **Bild 2 Original** `vlcsnap-2026-10-31-00h29m02s830.png`, 640x480. KI-px = 2 x Original-px (+0,5). Alle px-Angaben unten in Bild 2 Original, sonst ausdruecklich genannt. |
| Bild 1 | `vlcsnap-2026-10-31-00h28m45s774.png` (Rueckansicht Bahnsteig). |
| Kamera Bild 2 | ROOM5090 Cut 11 (Disc, sicher, `kamera.md`): fx 451, fy 478, Blick 13,1 Grad nach unten. |
| Original-Video | Beide Bilder sind Standbilder einer Diashow (je 24 gleiche Videobilder). Es gibt genau zwei Originalansichten und keine Bewegung (Leser A und B unabhaengig gleich). |
| Quelle | **O** = Original gemessen, **N** = Nutzer-Vorgabe (Linie, Schnitt, Wortlaut), **KI** = KI-Hochrechnung oder Tripo, **P** = Pruefer-Ableitung |
| Sicherheit | **sicher** / **wahrscheinlich** / **geschaetzt** (mit Rechenweg) |
| Seiten | **(W)** = aus Sicht des Wesens, **(B)** = aus Sicht des Betrachters. Das Wesen schaut in Bild 2 zur Kamera: rechts (W) = links (B). Die Frau steht links (B) bei x 140-210, also auf der rechten Seite des Wesens. |

## 1. Pruefung der beiden Lesungen

### 1.1 Linienzuordnung (Grundlage fuer alles Weitere)

Eigene Messung: je Beschriftung die Gerade vom Seitenfeld zum Linienende, Belegung mit rotem Pixel (Radius 2-3 px) entlang der ganzen Strecke.

| Beschriftung (N) | Start Seitenfeld (KI) | Ende (KI) | Ende Bild 2 | Belegung |
|---|---|---|---|---|
| rechtes Auge | (1304,259) | (446,542) | (222,8/270,9) | 100 % |
| linkes Auge | (1318,311) | (577,538) | (288,2/268,9) | 100 % |
| Oberseite vom Maul | (1307,363) | Knick (633,607), Ende (624,663) | Knick (316,1/303,1), Ende (311,7/331,2) | 100 % |
| offenes Maul nach rechts gerichtet | (1310,408) | (659,652) | (329,3/326,0) | 100 % |
| Ende, untere Vorderseite vom Maul | (1315,524) | **(558,799)** | **(278,8/399,5)** | 100 % |
| linke Klaue | (1326,602) | **(905,759)** | **(452,3/379,5)** | 100 % |
| Spitze - Obere Vorderseite vom Maul | (1336,683) | (491,743) | (245,3/371,2) | 100 % |
| rechte Klaue | (1305,872) | (563,868) | (281,3/433,8) | 100 % |

Gegenprobe vertauscht ("Ende" -> (905,759), "linke Klaue" -> (558,799)): Belegung nur 18 %. Die beiden Linien laufen fast parallel (Winkel 160,0 / 159,5 Grad) und kreuzen sich nicht.
**Entscheid:** Leser A und B haben recht. In der Auftragslesung, in E25 ("linke Klaue ~x 560 / y 800") und in E27 ("Spitze 'Ende, untere Vorderseite' weit rechts unten") sind die beiden Enden vertauscht (sicher).

### 1.2 Gegenueberstellung und Entscheid

| # | Thema | Leser A | Leser B | Eigene Messung | Entscheid |
|---|---|---|---|---|---|
| 1 | Augen-Lichtpunkte | (217,275) +12,5, Rang 10/177; (280,275) +30, Rang 2/177 | (217,275) 11,5 "Rauschniveau"; (280,275) 23,6 | Y(Gauss 0,8) minus Median 15x15: (217,274) **12,8**, Rang 5 von 54 Spitzen im dunklen Kopf; (280,275) **29,3**, Rang 2. 90-%-Wert der Spitzen 7,1, aber (225,253) 20,2 und (259,263) 14,0 sind gleich stark oder staerker | einig. Linkes Auge (W) im Original sicher. Rechtes Auge (W) ueber dem Rauschen, aber nicht eindeutig; die Lage traegt die Nutzerlinie und die gleiche Hoehe (wahrscheinlich). KI verstaerkt beide (47,5 / 30,1). |
| 2 | Kopfstellung | frontal, kein Profil, Mittellinie 1,9 Grad | frontal, Spitze 2,5 px neben der Augenmitte | bestaetigt: Augenlinie 0 Grad, Mittellinie 1,9 Grad. Dazu asymmetrisch: dunkler Keil bei y 322-340 reicht 25-36 px links (B) und 58-75 px rechts (B) der Mittellinie; sichtbarer Unterkiefer liegt ganz rechts (B); rechtes Auge (W) schwach | "von vorn, kein Profil" sicher. Verfeinerung: wahrscheinlich leicht zur rechten Seite des Wesens (zur Frau) gewendet; Betrag nicht messbar. |
| 3 | Unterkiefer-Ende | (279,399) = "unteres linkes Ende des Bandes" | (279,5/399,5) an der Bandunterkante, aeusserstes Ende 20 px weiter links | Maske `Unterkiefer.png`: tiefster Punkt **(264,408)**, linkester (243,374); der Lappen unter y 372 liegt bei x 243-308. Nutzerpunkt liegt 1 px innen am rechten Lappenrand, 17 px vom tiefsten Punkt | B ist genauer. Ziel = ganzer Lappen, Nutzerpunkt liegt darin. |
| 4 | Zahnflecke auf dem Band | 6, Abstand 16 px (aus K.md) | 5: (257,385) (288,370) (292,352) (307,348) (325,333) | lokale Maxima Y gegen Umgebung (Gauss 6): Prominenz >= 44: 6 Flecke; >= 30: 11 Flecke | Zahl haengt an der Schwelle. Vorgabe: **5-6 deutliche helle Flecke**, Abstand 16-34 px (wahrscheinlich). KI zeigt sie an denselben Stellen (B). |
| 5 | Maulspalt im Bild | 27,1 / 32,9 Grad | 27 Grad; Keilrand gegen Band 7 Grad | Scheitel (329,3/326): zur OK-Spitze 151,7 Grad, zum Nutzer-UK-Ende 124,5 Grad -> **27,2 Grad**; zum Masken-UK-Ende (264,408) 128,4 Grad -> **23,3 Grad** | einig: 23-27 Grad im Bild. Der fruehere Wert 50-56 Grad (K.md) ist ueberholt. |
| 6 | Boden in den Fussschnitten Bild 1 | fuss_links 383 px (37 %) hell + 334 Logo; fuss_rechts 338 px (27 %) | fuss_links 280 px (27 %); fuss_rechts 255 px (21 %) | kNN in Lab gegen Bodenprobe (138,175,201) und Beinkern (38,37,72): fuss_links **189 px (18 %)**, davon 185 mit dem Boden ausserhalb verbunden, Logo 121 px; fuss_rechts **160 px (13 %)**, Band ganz innen, nicht mit dem Boden verbunden | Zahl haengt an der Schwelle (13-37 %), Aussage gleich: helle Baender = Boden (N), Sohle und tuerkise Querbaender entfallen. fuss_links Boden auch geometrisch sicher, fuss_rechts nur durch den Nutzersatz. |
| 7 | K4-Augen in Pose Bild 2 | Projektion (300,263) / (283,261), verdeckt von der Brust | geschaetzt um (301,302) | nicht nachgerechnet; B hat selbst [g] markiert, A rechnet mit Tiefenpuffer. Sichtbarer K4-Kopf (B) x 223-356, y 283-366 liegt UNTER beiden Lagen | A gilt. Einig: Die K4-Augen sind in Bild 2 nicht sichtbar, die Nutzer-Augen (y 275) liegen ausserhalb des sichtbaren K4-Kopfs. |
| 8 | Kopfbreite 3D | Kopfbreite 85 px nur mit Symmetrie | Augenabstand 698-822 Einheiten bei Tiefe 5000-5881 | Der Kopf muss VOR der Brust liegen (Brust an den Augenpunkten Tiefe 4759-4822, Leser A); 63 px x d / 451 ergibt bei d 4000-5500 **560-770** | B's Tiefenspanne ist fuer den Kopf zu tief. Vorgabe: Augenabstand rund 560-770 Einheiten (geschaetzt), K4 hat 397. |
| 9 | Tripo-Maul | "gespiegelt": Band 17-43 px links (B) der Mitte, Original 45 px rechts | "Maulform falsch" | Bandachse Tripo `image_0` VORNE 67-68 Grad, `image_3 (1)` 59 Grad, faellt wie im Original nach links unten (B); Original 38,5 Grad. Schwerpunkt Tripo 17-19 px links (B), Original 45 px rechts (B) | A's Wort "gespiegelt" trifft nur die Seitenlage, nicht die Richtung. Richtig: gleiche Schraege, steiler, auf der anderen Seite der Mittellinie. Tripo-Maul nicht uebernehmen (N). |
| 10 | Tripo LINKS / RECHTS | nicht geprueft | RECHTS = Spiegel von LINKS | MAE gespiegelt 9,8 (8,8 mit 2 px Versatz), ungespiegelt 30,5 | B bestaetigt: nur eine echte Seitenansicht. |
| 11 | Schwanz | hoechster Punkt Bild 1 (tail y 184 ueber part_of_Body 204) | Scheitel y 184-187 oberster Punkt | tail oberste Zeile **184** (x 297-302), part_of_Body **205** | einig, sicher (O, Schnitt N). |
| 12 | Rueckansicht B/H | - | Original 0,85, Tripo HINTEN 1,09 | `maske_bild1.png` (255): 236 x 278 = **0,849** | B bestaetigt: Tripo spreizt die Beine. |
| 13 | Klauen-Seite | Linien passen nur mit (W), E25 | Linien passen nur mit (W) | Linie "linke Klaue" endet rechts (B) bei x 452, "rechte Klaue" links (B) bei x 281 | einig; (W) = E25. Siehe 3.1 und Frage F2. |
| 14 | Krallenspitzen | dunkle Flaeche: (406,391) (435,407) (465,412); Schnitt: (439,418) (468,417) | Schnitt: (439,418) (468,416); (274,443), (229,351) | Konturwinkel < 75 Grad an den Nutzerschnitten: links (W) **(439,418) 29 Grad, (468,417) 26 Grad**; rechts (W) **(274,443) 37 Grad**; (229,351) 37 Grad, zeigt nach oben, 26 px neben der OK-Spitze (nicht auf der Huelle, darum bei A/B unterschiedlich gezaehlt); (148,349) Schnittende; (329,314) 4 px ueber dem Unterkiefer (E20) | Massstab sind die Nutzerschnitte. Sicher Kralle: (274,443), (439,418), (468,417). (229,351): Hand oder Kopf offen. (329,314) gehoert zum Unterkiefer. |
| 15 | Weisse Fleckgruppe (161-221, 329-386) | Feuer-/Bodenlicht (215,207,204) wie (212,207,204) | offen (Frage) | neutralweiss, nahe Vollaussteuerung, heller als der beleuchtete Unterkiefer (hellste Stelle Y 200); mit "Maul nach rechts (W)" liegt sie vor der Spitze, nicht am Kiefer | wahrscheinlich nicht Wesen; Frage F7 nur zur Bestaetigung. |
| 16 | Tiefrote Zone aussen am Band | - | 20-35 px breit | Querprofile an 5 Stellen: 2 / 2 / 29 / 9 / 0 px; oben geht sie in eine grosse rote Flaeche ueber | keine gleich breite Flanke messbar; offen (F8). |
| 17 | Augenfarbe | dunkles Rotviolett (82,36,52) | dunkelrot-violett | (280,275): RGB (82,36,52), Y 47 | einig: kein Leuchtrot (das ist Tripo-Licht). |
| 18 | Seitenkonvention | Maul "nach links unten (B), zur rechten Seite des Wesens" | gleich | - | Kein Leser hat (B) und (W) verwechselt. |
| 19 | KI als Original | KI-Finger, KI-Augenring als [KI] markiert | Zahnflecke erst im Original gemessen, KI nur Gegenprobe | - | Kein Leser hat ein KI-Detail als Original gefuehrt. |

### 1.3 Widersprueche zu frueheren Entscheiden (nur der Orchestrator darf NUTZER_ENTSCHEIDE aendern)

| Stelle | Fehler | Richtig (sicher, gemessen) |
|---|---|---|
| E25 | "linke Klaue (obere Linie, ~x 560 / y 800)" | Linie "linke Klaue" endet bei KI (905,759) = Bild 2 (452,379), rechts (B). (558,799) ist "Ende, untere Vorderseite vom Maul". Die Seitenaussage "(W)" bleibt richtig. |
| E27 | "lange Schnauze vom Wesen aus nach rechts (von der Frau weg) ... Spitze 'Ende, untere Vorderseite' weit rechts unten" | Beide Kieferspitzen liegen links unten (B) vom Maulwinkel: (245,371) und (264-279, 399-408). Die Frau steht links (B) = rechts (W). "Nach rechts (W)" heisst also ZUR Frau. E27 widerspricht sich selbst (rechts (W) und "von der Frau weg") und E24. |
| E27 | "K4 (kurze Schnauze nach vorn unten) ist damit ueberholt" | K4 ist aus anderen Gruenden falsch (Profil, Augen verdeckt, zu schmal, Lage; Abschnitt 7). Die Richtung von K4 (Maul zur Frau, Bild links unten) stimmt; sie darf nicht umgedreht werden. |
| E26 | "Fuesse enden in kraeftigen Krallen wie in den Tripo-Ansichten" | Steht nicht im Nutzer-Wortlaut von 2026-11-05, ist Lesung des Orchestrators. Im Original (Bild 1) ist keine Fusskralle aufloesbar; Beleg nur KI. |
| E16 Nachtrag | "Sohlen frontal sichtbar, tuerkise Querbaender = Sohlenstruktur" | entfaellt durch den Nutzersatz 2026-11-05 (helle Dinge = Boden). |
| K.md 3 | "Oberkiefer links breit = Schnauze vorn, rechts schmal = Hals", Maulwinkel 50-56 Grad | Die Augen liegen 5-32 px UEBER `Oberkiefer.png`; der Kopf steht mit Augen oben und Spitze unten. Maulspalt 23-27 Grad. |

## 2. Formblatt V2: Kopf und Maul (Bild 2)

### 2.1 Landmarken (Ziele fuer die Projektion mit Kamera 5090 Cut 11)

Die Nutzerlinien enden 2-11 px vor ihrem Merkmal (B). Ziel ist darum das Merkmal im Original, der Nutzerpunkt steht daneben.

| Nr | Merkmal | Ziel Bild 2 (px) | Nutzerpunkt | Pruefgrenze (Vorschlag P) | Quelle | Sicherheit |
|---|---|---|---|---|---|---|
| L1 | linkes Auge (W), rechts (B) | **(280,275)**, RGB (82,36,52) | (288,2/268,9) | <= 6 px, sichtbar | O + N | sicher |
| L2 | rechtes Auge (W), links (B) | **(217,275)** | (222,8/270,9) | <= 6 px, sichtbar | N, O schwach | wahrscheinlich (F3) |
| L3 | Augenlinie | waagerecht, Abstand **63 px** | 65,4 px, -1,8 Grad | <= 3 Grad, 57-69 px | O + N | sicher |
| L4 | Oberkiefer-Spitze ("Spitze - Obere Vorderseite") | **(245,371)**, tiefster Teil von `Oberkiefer.png` (Maske bei y 372: x 244-259) | (245,3/371,2), 2 px in der Maske | <= 6 px | N + O | sicher |
| L5 | Kopfmittellinie Augenmitte (248,5/275) -> L4 | 96 px lang, **1,9 Grad** gegen die Senkrechte | - | <= 5 Grad | O + N | sicher |
| L6 | Unterkiefer-Vorderende ("Ende, untere Vorderseite") | Lappen von `Unterkiefer.png` unter y 372 (x 243-308, tiefster Punkt **(264,408)**) | (278,8/399,5), 1 px innen am Lappenrand | Modell-UK fuellt den Lappen, Nutzerpunkt darin | N + O | sicher als Bereich, Punkt wahrscheinlich |
| L7 | Maulwinkel links (W) = oberes Bandende ("offenes Maul nach rechts gerichtet") | **(329,326)**; Bandende der Maske (342-344, 320-324) | (329,3/326,0), 5,6 px in `Unterkiefer.png` | <= 8 px | N + O | wahrscheinlich (Deutung als Winkel P) |
| L8 | "Oberseite vom Maul" | Strichende **(312,331)** = Grenze Oberkiefer/Band (4,2 px in OK, 3,6 px vor UK); Knick (316,303) | wie Ziel | <= 6 px auf der Grenze | N | Punkt sicher, Bedeutung offen (F6) |

### 2.2 Flaechen (Zielmasken)

| Teil | Ziel | Messwert | Quelle | Sicherheit |
|---|---|---|---|---|
| Oberkiefer (dunkel, Y um 20, z. B. (54,7,31)) | `nutzer_teile/maske_oberkiefer.png`, 5832 px, x 217-329, y 280-374 | Keilbreite (Y < 50) y 322/334/346/358/370: 113/93/62/43/28 px; linker Rand (B) nahezu senkrecht (x 212 -> 233), rechter Rand (B) schraeg (x 324 -> 260) | N + O | sicher |
| Unterkiefer (helles rosa Band, Y 90-200) | `nutzer_teile/maske_unterkiefer.png`, 3355 px, x 243-343, y 318-408 | Achse 38,5 Grad steigend nach rechts (B), 115 px lang, Schwerpunkt (293,6/364,0); ganz rechts (B) der Kopfmittellinie (+45 px) | N + O | sicher |
| Zaehne | 5-6 helle Flecke auf dem Band, z. B. (325,333) (307,346) (292,352) (257,385) | Abstand 16-34 px | O | wahrscheinlich |
| Maulinneres | zwischen Keil und Band keine dunkle Luecke (B) | Keilrand gegen Bandachse 7 Grad | O | sicher (Bild); Tiefe offen |
| Kopfrand rechts (W) | dunkle Masse gegen Kittel bei **x 204-206** in Augenhoehe (Y 143 -> 24) | 11-13 px links (B) vom rechten Auge (W) | O | wahrscheinlich Kopfrand (koennte Schulter sein) |
| Kopfrand links (W) | nicht messbar (dunkel auf dunkel) | - | - | offen |

### 2.3 Verhaeltnisse und Richtungen (Einheit e = Augenabstand 63 px im Bild)

| Mass | Wert | Quelle | Sicherheit |
|---|---|---|---|
| Augenlinie -> OK-Spitze | 96 px = **1,53 e** (Tripo 1,48) | O + N | sicher im Bild; in 3D nur Untergrenze |
| OK-Spitze -> UK-Vorderende | 41-44 px = **0,66-0,70 e**, UK-Ende rechts unterhalb (B) der OK-Spitze (+19..+34 / +28..+37 px) | O + N | sicher |
| Augenmitte -> Maulwinkel L7 | (+81 / +51) px = 1,28 e rechts (B), 0,81 e tiefer | O + N | wahrscheinlich |
| Maulwinkel -> OK-Spitze | 95 px = 1,51 e, Richtung 151,7 Grad (nach links unten, B) | O + N | wahrscheinlich |
| Maulspalt im Bild | **23-27 Grad** | O + N | sicher im Bild; 3D-Oeffnung offen |
| Maulrichtung | Oeffnung nach links unten (B) = **nach rechts (W) = zur Frau** | O + N (E24) | sicher, Bestaetigung F1 |
| Sichtbarer Unterkiefer | nur der linke Ast (W), rechts (B) der Mittellinie; rechter Ast (W) nicht sichtbar | O | wahrscheinlich |
| Augenhoehe in der Figur Bild 2 | (275-142)/336 = **0,40** von oben (Tripo 0,42-0,43) | O + N | wahrscheinlich (Figur unten abgeschnitten) |
| Kopflage in der Breite Bild 2 | Augenmitte bei **0,32** der Figurbreite (x 143-468) von links (B); Tripo 0,51 | O + N | sicher |

### 2.4 3D-Ableitung (nur als Startwert fuer den Fit)

| Mass | Wert | Rechenweg | Sicherheit |
|---|---|---|---|
| Kopftiefe | 4000-5500 | vor der heutigen J-Brust (an den Augenpunkten 4759-4822, Leser A) hiesse <= 4760; mit geaendertem Rumpf (F5) bis rund 5500 | geschaetzt |
| Augenabstand | **560-770** Einheiten (K4: 397) | 63 px x d / 451 | geschaetzt; Untergrenze, wenn der Kopf gedreht ist |
| Auge -> Spitze | >= 800-1110 (K4: 1100-1155) | 96 px x d / 478, Verkuerzung macht es nur laenger | geschaetzt (F4) |
| Kopfdrehung | wahrscheinlich leicht nach rechts (W), zur Frau | Asymmetrie Tabelle 1.2 Nr 2 | Betrag im Fit an L1-L8 bestimmen |

## 3. Formblatt V2: Klauen (Bild 2) und Fuesse (Bild 1)

### 3.1 Vorderklauen

| Beschriftung (N) | Linienende | Merkmal im Original | Krallenspitzen (Nutzerschnitt, Konturwinkel) | Lesart (W) | Lesart (B) |
|---|---|---|---|---|---|
| "linke Klaue" (oberes Schild) | (452,3/379,5), rechts (B) | Aussenkante `left_arm_front`, 2 px daneben | **(439,418), (468,417)** | linke Klaue des Wesens = rechts (B): **passt** | linke Klaue = links (B): Linie zeigt nach rechts, **vertauscht** |
| "rechte Klaue" (unteres Schild) | (281,3/433,8), links unten (B) | Hand von `right_arm_front` (x 158-276, y 385-445), 5 px daneben | **(274,443)**; 2 weitere nur im Schnitt unter dem Logo; (229,351) nach oben, Zuordnung offen | rechte Klaue des Wesens = links (B): **passt** | **vertauscht** |

Vorgabe: Lesart (W), wie E25 (N) und wie die Augen (E24). Sicher nach Linienverlauf; Frage F2 nur zur Bestaetigung.
Krallen: je Hand **2-3 sichtbare, lange, spitze Krallen** (O). Tripo zeigt 3-5 gespreizte Kristallkrallen (KI): nicht uebernehmen. Die K4-Krallen (4/5 bzw. 2/3 Paare an den Schnitten) bleiben.

### 3.2 Hinterfuesse (Bild 1)

| Punkt | Vorgabe | Quelle | Sicherheit |
|---|---|---|---|
| Helle Baender in `fuss_links` (x 195-217, y 415-428) und `fuss_rechts` (x 294-315, y 415-427, 31-35 Grad) | **Boden**, keine Sohle; im Modell frei lassen | N (Satz 2026-11-05) + O (Farbe wie Boden; fuss_links mit dem Boden verbunden) | sicher (links), wahrscheinlich (rechts) |
| Fuss | dunkler Rest der Schnitte: fuss_links 719 px (x 193-218, y 396-447, ohne Logo 121 px), fuss_rechts 1077 px (x 288-325, y 403-452) | O + N | wahrscheinlich |
| Sohle frontal, tuerkise Querbaender (E16) | entfaellt | N | sicher |
| Fusskrallen | im Original nicht aufloesbar; Tripo zeigt Klotzfuesse mit 3 kleinen Krallen (KI). Hoechstens kleine Krallen innerhalb des dunklen Rests | KI, Lesung E26 | geschaetzt |
| Helle Flecken in Beinhoehe | `bein_links` 0 px bodenfarbig, `bein_rechts` 139 px (3 %, x 320-342, y 287-364, (189,194,211)); bisher Zeichnung (E10-E14) | O | offen (F9) |

## 4. Formblatt V2: Koerperbau

| Merkmal | Vorgabe | Original | KI (Tripo) | Quelle | Sicherheit |
|---|---|---|---|---|---|
| Vierbeiner | vier Gliedmassen, vorn Klauenhaende | Bild 1: 2 Beine, 1 Arm, Schwanz sichtbar; Bild 2: 2 Klauen, Kopf | vier Beine | N (E20, E24-E28) | sicher |
| Rueckenkuppel | ein runder Scheitel | Bild 1 part_of_Body ab y 205; Bild 2 Figur ab y 152-156 | Kuppel | O + KI | sicher |
| Schwanz | glatt, aus einem Stueck (E4, E9); in Bild 1 **hoechster Punkt** der Figur, Scheitel y 184 (x 297-302), 21 px ueber part_of_Body; Saeule x 204-322 bis y 478 | tail-Schnitt | haengt aus der Rueckenmitte unter der Kuppel | N + O | sicher; Tripo-Schwanz nicht uebernehmen |
| Stand von hinten | schmal: B/H **0,85** (maske_bild1) | O | 1,09 gespreizt | O | sicher; nicht spreizen |
| Kopf in Bild 2 | tief vor der Brust, links (B) der Figurmitte (0,32), zwischen den Klauen, Augen bei 0,40 der Hoehe | O + N | mittig (0,51), Augen 0,42-0,43 | O + N | wahrscheinlich |
| Kopf in Bild 1 | nicht sichtbar (Rueckansicht) | O | - | O | sicher |
| Augen | dunkles Rotviolett, kein Leuchten | (82,36,52) | leuchtend rot | O | sicher |
| Laenge zu Hoehe | keine Originalbelegung (keine Seitenansicht) | - | H : L : B rund 1 : 1,14 : 1,08 | KI | geschaetzt, nur Lesehilfe |

## 5. Nicht uebernehmen (nur KI, widerspricht dem Original oder ohne Beleg)

| KI-Merkmal | Grund |
|---|---|
| Tripo-Maul (Band links (B) der Mitte, 59-68 Grad steil) | Original: rechts (B), 38,5 Grad; Nutzer: "Tripo ... nicht in der Lage den Mund so richtig abzubilden" |
| Kopf symmetrisch mittig | Original 0,32 statt 0,51 der Breite |
| Schwanz haengt unter der Kuppel | Original: Schwanz-Scheitel ist der hoechste Punkt (Bild 1) |
| Gespreizter Stand (B/H 1,09) | Original 0,85 |
| Leuchtrote Augen | Original dunkles Rotviolett |
| Stangen, Gurte, Strebe unter dem Bauch | Bild 1 x 190-340, y 250-470: 0 gerade Kanten >= 30 px (Leser A) |
| 3-5 gespreizte Kristallkrallen, Klotzfuesse | Original 2-3 Krallen je Hand, Fuesse nicht lesbar |
| Facetten und Farben | Tripo-Licht |
| Augenring, lange helle "Finger" links der Schnauze (`16h27m04`, `16h26m29`) | nur KI-Zeichnung |

## 6. Abnahme

Tafel: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/leser/pruefer/abnahme_formblatt_v2.png` (2110 x 1345).

| Feld | Inhalt |
|---|---|
| A | Bild 2 Original (Gamma 0,6, 3x): Augen-Ziele L1/L2 mit Augenlinie, Mittellinie L5, OK-Spitze L4, UK-Ende Nutzer und Maskenspitze L6, Maulwinkel L7, Knickstrich L8, Maulstrahlen (23-27 Grad), Umrisse Oberkiefer/Unterkiefer/Arme, Krallenspitzen, Frau, Pfeil "Maul nach rechts (W)", K4 heute (blau x, Projektion Leser A) |
| B | dieselben Ziele auf `Vorderseite_Beschreibung.png` (KI + rote Nutzerlinien): Ziele und Linienenden decken sich |
| C | Bild 1 Original (2x): Bodenpixel in `fuss_links`/`fuss_rechts` rot, Schnitte gruen, Schwanz-Scheitel y 184 ueber part_of_Body y 205 |
| D | Tripo VORNE (KI): Augenlinie, Mittellinie, Bandachse 67-68 Grad, Schwerpunkt 17 px links (B) |

Pruefregeln fuer das naechste Modell (Kamera 5090 Cut 11, Pose Bild 2; Kamera Bild 1 fuer C):

| Regel | Grenze | Grundlage |
|---|---|---|
| R1 beide Augen sichtbar (Tiefenpuffer) | je <= 6 px von L1/L2 | 2.1 |
| R2 Augenlinie | <= 3 Grad, Abstand 57-69 px | L3 |
| R3 OK-Spitze | <= 6 px von (245,371) | L4 |
| R4 Kopfmittellinie | <= 5 Grad | L5 |
| R5 UK-Vorderende | fuellt den Lappen unter y 372 von `Unterkiefer.png`; (279,399) im Modell-UK | L6 |
| R6 Maulwinkel links (W) | <= 8 px von (329,326) | L7 |
| R7 sichtbarer Unterkiefer | Schwerpunkt rechts (B) der Mittellinie, Achse 30-47 Grad steigend | 2.2 |
| R8 IoU wie bisher | Oberkiefer, Unterkiefer, Umriss Bild 1 >= 0,954 / Bild 2 >= 0,948 | STUFE3 |
| R9 Bild 1 Boden frei | die roten Bodenpixel (189 + 160 px) nicht vom Modell bedeckt | 3.2 |
| R10 Schwanz | Scheitel Bild 1 hoechster Modellpunkt (y <= 190) | 4 |

Die Grenzen R1-R7 sind ein Vorschlag des Pruefers (Messunschaerfe der Linienenden 2-11 px), keine Nutzervorgabe.

## 7. Aenderungen am bisherigen Port-Modell (Zweig r41/form, J-Rumpf + Kopf K4)

| # | Heute (gemessen) | Muss werden | Grundlage | Sicherheit |
|---|---|---|---|---|
| AE1 | K4 in Pose Bild 2 ein 3/4-Profil: Mittellinie 34,3 Grad, Augenhoecker 16,8 px auseinander bei (300,263)/(283,261), beide hinter der Brust verdeckt (Leser A) | Gesicht zur Kamera: beide Augen sichtbar auf y 275, 63 px Abstand, OK-Spitze senkrecht darunter (R1-R4) | L1-L5 | sicher (Bild), Fit noetig |
| AE2 | K4 Augenabstand 397, Schaedel 764 breit | Augenabstand rund 560-770 (1,4-1,9 x K4), Schaedel entsprechend breiter | 2.4 | geschaetzt |
| AE3 | J-Halsende (342,280), Kopf erreicht rund 40 px; an den Augenpunkten liegen im Port Huefte (0), Brust (10), Wirbel (7) | Kopfmitte rund 94 px vom Halsende entfernt und VOR der Brust: Hals laenger und nach vorn unten, oder Rumpf zur Kamera drehen | STUFE3 NB2-K1, Leser A und B | sicher als Zielkonflikt; Weg F5 |
| AE4 | An OK-Spitze und UK-Ende liegt im Port die rechte Hand (Knochen 18) | rechte Hand aus dem Kieferbereich (243-308, 371-408) nehmen; sie gehoert an `right_arm_front` (x 158-276, y 385-445) | 3.1, Leser A | wahrscheinlich |
| AE5 | K4-Maul zeigt zur Frau (Bild links unten) | Richtung **beibehalten**; NICHT nach E27 umdrehen | 2.3, 1.3 | sicher |
| AE6 | Maul offen, Kieferspitze (254,404), Gelenk (316,326) (K4, 6-26 px neben den Zielen) | UK-Vorderende in den Lappen L6, Maulwinkel L7, Bildspalt 23-27 Grad; rechter UK-Ast (W) nicht sichtbar | L6, L7 | wahrscheinlich |
| AE7 | K4: 11 obere / 10 untere Zaehne je Seite | sichtbar am Band 5-6 helle Zahnflecke, Abstand 16-34 px | 2.2 | wahrscheinlich |
| AE8 | Schnauze in 3D 2,9 Augenabstaende (K4) | im Bild 1,53 e; 3D-Laenge nach F4 | 2.3 | offen |
| AE9 | Fuesse nach E16 mit Sohle frontal und Querbaendern (Pose 1) | Bodenpixel aus der Zielmaske, Fuss = dunkler Rest, Pose 1 ohne "Sohle zur Kamera" | 3.2 | sicher |
| AE10 | Augen im Port | dunkles Rotviolett, kein Glanzrot | 4 | sicher |
| AE11 | Pruefwerkzeug misst den Kopf nur ueber IoU Oberkiefer (Ziel 0,70) | Landmarken R1-R7 zusaetzlich; die IoU-Grenze 0,70 aus E22 an der Oberkiefer-Maske misst die alte Kopfrichtung mit | 6 | wahrscheinlich |
| AE12 | Krallen K4 vorn, Schwanz J, Kuppel J | unveraendert; nichts aus Tripo (Abschnitt 5) | 3.1, 4, 5 | sicher |

## 8. Fragen an den Nutzer (nur was das Material nicht entscheidet)

| # | Frage | Was gemessen ist | Vorgabe bis zur Antwort |
|---|---|---|---|
| F1 | "offenes Maul nach rechts gerichtet": rechts **aus Sicht des Wesens**, also nach links unten im Bild, zur Frau hin? | Maulwinkel rechts (B) bei (329,326), beide Kieferspitzen links unten (B) bei (245,371) und (264-279, 399-408). Eine Schnauze nach rechts (B), von der Frau weg (Lesung E27), zeigt das Bild nicht. | (W), zur Frau (Bestaetigung) |
| F2 | Klauen: "linke Klaue" = linke Klaue des Wesens (rechts im Bild, Linie endet bei (452,379)), "rechte Klaue" = rechte Klaue des Wesens (links unten im Bild, (281,434))? | Lesart (W) passt ohne Tausch; aus Sicht des Betrachters waeren die Beschriftungen vertauscht. E25 sagt (W), nannte aber die Koordinate der Maul-Linie. | (W) (Bestaetigung) |
| F3 | Rechtes Auge (W) bei (217,275): im Original gesehen oder aus dem KI-Bild uebernommen? | Original: schwacher Fleck (12,8), zwei andere Flecke im Kopf sind gleich stark oder staerker ((225,253) 20,2). Linkes Auge klar (29,3). | Lage (217,275) wie Nutzerlinie |
| F4 | Schnauzenlaenge im Raum: so kurz wie im Bild (1,5 Augenabstaende, Schnauze zeigt nach unten) oder lang wie ein Krokodil (rund 3 Augenabstaende) und stark zur Kamera geneigt? | Bild: 1,53 e als Projektion; beides ergibt dasselbe Bild. | K4-Laenge (rund 1100) bei breiterem Kopf, also rund 1,4-2 e |
| F5 | Darf fuer Bild 2 der Hals laenger werden oder der Rumpf zur Kamera drehen, damit der Kopf vor der Brust liegt? (Aenderung am gelobten Stand J) | Kopf braucht rund 94 px vom heutigen Halsende, der J-Hals reicht rund 40 px. | nichts bauen ohne Ja |
| F6 | "Oberseite vom Maul": zeigt der Knickstrich nur auf sein Ende (312,331) an der Grenze Oberkiefer/Band, oder markiert der kurze Strich (316,303)-(312,331) eine Kante? | Entlang des Strichs ist keine Kante messbar (Y 22-25). | Punkt (312,331) |
| F7 | Weisse Fleckgruppe links neben der Schnauze (x 161-221, y 329-386): Teil des Wesens oder Licht/Boden? | neutralweiss wie das Feuer rechts, heller als der beleuchtete Unterkiefer | nicht Wesen |
| F8 | Tiefrote Zone rechts unter dem hellen Band: Aussenseite des Unterkiefers oder Koerper dahinter? | Breite wechselt 0-29 px entlang des Bandes | nicht Kiefer |
| F9 | Gilt "helle Dinge = Boden" auch fuer helle Flecken in Beinhoehe (bein_rechts x 320-342, y 287-364)? | 3 % von bein_rechts bodenfarbig, bein_links 0 % | bleiben Zeichnung (E10-E14) |

## 9. Belege (Pruefer)

| Datei (scratchpad/leser/pruefer/) | Inhalt |
|---|---|
| `abnahme_formblatt_v2.png` | Abnahme-Tafel (Abschnitt 6) |
| `linien_enden.json`, `py/m2_linien.py`, `py/m3_luecken.py`, `py/m4_tausch.py` | Linienenden, Belegung, Gegenprobe Tausch (18 %) |
| `augen.json`, `py/m8_augen.py` | Prominenz der Augenflecke, Original und KI |
| `keil.json`, `py/m10_keil.py` | dunkler Keil je Zeile, Kopfrand x 204-206 |
| `zaehne.json`, `py/m9_zaehne.py` | Zahnflecke je Schwelle |
| `rotzone.json`, `py/m11_rotzone.py` | Querprofile am Band |
| `boden_bild1.json`, `py/m5_boden.py`, `fuss_boden_x5.png` | Boden in den Fussschnitten (kNN) |
| `py/m6_tripo.py`, `py/m7_tripo_band.py`, `t0_raster.png`, `t3_raster.png` | Tripo-Bandachse, Spiegelprobe LINKS/RECHTS |
| `py/m12_masken.py` | Kiefermasken, Krallenspitzen |
| `kopf_orig_gamma_x4.png`, `kopf_ki_x4.png`, `bild1_teile_x3.png` | Arbeitsansichten |
| Leser A `.../leser/A/`, Leser B `.../leser/leser_b/` | Tafeln der Leser (siehe LESER_A.md / LESER_B.md) |
