# SF V8 Runde 2 - Sicht-Pruefung (Pruefer, 2026-11-09, nicht committet)

Geprueft: Zweig r41/sichel_v8, HEAD 3db35d865 (Code 6dcbb3604), Spiel-Asset EMD/EM30_HANGAR.EMD
sha256 d0ef852c9070c7a970b53301c961c059f02f166befb525792ecbd19705e2911a (selbst nachgerechnet).
exe re15_port/build/platform/pc/re15_pc.exe 02:09:36 ist juenger als schwanz_r41.c (02:09:34) und dessen .obj (02:09:35); keine
ungesicherten Aenderungen unter re15_port. Referenz: V7 319bbd17 und die freigegebene Szene sfv4/r/V7_szene.
Pflichtlektuere gelesen: Memory reai-v2-texturen-nicht-aus-video-eichen (samt Nachtrag 2026-11-08).

**Ergebnis: NICHT BESTANDEN** - zwei Punkte in der dritten Klasse: (2) Ruecken zum Kopf von vorn (neuer kantiger Kamm hinter dem
Kopf) und (3) rechte Seite weiter ueberwiegend grau. Dazu (5) mit einem Restbefund an der Naht von hinten.

## Eigene Belege (roh)
Ordner C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pruef_sicht_r2/
- run/ : eigener Szenenlauf (lauf.sh -> lauf_v4.sh, BAUM r41_sichel_v8, Spiel-Asset, FD 200-480, Exit 490): EXIT=0, 281 Bilder,
  debug.log "EM30: Ersatzmodell EMD/EM30_HANGAR.EMD (Raum 50A0)", "23 meshes, 23 bones, 21 clips -> slot 12".
- kacheln_szene.png (F250-F370 jedes 5.), vorn_v7_v8r2.png (F262-F278), hinten_v7_v8r2.png (F318-F340), nah_v7_v8r2.png (F380-F446),
  naht_F334_F340_x3.png, naht_F334_v7_v8r1_v8r2.png, vorn_F360_v7_v8r2.png, **kamm_hinter_kopf_x3.png** (F350/360/370, V7|V8r1|V8r2),
  **foto1_buckel_x2.png** (Ausschnitt aus Tafel 03, V7|V8), pv_rein_x3.png und p2_rein_x3.png (Vorlage|V7|V8r2 ohne Nutzerlinien,
  pv.py/p2.py aus Runde 1 mit den Werkzeugen des Bauers), schwanz_unten_vorlage_v8r2_x6.png, ruecken_tafel01_x2.png,
  video_kacheln.png (spiderman_szene_v8.mp4 jedes 10. Bild), labor_kacheln.png (labor_v8r2 F400/600/800/1000).
- gdigrab (gdi.mp4): **misslungen** - ffmpeg griff per Fenstertitel "RE1.5 Rebuilt - PC" ein fremdes, schwarzes 320x240-Fenster
  (gleichzeitig liefen re15_gegenpr_39742/39743.exe eines anderen Agenten unter scratchpad/labor_check, gleicher Titel; gdi_uebersicht.png
  ganz schwarz). Fremde Prozesse nicht angefasst. Alle Spielbilder dieser Pruefung sind daher RE15_FRAMEDUMP wie der V7-Referenzlauf.

## Seite
(W)-rechts = die in PV (Darstellung_Vorlage) und P2 (Foto 2) zur Kamera gewandte Flanke: das Wesen schaut nach Bild-rechts, der Arm unter
dem Buckel haengt auf der Kameraseite (Messblatt: Oberarm Knochen 13 = (W)-rechts). In beiden Vorlagen ist das dieselbe sichtbare Flanke,
auch die "Bild-rechte" Haelfte (Buckel, Brust, Kopf) gehoert dazu. Ich folge dem.

## Antworten

### (1) Koerperbau Schwanz - Abweichung klein
- PV (Tafel 01, pv_rein_x3.png): der Schwanz folgt jetzt der gruenen und roten Nutzerlinie, steigt ohne Stufe aus der Buckel-Oberkante
  und faellt im Bogen bis Bodennaehe (Spitze etwa (283,335)); oben breit, unten schmal. Riegel S1 1,8/5,0 px, S2 gruen. In PV sehe ich
  keinen Huegel am Uebergang mehr. Das ist der deutliche Fortschritt gegen V7 (L-Form) und V8 r1 (Absatz).
- P2 (Tafel 02, p2_rein_x3.png): der Haken aus Runde 1 ist weg, der Schwanz faellt als Bogen nach unten links (Spitze etwa (271,339)).
- Rest: der untere Teil (tail area 1) ist deutlich duenner als die dunkle Masse der Vorlage. Zeilen der Vorlage (Darstellung_Vorlage.png,
  Leuchtdichte): bei y 270 dunkle Saeule etwa x 282-314 (gut 30 px), V8r2 dort etwa 18 px, bei y 300 nur etwa 5 px
  (schwanz_unten_vorlage_v8r2_x6.png). In der Vorlage ist dieser Teil auch dunkel-rotbraun, V8r2 oliv-grau (der Nutzer nannte den Schwanz
  frueher selbst "graeulich"/"dunkel gruenlich", daher nur Hinweis). Der Nutzer koennte die Peitschenspitze als zu duenn sehen.

### (2) Ruecken / Linie zum Kopf - Abweichung, die der Nutzer ablehnen wuerde
- PV: Ruecken vom Schwanz bis zum Kopf glatt, S3 gruen. Der "little red hill" ist als Form da (S3b 6,8 px bei x 437), aber **grau, nicht
  rot**: mittleres R-max(G,B) im Feld x 420-445 / y 192-210: Vorlage 25,8, V7 8,3, V8r2 11,0 (nur Farbrichtung, keine Eichung).
- **Neu und gegen V7 schlechter: von vorn steht hinter dem Kopf ein kantiger Kamm.** kamm_hinter_kopf_x3.png (Spielszene F350/F360/F370,
  die lange Phase, in der Birkin frontal zur Kamera steht) und foto1_buckel_x2.png (Foto-1-Pose P1, Tafel 03): Die Buckeloberseite hinter
  dem Kopf ist jetzt eine grosse flache Platte mit geraden Polygonkanten, einer spitzen Ecke ueber dem Kopf und einer dunklen senkrechten
  Kerbe links davon (etwa Bild (513,230-250) in F370, 960x720); die Textur darauf ist sichtbar langgezogen (gestreckte Streifen).
  V7 hatte dort eine runde, organische Kuppe; V8 r1 hatte die Platte noch nicht. Ursache nach Dossier V8 r2 Punkt 5: ruecken_ecken.py
  verschiebt die Silhouetten-Ecken 46/51/30/53/50/49/47/48 auf die PV-Kuppel - in PV glatt, aus anderer Richtung ein Grat.
  Genau das hat der Nutzer abgelehnt ("smooth straight to head without further hills", "davor wieder so einen Hueckel"); in Foto 1
  ist die Oberkontur rund. Kein Riegel misst das (S3 nur in PV), Tafel 03 zeigt es, das Dossier nennt es nicht.

### (3) Rechte Seite: mehr Rottoene/Adern an Arm und Koerper - Abweichung, die der Nutzer ablehnen wuerde
- Herkunft sauber: ganze V7-UV-Inseln wechseln die CLUT-Zeile (X1-X3 aus Original-EM30-Eintraegen), UV unveraendert, keine gemalten
  Inseln (S7a ok). Das entspricht der Regel aus dem Memory.
- Wirkung im Bild zu klein: Tafel 05/pv_rein_x3.png - Buckel, Schulter und Flanke bleiben wie V7 grau-braun; neu sind nur ein paar rote
  Striche am Schwanzansatz und **ein einzelnes kraeftig rotes, gerades Band ueber die ganze Unterarmlaenge** (Mesh 14 auf X3), das in P2
  (Tafel 02) wie ein aufgelegter Streifen wirkt; das Original-EM30 (Tafel 05 rechts) zeigt am Arm blassrosa Haut mit violetten Adern,
  kein rotes Band. Roetlich-Anteil der PV-Kachel (R-max(G,B)>15): Vorlage 7,6 %, V7 3,3 %, V8r2 5,1 %. Riegel S7c des Bauers selbst
  ROT (5,3 % statt 25-80 %), Dossier Offen 1 legt die Entscheidung zurueck.
- Von hinten (naht_F334_v7_v8r1_v8r2.png, Tafel 08 F328-F340): am Schwanzansatz rote Pixelsprenkel auf grauer Haut (Insel d0 auf X1),
  unruhiger als V7; ruhiger als der Flickenteppich von V8 r1, aber fleckig.
- "Nicht nur grau ... an verschiedenen Stellen, am Arm und Koerper" ist damit nicht erfuellt: Koerper weiter grau, am Arm nur ein Band.

### (4) Gesicht und linke Hand - wie gewuenscht
S5 gruen (Mesh 9, Zeile 486, 6501 Texel bytegleich). vorn_F360_v7_v8r2.png und Tafel 03: Gesicht unveraendert, linke Hand mit grauer
Daumenkralle und roten Fingerkrallen wie V7. Einschraenkung nur durch die Umgebung: der neue Kamm aus (2) steht direkt hinter dem Kopf.

### (5) Spielszene in Bewegung - Abweichung klein (mit einem Restbefund)
- Ablauf: eigener Lauf EXIT 0, Szene mit Spinne, Spider-Man, Kameraschnitten wie V7 (kacheln_szene.png, video_kacheln.png, Tafel 06/07).
- Bewegung: der Schwanz schwingt als glatter, sich verjuengender Bogen (vorn_v7_v8r2.png F262-F278, nah_v7_v8r2.png F380-F392); keine
  Horn- oder Balkenlagen mehr wie in V8 r1 (hinten_v7_v8r2.png F322-F340: Bogen ueber den Ruecken nach links unten). Boden: kein
  Durchstossen gesehen (S6 0 von 912, Szene tiefstes Gelenk y -66).
- Restbefund Naht von hinten (naht_F334_F340_x3.png): der Schwanz liegt wie ein Schlauch auf dem oberen Ruecken; an der Wurzel sticht
  weiterhin ein grauer Keil mit harter Kante in die Rueckenhaut (F328-F346), daneben ein hell gestreifter Rueckenstreifen. Die
  Saegezaehne aus Runde 1 sind zu einem Keil geworden, nicht verschwunden; das Dossier ("kein Durchstossen gesehen") trifft das nicht.
  "Tail directly smooth into the back" gilt damit nur in PV.
- Labor 50E0: labor_v8r2 F400-F1000 zeigen Leon im Raum, Birkin ist in keinem dieser Bilder sichtbar - nur "Form geladen" (debug.log
  "Ersatzmodell ... (Raum 50E0)", 23/23/21) ist belegt, kein Sichtbeleg.

## Tafeln des Bauers
Alle angesehen: 01-08 und runde2/szene_v7_v8r1_v8r2.jpg, ruecken_v7_v8r1_v8r2.jpg, adern_varianten_V7_V8r1_A_C.png, adern_V7_V8r2.png,
posen_V7_V8r2.png; dazu riegel_v8.txt (7/8, rot S7), tests_v8.txt (Gruppe 8/8, Unit 1013/1013), Gegenproben (V7 und V8 r1 rot wie
erwartet), szene_mass.txt. Der Kamm aus (2) ist in Tafel 03 (V8, ueber dem Kopf) sichtbar, aber nicht benannt; keine Tafel zeigt den Kopf
von vorn in der Szene vergroessert.

## Hinweise fuer die naechste Runde (keine Vorgabe)
1. Rueckenecken nicht nur in PV setzen: Pruefung von vorn (P1/Foto 1, Szene F345-F372) und von hinten mit in die Kosten; die
   Kuppe hinter dem Kopf muss rund bleiben wie V7.
2. Wurzel: Rohr ganz unter der Rueckenhaut enden lassen oder die Rueckenkanten an das Rohr anschliessen; Pruefbild F328-F346 von hinten.
3. Adern: Entscheidung Offen 1 des Dossiers (volle Original-Zeile fuer die (W)-rechten Inseln) ist die Stelle, an der mehr Rot auf Koerper
   und Arm kommt, ohne gemalte Inseln; das harte Unterarm-Band gegen das Original-EM30 pruefen.
