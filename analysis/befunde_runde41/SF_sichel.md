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
