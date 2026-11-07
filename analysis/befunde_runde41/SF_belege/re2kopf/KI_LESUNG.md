# SF Leser B: Lesung der KI-Bilder (Sichel-Form), Runde 41 (2026-11-07)

Kein RE-Befund: die Sichel-Form gibt es auf der Disc nicht; alles hier ist Bildmessung an drei KI-Bildern, die der Nutzer
als Vorlage benannt hat ("bei den KI Bildern kann man so verdammt gut sehen wie das verschwommene Bild eigentlich aussehen
muss ... Das Auge die Haare, den Mund, den Schwanz"). Keine Zahl ist eine @0x-Konstante. Nichts unter Version_80%/,
pri/, synchro/ geaendert. Nicht committen (Auftrag).

## 0. Quellen und Verfahren

| Kuerzel | Datei | Kacheln (512 x 512, Beschriftung ab y 470) | H (Scheitel-Sohle, FRONT) |
|---|---|---|---|
| **V1** | C:/workspace/git/reAi_v2/Version_80%/image_1.png | FRONT (0,0), RIGHT SIDE (512,0), BACK (0,512), LEFT SIDE (512,512) | Scheitel y 22 (Kuppel; roter Kamm bis y 5), Sohle y 462: **442 px** |
| **V2** | C:/workspace/git/reAi_v2/Version_80%/image_2 (2).png | FRONT / RIGHT / BACK / LEFT, gleiche Lage | Scheitel y 28 (Kamm bis y 12), Sohle y 463: **452 px** (+-4) |
| **V3** | C:/workspace/git/reAi_v2/Version_80%/image_3 (3).png | VORNE / RECHTS / HINTEN / LINKS, gleiche Lage | Scheitel y 18 (Kamm bis y 8), Sohle y 449: **442 px** (+-4) |

Nicht verwendet (Auftrag): image_0.png, image_3 (1).png, birkin_form3_*.png.
Nutzer-Beschriftungen: Beschreibung1.png / Beschreibung2.png (+ _Vorlage), Vorderseite_Beschreibung.png (5.11., nachrangig).

Verfahren: Kacheln geschnitten, Gitter in Kachel-Koordinaten ueberlagert (Schritt 5 px auf 4x-Kopfcrops, 20 px auf 2x-Kacheln),
Kanten und Merkmale abgelesen (+-2 px = +-0,005 H). Farben: Median CIE-Lab (D65, sRGB) in Rechteckfenstern, roh
(KI-Bilder haben ihr eigenes Studiolicht; keine Eichung auf den Port-Schirm moeglich). Crops, Gitterbilder, Skripte und
JSON: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ki_sf/
(crop1.py .. crop4.py, lab1.py, tafel.py, render_v4.py; zonen_median.json, v4_zonen.json, render_info.json).
V4-Vergleich: EM30_V4.EMD (sha256 a1bc9309e5dd9ce8..., 262004 B = ot3) und Referenz 2 (EM30_HANGAR.EMD sha256 cfd1ac42...)
mit v4_gemein.zeichne, Spielkamera 50A0 Cut 12, RDT-Licht, Pose 135 und 85, Id-Puffer (Kopf = Id 9).
Tafel: analysis/befunde_runde41/SF_belege/re2kopf/01_ki_merkmale.png (Zeilen: Kopf vorn 4x, Kopf seitlich 4x, Schwanz, Arme,
Ref2/V4; Spalten: KI V1 | KI V2 | KI V3 | Nutzer-Beschriftung).

Seiten: **(B)** = aus Sicht des Betrachters im FRONT-Bild, **(W)** = aus Sicht des Wesens; vorn gilt (B)-links = (W)-rechts.

## 1. KOPF

### 1.1 Groesse und Sitz (FRONT)

| | V1 | V2 | V3 |
|---|---|---|---|
| Kuppel (Kopfhaut ohne Maul) Kachel-px | x 222-290, y 22-88: 68 x 66 | x 222-282, y 28-94: 60 x 66 | x 213-252, y 18-50: 39 x 32 (Kuppel), mit Kiefer bis y 78: 39 x 60 |
| Kuppel in H | **0,154 x 0,149** | **0,133 x 0,146** | 0,088 x 0,072 (mit Kiefer 0,088 x 0,136) |
| Kopf gesamt bis Ende Kehlschlauch | y 22-118 = 0,217 H | y 28-120 = 0,204 H (roter Keil laeuft in die Brust) | y 18-100 = 0,186 H |
| Kopfmitte x | 256 | 252 | 232,5 |
| Schulterlinie (Oberkante Trapez) | y 48 -> Scheitel **0,059 H** darueber | y 40 -> 0,027 H | y 45 -> 0,061 H |

Der Kopf sitzt **tief zwischen den Schultern**: der Scheitel ragt nur 0,03-0,06 H ueber die Schulterlinie, kein Hals sichtbar;
unter dem Maul haengt ein Kehlschlauch bis in die Brust. Die Kuppel ist glatt, haarlos, nach vorn-unten geneigt (wir sehen
von vorn auf die Kuppel-Oberseite; Auge auf der Kuppel-Vorderseite, Maul darunter).

### 1.2 Seitlich (RIGHT) - Neigung und Hoehe

| | V1 RIGHT | V2 RIGHT | V3 RECHTS |
|---|---|---|---|
| Kopfbox | x 400-470, y 95-150: 70 x 55 = **0,158 x 0,124 H** | x 345-412, y 95-150: 0,148 x 0,121 H | x 92-150, y 60-118: 0,132 x 0,132 H |
| Buckel-Oberkante | y 20 | y 15 | y 20 |
| Kopf-Oberkante unter Buckel | **0,17 H** | **0,18 H** | 0,09 H |
| Schnauzenachse | ~35 Grad unter der Waagerechten, nach vorn-unten | ~35 Grad | ~30 Grad |
| Auge | (455,127): **0,74 H ueber Sohle** | (392,122): 0,75 H | (100,85): 0,83 H |
| Maul | Unterseite/Front, offen, Zaehne sichtbar | gleich | gleich |

Der Kopf haengt **vor** dem Buckel, nicht auf ihm; die Kuppel-Oberkante liegt 0,09-0,18 H unter dem Buckelscheitel.
Augenhoehe 0,74-0,83 H ueber der Sohle - **deckt sich mit Beschreibung2 (S2: 0,79 H2)**.

### 1.3 Maul

| | V1 | V2 | V3 |
|---|---|---|---|
| Oeffnung Kachel-px | x 232-275, y 66-90 | x 230-275, y 72-95 | x 218-250, y 50-78 |
| in H | **0,097 x 0,054** | **0,100 x 0,051** | 0,072 x 0,063 |
| Zustand | weit offen, Kiefer haengt nach unten | offen | weit offen |
| Zaehne | zwei Reihen kleiner weisser Zaehne (je 8-12 sichtbar), unregelmaessig | gleich | gleich, dichter |
| Richtung | frontal-symmetrisch, Oeffnung nach vorn-unten | gleich | gleich |
| Inneres Lab (Median) | 16,0 / 15,5 / 7,7 | 18,1 / 16,8 / 9,0 | 16,0 / 16,3 / 8,3 |
| Kehlschlauch darunter | y 90-118, Breite 26 px: **0,063 H lang, 0,059 H breit**, waagerecht geriffelt, Lab 22,9 / 17,6 / 8,0 | kein Schlauch; roter Fleischkeil bis zur Brust | y 78-100: 0,050 H, Lab 24,5 / 18,3 / 10,9 |

Gemeinsam: **offenes, zahnbesetztes Maul unter dem Auge, dunkelrotes Inneres (Lab ~16-18 / 15-17 / 8-9), Breite 0,07-0,10 H,
Hoehe 0,05-0,06 H**. Darunter ein rosa-roter Kehlbereich (2 von 3 als geriffelter Schlauch).

### 1.4 Auge(n)

| | V1 | V2 | V3 |
|---|---|---|---|
| Zahl | **1** | **2** | **1** |
| Lage FRONT (dx zur Kopfmitte, dy unter Kuppel-OK) | (254,51): **-0,005 H / 0,066 H** | (231,57): **-0,046 H / 0,064 H**; (258,58): +0,013 H / 0,066 H | (233,37): **0,000 H / 0,043 H** |
| Groesse | Pupille ~5 px = 0,011 H, dunkle Hoehle ~8 px | je ~5-6 px = 0,012 H | heller Ring 6 px = 0,014 H, Pupille 3 px |
| Aussehen | dunkle Kugel (Robbe/Hai), weisser Glanzpunkt | zwei dunkle Gruben ohne Glanz (Lesung Augen; alternativ Nasenloecher) | **weisser Ring um dunkle Pupille** (Zyklop), Glanzpunkt |
| Farbe | Pupille L* < 15, Glanz L* > 80 | L* < 15 | Ring L* ~75, Pupille < 15 |

Gemeinsam: **mindestens ein kleines (0,011-0,014 H) dunkles Auge im oberen Drittel der Kuppel-Vorderseite, 0,04-0,07 H unter
dem Scheitel, von mittig bis 0,046 H (B)-links.** Beschreibung1 (0,050 H (B)-links, heller Fleck) trifft V2s linkes Auge in
der Lage, aber **nicht** im Aussehen: in allen drei KI-Bildern ist das Auge dunkel mit hellem Glanzpunkt, im Video ein
heller Fleck (Messblatt K2 46,9 / 7,2 / -1,1). Vorderseite_Beschreibung (zwei Augen) deckt sich nur mit V2.

### 1.5 Haare

**Kein KI-Bild zeigt Haare als Straehnen oder Buschel.** Was der Nutzer "Haare, braeunlich, rechts" nennt, hat in den
KI-Bildern zwei Entsprechungen:
1. Die **(B)-rechte Kuppelhaelfte ist dunkler und braun-grau** als die linke, mit senkrechter Streifenstruktur (V1 und V3
   deutlich, V2 schwach): V1 links 32,2 / 7,8 / 2,8 gegen rechts **17,1 / 6,1 / 2,8** (dL -15); V2 38,4 / 8,8 / 4,7 gegen
   17,2 / 7,3 / 3,6 (dL -21); V3 26,3 / 9,3 / 5,0 gegen 24,2 / 6,3 / 3,7 (dL -2). Der Uebergang laeuft weich ueber die
   Kopfmitte (keine Kante). Das deckt sich in Ort und Farbrichtung (a* > 0, b* > 0, dunkel) mit Beschreibung1
   (Messblatt K5 13,3 / 7,4 / 1,5).
2. Ein **roter Fleischkamm** vom Scheitel ueber den Nacken bis zum Schwanzansatz (alle drei): V1 Scheitel 19,3 / 19,5 / 6,7,
   V3 Ruecken 25,2 / 25,0 / 11,0; in V3 HINTEN als rote Spitzen/Straehnen, in V1 BACK als rote Fasern. Von der Seite liegt er
   oben auf dem Kopf (Beschreibung2 "Kopfoberseite rot", Messblatt S4) - er ist rot, nicht braun.

### 1.6 Nase

Keine abgesetzte Nase in allen drei Bildern. Die Schnauzenfront ist eine glatte Rundung ueber dem Maul; in V2 zwei dunkle Gruben
(Abschnitt 1.4; wenn Nasenloecher, dann 0,046 H links und 0,013 H rechts der Mitte, 0,065 H unter der OK). Beschreibung1
"Nase" = Zonengrenze grau/braun mittig-unten (Messblatt K7) ist mit den KI-Bildern vereinbar (kein Merkmal dort).

### 1.7 Kopffarbe

| Zone | V1 | V2 | V3 | Richtung |
|---|---|---|---|---|
| Kuppel gesamt (Median) | 23,6 / 7,0 / 2,7 | 25,4 / 7,9 / 4,0 | 26,4 / 7,4 / 4,4 | dunkles warmes Grau |
| (B)-links | 32,2 / 7,8 / 2,8 | 38,4 / 8,8 / 4,7 | 26,3 / 9,3 / 5,0 | heller grau |
| (B)-rechts | 17,1 / 6,1 / 2,8 | 17,2 / 7,3 / 3,6 | 24,2 / 6,3 / 3,7 | dunkel braun-grau |
| Koerperhaut Brust/Schulter (Median, mit Blut) | 16,9 / 9,8 / 5,5 | 17,2 / 6,6 / 4,9 | 15,6 / 8,7 / 5,7 | |
| Koerperhaut beleuchtet (Mittel 24 x 20, V1 Brust) | 31,5 / 4,5 / 3,6 | | | blasses Grau |

Der Kopf ist im Mittel **so dunkel wie der Koerper-Median und 5-8 L* dunkler als die beleuchtete Koerperhaut**, unbunter als
die Referenz-2-Kopfhaut (Ref2 Pose 135 Id 9: 30,8 / 15,9 / 7,3). Kein blasses Rosa auf der Kuppel.

## 2. SCHWANZ

| | V1 RIGHT | V2 RIGHT | V3 LINKS |
|---|---|---|---|
| Ansatz | Buckelscheitel (270,25): **0,97 H ueber Sohle**, Mitte zwischen den Schultern | (260,20): 0,98 H | (260,20): 0,96 H |
| Breite nahe Ansatz (senkrecht gemessen) | 80 px = **0,18 H** | 70 px = 0,15 H | 60 px = 0,14 H |
| Verlauf | grosser **C-Bogen**: vom Scheitel nach hinten-oben, hinter dem Ruecken nach unten, Innenradius ~0,3 H; aeusserster Punkt 0,47 H hinter der Rueckenlinie | gleich, enger (0,40 H) | gleich (0,45 H) |
| Knick / Taille | **keine**; stetige Verjuengung | keine | keine |
| Spitze | (175,430): **0,05 H ueber Sohle**, hinter den Fersen, Klinge 0,20 H lang | (150,365): 0,22 H | (110,380): 0,14 H |
| Achslaenge (Bogen) | ~1,4 H | ~1,2 H | ~1,3 H |
| BACK | haengt **mittig gerade** am Ruecken herab bis zum Boden (x 230-300) | mittig, Spitze zwischen den Fuessen | nach (B)-links ausschwingend, Spitze (45,330) |
| Grundton (Median Bogen) | **18,6 / 0,5 / -0,8** (neutral dunkelgrau) | 9,9 / 0,5 / 0,7 | 15,6 / 3,9 / 0,0 |
| rote Streifen | ein rotes Band entlang der Aussen-/Oberlinie vom Ansatz bis zur Mitte; rote Flecken | rote Querstreifen (Fleischnaehte) | rote Laengsflecken oben, rosa Basis |
| Spitze | **blutrot** 12,8 / 0,5 / -0,8 (Median, mit Rot-Spritzern) | 11,2 / 7,5 / 3,5 | 13,1 / 4,9 / 3,4 |

**Gegen Beschreibung2** (Messblatt Abschnitt 4: Beginn 0,767 H2 ueber Sohle, 0,25 H2 waagerecht nach hinten, Biegung 70-90 Grad,
Knick/Taille 0,51 H2, Ende 0,37 H2, Achse 0,70 H2): Die KI-Schwaenze sind **doppelt so lang (1,2-1,4 H gegen 0,70 H2), setzen
am Buckelscheitel statt 0,23 H unter ihm an, reichen bis zum Boden und haben keine Taille.** Gemeinsam mit dem Video: Ansatz
am oberen Ruecken, Verlauf nach hinten und dann abwaerts, dick am Ansatz (0,14-0,18 H; Video 0,14 H2 bei f 0,7), spitz
endend, neutral dunkelgrauer Grundton mit Rot an Wurzel und Streifen (Video Wurzel 9,3 / 11,4 / 1,5, Bogen 26,8 / -0,1 / 2,4).
V4 (Meshes 16-22) folgt Beschreibung2; der KI-Schwanz waere eine andere Gestalt (Entscheid Orchestrator/Nutzer, kein Delta hier).

## 3. ARME

Alle drei Varianten gleich: im FRONT-Bild traegt der **(B)-linke = (W)-RECHTE Arm die Sichel**, der (B)-rechte = (W)-LINKE Arm
ist eine **normale Hand mit blutigen Fingern**. In RIGHT SIDE ist die Sichel am nahen Arm (V1 x 300-400, y 300-440), in
LEFT SIDE die Hand (V1 (130-160, 320-360)) - Seitenlage bestaetigt.

| | V1 | V2 | V3 |
|---|---|---|---|
| Sichelarm | ganzer Unterarm verwandelt: **olivbraun-schuppig** (13,3 / 2,5 / 3,7), drei Auswuechse, Klinge 0,26 H lang, blutig | Unterarm bandagiert, Hand -> rote Klinge 0,25 H | Unterarm dunkelgrau, Klinge 0,20 H + zweite Klaue |
| andere Hand | 5 Finger, rot | 5 Finger, rot | 5 Finger, rot, Krallen |

Vorderseite_Beschreibung ("linke Klaue" und "rechte Klaue") widerspricht: nach KI nur eine Sichel. V4/Referenz 2 hat die Sichel
bereits am (W)-rechten Arm (Render Pose 135: Klinge (B)-links) - **kein Delta**.

## 4. Vergleich mit dem V4-Stand (EM30_V4.EMD = ot3, RDT-Licht)

| Mass | KI (Schnittmenge) | V4 ot3 | Ref2 |
|---|---|---|---|
| Kopfbox vorn (Pose 135, Id 9) | Kuppel 0,13-0,15 x 0,15 H; mit Maul+Kehle 0,19-0,22 H | (325,154)-(380,221) = 55 x 67 / H_port 364 = **0,151 x 0,184 H** | 45 x 61 = 0,124 x 0,168 H |
| Kopf-OK unter Buckel (Pose 85) | 0,09-0,18 H | OK 185 - Koerper-OK 170 = 15 / 264 = **0,057 H** | 0,064 H |
| Auge | dunkle Pupille 0,011-0,014 H + Glanzpunkt, 0,04-0,07 H unter OK, mittig bis 0,046 H links | **heller rosa Rechteckfleck** (hellste 5 %: 54,2 / 6,1 / 2,2), ~0,03 x 0,03 H, 0,054 H links, 0,095 H unter OK, keine Pupille | kein Auge |
| Maul | offen, Zaehne, dunkelrot, 0,07-0,10 x 0,05-0,06 H; Kehle 0,05-0,06 H | **fehlt** | fehlt |
| Kuppe oben | dunkelroter Kamm 19-25 / 19,5-25 / 7-11 | oberes Drittel 31,8 / 17,1 / 7,3 (heller, blasser) | 30,8 / 23,2 / 10,8 |
| Kopfhaut links / rechts | 26-38 / 8-9 / 3-5 gegen 17-24 / 6-7 / 3-4, weicher Uebergang | 28,9 / 3,3 / 1,2 gegen 23,1 / 3,9 / 7,2, **harte Zonenkante, Kloetzchen** | 30,8 / 19,6 / 8,8 gegen 35,8 / 14,8 / 6,0 |
| Kopfform | glatte Kuppel/Schnauze, 30-35 Grad vorn-unten, Maul an der Unterseite | eckiger Klotz, Zonen flach gemalt, keine Schnauze, keine Unterseite | Ref2-Kopf (Birkin-Original) |

## 5. DELTAS zu V4 (Kopf zuerst; Schwanz/Arme nur Befund)

1. **Kopfform**: glatte, haarlose Kuppel mit Schnauze, Achse 30-35 Grad nach vorn-unten; Kopf haengt vor dem Buckel, Oberkante
   0,09-0,18 H unter dem Buckelscheitel (V4: 0,057 H, also 0,03-0,12 H zu hoch); Scheitel vorn nur 0,03-0,06 H ueber der
   Schulterlinie. Groesse der Kuppel 0,13-0,15 x 0,15 H bleibt (V4 0,151 x 0,184 passt mit Maul).
2. **Maul neu**: an der Kuppel-Unterseite/Front, 0,07-0,10 H breit, 0,05-0,06 H hoch, offen, dunkelrotes Inneres (Lab ~16-18 /
   15-17 / 8-9), zwei Reihen kleiner weisser Zaehne; darunter Kehlbereich 0,05-0,06 H lang, 0,06 H breit, rosa-rot
   (23-24 / 18 / 8-11), geriffelt. In V4 gibt es kein Maul.
3. **Auge**: dunkle Pupille (L* < 15) 0,011-0,014 H mit weissem Glanzpunkt (V3: heller Ring 0,014 H) statt des hellen rosa
   Rechteckflecks; Lage Schnittmenge KI + Beschreibung1: 0,035-0,050 H (B)-links der Kopfmitte, 0,065-0,090 H unter der OK
   (V4 0,054 / 0,095 liegt am Rand). Ein Auge; die (B)-rechte Spiegelstelle bleibt dunkel.
4. **"Haare" = dunkle (B)-rechte Kuppelhaelfte**: 9-21 L* dunkler als links, braun-grau (17-24 / 6-7 / 3-4), mit senkrechter
   Strichstruktur und **weichem Uebergang ueber die Mitte** - keine harte Zonenkante, keine Kloetzchen (V4 heute).
5. **Kuppe**: dunkelroter Fleischkamm (19-25 / 19,5-25 / 7-11) vom Scheitel ueber den Nacken bis zum Schwanzansatz,
   nahtlos in den Ruecken; V4-Oberseite ist heller und blasser (31,8 / 17,1 / 7,3) und endet am Kopf.
6. **Kopfhaut-Ton**: dunkles warmes Grau (Median 24-26 / 7-8 / 3-4), 5-8 L* dunkler als die beleuchtete Koerperhaut, kein
   Rosa; V4-Kopf ist bereits unbunt (3,4 / 4,5), aber mit Kante statt Verlauf.
7. **Schwanz (Befund, kein Delta ohne Entscheid)**: KI = C-Bogen vom Buckelscheitel (0,97 H) 0,40-0,47 H nach hinten bis
   0,05-0,22 H ueber den Boden, Achse 1,2-1,4 H, Basis 0,14-0,18 H, ohne Taille; Beschreibung2 = 0,70 H2 mit Knick. Farbe
   stimmt mit Video und V4 ueberein (neutral dunkelgrau, Rot an Wurzel/Streifen).
8. **Arme**: kein Delta (Sichel (W)-rechts, Hand (W)-links wie Ref2/V4).

## 6. Widersprueche

Zwischen den drei Varianten:
- Augenzahl: V1 und V3 ein Auge, V2 zwei.
- Augenlage: V1 -0,005 H, V3 0,000 H (mittig), V2 -0,046 H / +0,013 H.
- Augenaussehen: V1/V2 dunkle Kugel/Grube, V3 weisser Ring um dunkle Pupille.
- Kuppelgroesse: V3 0,088 x 0,072 H, V1 0,154 x 0,149 H.
- Kopf-OK unter Buckel: V1/V2 0,17-0,18 H, V3 0,09 H.
- Kehle: V1/V3 geriffelter Schlauch, V2 roter Fleischkeil in die Brust.
- Schwanzspitze: V1 0,05 H, V2 0,22 H, V3 0,14 H ueber der Sohle; BACK: V1/V2 mittig gerade, V3 nach (B)-links.
- Sichelarm: V1 ganzer Unterarm olivbraun-schuppig, V2 bandagiert mit roter Klinge, V3 grau mit Klinge + Klaue.

Zwischen KI und Nutzer-Beschriftung / Video:
- Auge: KI dunkel mit Glanzpunkt; Video/Beschreibung1 heller Fleck (Messblatt K2 46,9 / 7,2 / -1,1).
- Augenlage: KI mittig bis -0,046 H; Beschreibung1 -0,050 H (nur V2 deckt sich).
- Haare: KI ohne Straehnen (dunkle Schattenhaelfte + roter Kamm); Nutzer "Haare braeunlich".
- Schwanz: KI 1,2-1,4 H C-Bogen bis zum Boden ohne Taille; Beschreibung2 0,70 H2 mit Knick/Taille, Ende 0,37 H2.
- Vorderseite_Beschreibung: "zwei Augen" (nur V2), "offenes Maul nach rechts" (KI frontal-symmetrisch), "linke und rechte
  Klaue" (KI eine Sichel rechts, links Hand) - nach Auftrag gilt die KI.

**Schnittmenge aller drei KI-Bilder** (das, was gebaut werden kann, ohne eine Variante zu bevorzugen): glatte haarlose
dunkel-graubraune Kuppel tief zwischen den Schultern, 30-35 Grad vorn-unten geneigt, vor dem Buckel haengend; ein offenes
zahnbesetztes Maul mit dunkelrotem Inneren an der Unterseite; mindestens ein kleines dunkles Auge im oberen Drittel, mittig
bis (B)-links; (B)-rechte Haelfte dunkler braun-grau; roter Kamm vom Scheitel zum Nacken; Schwanz dick am oberen Ruecken,
dunkelgrau mit roten Streifen, spitz; Sichel (W)-rechts, Hand (W)-links.

## 7. Offen (nur, was kein Bild beantwortet)

- Ob der Nutzer mit "Haare" den roten Kamm (KI) oder die dunkle braune Haelfte (Beschreibung1) meint - beides ist moeglich;
  die KI-Bilder zeigen beides, aber keine Straehnen.
- Ob der Schwanz nach KI (langer C-Bogen) oder nach Beschreibung2 (kurz, Knick) gebaut werden soll; beide widersprechen sich
  in Laenge und Form, nicht in der Farbe.
- RE2-Herkunft (CDEMD0.EMS 0x30/0x31/0x33): nicht Aufgabe von Leser B; Form-Identitaet am Render zu pruefen.
