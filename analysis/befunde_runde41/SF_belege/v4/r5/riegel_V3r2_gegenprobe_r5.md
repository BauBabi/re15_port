| Mass | Soll | Ist | |
|---|---|---|---|
| G1 Koerper-Geometrie = Ref2 | Meshes 0-8, 10-15: Ecken, Normalen, Flaechen, UV gleich (O1: CLUT-Feld frei); Skelett/Keyframes 0-15, EDD gleich | Geometrie abweichend [8]; CLUT-Feld geaendert keine; byte-gleich 14 von 15; Skelett True; Keyframes True; EDD True; uebrige Sektionen True; Ref2 sha256 cfd1ac42 | ROT |
| M1 Dreiecke Schwanz | <= 212 | 160 ([30, 20, 20, 20, 20, 20, 30]) | OK |
| M2 Dreiecke Kopf | <= 118 | 109 | OK |
| M3 Farbtiefe | 8-bpp-TIM, Texel aus der CLUT | 8 bpp True; CLUT-Zeilen Kopf [485, 486], Schwanz [482], Koerper [480, 481, 482, 484] (TIM 7 Zeilen) | OK |
| M4 Texeldichte | Kopf und Schwanz 0,045-0,091 Texel/E. | Kopf 0.066; Schwanz [0.046, 0.053, 0.054, 0.059, 0.069, 0.08, 0.082] | OK |
| K1 Augen vorn | 1 Komponente; Kontrast >= 20 (Original 29,5) | 2 Komp.; 7.7 | ROT |
| K2 Augenfleck Farbe | 46,9/7,2/-1,1 dE00 <= 6; L* >= KV5 + 20 | 45.0/6.6/1.0 dE00 2.6; L* 45.0 vs KV5 18.8 + 20 | OK |
| K3 Augenfleck Groesse | B 0,016-0,036 / H 0,023-0,043 H | 0.016 x 0.019 H (6 x 7 px) | ROT |
| K4 Augenlage vorn | (B)-links 0,035-0,065 H; 0,07-0,12 H unter OK | -0.035 H links; 0.032 H unter OK | ROT |
| K5 Haarfeld vorn | 13,3/7,4/1,5 dE00 <= 6; L* <= Auge - 25 | 31.3/12.5/3.6 dE00 13.6; L* 31.3 vs 20.0 | ROT |
| K6 zweites Auge verdeckt | L* <= 18 (Original 12,9) | L* 34.6 (Box (338, 162, 343, 168)) | ROT |
| K7 Nase | 18,2/7,1/1,3 dE00 <= 6; L* zwischen Haar und Auge | 23.9/15.4/6.0 dE00 8.5 | ROT |
| K8 Kinn/Maulzone | 9,9/9,9/3,8 dE00 <= 6; Anteil L*>35 <= 0,05 | 41.3/3.9/3.3 dE00 23.9; Anteil 0.700 | ROT |
| K9 Kopf-Oberseite rot vorn | 22,6/15,4/5,2 dE00 <= 6; a* >= 11 | 40.5/5.6/0.9 dE00 16.6 | ROT |
| K10 Kopfbox vorn | 0,14-0,18 x 0,15-0,20 H | 0.126 x 0.170 H | ROT |
| K11 Kopf-Struktur vorn | Lum-Gradient Kopf 41.0 +-6.1 (Original, 9 Boxen 39.4-42.9, Viertel 34.9-47.0); Kopf >= Rumpf (Original 1.57 x) | Kopf 28.5, Rumpf 36.5 (0.78 x) (Pruefung V4 r2 Sicht 1.1: Runde 2 Kopf 27,8 / Rumpf 32,4) | ROT |
| S1 Auge seitlich | 1 heller Fleck (1 Komponente); 48,4/4,0/0,5 dE00 <= 6; 0,028-0,052 x 0,040-0,070 H | 50.3/5.7/-0.0 dE00 2.7; 0.061 x 0.057 H; 2 Komp. | ROT |
| S2 Augenlage seitlich | 0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle | 0.086 H; 0.906 H | ROT |
| S3 Haarkante seitlich | L* <= 22; 19,1/1,2/1,1 dE00 <= 6; Abstand zum Fleck <= 0,01 H | 48.8/3.3/5.6 dE00 24.5; Abstand 0.040 H | ROT |
| S4 Kopf-Oberseite rot seitlich | 27,6/15,3/3,3 dE00 <= 6; a* >= 11 | 45.4/5.4/-0.5 dE00 17.4 | ROT |
| T1 Schwanz Beginn | 0,15 H hinter Scheitel, 0,767 H ueber Sohle (+-0,03) | 0.222 H hinter, 0.761 H ueber (Id 16 an Id 7/8) | ROT |
| T2 Biegung | Lage dx -0,245 dy +0,036 (+-0,03) nach t2_mass_v4 (Original -0.268 / 0.056); Drehung 68.4 +-5.0 Grad (Original 70.1/65.8/69.5) | dx -0.276 dy 0.157; 22.9 Grad (Bauer-Mittellinie: dx -0.003 dy -0.137) | ROT |
| T3 Knick (Taille) | dx -0,265 dy +0,257 (+-0,03) | dx -0.318 dy 0.403 | ROT |
| T4 Ende | dx -0,245 dy +0,397 (+-0,03); 0,37 H ueber Sohle | dx -0.320 dy 0.494; 0.268 H ueber Sohle | ROT |
| TA Schwanz absolut | Beginn (357,243), Knick (290,308), Ende (295,343) je Achse +-0,03 H | Beginn (345,233) -0.045/-0.038 H; Knick (261,339) -0.109/+0.119 H; Ende (260,363) -0.131/+0.077 H | ROT |
| T5 Breite nach oben | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01 | 0.040/0.066/0.091/0.131 H; Abfall 0.000 | ROT |
| T5q Breite quer (unabhaengig) | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01 (Original 3 Masken: Abfall 0.016/0.020/0.000) | 0.036/0.066/0.087/0.127 H; Abfall 0.002; Profil f 0..1 [0.036, 0.036, 0.051, 0.066, 0.074, 0.087, 0.1, 0.127, 0.142, 0.165, 0.172] | ROT |
| T6 Endstueck | B 0,025-0,05; L 0,12-0,17 H; 75-95 Grad; Taille <= 0,035 und schmalste | B 0.032; L 0.091 H; 88 Grad (hinten); Taille 0.032, schmalste davor 0.000 | ROT |
| T8 Schwanzfarbe | F15 9,3/11,4/1,5; F16 8,7/8,4/2,2; F17 26,8/-0,1/2,4; F18 15,2/3,2/1,6; dE00 <= 6; a* Wurzel >= 7, Ende <= 6 | F15 21.1/8.3/0.1 dE00 8.3 (156 px); F16 22.0/8.1/4.9 dE00 9.0 (161 px); F17 22.9/3.7/3.7 dE00 6.0 (29 px); F18 21.3/6.7/1.5 dE00 5.7 (149 px) | ROT |
| T9 Schwanzachse | 7 Achsenpunkte: Mittel dE00 <= 6; L*/a* Mittel +-4/+-3 zum Original 11.6/6.1 | Mittel dE00 8.7; L*/a* 20.3/1.2; je Punkt 12.4 11.3 6.9 10.7 3.4 5.8 10.7 | ROT |
| T7a Gang koerperfest Lage | Gelenke 1/3/5/7 je <= 0,06 H vom Ziel (Keyframe der gezeichneten Pose), alle Gangtakte | 47 von 47 Takten > 0,06 H; max 0.634 H (h85 {'Beginn': 0.113, 'Biegung': 0.294, 'Knick': 0.468, 'Ende': 0.634}) | ROT |
| T7b Biegung/Knick im Gang | Gelenk 3 >= Keyframe - 10 Grad; Gelenk 5 >= halber Keyframe-Winkel | Biegung 2.9-16.2 Grad (Keyframe 14.6-16.1), 1 Takte darunter; Knick 6.9-16.1 (Keyframe 10.8-16.1), 0 darunter | ROT |
| T7c keine waagerechte Kette | Strecke Biegung -> Ende in Zielrichtung >= 0,75 x Keyframe | min 0.162 H (Keyframe 0.498 H); 14 Takte darunter [85, 90, 91, 92, 93, 94, 95, 96] | ROT |
| T7d keine Rueckfaltung | alle Gelenkwinkel <= 90 Grad, ganzer Lauf | max 16.2 Grad; 0 von 144 Takten > 90 | OK |
| Boden Mesh | kein Glied-Mesh und keine Dornspitze unter dem Boden, ganzer Lauf | tiefster Mesh-Punkt +96 E. (h33, Dornspitze +83 E.); 19 Takte darunter (Radien je Gelenk [380, 380, 375, 351, 273, 195, 136, 95] E., Dorn 605 E. (Mesh 22)) | ROT |
| F0 Lichtgegenprobe h118 | Rumpf b2 gegen FRAMEDUMP dE00 <= 3 | b2 32.5/7.5/4.5 / Spiel 32.6/7.5/4.4 dE00 0.1 (12131 px) | OK |
| F0 Lichtgegenprobe h135 | Rumpf b2 gegen FRAMEDUMP dE00 <= 3 | b2 31.7/7.2/4.5 / Spiel 31.7/7.1/4.4 dE00 0.2 (13693 px) | OK |
| F F6 (Fensterlage, O7) | 16.6/17.3/6.2 dE00 <= 5, |dL| <= 4 | 27.6/19.6/9.4 dE00 8.1 dL +11.0 (981 px) | ROT |
| F F7 (Fensterlage, O7) | 33.3/5.5/2.7 dE00 <= 5, |dL| <= 4 | 30.7/13.6/6.4 dE00 7.8 dL -2.6 (962 px) (O7: obere 58 % von Id 13 (Fensterlage) und FS Oberarm (B)-links unten (ROT)) | ROT |
| F F8 | 16.4/8.3/1.5 dE00 <= 5, |dL| <= 4 | 31.6/9.7/4.8 dE00 11.3 dL +15.2 (1991 px) | ROT |
| F F9 | 12.7/-0.9/-1.5 dE00 <= 5, |dL| <= 4 | 25.0/-2.2/-7.9 dE00 10.0 dL +12.3 (3629 px) | ROT |
| F F10 | 11.2/14.0/2.5 dE00 <= 5, |dL| <= 4 | 16.0/10.9/2.3 dE00 4.0 dL +4.8 (82 px) (Id 12 (Klaue (B)-rechts); Id 15: nan/nan/nan (0 px)) | ROT |
| F F14 | 60.0/11.0/1.2 dE00 <= 5, |dL| <= 4 | 41.7/9.6/5.4 dE00 18.7 dL -18.3 (1212 px) | ROT |
| F F19 | 18.4/0.5/-3.5 dE00 <= 5, |dL| <= 4 | 21.3/-2.9/-7.8 dE00 5.9 dL +2.9 (1142 px) | ROT |
| F Mittel | Mittel dE00 <= 4 | 9.39 | ROT |
| F F6 Messblatt-Wortlaut (Id ganz) | 16.6/17.3/6.2 dE00 <= 5, |dL| <= 4 (INFO, O7: kein Riegel) | 29.2/12.9/6.5 dE00 9.6 dL +12.6 (5937 px) (Entscheid O7: gemessen wird an der Fensterlage) | INFO |
| F F7 Messblatt-Wortlaut (Id ganz) | 33.3/5.5/2.7 dE00 <= 5, |dL| <= 4 (INFO, O7: kein Riegel) | 28.8/9.9/4.7 dE00 5.7 dL -4.5 (2112 px) (Entscheid O7: gemessen wird an der Fensterlage) | INFO |
| F F8 Messblatt-Wortlaut (Id ganz) | 16.4/8.3/1.5 dE00 <= 5, |dL| <= 4 | 31.8/8.6/4.3 dE00 11.4 dL +15.4 (3293 px) (Runde 3: Mesh 10 mit eigener CLUT-Zeile 489 (O1)) | ROT |
| FS Oberarm (B)-links unten | 11.0/2.0/0.9, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 27.2/6.8/3.3 dE00 12.5 dL +16.2; Rotanteil 0.51 (1150 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(300, 213, 345, 250)]) | ROT |
| FS Oberarm (B)-rechts unten | 20.9/3.2/-0.4, Rotanteil 0.02; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 32.2/6.9/3.4 dE00 9.9 dL +11.3; Rotanteil 0.58 (1302 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(455, 228, 500, 262)]) | ROT |
| FS Rumpf + Schultern | 17.3/4.8/1.9, Rotanteil 0.31; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 31.6/7.4/4.5 dE00 10.9 dL +14.3; Rotanteil 0.49 (14350 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(352, 196, 448, 292), (318, 160, 360, 190), (425, 165, 470, 195)]) | ROT |
| FS Unterarm (B)-rechts = (W)-links Id 11 | 29.3/0.7/-1.6, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 20.8/4.1/0.0 dE00 7.8 dL -8.5; Rotanteil 0.06 (1486 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(468, 268, 500, 330)]) | ROT |
| FS Unterschenkel | 15.1/1.7/-1.5, Rotanteil 0.03; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 27.6/10.6/5.1 dE00 13.9 dL +12.6; Rotanteil 0.72 (1901 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(356, 352, 392, 430)]) | ROT |
| FS Unterarm (B)-links = (W)-rechts Id 14 | 9.0/-3.8/3.5, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15; Anteil L* > 30 <= Original + 0,05 | 21.3/4.1/0.0 dE00 14.3 dL +12.3; Rotanteil 0.12 (2041 px); Anteil L* > 30 0.121 (Original 0.000, <= +0,05); L* p90 30.8 (Original 11.2) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(298, 262, 325, 340)]) | ROT |
| F0 Gueltigkeit Farbvergleich | Wand links 64,7/0,3/-2,1 dE00 <= 2 | 65.2/0.2/-2.5 dE00 0.52 | OK |
| TQ Schwanz Querprofil | x 310/320/330/340: Stufe auf 3 px <= 9.6 L*, Plateau-Anteil <= 0.31 (Original-Maximum, 3 Masken) | x 310 Stufe 6.2 Plateau 0.48; x 320 Stufe 4.6 Plateau 0.29; x 330 Stufe 8.6 Plateau 0.40; x 340 Stufe 4.7 Plateau 0.61 (Sicht-Pruefung V4 r3 1.3: V4 r3 16,6-19,9 L*, 0,33-0,43) | ROT |
| TK Schwanz Oberkontur | x 284-332: RMS zur Geraden >= 3.02 px (Original 3.87/4.71/4.69 minus Spanne); Oberkante x 276-292 je Spalte <= 0,03 H vom Original 251/250/243/236/234 | RMS 2.04 px; Oberkante 246/238/230/226/221 (max 0.049 H) (Sicht-Pruefung V4 r3 1.3: V4 r3 0,6 px, Oberkante 237/235/232/230/228; Toleranz Oberkante = TA (0,03 H)) | ROT |
| TN Schwanz Quernaht waagerecht | groesster Sprung dE76 ueber 3 Spalten (x 280-376, Mitte der Maske) <= 9.5 (Original-Maximum, 3 Masken 9.5/9.5/9.5) | 16.9 bei x 354 (Sicht-Pruefung V4 r4 1.3: V4 r4 16,7 an Mesh 17|18 / 16|17) | ROT |
| TV Schwanz Verlauf entlang der Achse | L*/a* an Wurzel, Mitte, Biegung, senkrecht, Endstueck je +-4 / +-3 zum Original (Toleranz wie T9; Stationen r5_mass.STATIONEN) | Wurzel 23.6/6.1 (Original 9.0/11.2) ROT; Mitte 20.7/8.9 (Original 10.3/8.1) ROT; Biegung 22.9/5.7 (Original 23.4/6.2); senkrecht 22.0/8.9 (Original 10.2/2.7) ROT; Endstueck 21.0/7.8 (Original 13.1/3.7) ROT (Sicht-Pruefung V4 r4 1.3: Helligkeit verkehrt, an der Biegung L* 7,3) | ROT |
| TR Schwanz Rotanteil waagerecht | Anteil a* > 12 (x 284-356, y < 262) 0.102 +-0,05 (Original 0.099/0.110/0.097; Toleranz wie FO-Schultern) | 0.021 (Sicht-Pruefung V4 r4 1.3: V4 r4 0,19) | ROT |
| AF Ausreisserfarben Pose 135 | je Mesh 0-14 Anteil Bildpunkte > 10 dE76 von jeder Farbe der Original-Region (Vorlage 1) <= 0.01; gelbe Bildpunkte gezaehlt | max 0.379 (Id 3); ueber der Grenze {0: 0.025, 1: 0.098, 2: 0.055, 3: 0.379, 4: 0.095, 5: 0.062, 6: 0.358, 8: 0.013, 11: 0.107, 12: 0.091, 13: 0.052, 14: 0.184}; gelb 0 px (je Id -) (Pruefung V4 r4 Sicht 1.5: V4 r4 Mesh 13 45 gelbe Texel; Id 15 ohne Region (Referenz-2-Farben)) | ROT |
| AF Ausreisserfarben Pose 85 | je Mesh 0-14 Anteil Bildpunkte > 10 dE76 von jeder Farbe der Original-Region (Vorlage 2) <= 0.01; gelbe Bildpunkte gezaehlt | max 0.582 (Id 6); ueber der Grenze {0: 0.048, 1: 0.259, 2: 0.082, 3: 0.489, 4: 0.037, 5: 0.155, 6: 0.582, 8: 0.021, 9: 0.024, 12: 0.071, 13: 0.269, 14: 0.059}; gelb 0 px (je Id -) (Pruefung V4 r4 Sicht 1.5: V4 r4 Mesh 13 45 gelbe Texel; Id 15 ohne Region (Referenz-2-Farben)) | ROT |
| FO Oberkoerper vorn Rot-Verteilung (Buehne) | Rot Mitte/aussen >= 2,75 (Original 2.75); Schultern a*>12 <= 0,05 (Original 0.031); Anteil a*>18 <= 0.041 (Original) | Mitte/aussen 1.08 (Drittel 0.56/0.60/0.56); Schultern 0.171; a*>18 0.089 (Sicht-Pruefung V4 r3 1.5 / Riegelvorschlag Abschnitt 3; V4 r3 1,21 / 0,089) | ROT |
| KS Kopf-Struktur Buehne 320 x 240 | Innenflaeche Gradient 29.1-40.4 (Original); Naeherung des Spielbilds | Gradient 31.1; glatt 0.106 (Buehne Pose 135 ueber 320 x 240 (INTER_AREA wie c6.port); V4 r3 28,0) | OK |
| FR Kraeftiges Rot Spielbild h85 | Anzahl a* > 22 <= max(10 x Video 2, 1 Promille 41) = 41 | 765 (Pruefung V4 r4 Sicht 1.5: V4 r4 h85 1170 / h100 531 (Video 2 / 107)) | ROT |
| FR Kraeftiges Rot Spielbild h100 | Anzahl a* > 22 <= max(10 x Video 107, 1 Promille 32) = 1070 | 682 (Pruefung V4 r4 Sicht 1.5: V4 r4 h85 1170 / h100 531 (Video 2 / 107)) | OK |
| FR Kraeftiges Rot Spielbild h118 | Anzahl a* > 22 <= max(10 x Video 0, 1 Promille 20) = 20 | 984 (Pruefung V4 r4 Sicht 1.5: V4 r4 h85 1170 / h100 531 (Video 2 / 107)) | ROT |
| FR Kraeftiges Rot Spielbild h135 | Anzahl a* > 22 <= max(10 x Video 156, 1 Promille 23) = 1560 | 1094 (Pruefung V4 r4 Sicht 1.5: V4 r4 h85 1170 / h100 531 (Video 2 / 107)) | OK |
| KS Kopf-Struktur Spielbild h118 | Innenflaeche Gradient 29.1-40.4 (Original, Box +-3 px); glatte Pixel <= 0.098 (Original max) | Gradient 33.0; glatt 0.044 (Sicht-Pruefung V4 r3 1.1: V4 r3 h135 27,2 / 0,197, h118 26,9 / 0,126) | OK |
| FO Oberkoerper Spielbild h118 | Rot Mitte/aussen >= 2,75 (Video 2.91); Schultern a*>12 <= 0,05 (Video 0.049); Anteil a*>18 <= Video 0.019 | Mitte/aussen 1.10 (Drittel 0.54/0.52/0.41); Schultern 0.227; a*>18 0.081; L* p10 15.7 (Video 7.5) (Sicht-Pruefung V4 r3 1.5 (Polygone von Hand, oberkoerper_spiel.py); V4 r3 h118 1,18 / 0,13) | ROT |
| KS Kopf-Struktur Spielbild h135 | Innenflaeche Gradient 29.1-40.4 (Original, Box +-3 px); glatte Pixel <= 0.098 (Original max) | Gradient 32.3; glatt 0.104 (Sicht-Pruefung V4 r3 1.1: V4 r3 h135 27,2 / 0,197, h118 26,9 / 0,126) | ROT |
| FO Oberkoerper Spielbild h135 | Rot Mitte/aussen >= 2,75 (Video 2.75); Schultern a*>12 <= 0,05 (Video 0.031); Anteil a*>18 <= Video 0.041 | Mitte/aussen 1.03 (Drittel 0.50/0.46/0.39); Schultern 0.223; a*>18 0.078; L* p10 16.6 (Video 7.5) (Sicht-Pruefung V4 r3 1.5 (Polygone von Hand, oberkoerper_spiel.py); V4 r3 h118 1,18 / 0,13) | ROT |
