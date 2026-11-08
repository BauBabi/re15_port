# SF Formblatt V2: Sichel-Form (zweibeinig, Hangar), Vorgabe fuer den Modellbau (Pruefer, 2026-11-06)

⛔ Kein RE-Befund. Die Sichel-Form gibt es auf der Disc nicht. Alles hier ist Bildmessung am 80%-Video 4 (Hangar,
ROOM50A0 Schnitt 12). Keine Zahl hier ist eine @0x-Konstante. Gelesen und gemessen wurde nur, unter Version_80%, pri/,
synchro/ und re15_port ist nichts geaendert.

Grundlage: SF_LESER_A.md und SF_LESER_B.md (vollstaendig gelesen), eigene Nachmessung (Skripte unten).

## 0. Konventionen

| Begriff | Bedeutung |
|---|---|
| Koordinaten | 1280x960 (KI-Groesse); Original 640x480 mal 2. Wo "(640)" steht, gilt die Originalgroesse. |
| n, h | n = Dateibild (1-basiert), h = n - 1350 |
| R1 | Referenzbild Seite: n1435 (h85, birkin12). OK (Buckel-Oberkante) y 367, Sohle y 871, **H = 504** |
| R2 | Referenzbild vorn: n1468 (h118, birkin19). OK y 315, Sohle y 884, **H = 569** |
| Port-Ist | Lauf R17 (Stand fade49ab8 = Modell Referenz 2 84e8a4d97), Bild h118 mal 2: OK 326, Sohle ~896 (Letterbox), H ~570; Modell EM30_HANGAR.EMD, H 3830-3900 Einheiten |
| Seiten | **(W)** = aus Sicht des Wesens, **(B)** = aus Sicht des Betrachters. Von vorn gilt B-links = W-rechts. |
| Quelle | **O** = Original-Video 4, **K** = KI (_slp, birkinNN, Tripo), **N** = Nutzer-Vorgabe, **L** = Messwert eines Lesers, nicht nachgemessen |
| Sicherheit | sicher / wahrscheinlich / geschaetzt (geschaetzt = Augenmass an der Lupe) |

Seitenpruefung Seitenansicht: Das Wesen blickt nach B-rechts (+x), oben ist +y. Rechts (W) = vorn x oben = +z = zur
Kamera. Die Kamera sieht in h83-h97 also die rechte Flanke (W) mit dem Sichelarm vorn. Beide Leser haben das richtig.

**Abnahme-Tafel:** `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/leser/P_sichel/tafeln/abnahme_SF_formblatt_v2.jpg`
- **Oben:** R1 Original | R1 KI mit OK, Sohle, Wurzelschnitt (gelb), Wurzelmitte, Mittellinie (magenta), Dicken bei 0,2/0,5/0,9 der Laenge (gruen), Spitzenhoehe sowie Port-Ist fuer Gelenk 16 und Spitze (orange, gestrichelt).
- **Unten:** R2 Original | R2 KI | Port h118 mit den Vorgabelinien (gruen) Bund, Saum, Krallenband, Sichelspitze, Breite und rote Zone; beim Port zusaetzlich die Ist-Linien (orange).

## 1. Leser A gegen Leser B: Gegenueberstellung und Entscheid

| # | Punkt | Leser A | Leser B | Eigene Messung | Entscheid |
|---|---|---|---|---|---|
| 1 | Zuordnung birkin01-24 | n1351 ... n1482, bitgleich | dieselbe Liste, RMS 0,0 | Stichprobe 13 Freisteller (04, 06, 07, 09-14, 19, 20, 22, 23): mittlere Abweichung 0,0, Nachbarbild 6,2-20,4 | **bestaetigt**; alle aus dem Hangar, keiner aus dem Labor |
| 2 | vlcsnap-2026-11-04-14h* | KI-Bilder (Korrelation 1,000) | KI-Bilder (NCC 1,000) | nicht nachgemessen, beide unabhaengig gleich | **bestaetigt**: kein Port-Bild vorhanden (Frage F6) |
| 3 | Vorderseite_Beschreibung.png | roter Korridor, Logo BIOHAZARD 1.5 / bioflames | Bahnsteig-Form (Final Build) | Sichtpruefung: rot beleuchtete Bahnsteig-/Korridorszene, Frau links, Kreatur, Logo | **kein Widerspruch**: beide Beschreibungen zutreffend; nicht die Sichel-Form |
| 4 | Ist der Bogen h77-h98 der Schwanz? | "Video entscheidet": drei Glieder gleichzeitig (h92/h97) | Original trennt Schwanz und nach hinten gestreckten Arm links (W) nicht | Lupe h92/h97: Das Original zeigt den Bogen GETRENNT vom vorderen Arm rechts (W) mit Sichel. Einen fernen Arm links (W) zeigt das Original nicht trennbar. Die KI malt selbst eine HAND ans Bogenende. | **B hat recht fuer das Original.** A fuehrt einen KI-Befund als Videobeleg. Deutung als Schwanz = **N** (Nutzer 2026-11-05) |
| 5 | Wurzelhoehe | Oberkante 0,03 H unter OK (Spalte x 709) | Durchmessersprung 0,13-0,14 H unter Buckel | Wurzelschnitt Kerbe -> naechster Oberkonturpunkt, h83/h85/h92: **Mitte 0,152/0,166/0,154 H**; Schnitt-Oberkante 0,08-0,11 H; Kerbe 0,22 H; Schnittlaenge 0,18-0,19 H. Die Oberkontur laeuft ohne Stufe vom Buckel in den Schwanz (x 670-720: y 379-392 = 0,02-0,05 H unter OK) | **Gelenkmitte 0,15-0,17 H.** A's 0,03 H ist die Rueckenkontur ueber der Kerbe, kein Gelenkmass; daraus folgt A's Ueberschaetzung "~880 E. zu tief". B liegt 0,02-0,03 H hoeher als die Schnittmitte. |
| 6 | Port-Wurzel | Glied 16 Oberkante 0,26 H | Gelenk 16 Mitte 0,31 H | - | **kein Widerspruch**: Oberkante gleich Mitte minus Radius (0,04 H) |
| 7 | Dicke an der Wurzel im ORIGINAL | Spalte 619: ~34 px quer = 0,07 H | 32-38 px (640) = 0,13-0,15 H | Querprofil bei 0,2 der Laenge (h85/h92): Original-Aussenkante (Differenz zur Platte > 14) bis t +36, Innenkante (r-g 10-17 gegen 0-2 im Hintergrund) bis t -32, also **68 px = 0,135 H**; KI 60-62 px. Die Innenhaelfte ist dunkelrot und schattig | **B hat recht.** A hat nur die beleuchtete Aussenhaelfte gemessen. |
| 8 | Dicke Mitte / vor der Spitze | KI 0,06-0,08 / 0,05 H | 0,07-0,09 / 0,055-0,07 H | h85 bei 0,5: O 32-36 px, K 34 px = 0,07 H; bei 0,9: K 17 px = 0,034 H | gleich; Vorgabe S10 |
| 9 | Spitzenhoehe im Gang (Seite) | 0,33 / 0,42 H | 0,32 / 0,40-0,44 H | h83 0,32, h85 0,33, h92 0,44 H (KI-Skelett; O-Lage +-3-6 px laut A/B) | **gleich** |
| 10 | Schwanz beim Halten "bis zum Boden" | Tiefe nicht entscheidbar | reicht bis zum Boden | h61: Spitze y 433 (640) = Sohlenzeile des Fusses B-rechts (434); h69/h70: 35-38 px darueber | **A hat recht**: Bodenhoehe im Bild nur in h61, die Tiefe ist unbekannt. Kein Modellmass (Animation). |
| 11 | Rote Stelle zwischen den Beinen h125-h132 | Original zeigt sie (x 759-791, y 671-720) | Original kaum (r-g > 25: 0-12 px) | Fenster x 370-400, y 320-372 (640): Original r-g max 25-28, **400-408 px mit r-g > 20** (h125-h128), Hintergrund r-g -5; h118: max 11. KI: 221-225 px mit r-g > 25 | **A hat recht.** Die Schwelle 25 von B liegt ueber dem Rotniveau des Originals (Schwanz vorn: O r-g 20-34). Das Original zeigt eine roetliche Stelle am Innenknie rechts (W) = B-links. Ob Schwanzende oder Knie: nicht trennbar (F3) |
| 12 | Klauenhoehe im Port | stimmt im Rahmen | Port 0,10-0,20 H zu hoch (endet am Bund) | Original-Krallen (r-g > 18) y 345/351/351/353/360 (640) in h125/126/128/130/132 = **0,30-0,36 H ueber der Sohle**; Port-Krallen (r-g > 20 im Handbereich) y 338/322/312/314 in h118/125/128/132 = **0,38-0,47 H** | **B hat recht.** A hat ohne Krallenmessung die KI-Hand h118 mit dem Port verglichen. |
| 13 | Sichelspitze | KI 0,11-0,17 H, Port 0,12 | KI 0,20-0,27 H, Port 0,16 | Lupe KI h118/h125: Spitze y ~380-390 (640) = 0,18-0,22 H (geschaetzt) | Vorgabe **0,17-0,27 H** (KI, haengt vom Bild ab); Port 0,12-0,16 H (L), also leicht zu tief. Geringer Unterschied. |
| 14 | Bund/Saum im Port | 0,50-0,55 / 0,64-0,70; "Jeanslaenge stimmt" | 0,50-0,53 / 0,73-0,74 | Port h118, Blauprofil: Bund y 309-318 (640) = **0,52 H**; Saum an den Beinen 360-367 = **0,69-0,72 H**; Jeans 0,19 H. Original 0,42 / 0,57 (Jeans 0,15 H) | **B liegt naeher.** A's "Jeanslaenge stimmt" haelt nicht: Port-Jeans 0,19 H gegen 0,15 H. |
| 15 | Breite | Port ~26 % breiter | nicht gemessen | bei 0,2 H unter OK: Original 0,57-0,65 H (h112-h125; KI-Maske +-3 %), Port **0,73-0,75 H** | **A bestaetigt**: Port 15-28 % breiter, haengt von der Pose ab |
| 16 | Kopfgroesse (KI) | 0,11 x 0,21 H | 0,08 x 0,14-0,15 H (geschaetzt) | Original nicht trennbar | Port 0,10 x 0,16-0,17 H liegt dazwischen: **keine Aenderung** |
| 17 | Form der Schwanzspitze | offen (Frage) | Original "spitz" | Originalbreite bei 0,85-0,95 der Laenge (h83/h85/h92): 14-34 px (1280), KI 13-29 px; 320x240 loest spitz/stumpf nicht auf | **B's "spitz (Original)" ist nicht belegt**, bleibt Frage F2 |
| 18 | Rotverteilung Rumpf | Port ohne abgegrenzte rote Zone | Rot auf Schultern/oberer Brust bis 0,25-0,28 H, Bauch frei | Anteil r-g > 20 je 0,05-H-Band (Mitte +-0,12 H), h118: Original 0,11/0,19/0,52/0,68/0,57/**0,02/0,00** (0-0,35 H); Port 0,80/0,75/0,70/0,58/0,50/**0,48/0,62** | **beide bestaetigt**: Original hat helle Kuppe, rote Zone 0,10-0,25 H, Bauch frei; der Port ist durchgehend rot |

Weitere Pruefpunkte:
- Seiten verwechselt? Nein. Beide setzen Sichel rechts (W) = von vorn B-links, Klaue links (W) = B-rechts, Seitenansicht = rechte Flanke (W).
- KI-Details als Original gefuehrt?
  - A: Beleg "drei Glieder" (#4) und die Original-Dicke 0,07 H, die zu klein ist (#7).
  - B: "spitz (Original)" (#17) und "bis zum Boden" (#10) ueberzogen, rote Stelle im Original unterschaetzt (#11).
  - Maul mit Zaehnen, Augen, rotes X, Nietenarmbaender, Turnschuhe und Kralle am Bogenende fuehren beide korrekt als nur-KI.
- Tripo (K): Wurzel oben am Ruecken, Spitze am Boden 0,05-0,16 H, Laenge ~1,3 H, Dicke ~0,12-0,15 H. Bei Spitze und Laenge widerspricht das dem Original, **es gilt das Original**.

## 2. Formblatt: Vorgabe fuer den Modellbau

### 2.1 Schwanz

| Nr | Merkmal | Vorgabe | px im Referenzbild | Quelle | Sicherheit | Port-Ist |
|---|---|---|---|---|---|---|
| S1 | Deutung | Der Bogen h77-h98 ist der Schwanz; er reicht "bis in den oberen Ruecken" | R1 | N (+ O zeigt den Bogen, + K) | sicher (Nutzer) | Bogen im Port nie sichtbar |
| S2 | Wurzel, Gelenkmitte, Hoehe | **0,15-0,17 H unter OK** | R1 (674,450) | O-Kontur + K-Maske | wahrscheinlich (Projektion) | 0,31 H (Gelenk 16) |
| S3 | Wurzel, Oberseite | geht **ohne Stufe** in den Schulterbuckel ueber; Oberkontur ueber der Wurzel 0,02-0,05 H unter OK; Oberkante des Wurzelschnitts 0,08-0,11 H | R1 x 670-720 y 379-392; Schnitt oben (637,422) | O (+-3-6 px, A/B) | sicher | Gliedoberkante 0,26 H |
| S4 | Wurzel, Unterseite (Kerbe) | 0,22 H unter OK, darunter Luecke zwischen Schwanz und Ruecken | R1 (712,479) | K; O schwach (Differenz 5-10 in der Luecke gegen 12-17 am Bogen) | wahrscheinlich | - |
| S5 | Wurzel, Lage hinten | Wurzelmitte 0,23-0,28 H hinter dem hoechsten Buckelpunkt (x 792-816) | R1 | K | wahrscheinlich (Projektion) | an Knochen 8, X -380 |
| S6 | Verlauf (Haken) | ab Wurzelmitte, (hinten, unten) in H: (0,08 0,04) (0,14 0,08) (0,17 0,14) (0,19 0,20) (0,21 0,26) (0,22 0,33) (0,21 0,39) (0,19 0,45) **Spitze (0,15 0,51)**. Erst fast waagerecht nach hinten, dann senkrecht nach unten, im letzten Viertel 0,07 H wieder nach vorn | R1 Mittellinie (magenta) | O (Aussenkante +-6 px bis y 687) + K | wahrscheinlich (Projektion, Lateralrichtung unbekannt) | gerade schraeg, Gliedwinkel 25 -> 57 Grad |
| S7 | Hoechster Punkt | steigt **nicht** ueber den Buckel | R1 | O | sicher | stimmt |
| S8 | Spitze im Gang | haengt frei, **0,32-0,44 H ueber der Sohle** (h83 0,32, h85 0,33, h92 0,44) | R1 (599,707) | O + K | sicher (Hoehe) | Keyframe 0,09-0,10 H, im Spiel 0,10-0,19 H (L, A) |
| S9 | Laenge | projiziert mindestens 0,66-0,68 H (Seite); Rueckprojektion vorn h22 (Dossier SF) mindestens 0,86 H. **Port-Laenge 3475 E. (0,89 H) behalten** | R1 | O/K | sicher als Untergrenze | 0,89 H |
| S10 | Dicke (quer zur Mittellinie) | 0,15-0,2 der Laenge: **0,13-0,14 H**; 0,3: 0,10 H; 0,4: 0,08 H; 0,5: **0,065-0,07 H**; 0,6-0,7: 0,055-0,06 H; 0,9: **0,034 H**; danach Ende (F2) | R1: 68 / 52 / 42 / 34 / 27-29 / 17 px | O (0,2 / 0,3 / 0,5), K (Rest) | sicher bis 0,5, danach wahrscheinlich | Durchmesser 300-311 -> 144-182 E. = 0,08 -> 0,04-0,05 H; die Wurzel ist halb so dick |
| S11 | Dicke von vorn (Halten, Mittelstueck) | 0,07-0,08 H | h24/h70: O 26-42 px (L, A) | O | sicher | Port-Bild 16-28 px (L, A, nur rote Pixel) |
| S12 | Ende | **offen (F2)**: spitz (KI vorn h20-h73) oder Kralle (KI Seite h78-h97) | - | K widerspricht sich, O loest nicht auf | - | stumpf + Dorn (3 x Endradius) |
| S13 | Farbe | rot-weiss gebaendert | h20-h73 | O (Dossier N1.5) | sicher | stimmt (CLUT 483) |
| S14 | Halten h20-h73 (Animation) | tritt unterhalb des Bunds seitlich hinter der Huefte hervor: h20-h24 rechts (W) = B-links, waagerecht; h61-h70 links (W) = B-rechts, schraeg abwaerts; in h61 liegt die Spitze im Bild auf Sohlenhoehe | h24 Austritt (289,340) (640), h69 (430,313) (640) (L, B) | O | sicher (Bildlage), Tiefe offen | stimmt qualitativ (L, B) |
| S15 | Gang von vorn | Schwanz hinter dem Koerper; am Innenknie rechts (W) = B-links h125-h132 roetliche Stelle 0,29-0,37 H ueber der Sohle | h125 (640) x 370-400, y 320-372 | O (r-g 20-28) | Lage sicher, Deutung offen (F3) | Port zeigt ein rot-weisses Glied zwischen den Beinen (L, B) |

### 2.2 Koerper

| Nr | Merkmal | Vorgabe | px im Referenzbild | Quelle | Sicherheit | Port-Ist |
|---|---|---|---|---|---|---|
| K1 | Kopf | mittige Kuppe des Umrisses, Oberkante 0-0,03 H unter OK, kein Hals; Groesse nur in der KI (0,08-0,11 breit x 0,14-0,21 H hoch) | R2 Mitte x ~785 | O (Lage), K (Groesse) | sicher (Lage) | stimmt (0,035 H, 0,10 x 0,16-0,17 H): **nicht aendern** |
| K2 | Farbverteilung Oberkoerper | helle Kuppe 0-0,08 H; **rote Zone mittig 0,08-0,26 H**, 0,13-0,19 H breit; **Bauch ab 0,26 H ohne Rot** | R2 y 360-463, x ~740-830 | O (A, B und #18) | sicher (Lage) | durchgehend rot 48-80 % von 0 bis 0,35 H |
| K3 | Hosenbund | **0,42 H unter OK** (0,40-0,43 in h109-h125) | R2 y 554 | O | sicher | 0,52 H |
| K4 | Jeanssaum | **0,57 H unter OK** (0,57-0,62); Jeans also 0,15 H | R2 y 639 | O | sicher | 0,69-0,72 H (Jeans 0,19 H) |
| K5 | Breite bei 0,2 H unter OK | **0,57-0,65 H** | R2 y 429, Original 334 px | O (KI +-3 %) | wahrscheinlich (Pose) | 0,73-0,75 H |
| K6 | Sichelarm | rechts (W) = B-links von vorn; Unterarm + Klinge 0,53-0,54 H; Klinge dunkel; Spitze **0,17-0,27 H ueber der Sohle** | R2 Spitze y ~770 | O (Seite, Laenge Dossier T2), K (Spitze) | sicher (Seite/Laenge), wahrscheinlich (Spitze) | 0,51 H; Spitze 0,12-0,16 H (L) |
| K7 | Klauenhand, Hoehe | links (W) = B-rechts; Krallen **0,30-0,36 H ueber der Sohle** (Saumhoehe), Hand 0,30-0,37 H seitlich neben der Mitte | h125-h132 (640) x 478-501, y 336-360 | O | sicher | Krallen 0,38-0,47 H |
| K8 | Klauenhand, Groesse | **kleiner** als EM30 (Nutzer); rote Krallen im Original 11-15 x 9-16 px (640) (L, B), KI 31-40 x 28-32 | - | N + O | sicher (N) | Klaue 0,55 x EM30 |
| K9 | Farbe | wie die anderen Birkin-Formen (nicht das dunkle Video); Spider-Man mit Originaltextur | - | N | sicher | - |
| K10 | Nur in der KI, **nicht bauen** (bis F4) | offenes Maul mit Zaehnen, Auge oben, Schulterauge rechts (W), rotes X ueber der Brust, Nietenarmbaender, rote Turnschuhe mit Schnuersenkeln, Hand/Kralle am Bogenende | - | K | - | nicht vorhanden |

## 3. Was am bisherigen Port-Modell geaendert werden muss

Datei `re15_port/tools/birkin_formen.py` (EM30_HANGAR, `schwanz_kette`, Stand fade49ab8 = Referenz 2). Port-H 3830-3900 E.;
Umrechnung 0,01 H ~ 38-39 E. Alle Bildmasse sind Projektionen der Kamera 50A0/12. Pruefen mit einem Seitenbild des Ports
in derselben Kamera bei h85 (Tafel oben, Linien muessen passen).

| # | Was | Ist | Soll | Grund |
|---|---|---|---|---|
| Ä1 | Wurzel `punkte[0]` (-380,120,0) an Knochen 8 | Gelenkmitte 0,31 H unter OK | **0,15-0,17 H**, also ~0,15 H = **~550-620 E. hoeher**. Oberseite buendig mit dem Buckel (S3), Unterseite-Kerbe 0,22 H (S4). Knochen 8 kann bleiben. | S2-S4 |
| Ä2 | Ruhe-Mittellinie `punkte[1..6]` | gerade schraeg nach hinten-unten | Haken nach S6; in Einheiten ab Wurzel (hinten, unten) etwa (310,150) (540,310) (650,540) (730,770) (810,1000) (850,1270) (810,1500) (730,1730) Spitze (580,1960). Die Projektion ist kuerzer als 3475 E.; den Rest durch Seitwaertslauf oder Verkuerzung zur Kamera aufnehmen, nicht durch eine tiefere Spitze | S6, S8, S9 |
| Ä3 | `radien` [150,146,140,132,120,102,72] | Durchmesser 0,08 -> 0,04 H | **etwa [260, 240, 195, 160, 130, 110, 65]** (Durchmesser 0,135 -> 0,034 H); die Mitte bleibt, die Wurzel wird fast doppelt so dick | S10 |
| Ä4 | Ende `dorn=3.0` | stumpf + Dorn | nach F2: spitz auslaufend oder Kralle | S12 |
| Ä5 | Nachschwingen `bewegung` (schwere/grenze) | Spitze im Spiel 0,10-0,19 H | Spitze im Gang **0,32-0,44 H ueber der Sohle** halten (Spitze frei, nicht am Boden) | S8 |
| Ä6 | Klauenarm `ducken.ellbogen_links=60` | Krallen 0,38-0,47 H ueber der Sohle | **0,30-0,36 H** (Saumhoehe): Ellbogen weniger beugen oder Unterarm laenger; Groesse "kleiner" bleibt | K7, K8 |
| Ä7 | Textur Oberkoerper (umfaerben/umtexturieren) | rot 48-80 % ueber 0-0,35 H | helle Kopfkuppe, abgegrenzte rote Zone 0,08-0,26 H mittig, Bauch ohne Rot, Grundfarbe nach K9 | K2 |
| Ä8 | Koerperproportion (**erst nach F1**) | Bund 0,52 / Saum 0,71 / Breite 0,74 H | Bund **0,42**, Saum **0,57**, Breite **0,57-0,65 H**: Rumpf kuerzer, Beine laenger bzw. weniger Kniebeuge, Schultern/Oberarme schmaler. Ob Pose oder Modell, trennt das Bild nicht. | K3-K5 |
| Ä9 | Sichelspitze (niedrige Prioritaet) | 0,12-0,16 H | 0,17-0,27 H | K6 |
| Ä10 | **Szene, nicht Modell** (Spur H, Schritt 1-4) | Port ab h92 schon zur Kamera gedreht (Gier 1515-1517, L, A), Bogen nie sichtbar | wie im Original: rechte Flanke (W) h83-h97, Drehung zur Kamera h98-h109 | S1, Original |

Unveraendert bleiben: Kopflage und -groesse (K1), Seiten von Sichel und Klaue, Schwanzlaenge (S9), Schwanzfarbe (S13),
Lage des Schwanzes beim Halten (S14).

## 4. Fragen an den Nutzer (nur was das Material nicht entscheidet)

| # | Frage |
|---|---|
| F1 | **Koerper:** Gemessen am Original liegen im Port der Hosenbund 0,10 H und der Saum 0,14 H zu tief, und die Figur ist 15-28 % zu breit. Das betrifft den von Ihnen gelobten Koerper von Referenz 2 ("nicht so schlecht"). Soll Ä8 gebaut werden? |
| F2 | **Schwanzende:** Die KI zeigt es in der Vorderansicht (h20-h73) spitz und rot-weiss, in der Seitenansicht (h78-h97) als Hand mit roter Kralle. Das Original loest das nicht auf. Spitz oder Kralle? |
| F3 | **Rote Stelle am Innenknie** rechts (W) = B-links in h125-h132, 0,29-0,37 H ueber der Sohle, im Original belegt: Ist das das sichtbare Schwanzende (dann haengt es dort zwischen den Beinen) oder eine Wunde am Knie? |
| F4 | **Kopfdetails nur in der KI:** offenes Maul mit Zaehnen, ein Auge oben, ein Auge auf der Schulter rechts (W). Das Original zeigt nur die helle Kuppe und die rote Zone. Uebernehmen oder weglassen? |
| F5 | **Vorderseite_Beschreibung.png** (Bahnsteig-Szene, nicht die Sichel-Form): "linke Klaue" (oben) und "rechte Klaue" (unten). Lesart A aus Sicht des Betrachters: "linke Klaue" = die Klaue Bild-links = rechte Klaue (W) des zugewandten Wesens. Lesart B aus Sicht des Wesens, wie die korrigierte Augen-Beschriftung: "linke Klaue" = linke Klaue (W). Welche gilt? |
| F6 | Die vlcsnap-2026-11-04-14h*-Dateien sind Bilder der KI-Fassung, keine Port-Bilder. Waren Port-Screenshots gemeint? Dann fehlen sie; verglichen wurde mit Lauf R17. |
| F7 | (Material) Final Build 58,5-64,5 s (Birkin mit rot-weiss gebaendertem Stab) und Video 4 n790-n840 (rote Kreatur): Gehoert das zur Sichel-Form? Dann gaebe es eine zweite Original-Quelle. |

Durch Original oder Nutzer entschieden, also KEINE Fragen mehr:
- Bogen = Schwanz (N)
- Wurzelhoehe (S2/S3)
- Spitze frei haengend statt am Boden (S8, Original gegen Tripo)
- Dicke (S10, Original)
- Laenge behalten (S9)
- Klaue tiefer (K7, Original)
- Drehung spaeter (Ä10, Original)

## 5. Werkzeuge und Belege

Alle Hilfsdateien liegen unter `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/leser/P_sichel/` (Python, nur lesend).

| Wofuer | Dateien |
|---|---|
| Zuordnung, Deckung | `py/zuordnung.py`, `py/deckung.py` |
| Hintergrundplatte | `py/platte2.py` -> `platte_modus.npy` |
| Bogen und Wurzelschnitt | `py/seite1.py` - `py/seite3.py` -> `seite2.json`, `seite3.json` |
| Querprofile Original/KI | `py/quer.py`, `py/quer2.py`, `py/ende.py` -> `quer.json` |
| Rot zwischen den Beinen | `py/schritt.py` |
| Klaue | `py/klaue.py`, `py/klaue2.py`, `py2/m_klaue_port2.py` |
| Port-Masken (R17) | `py/port1.py` - `py/port3.py` -> `port2.json`, `port3.json` |
| Weitere Messungen | `py2/m_kerbe.py` (Kerbe im Original), `py2/m_pfad.py` (Mittellinie in H), `py2/m_breite.py`, `py2/m_bund.py`, `py2/m_h61.py`, `py2/m_rot.py` |
| Tafel | `py2/tafel_abnahme.py` |

Grenzen der Messung:
- Das Original ist 320x240 hochskaliert, der Gang ist dunkel.
- Seitenmasse sind Projektionen (die Kamera sieht von schraeg oben).
- Port-Bildwerte stammen aus Lauf R17 mit seiner eigenen Gangphase.
- Die Port-Sohle liegt an der Letterbox (H +-5 px).
