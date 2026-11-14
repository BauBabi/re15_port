# SF V8 Runde 1 - Sicht-Pruefung (Pruefer, 2026-11-09, nicht committet)

Geprueft: Zweig r41/sichel_v8, HEAD f6b43670a, Spiel-Asset EMD/EM30_HANGAR.EMD sha256 fa31156c3b17dd4fe0946f3fad334e597eac91bdc9aa5168280566fd4b33a8ff.
Referenz: V7 319bbd17 und die freigegebene Szene sfv4/r/V7_szene (RE15_FRAMEDUMP, F200-F1100).

**Ergebnis: NICHT BESTANDEN** - drei Punkte in der dritten Klasse (1b, 3, 5).

## Eigene Belege (roh, ohne Overlay)
Ordner C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pruef_sicht_r1/
- run/ : eigener Szenenlauf (lauf_v4.sh, BAUM = r41_sichel_v8, Spiel-Asset, FD 225-470, Exit 480): EXIT=0, 246 Bilder, debug.log
  "EM30: Ersatzmodell EMD/EM30_HANGAR.EMD (Raum 50A0)", "23 meshes, 23 bones, 21 clips".
- gdi.mp4 + gdi/ + gdi_uebersicht.jpg + gdi_ruecken.jpg: gdigrab des echten Fensters "RE1.5 Rebuilt — PC" im selben Lauf (19,9 s, 960x720).
  Damit ist der vom Bauer offen gelassene gdigrab-Lauf nachgeholt; er zeigt dieselben Fehler wie der Framedump.
- kacheln_szene.jpg (F250-F346, jedes 6.), ruecken_v7v8.jpg (F322/328/334/340 V7|V8), ruecken_F334_x3.png, ruecken_F340_x3.png,
  schwanz_vorn_v7v8.jpg (F262/268/274/310).
- pv_rein_x3.png: Darstellung_Vorlage | V7 | V8 in PV ohne Nutzerlinien (pv.py mit den Werkzeugen des Bauers, nur gerendert).
- p2_rein_x3.png: Beschreibung2.png (mit den Nutzerlinien) | V7 | V8 in P2 (Clip 12 Bild 48 Gier 861).
- diff05.png: Pixel, in denen V8 von V7 in Tafel 05 abweicht (gruen = geaendert, magenta = geaendert und rot).
- diff03.png: dasselbe fuer Tafel 03 (Foto 1 von vorn).

## Seite
(W)-rechts = die in PV und P2 zur Kamera gewandte Flanke; das Dossier misst das im Knochenraum 8 als z < 0 (Oberarm Knochen 13 z -1136).
In der Vorlage ist das die sichtbare Flanke mit dem haengenden Arm unter dem Buckel (Bild-Mitte rechts). Ich uebernehme das;
der Nutzer kann mit "rechte Seite" auch die Bild-rechte Haelfte (Arm/Brust/Kopf) meinen - in diesem Bild ist das dieselbe Flanke.

## Antworten

### (1a) Koerperbau in PV (Darstellung) - Abweichung klein
Tafel 01 und pv_rein_x3.png: Der Schwanz folgt jetzt der gruenen Linie (tail area 2) und endet an der roten Linie (tail area 1);
Bogen statt der V7-L-Form, oben breit, unten schmal (Riegel S1 3,0/7,0 px, S4 59 -> 6 px, monoton). Das ist der erkennbare Fortschritt.
Rest: die Naht Schwanz/Ruecken traegt noch einen kleinen Absatz (S2 rot: Sattel 3,7 px bei x 358, Huckel 5,0 px bei x 406), in
pv_rein_x3.png als leichter Knick in der Oberkante zwischen Schwanz und Buckel sichtbar. In der Vorlage ist die Oberlinie eine Kuppel.

### (1b) Koerperbau in P2 (Foto 2 / Beschreibung2) - Abweichung, die der Nutzer ablehnen wuerde
p2_rein_x3.png und Tafel 02: In V8 laeuft der Schwanz fast waagerecht nach links und endet in halber Hoehe mit einem nach oben
gebogenen Haken (Spitze etwa Bild (247,273) der 640x480-Kamera), waehrend Beschreibung2 "Schwanzspitze Ende" bei etwa (282,320)
markiert und das Foto den Schwanz nach unten zum Boden zeigt. V7 erreichte dort etwa y 313. Das ist ein Rueckschritt gegen V7 an
einer vom Nutzer selbst beschrifteten Stelle (Dossier V8 Offen 3 nennt es auch).

### (2) Ruecken - Abweichung klein
Riegel S3 rot (Huegel H 3,0 px bei x 423 statt 5-10 bei x 428-445; Sattel zum Kopf 2,3 statt <= 2). Im Bild (pv_rein_x3.png) ist der
Ruecken vom Schwanz bis zum Kopf weitgehend glatt, ohne die frueheren Huckel; der "little red hill" ist nicht als eigene kleine
Erhebung zu sehen, sondern der ganze obere Buckel ist mit roten Adern ueberzogen. Kein neuer Huckel gesehen.

### (3) Rechte Seite: Adern an Arm und Koerper - Abweichung, die der Nutzer ablehnen wuerde
- Ort: diff05.png zeigt, dass die Aenderung fast nur den oberen Buckel/Ruecken und das bereits rote Schulter-Augen-Feld (Mesh 13,
  magenta) trifft. Der helle Oberarm-Wulst unter dem Buckel und die Flanke unten sind unveraendert grau (kein gruener Pixel). "Am Arm"
  ist damit im Bild nicht erfuellt; der Riegel S7b (43,3 % auf X1) misst Flaechen auf der Zeile, nicht sichtbare Adern.
- Eindruck: Im Spiel von hinten (ruecken_F340_x3.png, ruecken_v7v8.jpg F328-F340, gdigrab gdi_ruecken.jpg) ist der Ruecken ein
  Flickenteppich: jede Flaeche zeigt ein anderes Stueck Aderbild, rote Streifen enden hart an den Dreieckskanten, gleiche "V"-Adern
  wiederholen sich nebeneinander, zwischen den Inseln laufen sichtbare Naehte. V7 hatte an derselben Stelle eine durchgehende Textur.
  Die Texel stammen nachweislich aus dem Original-EM30 (S7a), aber die Verteilung per 9 Teil-Inseln mit eigenem UV-Schub wirkt genau
  wie das, was der Nutzer abgelehnt hat ("wie drauf gemalt", "unruhig und so frei verteilt", "Hautprobleme"; Memory
  reai-v2-texturen-nicht-aus-video-eichen). In PV (Tafel 05) bilden die Adern ein rotes Netz mit harten dunklen Raendern, deutlich
  kraeftiger als die weich rosa-roten Toene der Vorlage; das Original-EM30 (Tafel 05 rechts) zeigt eine grosse zusammenhaengende
  Fleischflaeche und blasse Haut, kein Netz.
- Rot-Anteil Tafel 05 (R - max(G,B) > 50, R > 110): V7 1,27 %, V8 2,44 %, Original-EM30 2,02 %.

### (4) Gesicht und linke Hand - wie gewuenscht
S5 gruen (Mesh 9, Zeile 486, 6501 Texel bytegleich). diff03.png: Hand-Ausschnitt 0,0 % geaendert, Gesicht unveraendert; Spielbilder
F262/F274 V7 = V8 im Gesicht. Daumenkralle grau, Fingerkrallen rot wie V7 (Tafel 03).

### (5) Spielszene in Bewegung - Abweichung, die der Nutzer ablehnen wuerde
- Riss am Uebergang: ruecken_F334_x3.png / ruecken_F340_x3.png (Framedump) und gdi_ruecken.jpg (echtes Fenster): graue Dreiecke des
  Wurzelrohrs stechen zackig durch die Rueckenhaut ("Saegezaehne" an der Naht). V7 hatte dort keine Durchstoesse.
- Schwanzlage: F322 (gdigrab Bild 27) liegt der Schwanz wie ein Balken quer ueber dem Ruecken und ragt mit der Spitze waagerecht nach
  rechts in die Luft; F328-F340 steht er senkrecht wie ein Horn/eine Flosse nach oben. Kein Bogen zum Boden.
- Boden: kein Bodendurchstoss gesehen (S6: tiefstes Gelenk y -66); F268 liegt der Schwanz flach auf Kniehoehe quer (wie V7).
- Ablauf: Lauf EXIT 0, Szene laeuft (Spinne, Spider-Man, Kameraschnitte wie V7); state.log-Gleichheit laut Bauer.

## Tafeln des Bauers
Alle sieben Tafeln angesehen. 01-05 sind Software-Raster (f_rast), 06/07 Framedump. Die Tafeln zeigen die Durchstoesse und den
Flickenteppich nicht, weil keine Tafel den Ruecken von hinten in der Szene vergroessert (07 nur in Kleinformat, F323-F337).

## Was fuer Runde 2 noetig waere (Hinweis, keine Vorgabe)
1. Naht: Wurzelrohr vollstaendig unter der Rueckenhaut halten oder die Rueckenecken an den Schwanz anschliessen (kein Durchstoss in
   den Rueckenposen F322-F346).
2. Schwanzbewegung: die V7-Clip-Deltas erzeugen auf der neuen Kette Horn- und Balkenlagen; die Bewegung braucht eine eigene
   Begrenzung, sodass die Spitze nach unten/hinten zeigt (P2, F322-F346).
3. Adern: statt 9 Teil-Inseln mit eigenem Schub zusammenhaengende Flaechen mit durchgehender Original-UV (ein Schub je Material/Flaeche-
   Verbund), Rot auch auf den sichtbaren Oberarm-Wulst; Pruefung von hinten im Spiel, nicht nur in PV.
