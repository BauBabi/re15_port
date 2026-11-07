| Mass | Soll | Ist | |
|---|---|---|---|
| G1 Koerper-Geometrie = Ref2 | Meshes 0-8, 10-15 byte-gleich; Skelett/Keyframes 0-15, EDD gleich | abweichend [8]; Skelett True; Keyframes True; EDD True; uebrige Sektionen True; Ref2 sha256 cfd1ac42 | ROT |
| M1 Dreiecke Schwanz | <= 212 | 160 ([30, 20, 20, 20, 20, 20, 30]) | OK |
| M2 Dreiecke Kopf | <= 118 | 109 | OK |
| M3 Farbtiefe | 8-bpp-TIM, Texel aus der CLUT | 8 bpp True; CLUT-Zeilen Kopf [485, 486], Schwanz [482] (TIM 7 Zeilen) | OK |
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
| S1 Auge seitlich | 1 heller Fleck (1 Komponente); 48,4/4,0/0,5 dE00 <= 6; 0,028-0,052 x 0,040-0,070 H | 50.3/5.7/-0.0 dE00 2.7; 0.061 x 0.057 H; 2 Komp. | ROT |
| S2 Augenlage seitlich | 0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle | 0.086 H; 0.906 H | ROT |
| S3 Haarkante seitlich | L* <= 22; 19,1/1,2/1,1 dE00 <= 6; Abstand zum Fleck <= 0,01 H | 48.8/3.3/5.6 dE00 24.5; Abstand 0.040 H | ROT |
| S4 Kopf-Oberseite rot seitlich | 27,6/15,3/3,3 dE00 <= 6; a* >= 11 | 45.4/5.4/-0.5 dE00 17.4 | ROT |
| T1 Schwanz Beginn | 0,15 H hinter Scheitel, 0,767 H ueber Sohle (+-0,03) | 0.222 H hinter, 0.761 H ueber (Id 16 an Id 7/8) | ROT |
| T2 Biegung | dx -0,245 dy +0,036 (+-0,03); Drehung 60-100 Grad auf 0,10 H (Tangenten +-0,025 H) | dx -0.003 dy -0.137; 18.4 Grad (Runde-1-Mass 49) | ROT |
| T3 Knick (Taille) | dx -0,265 dy +0,257 (+-0,03) | dx -0.318 dy 0.403 | ROT |
| T4 Ende | dx -0,245 dy +0,397 (+-0,03); 0,37 H ueber Sohle | dx -0.320 dy 0.494; 0.268 H ueber Sohle | ROT |
| TA Schwanz absolut | Beginn (357,243), Knick (290,308), Ende (295,343) je Achse +-0,03 H | Beginn (345,233) -0.045/-0.038 H; Knick (261,339) -0.109/+0.119 H; Ende (260,363) -0.131/+0.077 H | ROT |
| T5 Breite nach oben | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01 | 0.040/0.066/0.091/0.131 H; Abfall 0.000 | ROT |
| T6 Endstueck | B 0,025-0,05; L 0,12-0,17 H; 75-95 Grad; Taille <= 0,035 und schmalste | B 0.032; L 0.091 H; 88 Grad (hinten); Taille 0.032, schmalste davor 0.000 | ROT |
| T8 Schwanzfarbe | F15 9,3/11,4/1,5; F16 8,7/8,4/2,2; F17 26,8/-0,1/2,4; F18 15,2/3,2/1,6; dE00 <= 6; a* Wurzel >= 7, Ende <= 6 | F15 21.1/8.3/0.1 dE00 8.3 (156 px); F16 22.0/8.1/4.9 dE00 9.0 (161 px); F17 22.9/3.7/3.7 dE00 6.0 (29 px); F18 21.3/6.7/1.5 dE00 5.7 (149 px) | ROT |
| T9 Schwanzachse | 7 Achsenpunkte: Mittel dE00 <= 6; L*/a* Mittel +-4/+-3 zum Original 11.6/6.1 | Mittel dE00 8.7; L*/a* 20.3/1.2; je Punkt 12.4 11.3 6.9 10.7 3.4 5.8 10.7 | ROT |
| F F6 | 16.6/17.3/6.2 dE00 <= 5, |dL| <= 4 | 29.2/12.9/6.5 dE00 9.6 dL +12.6 (5937 px) | ROT |
| F F7 | 33.3/5.5/2.7 dE00 <= 5, |dL| <= 4 | 30.7/13.6/6.4 dE00 7.8 dL -2.6 (962 px) | ROT |
| F F8 | 16.4/8.3/1.5 dE00 <= 5, |dL| <= 4 | 31.6/9.7/4.8 dE00 11.3 dL +15.2 (1991 px) | ROT |
| F F9 | 12.7/-0.9/-1.5 dE00 <= 5, |dL| <= 4 | 25.0/-2.2/-7.9 dE00 10.0 dL +12.3 (3629 px) | ROT |
| F F10 | 11.2/14.0/2.5 dE00 <= 5, |dL| <= 4 | 16.0/10.9/2.3 dE00 4.0 dL +4.8 (82 px) (Id 12 (Klaue (B)-rechts); Id 15: nan/nan/nan (0 px)) | ROT |
| F F14 | 60.0/11.0/1.2 dE00 <= 5, |dL| <= 4 | 41.7/9.6/5.4 dE00 18.7 dL -18.3 (1212 px) | ROT |
| F F19 | 18.4/0.5/-3.5 dE00 <= 5, |dL| <= 4 | 21.3/-2.9/-7.8 dE00 5.9 dL +2.9 (1142 px) | ROT |
| F Mittel | Mittel dE00 <= 4 | 9.59 | ROT |
| F F7g (Id ganz, Runde-1-Mass) | 33.3/5.5/2.7 (nur Angabe) | 28.8/9.9/4.7 dE00 5.7 dL -4.5 (2112 px) (Meshes 10/13 teilen 5798 Texel und die CLUT-Zeile 480 (G1): das hellere Licht des Originals auf dem (B)-linken Arm ist mit gemeinsamen Texeln nicht nachzubilden) | OK |
| F F8g (Id ganz, Runde-1-Mass) | 16.4/8.3/1.5 (nur Angabe) | 31.8/8.6/4.3 dE00 11.4 dL +15.4 (3293 px) (Meshes 10/13 teilen 5798 Texel und die CLUT-Zeile 480 (G1): das hellere Licht des Originals auf dem (B)-linken Arm ist mit gemeinsamen Texeln nicht nachzubilden) | OK |
| FS Oberarm (B)-links unten | 11.0/2.0/0.9, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 27.2/6.8/3.3 dE00 12.5 dL +16.2; Rotanteil 0.51 (1150 px) (Sicht-Pruefung V4 r1 1.4, Boxen [(300, 213, 345, 250)]) | ROT |
| FS Oberarm (B)-rechts unten | 20.9/3.2/-0.4, Rotanteil 0.02; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 32.2/6.9/3.4 dE00 9.9 dL +11.3; Rotanteil 0.58 (1302 px) (Sicht-Pruefung V4 r1 1.4, Boxen [(455, 228, 500, 262)]) | ROT |
| FS Rumpf + Schultern | 17.3/4.8/1.9, Rotanteil 0.31; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 31.6/7.4/4.5 dE00 10.9 dL +14.3; Rotanteil 0.49 (14350 px) (Sicht-Pruefung V4 r1 1.4, Boxen [(352, 196, 448, 292), (318, 160, 360, 190), (425, 165, 470, 195)]) | ROT |
| FS Unterarme | 29.3/0.7/-1.6, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 21.1/4.1/0.0 dE00 7.6 dL -8.2; Rotanteil 0.09 (3527 px) (Sicht-Pruefung V4 r1 1.4, Boxen [(468, 268, 500, 330)]) | ROT |
| FS Unterschenkel | 15.1/1.7/-1.5, Rotanteil 0.03; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 27.6/10.6/5.1 dE00 13.9 dL +12.6; Rotanteil 0.72 (1901 px) (Sicht-Pruefung V4 r1 1.4, Boxen [(356, 352, 392, 430)]) | ROT |
| F0 Gueltigkeit Farbvergleich | Wand links 64,7/0,3/-2,1 dE00 <= 2 | 65.2/0.2/-2.5 dE00 0.52 | OK |
