# SF Leser A - Formbeschreibung der Sichel-Form fuer den Modellbau (2026-11-05)

Blickwinkel: erst Bild ansehen und beschreiben, dann jede Aussage messen. Nur gelesen und gemessen, keine Datei
unter Version_80%, pri/, synchro/ oder re15_port geaendert.

⛔ Kein RE-Befund. Die Sichel-Form gibt es auf der Disc nicht; alles hier ist Bildmessung am 80%-Video. Keine Zahl
hier ist eine @0x-Konstante.

Konventionen:
- Koordinaten: 1280x960 (KI-Groesse). Original-Bild (640x480) x2. Bild n = Datei-Bild (ffmpeg, 1-basiert), h = n - 1350.
- H = Koerperhoehe im Bild = Umriss-Oberkante bis Sohle (des vorderen Fusses), im selben Bild gemessen.
- Seiten: "Wesen-rechts" = aus Sicht des Wesens, "Bild-links" = aus Sicht des Betrachters.
- Quelle jeder Aussage: ORIGINAL = "Complete Disc Video 4 (480p Upscale)" (SF_belege/roh/alle/f%04d.png), KI = "_slp"-Fassung
  und Freisteller birkinNN, PORT = Lauf R17 (RE15_FRAMEDUMP, Stand fade49ab8,
  `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/r/R17`, registriert wie
  tools/r41_sichel/nb2neu/c6.port) bzw. Modell EM30_HANGAR.EMD (sha256 cfd1ac42729e5636..., = Referenz 2 84e8a4d97,
  seither unveraendert).
- "von Hand am Raster abgelesen" = Lupe mit 10-px-Raster, Unsicherheit +-10 px. Alles andere: Skript (Pfade unten).

Beleg-Tafeln (Original | KI | Port, mit Messlinien), Ordner
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/leser/A_form/tafeln/`:

| Tafel | Inhalt |
|---|---|
| T1_zuordnung_birkin01-24.jpg | je Freisteller: Original n | KI n | birkinNN |
| T2_schwanz_seite_h85_h92.jpg | Schwanzbogen Seitenansicht: Wurzel, Spitze, Mittellinie, Querschnitte; Port-Bild; Port-Modell rechte Flanke |
| T3_schwanz_vorn_h24_h70.jpg | Schwanz von vorn, Dicke je Spalte Original/KI; Port-Bild |
| T4_vorn_h118.jpg | Vorderansicht: Oberkante, Schulter, Hosenbund, Jeans-Ende, Sohle, Kopf/rote Zone; Port mit eigenen Linien |
| T5_schwanzende_vorn_h125_h128.jpg | rote Stelle zwischen den Beinen (Original/KI), Port |
| port_modell_ansichten.png | Port-Modell orthografisch (Clip 1 B0 / Clip 0 B0; rechte Flanke, vorn, hinten) mit Schwanzlinie und H-Linien |
| tripo_seiten.png | Tripo-Seitenansichten (KI-Modelle) mit Raster |
| freisteller_uebersicht.png | alle 24 Freisteller nebeneinander |

Werkzeuge (Python, nur Lesen): `.../scratchpad/leser/A_skripte/` (zuordnen.py, fein.py, vlc_zu.py, bogen3.py, profil2.py,
profil3.py, front.py, kopfrot.py, schritt.py, schwanz_vorn.py, portmass.py, portansicht.py, portbilder.py, tafeln_final.py).
Messdaten: `.../leser/A_form/bogen_mittellinie.json`, `port_modell_ansichten.json`.

## 0. Vorab: Widersprueche zum Auftragstext

| # | Auftrag sagt | Messung | Folge |
|---|---|---|---|
| W1 | vlcsnap-2026-11-04-14h22m34s340 ... 14h26m23s711 = "Port-Screenshots des jetzigen Stands" | Alle 30 sind Bilder der KI-Fassung _slp: normierte Korrelation 1,000 gegen _slp n1351, 1354, 1370, 1374, 1411, 1419-1423, 1426, 1427, 1429, 1433, 1435, 1442, 1447, 1451, 1455, 1459, 1462, 1468, 1475, 1476, 1478, 1480, 1482, 1488, 1490 (vlc_zu.py). Die 4 Dateien vlcsnap-2026-11-04-16h* zeigen den roten Korridor des Final Build. | Port-Spalte stattdessen aus Lauf R17 (Stand fade49ab8) und dem Modell selbst. |
| W2 | Vorderseite_Beschreibung.png (Augen-/Klauen-Beschriftung) | Zeigt nicht die Sichel-Form, sondern eine Szene im rot beleuchteten Korridor mit Logo "BIOHAZARD 1.5 / www.bioflames.com". | Fuer die Sichel-Form nicht verwendet; Klauen-Frage unter 5. |
| W3 | Labor 12-28 s als Material | KI-Fassung im Labor unbrauchbar: n530 zeigt einen Mann im karierten Hemd statt Birkin (Tafel-Ausschnitt rl_n530, geloescht nach Sichtung); n660-n840 ist ein anderes, rotes Wesen im Gang. Original im Labor gruen/dunkel (Dossier N2.6). Keiner der 24 Freisteller liegt im Labor. | Formbeschreibung nur aus dem Hangar. |

## 1. Zuordnung birkin01-24 -> Videobild

Methode: Freisteller-Alpha (>= 250) als Maske, Suche in allen _slp-Bildern n1340-1538 (zuordnen.py, 1/4 Groesse), dann voll
aufgeloest +-10 px und +-2 Bilder in _slp und _stab (fein.py). Alle 24 sind BITGLEICH mit genau einem _slp-Bild (mittlere
Abweichung 0,0; naechstes Bild 3,4-16,8; _stab 6,9-11,2). _slp-Bild n = Original-Bild n: Korrelation 0,996-0,998 bei
gleichem n, Nachbarn tiefer; Versatz 0,2/0,7 px (640er) (versatz.py). Kamera in allen 24: Hangar ROOM50A0 Schnitt 12
(feste Kamera, Dossier Abschnitt 1).

| Freisteller | n | h | Versatz (x,y) | Groesse | Blick auf Birkin (Sichtbefund) | Was das ORIGINAL zeigt |
|---|---|---|---|---|---|---|
| birkin01 | 1351 | 1 | 620,332 | 319x548 | 3/4 von vorn, gebueckt | Masse mit roter Kappe oben, Beine gespreizt |
| birkin02 | 1354 | 4 | 617,326 | 332x555 | wie 01 | wie 01 |
| birkin03 | 1370 | 20 | 471,218 | 425x665 | von vorn schraeg; Sichelarm (Wesen-rechts) Bild-links hoch, haelt Spider-Man | rot-weisses Glied Bild-links zum Boden |
| birkin04 | 1374 | 24 | 391,208 | 500x673 | wie 03 | Schwanz Bild-links waagerecht (T3) |
| birkin05 | 1411 | 61 | 538,273 | 408x598 | von vorn schraeg, Sichelarm oben | Schwanz Bild-rechts zum Boden |
| birkin06 | 1419 | 69 | 512,325 | 575x551 | von vorn, Wurf beginnt | Schwanz Bild-rechts |
| birkin09 | 1420 | 70 | 539,334 | 527x541 | von vorn | Schwanz Bild-rechts unter der Klauenhand (T3) |
| birkin07 | 1421 | 71 | 567,338 | 421x538 | von vorn | wie 09 |
| birkin10 | 1422 | 72 | 582,341 | 315x532 | von vorn | wie 09 |
| birkin08 | 1423 | 73 | 490,348 | 416x524 | von vorn, Arm schwingt Bild-links | wie 09 |
| birkin11 | 1433 | 83 | 548,368 | 390x453 | rechte Flanke (Wesen schaut nach Bild-rechts) | Bogen vom Buckel nach Bild-links unten |
| birkin12 | 1435 | 85 | 547,366 | 390x506 | rechte Flanke | Bogen (T2) |
| birkin13 | 1442 | 92 | 545,365 | 372x508 | rechte Flanke -> 3/4 | Bogen + dunkle Sichel am vorderen Arm (T2) |
| birkin14 | 1447 | 97 | 514,362 | 406x512 | 3/4 von vorn-rechts | Bogen sinkt |
| birkin15 | 1451 | 101 | 596,351 | 330x522 | 3/4 von vorn | Sichel Bild-links bis zum Boden |
| birkin16 | 1455 | 105 | 598,334 | 336x537 | fast von vorn | wie 15 |
| birkin17 | 1459 | 109 | 595,320 | 341x551 | von vorn, kommt auf die Kamera zu | helle Kuppe oben Mitte, rote Zone darunter |
| birkin18 | 1462 | 112 | 597,315 | 339x556 | von vorn | wie 17 |
| birkin19 | 1468 | 118 | 606,315 | 346x571 | von vorn | wie 17 (T4) |
| birkin20 | 1475 | 125 | 584,293 | 414x601 | von vorn | rote Stelle zwischen den Beinen (T5) |
| birkin22 | 1476 | 126 | 581,292 | 419x609 | von vorn | wie 20 |
| birkin23 | 1478 | 128 | 571,295 | 439x639 | von vorn | wie 20 (T5) |
| birkin21 | 1480 | 130 | 567,293 | 452x609 | von vorn | Fuesse am unteren Bildrand |
| birkin24 | 1482 | 132 | 564,284 | 462x622 | von vorn | Fuesse abgeschnitten (Umriss bis 904, Spielbild endet bei 900) |

Hinweis: Die Nummerierung ist nicht zeitlich (07/08/09/10 = h71/h73/h70/h72; 21/22 = h130/h126). birkin11/12 enthalten
unten den Spider-Man-Koerper im Vordergrund (Umriss-Unterkante 820 bei h83 ist Spider-Man, nicht die Sohle).

## 2. Schwanz

### 2.1 Was ist der Bogen in h78-h98? (Video entscheidet)

Sichtbefund KI, gemessen in den Rasterlupen: In h92 und h97 sind gleichzeitig drei getrennte Glieder zu sehen:
(a) der Bogen vom oberen Ruecken nach Bild-links unten, (b) am vorderen Arm (Wesen-rechts, weil Birkin in der rechten
Flanke zur Kamera steht) die dunkle Sichelklinge senkrecht bis zum Boden (h92 x 640-700, y 560-840), (c) Bild-rechts hinter
dem Rumpf der Klauenarm (h97 x 870-920, y 500-560). Der Bogen ist also weder Sichel- noch Klauenarm, sondern der Schwanz.
In h101-h106 senkt er sich Bild-links hinter die Sichel; ab h109 (von vorn) ist er hinter dem Koerper.
ORIGINAL: Der Bogen ist im Original vorhanden. Seine Aussenkante (Differenz zum Engine-Hintergrund BSS 50A/12 > 18,
profil2.py) liegt 0-6 px neben der KI-Kante (Tabelle 2.5). Das Ende des Bogens ist in der KI eine Hand mit roter Kralle
(h78-h97) - im Original nicht aufloesbar (nur KI).
Nutzer-Satz "Schwanz ... der bis in den oberen Ruecken reicht" = genau dieser Bogen.

### 2.2 Wurzel

| Bild | Umriss-Oberkante | Schwanz-Oberkante an der Wurzel (Spalte vor dem Rumpfsprung) | Abstand | Quelle |
|---|---|---|---|---|
| h83 | 368 | 381 (x 710) | 13 px | KI-Alpha (bogen3.py) |
| h85 | 367 | 381 (x 709) | 14 px = 0,03 H (H 504) | KI-Alpha |
| h92 | 365 | 382 (x 707) | 17 px = 0,03 H (H 506) | KI-Alpha |
| h85 Original | - | Oberkante Spalte 619: 432 (KI 438), Spalte 639: 420 (KI 416) | 4-6 px | Original Diff > 18 |

- Die Oberkante laeuft ohne Knick vom Buckel in den Schwanz (h85 je Spalte: x 787 -> 370, 747 -> 378, 709 -> 381, 669 -> 391,
  649 -> 404, 629 -> 429, 609 -> 447, 589 -> 466). Der Schwanz setzt also am hoechsten Punkt des Rueckens an (obere Ruecken-
  mitte, hinter dem Kopf), nicht an Brust oder Huefte.
- Die Unterseite trifft den Ruecken bei x 709-719 (Spaltenlauf springt von 99-101 auf 118/156 px).
- Von vorn tritt er hinter dem Rumpf aus: h24 Bild-links bei y ~570 (Bund ~620), h70 Bild-rechts bei y ~630-650 unter der
  Klauenhand (Bund ~590) - von Hand am Raster abgelesen. Das passt zu einer Wurzel oben am Ruecken, die hinter dem Koerper
  herabfaellt.

### 2.3 Verlauf (haengt er oder steigt er?)

- Er steigt NICHT ueber den Ruecken: hoechster Punkt des Schwanzes ist die Wurzel; seine Oberkante bleibt 13-17 px unter der
  Umriss-Oberkante (h83/h85/h92, Original bestaetigt die Lage 4-6 px).
- Form = Haken: erst fast waagerecht nach hinten (Mittellinie h85 von (709,430) bis (599,505): 110 px waagerecht, 75 px Fall),
  dann fast senkrecht nach unten (Zeilenmitten h85: y 527 -> x 591, 567 -> 573, 607 -> 563, 647 -> 568, 687 -> 585, 707 -> 596).
- Spitze haengt frei: h85 0,33 H, h92 0,42 H ueber der Sohle (KI). Original: Bogen-Aussenkante bis y 687 (h85) bzw. 625-645
  (h92) messbar, also Spitze bei >= 0,36 H bzw. 0,45-0,49 H.
- Von vorn h125/h128: rote Stelle zwischen den Beinen, Original x 759-791, y 671-720 (Flaeche 576/338), KI x 764-792,
  y 658-730 -> 0,29-0,37 H ueber der Sohle (H h125 = 598; schritt.py; Tafel T5). Passt zur frei haengenden Spitze.
  Ob das die Schwanzspitze ist oder eine Wunde am Knie, zeigt nur die KI (rot-weiss gebaendert wie der Schwanz) -> Frage 5.
- Beim Halten/Werfen (h20-h73) liegt der Schwanz weit seitlich: h24 waagerecht nach Bild-links bis x ~400, h70 schraeg nach
  Bild-rechts bis x ~1066, y ~802 (KI). Ob er dort den Boden beruehrt, ist ohne Tiefe nicht entscheidbar.

### 2.4 Laenge

| Mass | Wert | Quelle |
|---|---|---|
| Mittellinie Wurzel -> Spitze h85 | 353 px = 0,70 H (projiziert, Untergrenze) | KI-Alpha (bogen3.py) |
| Mittellinie h92 | 299 px = 0,59 H (staerker verkuerzt, Birkin dreht) | KI-Alpha |
| Hakenweg h85 | 110 px waagerecht + 277 px Fall (y 430 -> 707) | KI-Alpha |
| Rumpfhoehe Schulterkuppe -> Hosenbund h118 | 363 -> 553 = 190 px = 0,33 H | Schulterkuppen 353/372 (KI-Alpha-Profil + Raster); Bund ORIGINAL (front.py) |
| Oberkante -> Hosenbund h118 | 238 px = 0,42 H | wie oben |
| Schwanz / Rumpfhoehe | >= 0,70 / 0,33 = >= 2,1 x Rumpfhoehe; >= 0,70 x Koerperhoehe | Rechnung |

### 2.5 Dicke

Senkrecht zur Mittellinie (Spaltenlauf x cos, Zeilenlauf x sin der Mittelliniensteigung), h85, H 504:

| Stelle | KI (px / H) | ORIGINAL | 
|---|---|---|
| Ansatz am Ruecken (x 679-699) | 96-97 / 0,19 (mit Uebergang in den Ruecken) | nicht trennbar |
| Bogen (x 619-639) | 53-69 / 0,11-0,14 | Spalte 619: 432-478 = 46 senkrecht (~34 quer = 0,07 H) |
| Mitte (y 547-567) | 32-39 / 0,06-0,08 | nur Lichtkante 14-16 px (y 547: x 558-572, y 567: 552-566) |
| unten (y 587-607) | 26-28 / 0,05 | Lichtkante 8-16 px |
| Verdickung (y 627-647) | 32-35 / 0,06-0,07 ("Hand" der KI) | y 627: 544-572 = 28 px hell |
| Spitze (y 667-707) | 24 -> 10, spitz | y 687: 578-590 |

Die Innenhaelfte des Schwanzes liegt im Original im Schatten vor dem dunklen Tor (Differenz < 15) - Dicke dort nur KI.
Von vorn, gleiche Tiefe wie h85 (Port-Log: Birkin steht h20-h85 auf derselben Stelle -8488/-21651):

| Bild, Spalte | ORIGINAL senkrecht (rote Pixel mit Diff > 25) | KI senkrecht | PORT R17 senkrecht (rote Pixel) |
|---|---|---|---|
| h24 x 440 | 674-700 (26) | 680-706 (26) | 683-702 (20) |
| h24 x 480 | 676-714 (38) | 686-720 (34) | 671-688 (18) |
| h24 x 560 | 658-700 (42) | 665-702 (37) | 642-669 (28) |
| h70 x 920 | 704-738 (34) | 702-734 (32) | 694-720 (27) |
| h70 x 1000 | 746-784 (38) | 746-779 (33) | 761-776 (16) |

Original und KI decken sich hier auf 0-10 px: der Schwanz ist ~34-42 px = 0,07-0,08 H dick (H 504 der gleichen Stelle).

## 3. Kopf, Arme, Beine, Wunden

### 3.1 Kopf (Vorderansicht h118, H 569; T4)

| Mass | KI | ORIGINAL |
|---|---|---|
| Lage seitlich | mittig (Kopfmitte 787, Umrissmitte 778) | rote Zone h128 x 755-822, Mitte 788; Umrissmitte 790 |
| Oberkante | 318 = 3 px unter der Umriss-Oberkante (Kopf IST die Kuppe) | helle Kuppe oben Mitte (Dossier N2.4: h116/h124 x 772-800, y 304-344) |
| zwischen den Schultern | Kopf-Oberkante 35-54 px UEBER den Schulterkuppen (353/372); Maul in Hoehe der Schultergelenke | Umriss: Kuppe in der Mitte, kein Hals |
| Groesse | 65 x 122 px = 0,11 x 0,21 H | nicht trennbar |
| Maul | offen, senkrecht, mit Zaehnen, 762-812 x 368-415 = 0,09 x 0,08 H, 0,09-0,18 H unter OK | rote Zone ab y 360 (h118 und h128 gleich), 0,08-0,26 H unter OK, 67-103 px breit (kopfrot.py; KI h128 62 px) |
| Augen | ein rundes Auge oben Mitte (782,355); ein zweites Auge auf der Schulter Bild-links = Wesen-rechts (705-715, 360-372); in der Seitenansicht h83-h97 rot gluehende Augen vorn unten am Kopf | nicht aufloesbar (320x240) |
| Seitenansicht h83-h97 | Kopf vorn unten am Buckel, Schnauze nach vorn-unten (h85 x 880-920, y 440-510) | dunkle/rote Stelle vorn unten am Buckel |

### 3.2 Sichelarm

- Seite: Wesen-rechts (= Bild-links in der Vorderansicht; in der Seitenansicht h83-h97 der vordere Arm). Bestaetigt Dossier
  Abschnitt 2.
- Laenge (KI h118, von Hand am Raster): Schulterkuppe 353 -> Spitze ~785 = 432 px = 0,76 H; Armband (Ellbogen, ~485) -> Spitze
  = 300 px = 0,53 H; Spitze 0,17 H ueber der vorderen, 0,11 H ueber der hinteren Sohle. ORIGINAL: Dossier T2 (n1370/1373)
  rot gebaenderter Teil 0,54 der Koerperhoehe. Klinge dunkel, gekruemmt, Spitze nach innen zum Schienbein.

### 3.3 Klauenhand

- Seite: Wesen-links (= Bild-rechts in der Vorderansicht).
- KI h118 (von Hand am Raster): Hand x 880-945, y 600-700 = 65 x 100 px (0,11 x 0,18 H), rote Krallen, Fingerspitzen 0,32 H ueber
  der vorderen Sohle (Oberschenkel-/Kniehoehe). ORIGINAL: Dossier Abnahme 1 5c ~80 x 60 px (1280er) bei h124/h132.

### 3.4 Beine und Jeans (h118)

| Mass | ORIGINAL | KI |
|---|---|---|
| Hosenbund | 553 = 0,42 H unter OK | 548 |
| Jeans-Ende | 639 = 0,57 H unter OK (kurze Hose, Laenge 86 px = 0,15 H) | 641 |
| Knie | - | ~700-720 = 0,68-0,71 H unter OK (Raster) |
| Fuesse | rot (Dossier T4) | rote Turnschuhe mit Schnuersenkeln (nur KI) |
| Breite (Umriss) | - | x 606-951 = 345 px, H/B 1,65 (Dossier-Handumriss h116: 1,84) |

Jeansband h112/h128: Original 548-632 / 555-675, KI 540-633 / 554-673 (front.py).

### 3.5 Wundzeichnung (Sichtbefund, nicht gemessen ausser der roten Zone)

- ORIGINAL: rote Zone Brustmitte/Gesicht (3.1); rote Kappe oben auf der Masse h0-h6 (Dossier N2.4); rote Querstreifen ueber
  den Buckel in der Seitenansicht h83-h97; Schwanz rot-weiss (h20-h73).
- Nur KI: rotes X ueber der Brust (h20-h73), rote Rippenstreifen am Buckel, rote Unterseite des Schwanzbogens, Auge auf der
  Schulter, Nietenarmbaender an beiden Armen.

## 4. Vergleich mit dem jetzigen Port-Modell (fade49ab8 = Modell von Referenz 2)

Port-Modell orthografisch gemessen (portmass.py, portansicht.py; H = Oberkante bis Sohle, Clip 1 Bild 0 = Gang, H 3829):
Schwanz an Knochen 8 (Brust), Wurzel K16 (-449,-2614,-38), Glieder 7 x ~466, Radien 150 -> 72.

| Punkt | Video | Port | Urteil |
|---|---|---|---|
| Schwanz-Wurzel Hoehe | Oberkante 0,03 H unter Umriss-OK (Original +-6 px) | Glied 16 Oberkante 0,26 H unter OK (Clip 0 und 1) | weicht ab: 0,23 H (~880 Einheiten) zu tief |
| Schwanz-Verlauf | Haken: waagerecht nach hinten, dann senkrecht | gerade schraeg nach hinten-unten, Gliedwinkel 25 -> 57 Grad unter der Waagerechten (Clip 0 B0) | weicht ab |
| Spitze | frei, 0,33-0,49 H ueber Sohle (Seite), 0,29-0,37 H (vorn) | Keyframe 0,09-0,10 H; im Spiel h109-h132 (Log Schwanzspitze) 364-725 Einheiten = 0,10-0,19 H | weicht ab (zu tief) |
| Laenge | >= 0,70 H (projiziert) | 3475 = 0,87-0,91 H (rechte Flanke projiziert 0,89 H) | im Rahmen (Video ist Untergrenze) |
| Dicke | KI 0,11-0,14 H am Bogen, 0,06-0,08 H Mitte; Original/KI von vorn 34-42 px | Durchmesser 300 -> 144 = 0,078 -> 0,038 H; Port-Bild h24/h70 16-28 px an denselben Spalten | weicht ab: Port 21-58 % duenner (h24 x 480: 18 gegen 38) |
| Bogen sichtbar h83-h97 | ja (rechte Flanke) | nie: h83/h85 Clip 0x0B Wurf (Gier 2043/1893), ab h92 schon Gang zur Kamera (Gier 1517) | weicht ab (Szene, Spur H) |
| Kopf Lage/Groesse | Oberkante 0-5 px (640er 2/0/3/5, Dossier N2.4) unter OK, 0,11 x 0,21 H | 6-9 px (640er, Dossier N2.4); Mesh 9 0,10 x 0,17 H | stimmt |
| Kopf Gesicht/Maul | rote Zone 0,08-0,26 H unter OK, abgegrenzt; KI: offenes Maul + Auge | Wundfleisch, Kopfkasten zu 30-33 % rot ohne abgegrenzte Zone (Port h112-h128) | weicht ab |
| Sichelarm | Wesen-rechts, Unterarm+Klinge 0,53 H, Spitze 0,11-0,17 H | Wesen-rechts, Mesh 14+15 0,51 H, Spitze 0,12 H | stimmt |
| Klauenhand | Wesen-links, ~65 x 100 (KI) / 80 x 60 (Dossier), Spitzen ~700 | Wesen-links, Port h118 Hand y ~600-690 | stimmt im Rahmen |
| Hosenbund (Bild h118) | 0,42 H unter OK | 634 = 0,50-0,55 H (Port-Sohle unter der Letterbox, H 564-626) | weicht ab: 0,08-0,13 H tiefer (Rumpf laenger, Beine kuerzer) |
| Jeans-Laenge | 0,15 H | 88 px = 0,14-0,16 H | stimmt |
| Breite | H/B 1,65 | Umriss x 524-958 = 434 px, H/B 1,30-1,44 | weicht ab (Port ~26 % breiter) |

Leisten nb2neu:
- 03_schwanz_h18-h66.jpg ("neu"): Schwanz tritt Bild-links in Hufthoehe aus und haengt zum Boden - Lage der Vorderansicht wie
  im Video, aber duenn (oben). Port R17 h24: Schwanz bei x 520-560 hoeher (654-684 / 642-669) als im Original (698-702 /
  658-700).
- 04_klaue_h90-h132.jpg: kleine Klaue auf Oberschenkelhoehe - stimmt mit 3.3.
- 07_gang_h84-h132.jpg: h84/h92 zeigt der Port den Buckel von oben (vorgebeugt, schon zur Kamera gedreht), das Video die rechte
  Flanke mit Schwanzbogen; ab h100 beide von vorn.
- Schritt 1-4 (Dossier) aendern Wurf, Loslassen, Blut und Takt, nicht das Modell (birkin_formen.py und EM30_HANGAR.EMD seit
  84e8a4d97 unveraendert, git log).

Tripo-Ansichten (nur KI, Lesehilfe; tripo_seiten.png, von Hand am Raster):

| Bild | Schwanz-Wurzel | Spitze | Dicke Mitte |
|---|---|---|---|
| image_1 RIGHT SIDE | oben am Ruecken, Oberkante ~0,02-0,04 H unter OK | ~0,05 H ueber Sohle | ~55 px = 0,12 H |
| image_2 RIGHT | oben am Ruecken | ~0,16 H | - |
| image_3 LINKS (zeigt die rechte Flanke) | oben am Ruecken | ~0,11 H | - |

Tripo stimmt mit dem Original in der Wurzel (oben am Ruecken) und widerspricht ihm bei der Spitze (Original haengt frei bei
0,33-0,49 H) und der Dicke (Original/KI-Video 0,07-0,08 H von vorn). image_3 beschriftet die Flanken andersherum als image_1.

## 5. Offene Fragen an den Nutzer

1. Schwanzwurzel: Das Original zeigt den Ansatz am hoechsten Punkt des Rueckens (0,03 H unter der Oberkante), der Port hat
   ihn 0,26 H tief an der Brust. Ist "bis in den oberen Ruecken" so gemeint (Ansatz ganz oben hinter dem Kopf)?
2. Schwanzspitze: Im Original haengt sie im Gang frei auf Knie-/Oberschenkelhoehe (0,29-0,49 H). Die Tripo-Modelle fuehren sie
   bis zum Boden. Original (frei haengend) oder bis zum Boden?
3. Dicke: Video von vorn 0,07-0,08 H, KI-Bogen an der Wurzel 0,11-0,14 H, Port 0,04-0,08 H. Soll der Schwanz an der Wurzel so dick
   werden wie der KI-Bogen?
4. Ende des Bogens: Die KI zeigt in h78-h97 ein verdicktes Ende mit Kralle, in h20-h73 eine spitze rot-weisse Spitze; das
   Original loest es nicht auf. Spitz (wie h20-h73) oder Krallen-Ende?
5. Rote Stelle zwischen den Beinen h125-h132 (Original und KI, 0,29-0,37 H ueber der Sohle): Schwanzende oder Wunde am Knie?
6. Kopf: Uebernehmen, was nur die KI zeigt (ein Auge oben, offenes senkrechtes Maul mit Zaehnen, Auge auf der Schulter
   Wesen-rechts)? Das Original zeigt nur helle Kuppe + rote Zone.
7. Proportion: Hosenbund im Port 0,08-0,13 H tiefer und Figur ~26 % breiter als im Video. Anpassen, oder bleibt der Koerper
   wie Referenz 2 ("nicht so schlecht")?
8. Szene h83-h97: Im Video ist Birkin in der rechten Flanke zu sehen und der Schwanzbogen gut sichtbar; der Port dreht schon ab
   h92 zur Kamera. Soll die Drehung spaeter kommen (Spur H)?
9. Vorderseite_Beschreibung.png: "linke Klaue" (oben) und "rechte Klaue" (unten) - Lesart A aus Sicht des Betrachters (obere
   Klaue liegt Bild-links), Lesart B aus Sicht des Wesens (wie die Augen-Beschriftung). Welche gilt? (Das Bild zeigt nicht die
   Sichel-Form.)
10. vlcsnap-2026-11-04-14h*: Das sind Bilder der KI-Fassung, keine Port-Bilder. Waren Port-Bilder gemeint? Dann fehlen sie.
