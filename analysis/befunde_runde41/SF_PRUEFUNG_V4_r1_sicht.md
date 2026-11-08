# SF V4 - Pruefung Runde 1 (Sicht): V4 gegen die Nutzer-Beschriftung Beschreibung1/2

Pruefer "Auge des Nutzers", 2026-11-07, Worktree r41_sichel (Zweig r41/sichel_nb2), nichts committet.
Gegenstand: Leisten des Bauers `SF_belege/v4/01..08`, `kopf_nah_*_x6_eichung.png`, EMD `sfv4/emd/EM30_V4.EMD`
(sha256 0dddbdbe...), Spiellauf `sfv4/r/V4` (RE15_FRAMEDUMP), gegen `Version_80%/Beschreibung1/2(_Vorlage).png`
(= Videobilder n1485 / n1435, h = n - 1350) und das rohe Video.

**Ergebnis: NICHT bestanden.** Kopf vorn, Schwanz-Polygon (Beginn/Biegung/Knick/Verbreiterung/Ende) und die Geometrie
des Koerpers sind wie Vorlage bzw. nur klein abweichend. Drei Punkte gehoeren in die dritte Klasse ("der Nutzer wuerde
wieder ablehnen"): (1) die Koerperfarben (Rumpf als rotes Hemd, Oberarme doppelt so hell, Unterarme rot statt grau),
(2) der Kopf von der Seite (Keil-Vorbau statt runder Kopf, Auge halb so gross und in 5 Teile zerfallen, schwarzer
Lappen an der Vorderkante), (3) die Schwanzfarbe (obere Haelfte hellgrau statt dunkel-mauve, dE00 8,6-12,4 an 4 von
7 Achsenpunkten).

Eigene Werkzeuge: `re15_port/tools/r41_sichel/v4/pruefung_sicht/*.py` (Linien, Render ueber die Buehne des Bauers
`v4_gemein.zeichne`, Regionen, Spielbilder ueber `c6.port`). Belege: `SF_belege/v4/P1S_01..08*.png`, `P1S_*.json`.
Laufausgaben: Scratchpad `sfv4/sicht/`.

Konventionen: (B) = aus Sicht des Betrachters, (W) = aus Sicht des Wesens. Lab = CIE-Lab D65, dE00 = CIEDE2000.
"Videoraum" = Vergleichsregel des Messblatts V4 (Port-Schirm, Gauss sigma 1,5, V = a + b x P); Vorlage 1 entblendet
(k 0,766/0,764/0,768), Vorlage 2 roh. H1 = 302 px, H2 = 253 px (Messblatt V4), H_port 364 (Pose 135) / 264 (Pose 85).

## 0. Linienlesung (eigene Messung, `linien_sicht.py`, `P1S_linien.json`)

Linienfarbe RGB (237,28,36), Maske im Bildteil = Differenz Beschreibung - Vorlage: 3798 = 3798 (Bild 1), 6712 = 6712
(Bild 2). Belegung = Anteil der Abtastpunkte (0,5 px) der Strecke Linienstart (Randfeld) -> Skelett-Ende auf der 3x3
aufgeweiteten Maske; beste Permutation.

| Bild | Text | Ende | Belegung | beste Alternative |
|---|---|---|---|---|
| 1 | Auge | (382,165) | 0,387 gerade, Linie ist eine eigene Komponente mit Haken (gleiche Lesung wie A/B/Pruefer) | (403,160) 0,372 |
| 1 | Haare | (403,160) | 1,000 | (382,165) 0,946 (Verlaengerung ueber das Ende hinaus) |
| 1 | Nase | (394,177) | 1,000 | 0,349 |
| 2 | Auge von der Seite | (459,235) | 1,000 | (467,241) 0,548 |
| 2 | Haare von der Seite | (467,241) | 1,000 | (459,235) 0,608 |
| 2 | Schwanzspitze Ende | (294,344) | 1,000 | 0,300 |
| 2 | Schwanz Beginn | (357,243) | 1,000 | 0,299 |
| 2 | Schwanz Knick | (290,308) | 1,000 | 0,208 |

Deckt sich mit Leser A, Leser B und Messblatt V4 (Abschnitt 1a) bis auf 1-2 px. Die Lesung des Bauers steht also auf
den richtigen Punkten.

## 1. Befunde Punkt fuer Punkt

Klassen: **W** = wie Vorlage, **K** = Abweichung klein, **A** = Abweichung, die der Nutzer wieder ablehnen wuerde.

### 1.1 Kopf vorn (Beschreibung1)

| Punkt | Vorlage (gemessen) | V4 (gemessen) | Klasse |
|---|---|---|---|
| Kopf gesamt, Farbe | Box 368-416/139-191, 2 px erodiert, entblendet: Lab 20,1/8,5/2,1; L* p10/50/90 9,4/17,8/33,7; hellste 5x5-Stelle L* 52,7, dunkelste 6,2 | Id 9 im Videoraum: 19,7/8,4/1,8, **dE00 0,4**; p10/50/90 11,0/17,3/29,9; hellste 52,3, dunkelste 5,9 | W |
| Auge vorn, Lage | Nutzerpunkt (382,164) = u 0,29 / v 0,48 der Kopfbox, (B)-links der Mitte = (W)-rechts | auf die V4-Kopfbox (327-378/154-221) umgerechnet (342,186): Id 9, 5x5 Lab 27,7/6,4/-1,0 gegen Original 32,3/7,5/-1,2; Riegel K4 0,056 H links, 0,112 H unter OK | W |
| Auge vorn, Groesse/Aussehen | im rohen Video n1485: hellster Fleck (376,166), Kern 3x3 Lum 103,9 gegen Ring 15x15 56,5 = **+47**, Blob 8x10 px (52 px), max. Nachbar-Sprung 24 Lum | im Spielbild h135 (registriert): (332,181) Kern 200,7 / Ring 138,2 = **+62**, Blob 9x12 px (73 px), max. Nachbar-Sprung 45 Lum | K: Mass und Kontrast nah (1,3x), aber die Kante ist doppelt so hart; auf der glatten dunklen Kuppel wirkt der Fleck "aufgemalt" (P1S_07). Kein Pupillenring, kein zweites Auge - wie Vorlage |
| Haare vorn | Nutzerpunkt (404,160), 5x5 Lab 15,6/7,3/1,4, 17 L* dunkler als das Auge, (B)-rechts ueber dem Auge | umgerechnet (365,181): 13,0/8,1/2,0, 15 L* dunkler als das Auge; Riegel K5 dE00 0,3 | W |
| Nase | (395,177): 16,6/7,7/1,4; als Struktur nicht aufloesbar (Leser A/B) | (356,203): 15,2/6,0/1,4; K7 dE00 0,9; keine Struktur | W (nicht aufloesbar, V4 traegt nichts Falsches ein) |
| Kopf-Oberseite rot | KV1 22,6/15,4/5,2 | 22,9/15,4/5,4 dE00 0,2 | W |
| Maulzone (nicht beschriftet) | Box 370-412/183-206 unter dem Kopf: 17,1/11,6/4,4; **15 % der Pixel L* > 30**, p90 34,1 (helle Flecken, Zaehne oder Brust - nicht entscheidbar) | V4 unteres Kopfviertel + 14 px: 14,0/13,0/4,9 dE00 2,3; **0 %** L* > 30, p90 17,1 | K/offen: Nutzer sagte 20:35 "der Mund sieht nicht so comic-haft 100 % oval aus" (also: ein Mund ist da, nur nicht oval). V4 hat keinen Mund mehr; die Beschriftung nennt keinen. Nutzer fragen. |

### 1.2 Kopf seitlich (Beschreibung2)

| Punkt | Vorlage | V4 | Klasse |
|---|---|---|---|
| Kopf seitlich, Farbe | Box 412-467/190-263: 25,5/6,4/2,2 | Id 9 Pose 85: 23,8/6,2/3,4 dE00 1,6 | W |
| Kopfform seitlich | Kopfbox 56 x 74 px = **0,221 x 0,292 H2**; die Vorderseite ist eine fast senkrechte dunkle Kante (Zeile y240-248: Lum 85 bei x455 -> **27 bei x464** -> 127 bei x470, also 3-4 px breit); davor/darueber der Augenfleck 15 x 16 px; runder Kopf, kein Vorsprung | Kopfbox 45 x 52 px = **0,170 x 0,197 H**; die Spaltenhoehe des Kopfes faellt von 45 px (x451) auf 29 (x467) und **18-21 px (x475-483)**: ein nach vorn spitzer Keil ("Vorbau 200 E", Messblatt-Aenderung), das Auge sitzt als Punkt an der Keilspitze (P1S_04, 04_kopf_nah_seite_x4) | **A** |
| Auge seitlich | heller Fleck 15 x 16 px = 0,059 x 0,063 H2, eine Komponente | Riegel S1 **ROT**: 0,023 x 0,030 H, **5 Komponenten** (Bauer: sitzt an der Kante Vorder-/Seitenbild plus Straehne) | **A** (halbe Groesse, zerfallen) |
| Haare seitlich | duenne dunkle Vorderkante x462-465 (Lum 23-40), kein Block | **108 px mit L* < 8 bei x >= 470** (Box 470-483 x 210-232 = 14 x 23 px = 0,05 x 0,09 H), insgesamt 22 % des Seitenkopfs fast schwarz (die Haarstraehne, 1 Viereck): ein schwarzer Lappen vor dem Kopf (P1S_04 rechts) | **A** |

### 1.3 Schwanz (Beschreibung2)

| Punkt | Vorlage | V4 | Klasse |
|---|---|---|---|
| Beginn in den Ruecken ueberfuehrend | (357,243): 0,15 H hinter dem Scheitel, 0,767 H ueber der Sohle; Ansatz geht ohne Stufe in den Buckel ueber | Riegel T1 **ROT** 0,214 / 0,727 H; Ursache laut Bauer die Rueckenkontur von Referenz 2 (G1); Ansatz sitzt mit sichtbarer Naht (dunkelrote Unterseite trifft graue Oberseite) am Buckel | K (0,06 / 0,04 H, Koerper-bedingt) |
| Biegung / Knick | Maskenachse (Leser A): -163 -> -121 -> -101 -> -96 Grad, Drehung 42 + 20 Grad auf 20 px bei ~(295,250); Nutzer-Knick (290,308) = Taille | Skelett Ids 16-22 Pose 85: -163 -> -143 -> -101 -> -90 Grad, **max. 42 Grad je 10 px bei (289,245)**, danach 0-11 Grad: derselbe Bogen; Riegel T2 68 Grad, T3 dx -0,263 / dy 0,255 (Soll -0,265 / 0,257) | W |
| von da an breiter nach oben | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H | 0,034/0,066/0,123/0,140 H, Taille 0,021 H schmalste Stelle | W |
| Schwanzspitze Ende | (294,344): dx -0,245 / dy +0,397, 0,37 H ueber Sohle (Bildhoehe) | dx -0,241 / dy 0,398; 0,330 H ueber Sohle; Endstueck 0,028 x 0,144 H, 81 Grad | K (0,04 H tiefer) |
| Schwanzfarbe | 5x5-Fenster auf der Maskenachse (Leser A), roh: (330,236) 8,5/12,1/1,4; (310,243) 11,8/8,8/1,6; (292,262) 17,5/6,6/1,3; (289,280) 5,5/5,0/2,1; (288,300) 10,0/1,7/1,6; (287,318) 17,7/2,8/1,5; (290,335) 10,2/5,4/1,6 - dunkles Mauve-Grau, zur Wurzel hin roter | naechster Schwanzpixel (Abstand 0-7 px), Videoraum: 17,8/4,9/3,2 **dE 9,4**; 17,0/5,8/2,8 dE 4,7; 9,7/-1,8/1,6 **dE 12,4**; 18,0/-1,2/0,9 **dE 11,7**; 21,6/-0,9/1,6 **dE 8,6**; 15,2/2,9/0,7 dE 1,8; 12,1/2,4/2,0 dE 3,8. Mittel ueber die 7 Punkte: Original L* 11,6 / a* 6,1, V4 L* 15,9 / a* 1,7. Schwanz gesamt (Id 16-22) 22,6/1,4/2,0, Median L* 27 | **A**: die obere Haelfte ist 4-12 L* heller und 4-8 a* weniger rot als das Original - im Bild (P1S_02) ein hellgraues Brett neben dem dunkel-mauven Original. Der Bauer misst mit seinen vier Zonen-Fenstern (F15-F18) dE00 <= 3; die Zonen "Aussen 26,6" deckt in V4 die ganze breite Flaeche, im Original ist das helle Grau nur die Aussenkante (Leser A 3b.5). Die Textur ist nicht flach (Lum-Std im breiten Teil V4 22 gegen Original 11,5 - V4 ist zweitoeniger, nicht einfarbig) |

### 1.4 Koerperfarben ("EXAKT seine Farben", Spielkamera 50A0/12, Pose 135, P1S_01/06/08)

Original = Boxen auf n1485 entblendet (P1S_08 links), 2 px erodiert; V4 = Ids im Videoraum. "rot" = Anteil Pixel mit
R - G >= 15 (Mass des Bauers fuer F6).

| Region | Vorlage | V4 | Referenz 2 (gelobt) | Klasse |
|---|---|---|---|---|
| Rumpf (Id 8 / Box 352-448 x 196-292) | 13,0/4,7/2,1; **rot 0,31**; Chroma 6,6; L* > 30: 4 % - dunkles Grau-Mauve mit roter Brustmitte | 19,2/**13,3**/5,4 **dE00 9,1**; **rot 0,83**; Chroma 14,7; L* > 30: 17 % | 31,2/8,3/5,0 dE 13,5; rot 0,51 | **A**: V4 ist ueber 83 % des Rumpfs rot (Original 31 %), a* fast dreimal so hoch - im Spielbild ein rotes Hemd (P1S_06). Der Bauer-Riegel F6 misst nur die roten Pixel (8698 px in V4 gegen 1498 px rote Brust im Original, Leser A) und ist deshalb gruen |
| Oberarm (B)-links (Id 13 / Box 300-345 x 186-250) | 17,3/3,6/1,3 | 35,9/8,5/1,5 **dE 14,8** | 28,9/9,8/4,8 dE 10,7 | **A** (doppelt so hell) |
| Oberarm (B)-rechts (Id 10 / Box 455-500 x 196-262) | 16,8/5,2/0,0 | 40,6/6,6/0,6 **dE 18,2** (Bauer F8 ROT) | 31,9/8,8/4,4 dE 12,0 | **A** |
| Unterarm (B)-rechts (Id 11 / Box 468-508 x 268-330, Box streift rechts die Wand) | 25,0/**1,0**/-2,1, rot 0,00 - grau, nur die Klaue rot | 17,0/**20,0**/3,2 **dE 17,9**, rot 0,95 | 20,6/4,1/0,0 dE 5,5 | **A**: beide Unterarme sind in V4 rote Handschuhe (Id 14: 18,3/21,4/3,6), im Video graue Haut |
| Schulter (B)-links / rechts (Boxen 318-360 x 160-190, 425-470 x 165-195) | 37,5/5,6/3,6 (hellste Hautstelle vorn) / 27,9/4,3/0,8 | Schultern liegen in V4 auf Id 8/10/13 und sind hell (P1S_01) - Lage der Helligkeit passt | K |
| Jeans (Id 0,1,4 / Box 362-428 x 298-344) | 14,3/2,3/-2,4 | 18,9/2,3/-1,4 dE 3,2 | 27,7/1,6/-4,0 dE 9,5 | W/K |
| Unterschenkel (Id 2,3,5,6 / Box 356-392 x 352-430) | 14,7/1,6/-1,4 | 34,3/9,9/2,3 (nicht geeicht) | 27,6/11,1/5,5 | K (vorbestehend, nicht Teil der Eichung) |
| Kopf | s. 1.1: dE 0,4 | | | W |

### 1.5 Koerper unveraendert, Gang

| Punkt | Soll | Ist | Klasse |
|---|---|---|---|
| Geometrie = Referenz 2 | Meshes 0-8, 10-15, Skelett, Keyframes 0-15, EDD byte-gleich | `riegel_v4.machart(EM30_V4.EMD)` selbst ausgefuehrt: "abweichend keine; Skelett True; Keyframes True; EDD True; uebrige Sektionen True". Spiel-Asset `EM30_HANGAR.EMD` sha256 cfd1ac42... = Pin `release/em30_hangar.sha256` (unveraendert) | W |
| Gang (T7) | T1-T4 je +-0,06 H im Spiellauf | h118 / h135: T2-T4 <= 0,039 H (gruen); h100-h125: 0,05-0,36 H, weil die Szene den Schwanz zur Seite schwingt (Spur H) - Bauer T7 ROT. Keyframes gleich, der Gang selbst ist Referenz 2. Die Pose h135 im Port steht aufrechter und groesser (H_port 364 gegen H1 302) als im Video - vorbestehend seit Referenz 2 (P1S_06) | K |

## 2. Was den Nutzer-Eindruck bestimmt (P1S_06 = Spielbild gegen rohes Video, P1S_07 = Kopf)

1. Im Spielbild h135/h118/h100 traegt V4 einen einheitlich dunkelroten Rumpf mit weissen Schultern, roten Unterarmen
   und schwarzen Shorts. Das Video zeigt dunkle, fleckige grau-mauve Haut mit einer roten Brustmitte und hellen
   Schultern. Referenz 2 (gelobt) trifft die fleckige Haut, ist nur zu hell (Rumpf dE 13,5 wegen L* 31 gegen 13).
   Die Eichung des Bauers hat die Hautzeilen 480-482 der CLUT so skaliert, dass die roten Fenster stimmen; die
   nicht-roten zwei Drittel des Rumpfs wurden dabei mitgefaerbt.
2. Der Kopf vorn ist in Mittel, Extrema, Auge-/Haar-/Nasenpunkt wie das Original (dE 0,4-5). Dass er im Spiel als
   glatte dunkle Kuppel mit einem weissen Fleck wirkt, liegt an der doppelt so harten Kante (45 gegen 24 Lum je Pixel)
   und am fehlenden Mund - beides klein, aber der Nutzer hat "aufgemalte Augen" schon einmal abgelehnt.
3. Der Kopf seitlich ist kein runder Kopf mit Auge hinter einer duennen dunklen Kante, sondern ein spitzer Keil mit
   Punkt-Auge und einem schwarzen 14 x 23 px Lappen.
4. Der Schwanz hat die richtige Linie (Bogen, Taille, Verbreiterung, Endstueck), aber oben die falsche Farbe (hellgrau
   statt dunkel-mauve).

## 3. Empfehlung an den Bauer (Runde 2)

- Koerper: die CLUT-Skalierung der Hautzeilen so eichen, dass der GANZE Rumpf (Id 8 ohne rg-Maske) das Original trifft
  (13,0/4,7/2,1 im Videoraum, rot-Anteil ~0,3), die rote Brustmitte bleibt als Zone; Oberarme auf L* ~17, Unterarme
  auf a* ~1-4 (grau), Klauen rot. Riegel F6 ohne rg-Maske fuehren, dazu F-Riegel fuer Unterarme (Id 11/14).
- Kopf seitlich: Vorbau zurueck auf eine Vorderseite von >= 0,12 H Hoehe (Original: dunkle Kante 3-4 px breit, kein
  Block); Haarstraehne als duenne Kante statt Viereck (L* < 8 Anteil < 5 %); Auge seitlich eine Komponente
  0,05-0,06 H.
- Schwanz: Zone "Aussen" auf das Mass der Originalkante beschraenken, Flaeche auf 10-18 L* / a* 5-9 (Achsenpunkte
  oben), Endstueck bleibt.
- Mund: Nutzer fragen (nicht beschriftet; sein Satz von 20:35 setzt einen Mund voraus).

## 4. Belege

- `SF_belege/v4/P1S_01_vorn_x2.png` Vorlage 1 entblendet | V4 Pose 135 | Referenz 2 (Buehne des Bauers)
- `P1S_02_seite_x2.png` Vorlage 2 | V4 Pose 85 | Referenz 2
- `P1S_03_kopf_vorn_x6.png`, `P1S_04_kopf_seite_x6.png` Kopfboxen x6
- `P1S_05_neutral_texel.png` V4 | Referenz 2 | V3 r2 ohne Licht (Texelfarben)
- `P1S_06_spiel_vs_video.png` Video n1485/n1468/n1450 roh | Spiel V4 | Spiel V3 r2 (RE15_FRAMEDUMP, registriert)
- `P1S_07_spiel_kopf.png` Kopf im Spielbild h135
- `P1S_08_regionen.png` meine Boxen auf den Originalen und die Id-Karte von V4
- `P1S_linien.json`, `P1S_messung.json`, `P1S_auge_schwanz.json` Zahlen
