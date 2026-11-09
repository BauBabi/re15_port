# SF V8 Runde 3: Prüfung durch den Sicht-Prüfer (2026-11-09)

Geprüfter Stand: Zweig r41/sichel_v8, HEAD 100900c97, Spiel-Asset EM30_HANGAR.EMD sha256 66806bc4ae2537809ce2ebd827a661974ca560d024fccaca4ee79c559134fab3
(nachgerechnet, = Pin). Vergleichsstände: V7 319bbd17 (sfv8/emd/EM30_V7.EMD), V8 r2 d0ef852c (sfv8/r3/EM30_V8r2.EMD), Original-EM30
(CDEMD0.EMS Typ 0x30, birkin_formen.lade_em30).

**Ergebnis: NICHT BESTANDEN.** Drei Fragen bekommen die dritte Klasse: (2) Rücken und Kopf, (3) rechte Seite, (5) Wurzel von hinten.
Daraus folgt auch die dritte Klasse für (6). Erreicht und gehalten sind der Schwanzbogen in PV/P2, die Breite unten, das Gesicht, die linke Hand
und der Szenenablauf.

## Eigene Läufe (roh, kein AUTOSHOT, kein SOFTWARE_RENDER)
Alle Pfade unter C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr3s/

- Lauf 1: lauf_v4.sh mit eigener exe-Kopie (BAUM = r41_sichel_v8, exe-Bau 03:31 mit dem Code-Stand von 61cc6af85), FD 225-450. Ergebnis: EXIT 0,
  226 Bilder, state.log mit 1131 Zeilen **bytegleich** zu sfv4/r/V7_szene/state.log und zu sfv8/szene_v8r3/state.log, außerdem
  "[hangar] ROOM50A0 Szene Ende: Cut 4". Die gdigrab-Aufnahme dieses Laufs (gdi_pruef.mp4) ist **ungültig**: Die Fenster zweier fremder
  Prozesse (re15_pruef_46708/46710, nicht meine PIDs, nicht beendet) lagen über dem Ausschnitt.
- Lauf 2: gdi2.sh. Das ist dieselbe Aufnahme, nur wird das **eigene** Fenster (PID 80108) per SetWindowPos ganz nach oben gelegt.
  Ergebnis: gdi_pruef2.mp4 (960 x 720), state.log wieder bytegleich. Kacheln: kachel_gdi2.png (t 9,0-15,75 s) und gdi_nah.png
  (hinten t 12,50/12,75, vorn t 13,25, Seite t 10,25/11,75).
- Framedump-Kacheln V7 | V8 r3 bei gleicher Bildnummer: k_hinten.png (F320/326/332/338), k_vorn.png (F344/350/358/366),
  k_kopf.png (F346/354/362/370, x3), k_seite.png (F262/276/290/304), k_dreh.png (F308-F316).
- Offline mit den Werkzeugen des Bauers (f_rast, gleiche Spielkamera): Original-EM30 | V8 r3 in P2, P1, F354, F328, F332
  (P2_orig_v8.png usw.) sowie p2_unterarm.png (Knochen 14 in P2 für V7 | V8 r2 | V8 r3 mit Umriss). Messskripte: orig_vgl.py, taf.py
  und die Inline-Messungen unten.

## Seite
"Die Rechte Seite" ist die (W)-rechte Seite. An der Vorlage geprüft: In PV/Foto 2 ist das die zur Kamera gewandte Flanke. Dort liegt der
große Oberarm unter dem Buckel (Knochen 13, in PV 3671 px sichtbar), der (W)-linke Arm (Knochen 10) ist dagegen fast verdeckt (234 px).
Von vorn (P1, Szene F344-F370) liegt die (W)-rechte Seite im Bild links. Das stimmt mit dem Dossier überein.

## Antworten

### (1) Körperbau wie die Vorlage: Abweichung klein
- In PV (Tafel 01) folgt die Außenkontur des Schwanzes der grünen Nutzerlinie: Er kommt breit aus dem oberen Rücken, bildet einen Bogen
  und endet unten an der roten Linie (Riegel S1 3,0/7,0 px, S2 grün). Unten ist er nicht mehr dünn: s 140-160 sind 31,6/31,0 px
  (Vorlage etwa 30-33 px, V8 r2 20,4 px). Das sehe ich auch in Tafel 02 (P2) und in der Szene: k_seite F262/F276 und gdigrab t 10,25.
- Rest 1: Der Rücken ist jetzt rosa, der Schwanz bleibt olivgrau. An der Wurzel entsteht dadurch eine harte Farbkante mit einem dunkelroten
  Streifen (Tafel 01 rechts, etwa x 1320-1345). In der Vorlage geht der mauvegraue Schwanz farblich weicher in den Rücken über
  (Nutzer: "geschlossen ein zusammenhängendes Stück").
- Rest 2: Von schräg hinten (k_dreh F316, k_hinten F320) liegt der Bogen als dicker Wulst oben auf dem Rücken auf.

### (2) Rücken mit kleinem roten Hügel, glatte Linie zum Kopf, von vorn kein Kamm: Abweichung, die der Nutzer ablehnen würde
- Die Kontur ist besser als in V8 r2: In P1 liegt sie höchstens 2 px über V7, in der Szene 0 px (Riegel S9). Die graue Platte, die über die
  Silhouette hinausragte, ist weg.
- **Dafür liegt jetzt eine kantige ROTE Kappe hinter und über dem Kopf.** Die Hügelflächen v8/v18/v14 mit dem Original-Fleisch sind auch
  von vorn sichtbar. Sie bilden ein rotes Fünf- bzw. Dreieck mit geraden Kanten und einer Spitze oben. Links daneben läuft ein gerader
  dunkler Grat vom Kopf nach oben. Belege: k_kopf.png F354/F362/F370 (x3), k_vorn.png F344/F358/F366, gdigrab t 13,25 (gdi_nah.png),
  k_seite F262/F290 (rote Spitze auf dem Kopf), Tafel 09 rechts.
  Gemessen wurden kräftig rote Rumpfpixel (a* >= 30, Knochen 8) oberhalb der Kopfoberkante + 5:
  F354 **V8 r3 214** gegen V7 0, V8 r2 2 und Original-EM30 0; P1 **V8 r3 306** gegen V7 0, V8 r2 0 und Original 0.
  Der Riegel S9 prüft nur die Kontur und sieht diese Platte deshalb nicht. Gegen den Nutzer-Satz "smooth straight to head without further
  hills" steht von vorn wieder ein Kamm, nur rot statt grau, und er sitzt direkt am gelobten Gesicht.
- **Von hinten ist der Hügel nicht "little", sondern besteht aus zwei großen roten Kacheln mit harten Kanten.** Belege: k_hinten F332/F338,
  gdigrab t 12,50/12,75, Tafel 08 F334/F340, runde3/wurzel_hinten_offline.png. Gemessen auf Knochen 8 (a* >= 30, größte
  zusammenhängende Flecken):
  F332 V8 r3 **721 + 435 px** (9,1 % des Rumpfs) gegen Original-EM30 296 px (4,0 %), V8 r2 122 px und V7 0;
  F328 V8 r3 **623 + 238 px** (7,5 %) gegen Original 234 px (3,5 %).
  Die Kacheln wirken wie aufgelegte rote Rechtecke. In der Vorlage gibt es am Hügel nur einen kleinen roten Bereich, den der Nutzer
  ausdrücklich "Little red hill" nennt.

### (3) Rechte Seite: mehr rote Adern an Arm und Körper, sauber wie am Original: Abweichung, die der Nutzer ablehnen würde
- Positiv: In PV (Tafel 05 und 10) trägt die (W)-rechte Flanke jetzt die blassrosa Original-Haut mit roten Adern. Sie liegt deutlich näher an
  der Vorlage und am Original-EM30 als V7 (grau).
- **Das gerade rote Band über den Unterarm ist NICHT entfallen** (Vorgabe O1: "ENTFAELLT"). p2_unterarm.png zeigt in P2 einen durchgehenden
  roten Streifen an der Außenkante von Knochen 14, an derselben Stelle wie in V8 r2. Gemessen (r-g > 35 und r-b > 25 auf Knochen 14):
  V8 r2 384 px, **V8 r3 384 px, y 299-367 in beiden**, V7 0. Er ist auch in Tafel 02 (x 1390-1410, y 380-540), Tafel 03 (P1) und
  k_vorn F344/F350 zu sehen. Er stammt aus Original-Texeln, nämlich der Klingen-/Krallenleiste des Original-Birkin (P2_orig_v8.png). Ohne die
  Klinge wirkt er am V7-Unterarm aber als gerades rotes Band und nicht als Adern. Das Dossier behauptet "das V8-r2-Band entfaellt" und
  widerspricht damit der Messung.
- **Harte Farbnaht rosa/grau.** Die (W)-rechte Hälfte ist rosa, die (W)-linke grau. Von vorn läuft die Naht senkrecht dicht neben dem Kopf
  (k_kopf F354/F362: gerade Kante links vom Gesicht), von hinten schräg über den Rücken (k_hinten F332, gdigrab t 12,50). Das Original-EM30
  ist rundum einheitlich rosa (F332_orig_v8.png, F354_orig_v8.png) und hat keine solche Naht. Das Dossier führt den Punkt selbst unter
  "Offen 3".
- Zusammen mit den roten Hügelkacheln (siehe 2) wirkt die Seite stellenweise aufgelegt und fleckig. Das widerspricht "sauber wie am
  Original-Birkin".

### (4) Gesicht und linke Hand wie V7: wie gewünscht
Pixelvergleich V7 | V8 r3 mit gleicher Pose und Kamera:
- Knochen 9 (Kopf): 0 ungleiche Pixel in P1 (1267 px), P2 (902), F354 (1291) und PV (1120).
- Knochen 10/11/12 ((W)-linker Arm und Hand): 0 ungleiche Pixel in P1, F354, F332 und PV.
- Beine (0-7) und Knochen 15: 0 ungleiche Pixel.

Riegel S5 grün (6501 Texel). Das Gesicht selbst ist unverändert. Die neue rote Kappe direkt dahinter ändert aber den Eindruck am Kopf
(siehe 2).

### (5) Szene in Bewegung, von hinten kein Keil, kein Schlauch, kein Durchstoßen: Abweichung, die der Nutzer ablehnen würde
- Was läuft: state.log ist bytegleich mit dem freigegebenen V7-Lauf, "Szene Ende: Cut 4". Der Bogen bewegt sich ruhig und ohne Horn oder
  Balken (Tafel 07, kachel_gdi2.png). Die Gelenkwinkel liegen bei höchstens 25,1° (szene_mass.txt), Boden 0/912 (Riegel S6).
- **Der Keil an der Wurzel ist geblieben.** In k_hinten F332/F338 (x2) und gdigrab t 12,50/12,75 steckt ein dunkelgrauer Keil mit geraden
  harten Kanten in der Rückenhaut, und zwischen Keil und Haut sieht man eine Lücke. Riegel S10 ist laut Bauer selbst ROT: 8729 sichtbare
  Wurzelpixel (V8 r2: 9671). Das ist nur um 10 % besser. Genau diesen Punkt hatte die Prüfung r2 bemängelt.
- Schlauch: Von direkt hinten löst sich der Bogen jetzt hornartig nach oben und hinten (besser als in V8 r2). Von schräg hinten
  (k_dreh F316, k_hinten F320) liegt er als Wulst auf dem Rücken. Das ist eine kleine Abweichung.
- Durchstoßen: In den angesehenen Bildern (F225-F450, gdigrab 9-16 s) finde ich kein Durchstoßen.

### (6) Insgesamt klarer Fortschritt gegenüber V7: Abweichung, die der Nutzer ablehnen würde
Am Körperbau in der Vorlagenansicht ist der Fortschritt deutlich: Bogen, Breite, Übergang und Boden stimmen (Tafel 01/02 gegen V7).
Gleichzeitig kommen neue, gut sichtbare Fremdkörper hinzu:
- eine rote kantige Kappe hinter dem gelobten Gesicht (von vorn, also in der langen Frontalphase der Szene),
- große rote Rechtecke auf dem Rücken,
- das unveränderte gerade rote Unterarmband,
- eine harte Naht rosa/grau,
- der weiterhin sichtbare Wurzelkeil.

Der Nutzer ist mit "Schwanz und sowas" "0 zufrieden" und hat gerade diese Stellen (Kamm, Hügel, Huckel, Band) wiederholt abgelehnt. In dieser
Form würde er den Stand nicht als klaren Fortschritt abnehmen.

## Punkte für Runde 4 (aus den Messungen, keine Gestaltung)
1. Hügel: Das rote Fleisch darf nur auf Flächen liegen, die von vorn nicht sichtbar sind. Neuer Riegel: rote Pixel über der
   Kopfoberkante in P1/F354 höchstens auf V7-Niveau (0). Von hinten soll die größte rote Fläche höchstens so groß sein wie am Original-EM30
   (F332 296 px).
2. Unterarmband: Riegel "rote Pixel Knochen 14 in P2 <= V7 + kleiner Rest". Heute liegen sie bei 384 px, genauso wie in V8 r2.
3. Naht rosa/grau: Die Übergangsflächen an der Rückenmitte und neben dem Kopf müssen behandelt werden (Original-Zeilen beidseitig oder eine
   Übergangszeile).
4. S10 bleibt offen (Weg laut Dossier: die Haut über dem Rohr anheben).
5. Riegel S9 um einen Farb- oder Kantentest im Inneren der Silhouette erweitern. Der heutige Konturtest übersieht die rote Kappe.

Testbelege (ctest 8/8, 1013/1013, V8-KETTE-OK) habe ich aus den Dateien des Bauers übernommen und nicht selbst nachgerechnet. Das Asset
und der Szenenlauf sind selbst geprüft.
