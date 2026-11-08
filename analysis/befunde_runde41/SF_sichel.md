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

### N1.11 Stand, Tests, offen

Commits: 7f2ea1b7d (Merge r41/hangar), 91e2508c2 (Modell: Haut, Hose, Schwanz-, Sichelfarbe), f4066042c (Szene: Sichelhand,
Ansprung, Feder-Kette), 05ab73e0f (Schwanz-Ueberschwung), f852c1ff9 (Kopf).
Labor 50E0/50F1 laedt dieselbe Datei (re15_bf_labor_aktiv, unveraendert); die Feder-Kette greift dort ebenso (Typ 0x30 mit
23 Knochen). Paket-Gate release/make_package.sh prueft EMD/EM30_HANGAR.EMD (vorhanden, nicht leer, bytegleich) - die
neuen CLUT-Zeilen stecken in der Datei, das Gate deckt sie mit ab.
Tests (PowerShell, msys64 vorn): unit_birkin_formen, unit_birkin_szenen, unit_schwanz_r41 (neu), integration_r41_sichel_form,
integration_r41_birkin_pistole und alle Birkin-/Spider-Man-/G5-Tests: -R "birkin|hangar|50a|spiderman|formen|r38_birkin|
room50|sichel|schwanz". Endstand f852c1ff9: 21/21 gruen (494 s).
Offen:
1. Sichelfarbe weicht vom Auftragstext ab (dunkel statt rot-weiss) - begruendet in N1.7 (rot-weiss ist der Schwanz).
2. Schwanz schwingt nach der Freigabe noch ~40 Bildpunkte weiter nach rechts als das Video (h38-h44); die Seitenvorgabe
   ist Szenen-Gestaltung nur fuer den Hangar, im Kampf schwingt die Kette nur aus der Koerperbewegung.
3. Haltepose: die Hand liegt im Clip 0x0B hoechstens bei Bild-x 131; der Spider-Man steht mit dem Knie-Griff oben links,
   bleibt aber 10-20 Bildpunkte rechts vom Video (Video Mitte x 113-136).
4. Birkin selbst ist im Video in h0-h40 geduckt mit gespreizten Beinen, im Port aufrecht (EM30-Clips) - Spur H.
5. Kopf: nur die Farbe ist belegt korrigiert; Lage/Form im Video nicht trennbar messbar (N1.10).
6. Im Gang zur Kamera wird das Video deutlich dunkler (Haut L 42), der Port nicht (L 60) - Licht, nicht Textur.
7. PSX-Plattform: Feder-Kette nur im PC-Zeichner (main.c) eingehaengt; die zusaetzlichen CLUT-Zeilen 483-485 sind auf der
   PSX nicht geprueft.

## Nachbesserung 2 (Auftrag 2026-11-02, fortlaufend)

Auftrag: Haltung (geduckt statt aufrecht) und Kopf gegen Video 4, Schwanz-Ueberschwingen h38-h44, Tests, Paket-Gate,
Fensterbelege Leon/Elza. Nutzer: "Ich glaube der Kopf von Birkin mit Tail ist noch nicht ganz akkurat im Vergleich zum Video".
Basis 3799d7e94 (Datei EM30_HANGAR.EMD md5 371d431c...). Werkzeuge tools/r41_sichel/nb2/, Belege SF_belege/nb2/.

### N2.1 Messgrundlage Umriss

- Die automatische Trennung Figur/Hintergrund (BSS 50A Schnitt 12 per ECC-Homographie, Differenz, GrabCut) traegt nur fuer den
  hellen Oberkoerper; die Beine im Schatten vor dem dunklen Tor und der grosse Bodenschatten sind davon nicht zu trennen
  (Versuche seg1-seg4: Tor oder Schatten liefen in die Maske, Beine fehlten). Deshalb: Video-Umriss VON HAND nachgezeichnet
  (tools/r41_sichel/nb2/spur.py: Videobild + verschiebungstolerante Hintergrund-Differenz nebeneinander, Lineale), Punkte in
  SF_belege/nb2/umrisse_video.json fuer h0, h40, h64, h100, h116, h124 (n1350/1390/1414/1450/1466/1474). Unsicherheit an den
  Beinen im Schatten ~5 Bildpunkte (640x480). h132 (n1482) liegt schon in der Abblende - nicht nachgezeichnet.
- Port-Umriss: tools/r41_sichel/nb2/portmaske.py (Buehne birkin_buehne, Pose der Messzeile) bzw. schnell.py (nur Umriss,
  fillConvexPoly je Dreieck; IoU gegen die Buehnenmaske 0,91-0,93, Unterschied = Rueckseiten/transparente Texel).
- Mass (messung.py): IoU, Hoehe x Breite, H/B; in den Haltebildern h40/h64 nur y >= 160 (oben haengt der Spider-Man).
- Vorher (EM30_HANGAR 3799d7e94, Posen aus Lauf k2 = Szene wie gespielt):

| h | IoU | IoU verschoben (dx,dy) | Video H x B (H/B) | Port H x B (H/B) |
|---|---|---|---|---|
| 0 | 0,354 | 0,493 (34,14) | 278 x 139 (2,00) | 295 x 223 (1,32) |
| 40 | 0,394 | 0,498 (-40,-12) | 273 x 157 (1,74) | 287 x 255 (1,13) |
| 64 | 0,399 | 0,496 (-40,-14) | 278 x 233 (1,19) | 285 x 249 (1,14) |
| 100 | 0,294 | 0,406 (34,20) | 259 x 140 (1,85) | 301 x 241 (1,25) |
| 116 | 0,389 | 0,420 (40,0) | 283 x 154 (1,84) | 325 x 257 (1,26) |
| 124 | 0,461 | 0,474 (20,-14) | 300 x 191 (1,57) | 328 x 255 (1,29) |

  Befund: der Port ist 10-40 Bildpunkte HOEHER und 20-100 Bildpunkte BREITER als das Video. Die Orchestrator-Lesart
  "niedriger UND breiter" stimmt nur halb: niedriger ja, aber der Video-Umriss ist SCHMALER (H/B 1,6-2,0 gegen 1,1-1,3) -
  Arme/Sichel/Klaue haengen im Video nah am Koerper, im Port stehen sie seitlich ab; nur die Beine stehen im Video breiter.

### N2.2 Haltung (birkin_formen.py ducken(), GESTALTUNG nach Video 4)

Video (von Hand abgelesen, spur.py/lupe.py; 640x480): h0 Fuesse x 345/430 (Abstand 85), Knie ~362/428, Hose y 283-318, oben 163;
h40 Fuesse 308/430 (122), linkes Bein weit nach links gestreckt (Ausfallschritt zum Spider-Man), Hose schraeg 298-333;
h64 Fuesse 291/429 (138); h100 Fuesse 346/431, Knie 360/422, Oberkoerper stark vorgebeugt, Kopf oben vorn; h116/h124 von
vorn: Fuesse 356/433 bzw. 368/437, Knie weit, Schienbeine fast senkrecht, Arme nah am Rumpf. Port vorher: Fuesse 373/414
(Abstand 41), Knie 378/412 gerade, Arme und Sichel stehen seitlich ab.
Bau: ducken() auf ALLE Keyframes der EMR-Bank 0 (21 Clips: Stand, Gang, Angriffe, Griff/Wurf, Treffer; Baenke 1/2 sind
Opfer-Clips des Spielers), gewichtet mit der Rumpf-Aufrichtung (liegend unveraendert): Becken tiefer, Fussgelenke behalten ihre
Lage (+ Spreizung nach aussen) per Zwei-Glied-IK, Knie nach aussen, Bauch/Brust nach vorn, Oberarme halten anteilig ihre
Weltdrehung und ruecken zur Mitte. Schwanz-Ruhelinie um denselben Winkel zurueckgedreht (haengt in der Welt wie vorher).
Werte per Koordinatensuche (anpassen.py: Umriss-IoU nach Verschiebung gegen umrisse_video.json, Lage aus dem Video-Fusspunkt,
Clip-Bild/Gier je Bild frei): becken 110, spreiz 340, knie_aussen 0,2, rumpf 3, brust 20, arm_halten 0,575, arm_innen 20.
Breite (N2.2b): mit Haltung blieb Hoehe/Breite 1,47 gegen Video 1,70 -> Brust quer 1,30 -> 1,15, Arme in der Dicke 0,57 der
alten Verdickung (Gitter: IoU gleich 0,667/0,665, H/B 1,65). Clip-Katalog aller 21 Clips vorher/nachher: keine kaputte Pose
(Liegen/Aufstehen unveraendert). Fussgelenke: hoechstens 342 vom EM30-Ort (Spreizung 340), unit_birkin_formen prueft das.

Messung (gleiche Kamera Schnitt 12; Port-Umriss = Buehne an der gespielten Pose der Messzeile, exakt; vorher Lauf v1 =
exe 3799d7e94 + EMD 3799d7e94, nachher Lauf n2; Endstand-Laeufe v3/n6 messen dasselbe):

| h | IoU vorher | IoU nachher | IoU verschoben vorher -> nachher | H/B Video | H/B vorher | H/B nachher |
|---|---|---|---|---|---|---|
| 0 | 0,354 | 0,699 | 0,493 -> 0,720 | 2,00 | 1,32 | 1,64 |
| 40 | 0,394 | 0,479 | 0,498 -> 0,594 | 1,74 | 1,13 | 1,26 |
| 64 | 0,399 | 0,478 | 0,496 -> 0,590 | 1,19 | 1,14 | 1,28 |
| 100 | 0,294 | 0,558 | 0,406 -> 0,574 | 1,85 | 1,25 | 1,49 |
| 116 | 0,389 | 0,577 | 0,420 -> 0,631 | 1,84 | 1,26 | 1,46 |
| 124 | 0,461 | 0,547 | 0,474 -> 0,674 | 1,57 | 1,29 | 1,44 |
| Mittel | 0,382 | 0,556 | 0,464 -> 0,630 | 1,70 | 1,23 | 1,43 |

Hoehe x Breite (Bildpunkte 640x480) vorher 285-328 x 223-257, nachher 289-318 x 179-231, Video 259-300 x 139-233.
Gegenprobe im echten Fenster (fenstermass.py: Fensterbild gegen den Engine-Hintergrund, Umriss verrauscht durch
hautfarbene Teile vor der hellen Wand): Leon v3 -> n6 IoU 0,410 -> 0,483 (verschoben 0,475 -> 0,545), Elza e_v -> e2
0,400 -> 0,499 (0,460 -> 0,554).
Rest: in den Haltebildern h40/h64 steht Birkin im Video im weiten Ausfallschritt NACH LINKS (Fussmitte 25-30 Bildpunkte links
vom Stand h0); das ist Lage/Schritt der Szene (Spur H), keine Haltung - im Port steht er weiter am Fusspunkt von h0.

### N2.3 Spider-Man-Schrittgrenze (birkin_formen.c BF_SM_SCHRITT 600, PORT-WAHL)

Mit der neuen Haltung wandert die Sichelhand beim Zugreifen (Clip 0x0B B3->B5) 860 statt 750 Einheiten; zusammen mit dem
Griffknochen-Wechsel auf Clip 7 sprang der Spider-Man bei h13 803 Einheiten zurueck (unit_birkin_szenen: Grenze 700).
Jetzt folgt er der Hand bis zum Wurf (h66) mit hoechstens 600 je Takt (Video-Ansprung h4..8 ~600 je Bild, N1.4); groesster
Schritt im Unit-Riegel 600. Haltelage h16-h64 (Mitte der Hintergrund-Differenz oben links, PSX; spiderman.py, Laeufe v3/n6):
Video x 108-143, vorher 129-141 (mittlerer Abstand 10,4), nachher 124-136 (9,7); y Video 53-64, vorher 59-74, nachher 62-75.

### N2.4 Kopf (Nutzer: "der Kopf von Birkin mit Tail ist noch nicht ganz akkurat")

Was das Video zeigt (Lupe 2-4x, kopfblatt.py; h0-h6 3/4 von vorn, h84-h100 Drehung, h104-h140 Gang zur Kamera):
- h0/h3 (n1350/n1353): runde rote Kappe OBEN auf der Masse, x 355-390, y 163-185 (640x480), Median (107,55,58), p90
  (149,82,86), r/g ~1,95; darunter ein dunkles Band, dann helle Schultern. h6 schon schwaecher (68,51,49).
- h100 (n1450, Birkin dreht zur Kamera): rote Kuppe oben vorn, x 398-418, y 176-188, (109,59,64) / p90 (157,97,102).
- h116/h124 (von vorn): helle Kuppe oben Mitte (x 386-400, y 152-172, (74-81,59-68,58-68), p90 (127-140,110-122,110-124)),
  darunter roter senkrechter Streifen bis y ~235 ((65,37,35), r/g 1,85) = gesenktes Gesicht vor der Brust.
- Messgroessen (kopfmass2.py, Port = sichtbare Bildpunkte von Mesh 9, Lage/Gier/Clip-Bild wie die Umriss-Anpassung):

| | Video | Port vorher | Port nachher |
|---|---|---|---|
| Kopf-Oberkante unter Umriss-Oberkante (h0/h100/h116/h124) | 2 / 0 / 3 / 5 | 14 / 19 / 25 / 19 | 6 / 9 / 6 / 8 |
| sichtbare Kopfbreite | 20-30 | 20-23 | 25-29 |
| sichtbare Kopfflaeche (Bildpunkte) | - | 660-860 | 1200-1400 |
| Kopf/Rumpf-Helligkeit (Gang h100-h124, Buehne) | 1,16-1,28 (N1.10) | 1,21-1,23 | 1,18-1,21 |

Begruendung je Groesse:
- LAGE (geaendert): in allen vier Ansichten sitzt die Kopfkuppe an der Oberkante des Umrisses; der Port hatte den Kopf 170
  tief zwischen die Schultern gesetzt (Beta-Screenshots) - mit der geduckten Haltung waere er 31-39 Bildpunkte unter der
  Oberkante verschwunden. Jetzt rel[9] (60, -180, 0) statt (60, 170, 0): 350 hoeher im Brustraum (Gitter ueber vor/hoch,
  kopfmass2.py; nach vorn schieben senkt ihn wegen des vorgebeugten Rumpfs nur weiter ab).
- GROESSE (nicht geaendert): sichtbare Breite im Video 20-30, im Port 20-29 - im Rahmen gleich; ein groesserer Kopf ist im
  Video nicht belegt. Er wirkt jetzt groesser, weil er nicht mehr in der Brust steckt (sichtbare Flaeche ~1,7x).
- FORM (nicht geaendert): bei 320x240 nicht trennbar (Kappe/Kuppe oben, Gesicht als roter Streifen) - keine neue Form erfunden.
- FARBE: Zeile 485 x1,12 (114,57,55)/(167,129,125), weil der hoeher sitzende Kopf an der Buehne dunkler gegen den Rumpf wurde
  (1,07-1,09); jetzt 1,18-1,21 wie in N1.10 abgeglichen.
- Unsicher: ob die rote Kappe in h0 der Kopf oder die Wunde der rechten Schulter ist, ist bei 320x240 nicht entscheidbar; der
  Befund "Kopf an der Oberkante" stuetzt sich deshalb auch auf h100 und die Vorderansichten h116/h124.
Blatt: SF_belege/nb2/kopf_buehne_video_vorher_nachher.jpg (Buehne, Kopf-Mesh tuerkis) und die Fensterblaetter (N2.7).

### N2.5 Schwanz: Ueberschwingen h38-h44 (birkin_formen.c k_schwanz_seite, PORT-WAHL nach Videomessung)

schwanzbahn.py (Video = groesster roter Fleck unter der Huefte, Port = Schwanzspitze der Messzeile, PSX-x):

| h | Video | vorher (v1) | nur Haltung (n2) | Endstand (n5) |
|---|---|---|---|---|
| 18-24 | 134-164 | 148 -> 115 | 135 -> 101 | 153 -> 120 |
| 26-28 | 114-130 | 115-116 | 103-106 | 121-123 |
| 30-34 | 162-186 | 162 / 201 / 223 | 152 / 184 / 215 | 158 / 181 / 204 |
| 36-44 | 184-214 | 241 / 250 / 251 / 247 / 239 | 238 / 248 / 246 / 237 / 226 | 217 / 214 / 203 / 194 / 191 |
| 46-48 | 205-213 | 232 / 226 | 216 / 209 | 195 / 203 |
| 58-70 | 211-238 | 222-239 | 209-233 | 213-230 |

Aenderung: Gegenvorgabe 1700 (zur Bild-linken Seite) h31-h43 ({30,0},{31,0},{34,1700},{39,1700},{43,0}) faengt den
Federschwung nach der Freigabe ab; Vorgabe h16-h27 4000 -> 3300, weil die geduckte Haltung die Spitze 13-14 Bildpunkte weiter
nach links brachte (n2) - damit liegt h18-h28 wieder bei vorher. Ueberschwingen h38-h44 von ~40 auf hoechstens 9 (h38 214 gegen
205), danach h42-h46 bis 14 links vom Fleck. Lauf n3 (1800 ab h37) setzte zu spaet ein (h38 236) und schwang h44 auf 176.

### N2.6 Labor 50E0/50F1

Dieselbe Datei (re15_bf_labor_aktiv unveraendert), also dieselbe Haltung in allen Kampf-Clips. Video: am Pult (n440-n465)
fuellt Birkin das Bild gruen und dunkel - massiger, nach vorn gebeugter Oberkoerper, nur qualitativ; auf dem Steg (n480-n590)
~45 x 90 Bildpunkte, Beine als zwei helle Striche ab n540 auseinander, Arme nah am Rumpf. Blatt (Buehne, ROOM50E0 Schnitt 5,
Clip 1 Bild 10, Lage = Rueckprojektion der Sohlen +-10 Bildpunkte, Kamera der 80%-Fassung weicht ab, N1 Abschnitt 1):
SF_belege/nb2/labor_steg_buehne_video_vorher_nachher.jpg - nachher schmaler, Arme innen, Beine gespreizt wie im Video;
Groesse/Lage nur grob vergleichbar. Echtes Fenster im Labor weiter nicht moeglich (Birkin wartet ausserhalb bis zum Pult).

### N2.7 Echtes Fenster (Skill re15-port-visual-verify: eigene exe-Kopie, gdigrab hwnd der eigenen PID, kein AUTOSHOT /
SOFTWARE_RENDER), Blaetter Video | vorher | nachher

vorher = exe aus 3799d7e94 (git archive, eigener Bau im Scratch) + EMD 3799d7e94 (RE15_CD_ROOT auf eine Kopie), nachher =
d287fce53-Stand. Laeufe Leon v3/n6 (50A0, 34 s inkl. Kampf), Elza e_v/e2 (50A1, RE15_TITLE_SHOT_CHAR=1).
- ganzkoerper_leon_video_vorher_nachher.jpg, ganzkoerper_elza_video_vorher_nachher.jpg (h0/22/40/64/100/124)
- szene_h0-h132_leon_video_vorher_nachher.jpg (jedes 6. Bild), kopf_leon_video_vorher_nachher.jpg (h0/46/100-132)
- schwanz_h18-h66_leon_video_vorher_nachher.jpg, kampf_leon_vorher_nachher.jpg (KI-Kampf nach der Szene, gleiche Stelle
  vor dem Schnitt 4->3: Gang/Angriff geduckt, keine kaputte Pose)
- Buehne/Mass: umrisse_video_vorher_nachher.jpg (gelb Video von Hand, rot vorher, tuerkis nachher),
  kopf_buehne_video_vorher_nachher.jpg, ganzkoerper_buehne_video_vorher_nachher.jpg, labor_steg_buehne_video_vorher_nachher.jpg
- Daten: umrisse_video.json (Video-Umrisse), stoergroessen.json (Clip-Bild/Gier/Lage je Bild der Anpassung)

### N2.8 Tests, Commits, offen

Tests (PowerShell, msys64 vorn): -R "birkin|hangar|50a|spiderman|formen|r38_birkin|room50|sichel|schwanz" 21/21 gruen (601 s),
darin unit_birkin_formen (neuer Keyframe-Vertrag), unit_birkin_szenen (groesster Schritt 600), unit_schwanz_r41,
integration_r41_sichel_form, integration_r41_birkin_pistole.
Paket-Gate: release/make_package.sh prueft EM30_HANGAR.EMD zusaetzlich gegen den Pin release/em30_hangar.sha256 (Gegenprobe mit
der 3799d7e94-Datei: die). Generator deterministisch (zweimal erzeugt, gleiche sha256 d6932b22...).
Commits: 41112385c (Modell/Haltung/Kopf/Test/Gate), d287fce53 (Szene: Schwanz, Spider-Man-Schritt), Dossier/Werkzeuge danach.
Offen:
1. Haltebilder h40/h64: im Video ein weiter Ausfallschritt nach links (Fussmitte 25-30 Bildpunkte links); Port steht am Fleck
   (Lage/Schritt der Szene, kein Haltungs-Parameter) - IoU dort 0,48 statt 0,55-0,70.
2. H/B nachher 1,43 gegen Video 1,70: die Sichel haengt im Port weiter seitlich als im Video; Sichel-Laenge/Haltung nicht
   geaendert (T2 im Rahmen bestaetigt), ein staerkeres arm_innen kostete IoU.
3. Kopfform/Gesicht bei 320x240 nicht bestimmbar (N2.4); ob die rote Kappe in h0 Kopf oder Schulterwunde ist, offen.
4. Schwanz h42-h46 jetzt bis 14 Bildpunkte links vom Video-Fleck (vorher bis 40 rechts).
5. Labor nur an der Buehne (N2.6); PSX-Plattform ungeprueft (Feder-Kette und CLUT-Zeilen nur PC, wie N1).

## Neuaufbau ab NB2 (Runde 41, 2026-11-03, Zweig r41/sichel_nb2 ab 6a1e9430d)

Nutzer (2026-11-03, ueber den Koordinator):
- "Birkin mit Schwanz in der Vorschau gerade wirkt jetzt noch schlechter als gestern abend, gerade was die komische Bein Pose
  angeht.... hast du das über Nacht verschlechtert?"
- Zu Fangen und Wurf: "vollkommen asynchrones Verhalten hin zur Spinne ... Es ist nicht mal mehr ansatzweise wie gestern
  Abend noch..."
- Zu den Leisten mit NB2-Spalte: "Das 2. Bild von links bei den Bildern sah doch, bis auf die Farbe im Vergleich zum Original
  recht gut aus. Aber die Farbe wiederum ist wie bei den anderen Birkin Formen oder? Wenn ja, stimmt die Farbe. Die Farbe
  soll schon den anderen Birkin Formen entsprechen. Wir bauen ja auch auf einen schlecht aufgeloesten Video auf, das durchaus
  dunkler sein kann."
- Zur Flucht: "Und die Spiderman springt auch nicht oder sowas. Er wirft sie auf den Boden in Richtung Kamera wo sie fliegt."
- "ich habe dir noch mal höher aufgelöste Versionen von birkin gegeben ..., vielleicht helfen die dir ja?"

Entscheid des Koordinators: Die Sichel-Form wird ab NB2 neu aufgebaut. Uebernommen wird nur, was der Nutzer ausdruecklich
verlangt hat. Der Zweig r41/sichel bleibt als Sicherung stehen:
- ecabea078 NB6 Beinhaltung
- 98f5221cf NB6 Flucht
- 18c8dadf1 Sicherung
Neuer Zweig r41/sichel_nb2 im selben Worktree.

Werkzeuge: re15_port/tools/r41_sichel/nb2neu/
- c6.py: Helfer; die Registrierung der Abnahme 3 liegt als reg.json daneben.
- lauf6.sh, raum6.sh: Laeufe mit eigener exe-Kopie.
- Messhaken in main.c (nur Messung, im Normalpfad stumm): beinhaken.c = RE15_BEIN_LOG, griffhaken.c = RE15_GRIFF_LOG.
- griff6.py: Abstand Sichelhand zu Griffknochen.
- beine6.py: Kniewinkel.
- flucht6.py: Spider-Man-Maske ohne Birkin.
- haut.py, farbe_leiste.py: Hautton.
- zuordnung.py: Zuordnung der Nutzerbilder.
- leiste_neu.py: Pflicht-Leisten.

Laeufe (Sitzungs-Scratchpad):
- G2: NB2 6a1e9430d mit Messhaken
- V3: r41/sichel 3f91e5c57 mit Messhaken
- R5/R6: neu (R6 = Endstand)
- R5090/R5090b: Birkin-Formen in 5090
Alle Leisten haben vier beschriftete Spalten: Video | NB2 | 3f91e5c57 | neu. Ablage: SF_belege/nb2neu/.

### NB2neu.1 Farbe (Frage des Nutzers)

Antwort: Ja. NB2 hat den Farbton der anderen Birkin-Formen und liegt in deren Helligkeitsband (nur etwas dunkler als EM30). Die
NB2-Farbe bleibt; die Abdunklungen aus NB3-NB5 sind nicht uebernommen (Hautzeilen hell/kontrast/lichter, Lichtzonen 486/487,
Kopf x0,72, Klaue 488, Kappe 489, Sichel 484 x0,5). Leiste 01_farbe_birkin_formen.jpg.

Hautton = Mittel der texel-gewichteten Flaechen, ohne Jeans (b > r), Wunden (r > 2,2 g) und Schwarz (haut.py):

| Form | Haut RGB | Farbton | Saettigung | am Bild (Median, Spiellicht) |
|---|---|---|---|---|
| EM30 Auslieferung (Mensch, 3070/5080, 5090 erste Form) | (118,91,89) | 4 Grad | 0,24 | kein Spielbild (3070-Lauf zeigt in 900 Bildern nur Ada/Marvin) |
| NB2-Hangar 6a1e9430d (= neu) | (95,68,67) | 3 Grad | 0,29 | h40 (81,59,57) |
| r41/sichel 3f91e5c57 | (69,45,44) | 2 Grad | 0,35 | h40 (64,48,45) |
| Riesenform 5090 (Bahnsteig-G4) | (103,74,65) | 14 Grad | 0,36 | Feuerlicht (44,29,26) |
| G5 (RE2 EM36) | (126,81,58) | 20 Grad | 0,53 | Feuerlicht (58,38,25) |
| Video 4 h40 (Schwarzwert abgezogen) | - | - | - | (61,46,42) |

Der Schwarzwert des Videos ist am Hintergrund selbst gemessen: Video = 0,954 x Port + 12,65 im Dunklen, Letterbox 15-18.

### NB2neu.2 Uebertragen (nur Nutzerwuensche)

(a) Linke Klauenhand kleiner, nach dem Video (aus NB3 Mangel 2).
- tools/birkin_formen.py FORMEN skal 12 (0,55/0,54/0,55), Unterarm 11 Laenge 0,80 (Dicke wie NB2), ducken ellbogen_links 60.
- Die Klaue endet auf Oberschenkelhoehe wie im hochskalierten Video h111-h132 (Leiste 04_klaue_h90-h132.jpg).
(b) Schwanz: Laenge, Glieder und Baender aus NB3/NB4.
- Kette 7 x ~466, Radien 150 -> 72, 10 Seiten, Profil, Deckel, Dorn.
- Gemalte Textur (schwanz_textur) mit Verlaufszeile 483.
- Bewegung 1:1 nach dem Video: k_schwanz_bahn, Durchgangspunkte, Verdecken, Dicke h58-h72. engine/src/schwanz_r41.c und der
  Kopf re15_schwanz_r41.h stammen aus 3f91e5c57.
- Der NB2-Takt h wird im Bild h - 2 gezeichnet (Lauf G2: Messzeile "h-1 F240", erste Letterbox F243). Die Szene ruft deshalb
  die Bahn je Bild (BF_BILD_VERSATZ -2). Leiste 03_schwanz_h18-h66.jpg.
(c) Gruenes Blut beim Hochheben als matter Schleier, NB5-Fassung.
- bf_gruenes_blut, abr_port 4, Cull-Ausnahme re15_bf_szene_effekte, main.c Mischart.
- Aus 3f91 auch re15_damage.c (Sub-Byte ganz, gruene Zeile).
- Summe gruener Bildpunkte h18-h92: Video 12417, NB2 0, neu 12149 (Lauf R6; R5 vor dem ABR0-Wechsel der Spritzer 10614).
- Saeule h53/h56/h58 neu 12/37/31 (Video 753/558/453): sie liegt weiter links (x 271-344).
- Die Bodenspritzer h84-h88 stehen im Originalmischweg ABR0 (mit dem Schleier waren sie unsichtbar): h84/h86/h88 neu
  21/363/0, Video 779/1151/170.
- Leisten 05_gruenes_blut_h28-h63.jpg, 05b_gruene_spritzer_h84-h88.jpg.
(d) Kein Glaettungsfilter: unveraendert.
(e) Flucht nach dem Wurf: passiv geworfen (s. NB2neu.4).

Nicht uebertragen (wie angeordnet):
- Ausfallschritt mit Bein-IK, Becken- und Schienbein-Aenderungen
- Vorbeugung und Gang aus NB4/NB5
- Annaeherung an die Kamera aus NB5
- Spider-Man gelb
- alle Licht- und Farbaenderungen
- Abblende
- Keyframe-Tween
- NB3-Zeitkorrektur BF_VORLAUF
Der Spider-Man traegt wieder die Auslieferungs-Textur des EM2B (kein EM2B_HANGAR).

### NB2neu.3 Griff synchron (Riegel)

Messhaken RE15_GRIFF_LOG, griff6.py. Abstand Sichelhand (Knochen 15) zu Griffknochen Spider-Man (20) je gezeichnetem Bild:

| h | 12 | 16 | 20 | 36 | 52 | 64 | 68 | 70 |
|---|---|---|---|---|---|---|---|---|
| NB2 6a1e9430d (Einheiten / px) | 762/58 | 126/8 | 70/1 | 65/4 | 39/2 | 60/3 | 340/18 | 1377/91 |
| r41/sichel 3f91e5c57 | 663/34 | 589/43 | 1049/74 | 940/61 | 902/65 | 866/57 | 897/58 | 1159/58 |
| neu | 762/58 | 4/0 | 0/0 | 0/0 | 0/0 | 0/0 | 0/0 | geworfen |

Ursache in 3f91 (Commits 1ef4c9bf4/e25d029d9, NB4/NB5): der Spider-Man hing ueber eine eigene Bildversatz-Spur neben der Hand
(k_sm_halte_dx/dy, k_sm_halte5_dx/dy bis -113 / -55 Bildpunkte); ab h70 stand Birkin im Stand-Clip, die Hand warf nicht.
NB2 glaettete im Halten und im Wurf (halber Abstand je Takt) und lief der Hand h66-h71 bis 1419 Einheiten nach.
Neu: ab dem 4. Haltetakt (Video h16) liegt der Griffknochen JEDEN Takt genau auf der Hand (engine/src/birkin_formen.c,
Haltezweig). h12-h15 zieht der Griff ihn wie in NB2 geglaettet heran. Im Wurfbild (Clip 0x0B B44, Takt BF_H_WURF) verlaesst er die
Hand mit dem Tempo der Hand.
Riegel unit_birkin_szenen:
- Abstand im Halten ab h16 unter 150 Einheiten (~10 Bildpunkte in Birkins Blicktiefe ~7000), mindestens 40 Haltetakte.
- Neu: 56 Haltetakte, hoechstens 0.
- Gegenprobe mit der entkoppelten Spur (bf_bild_versatz(k, 10, -55) wie NB5, nur lokal gebaut): ROT, "Griffknochen bis 1086
  Einheiten von der Sichelhand". Leiste 02_griff_halten_wurf_h4-h78.jpg mit dem Abstand unter jeder Kachel.

### NB2neu.4 Wurf und Flucht (passiv)

Nutzer: "Er wirft sie auf den Boden in Richtung Kamera wo sie fliegt."
Der Spider-Man bewegt sich nach dem Loslassen nicht selbst: kein Lauf-, kein Sprung-Clip. Er liegt in Rueckenlage (Clip 0x0B
B0-30), die Glieder sind schlaff.
Bahn je Videobild in k_wurf_bahn (engine/src/birkin_formen.c):
- Vom Loslassen bis h74 eine quadratische Bahn, deren Anfangsrichtung das Handtempo ist.
- h74-h76 fliegt er auf dem Sehstrahl in Blicktiefe 4800-4600 vor Birkin.
- h78/h80 schlaegt er am Boden auf (Blicktiefe ~5200, unter dem Bildrand).
- h82-h87 fliegt er rechts an der Kamera vorbei (Tiefe 4300 -> 2800).
- BF_H_WEG 89 = Bild 87.
Laeufe R3/R4/R5/R6 am Bild gegen das Video (Leiste 06_wurf_flucht_h70-h87.jpg, mit Griff-Abstand: im Bild des Loslassens
h70 verlaesst er die Hand).
Der Massstab im Bild ist die Lage aus dem hochskalierten Nutzerbild h74 (Screenshot 100003: er fliegt kopfueber gross vor
Birkin) und h85 (Screenshot 100432: Glied dicht vor der Kamera rechts, gruene Spritzer am Boden).
Die Maske aus flucht6.py ist hier unbrauchbar, weil der werfende Birkin (Clip 0x0B B44-B59) in der Spider-Man-Maske liegt.
Deshalb gibt es keine Flaechenzahlen fuer den neuen Stand.

### NB2neu.5 Zuordnung der hochskalierten Nutzerbilder (Version_80%/Screenshot 2026-11-03 *.png, nur gelesen)

zuordnung.py: Bild auf 640x480 skaliert. Normierte Korrelation gegen jedes Videobild, ohne Schriftzug "Vorschaumodus" und
UI-Raender. Ergebnis auch in SF_belege/nb2neu/zuordnung_screenshots.json.

| Datei | Videobild h | NCC | zweitbestes |
|---|---|---|---|
| 100406.png | h0 | 0.995 | h1 0.988 |
| 100341.png | h5 | 0.998 | h4 0.753 |
| 100310.png | h18 | 0.991 | h19 0.970 |
| 100249.png | h22 | 0.974 | h23 0.970 |
| 100215.png | h28 | 0.989 | h29 0.978 |
| 100142.png | h37 | 0.995 | h38 0.991 |
| 100118.png | h44 | 0.995 | h43 0.988 |
| 100055.png | h51 | 0.995 | h52 0.993 |
| 100028.png | h61 | 0.998 | h60 0.996 |
| 100459.png | h63 | 0.993 | h62 0.991 |
| 100003.png | h74 | 0.982 | h75 0.954 |
| 100432.png | h85 | 0.994 | h83 0.943 |
| 095941.png | h90 | 0.992 | h89 0.990 |
| 095852.png | h104 | 0.989 | h103 0.987 |
| 095820.png | h111 | 0.997 | h112 0.997 |
| 095754.png | h117 | 0.996 | h118 0.996 |
| 095731.png | h121 | 0.985 | h122 0.982 |
| 095706.png | h127 | 0.995 | h126 0.993 |
| 095645.png | h132 | 0.996 | h131 0.995 |
| 095606.png | h135 | 0.986 | h134 0.985 |
| 095918.png | h139 | 0.761 | h140 0.759 (unsicher, Abblende) |

Widerlegt durch die Zuordnung:
- Die Bilder 18-20 der Aufnahme-Reihenfolge sind nicht alle Wurf. 100406 ist h0 (Spider-Man hockt links vorn), 100003 ist h74
  (Wurf), 100432 ist h85 (Glied vor der Kamera).
- Das grosse orange Glied h85 ist im hochskalierten Bild schwarz-orange gebaendert.
- Der Spider-Man in h0 (100406) hat gelb-schwarz gebaenderte Glieder. Die KI-Hochskalierung erfindet hier moeglicherweise
  Farbe; die NB2-Auslieferungstextur bleibt (Entscheid Farbe).
Die Bilder stehen in den Leisten als Videospalte, wo h exakt passt (Beschriftung "hochskaliert").

### NB2neu.6 Gang

Der Gang entspricht NB2: Haltung (ducken) und Szenenweg sind wie NB2. Nur der Klauen-Ellbogen ist neu, er wirkt nicht auf die
Beine. Kniewinkel aus RE15_BEIN_LOG stehen unter jeder Kachel der Leiste 07_gang_h84-h132.jpg.

### NB2neu.7 Tests, Pins

- unit_birkin_formen: Knochen 11 aus der Liste der unveraenderten Winkel (Klauen-Ellbogen).
- unit_schwanz_r41 (aus 3f91): freie Kette im Stand (Clip 0 B0) statt Clip 0x0B B20 geprueft. In 0x0B B20 liegt die Spitze mit
  NB2-Haltung und NB4-Laenge auf dem Boden (Gelenk 6 Modellraum y -133, 3f91 -478). Die Pruefung "linke Seite" gilt nur bei
  Ruhelage links.
- unit_birkin_szenen: Griff-Riegel (s. NB2neu.3).
- release/em30_hangar.sha256 nachgezogen.

### NB2neu.8 Offen / Vorschlaege fuer den Nutzer (nicht eingebaut)

1. Zeitversatz NB2: Die Szene laeuft 2 Bilder vor dem Video (Takt h wird im Bild h - 2 gezeichnet). NB3 hatte das
   korrigiert (BF_VORLAUF 1). Es ist nicht uebernommen, weil es nicht verlangt war; Schwanz und Blut rechnen mit dem Versatz.
2. Beinhaltung gegen das Video: Das Video zeigt h96-h124 tief gebeugte Knie mit langen Schienbeinen, NB2 Knie 115-140 Grad. Im
   Zweig r41/sichel liegt dafuer ecabea078 (Schienbein 1,25, Hosenbund hoeher). Nicht uebertragen.
3. Spider-Man: Das Video und die hochskalierten Bilder (h0, h85) zeigen orange-schwarz gebaenderte Glieder, NB2 traegt die
   dunkelbraune Auslieferungs-Textur. Ein Entwurf liegt auf r41/sichel
   (SF_belege/nb6/zwischenstand_spiderman_textur.patch).
4. Wurf/Flucht: Der Spider-Man ist h79-h83 kleiner als im Video (das Video zeigt hochstehende Glieder im Vordergrund).
   EM2B hat keinen passenden Original-Clip "kopfueber geworfen".
5. Gruene Saeule h53-h58 schwaecher und weiter links als im Video (neu 12-37, Video 453-753 im Kasten x 320-365);
   Bodenspritzer h84 schwaecher als im Video (21 gegen 779).

### Referenz 2 (vom Nutzer gelobt 2026-11-03)

Nutzer (2026-11-03, ueber den Koordinator): "die jetzige vorschau von birkin sah schon nicht so schlecht aus."
- Referenz 2 = Commit 84e8a4d97 auf r41/sichel_nb2.
- Die laufende exe ist aus genau diesem Stand gebaut: Arbeitsbaum sauber, Ninja "no work to do", exe 10:46:42.
- EM30_HANGAR.EMD sha256 cfd1ac42729e5636... = release/em30_hangar.sha256.

Inhalt von Referenz 2: NB2-Koerper, -Farbe und -Gang plus die Nutzerwuensche
- a: Klaue klein
- b: Schwanz aus NB3/NB4 mit Bahn und Verdecken
- c: gruenes Blut als Schleier, Bodenspritzer ABR0
- d: kein Filter
- e: passiver Wurf auf die Kamera zu
- Griff hart gekoppelt (Riegel unter 150 Einheiten)

Ab jetzt prueft jede weitere Aenderung zusaetzlich gegen Referenz 2; was dagegen schlechter aussieht, ist ein Befund. Die Leisten
bekommen dafuer eine Spalte "Ref2".

### Nutzerentscheid Spider-Man-Farbe (2026-11-03)

Nutzer (ueber den Koordinator): "na original textur!"
- Der Spider-Man behaelt die dunkelbraune Original-Textur der Auslieferung (EM2B aus CDEMD0.EMS, wie Referenz 2). Der
  orange-schwarze Patch-Entwurf (r41/sichel, SF_belege/nb6/zwischenstand_spiderman_textur.patch) ist verworfen.
- Abnahme 4 Punkt A (Spider-Man-Aussehen) ist damit kein Befund mehr.

### Schritt 1 nach Referenz 2: Wurf zur Kamera

Auftrag: Im Video fliegt der Spider-Man h81-h85 dicht an der Kamera vorbei, das grosse orange Glied fuellt die rechte
Bildhaelfte; bei Ref2 bleibt er klein im Hintergrund.
- Ref2 hatte k_wurf_bahn ab h82 in Blicktiefe 4300 -> 2800.
- Neu ab h81 auf die Kamera zu: Tiefe 4600 (h81), 3800 (h82), 3200 (h83), 2500 (h84), 2200 (h85), 2000 (h86), 1900 (h87). Die
  Bildpunkte wandern von x 370 nach x 940, der Weltweg geht Richtung Kameraposition Cut 12.
- Loslassen und Anfangsrichtung bleiben die der Hand (unveraendert gegen Ref2).
Laeufe:
- R7 (Tiefe bis 1300): er fuellte h81-h85 das ganze Bild, verworfen.
- R8: h84/h85 noch kleiner als das Video-Glied.
- R9: Endstand.
Leiste 08_schritt1_wurf_zur_kamera_h76-h87.jpg (Video | Ref2 | neu, Kacheln 460 px).
Am Bild:
- h81-h83 steht er gross in der rechten Bildhaelfte mit hochstehenden Gliedern (Video h82/h83: Glied rechts der Mitte,
  Oberkante ~196).
- h84-h87 Glieder am rechten Rand. Sie sind noch kleiner als das Video-Glied h84/h85 (Oberkante 111/120), das bleibt offen.
Gegen Ref2 ist bis h80 nichts veraendert (gleiche Bilder). Ab h81 ist nichts schlechter gefunden.
Tests: unit_birkin_szenen (Griff-Riegel), unit_birkin_formen, unit_schwanz_r41 gruen.

### Schritt 1b: Glied h84/h85 groesser (nicht eingebaut)

Ein groesseres Glied h84/h85 liesse sich nur ueber eine andere Pose des fliegenden Koerpers erreichen. Die Rueckenlage-Bilder 0x0B
B0-30 heben kein Glied zur Kamera. Ein aufgerichtetes Sprungbild (0x16) waere der verworfene "Seestern"/Sprung. Nicht eingebaut,
damit h81-h83 nicht wieder zuwachsen. Offen: Video-Glied h84/h85 bis y 111/120, neu nur am rechten Rand.

### Schritt 2 nach Referenz 2: Loslassen

Video 4:
- h66-h70: Birkin haelt den Spider-Man oben links (Rumpf x ~100-170, y ~60-250).
- h71: er sinkt links herab.
- h72: kopfueber links (x 60-150).
- h73/h74: unten links.
Das Loslassen liegt zwischen h70 und h71.
Ref2 84e8a4d97:
- Die Hand schwang ab Bild 68 nach unten: Clip 0x0B B38 -> B44 bei Takt 70 -> 72, der Takt h wird im Bild h - 2 gezeichnet.
- Bild 70 war schon 3041 Einheiten von der Hand weg, Bild 72 lag er am Boden.
Neu (engine/src/birkin_formen.c):
- Abschwung 2 Takte spaeter: k_birkin_bild B27 bei Takt 68, Wurfbild B44 bei Takt 74, Aufschlag B55 bei Takt 78;
  k_birkin_gier mit.
- Loslassen im Takt BF_H_LOS 73 = Bild 71 mitten im Abschwung (B41), mit dem Tempo der Hand.
Griff-Abstand (griff6.py):

| Stand | h69 | h70 | h71 | h72 |
|---|---|---|---|---|
| Ref2 | 0 | 3041 | 3120 | 2829 |
| neu | 0 | 0 | 2617 | 3388 |

Am Bild: neu h72 kopfueber oben links wie das Video, h73 links Mitte, h74 unten. Ref2 lag h70 schon unten links.
Leiste 09_schritt2_loslassen_h66-h74.jpg (Video | Ref2 | neu).
Gegen Ref2 schlechter: nichts gefunden. Birkin haelt 2 Bilder laenger hoch wie im Video.
Tests: unit_birkin_szenen (Griff-Riegel), unit_birkin_formen, unit_schwanz_r41 gruen.
Hinweis: Die hochskalierten Nutzerbilder (Version_80%/Screenshot 2026-11-03 *.png) liegen nicht mehr dort. Die Leisten nehmen ab
jetzt das Videobild.

### Schritt 3 nach Referenz 2: gruenes Blut auf Video-Staerke

Gruene Bildpunkte (mess6.py, gleiche Maske wie Abnahme 4):

| Messung | Video | Ref2 | neu (Lauf R14) |
|---|---|---|---|
| Saeule h53 (Kasten x 320-365 y 95-150) | 753 | 12 | 170 |
| Saeule h56 | 558 | 37 | 533 |
| Saeule h58 | 453 | 31 | 443 |
| Bodenspritzer h84 | 779 | 21 | 449 |
| Bodenspritzer h86 | 1151 | 363 | 1025 |
| Summe h18-h92 | 12417 | 12149 | 13543 |

engine/src/birkin_formen.c bf_gruenes_blut:
- Saeule: 85 statt 60 Bildpunkte rechts der Hand, ab h50 (h50-h54 jedes Bild, dann jedes zweite bis h60), Skala BF_BLUT_SAEULE
  0x1800 (Ref2 0x1000, h52-h68 jedes zweite).
- Bodenspritzer: ein Bild frueher (Bild 82/84), Skala 0x4000/0x3000.
- Wolke: Skala 0x3400 statt 0x4000, damit die Summe nahe am Video bleibt.
Laeufe:
- R11 (0x2800, h52-h58 jedes Bild): Summe 45043
- R12 (Saeule bis h68): 23724
- R13 (Wolke 0x4000): 20144
Leisten 10_schritt3_gruene_saeule_h40-h60.jpg, 10b_schritt3_bodenspritzer_h84-h88.jpg (Video | Ref2 | neu).
Gegen Ref2 schwaecher: die Wolke h28-h40 (neu 32/22, Ref2 64/~70, Video 468/149). Sie war schon in Ref2 viel schwaecher als das
Video. Offen: Wolke h28-h48 links der Hand (Video x 222-324).

### Schritt 4 nach Referenz 2: Zeitversatz

- Ref2: Der Takt h wurde im Bild h - 2 gezeichnet (Messzeile "h-1 F240", erstes Letterbox-Bild F243). Die Szene lief 2 Bilder
  vor dem Video.
- Neu: BF_VORLAUF 6 -> 8 (Messzeile "h-1 F242"), Takt h = Bild h. BF_BILD_VERSATZ ist damit 0, und Schwanz, Blut und Wurfbahn
  bleiben auf denselben Bildern.
- Damit sich nur der Start verschiebt, bleibt der Bildablauf aus Schritt 2 erhalten:
  - k_birkin_bild und k_birkin_gier wieder mit den NB2-Takten (B27 Takt 66, Wurfbild B44 Takt 72, B55 Takt 76).
  - Loslassen BF_H_LOS = 71 = Bild 71.
  - Phase GRIFF bis BF_H_LOS.
  - BF_H_WEG 87 = Bild 87.
Ergebnis Griff-Abstand (griff6.py):

| Stand | h8 | h10 | h12 | h14 | h16 |
|---|---|---|---|---|---|
| Ref2 | 717 | 512 | 762 | 128 | 4 |
| neu | 1854 | 716 | 514 | 762 | 128 |

- Beruehrung neu bei Bild 12 (Video h10-h12 an Birkins Brust), Ref2 bei Bild 10.
- Loslassen Bild 71, Wurfbild 72, Spritzer und Schwanz unveraendert.
- Gruenes Blut: Summe 13964 (R16), Saeule 177/522/479, Spritzer h84/h86 529/1113.
Leiste 11_schritt4_zeitversatz_h0-h14.jpg (Video | Ref2 | neu).
Gegen Ref2 schlechter: nichts gefunden. Die Szene beginnt 2 Bilder spaeter wie das Video. Die Gesamtdauer bis zum Abgang ist
gleich (Szenenende Takt 135 = Bild 135).
Tests: unit_birkin_szenen (Riegel; ein Starr-Fehler im Zwischenlauf R16 kam vom Abgang: BF_H_WEG 89 liess den Spider-Man 2 Takte
am letzten Bahnpunkt stehen -> 87), unit_birkin_formen, unit_schwanz_r41 gruen.

## Schwanz V2 (Runde 41, 2026-11-06, Formblatt V2)

Auftrag: Schwanz nach SF_FORMBLATT_V2.md S1-S15 umbauen, Varianten V1 (Koerper Referenz 2) / V2 (Koerper K1-K10), je Schwanzende
spitz / Kralle (F2), Kopf nach dem Original plus KI-Kopfdetails (Nutzer-Entscheid F4 2026-11-06: "ja, das soll mit uebernommen werden
aus der KI Fassung" und "natuerlich in der grafischen Qualitaet des spieles"). Nichts davon ist im Spiel-Asset: EM30_HANGAR.EMD bleibt
Referenz 2 (sha256 cfd1ac42..., Pin unveraendert). Die Varianten erzeugt "birkin_formen.py --variante NAME --aus DIR".
⛔ Kein RE-Befund: alle Zahlen sind Bildmessung am Original/KI oder PORT-WAHL (gekennzeichnet), keine @0x-Konstanten.

Werkzeuge re15_port/tools/r41_sichel/v2/: b2 (Buehne der Spielkamera 50A0/12 mit Knochen-Kennung je Bildpunkt), anpassen_v2 (Pose
an den KI-Freisteller, Koerper ohne Schwanz, Seite = rechte Flanke (W)), mass_v2 / koerper_mass (Masse in H), koerper_suche,
kopf_entwurf / treffer (KI-Lagen auf das Modell), maul_referenz, riegel_v2, lauf_v2.sh (Szenenlauf mit Overlay-CD-Wurzel, nur
EMD/EM30_HANGAR.EMD darin; der Port faellt fuer alles andere auf shared_assets zurueck, asset_root_pc.c rr_read_list), leisten_v2.
Laeufe (Sitzungs-Scratchpad sfv2/r): R2n = Referenz 2 mit dem neuen Bau, V1s/V1k/V2s/V2k = Varianten.

### SV2.1 Ursache je Abweichung (Referenz 2 gegen Original, gleiche Pose, Seitenansicht h85, H = Oberkante bis Sohle im Bild)

| Mass | Original (Formblatt) | Ref2 | Ursache in Ref2 |
|---|---|---|---|
| Wurzelmitte unter OK | 0,15-0,17 H (S2) | 0,277 H | Wurzel (-380,120,0) an Knochen 8 = Brustrueckseite, 0,30 H unter der Modell-Oberkante (Gang B0) |
| Wurzel hinter dem hoechsten Punkt | 0,23-0,28 H (S5) | 0,150 H | dito |
| Verlauf | Haken: waagerecht nach hinten, dann senkrecht (S6) | gerade schraeg, Glieder 25 -> 57 Grad | Ruhelinie als Gerade definiert |
| Dicke 0,2 / 0,5 / 0,9 | 0,135 / 0,065-0,07 / 0,034 H (S10) | 0,068 / 0,060 / 0,038 H | Radien 150 -> 72 (Wurzel halb so dick) |
| Spitze ueber der Sohle | 0,32-0,44 H (S8) | 0,26 H (h85), Gang Welt Mittel 0,16 H | gerade Linie nach unten + Schwere 0,004 im Nachschwingen |
| Vorderansicht Schwanz sichtbar | hinter dem Koerper (S15) | h118 409 Bildpunkte zwischen den Beinen (Buehne) | Spitze haengt mittig bis fast zum Boden |
| Formblatt-Port-Ist vorn (Bund 0,52 / Saum 0,71 / Breite 0,74) | 0,42 / 0,57 / 0,57-0,65 | 0,47-0,49 / 0,65-0,68 / 0,65-0,68 | Formblatt nahm die Letterbox als Sohle (H rund 9 % zu klein); mit der wahren Sohle (Buehne, angepasste Pose) bleiben Bund +0,06, Saum +0,09, Breite +0,04 |

### SV2.2 Was geaendert ist

Schwanz V2 (birkin_formen.py SCHWANZ_V2, kette_v2_punkte; GESTALTUNG nach Formblatt V2):
- Ruhe-Mittellinie direkt aus den Formblatt-Zahlen im Modellraum der Gangpose (Clip 1 B0): Wurzelmitte 0,255 H hinter dem hoechsten
  Koerperpunkt, 0,19 H unter der Oberkante (Modellraum; die Spielkamera von oben macht daraus im Bild h85 0,16 H), Pfad S6 als Haken.
- Laenge 3475 E. wie Ref2 (S9). Die Seitenprojektion des Pfads ist rund 2400 E. lang; der Rest (S9/Ae2 "Seitwaertslauf"): Gelenk 16
  400 E. im Buckel versenkt und zwei seitliche Wellen (+-325 E.) im haengenden Teil, Spitze 0,12 H nach rechts (W) hinter das rechte
  Bein (PORT-WAHL). Probe vorn h118/h125 (sichtbare Schwanzpunkte, 640er): eine Welle ohne Versenkung 722/612 (trat an der Taille
  aus), eine Welle + 400 versenkt 252/325, zwei Wellen + 400 versenkt 0/0. Erster Stand mit Welle ab der Wurzel drehte das
  Wurzelglied 59 Grad aus der Seitenebene (Wurzel in h85 als Scheibe) - verworfen.
- Radien je sichtbarem Laengenanteil (S10), Gliederprofil 0,90/1,03 (Ref2 0,80/1,05), Wurzelglied mit Kragen 1,15 (S3 ohne Stufe),
  Nachschwingen ohne Schwere (Ae5).
- Ende spitz: Endglied verjuengt, Dorn 4 x Endradius. Ende Kralle: Kopie der EM30-Klauenhand (Mesh 12 der Form, Originaltexel) x1,4
  quer / x1,2 laengs ans Ende (KI-Seitenansicht "Hand mit roter Kralle", Leser A 2.5 Verdickung 0,06-0,07 H).

Feder-Kette (schwanz_r41.c): haengen() erlaubt je Glied den Keyframe-Winkel plus Spielraum (Kosinus 0,10), hoechstens 80 Grad; die feste
62-Grad-Grenze drueckte die V2-Glieder 4-5 (Keyframe 70-71 Grad) in die Senkrechte. Referenz 2: Glied 1 darf 65-68 statt 62 Grad.

Szene (birkin_formen.c), nur wenn die V2-Wurzel erkannt wird (Bindversatz Gelenk 16 x < -700 im Brustraum; V2 -986, Ref2 -380):
- Durchgangspunkte (Austritt an der Huefte) 550 hinter der Koerperebene; mit -150 lief das Stueck Wurzel -> Austritt vor dem Bauch
  (Lauf V1s h22/h24, erster Stand).
- Freigabe h74-h80 statt Verdeckung bis h102: das Original zeigt h77-h98 den Bogen vom oberen Ruecken (Formblatt S1, Punkt 4); die
  NB4-Verdeckung stuetzte sich auf "0 kraeftig rote Bildpunkte rechts der Huefte" und hat den Bogen (r-g 2-16) nicht erfasst.
  Im Lauf haengt der Schwanz ab h80 in seiner Ruhe. Weil Birkin im Port h83-h97 nicht in der rechten Flanke steht (Ae10,
  Szene/Spur H), liegt der Bogen dort hinter dem Buckel statt seitlich (offen).

Kopf V2 (beide Varianten, KOPF_V2): helle Kuppe oben, rote Zone um und unter dem Maul (Original K1/K2); offenes senkrechtes Maul mit
Zaehnen oben/unten, Auge oben (KI-Lagen birkin19: Auge (0,415 0,30), Maul x 0,28-0,72 y 0,44-0,93 der Kopfbox), Schulterauge rechts (W)
= Bild-links auf Mesh 8 (aus KI (705-715, 360-372) zurueckgerechnet: (128,-1156,-589), 75 x 104 E.). PORT-WAHL nach Nutzer-Entscheid
F4 2026-11-06, Quelle KI-Fassung, kein Original-Beleg. Machart gegen ein Originalmaul (maul_referenz.py):

| Teil | Dreiecke | Texel | Texeldichte (Texel/E.) | helle Texel |
|---|---|---|---|---|
| RE2-G4 Kiefer (CDEMD0.EMS kind 0x34, Mesh 26), Originalmaul mit Zaehnen | 118 | 4427 | 0,045 | 7 % |
| RE2-G4 Rumpf/Kopf (Mesh 10) | 446 | - | 0,041 | - |
| Kopf V2 (Mesh 9, EM30-Geometrie unveraendert; 74 von 109 Dreiecken tragen das Kopfbild) | 0 neu | 1780 (Flaeche 40 x 64, TPage 128 frei) | 0,068 | 4 % |
| Schulterauge (Mesh 8) | 2 neu (1 Viereck) | 48 | 0,079 | - |
| Vergleich EM30-Haut (Mesh 8) / EM30-Kopf | - | - | 0,051 / 0,091 | - |

Farben: eigene CLUT-Zeile 486 aus Hautton (NB2 x1,2), Schwanzrot (92,40,42), dunklem Maulinneren (KI dunkelste 10 % (31,13,13)),
Zaehne/Augenweiss gedaempft (190,170,162); 8 bpp, kein Weichzeichner, keine Leuchtfarben.

Koerper V2 (KOERPER_V2, nur V2; Formblatt F1 offen): Koordinatensuche koerper_suche.py gegen h118/h125: Oberschenkel x1,48, Schienbein
x1,08, Rumpf laengs x0,90, Brust quer 1,15 -> 1,09, Becken 163, Arme 9 Grad zur Mitte, Klauen-Ellbogen 20 Grad, Klauen-Unterarm 0,92,
Wurzel aller Keyframes +420 (sonst standen die laengeren Beine 420 E. im Boden - die Suche mass nur den Umriss).

### SV2.3 Zahlen (Buehne in der Spielkamera, gleiche Pose fuer alle; Leisten 01/02, SF_belege/v2/masse_seite_vorn.json)

| Mass | Original | Ref2 | V1 | V2 |
|---|---|---|---|---|
| Wurzelmitte unter OK h85 / h92 / h83 | 0,166 / 0,154 / 0,152 | 0,277 / 0,276 / 0,260 | 0,159 / 0,159 / 0,127 | 0,167 / 0,166 / 0,131 |
| Wurzel hinter Buckel h85 | 0,23-0,28 | 0,150 | 0,240 | 0,234 |
| Dicke 0,2 / 0,5 / 0,9 (h85) | 0,135 / 0,065-0,07 / 0,034 | 0,068 / 0,060 / 0,038 | 0,140 / 0,079 / 0,026 | 0,123 / 0,072 / 0,025 |
| Spitze ueber Sohle h85 / h92 | 0,33 / 0,44 | 0,26 / 0,28 | 0,37 / 0,38 | 0,37 / 0,38 |
| Spitze Gang h108-h132 (Lauf, Welt, Mittel / Spanne) | (0,32-0,44 im Seitenbild) | 0,16 / 0,10-0,27 | 0,28 / 0,20-0,36 | 0,29 / 0,21-0,38 |
| Schwanz vorn sichtbar h118 / h125 (Bildpunkte 640er) | hinter dem Koerper | 409 / 0 | 0 / 0 | 29 / 2 |
| Bund / Saum unter OK h118 | 0,42 / 0,57 | 0,47 / 0,65 | 0,47 / 0,65 | 0,40 / 0,59 |
| Breite in 0,2 H h118 / h125 | 0,57-0,65 | 0,65 / 0,68 | 0,65 / 0,68 | 0,60 / 0,62 |
| Krallen / Sichelspitze ueber Sohle h118 | 0,30-0,36 / 0,17-0,27 | 0,44 / 0,15 | 0,44 / 0,15 | 0,37 / 0,17 |
| Griff h16-h70 (Lauf, groesster Abstand) | < 150 | 128 | 128 | 129 |

Grenzen: Bildmasse sind Projektionen; der Port steht in der Seitenansicht aufrechter als das Original (Koerperhaltung, Spur H), die
angepasste Pose deckt den KI-Umriss mit IoU 0,62-0,73. Das Weltmass im Gang liegt rund 0,05 H unter dem Bildmass der Seitenansicht
(Buehne h85: V1 Welt 0,31 / Bild 0,35).

### SV2.4 Riegel (riegel_v2.py, Exit 1 = rot)

R1 Wurzelmitte h85 0,13-0,19 H; R2 Wurzeldicke >= 0,11 H; R3 Spitze Gang (Lauf, Welt) Mittel 0,26-0,42 H, kleinster Wert >= 0,18 H;
R4 kein Gelenk tiefer als sein Radius (Keyframes Clip 0/1 und Lauf h0-h132); R5 keine Schwanz-Vertices/Gelenke in den Koerper-Huellen
(Keyframes Clip 0/1, Lauf h108-h132); R6 Griff h16-h70 < 150 E. Ergebnis: V1 spitz/Kralle und V2 spitz/Kralle GRUEN; Gegenprobe
Referenz 2 ROT (R1 0,277 H, R2 0,068 H, R3 Mittel 0,16 H). ctest unit_birkin_formen, unit_birkin_szenen, unit_schwanz_r41,
integration_r41_sichel_form 4/4 gruen (unit_schwanz_r41 (8) prueft die neue Haengegrenze: max(62 Grad, Keyframe + 8), <= 80 Grad).

### SV2.5 Leisten (analysis/befunde_runde41/SF_belege/v2/)

01_seite_h83_h85_h92 (Original | KI | Ref2 | V1 | V2, Linien und Zahlen), 02_vorn_h118_h125, 03_halten_h24_h61 (Laeufe, Griff-Abstand),
04_gang_h84-h132 (Laeufe, Kniewinkel, Spitzenhoehe), 05_kopf_nah (KI | Ref2 | V1 | V2), 06a/06b_gesamt_v1 (h0-h132, Ref2 | V1, Griff),
07_schwanzende_spitz_kralle (V1/V2 je spitz/Kralle, KI, Original), 08_rundum_* (je Variante 6 Blickwinkel, Kopf und Schwanz nah).

### SV2.6 Empfehlung

- Schwanz V2 (in V1 oder V2) erfuellt S2/S5/S6/S8/S10 in der Seitenansicht und S15 von vorn; Referenz 2 erfuellt keinen dieser Punkte.
  Empfehlung V1 spitz als naechster Spielstand: der Koerper bleibt der gelobte von Referenz 2, nur Schwanz und Kopf aendern sich.
- V2 nur, wenn der Nutzer F1 bejaht: Bund/Saum/Breite/Krallen naeher am Original (0,40/0,59/0,60/0,37 statt 0,47/0,65/0,65/0,44), aber
  Oberschenkel x1,48 - die Figur wirkt deutlich hochbeiniger (Leiste 08_rundum_V2); der Saum bleibt 0,02 H zu tief.
- Ende: spitz entspricht der KI-Vorderansicht h20-h73, Kralle der KI-Seitenansicht; das Original entscheidet nicht (F2) - Nutzerwahl.

### SV2.7 Offen

1. Szene: Birkin steht h83-h97 im Port nicht in der rechten Flanke (Ae10, Spur H) - der Bogen liegt dort hinter dem Buckel statt seitlich.
2. Halten h24 (V2): der sichtbare Schwanz ist kuerzer als bei Ref2, weil 400 E. im Buckel stecken und die Wurzel hoeher sitzt
   (Reichweite der NB4-Bahn); V1 erreicht die Videopunkte.
3. Dicke vor der Spitze 0,022-0,026 H statt 0,034 H (spitzes Endprofil).
4. Kopf: Maul/Augen sind KI-Gestaltung; im Spielbild h118 wirkt das Maul als dunkles Oval mitten im Kopf - Groesse/Lage nach Sichtung
   durch den Nutzer nachstellen. Die seitliche Kopf-Nahaufnahme (Leiste 05, h85) zeigt den Kopf in dieser Pose nicht.
5. F3 (rote Stelle am Innenknie rechts (W)): V1/V2 halten die Spitze hinter dem rechten Bein; im Lauf erscheint sie nicht klar am
   Innenknie. Die Labor/Gang-Sequenz n780-n840 (F7) zeigt die rote Kreatur zu klein und unscharf, der Schwanz ist nicht trennbar -
   keine Messung; Final Build 58,5-64,5 s nicht ausgewertet.
6. PSX: Feder-Kette und CLUT-Zeilen 483-486 nur auf dem PC geprueft.
7. Nach der Wahl des Nutzers: Variante nach FORMEN['hangar'], Pin release/em30_hangar.sha256, Suite/Paket-Gate (Schritt 5).

## Schwanz V3 / Kopf V3 (Runde 41, 2026-11-06, Messblatt V3)

Anlass: Der Nutzer hat V1/V2 abgelehnt ("Die Augen sehen nicht mal ansatzweise aus wie im Original ... Der grosse Schwanz auch nicht
und hat komplett die falsche Farbe"). Vorgabe: SF_MESSBLATT_V3.md (KI-Fassung, Nutzerentscheid F4, PORT-WAHL, PSX-Machart).
Kein RE-Befund, keine @0x-Konstante. Spiel-Asset EM30_HANGAR.EMD und Pin unveraendert (Ref2-Bau byte-gleich nachgeprueft,
sha256 cfd1ac42...). Variante: birkin_formen.py --variante v3 --aus DIR -> Scratchpad sfv3/emd/EM30_V3.EMD (sha256 9a0d3630...).

### SV3.1 Ursache der Ablehnung in Zahlen (V1, Riegel-Gegenprobe riegel_v3.py, SF_belege/v3/riegel_V1_gegenprobe.txt)

| Teil | V1 | Vorgabe | Ursache in V1 |
|---|---|---|---|
| Kopfauge | 0,007 x 0,007 H (nur Pupille), B/H 1,00, Lidring 87 % hell, L Lid/Haut 1,77 | 0,017-0,025 x 0,008-0,014 H, B/H 1,5-2,5, Lid hell <= 0,10 | rundes Ringauge mit Weiss und Kreuz (KOPF_V2 weiss 252) |
| Schulterauge | aequiv. 3,7 x Kopfauge | 0,8-1,3 x | Ringauge 75 x 104 E. |
| Maul | 0,045 x 0,047 H, B/H 0,94, innen a* -2,9 gegen Haut | 0,075-0,095 x 0,038-0,055 H, B/H 1,5-2,3, a* >= +4 | senkrechtes Oval, Zaehne als zwei grosse helle Boegen |
| Schwanzfarbe h85/h92 | L/Haut 0,84/0,81, C/Haut 1,47/1,42, Rot 0,87 gegen Haut 0,37, Rest-Std a* 3,95 | 0,50-0,72 / 0,45-0,90 / <= 0,8 x Haut / <= 2,0 | eigene Rot-Zeile 483 mit gemalten Ringen |
| Form | Dicke 0,2 0,140 H, 0,9 0,025 H; max 38,5-40,7 Grad, 4-5 Knicke | 0,15-0,20 / 0,028-0,045; <= 36, <= 3 | Haken + zwei Seitenwellen: Keyframe-Gelenkwinkel 100-126 Grad |
| Bewegung (Spiellauf) | Gelenkwinkel im Raum bis 154 Grad (Gang) / 148 Grad (Halten) | <= 25 Grad, <= 3 ueber 20 | Wellen + Feder-Nachlauf + FABRIK der Schwanzbahn |
| Machart | 720 Dreiecke, Texeldichte bis 0,140, CLUT-Zeile 483 | <= 212, 0,045-0,090, Haut-CLUT | 7 Rohrglieder x 10 Seiten x 5 Ringe |

### SV3.2 Was geaendert ist

Koerper: unveraendert Referenz 2 (R28: Meshes 0-15, Skelett 0-15, alle Keyframes 0-15, EDD gleich; nur Mesh 9 UV/CLUT und 1 Viereck auf Mesh 8).

Schwanz V3 (SCHWANZ_V3, kette_v3_punkte, kette_meshes_v3):
- Ruhelinie = Kreisbogen gleicher Kruemmung: Start 20 Grad unter der Waagerechten nach hinten, Gesamtdrehung 100 Grad (Ruhe-Gelenkwinkel
  ~14 Grad), Wurzel wie V2 (0,255 / 0,19 H Modellraum, 400 E. versenkt), Laenge 3475 E. Seitlich nur ein monotoner Zug (Spitze 0,16 H
  nach rechts (W)) statt zweier Wellen. Probe Startrichtung/Drehung: 10/110 -> max 35-40 Grad (R25 rot), 15/105 -> 28-33 Grad mit
  2-3 Knicken, 20/100 -> 24-27 Grad mit 1-2 Knicken. Seitenzug: 0,12 H -> von vorn 122 / 0 Bildpunkte, 0,16 H -> 0 / 3.
- Geometrie: 7 Kegelstuempfe mit 10 Seiten (Manschette ins Vorglied, Ringe mit Gehrung an den Gelenken), Deckel in den Rumpf, Spitze
  1,6 x Endradius = 160 Dreiecke (V1 720). Radien 380 -> 68 E.
- Textur: Hautstreifen A der Seite 2 (u 44-83, v 30-88) mit der Hautzeile 482, ohne die roten Querstreifen (v >= 92); Laenge auf u,
  Umfang gespiegelt auf v (die Marmorierung hat keine Vorzugsrichtung: Strukturtensor-Anteil v 0,49 Streifen / 0,48 Oberarm; mit
  Laenge auf v Rest-Std a* 2,2-2,6, auf u 1,65-1,9). Je Glied ein anderes Fenster. Keine eigene CLUT-Zeile.

Kopf V3 (KOPF_V3, nah_raster, kopf_v3_malen): Bild in der Riegel-Messansicht entworfen und je Texel abgetastet (gleiche Flaeche
TPage 128 u 0-39 v 156-219 und Projektion wie V2; Zeile 486 mit KI-Farben aus dem Messblatt):
- ein dunkles Mandelauge 0,022 x 0,0105 H, Pupille (18,6,11), ein heller Kern (183,171,178) mittig-unten, Lidrand (107,92,97), keine Sclera.
- helle Kuppe (K9, Rampe L* 40-53), graue Kopfhaut, rote Zone ab Maul-Oberkante bis Kinn (Ref2-Zeile 485 x 0,9-1,3).
- waagerechtes Maul 0,092 x 0,040 H (Umriss |x|^3 + y^2 <= 1), Inneres (54,27,28), dunkler Schlund, 11 / 7 kleine Zaehne (162,122,124).
- Schulterauge: 1 Viereck (0,062 H) 60 E. vor der Schulter, Lage -0,117 / +0,058 H zum Kopfauge, Haut = EM30-Texel der Schulter darunter
  (Zeile 480, dunkle Ader im Viereck uebermalt), Auge = Hautzeilen-Eintrag mit L* 0,375 x Hautton; Lichtnormale aus den Nachbarecken
  (mit der Flaechennormale war das Viereck im Spiellicht schwarz).

Feder-Kette (schwanz_r41.c glaetten): Knick-Grenze je Gelenk Keyframe-Winkel + 3 Grad, 12-16 Grad, Boden nur ohne neuen Knick; nur fuer
Ketten mit der hohen Wurzel (Gelenk 16 x < -700 = V1/V2/V3), Referenz 2 unveraendert. Probe mit 22 Grad: Raum max 22,2 Grad, im Bild
bis 55 Grad an verkuerzten Gliedern.

### SV3.3 Riegel-Tabelle V3 (riegel_v3.py, SF_belege/v3/riegel_V3.txt; Exit 0)

| Riegel | Soll | Ist V3 |
|---|---|---|
| R1 Kopfaugen | genau 1 | 1 |
| R2 Schulterauge | -0,10..-0,13 / +0,04..+0,07 H | -0,117 / +0,059 H |
| R3 Kopfauge | 0,017-0,025 x 0,008-0,014 H | 0,0176 x 0,0105 H |
| R4 Form | B/H 1,5-2,5 | 1,67 |
| R5 Helligkeit | L* Auge/Haut 0,35-0,55 | 0,475 |
| R6 kein heller Ring | Lid hell <= 0,10; Lid/Haut 0,70-1,00 | 0,019; 0,806 |
| R7 Sclera/Kern | L80 <= 0,05; hell 0,05-0,25; Kern x 0,4-0,7 y 0,55-0,8 | 0,000; 0,186; (0,44 0,73) |
| R8 Pupille | <= 0,12 | 0,060 |
| R9 Schulterauge | L 0,30-0,50; hell <= 0,05; 0,8-1,3 x | 0,462; 0,043; 1,28 |
| R10 Maul | 0,075-0,095 x 0,038-0,055 H | 0,0805 x 0,0428 H |
| R11 Maul Form | 1,5-2,3 | 1,88 |
| R12 Maul Lage | 0,04-0,07 H; Versatz <= 0,012 | 0,0535; 0,0067 |
| R13 Maul innen | L 0,20-0,40; da >= +4 | 0,213; +5,9 |
| R14 Zaehne | oben >= 5, unten >= 3, <= 0,005 H, L <= Haut+20, a* 12-20 | 11 / 4, 0,0032 H, L 43,0 (Haut 32,4), a* 13,8 |
| R15 L*/Haut h85/h92 | 0,50-0,72 | 0,580 / 0,594 |
| R16 C*/Haut | 0,45-0,90 | 0,55 / 0,53 |
| R17 Farbton | abs <= 10 Grad | -7,3 / -8,4 |
| R18 Rot | <= 0,8 x Haut | 0,225 (0,375) / 0,219 (0,381) |
| R19 dE00 | 6-11 | 10,1 / 10,0 |
| R20 Muster | Periode <= 0,25; Rest-Std a* <= 2,0 | 0,10; 1,67 / 0,01; 1,65 |
| R21-R23 Dicke 0,2/0,5/0,9 | 0,15-0,20 / 0,07-0,10 / 0,028-0,045 H | 0,164/0,085/0,036 (h85), 0,155/0,081/0,034 (h92) |
| R24 Verlauf | kein Anstieg > 0,01 H | 0,000 |
| R25 Glaette | <= 36 Grad | 27,4 / 24,4 |
| R26 Knicke | <= 3 | 2 / 1 |
| R27 Austritt | 0,10-0,16 H | 0,145 / 0,146 |
| R28 Koerper | = Ref2 | gleich (Mesh 8 +1 Viereck) |
| R29 Dreiecke Schwanz | <= 212 | 160 |
| R30 Kopf | <= 118; <= 2 | 109; 2 |
| R31 Texeldichte | 0,045-0,090; Kopf 0,05-0,091 | 0,046-0,082; 0,073 |
| R32 Palette | 8 bpp, Haut-CLUT, C* <= Haut | 8 bpp, Zeile 482, C* 29,7 <= 63,3 |
| R25b/R26b Gang h84-h132 (Lauf) | Raum <= 25 Grad, <= 3 ueber 20 | 16,2 Grad, 0 |
| R25b/R26b Halten h16-h70 (Lauf) | dito | 16,2 Grad, 0 |

Gegenprobe V1 SPITZ: 26 von 34 rot (u. a. R3-R11, R13-R16, R18-R21, R23-R26, R29, R31, R32, R25b/R26b). ctest der vier Pflichttests 4/4 gruen.
Schwanz von vorn (Buehne h118/h125): 0 / 3 Bildpunkte (Ref2 409 / 0).

### SV3.4 Leisten (analysis/befunde_runde41/SF_belege/v3/)

01_seite_h83_h85_h92, 02_vorn_h118_h125, 04_gang_h84-h132 (Spiellaeufe sfv3/r/R2n, V1s, V3), 05_kopf_nah, 07_schwanz_nah, 08_rundum_V3,
09_farbe, Messansicht und Riegelbild Kopf, Riegel-Ausgaben.

### SV3.5 Offen

1. Ende des Schwanzes (F2): V3 spitz; Hand/Kralle der KI-Seitenansicht nicht gebaut (Nutzerwahl).
2. R25b/R26b messen die Gelenkwinkel im Raum (Grenze 25 Grad aus dem Faktor Bild/Raum 1,45 der Buehne h85), nicht die Bildmaske: die
   Buehnen-Rekonstruktion der Lauf-Kette (riegel_v3.py --bild) passt Gier/Takt nicht sicher zum Spielbild (h90 Ruecken im Spiel,
   Vorderansicht in der Buehne) und liefert Scheinwinkel bis 135 Grad an verkuerzten Gliedern.
3. Knick-Grenze nur fuer V1/V2/V3 (hohe Wurzel); Referenz 2 behaelt im Spiel Gelenkwinkel bis 163-168 Grad. Uebernahme mit dem Einbau.
4. Spitze 0,16 H ueber der Sohle (Formblatt V2 S8 0,32-0,44 H, kein Messblatt-V3-Riegel): mit glattem Bogen und 3475 E. Laenge ohne
   Seitenwellen nicht erreichbar.
5. Gesamtdrehung im Bild h85/h92 67 / 48 Grad (Richtwert 80-125, kein Riegel).
6. Knappe Riegel: R31 Glied 0 0,046 (>= 0,045), R13 0,213 (>= 0,20); das Maulinnere liegt je nach Flaechenschattierung nahe der
   Messschwelle 0,5 x Fenster-Median (deshalb der dunkle Schlund).
7. Schulterauge ist ein ebenes Viereck 60 E. vor der Schulter; aus steilen Blickwinkeln als Kante sichtbar (Leiste 08).
8. PSX: Feder-Kette nur auf dem PC geprueft.

## Schwanz V3 / Kopf V3 - Runde 2 (2026-11-06, Pruefung V3 r1)

Anlass: Pruefung V3 r1 nicht bestanden (SF_PRUEFUNG_V3_r1_sicht.md: K3 bei A4 Kopf seitlich und B1/B2 Maul; SF_PRUEFUNG_V3_r1_zahlen.md:
Konstanten ohne Quelle, Spitze unter dem Boden, h83 4 Knicke, Schulterauge 0,032 Texel/E., knappe Riegel). Kein RE-Befund, keine
@0x-Konstante: Gestaltungsquelle bleibt die KI-Fassung (F4, PORT-WAHL). Spiel-Asset EM30_HANGAR.EMD und Pin unveraendert
(Referenz-2-Neubau sha256 cfd1ac42... byte-gleich). Variante: sfv3/emd/EM30_V3.EMD (sha256 1952713c...), Runde 1 als EM30_V3_r1.EMD.
Neue Belegzahlen der KI aus eigener Messung (tools/r41_sichel/v3/n2_ki_kopf.py, SF_belege/v3r2/ki_kopf.json, ki_kopf_profile.txt).
H = Oberkante Buckel bis Sohle im selben Bild; Seiten (W) = Sicht des Wesens.

### SV3r2.1 Ursache der Ablehnung in Zahlen (Runde 1)

| Punkt | Runde 1 | KI | Ursache |
|---|---|---|---|
| A4 Kopf in der Seitenkamera h85/h92 | 283 / 346 von 668 / 790 Kopf-Bildpunkten Wundfleisch (Zeile 485), kein Auge, kein Maul | Schnauze grau (L* 43 oben / 26 unten), Auge 0,034 H hinter der Spitze, rotes Maulprofil mit 5 Stummeln | nur die Vorderflaechen (Normale . Blick vorn <= -0,15) trugen das Kopfbild; von der Seite sieht die Kamera 22 reine Seitenflaechen und 25 Vorderflaechen streifend (n2_kopf_richtung.py) |
| A3 Gesichtsfeld / Nachbarhaut, Spielkamera h118 / h125 | 1,35 / 1,28 | 0,87 (Freisteller) | KI-Farben absolut, Port-Haut dunkler (n2_maul_spiel.py) |
| B1 Maul | schwarzes Inneres ohne Lippe, 0 Glanzpunkte max(RGB) > 170 | Lippe dunkelrot, 29-33 Glanzpunkte (1,4-1,9 % des Bands), Spalt 0,012 H | Inneres + Schlundband, keine Lippe |
| B2 Zaehne | 11 / 7 Spitzen bis 0,006 H | Stummel ~0,0031 H, 2-3 px | Zahnform Dreieck |
| Spitze im Spiellauf | Halten bis 612 E. unter dem Boden (Lauf sfv3/r/V3) | - | glaetten() liess die Spitze unter dem Boden, wenn der Bodenzug einen Knick erzeugt haette |
| h83 (Buehne) | max 37,7 Grad je 0,05 H, 5 Knicke (n2_schwanz_mass.py; Pruefer 35,6 / 4) | 28,4 Grad, 3 Knicke in Port-Aufloesung | Keyframes ohne Knick-Grenze (die Grenze galt nur im Spiel) |
| Schulterauge | 12 x 12 Texel auf dem Viereck = 0,032 Texel/E. | Haut 0,045-0,090 | zu kleine Texelflaeche |

### SV3r2.2 Was geaendert ist (birkin_formen.py, schwanz_r41.c)

- Seitenbild Kopf: Seitenflaechen von Mesh 9 (Normale . Blick der Spielkamera h85 <= -0,10, Gegenseite gespiegelt an der Ebene durch das
  Kopfauge) bekommen ein Bild in TPage 128 u 64-111 v 156-219 (frei, n2_frei.py), Parallelprojektion aus der Spielkamera, 0,072
  Texel/E. Inhalt: Schnauzenhaut KI birkin12 (oben L* 18-58 bis zur Unterkante des Augenprofils, unten 16-41, eigene Rampen Zeile 486
  Index 32-42, Faktor KOPF_V3_HELL_SEITE 1,3 nach Probe), Maulprofil rot (KI Lab [27,3 22,8 8,4]) vom projizierten Mundwinkel bis
  0,101 H hinter die Vorderkante, 0,028 H hoch, 5 Stummel; das Kopfauge selbst sitzt an der Vorderkante.
- Kopf vorn: KOPF_V3_HELL 0,85 auf alle KI-Hautfarben (Probe 0,75: R3/R4/R13 rot). Kuppe bis an die Oberlippe (KI-Halo-Rampe, Lage aus
  dem KI-Median, Streuung aus KI-Texelstreuung 5,2-7,4 L*). Die KI-Stirn (L* 22-31) und das dunkle Band unter dem Auge (L* 15-21) sind
  nicht uebernommen: Probe r2c zaehlte R1 dort 2-3 Augen und R10-R12 mass das Band als Maul.
- Maul: Lippe 0,007 H (KI birkin19 Zeilen 55-58) Lab [19,6 17,1 5,9], Oeffnung 0,036 H innen Messblatt-Farbe [13,5 13,7 4,8], Spaltkern
  0,29 der Oeffnung (KI-Zeilenprofil: 0,046-0,058 H unter dem Auge L* 9-13), 5 Glanzpunkte (KI RGB (198,143,146)), 6 / 4 stumpfe
  Stummel 0,0031 x 0,0053 H, L* 45 (KI 49,7 x 0,9; Begruendung im Generator), Breite 0,092 H bis an die Mundwinkel.
- Auge: Glanzkern 0,0044 x 0,0052 H, gemessen y 0,68 (Runde 1 0,73).
- Schulterauge: 20 x 20 Texel (u 42-61 v 156-175) = 0,053 Texel/E., Auge 0,0148 x 0,0122 H (R9 1,01 x Kopfauge).
- Unbelegte Konstanten der Runde 1 (Zahlen-Pruefer Abschnitt 5) ersetzt: Rampenlagen/Rauschgewichte aus KI-Median/-Streuung,
  Kern-Achsverhaeltnis 0,6 und Schlundbreite 0,97 entfallen, Zahnform = Rechteck mit belegter Groesse, Spitzen-UV 0,92 -> 0,5
  (Fenstermitte wie der Deckel), Ader-Schwelle 0,6 -> 0,5 (Halbwertregel P_kopf.auge), Lichtnormalen-Umkreis 400 E. -> halbe
  Viereck-Diagonale + Abstand (378 E.), Rauschfeld 1:1 statt y x 3 / x x 2, auf +-2 Std begrenzt (dunkle Adern erzeugten sonst ein
  zweites Auge in R1), rauschen 1,3 und zonen kuppe_bis/rot_ab entfallen.
- Schwanz-Keyframes: Knick-Grenze wie schwanz_r41.c glaetten (Ruhewinkel + 3 Grad, 12-16 Grad) schon in schwanz_bewegung; h83 max
  37,7 -> 25,7 Grad je 0,05 H, Knicke 5 -> 4 (Bildmessung; im Raum <= 16 Grad). h85/h92 unveraendert gruen.
- schwanz_r41.c glaetten: liegt nach der Knick-Grenze ein Gelenk unter dem Boden, wird die Kette starr um Gelenk 0 hochgedreht (1-Grad-
  Schritte, <= 90 Grad, alle Gelenkwinkel bleiben). Lauf sfv3/r/V3r2: unter Boden max 612 -> 0 E. (Halten), Gang -296 E.; Raum max
  16,2 Grad, 0 Gelenke > 20 Grad (SF_belege/v3r2/gang_boden_V3r2.json).
- Deckel an Glied 0 bleibt (Probe ohne Deckel: R25 h85 34,1 Grad, h83 42,6 Grad).

### SV3r2.3 Zahlen Spielkamera (SF_belege/v3r2/*.txt)

| Mass | KI | Runde 1 | Runde 2 |
|---|---|---|---|
| Kopf seitlich h85 / h92: Wundfleisch-Bildpunkte | - | 283 / 346 | 0 / 0 |
| Kopf seitlich h85: L* oben / Haut, unten / Haut | 1,48 / 0,90 (birkin12) | 1,28 / 0,73 (Wundfleisch) | 1,30 / 0,75 (h92 1,29 / 0,70) |
| Kopf seitlich h85: rote Bildpunkte (a* > 15) / helle Stummel | Maulprofil + 5 Stummel | 323 / 67 (Wundfleisch) | 166 / 45 |
| Kopfauge seitlich (Spielkamera, 640er) | 3,7 x 2,6 px | nicht sichtbar | 1 x 3 px an der Vorderkante (L* 0-14) |
| Gesichtsfeld / Nachbarhaut h118 / h125 | 0,87 | 1,35 / 1,28 | 1,05 / 1,00 |
| Lippenglanz max(RGB) > 170 (1280er) h118 / h125 | 69 / 31 | 0 / 0 | 16 / 8 |
| schwarze Flaeche L* < 0,25 x Haut am Auge (1e-4 H^2) h118 / h125 | 14,6 / 16,5 | 35,2 / 23,1 | 24,0 / 29,6 |

### SV3r2.4 Riegel-Tabelle (riegel_v3.py, SF_belege/v3/riegel_V3.txt, Lauf sfv3/r/V3r2; Exit 0)

| Riegel | Soll | Ist |
|---|---|---|
| R1 Kopfaugen | genau 1 | 1 |
| R2 Schulterauge | -0,10..-0,13 / +0,04..+0,07 H | -0,118 / +0,058 H |
| R3 Kopfauge | 0,017-0,025 x 0,008-0,014 H | 0,0176 x 0,0105 |
| R4 Form | 1,5-2,5 | 1,67 |
| R5 Helligkeit | 0,35-0,55 | 0,418 |
| R6 Lidring | hell <= 0,10; Lid/Haut 0,70-1,00 | 0,000; 0,920 |
| R7 Sclera/Kern | L80 <= 0,05; hell 0,05-0,25; x 0,4-0,7 y 0,55-0,8 | 0,000; 0,114; (0,49 0,68) |
| R8 Pupille | <= 0,12 | 0,039 |
| R9 Schulterauge | L 0,30-0,50; hell <= 0,05; 0,8-1,3 x | 0,362; 0,022; 1,01 |
| R10 Maul | 0,075-0,095 x 0,038-0,055 H | 0,0831 x 0,0454 |
| R11 Form | 1,5-2,3 | 1,83 |
| R12 Lage | 0,04-0,07 H; Versatz <= 0,012 | 0,0562; 0,0060 |
| R13 innen | L 0,20-0,40; da >= +4 | 0,251; +4,9 |
| R14 Zaehne | oben >= 5, unten >= 3; <= 0,005 H; L <= Haut+20; a* 12-20 | 7 / 6; 0,0046; L 42,3 (Haut 24,3); a* 19,2 |
| R15 L*/Haut h85/h92 | 0,50-0,72 | 0,571 / 0,585 |
| R16 C*/Haut | 0,45-0,90 | 0,56 / 0,54 |
| R17 dh | <= 10 Grad | -7,7 / -7,8 |
| R18 Rot | <= 0,8 x Haut | 0,228 (0,361) / 0,215 (0,367) |
| R19 dE00 | 6-11 | 10,1 / 10,0 |
| R20 Muster | AK <= 0,25; Rest-Std <= 2,0 | 0,04; 1,77 / 0,15; 1,70 |
| R21-R23 Dicke | 0,15-0,20 / 0,07-0,10 / 0,028-0,045 H | 0,162 / 0,089 / 0,032 (h85), 0,155 / 0,074 / 0,036 (h92) |
| R24 Verlauf | kein Anstieg > 0,01 H | 0,000 / 0,000 |
| R25 Glaette | <= 36 Grad | 27,4 / 24,4 |
| R26 Knicke | <= 3 | 2 / 1 |
| R27 Austritt | 0,10-0,16 H | 0,145 / 0,146 |
| R28 Koerper | = Ref2 | Meshes 0-15, Skelett, Keyframes, EDD gleich; Mesh 8 +1 Viereck |
| R29 Dreiecke Schwanz | <= 212 | 160 |
| R30 Kopf | <= 118; <= 2 | 109; 2 |
| R31 Texeldichte | 0,045-0,090; Kopf 0,05-0,091 | 0,046-0,082; 0,066 |
| R32 Palette | 8 bpp, Haut-CLUT | 8 bpp, Zeile 482, C* 29,7 <= 63,3 |
| R25b/R26b Gang h84-h132 (Lauf) | Raum <= 25 Grad, <= 3 ueber 20 | 16,2; 0 |
| R25b/R26b Halten h16-h70 (Lauf) | dito | 16,2; 0 |

Gegenprobe V1 SPITZ (riegel_V1_gegenprobe.txt): 26 von 34 rot, darunter R3-R9 (Augen), R15/R16/R18/R19 (Schwanzfarbe), R25/R26 und
R25b/R26b (Knicke). ctest unit_birkin_formen, unit_birkin_szenen, unit_schwanz_r41, integration_r41_sichel_form: 4/4 bestanden.

### SV3r2.5 Leisten (analysis/befunde_runde41/SF_belege/v3/, Spalten KI | Original | Referenz 2 | V1 | V3)

01_seite_h83_h85_h92, 02_vorn_h118_h125, 04_gang_h84-h132 (Lauf V3r2), 05_kopf_nah (vorn h118/h125, Seite h85, Messansicht),
07_schwanz_nah, 08_rundum_V3, 09_farbe, neu 10_kopf_seite_spielmassstab (KI birkin12 | V3 r1 | V3 r2, h85 x8) und
11_kopf_vorn_spielmassstab (KI | V3 r1 | V3 r2, h118/h125 x3), Riegelbilder riegel_kopf_V3.png, messansicht_kopf_V3.png.

### SV3r2.6 Offen

1. Maul in der Spielkamera weiter dunkler als die KI: schwarze Flaeche 24,0 / 29,6 gegen KI 14,6 / 16,5 x 1e-4 H^2 (Runde 1 35,2 / 23,1;
   h125 groesser, weil die Lippe die Maulhoehe auf 0,050 H bringt). R10 verlangt eine Oeffnung mit L* < 0,5 x Fenster-Median
   >= 0,038 H; das KI-Maul (Lippe L* 19-20 ~ Kinn L* 15-17) erfuellt diese Messregel selbst nicht (Messblatt Abschnitt 9 Punkt 9).
   Ein KI-gleich helleres Inneres (Probe G/H: L* 16-18,6) machte R13/R14 rot. Entscheidung Messblatt-Pruefer/Nutzer.
2. Kopfauge seitlich nur 1 x 3 px (KI 3,7 x 2,6 px): der Port-Kopf hat keine Schnauze vor dem Auge, die Seitenflaechen um das Auge
   gehoeren zum Kopfbild vorn (n2_seite_mess.py); ein Augenprofil auf den Seitenflaechen waere von vorn ein zweites Auge.
   Groesser nur mit Geometrie (Kopf-Freigabe des Nutzers).
3. Kopf seitlich unten L*/Haut 0,65-0,75 (KI 0,90): schattige Unterseite im RDT-Licht; heller nur mit Leuchtfarben (Probe 1,6: Weiss).
4. Gesichtsfeld/Nachbarhaut 1,0 statt KI 0,87 (A3-Grenze "Nachbarhaut + 5 L*" erfuellt; KI-Wert erst mit HELL 0,75, dann R3/R4/R13 rot).
5. KI-Stirn und dunkles Band unter dem Auge nicht uebernommen (R1-Konflikt, SV3r2.2).
6. h83 (keine Riegel-Pose): 4 Knicke > 20 Grad im Bild bei max 25,7 Grad (KI h83 Port-Aufloesung 3 / 28,5 Grad).
7. schwanz_r41.c SW_GLATT_MIN 12 Grad und SW_GLATT_RAND 3 Grad weiter ohne eigene Messreihe (Zahlen-Pruefer 5); MAX 16 gegen 22 geprobt.
8. R31 Glied 0 0,046 (Grenze 0,045) bleibt knapp: Deckel ohne Texelflaeche; ohne Deckel R25 rot.
9. Ende Hand/Kralle (F2) weiter Nutzerwahl; PSX-Pruefung der Feder-Kette offen.


## V4 nach der Nutzer-Beschreibung (Runde 41, 2026-11-06/07, Messblatt V4, Bauer Runde 1)

### Nutzer-Wortlaut
"Da du offenbar nicht in der Lage bist das beim Original Material vom 2 Beiner Birkin richtig zu erkennen, habe ich dir das mit
Beschreibung1 und Beschreibung2 und Beschreibung1_Vorlage und Beschreibung2_Vorlage auch noch mal im Detail beschrieben. Ich will
EXAKT diese Umsetzung und auch EXAKT seine Farben." Vorlage sind ab jetzt die Original-Videobilder n1485 (vorn, h135) und n1435
(seitlich, h85) mit der Beschriftung, nicht mehr die KI-Fassung.

### Lesung (SF_MESSBLATT_V4.md, Pruefer)
- Vorn: Das Auge ist ein heller Fleck (46,9 / 7,2 / -1,1) 0,050 H (W)-rechts der Kopfmitte. (W)-links liegt ein dunkles Haarfeld
  (13,3 / 7,4 / 1,5), die Nase ist mitteldunkel. Die Kopfoberseite ist rot (22,6 / 15,4 / 5,2), das Kinn dunkel.
- Seitlich: Das Auge ist ein heller Fleck 0,049 H hinter der Kopf-Vorderkante. Vor ihm liegt ein dunkler Haarstreifen, oben ist der
  Kopf rot.
- Schwanz: Der Ansatz laeuft waagerecht nach hinten und biegt dann um 70-90 Grad nach unten. Am Knick sitzt die Taille. Nach oben
  zum Ruecken wird der Schwanz breiter, darunter folgt ein gleich breites Endstueck. Er ist zweitoenig: Wurzel und Unterseite
  rotbraun, aussen dunkelgrau.
- Farben im Videoraum. Die Vergleichsregel lautet: Port-Render, Gauss sigma 1,5, dann V = a + b x P.

### Aenderungen (Commit adfab2a13)
- Koerper = Referenz 2: Die Meshes 0-8 und 10-15, das Skelett 0-15, alle Keyframes 0-15 und das EDD sind byte-gleich (G1).
  Referenz 2 neu gebaut ergibt cfd1ac42, der Pin ist unveraendert. Das Spiel-Asset bleibt unveraendert.
- Schwanz (SCHWANZ_V4, birkin_formen.py): Die Gelenke 16-22 kommen direkt aus den Bildmassen der Vorlage-2-Kamera und werden auf die
  Mittelebene rueckprojiziert. Der Ruhewinkel betraegt an der Biegung (Gelenk 19) 60,0 Grad, an den uebrigen Gelenken 6-12 Grad.
  Die Durchmesser je Gelenk sind 0,16 / 0,15 / 0,153 / 0,128 / 0,0475 / 0,022 (Knick) / 0,038 / 0,038 H. Nachgestellt wurde mit
  form_suche_v4.py (6 Runden) und gitter_v4.py. Die Textur liegt in TPage 130 u 64-127 mit CLUT-Zeile 487, in vier Farbklassen
  (Wurzel, Aussenseite, Unterseite, Endstueck). Die Texeldichte ist 0,068 je E.
- Kopf (KOPF_V4): Mesh 9 ist skaliert (Tiefe 1,3, Hoehe 0,9, Breite 1,4). Die (W)-linke Gesichtshaelfte ist bis zum Mund um 200 E.
  nach vorn gebaut (Haare bilden seitlich die Vorderkante). Dazu kommt 1 Viereck als Haarstraehne an der Vorderkante (M2: 111
  Dreiecke). Die UVs sind je Flaeche die Projektion der naeheren Vorlage-Kamera. Das Kopfbild liegt in TPage 128 v 160-223 mit
  CLUT-Zeile 486.
- Ein Auge fuer beide Ansichten: Der Augenpunkt liegt auf der Kopfoberflaeche (Strahl aus der Vorderkamera bei K4 0,050 / 0,12 H)
  und ist in beiden Bildern als 3D-Ellipsoid gemalt. Ohne Vorbau lag die K4-Stelle seitlich an der Kante (S2 0,00-0,02 H,
  augenpunkt_v4.py), mit Vorbau liegt sie bei 0,046 H.
- Farben (eichen_v4.py, farben_v4.npz):
  1. Ein Code-Render liefert je Bildpunkt Texelzeile und Index.
  2. Der weisse Pruefkoerper (Texel 248) liefert das Licht lambda.
  3. Das Regionsmittel nach der Vergleichsregel ist linear in den Texelfarben.
  4. Kopf und Schwanz bekommen Zonenfarben, geloest mit kleinsten Quadraten gegen die Fenster des Messblatts. Die zusaetzlichen
     Fenster KV6/KV8/KV9 und KS3-KS6 sind an den Originalen gemessen (Protokoll eichung_v4_protokoll.json).
  5. Fuer den Koerper werden die Hautzeilen 480-482 mit Faktor R/G/B und Kontrastexponent gefittet, die Jeans mit eigenen Faktoren.
     Die Texel bleiben dabei unveraendert.
  6. Die Hautstruktur sind 16 Helligkeitsstufen, amp = 2 x Hochpass-Std der Originalfenster. In 4 Zonen ist sie halbiert, weil sie
     dort Nebenkomponenten ueber der K1/S1-Schwelle bildete.
- Engine (schwanz_r41.c): SW_GLATT_KNICK_AB = 30 Grad. Ein Gelenk mit Keyframe-Winkel ueber 30 Grad behaelt Winkel + 3 Grad. Fuer
  V3 und Referenz 2 aendert sich nichts. Dieselbe Regel gilt beim Nachschwingen (knick "ab").
- Posen (posen_v4.json): "85" = sfv2 "85". "135" ist neu an die Silhouette von n1485 angepasst (IoU 0,619). Diese Pose steht
  aufrechter und naeher als das Original; die Sohle liegt unter dem Bild, daher gilt H_port = 364 auf der verlaengerten Leinwand.
  Eine Anpassung mit festgehaltener Sohle erreichte nur IoU 0,560.

### Riegel-Tabelle (riegel_v4.py, EM30_V4.EMD sha256 0dddbdbe..., Lauf sfv4/r/V4; Exit 1)
| Mass | Soll | Ist | |
|---|---|---|---|
| G1 Koerper-Geometrie = Ref2 | Meshes 0-8, 10-15 byte-gleich; Skelett/Keyframes 0-15, EDD gleich | abweichend keine; Skelett True; Keyframes True; EDD True; uebrige Sektionen True; Ref2 sha256 cfd1ac42 | OK |
| M1 Dreiecke Schwanz | <= 212 | 160 ([30, 20, 20, 20, 20, 20, 30]) | OK |
| M2 Dreiecke Kopf | <= 118 | 111 | OK |
| M3 Farbtiefe | 8-bpp-TIM, Texel aus der CLUT | 8 bpp True; CLUT-Zeilen Kopf [486], Schwanz [487] (TIM 8 Zeilen) | OK |
| M4 Texeldichte | Kopf und Schwanz 0,045-0,091 Texel/E. | Kopf 0.062; Schwanz [0.049, 0.061, 0.063, 0.063, 0.065, 0.066, 0.06] | OK |
| K1 Augen vorn | 1 Komponente; Kontrast >= 20 (Original 29,5) | 1 Komp.; 31.6 | OK |
| K2 Augenfleck Farbe | 46,9/7,2/-1,1 dE00 <= 6; L* >= KV5 + 20 | 47.3/7.3/-1.4 dE00 0.5; L* 47.3 vs KV5 20.0 + 20 | OK |
| K3 Augenfleck Groesse | B 0,016-0,036 / H 0,023-0,043 H | 0.022 x 0.036 H (8 x 13 px) | OK |
| K4 Augenlage vorn | (B)-links 0,035-0,065 H; 0,07-0,12 H unter OK | 0.056 H links; 0.112 H unter OK | OK |
| K5 Haarfeld vorn | 13,3/7,4/1,5 dE00 <= 6; L* <= Auge - 25 | 13.2/7.2/1.7 dE00 0.3; L* 13.2 vs 22.3 | OK |
| K6 zweites Auge verdeckt | L* <= 18 (Original 12,9) | L* 11.2 (Box (370, 188, 377, 200)) | OK |
| K7 Nase | 18,2/7,1/1,3 dE00 <= 6; L* zwischen Haar und Auge | 18.3/6.3/1.0 dE00 0.9 | OK |
| K8 Kinn/Maulzone | 9,9/9,9/3,8 dE00 <= 6; Anteil L*>35 <= 0,05 | 11.9/13.2/4.3 dE00 3.1; Anteil 0.000 | OK |
| K9 Kopf-Oberseite rot vorn | 22,6/15,4/5,2 dE00 <= 6; a* >= 11 | 22.9/15.4/5.4 dE00 0.2 | OK |
| K10 Kopfbox vorn | 0,14-0,18 x 0,15-0,20 H | 0.143 x 0.187 H | OK |
| S1 Auge seitlich | 48,4/4,0/0,5 dE00 <= 6; 0,028-0,052 x 0,040-0,070 H | 47.5/3.6/0.5 dE00 1.0; 0.023 x 0.030 H; 5 Komp. | ROT |
| S2 Augenlage seitlich | 0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle | 0.046 H; 0.818 H | OK |
| S3 Haarkante seitlich | L* <= 22; 19,1/1,2/1,1 dE00 <= 6; Abstand zum Fleck <= 0,01 H | 19.5/0.9/1.6 dE00 0.8; Abstand 0.008 H | OK |
| S4 Kopf-Oberseite rot seitlich | 27,6/15,3/3,3 dE00 <= 6; a* >= 11 | 27.5/14.3/2.9 dE00 0.8 | OK |
| T1 Schwanz Beginn | 0,15 H hinter Scheitel, 0,767 H ueber Sohle (+-0,03) | 0.214 H hinter, 0.727 H ueber (unterster Schwanz-Pixel an Id 7/8 (Id 16 versenkt)) | ROT |
| T2 Biegung | dx -0,245 dy +0,036 (+-0,03); Drehung 60-100 Grad auf 0,10 H | dx -0.244 dy 0.025; 68 Grad | OK |
| T3 Knick (Taille) | dx -0,265 dy +0,257 (+-0,03) | dx -0.263 dy 0.255 | OK |
| T4 Ende | dx -0,245 dy +0,397 (+-0,03); 0,37 H ueber Sohle | dx -0.241 dy 0.398; 0.330 H ueber Sohle | OK |
| T5 Breite nach oben | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01 | 0.034/0.066/0.123/0.140 H; Abfall 0.002 | OK |
| T6 Endstueck | B 0,025-0,05; L 0,12-0,17 H; 75-95 Grad; Taille <= 0,035 und schmalste | B 0.028; L 0.144 H; 81 Grad (vorn); Taille 0.021, schmalste davor 0.021 | OK |
| T8 Schwanzfarbe | F15 9,3/11,4/1,5; F16 8,7/8,4/2,2; F17 26,8/-0,1/2,4; F18 15,2/3,2/1,6; dE00 <= 6; a* Wurzel >= 7, Ende <= 6 | F15 9.3/8.5/3.2 dE00 3.0 (146 px); F16 9.2/8.7/3.2 dE00 0.9 (131 px); F17 26.6/-0.0/1.9 dE00 0.4 (2032 px); F18 15.5/3.4/1.9 dE00 0.3 (218 px) | OK |
| T7 Gang h85 | T1-T4 je +-0,06 H | T1 (0.116, 0.659) (0.108) T2 (-0.244, 0.447) (0.411) T3 (-0.238, 0.476) (0.219) T4 (-0.221, 0.484) (0.087) (Laufpose clip 11 bild 64 gier 1893) | ROT |
| T7 Gang h100 | T1-T4 je +-0,06 H | T1 (0.153, 0.617) (0.150) T2 (-0.316, 0.025) (0.071) T3 (-0.345, 0.033) (0.224) T4 (-0.341, 0.033) (0.364) (Laufpose clip 1 bild 10 gier 1515) | ROT |
| T7 Gang h110 | T1-T4 je +-0,06 H | T1 (0.206, 0.723) (0.056) T2 (-0.372, 0.077) (0.127) T3 (-0.378, 0.163) (0.113) T4 (-0.305, 0.288) (0.109) (Laufpose clip 1 bild 20 gier 1515) | ROT |
| T7 Gang h118 | T1-T4 je +-0,06 H | T1 (0.218, 0.735) (0.068) T2 (-0.231, 0.039) (0.014) T3 (-0.236, 0.275) (0.029) T4 (-0.253, 0.413) (0.016) (Laufpose clip 1 bild 28 gier 1515) | ROT |
| T7 Gang h125 | T1-T4 je +-0,06 H | T1 (0.214, 0.727) (0.064) T2 (-0.255, -0.017) (0.053) T3 (-0.295, 0.207) (0.050) T4 (-0.267, 0.311) (0.086) (Laufpose clip 1 bild 35 gier 1515) | ROT |
| T7 Gang h135 | T1-T4 je +-0,06 H | T1 (0.214, 0.723) (0.064) T2 (-0.232, 0.047) (0.013) T3 (-0.263, 0.293) (0.036) T4 (-0.284, 0.393) (0.039) (Laufpose clip 1 bild 45 gier 1513) | ROT |
| F0 Lichtgegenprobe h118 | Rumpf b2 gegen FRAMEDUMP dE00 <= 3 | b2 22.6/11.9/4.0 / Spiel 22.5/12.0/4.1 dE00 0.1 (11424 px) | OK |
| F0 Lichtgegenprobe h135 | Rumpf b2 gegen FRAMEDUMP dE00 <= 3 | b2 17.9/13.1/5.0 / Spiel 17.9/12.9/4.9 dE00 0.1 (12873 px) | OK |
| F F6 | 16.6/17.3/6.2 dE00 <= 5, |dL| <= 4 | 16.1/15.2/6.1 dE00 1.5 dL -0.5 (8698 px) | OK |
| F F7 | 33.3/5.5/2.7 dE00 <= 5, |dL| <= 4 | 36.1/8.7/1.5 dE00 4.2 dL +2.8 (2181 px) | OK |
| F F8 | 16.4/8.3/1.5 dE00 <= 5, |dL| <= 4 | 40.6/6.4/0.4 dE00 18.5 dL +24.2 (3293 px) | ROT |
| F F9 | 12.7/-0.9/-1.5 dE00 <= 5, |dL| <= 4 | 14.8/0.3/-2.4 dE00 2.4 dL +2.1 (4090 px) | OK |
| F F10 | 11.2/14.0/2.5 dE00 <= 5, |dL| <= 4 | 11.0/13.5/1.4 dE00 0.8 dL -0.2 (677 px) (Id 12 (Klaue (B)-rechts); Id 15: nan/nan/nan (0 px)) | OK |
| F F14 | 60.0/11.0/1.2 dE00 <= 5, |dL| <= 4 | 56.3/8.0/0.8 dE00 4.3 dL -3.7 (1256 px) | OK |
| F F19 | 18.4/0.5/-3.5 dE00 <= 5, |dL| <= 4 | 15.6/-0.0/-2.1 dE00 2.4 dL -2.8 (1341 px) | OK |
| F Mittel | Mittel dE00 <= 4 | 4.87 | ROT |
| F0 Gueltigkeit Farbvergleich | Wand links 64,7/0,3/-2,1 dE00 <= 2 | 65.2/0.2/-2.5 dE00 0.52 | OK |

Gegenprobe V3 r2 (riegel_V3r2_gegenprobe.md, 28 von 35 rot): K4 -0,035 H (falsche Seite), S2 0,086 / 0,906 H, T2 34 Grad,
T3/T4 0,392 / 0,483 H, T6 0,091 H, T8 dE00 8,9-9,1, F6 9,6, F9 10,0, F14 18,7.

Tests: unit_birkin_formen, unit_birkin_szenen, unit_schwanz_r41 und integration_r41_sichel_form laufen 4/4 gruen (64,6 s,
SF_belege/v4/ctest_v4.txt).

### Leisten (SF_belege/v4/)
01_vorn_kamera12, 02_seite_wie_bild2, 03_kopf_nah_vorn_x4, 04_kopf_nah_seite_x4, 05_schwanz_x3, 06_farben, 07_gang,
08_rundum_V4. Spalten: Nutzer-Vorlage | Nutzer-Beschriftung | V3 r2 | V4. Dazu kommen kopf_nah_vorn/seite_x6_eichung.png
(Original | V4 mit den Fenstern).

### Offen
1. T1 (Beginn 0,214 / 0,727 H statt 0,15 / 0,767): Der Schwanz wird sichtbar an der Rueckenkontur der Figur Referenz 2. Diese liegt
   in Pose 85 auf 0,77 H Hoehe 0,21 H hinter dem Scheitel. Getestet wurde, die Wurzel bis 0,49 H nach vorn und 0-0,20 H zur Kamera
   zu legen; der Wert blieb bei 0,203-0,225. Abhilfe waere nur ueber den Koerper (G1, nicht erlaubt). Deshalb sind T2-T6 auf den
   gemessenen Beginn bezogen.
2. S1: Das Auge seitlich hat die richtige Farbe (dE00 1,0) und Lage (S2 gruen), zerfaellt aber in 5 Komponenten (0,023 x 0,030 H).
   Ursache: Es sitzt an der Kante zwischen Vorder- und Seitenbild, dazu kommt die Straehne. Pruefbild kopf_nah_seite_x6_eichung.png.
3. F8 (Oberarm (W)-links 40,6 statt 16,4): Beide Oberarme (Mesh 10 und 13) nutzen dieselbe Zeile 480 / Seite 128. Das Original ist
   links 17 L* dunkler (Licht im Video), das Port-Licht nicht. Ueber die CLUT allein ist das nicht trennbar; dazu waeren eigene Texel
   bzw. eine eigene CLUT-Zeile fuer Mesh 10 noetig, und die UVs sind G1. Das F-Mittel betraegt 4,87, ohne F8 2,6.
4. T7: Der Spiellauf zeigt h84-h135 den Gang auf die Kamera zu; h85 ist dort Clip 11 (Halten). Gemessen wurde mit dem Feder-Stand
   des Laufs an Pose 85. Fuer h118 und h135 liegt T2-T4 bei <= 0,039 H (gruen bis auf T1). h100-h125 weichen ab (0,05-0,36 H), weil
   die Szene den Schwanz zur Seite schwingt (Seitenvorgabe Spur H) und das Nachschwingen nach der Freigabe wirkt.
5. Das Schulterauge (Messblatt 8: "Stand V3 r2 bleibt") fehlt in V4. Es war ein Viereck auf Mesh 8, und G1 verlangt Mesh 8
   byte-gleich. Offene Frage an den Pruefer/Nutzer: Schulterauge als Texel auf Mesh 8 (geteilte Texel?) oder weglassen?
6. Koerperbild: Brust und Bauch sind nach der Eichung dunkelrot bzw. sehr dunkel. Das entspricht den Originalfenstern (Brust ohne
   Maske 16,9 / 15,8 / 5,8, Bauch 9,4 / 1,3 / 2,3), die Hautzeichnung von Referenz 2 wirkt dadurch aber flaecher. Die Jeans sind
   dunkel (F9 14,8 L*); A1 entscheidet am Bild.
7. Einbau ins Spiel-Asset erst nach der Nutzer-Abnahme. Bis dahin liegt die EMD im Scratchpad sfv4/emd/EM30_V4.EMD und ist
   reproduzierbar mit "birkin_formen.py --variante v4 --aus DIR".

### V4 Runde 2 (Bauer, 2026-11-07, nach Pruefung V4 r1)

Nutzer-Wortlaut und Lesung wie oben: "Ich will EXAKT diese Umsetzung und auch EXAKT seine Farben." Grundlage ist SF_MESSBLATT_V4.md,
dazu die beiden Pruefberichte SF_PRUEFUNG_V4_r1_zahlen.md und SF_PRUEFUNG_V4_r1_sicht.md (Commit 49f4c00f6). Kein RE-Befund: die Form
gibt es auf der Disc nicht, keine Zahl ist eine @0x-Konstante. Jede Konstante nennt im Generator bzw. im Werkzeug ihre Messblatt-Zeile
oder ist als PORT-WAHL mit dem Lauf gekennzeichnet, der sie festgelegt hat.

**Befunde Runde 1 und was geaendert ist** (Commits 5a89f427e, 28e928138, 3e603a4c0, 951fde6ed, 2b50e6414)

| Befund (Pruefung V4 r1) | Aenderung | Ergebnis (riegel_V4r2) |
|---|---|---|
| glaetten() lief fuer V4 nie (Bindversatz -588 >= -700): 73/144 Takte mit einem Gelenk > 90 Grad. Die Ausnahme hob bei V3 die 16-Grad-Grenze auf | schwanz_r41.c: V4-Kette = Gliedlaengen max/min > 2,0 (Ref2 1,00, V3 1,00, V2 1,15, V1 1,49, V4 2,4). Fuer sie laeuft glaetten() immer, nur fuer sie gilt die Biegungs-Ausnahme. Untergrenze der Biegung: Keyframe - 10 Grad (SW_BIEGUNG_SPIEL) | Lauf sfv4/r/V4r2: 0/144 Takte > 90 Grad, Biegung 44,4-67,7 Grad, uebrige Gelenke <= 16,3 Grad. Unit-Test Teil 9: V4-Gliederung 13,3 Grad; Gegenprobe mit gleich langen Gliedern 134,7 Grad |
| T1 rot (0,218 / 0,727 H), Schwanz 11-12 px hinter den Nutzerpunkten | beginn (0,15, 0,767) wie im Messblatt. Der Ansatz liegt 0,12 H zur Kamera hin vor der Flanke ('tiefe'). Gelenke auf die Mitte aus Messblatt-Mass und Nutzer-Linienende nachgestellt (r2_form_suche.py). T1 misst jetzt wie das Messblatt an Id 16 | T1 0,165 / 0,750 H; TA: Beginn +0,011/-0,027, Knick -0,001/-0,029, Ende +0,003/-0,012 H |
| T2-Drehung 54 Grad (Pruefer-Mass), Original 66-70 | Biegung geschaerft: Gelenk 2 flacher, Gelenk 3 nach hinten oben, Gelenk 4 senkrecht darunter. Der Riegel misst nach dem Pruefer-Verfahren (Tangenten +-0,025 H) | 67,3 Grad |
| T4: Sohlenhoehe nicht geprueft (0,336 H) | Riegel prueft 0,37 +-0,03 H | 0,357 H |
| Knick-Durchmesser 0,022 lag unter der gemessenen Taille | 0,027 H (Mitte von 0,024-0,032) | Taille 0,025 H, schmalste Stelle |
| Kopf seitlich: Keil-Vorbau mit Spaltenhoehe 18-21 px, schwarzer Lappen (Straehne), Auge 0,023 x 0,030 H in 5 Teilen | Haarvorhang mit senkrechter Kante im Bild der Vorlage-2-Kamera (Ebene 0,06 H vor dem Augenpunkt), keine Straehne, Breite x1,55. Auge seitlich als Ellipse an der S2-Stelle, Lichtausgleich je Texel. Vorhangtexel im Vorderbild in der seitlichen Haarzone | Spaltenhoehe an der Kante 31-36 px; S1 0,034 x 0,049 H in 1 Komponente; S2 0,058 / 0,811 H; K10 0,154 x 0,187 H |
| Schwanz oben hellgrau (Achse dE00 8,6-12,4) | 6 Farbklassen: Wurzel, Aussenkante (c > 0,5 der Aussennormale der Bogenlinie), Mitte, senkrecht, Unterseite, Endstueck. Die 7 Achsenpunkte sind Regionen der Eichung; neuer Riegel T9 | T9 Mittel dE00 2,9 (L*/a* 11,3/6,3 gegen 11,6/6,1); T8 F17 26,6 (Aussenkante, 971 px) |
| Koerper: Rumpf wie ein rotes Hemd (Rotanteil 0,83), Oberarme doppelt so hell, Unterarme rot; F8 rot | r2_koerper_farben.py: Texel je (Texel, CLUT-Zeile) per kleinsten Quadraten aus einem Histogramm-Angleich je Region an die Original-Fenster (Messblatt F6-F10/F14/F19, Sicht-Boxen). Getrennt: Rumpf rot/nicht rot, Oberarme oben/unten nach Fensterlage, Hals (K8), heller Buckel (F14). 256 Indizes per k-Mittel | FS Rumpf+Schultern 17,9/6,7/2,8 mit Rotanteil 0,36 (Original 17,3 / 0,31); Unterarme 28,5/1,7/-1,5 mit Rotanteil 0,00; F6-F10, F19 und F-Mittel 3,1 gruen |
| Konstanten ohne Quelle (vorbau, schwelle, Bogenmitte, Tiefe 1,3) | Alle im Generator mit Quelle bzw. Lauf: vorbau neu, schwelle = cos 87 Grad, Bogenmitte = Kruemmungsmitte, Tiefe = groesste mit K10-Hoehe <= 0,20 | - |

**Riegel-Tabelle** (riegel_v4.py, EM30_V4.EMD sha256 174b7879, Lauf sfv4/r/V4r2; volle Tabelle in SF_belege/v4/riegel_V4r2.md; Exit 1)

| Mass | Soll | Ist | |
|---|---|---|---|
| G1 Koerper = Ref2 | Meshes 0-8, 10-15 byte-gleich, Skelett/KF 0-15, EDD | abweichend keine, alle True, Ref2 cfd1ac42 | OK |
| M1 / M2 / M3 / M4 | <= 212 / <= 118 / 8 bpp / 0,045-0,091 | 160 / 109 / 8 bpp, Zeilen 486/487 / Kopf 0,054, Schwanz 0,051-0,066 | OK |
| K1 Augen vorn | 1 Komponente, Kontrast >= 20 | 1; 20,3 | OK |
| K2 Augenfleck Farbe | 46,9/7,2/-1,1, dE00 <= 6 | 46,7/7,5/-0,6, dE00 0,5 | OK |
| K3 Augenfleck Groesse | 0,016-0,036 x 0,023-0,043 H | 0,030 x 0,036 | OK |
| K4 Augenlage vorn | 0,035-0,065 links; 0,07-0,12 unter OK | 0,059; 0,107 | OK |
| K5 / K6 / K7 / K8 / K9 | Haarfeld / Spiegelstelle / Nase / Kinn / Kuppe | dE00 0,8 / L* 9,0 / dE00 1,4 / dE00 3,3 mit Anteil 0 / dE00 1,1 | OK |
| K10 Kopfbox vorn | 0,14-0,18 x 0,15-0,20 H | 0,154 x 0,187 | OK |
| S1 Auge seitlich | 1 Komponente; 48,4/4,0/0,5; 0,028-0,052 x 0,040-0,070 H | 1; 47,8/3,8/0,9, dE00 0,8; 0,034 x 0,049 | OK |
| S2 Augenlage seitlich | 0,03-0,07; 0,76-0,82 H | 0,058; 0,811 | OK |
| S3 Haarkante seitlich | L* <= 22, dE00 <= 6, Abstand <= 0,01 H | 13,8; dE00 3,7; **Abstand 0,011 H** | ROT |
| S4 Kuppe seitlich | 27,6/15,3/3,3, dE00 <= 6 | 27,3/14,8/3,1, dE00 0,4 | OK |
| T1 Beginn | 0,15 / 0,767 H +-0,03 | 0,165 / 0,750 | OK |
| T2 Biegung | -0,245 / +0,036; 60-100 Grad | -0,252 / 0,022; 67,3 | OK |
| T3 Knick | -0,265 / +0,257 | -0,266 / 0,244 | OK |
| T4 Ende | -0,245 / +0,397; 0,37 H ueber der Sohle | -0,243 / 0,393; 0,357 | OK |
| TA absolut | je Achse +-0,03 H | hoechstens 0,029 H | OK |
| T5 Breite nach oben | 0,03 / 0,065 / 0,125 / 0,14 | 0,036 / 0,066 / 0,127 / 0,129, Abfall 0,008 | OK |
| T6 Endstueck | B 0,025-0,05; L 0,12-0,17; 75-95 Grad | 0,034; 0,152; 81 Grad; Taille 0,025 | OK |
| T8 Schwanzfarbe | F15-F18 dE00 <= 6 | 1,0 / 1,1 / 1,3 / 1,2 | OK |
| T9 Schwanzachse | Mittel dE00 <= 6, L*/a* +-4/+-3 | 2,9; 11,3/6,3 | OK |
| T7 Gang h118 / h135 | +-0,06 H | hoechstens 0,019 / 0,025 | OK |
| T7 Gang h85 / h100 / h110 / h125 | +-0,06 H | **0,294 / 0,436 / 0,255 / 0,088** | ROT |
| F0 Licht h118 / h135 | dE00 <= 3 | 0,1 / 0,3 | OK |
| F6 / F7 / F8 / F9 / F10 | dE00 <= 5, \|dL\| <= 4 | 3,3 / 3,9 / 3,0 / 2,1 / 2,7 | OK |
| F14 Buckel | 60,0/11,0/1,2, \|dL\| <= 4 | 54,8/10,6/2,9, dE00 5,0, **dL -5,2** | ROT |
| F19 / F-Mittel | dE00 <= 5 / <= 4 | 1,7 / 3,11 | OK |
| FS Oberarme unten, Rumpf, Unterarme, Unterschenkel | Sicht-Boxen: dE00 <= 5, \|dL\| <= 4, Rotanteil +-0,15 | dE00 2,8 / 1,5 / 2,2 / 1,5 / 4,4 | OK |
| F0 Wand | dE00 <= 2 | 0,52 | OK |

Gegenprobe V3 r2 (riegel_V3r2_gegenprobe_r2.*): 36 von 44 Riegeln rot. Darunter sind K4 (-0,035 H, falsche Seite) und S2 (0,086 / 0,906 H),
T1-T6 und TA (Schwanzverlauf) sowie T8, T9, F6-F19 und FS (Farben). Tests: unit_birkin_formen, unit_birkin_szenen, unit_schwanz_r41 und
integration_r41_sichel_form laufen 4/4 Passed (SF_belege/v4/ctest_v4_r2.txt). Spiel-Asset EM30_HANGAR.EMD und Pin cfd1ac42 sind
unveraendert. Die Varianten-EMD sfv4/emd/EM30_V4.EMD (sha256 174b7879) ist reproduzierbar mit "birkin_formen.py --variante v4 --aus DIR".

**Leisten** (SF_belege/v4/, Spalten Nutzer-Vorlage | Nutzer-Beschriftung | V3 r2 | V4 neu): 01_vorn_kamera12, 02_seite_wie_bild2,
03_kopf_nah_vorn_x4 und 04_kopf_nah_seite_x4 (je Zeile 1 ohne, Zeile 2 mit Messpunkten), 05_schwanz_x3 (Polygonzug der Vorlage),
06_farben (mit den Sicht-Boxen), 07_gang (Lauf sfv4/r/V4r2), 08_rundum_V4.

**Offen**
1. S3: Der helle Teil des Seitenauges endet an der Naht zu den Vorderbild-Flaechen; dort liegt das Licht bei 0,24-0,9. Das Fenster KS2
   (0,84-0,96 der Kopfbox) beginnt 3 px dahinter: 0,011 H statt <= 0,010. Versucht wurden: Augenmitte 0,042-0,049 H, Malgroesse 1,25-2,0,
   Vorhang z0/z1 und vor 0,05-0,06, Vorderbild-Texel bis 360 E. als Auge sowie das Seitenbild fuer Flaechen am Auge. Letzteres ist verworfen,
   weil K1-S3 dann rot wurden. Laeufe r2 Eichung 7-20.
2. F14 (Buckel hell 54,8 statt 60, dL -5,2): Die Buckel-Texel sind dieselben wie die der Schultern vorn. Mit der Helligkeitsdecke Lum 200,
   215 oder ganz ohne laege dL bei -4,6, -4,4 bzw. -4,2. Dann zeigt die (B)-linke Schulter vorn aber zwei weisse Texel wie ein Augenpaar.
   Gewaehlt ist Lum 190 ohne diese Punkte. Entscheid durch den Nutzer: Schulterpunkte oder Buckel 1 L* dunkler.
3. F7/F8 auf die ganzen Ids 13/10 (Runde-1-Mass) liegen weiter auseinander: 22,4 statt 33,3 bzw. 20,9 statt 16,4. Die Oberarme teilen
   5798 Texel und die CLUT-Zeile 480, und UVs sowie CLUT-Feld sind G1. Das hellere Licht des Originals auf dem (B)-linken Arm laesst sich so
   nicht nachbilden. Gemessen wird deshalb an der Lage der Messblatt-Fenster im Oberarm (oben 58 / 56 %): F7 3,9 und F8 3,0 sind gruen,
   unten (Sicht-Boxen) ebenfalls. Abhilfe waere eine eigene UV/CLUT-Zeile fuer Mesh 10; dafuer muesste G1 geaendert werden (Nutzerentscheid).
4. T7: h118 und h135 liegen innerhalb +-0,06 H. h85 gehoert zu Clip 11 (Halten, Gier 1893), bei h90 dreht die Szene auf Gier 1515.
   Danach haengt die Kette dem Rumpf nach und steht bei h100 waagerecht nach hinten (Spitze 2057 E. ueber dem Boden); h110 und h125
   schwingen aus. Die Biegung bleibt dabei erhalten (Gelenk 3 >= 44 Grad). Den Nachlauf zu daempfen hiesse, die Feder (k_feder,
   SW_ERHALT) fuer alle Formen zu aendern. Das gehoert zum Szenenablauf G2 und ist nicht angefasst.
5. Kopf im Spielbild: Die Spielpose h135 (Clip 1 Bild 45, Gier 1513) weicht von der angepassten Pose "135" (Clip 0 Bild 9, Gier 1737)
   ab. Dort wirkt der Kopf mit den Zonenfarben flaechig (Leiste 07). Die K-Riegel gelten fuer Pose "135" (Pruefung V4 r1 4.3).
6. Schwanzansatz vor der Flanke: Damit der Schwanz an den Nutzerpunkten aus dem Ruecken tritt, liegt der Ansatz 0,12 H zur Kamera hin.
   Der Koerper Ref2 (G1) reicht 0,06 H weiter nach hinten als das Video. Aus anderen Blickwinkeln (08_rundum) steht der Ansatz deshalb
   seitlich etwas vom Buckel ab.
7. Die Sichel (B)-links (Id 15) behaelt die Farben von Referenz 2; im Original ist sie nicht abgrenzbar (Messblatt 9.3).
8. Die Hautfaktoren in eichen_v4.py ('haut') wirken nur noch auf die Zeilen 483/485. Die Zeilen 480/481/482/484 und die Koerpertexel kommen
   aus farben_koerper_v4.npz.
9. Einbau ins Spiel-Asset erst nach der Nutzer-Abnahme.

### V4 Runde 3 (Bauer, 2026-11-07, nach Pruefung V4 r2)

Nutzer-Wortlaut wie oben ("Ich will EXAKT diese Umsetzung und auch EXAKT seine Farben"). Grundlage: SF_MESSBLATT_V4.md (Commit
7a4f3b684), die Pruefberichte SF_PRUEFUNG_V4_r2_zahlen.md und SF_PRUEFUNG_V4_r2_sicht.md (Commit eadf288b4) und die Entscheide O1-O6
des Orchestrators. Kein RE-Befund: die Sichel-Form gibt es auf der Disc nicht, keine Zahl ist eine @0x-Konstante. Jede Konstante nennt im
Code ihre Quelle (Messblatt-Zeile, Original-Messung) oder ist als PORT-WAHL mit den Laeufen gekennzeichnet, die sie festgelegt haben.
Commits e2ad70c50 (Messwerkzeuge), 0669ae3dd (Engine), d64c021ac (Generator/Farben), a6baa4eed (Riegel, Leisten, Belege).

**Befund -> Aenderung -> Beleg**

| Befund (Pruefung V4 r2) | Aenderung (Entscheid) | Ergebnis (riegel_V4r3, Lauf sfv4/r/V4r3) |
|---|---|---|
| Sicht A: Kopf vorn glatte Kuppel, Gradient Kopf 27,8 = 67 % des Originals, Kopf/Rumpf 0,86 | (O4) eichen_v4.py struktur_original: Hautstruktur aus dem ORIGINAL - je Texel die relative Farbe von Vorlage 1/2 (Pixel / Gauss sigma 3) an derselben Anteilslage der Original-Kopfbox; Helligkeit x 10 (Klammer -2,0/+0,15), Farbton x 1,5 (+-0,4), 16 RGB-Stufen (k-Mittel); Augenhof 2-4 Augenradien ohne Struktur und dunkler (vorn 0,5, seitlich 0,6); helle Stufen aus in v_basis, v_oben_links, v_nase, s_basis, s_haar, s_hinten_unten; Auge vorn radial 0,55 | Neuer Riegel K11: Ziel aus dem Original 41,0 +-6,1 (9 um +-3 px verschobene Kopfboxen 39,4-42,9; Kopfviertel 34,9-47,0; Toleranz = groesser von Spanne der Boxen 3,5 und halber Viertel-Spanne 6,1), Kopf >= Rumpf (Original 1,57 x). V4 r2 27,7 (ROT) -> **36,1, Kopf/Rumpf 1,10 (OK)**. K1-K10, S1, S2, S4 bleiben gruen (K1 Kontrast 20,4) |
| Sicht A: Unterarm (B)-links = (W)-rechts weiss gefleckt, 28,1 L*, 35 % L* > 30 | (O1/O5) eigene CLUT-Zeile 488 fuer Mesh 14 (birkin_formen.py KOERPER_ZEILEN_V4, nur das CLUT-Feld); r2_koerper_farben.py Region unterarm_wr mit der Box der Sicht-Pruefung 298-325 x 262-340 | Neue Zeile FS Unterarm Id 14: Ziel 9,0/-3,8/3,5, Anteil L* > 30 <= Original + 0,05. V4 r2 28,2 L*, Anteil 0,337 (ROT) -> **11,4 L*, Anteil 0,000, dE00 2,9 (OK)** |
| Zahlen: T2-Drehung 52,5 Grad mit der Pruefer-Mittellinie (Original 66-70); Bauer-Mittellinie am Original nicht stabil | (O2) EIN Werkzeug t2_mass_v4.py (Pruefer-Mittellinie, Original-Masken in v4/t2_masken): Ziel 68,4, Toleranz max(Spanne 4,3; 5) = 5 Grad. Schwanzgelenke 2-4, Durchmesser 2-4, Tiefe 2-3 per r3_form_suche.py (Saat 2, Schritt 78, T1-T6/TA mit Marge 0,003 H, Protokoll SF_belege/v4/r3/) | T2 V4 r2 52,5 (ROT) -> **70,6 Grad (OK)**; der Ansatz faellt wie im Original schraeg zur Biegung (Original-Mittellinie 20-27 Grad vor der Biegung). T1 0,172/0,742, T3 -0,248/0,265, T4 -0,216/0,390 (0,353 H ueber der Sohle), TA <= 0,022 H, T5 0,028/0,080/0,140/0,131, T6 B 0,036 L 0,129 76 Grad Taille 0,021 - alle OK |
| Zahlen: T7 h85-h125 rot, h100 Kette waagerecht, 18/46 Gangtakte > 0,06 H; Figur bei h85 in clip 11 | (O3) T7 koerperfest (t7_koerperfest_v4.py): Kette gegen ihr Feder-Ziel = Keyframe-Gelenke der gezeichneten Pose (neue Messzeile "Schwanz Ziel"). Ursache h100: das Szenenziel zog bis h102 die Spitze auf den verdeckten Punkt hinter dem Koerper -> V4 frei ab h70-h74 (k_schwanz_gewicht_v4; Vorlage 2 = h85 zeigt die Ruheform). V4-Kette ohne haengen() (es zog die am Knochen 8 nach hinten oben getragene Ruheform auf die Waagerechte), Mitnahme 0,80, Feder x1,5, Daempfung 0,50 (Messreihe Laeufe V4r3c-g im Kommentar von schwanz_r41.c) | T7a V4 r2 38/47 Takte > 0,06 H, max 0,442 H (ROT) -> **0/47, max 0,042 H (h85)**; T7b Biegung 51,7-59,2 Grad (Keyframe 51,7-57,4), Knick 10,9-16,1; T7c Strecke Biegung -> Ende min 0,387 H (Keyframe 0,385; V4 r2 -0,090 = Kette zurueckgeklappt); T7d max 60,9 Grad - alle OK |
| Zahlen: Dornspitze h33-h35 bis 57 E. unter dem Boden; glaetten() prueft nur Kettenpunkte | (O3) schwanz_r41.c: Bodenpruefung der V4-Kette mit dem Radius des Glied-Meshes je Gelenk und der Dornspitze von Mesh 22 (aus der geladenen Bank gemessen) | Boden V4 r2 +76 E. (h34, ROT) -> **tiefster Mesh-Punkt -4 E. (h68), Dornspitze -43 E., 0 Takte darunter (OK)** |
| Zahlen: F7/F8 auf "obere 58/56 %" umgedeutet, Messblatt-Lesart fest OK; Mesh 10/13 teilen 5797 Texel und CLUT 480 | (O1) eigene CLUT-Zeile 489 fuer Mesh 10; Id 10 ganz mit der F8-Verteilung. Die Messblatt-Lesart (Id ganz) ist jetzt eine echt bewertete Riegelzeile | F8 Wortlaut V4 r2 20,9 dL +4,5 (ROT) -> **19,6 dL +3,2 (OK)**; F7 Wortlaut 22,7 dL -10,6 **ROT** (Offen 2) |
| Zahlen: F14 dL -5,2 (Helligkeitsdecke) | Neue Koerperfarben (die Zeilen 488/489 nehmen die Arme aus den geteilten Texeln) | F14 **57,0, dL -3,0 (OK)**; F-Mittel 2,65 |
| Zahlen: S3 Abstand 0,015 H | versucht: Vorderkante der gemalten Seitenellipse 0,006 / 0,010 H nach vorn (eichen_v4.py AUGE_S_VOR, Laeufe r3 S3 1/2): der gemessene Fleck rueckt nicht vor (Naht zu den Vorderbild-Flaechen), K1 fiel auf 16,7-18,7 -> zurueckgenommen (AUGE_S_VOR 0) | S3 **ROT** (Offen 1) |
| Konstanten: S2_MITTE 0,045 gegen zitierte 0,049; 'tiefe' ohne PORT-WAHL | Kommentar: gemalt 0,045 -> gemessen S2 0,058 H; mit 0,049 laege die Mitte bei ~0,062 und S3 wuerde groesser. 'tiefe' als PORT-WAHL mit Quelle | - |

**Riegel-Tabelle** (riegel_v4.py, EM30_V4.EMD = EM30_V4r3.EMD sha256 dfbc30bc, Lauf sfv4/r/V4r3; volle Tabelle SF_belege/v4/riegel_V4r3.md/.txt/.json; **Exit 1, 3 rot von 53**)

| Mass | Soll | Ist V4 r2 (neue Riegel) | Ist V4 r3 | |
|---|---|---|---|---|
| G1 Koerper = Ref2 (O1) | Ecken, Normalen, Flaechen, UV der Meshes 0-8/10-15 gleich; Skelett/KF/EDD gleich; CLUT-Feld frei | keine Abweichung | Geometrie gleich; CLUT Mesh 10 480->489, Mesh 14 482->488; 13/15 byte-gleich | OK |
| M1-M4 | <= 212 / <= 118 / 8 bpp / 0,045-0,091 | 160 / 109 / ok / ok | 160 / 109 / Zeilen 480-482, 484, 486-489 (TIM 10 Zeilen) / Kopf 0,054, Schwanz 0,051-0,066 | OK |
| K1 | 1 Komponente, Kontrast >= 20 | 1; 20,3 | 1; 20,4 | OK |
| K2-K10 | Messblatt V4 Abschnitt 7 | OK | K2 dE00 0,7; K3 0,030 x 0,036; K4 0,060 / 0,107; K5 0,7; K6 L* 12,7; K7 1,4; K8 3,3 Anteil 0; K9 1,6; K10 0,154 x 0,187 | OK |
| K11 Kopf-Struktur vorn (O4) | Gradient 41,0 +-6,1; Kopf >= Rumpf | 27,7 / 32,9 (0,84) | 36,1 / 32,7 (1,10) | OK |
| S1 / S2 / S4 | Messblatt | OK | 48,0/3,9/0,8 0,030 x 0,049 1 Komp. / 0,058; 0,811 / dE00 0,3 | OK |
| S3 | L* <= 22, dE00 <= 6, Abstand <= 0,01 H | Abstand 0,011 | 14,1, dE00 3,7, **Abstand 0,015 H** | ROT |
| T1 / T3 / T4 / TA / T5 / T6 | Messblatt | OK | s. oben | OK |
| T2 (O2) | dx -0,245 dy +0,036 +-0,03; Drehung 68,4 +-5,0 (t2_mass_v4) | 52,5 Grad | -0,262 / 0,061; 70,6 Grad | OK |
| T8 / T9 | dE00 <= 6 / Mittel <= 6 | OK | 1,9/2,9/0,7/1,5 / 2,5 | OK |
| T7a Gang koerperfest (O3) | Gelenke 1/3/5/7 <= 0,06 H vom Ziel, h85 + alle Gangtakte | 38/47, max 0,442 | 0/47, max 0,042 | OK |
| T7b / T7c / T7d | Biegung >= KF-10, Knick >= KF/2 / Strecke >= 0,75 KF / <= 90 Grad | ROT / ROT / OK | OK / OK (min 0,387) / OK (60,9) | OK |
| Boden Mesh (O3) | kein Glied-Mesh, keine Dornspitze unter dem Boden | +76 E. | -4 E. | OK |
| F0 Licht h118 / h135, F0 Wand | dE00 <= 3 / <= 2 | OK | 0,1 / 0,1 / 0,52 | OK |
| F6 / F7 / F8 / F9 / F10 / F14 / F19 | dE00 <= 5, Betrag dL <= 4 | F14 ROT | 3,7 / 3,6 / 2,0 / 2,4 / 2,8 / 2,8 (dL -3,0) / 1,2 | OK |
| F-Mittel | <= 4 | 3,11 | 2,65 | OK |
| F7 Messblatt-Wortlaut (Id 13 ganz) | 33,3/5,5/2,7 | 22,4 dL -10,9 | **22,7 dL -10,6** | ROT |
| F8 Messblatt-Wortlaut (Id 10 ganz) | 16,4/8,3/1,5 | 20,9 dL +4,5 | 19,6 dL +3,2 | OK |
| FS Unterarm (W)-rechts Id 14 (O5) | 9,0/-3,8/3,5; Anteil L* > 30 <= 0,05 | 28,2, Anteil 0,337 | 11,4, Anteil 0,000 | OK |
| FS Unterarm (W)-links Id 11, Rumpf, Oberarm (B)-links unten, Unterschenkel | Sicht-Boxen | OK | dE00 2,1 / 2,6 / 2,7 / 4,5 | OK |
| FS Oberarm (B)-rechts unten | 20,9/3,2/-0,4, Rotanteil 0,02 +-0,15 | OK (0,09) | 21,5 dE00 4,7, **Rotanteil 0,49** | ROT |

Gegenprobe V3 r2 (riegel_V3r2_gegenprobe_r3.*, Lauf sfv3/r/V3r2 ohne Ziel-Zeile -> T7 im Raum von Knochen 8): **44 von 53 rot**, darunter
K11 (28,5 / 36,5 = 0,78), T2 22,9 Grad, T7a 47/47 (max 0,634 H), T7c 14 Takte, Boden +96 E. (19 Takte), Unterarm Id 14 21,3 L* mit
Anteil 0,121. Gegenprobe V4 r2 mit den Riegeln dieser Runde (riegel_V4r2_mit_r3_riegeln.*): 11 von 53 rot (K11, S3, T2, T7a-c, Boden,
F14, F7/F8 Wortlaut, Unterarm Id 14). Tests: unit_birkin_formen, unit_birkin_szenen, unit_schwanz_r41, integration_r41_sichel_form
**4/4 Passed** (SF_belege/v4/ctest_v4_r3.txt). Spiel-Asset EM30_HANGAR.EMD und Pin cfd1ac42 unveraendert; die Variante ist
reproduzierbar ("birkin_formen.py --variante v4 --aus DIR" bitgleich dfbc30bc).

**Leisten** (SF_belege/v4/, Spalten Nutzer-Vorlage | Nutzer-Beschriftung | V4 Runde 2 (vorher) | V4 Runde 3 (neu); Runde-2-Leisten in
SF_belege/v4/r2/): 01_vorn_kamera12 (mit K11), 02_seite_wie_bild2, 03_kopf_nah_vorn_x4 (K1-K11), 04_kopf_nah_seite_x4, 05_schwanz_x3,
06_farben (mit den neuen FS-Zeilen), 07_gang (Spielbilder h85/h100/h118/h135, Unterschriften mit der koerperfesten Abweichung je Gelenk,
Biegung und Strecke; Kopfzeilen mit T7a-T7d/Boden beider Staende; letzte Zeile Feder-Stand V4 r3 an Pose 85), 08_rundum_V4 (V4 r3).

**Offen**
1. S3 (Abstand Haarkante -> Augenfleck 0,015 H, erlaubt 0,01 H): der helle Teil des Seitenauges endet an der Naht zu den Vorderbild-
   Flaechen. Runde 2 versuchte Augenmitte, Malgroesse, Vorhang, Vorderbild-Texel als Auge; Runde 3 die Vorderkante der Ellipse
   (+0,006 / +0,010 H): ohne Wirkung auf den gemessenen Fleck, K1 fiel. Naechster Weg: die Naht verlegen (Ansichtswahl der Flaechen am Auge
   mit eigener Schwelle) - beruehrt die Augen vorn, daher nicht in dieser Runde.
2. F7 Messblatt-Wortlaut (Id 13 ganz gegen das Fenster 307-347 x 167-212 = 33,3 L*) widerspricht der Sicht-Pruefung (ganzer Oberarm
   (B)-links 17,3 L*, unterer Teil 11,0 L*): das F7-Fenster liegt nur auf der hellen Schulterkuppe. Nach Fensterlage ist F7 gruen
   (31,4, dE00 3,6). Bitte Messblatt-Entscheid: Fensterlage statt "Id ganz" fuer F7.
3. FS Oberarm (B)-rechts unten Rotanteil 0,49 (Original 0,02): Id 10 traegt jetzt ganz die F8-Verteilung (Messblatt-Wortlaut F8 gruen);
   das rote F8-Fenster faerbt damit auch den unteren Oberarm. Mit getrennten Regionen (oben F8, unten Sicht-Box) ist der Rotanteil gruen,
   F8 Wortlaut aber dL +4,4 (rot, Lauf r3 Koerper 2) - die Sicht-Box unten (20,9 L*) ist heller als das F8-Fenster (16,4 L*). Entscheid wie 2.
4. K11 liegt mit 36,1 im unteren Teil des Bands (41,0 +-6,1), K1 mit Kontrast 20,4 knapp ueber 20: mehr Struktur (Verstaerkung 11-12,
   Klammer -2,5) brachte 38-41, aber helle Flecken neben den Augen wurden eigene Komponenten (K1 2 Komp., S1/S2 fanden einen Fleck auf
   der Kopfoberseite seitlich), Laeufe r3 Struktur 12-15.
5. Schwanz: die Aussenecke am Bogen bleibt spitz (Sicht-Pruefung r2 1.3 "K"); der Ansatz ist jetzt schraeg wie im Original. Die Kette ist
   im Gang fast starr am Koerper (Abweichung im Mittel 0,010 H) - mit Mitnahme 0,60 schwingt sie mehr, h85 liegt dann 0,103 H neben der
   Ruheform (Lauf V4r3g0.60).
6. Spielbild h135/h100 (Sicht-Pruefung r2 1.1): die Kopfstruktur gilt fuer alle Flaechen (beide Ansichtsbilder); der Augenfleck im
   Spielbild (Spielpose h135 ist nicht Pose "135") ist nicht eigens gemessen; die FRAMEDUMPs liegen in sfv4/r/V4r3/fd (Leiste 07).
7. Tests: die neuen Engine-Regeln (Boden mit Mesh-Massen, Mitnahme/Feder/Daempfung V4, V4-Freigabe h70-h74, kein haengen fuer V4) haben
   keinen eigenen Unit-Test; integration_r41_sichel_form laedt weiter das Spiel-Asset (Referenz 2).
8. Einbau ins Spiel-Asset erst nach der Nutzer-Abnahme.

### V4 Runde 4 (Bauer, 2026-11-07, nach Pruefung V4 r3)

Nutzer-Wortlaut wie oben ("Ich will EXAKT diese Umsetzung und auch EXAKT seine Farben"). Grundlage: SF_MESSBLATT_V4.md (Commit
7a4f3b684), die Pruefberichte SF_PRUEFUNG_V4_r3_zahlen.md und SF_PRUEFUNG_V4_r3_sicht.md (Commit 349f4360c) und die Entscheide O1-O6.
Kein RE-Befund: die Sichel-Form gibt es auf der Disc nicht, keine Zahl ist eine @0x-Konstante. Jede neue Konstante nennt im Code ihre
Quelle (Messblatt-Zeile, Original-Messung, Lauf) oder ist PORT-WAHL mit Nutzer-Wortlaut. Commits 349f4360c (Pruefberichte r3),
6488773d2 (O6 Engine-Kommentare), 41ac7f058 (Messwerkzeuge), 8ee38bb7d (Generator/Farben), 5e56f89a0 (Riegel, Gegenproben, Leisten,
ctest), 89a05d1b0 (Nachstellung F8/Beine), 72c7d7d4a (Endstand Riegel/Leisten). Endstand EM30_V4.EMD sha256 00d642b7 (Neubau
"birkin_formen.py --variante v4 --aus DIR" bitgleich), Lauf sfv4/r/V4r4. Spiel-Asset EM30_HANGAR.EMD und Pin cfd1ac42 unveraendert.

**Befund -> Aenderung -> Beleg**

| Befund (Pruefung V4 r3) | Aenderung | Ergebnis (riegel_V4r4, Lauf sfv4/r/V4r4) |
|---|---|---|
| Zahlen S3: Abstand Haarkante -> Augenfleck 0,015-0,019 H (<= 0,01) | KOPF_V4 vorbau vor 0,06 -> 0,048 H: das Fenster KS2 (0,84-0,96 der Kopfbox) rueckt mit der Vorderkante an den Fleck, der an der Naht zu den Vorderbild-Flaechen endet. Laeufe r4 Eichung f-o: 0,053 / 0,050 schwankten zwischen 0,004 und 0,011 H (ein Pixel Fleckrand), 0,048 stabil | **S3 Abstand 0,004 H, 17,2/2,8/0,6 dE00 2,5 (OK)**; S2 0,054 H / 0,809 H, K10 0,154 x 0,187 H unveraendert |
| Zahlen T2-Lage nach O2: dy +0,100 H (Soll +0,036 +-0,03, Original +0,056) | Riegel T2 misst jetzt Lage UND Drehung mit t2_mass_v4 (r4_mass.t2_lage_port). Gelenke 2-4, Durchmesser 1-4, Tiefe 2-3 per r4_form_suche.py (Saat 1, Schritt 253) gegen T1-T6/TA, T2 (O2), T5q und die Oberkante; Ziel = Ober-/Unterkontur der Leser-Maske | **T2 dx -0,251 dy +0,051, 66,6 Grad (OK)** (Original -0,268 / +0,056, 70,1/65,8/69,5) |
| Zahlen T5: Ausbuchtung an der Biegung, Abfall 0,027-0,061 H; Durchmesser Gelenk 3 0,1599 > Gelenk 2 | Durchmesser im Suchlauf monoton fallend erzwungen (Messblatt Abschnitt 4 Punkt 3): 0,139 / 0,130 / 0,079 / 0,046 H; zweites, unabhaengiges Breitenmass T5q (quer zur t2_mass_v4-Mittellinie, Pruefer pz3_t5orig) als eigener Riegel | **T5q Abfall 0,000, T5 Abfall 0,008 (OK)**; Ausbuchtung weg (Profil f 0..1: 0,025/0,032/0,045/0,055/0,066/0,095/0,104/0,129/0,14/0,146/0,144). **Rot: f 0,5 = 0,095 H (Soll 0,125 +-0,025)** - der Teil an der Biegung ist schmaler als im Original (0,117-0,121); ein dickeres Gelenk 3 hob die Oberkante wieder an (Laeufe d3 0,085/0,088/0,098: Oberkante 240..230, Drehung 60,6-63,6 Grad) |
| Zahlen F7 nach Wortlaut (Id 13 ganz 23,0 L* gegen 33,3) | keine - Widerspruch im Messblatt belegt: Id 13 = 1031 px Fensterlage (31,2 L*) + 1150 px unten (FS-Box Original 11,0 L*). Fuer Wortlaut \|dL\| <= 4 (Id ganz >= 29,3) muesste der untere Teil (29,3 x 2181 - 33,3 x 1031) / 1150 = 25,7 L* haben: dL +14,7 gegen die Sicht-Box, FS Oberarm (B)-links unten rot | F7 Fensterlage 31,2 dE00 2,1 (OK), FS Oberarm (B)-links unten 14,9 dE00 3,7 (OK), **F7 Wortlaut ROT (22,8, dL -10,5) - Entscheid noetig** |
| Zahlen FS Oberarm (B)-rechts unten: Rotanteil 0,49 (Rueckschritt) | Id 10 wieder oben/unten getrennt: unten Region oberarm_br_unten (Sicht-Box 455-500 x 228-262), oben die Kacheln (Original an derselben Stelle = F8-Fenster); Nachstellung F8 -4,6 L* als RGB-Faktor (r4_koerper_korrektur.json) | **FS Oberarm (B)-rechts unten 22,6/4,6/0,6, Rotanteil 0,02 (OK)**; F8 Fensterlage 18,6 dE00 2,1, **F8 Wortlaut 20,2 dL +3,8 (OK)** |
| Zahlen O6: 8 PORT-WAHL ohne Nutzer-Wortlaut, STRUKTUR_FARBE_KLAMMER ohne Quelle, 2 Kommentare gegen ihre Werte | schwanz_r41.c / birkin_formen.c (Commit 6488773d2) und eichen_v4.py: Nutzer-Wortlaut an jeder PORT-WAHL; STRUKTUR_FARBE_KLAMMER 0,4 = ueber p99 der Original-Farbtonabweichung x 1,5 (vorn 0,30, seitlich 0,15, max 0,95); STRUKTUR_KLAMMER 2,0 = Faktor >= 0,135; STRUKTUR_ZONE_R3 0 = keine helle Struktur | - |
| Sicht Schwanz "Vierkant-Balken": Querprofil-Stufe 16,6-19,9 L*, Oberkontur gerade (0,6 px), Dorn 27 Grad an der Bogenecke | 12 statt 10 Seiten (M1 192 <= 212); Textur: Zeile je Ecke aus der Lage um den Umfang (SCHWANZ_V4 uv verlauf), an jeder Klassengrenze Uebergang ueber 0,3 c (eichen_v4.UEBERGANG, lineare Mischung loese_zonen_misch). Ein durchgehender Verlauf ueber den ganzen Umfang (Laeufe r4 Eichung c-p, Grenze 0,5 und 0,8) war nicht stabil an T8/T9 zu eichen (Mitte (22,0,0), Aussenkante rot oder F17 4,9-9,6 L*) und wurde verworfen. Eichregionen F15/F16/F17 nach Baendern aus dem Code-Render, Aussenkante regularisiert (Gewicht 3,0, Laeufe r/s/t/u/v), gedaempft | **TQ Stufe auf 3 px 8,2/8,2/8,7/8,7 L* (Original <= 9,6, OK)**; Dorn an der Bogenecke weg (kleinster Douglas-Peucker-Eckwinkel bei x <= 300, y <= 270: 86,8 Grad bei (298,254) statt 27,3 Grad bei (272,233); an den Original-Masken 9,5-60 Grad wegen Maskenzacken, daher kein Riegel); Oberkante x 276-292 246/242/238/235/232 (V4 r3 237..228, Original 251..234, max 0,030 H). **Rot: TQ Plateau-Anteil 0,41-0,53 (Original <= 0,31), TK RMS 1,77 px (Original 3,87-4,71)**. T8 F15 11,5/8,4 dE00 3,3, F17 20,8 dE00 6,0, T9 Mittel dE00 3,9 (OK) |
| Sicht Rot-Verteilung Oberkoerper vorn: Rot Mitte/aussen 1,06-1,18 (Video 2,75-2,91), Schultern a*>12 0,10-0,13, a*>18 4,4-5,7 x Video | r2_koerper_farben.py: Kacheln 6 x 4 im Oberkoerper-Polygon der Sicht-Pruefung + Rumpfbox, Port -> Original nach Anteilslage, Lab-Tripel nach L*-Rang (Original-Verteilung je Kachel); Feinanteil des Port-Musters 0,5 (HF_OBERKOERPER, Laeufe r4 Koerper 6-8); eigene CLUT-Zeilen 490 (Mesh 13) und 491 (Beine) nach O1; Grenze je Texel auf Leuchtdichte; Quantisierung nach Sichtbarkeit gewichtet | **Buehne: Rot Mitte/aussen 1,21 -> 2,33, Schultern 0,089 -> 0,017, a*>18 0,053 -> 0,016 (Original 0,041)**; Spielbild h118 1,18 -> 1,55, Schultern 0,128 -> 0,081, a*>18 0,046 -> 0,016 (Video 0,019); h135 1,06 -> 1,49, 0,116 -> 0,070, 0,048 -> 0,013. **Rot: Mitte/aussen < 2,75 (Buehne/h118/h135), Schultern im Spielbild > 0,05** |
| Sicht Kopf vorn im Spielbild glatt: Innenflaeche 27,2/26,9 (Original 29,0-40,4), glatte Pixel 0,197/0,126 | eichen_v4.py: im Augenhof vorn bleiben die dunklen Flecken des Originals (HOF_DUNKELSTRUKTUR; seitlich aus, Lauf r4 Eichung a/b: sonst S1/S2/S4 rot), STRUKTUR_HELL 0,15 -> 0,10 (Lauf r4 Eichung k: K1 1 Komponente); Augen-Regionen der Eichung = Fleck auf den Augentexeln. Neuer Riegel KS (Spielbild ueber nb2neu/c6.port, Kreise des Sicht-Pruefers) und KS Buehne 320 x 240 (Naeherung) | **KS Spielbild h118 30,9 / glatt 0,072 (OK)**, h135 Gradient 36,8 (OK) aber **glatt 0,134 > 0,098 (ROT)**; Buehne 320 x 240 32,6 (OK); K1 1 Komp. Kontrast 26,1, K2 47,1/8,0/-1,2 dE00 0,8, K11 37,5 / Rumpf 36,8 (OK) |

**Riegel-Tabelle** (riegel_v4.py, EM30_V4.EMD 00d642b7, Lauf sfv4/r/V4r4; volle Tabelle SF_belege/v4/riegel_V4r4.md/.txt/.json; **Exit 1,
11 rot von 63**). Spalte "V4 r3" = derselbe Riegel an V4 r3 (riegel_V4r3_mit_r4_riegeln.*: 13 rot von 63).

| Mass | Soll | Ist V4 r3 | Ist V4 r4 | |
|---|---|---|---|---|
| G1 / M1-M4 | Geometrie = Ref2 (CLUT-Feld frei, O1); M1 <= 212; M4 0,045-0,091 | OK | OK; CLUT 2/3/5/6 -> 491, 10 -> 489, 13 -> 490, 14 -> 488; M1 192; M4 0,050-0,091 | OK |
| K1-K11 | Messblatt V4 / K11 41,0 +-6,1 | OK | K1 1 Komp. 26,1; K2 dE00 0,8; K10 0,154 x 0,187; K11 37,5 / 36,8 | OK |
| S1 / S2 / S4 | Messblatt | OK | 48,5/3,1/0,9, 0,042 x 0,049 H, 1 Komp. / 0,054; 0,809 / 27,6/15,6/2,9 | OK |
| S3 | L* <= 22, dE00 <= 6, Abstand <= 0,01 H | ROT 0,015 | 17,2/2,8/0,6, dE00 2,5, **0,004 H** | OK |
| T1 / T4 / TA / T6 | Messblatt | OK | 0,172/0,742 / -0,215 0,385 0,357 / <= 0,024 H / B 0,032 L 0,121 81 Grad | OK |
| T2 (O2, Lage und Drehung) | dx -0,245 dy +0,036 +-0,03; 68,4 +-5 Grad | ROT dy +0,100 | **-0,251 / +0,051; 66,6 Grad** | OK |
| T3 | dx -0,265 dy +0,257 +-0,03 | OK -0,248 | **-0,233** / 0,266 | ROT |
| T5 / T5q | f 0,1/0,3/0,5/0,7 0,03/0,065/0,125/0,14; Abfall <= 0,01 | OK / ROT (Abfall 0,061) | 0,028/0,053/**0,095**/0,121, Abfall 0,008 / 0,032/0,055/**0,095**/0,129, Abfall 0,000 | ROT |
| T8 / T9 | dE00 <= 6 / Mittel <= 6 | OK | F15 3,3 F16 2,1 F17 6,0 F18 1,3 / 3,9 | OK |
| T7a-d, Boden (O3) | <= 0,06 H; Biegung/Knick; Strecke; <= 90 Grad; nichts unter dem Boden | OK | 0/47 max 0,048 H; 61,2-70,5 / 10,6-16,2 Grad; min 0,389 H; 72,7 Grad; tiefster Mesh-Punkt -13 E. | OK |
| F0 Licht h118/h135, Wand | <= 3 / <= 2 | OK | 0,06 / 0,21 / 0,52; je Id 0,07-0,67 (r4/licht_je_id_V4r4.txt) | OK |
| F6 (Wortlaut) / F6 Fensterlage | 16,6/17,3/6,2 | OK / OK | **22,1/9,7/3,8 dE00 7,0** / 19,2/11,2/4,7 dE00 4,8 | ROT / OK |
| F7 / F8 / F9 / F10 / F14 / F19 | dE00 <= 5, \|dL\| <= 4 | OK | 2,1 / 2,1 / 3,0 / 2,8 / 2,2 / 1,6 | OK |
| F7 Wortlaut / F8 Wortlaut | Id ganz | ROT / OK | **22,8 dL -10,5** / 20,2 dL +3,8 | ROT / OK |
| FS (6 Sicht-Boxen) | dE00 <= 5, \|dL\| <= 4, Rotanteil +-0,15 | Oberarm (B)-rechts unten ROT 0,49 | alle OK; Oberarm (B)-rechts unten Rotanteil 0,02, Unterschenkel 0,14, Rumpf 0,37 | OK |
| TQ Querprofil | Stufe <= 9,6 L*, Plateau <= 0,31 | ROT 16,6-19,9 / 0,38-0,41 | Stufe 8,2-8,7, **Plateau 0,41-0,53** | ROT |
| TK Oberkontur | RMS >= 3,02 px; Oberkante <= 0,03 H | ROT 0,60 / 0,057 H | **RMS 1,77** / 0,030 H | ROT |
| FO Buehne / Spielbild h118 / h135 | Rot Mitte/aussen >= 2,75; Schultern <= 0,05; a*>18 <= Original | ROT 1,21 / 1,18 / 1,06 | **2,33 / 1,55 / 1,49**; Schultern 0,017 / 0,081 / 0,070; a*>18 OK | ROT |
| KS Buehne 320 / Spielbild h118 / h135 | Gradient 29,1-40,4; glatt <= 0,098 | ROT 28,0 / 26,9 / 27,2 | 32,6 / 30,9 (0,072) / 36,8 (**0,134**) | OK / OK / ROT |

Gegenprobe V3 r2 mit denselben Riegeln (riegel_V3r2_gegenprobe_r4.*, Lauf sfv3/r/V3r2): **52 rot von 63**. Gegenprobe V4 r3
(riegel_V4r3_mit_r4_riegeln.*): 13 rot von 63 - die neuen Riegel (T2-Lage, T5q, TQ, TK, FO, KS) trennen V4 r3 wie die Pruefung V4 r3.
Rueckschritte gegenueber V4 r3 unter denselben Riegeln: **T3, T5, F6 nach Wortlaut** (in V4 r3 gruen, in V4 r4 rot); F8 Wortlaut und
FS Unterschenkel, die mit Stand 8ee38bb7d ebenfalls zurueckgefallen waren, sind mit 89a05d1b0 wieder gruen. Tests: unit_birkin_formen,
unit_birkin_szenen, unit_schwanz_r41, integration_r41_sichel_form **4/4 Passed** (SF_belege/v4/ctest_v4_r4.txt; ninja "no work to do").

**Leisten** (SF_belege/v4/, Spalten Nutzer-Vorlage | Nutzer-Beschriftung | V4 Runde 3 (vorher) | V4 Runde 4 (neu); Runde-3-Leisten in
SF_belege/v4/r3/): 01_vorn_kamera12 (mit FO), 02_seite_wie_bild2, 03_kopf_nah_vorn_x4 (K1-K11, KS 320), 04_kopf_nah_seite_x4 (S1-S4),
05_schwanz_x3 (T2-T6, T5q, TQ, TK), 06_farben, 07_gang (Spielbilder h85/h100/h118/h135 mit T7 koerperfest, KS/FO h118/h135),
08_rundum_V4 (V4 r4).

**Offen**
1. F6 nach Messblatt-Wortlaut: F6 vergleicht ALLE roten Bildpunkte von Id 8 (4735 px, 22,1/9,7/3,8) mit den roten Bildpunkten des
   Brustfensters 372-424 x 197-232 (16,7/17,0/6,1 im Original). Im Original haben die roten Bildpunkte des ganzen Oberkoerpers
   (Polygon + Rumpfbox) 19,6/11,1/3,1 - wer F6 nach Wortlaut trifft, faerbt den ganzen Rumpf so rot wie die Brustmitte (Lauf r4
   Koerper-Eichung 1: Rot Mitte/aussen 1,81, Schultern a*>12 0,24) und widerspricht der Sicht-Pruefung V4 r3 1.5. An der Fensterlage
   (rote Id-8-Bildpunkte im Fenster) ist F6 gruen (dE00 4,8). Bitte Messblatt-Entscheid: F6 an der Fensterlage.
2. F7 nach Wortlaut: Widerspruch im Messblatt (Rechnung oben); bitte Entscheid "Fensterlage + Sicht-Box unten" statt "Id ganz".
3. FO Rot-Verteilung: Buehne 2,33, Spielbild 1,55 / 1,49 (Ziel >= 2,75). Grenzen: der Port-Kopf (Id 9, rot an der Kappe) liegt in
   Pose 135 im linken Drittel (Port x 325-380, Original-Kopf in der Mitte 368-416), und die Texel werden in zwei Ansichten gemeinsam
   gefittet. Im Spielbild sind die Schultern noch rot gesprenkelt (a*>12 0,07-0,08).
4. T3 dx -0,233 (0,002 H ausserhalb) und T5/T5q f 0,5 = 0,095 H (0,005 H zu schmal): Knick und Biegung haengen in der Kette zusammen; die
   Endstueck-Suche (Saat 22) behob T4, nicht T3; ein dickeres Gelenk 3 hob die Oberkante (TK) und senkte die Drehung.
5. TQ Plateau 0,41-0,53 und TK RMS 1,77 px: der waagerechte Teil hat keine harte Kante mehr (Stufe <= 8,7 L*), ist aber in der Mitte
   der Seitenflaeche einfarbig (eigene Klassenfarbe) und die Oberkontur nur schwach gekruemmt; ein durchgehender Verlauf liess sich
   nicht stabil eichen (Laeufe r4 Eichung c-p). Naechster Weg: eigene Eichregionen fuer Ober-/Unterhaelfte der Seite statt der Klassen.
6. KS Spielbild h135: glatte Pixel 0,134 (Original max 0,098) bei Gradient 36,8 im Band - die Kappe ist fleckig, die Mitte um das Auge
   bleibt glatt (Hof ohne helle Flecken, sonst K1 2 Komponenten).
7. Die Koerperfarben sind am Stand basis491 (Schwanz V4 r3-Form, Vorhang 0,06) gefittet; der Schwanz V4 r4 verdeckt in Pose 85 einen
   etwas anderen Teil des Buckels (F14 57,5 dE00 2,2, gruen).
8. Tests: die Engine ist in dieser Runde nur kommentiert (O6); ein Unit-Test fuer die V4-Regeln der Runde 3 (Mitnahme, Freigabe,
   Boden mit Mesh) fehlt weiter (Pruefung V4 r3 Zahlen Offen 6).
9. Einbau ins Spiel-Asset erst nach der Nutzer-Abnahme.

### V4 Runde 5 (Bauer, 2026-11-07, nach Pruefung V4 r4)

Nutzer-Wortlaut wie oben ("Ich will EXAKT diese Umsetzung und auch EXAKT seine Farben"). Grundlage: SF_MESSBLATT_V4.md mit dem Nachtrag
Runde 5 (Entscheid O7, Commit fb17942e5), die Pruefberichte SF_PRUEFUNG_V4_r4_zahlen.md / SF_PRUEFUNG_V4_r4_sicht.md (Commit 0936f2e84) und
die Entscheide O1-O11. Kein RE-Befund: die Sichel-Form gibt es auf der Disc nicht, keine Zahl ist eine @0x-Konstante; jede neue Konstante
nennt im Code ihre Quelle (Messung am Original, Lauf) oder ist PORT-WAHL mit Nutzer-Wortlaut. Commits 0936f2e84 (Pruefberichte r4),
fb17942e5 (Messblatt-Nachtrag O7), 8a83adce6 (Leisten r4 nach r4/), 496613e2c (Schwanz), a0638b2dc (Koerperfarben), 13830bca0 (Riegel,
Leisten-Werkzeug, Gegenproben), 54a1f60e8 (Endstand). Endstand EM30_V4.EMD = EM30_V4r5.EMD sha256 a762bd66 (Neubau "birkin_formen.py
--variante v4 --aus DIR"; Runde-4-Stand bleibt als EM30_V4r4.EMD 00d642b7), Lauf sfv4/r/V4r5 (191 FRAMEDUMPs). Spiel-Asset EM30_HANGAR.EMD
und Pin cfd1ac42 unveraendert.

**Befund -> Aenderung -> Beleg**

| Befund (Pruefung V4 r4) | Aenderung (Entscheid) | Ergebnis (riegel_V4r5, Lauf sfv4/r/V4r5) |
|---|---|---|
| S1/S2 Schwanz: drei einfarbige Bloecke je Mesh (L* 7,3/28,6/13,2), Quernaehte 16,7 dE76, Helligkeit verkehrt, Wurzel weinrot a* 22 | (O8) EINE Textur fuer alle Glieder: SCHWANZ_V4 uv art 'achse' (v = Bogenlaenge fortlaufend, u = Lage um den Umfang); Texel aus Vorlage 2 (r5_schwanz_textur.py): Gegenstelle im Original nach Anteil der Achse zwischen Beginn/Biegung/Knick/Ende und Lage quer zur Mittellinie (Lauf r5 Schwanz b mit "derselben Bildstelle": Station Mitte L* 21,1 gegen 10,2, Naht 19,0 an der Wurzel - verworfen); Texel = Sum lambda D / Sum lambda^2; 3 Runden Nachfuehrung nach der Vergleichsregel; 255 Indizes. Die Wurzel liest am Ruecken die Figur (ueberfuehrend) | **TN 10,7 dE76** (r4 16,7; Original 9,5, ROT knapp, bei x 297 = Zwischenring an der Biegung); **TV gruen** an allen 5 Stationen (Wurzel 9,7/10,4 gegen 9,0/11,2; Mitte 9,6/8,0 gegen 10,3/8,1; Biegung 22,8/6,2 gegen 23,4/6,2; senkrecht 12,4/3,8 gegen 10,2/2,7; Endstueck); T8 F15 1,3 / F16 3,8 / F17 5,9 / F18 0,7, T9 4,9 (gruen); Kurve und Streifen in Leiste 09. **TR 0,020 gegen 0,102 (ROT)**: die Weichzeichnung der Vergleichsregel glaettet die wenigen kraeftig roten Wurzelpunkte des rohen Originals |
| Zahlen TK RMS 1,77 px, T3 dx -0,234, T5 f 0,5 0,095 H (O11) | Zwischenring 'bauch' in Glied 2 (t 0,15, Radius x1,374, aussen 0,011), 11 Seiten (M1 198 <= 212), Gelenk 3 (-0,3003/+0,0743; 0,0636 H) aus r5_form_suche.py Lauf "bauch" (Saat 5, Schritt 46); Gelenke 5-7 je 0,0075/0,015 H nach hinten (T3). Laeufe r5 Form Saat 1-4 (alle 14 Parameter, Start r4 bzw. dicker waagerechter Teil): nach 40-60 Schritten Strafe 5-35, verworfen | **TK RMS 3,69 px** (Original 3,87-4,71, Grenze 3,02), Oberkante max 0,027 H; **T3 -0,239 / +0,247** (gruen); **T5q 0,032/0,047/0,110/0,161** (gruen); T2 64,0 Grad, dx -0,259 dy +0,049 (gruen); **T5 Bauer-Mittellinie f 0,5 0,098 H (ROT, Grenze 0,100)** |
| S3 gelber Fleck (B)-linke Schulter (45 Texel, Mesh 13, CLUT 490) | (O9) Ursache gemessen: die gelbe Lampe hinter der (B)-linken Schulter (Vorlage 1 x 306-341, y 145-167, b* bis 28) lag in der Schulterbox der Region 'rumpf' und im Kachel-Polygon. r2_koerper_farben: Original-Bildpunkte mit b* > 9,9 (= max b* der Figur in Rumpf- und Kopfbox) zaehlen nicht; dazu farbmenge(): Bildpunkte > 10 dE76 von jeder Farbe der Original-Region bekommen die naechste Farbe der Menge | gelb 0 px in beiden Ansichten; **AF Pose 135 max 0,009** (gruen), **AF Pose 85 max 0,012 Id 6 (ROT, Grenze 0,01)**; V4 r4 mit demselben Riegel 0,026 (Id 13 gelb 47) / 0,108 |
| S4 Rot ueberall, Mitte/aussen 1,55/1,49, Schultern 0,08/0,07, a* > 22 h85 1170 / h100 531 | (O10) Oberkoerper-Ziel nach Anteilslage (Farbe des Originals an derselben Stelle) statt Rangabbildung (die verteilte das Rot auf die dunklen Spalten des Referenz-2-Musters); Feinanteil nur als Helligkeit; Ausgleich je Drittel an den Laufposen h118/h135 mit dem Video-Verhaeltnis; Grenze a* <= 21,7 (p99 der Vorlage 1 im Polygon) fuer Kopf-, Schwanz- und Koerperzeilen | **FO Buehne 6,48** (Schultern 0,005), **Spielbild h135 2,76** (gruen), **h118 2,73 (ROT, 0,02 unter 2,75)**, Schultern 0,016/0,020; **FR h100/h118/h135 0 px** (gruen), **h85 165 (ROT, Grenze 41)** |
| F6/F7 nach Wortlaut (O7) | Messblatt-Nachtrag; Riegel an der Fensterlage, F7 mit FS Oberarm (B)-links unten; "Id ganz" INFO | F6 3,4 / F7 1,1 / F-Mittel 2,42 (gruen) |
| F16/F17 auskommentiert (Zahlen 4.2, O11) | Klarstellung: der Stand der farben_v4.npz (vom Pruefer bitgleich nachgebaut) ist OHNE F16/F17 entstanden (Lauf r4 Eichung n mit F17-Gewicht 2 verworfen); das Dossier V4 Runde 4 nannte irrtuemlich F15/F16/F17 - hiermit berichtigt. Seit Runde 5 kommt der Schwanz nicht mehr aus eichen_v4.py | eichen_v4.py: Kommentar statt Zeilenumbruch-Rest |
| O6: 5 PORT-WAHL ohne Wortlaut, vorbau-Kommentar | vorbau: Kommentar nennt 0,053 als ersten Versuch, Endstand 0,048, Wortlaut; zeilen_je_c, rgw: Wortlaut und "seit Runde 5 ohne Wirkung"; MU, KACHEL_MIN: Wortlaut und Laeufe | - |

**Riegel-Tabelle** (riegel_v4.py, EM30_V4.EMD a762bd66, Lauf sfv4/r/V4r5; volle Tabelle SF_belege/v4/riegel_V4r5.md/.txt/.json; **Exit 1,
8 rot von 70**, 2 INFO-Zeilen nicht gezaehlt). Spalte "V4 r4" = derselbe Riegel an V4 r4 (SF_belege/v4/r5/riegel_V4r4_mit_r5_riegeln.*:
16 rot von 70).

| Mass | Soll | Ist V4 r4 | Ist V4 r5 | |
|---|---|---|---|---|
| G1 / M1-M4 | Geometrie = Ref2; M1 <= 212; M4 0,045-0,091 | OK | OK; M1 198; M4 0,060-0,072 | OK |
| K1-K11 | Messblatt V4 / K11 41,0 +-6,1 | OK | K1 1 Komp. 25,3; K2 dE00 1,6; K11 37,8 / Rumpf 27,1 | OK |
| S1-S4 | Messblatt | OK | S3 0,004 H; S4 dE00 3,1 | OK |
| T1 / T2 / T4 / TA / T6 | Messblatt, T2 nach O2 | OK | T2 dx -0,259 dy +0,049, 64,0 Grad | OK |
| T3 | dx -0,265 dy +0,257 +-0,03 | ROT -0,233 | -0,239 / +0,247 | OK |
| T5 (Bauer) | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 | ROT 0,095 | 0,032/0,049/**0,098**/0,163 | ROT |
| T5q | dito, quer | ROT 0,095 | 0,032/0,047/0,110/0,161 | OK |
| T8 / T9 | dE00 <= 6 / Mittel <= 6, L*/a* +-4/+-3 | OK | F17 5,9 / 4,9; 14,4/3,6 | OK |
| T7a-d, Boden (O3) | koerperfest | OK | 0/47, max 0,056 H; 69,5-76,8 Grad; min 0,370 H; 79,4 Grad; -6 E. | OK |
| F0, F6 (Fensterlage), F7 (Fensterlage + FS), F8-F19, F-Mittel | dE00 <= 5, Betrag dL <= 4; Mittel <= 4 | F6 ROT 7,0 (Wortlaut) | 3,4 / 1,1 / 2,7 / 1,9 / 3,4 / 2,9 / 1,6; Mittel 2,42 | OK |
| FS (6 Sicht-Boxen) | dE00 <= 5, Betrag dL <= 4, Rotanteil +-0,15 | OK | alle OK | OK |
| TQ | Stufe <= 9,6, Plateau <= 0,31 | ROT 0,41-0,53 | Stufe 5,0-7,7, **Plateau 0,21-0,41** | ROT |
| TK | RMS >= 3,02 px; Oberkante <= 0,03 H | ROT 1,77 | 3,69 px; 0,027 H | OK |
| TN (neu, O8) | Quernaht <= 9,5 dE76 | ROT 16,7 | **10,7** bei x 297 | ROT |
| TV (neu, O8) | 5 Stationen +-4/+-3 | ROT (Wurzel 25,0/2,6) | alle OK | OK |
| TR (neu, O8) | a* > 12 waagerecht 0,102 +-0,05 | ROT 0,189 | **0,020** | ROT |
| AF (neu, O9) Pose 135 / 85 | je Mesh <= 0,01 | ROT / ROT | 0,009 / **0,012 (Id 6)** | OK / ROT |
| FO Buehne / Spielbild h118 / h135 | >= 2,75; Schultern <= 0,05 | ROT | 6,48 / **2,73** / 2,76 | OK / ROT / OK |
| FR (neu, O10) h85 / h100 / h118 / h135 | a* > 22 <= max(10 x Video, 1 Promille) | ROT h85, h118 | **165** / 0 / 0 / 0 | ROT / OK / OK / OK |
| KS Buehne / h118 / h135 | Gradient 29,1-40,4; glatt <= 0,098 | ROT h135 0,134 | 32,6 / 31,0 (0,048) / 37,5 (**0,120**) | OK / OK / ROT |

Gegenprobe V3 r2 mit denselben Riegeln (SF_belege/v4/r5/riegel_V3r2_gegenprobe_r5.*, Lauf sfv3/r/V3r2): **57 rot von 70**. Gegenprobe V4 r4
(riegel_V4r4_mit_r5_riegeln.*): 16 rot von 70 - alle neuen Riegel TN, TV, TR, AF 135/85, FR h85/h118 sind dort rot, dazu TK, TQ, T3, T5,
T5q, FO Buehne/h118/h135, KS h135. Behoben gegenueber V4 r4 (dort rot, hier gruen): T3, T5q, TK, TV, AF 135, FO Buehne, FO h135, FR h118.
Nicht behoben (beide rot): T5 Bauer, TQ, TN, TR, AF 85, FR h85, FO h118, KS h135 (jeweils besser als V4 r4). Rueckschritte gegenueber V4
r4 unter denselben Riegeln: keine Zeile, die in V4 r4 gruen war, ist rot. Tests unit_birkin_formen, unit_birkin_szenen, unit_schwanz_r41,
integration_r41_sichel_form **4/4 Passed** (SF_belege/v4/ctest_v4_r5.txt; Bau "LOCAL-BUILD-OK", Engine unveraendert).

**Leisten** (SF_belege/v4/, Spalten Nutzer-Vorlage | Nutzer-Beschriftung | V4 Runde 4 (vorher) | V4 Runde 5 (neu); Runde-4-Leisten in
SF_belege/v4/r4/): 01_vorn_kamera12 (mit FO, AF), 02_seite_wie_bild2, 03_kopf_nah_vorn_x4, 04_kopf_nah_seite_x4, 05_schwanz_x3 (T2-T6,
T5q, TQ, TK, TN, TR, TV), 06_farben, 07_gang (Laeufe V4r4 / V4r5), 08_rundum_V4 (V4 r5), **09_schwanz_verlauf** (L* und a* entlang der
Mittellinie, Abschnitte Beginn-Biegung-Knick-Ende, Original | V4 r4 | V4 r5 als Kurve und als Farbstreifen).

**Offen**
1. T5 nach der Bauer-Mittellinie f 0,5 = 0,098 H (Grenze 0,100; T5q quer zur t2_mass_v4-Mittellinie 0,110 gruen). In Runde 3 gruen
   (0,140), damals mit T2-Lage rot. Die Laeufe r5 Form zeigen den Zielkonflikt T5 f 0,5 <-> T2-Drehung: Gelenk 4 mit 0,0636 statt 0,0459 H
   gibt T5 0,093 und T2 62,5 Grad (Grenze 63,4); Gelenk 3 0,07 gibt T5 0,100, aber T2 58,8 Grad und f 0,7 0,169 (formprobe-Laeufe r5).
2. TQ Plateau 0,38-0,41 bei x 310-330 (Original <= 0,31): die Seitenflaeche hat Textur aus dem Original, nach der Weichzeichnung sigma 1,5
   bleiben aber Laengsbaender konstanter Helligkeit (11 Seiten). TN 10,7 dE76 bei x 297 am Zwischenring (Original 9,5).
3. TR 0,020 (Original 0,102): Rotanteil im rohen Original gegen den weichgezeichneten Port gemessen; das Mittel trifft (TV Wurzel a* 10,4
   gegen 11,2), die Spitzen a* > 12 nicht.
4. FR h85 165 px a* > 22 (Grenze 41, Video 2) und FO h118 2,73 (Grenze 2,75), KS h135 glatt 0,120 (<= 0,098): im Spielbild; die Probe an
   den b2-Laufposen (r5_probe.py) hatte h85 0 px und h118 2,88 - das Spielbild weicht ab (Licht/Ueberblendung h85).
5. AF Pose 85 Id 6 (Bein (W)-rechts) 0,012 (Grenze 0,01).
6. Tests: kein Unit-Test fuer die V4-Regeln (Pruefung V4 r3/r4); integration_r41_sichel_form laedt weiter das Spiel-Asset.
7. Einbau ins Spiel-Asset erst nach der Nutzer-Abnahme.

## V4 Originaltexturen (Bauer, Runde 1 von 2, 2026-11-07)

### Nutzer-Wortlaut und Ursache
Nutzer 2026-11-07 ~17:40: "Bei der Sichel Form V4, warum sind denn da die Farben so extrem scheisse? Das kann doch eigentlich garnicht
passieren.... die Form baut von Farben und Texturen her ja nun wirklich auf die Ursprüngliche Birkin Form auf.... Die Farben können doch
dann im Prinzip garnicht so scheisse aussehen."

Ursache (Orchestrator, hier bestaetigt): Ab V4 Runde 2 wurden auch die Koerper-Texel und Koerper-CLUTs per Farbeichung an die Videobilder
angepasst (eichen_v4.py, r2_koerper_farben.py, r4_koerper_eichen.py; Messblatt V4 Abschnitt 5). Die Gegenprobe mit dem neuen Riegel an
V4 Runde 5 (a762bd66) zeigt das Ausmass (SF_belege/v4/ot/riegel_ref2tex_V4r5_gegenprobe.*):
- 34486 von 34716 Koerpertexeln geaendert;
- CLUT-Zeilen 480-485 geaendert, sechs neue Zeilen 486-491;
- sieben Koerper-Meshes auf eigene Zeilen umgehaengt;
- 3129 Kopf-/Schwanztexel mit Farben, die es in Referenz 2 nicht gibt.

Messblatt V4 Abschnitt 5 ist als ueberholt markiert.

### Vorgabe (Orchestrator, bindend)
1. Koerper = Referenz 2 unveraendert.
2. Schwanz: Form V4, Textur aus der Koerperhaut des Ref2-Atlas, UV entlang der Achse durchgehend.
3. Kopf: Form V4, Merkmale an den Messblatt-Orten, nur Ref2-Farben in der Machart der Original-Textur.
4. Geometrie wie V4 Runde 5; Spiel-Asset und Pin bleiben unveraendert.

### Aenderungen
**birkin_formen.py** (Commit 514b69739): Neuer Schalter RE15_V4_TEXTUR, Standard ref2.
- Im Modus ref2 entfallen die Koerperzeilen 488-491, die Koerpertexel, die geeichten Hautzeilen und die Farbklammer.
- Kopf (Mesh 9) und Schwanz (Meshes 16-22) zeigen auf die Ref2-Zeile 480 (Oberarm-/Rueckenhaut).
- v4_ref2tex_einsetzen schreibt nur die freien Flaechen: TPage 128 v 160-223 (Kopf) und TPage 130 u 64-127 (Schwanz).
- RE15_V4_TEXTUR=eichung baut V4 Runde 5 bitgleich nach (gemessen a762bd66).

**ref2tex_v4.py** mit farben_ref2tex_v4.npz und Protokoll SF_belege/v4/ref2tex_protokoll.json (Commit 03ba6284f):
- Schwanz:
  - Quelle sind Texel der Oberarm-/Rueckenhaut (Seite 0, x 1-64, y 15-104). Das ist das groesste ganz in Zeile 480 belegte
    64er-Rechteck; die Texel gehoeren zu Mesh 13, 8 und 10.
  - Entlang der Achse wird die Quelle gespiegelt gekachelt, damit keine Naht entsteht.
  - Quer wird nach dem Umfangswinkel entzerrt, weil die UV-Abbildung 'achse' u = (1 - c)/2 x sp setzt. Ohne Entzerrung lagen an Ober-
    und Unterkante nur 1-2 Quellspalten auf einer ganzen Seitenflaeche; das war in der ersten Fassung als Querstreifen an der Wurzel
    sichtbar.
- Kopf:
  - Erlaubt sind nur Eintraege der Zeile 480, die Ref2 auf Koerperflaechen zeigt, in fuenf Klassen:

    | Klasse | Eintraege | Bedingung | Einsatz |
    |---|---|---|---|
    | haut | 149 | L* < 40 | uebrige Zonen |
    | rot | 132 | a* >= 12 | Kuppe |
    | hell | 21 | L* >= 45, a* <= 15 | Auge |
    | dunkel | 55 | L* <= 22 | Haare |
    | dunkelrot | 57 | L* <= 30, a* >= 8 | Kinn |

  - Die Feinstruktur stammt aus den Original-Kopftexeln von Ref2 (x 76-119, y 19-67, ganz von Mesh 9 abgedeckt): Helligkeit hoch 0,7,
    dazu die halbe a*/b*-Abweichung.
  - Die Orte stammen aus der Zonenkarte der Runde 5, also den Messblatt-Fenstern.
  - Der Augenfleck sitzt an den Riegelorten K4/S2 mit der Groesse K3/S1. Er wird im Bild der jeweiligen Vorlage-Kamera gesetzt und ueber
    Kenn-Renders auf beide Kopfbilder zurueckgetragen.
  - Die Zonenhelligkeit wird nach der Vergleichsregel nachgefuehrt, die hier nur Messverfahren ist. Je Texel wird immer ein vorhandener
    Eintrag gewaehlt. Es liefen 12 Runden; gewaehlt ist Runde 8, T-K4 ist ab Runde 5 durchgehend gruen.
- PORT-WAHL, jeweils mit Lauf im Kommentar: STRUKTUR_EXP 0,7, STRUKTUR_FARBE 0,5, AUGE_KERN 0,3, AUGE_PROFIL 1,5, UMGEBUNG_SIGMA 3,
  GLATT 1,2, DAEMPF 0,7 und die Klassengrenzen. Zwei davon mit eigener Begruendung:
  - AUGE_HOF 0,85: ohne Hof lag der K1-Kontrast bei 19,2-19,9.
  - UMGEBUNG_DL 6: die Augenumgebung (KV9/KV6) liegt dunkler, weil der hellste blasse Hauteintrag der Palette nach der Vergleichsregel
    nur L* ~48 statt 56 ergibt.

**riegel_ref2tex.py** (neu, Exit 1 bei Rot) und **leisten_ref2tex.py**.

### Riegel
riegel_ref2tex.py an EM30_V4.EMD (sha256 a9da924f), Lauf sfv4/r/V4ot mit 191 FRAMEDUMPs; Ausgabe SF_belege/v4/ot/riegel_ref2tex_V4ot.*.
**Exit 1: 1 rot von 40** (T5, unveraendert seit Runde 5), dazu 1 INFO-Zeile.

| Mass | Soll | Ist V4 neu | Ist V4 r5 (Gegenprobe) |
|---|---|---|---|
| T-K1a Koerper-Meshes | Meshes 0-8, 10-15 inkl. UV/CLUT-Feld/TPage bytegleich | OK, keine Abweichung | ROT [2, 3, 5, 6, 10, 13, 14] |
| T-K1b CLUT | Zeilen 480-485 bytegleich, weitere nur Kopie | OK, TIM 6 Zeilen | ROT, 480-485 abweichend, 486-491 neu |
| T-K1c Koerper-Texel | jeder abgedeckte Texel bytegleich | OK, 0 von 34716 | ROT, 34486 von 34716 |
| T-K2 keine neue Farbe | Kopf/Schwanz-Texel in der Ref2-CLUT | OK, 0 von 14008, 165 Farben, Zeile 480 | ROT, 3129 von 14008 |
| T-K3a Quernaht | Sprung zwischen Gliedern <= innerhalb | OK, 13,0 (Id 20/21) <= 13,5 | OK, 8,6 <= 21,0 |
| T-K3b Bloecke | je Glied Std L* >= 5,4, >= 16 Indizes | OK, Std 11,1-15,8, 136-174 Indizes | ROT |
| T-K3c Wurzel | Sprung Id 16 / Rumpf <= innerhalb | OK, 5,2 | OK, 10,7 |
| T-K4 K1-K10 | Messblatt V4 Abschnitt 7 | alle OK (K1 20,7; K2 dE00 3,0; K3 0,027 x 0,036 H; K4 0,052 / 0,096 H; K5 1,2; K6 11,5; K7 3,8; K8 2,2; K9 0,4; K10 0,154 x 0,187 H) | alle OK |
| T-K4 S1-S4 | dito | alle OK (S1 dE00 5,4 bei L* 42,7, 0,042 x 0,053 H; S2 0,046 / 0,817 H; S3 0,5; S4 0,4) | alle OK |
| G1, M1-M4 | wie Riegel V4 | OK (M3 Kopf/Schwanz Zeile 480; M4 0,057-0,072) | OK |
| T1-T4, TA, T5q, T6, TK | wie Riegel V4 | OK | OK |
| T5 (Bauer-Mittellinie) | f 0,5 >= 0,100 H | ROT 0,098 | ROT 0,098 |
| T7a-d, Boden | koerperfest am Lauf | OK, 0 von 47 Takten, max 0,056 H | OK |

K8 wird an den Kopf-Bildpunkten im Fenster KV4 gemessen (Id 9, 2 px erodiert wie alle Id-Masken in riegel_v4). Das Fenster reicht bis 1,05
der Kopfbox, und 145 seiner 276 Bildpunkte liegen auf dem Rumpf Id 8. Der Rumpf ist nach Vorgabe 1 Referenz 2 und hell. Nach dem
Messblatt-Wortlaut ("Fenster ganz") ergeben sich 30,1 L* und dE00 15,5. Dieser Wert haengt am Ref2-Rumpf, nicht am Kopf, und steht deshalb
nur als INFO-Zeile in der Ausgabe.

### Leisten (SF_belege/v4/; Runde-5-Leisten in v4/r5/)
Alle Leisten haben die Spalten Nutzer-Vorlage | Nutzer-Beschriftung | Referenz 2 (gelobt) | V4 neu (Originaltexturen):
- 01_vorn_kamera12
- 02_seite_wie_bild2
- 03_kopf_nah_vorn_x4
- 04_kopf_nah_seite_x4
- 05_schwanz_x3: Zeile 1 zeigt die Textur ohne Marken, Zeile 2 den Polygonzug.
- 07_gang: Spiellaeufe sfv4/r/R2ot (Spiel-Asset) und sfv4/r/V4ot.
- 08_rundum_V4: Zeile 1 Referenz 2, Zeile 2 V4 neu.

Am Bild:
- Der Koerper ist in allen Spalten bildgleich zu Referenz 2.
- Der Schwanz traegt die Oberarmhaut mit den roten Adern.
- Kopf vorn: rote Oberseite, dunkles Haarfeld (B)-rechts, blasser, unregelmaessiger Augenfleck (B)-links.
- Kopf seitlich: rote Oberseite, blasser Augenfleck, dunkle Vorderkante.

### Tests
unit_birkin_formen, unit_birkin_szenen, unit_schwanz_r41 und integration_r41_sichel_form: 4/4 Passed (100% tests passed, 64,6 s;
SF_belege/v4/ot/ctest_v4_ot.txt). Der Bau meldet "LOCAL-BUILD-OK (build)", die Engine ist unveraendert. Das Spiel-Asset EM30_HANGAR.EMD
und der Pin cfd1ac42 sind unveraendert.

### Offen
1. T5 ist der einzige rote Riegel und der Grund fuer Exit 1: die Bauer-Mittellinie ergibt f 0,5 = 0,098 H bei Grenze 0,100. T5 ist seit
   Runde 5 rot, weil die Geometrie nach Vorgabe 4 unveraendert bleibt. T5q quer zur Pruefer-Mittellinie ist gruen (0,110).
2. S1 und K1 halten nur knapp, beide aus demselben Grund: der hellste blasse Eintrag der Ref2-Hautzeile 480 hat L* 55 (Texel).
   - S1: dE00 5,4 bei Grenze 6, L* 42,7 statt 48,4. Heller geht es seitlich nur mit rosa Wundeintraegen (a* 22) oder einer anderen Zeile.
   - K1: Kontrast 20,7 bei Grenze 20 (Original 29,5).
3. Der Kopf ist vorn dunkel, weil die Messblatt-Ziele K5/K7/K8 bei 13-18 L* liegen. Er ist damit dunkler als Kopf und Rumpf von Referenz 2.
   Die Abnahme am Bild (Leiste 03) liegt bei Pruefer und Nutzer.
4. T-K3a haelt knapp: 13,0 zwischen den Gliedern 20/21 gegen 13,5 innerhalb. Ursache ist der Lichtwechsel an Gelenk 20/21 (Gouraud je
   Glied); die Textur laeuft dort durch.
5. Der Einbau ins Spiel-Asset folgt erst nach der Nutzer-Abnahme. Bis dahin liegt die Variante im Scratchpad als sfv4/emd/EM30_V4.EMD
   (= EM30_V4ot.EMD, a9da924f), reproduzierbar mit "birkin_formen.py --variante v4 --aus DIR". Runde 5 liegt als EM30_V4r5.EMD (a762bd66).


## V4 Originaltexturen - Runde 2 (Bauer, Runde 2 von 2, 2026-11-07)

Grundlage: Sicht-Pruefung Runde 1 (SF_PRUEFUNG_REF2TEX_r1_sicht.md, Commit 29debe4b1), Ergebnis "nicht bestanden" mit drei Befunden
(b1, b2b, c2). Orchestrator Runde 2: (a) Kopf-Grundton = blasse Ref2-Haut, dunkel nur das Haarfeld, keine violettstichigen Eintraege,
keine Nachfuehrung an Video-L*-Zielen, Abnahme gegen Ref2; (b) Schwanzwurzel ohne Streifenblock, Riegel mit Anisotropie-Mass je
Teilflaeche; (c) was in Runde 1 wie Vorlage oder klein war, bleibt. Der Zahlen-Pruefer der Runde 1 fiel dem Modell-Limit zum Opfer.

### Befunde und Ursachen (gemessen)
| Befund | Ursache (gemessen) | Aenderung |
|---|---|---|
| (b1) Kopf fast schwarz, violettstichig (L* P50 11,4, 80 % L* < 20, 23 % b* < 0) | 1. Die Zonenhelligkeit wurde an Video-L*-Ziele nachgefuehrt (K5/K7/K8, 13-18 L*). 2. Die Runde-5-Zonenkarte legt die Zone s_haar (Haar der SEITENansicht) auch ins Vorderbild, dort auf 569 Texel der Gesichtsmitte (u 10-35, v 18-46). Pose 85 sieht diese Flaechen nur als Streifen an der Kopfvorderkante (Kenn-Render, scratchpad r2ot/sicht.py). 3. Die Klassen enthielten 21 violettstichige Eintraege (b* < 0) der Zeile 480. | Grundton = die Original-Kopftexel von Ref2 in IHRER Zeile 485 (Lab), je Texel der naechste Eintrag der Zeile 480. Keine Nachfuehrung an Videowerten. Haar nur auf den Texeln, die Pose 135 im Fenster KV2 und an der K6-Stelle zeigt (haar_vorn), seitlich nur im Fenster KS2 (haar_seite). Violettstichige Eintraege (b* < 0, oder a* > 8 mit b* < 2) entfallen. |
| (c2) Haarfeld bedeckt 80 %, Nase nicht erkennbar | Folge von (b1) | wie (b1); Nase (Texel im Fenster KV3) mit hellem Nasenruecken (NASE_F) |
| (b2b) Wurzel Glied 16 oben senkrecht gestreift (waagerecht 7,45 / senkrecht 4,36) | Kenn-Render u/v (scratchpad r2ot/wurzel.py): die oberen 14 Zeilen von Id 16 zeigen v 19-53 ueber 33 px waagerecht (1 Texel je px), aber u nur 0-5 ueber 12 Zeilen. Die Hautstruktur erscheint dort ~3-fach laengs gestaucht. Am Rand (u 0-1) bestimmte zudem eine einzelne Quellspalte (Ader/Licht) den ganzen Streifen. | Im Wurzelband (v < 56) die Quelle laengs gedehnt (WURZEL_K 0,55), und je Texel die mittlere Farbe der ueberdeckten Quellspalten; ab Texelzeile 56 texelgleich Runde 1 (Glieder 17-22, Vorgabe (c)). |

Ein erster Versuch mit gleichmaessiger Umfangs-UV (u nach dem Winkel, birkin_formen) wurde verworfen und nicht committet. Er hob die
Wurzel nicht (oben 14 Zeilen 1,60) und veraenderte die Glieder 17-22 (Id 20-22 1,6-1,8 statt 1,3-1,5).

### Aenderungen (Commits)
- fd2cb8534 **ref2tex_v4.py** Runde 2 (main; Runde 1 bleibt als main_r1), farben_ref2tex_v4.npz, Protokoll ref2tex_protokoll.json,
  **ref2tex_gitter.py** mit SF_belege/v4/ot2/ref2tex_gitter.txt.
- b08d1c79a **riegel_ref2tex.py**: T-K4 gegen Ref2 statt Video, T-K5 neu, T-K3d neu, tk4_r1 fuer die Gegenprobe. Belege
  SF_belege/v4/ot2/, Leisten SF_belege/v4/01-05, 07, 08, ctest.
- 7baaa0eec **SF_MESSBLATT_V4.md**: Nachtrag Runde 2 in Abschnitt 5, Ersatztabelle in Abschnitt 7, ueberholte Ziele markiert.
- a5db68416 Leisten der Runde 1 nach SF_belege/v4/ot1/.
- 29debe4b1 Pruefbericht Sicht Runde 1.
- Geometrie, UV und birkin_formen.py unveraendert; Spiel-Asset EM30_HANGAR.EMD (cfd1ac42) und Pin unveraendert. Die Variante liegt
  im Scratchpad: sfv4/emd/EM30_V4.EMD = EM30_V4ot2.EMD (sha256 415a921f436fe8e1...); Runde 1 = EM30_V4ot.EMD (a9da924f), Runde 5 =
  EM30_V4r5.EMD (a762bd66). Reproduzierbar: "python ref2tex_v4.py" und dann "python birkin_formen.py --variante v4 --aus DIR".

PORT-WAHLen (kein RE-Befund, keine @0x-Konstante; je mit Lauf im Code-Kommentar):
- aus dem Gitter (36 Punkte, 6 ohne rote Zeile, gewaehlt der erste): S_VORN 1,2, S_SEITE 1,0, HOF_F 0,62, HOF_R 8, HOF_MIN 29;
- KNIE 30 / 0,4, HAUT_L2 44, HAAR_F 0,5, KUPPE_DA 6 / KUPPE_L 1,0, KINN_F 1,15 / KINN_L 56, NASE_F 1,1;
- HAAR_SIGMA 1,5 / HAAR_STRUKTUR 0,8 (Haarrand folgt der Ref2-Kopfstruktur, kein Rechteck), AUGE_KERN_S 0,15, HOF_F_S 0,9;
- WURZEL_K 0,55 (Rampe 50-56).

### Riegel (riegel_ref2tex.py, Lauf sfv4/r/V4ot2 mit 191 FRAMEDUMPs)
Ausgabe: SF_belege/v4/ot2/riegel_ref2tex_V4ot2.txt/.md/.json. **Exit 1: 1 rot von 49** (T5, Geometrie seit Runde 5), 8 INFO-Zeilen.

| Mass | Soll | Ist Runde 2 | Ist Runde 1 (Gegenprobe) |
|---|---|---|---|
| T-K1a/b/c Koerper = Ref2 | bytegleich | OK, 0 von 34716 Texeln | OK |
| T-K2 keine neue Farbe | Ref2-CLUT | OK, 0 von 14008, 161 Farben | OK |
| T-K3a/b/c Schwanz | Quernaht, Bloecke, Wurzelkante | OK (13,0 <= 14,0; Std 8,8-14,6, 86-154 Indizes; Sprung 4,7) | OK |
| T-K3d Wurzel Id 16 waagerecht/senkrecht | je Teilflaeche in 0,67-1,44 (Rumpf Id 8 / Oberarm Id 13) | OK: ganz 0,81, oben 14 Zeilen 1,29 (1,92 / 1,49), Quadranten 0,71-0,99 | **ROT**: oben 14 Zeilen 1,70 (7,41 / 4,36) |
| T-K4 K1 Auge vorn | 1 Komponente, Kontrast >= 20 | OK 1; 20,7 | OK 20,7 |
| T-K4 K2 Augenfleck | L* >= KV5 + 20, blass | OK 54,5/6,9/2,7 (KV5 23,8) | OK |
| T-K4 K3 / K4 | Groesse / Lage | OK 0,030 x 0,033 H; 0,054 H links, 0,095 H unter OK | OK |
| T-K4 K5 Haarfeld | <= Auge - 25 und <= 22,8 | OK 12,9 | OK 14,2 |
| T-K4 K6 Spiegelstelle | L* <= 18 | OK 10,1 | OK 11,5 |
| T-K4 K7 Nase | zwischen Haar und Auge, >= Haar + 8 | OK 21,7 (Haar 12,9) | **ROT** 16,4 (Haar 14,2) |
| T-K4 K8 Kinn | Albedo >= 29,2; dE00 <= 10 zu Ref2 | OK Albedo 34,8; dE00 5,7 | **ROT** |
| T-K4 K9 / S4 Kuppe | a* >= 11; dL* zu Ref2 <= 8 | OK 38,0/22,5/7,7; 36,8/16,0/3,9 | **ROT** K9 22,7 L* |
| T-K4 K10, S1, S2 | wie Messblatt | OK (0,154 x 0,187 H; S1 1 Komp. 0,042 x 0,049 H; S2 0,050 / 0,818 H) | OK |
| T-K4 S3 Haarkante seitlich | KS2 L* <= 22,8; Abstand <= 0,01 H | OK 22,0; 0,000 H | OK |
| T-K5a Grundton L* P50 ausserhalb Haarfeld, vorn / seitlich | Ref2 30,8 +- 4 | OK 30,9 / 34,2 | **ROT** 13,5 / 20,7 |
| T-K5b Grundton/Rumpf, vorn / seitlich | >= Ref2 - 0,2 (1,18 / 1,06) | OK 1,07 / 1,05 | **ROT** 0,57 / 0,66 |
| T-K5c Anteil b* < 0, vorn / seitlich | <= 0,02 | OK 0,000 / 0,000 | **ROT** 0,257 / 0,206 |
| T-K5d dunkle Textur ausserhalb Haarfeld | <= Ref2 + 0,05 (0,00) | OK 0,03 / 0,00 | **ROT** 0,86 / 0,60 |
| INFO ganzer Kopf mit Haarfeld, vorn | Ref2 30,8 / 1,28 / 0,12 | L* P50 23,9, Kopf/Rumpf 0,87, L* < 20 0,46 | 11,5 / 0,52 / 0,85 |
| G1, M1-M4, T1-T4, TA, T5q, T6, TK, T7a-d, Boden | wie Riegel V4 | OK | - |
| T5 | f 0,5 >= 0,100 H | **ROT** 0,098 (Geometrie seit Runde 5) | ROT 0,098 |

Gegenproben (SF_belege/v4/ot2/riegel_ref2tex_*_gegenprobe.*):
- V4 Runde 5 (a762bd66): 19 rot, darunter T-K1a/b/c und T-K2.
- Runde 1 (a9da924f): 13 rot, darunter T-K3d und alle T-K5.

### Leisten (SF_belege/v4/; Runde 1 in v4/ot1/, Runde 5 in v4/r5/)
Spalten Nutzer-Vorlage | Nutzer-Beschriftung | Referenz 2 (gelobt) | V4 neu (Originaltexturen):
- 01_vorn_kamera12
- 02_seite_wie_bild2
- 03_kopf_nah_vorn_x4
- 04_kopf_nah_seite_x4
- 05_schwanz_x3
- 07_gang: Laeufe sfv4/r/R2ot und sfv4/r/V4ot2.
- 08_rundum_V4

Am Bild:
- Kopf vorn: rosa-braune Ref2-Haut mit der Feinstruktur der Original-Kopftextur, rote Kuppe und blasser, unregelmaessiger Augenfleck
  (B)-links. Das dunkle Haarfeld liegt (B)-rechts daneben; sein Rand folgt der Hautstruktur.
- Kopf seitlich: rosa Haut, rote Oberseite, blasser Augenfleck an der Vorderkante, dunkle Haarkante davor.
- Wurzel: die Oberseite ist ein ruhiger, grau-rosa Streifen ohne gesaettigte Adern (scratchpad r2ot/wurzel_vgl.png, oben Runde 1,
  unten Runde 2).
- Glieder 17-22 und der Koerper sind unveraendert.

### Tests
- ctest (unit_birkin_formen, unit_birkin_szenen, unit_schwanz_r41, integration_r41_sichel_form): "100% tests passed, 0 tests failed
  out of 4" (64,9 s; SF_belege/v4/ot2/ctest_v4_ot2.txt).
- Bau "LOCAL-BUILD-OK (build)". Engine unveraendert.

### Offen
1. **T5 bleibt rot.** Grund ist die Geometrie, die nach Vorgabe 4 unveraendert bleibt: f 0,5 = 0,098 H bei Grenze 0,100. T5q quer zur
   Pruefer-Mittellinie ist gruen.
2. **Ganzer Kopf vorn dunkler als Ref2.** Mit Haarfeld liegt der ganze Kopf vorn bei L* P50 23,9 und Kopf/Rumpf 0,87 (Ref2 30,8 / 1,28).
   Das Haarfeld nimmt 26 % der Kopfpixel ein, so gross wie die Messblatt-Fenster KV2 und die K6-Stelle; dort verlangen K5/K6 Dunkel.
   Ausserhalb des Haarfelds:
   - Grundton 30,9 (Ref2 30,8), Albedo 36,7 (Ref2 37,2);
   - Render-Anteil L* < 20 bei 0,30 (Ref2 0,18). Das ist Schattierung der V4-Kopfform, deren Geometrie seit Runde 5 unveraendert ist;
     die Textur ist dort nicht dunkel (T-K5d Albedo 0,03).
3. **K1 haelt nur mit einem Augenhof.** Mit Hof steht K1 bei 20,7 (Grenze 20), ohne Hof bei 12,1-13,9. Der hellste blasse Eintrag der
   Zeile 480 hat L* 55, die Ref2-Haut um das Auge ist blass. Der Hof dunkelt die Haut um den Augenkern auf hoechstens 0,62 ab, mit
   Boden Texel-L* 29; er ist kein dunkler Ring (T-K5d).
4. **T-K3d an der Wurzeloberseite.** Die oberen 14 Zeilen stehen bei 1,29, innerhalb der Rumpf-/Oberarm-Spanne, aber ueber dem
   Pruefer-Richtwert 0,8-1,0. Die Spruenge sind dort klein (1,92 / 1,49 gegen 7,41 / 4,36 in Runde 1).
5. Die Glieder 20 und 22 haben Anisotropie 1,35-1,46 (INFO). Sie sind ab Texelzeile 56 texelgleich Runde 1, die die Sicht-Pruefung als
   "klein" bewertet hat (Vorgabe (c)).
6. Der Einbau ins Spiel-Asset folgt erst nach der Abnahme. Nach dem Nutzerentscheid ist dies die letzte Nachbesserungsrunde vor
   v0.8.30.

## V4 Kopf nach Nutzerbeschreibung (Bauer, Runde 3, 2026-11-07)

Grundlage: der Kopf der Runde 2 (EM30_V4ot2.EMD, sha256 415a921f; Leiste SF_belege/v4/ot2/03_kopf_nah_vorn_x4.jpg) wurde vom Nutzer
abgelehnt. Nutzerentscheid: letzte Runde dieser Spur vor v0.8.30; eine Umsetzung genau dieser Lesung, keine Experimente.

### Nutzer-Wortlaut
- 2026-11-07 ~17:40: "die Form baut von Farben und Texturen her ja nun wirklich auf die Urspruengliche Birkin Form auf.... Die Farben
  koennen doch dann im Prinzip garnicht so scheisse aussehen"
- 2026-11-07 abends (nach dem Kopf der Runde 2): "Nein, es passt eben nicht.... ich weiss nicht wie ich das noch beschreiben soll.... da ist
  links ein Auge. Linke Seite ist so graeulich. Bis Nase. Dann rechts Seite sieht man Haare - braeunlich.... Warum mischst du die Farben so
  daemlich?"
- Abgelehnt (Runde 2): rosa-roter Kopf mit lachsroter heller Kuppe (L* 38), weissem Augenfleck, fast schwarzem rechteckigem Haarblock (L* 9)
  und rosa Haut - vier Farben nebeneinander.

### Lesung (bindend; Seiten = Bildkoordinaten der Spielkamera 50A0 Cut 12 wie Beschreibung1)
1. Linke Bildhaelfte = graeuliche Haut mit dem Auge (heller, unbunter Fleck, Ort K4), bis zur Nase (K7) - die Nase ist die Grenze.
2. Rechte Bildhaelfte = Haare, braeunlich (a* > 0, b* > 0, nicht schwarz), etwa 25 % dunkler als der Kopf-Median (Video 12,9 / 17,3), eine
   Flaeche mit weichem, strukturfolgendem Rand; seitlich bleiben Auge (S2) und Haarkante (KS2) an den gemessenen Orten.
3. Keine Farbmischung: Kopfoberseite dunkel rotbraun (hoechstens Kopf-Median + 6 L*), nahtlos zu den Haaren.
4. Farbquelle zuerst die Ref2-Palette; Ableitungen nur bei Bedarf und belegt. Nie Texel an Video-Farbwerte eichen.

### Seitenbestimmung (Id-Puffer, ref2tex_v4.geometrie_r3, Protokoll 'geometrie')
- Pose 135 (Kamera 50A0/12, Kopfbox x 325-380, y 154-221): die lokale z-Koordinate des Kopf-Meshes (Mesh 9; z > 0 = (W)-links, KOPF_V4)
  ist bei den Kopfpixeln LINKS der Kopfmitte im Median 14,3 E., RECHTS 215,4 E. Also: **Bild-rechts = (W)-links = Haare, Bild-links =
  (W)-rechts = graue Seite mit dem Auge** - wie die Messblatt-Konvention "(B)-links = (W)-rechts" (Abschnitt 1).
- Nasenebene: Mitte des Messblatt-Fensters KV3 (Bild 354,0 / 203,0) liegt im Kopfraum bei (498,5 / -196,0 / 185,1) E.; die Ebene enthaelt
  Sehstrahl und Bild-oben der Pose 135 (Normale lokal (-0,316 / 0,521 / 0,793), Bild-rechts positiv). 13,2 / 11,9 E. je Bildpunkt an der
  Nase. Auge K4 (Bild 334,3 / 187,9) liegt im Kopfraum bei (336 / -331 / -43): grau-Seite, 150 E. von der Ebene.
- Haarkante aus Beschreibung2 (KS2, Pose 85): die 82 Bildpunkte des Fensters KS2 zeigen 77 Texel des VORDERbilds (kein Seitenbild-Texel);
  dieselben Texel erscheinen in Pose 135 zwischen Auge und Nase (Bbox x 333-350, y 183-207, Schwerpunkt 340,5 / 194,4; Scratchpad
  ot3/explore3.py). Die Haarkante der Seitenansicht und die graue Zone "Auge bis Nase" der Vorderansicht sind also dieselben Texel. Nach der
  Lesung (1) bleiben sie grau; der dunkle Streifen in KS2 entsteht - wie schon in Runde 2 - aus dem Augenhof und dem streifenden RDT-Licht
  an der Kopfvorderkante (KS2 roh L* 14,9; S3 Videoraum 23,1 <= Kopf-Median 37,0 - 5). Ein Haarfleck dort haette der Lesung (1)
  widersprochen (Nutzer: "Warum mischst du die Farben so daemlich?").

### Farbquelle (Ref2-Zeile 480, Eintraege, die Referenz 2 auf Koerperflaechen zeigt; 226 Eintraege, 22 violettstichige ausgenommen)
Klassen disjunkt (ref2tex_v4.klassen_r3; Lab der Texelfarbe, Liste im Protokoll 'klassen' / 'klassen_lab'):
- **grau** (32): Chroma <= 7,5, L* >= 26 - z. B. 72/64/64 (27,9), 72/72/72 (30,6), 88/72/72 (32,2), 88/80/80 (34,8), 88/88/88 (37,4),
  96/88/88 (38,1), 104/88/88 (38,9), 104/96/96 (41,5), 112/104/104 (44,7), 120/112/112 (47,9) ... 144/128/128 (55,0).
- **braun** (13): a* > 0, b* > 0, b* >= 0,6 a*, L* 12-40, Chroma <= 17 - 40/32/24 (12,9/2,3/6,9), 48/32/16 (13,8/5,5/13,5),
  56/40/24 (17,6/5,1/13,4), 56/48/32 (20,3/0,7/11,6), 64/56/40 (23,9/0,6/11,3), 72/56/40 (24,8/4,5/12,7), 96/64/56 (30,5/12,8/10,5),
  88/72/56 (31,8/4,1/12,2), 88/72/64 (32,0/5,4/7,3), 104/80/72 (36,3/8,7/8,6), 112/88/80 (39,6/8,6/8,4).
- **kuppe** (9, dunkel rotbraun): a* 12-22, b* >= 0,4 a*, L* 20-36, Chroma > 17 - 80/40/40 (21,5/18,6/8,4), 88/40/32 (22,6/21,3/15,5),
  88/48/48 (25,0/18,2/8,0), 96/48/48 (26,2/21,7/9,9), 104/64/64 (31,8/17,4/7,3), 112/72/48 (34,6/14,3/21,3), 112/72/72 (35,1/17,1/7,1).
- **hell** (12, Auge; Teilmenge von grau): L* >= 45, Chroma <= 7,5 - Kern = hellster Eintrag 144/128/128 (55,0/6,1/2,2).
Die Ref2-Palette reicht fuer "graeulich" und "braeunlich"; **keine Ableitungen** angelegt (Protokoll ableitungen = [], T-K2 0 Ableitungs-
Texel). Haut- und Schwanzfarben unveraendert.

### Aenderungen
- ref2tex_v4.py Runde 3 (main; Runde 2 als main_r2): Zonen aus der Kopfgeometrie (tex3d: Ort je Texel aus den UV-Flaechen von Mesh 9;
  Nasenebene s; Kuppe = Ellipse um die rote Zone des Messblatts im Bild der Pose 135 (Mitte 0,60 / 0,17, Halbachsen 0,19 / 0,15 der Kopfbox)
  bzw. Pose 85 (0,32 / 0,16; 0,21 / 0,12), Halbachsen x 1,3), Raender strukturfolgend (Ref2-Kopfstruktur Gauss 2 Texel, 200 E. Hub, Rampe
  8 E.), Haaransatz als Saum (25 E., Faktor 0,6, nicht unter 0,85 der Kopfbox). Helligkeit = oertliche Struktur der Ref2-Kopftexel
  (L / Gauss 6, Knie 0,4) x 40 (Ref2-Kopf-Albedo 37,2 bei Render 30,8); Haar 0,8 x Grau, Kuppe 0,9 x Grau, Nase (KV3, Grau-Seite) 0,9.
  Auge wie Runde 2 mit Hof nur um das Auge der eigenen Kamera (vorn 0,5 / 6 Texel; seitlich 0,7, Kernschwelle 0,10). Eintragswahl mit
  a*/b*-Gewicht 0,15 (braun / kuppe) bzw. 0,4 (grau). Schwanz texelgleich Runde 2 (assert). Alle Zahlen PORT-WAHL, Laeufe 1-9 im
  Code-Kommentar; kein RE-Befund.
- riegel_ref2tex.py: T-K4 nach der Lesung (K5, K6, K7, K8, K9, S3, S4), T-K5 Grundton = graue Zone, NEU T-K9 Zonen (a-f) und T-K8
  Aenderungsumfang (--vorher), T-K2 "Ref2-Farbe oder belegte Ableitung". Zonen am EMD = Klasse des Texel-Index (zonen_emd), so sind sie
  auch an Gegenproben messbar.
- leisten_ref2tex.py: Runde-3-Texte, neue Leiste 09_kopf_zonen_x6.
- SF_MESSBLATT_V4.md: Nachtrag Runde 3 (Abschnitt 5, Ersatztabelle Abschnitt 7).
- Nicht geaendert: birkin_formen.py, Geometrie/UV aller Meshes, Koerper-Texel, Schwanz-Texel, CLUT, Spiel-Asset EM30_HANGAR.EMD
  (cfd1ac42) und Pin, Engine. T-K8: Sektionen 0-7 und CLUT bytegleich zu EM30_V4ot2, Texel geaendert nur in den Kopf-UV-Flaechen (vorn
  2989 von 3072, seite 3413 von 3584; ausserhalb 0).
- Variante: sfv4/emd/EM30_V4ot3.EMD = EM30_V4.EMD (sha256 a1bc9309e5dd9ce8a948371ec4fef8595bd86310e3ea5e01dc3326552d5f8998,
  262004 Byte); Vorstand EM30_V4ot2.EMD (415a921f) behalten. Reproduzierbar: "python ref2tex_v4.py", dann
  "python birkin_formen.py --variante v4 --aus DIR".

### Zonen-Masse (Ziele aus dem Auftrag; Texel = Lab der gewaehlten Eintraege, Render = Pose 135 roh, RDT-Licht; Protokoll 'zonen_texel' /
'zonen_bild', Riegel T-K9)
| Ziel | Soll | Ist Texel (P50) | Ist Render (Pose 135, roh) |
|---|---|---|---|
| graue Zone Chroma | <= 0,5 x Rumpf (Rumpf Id 8 7,8 -> 3,9) | 3,5 (34,8/3,3/1,2) | 3,5 |
| graue Zone L* | um Ref2-Kopf P50 30,8 (+- 5) | 34,8 | 27,9 |
| Haar L* | 0,62-0,90 x Kopf-Median ("um 22-24") | 31,8 (31,8/4,1/12,2) | 18,4 = 0,67 x Median 27,4 |
| Haar a* / b* | > 0 / > 0 | 4,1 / 12,2 | 3,3 / 7,5 |
| Haarflaeche (Haar + Kuppe) | 0,40-0,55 der Kopfpixel | - | 0,52 (Haar 0,36, Kuppe 0,16) |
| Rand | Gradient 3-8, Kante Std > 4 px | - | 3,2 (704 Randpixel); 6,9 px (61 Zeilen) |
| Kuppe L* | <= Median + 6 (33,4), >= Haar | 31,8 (31,8/17,4/7,3) | 31,8; a* 17,4, b* 7,3 |
| Auge | heller unbunter Fleck am Ort K4 | Kern 55,0/6,1/2,2 | K1 1 Komp., Kontrast 20,2; K2 54,5/4,9/1,9 (Videoraum); K3 0,030 x 0,033 H; K4 0,054 H links, 0,095 H unter OK; Zone grau |
| Nase | K7 mitteldunkel an der Grenze | Grau-Soll x 0,9 | KV3 23,2 (Videoraum; KV5 25,3, Haar rechts 17,7); Fenster KV3 0,37 grau |
| seitlich S2 / KS2 | gemessene Orte | - | S2 0,051 H / 0,817 H; S1 0,045 x 0,049 H; KS2 roh 14,9, Videoraum 23,1 (<= 32,0) |
| ganzer Kopf Pose 135 | L* P50 >= 23, Kopf/Rumpf >= 0,9 | - | 27,4; 0,99 (Ref2 30,8 / 1,28) |
| Rundum +120 / +180 | kein Fleck L* < 15 > 100 px | - | 2 px / 0 px |

### Riegel (riegel_ref2tex.py, EM30_V4ot3.EMD, --vorher EM30_V4ot2.EMD, --lauf sfv4/r/V4ot3 mit 191 FRAMEDUMPs)
Ausgabe: SF_belege/v4/ot3/riegel_ref2tex_V4ot3.txt/.md/.json. **Exit 1: 1 rot von 57** (T5, Geometrie seit Runde 5), 8 INFO-Zeilen.

| Mass | Soll | Ist Runde 3 | Gegenprobe Runde 2 (EM30_V4ot2) |
|---|---|---|---|
| T-K1a/b/c Koerper = Ref2 | bytegleich | OK, 0 von 34716 Texeln | OK |
| T-K2 Ref2-Farbe oder belegte Ableitung | 0 Texel ausserhalb | OK 0 von 14008, 160 Farben, 0 Ableitungen | OK |
| T-K3a/b/c/d Schwanz | wie Runde 2 | OK (texelgleich Runde 2: 13,0 <= 14,0; Std 8,8-14,6; Sprung 4,7; Anisotropie 0,71-1,29) | OK |
| T-K4 K1 Auge vorn | 1 Komp., Kontrast >= 20 | OK 1; 20,2 | OK 20,7 |
| T-K4 K2 Augenfleck | >= KV5 + 20, hell und unbunt | OK 54,5/4,9/1,9 (KV5 25,3) | OK |
| T-K4 K3 / K4 | Groesse / Lage | OK 0,030 x 0,033 H; 0,054 H links, 0,095 H unter OK | OK |
| T-K4 K5 Haarfeld (KV2) | 0,62-0,90 x Median (20,2-29,3), a*/b* > 0 | OK 28,0/4,6/7,2 | **ROT** 12,9 |
| T-K4 K6 Spiegelstelle | in der Haarflaeche, <= Median - 3, kein Fleck | OK Mittel 23,1, max 31,6 | OK |
| T-K4 K7 Nase (KV3) | Wange +- 4, >= Haar rechts + 1 | OK 23,2 (KV5 25,3, KV3r 17,7) | OK |
| T-K4 K8 Kinn | Albedo >= 29,2, b* >= 0 | OK Albedo 31,9 | OK |
| T-K4 K9 / S4 Kuppe | a* >= 8, b* > 0, <= Median + 8 | OK 37,3/14,4/6,7; 34,5/10,1/4,0 | **ROT** K9 38,0 > 35,6 |
| T-K4 K10, S1, S2 | wie Messblatt | OK (0,154 x 0,187 H; S1 1 Komp. 0,045 x 0,049 H; S2 0,051 / 0,817 H) | OK |
| T-K4 S3 Haarkante seitlich | KS2 <= Median (85) - 5 (32,0); Abstand <= 0,01 H | OK 23,1; 0,000 H | OK |
| T-K5a Grundton graue Zone, vorn / seitlich | Ref2 30,8 +- 4 | OK 27,9 / 30,6 | OK / **ROT** 37,0 |
| T-K5b Grundton/Rumpf | >= Ref2 - 0,2 | OK 1,10 / 0,98 | OK |
| T-K5c violett | <= 0,02 | OK 0,000 / 0,000 | OK |
| T-K5d dunkle Textur in der grauen Zone | <= Ref2 + 0,05 | OK 0,03 / 0,00 | **ROT** 0,23 |
| T-K9a graue Zone | Chroma <= 3,9; L* 30,8 +- 5; Lage | OK Chroma 3,5; 27,9; links 0,83; Kopf links grau 0,73; 0,44 der Kopfpixel | **ROT** |
| T-K9b Haar | 17,0-24,7; a*/b* > 0; Flaeche 0,40-0,55 | OK 18,4 (0,67 x); 3,3 / 7,5; 0,52 | **ROT** 6,2 (0,26 x), Flaeche 0,32 |
| T-K9b Rand / Kanten | 3-8; Std > 4 px | OK 3,2; 6,9 px | OK (7,8; Block) |
| T-K9c Kuppe | <= 33,4, >= Haar; a* >= 8 | OK 31,8, a* 17,4, b* 7,3; 0,16 | OK |
| T-K9d Auge / Nase / Seite | grau, K4, KV3 0,3-0,7, S2, KS2 | OK Zone 1; 0,37; KS2 14,9 | **ROT** |
| T-K9e ganzer Kopf | P50 >= 23, Kopf/Rumpf >= 0,9 | OK 27,4; 0,99 | **ROT** 23,9; 0,87 |
| T-K9f Rundum | kein Fleck L* < 15 > 100 px | OK 2 / 0 px | **ROT** 494 / 360 px |
| T-K8 Aenderungsumfang | Sektionen/CLUT gleich, Texel nur Kopf | OK vorn 2989, seite 3413, ausserhalb 0 | - |
| G1, M1-M4, T1-T4, TA, T5q, T6, TK, T7a-d, Boden | wie Riegel V4 | OK | OK |
| T5 | f 0,5 >= 0,100 H | **ROT** 0,098 (Geometrie seit Runde 5, unveraendert) | ROT 0,098 |

Gegenprobe gesamt (SF_belege/v4/ot3/riegel_ref2tex_V4ot2_gegenprobe.*): **10 rot von 51** - die Runde-2-Fassung faellt an T-K9a/b/d/e/f,
K5, K9, T-K5a (85), T-K5d (135) und T5.

### Leisten (SF_belege/v4/; Runde 2 in v4/ot2/, Runde 1 in v4/ot1/)
Spalten Nutzer-Vorlage | Nutzer-Beschriftung | Referenz 2 (gelobt) | V4 neu (Runde 3); roh, gleiches Licht; keine Spalten frueherer
V4-Runden:
- 01_vorn_kamera12, 02_seite_wie_bild2, 03_kopf_nah_vorn_x4, 04_kopf_nah_seite_x4, 08_rundum_V4,
- 09_kopf_zonen_x6 (neu): Vorlage-Kopf x6 mit den Nutzerpunkten (Auge gelb, Haare / Nase cyan) | Beschreibung1 | Ref2-Kopf | neu mit
  Messpunkten; Zeile 2 neu mit Zonenkarte grau / braun / Kuppe und den T-K9-Zeilen.
- 05 (Schwanz) und 07 (Gang) unveraendert gegenueber Runde 2 (T-K8: Schwanz-Texel und Geometrie bytegleich; Spiellauf sfv4/r/V4ot3 mit
  191 FRAMEDUMPs, T7a-d und Boden gruen) - nicht neu erzeugt.
Am Bild (03 / 09): links graue Haut mit dem hellen Augenfleck, rechts eine braune Haarflaeche mit weichem, der Kopftextur folgendem Rand,
oben rechts die dunkel rotbraune Kuppe; keine rosa Haut und kein schwarzer Block mehr. Seitlich (04): grauer Kopf, heller Augenfleck,
dunkle Vorderkante, rotbraune Oberseite. Spielbild (RE15_FRAMEDUMP h135, sfv4/r/V4ot3): gleiche Verteilung.

### Tests
- ctest (unit_birkin_formen, unit_birkin_szenen, unit_schwanz_r41, integration_r41_sichel_form): "100% tests passed, 0 tests failed
  out of 4" (64,44 s; SF_belege/v4/ot3/ctest_v4_ot3.txt).
- Bau "=== LOCAL-BUILD-OK (build)". Engine unveraendert.

### Offen
1. **T5 bleibt rot** (f 0,5 = 0,098 H bei Grenze 0,100): Geometrie seit Runde 5, nach Vorgabe nicht angefasst. T5q gruen.
2. **Haar im Render dunkler als "um 22-24"**: Pose 135 L* 18,4 = 0,67 x Kopf-Median (Band 0,62-0,90 gruen). Die Haarseite liegt im Schatten
   des RDT-Lichts (Lauf 1 mit texelgleich hellem Haar schon 0,75 x Median); die Texel sind nur 0,8 x so hell wie die graue Haut (31,8 gegen
   34,8). Heller ginge nur mit Haar-Texeln heller als die Haut.
3. **Graue Zone im Render 27,9** statt Ref2 30,8 (T-K5a +- 4 gruen, T-K9a +- 5 gruen): die Eintragswahl zieht zu den neutralen grauen
   Eintraegen; Basis 40 statt 37,2 gleicht das nur zum Teil aus.
4. **Randgradient 3,2** (Soll "um 6"): ein Saum von 0,6 am Haaransatz liefert 3,2; die 6 der Vorgabe stammen aus den Kanten fast schwarzer
   Wundflecken der Ref2-Textur. Der Rand ist bewusst weich ("Rand weich und strukturfolgend").
5. **KS2 ist kein Haar**: die Haarkante der Seitenansicht liegt auf denselben Texeln wie "Auge bis Nase" der Vorderansicht (s.
   Seitenbestimmung); sie bleibt grau, dunkel durch Hof und Licht. S3 gruen.
6. Der Einbau ins Spiel-Asset folgt erst nach der Abnahme; nach dem Nutzerentscheid wird nicht weiter poliert.

## V5 Kopf aus RE2 (KI-Bilder) (Bauer, Runde 1 von 2, 2026-11-07)

Grundlage: der Kopf der Runde 3 (EM30_V4ot3.EMD, sha256 a1bc9309) ist ein eigener Klotz mit gemalten Zonen. Nutzerentscheid heute Abend: der
Kopf kommt aus Resident Evil 2 (Birkin-Formen in CDEMD0.EMS), die KI-Bilder sind die Vorlage fuer Kopf, Maul, Auge, Haare, Schwanz.

### Nutzer-Wortlaut (bindend)
- "Gerade bei den Sichel Kopf verstehe ich es einfach nicht.... bei den KI Bildern kann man so verdammt gut sehen wie das verschwommene Bild
  eigentlich aussehen muss.... Man sieht alles einwandfrei! Das Auge die Haare, den Mund, den Schwanz.... Ich raffe wirklich nicht, warum du das
  nicht genauso hin bekommst...."
- "Vielleicht kannst du dir auch einen Birkin aus Resident Evil 2 nehmen. Ich nehme an die duerften aehnlich aufgebaut sein die anderen Formen...."
- frueher heute: "die Form baut von Farben und Texturen her ja nun wirklich auf die Urspruengliche Birkin Form auf" (Koerper = Referenz 2) und
  "da ist links ein Auge. Linke Seite ist so graeulich. Bis Nase. Dann rechts Seite sieht man Haare - braeunlich...." (Beschreibung1).

### Leser-Ergebnisse (SF_belege/re2kopf/, committet 0b6d20c63)
- Leser A (00_koepfe_re2_vs_ki.png, koepfe.json, mess.json): die Nutzer-Annahme stimmt - 0x30/0x31/0x33 haben denselben Aufbau (8 dir-Eintraege,
  Kopf = Mesh 3 am Knochen 3, Kopftextur TIM page 0 / CLUT-Zeile 0). Formidentitaet am Render: 0x30 = G1 (Menschenkopf, Kittel, Augenarm,
  Rohr), 0x31 = G2 (Keilkopf, Brustgesicht, Dreifingerklaue), 0x33 = G3 (vier Arme, Schaedelkopf, Zahnmaul). Naechster Einzelkandidat zu
  KI-Front / Beschreibung1 / Beschreibung2: **G2-Kopf** (dE00 3,0 / 6,6 / 6,5; G1 12,4 / 15,2 / 10,2; G3 14,6 / 19,2 / 10,5; Ref2-Kopf 14,9 /
  19,4 / 15,6). Zaehne nur im G3-Kopf (Zahn-Anteil 0,0225); Haare nur im G1 (0,286). Der Ref2-Kopf (Mesh 9) ist ein rosa Klumpen ohne Auge,
  Maul oder Haar.
- Leser B (01_ki_merkmale.png, KI_LESUNG.md): glatte haarlose Kuppel mit Schnauze (Hai/Robbe), Achse 30-35 Grad vorn-unten, Kopf haengt vor dem
  Buckel; Maul offen an der Unterseite (0,07-0,10 x 0,05-0,06 H) mit zwei Reihen kleiner weisser Zaehne, Inneres dunkelrot; ein kleines dunkles
  Auge im oberen Drittel, mittig bis (B)-links (V2: zwei); "Haare" = (B)-rechte Kuppelhaelfte 9-21 L* dunkler, braun-grau, weicher Uebergang,
  keine Straehnen; roter Fleischkamm vom Scheitel ueber den Nacken; Schwanz: C-Bogen 1,2-1,4 H bis zum Boden (Widerspruch zu Beschreibung2,
  Entscheid noetig); Arme: kein Delta.

### Herkunft (byte-true, Zaehlung re2_leser.rec: Index ((kind-0x10)*4+rec)*2 der TOC re2_ems_toc.inc; Protokoll SF_belege/v4/re2/kopf_re2_protokoll.json)
| Teil | Quelle | Bytes | im EMD |
|---|---|---|---|
| Kopf-Geometrie (Mesh 9) | CDEMD0.EMS (sha256 8724200c...) kind 0x31 (G2) EMD Sektor 2944, 193124 B, sha256 bb966cd5...; MD1 dir[7] Mesh 3: 48 Ecken, 48 Normalen, 56 Dreiecke, 14 Vierecke | Ecken/Normalen transformiert (s. u.), Flaechen-Indizes gleich | Mesh 9 am Ref2-Knochen 9 (rel (111,-971,-133) unveraendert) |
| Kopf-Texel | G2 TIM Sektor 2895, 99872 B, sha256 e0929cef..., 384 x 256 8 bpp, Texel u 0-125 v 0-89 (11340 Texel, 6524 belegt) | 7519 Texel bytegleich, 3821 auf dunklere Eintraege DERSELBEN Zeile (s. Andunkeln) | TPage 128 (x 0-125), Zeilen 160-249 |
| Kopf-CLUT | G2 TIM CLUT-Zeile 0 (y 480), 256 Eintraege, 15-Bit-Rohwerte | bytegleich | Zeile 486 |
| Maul (11 Dreiecke des G2-Kopfes) | kind 0x33 (G3) TIM Sektor 3065, 99872 B, sha256 99b793e2..., Zahn-Kachel u 2-13 v 5-40 (432 Texel: obere Zahnreihe v 6-12, rote Kehle v 13-22, untere Zahnreihe v 22-28, Kinn v 30-40; Spalten 0-1 / 14-20 der Kachel tragen den blauen Stirnstreifen des G3, b* < -15, nicht uebernommen) | bytegleich | TPage 130 (x 256-267), Zeilen 200-235 |
| Maul-CLUT | G3 TIM CLUT-Zeile 0 (y 480) | bytegleich | Zeile 487 |
| Koerper Meshes 0-8, 10-15, Schwanz 16-22, Skelett, Clips, EDD | EM30_V4ot3.EMD (Sektionen 0-6 und alle Meshes ausser 9 bytegleich; Koerper = Referenz 2 cfd1ac42) | bytegleich | - |
Die alten Kopfflaechen der Runde 3 (TPage 128 u 0-47 / 56-111 v 160-223, 6656 Texel) sind geloescht (0); Schwanz-Texel unveraendert.
Maul-UV: die 11 unteren Vorderflaechen des G2 (T25, T26, T34, T35, T42, T44, T28, T29, T22, T43, T5 - unter den Augen, Normale +x, y -186..118
vor der Drehung) sind als Parallelprojektion (u aus z, v aus y) auf die Zahn-Kachel gelegt: obere Zahnreihe oben, Kinn unten. Das ist die
einzige UV-Aenderung am G2-Kopf (Riegel T-R1a: UV abweichend 0, Maul 11).

### Transformation (PORT-WAHL nach den Messblatt-Fenstern; Suche Scratchpad re2kopf_bau/grid1-4.log, Masse kopf_re2_mess.py)
q = Rz(+10 Grad) . Ry(-34 Grad) . 1,50 v + (140, +10, 0) E. im Raum des Knochens 9 (Normalen mitgedreht). Gewaehlt ist die Messblatt-Kopfbox
K10 (0,14-0,18 x 0,15-0,20 H), weil die Riegel T-R2 dort messen: Skala 1,50 -> K10 0,165 x 0,170 H (Leser A rechnete 1,577 auf 724 E.
Kopfhoehe; Leser B: Kuppel 0,13-0,15 x 0,15 H, mit Maul 0,19-0,22 H). Neigung 10 Grad nach unten: ab 20 Grad kippt die Schnauze aus der Kamera
50A0/12 (K10-Hoehe 0,137 H), das Gesicht der Beschreibung1 waere nicht mehr zu sehen (Leser B 30-35 Grad bleibt Offen). Gieren -34 Grad zur
(W)-linken Seite: das G2-Auge sitzt bei z -110 = 0,6 der halben Kopfbreite weit aussen; ohne Gieren laege es 0,08 H links der Kopfmitte
(Fenster 0,035-0,065). Versatz y +10: S2 Augenhoehe 0,812 H (mit y -20 lag sie bei 0,8203).

### Andunkeln (3821 von 11340 Texeln, nur Eintraege der G2-Zeile 486, kein Texel heller als die Quelle; Riegel T-R1b)
Ziel je Texel (L* x f, a* x g, b* x g), f = 1 - (1 - faktor) x w, g = 1 - 0,6 w, dann der Lab-naechste Eintrag derselben Zeile mit L* <= Quelle:
- Haar (Nutzer "rechts Seite sieht man Haare - braeunlich"; Leser B D4): (W)-linke Kopfhaelfte, w = glatt((z + 80) / 80), faktor 0,65; das
  (W)-rechte Auge (Flaechen T19/T20/T41, z -98..-125) bleibt unberuehrt; das (W)-linke Auge (T49/T55/T38/T40) liegt unter der Andunkelung
  (K6 "zweites Auge verdeckt": vorher 34 rote Bildpunkte a* > 25 Bild-rechts oben, Lauf c6).
- Nase (Nutzer "Bis Nase"; Messblatt K7 Nase mitteldunkel 18,2 L*): die leuchtende Mittelspalte der G2-Textur (u 41-60 = Gesichtsspalt,
  Flaechen Q1/T1/T33/T27/T32), faktor 0,50 - sonst laege der hellste Punkt des Gesichts in der Mitte statt am Auge (Messblatt Abschnitt 2).
- Spitze (Beschreibung2 KS2 dunkle Vorderkante, Messblatt Streifen L* 12,6): Flaechen mit Mittelpunkt x > 150..175 (Q0 173, T33/T31 177,
  T27 184, T0 192, T32 196, T2 204, T21 223), faktor 0,50; das Fenster KS2 zeigt in Pose 85 zu 0,93 die Flaeche Q0 (probe6).
- Mittel dL* -8,1 (P10 -15,6). Keine gemalten Augen, kein gemaltes Maul, keine Farbe ausserhalb der RE2-Zeilen.

### Kopfmasse (kopf_re2_mess.py, Render b2 Kamera 50A0/12, RDT-Licht; Auge = Marker-Render des (W)-rechten G2-Augenkerns auf T19/T20/T41,
Zaehne = Marker-Render der Zahn-Texel L* > 60 / Chroma < 20)
| Mass | Soll (Messblatt V4) | Ist neu | ot3 (Runde 3) |
|---|---|---|---|
| K10 Kopfbox vorn | 0,14-0,18 x 0,15-0,20 H | 0,165 x 0,170 H (Box 325-384 x 160-221, H 364) | 0,154 x 0,187 |
| K4 Auge vorn | 0,035-0,065 H links; 0,07-0,12 H unter OK | 0,060 / 0,097 H (19 px) | 0,054 / 0,095 |
| K7 Nase/Maul-Lage | Zahn-Schwerpunkt x 0,3-0,7 / y 0,55-1,0 der Kopfbox | 0,60 / 0,66 (58 px) | kein Maul |
| S2 Auge seitlich | 0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle | 0,043 / 0,812 H (16 px) | 0,051 / 0,817 |
| KS2 Haarkante seitlich | Kopf-Bildpunkte im Fenster L* <= Kopf-Median (Videoraum 22,5) - 5 = 17,5 | 14,8 (68 % des Fensters Kopf; ganzes Fenster 22,8) | 23,1 (ganzes Fenster) |
| Zaehne (T-R3) | >= 20 Marker-Bildpunkte, >= 5 helle unbunte | 58 Marker; 44 helle; Pose 85: 0 | 0 |
| CLUT-Zeilen (T-R4) | <= 16, Port-Texturhoehe <= 4096 | 8 Zeilen (480-485 Ref2, 486 G2, 487 G3); 256 x 8 = 2048 | 6 |
Kopf-Median Pose 135 roh L* 15 (Haelften links 15,4 / rechts 15,9); Ref2-Kopf 30,8, KI-Koepfe 23-26 (Leser B) - s. Offen.

### Riegel (riegel_re2kopf.py, EM30_V4re2.EMD, --basis EM30_V4ot3.EMD, --lauf sfv4/r/V4re2/debug.log mit 191 FRAMEDUMPs)
Ausgabe SF_belege/v4/re2/riegel_re2kopf_V4re2.txt/.md/.json. **Exit 1: 1 rot von 39** (T5, Geometrie seit Runde 5), 6 INFO-Zeilen.
| Mass | Soll | Ist |
|---|---|---|
| T-K1a/b/c Koerper = Ref2 | Meshes/Texel bytegleich, Zeilen 480-485 bytegleich, weitere = Ref2-Kopie oder RE2-Kopfzeile | OK: keine Abweichung; 0 von 34716 Texeln; weitere 486 = G2 Zeile 0, 487 = G3 Zeile 0 |
| T-K2s Schwanz Ref2-Farben | 0 Texel ausserhalb | OK 0 von 10356 |
| T-K3a-d Schwanz | wie Runde 3 | OK (texelgleich: 13,0 <= 14,0; Std 8,8-14,6; Sprung 4,7; Anisotropie 0,71-1,29) |
| T-R0 Aenderungsumfang gegen ot3 | Sektionen 0-6, andere Meshes, CLUT 480-485, Texel ausserhalb | OK: gleich / keine / gleich / 0 |
| T-R1a Kopf-Geometrie = G2 Mesh 3 | Ecken <= 1 E., Normalen <= 2, Flaechen gleich, UV = Quelle + (0,160) ausser 11 Maulflaechen | OK 0,50 E. / 0,50 / gleich / UV abweichend 0, Maul 11 |
| T-R1b Kopf-Texel = G2-TIM | abweichend nur die angedunkelten, selbe Zeile, nie heller | OK 3821 = 3821 = 3821; 7519 unveraendert |
| T-R1c Zahn-Kachel = G3-TIM | bytegleich | OK 0 von 432 |
| T-R1d Zeilen 486/487 = RE2 | 15-Bit-Rohwerte bytegleich | OK gleich / gleich |
| T-R2 K10 / K4 / K7 / S2 / KS2 | Messblatt-Fenster | OK (Tabelle oben) |
| T-R3 Zaehne sichtbar | >= 20 / >= 5 | OK 58 / 44 |
| T-R4 Zeilen | <= 16, <= 4096 | OK 8, 2048 |
| G1, M1-M4 | wie Riegel V4 | OK (M2 Kopf 84 Dreiecke, M4 Dichte Kopf 0,072) |
| T1-T4, TA, T5q, T6, TK, T7a-d, Boden | wie Riegel V4 | OK |
| T5 | f 0,5 >= 0,100 H | **ROT** 0,098 (Geometrie seit Runde 5, nicht angefasst) |
Gegenprobe ot3 (riegel_re2kopf_V4ot3_gegenprobe.*): **9 rot von 34** - T-R1a (61 Ecken / 73 Tri / 18 Quad statt 48 / 56 / 14), T-R1b (11254 von
11340 Texeln abweichend), T-R1c (753 von 756 der alten Kachelgroesse), T-R1d (Zeilen fehlen), T-R2 K4 / K7 / S2 (kein G2-Auge, keine Zaehne),
T-R3, T5. Der Riegel unterscheidet die Fassungen.

### Leisten (SF_belege/v4/; Runde 3 in v4/ot3/, Runde 2 in v4/ot2/, Runde 1 in v4/ot1/; Spalten KI-Bild | Nutzer-Beschriftung | Referenz 2 | neu)
- 01_vorn_kamera12, 02_seite_wie_bild2, 03_kopf_nah_vorn_x4 (Zeile 2 mit Marken: Auge gelb, Zaehne cyan), 04_kopf_nah_seite_x4 (Zeile 2 Auge
  gelb, Fenster KS2 cyan), 08_rundum_V4 (Zeile 1 KI 4 Ansichten + Beschriftungen, Zeile 2 Referenz 2, Zeile 3 neu).
- 10_kopf_ki_re2_neu_x6: je Zeile ein KI-Bild (FRONT | RIGHT) und drei Modell-Kacheln unter gleichem Licht (emd_ansichtsblatt.render, Kopfbox
  240 px): Zeile 1 G2-Quelle vorn | neu vorn | neu rechts; Zeile 2 G2 rechts | neu links (Haarseite) | neu oben (Kuppe mit roten Adern); Zeile 3
  G3-Zahnquelle | neu von unten (Maul mit Zahnreihen) | neu hinten.
- 11_spiel_koepfe: RE15_FRAMEDUMP h135 (Kopf x3) und h85 (Oberkoerper x2; der Kopf liegt im Spielbild h85 hinter dem Buckel) fuer Referenz 2
  (Lauf R2ot) und neu (Lauf V4re2); Lauf-Log SF_belege/v4/re2/lauf_V4re2.txt (EXIT=0, 191 Bilder, "EM30 loaded: 23 meshes, 23 bones, 21 clips").
Am Bild: dunkelgrauer Keilkopf mit Schnauze, rote Adern auf der Kuppe, ein rot gluehendes Auge mit hellem Kern Bild-links, Mitte und Bild-rechts
dunkler (Nase / Haarseite), unten das offene Maul mit zwei Zahnreihen und dunkelrotem Inneren; seitlich eine nach vorn-unten weisende dunkle
Schnauze mit dem Auge nahe der Spitze.

### Tests
- ctest (unit_birkin_formen, unit_birkin_szenen, unit_schwanz_r41, integration_r41_sichel_form): "100% tests passed, 0 tests failed out of 4"
  (64,52 s; SF_belege/v4/re2/ctest_v4_re2.txt). Engine unveraendert, kein Neubau noetig (Bau vom 2026-11-07 12:37).
- Variante: sfv4/emd/EM30_V4re2.EMD = EM30_V4.EMD (sha256 f37bc5514c6892186d90a808dfd20f646aeecb59b0667933b3daa84f296ff18d, 262284 Byte);
  Spiel-Asset EM30_HANGAR.EMD (cfd1ac42) und Pin release/em30_hangar.sha256 unveraendert. Reproduzierbar: "python kopf_re2_bau.py --json PROT"
  (Basis EM30_V4ot3.EMD a1bc9309).

### Offen
1. **Kopf-Helligkeit**: die G2-Textur ist dunkel (Texel L* 27 flach, Leser A); im RDT-Licht der Pose 135 liegt der Kopf-Median bei L* 15, der
   Ref2-Kopf bei 30,8, die KI-Koepfe bei 23-26 (Leser B D6: 5-8 L* dunkler als die beleuchtete Haut, hier ~13). Heller nur mit helleren
   Eintraegen der G2-Zeile (waere wieder Eintragswahl) oder mit einer eigenen, helleren Kopie der Zeile 486 - nicht gemacht (Nutzer: "Warum
   mischst du die Farben so daemlich?").
2. **Schnauzenwinkel**: Leser B 30-35 Grad vorn-unten; gebaut 10 Grad, weil die Beschreibung1 (Gesicht mit Auge/Haaren/Nase in Cut 12) bindend
   ist und das Gesicht ab 20 Grad aus der Kamera kippt (K10-Hoehe 0,137 H).
3. **Auge klein**: der G2-Augenkern ist in Pose 135 19 Bildpunkte, in Pose 85 16 (Marker-Render); im rohen Render ein dunkelroter Punkt mit
   hellem Kern (G2: "rot leuchtend"), KI: dunkle Pupille mit weissem Glanzpunkt (Leser B D3). Kein gemaltes Auge.
4. **Haarseite in Pose 135 kaum zu sehen**: durch das Gieren -34 Grad (noetig fuer K4) wendet sich die (W)-linke Haelfte von der Kamera ab;
   Haelften-Median links 15,4 / rechts 15,9 L*. Sichtbar ist sie in 08 (rundum) und 10 (Zeile 2).
5. **Schwanz**: Leser B D7 (C-Bogen 1,2-1,4 H bis zum Boden) widerspricht Beschreibung2 (0,70 H2 mit Knick); kein Delta ohne Entscheid, Schwanz
   bytegleich Runde 3 (T-R0). T5 bleibt rot (0,098).
6. **Texel-Zeilen der Zaehne**: die Zahn-Kachel ist 12 x 36 Texel auf 11 Flaechen (Texeldichte des Maulbereichs unter der des G2-Kopfes); im
   Spielbild h135 sind die Zaehne als helle Punkte zu sehen (Leiste 11, Zeile 1).
7. Der Einbau ins Spiel-Asset folgt erst nach der Abnahme; Runde 2 dieser Spur ist die letzte.

## V5 Kopf aus RE2 (KI-Bilder) - Runde 2 (Bauer, Runde 2 von 2, 2026-11-07, nach Pruefung RE2-Kopf r1)

Grundlage: Runde 1 (EM30_V4re2 f37bc551) wurde nicht bestanden (SF_PRUEFUNG_RE2KOPF_r1_zahlen.md / _sicht.md): der Kopf war im
RDT-Licht ein fast schwarzer Klotz (Kopf L* 15,6, Ref2 30,8), die rechte Haelfte nicht dunkler als die linke (15,4 / 15,9), das Auge ein
1-2 px grosser roter Punkt. Diese Runde behebt genau diese Befunde; Koerper (Referenz 2), Schwanz (Runde 3), Drehung/Skala/Versatz des
Kopfes und die Zaehne bleiben.

### Nutzer-Wortlaut (bindend, unveraendert)
- "bei den KI Bildern kann man so verdammt gut sehen wie das verschwommene Bild eigentlich aussehen muss.... Man sieht alles einwandfrei! Das
  Auge die Haare, den Mund, den Schwanz...."
- "Vielleicht kannst du dir auch einen Birkin aus Resident Evil 2 nehmen. Ich nehme an die duerften aehnlich aufgebaut sein die anderen Formen...."
- "Linke Seite ist so graeulich. Bis Nase. Dann rechts Seite sieht man Haare - braeunlich.... Warum mischst du die Farben so daemlich?"

### Befunde der Pruefung r1 und was gemessen wurde (re2kopf/suche_r2/, Belege SF_belege/v4/re2/suche/)
1. **Warum der Kopf schwarz war**: die G2-Textur ist flach dunkel (Texel-Median L* 21,6, Leser A 22,1); im RDT-Licht 50A0/12 (drei
   Parallellichter aus Richtung (2000, 2000, 2000), Farbe 128, Umgebung 63; b2.raum) faellt der Kopf-Median auf 15,6. Referenz 2 hat Texel
   42,5 -> 30,8 (diag1.json). Das Andunkeln der Runde 1 (3821 Texel auf dunklere Eintraege) verschaerfte das noch.
2. **Warum die Haarseite nicht ankam**: die Runde-1-Zone war z > -80 im Kopfraum ((W)-links); die Kamera 50A0/12 steht (W)-links-vorn-oben
   (Richtung im Knochen-9-Raum (0,72, -0,43, 0,54)), die Lichter kommen aus (0,22, -0,83, 0,52). In Pose 135 zeigt die Bild-rechte
   Haelfte zu 708 von 950 Pixeln die Texel u 61-80, die Bild-linke zu 514 von 1193 die Kuppe u 101-125 (diag1.json, u_hist); die
   Zonengrenze z = 0 lag am linken Bildrand, nicht in der Bildmitte (z1_135.png: 366 graue gegen 1454 dunkle Kopfpixel).
3. **Warum das Auge nicht als heller Fleck geht**: der Augenfaecher T19/T20/T41 hat die Normale (0,33, -0,19, -0,88) ((W)-rechts), nach
   Rz(10) Ry(-34) (0,89, -0,15, -0,43) - Skalarprodukt mit dem Licht ~0 -> nur Umgebungslicht, Faktor 0,24 (ncct: 63 -> 62/255; gemessen
   0,22-0,25, fan.py). Ein weisser Texel erreicht dort hoechstens L* 26, die Zaehne (Normale +x, Faktor 0,81) liegen bei 55. Suche ueber
   yaw -34..-65 und pitch 10..30 (grid_rot.log, 12 Faelle): Faecher-Faktor hoechstens 0,47 (yaw -65), dabei K4 0,034 H (rot) und KS2
   zerstoert; Rollen um die Schnauzenachse (grid_roll.log, 24 Faelle): Faktor 0,51 erst bei roll -20, dabei K10-Breite 0,143 H und KS2 nur
   noch 0,26 Kopf - kein Vorbild zeigt einen schiefen Kopf. Die Messblatt-Methode "hellster Kopfpunkt = Augenfleck" (Video) ist in Cut 12
   mit dem G2-Kopf physikalisch nicht zu erfuellen; gebaut ist die KI-Lesung (Leser B D3: dunkle Pupille mit Glanzpunkt, s. u.).

### Aenderungen (Commit aa6c8a181; Variante sfv4/emd/EM30_V4re2.EMD = EM30_V4.EMD, sha256 e9c66c2edcb846f8613ea2f309810bb82b09bb7b4e25bd0917ba7ae9297363a5, 262796 Byte)
| Teil | Runde 1 | Runde 2 |
|---|---|---|
| Kopf-Texel (G2 u 0-125 v 0-89 -> TPage 128 v 160-249) | 7519 bytegleich, 3821 auf dunklere Eintraege umgesetzt | **alle 11340 bytegleich** (T-R1b 0 abweichend) |
| Zeile 486 | G2-Zeile 0 bytegleich | G2-Zeile 0 mit **L* x 1,90**, a*/b* unveraendert (Belichtung; Farbton-erhaltende sRGB-Gamut-Abbildung nur ueber die Chroma; Farbwinkel vor der 5-Bit-Rundung 0,00 Grad, Rundung dE76 max 5,6); Texel-Median L* 21,6 -> 42,2 |
| Zeile 487 | G3-Zeile 0 bytegleich | unveraendert bytegleich (Zaehne) |
| Zeile 488 (neu) | - | G2-Zeile 0 mit L* x 1,00, Chroma x 0,50, L* <= 30 (Haarseite/Nase/Spitze; Texel-Median 21,0) |
| CLUT-Zeile je Flaeche | alle G2-Flaechen 486 | 33 Flaechen 486 (grau), 26 Flaechen 488 (dunkel), 11 Maulflaechen 487 |
| Zonenregel | Texel z > -80 (Haar), u 41-60 (Nase), Flaechen x > 150 (Spitze) | Flaechenmittelpunkt Bild-rechts der Kopfbox-Mitte in Cut 12 Pose 135: Ebene n . c > d mit n = (0,2543, 0,5728, 0,7792), d = -60,1 (G2-Kopfraum; n = normierter Bild-x-Gradient der Spielkamera, suche/ebene.json) oder Gesichtsspalt u 41-60 oder Spitze Q0 (KS2) |
| Drehung/Skala/Versatz | Rz(10) Ry(-34) x 1,50 + (140, 10, 0) | unveraendert (roll 0, als Parameter gemessen und verworfen) |
| Geometrie, Maul-UV, Koerper, Schwanz, Skelett, Clips, EDD | - | unveraendert (T-R0: Sektionen 0-6 bytegleich ot3, nur Mesh 9 anders, Texel ausserhalb 0) |
Palettenwahl (grid_pal.log ... grid_pal4.log, 21 Faelle, Masse kopf_re2_mess + mess_r2): k_grau 1,7 / 1,9 / 2,3 / 2,6 -> Kopf P50 23,1-23,8;
dunkel k 0,9 / lmax 26 -> P50 20,5 (T-K9e < 23); k 1,1 / lmax 34 -> KV2 31,6 (> Ref2-Kopf - 8 = 31,4). Gewaehlt 1,90 und 1,00 / 30 / 0,50.
Der Sicht-Pruefer r1 nannte diesen Weg ("ein Faktor auf L*, Farbton und Adern bleiben") ausdruecklich keine Farbmischung; die zweite Zeile
ist derselbe Faktor-Weg fuer die Haarseite. Folge der Belichtung: 49 % der belegten Eintraege liegen nach L* x 1,9 ausserhalb des
sRGB-Gamuts und verlieren Chroma (Eintraege p50 31,3 -> 9,7), die Adern der grauen Seite werden blasser (Farbton bleibt).

### Herkunft (byte-true, unveraendert Runde 1; Protokoll SF_belege/v4/re2/kopf_re2_protokoll.json)
CDEMD0.EMS (sha256 8724200c...) kind 0x31 (G2): EMD Sektor 2944 (193124 B, bb966cd5...), MD1 dir[7] Mesh 3 (48 Ecken, 48 Normalen, 56
Dreiecke, 14 Vierecke); TIM Sektor 2895 (99872 B, e0929cef...), Texel u 0-125 v 0-89, CLUT-Zeile 0 (y 480). kind 0x33 (G3): TIM Sektor
3065 (99872 B, 99b793e2...), Zahn-Kachel u 2-13 v 5-40 (432 Texel), CLUT-Zeile 0. Ins EM30: Mesh 9 am Ref2-Knochen 9, Texel TPage 128
x 0-125 / y 160-249 und TPage 130 x 256-267 / y 200-235, Zeilen 486 / 487 / 488 (9 Zeilen, Port-Texturhoehe 2304 <= 4096).

### Kopfmasse (kopf_re2_mess.py / riegel_re2kopf.py, Render b2 Kamera 50A0/12 RDT-Licht, Pose 135 / 85)
| Mass | Soll | Ist Runde 2 | Runde 1 |
|---|---|---|---|
| K10 Kopfbox vorn | 0,14-0,18 x 0,15-0,20 H | 0,165 x 0,170 H (Box 325-384 x 160-221, H 364) | gleich |
| K4 Auge vorn (Marker Augenkern) | 0,035-0,065 H links; 0,07-0,12 H unter OK | 0,060 / 0,097 H (19 px) | gleich |
| K1k Auge sichtbar (KI D3) | Faecher (Videoraum) <= Umgebung - 6; Glanzkern >= Faecher + 8 | Faecher 104 px (0,044 x 0,071 H) L* 18,2 gegen Umgebung 36,4; RDT-Licht Faecher 10,6, Glanzkern max 29,7 | Faecher 17,4 gegen 26,3 (kaum lesbar, Kopf dunkel) |
| K5 dunkle Seite Bild-rechts | Haelften rechts <= 0,90 x links (Licht und Videoraum); KV2 <= Ref2-Kopf P50 - 8 = 31,4, a*, b* > 0 | **28,4 / 20,9 (0,73)**, Videoraum 36,1 / 26,7 (0,74); KV2 30,1 / 10,6 / 6,3; Zonen: grau 39,4, dunkel 21,6, Maul 17,4 (RDT-Licht) | 15,4 / 15,9 |
| K6 zweites Auge verdeckt | Spiegelbox <= Kopf-Median (Videoraum 29,2) | 28,3 | - |
| K7 Maul-Lage | Zahn-Schwerpunkt x 0,3-0,7 / y 0,55-1,0 | 0,60 / 0,66 (58 px) | gleich |
| T-K9e ganzer Kopf | L* P50 >= 23; Kopf/Rumpf >= 0,9 | **23,8** (Videoraum 29,2); **1,02** (Rumpf 26,2) | 15,6; 0,59 |
| S2 Auge seitlich | 0,03-0,07 H; 0,76-0,82 H | 0,043 / 0,812 H | gleich |
| KS2 Haarkante seitlich | Kopf-Pixel <= Kopf-Median (85, Videoraum 39,6) - 5 | 20,7 (68 % Kopf; ganzes Fenster 26,7) | 14,8 / 22,5 |
| T-R3 Zaehne | >= 20 Marker, >= 5 helle | 58 Marker; 78 helle | 58 / 44 |
| INFO K9 Kuppe rot KV1 | Messblatt a* >= 11 | 30,0 / 7,2 / 4,0 (a* 7,2; G2-Kuppe = rote Adern auf Grau) | 6,1 |
| INFO Messblatt K1/K3/K4 (hellster Kopfpunkt) | = Augenfleck | hellster Fleck (333, 170) L* 69,2, Kontrast 25,1, 0,022 x 0,019 H, K4 0,060 / 0,028 H = die graue Kuppe ueber dem Auge, nicht die Zaehne (r1) und nicht das Auge (Lichtbeleg oben) | Zaehne |
Kopf/Rumpf-Verhaeltnis gegen die Vorlagen (Sicht r1 Punkt 4): Video 1,59, KI 1,6-2,2, Ref2 1,28 - neu 1,02 (Kopf-Mittel 26,7 gegen Rumpf 26,2).
Die graue Seite allein liegt bei 39,4 (1,5 x Rumpf, wie die Vorlagen), Haarseite und Maul sind absichtlich dunkel (KI rechte Haelfte 17-24,
Maul 16-18; Leser B), darum bleibt der ganze Kopf im Mittel beim Rumpf.

### Riegel (riegel_re2kopf.py, --basis EM30_V4ot3.EMD, --lauf sfv4/r/V4re2b/debug.log; SF_belege/v4/re2/riegel_re2kopf_V4re2.txt/.md/.json)
**Exit 1: 1 rot von 43** (T5 0,098, Geometrie seit Runde 5), 8 INFO-Zeilen. Woertlich (gekuerzt auf Mass / Ist):
| Mass | Ist | |
|---|---|---|
| T-K1a/b/c Koerper = Ref2 | keine Abweichung; Zeilen 480-485 gleich, weitere 486 = G2 grau (L* x 1,90), 487 = G3, 488 = G2 dunkel (L* x 1,00, <= 30); 0 von 34716 Texeln | OK |
| T-K2s / T-K3a-d Schwanz | 0 von 10356 ausserhalb; Sprung 13,0 <= 14,0; Std 8,8-14,6; Wurzel 4,7; Anisotropie 0,71-1,29 | OK |
| T-R0 Aenderungsumfang gegen ot3 | Sektionen gleich [0-6]; andere Meshes keine; CLUT 480-485 gleich; Texel ausserhalb 0 | OK |
| T-R1a Kopf-Geometrie = G2 Mesh 3 | 48 / 48 / 56 / 14; Ecken max 0,50 E., Normalen 0,50, Flaechen gleich, UV abweichend 0, Maul 11; Zeilen grau 33 / dunkel 26, Zone abweichend 0 | OK |
| T-R1b Kopf-Texel = G2-TIM | 0 von 11340 Texeln abweichend (6524 belegt) | OK |
| T-R1c Zahn-Kachel = G3-TIM | 0 von 432 abweichend | OK |
| T-R1d CLUT 486 / 487 / 488 | 486 gleich; 487 gleich; 488 gleich (Nachrechnung); Lab p50 Quelle 39,4/25,1/14,5 -> grau 74,3/4,6/1,2 -> dunkel 29,6/12,8/7,5; Farbwinkel p90 0,00 Grad; Rundung dE76 p90 3,8 max 5,6; Gamut-reduziert 0,49 | OK |
| T-R2 K10 / K4 / K7 / S2 / KS2 | 0,165 x 0,170; 0,060 / 0,097; 0,60 / 0,66; 0,043 / 0,812; 20,7 | OK |
| T-R2 K1k / K6 / K5 / T-K9e | s. Tabelle oben | OK |
| T-R3 Zaehne | 58 Marker, 78 helle | OK |
| T-R4 Zeilen | 9 Zeilen; 256 x 9 = 2304 | OK |
| G1, M1-M4, T1-T4, TA, T5q, T6, TK, T7a-d, Boden | wie Runde 1 | OK |
| T5 | 0,032/0,049/0,098/0,163 (f 0,5 >= 0,100 verfehlt) | ROT (bekannt) |
Gegenprobe ot3 (riegel_re2kopf_V4ot3_gegenprobe.txt): **10 rot von 37** - T-R1a/b/c/d, K4, K7, S2, T-R3, K1k, T5. Der Riegel unterscheidet.

### Leisten (SF_belege/v4/; Runde 1 in v4/re2r1/, Runde 3 in v4/ot3/; Spalten KI-Bild | Nutzer-Beschriftung | Referenz 2 | neu)
01_vorn_kamera12, 02_seite_wie_bild2, 03_kopf_nah_vorn_x4 (Zeile 2 Marken: Auge gelb, Zaehne cyan), 04_kopf_nah_seite_x4, 08_rundum_V4,
10_kopf_ki_re2_neu_x6 (KI FRONT/RIGHT | G2-Quelle | neu vorn/rechts/links/oben/unten/hinten unter gleichem Licht), 11_spiel_koepfe
(RE15_FRAMEDUMP h135 / h85, Lauf sfv4/r/V4re2b, 191 Bilder, EXIT=0, "EM30 loaded: 23 meshes, 23 bones, 21 clips", 0 Fehlerzeilen;
SF_belege/v4/re2/lauf_V4re2b.txt). Am Spielbild h135 (11, Spalte 4): grau-rosa Kuppe mit blassen Adern Bild-links, dunkle braun-graue
Seite Bild-rechts, dunkle Augenhoehle mit Glanzpunkt links oben, offenes Maul mit Zahnreihen unten - gegen Runde 1 (suche/spiel_vgl.png,
Zeile 2) kein schwarzer Klotz mehr.

### Tests
ctest (unit_birkin_formen, unit_birkin_szenen, unit_schwanz_r41, integration_r41_sichel_form): "100% tests passed, 0 tests failed out of 4",
Total Test time (real) = 64.42 sec (SF_belege/v4/re2/ctest_v4_re2.txt). Engine unveraendert (Bau 2026-11-07 12:37). Spiel-Asset
EM30_HANGAR.EMD (cfd1ac42) und Pin release/em30_hangar.sha256 unveraendert.

### Offen
1. **Auge nach Messblatt (heller Fleck)**: in Cut 12 nicht erfuellbar (Lichtbeleg oben: Umgebungslicht 0,24 am Augenfaecher, Zaehne 0,81).
   Gebaut ist die KI-Lesung (dunkle Augenhoehle 18 gegen Umgebung 36 im Videoraum, Glanzkern 29,7). Entscheid beim Orchestrator, ob die
   Messblatt-Zeilen K1-K3 fuer den RE2-Kopf durch K1k ersetzt werden.
2. **Zwei Palettenzeilen statt einer**: die Haarseite braucht eine eigene dunkle Zeile (488), weil die Belichtung x 1,9 die dunkelsten
   Eintraege der grauen Zeile auf L* ~26 hebt. Harte Zonengrenze entlang der Flaechenkanten (kein weicher Uebergang wie Leser B D4).
3. **Kuppe**: KV1 a* 7,2 (Messblatt >= 11) - die G2-Kuppe ist grau mit roten Adern, nach der Gamut-Abbildung blasser. Die hellste Stelle
   des Kopfes ist jetzt diese Kuppe (L* 69 im Videoraum), nicht das Auge.
4. **KV2 schneidet das Maul**: das Haar-Fenster des Messblatts (0,52-0,85 x 0,25-0,75) enthaelt die Zahnreihe; KV2 30,1 liegt nur 1,3
   unter der Grenze 31,4; die dunkle Zone selbst misst 27,1 (Videoraum).
5. **Kopf/Rumpf 1,02** statt 1,4-1,6 (Vorlagen): nur die graue Seite ist heller als der Rumpf (39,4 / 26,2 = 1,5); Haarseite und Maul sind
   nach KI dunkel. Heller nur mit einem hoeheren Faktor auf beiden Zeilen (dann KV2 rot).
6. Schwanz (Leser B D7 gegen Beschreibung2), Schnauzenwinkel (Leser B 30-35 Grad gegen Beschreibung1-Sicht), T5 0,098: wie Runde 1 offen.
7. Einbau ins Spiel-Asset erst nach der Abnahme.


## V5 Runde 3 (Nutzer: grau, abgerundet) (Bauer, 2026-11-08)

Grundlage: V5 Runde 2 (EM30_V4re2, sha256 e9c66c2e). Diese Runde aendert nur die Palettenzeilen 486/488 und die Lage von 9 Ecken des Kopf-Meshes.
Koerper (Referenz 2), Schwanz (Runde 3 ot3), Texel (G2/G3 bytegleich), UVs, Zonen, Drehung/Skala/Versatz des Kopfes und das Maul bleiben.
Kein RE-Befund (die Sichel-Form gibt es auf keiner Disc): keine Zahl hier ist eine @0x-Konstante; jede Zahl ist ein Mass gegen Foto/Referenz.

### Nutzer-Wortlaut (2026-11-08, bindend)
"Mach die Feuerflecken vom 2 beinigen Birkin auf dem Kopf noch weg. DIese hellen Kratzer im Gesicht. Ich weiß nicht woher du die ableitest. Die gibt
es eindeutige nicht! Die Seite ist einfach grau wie der Rest auch. Außerdem hast du vorne am Kopf eine Verlängerung wie so ein spitzer Schnabel in
grau. AUch die gibt es nicht. Es ist abgerundet so wie es auf dem Original Foto aussieht."
Weiter gueltig: "da ist links ein Auge. Linke Seite ist so gräulich. Bis Nase. Dann rechts Seite sieht man Haare - bräunlich" (Beschreibung1).

### Woher die Flecken kamen (gemessen)
Die "Feuerflecken"/"hellen Kratzer" sind die roten Adern der G2-Textur (Quell-Eintraege mit Chroma > 12: 192 von 255 Eintraegen der
G2-Zeile 0; im Texel der grauen Zone 30 % der Texel, Median-L* 27,5 gegen 21,0 der grauen Haut). Runde 2 hat sie mit L* x 1,9 belichtet:
Zeile 486 Texel L* P50 / P99 / max 42,2 / 97,6 / 97,6, 30,6 % der Texel ueber P50 + 8, Chroma P90 30,9, a* P99 45,2, b* P10 -3,7
(silber-lila). Im Render Pose 135 / 85: a* > 12 bei 25,4 / 23,4 % der Kopfpixel, L* P99 78,6 / 90,9.

### Farbe: Palettenregel r3 (kopf_re2_bau.zeile_r3 / zeilen_r3; PARAM_R3['pal']; Texel bytegleich, nur Zeilen 486 / 488)
Je Eintrag: L*k = k x L*Quelle; M = Texel-Median von L*k ueber die Texel der Zone (UV-Dreiecke der Zonenflaechen ohne Maul gerastert);
L*' = L*k begrenzt auf [M - 12, M + 6]; Eintraege mit Quell-Chroma > 12 (Adern) hoechstens M (= Grau derselben bzw. geringeren Helligkeit);
a*/b* fuer ALLE Eintraege = Zielton der Zeile; Ausnahme Glanzpunkt des Auges (Eintraege des Augenkerns mit Quell-L* >= 70: 133, 152, 164, 166):
L*' = L*k. Danach sRGB-Gamut ueber die Chroma, 5 Bit je Kanal, Eintrag 0 bleibt 0.
| Zeile | k | M | Ton a*/b* | Texel L* P50/P99/max | Texel > P50 + 8 | Chroma P50/P90 | a* P99 | b* P10 |
|---|---|---|---|---|---|---|---|---|
| 486 grau Runde 2 | 1,90 | - | Quelle | 42,2 / 97,6 / 97,6 | 30,6 % | 6,9 / 30,9 | 45,2 | -3,7 |
| 486 grau Runde 3 | 1,941 | 42,0 | 1,5 / 2,0 | 41,5 / 47,9 / 47,9 | 0,0 % | 3,4 / 5,1 | 3,4 | 1,1 |
| 488 dunkel Runde 2 | 1,00 (<= 30, Chroma x 0,5) | - | Quelle x 0,5 | 21,0 / 31,1 / 31,4 | 22,5 % | 6,9 / 22,7 | 29,1 | -3,9 |
| 488 dunkel Runde 3 | 1,20 | 26,0 | 3,0 / 5,0 | 25,2 / 31,1 / 97,6 (1 Glanz-Texel) | 0,0 % | 7,0 / 7,6 | 7,1 | 1,4 |
(Texel der Zonen ohne Augenkern, sfv5r3-Messung; Riegel T-R1d)
Helligkeit (Suche re2kopf/suche_r3/grid_pal.py, Render b2 Kamera 50A0/12 RDT-Licht Pose 135): m_ziel 27,9 = Texel-Median der Koerperhaut
(Referenz 2 Meshes 0-8/10-15, 36244 Texel, suche_r3/haut.py) ergab im Render graue Zone 25,5 gegen Koerperhaut 27,7, aber T-K9e 20,0 / 0,78 und
K5 0,97 (rot) - der Kopf wirkte dunkler und oliv als der Koerper. 30,5-40: K5, T-K9e oder K6 rot. 42 (k 1,94, wie Runde 2): graue Zone im Render
37,8-39,1 (Videoraum 41,5) = helle Schulterhaut der Leiste 01, Kopf/Rumpf 0,99, alle Zeilen gruen. Ton: b* 4 wirkte bei L* ~25 oliv-braun
(kopf_ref2_r2_r3_x5.png aus dem ersten Versuch), darum a*/b* 1,5/2,0 (warm-neutral, b* >= 0, a* < 8). Dunkle Zeile k 1,2: 1,0 -> T-K9e rot,
1,25/1,3 -> KV2 > 31,4 (K5 rot); Ton braeunlich-grau 3,0/5,0.
Render nachher (Riegel T-V1, Kopfpixel ohne Augenfaecher/Maul): Pose 135 a* > 12 0,0 % (vorher 25,4 %), helle Kratzer 1,5 % (vorher 10,0 %),
b* P10 0,6 (vorher -3,8), L* P99 47,9 (vorher 78,6); Pose 85 0,0 % (23,4 %), 1,9 % (22,0 %), b* P10 0,0 (-3,7), L* P99 47,9 (90,9).

### Form: Kappe (kopf_re2_bau.kappe / runden / normalen_neu; PARAM_R3['form'] = trimm 1,15, cos0 0,45, cos1 0,55, fest 'auge')
Profil der Fotos: Kopf-Vorderkante seitlich (Beschreibung2_Vorlage, Zeilen 186-217 vor dem weissen Pfosten, |dL*| > 9 gegen das BSS im
Videoraum) x 464-465 in allen 32 Zeilen = Messblatt "Vorderkante x 465"; Foto vorn (Vorlage 1) ist rund ohne Vorsprung (Messblatt Abschnitt 2).
Kappe = achsparalleles Ellipsoid, Kleinste-Quadrate-Anpassung an die 48 G2-Ecken (unskaliert), iterativ ohne r > 1,15: c (-17,5, 18,1, 0,0),
Halbachsen (214,5, 396,7, 173,1), 3 Schritte, ausgeschlossen 7 / 8 / 10 / 18 (r 1,35 / 1,19 / 1,36 / 1,35) = die Keilspitze (lokale Abweichung
9,2-10,4 % der Kopfhoehe = die drei groessten des ganzen Meshes, suche_r3/form_probe.py). Vordere Ecken mit r > 1 werden radial auf
r' = 1 + (1 - w)(r - 1) gelegt, w = 0 bei cos <= 0,45 bis 1 bei cos >= 0,55 (cos = Richtung zur Schnauzenachse +x).
Fest bleiben die 5 Ecken des Augenfaechers (2, 3, 5, 17, 39 = T19/T20/T41: K4/S2). **Bewegt: 9 Ecken** 1, 7, 8, 10, 11, 18, 29, 30, 41
(7/10/18 je ~90-99 E., 8 58, 11 37, 30 36, 1/29/41 klein); Normalen dieser 9 Ecken neu (flaechengewichtetes Mittel der Flaechennormalen,
Methode an G2 nachgerechnet: p50 4,8 / p90 11,7 Grad zur Original-Normale), alle anderen bytegleich; UVs und Zonen unveraendert (an den
Quell-Ecken). Verworfen (Varianten A-W, suche_r3/feval.py): Auge mitrunden -> Augenfaecher an der Vorderkante, KS2 rot (A/J/K/L); Maul
festhalten -> Maulecke 41 steht 4,8 % vor (C); breiter Uebergang 0,30/0,60 -> Kronenecken 16/26 mit, S2 0,028 (B); lokale Kugeln
(form_probe3) liefen in Teilhaufen; radiale Glaettung / Randbindung ans Auge (M-W) liessen Spitze oder Maulecke wieder vorstehen.
| Mass | Runde 2 | Runde 3 |
|---|---|---|
| T-V2a vorderste Ecke gegen die Kappe (Riegel-Anpassung am Ergebnis) | Ecke 10 (x 228), 99,0 E. = 21,3 % der Kopfhoehe | Ecke 41 (x 170), 7,0 E. = 1,5 % |
| T-V2b Dellen / neue Vorspruenge (Grenze = Relief der G2-Quelle P75 7,7 %) | - | keine / keine (24 Ecken geprueft) |
| T-V2c IoU vorn seitlich gegen Foto 2 (Zeilen 186-217) | 0,529 | 0,549 (Referenz 2: 0,503) |
| Kopf-Vorderkante seitlich Render / Foto | 484 / 464 (Ueberstand max 20 px) | 481 / 464 (max 17 px) |
| K10 / K4 / K7 / S2 / KS2 | 0,165x0,170 / 0,060-0,097 / 0,60-0,66 / 0,043 / 20,7 | 0,165x0,170 / 0,060-0,097 / 0,62-0,64 / 0,032 / 33,6 |

### Riegel (riegel_re2kopf.py, --prot SF_belege/v4/re2r3/kopf_re2_protokoll_r3.json, --lauf sfv4/r/V5r3/debug.log; SF_belege/v4/re2r3/riegel_re2kopf_V5r3.txt/.md/.json)
EMD sha256 dd546074; **Exit 1: 1 rot von 48 (T5 0,098, Schwanz-Geometrie seit Runde 5, bekannt)**, 8 INFO. Neu bzw. geaendert:
| Mass | Soll | Ist | |
|---|---|---|---|
| T-K1a/b/c Koerper = Ref2 | Meshes 0-8/10-15, Zeilen 480-485, Koerper-Texel bytegleich | keine Abweichung; 486/488 = Regel r3, 487 = G3; 0 von 34716 | OK |
| T-R1a Kopf-Geometrie | Ecken = Rz Ry Rx S runden(v) + t, Normalen, Flaechen, UV, Zonen | max 0,49 E. / 0,50; UV abweichend 0; Zone abweichend 0; 9 Ecken gerundet, 9 Normalen neu | OK |
| T-R1d Zeilen 486/487/488 | Nachrechnung bytegleich; kein Eintrag > M + 6, keine Ader > M, Rundung dE76 <= 7, a* <= 8, b* >= -0,5 | gleich/gleich/gleich; grau M 42,0: 0 / 0, dE 4,3, a* max 3,4, b* min 0,0; dunkel M 26,0: 0 / 0, dE 4,7, a* 7,1, b* 1,3 | OK |
| T-V1 Pose 135 | a* > 12 <= 2 %; L* > lokaler Median (9x9, gleiche Zone) + 8 <= 2 %; b* P10 >= -1 | 0,0 %; 1,5 %; 0,6 (Zonen-Median + 8: 0,6 %; Kopf-Median + 8: 30,0 %) | OK |
| T-V1 Pose 85 | wie oben | 0,0 %; 1,9 %; 0,0 (Zonen-Median + 8: 10,7 % = Lichtseite; Kopf-Median + 8: 11,1 %) | OK |
| T-V2a / T-V2b / T-V2c | s. Form | 1,5 %; keine/keine; 0,549 >= 0,529 | OK |
| K10 / K4 / K7 / S2 / KS2 / K1k / K5 / K6 / T-K9e / T-R3 | Messblatt-Fenster wie Runde 2 | alle OK (K5 30,4/23,1 = 0,76; K6 29,4 <= 29,7; T-K9e 24,8 / 0,99) | OK |
| T5 | f 0,5 >= 0,100 | 0,098 | ROT (bekannt) |
Abweichung von der Auftragsformel bei T-V1 (offen benannt): "Anteil L* > Kopf-Median + 8" ist bei JEDER Variante mit dunkler Haarseite rot
(die graue Seite liegt ganz ueber dem Kopf-Median + 8: 30 % in Pose 135), und gegen den Zonen-Median zaehlt in Pose 85 die Lichtseite der oberen
Vorderflaechen (10,7 %; dieselben Pixel ohne Licht liegen bei Texel-L* <= M + 6, hell85.py). Darum prueft der Riegel helle Kratzer gegen den
lokalen Median derselben Zone; beide Auftragszahlen stehen als Ausgabe in der Zeile. Die Texel-Regel "kein Texel heller als Median + 8" prueft
T-R1d an der Palette (0 Eintraege > M + 6).
**Gegenprobe V5 Runde 2** (riegel_re2kopf_V4re2_gegenprobe.txt): 4 rot von 48 - T-V1 135 (a* > 12 25,4 %, Kratzer 10,0 %, b* P10 -3,8),
T-V1 85 (23,4 %, 22,0 %, -3,7), T-V2a (21,3 %), T5. Der Riegel unterscheidet.

### Leisten (SF_belege/v4/; Runde 2 nach v4/re2r2/ verschoben; Spalten Originalfoto (roh) | Nutzer-Beschriftung | Referenz 2 | neu)
01_vorn_kamera12, 02_seite_wie_bild2, 03_kopf_nah_vorn_x4, 04_kopf_nah_seite_x4, 11_spiel_koepfe (RE15_FRAMEDUMP h135 / h85, Lauf
sfv4/r/V5r3: EXIT=0, 191 Bilder, "EM30 loaded: 23 meshes, 23 bones, 21 clips", 0 Fehlerzeilen; re2r3/lauf_V5r3.txt). Zusatz in re2r3/:
kopf_ref2_r2_r3_x5.png (Foto | Referenz 2 | Runde 2 | Runde 3, h135/h85), ecken_V5r3_x8.png (Ecken-Nummern im Render),
draht_quelle_gerundet.png (Drahtmodell Quelle/gerundet mit Kappe, drei Ansichten).

### Tests
Bau "LOCAL-BUILD-OK (build)" (Engine unveraendert, ninja: no work to do); ctest unit_birkin_formen / unit_birkin_szenen / unit_schwanz_r41 /
integration_r41_sichel_form: "100% tests passed, 0 tests failed out of 4", Total Test time (real) = 64.48 sec (re2r3/ctest_v5r3.txt).
Variante sfv4/emd/EM30_V5r3.EMD = EM30_V4.EMD (sha256 dd5460740e9a18cfe7aad5a06f1466fb2ea7dc06144c669a58e3ca9ff7dd7872, 262796 Byte).
Spiel-Asset EM30_HANGAR.EMD (cfd1ac42) und Pin release/em30_hangar.sha256 unveraendert. Runde 2 bleibt mit "kopf_re2_bau.py --r2" bytegleich
baubar (e9c66c2e nachgerechnet).

### Offen
1. **Lage des Kopfes seitlich**: auch gerundet steht die Kopf-Vorderkante in Pose 85 bei x 481, im Foto bei 464 (Ueberstand max 17 px =
   0,07 H). Das ist die Lage des ganzen Kopfes, nicht die Spitze: das Auge liegt ebenso weit vor dem Foto-Auge (S2 misst relativ zur Vorderkante).
   Zurueckschieben (Versatz) ist ein eigener Schritt, der K4/S2/K10 neu eicht - nicht in diesem Auftrag.
2. Die Spitze ist auf die Kappe gelegt; die Front ist damit stumpf-rund, aber der Kopf bleibt als G2-Form laenglich (Kronenecke 16/26 bildet
   seitlich die Vorderkante). Ob das "abgerundet wie auf dem Foto" genug ist, entscheidet der Blick des Nutzers auf Leiste 02/04/11.
3. T-V1-Formel (Kratzer gegen lokalen statt Kopf-Median) und T-V2b-Grenze (Relief der G2-Quelle P75 7,7 % statt fester 3 %) sind Bauer-Wahl
   mit Begruendung oben; Pruefer bitte gegenlesen.
4. Graue Seite im Render L* ~39 (Videoraum 41,5) liegt bei der hellen Schulterhaut, nicht beim Haut-Median des Rumpfes (27,7): mit dem
   Rumpf-Median waeren T-K9e und K5 rot. S2 0,032 und K6 29,4 / 29,7 liegen knapp an der Grenze.
5. Schwanz T5 0,098 (seit Runde 5), Einbau ins Spiel-Asset erst nach der Abnahme.

## V5 Runde 4 (Nutzer: kein Schnabel ueber dem Mund) (Bauer, 2026-11-08)

Grundlage: V5 Runde 3 (EM30_V5r3, sha256 dd546074). Diese Runde aendert nur die Geometrie und die Lage des Kopf-Meshes (Mesh 9).
Farben, Texel, UVs, Zonen und die CLUT-Zeilen 486/487/488 bleiben bytegleich Runde 3 (T-R1b/c/d gleich, T-V1 gruen). Koerper (Referenz 2),
Schwanz (Runde 3 ot3), Knochen und Clips des EM30 unveraendert. Spiel-Asset EM30_HANGAR.EMD (cfd1ac42) und Pin release/em30_hangar.sha256 unveraendert.
Kein RE-Befund (die Sichel-Form gibt es auf keiner Disc): keine Zahl hier ist eine @0x-Konstante; jede Zahl ist ein Mass gegen Foto/Referenz.

### Nutzer-Wortlaut (2026-11-08, bindend)
"er hat immer noch eine schnabelmäßige Form über den Mund." (nach den Runde-3-Leisten 01/02/03/04). Weiter gueltig: "vorne am Kopf eine Verlängerung
wie so ein spitzer Schnabel in grau. AUch die gibt es nicht. Es ist abgerundet so wie es auf dem Original Foto aussieht."

### Befund (gemessen, sfv5r4/probe1.py, geo.py, achsen.py, proj.py)
- Der G2-Kopf ist ein Keil: im eigenen Raum (x vorn, y unten) liegt die Oberkante von Scheitel 16/26 (x 80, y -347) ueber 11/8/7/10 (x 147-228) bis
  zur Keilspitze vor; die Maulvorderkante (Ecke 41) liegt bei x 185, das Kinn weicht nach hinten (45: x 154, 9: x 82). Runde 3 hat nur die Spitze
  (9 Ecken) auf eine hohe Ellipsoid-Kappe (Halbachsen 214 / 397 / 173 = Seitenverhaeltnis 1,85, selbst eine Keilform) gelegt.
- Pose 85 sieht den Kopf schraeg: G2 +x (Schnauze) liegt im Bild (6,6 / 5,2) px je 100 E., G2 -y (oben) bei (8,6 / -5,7); die Vorderkante im
  Spielbild ist die Ecke aus Scheitel und Schnauzenruecken (Ecken 26/16/2/11/1/8, x 477-483). Das ist die "Schnabelform ueber dem Mund".
- Auge seitlich (S2) haengt an der Nabe 39 des Augenfaechers (T19/T20/T41, UV 17/10; x 60, z -126 = auf der Kopfseite hinter der Front): jede
  zurueckgenommene Vorderkante verkleinert S2 (Runde 3: 0,032 bei Grenze 0,03).

### Foto-Ziel Tiefe/Hoehe (Originalfoto 2)
Vorlage 2 ist nicht abgeblendet (Messblatt V4 Abschnitt 1: entblendet wird nur Vorlage 1). Der Kopf-Umriss trennt sich im Foto hinten nicht vom
Buckel (Farbabstand Foto gegen BSS im Videoraum, re2r4/foto2_kopf_dE_bss_x6.png; Umriss mit Port-Kopf, re2r4/foto2_kopf_umriss_x6.png).
Zielwert = Kopfbox seitlich des Messblatts (Abschnitt 3 / p_riegelfenster.py, v4_gemein.KB_ORIG[2]) x 412-467, y 190-263 = 56 x 74 px = **0,757**;
Gegenlesung aus den Messblatt-Merkmalen (Vorderkante x 465, rote Zone Kopfoberseite ab x 418 / y 193, KS3 "Kopf unter dem Auge" bis y 262) = 47 / 69 =
0,68. Grenze des Riegels = 0,757 + 0,10 = 0,857 (die groessere der beiden Lesungen, damit die Grenze nicht strenger ist als das Foto).

### Form: Weg (a) + (b), dazu Augennabe und Lage (kopf_re2_bau.form_r4 / kappe_rund; PARAM_R4)
- **(a) Verkuerzen**: x-Skalierung 0,85 um den Hinterkopf (kleinstes x = Ecke 19, x -248), alle 48 Ecken; Normalen affin (n / (0,85, 1, 1), Laenge
  wie das Original). Begruendung: nur so wird die ganze Keilform kuerzer (Profil x/y 1,024 -> 0,870 vor der Kuppe); (b) allein (sx 1,0) liess
  den Ueberstand bei 15 px (exp1).
- **(b) Runde Kuppe**: Kappe aus drei Massen des verkuerzten Kopfes, kein Fit: Maullinie ym = Oberkante der Maulflaechen (kleinstes y der
  Flaechenmittelpunkte MAUL_T, -129,7), Scheitel yt = -347,0, Maulvorderkante xf = groesstes x der Maulecken (120,1). Mittelpunkt (xf - R, ym, 0) =
  (-97,3 / -129,7 / 0), Halbachsen (R, R, rz) = (217,3 / 217,3 / 185,0) mit R = ym - yt und rz = groesste |z|; Seitenverhaeltnis x : y = **1,00**.
  Ecken oberhalb der Maullinie und vor dem Mittelpunkt mit r > 1 radial auf die Kappe: 14 Ecken (1, 2, 3, 7, 8, 10, 11, 16, 18, 26, 30, 39, 42, 46;
  7/10/18 je ~65 E., 8/11 ~35/25, 16/26/1/2/42 ~20-30, 3/30/39/46 klein). Das Maul sitzt danach direkt unter der Kuppe vorn (Kappe vorn tangential
  an der Maulvorderkante), das Maul ist mit der Verkuerzung 65 E. zurueckgewandert. Keine Flaechen-Umkehr (Flaechennormalen vor/nach, 0 gekippt).
- **(c) Augennabe**: Ecke 39 um (-20, +70, 0) E. (G2-Raum) - gleitet auf der Kopfseite (z der Nachbarn -112..-145) nach hinten/unten; Pose 85: Auge
  nach links (S2), Pose 135: zur Kopfmitte (K4 bleibt). Die Augentexel (UV) bleiben.
- **(d) Lage**: Versatz im Knochen-9-Raum (80, 70, 0) statt (140, 10, 0) = 60 E. zurueck, 60 E. tiefer (Pose 85: Knochen-x (5,0 / 5,0) px, Knochen-y
  (-4,6 / 4,5) px je 100 E. -> ~5,8 px nach links, waagerecht). Knochen, Clips, EDD unveraendert (nur der Mesh-Versatz).
- Normalen der in (b)/(c) bewegten 14 Ecken flaechengewichtet neu (normalen_flaechen wie Runde 3), die uebrigen 34 affin mitgerechnet.
- **Verworfen** (Suche sfv5r4/exp1-12, suche_r4/): sx 0,9 (Ueberstand 9-11 px oder S2 rot); sx 0,8 / 0,75 / 0,7 (K5/K6/K1k am Bild rot, exp9);
  Versatz-Schritt 75/90 (K5/K6/K1k rot, exp11); Kuppe nur entlang -x (x-Klemmung: Scheitel 16/26 hinter 6/33 = Faltung, Spitze am Scheitel, vis1);
  untere Rundung der Kieferecken 4/20/34 (beidseitig: K10-Hoehe 0,143-0,154 bzw. K6 rot; nur (W)-rechts: K10-Hoehe 0,154 knapp, Sicht kaum anders,
  exp6-10, re2r4/suche_vorn_h135_x6.png); weicher Kappenrand (tanh) und Relief-Ausgleich der Nachbarecken (K5 bzw. T-V3ii rot, exp12).

| Mass | Runde 3 (dd546074) | Runde 4 (38562ced) | Grenze |
|---|---|---|---|
| T-V3i Tiefe/Hoehe Pose 85 (Bbox Id 9) | 0,860 (37 x 43 px) | **0,649** (24 x 37 px) | <= 0,857 (Foto 0,757) |
| T-V3i Tiefe/Hoehe Profil rein seitlich (G2-Raum x/y) | 0,899 (418 / 465 E.; Quelle 1,024) | **0,815** (368 / 452 E.) | <= 0,857 |
| T-V3ii vor der runden Kappe (Ecken oberhalb der Maullinie) | 7,8 % (Ecke 16; 26 7,7, 2 6,0, 42 5,7, 3 5,2, 1 3,9) | **1,9 %** (Ecke 33) | <= 3 % |
| T-V3iii / T-V2c Ueberstand seitlich gegen Beschreibung2_Vorlage | 17 px max / 4,8 Mittel; IoU 0,549 | **3 px** / -2,8; IoU 0,779 | <= 8 px |
| Lage: Kopf-Vorderkante Pose 85 (Id 9) / Foto | x 481 / 464-465 | **x 467** / 464-465 | - |
| T-V2a (Runde-3-Mass, Ellipsoid 1 : 1,9) | 1,5 % (Ecke 41) | 3,8 % (Ecke 41) - INFO | s. Offen 2 |
| K10 / K4 / K7 | 0,165 x 0,170 / 0,060 - 0,097 / 0,62 - 0,64 | 0,165 x 0,165 / 0,054 - 0,096 / 0,58 - 0,62 | Messblatt |
| S2 / KS2 | 0,032 - 0,812 / Kopf L* 33,6 | 0,038 - 0,817 / 28,2 (<= 35,5) | Messblatt |
| K1k / K5 / K6 / T-K9e | OK / 0,76 / 29,4 / 24,8 - 0,99 | OK / 0,88 / 27,6 / 24,3 - 0,96 | Runde 2 |

### Riegel (riegel_re2kopf.py, --prot SF_belege/v4/re2r4/kopf_re2_protokoll_r4.json, --lauf sfv4/r/V5r4/debug.log; SF_belege/v4/re2r4/riegel_re2kopf_V5r4.txt/.md/.json)
EMD sha256 38562ced; **Exit 1: 1 rot von 50 (T5 0,098, Schwanz-Geometrie seit Runde 5, bekannt)**, 9 INFO. Neu bzw. geaendert:
| Mass | Soll | Ist | |
|---|---|---|---|
| T-K1a/b/c Koerper = Ref2 | Meshes 0-8/10-15, Zeilen 480-485, Koerper-Texel bytegleich | keine Abweichung; 486/488 = Regel r3, 487 = G3; 0 von 34716 | OK |
| T-R1a Kopf-Geometrie | Ecken = Rz Ry Rx S form(v) + t (form_r4), Normalen, Flaechen, UV, Zonen | max 0,50 E. / 0,50; UV abweichend 0; Zone abweichend 0; 14 Ecken auf die runde Kappe, Augennabe 39, 14 Normalen neu, 34 affin | OK |
| T-R1b/c/d Texel, Zahn-Kachel, Zeilen 486-488 | bytegleich / Regel r3 | 0 von 11340 / 0 von 432 / gleich, gleich, gleich | OK |
| T-V1 Pose 135 / 85 | a* > 12 <= 2 %; Kratzer <= 2 %; b* P10 >= -1 | 0,0 % / 0,9 % / 0,6; 0,0 % / 0,7 % / 0,0 | OK |
| T-V2a (Runde 4: INFO) | Ellipsoid-Kappe der Runde 3 | Ecke 41 3,8 % | INFO |
| T-V2b keine Delle / kein neuer Vorsprung | Rahmen runde Kappe T-V3ii, gegen den verkuerzten G2-Kopf | Grenze 9,1 % (Relief P75); 27 Ecken geprueft; Dellen keine; neue Vorspruenge keine (Runde-3-Rahmen als Ausgabe: 29 7,9 (Quelle 6,3), 43 8,2 (Quelle 5,0)) | OK |
| T-V2c Silhouette seitlich | IoU >= Vorgaenger (Runde 3) | 0,779 >= 0,549 | OK |
| **T-V3i** Tiefe/Hoehe | Pose 85 und Profil <= 0,757 + 0,10 | 0,649 / 0,815 | OK |
| **T-V3ii** runde Kappe | Seitenverhaeltnis 1,00 <= 1,05; Ecken oberhalb der Maullinie <= 3 % vor der Kappe | Kappe am geprueften Kopf c (-84,3 / -129,7), Halbachsen 204,5 / 204,5 / 185,0; 22 geprueft, max 1,9 % (Ecke 33) | OK |
| **T-V3iii** Ueberstand seitlich | <= 8 px | 3 px | OK |
| K10 / K4 / K7 / S2 / KS2 / K1k / K5 / K6 / T-K9e / T-R3 | Messblatt-Fenster wie Runde 3 | alle OK (Zahlen s. Tabelle oben; T-R3 45 Marker / 46 hell) | OK |
| T5 | f 0,5 >= 0,100 | 0,098 | ROT (bekannt) |
Die Kappe in T-V3ii wird am GEPRUEFTEN Kopf nach derselben Regel wie im Bau bestimmt (kein Fit); da (b) die Scheitel-Ecken 16/26 absenkt, ist sie
am Ergebnis kleiner (R 204,5 statt 217,3) - die Ecken liegen trotzdem hoechstens 1,9 % davor (Iteration bis zum Fixpunkt verworfen: die Kappe
schrumpft je Schritt weiter, R 204,3 -> 196,5 -> 190,6 -> 185,9 ..., Probe im Scratchpad).
**Gegenprobe V5 Runde 3** (riegel_re2kopf_V5r3_gegenprobe.txt, --prot re2r3/kopf_re2_protokoll_r3.json, --vorher EM30_V4re2): 4 rot von 51 -
T-V3i (Pose 85 0,860, Profil 0,899), T-V3ii (7,8 % Ecke 16; ueber 3 %: 16, 26, 2, 42, 3, 1), T-V3iii (17 px), T5. Der Riegel unterscheidet.

### Leisten (SF_belege/v4/; Runde 3 nach v4/re2r3/leisten/ verschoben; Spalten Originalfoto (roh) | Nutzer-Beschriftung | Referenz 2 | neu)
01_vorn_kamera12, 02_seite_wie_bild2, 03_kopf_nah_vorn_x4, 04_kopf_nah_seite_x4, 11_spiel_koepfe (RE15_FRAMEDUMP h135 / h85, Lauf sfv4/r/V5r4:
EXIT=0, 191 Bilder, "EM30 loaded: 23 meshes, 23 bones, 21 clips", 0 Fehlerzeilen; re2r4/lauf_V5r4.txt) und neu **12_kopf_profil_x6**
(Mesh 9 in den G2-Raum zurueckgerechnet, orthogonal, neutrales kamerarelatives Licht, roh; Zeile 1 rein seitlich mit runder Kappe (gruen) und
Maullinie (gelb), Zeile 2 rein von vorn; Spalten neu | Runde 3, gleicher Massstab und Ursprung je Zeile).

### Tests
Bau "LOCAL-BUILD-OK (build)" (Engine unveraendert, ninja: no work to do); ctest unit_birkin_formen / unit_birkin_szenen / unit_schwanz_r41 /
integration_r41_sichel_form: "100% tests passed, 0 tests failed out of 4", Total Test time (real) = 64.42 sec (re2r4/ctest_v5r4.txt).
Variante sfv4/emd/EM30_V5r4.EMD = EM30_V4.EMD (sha256 38562ced41061258f435be0d6598697b10ed241b09ba1a948007bc5bdf5993d5, 262796 Byte).
Runde 3 bleibt mit "kopf_re2_bau.py --r3" bytegleich baubar (dd546074 nachgerechnet), Runde 2 mit --r2.

### Offen
1. **Vorn (Pose 135 / Spielbild h135)**: die beleuchtete (W)-rechte Kopfseite bildet links vom Maul weiter eine graue Sichel bis zur Mundecke
   (Leiste 03/11 Zeile 1). Das ist die Seitenflaeche des G2-Kopfes unter dem Augenfaecher (Ecken 3/17/5/20/4), keine Spitze mehr; sie wird
   durch die Verkuerzung kuerzer, verschwindet aber nicht. Eine untere Rundung der Kieferecken (Variante 'unten') machte die Kopfbox zu niedrig
   bzw. K6 rot. Ob die Front von vorn jetzt rund genug ist, entscheidet der Blick des Nutzers auf Leiste 03/11/12.
2. **T-V2a/T-V2b bei Form r4 (Bauer-Wahl, Pruefer bitte gegenlesen)**: T-V2a misst gegen die Ellipsoid-Kappe der Runde 3 (1 : 1,9, selbst keilfoermig);
   am verkuerzten Kopf liegen dort viele Ecken 3-6 % ausserhalb (46 6,5 %, 42 4,1 %, 41 3,8 %) - das misst den Fit, nicht einen Schnabel. Fuer
   Form r4 ist T-V2a darum INFO, die Pruefung leistet T-V3ii (runde Kappe, Auftrag). T-V2b rechnet im Rahmen der runden Kappe; im Runde-3-Rahmen
   waeren 29 (7,9 % gegen Quelle 6,3 %) und 43 (8,2 % gegen 5,0 %) "neue Vorspruenge" - beide Ecken wurden nicht bewegt, ihre Nachbarn (Kuppe bzw.
   Augennabe) ruecken ab; Zuwachs 7-14 E. = unter 1 px im Spielbild. Beide Zahlen stehen in der Riegelzeile.
3. S2 0,038 / 0,817 liegt mit der Hoehe knapp unter 0,82; K10-Hoehe 0,165 (Runde 3 0,170), weil die Scheitel-Ecken 16/26 auf die Kappe sinken.
4. Schwanz T5 0,098 (seit Runde 5), Einbau ins Spiel-Asset erst nach der Abnahme.

## V6 Kopf aus dem Original RE1.5-Birkin (Bauer, 2026-11-08)

Grundlage: V5 Runde 4 (EM30_V5r4, sha256 38562ced). Diese Runde ersetzt den RE2-G2-Kopf vollstaendig durch den ORIGINAL-Kopf des RE1.5-Birkin
(EM30 = CDEMD0.EMS Eintrag 16) und erweitert nur dessen Textur. Koerper (Referenz 2), Schwanz (V4), Knochen, Clips, EDD unveraendert.
Spiel-Asset EM30_HANGAR.EMD (cfd1ac42) und Pin release/em30_hangar.sha256 unveraendert (nachgerechnet). Nichts in Version_80%/, pri/, synchro/.
Kein RE-Befund (die Sichel-Form gibt es auf keiner Disc): keine Zahl hier ist eine @0x-Konstante; Texel-Koordinaten sind Lesestellen der
Original-Textur, Grenzen sind Masse gegen Foto/Original.

### Nutzer-Wortlaut (2026-11-08, bindend, geht allen frueheren Kopf-Vorgaben vor)
"Für die Textur anders - das was haare sind, sind offenbar keine Haare. Orientiere dich an den Original Birkin in Resident Evil 1.5. Der hat auch
schon links in seinen Gesicht diesen grauen Fleck. Der scheint bei den mutierten Modell einfach nur erweitert zu sein. Das Hellgrau in der Mitte
auch, so dass der Mund eher geschlossen ist. Also nutze das Original Resident Evil 1.5 Birkin Modell als Ausgangsgrundlage, damit solltest du nicht
viel falsch machen." und "Immerhin ist dieses Modell ja eine Mutation/Weiterentwicklung des anderen Models! Dementsprechend muss ein Teil des
Vormodells erkenntbar drin sein."

### Schritt 0: Ist der Kopf der Referenz 2 das Original? (gemessen, sfv6/p1.py-p3.py)
Nein. Massgeblich ist deshalb das Original selbst:

| | Original EM30 (CDEMD0.EMS Eintrag 16, 226404 B, sha256 569e3138) | Referenz 2 (EM30_HANGAR.EMD) | V5 Runde 4 |
|---|---|---|---|
| Mesh 9 | 61 Ecken / 61 Normalen / 73 Dreiecke / 18 Vierecke | Ecken = round(1,05 x Original) (FORMEN skal 9), Flaechen gleich | RE2-G2-Kopf, 48 / 48 / 56 / 14 |
| UV / TPage | TPage 128, u 56-127 v 136-254 (6501 Texel) | auf das Wundfleisch u 66-126 v 4-90 umgelegt | G2 x 0-125 y 160-249, G3-Kachel |
| CLUT | Zeile 480 (= Zeile 0 der EM30-TIM, Original-Palette) | Zeile 485 (Kopf-Zeile aus Zeile 480, Haut-Transformation) | 486 / 487 / 488 |
| Kopftexel u 56-127 v 136-254 | Original | 6501 von 6501 gleich dem Original | zu 85 % vom G2-Kopf ueberschrieben |

Die Zeilen 480-482 der Referenz 2 sind die Haut-Transformation (nicht Original). Von den Original-Kopftexeln nutzen die Meshes 10 und 13 vier Texel mit.

### Rueckbau und Aufbau (re15_port/tools/r41_sichel/v6kopf/kopf_orig_bau.py; Protokoll SF_belege/v4/v6k/kopf_orig_protokoll.json)
1. Mesh 9 := Original-Mesh 9 bytegleich (Ecken, Normalen, Flaechen, UV); nur das CLUT-Feld jeder Flaeche zeigt auf Zeile 486 (Spalte wie das
   Original). Form, Groesse (1,0 statt 1,05 der Referenz 2) und Lage im Knochen-9-Raum = Original; das Skelett (Bindoffset Knochen 9 der Referenz 2)
   bleibt, weil es zum Koerper gehoert.
2. RE2-G2-Kopf, G3-Zahnkachel und die Zeilen 486/487/488 der V5 zurueckgebaut: V5-Einsaetze TPage 128 x 0-125 y 160-249 und TPage 130
   x 256-267 y 200-235 auf Referenz 2 (11722 Texel). Schwanzfeld TPage 130 x 320-383 bleibt V4.
3. CLUT: 480-485 bytegleich Referenz 2; **486 = Original-Zeile 0 bytegleich**; 7 Zeilen (vorher 9), Port-Texturhoehe 256 x 7 = 1792 <= 4096.
4. Kopftextur erweitert (texel_erweitern.py, nur Texel, die AUSSCHLIESSLICH Mesh 9 nutzt; die 4 mitgenutzten Texel der Meshes 10/13 unberuehrt).
5. Ruecken -> Kopf (glaetten, Pose P2): Original-Kopf 3 px Wiederanstieg (Ecke 60 = Hinterkopf oben) -> 2 Ecken eingezogen.

Varianten: sfv4/emd/**EM30_V5k.EMD = EM30_V4.EMD** (sha256 99703bbda76ebda1317555c0e7c5e5669938c32df408a8d9a2042db570bd679e, 262516 Byte);
Spalte "Original" = sfv4/emd/EM30_V6orig.EMD (Stufe 0: Schritte 1-3 ohne 4/5, sha256 419ccfd4bcf26a83). Bau deterministisch (zweimal gebaut, gleiche sha256).

### Lesart der Original-Kopftextur (Texelkarte x10, L*/a*/b*-Raster der Original-Palette; Leiste k_textur.jpg)
- Die Textur liegt aufrecht und ungespiegelt: Texturbild links = (W)-rechte Gesichtshaelfte = Bild-links in der Vorderansicht.
  Sichtbare Texel: Pose 135 u 65-126, P1 u 58-124, P2 ((W)-rechts zur Kamera) u 59-113.
- **Grauer Fleck** (Nutzer: "links in seinem Gesicht"): dunkelgrauer Streifen unter dem (W)-rechten Auge, u 58-72 v 188-226 = **Bild-links,
  (W)-rechts**. Fleck-Texel (Box u 58-74 v 186-226, b* <= 10, L* <= 50): 300 Texel, 34 Indizes, L* P10/50/90 10,3 / 25,2 / 47,9, C* bis 19,9.
- **Hellgrau der Mitte**: Stirn / Nasenruecken / Wange u 72-100 v 166-221, L* 55-84, a* 2-8, b* 0-5.
- **Auge**: (W)-rechtes Auge = Texturbild links, Augenweiss L* 60-64 bei u 74-84 v 194-198 - ein heller Fleck; die Lage passt zu Beschreibung1
  "Auge links" und zum Messblatt-Augenfleck (W)-rechts (Foto 1 und 2). Das (W)-linke Auge (Texturbild rechts) ist rot in einem violetten
  Aderfeld u 96-127 v 168-212. Beide unveraendert, nichts dazugemalt.
- **Was als "Haare" gelesen wurde** (Messblatt 3.4: dunkles Feld Bild-rechts = (W)-links): das ist das violette Aderfeld der Original-Textur ->
  unveraendert, kein Haarbogen, kein dunkles Haarfeld. Das blonde Haar oben (b* 10-25) ist Original und bleibt.
- **Mund**: offener Mund u 80-104 v 222-238 (Hoehle L* < 15, Zaehne L* 50-68, rote Lippen a* > 20), rotes Kinnfleisch v 238-254; rechts davon
  (u 104-127 v 200-254) aufgerissene Wange - unveraendert.

### Textur-Erweiterung (texel_erweitern.PARAM; nur Indizes der Original-Textur)
- **(a) Zone A, grauer Fleck -> (W)-rechte Gesichtshaelfte bis zur Nase**: Polygon (58,186) (68,186) (70,200) (86,201) (87,221) (81,223) (80,254)
  (58,254) = vom Kopfrand bis an den Nasenruecken (u 88-96 bleibt hell), unter dem (W)-rechten Auge, unten bis v 254. Rangabbildung: jeder
  ersetzte Texel behaelt seinen Helligkeitsrang und bekommt den Fleck-Index desselben Rangs (Wangenschatten/Falten der Original-Textur bleiben,
  Farben = die des Flecks). Der Fleck selbst bleibt. 915 Texel.
- **(b) Zone B, Hellgrau ueber den Mund**: Polygon (81,222) (100,221) (105,224) (106,254) (81,254) = Mund, Lippen, rotes Kinn in der Mitte;
  Quelle = Hellgrau-Feld der Stirn u 74-92 v 166-185 (ohne Adern ab u 93, ohne Brauen ab v 187) als EIN Stueck auf die Box von B gelegt
  (naechster Texel): die Hautstruktur der Stirn laeuft ueber den Mund. 847 Texel.
- Raender beider Zonen ausgefranst wie der Original-Fleck: Abstandsrampe +-1,5 Texel, Rand +-1,5 Texel durch ein geglaettetes Rauschfeld
  verschoben, Ersatz wenn Gewicht > gleichverteilte Schwelle (Startwert 4108). Keine geraden Kanten.
- Verworfen (Bilder im Scratchpad sfv6/): Fleck nur der dunkle Kern L* <= 30 (A wurde eine glatte dunkle Flaeche, tex_vgl_a30.png); B als
  Spiegelkachel (Rorschach-Muster, tex_vgl_spiegel.png) und als Zufallsflicken 3 x 3 (Schachbrett, tex_vgl_flicken.png); erste Fassung mit
  geraden Polygon-Kanten (Rechteck-Kleckse).
- **Ausnahmeliste T-K1** (Kopf-exklusiv): 1762 Texel geaendert, Box u 60-107 v 186-254, alle nur von Mesh 9 genutzt; Indizes nur aus den Quellen
  (Fleck 34, Hellgrau 51 Indizes); 0 Texel ausserhalb der Kopf-exklusiven Menge. Zeile 486 = Original-Zeile 0 (keine Palettenaenderung).

### Ruecken -> Kopf (Nutzer 4; kopf_orig_bau.glaetten, Pose P2)
Obere Kontur (Knochen 0-15) von der Buckelkuppe (x 405, y 175) zur Kopf-Vorderkante: mit dem Original-Kopf faellt sie bis x 434 (y 196) und
steigt am Hinterkopf wieder auf y 193 (x 437) = 3 px Huckel. Geglaettet (einzige Geometrie-Aenderung; Normalen, Flaechen, UV, Texel, Palette
bytegleich): Ecke 60 (-184,-327,-21) -> (-156,-312,-21) (8,1 % zum Mesh-Schwerpunkt, 31,8 E.), Ecke 54 (-103,-369,-23) -> (-101,-367,-23)
(0,8 %, 2,8 E.). Danach 2 px (x 435 y 197 -> x 437 y 195). V5 Runde 4 hatte dort 5 px (Kopf als eigener Buckel hinter der Nackensenke).
INFO: Vor dem Kopf liegt die Modell-Kontur weiter bis 14 px ueber der Foto-2-Kontur (x 418, Knochen 8 = Buckel der Referenz 2) - das ist die
Buckelhoehe des Koerpers, nicht der Kopf; Koerper unveraendert (Regel), s. Offen 2. Kopf-Oberkante Modell y 195 / Foto y 194 bei x 437.

### Riegel (riegel_v6kopf.py --emd EM30_V5k.EMD; SF_belege/v4/v6k/riegel_v6kopf_V5k.txt/.json) - Exit 0, 0 rot von 10, 2 INFO

| Mass | Soll | Ist | |
|---|---|---|---|
| T-K1a/b | Meshes 0-8/10-22, Sektionen 0-6 = Basis V5r4 | keine Abweichung | OK |
| T-K1c | 480-485 = Referenz 2; <= 16 Zeilen; <= 4096 | gleich; 7 Zeilen; 1792 | OK |
| T-K1d | Koerpertexel = Referenz 2; Abweichung nur Schwanzfeld (= Basis) und Kopf-exklusiv | Koerper 0; Schwanzfeld = Basis; Kopf 1762 (u 60-107 v 186-254) | OK |
| **T-H6** | Mesh 9 = Original bis auf die Glaettung (<= 4 Ecken, Gerade zum Schwerpunkt, <= 10 %) | 61/61/73/18; Normalen, Flaechen, UV gleich; CLUT-Feld 486; Glaettung 54 (0,8 %), 60 (8,1 %) | OK |
| **T-H5** | Kopf nur Zeile 486 = Original-Zeile 0; Indizes aus den Original-Kopftexeln | Zeilen [486]; 486 = Original; 218 Indizes, 0 neu | OK |
| **T-H1i** | Maulkern (Zone B 2 Texel eingezogen): dunkel (L* < 30) <= 2 %, rot (a* >= 15) <= 2 %, Median L* >= 55 | 622 Texel: 0,0 % / 0,0 % / 75,9 | OK |
| **T-H1ii** | sichtbar je Pose: dunkel/rot (Palette) <= 5 %, beleuchtet a* >= 10 <= 10 % | 135: 86 px 0/0/8 %, Lab 41,6/1,8/2,6; P1: 38 px 0/0/5 %; P2: 16 px 0/0/0 % | OK |
| **T-H2** | Gesichtshaelfte bis zur Nase (Texel u 58-86 v 186-254 ohne Haar/Auge/Zone B), Fleck-Klasse (L* <= 50, C* <= 20): je Pose neu >= Foto 1 - 0,10 und >= Original + 0,10 | Foto 1 (x 368-395 y 150-191 ohne Augenfleck) 0,99; 135 1,00 / Original 0,37 (30 px); P1 1,00 / 0,74 (111 px); P2 1,00 / 0,69 (141 px); Texel 0,99 / 0,58 | OK |
| **T-H4** | P2: Wiederanstieg vor dem Kopf <= 2 px | 2 px | OK |
| INFO (d) Auge | Foto 1 L* 47,0 / Foto 2 L* 48,5 | sichtbares Augenweiss: P2 3 px bei (451/226), L* 26; P1 5 px L* 20; 135 6 px L* 12 (Schatten) | INFO |
| INFO T-H4 | Abstand zur Foto-2-Kontur | max 14 px bei x 418 (Knochen 8) | INFO |

**Gegenprobe V5 Runde 4** (riegel_v6kopf_V5r4_gegenprobe.txt): 6 rot von 10 - **T-H6** (48 Ecken statt 61), **T-H1i** (Zahnkachel 487: dunkel 53 %,
rot 84 %, L* 28,7), **T-H1ii** (135: rot 91 %, Licht-rot 64 %), T-H5 (Zeilen 486-488, 19 fremde Indizes), T-H2, T-H4 (5 px).
**Gegenprobe Original unveraendert** (EM30_V6orig, riegel_v6kopf_V6orig_gegenprobe.txt): 4 rot - T-H1i (dunkel 39 %, rot 81 %), T-H1ii, T-H2
(135 0,37 / P1 0,74 / P2 0,69 < Foto 0,99 - 0,10), T-H4 (3 px). Der Riegel unterscheidet Original, V5 und neu.

### Leisten (SF_belege/v4/; roh, x4 bzw. x6)
- k_kopf_foto1.jpg: Zeile Pose 135 (Kopf von vorn, Messblatt) und Pose P1 (Clip 20 Bild 139 Gier 1422); Spalten Foto 1 roh | Foto 1 entblendet |
  ORIGINAL RE1.5-Kopf unveraendert | neu.
- k_kopf_foto2.jpg: Pose P2 (Clip 12 Bild 48 Gier 861); Foto 2 roh (nicht abgeblendet) | ORIGINAL | neu.
- k_textur.jpg: Kopftextur u 50-127 v 130-255 x6, Original | neu | neu mit Zonen A (blau) / B (gruen) und Quellen (Fleck gelb, Hellgrau weiss).

Selbst angesehen: Der Original-Birkin ist erkennbar (blondes Haar, menschliches Gesicht, rotes (W)-linkes Auge im violetten Feld, aufgerissene
Wange). Der graue Fleck ueberzieht die (W)-rechte Wange bis an die Nase (k_textur; in 135 ist das Gesicht unten links dunkler). Der Mund ist zu,
die Stelle hellgrau (135: L* 41,6, a* 1,8). Kein Schnabel (Original-Kopfform), kein Haarbogen, kein dunkles Haarfeld. Im Spielbild ist der Kopf
klein und schaut in P1/P2 nach unten, darum zeigt er dort vor allem das blonde Haar von oben.

### Tests
Bau "LOCAL-BUILD-OK (build)" (ninja: no work to do; Engine unveraendert). ctest -R "unit_birkin_formen|unit_birkin_szenen|unit_schwanz_r41|
integration_r41_sichel_form": "100% tests passed, 0 tests failed out of 4", Total Test time (real) = 64.52 sec (SF_belege/v4/v6k/ctest_v5k.txt).
Kein Spiellauf mit RE15_FRAMEDUMP in dieser Runde (Leisten aus dem Renderer v4_gemein.zeichne, Spielkamera 50A0/12, RDT-Licht).

### Offen
1. **Blondes Haar**: Das Original-Haar (gelblich, b* 10-25) bleibt nach Auftrag unveraendert. In P1/P2 sieht man den Kopf vor allem von oben,
   dort ist das Haar die groesste Flaeche; die Fotos zeigen an der Stelle Grau. Ob das Haar ebenfalls ergraut sein soll, entscheidet der Nutzer
   (Weg waere dieselbe Rangabbildung auf die Fleck-Indizes).
2. **Buckel ueber der Foto-Kontur**: Vor dem Kopf liegt der Koerper (Knochen 8, Referenz 2) bis 14 px ueber der Foto-2-Kontur; in P1 liegt der Kopf
   ganz unter der Schulterkontur. Die Kontur Ruecken -> Kopf selbst ist jetzt stetig (2 px); den Buckel abzusenken waere eine Koerper-Aenderung und
   nicht Teil dieses Auftrags.
3. **Auge im Licht**: Das (W)-rechte Augenweiss liegt in P2 bei (451/226) gegen den Foto-Fleck (452,5/237,2), also 11 px hoeher, und im Schatten
   (L* 26 gegen 48,5), weil das Gesicht in dieser Pose nach unten zeigt. Nichts dazugemalt (Auftrag d).
4. **Kopfgroesse/-lage**: Original-Groesse 1,0 (Referenz 2 hatte 1,05); Kopfbox 135 x 333-374 y 158-213, P2 x 432-461 y 195-238. Der Abstand zur
   Foto-Kopfstelle in Foto 1 (Messblatt Offen 1) bleibt.
5. Das Hellgrau ueber dem Mund kommt aus der Stirn (Paletten-L* 55-84) und ist im Texturbild heller als die Wange daneben; im Spielbild (Licht) L* 20-42.

## V5 wie Foto - Koerper (Bauer, 2026-11-08)

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc, keine Zahl hier ist eine @0x-Konstante. Masse = Bildmessung an den Originalfotos
(SF_MESSBLATT_FOTO.md, a909bd2cd) und am Modell (v4_gemein.zeichne, Spielkamera 50A0/12, RDT-Licht). Werkzeuge
re15_port/tools/r41_sichel/v5koerper/ (Commit 42757e3fa), Belege SF_belege/v4/v5f/ und SF_belege/v4/20-24_*.jpg (Commit d8a589057).
Variante sfv4/emd/EM30_V5f.EMD = EM30_V4.EMD, sha256 784c8f05dfc4ac57dfdf3ac52127ac2a4e900603043aafa0c6bada897a2d3019, 264564 Byte,
zweiter Bau gleiche sha256. Basis EM30_V5k.EMD (99703bbd). Kopf (Mesh 9, Zeile 486) unveraendert. Spiel-Asset EM30_HANGAR.EMD (cfd1ac42) und
Pin unveraendert, nachgerechnet.

### Nutzer-Wortlaut (2026-11-08, bindend)
(1) "In der Seitenansicht sieht man auch weder das Auge, noch den dicken schwanz..."; (3) "im original ist der Schanz wie man sieht gräulich
und vom rücken aus geschlossen ein zusammenhängendes stück Das direkt I. den Rücken übergeht. wir haben da wieder diverse komische huckel..."
und "dunkel grünlich"; (5) "Auch der rote Fleck oben auf der rechten Schulter, den gibt es in der Vorlage ganz offensichtlich nicht. In der
Vorlage ist es grau wie der Rest..."; (8) "Die roten Stellen sind korrekt auf der brust und am oberen linken Arm sonst dominiert im original
eher das grau."; (9) "Und die klauen der linken Hand sieht irgendwie falsch aus und zu klein." mit Klarstellung "gemeint ist die kleine Klaue
auf der linken Hand des Wesens."

### a) Schwanz: Form (schwanz_form.py, schwanz_suche.py; nur Ecken der Meshes 16-22)
- Messung der Breite: Der Lauf entlang der Normalen (Messblatt 5) misst an der Biegung quer durch die Ecke (Foto f 0,4: 0,225 H, f 0,5:
  0,083 H). T-F1 misst deshalb den Querschnitt = Inkreis-Durchmesser auf der Normalen durch die Station, Foto und Modell mit derselben
  Funktion; Foto-Achse = Nutzerlinie Beginn -> Biegung -> Knick -> Ende, Modell-Achse = projizierte Gelenkkette ab Gelenk 17 (Beginn).
  Foto-Soll = Median ueber 3 Stationen (roh f 0,7 / 0,8 / 0,9 = 0,033 / 0,055 / 0,033 H, Maskenrauschen):
  f 0,1-0,9 = 0,144 / 0,145 / 0,144 / 0,114 / 0,077 / 0,077 / 0,055 / 0,033 / 0,033 H.
- Huckel-Ursachen in V5k (gemessen, Knochenraum): Zwischenring bauch im Glied 18 mit 1,374 x linear (340 E. bei Gelenkradius 268,
  Steigungsknick 31 Grad), Endglied 22 dicker als 21 (82 gegen 58 E.).
- Bau: jeder Ring um seine Mitte skaliert, Manschette (-0,45 r) und Apex nach der Generatorregel neu gelegt; Flaechen, UV, Normalen,
  Skelett, Keyframes bytegleich. Endring von Glied k und Anfangsring von Glied k+1 tragen denselben Radius (deckungsgleich in der
  Ruhe-Kette). Die Manschette bleibt: die Schwanzgelenke drehen sich in den Keyframes um bis zu 86 Grad gegen das Mittel (gemessen ueber
  912 Keyframes), ohne Manschette oeffneten sich dort Spalten.
- Radien (Suche Lauf 3, Saat 9, 600 Schritte; Strafe aus T-F1, Steigungsknick <= 15 Grad, Riegel V4 T1-T5/TA/TK in Pose 85; Protokolle
  v5f/schwanz_suche_s1-9.log): Gelenk 0-7 = 289 / 258 / 258 / 160 / 151 / 60 / 65 / 65 E. (V5k 315 / 281 / 268 / 135 / 98 / 58 / 82 / 82);
  bauch t 0,334, 269 E. (4,5 % ueber Gelenk 2), Versatz 0,023 r nach aussen. Die Taille am Knick (Gelenk 5) bleibt, das Endstueck ist 7 %
  dicker (Steigungsknick 9,5 Grad).
- Wurzel: Glied 16 liegt wie bisher im Rumpf; am Wechsel Schwanz -> Rumpf ist die obere Kontur in P2 stetig (Stufe 1 px, keine Luecke).
  Den Anfangsring auf die Rueckenflaeche zu legen war dafuer nicht noetig; ein groesserer Wurzelradius aendert die Kontur nicht (r0 289-420
  geprueft, die sichtbare Oberkante dort ist Glied 17).

### a) Schwanz: Farbe (koerper_bau.schwanz_palette; Zeile 487)
Zeile 487 = Kopie der Referenz-2-Zeile 480; jeder der 187 Indizes, die die Schwanztexel tragen, bekommt den Eintrag der Sichel-Zeile 484
(Referenz 2, Mesh 15, 126 Indizes) mit demselben Helligkeitsrang (15-Bit-Wert bytegleich aus 484). Texel bytegleich. Paletten-Median der
Schwanz-Indizes 32,2 / 14,3 / 7,3 -> 23,3 / -1,7 / 5,0. Gerendert (P2, Videoraum): 22,8 / -1,5 / 3,4 gegen Foto-Schwanz 17,8 / 4,8 / 1,5 und
Modell-Rumpf grau 30,3 / 2,3 / 2,1: dunkler als der Rumpf, a* leicht negativ (Nutzer "dunkel gruenlich"; im Foto-Lab nicht messbar,
Messblatt Offen 4).

### b) Rot nur Brust und oberer Arm (koerper_bau.rot_behalten / kuppe_13 / grau_zeile)
- Lesart (Messblatt 4.3/4.4): "oberer linker Arm" = Bild-links in Foto 2 = (W)-rechter Oberarm (Knochen 13); "rechte Schulter" = Foto 1
  Bild-links = (W)-rechts.
- Zeilen 488 / 489 / 490 = Referenz-2-Zeilen 481 / 480 / 482 bytegleich. Auf die Kopie zeigen (nur CLUT-Feld): Mesh 8 Brust (f_analyse.teil am
  Flaechenschwerpunkt, ohne 8/v24, 8/v26) 34 Flaechen -> 488; Mesh 13 ohne Schulterkuppe 43 Flaechen -> 489; Mesh 12 Klauenhand (im Foto rote
  Handflaeche, Messblatt 6) 58 Flaechen -> 490.
- Schulterkuppe (kuppe_13): 13/d30, d32, d33 (Messblatt 4.4) und jede Flaeche von 13, deren in P1 sichtbare Pixel zu >= 50 % (>= 8 px) in der
  Foto-1-Region Schulter Bild-links (x 296-355, y 175-250, Foto rot 0,5 %) liegen. Grund: mit nur d30/d32/d33 blieb die Region zu 33 % rot
  (13/v11, d18, v6, v3, d3, d17, d16). Ergebnis: d3, d18, d30, d32, d33, v3, v10, v11, v12, v13, v14, v17, v18.
- Zeilen 480 / 481 / 482: jeder Eintrag mit Paletten-a* >= 10 (155 / 104 / 150 Eintraege) wird grau: L* gleich bzw. tiefer, a*/b* = graue
  Rumpfhaut im Foto 2 (3,9 / 1,5, Messblatt 5); 5-Bit-Wert = naechster Nachbar, der nicht heller ist. Der erste Lauf mit dem Median der grauen
  Paletten-Eintraege (Zeile 480: 3,1 / 5,2) ergab im Bild ein Braun (Leiste selbst angesehen), darum die Foto-Haut. C* aller neuen Eintraege
  <= P90 der grauen Eintraege derselben Zeile (Riegel T-K1d).
- CLUT: 11 Zeilen (480-490), Port-Texturhoehe 2816 <= 4096.

### c) Krallen (W)-links (koerper_bau.krallen_strecken; Mesh 12)
Je Kralle (f_krallen.krallen: Flaechen mit allen Ecken y >= 330) die Ecken um die Krallenwurzel gestreckt, Faktor = 0,036 H / projizierte
Laenge in P1 (Messblatt 6: Keil 11 px hoch), nie kuerzer. Faktoren 1,92 / 1,00 / 1,32 / 2,37; Laengen 0,019 / 0,038 / 0,027 / 0,015 H ->
0,037 / 0,038 / 0,036 / 0,036 H. 9 Ecken geaendert; Flaechen, UV, Normalen, Textur unveraendert. Die Krallen bleiben dunkel wie die Hand
(Foto: heller Keil L* 30), die Textur ist nach Auftrag unveraendert.

### Riegel (riegel_v5f.py; SF_belege/v4/v5f/riegel_v5f_V5f.txt/.json) - Exit 1, 12 rot von 42
| Pruefung | Ergebnis |
|---|---|
| T-K1a-d Ausnahmeliste (Sektionen und Texel bytegleich; nur CLUT-Felder der Rot-Flaechen/des Schwanzes, Krallen- und Schwanzecken; CLUT 483-486 = Basis, Kopien bytegleich, nur rote Eintraege grau, 487 aus 484) | gruen |
| T-F1 Schwanzbreite je Station (P2) +-20 % | gruen, groesste Abweichung 16 % |
| T-F5a Schwanz dunkel gruenlich-grau | gruen: 22,8 / -1,5 / 3,4 |
| T-F5b Gelenkringe deckungsgleich | gruen: 0,0-0,1 % |
| T-F5c Breitensprung im Bild (P2) | gruen: 3 / 3 / 2 / 9 % (0,7 px) / 0 %; 16/17 verdeckt |
| T-F5d Steigungsknick | gruen: max 12,2 Grad (bauch) |
| T-F5e Stufe Wurzel/Ruecken (P2) | gruen: 1 px, keine Luecke (Pose 85 nur ausgegeben: 4 px, V5k dort 2 px) |
| T-F8 Krallen | gruen: 0,037 / 0,038 / 0,036 / 0,036 H |
| T-F4 Umriss-IoU >= V5r4 (gleich gemessen) | gruen: P1 0,7038 / P2 0,7489 gegen 0,7031 / 0,7444 |
| V4 T1-T4, TA, T5 (Pose 85) | gruen (T5 war in V5k rot; jetzt 0,038 / 0,064 / 0,104 / 0,138 H) |
| Kopf T-H1i/ii, T-H2, T-H4, T-H5, T-H6 | gruen |
| V4 TK Oberkontur | rot: RMS 2,55 px (>= 3,02), Oberkante 0,034 H (<= 0,03) |
| T-F7 Rot-Karte | rot, 11 Zeilen (Liste unten) |

T-F7 im Einzelnen (Foto / Modell, Prozent a* >= 10): Foto 1 Region Brust Mitte 47,8 / 12,3; Schulter Bild-links 0,5 / 9,1; Schulter
Bild-rechts 2,2 / 11,7 (Brustflaechen 8/d32, d2, v15 fallen in P1 in diese Box); Unterarm/Hand 9,6 / 2,6; Bauch 0,0 / 3,4 (gruen).
Foto 2 Region Buckel 41,5 / 10,9; Schulter/Oberarm Mitte 27,9 / 6,3; Teile: Schulter (W)-rechts 55,0 / 0,6; Oberarm (W)-rechts 51,0 / 16,8;
Ruecken/Buckel 13,3 / 0,0; Brust 3,5 / 14,5; Bauch 6,5 / 0,0; gruen: Becken, Oberarm (W)-links, Oberschenkel, Unterarme.
V5k zum Vergleich: Schulter Bild-links 49,0, Bild-rechts 30,3, Foto-2-Schulter 60,6, Oberarm 48,7.

Warum T-F7 nicht gruen wird (gemessen, nicht geloest):
1. Foto 1 und Foto 2 widersprechen sich an denselben Flaechen: 13/d30, v11, d33, d16, v6 sind in P1 im Foto grau (0-15 %) und in P2 im Foto
   rot (87-100 %); 8/v24 in P1 1 %, in P2 87 %. Eine Textur kann nicht beides. Die Pose P1 trifft laut Messblatt Kopf- und Schulterhoehe
   nicht (Offen 1), die Zuordnung dort ist unsicher.
2. Die Auftragsregel (Rot nur Brust und oberer Arm) graut die Foto-2-Schulter und den Buckel, die im Foto rot sind (55 % / 42 %).
3. Die Brust behaelt die Referenz-2-Palette; mehr Rot auf der Brust (Foto-1-Box 48 %) waere neue Farbe.
4. TK verlangt eine gewoelbte Oberkontur des waagerechten Teils; in V5k kam sie aus dem bauch-Wulst (Knick 31 Grad). Mit Steigungsknick
   <= 15 Grad erreichte keine der 9 Suchen mehr als RMS 2,6 px.

Gegenproben (SF_belege/v4/v5f/): V5r4 25 rot (u. a. T-K1b-d, T-F1, T-F5a/c/d), V5k 18 rot (u. a. T-K1c/d, T-F1, T-F5a/c/d). Der Riegel
unterscheidet V5r4, V5k und V5f.

### Leisten (SF_belege/v4/; roh, x2 bzw. x4)
- 20_foto2_vergleich.jpg: P2 - Foto 2 roh | Beschreibung2 | Stand V5k | neu | 50 % Foto/neu mit Nutzerpunkten, Foto-Schwanzmaske, Kette.
- 21_foto1_vergleich.jpg: P1 und 135 - Foto 1 roh | entblendet | Beschreibung1 | V5k | neu | 50 %.
- 22_kopf_nah_beide.jpg: Kopf x4, Foto 1 roh/entblendet | neu P1 | neu 135; Foto 2 roh | neu P2.
- 23_spiel.jpg: Spielbild RE15_FRAMEDUMP (Lauf sfv4/r/V5f) h135 / h85 gegen Foto.
- 24_rot_karte.jpg: Rot-Karte Foto 1 / P1 und Foto 2 / P2.
- v5f/krallen_P1_x8.png: Foto 1 | V5k | neu, Hand (W)-links x8.

Selbst angesehen: Der Schwanz ist in P2 ein dickes, dunkles, gruenlich-graues Stueck ohne Wulst und tritt ohne Stufe aus dem Ruecken. In P2
ist er weiter kuerzer als im Foto (Spitze y ~306 statt 343; die Kette ist unveraendert, damit T1-T4/TA gruen bleiben). Der Koerper ist grau,
rot sind Brust, der Oberarm (W)-rechts unterhalb der Kuppe und die Klauenhand. Im Spielbild h135 bleibt oben Bild-links ein roter Rest auf dem
Arm (andere Pose als P1). Die Seitenansicht ist jetzt deutlich weniger rot als Foto 2 (Modell 3,4 % gegen Foto 20 % der Figur). Die Krallen
sind laenger, aber dunkel. Kopf unveraendert: Auge im Schatten, blondes Haar, Mund hellgrau zu.

### Tests
Bau mit RE15_BUILD_DIR=.../r41_sichel/re15_port/build bash re15_port/tools/local_build.sh build: "LOCAL-BUILD-OK (build)". ctest -R
"unit_birkin_formen|unit_birkin_szenen|unit_schwanz_r41|integration_r41_sichel_form" --timeout 900: "100% tests passed, 0 tests failed out
of 4", Total Test time (real) = 64.62 sec (v5f/ctest_v5f.txt). Spiellauf lauf_v4.sh mit EM30_V5f: EXIT=0, 191 FRAMEDUMPs, "EM30 loaded: 23
meshes, 23 bones, 21 clips", 0 Zeilen mit error/fehler/assert (v5f/lauf_V5f_debug.log).

### Offen
1. T-F7 / Lesart Rot: Foto 2 zeigt Rot auf Schulter (W)-rechts und Buckel (55 % / 42 %), Foto 1 an derselben Stelle (Bild-links) Grau.
   Nach Auftrag ist die Schulter jetzt grau; dadurch faellt auch der Oberarm (W)-rechts in P2 von 49 auf 17 % (Foto 51 %). Der Nutzer sollte
   bestaetigen, ob "rechte Schulter" (W)-rechts (so gebaut) oder Bild-rechts in Foto 1 meint und ob die rote Schulter in der Seitenansicht
   bleiben soll. Mit nur 13/d30, d32, d33 grau waeren es Oberarm P2 37 % und Schulter Bild-links 33 %.
2. TK (V4, Pose 85, alte Leser-Masken) und "ohne Huckel" (T-F5d) schliessen sich bei den Foto-Breiten aus; entschieden fuer T-F5d.
3. Schwanzlaenge in P2: Spitze 37 px zu hoch (Messblatt 5). Die Kette ist auf Pose 85 und die Nutzerpunkte eingestellt (T1-T4/TA);
   laenger in P2 hiesse die Kette aendern.
4. Krallen hell: Foto-Keil L* 30, Modell-Krallen so dunkel wie die Hand; nach Auftrag Textur unveraendert.
5. Foto-1-Pose: Die Zuordnung je Koerperteil in P1 bleibt grob (Messblatt Offen 1); T-F7 Foto 1 nutzt deshalb nur die festen Regionen.
