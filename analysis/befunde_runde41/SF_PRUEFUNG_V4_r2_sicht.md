# SF V4 - Pruefung Runde 2 (Sicht, Wiederholung): V4 r2 gegen die Nutzer-Beschriftung Beschreibung1/2

Pruefer "Auge des Nutzers", 2026-11-07, Worktree r41_sichel (Zweig r41/sichel_nb2, HEAD ee470c381), nichts committet.
Gegenstand: EMD `sfv4/emd/EM30_V4.EMD` sha256 174b7879... (= `SF_belege/v4/emd_sha256.txt`), Leisten `SF_belege/v4/01..08`,
Riegel `riegel_V4r2.md`, Spiellauf `sfv4/r/V4r2` (RE15_FRAMEDUMP, registriert ueber `c6.port`), gegen
`Version_80%/Beschreibung1/2(_Vorlage).png` (= Videobilder n1485 / n1435, h = n - 1350) und das rohe Video 4.
Spiel-Asset `EM30_HANGAR.EMD` sha256 cfd1ac42... = Pin `release/em30_hangar.sha256` (unveraendert).

**Ergebnis: NICHT bestanden.** Zwei Punkte gehoeren in die dritte Klasse: (1) der Kopf vorn ist im Spiel eine glatte,
flaechig gefaerbte Kuppel mit roter Kappe (h100: eine flach schattierte rot-rosa Flaeche) - im Original ist der Kopf die
am staerksten strukturierte Stelle der Figur; der Augenfleck ist im Spielbild doppelt so gross wie im Original; (2) der
(B)-linke Unterarm (= (W)-rechts) traegt einen weissen Fleck, wo das Original an derselben Stelle kein helles Pixel hat.
Alles, was die Beschriftung nennt (Auge/Haare/Nase vorn, Auge/Haare seitlich, Schwanz Beginn/Knick/Verbreiterung/Ende),
liegt an der richtigen Stelle und hat die richtige Farbe - diese Punkte sind "wie Vorlage" oder klein abweichend.

Konventionen: (B) = aus Sicht des Betrachters, (W) = aus Sicht des Wesens. Lab = CIE-Lab D65, dE00 = CIEDE2000.
"Videoraum" = Vergleichsregel des Messblatts V4 (Port-Schirm, Gauss sigma 1,5, V = a + b x P). "Schirm" = Port-Bild ohne
diese Abbildung (was der Nutzer sieht). Vorlage 1 entblendet (k 0,766/0,764/0,768, Messblatt V4), Vorlage 2 roh.
H1 = 302 px, H2 = 253 px (Messblatt V4), H_port 364 (Pose 135) / 264 (Pose 85).

Werkzeuge: Renders/Spielbilder/Regionen des abgebrochenen ersten Laufs dieser Pruefung
`re15_port/tools/r41_sichel/v4/pruefung_sicht_r2/*.py` (Ausgaben `sfv4/sicht_r2/`, erzeugt nach Commit ee470c381, EMD-Hash
174b7879 geprueft) und neu `re15_port/tools/r41_sichel/v4/pruefung_sicht_r2b/*.py` (Ausgaben `sfv4/sicht_r2b/`). Ein Wert
des ersten Laufs ist ungueltig und nicht verwendet: `auge_vorn_spiel_*` in `kopf_schwanz_r2.json` fand die Lampe
(Lum 213 bei (328,148)), nicht das Auge; ersetzt durch `pruefung_sicht_r2b/auge_spiel.py`. Belege: `SF_belege/v4/PS2_*`.

## 0. Linienlesung

Die vier Nutzerbilder sind seit 2026-11-06 21:57 unveraendert (mtime), die Lesung der Runde 1 (`P1S_linien.json`, Farbe
(237,28,36), Maske = Beschreibung - Vorlage, Belegung je Permutation) gilt weiter: Bild 1 Auge (382,165) [Haken; 0,387
gerade gegen 0,372], Haare (403,160) [1,000 gegen 0,946], Nase (394,177) [1,000 gegen 0,349]; Bild 2 Auge seitlich
(459,235) [1,000 gegen 0,548], Haare seitlich (467,241) [1,000 gegen 0,608], Schwanzspitze Ende (294,344) [1,000 gegen
0,300], Schwanz Beginn (357,243) [1,000 gegen 0,299], Schwanz Knick (290,308) [1,000 gegen 0,208]. Der Bauer steht auf
denselben Punkten (Riegel TA: Beginn (360,236), Knick (290,300), Ende (296,340)).

## 1. Befunde Punkt fuer Punkt

Klassen: **W** = wie Vorlage, **K** = Abweichung klein, **A** = Abweichung, die der Nutzer wieder ablehnen wuerde.

### 1.1 Kopf vorn (Beschreibung1, Kamera 50A0/12, Pose 135 bzw. Spiel h135)

| Punkt | Vorlage (gemessen) | V4 r2 (gemessen) | Klasse |
|---|---|---|---|
| Auge, Lage | Nutzerpunkt (382,165), (B)-links der Kopfmitte = (W)-rechts | Riegel K4: 0,059 H links, 0,107 H unter OK (Soll 0,035-0,065 / 0,07-0,12) | W |
| Auge, Farbe/Kontrast | Fleck im rohen Video n1485 (377,167): Kern 3x3 Lum 102,5 / Ring 15x15 57,5 = **+45,0**; Lab 46,9/7,2/-1,1 | Buehne (333,191): +47,4; Spiel h135 (345,204): Kern 123,7 / Ring 74,8 = **+48,9**; K2 46,7/7,5/-0,6 dE00 0,5 | W |
| Auge, Groesse | Blob (Schwelle Mitte Kern/Ring) **52 px, 8 x 10 px = 0,026 x 0,033 H1** | Buehne 86 px, 12 x 15 px = 0,033 x 0,041 H (Riegel K3 gruen); **Spiel h135 162 px, 17 x 16 px = 0,047 x 0,044 H_port** - ausserhalb des K3-Bandes (max 0,036 x 0,043), Flaeche auf H normiert 2,1 x Original (`PS2_auge_spiel_b.json`, `PS2_kopf_vorn_x8.png`) | K (der Riegel misst die Buehne, das Spielbild zeigt den Fleck doppelt so gross; geht in die A-Bewertung der Kopf-Oberflaeche ein) |
| Haare | (403,160): Haarfeld 13,3/7,4/1,5, 17 L* dunkler als das Auge | K5 14,4/7,2/1,7 dE00 0,8 | W |
| Nase | (394,177): 18,2/7,1/1,3; als Struktur nicht aufloesbar | K7 20,2/6,8/1,0 dE00 1,4; keine Struktur eingetragen | W (nicht aufloesbar) |
| Kopf-Oberseite rot | KV1 22,6/15,4/5,2 | K9 21,3/16,1/5,5 dE00 1,1 | W |
| Kinn/Maulzone (nicht beschriftet) | 17,1/11,6/4,4 (Box 370-412 x 183-206) | 16,2/7,9/3,3 dE00 3,4; dunkelroter Mundspalt mit rosa Fleck auf der Buehne (349-362 x 204-216), im Videoraum nur ein dunkler Fleck | K |
| **Kopf-Oberflaeche (Struktur)** | Lum-Gradient (Sobel) im Kopf **41,2**, im Rumpf 26,2: der Kopf ist **1,57 x so strukturiert wie der Rumpf** (Boxen 368-416 x 139-191 / 352-448 x 196-292, 2 px erodiert) | nach derselben Unschaerfe (Videoraum): Kopf **27,8**, Rumpf 32,4 = **0,86**; der Kopf hat 67 % der Struktur des Original-Kopfs, der Rumpf 124 % (`PS2_kopf_textur_vorn.json`; Referenz 2: 47,4 / 37,0 = 1,28). Im Spielbild h135/h118 eine glatte dunkel-mauve Kuppel mit flacher roter Kappe und einem weichen hellen Fleck unten-links; **in h100 eine flach schattierte rot-rosa Flaeche ohne Flecken** auf dem fleckigen Koerper (`PS2_spiel_vs_video.png` Zeile 3, `PS2_spiel_h85_h100.png` rechts). Das Video zeigt in keinem dieser Bilder einen vom Koerper abgesetzten glatten Kopf | **A** |
| Heller Fleck neben dem Kopf (Koerpertextur) | - | die hellste Stelle der Kopfgegend im Spielbild h135 ist nicht das Auge, sondern ein grauweisser Texturfleck mit dunklen Punkten (B)-links oben neben dem Kopf bei (332,178): Kern Lum 154,6 / Ring 74,4 (Auge: 123,7); wirkt im x8-Bild wie ein kleines Gesicht (`PS2_kopf_vorn_x8.png`, `03_kopf_nah_vorn_x4.jpg` V4 oben links) | K (Koerpertextur, nicht beschriftet; kann als zweites Auge gelesen werden) |

### 1.2 Kopf seitlich (Beschreibung2, Pose 85 auf der Buehne)

| Punkt | Vorlage | V4 r2 | Klasse |
|---|---|---|---|
| Auge seitlich | (459,235), heller Fleck 48,4/4,0/0,5 | S1 47,8/3,8/0,9 dE00 0,8; 0,034 x 0,049 H; 1 Komponente (r1: 5) | W |
| Augenlage | 0,03-0,07 H hinter der Vorderkante, 0,76-0,82 H ueber Sohle | S2 0,058 H; 0,811 H | W |
| Haare seitlich | (467,241), Haarkante 19,1/1,2/1,1 | S3 13,8/2,0/1,0 dE00 3,7, Abstand 0,011 H (Riegel ROT knapp: Soll Abstand <= 0,01 H) | K |
| Kopfform / Dunkelanteil | Box 412-467 x 190-263: L* < 12 in 3,8 %; Box 440-474 x 205-255: 1,8 %; runder, fleckiger Kopf | Spaltenhoehe 46 px (x450) -> 31 (x477) -> 24 (x480): Keil deutlich flacher als r1 (18-21 px); im Videoraum L* < 12 in **8,6 %** (2,3 x); auf dem Schirm 16,9 % L* < 8; flache, kaum texturierte Flaechen: Gradient < 4 auf 13,1 % des Kopfs gegen 3,7 % des Rumpfs (`PS2_kopf_seite.json`, `PS2_kopf_seite_x6.png`) | K (Form naeher als r1; die glatten Flaechen gehoeren zum Befund Kopf-Oberflaeche in 1.1) |
| Kopf-Oberseite rot seitlich | KS1 27,6/15,3/3,3 | S4 27,3/14,8/3,1 dE00 0,4 | W |

Hinweis: Im Spiel ist die Seitenansicht der Vorlage 2 nicht zu sehen - bei h85/h90 kriecht die Figur im Port aus dem
Loch, Ruecken zur Kamera (`PS2_spiel_h85_h100.png`); das ist die Szene (vorbestehend, Vorgabe "Szene unveraendert").

### 1.3 Schwanz (Beschreibung2)

| Punkt | Vorlage | V4 r2 | Klasse |
|---|---|---|---|
| Beginn in den Ruecken ueberfuehrend | (357,243), 0,15 H hinter Scheitel, 0,767 H ueber Sohle | T1 0,165 H / 0,750 H; TA (360,236) +0,011/-0,027 H | W |
| Knick (Taille) | (290,308) | T3 dx -0,266 dy 0,244 (Soll -0,265 / 0,257); TA (290,300) | W |
| von da an breiter nach oben | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H | 0,036/0,066/0,127/0,129 H | W |
| Spitze Ende | (294,344), 0,37 H ueber Sohle | T4 dx -0,243 dy 0,393; 0,357 H; TA (296,340) | W |
| Umriss (Silhouette) | Schwanzmaske Leser A (`sfv4/schwanz_maske.npy`, 4452 px); Aussenkontur am Bogen rund: Douglas-Peucker-Eckwinkel 121-147 Grad bei (293,233)/(286,236)/(280,251) | IoU **0,53** (V3 r2 0,37, Referenz 2 0,08), 70 % der Originalmaske gedeckt; aber die Aussenkante ist eine Gerade mit **Ecke 80 Grad bei (282,228) und Spitze 7 Grad bei (281,223)**; Oberkontur bei x 284/288/292 auf y 226/224/223 statt 243/236/234 = **12-17 px (0,05-0,07 H2) ueber dem Original** (`PS2_schwanz_silhouette.json`, `PS2_schwanz_silhouette_x4.png`). Der Schwanz ist ein flaches Brett mit geraden Kanten (`08_rundum_V4.jpg` Gier 180-300); die Dicke ist aus den zwei Vorlagen nicht widerlegbar (vorn verdeckt) | K |
| Farbe Achse | 7 Achsenpunkte (5x5, roh): Mittel L* 11,6 / a* 6,1 | Videoraum dE00 0,4/4,5/3,1/3,4/4,6/1,6/2,8 (Mittel 2,9); r1 hatte 8,6-12,4 an 4 Punkten | W |
| Farbe Oberkante (helles Band) | (300,225) 28,8/-0,2/1,4; (310,222) 25,1/0,9/0,6; (320,216) 33,1/0,1/2,2; (330,212) 30,6/3,3/-0,4 (5x5, roh) | 34,1/-1,4/2,0 dE00 4,5; 32,9/-1,5/1,6 **7,0**; 32,9/-1,2/1,4 2,0; 34,6/-1,8/2,8 **8,6** - neutralgrau und gleichmaessig, das Original wechselt hell/dunkel und ist bei (330,212) mauve (`PS2_sichel_schwanzoben.json`) | K |
| Farbe Zonen | F15-F18 | T8 dE00 1,0/1,1/1,3/1,2 | W |

### 1.4 Farben je Region ("EXAKT seine Farben")

Vorn: Original-Boxen auf Vorlage 1 entblendet, 2 px erodiert; V4 Ids im Videoraum (`PS2_messung_regionen.json`).

| Region | Vorlage | V4 r2 | Klasse |
|---|---|---|---|
| Kopf | 20,1/8,5/2,1 | 20,8/8,3/2,0 dE00 0,5 | W (Mittel; Struktur s. 1.1) |
| Rumpf (Id 8 / Box) | 13,0/4,7/2,1, rot 0,31 | Id 18,6/6,9/3,1 dE00 4,4, rot 0,37; Box 15,5/7,4/3,6 dE00 3,4 (r1: 9,1, rot 0,83) | K |
| Oberarm (B)-links / (B)-rechts | 17,3/3,6/1,3 / 16,8/5,2/0,0 | Id 21,6/5,6/1,5 dE00 3,7 / 20,8/5,7/1,2 dE00 3,0 (r1: 14,8 / 18,2) | K |
| Unterarm (B)-rechts | 25,0/1,0/-2,1 (Box streift die Wand) | Id 28,2/1,5/-1,7 dE00 2,5 | W |
| **Unterarm (B)-links (Id 14) = (W)-rechts** | Box 298-325 x 262-340 (deckt sich mit der Id-14-Box 289-334 x 264-334 der Buehne): **8,8/-3,4/3,2, L* p10/50/90 5,7/8,8/11,1, 0 % L* > 30** - kein helles Pixel; ein dunkles Band zieht im Video (n1450/n1468/n1485) von der Schulter zum Boden. Ob es der Arm im Schatten oder der Raum dahinter ist, ist nicht aufloesbar (Raum-Median n1350-1473 an der Stelle 14,2, Original 8,8 = dunkler als der Raum); ein heller Fleck von 15 x 30 px wuerde die Unschaerfe aber nicht verschwinden lassen | **28,1/1,8/-1,4, p10/50/90 11,7/23,9/52,1, 35 % L* > 30, dE00 15,7** (Referenz 2: 21,4, 13 % > 30, dE00 14,0 - V4 hat den Arm aufgehellt). Im Spielbild h135 und h118 ein weiss gefleckter Unterarm, wo das Video dunkel ist (`PS2_unterarm_links_x3.png`, `PS2_unterarm_links.json`). Vermutete Ursache: beide Unterarme auf das Ziel "Unterarme 29,3" geeicht (Box nur (B)-rechts, streift die helle Wand) | **A** |
| Sichel (B)-links (Id 15) | Pfad (318,280)...(306,400), 5x5 entblendet: Mittel 14,3/-2,2/1,4 (oben 5-10 L*, unten 22-26); roh minus Schwarzwert 17: oben 0,3-2,4 L*, unten 9,9-13,5 | 24,3/-2,1/4,1 dE00 7,3, einheitlich oliv-grau | K (Lage der Sichel im Original nicht sicher aufloesbar; die Farbe ist heller) |
| Jeans | 14,3/2,3/-2,4 | 15,5/0,8/-2,5 dE00 2,2 | W |
| Unterschenkel | 14,7/1,6/-1,4 | Box 14,8/1,0/-2,9 dE00 1,7 | W |
| Oberkoerper ueber den Gang (Box 345-445 x 175-270, roh, Spiel gegen Video) | h100 22,6/5,8/1,9 rot 0,31; h118 18,0/5,5/2,7 rot 0,35; h135 13,1/4,8/2,7 rot 0,27 (h135 noch eingeblendet: Wand 37,2 gegen 46,2) | 20,0/9,0/2,5 dE00 3,7 rot 0,56; 14,5/8,0/2,8 dE00 3,5 rot 0,48; 14,0/7,8/2,8 dE00 3,2 rot 0,47 (V3 r2: 4,3/6,4/8,9) (`PS2_gang_farben.json`) | K (a* +2,5-3,2, Rotanteil 1,6 x) |
| Seite: Buckel-Glanz F14 | 60,0/11,0/1,2 | 54,8/10,6/2,9 dE00 5,0, dL -5,2 (Riegel ROT) | K |
| Seite: Kopf / Buckel / Jeans / Oberarm nah | 25,5/6,4/2,2; 39,4/12,0/2,0; 20,8/1,8/-1,1; 14,6/3,7/0,1 | 27,0/7,0/2,0; Box 35,3/10,6/2,4; Box 16,6/0,6/-4,7; Box 19,3/2,3/-0,2 | W/K |

### 1.5 Koerper unveraendert, Gang

| Punkt | Soll | Ist | Klasse |
|---|---|---|---|
| Geometrie = Referenz 2 | Meshes 0-8, 10-15, Skelett, Keyframes 0-15, EDD byte-gleich | `riegel_v4.machart(EM30_V4.EMD)` selbst ausgefuehrt: "abweichend keine; Skelett True; Keyframes True; EDD True; uebrige Sektionen True; Ref2 sha256 cfd1ac42"; M1-M4 gruen (Schwanz 160, Kopf 109 Dreiecke, 8 bpp) | W |
| Gang / Schwanz im Lauf (T7) | T1-T4 je +-0,06 H | h118 0,019/0,014/0,013/0,011 und h135 0,025/0,007/0,021/0,023 gruen; h85 0,294, h100 0,436, h110 0,255, h125 0,088 ROT - die Feder schwenkt den Schwanz bei h100 waagerecht zur Seite (`07_gang.jpg` letzte Zeile). Im Spielbild vorn (h100-h135) ist der Schwanz nicht sichtbar, bei h85/h90 zeigt die Szene den Ruecken (vorbestehend) | K |

## 2. Was den Nutzer-Eindruck bestimmt

1. **Kopf vorn**: Lage und Farbe von Auge, Haaren, Nase und roter Kuppe stimmen (dE00 0,5-1,4). Aber der Kopf ist eine
   glatte Kuppel: nach gleicher Unschaerfe hat er 0,67 x die Struktur des Original-Kopfs und ist glatter als der eigene
   Rumpf (0,86 gegen 1,57 im Original). Im Spiel h100 sieht man eine flach schattierte rot-rosa Flaeche, in h135 eine
   dunkle Kuppel mit einem weichen hellen Fleck, der doppelt so gross ist wie im Video (17 x 16 px). Das ist genau die
   Art "aufgesetzter, aufgemalter" Kopf, die der Nutzer zweimal abgelehnt hat.
2. **Unterarm (B)-links**: weiss gefleckt, wo das Original nur Werte bis L* 11 hat.
3. Schwanz: Linie, Breite, Taille, Spitze und Farbe der Flaeche passen; die Aussenkante ist eine Gerade mit spitzer Ecke
   (0,05-0,07 H ueber dem runden Original-Bogen), das helle Band oben ist neutralgrau statt mauve - klein.
4. Koerperfarben insgesamt: von r1 (Rumpf dE00 9,1, Oberarme 14,8/18,2) auf 2,2-4,4 verbessert; Rest-Rotstich (a* +2-3).

## 3. Empfehlung an den Bauer

- Kopf: Texel des Kopfes mit der Fleckung des Koerpers fuellen (Ziel: Gradient Kopf/Rumpf im Videoraum ~1,5,
  Kopf-Gradient ~41 wie Original), statt glatter Flaechen; Augenfleck im SPIELBILD messen (K3 auf dem FRAMEDUMP, nicht
  nur auf der Buehne) und auf 0,026 x 0,033 H bringen; rote Kappe nicht als eine flache Flaeche.
- Unterarm (B)-links: eigenes Ziel aus der Box 298-325 x 262-340 (8,8/-3,4/3,2, Anteil L* > 30 = 0) bzw. Unterarme
  getrennt eichen; die Box des Ziels "Unterarme" (468-500 x 268-330) auf Hautpixel begrenzen (Wand rechts ausschliessen).
- Schwanz: Aussenecke bei (281,223) runden (Oberkontur x 284-292 auf y 234-243), Oberband mit a* 0-3 und hell/dunkel-Wechsel.

## 4. Belege (`SF_belege/v4/`)

- `PS2_kopf_vorn_x8.png` Vorlage 1 mit Nutzerpunkten | V4 Buehne (Id-9-Kontur, Augenpunkt) | Spiel h135 | V4 Videoraum
- `PS2_kopf_textur_vorn.json`, `PS2_auge_spiel.json`, `PS2_auge_spiel_b.json`, `PS2_schirm_roh.json`
- `PS2_kopf_seite_x6.png`, `PS2_kopf_seite.json`, `PS2_kopf_schwanz.json`
- `PS2_schwanz_silhouette_x4.png` (magenta Originalmaske, cyan V4 Ids 16-22), `PS2_schwanz_silhouette.json`, `PS2_sichel_schwanzoben.json`
- `PS2_unterarm_links_x3.png`, `PS2_unterarm_links.json`
- `PS2_spiel_vs_video.png` (Video n1485/n1468/n1450/n1435 roh | Spiel V4 r2 | Spiel V3 r2), `PS2_spiel_h85_h100.png`, `PS2_gang_farben.json`
- `PS2_messung_regionen.json` (alle Regionen vorn/seitlich, V4/Referenz 2/V3 r2)
