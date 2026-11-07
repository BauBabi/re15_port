| Mass | Soll | Ist | |
|---|---|---|---|
| G1 Koerper-Geometrie = Ref2 | Meshes 0-8, 10-15: Ecken, Normalen, Flaechen, UV gleich (O1: CLUT-Feld frei); Skelett/Keyframes 0-15, EDD gleich | Geometrie abweichend keine; CLUT-Feld geaendert keine; byte-gleich 15 von 15; Skelett True; Keyframes True; EDD True; uebrige Sektionen True; Ref2 sha256 cfd1ac42 | OK |
| M1 Dreiecke Schwanz | <= 212 | 160 ([30, 20, 20, 20, 20, 20, 30]) | OK |
| M2 Dreiecke Kopf | <= 118 | 109 | OK |
| M3 Farbtiefe | 8-bpp-TIM, Texel aus der CLUT | 8 bpp True; CLUT-Zeilen Kopf [486], Schwanz [487], Koerper [480, 481, 482, 484] (TIM 8 Zeilen) | OK |
| M4 Texeldichte | Kopf und Schwanz 0,045-0,091 Texel/E. | Kopf 0.054; Schwanz [0.051, 0.056, 0.051, 0.062, 0.066, 0.066, 0.059] | OK |
| K1 Augen vorn | 1 Komponente; Kontrast >= 20 (Original 29,5) | 1 Komp.; 20.3 | OK |
| K2 Augenfleck Farbe | 46,9/7,2/-1,1 dE00 <= 6; L* >= KV5 + 20 | 46.7/7.5/-0.6 dE00 0.5; L* 46.7 vs KV5 24.6 + 20 | OK |
| K3 Augenfleck Groesse | B 0,016-0,036 / H 0,023-0,043 H | 0.030 x 0.036 H (11 x 13 px) | OK |
| K4 Augenlage vorn | (B)-links 0,035-0,065 H; 0,07-0,12 H unter OK | 0.059 H links; 0.107 H unter OK | OK |
| K5 Haarfeld vorn | 13,3/7,4/1,5 dE00 <= 6; L* <= Auge - 25 | 14.4/7.2/1.7 dE00 0.8; L* 14.4 vs 21.7 | OK |
| K6 zweites Auge verdeckt | L* <= 18 (Original 12,9) | L* 9.0 (Box (369, 187, 379, 199)) | OK |
| K7 Nase | 18,2/7,1/1,3 dE00 <= 6; L* zwischen Haar und Auge | 20.2/6.8/1.0 dE00 1.4 | OK |
| K8 Kinn/Maulzone | 9,9/9,9/3,8 dE00 <= 6; Anteil L*>35 <= 0,05 | 11.7/6.7/2.8 dE00 3.3; Anteil 0.000 | OK |
| K9 Kopf-Oberseite rot vorn | 22,6/15,4/5,2 dE00 <= 6; a* >= 11 | 21.3/16.1/5.5 dE00 1.1 | OK |
| K10 Kopfbox vorn | 0,14-0,18 x 0,15-0,20 H | 0.154 x 0.187 H | OK |
| K11 Kopf-Struktur vorn | Lum-Gradient Kopf 41.0 +-6.1 (Original, 9 Boxen 39.4-42.9, Viertel 34.9-47.0); Kopf >= Rumpf (Original 1.57 x) | Kopf 27.7, Rumpf 32.9 (0.84 x) (Pruefung V4 r2 Sicht 1.1: Runde 2 Kopf 27,8 / Rumpf 32,4) | ROT |
| S1 Auge seitlich | 1 heller Fleck (1 Komponente); 48,4/4,0/0,5 dE00 <= 6; 0,028-0,052 x 0,040-0,070 H | 47.8/3.8/0.9 dE00 0.8; 0.034 x 0.049 H; 1 Komp. | OK |
| S2 Augenlage seitlich | 0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle | 0.058 H; 0.811 H | OK |
| S3 Haarkante seitlich | L* <= 22; 19,1/1,2/1,1 dE00 <= 6; Abstand zum Fleck <= 0,01 H | 13.8/2.0/1.0 dE00 3.7; Abstand 0.011 H | ROT |
| S4 Kopf-Oberseite rot seitlich | 27,6/15,3/3,3 dE00 <= 6; a* >= 11 | 27.3/14.8/3.1 dE00 0.4 | OK |
| T1 Schwanz Beginn | 0,15 H hinter Scheitel, 0,767 H ueber Sohle (+-0,03) | 0.165 H hinter, 0.750 H ueber (Id 16 an Id 7/8) | OK |
| T2 Biegung | dx -0,245 dy +0,036 (+-0,03); Drehung 68.4 +-5.0 Grad (t2_mass_v4, Original 70.1/65.8/69.5) | dx -0.252 dy 0.022; 52.5 Grad (Bauer-Mittellinie Runde 2: 67.3) | ROT |
| T3 Knick (Taille) | dx -0,265 dy +0,257 (+-0,03) | dx -0.266 dy 0.244 | OK |
| T4 Ende | dx -0,245 dy +0,397 (+-0,03); 0,37 H ueber Sohle | dx -0.243 dy 0.393; 0.357 H ueber Sohle | OK |
| TA Schwanz absolut | Beginn (357,243), Knick (290,308), Ende (295,343) je Achse +-0,03 H | Beginn (360,236) +0.011/-0.027 H; Knick (290,300) -0.001/-0.029 H; Ende (296,340) +0.003/-0.012 H | OK |
| T5 Breite nach oben | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01 | 0.036/0.066/0.127/0.129 H; Abfall 0.008 | OK |
| T6 Endstueck | B 0,025-0,05; L 0,12-0,17 H; 75-95 Grad; Taille <= 0,035 und schmalste | B 0.034; L 0.152 H; 81 Grad (vorn); Taille 0.025, schmalste davor 0.025 | OK |
| T8 Schwanzfarbe | F15 9,3/11,4/1,5; F16 8,7/8,4/2,2; F17 26,8/-0,1/2,4; F18 15,2/3,2/1,6; dE00 <= 6; a* Wurzel >= 7, Ende <= 6 | F15 8.7/11.6/0.3 dE00 1.0 (295 px); F16 9.5/9.3/1.7 dE00 1.1 (216 px); F17 26.6/0.2/1.2 dE00 1.3 (971 px); F18 13.9/3.9/2.0 dE00 1.2 (284 px) | OK |
| T9 Schwanzachse | 7 Achsenpunkte: Mittel dE00 <= 6; L*/a* Mittel +-4/+-3 zum Original 11.6/6.1 | Mittel dE00 2.9; L*/a* 11.3/6.3; je Punkt 0.5 4.5 3.1 3.4 4.6 1.5 2.8 | OK |
| T7a Gang koerperfest Lage | Gelenke 1/3/5/7 je <= 0,06 H vom Ziel (Keyframe der gezeichneten Pose), alle Gangtakte | 38 von 47 Takten > 0,06 H; max 0.442 H (h100 {'Beginn': 0.321, 'Biegung': 0.284, 'Knick': 0.306, 'Ende': 0.442}) | ROT |
| T7b Biegung/Knick im Gang | Gelenk 3 >= Keyframe - 10 Grad; Gelenk 5 >= halber Keyframe-Winkel | Biegung 46.7-62.3 Grad (Keyframe 58.1-65.2), 23 Takte darunter; Knick 14.0-16.2 (Keyframe 15.8-16.2), 0 darunter | ROT |
| T7c keine waagerechte Kette | Strecke Biegung -> Ende in Zielrichtung >= 0,75 x Keyframe | min -0.090 H (Keyframe 0.405 H); 17 Takte darunter [90, 91, 92, 93, 94, 95, 96, 97] | ROT |
| T7d keine Rueckfaltung | alle Gelenkwinkel <= 90 Grad, ganzer Lauf | max 67.6 Grad; 0 von 144 Takten > 90 | OK |
| Boden Mesh | kein Glied-Mesh und keine Dornspitze unter dem Boden, ganzer Lauf | tiefster Mesh-Punkt +76 E. (h34, Dornspitze +54 E.); 4 Takte darunter (Radien je Gelenk [315, 315, 281, 281, 271, 110, 82, 82] E., Dorn 319 E. (Mesh 22)) | ROT |
| F0 Lichtgegenprobe h118 | Rumpf b2 gegen FRAMEDUMP dE00 <= 3 | b2 20.5/6.8/2.6 / Spiel 20.5/6.8/2.6 dE00 0.1 (11173 px) | OK |
| F0 Lichtgegenprobe h135 | Rumpf b2 gegen FRAMEDUMP dE00 <= 3 | b2 19.0/6.9/2.9 / Spiel 18.8/6.6/2.8 dE00 0.3 (12422 px) | OK |
| F F6 | 16.6/17.3/6.2 dE00 <= 5, |dL| <= 4 | 18.7/13.2/5.5 dE00 3.3 dL +2.1 (3851 px) | OK |
| F F7 | 33.3/5.5/2.7 dE00 <= 5, |dL| <= 4 | 30.5/8.6/2.6 dE00 3.9 dL -2.8 (1031 px) | OK |
| F F8 | 16.4/8.3/1.5 dE00 <= 5, |dL| <= 4 | 20.1/6.6/1.8 dE00 3.0 dL +3.7 (1991 px) | OK |
| F F9 | 12.7/-0.9/-1.5 dE00 <= 5, |dL| <= 4 | 14.2/-0.7/-3.6 dE00 2.1 dL +1.5 (4255 px) | OK |
| F F10 | 11.2/14.0/2.5 dE00 <= 5, |dL| <= 4 | 13.1/10.9/1.9 dE00 2.7 dL +1.9 (300 px) (Id 12 (Klaue (B)-rechts); Id 15: nan/nan/nan (0 px)) | OK |
| F F14 | 60.0/11.0/1.2 dE00 <= 5, |dL| <= 4 | 54.8/10.6/2.9 dE00 5.0 dL -5.2 (904 px) | ROT |
| F F19 | 18.4/0.5/-3.5 dE00 <= 5, |dL| <= 4 | 17.5/-0.3/-4.9 dE00 1.7 dL -0.9 (1507 px) | OK |
| F Mittel | Mittel dE00 <= 4 | 3.11 | OK |
| F F7 Messblatt-Wortlaut (Id ganz) | 33.3/5.5/2.7 dE00 <= 5, |dL| <= 4 | 22.4/5.8/1.6 dE00 8.3 dL -10.9 (2181 px) (Runde 3: Mesh 10 hat eine eigene CLUT-Zeile 489 (O1). Das Fenster F7 (x 307-347, y 167-212) liegt nur auf der hellen Schulterkuppe; der ganze Oberarm (B)-links ist im Original 17,3 L* (Sicht-Pruefung V4 r2 1.4, Box 300-345 x 186-250) - Widerspruch im Messblatt-Wortlaut, Offen) | ROT |
| F F8 Messblatt-Wortlaut (Id ganz) | 16.4/8.3/1.5 dE00 <= 5, |dL| <= 4 | 20.9/5.6/1.1 dE00 4.1 dL +4.5 (3293 px) (Runde 3: Mesh 10 mit eigener CLUT-Zeile 489 (O1), Verteilung des F8-Fensters fuer Id 10 ganz (r2_koerper_farben oberarm_br)) | ROT |
| FS Oberarm (B)-links unten | 11.0/2.0/0.9, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 14.8/3.1/0.6 dE00 2.8 dL +3.7; Rotanteil 0.05 (1150 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(300, 213, 345, 250)]) | OK |
| FS Oberarm (B)-rechts unten | 20.9/3.2/-0.4, Rotanteil 0.02; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 22.2/4.1/0.1 dE00 1.5 dL +1.2; Rotanteil 0.09 (1302 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(455, 228, 500, 262)]) | OK |
| FS Rumpf + Schultern | 17.3/4.8/1.9, Rotanteil 0.31; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 17.9/6.7/2.8 dE00 2.2 dL +0.6; Rotanteil 0.36 (13358 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(352, 196, 448, 292), (318, 160, 360, 190), (425, 165, 470, 195)]) | OK |
| FS Unterarm (B)-rechts = (W)-links Id 11 | 29.3/0.7/-1.6, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 28.9/1.6/-1.7 dE00 1.3 dL -0.4; Rotanteil 0.00 (1486 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(468, 268, 500, 330)]) | OK |
| FS Unterschenkel | 15.1/1.7/-1.5, Rotanteil 0.03; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 17.9/4.5/0.7 dE00 4.4 dL +2.8; Rotanteil 0.17 (1901 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(356, 352, 392, 430)]) | OK |
| FS Unterarm (B)-links = (W)-rechts Id 14 | 9.0/-3.8/3.5, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15; Anteil L* > 30 <= Original + 0,05 | 28.2/1.8/-1.4 dE00 15.9 dL +19.1; Rotanteil 0.00 (2041 px); Anteil L* > 30 0.337 (Original 0.000, <= +0,05); L* p90 51.9 (Original 11.2) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(298, 262, 325, 340)]) | ROT |
| F0 Gueltigkeit Farbvergleich | Wand links 64,7/0,3/-2,1 dE00 <= 2 | 65.2/0.2/-2.5 dE00 0.52 | OK |
