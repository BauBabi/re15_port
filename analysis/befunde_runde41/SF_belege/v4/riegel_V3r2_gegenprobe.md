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
| S1 Auge seitlich | 48,4/4,0/0,5 dE00 <= 6; 0,028-0,052 x 0,040-0,070 H | 50.3/5.7/-0.0 dE00 2.7; 0.061 x 0.057 H; 4 Komp. | ROT |
| S2 Augenlage seitlich | 0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle | 0.086 H; 0.906 H | ROT |
| S3 Haarkante seitlich | L* <= 22; 19,1/1,2/1,1 dE00 <= 6; Abstand zum Fleck <= 0,01 H | 48.8/3.3/5.6 dE00 24.5; Abstand 0.040 H | ROT |
| S4 Kopf-Oberseite rot seitlich | 27,6/15,3/3,3 dE00 <= 6; a* >= 11 | 45.4/5.4/-0.5 dE00 17.4 | ROT |
| T1 Schwanz Beginn | 0,15 H hinter Scheitel, 0,767 H ueber Sohle (+-0,03) | 0.233 H hinter, 0.750 H ueber (unterster Schwanz-Pixel an Id 7/8 (Id 16 versenkt)) | ROT |
| T2 Biegung | dx -0,245 dy +0,036 (+-0,03); Drehung 60-100 Grad auf 0,10 H | dx -0.180 dy 0.006; 34 Grad | ROT |
| T3 Knick (Taille) | dx -0,265 dy +0,257 (+-0,03) | dx -0.306 dy 0.392 | ROT |
| T4 Ende | dx -0,245 dy +0,397 (+-0,03); 0,37 H ueber Sohle | dx -0.309 dy 0.483; 0.267 H ueber Sohle | ROT |
| T5 Breite nach oben | f 0,1/0,3/0,5/0,7: 0,03/0,065/0,125/0,14 H; Abfall <= 0,01 | 0.040/0.064/0.108/0.150 H; Abfall 0.002 | OK |
| T6 Endstueck | B 0,025-0,05; L 0,12-0,17 H; 75-95 Grad; Taille <= 0,035 und schmalste | B 0.032; L 0.091 H; 88 Grad (hinten); Taille 0.032, schmalste davor 0.032 | ROT |
| T8 Schwanzfarbe | F15 9,3/11,4/1,5; F16 8,7/8,4/2,2; F17 26,8/-0,1/2,4; F18 15,2/3,2/1,6; dE00 <= 6; a* Wurzel >= 7, Ende <= 6 | F15 21.7/8.0/-0.6 dE00 8.9 (100 px); F16 22.1/8.7/-0.3 dE00 9.1 (150 px); F17 20.5/3.2/1.7 dE00 6.5 (632 px); F18 21.3/6.7/1.5 dE00 5.7 (149 px) | ROT |
| F F6 | 16.6/17.3/6.2 dE00 <= 5, |dL| <= 4 | 29.2/12.9/6.5 dE00 9.6 dL +12.6 (5937 px) | ROT |
| F F7 | 33.3/5.5/2.7 dE00 <= 5, |dL| <= 4 | 28.8/9.9/4.7 dE00 5.7 dL -4.5 (2112 px) | ROT |
| F F8 | 16.4/8.3/1.5 dE00 <= 5, |dL| <= 4 | 31.8/8.6/4.3 dE00 11.4 dL +15.4 (3293 px) | ROT |
| F F9 | 12.7/-0.9/-1.5 dE00 <= 5, |dL| <= 4 | 25.0/-2.2/-7.9 dE00 10.0 dL +12.3 (3629 px) | ROT |
| F F10 | 11.2/14.0/2.5 dE00 <= 5, |dL| <= 4 | 16.0/10.9/2.3 dE00 4.0 dL +4.8 (82 px) (Id 12 (Klaue (B)-rechts); Id 15: nan/nan/nan (0 px)) | ROT |
| F F14 | 60.0/11.0/1.2 dE00 <= 5, |dL| <= 4 | 41.7/9.6/5.4 dE00 18.7 dL -18.3 (1212 px) | ROT |
| F F19 | 18.4/0.5/-3.5 dE00 <= 5, |dL| <= 4 | 21.3/-2.9/-7.8 dE00 5.9 dL +2.9 (1142 px) | ROT |
| F Mittel | Mittel dE00 <= 4 | 9.30 | ROT |
| F0 Gueltigkeit Farbvergleich | Wand links 64,7/0,3/-2,1 dE00 <= 2 | 65.2/0.2/-2.5 dE00 0.52 | OK |
