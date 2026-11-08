| Mass | Soll | Ist | |
|---|---|---|---|
| G1 Koerper-Geometrie = Ref2 | Meshes 0-8, 10-15: Ecken, Normalen, Flaechen, UV gleich (O1: CLUT-Feld frei); Skelett/Keyframes 0-15, EDD gleich | Geometrie abweichend keine; CLUT-Feld geaendert {2: '[480] -> [491]', 3: '[480] -> [491]', 5: '[480] -> [491]', 6: '[480] -> [491]', 10: '[480] -> [489]', 13: '[480] -> [490]', 14: '[482] -> [488]'}; byte-gleich 8 von 15; Skelett True; Keyframes True; EDD True; uebrige Sektionen True; Ref2 sha256 cfd1ac42 | OK |
| M1 Dreiecke Schwanz | <= 212 | 198 ([33, 22, 44, 22, 22, 22, 33]) | OK |
| M2 Dreiecke Kopf | <= 118 | 109 | OK |
| M3 Farbtiefe | 8-bpp-TIM, Texel aus der CLUT | 8 bpp True; CLUT-Zeilen Kopf [486], Schwanz [487], Koerper [480, 481, 482, 484, 488, 489, 490, 491] (TIM 12 Zeilen) | OK |
| M4 Texeldichte | Kopf und Schwanz 0,045-0,091 Texel/E. | Kopf 0.057; Schwanz [0.06, 0.07, 0.065, 0.072, 0.07, 0.069, 0.065] | OK |
| K1 Augen vorn | 1 Komponente; Kontrast >= 20 (Original 29,5) | 1 Komp.; 25.3 | OK |
| K2 Augenfleck Farbe | 46,9/7,2/-1,1 dE00 <= 6; L* >= KV5 + 20 | 47.8/8.5/-1.4 dE00 1.6; L* 47.8 vs KV5 22.4 + 20 | OK |
| K3 Augenfleck Groesse | B 0,016-0,036 / H 0,023-0,043 H | 0.025 x 0.025 H (9 x 9 px) | OK |
| K4 Augenlage vorn | (B)-links 0,035-0,065 H; 0,07-0,12 H unter OK | 0.056 H links; 0.101 H unter OK | OK |
| K5 Haarfeld vorn | 13,3/7,4/1,5 dE00 <= 6; L* <= Auge - 25 | 13.9/8.1/1.0 dE00 0.9; L* 13.9 vs 22.8 | OK |
| K6 zweites Auge verdeckt | L* <= 18 (Original 12,9) | L* 12.3 (Box (369, 187, 377, 195)) | OK |
| K7 Nase | 18,2/7,1/1,3 dE00 <= 6; L* zwischen Haar und Auge | 18.1/7.0/-0.1 dE00 1.2 | OK |
| K8 Kinn/Maulzone | 9,9/9,9/3,8 dE00 <= 6; Anteil L*>35 <= 0,05 | 10.9/7.9/4.1 dE00 2.1; Anteil 0.000 | OK |
| K9 Kopf-Oberseite rot vorn | 22,6/15,4/5,2 dE00 <= 6; a* >= 11 | 20.9/13.0/5.4 dE00 2.2 | OK |
| K10 Kopfbox vorn | 0,14-0,18 x 0,15-0,20 H | 0.154 x 0.187 H | OK |
| K11 Kopf-Struktur vorn | Lum-Gradient Kopf 41.0 +-6.1 (Original, 9 Boxen 39.4-42.9, Viertel 34.9-47.0); Kopf >= Rumpf (Original 1.57 x) | Kopf 37.8, Rumpf 27.1 (1.40 x) (Pruefung V4 r2 Sicht 1.1: Runde 2 Kopf 27,8 / Rumpf 32,4) | OK |
| S1 Auge seitlich | 1 heller Fleck (1 Komponente); 48,4/4,0/0,5 dE00 <= 6; 0,028-0,052 x 0,040-0,070 H | 48.5/3.1/0.9 dE00 1.2; 0.042 x 0.049 H; 1 Komp. | OK |
| S2 Augenlage seitlich | 0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle | 0.054 H; 0.809 H | OK |
| S3 Haarkante seitlich | L* <= 22; 19,1/1,2/1,1 dE00 <= 6; Abstand zum Fleck <= 0,01 H | 17.2/2.8/0.6 dE00 2.5; Abstand 0.004 H | OK |
| S4 Kopf-Oberseite rot seitlich | 27,6/15,3/3,3 dE00 <= 6; a* >= 11 | 27.5/11.4/3.2 dE00 3.1 | OK |
| T1 Schwanz Beginn | 0,15 H hinter Scheitel, 0,767 H ueber Sohle (+-0,03) | 0.172 H hinter, 0.742 H ueber (Id 16 an Id 7/8) | OK |
| T2 Biegung | Lage dx -0,245 dy +0,036 (+-0,03) nach t2_mass_v4 (Original -0.268 / 0.056); Drehung 68.4 +-5.0 Grad (Original 70.1/65.8/69.5) | dx -0.259 dy 0.049; 64.0 Grad (Bauer-Mittellinie: dx -0.266 dy 0.061) | OK |
| T3 Knick (Taille) | dx -0,265 dy +0,257 (+-0,03) | dx -0.239 dy 0.247 | OK |
| T4 Ende | dx -0,245 dy +0,397 (+-0,03); 0,37 H ueber Sohle | dx -0.218 dy 0.388; 0.354 H ueber Sohle | OK |
| TA Schwanz absolut | Beginn (357,243), Knick (290,308), Ende (295,343) je Achse +-0,03 H | Beginn (358,238) +0.004/-0.019 H; Knick (295,303) +0.019/-0.018 H; Ende (300,340) +0.020/-0.010 H | OK |
| T5 Breite nach oben | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01 | 0.032/0.049/0.098/0.163 H; Abfall 0.000 | ROT |
| T5q Breite quer (unabhaengig) | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01 (Original 3 Masken: Abfall 0.016/0.020/0.000) | 0.032/0.047/0.110/0.161 H; Abfall 0.002; Profil f 0..1 [0.025, 0.032, 0.045, 0.047, 0.081, 0.11, 0.133, 0.161, 0.157, 0.142, 0.144] | OK |
| T6 Endstueck | B 0,025-0,05; L 0,12-0,17 H; 75-95 Grad; Taille <= 0,035 und schmalste | B 0.036; L 0.144 H; 82 Grad (vorn); Taille 0.021, schmalste davor 0.021 | OK |
| T8 Schwanzfarbe | F15 9,3/11,4/1,5; F16 8,7/8,4/2,2; F17 26,8/-0,1/2,4; F18 15,2/3,2/1,6; dE00 <= 6; a* Wurzel >= 7, Ende <= 6 | F15 9.1/10.1/2.1 dE00 1.3 (314 px); F16 14.5/8.1/3.3 dE00 3.8 (167 px); F17 22.2/3.2/1.3 dE00 5.9 (1570 px); F18 16.1/3.0/1.2 dE00 0.7 (267 px) | OK |
| T9 Schwanzachse | 7 Achsenpunkte: Mittel dE00 <= 6; L*/a* Mittel +-4/+-3 zum Original 11.6/6.1 | Mittel dE00 4.9; L*/a* 14.4/3.6; je Punkt 6.5 2.9 3.6 13.1 1.3 1.6 5.3 | OK |
| T7a Gang koerperfest Lage | Gelenke 1/3/5/7 je <= 0,06 H vom Ziel (Keyframe der gezeichneten Pose), alle Gangtakte | 0 von 47 Takten > 0,06 H; max 0.056 H (h85 {'Beginn': 0.003, 'Biegung': 0.017, 'Knick': 0.036, 'Ende': 0.056}) | OK |
| T7b Biegung/Knick im Gang | Gelenk 3 >= Keyframe - 10 Grad; Gelenk 5 >= halber Keyframe-Winkel | Biegung 69.5-76.8 Grad (Keyframe 70.2-76.2), 0 Takte darunter; Knick 11.2-16.2 (Keyframe 15.6-16.5), 0 darunter | OK |
| T7c keine waagerechte Kette | Strecke Biegung -> Ende in Zielrichtung >= 0,75 x Keyframe | min 0.370 H (Keyframe 0.364 H); 0 Takte darunter [] | OK |
| T7d keine Rueckfaltung | alle Gelenkwinkel <= 90 Grad, ganzer Lauf | max 79.4 Grad; 0 von 144 Takten > 90 | OK |
| Boden Mesh | kein Glied-Mesh und keine Dornspitze unter dem Boden, ganzer Lauf | tiefster Mesh-Punkt -6 E. (h35, Dornspitze -53 E.); 0 Takte darunter (Radien je Gelenk [315, 315, 343, 343, 135, 98, 83, 83] E., Dorn 308 E. (Mesh 22)) | OK |
| F0 Lichtgegenprobe h118 | Rumpf b2 gegen FRAMEDUMP dE00 <= 3 | b2 22.5/6.0/2.3 / Spiel 22.5/5.9/2.3 dE00 0.0 (11192 px) | OK |
| F0 Lichtgegenprobe h135 | Rumpf b2 gegen FRAMEDUMP dE00 <= 3 | b2 21.0/5.9/2.5 / Spiel 20.9/5.7/2.4 dE00 0.2 (12464 px) | OK |
| F F6 (Fensterlage, O7) | 16.6/17.3/6.2 dE00 <= 5, |dL| <= 4 | 19.0/13.1/5.9 dE00 3.4 dL +2.4 (1522 px) | OK |
| F F7 (Fensterlage, O7) | 33.3/5.5/2.7 dE00 <= 5, |dL| <= 4 | 32.8/4.7/2.1 dE00 1.1 dL -0.5 (1031 px) (O7: obere 58 % von Id 13 (Fensterlage) und FS Oberarm (B)-links unten (OK)) | OK |
| F F8 | 16.4/8.3/1.5 dE00 <= 5, |dL| <= 4 | 16.8/5.7/0.7 dE00 2.7 dL +0.4 (1991 px) | OK |
| F F9 | 12.7/-0.9/-1.5 dE00 <= 5, |dL| <= 4 | 13.5/-0.5/-3.4 dE00 1.9 dL +0.8 (4176 px) | OK |
| F F10 | 11.2/14.0/2.5 dE00 <= 5, |dL| <= 4 | 13.2/10.1/2.0 dE00 3.4 dL +2.0 (291 px) (Id 12 (Klaue (B)-rechts); Id 15: nan/nan/nan (0 px)) | OK |
| F F14 | 60.0/11.0/1.2 dE00 <= 5, |dL| <= 4 | 57.4/9.2/1.6 dE00 2.9 dL -2.6 (890 px) | OK |
| F F19 | 18.4/0.5/-3.5 dE00 <= 5, |dL| <= 4 | 17.7/-0.3/-4.7 dE00 1.6 dL -0.7 (1480 px) | OK |
| F Mittel | Mittel dE00 <= 4 | 2.42 | OK |
| F F6 Messblatt-Wortlaut (Id ganz) | 16.6/17.3/6.2 dE00 <= 5, |dL| <= 4 (INFO, O7: kein Riegel) | 19.4/12.1/4.9 dE00 4.2 dL +2.8 (4170 px) (Entscheid O7: gemessen wird an der Fensterlage) | INFO |
| F F7 Messblatt-Wortlaut (Id ganz) | 33.3/5.5/2.7 dE00 <= 5, |dL| <= 4 (INFO, O7: kein Riegel) | 23.5/3.5/1.5 dE00 7.9 dL -9.8 (2181 px) (Entscheid O7: gemessen wird an der Fensterlage) | INFO |
| F F8 Messblatt-Wortlaut (Id ganz) | 16.4/8.3/1.5 dE00 <= 5, |dL| <= 4 | 19.1/4.9/0.7 dE00 4.0 dL +2.7 (3293 px) (Runde 3: Mesh 10 mit eigener CLUT-Zeile 489 (O1)) | OK |
| FS Oberarm (B)-links unten | 11.0/2.0/0.9, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 14.6/2.4/0.9 dE00 2.4 dL +3.6; Rotanteil 0.00 (1150 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(300, 213, 345, 250)]) | OK |
| FS Oberarm (B)-rechts unten | 20.9/3.2/-0.4, Rotanteil 0.02; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 22.4/3.8/0.7 dE00 1.6 dL +1.5; Rotanteil 0.00 (1302 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(455, 228, 500, 262)]) | OK |
| FS Rumpf + Schultern | 17.3/4.8/1.9, Rotanteil 0.31; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 20.0/5.9/2.5 dE00 2.2 dL +2.7; Rotanteil 0.31 (13358 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(352, 196, 448, 292), (318, 160, 360, 190), (425, 165, 470, 195)]) | OK |
| FS Unterarm (B)-rechts = (W)-links Id 11 | 29.3/0.7/-1.6, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 27.4/1.5/-1.4 dE00 1.8 dL -1.9; Rotanteil 0.00 (1486 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(468, 268, 500, 330)]) | OK |
| FS Unterschenkel | 15.1/1.7/-1.5, Rotanteil 0.03; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 18.5/2.7/0.5 dE00 3.2 dL +3.4; Rotanteil 0.04 (1901 px) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(356, 352, 392, 430)]) | OK |
| FS Unterarm (B)-links = (W)-rechts Id 14 | 9.0/-3.8/3.5, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15; Anteil L* > 30 <= Original + 0,05 | 11.4/-2.1/2.3 dE00 2.7 dL +2.3; Rotanteil 0.00 (2041 px); Anteil L* > 30 0.000 (Original 0.000, <= +0,05); L* p90 15.4 (Original 11.2) (Sicht-Pruefung V4 r1 1.4 / r2 1.4, Boxen [(298, 262, 325, 340)]) | OK |
| F0 Gueltigkeit Farbvergleich | Wand links 64,7/0,3/-2,1 dE00 <= 2 | 65.2/0.2/-2.5 dE00 0.52 | OK |
| TQ Schwanz Querprofil | x 310/320/330/340: Stufe auf 3 px <= 9.6 L*, Plateau-Anteil <= 0.31 (Original-Maximum, 3 Masken) | x 310 Stufe 7.7 Plateau 0.40; x 320 Stufe 5.9 Plateau 0.41; x 330 Stufe 5.0 Plateau 0.38; x 340 Stufe 6.1 Plateau 0.21 (Sicht-Pruefung V4 r3 1.3: V4 r3 16,6-19,9 L*, 0,33-0,43) | ROT |
| TK Schwanz Oberkontur | x 284-332: RMS zur Geraden >= 3.02 px (Original 3.87/4.71/4.69 minus Spanne); Oberkante x 276-292 je Spalte <= 0,03 H vom Original 251/250/243/236/234 | RMS 3.69 px; Oberkante 249/243/238/233/228 (max 0.027 H) (Sicht-Pruefung V4 r3 1.3: V4 r3 0,6 px, Oberkante 237/235/232/230/228; Toleranz Oberkante = TA (0,03 H)) | OK |
| TN Schwanz Quernaht waagerecht | groesster Sprung dE76 ueber 3 Spalten (x 280-376, Mitte der Maske) <= 9.5 (Original-Maximum, 3 Masken 9.5/9.5/9.5) | 10.7 bei x 297 (Sicht-Pruefung V4 r4 1.3: V4 r4 16,7 an Mesh 17|18 / 16|17) | ROT |
| TV Schwanz Verlauf entlang der Achse | L*/a* an Wurzel, Mitte, Biegung, senkrecht, Endstueck je +-4 / +-3 zum Original (Toleranz wie T9; Stationen r5_mass.STATIONEN) | Wurzel 9.7/10.4 (Original 9.0/11.2); Mitte 9.6/8.0 (Original 10.3/8.1); Biegung 22.8/6.2 (Original 23.4/6.2); senkrecht 12.4/3.8 (Original 10.2/2.7); Endstueck 13.6/3.6 (Original 13.1/3.7) (Sicht-Pruefung V4 r4 1.3: Helligkeit verkehrt, an der Biegung L* 7,3) | OK |
| TR Schwanz Rotanteil waagerecht | Anteil a* > 12 (x 284-356, y < 262) 0.102 +-0,05 (Original 0.099/0.110/0.097; Toleranz wie FO-Schultern) | 0.020 (Sicht-Pruefung V4 r4 1.3: V4 r4 0,19) | ROT |
| AF Ausreisserfarben Pose 135 | je Mesh 0-14 Anteil Bildpunkte > 10 dE76 von jeder Farbe der Original-Region (Vorlage 1) <= 0.01; gelbe Bildpunkte gezaehlt | max 0.009 (Id 11); ueber der Grenze keine; gelb 0 px (je Id -) (Pruefung V4 r4 Sicht 1.5: V4 r4 Mesh 13 45 gelbe Texel; Id 15 ohne Region (Referenz-2-Farben)) | OK |
| AF Ausreisserfarben Pose 85 | je Mesh 0-14 Anteil Bildpunkte > 10 dE76 von jeder Farbe der Original-Region (Vorlage 2) <= 0.01; gelbe Bildpunkte gezaehlt | max 0.012 (Id 6); ueber der Grenze {6: 0.012}; gelb 0 px (je Id -) (Pruefung V4 r4 Sicht 1.5: V4 r4 Mesh 13 45 gelbe Texel; Id 15 ohne Region (Referenz-2-Farben)) | ROT |
| FO Oberkoerper vorn Rot-Verteilung (Buehne) | Rot Mitte/aussen >= 2,75 (Original 2.75); Schultern a*>12 <= 0,05 (Original 0.031); Anteil a*>18 <= 0.041 (Original) | Mitte/aussen 6.48 (Drittel 0.10/0.58/0.08); Schultern 0.005; a*>18 0.025 (Sicht-Pruefung V4 r3 1.5 / Riegelvorschlag Abschnitt 3; V4 r3 1,21 / 0,089) | OK |
| KS Kopf-Struktur Buehne 320 x 240 | Innenflaeche Gradient 29.1-40.4 (Original); Naeherung des Spielbilds | Gradient 32.6; glatt 0.054 (Buehne Pose 135 ueber 320 x 240 (INTER_AREA wie c6.port); V4 r3 28,0) | OK |
| FR Kraeftiges Rot Spielbild h85 | Anzahl a* > 22 <= max(10 x Video 2, 1 Promille 41) = 41 | 165 (Pruefung V4 r4 Sicht 1.5: V4 r4 h85 1170 / h100 531 (Video 2 / 107)) | ROT |
| FR Kraeftiges Rot Spielbild h100 | Anzahl a* > 22 <= max(10 x Video 107, 1 Promille 32) = 1070 | 0 (Pruefung V4 r4 Sicht 1.5: V4 r4 h85 1170 / h100 531 (Video 2 / 107)) | OK |
| FR Kraeftiges Rot Spielbild h118 | Anzahl a* > 22 <= max(10 x Video 0, 1 Promille 20) = 20 | 0 (Pruefung V4 r4 Sicht 1.5: V4 r4 h85 1170 / h100 531 (Video 2 / 107)) | OK |
| FR Kraeftiges Rot Spielbild h135 | Anzahl a* > 22 <= max(10 x Video 156, 1 Promille 23) = 1560 | 0 (Pruefung V4 r4 Sicht 1.5: V4 r4 h85 1170 / h100 531 (Video 2 / 107)) | OK |
| KS Kopf-Struktur Spielbild h118 | Innenflaeche Gradient 29.1-40.4 (Original, Box +-3 px); glatte Pixel <= 0.098 (Original max) | Gradient 31.0; glatt 0.048 (Sicht-Pruefung V4 r3 1.1: V4 r3 h135 27,2 / 0,197, h118 26,9 / 0,126) | OK |
| FO Oberkoerper Spielbild h118 | Rot Mitte/aussen >= 2,75 (Video 2.91); Schultern a*>12 <= 0,05 (Video 0.049); Anteil a*>18 <= Video 0.019 | Mitte/aussen 2.73 (Drittel 0.22/0.48/0.14); Schultern 0.016; a*>18 0.016; L* p10 10.8 (Video 7.5) (Sicht-Pruefung V4 r3 1.5 (Polygone von Hand, oberkoerper_spiel.py); V4 r3 h118 1,18 / 0,13) | ROT |
| KS Kopf-Struktur Spielbild h135 | Innenflaeche Gradient 29.1-40.4 (Original, Box +-3 px); glatte Pixel <= 0.098 (Original max) | Gradient 37.5; glatt 0.120 (Sicht-Pruefung V4 r3 1.1: V4 r3 h135 27,2 / 0,197, h118 26,9 / 0,126) | ROT |
| FO Oberkoerper Spielbild h135 | Rot Mitte/aussen >= 2,75 (Video 2.75); Schultern a*>12 <= 0,05 (Video 0.031); Anteil a*>18 <= Video 0.041 | Mitte/aussen 2.76 (Drittel 0.17/0.43/0.14); Schultern 0.020; a*>18 0.020; L* p10 11.3 (Video 7.5) (Sicht-Pruefung V4 r3 1.5 (Polygone von Hand, oberkoerper_spiel.py); V4 r3 h118 1,18 / 0,13) | OK |
