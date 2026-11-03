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
