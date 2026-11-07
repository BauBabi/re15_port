# SF V4 - Pruefung Runde 3 (Sicht): V4 r3 gegen die Nutzer-Beschriftung Beschreibung1/2

Pruefer "Auge des Nutzers", 2026-11-07, Worktree r41_sichel (Zweig r41/sichel_nb2, HEAD ef41724b7), nichts committet.
Gegenstand: EMD `sfv4/emd/EM30_V4.EMD` sha256 dfbc30bc... (= `SF_belege/v4/emd_sha256.txt`, V4 r3), Vergleich V4 r2
`sfv4/emd/EM30_V4r2.EMD` sha256 174b7879..., Leisten `SF_belege/v4/01..08` (r3) und `SF_belege/v4/r2/01..08`, Spiellauf
`sfv4/r/V4r3` (RE15_FRAMEDUMP, registriert ueber `c6.port`, Versatz 243 wie V4r2). Nutzerbilder `Version_80%/Beschreibung1/2(_Vorlage).png`
unveraendert (mtime 2026-11-06 21:46-21:57). Spiel-Asset `EM30_HANGAR.EMD` sha256 cfd1ac42... = Pin (unveraendert).

**Ergebnis: NICHT bestanden.** Drei Punkte gehoeren in die dritte Klasse:

1. **Schwanz, waagerechter Teil = Balken mit geraden Kanten** (Beobachtung (ii) des Orchestrators bestaetigt): jedes Schwanzglied
   ist ein vierseitiges Prisma (10 Flaechen, 40 Ecken je Glied; Referenz 2: 50 Flaechen / 120 Ecken); im Seitenbild eine helle graue
   Deckflaeche und eine einheitlich weinrote Seitenflaeche mit harter Kante (Querprofil-Stufe 16,6-19,9 L* auf 3 px, Original 5,4-9,6),
   gerade Oberkontur (0,6 px RMS zur Geraden, Original 3,9 px) und ein Dorn mit 27 Grad an der Bogenecke (272,233). Im Spiel (h85/h90)
   ragt der Schwanz als grauer Klotz bzw. weinrotes Brett hinter dem Ruecken auf.
2. **Rot-Verteilung des Oberkoerpers vorn** (Beobachtung (i) bestaetigt, im Spielbild gemessen): im Video liegt das Rot in der
   Brustmitte und die Schultern sind rosa-grau ohne kraeftiges Rot (Rotanteil Mitte / aussen 2,75-2,91; Anteil a* > 12 auf den
   Schultern 0,00-0,05). V4 r3 verteilt das Rot gleichmaessig ueber Schultern und Oberarme (Mitte / aussen 1,06-1,18; Schultern
   a* > 12 0,10-0,13) und hat 4,4-5,7 x so viele kraeftig rote Pixel wie das Video (h100/h118). r3 ist hier schlechter als r2 (1,36-1,50).
3. **Kopf vorn im Spiel weiter eine glatte Kuppel**: auf der Buehne erreicht K11 jetzt 36,2 (Band 34,9-47,1), im Spielbild aber hat
   die Innenflaeche des Kopfs 27,2 / 26,9 (h135/h118) Gradient gegen 32,9 im Original (Fensterspanne 29,0-40,4) und 1,7-2,7 x so
   viele glatte Pixel; der Rumpf daneben trifft das Original (25,3 / 24,4 gegen 26,2). Sichtbar: einheitlich mauve-graue Kuppel mit
   weichem hellem Fleck; neu sind nur rote/dunkle Striche auf der Kappe.

Bestanden haben: Auge/Haare/Nase vorn (Lage und Farbe), Auge seitlich, Schwanz Beginn/Biegung/Knick/Verbreiterung/Spitze (Linie
und Mass), Unterarm (W)-rechts (dunkel, keine weissen Flecken mehr), Unterarm (W)-links, Jeans, Kopfmittelfarbe. Das darf nicht
verloren gehen.

Konventionen: (B) = aus Sicht des Betrachters, (W) = aus Sicht des Wesens. Lab = CIE-Lab D65, dE00 = CIEDE2000. "Videoraum" =
Vergleichsregel Messblatt V4 (Gauss sigma 1,5, V = a + b x P). "Schirm" = Port-Bild ohne diese Abbildung. Vorlage 1 entblendet
(k 0,766/0,764/0,768), Vorlage 2 roh; Video h100/h118 roh und nicht in der Blende (Wand links 46,2/46,3 L* gegen Spiel 48,2; bei
h135 37,2 = noch eingeblendet, daher dort Vorlage 1 entblendet). H1 = 302 px, H2 = 253 px, H_port 364 (Pose 135) / 264 (Pose 85).

Werkzeuge (neu, untracked): `re15_port/tools/r41_sichel/v4/pruefung_sicht_r3/` - `render_r3.py` (Buehne V4 r3 / V4 r2 / REF2,
Posen 135/85, Hash geprueft: dfbc30bc / 174b7879 / cfd1ac42), `spiel_r3.py` (Spielbilder V4r3/V4r2 h85-h135, Video h85/h100/h118/h135),
`messen_r3.py` (Regionen), `oberkoerper_vorn.py`, `oberkoerper_spiel.py` (Beobachtung (i)), `rotkappe_spiel.py`,
`kopf_struktur_spiel.py`, `kopf_textur_vorn_r3.py`, `kopf_seite_r3.py`, `auge_spiel_r3.py`, `schwanz_silhouette_r3.py`,
`schwanz_brett.py` (Beobachtung (ii)), `tafel_schwanz.py`, `unterarm_links_r3.py`. Ausgaben `sfv4/sicht_r3/`, Belege `SF_belege/v4/PS3_*`.
Polygone/Kreise im Spielbild sind von Hand auf Koordinatengittern gesetzt (`sfv4/sicht_r3/grid_*.png`) und in den Skripten notiert.

## 1. Befunde Punkt fuer Punkt

Klassen: **W** = wie Vorlage, **K** = Abweichung klein, **A** = Abweichung, die der Nutzer wieder ablehnen wuerde.

### 1.1 Kopf vorn (Beschreibung1, Kamera 50A0/12)

| Punkt | Vorlage (gemessen) | V4 r3 (gemessen) | Klasse |
|---|---|---|---|
| Auge, Lage | Nutzerpunkt (382,165), (B)-links der Kopfmitte | K4 0,060 H links / 0,107 H unter OK (Soll 0,035-0,065 / 0,07-0,12) | W |
| Auge, Farbe | 46,9/7,2/-1,1, Sprung Kern/Ring +45,0 | Buehne K2 46,6/7,7/-1,5 dE00 0,7; Spiel h135 Kern 127,8 / Ring 81,9 = +45,9 (`PS3_auge_spiel.json`) | W |
| Auge, Groesse | 52 px, 0,026 x 0,033 H1 | Buehne K3 0,030 x 0,036 H OK; **Spiel h135 130 px, 15 x 16 px = 0,041 x 0,044 H, Flaeche auf H normiert 1,72 x Original** (r2 2,14 x) | K |
| Haare | (403,160) 13,3/7,4/1,5 | K5 14,3/7,3/1,3 dE00 0,7 | W |
| Nase | 18,2/7,1/1,3 (als Struktur nicht aufloesbar) | K7 20,1/6,8/1,7 dE00 1,4 | W |
| Kopf-Oberseite rot (Buehne) | KV1 22,6/15,4/5,2 | K9 20,6/16,5/5,7 dE00 1,6 | W |
| Kopf-Mittelfarbe | Box 20,1/8,5/2,1 | Id 9 Videoraum 20,5/8,5/1,9 dE00 0,3 | W |
| Kinn/Maulzone | 17,1/11,6/4,4 | 15,9/8,1/3,3 dE00 3,3 | K |
| **Kopf-Oberflaeche (Struktur)** | Sobel-Gradient Kopfbox 41,2 (Fenster +-3 px 39,4-42,9), Rumpf 26,2, Kopf/Rumpf 1,57. **Innenflaeche** (Box 376-408 x 148-182, ohne Kopfumriss) 32,9 (Fenster +-3 px 29,0-40,4), glatte Pixel (Gradient < 8) 0,074 | Buehne (Videoraum): Kopf 36,2, Rumpf 32,1, 1,13 (r2 27,8 / 0,86) - im K11-Band. **Spielbild** (Videoraum, Kreis um die Kuppel, `PS3_kopf_struktur_spiel*.json`): ganzer Kreis h135 37,0 / h118 **29,4** (0,71 x Original, unter dem K11-Band 34,9); **Innenflaeche h135 27,2 / h118 26,9 - unter der Originalspanne 29,0** (r2 29,6 / 23,4); glatte Pixel **0,197 / 0,126 = 2,7 / 1,7 x Original**; Rumpf daneben 25,3 / 24,4 = Original (26,2) - die Rauschlage ist vergleichbar, nur der Kopf ist glatter. Im Spielbild (`PS3_kopf_spiel_x4.png`, Zeilen h118/h135) ist die Kuppel wie in r2 einheitlich mauve-grau mit weichem hellem Fleck; neu sind dunkle und rote Striche auf der Kappe. Bei h100 (Kopf von oben) zeigt r3 statt der flachen Flaeche von r2 rosa-rote Zacken | **A** |

### 1.2 Kopf seitlich (Beschreibung2, Pose 85)

| Punkt | Vorlage | V4 r3 | Klasse |
|---|---|---|---|
| Auge seitlich | (459,235), 48,4/4,0/0,5 | S1 48,0/3,9/0,8 dE00 0,5; 0,030 x 0,049 H; 1 Komponente | W |
| Augenlage | 0,03-0,07 H hinter der Vorderkante, 0,76-0,82 H ueber Sohle | S2 0,058 H; 0,811 H | W |
| Haare seitlich | (467,241), 19,1/1,2/1,1 | S3 14,1/2,3/0,6 dE00 3,7, Abstand 0,015 H (= 4 px; r2 0,011 H; Riegel ROT) | K |
| Kopf seitlich Farbe | Box 25,5/6,4/2,2 | Id 9 27,2/6,9/1,7 dE00 1,4 | W |
| Kopf seitlich Flaeche/Form | Gradient Kopfbox 412-467 x 190-263: 46,3, glatt 0,103 | Id 9 47,8, glatt 0,093 (`PS3_kopf_seite.json`); Form weiter ein Keil mit gerader Oberkante (`04_kopf_nah_seite_x4.jpg`) | K |
| Kopf-Oberseite rot seitlich | KS1 27,6/15,3/3,3 | S4 27,6/15,4/2,9 dE00 0,3 | W |

### 1.3 Schwanz (Beschreibung2)

| Punkt | Vorlage | V4 r3 | Klasse |
|---|---|---|---|
| Beginn in den Ruecken ueberfuehrend | (357,243), 0,15 H hinter Scheitel, 0,767 H ueber Sohle | T1 0,172 / 0,742 H; TA <= 0,022 H | W |
| Biegung | Pruefer-Mittellinie 65,8/69,5/70,1 Grad (Ziel 68,4 +-5) | T2 70,6 Grad (r2 52,5) | W |
| Knick (Taille) | (290,308) | T3 dx -0,248 dy 0,265; Taille 0,021 H | W |
| von da an breiter nach oben | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H | T5 0,028/0,080/0,140/0,131 H | W |
| Spitze Ende | (294,344), 0,37 H ueber Sohle | T4 0,353 H ueber Sohle | W |
| Farbe Achse | 7 Punkte (5x5, roh), Mittel L* 11,6 | Videoraum dE00 1,8/2,4/4,6/4,2/0,7/1,9/2,5 (`PS3_messung_regionen.json`) | W |
| Umriss | Leser-Maske 4452 px | IoU 0,514 (r2 0,532), 70 % der Originalmaske gedeckt; Oberkontur x 276-292: y 237/235/232/230/228 gegen 251/250/243/236/234 = 6-16 px (bis 0,06 H2) ueber dem Original (`PS3_schwanz_silhouette.json`) | K |
| **Form waagerechter Teil (Beobachtung ii)** | Querprofil L* bei x 310/320/330/340 (roh, in der Leser-Maske): stetiger Abfall von hell oben nach dunkel unten, groesste Stufe auf 3 px **5,4 / 7,1 / 6,2 / 9,6**, Plateau-Anteil (Betrag dL/dy < 0,3) 0,10-0,31; Oberkontur x 284-332 gekruemmt: **3,9 px RMS** zur Geraden (max 9,7); Douglas-Peucker 10 Ecken auf dem Bogen, 121-147 Grad | Videoraum: zwei Flaechen mit harter Kante - Deckflaeche 38-42 L* (Plateau), Seitenflaeche 11 L* (Plateau), **Stufe auf 3 px 16,6 / 17,7 / 19,9 / 19,0** (2-3 x Original), Plateau-Anteil 0,33-0,43; Oberkontur **0,6 px RMS** (max 2,0) = eine Gerade von (380,191) bis (276,237) mit einem einzigen Knick 166 Grad; **Dorn 27 Grad bei (272,233)** (r2: 7 Grad bei (281,223)). Ursache im Modell: jedes Glied 16-22 ist ein **vierseitiges Prisma** (10 Flaechen, 40 Ecken; Dicke/Breite 0,80-1,06 - also kein duennes Brett, sondern ein Vierkant-Balken), Referenz 2 hat je Glied 50 Flaechen / 120 Ecken (`PS3_schwanz_brett.json`). Im Spiel h90 ein grauer Klotz hinter dem Ruecken, h85 ein weinrotes Brett (`PS3_spiel_seite_h85_h110.png`, `PS3_schwanz_brett.png`, `08_rundum_V4.jpg` Gier 180-300) | **A** |
| Farbe Zonen | F15-F18 | T8 dE00 1,9/2,9/0,7/1,5 | W |

### 1.4 Unterarme

| Punkt | Vorlage | V4 r3 | Klasse |
|---|---|---|---|
| Unterarm (B)-links = (W)-rechts (Id 14) | Box 298-325 x 262-340: 8,8/-3,4/3,2, L* p10/50/90 5,7/8,8/11,1, 0 % L* > 30 | Id 14 Videoraum 11,4/-1,9/2,2, p10/50/90 6,9/11,4/15,6, **0 % L* > 30, dE00 2,6** (r2 28,1 / 35 % / 15,7). Im Spielbild h118/h135 dunkel, keine weissen Flecken mehr (`PS3_unterarm_spiel_x3.png`); Textur dunkel-oliv mit einzelnen roten Punkten | W |
| Unterarm (B)-rechts = (W)-links (Id 11) | 25,0/1,0/-2,1 | 26,8/1,7/-1,5 dE00 1,8 | W |

### 1.5 Farben je Region und Gesamteindruck vorn ("EXAKT seine Farben")

| Region | Vorlage | V4 r3 | Klasse |
|---|---|---|---|
| **Gesamteindruck Oberkoerper vorn (Beobachtung i)** - Spielbild gegen Video, Polygon um Buckel/Schultern/Brust je Bild (`PS3_poly_spiel_h118.png`, `PS3_poly_spiel_h135.png`), Drittel (B)-links / Mitte / (B)-rechts (`PS3_oberkoerper_spiel*.json`) | Mittel h118 20,4/5,4/2,7, h135 22,1/5,7/3,0. Rotanteil (R-G >= 15) je Drittel h118 0,23/**0,55**/0,14, h135 0,18/**0,58**/0,24 -> **Mitte/aussen 2,91 / 2,75**; Anteil a* > 12 auf den Schultern 0,00/0,05 (h118) und 0,00/0,04 (h135). Im Bild rosa-graue Schultern mit dunklen Adern, Rot in der Brustmitte | Videoraum-Mittel trifft (dE00 1,4 / 0,2; L* 21,2 / 22,0), Rosa-Grau-Anteil (L* > 25, C* < 12) auch (0,25 gegen 0,25 bei h118). Aber Rotanteil je Drittel h118 0,35/0,44/0,40, h135 0,33/0,38/0,39 -> **Mitte/aussen 1,18 / 1,06** (r2 1,50 / 1,36); Schultern a* > 12 **0,13/0,10** und **0,12/0,11**; kraeftig rote Pixel (a* > 18, ganzes Bild, Videoraum) h100 1063 gegen Video 186, h118 1056 gegen 241 (5,7 / 4,4 x), a* > 22 623 gegen 107 bzw. 394 gegen 0 (`PS3_rot_spiel.json`, Uebersicht in sfv4/sicht_r3). Gleiche Aussage auf der Buehne Pose 135 gegen Vorlage 1: Rotanteil je Drittel Original 0,18/0,58/0,24, V4 r3 0,31/0,47/0,47 (`PS3_oberkoerper_vorn.json`). Auf dem Schirm zusaetzlich fast schwarze Risslinien: L* p10 3,3 gegen Video 7,5 - das ist der "dunklere" Eindruck; im Videoraum ist der Oberkoerper im Mittel nicht dunkler. Sicht (`PS3_oberkoerper_spiel_vs_video.png`): Original blass rosa-grau mit roter Brustmitte, V4 r3 auch nach der Video-Unschaerfe weinrot gefleckt bis in die Schultern | **A** |
| Oberarm (B)-rechts (Id 10) | 16,8/5,2/0,0, Rotanteil 0,25 | 19,5/8,5/2,7 dE00 4,4, **Rotanteil 0,66** (r2 20,8/5,7/1,2, 0,29) - Teil des Befunds oben (Riegel "FS Oberarm (B)-rechts unten" ROT, Dossier Offen 3) | K (geht in A oben ein) |
| Oberarm (B)-links (Id 13) | 17,3/3,6/1,3, Rotanteil 0,12 | 21,8/5,6/1,3 dE00 3,9, Rotanteil 0,34 | K (geht in A oben ein) |
| Rumpf (Id 8) | 13,0/4,7/2,1, Rotanteil 0,31 | 18,8/7,2/2,9 dE00 4,7, Rotanteil 0,39 | K |
| Sichel (B)-links (Id 15) | Pfad 14,3/-2,2/1,4 (oben 5-10, unten 22-26 L*) | 24,2/-2,1/4,1 dE00 7,3, einheitlich oliv-grau (wie r2) | K |
| Jeans | 14,3/2,3/-2,4 | 15,6/1,0/-2,7 dE00 2,0 | W |
| Unterschenkel | 14,7/1,6/-1,4 | Id 17,9/4,6/0,7 dE00 4,7 | K |
| Seite: Buckel-Glanz F14 | 60,0/11,0/1,2 | Riegel 57,0/10,8/1,6 dE00 2,8 | W |

### 1.6 Gang

| Punkt | Soll | Ist | Klasse |
|---|---|---|---|
| T7 koerperfest | Gelenke <= 0,06 H vom Ziel | Riegel 0/47 Takte, max 0,042 H; Boden -4 E. (`07_gang.jpg`) | W |
| Schwanz im Spielbild | - | sichtbar nur h85/h90 (Ruecken zur Kamera, Szene vorbestehend), dort als Brett/Klotz - geht in 1.3 "Form" ein | - |

Hinweis ohne Bewertung (Szene, vorbestehend): bei h118 ist die Figur im Spielbild groesser als im Video (Buckelbreite ~200 px
gegen ~155 px, `sfv4/sicht_r3/grid_118.png`), bei h135 aehnlich (~230 gegen ~208 px).

## 2. Was den Nutzer-Eindruck bestimmt

1. Seitlich sieht man einen **Vierkant-Balken** statt eines organischen Schwanzes: flache graue Deckflaeche, flache weinrote Seite,
   gerade Kanten, Dorn an der Ecke. Linie, Knick, Breite und Spitze stimmen - aber "EXAKT diese Umsetzung" heisst auch ein runder
   Koerper wie im Original (stetiger Helligkeitsverlauf quer, gekruemmte Oberkante).
2. Von vorn wirkt V4 r3 **weinrot gefleckt bis in die Schultern**; das Original ist blass rosa-grau mit einer roten Brustmitte. Die
   Mittelwerte passen, die Verteilung nicht: das Rot gehoert in die Mitte (Faktor 2,75-2,91), nicht gleichmaessig ueber alles
   (1,06-1,18). r3 hat den (B)-rechten Oberarm zusaetzlich rot gemacht (Id 10 mit der F8-Verteilung).
3. Der **Kopf** ist im Spiel weiter die glatte mauve Kuppel; die Struktur aus Runde 3 wirkt auf der Buehne (K11), im Spielbild kaum.

## 3. Empfehlung an den Bauer

- Schwanz: Glieder 16-22 mit rundem Querschnitt (mehr Seiten, wie Referenz 2) und weicher Schattierung; Ziel Querprofil-Stufe auf
  3 px <= 9,6 L* (Originalmaximum), Oberkontur x 284-332 gekruemmt (RMS zur Geraden in der Groesse des Originals, 3,9 px), Oberkante
  bei x 276-292 auf y 243-251 statt 228-237; Dorn bei (272,233) entfernen. Linie/Knick/Breite/Spitze (T1-T6, TA) halten.
- Oberkoerper: Rot in die Brustmitte, Schultern/Oberarme rosa-grau. Riegelvorschlag aus diesem Bericht: Rotanteil Mitte/aussen
  >= 2,75 (Originalspanne 2,75-2,91, Spielbild h118/h135; Buehne Original 2,76), Schultern a* > 12 <= 0,05; Id 10 nicht ganz mit der
  F8-Verteilung (Konflikt F8-Fenster / Sicht-Box, Dossier Offen 3 - Messblatt-Entscheid noetig).
- Kopf: Struktur im SPIELBILD messen (FRAMEDUMP h118/h135, Videoraum): Innenflaeche Gradient in der Originalspanne 29,0-40,4,
  glatte Pixel nahe dem Original (0,074); Augenfleck im Spielbild auf <= 0,036 x 0,043 H (K3-Band).
- Erhalten: Auge/Haare/Nase vorn, Auge seitlich, Unterarm Id 14, T1-T7.

## 4. Belege (`SF_belege/v4/`)

- `PS3_oberkoerper_spiel_vs_video.png` (Video h118/h135 | Spiel V4 r3 Videoraum | Spiel V4 r3 Schirm), `PS3_poly_spiel_h118.png`,
  `PS3_poly_spiel_h135.png`, `PS3_oberkoerper_spiel.json`, `PS3_oberkoerper_spiel_drittel.json`, `PS3_oberkoerper_vorn.json`, `PS3_rot_spiel.json`
- `PS3_kopf_spiel_x4.png` (Video | Spiel r2 | Spiel r3, h100/h118/h135), `PS3_kopf_struktur_spiel.json`, `PS3_kopf_struktur_spiel_innen.json`,
  `PS3_kopf_textur_vorn.json`, `PS3_kopf_seite.json`, `PS3_auge_spiel.json`
- `PS3_schwanz_brett.png` (Vorlage 2 x4 | V4 r3 x4 | Spiel h90 x3 | Querprofil x 320), `PS3_schwanz_brett.json`,
  `PS3_schwanz_brett_kennzahlen.json`, `PS3_schwanz_silhouette_x4.png`, `PS3_schwanz_silhouette.json`, `PS3_spiel_seite_h85_h110.png`
- `PS3_unterarm_spiel_x3.png`, `PS3_unterarm_links.json`, `PS3_messung_regionen.json`
