# Farbe Versuch 3, Leser B: Feuerfarben an Gesicht und Maul, schwarz-rote Flaechen

Stand 2026-11-07, Arbeitsbaum r41_form (HEAD 02aed15f7), kein Commit. Messung an zwei Videostandbildern, kein RE-Wert.
Seiten: (W) = aus Sicht des Wesens, (B) = Betrachter. Farben RGB 0..255 und CIE-Lab (D65, sRGB, OpenCV).
**V** = Videorohwert, **e** = Schirmfarbe (Rueckrechnung `form_video.video_umkehr`, danach Gauss 1,0 wie Leser B V2),
**T** = Texelschaetzung = e / Licht. Licht nach `farbe_palette.json`: Bild 1 Figur (0,487/0,432/0,374), Bild 2 (0,385/0,342/0,307).
H = Figurhoehe: Bild 1 295 px (y 184-478, `ziel2_bild1.png` = 255), Bild 2 331 px (y 142-472); Modell K5c stehend 3149 Einheiten.

Belegbilder (kein Bild aufgehellt; e-Fassungen roh):
`C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe_lesung/V3B_1_bild1_F1_x4.png`,
`.../V3B_2_bild2_maul_x4.png`, `.../V3B_3_schwarzrot_x3.png`, `.../V3B_4_gegenprobe.png`.
Skripte und Rohzahlen: `.../farbe_lesung/V3B_skripte/` (`messen.py`, `flaechen_id.py`, `idkarten.py`, `blaetter.py`, `messung.json`, `idkarten.json`;
die .npy-Zwischenstaende liegen im Scratchpad `.../scratchpad/formfarbe3/leserb/`).

## 0. Kurz

1. **Bild 1, die Stelle, die der Nutzer als leuchtendes Gesicht liest (F1):** 708 px, bbox (300,221)-(331,258), Schwerpunkt (317,4/240,9),
   41 x 20 px, Achse 120 Grad (= Z1/A01 der frueheren Blaetter). Schirmfarbe e (66,41,41), Lab 19,6/12,0/4,4, Farbton 20 Grad: **lachsrot**.
   Das Rosa-Magenta im Video ist Videofarbe (Blauanhebung Bild 1, farbe.md 6).
2. **In der festen K5c-Geometrie liegt F1 nicht auf dem Gesicht.** 706 von 708 px fallen auf Knochen 0 (Becken, Oberseite hinten).
   Hals 21 und Kopf 22 sind in der Bild-1-Pose ganz verdeckt, vom Unterkiefer 23 sind 125 px sichtbar (x 365-377, y 294-314).
   Unverdeckt laege Hals/Kopf bei bbox (319,230)-(403,314). F1 beruehrt davon nur den Hals (98 px = 14 %), den Kopf 22 gar nicht.
   Soll Bild 1 stimmen, muss das Leuchten auf Knochen 0 gemalt werden (Flaechen in Abschnitt 1.3). Am Gesicht waere es in Bild 1 unsichtbar.
3. **Bild 2, "Feuerfarbe beim Mund":** gemeint ist das Unterkieferband. Es zerfaellt in drei Teile:
   **Fleischband** (56,5 % des Unterkiefers, T (177,89,84), Lab 48,8/35,1/20,4, Farbton 32 Grad),
   **Zaehne** (18,2 %, T (199,191,183), Lab 77,7/2,1/4,8, neutral wie der Kittel der Frau) und **gelbe Saeume** (5,5 %, T (209,160,118), Lab 69/13/29).
   Die drei weissen Flecken links unten (x 164-216, y 331-383) sind geclippt und liegen ausserhalb der sicheren Figur und aller Nutzer-Teile.
   Sie tragen keine Farbaussage (Feuer oder Krallenspitzen, offen). Gesicht und Schnauze darueber sind **dunkel blutrot**: T (71,24,26), Lab 16/22/11, 98 % dunkel.
   Die Augen sind dunkel (E24 gilt). Das Leuchten liegt mindestens 43 px unter der Augenlinie.
4. **Gegenprobe:** Eine einzige Texelfarbe erklaert F1 (Bild 1) und das Fleischband (Bild 2), **wenn F1 nahe am Umgebungslicht liegt**.
   Gemeinsame Anpassung **T (174,95,96), Lab 49/32/14, Farbton 24 Grad**, chi^2 2,5 bei 2 Freiheitsgraden (p 0,28), alle Reste <= 0,92 sigma.
   Mit dem festen Figurlicht Bild 1 ist es knapper: (163,91,96), chi^2 6,8 bei 3 Freiheitsgraden (p 0,08).
   **Gesaettigte Feuertexel scheitern deutlich:** Orange (255,140,40) chi^2 170, Gelb (255,200,60) chi^2 265, Rot (220,60,30) chi^2 98.
   Im Video stuenden sie in Bild 1 als Beige-Weiss (218,181,122) statt Rosa (145,85,130), dE 64-76.
   "Leuchten" heisst gemessen: ein gedecktes Lachsrot mit L* 47-49 auf einer Haut mit L* 13-15. Gelb-Orange gehoert nicht dazu.
5. **Schwarz-rot (Ziel fuer die G4-Uebertragung):** Bild 1 Figur 62,5 % dunkel (T L* < 20, Mittel (37,32,37)), 15,8 % dunkelrot, 3,9 % rot (Lab 39/21/-1).
   Bild 2 Figur 53,4 % dunkel (Mittel (64,22,25)), 51,4 % dunkelrot, 31,9 % rot (Lab 34/27,5/17, Farbton 34 Grad, vor allem Klauen und Kiefer).
   Das Rot von Bild 2 ist in den dunklen Teilen nach oben unsicher, weil dort bis +21 Rot-Einstreuung in e moeglich ist (farbe.md 4).
   Das Bahnsteig-Rot (Bild 1) hat den Farbton -8 Grad (Figurlicht) bzw. +17 Grad (Licht nahe Ambient). 34 Grad ist es nicht (vgl. Ablehnung V2).

## 1. Bild 1 (Rueckansicht Bahnsteig, blaues Licht): Stelle F1

### 1.1 Abgrenzung

Regel: e-Lab L* >= 12 und a* >= 8, Zusammenhang mit (325,232), innerhalb `ziel2_bild1.png` >= 128 (PORT-WAHL dieses Lesers).
Die Umgebung liegt bei e L* 3-7, das Querband A02 darunter bei a* 2-7: Beide fallen heraus.
Die Flaeche haengt so an den Schwellen (px): L>=8,a>=6: 1178 | L>=10,a>=8: 773 | **L>=12,a>=8: 708** | L>=12,a>=10: 585 | L>=16,a>=10: 353 | L>=20,a>=10: 249 | L>=25,a>=10: 83.
Kern (L* >= 20): 307 px, bbox (305,224)-(329,258), Schwerpunkt (320,1/236,8), 37,5 x 14,3 px, Achse 115 Grad.
Das deckt sich mit `bewertung_bild.md` Z1 (670 px, (318,240), 44 x 21, 120 Grad).

### 1.2 Farbe (708 px, nichts geclippt)

| Stufe | RGB Mittel (Std) | Lab Pixelmittel (Std) | Farbton |
|---|---|---|---|
| V roh | (145,85,130) (35,35,32) | 44,3/31,5/-14,6 (13,1/6,0/4,5) | magenta (Video) |
| V Kern | (178,117,160) | 56,9/30,3/-13,4 | |
| e | (66,41,41) (13,11,9) | 19,6/12,0/4,4 (5,1/2,3/2,2) | 20 Grad, Chroma 13 |
| e Kern | (78,51,50) | 24,5/11,8/5,9 | 24 Grad |
| T, Figurlicht (0,487/0,432/0,374) | (136,94,111) (26,26,24) | 44,7/19,6/-2,5 (10,0/4,5/3,5) | -9 Grad |
| T, Licht wie Leons Gesicht (0,339/0,338/0,349, Ambient) | (195,121,119) | 58,8/29/13 | |
| T, Licht wie Leons hellste Stelle (0,702/0,633/0,493) | (94,64,84) | 31,4/16/-7 | |
| T Kern, Figurlicht | (160,119,133) | 54,4/18,4/-1,4 | -6 Grad |

K5c-Render mit Szenenlicht: Das Licht am Ort von F1 (sauber/Albedo) liegt im Median bei (0,488/0,431/0,375), gleich dem Figurlicht.
Leons Spanne (farbe_palette.json licht.bild1) macht die Helligkeit von T zu einer Spanne von Faktor 2. Der Farbton von T liegt zwischen -9 und +24 Grad, je nachdem, welches Licht die Stelle trifft.
Die frueheren Werte A01 e (88,59,56) und T (181,137,150) aus `farbe.md` sind Kernwerte (heller Teil); die Fläche oben ist groesser.

### 1.3 Lage auf dem Koerper (K5c, Pose Bild 1 = Clip 0, Bild 0, Kamera `kamera_bild1.json`)

- Knochen-ID je Pixel aus eigenem Rasterer `flaechen_id.py`; stimmt zu 99,95 % mit `form_pruefung.teilbild` ueberein.
- F1: **Knochen 0 (Becken) 706 px**, Knochen 2 (Schwanz) 2 px. Flaechen (Zaehlung tf, danach qf; Mesh 0 hat 88 Dreiecke):
  107 (252 px), 119 (233), 108 (183), 120 (25), 95 (10), 96 (3). Das sind die Vierecke qf 19, 31, 20, 32, 7, 8.
- Knochenraum Knochen 0: Mittel (-576,-313,-486), von (-753,-564,-597) bis (-355,-103,-343). Auf der Oberflaeche 567 x 243 Einheiten (p1-p99),
  also 0,18 x 0,08 H_Modell. Im Bild 0,14 x 0,07 H. Kameratiefe z 5104.
- Kopf in dieser Pose: Hals 21 0 px sichtbar, Kopf 22 0 px, Unterkiefer 23 125 px (x 365-377, y 294-314). Projektion von 21-23 ohne Verdeckung:
  bbox (319,230)-(403,314), Schwerpunkt (361,9/270,5), Achse 44 Grad. F1 deckt davon 98 px, alle auf Hals 21. Schwerpunkt-Abstand 5,7 px zum Rand der Projektion.
- **Folgerung:** Die Lesart "Gesicht vorn leuchtet durch" ist mit der festen K5c-Geometrie nicht abbildbar: Das Gesicht steht hinter dem Hueftbuckel.
  Sichtbar wird F1 nur, wenn es auf diese Becken-Flaechen gemalt wird. `FORMBLATT.md` 6 D fuehrte als Deutung schon "Vorsprung am Ende N (Kopf oder Hals zur Kamera)"
  und verwarf sie damals nicht. Gewaehlt wurde die Huefte. Ob ein Leuchten auf dem Becken zur Geschichte passt (Brandwunde), entscheidet der Messblatt-Pruefer.
- Weitere rote Stellen in Bild 1 (gleiche Regel, >= 25 px): Schwanzende X (258-283, 422-443) 265 px, T (145,107,125) Lab 49/18/-4, Knochen 6/5;
  (262-277, 461-479) 256 px, T Lab 43,5/25/1, Knochen 6; sonst nur kleine Punkte <= 60 px (Knochen 0, 2, 3, 8).
  Ausser F1 und dem Schwanzende gibt es in Bild 1 kein Leuchten.

## 2. Bild 2 (Vorderansicht 5090 Cut 11, Feuerlicht): Maul

### 2.1 Klassen (e-Lab, PORT-WAHL dieses Lesers; ROI x 140-349, y 290-424 ohne Logo)

dunkel: L* < 8 | Band rot: L* >= 8, a* >= 12, nicht b* > a* + 3 | hell neutral: L* >= 18, a* < 8, b* < 15 | gelb: L* >= 12, b* >= 15, b* > a* + 3 | geclippt: Video min(R,G,B) >= 200.

### 2.2 Unterkiefer (Nutzer-Teil `maske_unterkiefer.png`, 3355 px)

| Teil | Anteil | Lage (bbox, Schwerpunkt) | V | e Lab (Std) | T RGB (Std) | T Lab (Std) | Ton T | K5c-Knochen (px) |
|---|---|---|---|---|---|---|---|---|
| **Fleischband** | 56,5 % (1896) | (243,324)-(341,408), (287,9/370,4), 105 x 37 px, 145 Grad | (161,42,53) | 16,9/17,7/11,3 (5,9/3,9/4,0) | (177,89,84) (34,41,42) | 48,8/35,1/20,4 (13,7/9,6/11,2) | 32 | 23: 1131, 22: 337, 18: 166, 24: 38 |
| **Zaehne** (hell neutral) | 18,2 % (612) | (255,318)-(339,407), (303,4/351,2) | (182,110,121) | 28,5/3,4/7,3 (4,5/2,8/5,4) | (199,191,183) (28,32,32) | 77,7/2,1/4,8 (10,7/6,9/13,6) | (72, C* klein) | 23: 307, 22: 278 |
| Gelber Saum | 5,5 % (185) | (278,352)-(315,397), (301,7/363,5) | (190,86,74) | 25,5/9,1/16,4 | (209,160,118) | 69,4/13,0/29,0 | 65 | 23: 168 |
| Uebergang (mittel) | 16,2 % (543) | (248,318)-(341,408) | (169,76,96) | 23,1/9,3/6,0 | (186,146,151) | 64,2/16,3/2,9 | 0 | 23: 282, 22: 171 |
| dunkel | 3,5 % (117) | Rand oben rechts | (94,9,22) | 6,8/17/6,1 | (113,27,33) | 25,4/36,7/19,2 | 29 | 24: 54, 22: 20 |
| geclippt | 0,1 % | | | | | | | |

Groesse des Fleischbands: 0,32 x 0,11 H im Bild. Auf K5c-Knochen 23: Ausdehnung 1067 x 332 Einheiten, Knochenraum von (102,-259,-241) bis (940,39,638).
Flaechenlisten je Knochen in `messung.json` B_bild2_maul.unterkiefer.*.koerper.

**Trennung Zaehne / Lippe / Feuerschein:**
- Zaehne sind Oberflaeche und weiss: T Lab 77,7/2,1/4,8 gleicht dem Kittel der Frau (Leser B V2: T Lab 79/1/7). Feuerlicht faerbt nicht (ROOM5090 hat nur Ambient, alle Lichtfarben 0, farbe.md 5).
- Das Fleischband ist die "Feuerfarbe": lachsrot, T Farbton 32 Grad, e Farbton 33 Grad. Mit den Zaehnen als Weissbezug (Leser B V2): (214,132,137).
- Gelber Saum: Er liegt nicht naeher am geclippten Feuer als das Band (Abstand zu geclippten Pixeln ausserhalb der sicheren Figur: Median 46 px gegen 37 px beim Band, p10 29 gegen 17).
  Gegen Feuerschein spricht der Abstand, fuer Feuerschein der Farbton. Belegt ist keins von beiden (offen). Als Zahnbasis lesbar.
- Die drei weissen Flecken links unten: geclippt, (164-181,332-362) 322 px, (182-202,344-383) 433 px, (202-216,331-351) 208 px. Sie liegen nur im unsicheren Saum von `ziel2_bild2`
  (128), in keinem Nutzer-Teil, K5c deckt 5 von 963 px. Keine Farbaussage (farbe.md 1: Weiss im Video heisst nicht weiss im Spiel).
  `FORMBLATT.md` 4: "K2 (206,387) liegt unter Feuer". Die KI-Fassung zeigt sie als fingerartige weisse Formen: Feuer oder Krallenspitzen, offen.

### 2.3 Gesicht vorn in Bild 2 (gegen G2 und E24)

- Oberkiefer im ROI (5606 px): 97,9 % dunkel, T (71,24,26) (9,9,12), Lab 16,1/22,4/10,8, Farbton 28 Grad. Gesicht und Schnauze leuchten in Bild 2 **nicht**, sie sind dunkel blutrot.
  Innenflaeche des Kopfes (`FORMBLATT.md` 2.3): V (40,6,16). Das bestaetigt es.
- Augen (3 x 3 px): rechtes Auge (W) (217,275) V (57,23,32), T (77,64,49), Lab 28/3/11; linkes Auge (W) (280,275) V (77,31,47), T (102,82,83), Lab 37/9/3.
  Etwas heller und weniger rot als das Gesicht, nicht leuchtend. E24 "Augenstellen dunkel" bleibt.
- Abstand: Augenlinie y 275, Oberkante Fleischband y 324 (Zaehne ab y 318). Das Leuchten in Bild 2 sitzt am Unterkiefer (Knochen 23) und an der Kieferkante von Knochen 22, nicht an Stirn oder Augen.

## 3. Gegenprobe: eine Texelfarbe fuer beide Bilder (V3B_4)

Gemessen: e1 = F1 (66,1/40,7/41,4), e2 = Fleischband (68,5/30,5/25,9). Sigma je Kanal fuer den Flaechenmittelwert: Balance (Bild 1 +-8 %, Bild 2 +-(0,02/0,03/0,04) auf (0,951/0,665/0,620),
farbe.md 3) und Bias-Breite der Rueckrechnung (Bild 1 5, Bild 2 4 e-Stufen, farbe.md 4). Ergebnis: sigma e1 (7,3/6,0/6,0), e2 (4,3/4,2/4,3).

| Texel | Lab | Reste Bild 1 (sigma) | Reste Bild 2 (sigma) | chi^2 | dE e1 / e2 | Video dE Bild 1 / Bild 2 |
|---|---|---|---|---|---|---|
| T aus Bild 1 (136,94,111) | 44,7/20/-3 | 0 | -3,8 / 0,4 / 1,9 | 18,3 | 0 / 11,5 | 4,4 / 20,1 |
| T aus Bild 2 (178,89,84) | 48,6/36/20 | 2,8 / -0,4 / -1,7 | 0 | 10,8 | 13,7 / 0 | 24,6 / 2,4 |
| gemeinsam, Figurlicht (163,91,96) | 47,1/30/10 | 1,8 / -0,2 / -0,9 | -1,4 / 0,2 / 0,8 | 6,8 (3 FG, p 0,08) | 8,6 / 4,3 | 15,2 / 9,0 |
| **gemeinsam, Bild-1-Licht frei (174,95,96)** | **49,4/32/14** | 0,58 / -0,59 / -0,92 | -0,36 / 0,47 / 0,81 | **2,5 (2 FG, p 0,28)** | | |
| Probe Orange (255,140,40) | 69,8/37/67 | 8,0 / 3,3 / -4,4 | 7,0 / 4,1 / -3,1 | 170 | 37,9 / 22,2 | 64,5 / 20,3 |
| Probe Gelb (255,200,60) | 83,5/7/73 | 8,0 / 7,7 / -3,2 | 7,0 / 9,0 / -1,7 | 265 | 41,2 / 29,2 | 75,9 / 33,0 |
| Probe Rot (220,60,30) | 50,2/60/53 | 5,7 / -2,5 / -5,0 | 3,8 / -2,4 / -3,9 | 98 | 33,4 / 16,3 | 44,0 / 16,0 |
| Probe Lachs (235,120,100) | 63,5/43/31 | 6,7 / 1,9 / -0,7 | 5,2 / 2,5 / 1,1 | 82 | 24,3 / 9,4 | |

- Freies Bild-1-Licht: Mischung Ambient -> hellste Leon-Stelle, t = 0,18 ergibt Licht (0,404/0,391/0,375). Bei t = 0 waere T aus Bild 1 (195,120,119), Lab 58/29/13; bei t = 0,5 (127,84,98), Lab 41/20/-1.
- Video-Raum: Region im e-Bild ersetzt, `video_filter(entzerren=False, rauschen=False)`, Mittel gegen Original. Bezug ist Hin- und Rueckweg ohne Ersatz: dE 3,7 (Bild 1) und 2,5 (Bild 2).
- **Antwort:** Ja, eine Texelfarbe erklaert beide Bilder. Sie ist ein **gedecktes Lachsrot, T rund (165-175, 90-95, 95), L* 47-49, a* 30-32, b* 10-14**, Farbton 18-24 Grad.
  Bedingung: F1 bekommt in Bild 1 ueberwiegend Umgebungslicht. Das passt zu einer vom Licht abgewandten Flaeche; belegt ist es nicht.
  Gesaettigtes Gelb, Orange oder Feuerrot passt zu keinem der beiden Bilder.
- Zaehne (199,191,183) sind eine zweite, neutrale Farbe und gehoeren nicht zum Leuchten.

## 4. Schwarz-rote Koerperflaechen (V3B_3)

Klassen an T: dunkel L* < 20; dunkelrot = dunkel und a* >= 8; rot = a* >= 15, L* >= 20, Farbton -30..70 Grad (Definition T7 Leser B V2); hell = L* >= 45, a* < 10.
Zonen = Nutzer-Teile (`zonen.regionen`) innerhalb `ziel2 >= 128`, ohne Logo und Bodenfuss-Pixel.

| Bild / Zone | px | dunkel | dunkelrot | rot | hell | geclippt | T dunkel RGB (Lab) | T rot RGB (Lab, Ton) | L* p5/p50/p95 | K5c-Knochen (Hauptanteil) |
|---|---|---|---|---|---|---|---|---|---|---|
| 1 Schwanz | 11649 | 78,0 % | 26,6 % | 4,1 % | 2,4 % | 1,1 % | (37,29,33) (12/5/-1) | (127,78,94) (39/23/0, -5) | 8/13/44 | 1-6 |
| 1 Ruecken/Flanke | 7293 | 54,8 % | 13,4 % | 10,2 % | 7,8 % | 0,7 % | (40,32,35) (14/4/0) | (123,81,99) (39/21/-3, -10) | 10/18/57 | 0, 7, 9 |
| 1 Hinterbein l (W) | 3767 | 50,3 % | 8,2 % | 1,5 % | 7,7 % | 0 | (38,34,40) (14/4/-3) | (96,55,44) (28/17/15, 42) | 11/20/49 | 8, 24, 25 |
| 1 Hinterbein r (W) | 5318 | 49,6 % | 10,0 % | 0 | 6,2 % | 0 | (35,36,46) (15/3/-7) | - | 11/20/48 | 9, 26, 27 |
| 1 Vorderglied r (W) | 4299 | 60,9 % | 4,6 % | 0 | 6,4 % | 0 | (36,36,40) (14/1/-3) | - | 11/17/47 | 16-18 |
| **1 Figur** | 32326 | **62,5 %** | **15,8 %** | **3,9 %** | 5,4 % | 0,5 % | (37,32,37) (13/4/-2) | (124,79,94) (39/21/-1, -8) | 9/16/49 | |
| 2 Oberkiefer | 5832 | 87,4 % | 86,4 % | 10,4 % | 0,4 % | 0 | (69,23,23) (15/22/11) | (92,40,49) (24/25/6, 22) | 12/16/25 | 22 |
| 2 Kopfoberseite/Rumpf | 24344 | 72,0 % | 68,6 % | 12,0 % | 3,1 % | 0 | (62,22,26) (14/20/7) | (95,48,64) (27/23/0, -2) | 10/16/40 | 10, 22, 21 |
| 2 Unterkiefer | 3355 | 0 | 0 | 70,8 % | 18,0 % | 0,1 % | - | (175,93,89) (50/33/19, 31) | 30/57/86 | 23, 22 |
| 2 Klaue r (W) | 6035 | 19,3 % | 19,3 % | 70,1 % | 7,6 % | 0 | (70,30,25) (17/18/13) | (125,53,39) (32/29/24, 39) | 16/28/62 | 18, 20, 29 |
| 2 Klaue l (W) | 5559 | 4,6 % | 4,5 % | 77,0 % | 10,5 % | 0 | (80,24,27) (18/26/13) | (122,59,46) (33/26/21, 39) | 20/33/60 | 11-13 |
| **2 Figur** | 45038 | **53,4 %** | **51,4 %** | **31,9 %** | 5,4 % | 0 | (64,22,25) (14,5/20/8) | (125,60,55) (34/27,5/17, 34) | 11/18/61 | |

Lesart fuer die G4-Uebertragung:
- **Grundhaut schwarz:** In beiden Bildern T L* 13-15. Bild 1 ist fast unbunt (37,32,37). Bild 2 ist rot getoent (64,22,25).
  Wer Bild 2 dunkelrot nimmt, muss wissen: Die dunklen Stoffe der Frau zeigen dort bis +21 Rot-Einstreuung in e (farbe.md 4). Das sind bis zu 55 in T_R, die Grundhaut kann also auch (9-64, 22, 25) sein.
  Der "schwarz-rot verbrannt"-Eindruck ist mit beiden Bildern vertraeglich, solange die Haut dunkel ist (L* < 20) und hoechstens leicht rot.
- **Rot in Bild 1 (Bahnsteig):** 3,9 % der Figur, fast nur F1, Ruecken und Schwanzende. Farbton -8 Grad (Figurlicht).
  Mit dem Licht der Gegenprobe (0,404/0,391/0,375) sind es 6,5 %, Lab 38/22/9, Farbton +16,5 Grad, T (128,76,77); dunkel 48,1 %, T (44,34,34).
  Rostrot (Farbton 34 Grad und mehr) kommt in Bild 1 nicht vor.
- **Rot in Bild 2 (Zug):** 32 % der Figur, getragen von Unterkiefer (71 %) und Klauen (70-77 %, Farbton 39 Grad). Kopf und Rumpf nur 10-12 %.
  Die Klauen liegen neben dem Feuer. Leser B V2 maß dort gelblichen Feuerschein; ihr Orange ist nicht sicher Oberflaeche.
- Schirmfarbe e insgesamt: Bild 1 nur 25,5 % der Pixel mit C* > 6 (Farbton-Median 42 Grad, Streuung p25-p75 10-86), Bild 2 71,6 % (27 Grad, 16-39).

## 5. KI-Fassung (nur Zonen)

`Version_80%/vlcsnap-2026-11-04-16h26m21s385.png` / `16h26m59s430.png` (Bild 1): an der Stelle von F1 ein kraeftig karmin-rosa Fleck, sonst nur am Schwanzende Rot.
Den Kopf zeigt die KI in Bild 1 auch nicht. `16h26m29s692.png` / `16h27m04s664.png` (Bild 2): Unterkiefer als rot-weiss gestreiftes Band (Zahnreihe auf rotem Fleisch), Gesicht dunkel,
links unten die weissen Flecken als fingerartige Formen. Das bestaetigt die Zonen aus 1 und 2. Farben werden daraus nicht gelesen.

## 6. Fuer den Messblatt-Pruefer (Entscheide, die die Gestaltung G2 trifft)

1. **Ort des Leuchtens in Bild 1:** Messung = Knochen 0, Flaechen 107/108/119 (+120/95/96). Nutzerdeutung = Gesicht. In K5c ist das Gesicht dort verdeckt.
   Wird nur das Gesicht bemalt, bleibt Bild 1 ohne F1, das waere ein Verlust gegen das Original. Wird das Becken bemalt, stimmt Bild 1, aber nicht die Erzaehlung "Gesicht".
2. **Ort des Leuchtens in Bild 2:** Fleischband am Unterkiefer (Knochen 23) und Kieferkante von Knochen 22, darauf weisse Zaehne. Stirn, Schnauze und Augen bleiben dunkel (E24 und G2 widersprechen sich hier nicht).
3. **Farbe des Leuchtens:** Lachsrot T (165-175, 90-95, 95), Lab 47-49/30-32/10-14. Gelb-Orange-Feuertexel sind durch beide Bilder widerlegt (chi^2 98-265).
4. **Kiefer nicht cremefarben** (Vorgabe): Gemessen ist der Kiefer zu 57-71 % lachsrot. Creme (Zahn-Weiss) gibt es nur in den 18 % Zahnflecken.

## 7. Offen

- Licht am Ort F1 in Bild 1: Die Spanne nach Leon macht T-Helligkeit Faktor 2. Die gemeinsame Anpassung waehlt Licht nahe Ambient, belegt ist das nicht.
- Gelber Saum am Unterkiefer (185 px): Feuerschein oder gelbliche Zahnbasis, nicht entschieden.
- Weisse Flecken links unten in Bild 2: Feuer oder Krallen, geclippt, keine Farbe.
- Rotanteil der dunklen Haut in Bild 2: Einstreuung bis +21 e, die Unterkante der Haut-Roete ist nicht bestimmt.
- Die Klassen-Grenzen (1.1, 2.1, 4) sind PORT-WAHL dieses Lesers; die Abhaengigkeit der F1-Flaeche von den Schwellen steht in 1.1.
