# SF Sicht-Pruefung V8 Runde 6 (Sichel-Form, EMD/EM30_HANGAR.EMD)

Stand: Zweig r41/sichel_v8, HEAD 3d2d230f5, Asset sha256
e9e71bca28f4f8596b888436f4b02406b359ff6a6067aaf21fb344e142c90e83 (= Pin, selbst nachgerechnet).
Pruefer: Sicht-Pruefer (Auge des Nutzers, Ziel: widerlegen). Datum 2026-11-11. Nicht committet.

## Ergebnis

**NICHT BESTANDEN.** Eine Bewertung der dritten Klasse: (1c) Luecke/Kerbe zwischen Ruecken und Kopf (P5).
Alle anderen Punkte sind "wie gewuenscht" oder "Abweichung klein".

Kurz in Alltagssprache: Der Kragen der Runde 5 ist weg, die breite Hand ist da und sitzt richtig herum,
der Schwanz ist unten rot und oben grau. Aber die Kerbe neben dem Kopf, die der Nutzer in Runde 4
beanstandet hat ("es gibt immer noch eine Luecke zwischen Ruecken und Kopf"), ist wieder da - in der
Griff-Phase rund 50 Bilder am Stueck und auch in zwei Foto-Posen.

## Methode (eigene Bilder, eigener Lauf)

Ordner P = C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr6s

- Eigener Szenenlauf Hangar 50A0 mit eigener exe-Kopie (Muster lauf_v4.sh, BAUM = Worktree r41_sichel_v8):
  901 Framedumps F200-F1100 in P/szene_fd/fd/, Lauf EXIT=0. debug.log: "EM30: Ersatzmodell EMD/EM30_HANGAR.EMD
  (Raum 50A0)", "EM30 loaded: 23 meshes, 23 bones, 21 clips -> slot 12". state.log sha256
  e48e1cad143cc5776401bc6a415652d694aaba972c1c23235cd18a5cc1ece349 = gleich der V7-Referenz (Spiellogik unveraendert).
- Echtes Fenster per gdigrab (ffmpeg, 60 fps, roh, 960x720): P/gdi/gdi.mp4 (2366 Bilder) und zweite Aufnahme
  P/gdi2/gdi.mp4 (2192 Bilder). Zuordnung Aufnahmebild -> Spielbild F ueber die kleinste mittlere Grauwert-Differenz
  gegen die eigenen Framedumps (P/zuord.py, P/zuord.txt): 901 Spielbilder zugeordnet, groesste Abweichung 3,20.
  In Aufnahme 1 fehlten die Spielbilder F235, F356, F369, F391, F823 (Abweichung > 1,0 = uebersprungen). F235, F356,
  F369, F391 stammen deshalb aus Aufnahme 2 (Abweichung 0,37-0,42). F823 ist nicht ersetzt und wird nicht verwendet.
- Alle "NEU"-Kacheln unten sind Bilder des echten Fensters (P/g/F%04d.png), nicht Framedump, nicht AUTOSHOT.
- Vergleichsquellen: V7 = .../scratchpad/sfv4/r/V7_szene/fd; V8 r4 = .../sfv8/pr4s/szene_fd/fd (F250-380) bzw.
  Bauer-Lauf .../sfv8/szene_v8r4/fd; V8 r5d = .../sfv8/pr5s/szene_fd/fd (F225-450) bzw. .../sfv8/szene_v8r5d/fd.
- Einschraenkungen, offen gesagt:
  - Ich kann ein Video nicht in Echtzeit ansehen. "In Bewegung" ist aus aufeinanderfolgenden Einzelbildern und
    Bildfolgen (Schritt 1-6 Bilder) beurteilt. Was ueber viele Bilder am Stueck steht, ist bei 30 Bildern/s sichtbar.
  - Die beiden Bauer-Videos (spiderman_szene_v8_r6.mp4, r6d/gdi/gdi_szene_v8r6d.mp4) habe ich nicht abgespielt;
    meine eigene Aufnahme derselben Szene ersetzt sie. Die Bauer-Fensterbilder runde6d/gdi_fenster_F*.png habe ich
    nur ueber Tafel 11 gesehen, nicht einzeln.
  - re15_pc.exe im Worktree: mtime 2026-11-09 03:31, letzter Engine-Commit 61cc6af85 2026-11-09 03:32:31
    (eine Minute spaeter). Das Modell wird zur Laufzeit geladen (debug.log oben), die Figur ist also der Stand
    e9e71bca; der Vorbehalt betrifft nur Engine-Code.

Gesehen: Bauer-Tafeln 01-18 samt 14b, 16a, 16b, 17, 18; c3_keil.png; Darstellung.png, Beschreibung1/2.png,
birkin19-24.png, hand/drei_posen.png; runde6d/riegel_v8r6d.txt; Dossier SF_sichel.md Abschnitt "V8 Runde 6 - Abgabe
am Endstand" (Zeile 4026 ff., dort steht selbst: "P5 ... NICHT erfuellt. Der Kragen ist weg, die Luecke ist wieder da").

## Eigene Kacheln (alle unter P/k/)

| Kachel | Inhalt |
|---|---|
| paar_ganz.png | fuenf Paare ganze Figur: birkin20/F368, birkin22/F369, birkin23/F371, birkin21/F373, birkin24/F375 |
| hand_a.png, hand_b.png, hand_c.png | Hand/Unterarm im gleichen Ausschnitt x3: F368:20, F371:23 / F373:21, F375:24 / F369:22 |
| luecke_a.png | F278, F310, F311 je V7, V8 r4, V8 r5d, NEU |
| kragen_a.png, kopf_zoom.png | Kopf von vorn F346-F378, NEU gegen V7 / r4 / r5d |
| f312_314.png | F312-F314 V7 / r4 / NEU |
| schwanz_vorn.png, tonne.png | Schwanz von vorn F264/F276; Wurf-Phase F318/F321/F324 |
| gang_seite.png | Gang von der Seite F528/F564/F600: V7, r4 (Bauer-Lauf), NEU |
| bogen_wurf.png, bogen_gang.png | Uebersicht F228-F330 (Schritt 6), F336-F1080 (Schritt 24) |
| dreh_folge.png, dreh_v7.png | Bildfolge F332-F384 (Schritt 4); V7 gegen NEU F340/F352/F364/F376 |

## Befunde

### (1) Koerperbau wie Vorlage - wie gewuenscht
gang_seite.png F600, dreh_folge.png F332-F340: Der Schwanz ist breit, kommt ohne Stufe aus dem Ruecken und laeuft
in einem weiten Bogen bis zum Boden. Zweibeiner mit grossem Sichel-Arm rechts, Hose, Bauchmuskeln wie Vorlage.

### (1c) P5 Luecke/Kerbe Ruecken-Kopf - Abweichung, die der Nutzer ablehnen wuerde
luecke_a.png (echtes Fenster): In F278, F310 und F311 steht bei NEU direkt neben dem Kopf eine helle, spitze Zacke,
dazwischen eine dunkle V-Kerbe zwischen Kopf und Ruecken/Schulter. Das Bild gleicht dem Stand V8 r4 (den der Nutzer
beanstandet hat), V7 ist an der Stelle glatt, r5d hatte stattdessen die Kapuze. Der Riegel des Bauers sagt dasselbe
(riegel_v8r6d.txt S16 ROT): 62 von 177 Ansichten ueber dem Soll (Soll <= 21 px / <= 7 tief / <= 3,2), darunter die
Foto-Posen PV (26 px, 10 tief, 18,4) und P2 (43, 17, 3,6) sowie F278 (189, 141, 9,8), F310 (184, 131, 10,0),
F311 (226, 165, 11,7), F312 (146, 86, 6,4). Das ist kein Einzelbild: die Kerbe steht in meiner Aufnahme von der
Griff-Phase F262 bis F313 durchgehend (rund 1,7 s bei 30 Bildern/s) und in zwei Foto-Posen. Nach Entscheid E3 ist
das rot. In der Vorlage (birkin19-24, Darstellung.png) gibt es diese Kerbe nicht.

### (1d) Kragen - Abweichung klein
kragen_a.png, kopf_zoom.png, Bauer-Tafel 17, c3_keil.png: Die Kapuze/der Bogen der Runde 5 ueber dem Kopf ist weg.
Von vorn (F344-F378) sitzt der Kopf wieder im Rumpf, etwa auf dem Stand r4. Nicht so ruhig wie V7: direkt ueber dem
Kopf eine kantige, zeltartige Flaeche, in F346 ein kleiner dunkelroter Schlitz links oben am Kopf, in F354/F362 eine
kleine Spitze auf dem Grat, auf den Schultern gezogene rosa-weisse Streifen, rechts vom Gesicht eine blasse Flaeche
(die hat r4 auch). Der Riegel S18 des Bauers ist dazu ROT (helle Pixel F344-F378 81-112 gegen V7 69-78, Soll <= 87,
in 32 von 35 Bildern). Mit dem Auge ist das ein unruhiger Bereich, kein Rahmen und kein Keil wie in r5d.
F313/F314 (f312_314.png): nichts ueber das Gesicht gezeichnet. F312: der Bauer misst 18 px Gesicht vom Oberarm
verdeckt - im Standbild kaum zu sehen, Einzelbild. Das neue Nackenstueck faellt von der Seite/hinten nicht als Klotz
oder Fremdfarbe auf; von vorn ist es die oben genannte kantige Flaeche.

### (1b) P4 Schwanz-Unterseite - Abweichung klein (mit Risiko)
Die grauen Ringe/Reifen und Schlitze der Runde 5 sind weg (schwanz_vorn.png, tonne.png, bogen_wurf.png).
Oberseite durchweg grau, Unterseite rot bis zur Spitze (E1), Uebergang von der Seite ruhig (gang_seite.png: schmales
dunkelrotes Band an der Unterkante). Zwei Dinge wuerde ich dem Nutzer nicht als "fleischig" verkaufen:
- Die Unterseite ist ein einheitliches, glattes, sattes Rot, fast ohne Fleischstruktur. Von vorn in der Griff-Phase
  (F264, F276, auch F306/F312) wirkt der Schwanz als grosse leuchtend rote Klinge. Der Bauer-Riegel zeigt das als
  flaches Helligkeitsprofil (S15: Modell 26,1/21,6/27,2/28,3/26,4/26,1 gegen Vorlage 11,4/16,7/24,1/30,5/34,8/36,8).
- In F315-F324 (tonne.png, Bauer-Tafel 14b) erscheint die Schwanzwurzel von hinten rund zehn Bilder lang als glatter
  roter Kasten mit harten Kanten auf dem Ruecken (r4: grau, r5d: rot mit grauen Reifen).
Kein aufgemalter Fleck, keine Ringe. Klein, aber der wahrscheinlichste naechste Einwand zum Schwanz.

### (2) Ruecken mit kleinem roten Huegel - Abweichung klein
Roter Huegel vorhanden (Tafeln 03/08, dreh_v7.png), Linie zum Kopf von der Seite ohne Kappe. Von vorn kein Kamm und
keine Kappe mehr, aber eine kleine Spitze/Facette auf dem Grat (F354/F362, kopf_zoom.png). Bauer-Riegel S3 ROT in (e):
K 3,4 gegen Soll <= 3.

### (3) Rechte Seite mit Original-Adern, von vorn keine Platte - Abweichung klein
Adern der rechten Seite sind da (dreh_v7.png, dreh_folge.png: rosa Haut mit roten Adern statt des grauen V7-Rumpfs;
Bauer-Tafeln 05/10). Von vorn keine graue Platte und keine harte graue Kante. Neben dem Gesicht bleibt rechts eine
blasse Flaeche wie in r4; ein weisser Keil wie in r5d ist es nicht.

### (4) Gesicht wie V7 - wie gewuenscht
dreh_v7.png F352/F364/F376, kopf_zoom.png: Gesicht, Zaehne, Kopfform wie V7. F312 siehe (1d), Einzelbild.

### (4b) P6 linke Hand/Unterarm an den fuenf Paaren - Abweichung klein
hand_a.png, hand_b.png, hand_c.png, paar_ganz.png (echtes Fenster, gleicher Ausschnitt/Massstab):
- Erfuellt: Die Hand ist breit, der Unterarm breit mit Manschette, die Groesse wirkt natuerlich und nicht mehr
  verkuemmert; sie haengt auf Oberschenkelhoehe wie in der Vorlage. Bauer-Zahlen passen dazu (S17: Handgelenk
  21-22 px gegen Vorlage 20-25,5; Breite/Laenge 0,68-0,72 gegen 0,655-0,783).
- Krallen: vier, Daumenkralle grau, drei Fingerkrallen rot, kein Kreuzen, Abstand gleichmaessig, Richtung nach unten.
- Was noch anders ist als in der Vorlage: (a) Farbe - bei uns ist die Hand ab dem Handgelenk dunkel rot-violett
  gesprenkelt, in der Vorlage ruhig grau mit Rot nur an den Fingern (keine Eichung am Bild, E4; nur als Eindruck);
  (b) Daumen - bei uns eine duenne, blassgraue Nadel-Kralle, in der Vorlage ein kurzer dicker Daumen mit roter Spitze;
  (c) Fingerende - bei uns drei getrennte spitze Krallen, in der Vorlage eher ein runder Faeustling mit kurzen
  Spitzen; (d) die Einschnuerung am Handgelenk ist schwaecher; (e) Hand mit Krallen etwas laenger.
- In Bewegung (bogen_wurf.png, dreh_folge.png, gang_seite.png): keine Hand im Bein oder Rumpf gesehen, kein
  klaffendes Handgelenk. Die Hand ist dunkel, aber nicht flach.
Wuerde der Nutzer noch "irgendwas passt nicht" sagen? Am ehesten zur duennen Daumen-Nadel und zur dunkel
gesprenkelten Farbe (hand_c.png, hand_a.png). Riegel S17 des Bauers ist dazu ROT in (a) und (d) (Spitzen, Daumen).

### (4c) P7 Hand richtig herum, Arm haengend - wie gewuenscht
hand_a/b/c.png, paar_ganz.png: Handruecken zur Kamera, Daumen auf der Koerperseite wie in birkin22, Krallenfaecher
nach unten, Arm haengt. Ellbogen, Handgelenk und Krallenspitze liegen auf Vorlagen-Hoehe (Bauer S20 gruen:
Handgelenk 0,475-0,487 gegen 0,462-0,503; Krallenspitze 0,274-0,285 gegen 0,256-0,318). "Verkehrt herum" sehe ich
nicht mehr.

### (4d) Schadet der neue Arm der gelobten Gestalt - Abweichung klein
Kein Durchstossen in Oberschenkel, Rumpf oder Boden gesehen (bogen_wurf.png, bogen_gang.png, dreh_folge.png).
Was sich gegen V7 und den Stand vor P7 aendert:
- F311-F313: die dunkle Hand mit gespreizten Krallen ragt links vor die Brust, groesser als in V7 (f312_314.png).
- F315-F324: Unterarm und Hand stehen steif zur Seite ab (tonne.png); in V7/r4/r5d lag der Arm an.
- Gang von der Seite F528-F600: die linke Hand baumelt sichtbar hinter der Huefte (gang_seite.png), in V7/r4 nicht
  zu sehen.
- Von vorn F344-F378 (dreh_v7.png): die Hand haengt tiefer und ist groesser als in V7, passt zur Vorlage.
Puppenhaft wirkt nur das Abstehen in F315-F324; das sind rund zehn Bilder.

### (5) Szene in Bewegung - Abweichung klein
Keine Risse, keine neue Naht, kein Durchstossen in den Bildfolgen. Die Kerbe neben dem Kopf aus (1c) steht aber
die ganze Griff-Phase ueber; sie ist der sichtbare Fehler in der Bewegung und unter (1c) bewertet.

### (6) Insgesamt
Gegen V7 klarer Fortschritt: Schwanz, Adern der rechten Seite, breite Hand. Gegen die Vorschau der Runde 4:
Schwanz-Unterseite (P4) und Hand (P6/P7) sind besser, die Luecke Ruecken/Kopf (P5) ist NICHT besser - sie ist wieder
auf dem Stand der Runde 4, also genau der Punkt, den der Nutzer beanstandet hat. Der Rest der Gestalt ist nicht
schlechter als die Runde-4-Vorschau; neu hinzugekommen sind nur der rote Kasten F315-F324 und der abstehende Arm.

## Was der naechste Bauer wissen muss
1. P5 ist der einzige Ablehnungsgrund: Kerbe neben dem Kopf in F262-F313 und in den Foto-Posen PV/P2 schliessen,
   ohne den Kragen der Runde 5 zurueckzuholen (Vergleich luecke_a.png gegen kragen_a.png).
2. Kleinere Punkte in der Reihenfolge, in der der Nutzer sie am ehesten anspricht: einheitlich rotes Schwanz-Unterteil
   und roter Kasten F315-F324; Daumen-Nadel und Farbe der Hand; kantige Flaeche ueber dem Kopf von vorn; Arm steht in
   F315-F324 ab.
