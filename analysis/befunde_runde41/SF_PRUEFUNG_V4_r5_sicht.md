# SF V4 - Pruefung Runde 5 (Sicht): V4 r5 gegen die Nutzer-Beschriftung Beschreibung1/2

Pruefer "Auge des Nutzers", 2026-11-07, Worktree r41_sichel (Zweig r41/sichel_nb2, HEAD 7320d8a1f), nichts committet.
Gegenstand: `sfv4/emd/EM30_V4.EMD` = `EM30_V4r5.EMD` sha256 a762bd66... (Dossier V4 Runde 5), Vergleich V4 r4 `EM30_V4r4.EMD` 00d642b7...,
Referenz 2 = Spiel-Asset `re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD` cfd1ac42... (= Pin, unveraendert). Leisten
`SF_belege/v4/01..09` (r5) und `SF_belege/v4/r4/01..08`; Spiellaeufe `sfv4/r/V4r5` und `sfv4/r/V4r4` (RE15_FRAMEDUMP, registriert ueber
`c6.port`, Versatz 243 in beiden Laeufen). Nutzerbilder `Version_80%/Beschreibung1/2(_Vorlage).png` unveraendert (mtime 2026-11-06 21:46-21:57).

**Ergebnis: NICHT bestanden.** Ein Punkt gehoert in die dritte Klasse:

1. **Schwanz ausserhalb der Messansicht: weiss-graue Baender / rot-weiss gefleckte Kette** (neu in r5, Rueckschritt gegen r4). In der
   Buehnen-Pose 85 (Leisten 02/05/09) stimmt der Schwanz jetzt (s. 1.3). Die Textur ist aber aus EINER Ansicht (Vorlage 2) gebacken und
   ringsum gewickelt; die hellen Stellen der Vorlage (Biegung, zwei Baender am senkrechten Teil) werden in jeder anderen Ansicht zu hellen
   Ringen. Gemessen: Texel neutral Mesh 18 bis L* 60, Mesh 20 bis L* 75 (hellster Original-Schwanzpixel 46,1, p99 40,8). Im SPIEL genau
   am Vorlage-2-Zeitpunkt h85 (Video n1435) liegt ueber dem Schwanz ein weiss-grauer Zickzack-Streifen (57,2/1,8/-3,2): Mesh 18 11,7 %
   der Pixel L* > 50, max 60; Schwanz im Videoraum 25,4/5,2/1,0 gegen Original-Schwanz 17,8/5,1/1,5 = **dE00 5,4 (r4 2,2)**, Anteil
   L* > 40 **16,8 % gegen 1,2 %** im Original. Spielbilder h84/h88: 110/171 px L* > 50 auf dem Schwanz (r4: 0). Sprungbilder h68/h76:
   der Schwanz ist eine rot-weiss gefleckte Kette (Schirm L* > 45 3,7 / 4,6 %, r4 0,0 / 1,0 %; Video h68 glatt rosa-rot). Nach der
   Regel O9 ("keine Farbe, die im Original-Bild an der Stelle nicht vorkommt", alle Koerper-Meshes) am Schwanz gemessen: Anteil > 10 dE76
   von jeder Farbe des Original-Schwanzes Gier +60/+180/+240 **0,038/0,034/0,033**, Spiel h85 **0,032** (Grenze des AF-Riegels 0,01;
   r4 <= 0,005). Der Riegel AF prueft nur Ids 0-15 (riegel_v4.py ausreisser_farben / r5_mass.ausreisser: range(16)) - der Schwanz
   (Ids 16-22) faellt durch. Vorgeschichte: V1 wurde u. a. wegen "gemalter Ringe" am Schwanz abgelehnt ("hat komplett die falsche
   Farbe", Dossier SV3.1).

Bestanden und gegen r4 besser geworden (gehalten werden muss): **Schwanz waagerechter Teil** (kein Rohr, keine Bloecke, keine Manschette,
Verlauf entlang der Achse wie Vorlage 2), **Wurzel geht in den Ruecken ueber**, **gelber Fleck weg** (0 gelbe Texel, 0 Ausreisser),
**Rot in der Brustmitte** (Mitte/aussen im Spiel 2,73/2,76 gegen Video 2,91/2,75), grelle Einzelpixel weg (C* > 45: 0 gegen 253),
Kopf vorn, Auge/Haare/Nase, Auge/Haarkante seitlich, Schwanz-Linie (T1-T4), Unterarme, Jeans, Gang.

Konventionen: (B) = aus Sicht des Betrachters, (W) = aus Sicht des Wesens. Lab = CIE-Lab D65, dE00 = CIEDE2000. "Videoraum" =
Vergleichsregel Messblatt V4 (Gauss sigma 1,5, V = a + b x P). "Schirm" = Port-Bild ohne diese Abbildung. Vorlage 1 entblendet, Vorlage 2
roh. H1 = 302 px, H2 = 253 px, H_port 364 (Pose 135) / 264 (Pose 85).

Werkzeuge (untracked): `C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4/pruefung_sicht_r5/` - die
Skripte der Runde 4 auf r5 umgestellt (`render_r5.py` mit Hash-Ausgabe a762bd66 / 00d642b7 / cfd1ac42, `spiel_r5.py`, `messen_r5.py`,
`oberkoerper_vorn.py`, `oberkoerper_spiel.py`, `rotkappe_spiel.py`, `kopf_struktur_spiel.py`, `kopf_textur_vorn_r5.py`, `kopf_seite_r5.py`,
`auge_spiel_r5.py`, `schwanz_brett.py`, `schwanz_silhouette_r5.py`, `schwanz_farbe_waagerecht.py`, `schwanz_naehte.py`,
`gelb_und_ausreisser.py`, `unterarm_links_r5.py`) und neu `laufposen_ids.py` (Laufposen aus debug.log mit Id-Bild, um Spielbild-Flecken
einem Mesh zuzuordnen; Deckung h85 im Bild geprueft), `schwanz_senkrecht.py`, `schwanz_spiel_hell.py`, `schwanz_sprung.py`,
`schwanz_sprung_ringe.py`, `schwanz_ausreisser.py`, `rundum_schwanz.py`, `ausreisser_farbmenge.py`, `koernung.py`. Ausgaben
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/sicht_r5/`, Belege
`C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PS5_*`. Gegenprobe: meine Werte
reproduzieren die Riegel des Bauers (FO h118 2,73 / h135 2,76; KS h135 37,5 / glatt 0,120; TN 10,7 bei x 297; TK 3,69 px; FR h85 165).

## 1. Befunde Punkt fuer Punkt

Klassen: **W** = wie Vorlage, **K** = Abweichung klein, **A** = Abweichung, die der Nutzer wieder ablehnen wuerde.

### 1.1 Kopf vorn (Beschreibung1, Kamera 50A0/12)

| Punkt | Vorlage (gemessen) | V4 r5 (gemessen) | Klasse |
|---|---|---|---|
| Auge, Lage | Nutzerpunkt (382,165) | K4 0,056 H links / 0,101 H unter OK (Soll 0,035-0,065 / 0,07-0,12) | W |
| Auge, Farbe | 46,9/7,2/-1,1 | K2 47,8/8,5/-1,4 dE00 1,6 (r4 0,8) | W |
| Auge im Spielbild h135 | 52 px, 0,026 x 0,033 H1, Sprung 45,0 | 108 px, 16 x 14 px = 0,044 x 0,038 H, Flaeche 1,43 x Original (r4 0,54 x), Sprung 64,7 (`PS5_auge_spiel.json`). Kopf-Mesh unveraendert, der Fleck verschmilzt mit der helleren Umgebung; Sicht (`PS5_kopf_spiel_x4.png`): wie r4 ein weicher heller Fleck | K |
| Haare | 13,3/7,4/1,5 | K5 13,9/8,1/1,0 dE00 0,9 | W |
| Nase | 18,2/7,1/1,3 | K7 18,1/7,0/-0,1 dE00 1,2 | W |
| Kopf-Oberseite rot (Buehne) | KV1 22,6/15,4/5,2 | K9 20,9/13,0/5,4 dE00 2,2 (Bunttonwinkel 22,6 gegen 18,7 Grad: etwas rostiger) | W |
| Kopf-Mittelfarbe | 20,1/8,5/2,1 | Id 9 Videoraum 19,4/7,8/1,6 dE00 0,9 | W |
| Kinn/Maulzone | 17,1/11,6/4,4 | 15,7/8,8/3,7 dE00 2,6 (r4 3,9) | W |
| Kopf-Oberflaeche im Spielbild | Innen Gradient 32,9 (Spanne 29,0-40,4), glatt 0,074; Kopf/Rumpf 1,57 | h118 31,0 / glatt 0,048; **h135 37,5 / glatt 0,120** (r4 0,134); Kopf/Rumpf h135 1,96 (Rumpf im Spiel 19,7 gegen 26,2: glatter) (`PS5_kopf_struktur_spiel*.json`). Dunkler Kreisrand um den Kopf wie r4 | K |

### 1.2 Kopf seitlich (Beschreibung2, Pose 85)

| Punkt | Vorlage | V4 r5 | Klasse |
|---|---|---|---|
| Auge seitlich | 48,4/4,0/0,5, 77 px | S1 48,5/3,1/0,9 dE00 1,2; 0,042 x 0,049 H; 1 Komp. | W |
| Augenlage | 0,03-0,07 H; 0,76-0,82 H | S2 0,054 H; 0,809 H | W |
| Haare seitlich | 19,1/1,2/1,1, 0-2 px vor dem Fleck | S3 17,2/2,8/0,6 dE00 2,5, Abstand 0,004 H | W |
| Kopf seitlich Flaeche/Form | Gradient Kopfbox 46,3, glatt 0,103 | Id 9 49,1, glatt 0,074 (`PS5_kopf_seite.json`, = r4); Form weiter ein Keil mit gerader Oberkante | K |
| Kopf-Oberseite rot seitlich | 27,6/15,3/3,3 | S4 27,5/11,4/3,2 dE00 3,1 (**r4 0,4**: Rueckschritt, laut Dossier durch die a*-Grenze 21,7 in den Kopfzeilen; in `04_kopf_nah_seite_x4.jpg` etwas blassere Kuppe) | K |
| Gelb hinter dem Kopf | kein Gelb | 0 gelbe Texel Pose 85 (r4 11) (`PS5_gelb_ausreisser.json`) | W |

### 1.3 Schwanz (Beschreibung2)

| Punkt | Vorlage | V4 r5 | Klasse |
|---|---|---|---|
| Beginn, Lage | (357,243), 0,15 H hinter Scheitel | T1 OK (Riegel), Oberkontur an der Wurzel x 352-360 y 198-201 (Original 196) | W |
| Beginn "in den Ruecken ueberfuehrend" | stetig, a* 8,0-9,4 an der Wurzel, groesster Sprung 9,5 dE76 (am Maskenrand x 356) | keine Naht > 8 dE76 zwischen x 300 und 376; a* entlang der Wurzel 7,5-10,1; TV Wurzel 9,7/10,4 gegen 9,0/11,2; keine Manschette, keine gerade Kante (`PS5_schwanz_seite_x4.png`, `PS5_schwanz_naehte.json`) | W |
| Biegung | 65,8-70,1 Grad | T2 64,0 Grad, dx -0,259 dy +0,049 | W |
| Knick | (290,308) | T3 dx -0,239 dy +0,247 (r4 ROT -0,233) | W |
| "von da an breiter nach oben" | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Zeilen y 260/270/280: 23/17/22 px | T5 0,032/0,049/**0,098**/0,163 H (Bauer-Mittellinie), T5q 0,110; Zeilen 16/15/11 px: um den Knick duenner als das Original, Zunahme nach oben vorhanden (`PS5_schwanz_silhouette.json`) | K |
| Spitze Ende | (294,344), 0,37 H | T4 0,354 H | W |
| Umriss | Leser-Maske | IoU 0,483 (r4 0,466), 67 % gedeckt; Oberkontur RMS 3,69 px (Original 3,87, r4 1,77); kleinster Eckwinkel 68 Grad bei (294,257) (`PS5_schwanz_silhouette*.{json,png}`) | K |
| Form waagerechter Teil | Plateau x 290-350 0,08-0,39; glatt (Grad < 3) 0,189; Stufe 3 px 3,5-10,0; keine Quernaht > 9,5 dE76 | **kein Rohr, keine Bloecke**: Plateau 0,18-0,42 (2 von 11 Spalten ueber 0,39: x 290 0,42, x 320 0,41), glatt 0,262 (r4 0,468), Stufe 3,9-8,8; Querprofile hell oben / dunkel unten wie das Original (x 320: 29 -> 7 L*, Original 32 -> 6). Eine Quernaht **10,7 dE76 bei x 297** (Mesh 18 / 19, heller Biegungsring gegen den dunklen Mittelteil; Schirm 12,9; Original 24 -> 18 L* auf 4 px, Port 25,8 -> 10,5 auf 8 px) (`PS5_schwanz_brett*.json`, `PS5_schwanz_naehte.json`) | K |
| Farbe waagerecht / senkrecht | 17,9/6,1/1,3 / 15,6/3,3/1,9; a* > 12 0,099 | 15,9/5,7/1,9 dE00 **1,5** / 15,3/3,0/1,7 dE00 **0,4**; Verlauf L*/a* entlang der Achse deckt das Original an Wurzel, Mitte, Biegung, Knick, Ende (Leiste 09, TV gruen); a* > 12 Videoraum 0,020, Schirm 0,066 (Original roh 0,099) (`PS5_schwanz_farbe.json`) | K |
| Senkrechter Teil | 2 helle Baender (L* > 22) bei y 292 und 312-316 (Mitte 29,5 gegen Hintergrund links/rechts 18,4/19,3: Schwanz, nicht Wand), groesster Sprung 15,7 | Videoraum 2 helle Baender, Spruenge bei y 281/292/304, max 14,0 - wie Original. Schirm: harte Kanten (Sprung 20,5), 4 Laeufe L* > 22; sichtbar als hellgraue Ringe in `PS5_schwanz_seite_x4.png` (`PS5_schwanz_senkrecht.json`) | K |
| **Schwanz in anderen Ansichten und im Spiel (O9)** | Original-Schwanz: max L* 46,1, p99 40,8, L* > 40 1,2 %; Video h85 glatt mauve, h68 glatt rosa-rot 23,5/16,2/7,6 | Texel neutral bis L* 60 (Mesh 18) / 75 (Mesh 20). **Spiel h85** (= Vorlage-2-Zeitpunkt, andere Spielpose): weiss-grauer Zickzack-Streifen 57,2/1,8/-3,2 auf Mesh 18, Schwanz Videoraum 25,4/5,2/1,0 dE00 **5,4** (r4 2,2), L* > 40 **16,8 %**, L* > 46 10,3 %. Spiel h84/h88: 110/171 px L* > 50 (r4 0). h68/h76: rot-weiss gefleckte Kette, Schirm L* > 45 3,7/4,6 % (r4 0,0/1,0 %); h68 Farbe 16,7/3,7/1,2 gegen Video 23,5/16,2/7,6 dE00 12,5 (r4 15,1). O9 am Schwanz: Anteil > 10 dE76 zur Farbmenge des Original-Schwanzes Gier 0/60/120/180/240/300 = 0,000/0,038/0,012/0,034/0,033/0,005, Spiel h85 0,032 (r4 <= 0,005). Rundum (neutral) `PS5_schwanz_rundum.png`: Gier 240 breite weiss-graue Laengsstreifen am waagerechten Teil, L* > 45 9,5 % (`PS5_schwanz_ausreisser.json`, `PS5_schwanz_spiel_hell.json`, `PS5_schwanz_spiel_h85.json`, `PS5_schwanz_sprung*.json`, Bilder `PS5_schwanz_spiel_h85_x3.png`, `PS5_lp_vs_spiel_h85_x4.png`, `PS5_schwanz_sprung_x3.png`, `PS5_sprung_h68_h88.png`) | **A** |

### 1.4 Unterarme

| Punkt | Vorlage | V4 r5 | Klasse |
|---|---|---|---|
| Unterarm (B)-links = (W)-rechts (Id 14) | 8,8/-3,4/3,2, p90 11,1, 0 % L* > 30 | 11,4/-2,1/2,3, p10/50/90 7,0/11,3/15,6, 0 % L* > 30, dE00 2,4 (`PS5_unterarm_links.json`) | W |
| Unterarm (B)-rechts = (W)-links (Id 11) | 25,0/1,0/-2,1 | 26,9/1,5/-1,4 dE00 1,7 | W |

### 1.5 Farben je Region und Gesamteindruck vorn

| Region | Vorlage | V4 r5 | Klasse |
|---|---|---|---|
| **Rot nur in der Brustmitte** (Spielbild gegen Video, Polygone wie Runde 3/4, `PS5_oberkoerper_spiel*.json`, `PS5_oberkoerper_spiel_x3.png`) | Mitte/aussen h118 2,91, h135 2,75; Schultern a* > 12 0,00/0,05 und 0,00/0,04 | **h118 2,73, h135 2,76** (r4 1,55/1,49); Drittel h135 0,17/0,43/0,14 (Video 0,18/0,58/0,24); Schultern a* > 12 0,01/0,02 und 0,01/0,02. Sicht: die rote Zone liegt unter dem Kopf in der Brustmitte wie im Video | W |
| Kraeftiges Rot a* > 22 im Spiel | Video h85 2, h100 107 | h100/h118/h135 0 px; h85 165 px = **Leons Arm/Messer** (x 426-445, y 298-315, Id -1 bzw. vor Id 10/11 der Laufpose; in r4 und r5 identisch, `PS5_rot_spiel.json`); der Birkin selbst hat bei h85 kein a* > 22. Der Riegel FR h85 misst damit den Spieler, nicht die Figur | W |
| Brustmitte, Staerke des Rots | Mitte rot 0,55/0,58, a* > 12 0,23/0,28; F6 16,6/17,3/6,2 | Mitte rot 0,48/0,43, a* > 12 0,22/0,21; F6 19,0/13,1/5,9 dE00 3,4: Brustmitte etwas schwaecher rot | K |
| Gelb / Ausreisserfarben Koerper (O9) | kein Gelb | gelbe Texel 0 (Pose 135 und 85; r4 46/11); Anteil > 10 dE76 zur Farbmenge der Original-Figur je Id 0-15: 0,000-0,001 (Pose 135/85), Spiel h118/h135/h85 0,000 (r4 0,003-0,006) (`PS5_ausreisser_farbmenge.json`) | W |
| Grelle Einzelpixel | - | C* > 45 0 px (r4 253); blau (b* < -15) 20/17 (r4 142/43) | W |
| Koernung im Spielbild (neu gemessen) | Video glatt (Weichzeichnung) | Anteil Betrag Hochpass > 8 L*: h85 0,082 (r4 0,052), h90 0,109 (0,067), h118 0,034 (0,024), h135 0,040 (0,027); (B)-linke Schulter h118 0,212 (r4 0,082), h135 0,157 (0,065). Buehne Id 13 0,048 (Referenz 2 0,121). Sicht: Rumpf/Buckel bei h85/h90 und (B)-linke Schulter koerniger als r4 (`PS5_koernung.json`, `PS5_schulter_brust_spiel.json`, `PS5_spiel_h85_h90_x4.png`) | K |
| Oberarm (B)-rechts (Id 10) | 16,8/5,2/0,0 | 18,8/5,1/0,8 dE00 1,5 | W |
| Oberarm (B)-links (Id 13) | 17,3/3,6/1,3 | 22,7/3,5/1,4 dE00 3,7 | K |
| Rumpf (Id 8) | 13,0/4,7/2,1 | 21,8/7,5/3,2 dE00 6,7 (Box 16,8/6,8/3,4 dE00 3,5) | K |
| Jeans | 14,3/2,3/-2,4 | 15,8/0,6/-2,4 dE00 2,6 | W |
| Unterschenkel (Box) | 14,7/1,6/-1,4 | 15,8/0,4/-2,6 dE00 2,2 | W |
| Seite: Buckel-Glanz F14 | 60,0/11,0/1,2 | 57,4/9,2/1,6 dE00 2,9 | W |

### 1.6 Gang

| Punkt | Soll | Ist | Klasse |
|---|---|---|---|
| T7 koerperfest | Gelenke <= 0,06 H | 0/47 Takte, max 0,056 H; Biegung 69,5-76,8 Grad (Riegel, Leiste 07) | W |
| Schwanz im Gang/Sprung | - | s. 1.3 letzte Zeile (helle Baender h76-h90) | (A, s. 1.3) |

## 2. Was den Nutzer-Eindruck bestimmt

1. **Seitenansicht wie Vorlage 2 (Leiste 02/05/09)**: der Schwanz ist jetzt ein organischer, dunkel-mauve Koerperteil, hell an der
   Biegung, stetig in den Ruecken; Linie, Knick und Spitze stimmen. Das ist der grosse Fortschritt gegen r4.
2. **Im Spiel** sieht der Nutzer den Sprung nicht in der Buehnen-Pose: genau am Vorlage-2-Zeitpunkt (h85) und davor (h68-h88) zeigt der
   Schwanz weiss-graue Ringe/Streifen und eine rot-weiss gefleckte Kette - Farben, die der Original-Schwanz nirgends hat (hellster
   Original-Schwanzpixel L* 46). Der Nutzer hat am Schwanz Farbe und gemalte Ringe schon einmal abgelehnt.
3. Vorn: Rot liegt jetzt in der Brustmitte, kein Gelb, keine grellen Pixel; Kopf, Auge, Haare, Nase wie Vorlage. Die Oberflaeche ist im
   Spiel koerniger als r4 (Schulter (B)-links 2,5 x).

## 3. Empfehlung an den Bauer

- Schwanz-Texel auf die Farbmenge des Original-Schwanzes begrenzen (L* <= 46, besser p99 40,8; Abstand jedes Texels zur Farbmenge
  <= 10 dE76) - das ist O9 fuer die Ids 16-22. Die hellen Stellen der Vorlage 2 (Biegungsglanz, zwei Baender senkrecht) nur auf der in
  Vorlage 2 sichtbaren Seite des Umfangs lassen bzw. als Licht statt als Texel; die abgewandte Seite aus den dunklen Farben der Vorlage.
- Riegelvorschlag: AF auf Ids 16-22 ausdehnen (Farbmenge = Leser-Maske `sfv4/schwanz_maske.npy`), gemessen in Gier 0/60/.../300
  (beleuchtet) und im Spielbild h76-h90 (Schwanzmaske aus der Laufpose, `pruefung_sicht_r5/laufposen_ids.py`); dazu Schwanz im Spielbild
  h85 dE00 <= 3 zum Original-Schwanz und L* > 40 <= 2 x Original (1,2 %).
- Riegel FR h85: Leons Arm aus dem Fenster nehmen (x > 420 bei y 290-320) oder nur Figurpixel der Laufpose zaehlen.
- Erhalten: waagerechter Teil (Textur entlang der Achse, Verlauf, Wurzel), T1-T4, Rot-Verteilung vorn, kein Gelb, Kopf/Auge/Haare/Nase,
  Unterarme, Jeans, Gang.
- Klein: T5 f 0,5 (0,098 H) und Breite um den Knick, Quernaht am Biegungsring (10,7 gegen 9,5), Kopfkuppe seitlich wieder roter
  (S4 a* 11,4 gegen 15,3), Koernung der (B)-linken Schulter im Spiel.

## 4. Belege (`C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/`, untracked)

- Schwanz Spiel/Rundum (Klasse A): `PS5_schwanz_spiel_h85_x3.png` (Video h85 | Spiel r4 | Spiel r5), `PS5_lp_vs_spiel_h85_x4.png`
  (Spielbild r5 | Laufpose gezeichnet, Zuordnung Mesh 18), `PS5_sprung_h68_h88.png`, `PS5_schwanz_sprung_x3.png`,
  `PS5_spiel_h85_h90_x4.png`, `PS5_schwanz_rundum.png`, `PS5_schwanz_rundum.json`, `PS5_schwanz_ausreisser.json`,
  `PS5_schwanz_spiel_hell.json`, `PS5_schwanz_spiel_h85.json`, `PS5_schwanz_sprung.json`, `PS5_schwanz_sprung_ringe.json`
- Schwanz Buehne: `PS5_schwanz_seite_x4.png` (Vorlage 2 | r4 | r5 | r5 neutral), `PS5_seite_x2.png`, `PS5_schwanz_naehte.json`,
  `PS5_schwanz_farbe.json`, `PS5_schwanz_brett.json`, `PS5_schwanz_brett_kennzahlen.json`, `PS5_schwanz_senkrecht.json`,
  `PS5_schwanz_silhouette.json`, `PS5_schwanz_silhouette_x4.png`
- Oberkoerper/Rot/Gelb: `PS5_oberkoerper_spiel_x3.png` (Video | r4 | r5, h118/h135), `PS5_oberkoerper_spiel.json`,
  `PS5_oberkoerper_spiel_drittel.json`, `PS5_oberkoerper_vorn.json`, `PS5_rot_spiel.json`, `PS5_gelb_ausreisser.json`,
  `PS5_ausreisser_farbmenge.json`, `PS5_koernung.json`, `PS5_schulter_brust_spiel.json`, `PS5_koerper_ref2_r4_r5.png`
- Kopf/Arme: `PS5_kopf_spiel_x4.png`, `PS5_kopf_struktur_spiel.json`, `PS5_kopf_struktur_spiel_innen.json`, `PS5_kopf_textur_vorn.json`,
  `PS5_kopf_seite.json`, `PS5_auge_spiel.json`, `PS5_unterarm_links.json`, Regionen-Rohwerte `PS5_messen.txt`
