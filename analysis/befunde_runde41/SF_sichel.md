# Spur SF "Sichel-Form" (Runde 41)

Arbeitsbaum `.claude/worktrees/r41_sichel`, Zweig `r41/sichel`, Basis master 789f6e681.
Auftrag: Hangar-Birkin = Labor-Birkin (NUTZER_ENTSCHEIDE E17). Neues Asset EM30_SICHEL.EMD.

⛔ KEIN BYTE-TRUE-ASSET. Die Sichel-Form existiert auf der ausgelieferten Disc nicht. Alles hier ist
Nachbildung nach dem 80%-Video (Complete Disc Video 4) und traegt keine @0x-Adresse.

## Protokoll (fortlaufend)

- 2026-11-02 Start. Dossier angelegt.
- Nachtrag Nutzer (ueber Orchestrator, Vorrang): "Der Hangar Birkin der ist als Grundlage da, den musst du nur
  noch mal gegen die 80% Version im Video genaustens analysieren und im Zweifel korrigieren." -> KEIN Neubau.
  EM30_HANGAR (birkin_formen.py FORMEN 'hangar') ist die Grundlage; Teil fuer Teil gegen das Video, nur
  Abweichungen korrigieren. Labor-Kampf bekommt dasselbe Modell.
- Videobilder: alle 1538 Einzelbilder aus "Complete Disc Video 4 (480p Upscale)" (29,97 fps) nach SF_belege/roh/alle
  (nicht versioniert). Bild n liegt bei t = (n-1)/29,97 s. Quelle ist 320x240 hochskaliert (All Footage und
  documentation/...360p sind dieselben Szenen in 320x240, kein Zusatzinhalt).
- Nachtrag Nutzer (Orchestrator): "Der Schwanz ist viel zu starr ..." -> Schwanz als eigene Knochenkette (16-22) an
  Knochen 8, Keyframes aller 21 Clips erweitert (Nachschwing-Rechnung), Port-Lader pruefen. Szene = Spur H.
- Zwischenstand (fortlaufend): Kamera-Registrierung, Seitenbestimmung, Teilvergleich und Generator fertig (Abschnitte
  unten); Build/Tests/Fensterbilder laufen.

## 1. Messgrundlage

- Hangar-Kamera: ROOM50A0 Schnitt 12 (RID pos (-13140,-3258,-26604), Ziel (-2808,-1440,-11556), H 231).
  Homographie Port-Bild -> Video aus ECC gegen den Engine-dekodierten Hintergrund (BSS 50A Schnitt 12):
  ECC 0,988, M ~ 2,00 / 2,07 Skalierung, Versatz ~2 px (SF_belege/kamera_50A0_12.json). Das 480p-Video zeigt das
  Spielbild 640x400 zwischen schwarzen Balken (y 50-450).
- Farbe: lineare Abbildung Spielbild -> Video an den Hintergrundpixeln, mittlerer Fehler 4 je Kanal, nahe der
  Identitaet (SF_belege/farbe_50A0_12.json). Videofarben im Hangar sind also fast die Spielfarben.
- Labor: Gradienten-Korrelation aller BSS-Bilder 50E/50F gegen die Videobilder: Pult (Bild 418) = ROOM50E0 Schnitt 8
  (0,602, alle anderen < 0,07), Steg (Bild 520) = ROOM50E0 Schnitt 5 (0,646). Der 80%-Labor ist also der Raum 50E0
  der Mod. Homographien SF_belege/kamera_50E0_8.json (ECC 0,92) und _5.json (ECC 0,85; Zoom 2,1-2,5 statt 2,0 -
  Kamera der 80%-Fassung weicht dort leicht ab, Vergleiche im Labor nur grob).
- Werkzeug: re15_port/tools/birkin_buehne.py (Zeichenweg wie form_buehne.py der Bahnsteig-Spur, fuer EMD mit
  eingebetteter TIM): vergleich / rundum / streifen; boden_aus_video() rechnet einen Fusspunkt im Video auf den
  Boden zurueck.

## 2. Welcher Arm traegt die Sichel (Video entscheidet)

Ergebnis: Sichel = RECHTER Arm des Birkin (Koerperseite, Knochen 13-15), Klaue = linker Arm (10-12). Das deckt sich
mit dem Wortlaut des Nutzers in Koerperseiten und mit dem Capcom-D0 (Mesh 15 = lange, gekruemmte rote Klinge).
Belege (Bildnummern im 480p-Video, Bild n bei (n-1)/29,97 s; Blaetter SF_belege/seite_*.png):
1. Blickrichtung: 1440-1500 kommt er auf die Kamera zu (wird groesser, Fuesse/Knie nach vorn) = Vorderansicht. Das
   helle Muster am Hosenbund (Fetzen ueber dem Guertel) ist in 1395-1418 dasselbe -> auch dort Vorderansicht.
   Vorderansicht: Bild-links = Koerper-rechts.
2. 1367-1376 (Vorderansicht, Spider-Man springt von links an): der Bild-LINKE Arm ist ab dem Ellbogen eine lange
   rot-weiss gebaenderte Klinge bis zum Boden (rote Pixel 1370 x 289-319 y 270-330, 1373 x 221-317 y 294-375,
   Rot im Median (138,85,87), Weissanteile (185,145,141)) und schwingt dann zur Mitte (1376-1383).
3. 1382-1418: derselbe Bild-linke Arm hebt sich zum Spider-Man und haelt ihn (Szene des Ports: Griff mit Knochen 15
   = rechts). Mit EM30_HANGAR Clip 0x0B Bild 20 vorn zur Kamera gerendert liegt der erhobene Arm ebenfalls links.
4. 1452-1500 (Gang zur Kamera): Bild-links haengt der lange Bogen bis zum Boden (im Schatten dunkel), Bild-rechts
   eine kurze Hand mit roten Krallenfingern auf Hueft-/Kniehoehe. Gerendert (Clip 1 Bild 10, vorn) liegen die
   lange Klinge (Mesh 15, rechts) und die Klauenhand (Mesh 12, links) genau so.
Das rot-weisse Glied Bild-rechts in 1404-1420 ist NICHT die Sichel (die haelt dann oben den Spider-Man), sondern der
Schwanz (Teil T7).

## 3. Teil fuer Teil: EM30_HANGAR gegen das Video

| Teil | Befund | Beleg | Folge |
|---|---|---|---|
| T1 Rohr | weicht ab: im Hangar zeichnete Teil 15 das Rohr (grid 0x10 -> birkin_rohr, enemy_ai_common.c 12140), das Video zeigt nie ein Rohr | alle Hangarbilder 1352-1505 | korrigiert: Rohr-Tausch nur bei 16 Knochen (main.c), Mesh 16 ist jetzt Schwanzglied |
| T2 Sichel (Form, Laenge) | stimmt: rot gebaenderter Teil im Video 142 px bei ~265 px Koerperhoehe = 0,54; Unterarm+Klinge des Modells (839+1405 von ~3400) = 0,66, Klinge allein 0,41 | 1370/1373 | Form unveraendert |
| T5 Sichel-Unterarm | weicht ab: im Video ist der Arm ab dem Ellbogen rot-weiss, im Modell Haut mit violetten Adern | 1367-1376 | korrigiert: Mesh 14 auf die Klauen-Baender (TPage 130, CLUT 2) |
| T2b Klaue links | stimmt: kurze Hand mit roten Krallen | 1476-1500 Bild-rechts | unveraendert |
| T3 Hose | weicht ab: kurze Jeans bis ~15 px ueber dem Knie (1470: Bund ~290, Knie ~345, Hosenende ~330), Unterschenkel nackt rosa-grau; Modell: lange Jeans bis Schienbeinmitte mit Umschlag | 1355, 1460-1476 | korrigiert: Schienbein-Jeans und Oberschenkel unterhalb y 520 auf Beinhaut |
| T4 Fuesse | weicht ab: rote Fuesse in allen Bildern; Modell schwarze Schuhe | 1355, 1412, 1470 | korrigiert: Wundfleisch der Schulter |
| T6 Kopf | weicht ab: kein blondes Menschengesicht, oben Mitte rotes Fleisch | 1460-1500 vorn | korrigiert: Kopf-Mesh 9 auf Wundfleisch |
| T7 Schwanz Lage | stimmt: alte Mittellinie an Knochen 8 liegt in Clip 0x0B Bild 20 dort, wo 1408-1420 das rot-weisse Glied bis zum Boden zeigt; von vorn haengt er zwischen den Beinen (1484-1500). Ein Versuch, ihn ans Becken zu setzen, traf 1412 nicht mehr und ist verworfen | 1408-1420, 1484-1500 | Lage unveraendert |
| T7b Schwanz Bewegung | weicht ab (Nutzerurteil "viel zu starr"; Video: peitscht zwischen die Beine und seitlich bis zum Boden) | 1374-1383, 1404-1420 | korrigiert: Knochenkette 16-22 + Nachschwingen |
| T8 Haut/Farben Rumpf | stimmt im Mass: Rotanteil (r > 1,5 g) Oberkoerper Video 0,08-0,09 (1460/1468/1476) gegen Render 0,09; Video dunkler (Figur nahe der Kamera im Schatten), kein Texturfehler | 1460-1476 | unveraendert |
| T9 Rumpf/Schultern/Masse | stimmt im Rahmen: Hoehe im Video ~255-280 px, Modell 3400 Einheiten bei Abstand ~6900 -> ~236 px; massige Schultern, tiefer Kopf wie im Bild | 1355, 1470 | unveraendert |
| T10 Labor | dieselbe Figur (Nutzer E17); Bild gruen und dunkel, nur Umriss pruefbar | 418-465, 470-600 | Labor 50E0/50F1 laedt EM30_HANGAR (birkin_formen.c re15_bf_labor_aktiv) |

## 4. Gebaut (alles in tools/birkin_formen.py FORMEN['hangar'], Datei bleibt EM30_HANGAR.EMD)

Name bleibt EM30_HANGAR.EMD: die Grundlage ist dieselbe Form (Nutzer: "Der Hangar Birkin der ist als Grundlage da"),
geaendert sind Teile; ein zweiter Name haette Lader, Tests und Paket doppelt gebraucht.

- Schwanzkette (Nutzerurteil "viel zu starr"): 7 Knochen 16-22, Knochen 16 an Knochen 8 (Brust), jeder weitere am
  Vorglied; Ruhegestalt = die bisherige Mittellinie (gleichlange Abschnitte, je 424 Einheiten), Radien 300 -> 34.
  Je Glied ein Rohr-Mesh (8 Seiten, Ringe bei 0, L/2, L; Manschette 0,45 Radius in das Vorglied, damit die Gelenke
  beim Biegen keine Luecke zeigen; Textur laeuft ueber alle Glieder durch; Spitze am letzten Glied). Mesh 16 (Rohr-
  Hand ohne Knochen) entfaellt, Mesh i == Knochen i (emd_common.c bone_mesh_index).
- EMR-Bank 0 neu geschrieben: 23 Knochen, Keyframe 116 statt 84 Byte; die ersten 84 Byte jedes Keyframes sind die
  EM30-Bytes (Wurzel, Spur, 16 x 36 Bit), dahinter 7 x 36 Bit Schwanzwinkel. EDD, Clips, Bank 1/2 (Opfer) unveraendert.
- Schwanzwinkel je Keyframe (schwanz_bewegung): "Wackelknochen"-Rechnung je Clip im Winkelraum. Dreht sich das
  Vorglied, behaelt ein Glied anteilig seine Weltlage (Nachlauf 0,55 Wurzel -> 0,92 Spitze), Feder zur Ruhelage
  (0,22 -> 0,08), Daempfung 0,80 je Bild, Schwere, Grenzwinkel 55 Grad, Boden (Glied-Ende nie unter y = -0,5 Radius,
  schleift). Kreisende Clips laufen dreimal (Einschwingen), sonst Start in Ruhe. Weil jedes Glied am tatsaechlichen
  Vorglied haengt, laeuft der Verzug zur Spitze auf (Peitsche). Gemessen (Spitze relativ zur Brust, Spanne x/y/z):
  Clip 0 Ruhe 359/139/3, Clip 1 Gang 429/180/312, Clip 3 2001/564/1697... Clip 11 3262/1928/3263; Spitze nie unter
  dem Boden (min. -18). Blaetter: schwanz_gang_clip1.png, schwanz_griff_clip0B.png. Alle Zahlen sind GESTALTUNG.
  Verworfen: Punkt-Verlet mit Ziel am starren Ruhebild (fast starr) und mit Ziel am Vorglied (instabil, Schwanz
  schlug um den Koerper, Spanne 3000 schon in Ruhe).
- Umtexturieren (nur UV/CLUT/TPage, vorhandene Texel, keine neuen Farben): T3 Schienbein-Jeans (Mesh 2/5, TPage 129)
  und Oberschenkel ab y 520 (Mesh 1/4) -> Beinhaut (TPage 128 CLUT 0 u 19-55 v 55-115, der Bereich, den der untere
  Schienbeinteil schon nutzt); T4 Fuesse (Mesh 3/6) und T6 Kopf (Mesh 9) -> Wundfleisch der Schulter (TPage 128
  CLUT 0 u 66-126 v 4-90); T5 rechter Unterarm (Mesh 14) -> Klauen-Baender (TPage 130 CLUT 2 u 258-298 v 92-166).
- Port: main.c bk_rohr nur noch bei genau 16 Knochen (Capcom-EM30); birkin_formen.c re15_bf_labor_aktiv
  (50E0/50E1/50F0/50F1) -> re15_bf_emd_datei liefert EM30_HANGAR auch im Labor. Kein neuer Env-Schalter.
- Werkzeuge: tools/birkin_buehne.py (Buehne, Rundum, Streifen, Ruecklage Video -> Boden), tools/sichel_spiel.sh
  (echtes Fenster, eigene exe-Kopie, nur eigene PID; Muster form_spiel.sh), tools/form_fensterbild.ps1 (Kopie aus
  r41/form, identisch). Rohbilder: ffmpeg -i "<Video 4 480p>" -vsync 0 SF_belege/roh/alle/f%04d.png (nicht versioniert).

## 5. Pruefung

- Vergleichsblaetter (Video | alt | neu, Kamera ROOM50A0 Schnitt 12, Abbildung registriert):
  SF_belege/vergleich_hangar_griff.png (Griff/Wurf, Clip 0x0B, Szenenlage), SF_belege/vergleich_hangar_gehen.png
  (Gang zur Kamera, Clip 1, Fusspunkt aus dem Video), SF_belege/vergleich_alt_neu_1355_1470_1412.png;
  Labor: SF_belege/vergleich_labor_steg.png (ROOM50E0 Schnitt 5). Rundum hell: SF_belege/rundum_hell.png.
  Grenze: die Szene des Ports steht anders als im Video (Blickrichtung, Lage) - das ist Spur H; die Blaetter
  vergleichen Teile, nicht die Szene.
- Echtes Fenster (gdigrab, kein AUTOSHOT, kein SOFTWARE_RENDER, Leon): SF_belege/spiel/hangar_cut4_1.png und
  hangar_cut4_uebersicht.png (50A0, RE15_FORCE_CUT=4): Birkin von hinten mit Schwanzkette am Boden, roter Sichel-
  Unterarm; enemy_dbg.log "ZEICHNE Typ 0x30: 23 Teile (Bones 23, Meshes 23) grid 0x10 rohr 0".
  50E0: Modell geladen ("EM30: Ersatzmodell EMD/EM30_HANGAR.EMD (Raum 50E0)", "23 meshes, 23 bones, 21 clips"),
  der Birkin wartet aber ausserhalb des Raums (z 12200, st 1 ss1 9) bis zum Ausloeser am Pult - kein Fensterbild.
- Tests (PowerShell, msys64 vorn): unit_birkin_formen (Datei, Knochen 0-15/EDD/Keyframe-Anfaenge = EM30, Kette 16-22,
  912 Keyframes mit bewegtem Schwanz, 8 Teile umtexturiert), unit_birkin_szenen (Hangar-Szene laeuft mit Knochen 15,
  Labor 50E0/50F1 waehlt die Form, Bank 23/23), integration_r41_sichel_form (neu: 50A0 + 50E0 geladen, 23 Teile
  gezeichnet), dazu alle Birkin-/G5-/Bahnsteig-Tests inkl. integration_r41_birkin_pistole: gruen.
- Paket-Gate: release/make_package.sh prueft EMD/EM30_HANGAR.EMD (vorhanden, nicht leer, bytegleich).

## 6. Offen

- FUER SPUR H (Befund, nicht geaendert): Schnitt 12 und 13 von ROOM50A0 haben als Ankerviereck (RVD @0x564 ff.)
  x -25000..-21000 / z -25000..-21000; der Port cullt jeden Aktor ausserhalb (main.c Region-Quad, FUN_80014368). Der
  Birkin der Szene (-8475,-21675) und Spider-Man liegen ausserhalb -> mit RE15_FORCE_CUT=12 war im echten Fenster
  KEIN Aktor zu sehen (SF_belege/spiel, Laufprotokolle build/sichel_spiel). Die Szene braucht dort einen anderen
  Schnitt oder ein Viereck.
- Sichel-Farbe: Video rot mit weissen Baendern (Median (138,85,87) / (185,145,141)), Modell dunkelrot gestreift;
  Licht als Ursache nicht ausgeschlossen, Klinge selbst unveraendert.
- Schwanzfarbe: Video zeigt das Glied rot-weiss gebaendert (1408-1420), das Modell Fleisch mit Adern wie bisher.
- Labor: Video gruen und dunkel, nur Umriss und Groesse vergleichbar; Labor-Kamera der 80%-Fassung weicht etwas ab
  (Zoom 2,1-2,5 statt 2,0).
- Der Schwanz kann sich nur in den Keyframes bewegen (kein Laufzeit-Nachschwingen); bei Clip-Wechseln springt er wie
  der Koerper (Ueberblendung des Ports gilt fuer alle Knochen).

## Nachbesserung 1 (Auftrag 2026-11-02, fortlaufend)

Auftrag: Modell gegen Video 4 nachbessern (Haut, Hose, Sichel, Schwanz), r41/hangar mergen, Widerspruch "welche Hand
haelt den Spider-Man" klaeren, Schwanz und Ansprung im echten Fenster pruefen, Labor/Tests/Paket-Gate.
Bildnummern: n = Datei-Bild von Video 4 (480p, f%04d.png, n bei (n-1)/29,97 s), h = n - 1350 (Schnitt in den Hangar bei
n1350 = 45,01 s; Helligkeitssprung 54,8 -> 77,0 zwischen n1349 und n1350). h ist dieselbe Zaehlung wie in H_hangar.md.

### N1.1 Merge r41/hangar (Commit 7f2ea1b7d)

Keine textuellen Konflikte; die vier gemeinsamen Dateien (birkin_formen.c, re15_birkin_formen.h, main.c,
test_birkin_szenen.c) haben getrennte Hunks, alle gelesen und beide Seiten erhalten (Beleg im Merge-Text). Bau
LOCAL-BUILD-OK, ctest -R "birkin|hangar|50a|spiderman|formen|r38_birkin|room50|sichel" 20/20 gruen.

### N1.2 Welche Hand haelt den Spider-Man (Video entscheidet)

Ergebnis: die SICHEL-Hand (rechter Arm, Knochen 13-15) haelt ihn, und zwar ab h16 durchgehend bis zum Wurf (h70).
Das rot-weiss gebaenderte Glied, das h18-h28 Bild-links bis zum Boden haengt, ist NICHT die Sichel, sondern der Schwanz.
Damit irrten SF Abschnitt 2 Punkt 2 (las 1367-1376 als Sichelarm) und H 6.4 ("Sichel unten links") in derselben Sache;
die Seitenzuordnung Sichel = rechts (Abschnitt 2 Punkte 1, 3, 4) bleibt.
Belege (SF_belege/nb1/hand_h12-h58.jpg, Messung der roten Flecken unterhalb der Huefte je 2 Bilder, Schwelle
r > 80, r > 1,35 g, r > 1,3 b, Bild 640x480, Wandschild x151-169 ausgenommen):
1. Das rote Glied wandert in EINEM Zug von Bild-links durch die Mitte nach Bild-rechts: h18-h24 x 270-331 y 258-349
   (Spitze am Boden h22/h24 x 223-250 y 342-399), h26-h28 x 230-263 y 300-329, h30-h34 x 327-374 y 338-369 (zwischen den
   Knien), h38-h48 x 402-430 y 289-326 (Bild-rechts auf Hufthoehe), h60-h70 x 425-533 y 320-413 (Bild-rechts bis zum Boden).
   Ein Arm der Bild-linken Schulter kann nicht zwischen die Knie und weiter auf die Bild-rechte Seite; ein Schwanz von der
   Brust-Rueckseite (Kette an Knochen 8) kann das - und genau dort zeigt ihn SF T7 (1408-1420 = h58-h70).
2. Das Glied setzt h20/h24 NICHT an der Schulter an: die helle Bild-linke Schulter liegt bei y ~190-240, das Rot beginnt
   erst bei y ~258 seitlich am Rumpf (SF_belege/nb1/v_schulter.jpg: Ausschnitt x200-440 y100-340, h16/20/24/28/32/40).
3. Ueber der Bild-linken Schulter steht ab h16 ein dunkler (verschatteter) Oberarm schraeg nach oben zum Spider-Man, in
   h16, h20, h24, h28, h32 und h40 gleich (v_schulter.png); h20 und h28 ragt oben eine helle Spitze hinter dem Spider-Man
   heraus (h20 x~355 y~100, h28 x~360 y~120) - die Sichelspitze.
4. Der Spider-Man steht von h16 bis h70 an fast derselben Stelle oben links (Mitte Bild ~(110-136, 42-48) PSX, gelbe
   Bildpunkte h40-h65); es gibt keinen Wechsel des Halte-Mechanismus bei h29-h31.
5. Die Klaue (linker Arm) haengt Bild-rechts: h36 (n1386) rote Klaue auf Hufthoehe (Fleck x 402-413 y 296-315 ab h38).
Folge fuer die Szene: Knochen 15 (Sichelhand) ist richtig; der seitliche Versatz BF_HALTE_SEITE war nur noetig, weil die
Hand allein nicht weit genug links lag. Umsetzung N1.3.

### N1.3 Haltepunkt ohne seitlichen Versatz (birkin_formen.c, PORT-WAHL nach Videomessung)

- Vorher (Spur H): Spider-Man-Aktor = Knochen 15 + 1200 nach unten + 1000 seitlich (BF_HALTE_DY/BF_HALTE_SEITE); er hing
  neben der Hand. Die Hand allein (Becken auf der Hand) haette ihn 30-40 Bildpunkte zu tief gesetzt: EM2B-Koerper reicht
  von y -2960 bis 78, Mitte = Becken (-1795); die Hand liegt in B10-B36 bei Bild-y 72-81, das Video zeigt den Spider-Man
  oben am Bildrand (Bild-y 21 = Letterbox, Rumpf ~40-65).
- Jetzt: Griffknochen EM2B 20 (Knie der Beinkette 19-20-21; Clip 7 rel (859,-1045,-369)) liegt jeden Takt GENAU auf
  Birkins Knochen 15 (bf_klauenziel: Hand - (Knie - Wurzel)). Der Rumpf steht damit ueber der Hand und von Birkin weg.
  Kein Versatz mehr; BF_HALTE_DY/BF_HALTE_SEITE entfallen.
- Pendel nur noch B10..28 (k_birkin_bild {17,10},{35,28},{51,12},{66,27}): Hand-Bild-x je Bild (Gier Bezug-1270,
  tools/r41_sichel/hand.py) B10-24 131-136, B28 143, B34 154, B37 158; Video Spider-Man x 113 (h40) -> 136 (h60).
- Messung echtes Fenster (Lauf n1/n3 gegen vorher, Spider-Man-Kasten per Hintergrund-Differenz, tools/r41_sichel):
  vorher h40 Kasten y 62-129, nachher oben am Bildrand wie das Video (Blatt SF_belege/nb1/halten_video_vorher_nachher.jpg).

### N1.4 Ansprung (birkin_formen.c bf_anlauf/bf_bogen, Gestaltung nach Videomessung)

Video (Spider-Man-Mitte je Bild, Hintergrund-Differenz gegen h112, PSX): x 28 28 28 35 43 59 74 92 111 (h0-h8),
y 141 139 131 110 110 115 118 117 114: hebt h2->h3 in EINEM Bild ab, fliegt dann fast eben mit 16-19 Bildpunkten je Bild.
Vorher (smoothstep): x 36 35 33 35 41 47 57 70 83 99 105 - langsamer Start, Abbremsen vor dem Aufprall, Rueckzug h10->h11
(105 -> 95) und zwei gleiche Bilder. Nachher (Anlauf 30 % quadratisch, dann gleichmaessig; Bogen 27/4 u (1-u)^2, Scheitel
bei 1/3, BF_BOGEN 1000): x 36 35 33 36 42 53 65 86 100 114 (n3), y 161 161 144 131 119 112 109 105 104 101.
x liegt h3-h7 auf 1-9 Bildpunkte am Video; der Port-Spider-Man hockt h0 20 Bildpunkte tiefer (EM2B-Hockpose).
Groesster Schritt im Unit-Riegel 678 (Grenze 700, Video h4..8 ~600 je Bild).
Bildfolge Video | vorher | nachher h1-h12: SF_belege/nb1/ansprung_h1-h12_video_vorher_nachher.jpg (echtes Fenster, gdigrab
30 Bilder/s, p0 per tools/r41_sichel/p0.py; einzelne doppelte Aufnahmebilder sind gdigrab, nicht das Spiel).

### N1.5 Schwanz rot-weiss gebaendert (birkin_formen.py schwanz_kette uv, CLUT-Zeile 483)

Video (2-Mittel ueber die Schwanz-Bildpunkte h20-h24 und h64-h68, rote Flecken + Nachbarschaft): dunkel (92,54,55) 63 %,
hell (137,90,91), p90 hell (163,112,113). SFs alter Wert (138,85,87)/(185,145,141) stammt aus denselben Bildern
(1370/1373 = h20/h23), war aber der Sichel zugeschrieben.
Modell vorher: Fleischtextur mit Adern (TPage 128 u 4-60 v 4-124). Nachher: Bandstreifen TPage 130 u 0-42 v 92-130
(senkrechte hellere Streifen, Spalten-L 41..63, drei Gipfel u 6/24/35), Laenge auf u -> drei helle Ringe, eigene
CLUT-Zeile 483 (band_zeile). Abgleich im echten Fenster (Schwanz h20-h26 Bild-links, Differenz gegen einen Lauf ohne
Feder-Kette, 2-Mittel):
| Stand | rot | Anteil rot | hell |
|---|---|---|---|
| Video 4 (h20-h24, h64-h68) | (92,54,55) | 63 % | (137,90,91) |
| n5: Schwellen 44/52, (100,52,54)/(180,128,128) | (99,66,66) | 41 % | (176,128,128) |
| n6: 48/56, (96,44,46)/(140,100,100) | (88,50,50) | 42 % | (142,96,96) |
| n7 (Endstand): 52/58, (96,44,46)/(140,100,100) | (93,52,52) | 52 % | (142,95,95) |
Die Buehne (Licht von vorn) misst den Schwanz dunkler als das Fenster (dort steht seine Flanke im Licht); abgeglichen
wurde am Fenster.

### N1.6 Kurze Hose (birkin_formen.py teilen + umtexturieren)

Video h120-h132 (Blau-/Hautprofil der Beinspalten x 160-215, PSX): Jeans y 136..157, Haut 157..178 -> Hosenende bei 62 %
von Bund bis Knie. Port vorher (n1): Jeans 145..187, Knie (Knochen 2, Projektion) 187 - die T3-Regel y_ab 520 traf nur
das Kniedreieck (Oberschenkel-Mesh 1/4 hat Ringe bei y ~-124, ~76, ~818 + Spitze 951; das Viereck 76..818 blieb ganz Jeans).
Jetzt: teile_quer schneidet die Vierecke 76..818 bei y 360 (Ecken auf den alten Kanten, Form gleich), der untere Teil
wird Beinhaut. Nachher (n3, h124): Jeans enden bei Bild-y ~171 (Soll 145 + 0,62 x 42 = 171).

### N1.7 Sichel dunkel statt rot-weiss (CLUT-Zeile 484) - Abweichung vom Auftragstext

Der Auftrag nannte fuer die Sichel "rot mit weissen Baendern (138,85,87)/(185,145,141)". Nach N1.2 ist das der Schwanz.
Wo das Video die Sichel zeigt (Gang zur Kamera h92-h146, Bild-links langer Bogen bis zum Boden,
SF_belege/nb1/v_gang.jpg), ist sie DUNKEL: Hintergrund-Differenz gegen h0 im Streifen x 255-300: h128 (23,23,19),
h132 (21,23,18), L 25 gegen Haut-L 42-45 in denselben Bildern = 0,58; nur am Ellbogen rotgrau (55,43,46) (h96-h100).
Keine Farbe erfunden: Mesh 15 bekommt eine dunkle, fast unbunte Zeile 484 (band_zeile (40,40,35) / (84,81,72)); der
Gruenstich der Videowerte ist Kompression (Torbogen daneben (14,16,16)). Lauf n2 mit olivem Ton (38,46,32)/(76,90,64)
wirkte im Fenster gruen -> verworfen. Buehne: Sichel (30,30,27), 0,56 der Hauthelligkeit (Video 0,58).
Der Unterarm (Mesh 14) traegt wieder die EM30-Haut (T5 zurueckgenommen).

### N1.8 Haut dunkler und roter gefleckt (CLUT-Zeilen 480-482)

Mass (Kasten PSX x150-240 y95-150, Haut = r > g+8 und r > b+4, L = Luma, rot = r > 1,5 g; Video h40/46/52/58/64):
| | L | Rotanteil | hell (Median) | rot (Median) |
|---|---|---|---|---|
| Video 4 | 66,0 | 0,328 | (78,62,61) | (72,40,40) |
| Port vorher (n1, echtes Fenster) | 92,9 | 0,174 | (116,92,89) | (73,34,34) |
| Port nachher (n3, echtes Fenster) | 63,6 | 0,342 | (83,65,63) | (70,35,36) |
haut_zeile je Eintrag: q = r/g; blass x0,63; ab q 1,2 bis 1,6 linear zur roten Fassung (r x0,95, g/b x0,95 x0,82); Eintraege
mit b > r (Jeans) unveraendert (Unit-Test prueft: 0 blaue Eintraege geaendert, 628 Haut-Eintraege). Parameter per
Gittersuche an der Buehne (deren Mass trifft das Fenster: vorher Buehne 93,9/0,158/(115,91,87) gegen Fenster 92,9/0,174/
(116,92,89)). Grenze: im Gang zur Kamera wird das Video noch dunkler (L 42), der Port nicht (Licht, nicht Textur).

Belege: SF_belege/nb1/modell_video_vorher_nachher.jpg (Video | vorher | nachher, h0/h22/h64/h100/h124, echtes Fenster).

### N1.9 Schwanz mit eigenem Schwung (engine/src/schwanz_r41.c, PORT-WAHL)

Befund vorher (echtes Fenster, Lauf "vorher" = 7f2ea1b7d): der Schwanz haengt in der ganzen Halte-Phase h16-h70
Bild-rechts von der Huefte zum Boden und bewegt sich nur mit den Clip-Keyframes (Pendel B10->34->22->37); h18-h28, wo
das Video ihn Bild-links bis zum Boden zeigt und ihn danach durch die Knie nach rechts schwingt, ist er im Port hinter
dem Koerper unsichtbar. Bei Clip-/Bild-/Gierwechseln springt er mit dem Koerper (SF Abschnitt 6).
Bau:
- Feder-Kette im Weltraum, ein Schritt je Spieltakt (g_engine.frame_count): Ziel = Keyframe-Gelenke 16..22 + Spitze;
  Erhalt 0,85 der Geschwindigkeit, Feder 0,34 (Wurzel) .. 0,11 (Spitze), Durchhang 3 je Takt, feste Gliedlaengen
  (Keyframe), Boden y = Aktor-y, Wurzel fest am Rumpf, Sprung > 3000 = neu auf das Ziel. Knochen 16-22 werden danach
  neu gedreht (Keyframe-Lokaldrehung, dann kleinste Drehung auf das simulierte Glied), Knochen 0-15 bleiben. Nur Typ 0x30
  mit genau 23 Knochen und Kette 8-16-..-22 (EM30 mit 16 Knochen unberuehrt). Aufruf im Zeichner direkt nach
  re15_skel_compute_pose (main.c, neben re15_bissarm_r37_handkontakt).
- Endstand der Feder: Erhalt 0,88, Feder 0,22 (Wurzel) .. 0,07 (Spitze), keine Schwerkraft (Ruhe = Keyframe). Erster
  Stand (0,85; 0,34..0,11; Durchhang 3) wechselte die Seite im Unit-Test in 3 Takten und hing in Ruhe 74 neben dem
  Keyframe -> verworfen.
- Die Posenmatrizen des Ports sind nicht normiert (Spaltenlaengen 0,996 an Knochen 15 bis 0,94 an Knochen 22, gemessen):
  die Kette orthonormiert fuer die Drehrechnung und bringt den Massstab wieder an (sonst schrumpften die Glieder um bis
  zu 5 %, erster Testlauf "Glied 21 Laenge 403 statt 424").
- Hangar-Szene (birkin_formen.c k_schwanz_seite): Seitenvorgabe an der Spitze 0 (h11) -> 4000 nach rechts (h16..h27)
  -> 0 (h30) -> -900 (h36, Schwung) -> 0 (h46), zur Spitze quadratisch gewichtet. In Clip 0x0B liegt die Spitze sonst
  links (Modell z +1500..+1850). 3200 erreichte nur z -721 (Kette gestreckt); linear gewichtet (Lauf n4) stand der
  Schwanz h22-h26 waagerecht in Hufthoehe ab, quadratisch (n5) haengt er von der Huefte zum Boden wie im Video.
- Unit-Test unit_schwanz_r41: Ruhe (Spitze < 60 vom Keyframe, Knochen 0-15 unveraendert), 45-Grad-Drehung in einem Takt
  (Nachlauf max 1657, nach 120 Takten 5), Glieder so lang wie im Keyframe (+-3), kein Gelenk unter dem Boden,
  Vorgabe 4000 -> Spitze z < -800, Freigabe -> Mitte nach 1-3, Gegenseite nach 3-4 Takten (schlagartig freigegeben; die
  Szene gibt ueber 3 Bilder frei), EM30 mit 16 Knochen unberuehrt.
- Messung echtes Fenster (Lauf n8, tools/r41_sichel/schwanzbahn.py: Video = groesster roter Fleck unter der Huefte,
  Port = Schwanzspitze aus der Messzeile, durch die Cut-12-Kamera projiziert; PSX-Bildpunkte):
| h | Video Fleck x | Port Spitze x | |
|---|---|---|---|
| 18-24 | 134-164 | 155 -> 110 | Bild-links bis zum Boden, wie Video |
| 26-28 | 114-130 | 111-115 | Spitze links oben |
| 30 | 162-172 | 166 | Mitte |
| 32-34 | 172-186 | 212-241 | Port schwingt zu weit nach rechts (Vorgabe -900 bei h36 + Federueberschwung) |
| 38-48 | 200-214 | 273 -> 219 | dito, ab h48 wieder am Video |
| 58-70 | 211-238 | 222-238 | Bild-rechts bis zum Boden, wie Video |
  Nachbesserung (Laeufe n9/n10): Vorgabe -900 bei h36 gestrichen (n9: Spitze h38 260), Erhalt 0,88 -> 0,80 (n10).
  n10 (Endstand): h18-h28 148 -> 116 (Video 114-164), h30 162 (162-172), h32-h34 201-223 (172-186), h38-h44 250-239
  (200-214, Rest-Ueberschwung ~40), h48-h70 226-239 (205-238). Unit-Test unveraendert gruen (Rest 4, Mitte nach 1,
  Gegenseite nach 2 Takten bei schlagartiger Freigabe).
  Vorher (7f2ea1b7d) war der Schwanz h18-h28 hinter dem Koerper (nicht sichtbar) und hing ab h34 starr rechts.
  Bildfolge Video | vorher | nachher: SF_belege/nb1/schwanz_h18-h66_video_vorher_nachher.jpg (Lauf n7).

### N1.10 Kopf (Nutzerbefund 2026-11-02 ueber den Orchestrator: "der Kopf von Birkin mit Tail ist noch nicht ganz akkurat")

Vergleichsblatt nur Kopf, mehrere Ansichten (Video 4 | Port vorher n10 | Port nachher k2, Kopf-Mesh-9-Umriss tuerkis):
SF_belege/nb1/kopf_video_vorher_nachher.jpg - h0 (n1350, 3/4 von vorn links), h46 (n1396, Halten), h100/h116/h124/h132
(n1450-n1482, Gang zur Kamera, von vorn). Labor/Steg (n361-n660): Birkin klein, gruen und dunkel (n400/n430/n520 in
SF_belege/nb1 nicht abgelegt, Ausschnitte scratch) - Kopf dort nicht messbar.
Messung (tools/r41_sichel/kopfmass.py; Video: Kopfbereich x 372-402 y 165-215 von 640x480 = rotes Fleisch oben in der
Mitte; Port: Maske = nur Mesh 9 an der Pose der Messzeile offline gezeichnet und auf die Fensteraufnahme gelegt; Kopf/Rumpf
= Luma Kopf / Luma Rumpfkasten im selben Bild):
| h | Video: Median, p90, r/g, kraeftig rot, Kopf/Rumpf | Port vorher (n10) | Port nachher (k2) |
|---|---|---|---|
| 116 | (70,46,45) (122,96,97) 1,51 0,43 1,28 | (90,60,57) (125,102,98) 1,39 0,33 1,19 | (102,59,58) (127,104,100) 1,55 0,45 1,25 |
| 124 | (64,38,38) (104,77,77) 1,62 0,52 1,22 | (94,56,54) (152,89,86) 1,77 0,55 1,17 | (104,57,57) (134,95,94) 1,67 0,58 1,23 |
| 132 | (53,33,33) (87,67,68) 1,44 0,35 1,11 | (92,56,53) (151,84,82) 1,69 0,54 1,11 | (103,56,55) (131,95,94) 1,65 0,54 1,18 |
Befund: vorher ein gesaettigt roter Ball (Lichter p90 r = g + 63..67, Wundfleisch der Schulter, T6); im Video sind die
Lichter rosa-weisslich (p90 r = g + 20..26) bei sonst aehnlichem Rotgrad und Kopf/Rumpf 1,1-1,3.
Korrektur: eigene CLUT-Zeile 485 fuer Mesh 9 (aus Zeile 480, Schwellen 109/140, dunkel (102,51,49), hell (149,115,112);
Gittersuche an der Buehne im Gang, dann x0,85). Erster Versuch (110,62,60)/(200,165,160) 93/126 war im Fenster blassrosa
(Lauf k1: Median (125,92,89), r/g 1,33, Kopf/Rumpf 1,70) -> verworfen. Nachher Lichter p90 r = g + 23..39.
Form, Groesse, Lage, Neigung: NICHT geaendert, weil das Video sie nicht trennbar zeigt. Port-Kopf im Gang 16 x 23
PSX-Bildpunkte (Mesh-9-Maske), Kopfmitte 48 ueber dem Hosenbund; Video-Kopfbereich ~15 x 25, Mitte ~45 ueber dem Bund -
im Rahmen gleich. Die Seitenlage liess sich nicht messen: der Rot-Schwerpunkt der Kopfzeilen trifft im Port nicht den
Kopf (Maske x 177-181), sondern die Brustadern (x 186-192) - dieselbe Methode ist im Video also nicht kopfgenau.
Gesicht/Haare: im Video keins (Fleisch, h108-h132), im Port seit T6 keins. Im 3/4-Bild h0-h6 wirkt die Kopfvorderseite im
Video dunkel unter einer roten Kappe (n1350-n1356); ob Schatten oder Gesicht, ist bei 320x240 nicht entscheidbar ->
keine Formaenderung.
