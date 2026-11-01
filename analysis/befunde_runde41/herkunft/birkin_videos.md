# Herkunft Birkin des 2. Kampfes: Videos und 40%-Disc

Stand 2026-11-01, abgeschlossen. Reine Ermittlung, kein Spielcode, kein Commit.
Trennung: **Disc** = `Version_40%/` (Capcom, Nov 1996). **Mod** = `info/Re1.5/` (Magic Zombie Door). **Port** = `re15_port/`. **RE2** = `re15_port/shared_assets/RE2/`.
Modellblaetter: `birkin_modelle/blatt_*.png` (paralleler Agent). Bilder dieses Dossiers: `birkin_videos/`.

## 0. Ergebnis

1. "Dieser Birkin" (U-Bahn-Kampf, vor jeder Mutation) ist das Mod-Modell **M1** (`info/Re1.5/PSX/EMD/CDEMD1.EMS` Sektor 2015, 228460 B, md5 819acc8c). **Sicher** (Abschnitt 2).
2. In keinem der 80%-Videos zeigt sich ein Birkin mit M1-Merkmalen: dunkle Haut, abgeschnittene Jeans, nackte Beine und Fuesse, G2-Dreifachklaue, rotes Auge mitten auf dem Ruecken. **Sicher** fuer 307 (vier Ansichten). **Wahrscheinlich** fuer 50A (Farbmessung). **Offen** fuer 50E: das Gruenlicht verdeckt die Merkmale.
3. Alle auswertbaren Video-Birkins passen zum Disc-Modell **D0** (Typ 0x30). Merkmale: blasse Haut mit roten Adern, Birkins Kopf mittig, lange Jeans, dunkler Umschlag, helle Knoechel, dunkle Schuhe. **Sicher** fuer 307. **Wahrscheinlich** fuer 50A.
4. Den 2. Kampf zeigen die Videos nur an zwei Stellen, beide Standbilder: Bahnsteig #143 und 509 #145. Dort steht die Riesenform (eigene Spur). Einen Birkin in Menschengestalt im U-Bahn-Bereich gibt es in keinem Video. **Sicher** fuer die vorhandenen Bilder.
5. Die Disc hat keine U-Bahn-Raeume (508, 509 fehlen), kein M1 und keinen Text oder Bild zu diesem Birkin. **Sicher.**
6. Antwort: Fuer M1 gibt es **keine Referenz**, weder auf der Disc noch in den Videos. M1 ist ein Fan-Modell. README Z. 92 sagt das selbst. [Gegenprobe] Die Bytes stammen aus Capcoms Disc-D0 (Skelett, alle Keyframes, Meshes 8-14) und RE2-G2 (Beine, Fuesse, Klaue, Teile der Textur) plus neuen Teilen; G3 ist nicht belegt (Abschnitt 2). Als **Gestalt** hat M1 im 40%/80%-Material keine Vorlage; aus dem Nichts erfunden ist es nicht. Die einzige belegte Gestalt fuer den Kampf am Bahnsteig ist die Riesenform (#143, #145).

## 1. Quellen und Zeitbezug

- Video A: `Version_80%/Biohazard 1.5 Final Build.mp4`. Diashow aus 174 Fotos #000 bis #173, je 0,8 s (Schnittliste `analysis/story_begegnungen/pruefung/video_a.md` §9).
- Video B: `Version_80%/Resident Evil 1.5 (All Footage).mp4`, 14:19, 320x240, 30 fps.
- CD1 bis CD4: `documentation/Biohazard 1.5 Complete Disc Video N of 4 (Resident Evil 1.5) (360p).mp4`, 73/44/54/59 s.
- Zeitbezug gemessen per Korrelation (64x48, normiert): CD3 0:48.00 = B 3:59.27 (0,954). CD4 0:55.60 = B 13:06.87 (0,971). CD4 0:21.33 = B 12:32.87 (0,950). Also **CD3 = B - 3:11.3** und **CD4 = B - 12:11.3 bis 12:11.5**. Massgeblich sind CD3/CD4, sie sind schaerfer als B.
- Einzelbilder: `ffmpeg` -> `scratchpad/s41video/f{a,b,c1..c4}/%05d.jpg`, Bild n = n/30 s. A gemittelt je Foto: `fa_avg/NNN.png`.

## 2. Welches Modell ist "dieser Birkin" (Bytes)

- Disc: Typ 0x30 steht nur in `ROOM3070` (`Sce_em_set` @0x3064, @0x30AC) und `ROOM3071` (@0x3072, @0x30BE, @0x30D6). Typ 0x36 steht nur in `ROOM3080` @0x78C. Eigener Zensus aller `Sce_em_set` (Op 0x44) ueber alle Disc-RDTs. **Sicher.**
- Mod: 0x30 in 3070 (@0x33CE, @0x340C), 3071 (@0x3434, @0x347A), 5080 @0x746, 5081 @0x742, 5090 @0x124A, 5091 @0x1232, 50E0 (@0xACE, @0xAEE), 50F1 (@0x9AC, @0x9D4). 0x36 in 3080 @0x7D4. **Sicher.**
- Archivwahl ueber Flag (1,0) (`FUN_80022300`, `bltz` @0x80022334). Mod 5080/5090 setzen `Set(1,0)=1` (@0x742, @0x1246) -> CDEMD1 -> **M1**. 3070, 50E0 und 50F1 bleiben auf CDEMD0 -> Mod-0x30 Sektor 1261. Das ist **byte-gleich mit Disc D0** (md5 68eea28c, 226404 B, eigener Vergleich). **Sicher.**
- M1 gibt es nicht auf der Disc: kein Blob mit 228460 B (`birkin_modelle/blobs.txt`). Die Mod nennt es selbst: README Z. 92 "Added a new Birkin model adapted from RE2 to use in Subway area". README Z. 158 "Added new room 509 (WIP)". README Z. 380 "Added Birkin battle in room 50E (based on Complete Disc's video #4)". **Sicher.**
- Bauweise von M1 (paralleler Agent, `birkin_modelle/`):
  - Meshes 8 bis 14 sind vertexgleich mit D0. [Gegenprobe, eigene Messung] Meshes 3/6 liegen mit allen 16 Punkten <= 1 Einheit an RE2 G2 (0x31) Mesh 14/17, davon 6 exakt (nicht "identisch"); Beine 1/2/4/5 <= 2 Einheiten (nicht +-1). Die Klaue 15/16 stammt aus G2-Teilen.
  - Textur: 125 von 380 Kacheln mit SAD<8 aus G2 (0x31). [Gegenprobe] G3 hat nur 16, das liegt im Grundrauschen (G4 11, G1 6, D0 2; `textur_suche.txt`); die staerkere Kachelsuche (`textur_karte.txt`) findet G2 17+23, G1-aehnlich 4, G3 0. Ein G3-Anteil ist nicht belegt.
  - In einer ungenutzten Texturecke (301,7) steht eine Wolverine-Figur (`wolverine_zoom.png`).
  - **Sicher** (fremde Messung, hier gegengelesen).

## 3. Jede Birkin-Stelle in den Videos

Kamera = Raum/Cut nach Hintergrundabgleich (Vorarbeit `video_a.md`, `video_b.md`, hier nachgesehen).

| Video, Zeit | Raum, Cut | Zu sehen | Modell-Urteil |
|---|---|---|---|
| A #073, #074 (0:58.5-1:00.1) | 307 Cut 8 | Birkin im Tor, von vorn, Stange quer. Lange blaue Jeans bis zum Knoechel, helle Flecken an den Fuessen (`a_kandidaten`, Lupe `scratchpad/bv/a/l073b.jpg`) | D0, wahrscheinlich (Foto vom Fernseher) |
| A #077 (1:01.7) | 307 Cut 10 | Nah: blasse Brust, rote Adern, Kopf mittig, Mund/Hals rot | D0, wahrscheinlich |
| A #079, #081 (1:03.3-1:05.7) | 307 Cut 12 | Birkin mit Stange gegen Elza | D0-Gestalt, wahrscheinlich |
| A #080 (1:04.1) | 307 Cut 1 | Figur, Blitz | nicht bestimmbar |
| A #141 (1:52.9) | 307 Cut 5 | Birkin vorn unten, blass ueberstrahlt, rot | D0-Farbe, wahrscheinlich |
| A #142 (1:53.7) | 307 Cut 4 | helle Grossfigur hinten, Leon links | D0-Farbe, wahrscheinlich |
| A #143, #145 (1:54.5, 1:56.1) | Bahnsteig, 509 Cut 11 | Riesenform, dunkel. **Ausgenommen, eigene Spur** | - |
| B 2:05.6-2:08.0 | 307 Cut 8 | Birkin hinter dem Tor, dunkel, Jeans (`videoB_307.jpg`) | D0-Gestalt |
| B 3:11.0-3:12.5 | 307 Cut 10 | Nah: menschlicher Kopf mittig, Gesicht grau, Mund rot, rote Risse auf der Brust | D0-Gestalt, sicher. [Gegenprobe] Begruendung berichtigt: M1 traegt mittig dasselbe D0-Kopfmesh 9, aber dunkel G2-bemalt ohne Menschengesicht. Entscheidend sind das Menschengesicht und die blasse Brust |
| **CD3 0:43-0:49** (= B 3:54-4:00) | 307 Cut 8 | Tor oeffnet sich. Birkin steht im Tor: blasser Rumpf, Kopf mittig, lange Jeans, dunkler Umschlag am Schienbein, helle Knoechel, dunkle Schuhe (`cd3_307_cut8_lupe.jpg`) | **D0, sicher** |
| **CD3 0:50-0:52** (= B 4:01-4:03) | 307 Cut 10 | Nah: Kopf mittig mit Haar, rote Mund-/Halswunde, blasse Schultern mit roten Adern (`cd3_307_cut10.jpg`) | **D0, sicher** |
| B 7:10.6-7:14.0 | 307 Cut 8, 4, 10 | Birkin bricht durchs Tor, Elza (Cut 4), Nah | D0-Gestalt |
| B 13:28.5-13:49 | 307 Cut 3, 4 | Leon kaempft gegen Birkin, rote Treffer 13:39, 13:46 | D0-Gestalt |
| **B 13:50-13:56.9** | 307 Cut 5 | Ganzer Koerper von hinten. Heller Buckel mit rotem Adernstern, lange dunkle Jeans, helle Knoechel, dunkle Schuhe, Stange rechts (`vergleich_307_hinten.jpg`) | **D0, sicher** |
| CD4 0:20.9-0:22.1 (= B 12:32.4-12:33.6) | 50E Cut 8 | Grosse Kreatur faellt von oben vor Leon. Im Gruenlicht oliv-braun, gebueckt, lange Arme, Hand mit Krallen (`cd4_50e_cut8_lupe.jpg`) | **offen**: Jeans/Fuesse nicht erkennbar |
| CD4 0:22.2-0:26.2 | 50E Cut 5 | Dieselbe Figur hinten am Gelaender, rosa-violett, Rumpf hell mit roter Mitte, ca. 40 px hoch (`cd4_50e_cut5_lupe.jpg`) | offen |
| **CD4 0:51.6-0:56.8** (= B 13:03-13:07.3) | 50A Cut 12 | Birkin packt Spider-Man (0x2B), wirft ihn und kommt mit der Stange auf die Kamera zu. Buckel rot-weiss gefleckt, weisser Fetzen am Guertel, blaue Jeans an der Huefte, langer violetter Arm (`cd4_50a_lupe.jpg`, `cd4_50a_beine.jpg`) | **D0 wahrscheinlich** (Abschnitt 5). Beinlaenge **offen** |

Kein Birkin (geprueft):
- CD1 (Stufe 1), CD2 (Kanal, Spinnen): 1-fps-Boegen `cd1_1fps.jpg`, `cd2_1fps.jpg`.
- CD4 0:29-0:35 (408): dunkelrotes Affenwesen, keine Jeans.
- CD4 0:36-0:41: blauer Gang, mehrere braun-orange Wesen am Boden vor Elza, keine Jeans, nicht menschengross (`cd4_blauer_gang.jpg`).
- A #039/#040: 200 Cut 13, dunkle Figur ganz vorn ohne helle Haut und ohne Jeans. Die Mod setzt in 2001 Typ 0x16 (Zombie) sowie John und Sherry; das ist Mod, kein Beleg fuer das Video.
- A #022, #086, #106-#112 (Spinne), #013/#018 (114, rotes Riesenwesen), #114/#062/#066 (304, in der Mod Typ 0x29 `ROOM3040 @0x1310`), #144, #150, #155, #165, #166/#167 (308, nur blasse Schemen), #168 (509 Konsole).
- B 10:01.6-10:02.8: Trailer, schwarzes Stachelwesen.
- `videoA_kandidaten.jpg`.

## 4. Merkmale je Modell

| Merkmal | D0 Disc 0x30 | M1 Mod-Subway | RE2 G1 (0x30) | RE2 G2 (0x31) | RE2 G3 (0x33) |
|---|---|---|---|---|---|
| Haut | blass rosa, rote Adern | dunkelgrau, leuchtend rot | Laborkittel weiss | dunkelgrau-rot | dunkelgrau-rot |
| Kopf | Birkins Kopf mittig, blond | mittig dunkle Masse, Gesicht an der rechten Brust | Kopf mittig | Gesicht auf der Brust | kleiner Kopf, Auge Schulter |
| Ruecken | heller Buckel, roter Adernstern | grosses rotes Auge mittig | Kittel | Auge oben rechts | Auge mittig |
| Hose | lange Jeans, dunkler Umschlag | abgeschnitten am Knie | lange Jeans | abgeschnitten | keine |
| Fuesse | helle Knoechel, dunkle Schuhe | nackt, rosa | Schuhe | nackt | Krallenfuesse |
| Arme | rote Sichel / violette Klaue | G2-Dreifachklaue, hell | Stange, Auge am Arm | Dreifachklaue | vier Arme, Klauen |

Video 307 (CD3, B 13:50) trifft D0 in allen Spalten. M1, G2 und G3 widersprechen bei Hose, Fuessen, Haut und Kopf. G1 widerspricht durch den Kittel. Video 50A: Gestalt, Stange, weisser Guertelfetzen und violetter Arm passen zu D0. Die Hautfarbe ist gemessen (Abschnitt 5). Video 50E: nicht entscheidbar.

## 5. Farbmessung Oberkoerper (gegen Lichtstaerke unempfindlich)

Werkzeug `birkin_videos/werkzeuge/farbtest.py`.
- Video-Pixel ueber dem Guertel. Freistellung per Differenz zum Raumhintergrund, ausgerichtet per Homographie. Weissabgleich je Kanal am Hintergrund.
- Modelle D0 und M1 in derselben Spielkamera, je 40 Ansichten (5 Posen x 8 Drehungen). Beide Modelle wurden wie das Video verschlechtert: 320x240, Chroma halbiert, JPEG q35.
- Kennzahl rr = R/(R+G+B), blaue Pixel ausgenommen.
- Raumlicht ist neutral: Disc `ROOM3070` LIT @0x564/@0x5DC und `ROOM50A0` @0x748 haben alle drei Lichter (128,128,128), Ambient (63,63,63). Fuer den 80%-Stand von 50A ist das nicht belegt.

| Stelle | Quelle | Video rr | D0 rr (min-max) | M1 rr (min-max) |
|---|---|---|---|---|
| 307 Cut 5 hinten | B 13:50.5-13:55.5, 6 Bilder | 0,391 | 0,385-0,430 | 0,425-0,528 |
| 307 Cut 8 vorn (Kontrolle, Querstange ausgeblendet) | CD3 0:46-0:49, 7 Bilder | 0,371 | 0,378-0,425 | 0,439-0,491 |
| 50A Cut 12 vorn | CD4 0:55.2-0:56.0, 4 Bilder | 0,365 | 0,377-0,421 | 0,442-0,501 |

- Die Werte bleiben bei Masken-Erosion 3 und 5 px stabil (0,391/0,393, 0,365/0,364). Randpixel des Hintergrunds verfaelschen also nichts.
- Die Kontrolle zeigt: In der CD-Kette misst der sichtbar D0-artige Birkin aus 307 rund 0,37, etwa 0,02 bis 0,03 unter der D0-Simulation. 50A liegt mit 0,365 gleichauf. M1 laege mindestens 0,07 hoeher.
- **Urteil:** Die Haut in 50A ist so blass wie D0 in 307 und nicht so rot wie M1. Wahrscheinlich, nicht sicher: Der 80%-Lichtwert von 50A ist unbekannt.
- Maske: `farbtest_maske_307c8.png`. Q-Wert und Korrelation trennen nach der Verschlechterung nicht sauber und gehen nicht ins Urteil ein.

## 6. Zeigen die Videos einen 2. Kampf dieser Gestalt?

- **U-Bahn/Bahnsteig:** Nur A #143 (Bahnsteig, Kamera in keinem Bestand) und #145 (509 Cut 11). Beide zeigen die Riesenform als Standbild. #144 (509 Cut 1), #150 (Halle wie #143) und #168 (509 Konsole) zeigen keinen Birkin. B, CD1 bis CD4 haben kein Bild aus 508/509/Bahnsteig (`video_b.md` Z. 298, hier mit 1-fps-Boegen bestaetigt). **Sicher.**
- **50E:** Eine grosse Kreatur faellt vor Leon und steht dann am Gelaender (CD4 0:20.9-0:26.2). Ein Kampf ist nicht zu sehen, Leon laeuft weg. Gestalt massig mit langen Armen; Birkin wahrscheinlich, Modell offen. Die Mod hat den 50E-Kampf nach genau diesem Clip gebaut (README Z. 380) und nutzt dort D0.
- **50A:** Birkin gegen Spider-Man, kein Spieler im Bild. Disc 50A0/50A1 setzt keinen 0x30. [Gegenprobe] Die Disc-STAGE5.BIN registriert aber Typ 0x30 (@0x8011dd68 -> 0x80116a44, Modul = STAGE3-Birkin, verschoben um 0x814). Der Stufe-5-Birkin der Videos ist im 40%-Code also vorbereitet; geladen wuerde die einzige 0x30-TOC-Zeile = D0/D1.
- **307:** Echte Kaempfe mit Elza (A #079/#081, B 7:10) und Leon (B 13:28-13:56). Das ist der 1. Kampf mit D0.
- **Ergebnis:** Kein Video zeigt einen Kampf mit M1 oder einem anderen Birkin in Menschengestalt nach 307 ausser 50A/50E. Diese sehen D0-artig aus, soweit messbar. **Sicher** fuer "kein M1", **wahrscheinlich** fuer "50A = D0".

## 7. Bildbelege auf der 40%-Disc

- **Raeume:** Disc STAGE5 hat 501, 503, 504, 506, 50A, 50C, 50D (nur BSS), 510, 511, 512, 514. **Kein 508, 509, 50E, 50F.** STAGE6 hat nur 603, 604. Kein Bahnsteig-Hintergrund. **Sicher** (Dateiliste).
- **Modelle:** Birkin-Familie nur 0x30 (CDEMD0 Sektor 1263, CDEMD1 Sektor 1266) und 0x36 (Sektor 1374/1377, Oberkoerper mit einem Arm, Aufzugszene 3080). Keine Textur mit G2-Kunst. **Sicher** (`birkin_modelle/blobs.txt`, `textur_suche.txt`). [Gegenprobe] Eigene Messung: Disc-TOC deckt beide Archive lueckenlos (25 Eintraege, 0 Sektoren ohne TOC); M1-TIM 0 von 63 Stichproben im ganzen Rohabbild `BH1.5-40%.bin`.
- **Code:** [Gegenprobe] Birkin-Modul 0x30 in STAGE3.BIN und STAGE5.BIN der Disc (siehe Abschnitt 6).
- **Texte** (`TEXTE_40.md`, 291 Zeilen):
  - バーキン und ウィリアム kommen 0-mal vor.
  - Das Monster wird nur ohne Aussehen genannt: 307 #7 "kam aus diesem Aufzug", 307 #13 "unbekanntes Monster", 308 #0 "hartnaeckiger Kerl", 30E #21 "lauert vielleicht irgendwo", 501 #29 (tot) "zerstoerte die Energie".
  - Keine Zeile zu U-Bahn oder Bahnsteig. **Sicher.**
- **Bilddateien:**
  - Alle 12 `DATA/*.TIM` angesehen (`disc_data_tims.jpg`): Titel mit Reptilauge im Logo, Spielerwahl, Optionen, Waffen, YOU DIED. Kein Birkin. **Sicher.**
  - Die 15 PIX (MAP01-0D, ITEMALL, MIXITEM) habe ich nicht angesehen. Offen, nach Dateinamen Karten und Items.
- **Ergebnis:** Die Disc hat keinen Bildbeleg fuer M1 und keinen fuer einen 2. Kampf im U-Bahn-Bereich.

## 8. Antwort

Fuer M1 gibt es keine Referenz, weder auf der Disc noch in den 80%-Videos. Staerke der Gegenbelege:
- **stark:** 307 (CD3 und B 13:50, D0 in allen Merkmalen), Disc-Bytes (kein M1, keine U-Bahn-Raeume), README Z. 92/158, M1 aus D0 + RE2-G2-Teilen + neuen Teilen mit Wolverine-Bild ([Gegenprobe] G3 gestrichen).
- **mittel:** 50A, Farbmessung mit Kontrolle.
- **keiner:** 50E, nicht entscheidbar.

Lesart (a): Am unveraenderten 40%-Stand ist M1 nicht vorhanden; es gibt dort keinen 2. Birkin-Kampf. Der zweite Auftritt der Disc ist der Aufzug 308 mit D36.
Lesart (b): Der unveraenderte Birkin des 2. Kampfes in der Mod ist M1 = Fan-Modell. Das 80%-Material zeigt am Bahnsteig nur die Riesenform. Wo die Videos nach 307 einen Birkin in Menschengestalt zeigen (50A, 50E), sieht er wie D0 aus.

## 9. Reproduktion

`birkin_videos/werkzeuge/`, aus diesem Ordner starten. Bildpfade zeigen auf `scratchpad/s41video`.
- `farbtest.py [--erod N] [--roh]`: Abschnitt 5.
- `vergleich.py`: Modell in der Spielkamera neben dem Videobild (`vergleich_307_vorn.jpg`, `vergleich_307_hinten.jpg`, `vergleich_50a.jpg`).
- `buehne.py`: Kamera aus RDT, Rasterer. Braucht `birkin_modelle/werkzeuge/lib_fassungen.py`.
- `homo.py`: SIFT-Homographie Hintergrund -> Video (`h*.json`).
- `zoom.py`: Lupen.
- `tims.py`: Disc-TIMs.

## Offen

- 50E: Modell im Gruenlicht nicht bestimmbar. Naechster Weg: Farbmessung wie Abschnitt 5 mit dem Licht aus Mod-50E0. Das ist aber Fan-LIT, also kein Beleg fuer 80%.
- 50A Beinlaenge: Bei 320x240 kreuzt die Stange auf Kniehoehe, Jeansende nicht ablesbar.
- PIX-Dateien der Disc nicht angesehen.

## Aenderungsliste (Gegenprobe 2026-11-01, `gegenprobe.md`)

- 0 Punkt 6 / 8: "M1 frei erfunden" und "Bytes aus RE2 G2/G3" berichtigt: Gestalt ohne Vorlage, Daten aus D0 + RE2-G2 + neu; G3 nicht belegt.
- 2: "Meshes 3/6 identisch mit G2" -> <= 1 Einheit, 6/16 exakt; "Beine +-1" -> <= 2; G3-Kacheln = Grundrauschen.
- 3, B 3:11: Begruendung "M1 hat keinen Kopf mittig" falsch (M1 hat das D0-Kopfmesh 9 mittig); Urteil D0-Gestalt bleibt wegen Menschengesicht.
- 6 und 7: Disc-STAGE5.BIN enthaelt das 0x30-Modul (Stufe-5-Birkin vorbereitet, Modell waere D0/D1).
- Bestaetigt (eigene Standbilder): CD3 0:43-0:52, B 3:11, B 13:50-13:56, CD4 0:20-0:27 (offen), CD4 0:51-0:57, A #141-#145. Video-"D0" heisst D0-Gestalt; Bytegleichheit ist aus Video nicht pruefbar.
- Mod-Kamera fuer 50A in `buehne.py` unschaedlich: Kamerageometrie ROOM50A0 Disc = Mod (nur letztes Wort je Kamera +40).
