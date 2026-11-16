# Prüfung der Zonen-Zuordnung (unabhängig, Runde 41, Riesenform)

Stand 2026-11-09. Geprüft wird `MESSBLATT_ZONEN.md` / `zonen.json` (Commit e53cef212).
Nur gemessen. Kein Asset, keine Variante und kein Code wurde geändert, und es lief kein Bau.

**Eigener Code:**
- Ablage: im Scratchpad dieser Sitzung (`.../scratchpad/pz/`), nicht committet, weil laut Auftrag nur diese Datei committet wird.
- Wiederverwendet habe ich nur den vorhandenen Renderer `v4f.szene_id` / `Kandidat.rendern` (Mesh/Fläche je Pixel).
- Ausrichtung, Umrisse, Flächenzählung, Krallen und Seiten sind eigene Skripte, ohne die Skripte des Messers.

**Modell, Posen, Seiten:**
- Modell: Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` (md5 6e00f105..., TIM 4668e504...), in einen eigenen Ordner kopiert.
- Posen nach `EINBAU_PRUEFUNG_r1_sicht.md`:
  - Bild 1: Clip 0 Bild 11, Lage -18564/0/-18386, Gier 80
  - Bild 2: Clip 12 Bild 0, Lage 941/0/-23776, Gier 3874
- Seiten gelten aus Sicht des Wesens.

**K5c (Geometrie vor dem Einziehen):**
- Quelle: `stufe3/basis_k5c/FORM.EMD`. Diese Datei hat nur 3 Clips. Clip 0 Bild 0 und Clip 1 Bild 0 sind gemessen bytegleich (R und T aller Knochen) mit Clip 0 Bild 11 und Clip 12 Bild 0 des Spiel-Assets.
- Ecken: Gegenüber dem Asset weichen nur die Krallen 19, 20, 28 und 29 ab.
- Der Vergleich in derselben Pose ist damit gültig.

**Gegenprobe des Kennungs-Renders:**
- `v4f.szene_id` gegen `Kandidat.rendern`: 0 px Unterschied in beiden Bildern.
- Modellpixel: Bild 1 36 372, Bild 2 47 987. Bestätigt.

## Ergebnis

1. **Ausrichtung bestätigt.**
   - Beide Grundlagen sind die Vorlage mal 2, nicht gespiegelt und nicht gedreht (höchstens 0,11°).
   - Meine Abbildung weicht über der ganzen Figur höchstens 0,63 px (Bild 1) bzw. 0,50 px (Bild 2) von der Abbildung des Messers ab.
   - Bild 2 ist **besser** ausgerichtet, als das Messblatt angibt: Kachel-Rest p90 2,5 px statt 7,9 px.
2. **Große Zonen bestätigt** (IoU 0,95–0,98, Flächenlisten bis auf Randflächen mit 1–9 px gleich):
   - großer fleischfarbener Teil
   - Oberkörper vorn
   - Zipfel
   - Vorderbein links
   - Krallen vorn links
   - weiße Musterung des Vorderbeins r
   - Kopf von hinten (nur Oberarm 16)
3. **Punkt-Zonen fehlerhaft.** Der Messer hat einen Kreis um das Linienende gesetzt, doch die gemeinte helle Stelle liegt neben dem Linienende:
   - **weiße Musterung Hinterbein l:** IoU 0,06.
     - Die Zone des Messers ist dunkel (Video-L* 30), der weiße Fleck hat L* 65.
     - Der Fleck liegt auf Oberschenkel 8, die Zone des Messers vor allem auf Hüfte 0.
   - **Augen vorn:** IoU 0,05. Die Kreise decken nur 17 % der beiden Augenpunkte, 93 % der Kreise sind Haut.
   - **weiße Musterung Hinterbein r:** Die Zone des Messers deckt nur 39 % des Flecks. Es fehlen 3 Flächen von Oberschenkel 9.
4. **Krallen:**
   - **Vorne links „exakt 4“:** bestätigt (4 geschlossene Schleifen).
     - Jede Nutzerspitze hat eine Modellspitze in ≤ 7 px Abstand. Die vierte gehört zu K5c-Kralle 28 (4,9 px).
     - „28 zurück“ ist damit stärker belegt als im Messblatt (dort nur 8 px Überdeckung).
   - **Vorne rechts:** Der Nutzer zeichnet **2** geschlossene Krallenformen, nicht 3 Spitzen.
     - Zwei der drei „Spitzen“ des Messers sind das obere und das untere Ende derselben Form.
     - Form 3 entspricht K5c-Kralle 19 (46 von 48 px). Die Spitze von 19 liegt aber 42 px tiefer als die Nutzerspitze.
     - 20 und 29 liegen zu 0,7 % bzw. 0 % in der Video-Silhouette. Das Einziehen der beiden ist damit gedeckt.
   - **Hinten:** Fuß 25 und Fuß 27 haben je 3 Zehenspitzen (bestätigt).
     - Die beiden Krallenmarken des Nutzers liegen überwiegend auf **keinem** Modellteil (Deckung 0,08 / 0,26).
     - Sie liegen 4–16 px rechts neben dem sichtbaren Fuß 25. „Auf Schwanz 5“ trifft nur für 4 bzw. 23 Randpixel zu.
5. **Seiten bestätigt.**
   - Gemessen über die Tiefe: Bild 1 zeigt das Wesen von hinten, Bild 2 von vorn.
   - Alle Seitenwörter des Nutzers und alle l/r-Teile des Modells liegen auf der erwarteten Bildseite.
   - Einzige Ausnahme ist die Beschreibung „Maul nach unten rechts blickend“. Sie meint die Bildrichtung, also die linke Seite des Wesens, und betrifft keine Zone.
6. **Weitere Befunde:**
   - Ein grüner Strich in `linkes_bein_hinten.png` fehlt im Messblatt.
   - Bild-2-Kopfzonen: Der Modellkopf sitzt 70 px zu hoch. Die Flächenlisten der Kopfzonen treffen deshalb andere Teile des Modells (siehe 2.3).

## 1 Ausrichtung (eigene Messung)

Die Abbildung geht von der Grundlage zur Vorlage: v = s·R(θ)·g + t. Zwei unabhängige Verfahren:
- **A:** SIFT auf CLAHE-Grau (Grundlage mit Alpha-Maske) mit Ratio-Test 0,8, danach RANSAC-Ähnlichkeit.
- **B:** dichte Ähnlichkeit (s, t, θ) über die NCC der Gradientenbeträge. Dafür wird die Vorlage auf das Zweifache vergrößert, der Optimierer ist Nelder-Mead, gestartet bei A.

| | Bild 1 (`Rückseite.png`) | Bild 2 (`birkin_form3_02.png`) |
|---|---|---|
| A: s / Faktor / θ / t | 0,50156 / 1,9938 / 0,038° / 172,58·180,62 (12 Inlier, Rest median 0,53 px) | 0,50154 / 1,9939 / −0,108° / 192,83·150,14 (19 Inlier, Rest median 0,78 px) |
| B: s / Faktor / θ / t | 0,50023 / 1,9991 / 0,039° / 172,77·181,59 | 0,50014 / 1,9994 / −0,109° / 193,08·150,73 |
| B: NCC eigene / mit Parametern des Messers | 0,806 / 0,796 | 0,818 / 0,818 |
| gespiegelt: beste NCC (B, ±30 px gesucht); A Inlier | 0,039; 3 von 40 | 0,064; 2 von 33 |
| B gegen Ähnlichkeit des Messers über der Figur (8-px-Gitter), median / p90 / max | 0,44 / 0,56 / 0,63 px | 0,26 / 0,43 / 0,50 px |
| A gegen Messer, max | 0,78 px | 0,73 px |
| lokaler Rest: 96-px-Kacheln, Gradienten-NCC ≥ 0,4, Suche ±12 px; median / p90 / max | 0,71 / 1,5 / 2,9 px (28 Kacheln) | 1,58 / 2,5 / 3,5 px (31 Kacheln) |
| Messblatt: Kachel-Rest median / p90 / max | 1,58 / 2,71 / 5,95 | 2,58 / 7,88 / 10,61 |

- **Beschriftete Bilder:** Gesucht wurde der Versatz der Grundlage mit der höchsten Zahl gleicher Pixel nach Alpha-Verrechnung.
  - dx: Rücken 0, Kopf 0, rechtes Bein hinten 0, Bein_Vorne_rechts 0, Vorne_Oberkörper 0, Vorderes_Linkes_Bein 0, linkes Bein hinten 223, Schwanz 425, Fuß 375, Hand 282, Gesicht 401.
  - dy ist überall 0. Alle 11 Werte stimmen mit dem Messblatt überein.
- **Pixelgleichheit:** Abweichende Pixel mit mehr als 2 px Abstand zu Linienpixeln (Paint-Palette, Toleranz 90) gibt es nur in `linkes_bein_hinten.png`.
  - Dort sind es 64 px in 29 Gruppen, die größte hat 10 px. Sie sind übers Bild verstreut, keine liegt in einer geprüften Zone.
  - Das Messblatt nennt 627 px, mit anderer Schwelle. Die Aussage stimmt.
- Für die Zonen habe ich meine eigene Abbildung B benutzt.

## 2 Zonen: Umriss selbst gezogen, Flächen selbst bestimmt

**Umrisse:**
- Linienpixel = Pixel, die von der Grundlage abweichen und höchstens 60 von einer Paint-Palettenfarbe entfernt liegen.
- Umrisse habe ich als Flutfüllung gebaut: Wandfarben um 2 px verdickt, die Region um 2 px zurückgegeben und auf das Alpha der Grundlage geschnitten.
- Linienenden sind Endpunkte des Skeletts.
- Punkt-Zonen bilde ich **nicht** als Kreis, sondern als hellen Fleck in der Grundlage, als Zusammenhangskomponente am Linienende:
  - weiß: L* ≥ 70 und Chroma ≤ 30
  - Augen: L* ≥ lokaler Median (21 px) + 12
- Übertragung in die Vorlage mit `warpAffine` (INTER_AREA), Schwelle 0,5.

**Flächen:**
- Gezählt habe ich (Mesh, Viereck, Index) je Pixel aus meinem Kennungs-Render.
- Anteil = Pixel in der Zone ÷ sichtbare Pixel der Fläche; Kern = Anteil ≥ 0,5.
- Für den Vergleich habe ich die Polygone des Messers gerastert (IoU) und seine Liste `flaechen_sichtbar` herangezogen.

### 2.1 Tabelle

| Zone | mein Bau | px ich / Messer | IoU | Flächen ich / Messer / beide | nur Messer | nur ich | Urteil |
|---|---|---|---|---|---|---|---|
| fleisch_vorn („großer fleischfarbener Teil“) | Flut in `Vorne_Oberkörper.png`, Wände rot + grün, Saat 300/200 | 14 400 / 14 746 | 0,976 | 99 / 102 / 99 | 3 Fl., 3 px (24, 22, 11) | 0 | bestätigt. Meshes ich 10: 5648, 22: 4714, 21: 3568 (Messer 5791 / 4799 / 3622); Kern 76 / 78, beide 76 |
| oberkoerper_vorn | Flut, Wand rot | 18 072 / 18 425 | 0,981 | 119 / 123 / 119 | 4 Fl., 4 px | 0 | bestätigt |
| zipfel | Flut in `Schwanz_hinten.png`, Wand blau, Saat 600/540; unten schließt der Bildrand | 2 839 / 2 974 | 0,955 | 24 / 24 / 24 | 0 | 0 | bestätigt. 6: 2692, 5: 130 (Messer 2809 / 144); Kern ohne [5,1,63] |
| vorderbein_links („mit Krallen“) | Flut, Wände rot + hellblau, Saat 400/300 | 6 443 / 6 751 | 0,951 | 97 / 99 / 96 | 3 Fl., 11 px (12: 9, 13, 11) | 1 Fl., 2 px (Fuß 25) | bestätigt |
| krallen_vorn_links | die 4 geschlossenen blauen Schleifen | 370 / 455 | 0,763 | 16 / 19 / 16 | 3 Fl., 3 px (13, 15, 30) | 0 | bestätigt. 15: 198, 13: 65, 14: 5 |
| weisse_musterung_vorderbein_rechts | Flut, Wände rot + grün, Saat 460/208 | 206 / 250 | 0,824 | 7 / 7 / 7 | 0 | 0 | bestätigt, 16: 202 |
| kopf_hinten | Flut, Wände rot + grün + lila, Saat 480/190 | 621 / 742 | – | – | – | – | bestätigt: im Modell nur Oberarm 16 (597 px, Deckung 0,96) |
| **weisse_musterung_bein_hinten_rechts** | heller Fleck am Linienende 315/219 (enthält das Ende) | 105 / 49 | 0,363 | 5 / 2 / 2 | 0 | **3 Fl., 14 px auf Oberschenkel 9** | **zu klein.** Die Zone des Messers deckt 39 % des Flecks und liegt zu 84 % in ihm. Fleck: Hüfte 0 mit 91 px, Oberschenkel 9 mit 14 px |
| **weisse_musterung_bein_hinten_links** | heller Fleck, 5 px rechts neben dem Linienende 352/209 (beschriftet x 355–367, y 208–221) | 23 / 48 | **0,06** | 4 / 5 / 3 | 2 Fl., 15 px (8: 12, 0: 3) | 1 Fl., 2 px (3) | **falsch platziert.** Der Kreis liegt links neben dem Fleck auf dunkler Haut (Video-L* 30 gegen L* 65 im Fleck). Fleck: **Oberschenkel 8** mit 20 px. Zone des Messers: Hüfte 0 mit 33 px, 8 mit 15 px |
| **augen_vorn** | 2 helle Punkte 6 bzw. 3 px rechts neben den Linienenden 440/245 und 567/248 (beschriftet 446–452 / 244–250 und 570–578 / 246–253) | 18 / 44 | **0,05** | 5 / 9 / 2 | 7 Fl., 33 px (22: 26, **0: 7**) | 3 Fl., 11 px (22) | **verfehlt.** Die Kreise decken 17 % der Augenpunkte, 93 % der Kreise sind Haut (Video-L* 10 gegen 18). Hüfte 0 in der Liste des Messers ist kein Auge |

Die Linienenden des Messers habe ich nachgemessen und an keiner Stelle Abweichungen gefunden:
- weiß r 315/219, weiß l 352/209, Vorderbein weiß 455/204
- Augen 441/245 und 568/248
- Hand-Enden 442/511, 448/583, 301/613, 412/584

Der Fehler in Zeile 8–10 liegt im Bau der Zone, nicht in den Linienenden. Der Kreis sitzt auf dem Ende der Hinweislinie, die gemeinte Stelle liegt aber daneben.

### 2.2 Fehlender Strich

`linkes_bein_hinten.png` enthält einen zweiten grünen Strich, der nicht zum Text führt:
- Er läuft von 341/180 nach 366/197 (100 px), knapp oberhalb des weißen Flecks.
- Weder die Linienend-Tabelle noch `zonen.json` verwenden ihn.
- Was er bedeutet, ist offen. Möglich ist die Oberkante der weißen Musterung.

### 2.3 Kopfzonen in Bild 2

Die Abweichung habe ich selbst nachgemessen:
- Modellkopf 22 sichtbar bei x 212–348, y 196–374. Das Nutzer-Gesicht reicht von x 208–349, y 266–406. Der Kopf sitzt also **70 px zu hoch**.
- Von 16 561 px aus Kopf und Kiefer liegen 5 582 außerhalb des Nutzer-Gesichts.
- Der Kopf liegt zu 100 % in der Video-Silhouette. Er deckt dort also den Oberkörper des Videos.

Folge für die Flächenlisten, die nach Bildlage übertragen werden:
- „fleisch_vorn“ enthält 4 714 px von Kopf 22 (Messer: 4 799).
- Die „Augen“-Flächen liegen an einer anderen Stelle des Modellkopfs als die Augen im Video:
  - Im Video sitzen die Augenpunkte (y 271–277) etwa 9 px unter der Gesichtsoberkante.
  - Im Modell liegt dieselbe Bildhöhe 78 px unter der Oberkante des Kopfs 22.
- Diese Listen beschreiben die Video-Lage, nicht das Modellteil. Solange Form oder Pose des Kopfes abweichen, eignen sie sich nicht als Malvorlage.
- Das Messblatt nennt die Abweichung, verwendet die Listen aber unverändert.

In Bild 1 bestätigt:
- Kopf 22: 0 px sichtbar. Projiziert ohne Verdeckung liegt er bei x 336–404; der Nutzer-Kopf liegt bei x 402–430.
- Kiefer 23: 125 px bei x 365–377, y 294–314.

**Nicht geprüft:** Wunde, Musterung Rücken l/r, Schwanz und Schwanz-Muster, Bein-Abschnitte, bräunliche Musterung, Maul, Unterkiefer, Hand-r-Zonen, Ganzteil-Ergänzung, `namensteile`, Lab-Spalte.

## 3 Krallen (Nutzerbild und Modell, selbst gezählt)

**Nutzer:**
- Krallenformen = geschlossene Gebiete zwischen blauen (und roten) Linien, je 15–6000 px groß.
- Spitze = spitzeste Ecke nach `approxPolyDP` (2,5 px).

**Modell:**
- Spitzen sind Ecken, deren Nachbarn alle in eine Richtung liegen (|Mittel der Einheitsvektoren| > 0,85).
- Projektion mit derselben Kamera, ohne Verdeckung.
- Zehen der Füße: Spitzen der Silhouette mit Winkel < 60° bei k = 7, aus 32 Ansichten (8 Azimute × Elevation ±60/±85, Mesh einzeln).

**Eingezogen im Asset:**
- Ausdehnung der Krallen im Asset: 19 = 32×13×13, 20 = 35×17×15, 28 = 30×8×17, 29 = 39×12×23.
- In K5c: 809×322×323, 870×433×394, 740×217×432, 973×286×568.
- Je Hand gibt es 4 Krallen-Meshes (l 14/15/28/30, r 19/20/29/31). Im Asset wirken links 3 und rechts 1.

### Vorne links (Bild 2), „Exakt 4 Krallen“

- Am Nutzerbild gezählt: **4 geschlossene blaue Schleifen**. Die Angabe „exakt 4“ ist damit bestätigt.
- Im Asset wirken links nur 3 Krallen (28 ist eingezogen).

| Schleife (beschriftet) | Nutzerspitze (Vorlage) | Asset in der Schleife | K5c in der Schleife | nächste Modellspitze (K5c) |
|---|---|---|---|---|
| 1: 535–554 / 475–489 | 470,6 / 394,8 | – | 28: 8 px | **Kralle 28: 466,0 / 393,1 (4,9 px)** |
| 2: 419–426 / 480–490 | 406,1 / 395,4 | – (Spitze: 30 mit 1 px) | – | Kralle 30: 408,3 / 393,4 (3,0 px) |
| 3: 496–548 / 493–527 | 467,7 / 413,8 | 15: 198, 13: 4 | gleich | Kralle 15: 467,9 / 418,0 (4,2 px) |
| 4: 467–490 / 502–530 | 438,7 / 415,3 | 13: 61, 14: 5 | gleich | Hand 13: 439,0 / 419,0 (3,7 px); Kralle 14: 445,7 / 414,8 (7,0 px) |

- **Folgerung „28 zurück“:** bestätigt durch die Zahl und durch die Lage der Spitze.
- Wohin die vierte Schleife gehört, ist offen: Spitze von Hand 13 oder Kralle 14.
- Kralle 14 ist sichtbar mit 357 px. Davon liegen nur 5 px in einer Nutzerschleife und 329 px im Nutzer-Bein außerhalb der Schleifen.
- K5c-Kralle 28: 292 px, davon 96 % in der Video-Silhouette und 260 px im Nutzer-Bein.

### Vorne rechts (Bild 2), „Krallen“ ohne Zahl

- Am Nutzerbild gezählt: **2 geschlossene Krallenformen**:
  - Form 1 (beschriftet 422–451 / 521–577): Ihre Strichenden 442/511 und 448/583 sind ihr oberes und unteres Ende.
  - Form 3 (309–319 / 579–601): endet bei 301/613.
- Dazwischen liegt eine Fläche von 2 639 px, oben blau und unten rot begrenzt; das Wasserzeichen liegt darin. Ob dort weitere Krallen sind, zeigt das Bild nicht.
- Die „3 Spitzen“ des Messers zählen Form 1 doppelt, die Zahl 3 ist nicht belegt.
- Die übrigen freien Enden sind 412/584 (Ansatz der Hinweislinie) und die Schrift „Krallen“.

| Nutzerform | Asset | K5c | Spitzenabstand |
|---|---|---|---|
| Form 1 (252 Vorlage-px) | **31: 182**, 18: 6 | gleich | Kralle 31: 273,2 / 457,3, Nutzerspitze 275,7 / 439,2 (18 px) |
| Form 3 (48 px) | Hand 18: 47 | **19: 46**, 18: 2 | Kralle 19: 203,8 / **499,6**, Nutzerende 203 / 457: **42 px tiefer**, außerhalb des Bildes |
| Fläche dazwischen (733 px) | 18: 392 | 19: 335, 18: 196 | – |

- **19:** 1 521 px in K5c, davon 78 % in der Video-Silhouette.
  - Folgerung „19 zurück“: Die Lage ist bestätigt, die Länge nicht. In K5c-Geometrie ragt 19 über die gezeichnete Kralle hinaus.
- **20 und 29:** 1 734 bzw. 1 479 px in K5c, davon **0,7 % bzw. 0 %** in der Video-Silhouette.
  - Sie stünden in dieser Pose frei vor dem Hintergrund, wo das Video nichts zeigt. Ihr Einziehen ist gedeckt.
- **Hand 18:** Ihre eigene Spitze projiziert auf 169,6 / 477,1. Nur 66 % von Hand 18 liegen in der Video-Silhouette.

### Hinten (Bild 1), „Kaum sichtbar, aber wahrscheinlich 3“

- **Nutzer:** 2 kurze rote Krallenstriche entlang heller Lücken der Freistellung und 1 Kantenstrich. Eine Zahl ist nicht gezeichnet.
- **Modell:**
  - Fuß 25 hat 3 Zehenspitzen in 32 von 32 Ansichten.
  - Fuß 27 hat 3 in 31 von 32 Ansichten (eine Ansicht zeigt 2, Verdeckung).
  - Das stimmt mit „wahrscheinlich 3“ und „rechts genauso“ überein.

| Strich (beschriftet) | Vorlage | Deckung durch das Modell | Meshes |
|---|---|---|---|
| Kralle 1: 518–533 / 384–393 | 243–252 / 373–379 | **0,08** | Schwanz 5: 4 px |
| Kralle 2: 517–538 / 423–454 | 243–255 / 392–409 | **0,26** | Schwanz 5: 23 px |
| Kante: 460–470 / 436–497 | 214–221 / 399–431 | 0,41 | Fuß 25: 52 px |

- Sichtbarer Fuß 25: x 192–239, y 392–415. Schwanz 5: x 244–289.
- Die Krallenmarken liegen 4–16 px rechts neben dem Modellfuß, fast ganz über Hintergrund.
- Das Messblatt sagt „liegen auf Schwanz 5“. Das stimmt nur für 4 bzw. 23 Randpixel; im Modell fehlt dort der Fuß.

## 4 Seiten (aus Sicht des Wesens)

**Blickrichtung über die Tiefe** (Mittel der Kamera-z aller Vorderflächen je Mesh):

| Bild | Kopf 22 | Kiefer 23 | Schwanzspitze 6 | Ergebnis |
|---|---|---|---|---|
| Bild 1 | 9 005 | 9 291 | 2 949 | Wesen von hinten, seine linke Seite liegt links im Bild |
| Bild 2 | 4 601 | – | 10 338 | Wesen von vorn, seine linke Seite liegt rechts im Bild |

**Bildlage der l/r-Teile** (Mittel x, Vorlage):

| Bild | linke Teile | rechte Teile |
|---|---|---|
| Bild 1 | Oberschenkel 8: 242, Unterschenkel 24: 217 | Oberschenkel 9: 323, Unterschenkel 26: 320, Oberarm 16: 399 |
| Bild 2 | Oberarm 11: 392, Hand 13: 417, Unterschenkel 24: 364 | Oberarm 16: 281, Hand 18: 243, Unterschenkel 26: 281 |

Alle Teile liegen auf der erwarteten Seite.

**Nutzerwörter:**
- Bild 1:
  - „linkes Bein“ (hinten) und „linker Fuß“ links im Bild; Modell 8/24/25.
  - „rechtes Bein hinten“ rechts davon; 9/26.
  - „rechtes Bein und Fuß“ (vorn) ganz rechts; 16–18.
  - „Musterung links“ (beschriftet 135/135) links; „Musterung rechts“ (rechts davon) innerhalb des Oberkörpers.
- Bild 2:
  - „vorderer rechter Arm / Hand“ links im Bild; 18/31.
  - „Vorderes linkes Bein“ rechts im Bild; 11–15.
- Alle Seitenwörter stimmen aus Sicht des Wesens. Die Seitenangaben des Messers sind bestätigt.

**Einzige Ausnahme:**
- „Gesicht mit Maul nach unten rechts blickend“: Das Maul liegt rechts unten im Bild (beschriftet 520–700 / 335–515). Aus Sicht des Wesens ist das seine linke Seite.
- Gemeint ist also die Bildrichtung. Keine Zone hängt davon ab; das zählt erst, wenn jemand daraus eine Kopfdrehung ableitet.

## 5 Zusammenfassung der Abweichungen zum Messblatt

1. Zone `weisse_musterung_bein_hinten_links`: falsch platziert. Richtig ist der Fleck auf Oberschenkel 8, 5 px rechts neben dem Linienende.
2. Zone `augen_vorn`: verfehlt die Augenpunkte, die 3–6 px rechts neben den Linienenden liegen.
3. Zone `weisse_musterung_bein_hinten_rechts`: deckt nur 39 % des Flecks. Es fehlen Flächen von Oberschenkel 9.
4. Krallen vorne rechts: 2 gezeichnete Krallen, nicht 3 Spitzen. Kralle 19 trifft die Lage, ist in K5c-Geometrie aber 42 px zu lang.
5. Krallen hinten: Die Marken liegen über Hintergrund neben Fuß 25, nicht auf Schwanz 5.
6. Kralle 28: Die Spitze trifft die vierte Nutzerspitze auf 4,9 px. Das stützt das Zurückholen stärker als das Messblatt.
7. Ausrichtung Bild 2: Rest p90 2,5 px statt 7,9 px. Die Vorsicht bei kleinen Bild-2-Zonen ist so nicht durch die Ausrichtung begründet.
8. Ein grüner Strich in `linkes_bein_hinten.png` (341/180–366/197) wird nicht verwendet.
9. Die Flächenlisten der Kopfzonen in Bild 2 treffen wegen des um 70 px versetzten Modellkopfs andere Modellteile.
