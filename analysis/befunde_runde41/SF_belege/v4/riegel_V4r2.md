| Mass | Soll | Ist | |
|---|---|---|---|
| G1 Koerper-Geometrie = Ref2 | Meshes 0-8, 10-15 byte-gleich; Skelett/Keyframes 0-15, EDD gleich | abweichend keine; Skelett True; Keyframes True; EDD True; uebrige Sektionen True; Ref2 sha256 cfd1ac42 | OK |
| M1 Dreiecke Schwanz | <= 212 | 160 ([30, 20, 20, 20, 20, 20, 30]) | OK |
| M2 Dreiecke Kopf | <= 118 | 109 | OK |
| M3 Farbtiefe | 8-bpp-TIM, Texel aus der CLUT | 8 bpp True; CLUT-Zeilen Kopf [486], Schwanz [487] (TIM 8 Zeilen) | OK |
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
| S1 Auge seitlich | 1 heller Fleck (1 Komponente); 48,4/4,0/0,5 dE00 <= 6; 0,028-0,052 x 0,040-0,070 H | 47.8/3.8/0.9 dE00 0.8; 0.034 x 0.049 H; 1 Komp. | OK |
| S2 Augenlage seitlich | 0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle | 0.058 H; 0.811 H | OK |
| S3 Haarkante seitlich | L* <= 22; 19,1/1,2/1,1 dE00 <= 6; Abstand zum Fleck <= 0,01 H | 13.8/2.0/1.0 dE00 3.7; Abstand 0.011 H | ROT |
| S4 Kopf-Oberseite rot seitlich | 27,6/15,3/3,3 dE00 <= 6; a* >= 11 | 27.3/14.8/3.1 dE00 0.4 | OK |
| T1 Schwanz Beginn | 0,15 H hinter Scheitel, 0,767 H ueber Sohle (+-0,03) | 0.165 H hinter, 0.750 H ueber (Id 16 an Id 7/8) | OK |
| T2 Biegung | dx -0,245 dy +0,036 (+-0,03); Drehung 60-100 Grad auf 0,10 H (Tangenten +-0,025 H) | dx -0.252 dy 0.022; 67.3 Grad (Runde-1-Mass 73) | OK |
| T3 Knick (Taille) | dx -0,265 dy +0,257 (+-0,03) | dx -0.266 dy 0.244 | OK |
| T4 Ende | dx -0,245 dy +0,397 (+-0,03); 0,37 H ueber Sohle | dx -0.243 dy 0.393; 0.357 H ueber Sohle | OK |
| TA Schwanz absolut | Beginn (357,243), Knick (290,308), Ende (295,343) je Achse +-0,03 H | Beginn (360,236) +0.011/-0.027 H; Knick (290,300) -0.001/-0.029 H; Ende (296,340) +0.003/-0.012 H | OK |
| T5 Breite nach oben | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01 | 0.036/0.066/0.127/0.129 H; Abfall 0.008 | OK |
| T6 Endstueck | B 0,025-0,05; L 0,12-0,17 H; 75-95 Grad; Taille <= 0,035 und schmalste | B 0.034; L 0.152 H; 81 Grad (vorn); Taille 0.025, schmalste davor 0.025 | OK |
| T8 Schwanzfarbe | F15 9,3/11,4/1,5; F16 8,7/8,4/2,2; F17 26,8/-0,1/2,4; F18 15,2/3,2/1,6; dE00 <= 6; a* Wurzel >= 7, Ende <= 6 | F15 8.7/11.6/0.3 dE00 1.0 (295 px); F16 9.5/9.3/1.7 dE00 1.1 (216 px); F17 26.6/0.2/1.2 dE00 1.3 (971 px); F18 13.9/3.9/2.0 dE00 1.2 (284 px) | OK |
| T9 Schwanzachse | 7 Achsenpunkte: Mittel dE00 <= 6; L*/a* Mittel +-4/+-3 zum Original 11.6/6.1 | Mittel dE00 2.9; L*/a* 11.3/6.3; je Punkt 0.5 4.5 3.1 3.4 4.6 1.5 2.8 | OK |
| T7 Gang h85 | T1-T4 je +-0,06 H | T1 (0.169, 0.674) (0.093) T2 (0.049, -0.174) (0.294) T3 (-0.17, 0.427) (0.170) T4 (-0.22, 0.544) (0.147) (Laufpose clip 11 bild 64 gier 1893) | ROT |
| T7 Gang h100 | T1-T4 je +-0,06 H | T1 (0.025, 0.549) (0.218) T2 (0.191, -0.12) (0.436) T3 (0.059, -0.034) (0.324) T4 (0.015, -0.024) (0.421) (Laufpose clip 1 bild 10 gier 1515) | ROT |
| T7 Gang h110 | T1-T4 je +-0,06 H | T1 (0.172, 0.742) (0.025) T2 (-0.274, -0.017) (0.053) T3 (-0.308, 0.002) (0.255) T4 (-0.415, 0.248) (0.170) (Laufpose clip 1 bild 20 gier 1515) | ROT |
| T7 Gang h118 | T1-T4 je +-0,06 H | T1 (0.169, 0.75) (0.019) T2 (-0.231, 0.036) (0.014) T3 (-0.261, 0.244) (0.013) T4 (-0.234, 0.388) (0.011) (Laufpose clip 1 bild 28 gier 1515) | OK |
| T7 Gang h125 | T1-T4 je +-0,06 H | T1 (0.169, 0.742) (0.025) T2 (-0.253, -0.027) (0.063) T3 (-0.309, 0.176) (0.081) T4 (-0.288, 0.309) (0.088) (Laufpose clip 1 bild 35 gier 1515) | ROT |
| T7 Gang h135 | T1-T4 je +-0,06 H | T1 (0.172, 0.742) (0.025) T2 (-0.238, 0.042) (0.007) T3 (-0.258, 0.278) (0.021) T4 (-0.268, 0.398) (0.023) (Laufpose clip 1 bild 45 gier 1513) | OK |
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
| F F7g (Id ganz, Runde-1-Mass) | 33.3/5.5/2.7 (nur Angabe) | 22.4/5.8/1.6 dE00 8.3 dL -10.9 (2181 px) (Meshes 10/13 teilen 5798 Texel und die CLUT-Zeile 480 (G1): das hellere Licht des Originals auf dem (B)-linken Arm ist mit gemeinsamen Texeln nicht nachzubilden) | OK |
| F F8g (Id ganz, Runde-1-Mass) | 16.4/8.3/1.5 (nur Angabe) | 20.9/5.6/1.1 dE00 4.1 dL +4.5 (3293 px) (Meshes 10/13 teilen 5798 Texel und die CLUT-Zeile 480 (G1): das hellere Licht des Originals auf dem (B)-linken Arm ist mit gemeinsamen Texeln nicht nachzubilden) | OK |
| FS Oberarm (B)-links unten | 11.0/2.0/0.9, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 14.8/3.1/0.6 dE00 2.8 dL +3.7; Rotanteil 0.05 (1150 px) (Sicht-Pruefung V4 r1 1.4, Boxen [(300, 213, 345, 250)]) | OK |
| FS Oberarm (B)-rechts unten | 20.9/3.2/-0.4, Rotanteil 0.02; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 22.2/4.1/0.1 dE00 1.5 dL +1.2; Rotanteil 0.09 (1302 px) (Sicht-Pruefung V4 r1 1.4, Boxen [(455, 228, 500, 262)]) | OK |
| FS Rumpf + Schultern | 17.3/4.8/1.9, Rotanteil 0.31; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 17.9/6.7/2.8 dE00 2.2 dL +0.6; Rotanteil 0.36 (13358 px) (Sicht-Pruefung V4 r1 1.4, Boxen [(352, 196, 448, 292), (318, 160, 360, 190), (425, 165, 470, 195)]) | OK |
| FS Unterarme | 29.3/0.7/-1.6, Rotanteil 0.00; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 28.5/1.7/-1.5 dE00 1.5 dL -0.8; Rotanteil 0.00 (3527 px) (Sicht-Pruefung V4 r1 1.4, Boxen [(468, 268, 500, 330)]) | OK |
| FS Unterschenkel | 15.1/1.7/-1.5, Rotanteil 0.03; dE00 <= 5, |dL| <= 4, Rotanteil +-0,15 | 17.9/4.5/0.7 dE00 4.4 dL +2.8; Rotanteil 0.17 (1901 px) (Sicht-Pruefung V4 r1 1.4, Boxen [(356, 352, 392, 430)]) | OK |
| F0 Gueltigkeit Farbvergleich | Wand links 64,7/0,3/-2,1 dE00 <= 2 | 65.2/0.2/-2.5 dE00 0.52 | OK |
