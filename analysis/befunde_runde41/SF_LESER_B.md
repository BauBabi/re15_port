# SF Leser B: Formbeschreibung der Sichel-Form (gemessen, 2026-11-05)

Leser B, Blickwinkel "erst messen, dann beschreiben". Nur gelesen, nichts im Port geaendert.
Hilfsbilder, Skripte, Messdaten: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/leser/B/`
(Unterordner `tafeln/` = Belegtafeln, `py/` = Messskripte, `*.json` = Messwerte).

Konventionen
- Koordinaten: Videobild 640x480 (Original "Complete Disc Video 4 (480p)"); KI-Bild (_slp, 1280x960) durch 2 geteilt.
- Bild n = Dateibild (n = 1 erstes Bild), h = n - 1350. Kamera Hangar = ROOM50A0 Schnitt 12 (statisch, Dossier Abschn. 1).
- H = Koerperhoehe im Bild = Oberkante der Figur (Schulterbuckel) bis Sohle. Rumpf = Oberkante bis Hosenbund.
- Seiten: "(B)" = aus Sicht des Betrachters, "(W)" = aus Sicht des Wesens. Von vorn gilt Bild-links (B) = rechts (W).
- (a) = Original zeigt es, (b) = nur KI (_slp, Freisteller, Tripo), (c) = Nutzervorgabe. Port = Referenz 2 (84e8a4d97),
  EM30_HANGAR.EMD sha256 cfd1ac42... (Worktree-HEAD fade49ab8 hat dieselbe Datei; Schritte 1-4 aendern nur die Szene).
- "geschaetzt" = Augenmass an der Lupe, sonst gemessen.

Methode (kurz)
- birkinNN.png: maskierte Vorlagensuche (1/8 grob, dann volle Aufloesung) gegen alle 1538 _slp-Bilder.
- _slp-Bild n gegen Original-Bild n: NCC 0,976-0,979 (Versatz 0 Bilder), Lageversatz <= 0,3 px, Farbe _slp = 1,00 x Orig - 1..2.
  Die Freisteller-Masken liegen damit pixelgenau auf dem Original.
- Original-Belege: Hintergrundplatte (Median der Originalbilder ausserhalb aller 24 Masken, `plate.png`); Differenz |Bild - Platte|
  und Rotanteil r-g quer zum KI-Verlauf. Port: Modell EM30_HANGAR.EMD (Posen ueber birkin_formen.modell_aus_blob) und die
  Port-Spalte "neu" der Leisten 03/04/07, an der statischen Tuersaeule registriert (Leiste 07: +1/+5 px, 04: +1/+14 px, 03: 0/0).

## 0. Widersprueche zum Auftrag (vorab)

| Punkt | Befund (gemessen) |
|---|---|
| vlcsnap-2026-11-04-14h22m34s340 ... 14h26m23s711 "Port-Screenshots" | Sind KEINE Port-Bilder: alle 30 haben NCC 1,000 gegen das _slp-Bild (n1351 ... n1490), zweitbestes 0,85-0,99. Es sind VLC-Schnappschuesse der KI-Fassung, aus denen birkin01-24 geschnitten sind. Port-Vergleich deshalb ueber Modell + Leisten 03/04/07. Tafel `tafeln/T9_vlcsnap_14h22-14h26_sind_KI-Bilder.jpg` |
| Vorderseite_Beschreibung.png | Zeigt die Bahnsteig-Form (Final Build, Bahnsteig mit Annette), nicht die Sichel-Form. Fuer diese Beschreibung nicht verwendet. |
| vlcsnap-2026-11-04-16h26/16h27 (4 Bilder) | Ebenfalls Bahnsteig-Szene (Final Build), NCC gegen Video 4 <= 0,36. |

## 1. Zuordnung birkin01-24 (alle Hangar, keiner aus dem Labor)

Alle 24 Freisteller sind pixelgenaue Ausschnitte des _slp-Bildes (RMS 0,0; naechstes Nachbarbild RMS 6-27).
Versatz = linke obere Ecke im 1280x960-Bild.

| Datei | n | h | Versatz | Kameralage / Haltung |
|---|---|---|---|---|
| birkin01 | 1351 | h1 | (620,332) | von vorn (Guertelring sichtbar), geduckt, Arme haengen |
| birkin02 | 1354 | h4 | (617,326) | von vorn, wie h1 |
| birkin03 | 1370 | h20 | (471,218) | von vorn; Arm Bild-links (B) = rechts (W) hoch (haelt Spider-Man), Schwanz Bild-links (B) |
| birkin04 | 1374 | h24 | (391,208) | von vorn; Schwanz waagerecht Bild-links (B) |
| birkin05 | 1411 | h61 | (538,273) | von vorn, Halten; Schwanz Bild-rechts (B) bis Boden |
| birkin06 | 1419 | h69 | (512,325) | von vorn, Halten; Schwanz Bild-rechts (B) diagonal zum Boden |
| birkin09 | 1420 | h70 | (539,334) | wie h69 |
| birkin07 | 1421 | h71 | (567,338) | von vorn, Loslassen |
| birkin10 | 1422 | h72 | (582,341) | von vorn, Wurf |
| birkin08 | 1423 | h73 | (490,348) | von vorn, Wurf |
| birkin11 | 1433 | h83 | (548,368) | Seitenansicht: blickt nach Bild-rechts, Kamera sieht die rechte Seite (W) = Sichelseite; Bogen hinter dem Ruecken Bild-links (B) |
| birkin12 | 1435 | h85 | (547,366) | wie h83 |
| birkin13 | 1442 | h92 | (545,365) | 3/4 seitlich, dreht zur Kamera |
| birkin14 | 1447 | h97 | (514,362) | 3/4 seitlich |
| birkin15 | 1451 | h101 | (596,351) | 3/4 von vorn |
| birkin16 | 1455 | h105 | (598,334) | 3/4 von vorn |
| birkin17 | 1459 | h109 | (595,320) | von vorn, Gang zur Kamera |
| birkin18 | 1462 | h112 | (597,315) | von vorn, Gang |
| birkin19 | 1468 | h118 | (606,315) | von vorn, Gang |
| birkin20 | 1475 | h125 | (584,293) | von vorn, Gang |
| birkin22 | 1476 | h126 | (581,292) | von vorn, Gang |
| birkin23 | 1478 | h128 | (571,295) | von vorn, Gang |
| birkin21 | 1480 | h130 | (567,293) | von vorn, Gang |
| birkin24 | 1482 | h132 | (564,284) | von vorn, Gang |

Tafeln `tafeln/T1a..T1d_zuordnung_*.jpg` (oben Original n, unten KI-Freisteller).
Labor (n360-n840): Original gruen und dunkel; am Pult n440-n466 nur der Buckel von hinten-oben, auf dem Steg n480-n560
Figur ~45 x 90 px. Schwanz dort nicht messbar (Tafel `T10a`). n790-n840 zeigt eine rote Kreatur ohne erkennbare Jeans
(Tafel `T10b`), nicht ausgewertet (Frage 13).

## 2. Schwanz

### 2.1 Seitenansicht h77-h98: Bogen vom oberen Ruecken (Haupt-Beleg fuer "bis in den oberen Ruecken")

Original-Oberkante je Spalte (erste Zeile mit |Bild - Platte| > 25), Auszug:

| h | x 290 | x 300 | x 310 | x 330 | x 340 | Buckel x 380-410 |
|---|---|---|---|---|---|---|
| 78 | - | 209 | 200 | 195 | 195 | 197-202 (x 400-410) |
| 84 | 240 | 225 | 221 | 200 | 193 | 187-192 |
| 92 | - | 229 | 219 | 202 | 193 | 186-191 |
| 96 | - | - | 228 | 208 | 196 | 185-192 |

- (a) Die obere Umrisslinie laeuft vom Schulterbuckel ohne Knick nach Bild-links (B) = hinter den Ruecken in einen
  Bogen ueber: bei x 340 liegt sie 5-8 px unter dem Buckel, bei h78 auf Buckelhoehe (195 gegen 197-202). Original-Oberkante
  = KI-Maskenoberkante +-3 px, wo die Platte gilt (h83: x300 223/224, x330 199/199, x340 193/195, x380 188/188).
- (a) Entlang des KI-Verlaufs ist das Original deutlich verschieden vom Hintergrund: Differenz innen 10-22 gegen 3-6 aussen
  (h83 s 40-160), r-g innen 2-16 gegen 0-3,5. Zwischen Bogen und Rumpf liegt Hintergrund (KI-Maske h83: Luecke 39 px bei
  y 252, 64 px bei y 275; Original-Differenz dort 3-6).
- Verlauf: vom Ansatz ~60-70 px waagerecht bis leicht fallend nach hinten, dann Bogen nach unten, Spitze frei in der Luft.
- Tafeln `tafeln/T2_schwanz_seite_h83_h92.jpg`, Verlauf h74-h107 `T6a`, `T6b`, Lupe `T6c`, KI-Freisteller `T6d`.

Masse (H = 252 px bei h85/h92; Rumpf h92 = Oberkante 183 bis Bund 290/292 = 107-109 px):

| Groesse | Original (a) | KI (b) | Port Ref2 | Urteil |
|---|---|---|---|---|
| Ansatz (Bogen trifft Rueckenumriss) | Oberkante bei x 340 = Buckel + 5..8 px tiefer | Durchmessersprung bei (344-356, 217-221) = 33-35 px unter dem Buckel = 0,13-0,14 H = 0,31-0,33 Rumpf unter der Oberkante | Gelenk 16 (rel (-380,120,0) an Knochen 8): 1198 E. unter Oberkante = 0,31 H = 0,66 Rumpf; 613 E. ueber dem Bund | weicht ab: Port-Ansatz 0,17-0,18 H (~700 E. bei H 3900) zu tief |
| Verlauf | Bogen nach hinten auf Buckelhoehe, dann abwaerts | dito | Ruhelinie nach hinten-unten; Port-Seitenbild h84/h92 (Leiste 07): Oberkante x 330 = 234/343, x 300 = 297/- statt 200-202/225-229 -> kein Bogen sichtbar | weicht ab |
| Spitze | h83/h85 (301,355)/(299,353) = 0,32 H ueber Sohle; h92/h97 (284,326)/(259,334) = 0,40-0,44 H | dito | Spitze am Boden (Gang Clip 1 B10: y +154) | weicht ab |
| Sichtbare Laenge (Projektion, Untergrenze) | Lage +-3 px bestaetigt, Laenge ueber KI-Pfad | 180-185 px (h83/h85), 160 px (h92) = 0,63-0,73 H = 1,5-1,7 Rumpf | gesamt 3474 E. = 0,89 H = 1,92 Rumpf | stimmt im Rahmen |
| Dicke am Ansatz (Pfad s 140-160) | 32-38 px (Aussenkante aus Differenz + Innenkante aus r-g) | 34-42 px = 0,13-0,17 H | 311 E. = 0,080 H | weicht ab: Port etwa halb so dick |
| Dicke Mitte (s 90-110) | 16-20 (h83/h85), 28-35 (h92) | 18-24 px = 0,07-0,09 H | 0,065-0,071 H (Glied 4-5) | stimmt |
| Dicke 1/4 vor der Spitze (s 40-50) | 13-16 px | 13-18 px = 0,055-0,07 H | 0,057 / 0,047 H (Glied 6/7) | stimmt |
| Spitze, Form | spitz (r-g-Halbwert 5-14 px in den letzten 20 px) | spitz, letzte 20 px 10 -> 3-6 px, rot | stumpfes Ende + Dorn (3 x Endradius) | weicht ab |

Port-Durchmesser je Glied (90. Perzentil Abstand zur Achse x 2, Modell): 311/303/292/275/254/224/182 E.

### 2.2 Von vorn beim Halten (h20-h73): Austritt hinter der Huefte bis zum Boden

| h | Seite | Austritt | Spitze | sichtbar | Dicke Austritt / Mitte / vor Spitze [px] | Original entlang KI-Pfad (Differenz innen/aussen, r-g innen/aussen) |
|---|---|---|---|---|---|---|
| 24 | Bild-links (B) = rechts (W) | (289,340), 38 px unter Bund (302) | (198,342), waagerecht | 100 px | 21 / 13-16 / 10 | 56/7, 20/0 |
| 61 | Bild-rechts (B) = links (W) | (425,311) | (470,432) am Boden | 130 px | 27 / 9-11 / 9 | 42/5, 25/1 |
| 69 | Bild-rechts (B) = links (W) | (430,313), 26 px unter Bund (287) | (540,395) | 150 px | 21-26 / 11-13 / 10 | 69/6, 34/2 |
| 70 | wie h69 | (426,320) | (528,400) | 140 px | 24 / 12-13 / 10 | wie h69 |

- (a) Von vorn ist der Schwanz klar im Original: hoher Kontrast und Rotanteil (Dossier N1.5: rot-weiss gebaendert).
  Er kommt hinter der Huefte hervor (Wurzel verdeckt) und reicht bis zum Boden oder peitscht waagerecht.
- Port (Leiste 03 h22/h61/h66): Schwanz auf derselben Seite von der Huefte zum Boden -> stimmt qualitativ.
- Tafel `tafeln/T3_schwanz_vorn_h24_h61_h69.jpg`.

### 2.3 Gang von vorn (h101-h132)

- (b) Die KI zeigt ein rotes Glied zwischen den Beinen (x 380-399, y 330-365; r-g > 25: 124-211 px in h125-h132).
- (a) Im Original dort nur schwach roetlich (r-g > 25: 0-12 px; 90. Perzentil r-g 14-19 gegen KI 19-22).
- Port h124/h132 (Leiste 07): rot-weisses Glied zwischen den Beinen sichtbar -> folgt der KI-Darstellung staerker als dem Original.
- Bei h101 liegt die Bogen-Spitze der KI (300,360) links neben der Sichel; im Original dort kein Kontrast (Differenz 4-8 innen
  wie aussen) -> nicht belegt.

### 2.4 Nur KI / Tripo (b) und Nutzer (c)

- Tripo (image_1/2/3, Seitenansichten, Raster `tafeln/T7_tripo_seitenansichten_raster.jpg`, geschaetzt an image_1 RIGHT SIDE,
  H ~445 px): Wurzel am Nacken zwischen den Schulterblaettern (rote Wirbelleiste in den BACK/HINTEN-Ansichten), Bogenoberkante
  ~25 px (0,06 H) unter dem Buckel, Bogen weit nach hinten, Spitze als Klinge am BODEN hinter den Fuessen, Laenge ~1,3 H,
  Dicke am Ansatz ~0,15 H.
  Widerspruch zum Original: dort Spitze 0,32-0,44 H ueber der Sohle, sichtbare Laenge 0,63-0,73 H (Untergrenze).
  Uebereinstimmung: Ansatz oben am Ruecken, Bogen nicht ueber Buckelhoehe, Ansatzdicke ~0,15 H.
- (b) Tripo-Sichel ist eine lange rote Klinge (image_1 FRONT), Video-KI dunkelgruen, Original dunkel.
- (c) Nutzer 2026-11-05: "Man sieht klar Kopf und den langen dicken Schwanz der bis in den oberen Ruecken reicht ... dem
  Original entsprechend." Das deckt sich mit 2.1 (Ansatz auf Buckelhoehe, Ansatzdicke 0,13-0,17 H).

### 2.5 Deutung des Bogens: Schwanz oder Arm?

Das Original allein trennt nicht, ob der Bogen h77-h98 der Schwanz oder der nach hinten gestreckte linke (W) Klauenarm ist
(beide ~0,7 H lang). Fuer Schwanz sprechen: (b) die KI zeigt in h92/h97 den nahen Sichelarm mit Klinge separat vor dem Rumpf,
einen Arm auf der fernen Seite und am Bogen eine rote Spitze statt einer Hand; (b) alle drei Tripo-Modelle; (c) Nutzer. Im
Original laeuft das Ende spitz zu (r-g-Halbwert 5-14 px). -> Frage 1.

## 3. Kopf, Arme, Beine, Wunden (von vorn h109-h132, H 274-310 px)

Tafeln `tafeln/T4_kopf_h112_h125_h132.jpg`, `tafeln/T5_rumpf_klaue_sichel_h109_h118_h125.jpg`.

### 3.1 Kopf

| Groesse | Original (a) | KI (b) | Port Ref2 | Urteil |
|---|---|---|---|---|
| Lage | heller Fleck oben Mitte = Kopfkuppe: h112 (387,167), h125 (390,164), h132 (376,170) = 9/17/28 px unter der Umriss-Oberkante (0,03-0,09 H); sinkt beim Naeherkommen zwischen die Schultern | heller Punkt deckungsgleich (+-2 px): (385,166)/(390,162)/(375,169) | Kopf oben 136 E. unter Oberkante = 0,035 H (Gang B10) | stimmt |
| roter senkrechter Bereich (Gesicht/Maul/Brustwunde) | Halbwertbreite 37-53 px (Unschaerfe), y 150-176 bis 224-241 | 17-24 px breit, y 150-167 bis 228-243 | Wundfleisch-Textur (CLUT 485), kein Maul | Lage stimmt; Breite im Original nicht trennbar |
| Maul | nicht trennbar (nur Rot) | offen, Zaehne, rote Lippen, langer roter Unterkiefer zur Brust (Kinn geschaetzt y 205-210) | geschlossen | nur KI |
| Augen | heller Fleck, kein Auge trennbar | ein Auge oben mittig + ein Auge auf der rechten Schulter (W) = Bild-links (B), h125 bei (358,185); Original dort dunkler Fleck (54 gegen 77 Umgebung), nicht als Auge belegbar | keine | nur KI |
| Kopfbreite / -hoehe | nicht trennbar | geschaetzt 22-26 px (0,08 H) / Kuppe bis Kinn geschaetzt 40-45 px (0,14-0,15 H) | 395 E. (0,10 H) / 637 E. (0,16 H) | stimmt (geschaetzt) |

### 3.2 Sichelarm und Klauenhand

| Groesse | Original (a) | KI (b) | Port Ref2 | Urteil |
|---|---|---|---|---|
| Sichel-Seite | rechts (W) = Bild-links (B) von vorn (Dossier Abschn. 2, n1367-1376); Seitenansicht h83: nahe Seite = rechts (W) | dito, Klinge dunkelgruen | Knochen 13-15 rechts | stimmt |
| Sichel senkrechte Ausdehnung (Schulter bis tiefster Punkt) | nicht trennbar | 0,65-0,71 H | 0,73 H | stimmt |
| Klingenspitze ueber Sohle | nur h118/h132 belegt (Differenz Klinge 46/31 gegen Ring 7/13), h109/h125 kein Kontrast | 0,20-0,27 H (tiefster Punkt y 361-389) | 0,16 H | weicht ab: Port 0,04-0,11 H tiefer |
| Klingenfarbe | dunkel | dunkelgruen (Tripo: rot) | dunkel (CLUT 484) | stimmt mit Original |
| Klauen-Seite | links (W) = Bild-rechts (B) | dito | Knochen 10-12 links | stimmt |
| Hoehe rote Krallen | y 336-360 (h125-h132) = 0,30-0,34 H ueber Sohle, 60-84 px unter dem Bund = Saumhoehe | y 325-368 | endet am Bund (Leiste 04 h121-h132, Augenschein); Modell 0,44 H ueber Boden, 0,08 H unter Bund | weicht ab: Port 0,10-0,20 H zu hoch |
| Groesse rote Krallen | 11-15 x 9-16 px | 31-40 x 28-32 px | Klaue 0,55 x EM30 (Nutzerentscheid "kleiner") | KI-Groesse nicht belegt |

### 3.3 Beine, Jeans (Anteil von oben, H = Oberkante bis Sohle)

| h | Bund Orig / KI | Saum Orig / KI | Port h (Leiste 07) | Port Bund / Saum |
|---|---|---|---|---|
| 109 | 0,42 / 0,40 | 0,57 / 0,58 | 108 | 0,50 / 0,73 |
| 118 | 0,42 / 0,41 | 0,57 / 0,57 | 116 | 0,53 / 0,73 |
| 125 | 0,43 / 0,42 | 0,62 / 0,62 | 124 | 0,52 / 0,74 |

- (a) Kurze Jeans Bund -> Saum 0,15-0,19 H; Port 0,22-0,23 H. In Bildpunkten liegt der Port-Bund 23-34 px, der Saum 35-50 px
  tiefer als im Original (gleiche Kamera; Oberkante und Sohle fast gleich: Port 155-158 / 444, Original 158-160 / 434-446).
  Folge: Port-Rumpf zu lang, Beine zu kurz (Modell Gang B10: Rumpf 0,46 H). Ob Pose (Knie 115-139 Grad laut Leiste 07) oder
  Modell, trennt das Bild nicht.
- Seitenansicht h84/h92: Port-Buckel y 220/216 gegen Original 186-193 (Port 30 px tiefer, staerker geduckt; H 228 gegen 252).
  Port steht dort naeher an der Kamera (Szene, Spur H).

### 3.4 Wundzeichnung

| Zeilenband (10 px) | Rotanteil r-g > 20 Original | KI |
|---|---|---|
| h118 y 168-227 (Schultern/Brust) | 0,21-0,37 | 0,24-0,36 |
| h118 y 228-277 (Bauch) | 0,00-0,02 | 0,00-0,06 |
| h125 y 177-216 | 0,24-0,34 | 0,23-0,36 |
| h125 y 217-276 | 0,00-0,08 | 0,00-0,08 |

- (a) Rot sitzt auf Schultern/oberer Brust bis ~70-80 px unter der Oberkante (0,25-0,28 H), Bauch frei; Rot-Schwerpunkt der
  unteren Brustbaender bei x 400-408 = links (W). KI deckungsgleich. Port: nicht gemessen (andere Farbtreppe, Leiste x1,5).

## 4. Vergleich mit dem Port (Referenz 2), je Punkt

| Punkt | Urteil | Zahl |
|---|---|---|
| Schwanzansatz Hoehe | weicht ab | Original 0,13-0,14 H unter Oberkante, Port 0,31 H (~700 E. zu tief) |
| Schwanz Verlauf Seitenansicht | weicht ab | Original Bogen auf Buckelhoehe nach hinten; Port h84/h92 kein Bogen (Oberkante x 330: 234/343 statt 200/202) |
| Schwanzspitze Seitenansicht | weicht ab | Original 0,32-0,44 H ueber Sohle, Port am Boden |
| Schwanzlaenge | stimmt im Rahmen | sichtbar >= 0,63-0,73 H, Port 0,89 H |
| Schwanzdicke Ansatz | weicht ab | 0,13-0,17 H gegen 0,080 H |
| Schwanzdicke Mitte / Ende | stimmt | 0,055-0,09 H gegen 0,047-0,071 H |
| Schwanzspitze Form | weicht ab | spitz gegen stumpf + Dorn |
| Schwanz von vorn beim Halten | stimmt qualitativ | gleiche Seite, Huefte -> Boden (Leiste 03) |
| Schwanz zwischen den Beinen im Gang | Port wie KI, Original schwach | r-g > 25: KI 124-211 px, Original 0-12 px |
| Kopf Lage | stimmt | 0,03-0,09 H gegen 0,035 H unter Oberkante |
| Kopf Groesse | stimmt (geschaetzt) | ~0,08 x 0,15 H gegen 0,10 x 0,16 H |
| Maul/Augen | nur KI | Port ohne Maul/Augen; Original nicht trennbar |
| Sichel Seite / Ausdehnung | stimmt | rechts (W); 0,65-0,71 gegen 0,73 H |
| Sichel Spitze | weicht ab (gering) | 0,20-0,27 H gegen 0,16 H ueber Sohle |
| Klaue Seite | stimmt | links (W) |
| Klaue Hoehe | weicht ab | Krallen 0,30-0,34 H ueber Sohle (Saumhoehe) gegen Port am Bund (Modell 0,44 H) |
| Bund / Saum | weicht ab | Bund 0,40-0,43 gegen 0,50-0,53 von oben; Saum 0,57-0,62 gegen 0,73-0,74 |
| Wunden | Original = KI | Port nicht gemessen |

Dossier-Abschnitte gelesen: Referenz 2, NB2neu.1-.8, Schritt 1-4 (betreffen Griff, Wurf, Blut, Zeitversatz; das Modell ist
seit 84e8a4d97 unveraendert). Leisten nb2neu/03, 04, 07 Spalte "neu" registriert verwendet.

## 5. Offene Fragen an den Nutzer

1. Ist der Bogen hinter dem Ruecken in h77-h98 der Schwanz (KI, Tripo, Ihre Aussage) und nicht der nach hinten gestreckte
   linke (W) Arm? Das Original allein trennt es nicht (2.5).
2. Schwanzwurzel auf Buckelhoehe (0,13-0,14 H unter der Oberkante, zwischen den Schulterblaettern) statt wie jetzt 0,31 H
   am mittleren Ruecken - so bauen?
3. Ruhelage der Spitze: Original seitlich in Knie-/Oberschenkelhoehe (0,32-0,44 H), vorn beim Halten am Boden oder waagerecht;
   Tripo immer am Boden. Welche Ruhelage gilt?
4. Laenge: sichtbar mindestens 0,63-0,73 H (Projektion), Port 0,89 H, Tripo ~1,3 H. Port-Laenge behalten?
5. Spitze spitz zulaufend (Original/KI) statt stumpf mit Dorn?
6. Ansatzdicke ~0,15 H (fast doppelt so dick wie jetzt) - gewuenscht ("langer dicker Schwanz")?
7. Rotes Glied zwischen den Beinen im Gang: KI deutlich, Original kaum. Soll der Schwanz dort sichtbar haengen?
8. Kopf: offenes Maul mit Zaehnen, ein Auge oben, ein Auge auf der rechten Schulter (W) zeigt nur die KI. Uebernehmen?
9. Klauenhand: Krallen im Original auf Saumhoehe (0,30-0,34 H ueber der Sohle), im Port am Bund. Tiefer haengen?
10. Hosenbund und Saum liegen im Port 0,08-0,16 H tiefer (Rumpf zu lang / Beine zu kurz). Anpassen?
11. Vorderseite_Beschreibung.png (Bahnsteig-Form): "linke Klaue" (oben) / "rechte Klaue" (unten) - aus Sicht des Betrachters
    oder aus Sicht des Wesens? Betrachter-Lesart: "linke Klaue" = Klaue Bild-links = rechte Klaue (W) des zugewandten Wesens;
    Wesens-Lesart: "linke Klaue" = linke Klaue (W). (Die Augen-Beschriftung gilt laut Korrektur aus Sicht des Wesens.)
12. Die vlcsnap-Bilder 14h22-14h26 sind KI-Bilder. Gibt es echte Port-Screenshots vom 2026-11-04, die gemeint waren?
13. Final Build 58,5-64,5 s (Tafel `T8`): Birkin in Tuer mit langem rot-weiss gebaendertem Stab, Nahaufnahme heller Kopf
    zwischen den Schultern, Kampf gegen Elza im Aufzugsraum - gehoert das zur Sichel-Form (dann zweite Originalquelle) oder
    zur Rohr-Form? Und: ist die rote Kreatur in Video 4 n790-n840 eine andere Form?

## 6. Tafeln und Werkzeuge

Ordner `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/leser/B/tafeln/`:
- `T1a..T1d_zuordnung_*.jpg` Original n | KI-Freisteller je birkinNN
- `T2_schwanz_seite_h83_h92.jpg` Original | KI | Port: KI-Pfad, Breiten, Ansatz, Spitze, Original-Oberkante (gruen), Port-Oberkante (orange)
- `T3_schwanz_vorn_h24_h61_h69.jpg` Austritt/Spitze/Dicke, Original-Kontrast je Zeile
- `T4_kopf_h112_h125_h132.jpg` heller Kopfpunkt + Rotbereich Original/KI, Port
- `T5_rumpf_klaue_sichel_h109_h118_h125.jpg` Oberkante/Bund/Saum/Sohle, Klauenkrallen, Sichel-Tiefpunkt, Port-Linien
- `T6a/T6b_bogen_verlauf_*.jpg`, `T6c_bogen_lupe_h86-h104.jpg`, `T6d_ki_freisteller_seite_*.jpg` Bogen ueber die Drehung
- `T7_tripo_seitenansichten_raster.jpg`, `T8_final_build_575-640s.jpg`, `T9_vlcsnap_*.jpg`, `T10a/T10b_labor_*.jpg`

Skripte `py/`: match1/match2 (Zuordnung), align (Original/KI-Deckung), plate (Hintergrundplatte), arc/arc2 (Schwanzpfad),
band/fwhm/rgw (Original-Beleg quer zum Pfad), topedge (Oberkante), front/kopf (Vorderansicht), portmass (Modellmasse),
porttile (Leisten-Kacheln registrieren), tafel (Tafeln). Messwerte: `fine.json`, `arcs_*.json`, `band_*.json`, `fwhm_*.json`,
`front.json`, `kopf.json`, `portmass.json`, `t5_tab.json`.

Grenzen: Original ist 320x240 hochskaliert und im Gang dunkel; Dicken im Original nur, wo die Platte gilt (Rand +
Rotanteil); Laengen in der Seitenansicht sind Projektionen (Untergrenzen); Port-Bildwerte stammen aus den Leisten (Leiste 07
x1,5 aufgehellt), Port-Modellwerte aus EM30_HANGAR.EMD in Clip 1 Bild 10.
