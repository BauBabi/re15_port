| Mass | Soll | Ist | |
|---|---|---|---|
| G1 Koerper-Geometrie = Ref2 | Meshes 0-8, 10-15: Ecken, Normalen, Flaechen, UV gleich (O1: CLUT-Feld frei); Skelett/Keyframes 0-15, EDD gleich | Geometrie abweichend keine; CLUT-Feld geaendert {10: '[480] -> [489]', 14: '[482] -> [488]'}; byte-gleich 13 von 15; Skelett True; Keyframes True; EDD True; uebrige Sektionen True; Ref2 sha256 cfd1ac42 | OK |
| M1 Dreiecke Schwanz | <= 212 | 160 ([30, 20, 20, 20, 20, 20, 30]) | OK |
| M2 Dreiecke Kopf | <= 118 | 109 | OK |
| M3 Farbtiefe | 8-bpp-TIM, Texel aus der CLUT | 8 bpp True; CLUT-Zeilen Kopf [486], Schwanz [487], Koerper [480, 481, 482, 484, 488, 489] (TIM 10 Zeilen) | OK |
| M4 Texeldichte | Kopf und Schwanz 0,045-0,091 Texel/E. | Kopf 0.054; Schwanz [0.051, 0.058, 0.051, 0.056, 0.066, 0.066, 0.059] | OK |
| K1 Augen vorn | 1 Komponente; Kontrast >= 20 (Original 29,5) | 1 Komp.; 20.4 | OK |
| K2 Augenfleck Farbe | 46,9/7,2/-1,1 dE00 <= 6; L* >= KV5 + 20 | 46.6/7.7/-1.5 dE00 0.7; L* 46.6 vs KV5 24.6 + 20 | OK |
| K3 Augenfleck Groesse | B 0,016-0,036 / H 0,023-0,043 H | 0.030 x 0.036 H (11 x 13 px) | OK |
| K4 Augenlage vorn | (B)-links 0,035-0,065 H; 0,07-0,12 H unter OK | 0.060 H links; 0.107 H unter OK | OK |
| K5 Haarfeld vorn | 13,3/7,4/1,5 dE00 <= 6; L* <= Auge - 25 | 14.3/7.3/1.3 dE00 0.7; L* 14.3 vs 21.6 | OK |
| K6 zweites Auge verdeckt | L* <= 18 (Original 12,9) | L* 12.7 (Box (369, 187, 379, 199)) | OK |
| K7 Nase | 18,2/7,1/1,3 dE00 <= 6; L* zwischen Haar und Auge | 20.1/6.8/1.7 dE00 1.4 | OK |
| K8 Kinn/Maulzone | 9,9/9,9/3,8 dE00 <= 6; Anteil L*>35 <= 0,05 | 11.7/6.7/2.6 dE00 3.3; Anteil 0.000 | OK |
| K9 Kopf-Oberseite rot vorn | 22,6/15,4/5,2 dE00 <= 6; a* >= 11 | 20.6/16.5/5.7 dE00 1.6 | OK |
| K10 Kopfbox vorn | 0,14-0,18 x 0,15-0,20 H | 0.154 x 0.187 H | OK |
| K11 Kopf-Struktur vorn | Lum-Gradient Kopf 41.0 +-6.1 (Original, 9 Boxen 39.4-42.9, Viertel 34.9-47.0); Kopf >= Rumpf (Original 1.57 x) | Kopf 36.1, Rumpf 32.7 (1.10 x) (Pruefung V4 r2 Sicht 1.1: Runde 2 Kopf 27,8 / Rumpf 32,4) | OK |
| S1 Auge seitlich | 1 heller Fleck (1 Komponente); 48,4/4,0/0,5 dE00 <= 6; 0,028-0,052 x 0,040-0,070 H | 48.0/3.9/0.8 dE00 0.5; 0.030 x 0.049 H; 1 Komp. | OK |
| S2 Augenlage seitlich | 0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle | 0.058 H; 0.811 H | OK |
| S3 Haarkante seitlich | L* <= 22; 19,1/1,2/1,1 dE00 <= 6; Abstand zum Fleck <= 0,01 H | 14.1/2.3/0.6 dE00 3.7; Abstand 0.015 H | ROT |
| S4 Kopf-Oberseite rot seitlich | 27,6/15,3/3,3 dE00 <= 6; a* >= 11 | 27.6/15.4/2.9 dE00 0.3 | OK |
| T1 Schwanz Beginn | 0,15 H hinter Scheitel, 0,767 H ueber Sohle (+-0,03) | 0.172 H hinter, 0.742 H ueber (Id 16 an Id 7/8) | OK |
| T2 Biegung | Lage dx -0,245 dy +0,036 (+-0,03) nach t2_mass_v4 (Original -0.268 / 0.056); Drehung 68.4 +-5.0 Grad (Original 70.1/65.8/69.5) | dx -0.280 dy 0.100; 70.6 Grad (Bauer-Mittellinie: dx -0.262 dy 0.061) | ROT |
| T3 Knick (Taille) | dx -0,265 dy +0,257 (+-0,03) | dx -0.248 dy 0.265 | OK |
| T4 Ende | dx -0,245 dy +0,397 (+-0,03); 0,37 H ueber Sohle | dx -0.216 dy 0.390; 0.353 H ueber Sohle | OK |
| TA Schwanz absolut | Beginn (357,243), Knick (290,308), Ende (295,343) je Achse +-0,03 H | Beginn (358,238) +0.004/-0.019 H; Knick (293,308) +0.010/-0.000 H; Ende (301,341) +0.022/-0.008 H | OK |
| T5 Breite nach oben | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01 | 0.028/0.080/0.140/0.131 H; Abfall 0.008 | OK |
| T5q Breite quer (unabhaengig) | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01 (Original 3 Masken: Abfall 0.016/0.020/0.000) | 0.032/0.080/0.138/0.127 H; Abfall 0.061; Profil f 0..1 [0.023, 0.032, 0.045, 0.08, 0.188, 0.138, 0.129, 0.127, 0.136, 0.142, 0.142] | ROT |
| T6 Endstueck | B 0,025-0,05; L 0,12-0,17 H; 75-95 Grad; Taille <= 0,035 und schmalste | B 0.036; L 0.129 H; 76 Grad (vorn); Taille 0.021, schmalste davor 0.021 | OK |
| T8 Schwanzfarbe | F15 9,3/11,4/1,5; F16 8,7/8,4/2,2; F17 26,8/-0,1/2,4; F18 15,2/3,2/1,6; dE00 <= 6; a* Wurzel >= 7, Ende <= 6 | F15 9.2/13.8/1.8 dE00 1.9 (264 px); F16 10.8/6.0/3.1 dE00 2.9 (157 px); F17 26.5/0.3/2.0 dE00 0.7 (1214 px); F18 14.4/4.3/2.2 dE00 1.5 (248 px) | OK |
| T9 Schwanzachse | 7 Achsenpunkte: Mittel dE00 <= 6; L*/a* Mittel +-4/+-3 zum Original 11.6/6.1 | Mittel dE00 2.5; L*/a* 11.3/5.5; je Punkt 1.6 2.4 4.6 4.1 0.7 1.8 2.5 | OK |
| T7a Gang koerperfest Lage | Gelenke 1/3/5/7 je <= 0,06 H vom Ziel (Keyframe der gezeichneten Pose), alle Gangtakte | 0 von 47 Takten > 0,06 H; max 0.042 H (h85 {'Beginn': 0.003, 'Biegung': 0.007, 'Knick': 0.013, 'Ende': 0.042}) | OK |
| T7b Biegung/Knick im Gang | Gelenk 3 >= Keyframe - 10 Grad; Gelenk 5 >= halber Keyframe-Winkel | Biegung 51.7-59.2 Grad (Keyframe 51.7-57.4), 0 Takte darunter; Knick 10.9-16.1 (Keyframe 11.8-16.2), 0 darunter | OK |
| T7c keine waagerechte Kette | Strecke Biegung -> Ende in Zielrichtung >= 0,75 x Keyframe | min 0.387 H (Keyframe 0.385 H); 0 Takte darunter [] | OK |
| T7d keine Rueckfaltung | alle Gelenkwinkel <= 90 Grad, ganzer Lauf | max 60.9 Grad; 0 von 144 Takten > 90 | OK |
| Boden Mesh | kein Glied-Mesh und keine Dornspitze unter dem Boden, ganzer Lauf | tiefster Mesh-Punkt -4 E. (h68, Dornspitze -43 E.); 0 Takte darunter (Radien je Gelenk [315, 315, 337, 337, 337, 96, 82, 82] E., Dorn 319 E. (Mesh 22)) | OK |
| F0 Lichtgegenprobe h118 | Rumpf b2 gegen FRAMEDUMP dE00 <= 3 | b2 20.9/7.2/2.4 / Spiel 20.8/7.2/2.4 dE00 0.1 (11173 px) | OK |
| F0 Lichtgegenprobe h135 | Rumpf b2 gegen FRAMEDUMP dE00 <= 3 | b2 19.2/7.2/2.8 / Spiel 19.1/7.1/2.7 dE00 0.1 (12422 px) | OK |
| F F6 | 16.6/17.3/6.2 dE00 <= 5, |dL| <= 4 | 19.5/13.0/5.1 dE00 3.7 dL +2.9 (4026 px) | OK |
| F F7 | 33.3/5.5/2.7 dE00 <= 5, |dL| <= 4 | 31.4/8.7/2.3 dE00 3.6 dL -1.9 (1031 px) | OK |
| F F8 | 16.4/8.3/1.5 dE00 <= 5, |dL| <= 4 | 18.4/9.3/3.0 dE00 2.0 dL +2.0 (1991 px) | OK |
| F F9 | 12.7/-0.9/-1.5 dE00 <= 5, |dL| <= 4 | 14.6/-0.1/-3.5 dE00 2.4 dL +1.9 (4540 px) | OK |
| F F10 | 11.2/14.0/2.5 dE00 <= 5, |dL| <= 4 | 13.6/11.1/1.7 dE00 2.8 dL +2.4 (324 px) (Id 12 (Klaue (B)-rechts); Id 15: nan/nan/nan (0 px)) | OK |
| F F14 | 60.0/11.0/1.2 dE00 <= 5, |dL| <= 4 | 57.0/10.8/1.6 dE00 2.8 dL -3.0 (890 px) | OK |
| F F19 | 18.4/0.5/-3.5 dE00 <= 5, |dL| <= 4 | 17.8/0.2/-4.7 dE00 1.2 dL -0.6 (1593 px) | OK |
| F Mittel | Mittel dE00 <= 4 | 2.65 | OK |
| F F6 Fensterlage | 16.6/17.3/6.2 dE00 <= 5, |dL| <= 4 (rote Id-8-Bildpunkte im Fenster 372-424 x 197-232) | 18.4/16.0/6.8 dE00 1.6 dL +1.8 (1132 px) | OK |
| F F7 Messblatt-Wortlaut (Id ganz) | 33.3/5.5/2.7 dE00 <= 5, |dL| <= 4 | 22.7/5.8/1.3 dE00 8.1 dL -10.6 (2181 px) (Runde 3: Mesh 10 hat eine eigene CLUT-Zeile 489 (O1). Das Fenster F7 (x 307-347, y 167-212) liegt nur auf der hellen Schulterkuppe; der ganze Oberarm (B)-links ist im Original 17,3 L* (Sicht-Pruefung V4 r2 1.4, Box 300-345 x 186-250) - Widerspruch im Messblatt-Wortlaut, Offen) | ROT |
| F F8 Messblatt-Wortlaut (Id ganz) | 16.4/8.3/1.5 dE00 <= 5, |dL| <= 4 | 19.6/8.4/2.6 dE00 2.4 dL +3.2 (3293 px) (Runde 3: Mesh 10 mit eigener CLUT-Zeile 489 (O1), Verteilung des F8-Fensters fuer Id 10 ganz (r2_koerper_farben oberarm_br)) | OK |
| FS Oberarm (B)-links unten | 11.0/2.0/0.9, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 14.6/3.0/0.5 dE00 2.7 dL +3.6; Rotanteil 0.05 (1150 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(300, 213, 345, 250)]) | OK |
| FS Oberarm (B)-rechts unten | 20.9/3.2/-0.4, Rotanteil 0.02; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 21.5/7.0/2.1 dE00 4.7 dL +0.6; Rotanteil 0.49 (1302 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(455, 228, 500, 262)]) | ROT |
| FS Rumpf + Schultern | 17.3/4.8/1.9, Rotanteil 0.31; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 18.0/7.1/2.7 dE00 2.6 dL +0.7; Rotanteil 0.39 (13358 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(352, 196, 448, 292), (318, 160, 360, 190), (425, 165, 470, 195)]) | OK |
| FS Unterarm (B)-rechts = (W)-links Id 11 | 29.3/0.7/-1.6, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 27.3/1.7/-1.5 dE00 2.1 dL -1.9; Rotanteil 0.00 (1486 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(468, 268, 500, 330)]) | OK |
| FS Unterschenkel | 15.1/1.7/-1.5, Rotanteil 0.03; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 18.2/4.5/0.6 dE00 4.5 dL +3.2; Rotanteil 0.15 (1901 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(356, 352, 392, 430)]) | OK |
| FS Unterarm (B)-links = (W)-rechts Id 14 | 9.0/-3.8/3.5, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15; Anteil L* > 30 <= Original + 0,05 | 11.4/-2.0/2.3 dE00 2.9 dL +2.3; Rotanteil 0.00 (2041 px); Anteil L* > 30 0.000 (Original 0.000, <= +0,05); L* p90 15.5 (Original 11.2) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(298, 262, 325, 340)]) | OK |
| F0 Gueltigkeit Farbvergleich | Wand links 64,7/0,3/-2,1 dE00 <= 2 | 65.2/0.2/-2.5 dE00 0.52 | OK |
| TQ Schwanz Querprofil | x 310/320/330/340: Stufe auf 3 px <= 9.6 L*, Plateau-Anteil <= 0.31 (Original-Maximum, 3 Masken) | x 310 Stufe 16.6 Plateau 0.40; x 320 Stufe 17.7 Plateau 0.41; x 330 Stufe 19.8 Plateau 0.41; x 340 Stufe 19.0 Plateau 0.38 (Sicht-Pruefung V4 r3 1.3: V4 r3 16,6-19,9 L*, 0,33-0,43) | ROT |
| TK Schwanz Oberkontur | x 284-332: RMS zur Geraden >= 3.02 px (Original 3.87/4.71/4.69 minus Spanne); Oberkante x 276-292 je Spalte <= 0,03 H vom Original 251/250/243/236/234 | RMS 0.60 px; Oberkante 237/235/232/230/228 (max 0.057 H) (Sicht-Pruefung V4 r3 1.3: V4 r3 0,6 px, Oberkante 237/235/232/230/228; Toleranz Oberkante = TA (0,03 H)) | ROT |
| FO Oberkoerper vorn Rot-Verteilung (Buehne) | Rot Mitte/aussen >= 2,75 (Original 2.75); Schultern a*>12 <= 0,05 (Original 0.031); Anteil a*>18 <= 0.041 (Original) | Mitte/aussen 1.21 (Drittel 0.31/0.47/0.47); Schultern 0.089; a*>18 0.053 (Sicht-Pruefung V4 r3 1.5 / Riegelvorschlag Abschnitt 3; V4 r3 1,21 / 0,089) | ROT |
| KS Kopf-Struktur Buehne 320 x 240 | Innenflaeche Gradient 29.1-40.4 (Original); Naeherung des Spielbilds | Gradient 28.0; glatt 0.109 (Buehne Pose 135 ueber 320 x 240 (INTER_AREA wie c6.port); V4 r3 28,0) | ROT |
| KS Kopf-Struktur Spielbild h118 | Innenflaeche Gradient 29.1-40.4 (Original, Box +-3 px); glatte Pixel <= 0.098 (Original max) | Gradient 26.9; glatt 0.126 (Sicht-Pruefung V4 r3 1.1: V4 r3 h135 27,2 / 0,197, h118 26,9 / 0,126) | ROT |
| FO Oberkoerper Spielbild h118 | Rot Mitte/aussen >= 2,75 (Video 2.91); Schultern a*>12 <= 0,05 (Video 0.049); Anteil a*>18 <= Video 0.019 | Mitte/aussen 1.18 (Drittel 0.35/0.44/0.40); Schultern 0.128; a*>18 0.046; L* p10 9.8 (Video 7.5) (Sicht-Pruefung V4 r3 1.5 (Polygone von Hand, oberkoerper_spiel.py); V4 r3 h118 1,18 / 0,13) | ROT |
| KS Kopf-Struktur Spielbild h135 | Innenflaeche Gradient 29.1-40.4 (Original, Box +-3 px); glatte Pixel <= 0.098 (Original max) | Gradient 27.2; glatt 0.197 (Sicht-Pruefung V4 r3 1.1: V4 r3 h135 27,2 / 0,197, h118 26,9 / 0,126) | ROT |
| FO Oberkoerper Spielbild h135 | Rot Mitte/aussen >= 2,75 (Video 2.75); Schultern a*>12 <= 0,05 (Video 0.031); Anteil a*>18 <= Video 0.041 | Mitte/aussen 1.06 (Drittel 0.33/0.38/0.39); Schultern 0.116; a*>18 0.048; L* p10 9.9 (Video 7.5) (Sicht-Pruefung V4 r3 1.5 (Polygone von Hand, oberkoerper_spiel.py); V4 r3 h118 1,18 / 0,13) | ROT |
